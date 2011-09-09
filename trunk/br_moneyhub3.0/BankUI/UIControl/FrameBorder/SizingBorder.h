#pragma once


template<class T>
class CSizingBorder
{

	BEGIN_MSG_MAP_EX(T)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	END_MSG_MAP()


	LRESULT OnMouseMove(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		T *pT = static_cast<T*>(this);
		POINT pt;
		::GetCursorPos(&pt);
		HWND hCapture = ::GetCapture();
		CWindow hRoot = ::GetAncestor(pT->m_hWnd, GA_ROOTOWNER);
		if (hCapture != hRoot && hCapture != NULL)
		{
			bHandled = FALSE;
			return 0;
		}

		if (hRoot.GetStyle() & WS_MAXIMIZE)
		{
			bHandled = FALSE;
			return 0;
		}
		RECT rcWnd;
		::GetWindowRect(hRoot, &rcWnd);
		int iBorder = s()->MainFrame()->GetBorderWidth();
		int iResizeRange = s()->MainFrame()->GetResizeRange();

		LPCTSTR uMouseCursor = IDC_ARROW;
		if (pt.x <= rcWnd.left + iBorder)
		{
			if (pt.y <= rcWnd.top + iResizeRange)
				uMouseCursor = IDC_SIZENWSE;
			else if (pt.y >= rcWnd.bottom - iResizeRange)
				uMouseCursor = IDC_SIZENESW;
			else
				uMouseCursor = IDC_SIZEWE;
		}
		else if (pt.x >= rcWnd.right - iBorder)
		{
			if (pt.y <= rcWnd.top + iResizeRange)
				uMouseCursor = IDC_SIZENESW;
			else if (pt.y >= rcWnd.bottom - iResizeRange)
				uMouseCursor = IDC_SIZENWSE;
			else
				uMouseCursor = IDC_SIZEWE;
		}
		else if (pt.y <= rcWnd.top + iBorder)
		{
			if (pt.x <= rcWnd.left + iResizeRange)
				uMouseCursor = IDC_SIZENWSE;
			else if (pt.x >= rcWnd.right - iResizeRange)
				uMouseCursor = IDC_SIZENESW;
			else
				uMouseCursor = IDC_SIZENS;
		}
		else if (pt.y >= rcWnd.bottom - iBorder)
		{
			if (pt.x <= rcWnd.left + iResizeRange)
				uMouseCursor = IDC_SIZENESW;
			else if (pt.x >= rcWnd.right - iResizeRange)
				uMouseCursor = IDC_SIZENWSE;
			else
				uMouseCursor = IDC_SIZENS;
		}

		bHandled = uMouseCursor != IDC_ARROW;
		SetCursor(::LoadCursor(NULL, uMouseCursor));
		return 0;
	}

	LRESULT OnLButtonDown(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		T *pT = static_cast<T*>(this);
		POINT pt;
		::GetCursorPos(&pt);
		CWindow hRoot = ::GetAncestor(pT->m_hWnd, GA_ROOTOWNER);
		if (hRoot.GetStyle() & WS_MAXIMIZE)
		{
			bHandled = FALSE;
			return 0;
		}
		RECT rcWnd;
		::GetWindowRect(hRoot, &rcWnd);
		int iBorder = s()->MainFrame()->GetBorderWidth();
		int iResizeRange = s()->MainFrame()->GetResizeRange();

		UINT uHitTest = HTCLIENT;
		LPCTSTR uMouseCursor = IDC_ARROW;
		if (pt.x <= rcWnd.left + iBorder)
		{
			if (pt.y <= rcWnd.top + iResizeRange)
			{
				uHitTest = HTTOPLEFT;
				uMouseCursor = IDC_SIZENWSE;
			}
			else if (pt.y >= rcWnd.bottom - iResizeRange)
			{
				uHitTest = HTBOTTOMLEFT;
				uMouseCursor = IDC_SIZENESW;
			}
			else
			{
				uHitTest = HTLEFT;
				uMouseCursor = IDC_SIZEWE;
			}
		}
		else if (pt.x >= rcWnd.right - iBorder)
		{
			if (pt.y <= rcWnd.top + iResizeRange)
			{
				uHitTest = HTTOPRIGHT;
				uMouseCursor = IDC_SIZENESW;
			}
			else if (pt.y >= rcWnd.bottom - iResizeRange)
			{
				uHitTest = HTBOTTOMRIGHT;
				uMouseCursor = IDC_SIZENWSE;
			}
			else
			{
				uHitTest = HTRIGHT;
				uMouseCursor = IDC_SIZEWE;
			}
		}
		else if (pt.y <= rcWnd.top + iBorder)
		{
			if (pt.x <= rcWnd.left + iResizeRange)
			{
				uHitTest = HTTOPLEFT;
				uMouseCursor = IDC_SIZENWSE;
			}
			else if (pt.x >= rcWnd.right - iResizeRange)
			{
				uHitTest = HTTOPRIGHT;
				uMouseCursor = IDC_SIZENESW;
			}
			else
			{
				uHitTest = HTTOP;
				uMouseCursor = IDC_SIZENS;
			}
		}
		else if (pt.y >= rcWnd.bottom - iBorder)
		{
			if (pt.x <= rcWnd.left + iResizeRange)
			{
				uHitTest = HTBOTTOMLEFT;
				uMouseCursor = IDC_SIZENESW;
			}
			else if (pt.x >= rcWnd.right - iResizeRange)
			{
				uHitTest = HTBOTTOMRIGHT;
				uMouseCursor = IDC_SIZENWSE;
			}
			else
			{
				uHitTest = HTBOTTOM;
				uMouseCursor = IDC_SIZENS;
			}
		}
		SetCursor(::LoadCursor(NULL, uMouseCursor));

		bHandled = uMouseCursor != IDC_ARROW;
		if (uHitTest != HTCLIENT)
			::SendMessage(hRoot, WM_NCLBUTTONDOWN, uHitTest, MAKELPARAM(pt.x, pt.y));
		return 0;
	}
};
