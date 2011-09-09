// Moneyhub_Agent.cpp : main source file for Moneyhub_Agent.exe
//

#include "stdafx.h"

#include "Moneyhub_Agent.h"
#include "MainDlg.h"
#include "Skin/TuoImage.h"
#include "skin/CoolMessageBox.h"
#include "Config.h"
#include "Security/SecurityCheck.h"
#include "Security/Security.h"
#include "Util.h"
//#include "../USBControl/USBControl.h"

CAppModule _Module;

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

	dlgMain.ShowWindow(nCmdShow);
	dlgMain.UpdateWindow();


	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"启动");

	DWORD pid = ::GetCurrentThreadId();
	HRESULT hRes = ::CoInitialize(NULL);
	g_strSkinDir = ::GetModulePath();
	g_strSkinDir += _T("\\Skin\\");
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (_tcsncmp(lpstrCmdLine, _T("-reblack"), 8) == 0)
	{
		HANDLE hEvent = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"MONEYHUBEVENT_BLACKUPDATE");
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"重新创建黑名单");
		CGlobalData::GetInstance()->Init();
		int iReturn = (int)_SecuCheckPop.CheckBlackListCache();
		if( hEvent )
		{
			SetEvent(hEvent);
			CloseHandle(hEvent);
		}
		return iReturn;
	}

	if (_tcsncmp(lpstrCmdLine, _T("-rebuild"), 8) == 0)
	{
		HANDLE hEvent = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"MONEYHUBEVENT_WHITEUPDATE");
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"重新创建白名单");
		CGlobalData::GetInstance()->Init();
		int iReturn = (int)_SecuCheckPop.ReBuildSercurityCache();
		if( hEvent )
		{
			SetEvent(hEvent);
			CloseHandle(hEvent);
		}
		return iReturn;
	}

	ThreadCacheDC::InitializeThreadCacheDC();
	ThreadCacheDC::CreateThreadCacheDC();


	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if(CGlobalData::GetInstance()->IsPopAlreadyRunning() == TRUE)
	{
		mhMessageBox(NULL, _T("泡泡程序已经在运行中..."), L"财金汇", MB_OK | MB_SETFOREGROUND);
		return 0;
	}

	ATLASSERT(SUCCEEDED(hRes));

	

	_SecuCheckPop.Start();// 开启安全检测及驱动进程
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"启动安全检测及驱动进程");

	//CUSBControl::GetInstance()->BeginUSBControl();
	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_WIN95_CLASSES | ICC_PROGRESS_CLASS | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));


	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"运行");

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();

	//::CoUninitialize();

	return nRet;
}




//DWORD WINAPI _threadWaitListCheck(LPVOID lp)
//{
//	//DebugBreak();
//	set<SecCachStruct*> waitlist;
//	CSecurityCache waitCache;
//	waitCache.Init();
//	waitCache.SetAllDataInvalid();
//	HWND frameWnd = (*(HWND*)lp);
//
//	HANDLE mphd=OpenFileMappingW(FILE_MAP_READ,true,L"MoneyHubWaitList");//创建内存映射文件
//	if(mphd)
//	{
//		LPVOID lpMapAddr = MapViewOfFile(mphd,FILE_MAP_READ,0,0,0);
//		if(lpMapAddr)
//		{
//			//将列表放入待查杀列表内存映射文件中
//			DWORD num,i;
//			memcpy(&num,lpMapAddr,sizeof(DWORD));
//			if(num > 100000 || num < 0)
//				return 0;
//
//			char * unPackBuf = (char*)lpMapAddr;
//			unPackBuf += sizeof(DWORD);
//			for( i = 0;i < num;i ++)
//			{
//				SecCachStruct *data = new SecCachStruct;
//				data->tag = 0;
//				memcpy(data->filename,unPackBuf,sizeof(data->filename));
//				unPackBuf += sizeof(data->filename);
//				waitlist.insert(data);
//			}
//			::UnmapViewOfFile(lpMapAddr);
//		}
//		::CloseHandle(mphd);
//	}
//	else 
//		return 0;
//
//	//已经获得待查杀列表中的所有文件
//	set<wstring> waitfile;
//	set<SecCachStruct*>::iterator site = waitlist.begin();
//	for(;site != waitlist.end();site ++)
//	{
//		wstring fname = (*site)->filename;
//		waitfile.insert(fname);
//	}
//
//	::PostMessageW(frameWnd,WM_CLOUDALARM,0,0);
//	while(1)
//	{
//		//Sleep(1000);
//		Sleep(10*60*1000);//十分钟
//		//界面显示有风险
//		::PostMessageW(frameWnd,WM_CLOUDALARM,0,0);
//
//		bool ret = CCloudCheckor::GetCloudCheckor()->Initialize();
//
//		if (ret)
//		{	
//			CCloudCheckor::GetCloudCheckor()->Clear();
//			//添加界面显示消息
//			::PostMessageW(frameWnd,WM_CLOUDCHECK,0,0);
//
//			CCloudCheckor::GetCloudCheckor()->SetFiles(&waitfile,frameWnd);
//			::PostMessageW(frameWnd,WM_CLOUDCHECK,0,0);
//			//界面显示：开始云查杀
//			ret = CCloudCheckor::GetCloudCheckor()->BeginScanFiles();
//			if(!ret)
//			{
//				CCloudCheckor::GetCloudCheckor()->Uninitialize();
//				//取消界面显示
//				::PostMessageW(frameWnd,WM_CLOUDNCHECK,0,0);
//				continue;
//			}
//
//			set<wstring>* passfiles = CCloudCheckor::GetCloudCheckor()->GetPassFiles();
//			if(passfiles->size() > 0 )
//			{
//				//将经过查杀的所有文件加入到安全缓存中
//				for(set<wstring>::iterator tite = passfiles->begin();tite != passfiles->end();tite ++)
//				{
//					SecCachStruct scc;
//
//					wcscpy_s(scc.filename,MAX_PATH,(*tite).c_str());
//					if(GenerateHash((*tite).c_str(),scc.chkdata))
//					{
//						waitCache.Add(scc);
//					}
//				}
//				waitCache.Flush();
//			}
//			//取消界面显示111
//			set<wstring>* nopass = CCloudCheckor::GetCloudCheckor()->GetUnPassFiles();
//			//nopass->insert(L"D:\\myHiew.dll");
//			if(nopass->size() <= 0)
//			{
//				::PostMessageW(frameWnd,WM_CLOUDNCHECK,0,0);
//				::PostMessageW(frameWnd,WM_CLOUDCLEAR,0,0);				
//				//说明没有非法文件
//				return 0;
//
//			}
//			else
//			{
//				LookUpHash luh;
//				//将没有经过查杀的所有文件加入hash列表中
//				for(set<wstring>::iterator tite = nopass->begin();tite != nopass->end();tite ++)
//				{
//					unsigned char chkdata[SECURE_SIZE] = {0};
//
//					if(GenerateHash((*tite).c_str(),chkdata))
//					{
//						luh.AddSecureHash(chkdata);
//					}
//				}
//
//				//发送给驱动清除要加载的hash文件
//
//				//将新的hash表传给驱动，用来更新过滤
//				g_moduleHashList.clear();
//				waitCache.GetEigenvalue();
//				CBankLoader::GetInstance()->setSecuModHashBR();
//
//
//				//成功后检测模块运行情况
//				if(CheckLoadModule(luh))
//				{
//					//还没有进行加载可疑文件
//					::MessageBox(NULL, _T("经过云查杀，系统内可能存在安全风险，财金汇已经对风险模块禁止加载，但可能导致财金汇运行中的异常，建议您退出财金汇进行全盘杀毒扫描！"), L"财金汇安全检测", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
//					::PostMessageW(frameWnd,WM_CLOUDNCHECK,0,0);
//					::PostMessageW(frameWnd,WM_CLOUDCLEAR,0,0);
//					return 0;
//
//				}
//				else
//				{
//					MessageBoxW(NULL,L"检测到有非法模块加载，为保证安全，财金汇将退出！",L"财金汇",MB_OK| MB_ICONERROR | MB_SETFOREGROUND);
//					SendMessageW(frameWnd,WM_CLOSE,NULL,NULL);
//					exit(-1);
//				}
//			}
//		}
//	}
//}