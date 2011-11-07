#pragma once
#include "../../Skin/ResourceSkin/Common.h"



template <class T>
class ATL_NO_VTABLE CMainFrameBorder : public CWindowImpl<T>
{

public:

	CMainFrameBorder() : m_bNeedAdditionalRgn(false), m_bIsFullScreen(false)
	{
		memset(&m_rcLastWindowRgn, 0, sizeof(m_rcLastWindowRgn));
	}

	void SetRestoreRectAndMaxStatus(RECT rc, bool bMax)		{ m_rcLastWindowPos = rc; }
	RECT GetRestoreRect() const		{ return m_rcLastWindowPos; }
	bool IsFullScreen() const		{ return m_bIsFullScreen; }

	void SetRgnForWindow(int w, int h)
	{
		if ((GetStyle() & WS_MINIMIZE) != WS_MINIMIZE)
		{
			HRGN hRgn;
			if ((GetStyle() & WS_MAXIMIZE) != WS_MAXIMIZE)
			{
				hRgn = ::CreateRoundRectRgn(0, 0, w + 1, h + 1, 5, 5);
			}
			else
				hRgn = NULL;
			SetWindowRgn(hRgn, TRUE);
		}
	}

private:

	BEGIN_MSG_MAP_EX(T)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_NCMOUSELEAVE, OnNcMouseLeave)
	END_MSG_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		ModifyStyle(WS_CAPTION, 0);

		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		if ((wParam & 0xfff0) == SC_MINIMIZE)
		{
			if (!(GetStyle() & WS_MAXIMIZE))
				GetWindowRect(&m_rcLastWindowPos);
		}
		else if ((wParam & 0xfff0) == SC_MAXIMIZE)
		{
			if (!(GetStyle() & WS_MAXIMIZE))
				GetWindowRect(&m_rcLastWindowPos);
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		if (wParam == FALSE)
			return 0;
		T *pT = static_cast<T*>(this);
		LPNCCALCSIZE_PARAMS calc = (LPNCCALCSIZE_PARAMS)lParam;
		RECT &rc = ((LPNCCALCSIZE_PARAMS)lParam)->rgrc[0];

		rc.left -= ::GetSystemMetrics(SM_CXFRAME) - 1;
		rc.top -= ::GetSystemMetrics(SM_CYFRAME) - 1;
		rc.right += ::GetSystemMetrics(SM_CXFRAME) - 1;
		rc.bottom += ::GetSystemMetrics(SM_CYFRAME) - 1;
		return 0;
	}

	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
		if ((lpWndPos->flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE))
		{
			// 如果是系统接管边框的大小计算，则无需进行特殊处理
			if ((GetStyle() & WS_MAXIMIZE))
			{
				RECT rc;
				GetWindowRect(&rc);
				HMONITOR hMon = NULL;
				if (rc.right - rc.left < 320 || rc.bottom - rc.top < 300)
					hMon = ::MonitorFromRect(&m_rcLastWindowPos, MONITOR_DEFAULTTONEAREST);
				else
					hMon = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);

				RECT rcWork;
				MONITORINFO mi = { sizeof(MONITORINFO) };
				::GetMonitorInfo(hMon, &mi);
				rcWork = IsFullScreen() ? mi.rcMonitor : mi.rcWork;

				lpWndPos->x = rcWork.left;
				lpWndPos->y = rcWork.top;
				lpWndPos->cx = rcWork.right - rcWork.left;
				lpWndPos->cy = rcWork.bottom - rcWork.top;
			}
		}
		return 0;
	}

	LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
		if ((lpWndPos->flags & SWP_NOSIZE) == 0)
			SetRgnForWindow(lpWndPos->cx, lpWndPos->cy);
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	bool m_bIsMax;							// 是否处于最大化

	RECT m_rcLastWindowPos;
	RECT m_rcLastWindowRgn;

	bool m_bNeedAdditionalRgn;

	bool m_bIsFullScreen;
};
