#pragma once
#include "../../Skin/ResourceSkin/Common.h"
#include "../../Skin/UISkin/PopupFrameBorderSkin.h"
#include "../../Util/MainFrameAux/TaskbarExtension.h"
#include "../../UIControl/Tooltip/SogouToolTip.h"

#define HT_USER				100
#define HT_USER_CLOSE		500
#define HT_USER_MAXBTN		501
#define HT_USER_MINBTN		502

#define WM_CMD_USER			0x0200


template <class T>
class ATL_NO_VTABLE CPopupFrameBorder : public CWindowImpl<T>
{

public:

	CPopupFrameBorder() : m_uLastHitTest(HTNOWHERE), m_bInWindow(false),
		m_bActivate(false), m_iAdditionalBtnCount(0) {}

	void SetAdditionalBtnCount(int iCount)		{ m_iAdditionalBtnCount = iCount; }

private:

	BEGIN_MSG_MAP_EX(T)

		MESSAGE_HANDLER(WM_CREATE, OnCreate)

		MSG_WM_NCPAINT(OnNcPaint)
		MSG_WM_SETTEXT(OnSetText)

		MSG_WM_NCACTIVATE(OnNcActivate)

		MSG_WM_NCCALCSIZE(OnNcCalcSize)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)
		MESSAGE_HANDLER(WM_SIZE, OnSize)

		MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
		MSG_WM_NCMOUSELEAVE(OnNcMouseLeave)
		MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_NCRBUTTONUP(OnNcRButtonUp)

		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnGetDispInfo)
	END_MSG_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		ModifyStyle(WS_CAPTION, WS_SYSMENU);
		_DWMNCRENDERINGPOLICY ncrp = _DWMNCRP_DISABLED;
		TuoDwmSetWindowAttribute(m_hWnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////

	void OnNcPaint(CRgn rgn)
	{
		RECT rcWnd;
		GetWindowRect(&rcWnd);

		CWindowDC dc(m_hWnd);
		RECT rcTopBar = { 0, 0, rcWnd.right - rcWnd.left, TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight() };
		CMemoryDC memDC(dc, rcTopBar);

		T *pT = static_cast<T*>(this);
		int iRightMargin = TSM::GetTS()->pSkin->PopupFrame()->GetCloseButtonWidth() + ((GetStyle() & WS_MAXIMIZEBOX) ? TSM::GetTS()->pSkin->PopupFrame()->GetMaxRestoreButtonWidth() : 0) + TSM::GetTS()->pSkin->PopupFrame()->GetMinButtonWidth();
		for (int i = m_iAdditionalBtnCount - 1; i >= 0; i--)
			iRightMargin += pT->GetAdditionalButtonWidth(i);

		if ((GetStyle() & WS_MAXIMIZE) == WS_MAXIMIZE)
		{
			TSM::GetTS()->pSkin->PopupFrame()->DrawCaption(memDC, rcWnd, true);
			CString str;
			GetWindowText(str);
			TSM::GetTS()->pSkin->PopupFrame()->DrawText(memDC, rcWnd.right - rcWnd.left - iRightMargin, str, s()->Common()->GetVideoIcon(), m_bActivate);
		}
		else
		{
			TSM::GetTS()->pSkin->PopupFrame()->DrawLeftBorder(dc, rcWnd);
			TSM::GetTS()->pSkin->PopupFrame()->DrawRightBorder(dc, rcWnd);
			TSM::GetTS()->pSkin->PopupFrame()->DrawBottomBorder(dc, rcWnd);

			TSM::GetTS()->pSkin->PopupFrame()->DrawCaption(memDC, rcWnd, false);
			CString str;
			GetWindowText(str);
			TSM::GetTS()->pSkin->PopupFrame()->DrawText(memDC, rcWnd.right - rcWnd.left - iRightMargin, str, s()->Common()->GetVideoIcon(), m_bActivate);
		}

		TSM::GetTS()->pSkin->PopupFrame()->DrawCloseButton(memDC, rcWnd, Btn_MouseOut);
		if (GetStyle() & WS_MAXIMIZEBOX)
			TSM::GetTS()->pSkin->PopupFrame()->DrawMaxOrRestoreButton(memDC, rcWnd, Btn_MouseOut, (GetStyle() & WS_MAXIMIZE) != WS_MAXIMIZE);
		TSM::GetTS()->pSkin->PopupFrame()->DrawMinButton(memDC, rcWnd, Btn_MouseOut);
		rcWnd.right -= rcWnd.left;
		for (int i = m_iAdditionalBtnCount - 1; i >= 0; i--)
		{
			rcWnd.right -= pT->GetAdditionalButtonWidth(i);
			pT->DrawAdditionalButton(memDC, i, rcWnd.right, Btn_MouseOut);
		}
	}

	int OnSetText(LPCTSTR lpstrText)
	{
		DefWindowProc();
		OnNcPaint(NULL);
		return 0;
	}

	BOOL OnNcActivate(BOOL bActive)
	{
		BOOL bRet = CSkinLoader::IsVista() ? TRUE : DefWindowProc();
		m_bActivate = bActive != FALSE;
		OnNcPaint(NULL);
		return bRet;
	}

	//////////////////////////////////////////////////////////////////////////

	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
	{
		if (bCalcValidRects == FALSE)
			return 0;

		LPNCCALCSIZE_PARAMS calc = (LPNCCALCSIZE_PARAMS)lParam;

		RECT &rc = bCalcValidRects ? ((LPNCCALCSIZE_PARAMS)lParam)->rgrc[0] : *(LPRECT)lParam;
		if ((GetStyle() & WS_MAXIMIZE) == WS_MAXIMIZE)
		{
			rc.left += 1 - ::GetSystemMetrics(SM_CXFRAME);
			rc.top += TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight() - ::GetSystemMetrics(SM_CYFRAME) + 1;
			rc.right += ::GetSystemMetrics(SM_CXFRAME) - 1;
			rc.bottom += ::GetSystemMetrics(SM_CYFRAME) - 1;
		}
		else
		{
			rc.left += TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth() - ::GetSystemMetrics(SM_CXFRAME) + 1;
			rc.top += TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight() - ::GetSystemMetrics(SM_CYFRAME) + 1;
			rc.right += ::GetSystemMetrics(SM_CXFRAME) - TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth() - 1;
			rc.bottom += ::GetSystemMetrics(SM_CYFRAME) - TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth() - 1;
		}

		DefWindowProc();
		return 0;
	}

	UINT OnNcHitTest(CPoint point)
	{
		RECT rcWnd;
		GetWindowRect(&rcWnd);
		if (!::PtInRect(&rcWnd, point))
			return HTNOWHERE;

		RECT rcClient;
		GetClientRect(&rcClient);
		ScreenToClient(&point);
		if (point.x >= 0 && point.x <= rcClient.right && point.y >= 0 && point.y <= rcClient.bottom)
			return HTCLIENT;

		if ((GetStyle() & WS_MAXIMIZE) != WS_MAXIMIZE)
		{
			if (GetStyle() & WS_THICKFRAME)
			{
				if (point.x < 0)
				{
					if (point.y < 0)
						return HTTOPLEFT;
					else if (point.y > rcClient.bottom + TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth() - TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight())
						return HTBOTTOMLEFT;
					else
						return HTLEFT;
				}
				else if (point.x > rcClient.right)
				{
					if (point.y < 0)
						return HTTOPRIGHT;
					else if (point.y > rcClient.bottom + TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth() - TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight())
						return HTBOTTOMRIGHT;
					else
						return HTRIGHT;
				}
				else
				{
					if (point.y < TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth() - TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight())
					{
						if (point.x < TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight() - TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth())
							return HTTOPLEFT;
						else if (point.x > rcClient.right - TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight())
							return HTTOPRIGHT;
						return HTTOP;
					}
					else if (point.y > rcClient.bottom)
					{
						if (point.x < TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight() - TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth())
							return HTBOTTOMLEFT;
						else if (point.x > rcClient.right - TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight())
							return HTBOTTOMRIGHT;
						return HTBOTTOM;
					}
				}
			}

			if (point.y < 0)
			{
				int iRightMargin = rcWnd.right - rcWnd.left - TSM::GetTS()->pSkin->PopupFrame()->GetCloseButtonWidth() - TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth();
				if (point.x > iRightMargin)
					return HT_USER_CLOSE;
				if (GetStyle() & WS_MAXIMIZEBOX)
				{
					iRightMargin -= TSM::GetTS()->pSkin->PopupFrame()->GetMaxRestoreButtonWidth();
					if (point.x > iRightMargin)
						return HT_USER_MAXBTN;
				}
				iRightMargin -= TSM::GetTS()->pSkin->PopupFrame()->GetMinButtonWidth();
				if (point.x > iRightMargin)
					return HT_USER_MINBTN;
				T *pT = static_cast<T*>(this);
				for (int i = m_iAdditionalBtnCount - 1; i >= 0; i--)
				{
					iRightMargin -= pT->GetAdditionalButtonWidth(i);
					if (point.x >= iRightMargin)
						return HT_USER + i;
				}
			}

			if (point.x < (TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight() - ICON_WIDTH) / 2 + ICON_WIDTH && point.y < 0)
				return HTSYSMENU;
		}
		else		// 最大化时
		{
			if (point.x < (TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight() - ICON_WIDTH) / 2 + ICON_WIDTH)
				return HTSYSMENU;
			if (point.y < 0)
			{
				int iRightMargin = rcWnd.right - rcWnd.left - TSM::GetTS()->pSkin->PopupFrame()->GetCloseButtonWidth() - TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth();
				if (point.x > iRightMargin)
					return HT_USER_CLOSE;
				if (GetStyle() & WS_MAXIMIZEBOX)
				{
					iRightMargin -= TSM::GetTS()->pSkin->PopupFrame()->GetMaxRestoreButtonWidth();
					if (point.x > iRightMargin)
						return HT_USER_MAXBTN;
				}
				iRightMargin -= TSM::GetTS()->pSkin->PopupFrame()->GetMinButtonWidth();
				if (point.x > iRightMargin)
					return HT_USER_MINBTN;
				T *pT = static_cast<T*>(this);
				for (int i = m_iAdditionalBtnCount - 1; i >= 0; i--)
				{
					iRightMargin -= pT->GetAdditionalButtonWidth(i);
					if (point.x >= iRightMargin)
						return HT_USER + i;
				}
			}
		}

		return HTCAPTION;
	}

	void OnWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		if ((lpWndPos->flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE))
		{
			if (GetStyle() & WS_MAXIMIZE)
			{
				HMONITOR hMon = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO mi = { sizeof(MONITORINFO) };
				::GetMonitorInfo(hMon, &mi);
				lpWndPos->x = mi.rcWork.left;
				lpWndPos->y = mi.rcWork.top;
				lpWndPos->cx = mi.rcWork.right - mi.rcWork.left;
				lpWndPos->cy = mi.rcWork.bottom - mi.rcWork.top;
			}
		}
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		if ((GetStyle() & WS_MINIMIZE) != WS_MINIMIZE && (GetStyle() & WS_MAXIMIZE) != WS_MAXIMIZE)
		{
			RECT rcWnd;
			GetWindowRect(&rcWnd);
			HRGN hRgn = ::CreateRoundRectRgn(0, 0, rcWnd.right - rcWnd.left + 1, rcWnd.bottom - rcWnd.top + 1, 6, 6);
			SetWindowRgn(hRgn, TRUE);
		}
		else
			SetWindowRgn(NULL, TRUE);

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	void OnNcMouseMove(UINT nHitTest, CPoint point)
	{
		if (!m_bInWindow)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_NONCLIENT, m_hWnd, 0 };
			::TrackMouseEvent(&tme);
			m_bInWindow = true;
		}

		if (m_uLastHitTest == nHitTest)
			return;

		CWindowDC dc(m_hWnd);
		RECT rcWnd;
		GetWindowRect(&rcWnd);
		ChangeStatus(m_uLastHitTest, Btn_MouseOut);
		m_uLastHitTest = nHitTest;
		ChangeStatus(m_uLastHitTest, Btn_MouseOver);

		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, m_uLastHitTest);
	}

	void OnNcMouseLeave()
	{
		ChangeStatus(m_uLastHitTest, Btn_MouseOut);
		m_bInWindow = false;
		m_uLastHitTest = HTNOWHERE;

		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}

	void OnNcLButtonDown(UINT nHitTest, CPoint point)
	{
		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);

		if (nHitTest == HT_USER_CLOSE || nHitTest == HT_USER_MAXBTN || nHitTest == HT_USER_MINBTN || nHitTest >= HT_USER)
		{
			ChangeStatus(nHitTest, Btn_MouseDown);
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_LEAVE | TME_NONCLIENT, m_hWnd, 0 };
			::TrackMouseEvent(&tme);
			m_bInWindow = false;
			SetCapture();
		}
		else if (nHitTest == HTSYSMENU)
		{
			RECT rcWnd;
			GetWindowRect(&rcWnd);
//			CMenuHandle mm = GetSystemMenu(FALSE);
//			UINT uCmd = mm.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_RECURSE, rcWnd.left + ((GetStyle() & WS_MAXIMIZE) == WS_MAXIMIZE ? 0 : TSM::GetTS()->pSkin->PopupFrame()->GetBorderWidth()), rcWnd.top + TSM::GetTS()->pSkin->PopupFrame()->GetCaptionHeight(), m_hWnd, NULL);
//			if (uCmd != 0)
//				PostMessage(WM_SYSCOMMAND, uCmd, 0);

			DefWindowProc(WM_NCLBUTTONDOWN, nHitTest, MAKELPARAM(rcWnd.left + ::GetSystemMetrics(SM_CXFRAME), rcWnd.top + ::GetSystemMetrics(SM_CYFRAME)));
		}
		else
			DefWindowProc();
	}

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (::GetCapture() == m_hWnd)
		{
			ReleaseCapture();
			ClientToScreen(&point);
			UINT uHitTest = OnNcHitTest(point);

			if (m_uLastHitTest == uHitTest)
			{
				switch (uHitTest)
				{
					case HT_USER_CLOSE:
						PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
						break;
					case HT_USER_MAXBTN:
						PostMessage(WM_SYSCOMMAND, (GetStyle() & WS_MAXIMIZE) != WS_MAXIMIZE ? SC_MAXIMIZE : SC_RESTORE, 0);
						break;
					case HT_USER_MINBTN:
						PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
						break;
					default:
						if (uHitTest >= HT_USER)
							PostMessage(WM_COMMAND, MAKELONG(0, WM_CMD_USER + uHitTest - HT_USER));
				}
				ChangeStatus(uHitTest, Btn_MouseOver);
			}
			else
			{
				ChangeStatus(m_uLastHitTest, Btn_MouseOut);
				m_uLastHitTest = uHitTest;
				ChangeStatus(m_uLastHitTest, Btn_MouseOver);
			}
			m_uLastHitTest = HTNOWHERE;
		}
	}

	void OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		if (m_uLastHitTest == HTSYSMENU)
			PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
	}

	void OnNcRButtonUp(UINT nHitTest, CPoint point)
	{
		if (nHitTest == HTCAPTION)
			SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(point.x, point.y));
	}

	LRESULT OnGetDispInfo(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
	{
		static LPCTSTR lpszToolbarTip[] = { _T("关闭"), _T("最大化"), _T("最小化") };
		LPNMSTTDISPINFO pnmdsp = (LPNMSTTDISPINFO)pnmh;
		if (pnmdsp->pdwIndex >= HT_USER_CLOSE)
		{
			pnmdsp->szText = lpszToolbarTip[pnmdsp->pdwIndex - HT_USER_CLOSE];
			pnmdsp->dwFlags = STTDSPF_TEXT;
		}
		else
			bHandled = FALSE;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	void ChangeStatus(UINT uHitTest, ButtonStatus btnStatus)
	{
		RECT rcWnd;
		GetWindowRect(&rcWnd);
		CWindowDC dc(m_hWnd);
		switch (uHitTest)
		{
			case HT_USER_CLOSE:
				TSM::GetTS()->pSkin->PopupFrame()->DrawCloseButton(dc, rcWnd, btnStatus);
				break;
			case HT_USER_MAXBTN:
				rcWnd.right -= TSM::GetTS()->pSkin->PopupFrame()->GetCloseButtonWidth();
				TSM::GetTS()->pSkin->PopupFrame()->DrawMaxOrRestoreButton(dc, rcWnd, btnStatus, (GetStyle() & WS_MAXIMIZE) != WS_MAXIMIZE);
				break;
			case HT_USER_MINBTN:
				rcWnd.right -= ((GetStyle() & WS_MAXIMIZEBOX) ? TSM::GetTS()->pSkin->PopupFrame()->GetMaxRestoreButtonWidth() : 0) + TSM::GetTS()->pSkin->PopupFrame()->GetCloseButtonWidth();
				TSM::GetTS()->pSkin->PopupFrame()->DrawMinButton(dc, rcWnd, btnStatus);
				break;
			default:
				if (uHitTest >= HT_USER)
				{
					T *pT = static_cast<T*>(this);
					rcWnd.right -= rcWnd.left + ((GetStyle() & WS_MAXIMIZEBOX) ? TSM::GetTS()->pSkin->PopupFrame()->GetMaxRestoreButtonWidth() : 0) + TSM::GetTS()->pSkin->PopupFrame()->GetCloseButtonWidth() + TSM::GetTS()->pSkin->PopupFrame()->GetMinButtonWidth();
					for (int i = m_iAdditionalBtnCount - 1; i >= (int)uHitTest - HT_USER; i--)
						rcWnd.right -= pT->GetAdditionalButtonWidth(i);
					pT->DrawAdditionalButton(dc, uHitTest - HT_USER, rcWnd.right, btnStatus);
				}
		}

	}

	//////////////////////////////////////////////////////////////////////////
	// overridable

	int GetAdditionalButtonWidth(int iIndex) { return 0; }
	void DrawAdditionalButton(CDCHandle dc, int iIndex, int iXPos, ButtonStatus btnStatus) {}
	bool OnAllowFrameNcDeactivate()	{ return true; }

	//////////////////////////////////////////////////////////////////////////

	UINT m_uLastHitTest;
	bool m_bInWindow;
	bool m_bActivate;
	int m_iAdditionalBtnCount;
};
