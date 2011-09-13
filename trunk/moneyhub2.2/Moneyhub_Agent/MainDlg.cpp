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
//#define OWN_DEBUG 
#include <Wininet.h>
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
		if(m_timer >= 6)
		{
			KillTimer(MH_CHECKEVENT);
			m_timer = 0;
			AnimateWindow(m_hWnd, 1000, AW_BLEND | AW_HIDE); //淡出1秒
			ShowWindow(SW_HIDE);
		}
	}
	return 0;
}
static /*const*/ GUID MYGUID_DEVINTERFACE_USB_DEVICE = 
{ 0xA5DCBF10L, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

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
				lpChangeWindowMessageFilter(WM_MONEYHUB_UAC, MSGFLT_ADD);
			}
			FreeLibrary(hDLL);
		}

		//DEV_BROADCAST_DEVICEINTERFACE   broadcastInterface; 
		//broadcastInterface.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		//broadcastInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

		//memcpy( &(broadcastInterface.dbcc_classguid),
		//	&(MYGUID_DEVINTERFACE_USB_DEVICE),
		//	sizeof(struct _GUID));

		//m_NotifyDevHandle = RegisterDeviceNotification(m_hWnd,
		//	&broadcastInterface,
		//	DEVICE_NOTIFY_WINDOW_HANDLE);
		//if(m_NotifyDevHandle)
		//	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, L"注册USB提示");
		//else
		//{
		//	int error = GetLastError();
		//	CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, CRecordProgram::GetInstance()->GetRecordInfo(L"注册USB提示失败:%d", error));
		//}


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

	if(time > 0)
	{
		ShowWindow(SW_HIDE);
		time --;
	}
	return 0;
}
void CMainDlg::ShowPop()
{
	SetWindowPos(HWND_TOP, 0, 0, 0, 0 ,SWP_NOMOVE | SWP_NOSIZE);
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

			SetTimer(MH_CHECKEVENT,CHECK_TIME,NULL);
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
	ShowWindow(false);
	WCHAR szPath[MAX_PATH] ={0};
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	::PathRemoveFileSpecW(szPath);
	
	wstring path(szPath);
	path += L"\\MoneyHub.exe -agent";

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

LRESULT CMainDlg::OnRunUAC(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam/**/, BOOL &/*bHandled*/)
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"RunUAC");
	char appid[16] = {0};
	memcpy(appid, (void *)&lParam, sizeof(LPARAM));
	string aid(appid);

	RunUAC(aid);
	return 0;
}

void CMainDlg::RunUAC(string aid)
{
	//OSVERSIONINFOEX OSVerInfo; 
	//OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	//if(!GetVersionEx((OSVERSIONINFO *)&OSVerInfo)) 
	//{ 
	//	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
	//	GetVersionEx((OSVERSIONINFO *)&OSVerInfo); 
	//} 

	//if(OSVerInfo.dwMajorVersion >= 6) // Vista 以上 
	//{ 
	//	::SwitchToThisWindow(m_hWnd, true);
	//	WCHAR szPath[MAX_PATH] ={0};
	//	::GetModuleFileName(NULL, szPath, _countof(szPath));
	//	::PathRemoveFileSpecW(szPath);

	//	wstring path(szPath);
	//	path += L"\\Moneyhub.exe";

	//	SHELLEXECUTEINFOW shExecInfo ;    
	//	memset(&shExecInfo,0,sizeof(SHELLEXECUTEINFOW));    
	//	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);    
	//	shExecInfo.lpVerb = L"runas";    
	//	shExecInfo.lpFile = path.c_str();    
	//	shExecInfo.lpParameters = L"-uac";    
	//	shExecInfo.nShow = SW_SHOW ;    
	//	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;   
	//	shExecInfo.hInstApp = NULL;
	//	//::MessageBoxW(NULL, L"ShellExecuteEx", L"财金汇UAC", MB_OK);
	//	if(ShellExecuteEx(&shExecInfo)) 
	//	{ 
	//		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"RunUAC 成功");
	//		if(shExecInfo.hProcess != 0)
	//		{
	//			::WaitForSingleObject(shExecInfo.hProcess, 5000);
	//		}			
	//		// 启动成功 
	//	} 
	//	else 
	//	{ 
	//		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"RunUAC失败");
	//		// 启动失败，可能UAC没有获得用户许可 
	//		//::MessageBoxW(NULL, L"取消收藏",L"财金汇", MB_OK);
	//		HWND hWnd = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	//		if (hWnd)
	//		{
	//			LPARAM lParam = 0;
	//			memcpy((void *)&lParam, aid.c_str(), sizeof(LPARAM));
	//			::PostMessageW(hWnd, WM_CANCEL_ADDFAV, 0, lParam);//
	//		}
	//	}

	//	HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	//	if(hMainFrame != NULL)
	//	{
	//		::SwitchToThisWindow(hMainFrame, true);
	//	}
	//} 
}
