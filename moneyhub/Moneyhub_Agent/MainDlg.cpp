// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "Moneyhub_Agent.h"
#include "Shlwapi.h"
#include "atltime.h"
#include "../Utils/Config/HostConfig.h"
#include "../Utils/HardwareID/genhwid.h"
#include "../Utils/sn/SNManager.h"
#include "../Utils/PostData/UrlCrack.h"
//#define OWN_DEBUG 
#include <Wininet.h>
#include "Zip/zip.h"
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Shlwapi.lib")
#ifdef OWN_DEBUG 
#define DISPALY_DELAY_FULL_TIME	4
#define DISPALY_DELAY_TIME		10*1000// 应该是3小时
#else
#define DISPALY_DELAY_FULL_TIME	10800
#define DISPALY_DELAY_TIME		10*1000// 10s中查一次系统时间
#endif
#define CHECK_TIME 1000
#define MH_DELAYEVENT (0xff00)
#define MH_CHECKEVENT (0xff01)
#define MH_STARTDELAYEVENT (0xff02)

typedef BOOL (WINAPI * pChangeWindowMessageFilter)(UINT message, DWORD dwFlag);

CMainDlg::CMainDlg():
bTrackLeave(false),m_bShouldShow(false),m_bTodayShow(true),m_timer(0),m_bShowNow(false),m_delayflag(false),m_NotifyDevHandle(NULL)
{
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}
LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ShowWindow(SW_HIDE);
	return 0;
}

void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!bTrackLeave)
	{
		// 鼠标第一次移入窗口时， 请求一个WM_MOUSELEAVE 消息(在窗口内移动时，不请求)
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE;
        _TrackMouseEvent(&tme);
        bTrackLeave = true;
    }
	m_timer = 0;
}

__int64  CMainDlg::TimeDiff(SYSTEMTIME  left,SYSTEMTIME  right)  
{  
	CTime  tmLeft(left.wYear,left.wMonth,left.wDay,0,0,0);  
	CTime  tmRight(left.wYear,left.wMonth,left.wDay,0,0,0);  
	CTimeSpan  sp;  
	sp  =  tmLeft  -  tmRight;
	long  lLMinllis  =  (left.wHour*3600  +  left.wMinute*60  +  left.wSecond);  
	long  lRMinllis  =  (right.wHour*3600  +  right.wMinute*60  +  right.wSecond);  

	return  (__int64)sp.GetDays() * 86400  +  (lLMinllis  -  lRMinllis);  //此处返回秒，可用根据自己的格式需要进行转换，如时分秒
} 

int CMainDlg::PostData2Server(string hid, string sn, wstring file)
{
	wstring totalurl = CHostContainer::GetInstance()->GetHostName(kWeb) + L"get_file.php";
	CUrlCrack url;
	if (!url.Crack(totalurl.c_str()))
		return 1000;

	HINTERNET		m_hInetSession; // 会话句柄
	HINTERNET		m_hInetConnection; // 连接句柄
	HINTERNET		m_hInetFile; //
	HANDLE			m_hSendFile;

	m_hInetSession = ::InternetOpen(L"Moneyhub3.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
	{
		return 3000;
	}
	
	DWORD dwTimeOut = 60000;
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
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
	m_hInetFile = HttpOpenRequestW(m_hInetConnection, _T("POST"), url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
	if (m_hInetFile == NULL)
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetSession);
		return 3002;
	}	

	char *pBoundary = "---------------------170081691720202";//比实际的boundary多两个--！！！

	char cfileinfo[10240] = {0};//--\nformadata和实际数据之间要有2个\r\n，否则有问题
	sprintf_s(cfileinfo, 10240, "%s\r\nContent-Disposition: form-data; name=\"hid\"\r\n\r\n%s\r\n%s\r\nContent-Disposition: form-data; name=\"sn\"\r\n\r\n%s\r\n%s\r\n\
Content-Disposition: form-data; name=\"file\"; filename=\"Feedback.zip\"\r\n\r\n",pBoundary, hid.c_str(), pBoundary, sn.c_str(), pBoundary);

	WCHAR szAppDataFileName[MAX_PATH + 1];
	ExpandEnvironmentStringsW(L"%APPDATA%\\MoneyHub\\Feedback.zip", szAppDataFileName, MAX_PATH);

	m_hSendFile = CreateFile(szAppDataFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hSendFile == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		return err;
	}

	DWORD dwLength = GetFileSize(m_hSendFile, NULL);
	unsigned char* lpBuffer = new unsigned char[dwLength + 1];

	if (lpBuffer == NULL)
	{
		return false;
	}

	DWORD dwRead = 0;
	if (!ReadFile(m_hSendFile, lpBuffer, dwLength, &dwRead, NULL))
	{
		delete[] lpBuffer;
		return false;
	}
	CloseHandle(m_hSendFile);

	DWORD dDataSize = dwRead + strlen(cfileinfo) + strlen(pBoundary) + 6;
	unsigned char* lpData = new unsigned char[dDataSize + 1];
	if(lpData == NULL)
	{
		delete[] lpBuffer;
		return false;
	}
	memset(lpData, 0, dDataSize + 1);
	unsigned char* lpCur = lpData;

	memcpy(lpData, cfileinfo, strlen(cfileinfo));
	lpCur = lpData + strlen(cfileinfo);
	memcpy(lpCur, lpBuffer, dwRead);

	delete[] lpBuffer;
	lpCur += dwRead;
	memcpy(lpCur, "\r\n", 2);	
	lpCur += 2; 
	memcpy(lpCur, pBoundary, strlen(pBoundary));	
	lpCur += strlen(pBoundary);
	memcpy(lpCur, "--\r\n", 4);
	
	TCHAR szHeaders[1024];	
	_stprintf_s(szHeaders, _countof(szHeaders), _T("Content-Type: multipart/form-data;boundary=-------------------170081691720202\r\nContent-Length:%d"), dDataSize);
	BOOL ret = HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

	HttpAddRequestHeaders(m_hInetFile, _T("User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %s\r\n"), A2W(hid.c_str()));
	ret = HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	wstring heads = L"Connection:keep-alive\r\nUser-Agent:Moneyhub3.1\r\n";
	ret = HttpAddRequestHeadersW(m_hInetFile, heads.c_str(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	BOOL bSend = ::HttpSendRequestW(m_hInetFile, NULL, 0, (LPVOID)lpData, dDataSize);
	delete[] lpData;
	if (!bSend)
	{
		int err = GetLastError();
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);
		return err;
	}
	InternetCloseHandle(m_hInetConnection);
	InternetCloseHandle(m_hInetFile);
	InternetCloseHandle(m_hInetSession);

	return 0;

}
LRESULT CMainDlg::OnFeedBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	//获得HID和SN
	string sn = CSNManager::GetInstance()->GetSN();
	string hid = GenHWID2();
	//组装feedback的文件包
	WCHAR szAppDataPath[MAX_PATH + 1];
	ExpandEnvironmentStringsW(L"%APPDATA%\\MoneyHub\\", szAppDataPath, MAX_PATH);
	wstring dir = szAppDataPath;

	wstring tempzip = dir + L"Feedback.zip";

	HZIP hzip = CreateZip(tempzip.c_str(), NULL);

	std::wstring dirdata = dir;
	dirdata += L"Data\\*.dat";
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFileW(dirdata.c_str(), &FindFileData);
	if(INVALID_HANDLE_VALUE != hFind)
	{
		do{
			std::wstring filename = dir + L"Data\\" + FindFileData.cFileName;
			ZipAdd(hzip, FindFileData.cFileName, filename.c_str());
		}while (FindNextFileW(hFind, &FindFileData) != 0);	
		FindClose(hFind);
	}

	dirdata = dir + L"*.dmp";
	hFind = FindFirstFileW(dirdata.c_str(), &FindFileData);
	if(INVALID_HANDLE_VALUE != hFind)
	{
		do{
			std::wstring filename = dir + FindFileData.cFileName;
			ZipAdd(hzip, FindFileData.cFileName, filename.c_str());
		}while (FindNextFileW(hFind, &FindFileData) != 0);	
		FindClose(hFind);
	}

	TCHAR szPath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	wstring exepath = szPath;
	size_t lpos = exepath.find_last_of(L"\\");
	exepath = exepath.substr(0, lpos + 1);

	exepath += L"syslog.txt";
	if(PathFileExistsW(exepath.c_str()))
		ZipAdd(hzip, L"syslog.txt", exepath.c_str());

	wstring file1 = dir + L"Run.log";
	if(PathFileExistsW(file1.c_str()))
		ZipAdd(hzip, L"Run.log", file1.c_str());

	CloseZip(hzip);
	PostData2Server(hid, sn, L"");

	if(PathFileExistsW(tempzip.c_str()))//删除因反馈而生成的临时文件
		DeleteFile (tempzip.c_str());
	return 0;
}
LRESULT CMainDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if(wParam == MH_STARTDELAYEVENT)
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"推迟显示");

		GetLocalTime(&m_lastTime);
		//延迟2分钟显示
		KillTimer(MH_STARTDELAYEVENT);
		CheckShow();
		ShowPop();
#ifndef OWN_DEBUG 
		AutoUpdate();
#endif

	}
	if(wParam == MH_DELAYEVENT)
	{
		bool run = false;
		if(m_Netstat == false)
		{
			m_Netstat = CheckNetState();
			if(m_Netstat == true)
			{
				run = true;
				AutoUpdate();
			}
		}
		SYSTEMTIME tmCur;
		GetLocalTime(&tmCur);

		if(TimeDiff(tmCur, m_lastTime) > DISPALY_DELAY_FULL_TIME)
		{
			GetLocalTime(&m_lastTime);//记录当前时间
			//3小时一次弹出窗口
#ifndef OWN_DEBUG
			if(!run)
			{
				m_Netstat = CheckNetState();
				AutoUpdate();
			}
#endif
			CheckShow();
			ShowPop();
		}
	}
	else if(wParam == MH_CHECKEVENT)
	{
		if(!bTrackLeave)
			m_timer ++;
		if(m_timer >= 10)
		{
			KillTimer(MH_CHECKEVENT);
			m_timer = 0;
			AnimateWindow(m_hWnd, 1000, AW_BLEND | AW_HIDE); //淡出1秒
			ShowWindow(SW_HIDE);
		}
	}
	return 0;
}


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_Netstat = true;
#ifndef OWN_DEBUG 
	//AutoUpdate();// 开机后启动自动更新
#endif
	ModifyStyleEx(0, WS_EX_TOOLWINDOW);
	SetWindowText(L"MoneyHub_Svr_Mainframe");

	//设置所用到的字体
	LPCTSTR lpszFontName[] = { _T("Tahoma"), _T("微软雅黑") };

	int iIndex = IsVista() ? 1 : 0;
	int sSize = IsVista() ? 16 : 15;
	m_Font = CreateFont(sSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);


	m_imgBackGround.m_strFileName = L"Pop\\pop_bk";
	m_imgBackGround.LoadFromFile();
	//SetWindowPos(NULL,0,0,m_imgBackGround.GetWidth() + 1,m_imgBackGround.GetHeight() + 1,SWP_NOZORDER);
	SetWindowPos(NULL,0,0,m_imgBackGround.GetWidth(),m_imgBackGround.GetHeight(),SWP_NOZORDER);

	CRect rc;
	GetWindowRect(&rc);

	ScreenToClient(&rc);
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, rc.Width(), rc.Height(), 5, 5);
	SetWindowRgn(hRgn, TRUE);
	DeleteObject(hRgn);


	m_imgDlgButton.LoadFromFile(_T("Pop\\pop_button_back.png"));
	m_imgCloseButton.LoadFromFile(_T("Pop\\pop_close.png"));

	m_show = GetDlgItem(IDC_LOOKUP);
	m_notips = GetDlgItem(IDC_NOTIPS);
	m_close= GetDlgItem(ID_CLOSE);

	m_show.ApplySkin(&m_imgDlgButton, 4);
	m_notips.ApplySkin(&m_imgDlgButton, 4);
	m_close.ApplySkin(&m_imgCloseButton, 3);


	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	//设置图片位置
	GetClientRect(&rc); 
	RECT wkrc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &wkrc, 0);
	int x = (wkrc.right - wkrc.left) - rc.Width() - 20;
	int y = (wkrc.bottom - wkrc.top) - rc.Height();
	SetWindowPos(HWND_TOP, x, y, 0, 0 ,SWP_NOSIZE);

	CRect rcClose;
	m_close.GetWindowRect(&rcClose);
	rcClose.SetRect(rc.Width() - rcClose.Width() - 6, 6, rc.Width() - 6, rcClose.Height() + 6);
	m_close.MoveWindow(&rcClose);
	//设置时间5s
	SetTimer(MH_DELAYEVENT,DISPALY_DELAY_TIME,NULL);//3小时重新检查和弹出，10s查一次系统时间

	// 第一次隐藏
	SetTimer(MH_STARTDELAYEVENT, 5 ,NULL);// 修改为立即之后开始显示
#ifndef OWN_DEBUG 
	ShowWindow(SW_HIDE);
#else

	CheckShow();
	ShowPop();
#endif
	CGlobalData::GetInstance()->SetFramHwnd(m_hWnd);

	OSVERSIONINFOEX OSVerInfo; 
	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(!GetVersionEx((OSVERSIONINFO *)&OSVerInfo)) 
	{ 
		OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
		GetVersionEx((OSVERSIONINFO *)&OSVerInfo); 
	} 

	if(OSVerInfo.dwMajorVersion >= 6) // Vista 以上 
	{ 
		HMODULE hDLL = ::LoadLibraryW(L"user32.dll");	// 动态装载组件

		if(hDLL)
		{
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"ChangeWindowMessageFilter");
			pChangeWindowMessageFilter lpChangeWindowMessageFilter = (pChangeWindowMessageFilter)::GetProcAddress( hDLL, "ChangeWindowMessageFilter");	
			// "ChangeWindowMessageFilter"函数指针
			if(lpChangeWindowMessageFilter)	
			{
				lpChangeWindowMessageFilter(WM_MONEYHUB_FEEDBACK, MSGFLT_ADD);
			}
			FreeLibrary(hDLL);
		}
	}
	m_Netstat = CheckNetState();


	return TRUE;
}

bool CMainDlg::CheckNetState()
{
	if(!(InternetCheckConnection(L"http://www.sohu.com", FLAG_ICC_FORCE_CONNECTION, 0) ||
		InternetCheckConnection(L"http://www.baidu.com", FLAG_ICC_FORCE_CONNECTION, 0) ||
		InternetCheckConnection(L"http://www.sina.com", FLAG_ICC_FORCE_CONNECTION, 0)))
	{
		return false;
	}
	return true;
}
LRESULT CMainDlg::OnDeviceChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case DBT_DEVICEARRIVAL://插入U盘
		{
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, L"USB提示");
			HWND hUSBwnd = ::FindWindowW(L"MH_USBMoneyhubAxUI", NULL);

			if(hUSBwnd)
			{
				::PostMessageW(hUSBwnd, WM_SHNOTIFY , 0 , 0);
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

//处理第一次不能隐藏窗口的问题
LRESULT CMainDlg::OnNcPaint(WORD /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	static int time = 2;

	if(time > 1)
	{
		ShowWindow(SW_HIDE);
		time --;
	}
	return 0;
}
void CMainDlg::ShowPop()
{
	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0 ,SWP_NOMOVE | SWP_NOSIZE);
	if(m_bShouldShow)//需要再提醒
		if(m_bTodayShow && m_bShowNow)//今天需要提醒
		{
			CRect rc,rc1;
			GetClientRect(&rc); 
			HWND   hTop = ::FindWindowExW(::GetDesktopWindow(),NULL,L"Progman",NULL);//
			int x = 0, y = 0;
			if(hTop)
			{
				HWND hTopc = ::FindWindowEx(hTop, NULL, L"SHELLDLL_DefView",NULL);
				if(hTopc)
				{
					HWND hTopb = ::FindWindowEx(hTopc, NULL, L"SysListView32",NULL);
					if(hTopb)
					{
						::GetWindowRect(hTopb,&rc1);
						if(IsVista())//Vista、Win7的位置和XP下的不一样
						{
							x = rc1.right - rc.Width() - 20;
							y = rc1.bottom - rc.Height() - 20;
						}
						else
						{
							x = rc1.right - rc.Width() - 20;
							y = rc1.bottom - rc.Height();
						}
					}
				}				
			}
			if(x == 0 || y == 0)
			{
				RECT wkrc;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &wkrc, 0);
				x = (wkrc.right - wkrc.left) - rc.Width() - 20;
				y = (wkrc.bottom - wkrc.top) - rc.Height();
			}
			::SetWindowPos(m_hWnd,NULL, x, y, 0, 0 ,SWP_NOSIZE);

			AnimateWindow(m_hWnd, 2000, AW_BLEND);// 淡入

		//	SetTimer(MH_CHECKEVENT,CHECK_TIME,NULL);
			ShowWindow(SW_SHOW);
			return;
		}
	ShowWindow(SW_HIDE);
}
void CMainDlg::CheckShow()
{
	DWORD dwType;
	DWORD dwValue;
	DWORD dwReturnBytes = sizeof(DWORD);

	m_bShouldShow = true;
	m_bTodayShow = true;
	m_bShowNow = false;
	if (ERROR_SUCCESS == ::SHGetValueW(HKEY_CURRENT_USER, L"Software\\Bank\\Setting",L"ReminderEnabled", &dwType, &dwValue, &dwReturnBytes))
	{
		if(dwValue == 0)
			m_bShouldShow = false;//设置显示状态
	}
	if (ERROR_SUCCESS == ::SHGetValueW(HKEY_CURRENT_USER, L"Software\\Bank\\Setting",L"TodayReminderEnabled", &dwType, &dwValue, &dwReturnBytes))
	{
		SYSTEMTIME sys;
		GetLocalTime( &sys );
		if((sys.wMonth * 100 + sys.wDay) == dwValue)
			m_bTodayShow = false;
	}

	int tipnum = CBankData::GetInstance()->GetTodayAlarmsNumber();

	LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B7}");
	HANDLE _hProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

	DWORD err = GetLastError();
	if(_hProcessMutex != NULL)
		CloseHandle(_hProcessMutex);


#ifndef OWN_DEBUG
	if(tipnum > 0)
#endif
	  m_bShowNow = true;

	WCHAR str[10] = {0};
	wsprintfW(str, L"%d", tipnum);//

	m_tipnum = str;

}

LRESULT CMainDlg::OnMouseLeave(WORD /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	bTrackLeave = false;
	return 0 ;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DeleteObject(m_Font);
	DestroyWindow();
	::PostQuitMessage(nVal);
}


BOOL CMainDlg::OnEraseBkgnd(CDCHandle dc)
{
	CMemDC hdc(dc);
	hdc.SetBkMode(TRANSPARENT);
	CRect rect;


	GetClientRect(&rect);
	DrawBack(hdc, rect);

	int w = rect.Width();
	int h = rect.Height();
	hdc.SetPixel(1, 1, RGB(146, 146, 146));
	hdc.SetPixel(0, 3, RGB(146, 146, 146));
	hdc.SetPixel(0, 2, RGB(146, 146, 146));
	hdc.SetPixel(2, 0, RGB(146, 146, 146));
	hdc.SetPixel(3, 0, RGB(146, 146, 146));

	hdc.SetPixel(w - 3, 1, RGB(146, 146, 146));
	hdc.SetPixel(w - 4, 0, RGB(146, 146, 146));
	hdc.SetPixel(w - 5, 0, RGB(146, 146, 146));
	hdc.SetPixel(w - 2, 3, RGB(146, 146, 146));
	hdc.SetPixel(w - 2, 4, RGB(146, 146, 146));
	hdc.SetPixel(w - 2, 2, RGB(146, 146, 146));


	hdc.SetPixel(1, h - 2, RGB(146, 146, 146));
	hdc.SetPixel(1, h - 3, RGB(146, 146, 146));
	hdc.SetPixel(0, h - 4, RGB(146, 146, 146));
	hdc.SetPixel(0, h - 5, RGB(146, 146, 146));
	hdc.SetPixel(2, h - 2, RGB(146, 146, 146));
	hdc.SetPixel(3, h - 2, RGB(146, 146, 146));
	

	hdc.SetPixel(w - 3, h - 2, RGB(146, 146, 146));
	hdc.SetPixel(w - 2, h - 3, RGB(146, 146, 146));
	hdc.SetPixel(w - 2, h - 4, RGB(146, 146, 146));
	hdc.SetPixel(w - 2, h - 5, RGB(146, 146, 146));
	hdc.SetPixel(w - 4, h - 2, RGB(146, 146, 146));
	hdc.SetPixel(w - 5, h - 2, RGB(146, 146, 146));
	hdc.SetPixel(w - 3, h - 3, RGB(146, 146, 146));

	hdc.SetBkMode(TRANSPARENT);
	hdc.SetTextColor(RGB(16, 93, 145));

	hdc.SelectFont(m_Font);
	hdc.TextOut(35, 45, _T("您"));
	hdc.TextOut(47, 45, _T("有"));
	hdc.SetTextColor(RGB(200,0,0));		
	int len = m_tipnum.size();
	hdc.TextOut(62, 45, m_tipnum.c_str());
	hdc.SetTextColor(RGB(16, 93, 145));
	hdc.TextOut(62 + len*10, 45, _T("个提醒！"));

	return 0;
}

void CMainDlg::DrawBack(CDCHandle dc, const RECT &rect)
{
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc, rect.right - rect.left, rect.bottom - rect.top);
	memDC.SelectBitmap(bmp);

	m_imgBackGround.Draw(memDC, 0, 0, m_imgBackGround.GetWidth(), m_imgBackGround.GetHeight(), 0, 0, m_imgBackGround.GetWidth(), m_imgBackGround.GetHeight());

	dc.BitBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

	::DeleteObject(&bmp);
	::DeleteObject(&memDC);
}


void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));
}
LRESULT CMainDlg::OnLookUp(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OSVERSIONINFOEX OSVerInfo; 
	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(!GetVersionEx((OSVERSIONINFO *)&OSVerInfo)) 
	{ 
		OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
		GetVersionEx((OSVERSIONINFO *)&OSVerInfo); 
	} 
	//ShowWindow(false);
	WCHAR szPath[MAX_PATH] ={0};
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	::PathRemoveFileSpecW(szPath);

	wstring path(szPath);
	path += L"\\MoneyHub.exe";

	if(OSVerInfo.dwMajorVersion >= 6) // Vista 以上 
	{
		SHELLEXECUTEINFOW shExecInfo ;    
		memset(&shExecInfo,0,sizeof(SHELLEXECUTEINFOW));    
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);    
		shExecInfo.lpVerb = L"runas";    
		shExecInfo.lpFile = path.c_str();    
		shExecInfo.lpParameters = L"-agent";    
		shExecInfo.nShow = SW_SHOW ;    
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;   
		shExecInfo.hInstApp = NULL;
		//::MessageBoxW(NULL, L"ShellExecuteEx", L"财金汇UAC", MB_OK);
		if(ShellExecuteEx(&shExecInfo)) 
		{ 
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"Svr启动moneyhub成功");
			if(shExecInfo.hProcess != 0)
			{
				CloseHandle(shExecInfo.hProcess);
			}	 
		} 
		else 
		{ 
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"Svr启动moneyhub失败");
		}

	}
	else
	{
		path +=  L" -agent";
		STARTUPINFO si;	
		PROCESS_INFORMATION pi;	
		ZeroMemory( &pi, sizeof(pi) );	
		ZeroMemory( &si, sizeof(si) );	
		si.cb = sizeof(si);	
		//带参数打开
		if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{	
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"启动财金汇成功");
			CloseHandle( pi.hProcess );		
			CloseHandle( pi.hThread );		
		}
		else
		{
			int error = GetLastError();
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DLG_FUC, CRecordProgram::GetInstance()->GetRecordInfo(L"启动财金汇失败:%d", error));
		}
	}

	ShowWindow(SW_HIDE);

	return 0;
}

LRESULT CMainDlg::OnNoTips(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DWORD data; 
	SYSTEMTIME sys;
	GetLocalTime( &sys );
	data = sys.wMonth*100+sys.wDay;

	::SHSetValueW(HKEY_CURRENT_USER, L"Software\\Bank\\Setting",L"TodayReminderEnabled",REG_DWORD, &data, sizeof(DWORD));
	m_bTodayShow = false;
	ShowPop();

	return 0;
}

bool CMainDlg::IsVista()
{
	OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
	BOOL bRet = ::GetVersionEx(&ovi);
	return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6));
}

void CMainDlg::AutoUpdate()
{
	BOOL isAuto = IsAutoRunUpdate();

	if(isAuto == FALSE)
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"用户关闭了定时升级");
		return;
	}

	WCHAR szPath[MAX_PATH] ={0};
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	::PathRemoveFileSpecW(szPath);
	
	wstring path(szPath);
	path += L"\\Moneyhub_Updater.exe /h";

	STARTUPINFO si;	
	PROCESS_INFORMATION pi;	
	ZeroMemory( &pi, sizeof(pi) );	
	ZeroMemory( &si, sizeof(si) );	
	si.cb = sizeof(si);	
	//带参数打开	
	if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{	
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"定时升级");
		CloseHandle( pi.hProcess );		
		CloseHandle( pi.hThread );		
	}
	else
	{
		int error = GetLastError();
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DLG_FUC, CRecordProgram::GetInstance()->GetRecordInfo(L"定时升级失败:%d", error));
	}

}	

BOOL CMainDlg::IsAutoRunUpdate()
{
	BOOL bAutoRun = FALSE;

	DWORD dwType;
	DWORD dwValue;
	DWORD dwReturnBytes = sizeof(DWORD);

	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("AutoRun"), &dwType, &dwValue, &dwReturnBytes))
	{
		bAutoRun = TRUE;
	}
	else if (dwValue == 1)
	{
		bAutoRun = TRUE;
	}

	return bAutoRun;
}
