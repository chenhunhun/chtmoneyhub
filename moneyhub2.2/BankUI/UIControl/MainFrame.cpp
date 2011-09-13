#include "stdafx.h"
#include "../Util/ProcessManager.h"
#include "../Util/Util.h"
#include "../Util/SelfUpdate.h"
#include "../Include/Util.h"
#include "TabCtrl/TabItem.h"
#include "CategoryCtrl/CategoryItem.h"
#include "MainFrame.h"
#include "SettingDlg.h"
#include "AboutDlg.h"

#include "../../Utils/HardwareID/genhwid.h"
#include "../../Utils/sn/SNManager.h"
#include "Version.h"
#include "CoolMenuHook.h"
#include "../Tips/TipsDlg.h"
#include "../../Utils/Config/HostConfig.h"
#include "../../Include/ConvertBase.h"
#include "../../Utils/UserBehavior/UserBehavior.h"

#define TIMER_CHECKKERNEL (0xf2)

const UINT WM_BROADCAST_QUIT = RegisterWindowMessage(_T("BankQuitBroadcastMsg"));
const UINT WM_UPDATERETRY = RegisterWindowMessage(_T("BankUpdateRetryMsg"));
const UINT WM_SWITCHTOPPAGE = RegisterWindowMessage(_T("BankSwitchTopPageMsg"));

CMainFrame::CMainFrame() : CFSMUtil(&m_tsm), m_BigButton(&m_tsm), m_TabCtrl(&m_tsm), m_CatetoryCtrl(&m_tsm), m_StatusBar(&m_tsm),
	m_SysBtnBar(&m_tsm), m_TitleBar(&m_tsm), m_Toolbar(&m_tsm), m_wndMDIClient(m_TabCtrl, m_CatetoryCtrl, &m_tsm), m_hDWP(NULL), m_MenuButton(&m_tsm),
	m_LogoButton(&m_tsm), m_BackButton(&m_tsm), m_ForwardButton(&m_tsm), m_RefreshButton(&m_tsm), m_HelpButton(&m_tsm),m_IsShowCloudMessage(false),m_IsShowCloudStatus(false)//,m_SettingButton(&m_tsm)
{
	FS()->pMainFrame = this;
}


void CMainFrame::RecalcClientSize(int w, int h)
{
	int iTop = 0;
	if (w == -1)
	{
		RECT rcClient;
		GetClientRect(&rcClient);
		w = rcClient.right;
		h = rcClient.bottom;
	}

	POINT pt;
	SIZE sz;

	bool bIsWindowMax = (GetStyle() & WS_MAXIMIZE) != 0;

	RECT rcSysBtn;
	m_SysBtnBar.GetClientRect(&rcSysBtn);
	m_SysBtnBar.SetWindowPos(NULL, w - rcSysBtn.right - 2, 1, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_SysBtnBar.Invalidate();
	
	pt.x = w - m_SysBtnBar.GetFullWidth() - m_MenuButton.GetFullWidth() - 1;
	pt.y = 1;
	m_MenuButton.SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_MenuButton.Invalidate();

	m_TitleBar.SetWindowPos(NULL, 0, 0, pt.x, 30/*rcSysBtn.bottom*/, SWP_NOZORDER);

	pt.x = w - 105;
	pt.y = 34;// m_BigButton.GetHeight() + 6;
	sz.cx = m_LogoButton.GetFullWidth();
	sz.cy = m_LogoButton.GetHeight();
	m_LogoButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);

	m_Toolbar.SetWindowPos(NULL, 0, 30/*rcSysBtn.bottom*/, w - s()->LogoButton()->GetWidth(), m_Toolbar.GetHeight(), SWP_NOZORDER);

	// (B-1)
	// Back Button
	pt.x = 9; //s()->MainFrame()->GetBorderWidth() + 2;
	pt.y = 30 + 3;// m_BigButton.GetHeight() + 3;
	sz.cx = m_BackButton.GetFullWidth();
	sz.cy = m_BackButton.GetHeight();
	m_BackButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);

	// (B-2)
	// Forward Button
	pt.x = 40;
	pt.y = 30 + 8;//m_BigButton.GetHeight() + 8;
	sz.cx = m_ForwardButton.GetFullWidth();
	sz.cy = m_ForwardButton.GetHeight();
	m_ForwardButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);

	// (B-3)
	// Reload Button
	pt.x = 40 + 6 + m_ForwardButton.GetFullWidth();
	pt.y = 30 + 8;//m_BigButton.GetHeight() + 8;
	sz.cx = m_RefreshButton.GetFullWidth();
	sz.cy = m_RefreshButton.GetHeight();
	m_RefreshButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);

	// (B-4)
	// setting button
	/*pt.x = 40 + 12 + m_ForwardButton.GetFullWidth() + m_RefreshButton.GetFullWidth();
	pt.y = 30 + 8;
	sz.cx = m_SettingButton.GetFullWidth ();
	sz.cy = m_SettingButton.GetHeight ();
	m_SettingButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);*/

	// (B-5)
	// Help Button
	pt.x = 40 + 12 + m_ForwardButton.GetFullWidth() + m_RefreshButton.GetFullWidth();//40 + 18 + m_ForwardButton.GetFullWidth() + m_RefreshButton.GetFullWidth() + m_SettingButton.GetFullWidth ();
	pt.y = 30 + 8;//m_BigButton.GetHeight() + 8;
	sz.cx = m_HelpButton.GetFullWidth();
	sz.cy = m_HelpButton.GetHeight();
	m_HelpButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);

	

	// 剩余高度
	int iTopHeight = 79;	
	int iLessHeight = h - iTopHeight - s()->StatusBar()->GetStatusBarHeight(false);

	if (bIsWindowMax)
		m_CatetoryCtrl.SetWindowPos(NULL, 0, iTopHeight, w, s()->Category()->GetCategoryHeight(), SWP_NOZORDER);
	else
	{
		if (iLessHeight > s()->Category()->GetCategoryHeight())
			m_CatetoryCtrl.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), s()->Category()->GetCategoryHeight(), SWP_NOZORDER);
		else
			m_CatetoryCtrl.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), iLessHeight, SWP_NOZORDER);
	}

	iTopHeight += s()->Category()->GetCategoryHeight();
	iLessHeight = h - iTopHeight - s()->StatusBar()->GetStatusBarHeight(false);

	if (FS()->TabItem() == NULL || FS()->TabItem()->m_pCategory->GetWebData() == START_PAGE_WEBSITE_DATA)
		m_TabCtrl.ShowWindow(SW_HIDE);
	else
	{
		if (bIsWindowMax)
			m_TabCtrl.SetWindowPos(NULL, 0, iTopHeight, w, s()->Tab()->GetTabHeight(), SWP_NOZORDER | SWP_SHOWWINDOW);
		else
		{
			if (iLessHeight > s()->Tab()->GetTabHeight())
				m_TabCtrl.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), s()->Tab()->GetTabHeight(), SWP_NOZORDER | SWP_SHOWWINDOW);
			else
				m_TabCtrl.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), iLessHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
		}

		iTopHeight += s()->Tab()->GetTabHeight() + 1;
	}

	int xMdi = bIsWindowMax ? 0 : s()->MainFrame()->GetBorderWidth();
	int yMdi = iTopHeight;
	int cxMdi = w - (bIsWindowMax ? 0 : s()->MainFrame()->GetBorderWidth() * 2);
	int cyMdi = h - iTopHeight - s()->StatusBar()->GetStatusBarHeight(bIsWindowMax);
	m_wndMDIClient.SetWindowPos(NULL, xMdi, yMdi, cxMdi, cyMdi, SWP_NOZORDER | SWP_NOSENDCHANGING);
	m_wndMDIClient.PostMessage(WM_SIZE, 0, MAKELPARAM(cxMdi, cyMdi));

	m_StatusBar.SetWindowPos(NULL, 0, h - s()->StatusBar()->GetStatusBarHeight(bIsWindowMax), w, s()->StatusBar()->GetStatusBarHeight(false), SWP_NOZORDER);

	CClientDC dc(m_hWnd);
	OnEraseBkgnd(dc);
}

void CMainFrame::OnFinalMessage(HWND hWnd)
{
	delete this;
}

void CMainFrame::UpdateTitle()
{
	std::wstring str = _T("财金汇");
	if (FS()->TabItem())
	{
		str += _T(" - ");
		if (FS()->TabItem()->m_pCategory->GetWebData() == START_PAGE_WEBSITE_DATA)
			str += _T("我的首页");
		else if (FS()->TabItem()->m_pCategory->GetWebData())
			str += std::wstring(FS()->TabItem()->m_pCategory->GetWebData()->GetName()) + _T(" - ") + FS()->TabItem()->GetText();
	}

	SetWindowText(str.c_str());
	m_TitleBar.Invalidate();
}

void CMainFrame::UpdateSSLState()
{
	Invalidate();
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	::SetActiveWindow(m_hWnd);
	//SetWindowPos(HWND_TOP, 0, 0, 0, 0,SWP_NOSIZE);
	SetClassLongPtr(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE) | CS_DROPSHADOW);
	ModifyStyle(WS_CAPTION, 0);

	// icons
	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(NULL);
	HICON hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	//
	m_TextFont.CreateFont(13, 0, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, _T("Tahoma"));

	// use data
	SetWindowLongPtr(GWLP_USERDATA, (LONG_PTR)&m_tsm);
	FS()->hMainFrame = m_hWnd;
	s()->CreateTheme(m_hWnd, false);

	RECT rc = { lpCreateStruct->x, lpCreateStruct->y, lpCreateStruct->x + lpCreateStruct->cx, lpCreateStruct->y + lpCreateStruct->cy };
	SetRestoreRectAndMaxStatus(rc, (lpCreateStruct->style & WS_MAXIMIZE) != 0);

	m_MenuButton.CreateButton(m_hWnd);

	m_Toolbar.CreateBrowserToolbar(m_hWnd);
	m_SysBtnBar.CreateSystemButtonBar(m_hWnd);
	m_TitleBar.CreateTitleBar(m_hWnd);
	m_LogoButton.CreateLogoButton(m_hWnd);

	m_BackButton.CreateButton(m_hWnd);
	m_ForwardButton.CreateButton(m_hWnd);
	m_RefreshButton.CreateButton(m_hWnd);
	m_HelpButton.CreateButton(m_hWnd);
//	m_SettingButton.CreateButton(m_hWnd);


	m_TabCtrl.Create(m_hWnd, NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE);
	m_CatetoryCtrl.Create(m_hWnd, NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE);
	m_wndMDIClient.CreateTabMDIClient(m_hWnd);
	m_StatusBar.CreateBrowserStatusBar(m_hWnd);

	GetClientRect(&rc);
	SetRgnForWindow(lpCreateStruct->cx, lpCreateStruct->cy);
	//RecalcClientSize(rc.right, rc.bottom);

/*

	m_Rebar.CreateBrowserReBarCtrl(m_hWnd, lpCreateStruct->cx);
	m_CmdBar.CreateCommandBarCtrl(m_Rebar);
	m_AddressBar.CreateAddressBar(m_Rebar);
	m_SearchBar.CreateSearchBar(m_Rebar);
	m_FavorBar.CreateFavoriteBar(m_Rebar);
	m_RecentBar.CreateRecentBar(m_Rebar);

	m_Rebar.InitBand(&m_TitleBar, &m_SysBtnBar, &m_CmdBar, &m_Toolbar, &m_AddressBar, &m_SearchBar, &m_FavorBar, &m_RecentBar);

	m_FindBar.CreateFindTextBar(m_hWnd);

	// 如果是tab在顶上，则tab需要放在rebar里面（代替titlebar）
*/

	CCheckForUpdate::ClearInstallFlag();

	//InitCustomDrawMenu();
	CCoolMenuHook::InstallHook(m_hWnd);
	InitCoolMenuSettings();

	m_RefreshButton.EnableWindow(FALSE);
	m_ForwardButton.EnableWindow(FALSE);
	m_BackButton.EnableWindow(FALSE);
	return 0;
}


void CMainFrame::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
	lpMMI->ptMinTrackSize.x = 340;
	lpMMI->ptMinTrackSize.y = 110;
}

BOOL CMainFrame::OnEraseBkgnd(CDCHandle dc)
{
	bool bIsSecure = false;
	if (m_CatetoryCtrl.GetCurrentSelection() && m_CatetoryCtrl.GetCurrentSelection()->ShowSecureColor())
		bIsSecure = true;

	RECT rect;
	GetClientRect(&rect);
	rect.bottom = 139; //s()->Toolbar()->GetBigButtonHeight() + s()->Toolbar()->GetToolbarHeight() + s()->Tab()->GetTabHeight();
	s()->Toolbar()->DrawRebarBack(dc, rect, (GetStyle() & WS_MAXIMIZE) != 0, bIsSecure);
	FS()->pCate->Invalidate();


	// 画边框
	if ((GetStyle() & (WS_MAXIMIZE | WS_MINIMIZE)) == 0) 
	{
		GetClientRect(&rect);
		//rect.top += s()->Toolbar()->GetBigButtonHeight() + s()->Toolbar()->GetToolbarHeight() + s()->Tab()->GetTabHeight();
		rect.top += 108;
		s()->MainFrame()->DrawBorder(dc, rect, false, bIsSecure);
	}

	RECT rcSysBtn;
	m_SysBtnBar.GetClientRect(&rcSysBtn);
	GetClientRect(&rect);
	dc.SetPixel(rect.right - 2, rect.top + rcSysBtn.bottom - 1 , RGB(24, 82, 123));


	if(m_IsShowCloudMessage)
	{
		//这个位置要输出警告的字体
		HFONT oldFont = dc.SelectFont(m_TextFont);
		dc.SetTextColor(RGB(16, 93, 145));

		SetBkMode(dc,TRANSPARENT); 

		//获取显示文字的位置
		POINT pt;
		pt.x = 40 + 18 + m_ForwardButton.GetFullWidth() + m_RefreshButton.GetFullWidth() + m_HelpButton.GetFullWidth()+ 45;// + m_SettingButton.GetFullWidth ()
		pt.y = 30 + 8 + 8;
		LPTSTR mes = _T("无法连接云安全服务器，可能存在安全风险");
		dc.TextOut(pt.x,pt.y,mes,_tcslen(mes));	
		dc.SelectFont(oldFont);
	}
/*
	m_wndMDIClient.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.left -= s()->MainFrame()->GetBorderWidth();
	rect.right += s()->MainFrame()->GetBorderWidth();
	rect.top -= s()->MainFrame()->GetBorderHeight();
	rect.bottom += s()->MainFrame()->GetBorderHeight();
	dc.FillSolidRect(&rect, FS()->TabItem() == NULL || FS()->TabItem()->GetSecure() == false ? 0xcc0000 : 0x00cc00);

*/
	return TRUE;
}


void CMainFrame::OnSize(UINT nType, CSize size)
{
	RecalcClientSize(size.cx, size.cy);
}


LRESULT CMainFrame::OnClose(UINT /* uMsg */, WPARAM wParam, LPARAM lParam)
{

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"用户关闭");

	WINDOWPLACEMENT winp = { sizeof(WINDOWPLACEMENT) };
	::GetWindowPlacement(m_hWnd, &winp);

	DWORD dwPos = MAKELONG(winp.rcNormalPosition.left, winp.rcNormalPosition.top);
	DWORD dwSize = MAKELONG(winp.rcNormalPosition.right - winp.rcNormalPosition.left, winp.rcNormalPosition.bottom - winp.rcNormalPosition.top);
	DWORD dwMax = (GetStyle() & WS_MAXIMIZE) ? 1 : 0;
	::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndpos"), REG_DWORD, &dwPos, sizeof(DWORD));
	::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndsize"), REG_DWORD, &dwSize, sizeof(DWORD));
	::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndmax"), REG_DWORD, &dwMax, sizeof(DWORD));

	// 退出时删除Software\\Bank\\VirtualReg
	::SHDeleteKeyW(HKEY_CURRENT_USER, _T("Software\\Bank\\VirtualReg"));

	::SendMessageTimeoutW(m_IeWnd, WM_MAINFRAME_CLOSE,  0,  0, SMTO_NORMAL, 2000, 0);
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"通知内核退出");
	DestroyWindow();
	::PostQuitMessage(0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CMainFrame::OnNotifyAxUICreated(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	SwitchToThisWindow(m_hWnd, TRUE);
	m_IeWnd = (HWND)lParam;
	CProcessManager::_()->OnAxUICreated((HWND)lParam);
	AccessDefaultPage(m_hWnd); // 创建默认的页面并显示用户指定的标签页

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"内核创建了管理页");

	m_RefreshButton.EnableWindow(TRUE);
	m_ForwardButton.EnableWindow(TRUE);
	m_BackButton.EnableWindow(TRUE);

	SetTimer(TIMER_CHECKKERNEL, 5000, NULL);

	return 0;
}

LRESULT CMainFrame::OnShowHelpMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{	
	TPMPARAMS tps;
	tps.cbSize = sizeof(TPMPARAMS);
	m_MenuButton.GetWindowRect(&tps.rcExclude);

	CPoint pt;
	pt.x = tps.rcExclude.left;
	pt.y = tps.rcExclude.bottom;

	CMenu cHelpMenu;
	cHelpMenu.LoadMenu(IDR_MENU_HELP);
	//::TrackPopupMenuEx(cHelpMenu.GetSubMenu(0), TPM_VERTICAL, pt.x, pt.y, m_hWnd, &tps);
	CCoolMenuHook::TrackPopupMenuEx(cHelpMenu.GetSubMenu(0), TPM_VERTICAL, pt.x, pt.y, m_hWnd, &tps);

	return 0;
}

LRESULT CMainFrame::OnHelpSetting(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSettingDlg dlg;
	dlg.DoModal();

	return 0;
}

LRESULT CMainFrame::OnHelpTips(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTipsDlg tip(1);
	tip.DoModal();

	return 0;
}

LRESULT CMainFrame::OnHelpAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();

	return 0;
}

LRESULT CMainFrame::OnHelpUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	KillTimer(TIMER_AUTOUPDATE);

	CCheckForUpdate* pCheck = CCheckForUpdate::CreateInstance(m_hWnd);
	//pCheck->Check(false);
	pCheck->Release();

	return 0;
}

LRESULT CMainFrame::OnBroadcastQuit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	static bool bIsQuit = false;
	if (!bIsQuit)
	{
		bIsQuit = true;
		PostMessage(WM_CLOSE);
	}
	
	return 0;
}

LRESULT CMainFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
{
    if(TIMER_CHECKKERNEL == wParam)
	{
		LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B7}");
		HANDLE _hIEcoreProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

		DWORD err = GetLastError();
		if(_hIEcoreProcessMutex != NULL)
			::CloseHandle(_hIEcoreProcessMutex);

		// 当内核对象已经存在或者禁止进入时，说明已经开启该进程
		if (err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED)
		{
			return 0;
		}
		else
		{
			KillTimer(TIMER_CHECKKERNEL);//关闭定时器
			mhMessageBox(NULL, L"软件运行错误，为保护您的帐户安全，财金汇将重启", L"财金汇检测",MB_OK);

			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_COMMON_ERROR, L"检测到内核无响应");

				// 重新启动财金汇
			WCHAR szPath[MAX_PATH] ={0};
			::GetModuleFileName(NULL, szPath, _countof(szPath));
			::PathRemoveFileSpecW(szPath);

			std::wstring path(szPath);
			path += L"\\Moneyhub.exe -wait";

			STARTUPINFO si;	
			PROCESS_INFORMATION pi;	
			ZeroMemory( &pi, sizeof(pi) );	
			ZeroMemory( &si, sizeof(si) );	
			si.cb = sizeof(si);	
			//带参数打开	
			if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{		
				CloseHandle( pi.hProcess );		
				CloseHandle( pi.hThread );		
			}

			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"重启财金汇");

			exit(0);
		}

		return 0;
	}
	else
		bHandled = FALSE;

	return 0;
}

LRESULT CMainFrame::OnUpdateRetry(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	//SetTimer(TIMER_AUTOUPDATE, 10 * 60 * 1000);

	return 0;
}

LRESULT CMainFrame::OnSwitchTopPage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	enum TopPageStruct tps = (enum TopPageStruct)wParam;
	SwitchTopPage(m_hWnd, tps);

	return 0;
}

LRESULT CMainFrame::OnHelpFeedback(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	char szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));
	USES_CONVERSION;
	sprintf_s(szBuf, sizeof(szBuf), "%sfeedback.php?MoneyhubUID=%s&v=%s&SN=%s",W2CA(CHostContainer::GetInstance()->GetHostName(kWeb).c_str()), GenHWID2().c_str(), ProductVersion_All, CSNManager::GetInstance()->GetSN().c_str());


	::CreateNewPage_0(m_hWnd, A2CT(szBuf),TRUE);
	//CListManager::NavigateWebPage(A2CT(szBuf));

	return 0;
}

LRESULT CMainFrame::OnShowCloudMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	m_IsShowCloudMessage = true;
	Invalidate();
	return 0;
}
LRESULT CMainFrame::OnClearCloudMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	m_IsShowCloudMessage = false;
	Invalidate();
	return 0;
}
LRESULT CMainFrame::OnShowCloudStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	FS()->Status()->SetStatusText(L"正在运行云安全扫描……", true);
	Invalidate();
	return 0;
}

LRESULT CMainFrame::OnNoShowCloudStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	FS()->Status()->SetStatusText(L"", true);
	Invalidate();
	return 0;
}

LRESULT CMainFrame::OnCloudDestory(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	mhMessageBox(NULL,L"财金汇发现安全威胁并已阻断，为保证安全，请重新启动财金汇",L"财金汇",MB_OK);
	
	CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_COMMON_ERROR, L"检测到安全威胁");

	::SendMessageTimeoutW(m_IeWnd, WM_MAINFRAME_CLOSE,  0,  0, SMTO_NORMAL, 2000, 0);
	DestroyWindow();

	// 重新启动财金汇
	WCHAR szPath[MAX_PATH] ={0};
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	::PathRemoveFileSpecW(szPath);

	std::wstring path(szPath);
	path += L"\\Moneyhub.exe -wait";

	STARTUPINFO si;	
	PROCESS_INFORMATION pi;	
	ZeroMemory( &pi, sizeof(pi) );	
	ZeroMemory( &si, sizeof(si) );	
	si.cb = sizeof(si);	
	//带参数打开	
	if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{		
		CloseHandle( pi.hProcess );		
		CloseHandle( pi.hThread );		
	}
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"重启财金汇");

	::PostQuitMessage(0);
	return 0;
}


// gao
//LRESULT CMainFrame::OnFavBankChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam/**/, BOOL &/*bHandled*/)
//{
//	int nTp = (int)lParam;
//
//	CFavBankOperator* pOper = CListManager::_()->GetFavBankOper();
//	ATLASSERT (NULL != pOper);
//	if (NULL == pOper)
//		return S_FALSE;
//
//	std::string strBankID; 
////	CFavBankOperator::MyTranslateBetweenBankIDAndInt (strBankID, nTp, false);// false表示将int转换成bankID
//
//	//std::string strTemp;
//	//std::set<std::string> setStor;
//
//	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"用户收藏银行，更新数据");
//
//	bool bBack = CListManager::_()->AddANewFavBank ((LPWSTR)AToW(strBankID.c_str ()).c_str (), false);
//	
//	
//	CListManager::_()->UpdateHMac (); // 更新URL校验数据
//	pOper->UpDateFavBankID (strBankID); // 添加到用户银行收藏数据到变量中
//	
//	return 0;
//}

//LRESULT CMainFrame::OnUSBAddFavBank(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
//{
//	if(m_IeWnd)
//		::SendMessageTimeoutW(m_IeWnd, WM_USB_ADD_FAV,  wParam,  lParam, SMTO_NORMAL, 2000, 0);
//	return 0;
//}
//LRESULT CMainFrame::OnUSBChange(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
//{
//	if(m_IeWnd)
//		::SendMessageTimeoutW(m_IeWnd, WM_USB_CHANGE,  wParam,  lParam, SMTO_NORMAL, 2000, 0);
//	return 0;
//}

LRESULT CMainFrame::OnShowCheckMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	::MessageBoxW(NULL, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描", L"财金汇安全提示", MB_OK);
	return 0;
}
// 取消收藏
LRESULT CMainFrame::OnCancelAddFav(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	if(m_IeWnd)
		::PostMessageW(m_IeWnd, WM_CANCEL_ADDFAV,  wParam,  lParam);
	return 0;
}