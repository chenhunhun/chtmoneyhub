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
#include "../Utils/UserBehavior/UserBehavior.h"
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

	CUserBehavior::GetInstance()->BeginFeedBack();

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

	CUserBehavior::GetInstance()->CloseFeedBack();

	return nRet;
}
