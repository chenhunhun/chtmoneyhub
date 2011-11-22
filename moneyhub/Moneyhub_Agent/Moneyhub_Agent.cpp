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
#include "../Utils/PostData/BankDownloadMode.h"
#include "../Utils/PostData/UrlCrack.h"
#include "../Utils/UserBehavior/UserBehavior.h"
#include "../Utils/HardwareID/genhwid.h"
#include "../Utils/Config/HostConfig.h"
CAppModule _Module;
DWORD WINAPI _threadTestDownloadMode(LPVOID lp);

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

	DWORD dw;
	CloseHandle(CreateThread(NULL, 0, _threadTestDownloadMode, NULL, 0, &dw));

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
DWORD TestDwonloadMode(DWORD dwSendTimeOut, wstring& tUrl, bool bBreakPoint)
{
	CUrlCrack url;
	if (!url.Crack(tUrl.c_str()))
		return 1000;

	HINTERNET		m_hInetSession; // 会话句柄
	HINTERNET		m_hInetConnection; // 连接句柄
	HINTERNET		m_hInetFile; //
	
	m_hInetSession = ::InternetOpen(L"Moneyhub3.1", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
	{
		return 3000;
	}
	
	DWORD dwTimeOut = 60000;//初始化为5s
	//DWORD dwSendTimeOut = 5000;
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwSendTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwSendTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	

	m_hInetConnection = ::InternetConnect(m_hInetSession, url.GetHostName(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (m_hInetConnection == NULL)
	{
		InternetCloseHandle(m_hInetSession);

		return 3001;
	}

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;
	
	USES_CONVERSION;
	m_hInetFile = HttpOpenRequestW(m_hInetConnection, _T("GET"), url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
	if (m_hInetFile == NULL)
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetSession);
		return 3002;
	}	

	TCHAR szHeaders[1024];
	BOOL ret;
	if(bBreakPoint)
	{
		_stprintf_s(szHeaders, _countof(szHeaders), _T("Range: bytes=2-"));//这个做断点续传的测试
		ret = HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	}
	HttpAddRequestHeaders(m_hInetFile, _T("User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %s\r\n"), A2W(GenHWID2().c_str()));
	ret = HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

	BOOL bSend = ::HttpSendRequestW(m_hInetFile, NULL, 0, NULL, 0);
	if (!bSend)
	{
		int Error = GetLastError();
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);
		return Error;
	}
	TCHAR szStatusCode[32];
	DWORD dwInfoSize = sizeof(szStatusCode);
	if (!HttpQueryInfo(m_hInetFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);
		return 3004;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);
		if (nStatusCode != HTTP_STATUS_PARTIAL_CONTENT && nStatusCode != HTTP_STATUS_OK)
		{
			InternetCloseHandle(m_hInetConnection);
			InternetCloseHandle(m_hInetFile);
			InternetCloseHandle(m_hInetSession);
			return 3005;
		}
	}


	TCHAR szContentLength[32];
	dwInfoSize = sizeof(szContentLength);
	if (::HttpQueryInfo(m_hInetFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
	{
	}
	else 
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);
		return 3006;
	}

	DWORD revSize = 0;
	if(wcslen(szContentLength) != 0)
		revSize = _wtol(szContentLength);

	DWORD dwBytesRead = 0;
	char szReadBuf[1024];
	DWORD dwBytesToRead = sizeof(szReadBuf);

	if (!::InternetReadFile(m_hInetFile, szReadBuf, dwBytesToRead, &dwBytesRead))
	{	
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);
		return 3007;
	}
	InternetCloseHandle(m_hInetConnection);
	InternetCloseHandle(m_hInetFile);
	InternetCloseHandle(m_hInetSession);
	return 0;
}
// 
DWORD WINAPI _threadTestDownloadMode(LPVOID lp)//测试下载模式的函数
{
	wstring totalurl = CHostContainer::GetInstance()->GetHostName(kDownloadMode);//测试文件

	DWORD TestTime = 5000;//5s钟进行测试

	//如果没联网或者链接不上服务器，那么一直测试，每隔10s测试一次
	DWORD res = 0;
	while( 1 )
	{
		res = TestDwonloadMode(TestTime, totalurl, true);
		if(res == 3000 || res == 3001 || res == 3002)
			Sleep(10000);			
		else
			break;
	}

	if(res == 0)//说明可以进行断点续传
	{
		CDownloadMode::GetInstance()->SetMode(eHttpBreak);
		return 0;
	}
	int i = 0;
	while( 1 )
	{
		res = TestDwonloadMode(TestTime, totalurl, false);
		if(res == 3000 || res == 3001 || res == 3002)//说明时间太短		
			Sleep(10000);
		else if(res == 0)
		{
			CDownloadMode::GetInstance()->SetMode(eHttpNormal);
			return 0;
		}
		else
		{
			if( i = 0)
				TestTime = 30000;
			else if(i == 1)
				TestTime = 60000;
			else
				return 0;
			res = TestDwonloadMode(TestTime, totalurl, true);//再测试断点续传
			if(res == 0)
			{
				CDownloadMode::GetInstance()->SetMode(eHttpBreak);
				return 0;
			}
			else if(res == 3000 || res == 3001 || res == 3002)//
			{
				Sleep(10000);
			}
			else
				i ++;				
		}

	}

}