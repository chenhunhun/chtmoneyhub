// Updater.cpp : main source file for Updater.exe
//

#include "stdafx.h"

#include "Skin/TuoImage.h"

#include "Skin/CoolMessageBox.h"
#include "Updater.h"
#include "resource.h"
#include "MainDlg.h"
#include "BankProcessMgr.h"
#include "config.h"
#include "DownloadOperation.h"
#include "..//Utils//UserBehavior/UserBehavior.h"



CUpdaterApp _Module;
VERSIONMAP g_oldVersion;
VERSIONMAP g_newVersion;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(_Module.m_bByUserClick ? nCmdShow : SW_HIDE);
	dlgMain.CenterWindow(_Module.m_hOuterParent);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}
__declspec(selectany) TCHAR szPath[MAX_PATH] = { 0 };
inline LPCTSTR GetModulePath()
{
	if (szPath[0] == 0)
	{
		::GetModuleFileName(NULL, szPath, _countof(szPath));
		TCHAR *p = _tcsrchr(szPath, '\\');
		if (p)
			*p = 0;
	}
	return szPath;
}
/**
*     bChoose = true    %temp%
*     bChoose = false   %appdata%
*/
tstring GetTempCachePath(HINSTANCE hInstance, bool bChoose = false)
{
	LPTSTR lpszTempPath = new TCHAR[MAX_PATH + 1];
	int nLength = 0;

	if( bChoose )
		nLength = GetTempPath(MAX_PATH, lpszTempPath);
	else
		nLength = ExpandEnvironmentStringsW(L"%appdata%", lpszTempPath, MAX_PATH);

	if (nLength > MAX_PATH)
	{
		delete[] lpszTempPath;
		lpszTempPath = new TCHAR[nLength + 1];

		if( bChoose )
			GetTempPath(nLength, lpszTempPath);
		else
			ExpandEnvironmentStringsW(L"%appdata%", lpszTempPath, nLength);
	}

	lpszTempPath[nLength] = '\0';
	tstring path = lpszTempPath;	
	delete[] lpszTempPath;

	if (path[path.size() - 1] != '\\')
		path += _T("\\");
	path += _T("BankUpdate");

	CreateDirectory(path.c_str(), NULL);

	return path;
}

static HANDLE _hUpdateMutex = NULL;

BOOL IsRunning()
{
	_hUpdateMutex = CreateMutex(NULL, TRUE, _T("__BANK_UPDATE_ONEPROCESS_6232"));

	if (GetLastError() == ERROR_ALREADY_EXISTS ||  ERROR_ACCESS_DENIED == GetLastError() )
	{
		CloseHandle(_hUpdateMutex);
		_hUpdateMutex = NULL;
		// Need Show Main Window;
		return TRUE;
	}

	return FALSE;
}  

BOOL IsDownload(LPTSTR szCmdLine, LPBOOL pbVisible, HWND* phParent)
{
	if (_tcsstr(szCmdLine, _T("/i")) != NULL)
		return FALSE;

	if (pbVisible)
		*pbVisible = (_tcsstr(szCmdLine, _T("/h")) == NULL);

	if (phParent)
	{
		TCHAR* cp = _tcsstr(szCmdLine, _T("/p"));
		if (cp != NULL)
			*phParent = (HWND)_ttol(cp + 3);
		else
			*phParent = NULL;
	}

	return TRUE;
}

void deleteAllReg()
{
	HKEY  hSubKey = NULL;

	if( ERROR_SUCCESS == RegOpenKeyExW(HKEY_CURRENT_USER,L"Software\\Bank\\Update",0,KEY_ALL_ACCESS,&hSubKey) )
	{
		if(ERROR_SUCCESS !=	RegDeleteValueW(hSubKey,L"InstallFlag") )
		{
			RegCloseKey(hSubKey);
			return ;
		}
		if(ERROR_SUCCESS !=	RegDeleteValueW(hSubKey,L"InstallPack") )
		{
			RegCloseKey(hSubKey);
			return ;
		}
	}

	if(hSubKey != NULL)    
		RegCloseKey(hSubKey);
}

std::string getModulePath()
{
	std::string str;
	char path[255];
	::GetModuleFileNameA(NULL,path,sizeof(path));
	

	*(strrchr(path,'\\') + 1) = 0 ;
	str = path;
	str += "MoneyHub.exe";
	return str;
}

void InstallPack(bool isvalid = true)
{
	DWORD dwType;
	TCHAR szCmdLine[1024];
	DWORD dwReturnBytes = sizeof(szCmdLine);

	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), 
		_T("InstallPack"), &dwType, szCmdLine, &dwReturnBytes)
		|| dwType != REG_SZ)
		return;

	CDownloadOperation don;
	
	don.ShowMessage();//显示升级信息

	don.uncompressFile();
	Sleep(1000);
	
	bool bIsFileSafe = _UpdateMgr.isSafeFile(szCmdLine);
	if(0 != _tcscmp(_T("modules"),szCmdLine) && bIsFileSafe )
	{
		ShellExecute(GetDesktopWindow(), _T("open"), szCmdLine, L"/upgrade", NULL, SW_SHOWNORMAL);
		//Sleep(3000);  
	}
	else//主程序升级包未通过完整性校验，启动moneyhub.exe
	{
		deleteAllReg();
		ShellExecuteA(GetDesktopWindow(),"open",getModulePath().c_str(),NULL,NULL,SW_SHOWNORMAL);
		don.deleteAllUpdataFile();
	}
}

bool  isDownLoadOK()
{
	DWORD dwType;
	DWORD dwReturnBytes = sizeof(DWORD);
	DWORD dwUpdate = 0;

	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("InstallFlag"), &dwType, &dwUpdate, &dwReturnBytes))
		return 0;

	if(!dwUpdate)
		return false;

	TCHAR szCmdline[1024];
    dwReturnBytes = sizeof(szCmdline);

	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), 
		_T("InstallPack"), &dwType, szCmdline, &dwReturnBytes)
		|| dwType != REG_SZ)
		return false;

	//非主程序升级
	if(0 == _tcscmp(szCmdline,_T("modules") ) )
		return true;

// 	wchar_t * sp = wcsrchr(szCmdline,'/');
// 	if(!sp)         
// 		return false;
// 	*(szCmdline + (sp - szCmdline) -1) = L'\0';
	CString strExe = szCmdline;
	strExe.Trim('"');

	if (_taccess_s(strExe, 0) != 0)
		return false;
	return true;
}



int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	g_strSkinDir = ::GetModulePath();
	g_strSkinDir += _T("\\Skin\\");
	ThreadCacheDC::InitializeThreadCacheDC();
	ThreadCacheDC::CreateThreadCacheDC();


	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// 临时目录
	_TempCachePath = GetTempCachePath(hInstance);

	if (IsRunning())
		return 0;

	if(!_tcsstr( lpstrCmdLine, _T("/h") ))
	{
		wchar_t wcsTmpPath[MAX_PATH] = {0};
		GetModuleFileNameW(NULL,wcsTmpPath,_countof(wcsTmpPath) );
		mhMessageBox(NULL,(wcscat_s(wcsTmpPath, _countof(wcsTmpPath), L" 不是有效的 win32 应用程序。"), wcsTmpPath),L"提示",MB_OK);
		exit(1);
	}	
	//////////////////////////////////////////////////////////////////////////
	// (1)是安装情况
	
	if ( !IsDownload(lpstrCmdLine, &_Module.m_bByUserClick, &_Module.m_hOuterParent)   )//moneyhub.exe execute
	{
		CDownloadOperation dop;
		int index = dop.isValid();

		if( index )
		{
			CBankProcessMgr BankMgr;
			std::vector<HANDLE> vecProcess;

			// 让所有bank退出
			::PostMessage(HWND_BROADCAST, WM_BROADCAST_QUIT, 0, 0);
			Sleep(1000);

			if (BankMgr.GetProcList(_T("MoneyHub.exe"), vecProcess) > 0)
				BankMgr.TermProcList(vecProcess);

			// 运行安装包
			InstallPack();
		}
		else
		{
			deleteAllReg();
			dop.deleteAllUpdataFile();
			ShellExecuteA(GetDesktopWindow(),"open",getModulePath().c_str(),NULL,NULL,SW_SHOWNORMAL);
		}

		return 0;
	}
	else if( isDownLoadOK()  )
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// (2)是下载情况

	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_WIN95_CLASSES | ICC_PROGRESS_CLASS | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	if (_hUpdateMutex)
		CloseHandle(_hUpdateMutex);

	return nRet;
}


// 创建一个快捷方式
BOOL CreateDesktopShotCut(tstring strName, tstring strSourcePath)
{
	if (FAILED(CoInitialize(NULL)))
		return FALSE;

	TCHAR szPath[MAX_PATH + 1];
	tstring strDestDir;

	LPITEMIDLIST pidl;
	LPMALLOC pShell;

	if (!SUCCEEDED(SHGetMalloc(&pShell)))
	{
		::CoUninitialize();
		return FALSE;
	}

	if (!SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &pidl)))
	{
		pShell->Release();
		::CoUninitialize();
		return FALSE;
	}


	if (!SHGetPathFromIDList(pidl, szPath))
	{
		pShell->Free(pidl);
		::CoUninitialize();
		return FALSE;
	}

	pShell->Free(pidl);
	pShell->Release();

	strDestDir = szPath;
	strDestDir += _T("\\") + strName + _T(".lnk");

	IShellLink* psl;
	if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl)))
	{
		psl->SetPath(strSourcePath.c_str());
		IPersistFile* ppf;
		if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf)))
		{
			USES_CONVERSION;
			if(SUCCEEDED(ppf->Save(CT2W(strDestDir.c_str()), TRUE)))
			{
				ppf->Release();
				psl->Release();
				::CoUninitialize();
				return TRUE;
			}
			else
			{
				ppf->Release();
				psl->Release();
				::CoUninitialize();
				return FALSE;
			}
		}
		else
		{
			ppf->Release();
			psl->Release();
			::CoUninitialize();
			return FALSE;
		}
	}
	else
	{
		::CoUninitialize();
		return FALSE;
	}
}

// 删除一个快捷方式
BOOL DeleteDesktopShotCut(tstring sShotCut) 
{
	TCHAR sPath[MAX_PATH + 1];
	tstring rPath;

	if (SUCCEEDED(SHGetSpecialFolderPath(0, sPath, CSIDL_DESKTOPDIRECTORY, 0)))
	{
		rPath = sPath;
		rPath += _T("\\") + sShotCut + _T(".lnk");
		return DeleteFile(rPath.c_str());
	}
	else
		return FALSE;
}

// 反馈安装信息到服务器
void  feedbackForUpdate()
{
	for(VERSIONMAP::iterator it= g_oldVersion.begin() ; it != g_oldVersion.end() ; it++)
	{
		for(VERSIONMAP::iterator itN= g_newVersion.begin() ; itN != g_newVersion.end() ; itN++)
		{
			if( strcmp(it->first.c_str() , itN->first.c_str() ) == 0)
			{
				CUserBehavior::GetInstance()->Action_Upgrade(it->second,itN->second);
			}
		}
	}
}