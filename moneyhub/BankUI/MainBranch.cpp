/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  main.cpp
 *      说明：  主进程分类执行及功能类的实现文件
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */

#include "StdAfx.h"
#include "MainBranch.h"
#include "UIControl/SecuCheckDlg.h"
//#include "UIControl/InstallCheckDlg.h"
#include "UIControl/CoolMessageBox.h"
#include "Util/SecurityCheck.h"
#include "Util/DriverCommunicator.h"

#include "../Security/BankLoader/export.h"
#include "../Security/BankLoader/DriverLoader.h"
#include "../Utils/HardwareID/genhwid.h"
#include "resource/resource.h"
#include "Version.h"
#include "../Utils/sn/SNManager.h"
#include "Util/Util.h"
#include "Util/Config.h"
#include "UIControl/MainFrame.h"
#include "Tlhelp32.h"
#include "Util/SelfUpdate.h"
#include "../Utils/Config/HostConfig.h"
#include "../Utils/UserBehavior/UserBehavior.h"
using namespace std;
#pragma comment(lib,"Kernel32.lib")

//#include "../Utils/RunLog/RunLog.h"

CMainBranch g_AppBranch;
static HANDLE _hProcessMutex = NULL;	//唯一进程标示所用的内核对象
static HANDLE _hIEcoreProcessMutex = NULL;
#define TIMERELAPSE 5000
// IE内核进程函数，在BankCore工程内实现
void RunIECore(LPCTSTR lpstrCmdLine);

CMainBranch::CMainBranch(void)
{
	
}

CMainBranch::~CMainBranch(void)
{
}

bool CMainBranch::UpdateCheck()
{
#ifndef SINGLE_PROCESS
	// 检查是否需要安装升级包
	CInstallUpdatePack update;

	if (update.Check())
	{
		update.Setup();
		return false;
	}
	else
	{
		update.setupUpdateWithMH();
	}
#endif

	return true;
}
bool CMainBranch::CheckPop(bool bCheck, HANDLE *pId )
{
#ifndef SINGLE_PROCESS
	if(IsPopAlreadyRunning() == false)
	{
		int ret = IDOK;
		if( bCheck )
			ret = mhMessageBox(NULL,L"财金汇关键进程未开启，财金汇不能运行，请点击确定开启财金汇关键进程，取消后将退出财金汇",L"财金汇检测",MB_OKCANCEL);

		if(ret == IDOK)
		{
			WCHAR wsPath[MAX_PATH] = {0};
			::GetModuleFileNameW(NULL,wsPath,MAX_PATH);
			::PathRemoveFileSpecW(wsPath);

			wstring path(wsPath);
			path += L"\\Moneyhub_Svc.exe";

			STARTUPINFO si;	
			PROCESS_INFORMATION pi;	
			ZeroMemory( &pi, sizeof(pi) );
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);	
			//带参数打开
			if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{				
				if( pId )
					*pId = pi.hProcess;
				else
				{
					::CloseHandle( pi.hProcess );		
					::CloseHandle( pi.hThread );
				}
			}
			Sleep(2*1000);//等待Pop进程完全启动，最好已经将白名单发送给驱动
			return true;
		}
		else
			return false;
	}
#endif

	return true;
}


bool CMainBranch::IsPopAlreadyRunning()
{
	LPCTSTR lpszProcessMutex = _T("_MoneyhubPop_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B9}");
	HANDLE _hProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

	DWORD err = GetLastError();
	if(_hProcessMutex != NULL)
		::CloseHandle(_hProcessMutex);
	// 当内核对象已经存在或者禁止进入时，说明已经开启该进程
	if (err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED)
	{
		return true;
	}
	return false;
}


bool CMainBranch::SecurityCheck()
{
	//安全检测进入提示
	HANDLE hMutex = CreateMutexW(NULL, FALSE, L"UI_SECURITY_MUTEX");

	//安全检测
	CSecuCheckDlg dlg;
	
	if (IDCANCEL == dlg.DoModal(NULL))
	{
		return false;
	}

	if( hMutex )
		::CloseHandle(hMutex);
		

	return true;
}
int	CMainBranch::InstallCheck()
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"进行安装检测");

	CDriverCommunicator cd;
	CSecuCheckDlg dlg(true,true);

	// 安装的时候发送白名单和黑名单到驱动
	DWORD ret = dlg.DoModal();
	if(ret == IDOK)
	{
		cd.sendData();
		cd.SendBlackList();
		return 0;
	}
	return 1;

}
void CMainBranch::StartMonitor()
{
	//定时检测HOOK
	if(!(m_returnTimer = SetTimer(NULL,1,TIMERELAPSE,CheckHookProc)))
	{
		 //MessageBoxA(NULL,"set timer is error"," ",MB_OK);
	}
}
void CMainBranch::StopMonitor()
{
	KillTimer(NULL,m_returnTimer);
}	 
	 
	////////////////////////////////////////////////
// 控制显示向导
void CMainBranch::CheckGuide(HWND& hFrame)
{	
	DWORD dwValue;
	DWORD dwType, dwReturnBytes = sizeof(DWORD);

	if (ERROR_SUCCESS == ::SHGetValueW(HKEY_CURRENT_USER, L"Software\\Bank\\Setting",L"IsGuideShow", &dwType, &dwValue, &dwReturnBytes))
	{

		bool isGuideShow = (dwValue != 0);
		if(isGuideShow)
		{
			PostMessage(hFrame,WM_COMMAND,ID_HELP_TIPS,0);
		}
	}
}
bool CMainBranch::GetFramePos(int& nShowWindow,RECT& rcWnd,DWORD& dwMax)
{
	DWORD dwType, dwReturnBytes = sizeof(DWORD), dwPos = 0, dwSize = 0;
	::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndpos"), &dwType, &dwPos, &dwReturnBytes);
	::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndsize"), &dwType, &dwSize, &dwReturnBytes);
	::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndmax"), &dwType, &dwMax, &dwReturnBytes);

	if (dwSize != 0)
	{
		rcWnd.left = LOWORD(dwPos);
		rcWnd.top = HIWORD(dwPos);
		rcWnd.right = rcWnd.left + LOWORD(dwSize);
		rcWnd.bottom = rcWnd.top + HIWORD(dwSize);

		HMONITOR hMon = ::MonitorFromRect(&rcWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(MONITORINFO) };
		::GetMonitorInfo(hMon, &mi);
		RECT rcTest = { mi.rcMonitor.left + 10, mi.rcMonitor.top + 10, mi.rcMonitor.right - 10, mi.rcMonitor.bottom - 10 };
		RECT rcIntersect;
		if (!::IntersectRect(&rcIntersect, &rcTest, &rcWnd))
		{
			::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWnd, 0);
			rcWnd.left += 50; rcWnd.top += 50;
			rcWnd.right -= 50; rcWnd.bottom -= 50;
		}
	}
	else
	{
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWnd, 0);
		rcWnd.left += 50; rcWnd.top += 50;
		rcWnd.right -= 50; rcWnd.bottom -= 50;

		nShowWindow = SW_SHOWMAXIMIZED;
	}
	return true;
}
VOID CALLBACK CMainBranch::CheckHookProc(HWND hwnd , UINT uMsg , UINT_PTR idEvent , DWORD dwTime)
{
	CDriverLoader::CheckDriverImagePath(true);	
	BankLoader::checkHook();
}

bool CMainBranch::PopSetPage()
{
	HWND hWnd = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	if (hWnd)
	{
		::PostMessage(hWnd, WM_SWITCHTOPPAGE, (WPARAM)kToolsPage, 0);
		return true;
	}
	else
	{
		SetTopPage(kToolsPage);
		return false;
	}

}

bool CMainBranch::Shell(LPCTSTR lpstrCmdLine)
{
	wstring path(lpstrCmdLine + 14);

	::ShellExecute(NULL, _T("open"), path.c_str(), NULL, NULL, SW_SHOWNORMAL);
	return true;
}
bool CMainBranch::RunUAC(LPCTSTR lpstrCmdLine)
{
	OSVERSIONINFOEX OSVerInfo; 
	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(!GetVersionEx((OSVERSIONINFO *)&OSVerInfo)) 
	{ 
		OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
		GetVersionEx((OSVERSIONINFO *)&OSVerInfo); 
	} 

	if(OSVerInfo.dwMajorVersion >= 6) // Vista 以上 
	{ 
		HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
		if(hMainFrame != NULL)
		{
			::CloseWindow(hMainFrame);
		}

		USES_CONVERSION;

		string aid(W2A(lpstrCmdLine + 10));

		WCHAR szPath[MAX_PATH] ={ 0 };
		::GetModuleFileName(NULL, szPath, _countof(szPath));
		::PathRemoveFileSpecW(szPath);

		wstring path(szPath);
		path += L"\\Moneyhub.exe";

		SHELLEXECUTEINFOW shExecInfo ;    
		memset(&shExecInfo,0,sizeof(SHELLEXECUTEINFOW));    
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);    
		shExecInfo.lpVerb = L"runas";    
		shExecInfo.lpFile = path.c_str();    
		shExecInfo.lpParameters = L"-uac";    
		shExecInfo.nShow = SW_SHOW ;    
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;   
		shExecInfo.hInstApp = NULL;
		//::MessageBoxW(NULL, L"ShellExecuteEx", L"财金汇UAC", MB_OK);
		if(ShellExecuteEx(&shExecInfo)) 
		{ 
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"RunUAC 成功");
			if(shExecInfo.hProcess != 0)
			{
				::WaitForSingleObject(shExecInfo.hProcess, 5000);
			}			
			// 启动成功 
		} 
		else 
		{ 
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"RunUAC失败");
			// 启动失败，可能UAC没有获得用户许可 
			//::MessageBoxW(NULL, L"取消收藏",L"财金汇", MB_OK);
			HWND hWnd = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
			if (hWnd)
			{
				if(aid.size() > 0)
				{
					LPARAM lParam = 0;
					memcpy((void *)&lParam, aid.c_str(), sizeof(LPARAM));
					::PostMessageW(hWnd, WM_CANCEL_ADDFAV, 0, lParam);//
				}
			}
		}

		hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
		if(hMainFrame != NULL)
		{
			::SwitchToThisWindow(hMainFrame, true);
		}
	}
	return true;
}
void CMainBranch::RunIECoreProcess(LPCTSTR lpstrCmdLine)
{
	LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B7}");
	_hIEcoreProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

	// gao 2010-12-16 将listmanager读取内核和UI数据分开
	CListManager::Initialize(true);

	RunIECore(lpstrCmdLine + 3);
	::CloseHandle(_hIEcoreProcessMutex);
}

bool CMainBranch::CheckIECoreProcess()
{
	LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B7}");
	_hIEcoreProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

	DWORD err = GetLastError();
	if(_hIEcoreProcessMutex != NULL)
		::CloseHandle(_hIEcoreProcessMutex);

	// 当内核对象已经存在或者禁止进入时，说明已经开启该进程
	if (err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED)
	{
		return true;
	}

	return false;
}
bool CMainBranch::CheckToken()
{
	HANDLE hToken;
	OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
	BOOL bRestricted = IsTokenRestricted(hToken);
	::CloseHandle(hToken);
	return (bRestricted == TRUE) ? false : true;
}
bool CMainBranch::InitManagerList()
{
	//CListManager::Initialize(false);
	if(! CListManager::_()->GetResult())
	{
		mhMessageBox(NULL,L"财金汇读取关键文件失败，请尝试重新启动系统，或重新安装财金汇",L"财金汇检测",MB_OK);
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_COMMON_ERROR, L"ListManager读取失败");
		return false;
	}
	return true;
}

bool CMainBranch::TerminateIECore()
{
	HANDLE hProcessToken = NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hProcessToken)) 
	{ 
		return false; 
	}

	TOKEN_PRIVILEGES tp={0};
	LUID luid={0};  
	if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid))  
	{ 
		return false; 
	}  
	tp.PrivilegeCount = 1;  
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  

	// Enable the privilege
	AdjustTokenPrivileges(hProcessToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);  

	if(GetLastError() != ERROR_SUCCESS)  
	{
		return false;  
	}
	::CloseHandle(hProcessToken);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnap == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32W pew = {sizeof(PROCESSENTRY32W)};
	bool bHaveOther=false;
	Process32FirstW(hSnap,&pew);

	wstring moneyhub = L"moneyhub.exe";//找到moneyhub进程
	wchar_t buf[500] = {0},tmp[500]={0} ;
	HANDLE ownH = GetCurrentProcess();

	do{
		wstring wExe(pew.szExeFile);
		transform(wExe.begin(), wExe.end(), wExe.begin(), tolower);

		if(wExe == moneyhub)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,false,pew.th32ProcessID);
			if(hProcess)
			{
				if(ownH != hProcess)
				{
					::TerminateProcess(hProcess,0);
				}			
				::CloseHandle(hProcess);
			}
		}

	}while(Process32NextW(hSnap,&pew));

	::CloseHandle(ownH);
	::CloseHandle(hSnap);
	return bHaveOther;	
}
// 判断MoneyHub正在运行并切换到其界面的函数
BOOL CMainBranch::IsAlreadyRunning()
{
	LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B6}");
	_hProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

	DWORD err = GetLastError();
	// 当内核对象已经存在或者禁止进入时，说明已经开启该进程
	if (err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED)
	{
		//主界面
		HWND hWnd = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
		if (hWnd)
		{
			SwitchToThisWindow(hWnd, true);
			SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0 ,SWP_NOMOVE | SWP_NOSIZE);
			return TRUE;
		}
		// 安全检测界面
		HWND hSwnd = FindWindowW (NULL, SECU_DLG_TITLE);
		if(hSwnd&& IsWindowVisible(hSwnd))
		{
			// 将现有界面放到正中间进行显示
			SwitchToThisWindow(hSwnd, true);
			return TRUE;
		}
		// 输入财金汇密码框
		hWnd = FindWindow(NULL, _T("财金汇密码"));
		if (hWnd && IsWindowVisible(hWnd))
		{
			SwitchToThisWindow(hWnd, true);
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}
void CMainBranch::CloseHandle()
{
	::CloseHandle(_hProcessMutex);
}
int CMainBranch::Install()
{
	DWORD dwType;
	DWORD dwValue;
	DWORD dwReturnBytes = sizeof(DWORD);

	if (ERROR_SUCCESS != ::SHGetValueW(HKEY_CURRENT_USER, L"Software\\Bank\\Setting",L"IsGuideShow", &dwType, &dwValue, &dwReturnBytes))
	{
		DWORD i = 1;
		::SHSetValueW(HKEY_CURRENT_USER,L"Software\\Bank\\Setting",L"IsGuideShow",REG_DWORD,&i,4);//设置显示使用向导
	}

	DWORD i = 31;
	::SHSetValueW(HKEY_CURRENT_USER,L"Software\\Bank\\Setting",L"Version",REG_DWORD,&i,4);//设置moneyhub版本号


	// 设置ie8的标准模式运行财金汇，不用默认的ie8的IE7兼容模式
	int iever = GetIEVersion();
	if(iever == 8)
	{
		DWORD iemode = 0x1F40;
		SHSetValueW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION", L"Moneyhub.exe", REG_DWORD, &iemode, sizeof(DWORD));
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"财金汇设置IE8");
	}
	else if(iever == 9)
	{
		DWORD iemode = 0x2328;
		SHSetValueW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION", L"Moneyhub.exe", REG_DWORD, &iemode, sizeof(DWORD));
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"财金汇设置IE9");
	}

	// 安装并启动驱动
	if(BankLoader::InstallAndStartDriver())
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"安装驱动成功");
		return 0;
	}

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"安装驱动失败");

	return 1;
}
DWORD CMainBranch::GetIEVersion()
{
	const TCHAR szFilename[] = _T("mshtml.dll");   
	DWORD dwMajorVersion =0;   
 
	DWORD dwHandle = 0;   
	DWORD dwVerInfoSize = GetFileVersionInfoSize(szFilename, &dwHandle);   
	if (dwVerInfoSize)   
	{   
		LPVOID lpBuffer = LocalAlloc(LPTR, dwVerInfoSize);   
		if (lpBuffer)   
		{   
			if (GetFileVersionInfo(szFilename, dwHandle, dwVerInfoSize, lpBuffer))   
			{   
				VS_FIXEDFILEINFO * lpFixedFileInfo = NULL;   
		        UINT nFixedFileInfoSize = 0;   
				if (VerQueryValue(lpBuffer, TEXT("\\"), (LPVOID*)&lpFixedFileInfo, &nFixedFileInfoSize) &&(nFixedFileInfoSize))   
				{   
					dwMajorVersion = HIWORD(lpFixedFileInfo->dwFileVersionMS);     
  
				}   
			}   
			LocalFree(lpBuffer);  
		}
	}   
	
	return dwMajorVersion;
}

int CMainBranch::UnInstall()
{
	// HardwareID & Version;
	TCHAR szTempFile[MAX_PATH + 1];
	GetTempPath(MAX_PATH, szTempFile);
	_tcscat_s(szTempFile, _T("A9BD62759DBE4df1B7F7F619F99F17FB"));
	HANDLE hFile = ::CreateFile(szTempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		char szBuf[1024];
		memset(szBuf, 0, sizeof(szBuf));
		USES_CONVERSION;
		sprintf_s(szBuf, sizeof(szBuf), "%suninstall.php?MoneyhubUID=%s&v=%s&SN=%s",W2CA(CHostContainer::GetInstance()->GetHostName(kWeb).c_str()), GenHWID2().c_str(), ProductVersion_All,CSNManager::GetInstance()->GetSN().c_str());

		DWORD dw;
		WriteFile(hFile, szBuf, strlen(szBuf), &dw, NULL);
		::CloseHandle(hFile);
	}



	if(BankLoader::UnInstallDriver())
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"卸载驱动成功");

		return 0;
	}
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"卸载驱动失败");

	return 1;
}


void CMainBranch::GenerationBlackCache()
{
	WCHAR wcsBuf[MAX_PATH] = {0};
	GetModuleFileNameW(NULL, wcsBuf, MAX_PATH);
	PathRemoveFileSpecW(wcsBuf);
	std::wstring wstrBuf = wcsBuf;
	wstrBuf += L"\\Moneyhub_Svc.exe";  
	

	ShellExecuteW(GetDesktopWindow(), L"open", wstrBuf.c_str(), L"-reblack", NULL, SW_HIDE);
}

void CMainBranch::CheckHID()
{
	if(g_strHardwareId.size() < 32)
	{
		char gid[ 33 ] = {0};
		int gleft = (int)32 - g_strHardwareId.size();
		for(int i = 0; i < gleft; i ++)
			gid[i] =  'F';//
		g_strHardwareId += gid;
	}
}
void CMainBranch::RenameChk()
{
	char wsPath[MAX_PATH] = {0};
	::GetModuleFileNameA(NULL,wsPath,MAX_PATH);
	::PathRemoveFileSpecA(wsPath);

	string wsDirectory = wsPath;

	wsDirectory += "\\BankInfo\\banks\\";

	string  dir = wsDirectory + "*.*";

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFileA(dir.c_str(), &FindFileData);

	if(INVALID_HANDLE_VALUE == hFind)
		return;
	do{
		std::string fn = FindFileData.cFileName;

		if ((fn !=  ".") && (fn != "..") && (fn != ".svn"))
		{
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				string indir = wsDirectory + fn + "\\*.chk";
				WIN32_FIND_DATAA FindFileData2;
				HANDLE hFind2;

				hFind2 = FindFirstFileA(indir.c_str(), &FindFileData2);

				if(INVALID_HANDLE_VALUE != hFind2)
				{
					do{
						std::string fn2 = FindFileData2.cFileName;
						string oldname = wsDirectory + fn + "\\" + fn2;
						DeleteFileA(oldname.c_str());

					}while (FindNextFileA(hFind2, &FindFileData2) != 0);

					FindClose(hFind2);
				}
			}
		}
	}while (FindNextFileA(hFind, &FindFileData) != 0);

	FindClose(hFind);

}