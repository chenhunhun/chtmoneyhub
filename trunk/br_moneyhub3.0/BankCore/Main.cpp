/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  Main.cpp
*      说明：  ie内核进程中的核心线程函数文件
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.11.17	融信恒通
*-----------------------------------------------------------*
*/
#include "stdafx.h"
#include "../Security/BankProtector/export.h"
#include "ProcessMonitor/RegMonitor.h"
#include "AxUI.h"
#include "AxHookManager.h"
#include "WebBrowserCore.h"
#include "AxControl.h"
#include "LookUpHash.h"
#include "InternetSecurityManagerImpl.h"
#include "DownloadManagerImpl.h"
#include "../BankUI/Util/CleanHistory.h"
#include "../Utils/ListManager/ListManager.h"
#include "..\BankUI\Util\ProcessManager.h"
#include "..\Utils\ExceptionHandle\ExceptionHandle.h"
#include "GetBill\BillUrlManager.h"
DWORD WINAPI _threadCheckState(LPVOID lpParameter);
HWND g_hMainFrame = NULL;
/**
*  ie内核线程函数
* @param lpszCmdline ie内核进程所用的父窗口句柄
*/
void RunIECore(LPCTSTR lpszCmdline)
{
	
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"进入RunIECore线程");


	CExceptionHandle::MapSEtoCE();
	CBillUrlManager::GetInstance()->Init();
		// 对历史信息的读写文件接口进行挂钩
		//CListManager::Initialize(false);
	if(! CListManager::_()->GetResult())
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_INIT, L"内核校验ListManager失败");
		return;
	}

	BankProtector::Init();
	// 设置进程内弹出窗口管理属性
	CoInternetSetFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT,SET_FEATURE_ON_PROCESS,TRUE);

	CoInitialize(NULL);

	// 初始化GDI对象
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Register Common Controls Class
	::AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

	// Initialize
	//::DebugBreak();
	CRegKeyManager::Initialize();

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"RegKeyManager初始化");


	// 对该ie页面的消息的处理进行挂钩
	CAxHookManager::Initialize();
	CInternetSecurityManagerImpl::Initialize();

	HWND hMainFrame = (HWND)_ttoi(lpszCmdline);	
	g_hMainFrame = hMainFrame; // gao

	DWORD dwThreadID;
	// 开启管理线程，界面线程
#ifndef SINGLE_PROCESS
	::CloseHandle(::CreateThread(NULL, 0, _threadCheckState, (LPVOID)&hMainFrame, NULL, &dwThreadID));
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"开启监控线程");
#endif

	CAxUI axui;

	::SendMessage(hMainFrame, WM_MULTI_PROCESS_NOTIFY_AXUI_CREATED, 0, (LPARAM)axui.m_hWnd);
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"WM_MULTI_PROCESS_NOTIFY_AXUI_CREATED完成");
	// 将axui句柄保存到CprocessManager中
	CProcessManager::_()->OnAxUICreated (axui.m_hWnd);
	

	// 消息循环，等待新窗口创建的消息
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	
	try
	{
		CoUninitialize();
	}

	catch(CExceptionHandle eH)
	{
		eH.SetThreadName ("RunIECore Thread Error");
		eH.RecordException ();
	}
}
DWORD WINAPI _threadCheckState(LPVOID lpParameter)
{

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"进入监控线程");

	HWND hMainFrame = (*(HWND*)(lpParameter));

	DWORD dwMainProcessId = 0;
	::GetWindowThreadProcessId(hMainFrame, &dwMainProcessId);

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwMainProcessId);
	if (hProcess == NULL)
	{
		CleanHistory();
		::TerminateProcess(::GetCurrentProcess(), 1);
	}

	HANDLE _hMainProcessMutex = NULL;
	DWORD dwExitCode = 0;
	while(1)
	{
		LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B6}");
		_hMainProcessMutex  = OpenMutexW(NULL, TRUE, lpszProcessMutex);

		DWORD err = GetLastError();
		if(_hMainProcessMutex != NULL)
			::CloseHandle(_hMainProcessMutex);

		// 当内核对象已经存在或者禁止进入时，说明主框架进程还在
		if (err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED)
		{
			Sleep(1300);
		}
		else // 如果主框架进程被杀了
		{
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"监控线程关闭内核进程");
			CleanHistory();
			::TerminateProcess(::GetCurrentProcess(), 1);
		}
	}


	return 0;
}


//////////////////////////////////////////////////////////////////////////
/**
*  每一个ie页面处理线程函数
* @param lpParameter 父窗口句柄
*/
DWORD WINAPI ThreadProcCreateAxControl(LPVOID lpParameter)
{
	CExceptionHandle::MapSEtoCE();

	HWND hChildFrame = (HWND) lpParameter;
	CAxHookManager hook;
	hook.Hook();

	CAxControl *pAxControl = new CAxControl(hChildFrame);
	RECT rcClient;
	::GetClientRect(hChildFrame, &rcClient);
	// 创建iwebBrowser对象进行浏览
	pAxControl->Create(hChildFrame, rcClient, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);

	pAxControl->CreateIEServer();
	// 通知父窗口创建完毕
	::PostMessage(hChildFrame, WM_ITEM_NOTIFY_CREATED, 0, (LPARAM)pAxControl->m_hWnd);

	MSG msg;
	// 消息循环，在其中屏蔽了鼠标右键，并处理关闭
	while (::GetMessage(&msg, NULL, 0, 0))
	{	
		if (msg.message == WM_RBUTTONDOWN || msg.message == WM_RBUTTONUP || msg.message == WM_RBUTTONDBLCLK)
			continue;
		else if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
		{
			BOOL bCtrlKey = GetKeyState(VK_CONTROL) & 0x8000;
			if (msg.message == WM_KEYDOWN && bCtrlKey && msg.wParam == VK_TAB)
			{
				::PostMessage(hChildFrame, WM_ITEM_TOGGLE_CATECTRL, 0, 0);
				continue;
			}
			if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)/*&& ((!bCtrlKey && msg.wParam == VK_TAB) || 
				(msg.wParam == VK_F5) || 
				(bCtrlKey && msg.wParam == 'C') || 
				(bCtrlKey && msg.wParam == 'V') || 
				(bCtrlKey && msg.wParam == 'A') || 
				(bCtrlKey && msg.wParam == 'X') ))*/
			{
				CComQIPtr<IOleInPlaceActiveObject, &__uuidof(IOleInPlaceActiveObject)> spInPlaceActiveObject(pAxControl->m_pCore->m_pWebBrowser2);
				if (spInPlaceActiveObject && spInPlaceActiveObject->TranslateAccelerator(&msg) == S_OK)
					continue;
			}

		}

		else if (WM_CLOSE == msg.message)
		{
			TCHAR szMsgHwndClassName[300];
			::GetClassName(msg.hwnd, szMsgHwndClassName, _countof(szMsgHwndClassName));
			if (_tcscmp(szMsgHwndClassName, _T("Shell Embedding")) == 0)
			{
				::PostMessage(hChildFrame, WM_CLOSE, 0, 0);
				continue;
			}
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	try
	{
		::OleUninitialize();
		hook.Unhook();
	}

	catch(CExceptionHandle eH)
	{
		eH.SetThreadName ("ThreadProcCreateAxControl Thread Error");
		eH.RecordException ();
	}
	return 0;
}


