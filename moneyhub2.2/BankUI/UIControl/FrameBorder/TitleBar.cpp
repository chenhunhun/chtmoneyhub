#include "stdafx.h"
#include "../../Skin/SkinManager.h"
#include "../TabCtrl/TabCtrl.h"
#include "../BrowserToolbar.h"
#include "TitleBar.h"


CTitleBarCtrl* CTitleBarCtrl::sm_pCurrentHookingTitlebar = NULL;


CTitleBarCtrl::CTitleBarCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_bDoubleClkDouble(false), m_hSysMenuHook(NULL), m_hSysMenuHook2(NULL)
{
	FS()->pTitleBar = this;
}
CTitleBarCtrl::~CTitleBarCtrl()
{
	if (m_hSysMenuHook)
		::UnhookWindowsHookEx(m_hSysMenuHook);
	if (m_hSysMenuHook2)
		::UnhookWindowsHookEx(m_hSysMenuHook2);
}


void CTitleBarCtrl::CreateTitleBar(HWND hParent)
{
	HWND hWnd  = Create(hParent, CRect(0, 0, 10, FS()->ToolBar()->GetHeight()), NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd && "Create main titlebar failed.");
}

//////////////////////////////////////////////////////////////////////////
// message handler

LRESULT CTitleBarCtrl::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return 0;
}



LRESULT CALLBACK HookCheckDblClickProc(int iCode, WPARAM wParam, LPARAM lParam)
{
	if (iCode == HC_ACTION && CTitleBarCtrl::sm_pCurrentHookingTitlebar)
	{
		MSG *pRet = (MSG*)lParam;
		if (pRet->message == WM_LBUTTONDBLCLK)
			CTitleBarCtrl::sm_pCurrentHookingTitlebar->PostMessage(WM_LBUTTONDBLCLK, -1, 0);
		return 0;
	}
	return ::CallNextHookEx(NULL, iCode, wParam, lParam);
}

LRESULT CALLBACK HookCheckMouseUpProc(int iCode, WPARAM wParam, LPARAM lParam)
{
	if (iCode == HC_ACTION && CTitleBarCtrl::sm_pCurrentHookingTitlebar)
	{
		MSG *pRet = (MSG*)lParam;
		if (pRet->message == WM_LBUTTONUP)
			CTitleBarCtrl::sm_pCurrentHookingTitlebar->PostMessage(WM_NCLBUTTONUP, 0, pRet->lParam);
		return 0;
	}
	return ::CallNextHookEx(NULL, iCode, wParam, lParam);
}

void CTitleBarCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.x > 20)
	{
		ClientToScreen(&point);
		CWindow wndFrame = ::GetRootWindow(m_hWnd);
		sm_pCurrentHookingTitlebar = this;
		m_hSysMenuHook2 = ::SetWindowsHookEx(WH_GETMESSAGE, HookCheckMouseUpProc, NULL, ::GetCurrentThreadId());
		::SendMessage(wndFrame, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	}
	else
	{
		CWindow wndFrame = ::GetRootWindow(m_hWnd);
		CMenuHandle menu = ::GetSystemMenu(wndFrame, FALSE);
		menu.EnableMenuItem(SC_RESTORE, MF_BYCOMMAND | ((wndFrame.GetStyle() & (WS_MINIMIZE | WS_MAXIMIZE)) ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		menu.EnableMenuItem(SC_MOVE, MF_BYCOMMAND | ((wndFrame.GetStyle() & (WS_MINIMIZE | WS_MAXIMIZE)) == 0 ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		menu.EnableMenuItem(SC_SIZE, MF_BYCOMMAND | ((wndFrame.GetStyle() & (WS_MINIMIZE | WS_MAXIMIZE)) == 0 ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		menu.EnableMenuItem(SC_MINIMIZE, MF_BYCOMMAND | ((wndFrame.GetStyle() & WS_MINIMIZE) == 0 ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		menu.EnableMenuItem(SC_MAXIMIZE, MF_BYCOMMAND | ((wndFrame.GetStyle() & WS_MAXIMIZE) == 0 ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		menu.SetMenuDefaultItem(SC_CLOSE, FALSE);

		RECT rcWnd;
		GetWindowRect(&rcWnd);
		TPMPARAMS tpm = { sizeof(TPMPARAMS), { 0, 0, 20, 26 } };
		ClientToScreen(&tpm.rcExclude);
		sm_pCurrentHookingTitlebar = this;
		m_hSysMenuHook = ::SetWindowsHookEx(WH_GETMESSAGE, HookCheckDblClickProc, NULL, ::GetCurrentThreadId());
		UINT uAction = ::TrackPopupMenuEx(menu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL | TPM_RETURNCMD, rcWnd.left, rcWnd.bottom, m_hWnd, &tpm);
		::UnhookWindowsHookEx(m_hSysMenuHook);
		m_hSysMenuHook = NULL;
		sm_pCurrentHookingTitlebar = NULL;
		if (uAction != 0)
			wndFrame.PostMessage(WM_SYSCOMMAND, uAction, 0);
	}
}

void CTitleBarCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (point.x <= 20)
	{
		point.x = point.y = 10;
		ClientToScreen(&point);
		::PostMessage(GetRootWindow(m_hWnd), WM_NCLBUTTONDOWN, HTMENU, MAKELPARAM(point.x, point.y));
	}
}


void CTitleBarCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWindow wndMain = ::GetRootWindow(m_hWnd);
	if (point.x < 20)
	{
		if (nFlags == -1)
		{
			if (!m_bDoubleClkDouble)
				m_bDoubleClkDouble = true;
			else
			{
				m_bDoubleClkDouble = false;
				wndMain.PostMessage(WM_CLOSE);
			}
		}
	}
	else
	{
		if (wndMain.GetStyle() & WS_MAXIMIZE)
			wndMain.PostMessage(WM_SYSCOMMAND, SC_RESTORE);
		else
			wndMain.PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
	}
}

void CTitleBarCtrl::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	::UnhookWindowsHookEx(m_hSysMenuHook2);
	m_hSysMenuHook2 = NULL;
	sm_pCurrentHookingTitlebar = NULL;
}

//////////////////////////////////////////////////////////////////////////
// override

void CTitleBarCtrl::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
	CWindow wndMain = ::GetRootWindow(m_hWnd);

	CString strTitle;
	wndMain.GetWindowText(strTitle);

	RECT rcClient;
	GetClientRect(&rcClient);
	//s()->MainFrame()->DrawTitleBar(hDC, rcClient, strTitle, s()->Common()->GetMainIcon(), ::GetForegroundWindow() == wndMain, (wndMain.GetStyle() & WS_MAXIMIZE) != 0);
	s()->MainFrame()->DrawTitleBar(hDC, rcClient, strTitle, ::GetForegroundWindow() == wndMain, (wndMain.GetStyle() & WS_MAXIMIZE) != 0);
}
