#include "stdafx.h"
#include "../../Skin/SkinManager.h"
#include "../TabCtrl/TabCtrl.h"
#include "../MainFrame.h"
#include "BrowserStatusBar.h"
#include "../../Util/ThreadStorageManager.h"

#define STATUS_BAR_MARGIN			5
#define STATUS_BAR_ICON_TEXT_SEP	3


CBrowserStatusBarCtrl::CBrowserStatusBarCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_bIgnoreUnderline(false), 
m_iIconIndex(0), m_bShowProgress(false), m_iProgress(0)
{
	FS()->pStatus = this;
}


void CBrowserStatusBarCtrl::CreateBrowserStatusBar(HWND hParent)
{
	DWORD dwToolbarStyle = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE;
	HWND hWnd = Create(hParent, CRect(0, 0, 0, s()->StatusBar()->GetStatusBarHeight(false)), NULL, dwToolbarStyle);
	ATLASSERT(hWnd && "Create status bar failed.");
}

void CBrowserStatusBarCtrl::SetPageProgress(int iProgress)
{
	bool bRedraw = m_iProgress != iProgress;
	m_iProgress = iProgress;

	if (iProgress == 100)
	{
		if (m_bShowProgress)
		{
			m_bShowProgress = false;
			KillTimer(1);
			bRedraw = true;
		}
	}
	else
	{
		if (!m_bShowProgress)
		{
			m_bShowProgress = true;
			SetTimer(1, 100);
			bRedraw = true;
		}
	}

	if (bRedraw)
		Invalidate();
}

void CBrowserStatusBarCtrl::SetStatusText(LPCTSTR lpszStatusText, bool bIgnoreUnderline)
{
	m_strText = lpszStatusText;
	m_bIgnoreUnderline = bIgnoreUnderline;
	Invalidate();
}

void CBrowserStatusBarCtrl::SetStatusIcon(int iIcon)
{
	m_iIconIndex = iIcon;
	//Invalidate();
	InvalidateRect(m_rcPageIcon);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CBrowserStatusBarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//	m_progress.CreateProgressBar(m_hWnd);
	return 1;
}


BOOL CBrowserStatusBarCtrl::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

void CBrowserStatusBarCtrl::OnPaint(CDCHandle dc, RECT rect)
{
	CMemDC Memdc(dc);

	GetClientRect(&rect);
	s()->StatusBar()->DrawStatusBarBack(Memdc, rect, (GetParent().GetStyle() & WS_MAXIMIZE) != 0, 0, FS()->pCate->IsSecure());

//	RECT rcRight;
//	if (::IsWindowVisible2(m_progress))
//		m_progress.GetWindowRect(&rcRight);
//	ScreenToClient(&rcRight);

	rect.top += 6;

	if (m_bShowProgress)
	{
		rect.left = STATUS_BAR_MARGIN + 5 + 16;
		rect.right = 320;//rect.right - STATUS_BAR_MARGIN - 6 - 200; 
	}
	else
	{
		rect.left = STATUS_BAR_MARGIN + 10;
		rect.right = rect.right - STATUS_BAR_MARGIN - 10;
	}

	//Memdc.SetTextColor(CSkinProperty::GetColor(Skin_Property_Statusbar_Text_Color));
	Memdc.SetTextColor(RGB(100, 100, 100));
	Memdc.SelectFont(s()->Common()->GetDefaultFont());

	Memdc.SetBkMode(TRANSPARENT);
	Memdc.DrawText(m_strText.c_str(), -1, &rect, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | (m_bIgnoreUnderline ? DT_NOPREFIX : 0));

	if (m_bShowProgress)
	{
		CRect rc(STATUS_BAR_MARGIN, (rect.bottom - rect.top - s()->StatusBar()->GetPageStatusIconHeight()) / 2 + 1 + 5, 0, 0);
		s()->StatusBar()->DrawStatusPageIcon(Memdc, rc, m_iIconIndex);
		m_rcPageIcon.SetRect(rc.left, rc.top, 24, 24);

		rc.SetRect(rect.right + 10, (rect.bottom - rect.top - s()->StatusBar()->GetPageStatusIconHeight()) / 2 + 1 + 5, 0, 0);
		s()->StatusBar()->DrawStatusSplit(Memdc, rc);

		rc.OffsetRect(10, 0);
		s()->StatusBar()->DrawStatusProgress(Memdc, rc, m_iProgress);
	}
}

void CBrowserStatusBarCtrl::OnSize(UINT nType, CSize size)
{
	RECT rcClient;
	GetClientRect(&rcClient);
	if (rcClient.right - rcClient.left < 10)
		return;

	Invalidate(FALSE);
}

void CBrowserStatusBarCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	RECT rcClient;
	GetClientRect(&rcClient);
	if (point.x > rcClient.right - 20 && (FS()->MainFrame()->GetStyle() & WS_MAXIMIZE) == 0)
		SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
	else
		SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

void CBrowserStatusBarCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	RECT rcClient;
	GetClientRect(&rcClient);
	if (point.x > rcClient.right - 20 && (FS()->MainFrame()->GetStyle() & WS_MAXIMIZE) == 0)
	{
		SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
		ClientToScreen(&point);
		::SendMessage(FS()->MainFrame()->m_hWnd, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, MAKELPARAM(point.x, point.y));
	}
}

void CBrowserStatusBarCtrl::OnTimer(UINT_PTR nIDEvent)
{
	m_iIconIndex = (m_iIconIndex + 1) % 8;
	SetStatusIcon(m_iIconIndex);
}