#pragma once


template <class T>
class ATL_NO_VTABLE CMouseObjChild
{

public:

	CMouseObjChild() : m_bIsInWindow(false), m_bIsInWindowAndInRegion(false), m_hMouseObjContainer(NULL) {}

	void SetContainerAlwaysOnOrOff(int iAlways)
	{
		::SendMessage(m_hMouseObjContainer, WM_USER + 0x000e, iAlways, 0);
	}

private:

	BEGIN_MSG_MAP_EX(T)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

		MESSAGE_HANDLER_EX(WM_USER + 0x000f, OnMouseMoveChild)
		MESSAGE_HANDLER_EX(WM_USER + 0x000e, OnCheckIsMouseInRegion)
	END_MSG_MAP()


	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		if (!m_hMouseObjContainer)
			return 0;
		T *pT = static_cast<T*>(this);
		if (!m_bIsInWindow)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, pT->m_hWnd };
			::TrackMouseEvent(&tme);
			m_bIsInWindow = true;
			m_bIsInWindowAndInRegion = pT->CheckIsMouseOver(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
			::SendMessage(m_hMouseObjContainer, WM_USER + 0x000f, m_bIsInWindowAndInRegion ? 1 : 0, 0);
		}
		else
		{
			bool bInRegion = pT->CheckIsMouseOver(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
			if (bInRegion != m_bIsInWindowAndInRegion)
			{
				m_bIsInWindowAndInRegion = bInRegion;
				::SendMessage(m_hMouseObjContainer, WM_USER + 0x000f, m_bIsInWindowAndInRegion ? 1 : 0, 0);
			}
		}
		return 0;
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		if (!m_hMouseObjContainer)
			return 0;
		m_bIsInWindow = false;
		m_bIsInWindowAndInRegion = false;
		::SendMessage(m_hMouseObjContainer, WM_USER + 0x000f, 0, 0);
		return 0;
	}

	LRESULT OnMouseMoveChild(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		m_hMouseObjContainer = (HWND)lParam;
		return 0;
	}

	LRESULT OnCheckIsMouseInRegion(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		T *pT = static_cast<T*>(this);
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		pT->ScreenToClient(&pt);
		return pT->CheckIsMouseOver(pt);
	}

	// callback

	bool CheckIsMouseOver(POINT pt)		{ return true; }

private:

	HWND m_hMouseObjContainer;
	bool m_bIsInWindow;
	bool m_bIsInWindowAndInRegion;

};
