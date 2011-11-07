#pragma once


// 之所以要弄这么一个类，主要是需要传递一些数据，模板类实例化之后，就不好写成指针的形式了

class CBaseProperties
{
public:

	CBaseProperties() : m_bIsInWindow(false), m_iAlwaysStatus(0), m_iContainerType(-1), m_bIsPushDownButtonDown(false) {}

	bool IsInWindow() const			{ return m_iAlwaysStatus > 0 ? true : m_bIsInWindow; }

	void SetContainerType(int iType)		{ m_iContainerType = iType; }

	ButtonStatus GetStatus() const
	{
		return IsInWindow() ? ((m_iContainerType == 0 && m_bIsPushDownButtonDown) ? Btn_MouseDown : Btn_MouseOver) : Btn_MouseOut;
	}

	int m_iAlwaysStatus;
	bool m_bIsInWindow;

	int m_iContainerType;
	bool m_bIsPushDownButtonDown;
};



template <class T>
class ATL_NO_VTABLE CMouseObjContainer : public CBaseProperties
{

public:

	void RegisterMouseObjChild(HWND hWnd, bool bAutoUpdate)
	{
		m_ChildWndSet.insert(std::make_pair(hWnd, bAutoUpdate));
		T *pT = static_cast<T*>(this);
		::SendMessage(hWnd, WM_USER + 0x000f, 0, (LPARAM)pT->m_hWnd);
	}

	void SetAlwaysOn(bool bFlag)
	{
		m_iAlwaysStatus = bFlag ? 1 : 0;
		InvalidateAll();
	}

private:

	void InvalidateAll()
	{
		T *pT = static_cast<T*>(this);
		pT->Invalidate();
		for (ChildWindowSet::iterator it = m_ChildWndSet.begin(); it != m_ChildWndSet.end(); it++)
			if (it->second)
				::InvalidateRect(it->first, NULL, FALSE);
	}

	BEGIN_MSG_MAP_EX(T)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)

		MESSAGE_HANDLER_EX(WM_USER + 0x000f, OnMouseMoveChild)
		MESSAGE_HANDLER_EX(WM_USER + 0x000e, OnSetAlwaysOnOrOff)
	END_MSG_MAP()


	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		T *pT = static_cast<T*>(this);
		if (!m_bIsInWindow)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, pT->m_hWnd };
			::TrackMouseEvent(&tme);
			m_bIsInWindow = true;
			InvalidateAll();
		}
		return 0;
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		POINT pt;
		::GetCursorPos(&pt);
		HWND hMouseWnd = ::WindowFromPoint(pt);
		if (m_ChildWndSet.find(hMouseWnd) == m_ChildWndSet.end() || ::SendMessage(hMouseWnd, WM_USER + 0x000e, 0, MAKELPARAM(pt.x, pt.y)) == 0)
		{
			m_bIsInWindow = false;
			InvalidateAll();
		}
		return 0;
	}


	LRESULT OnMouseMoveChild(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		T *pT = static_cast<T*>(this);
		if (wParam == 0)
		{
			POINT pt;
			::GetCursorPos(&pt);
			HWND hMouseWnd = ::WindowFromPoint(pt);
			if (hMouseWnd != pT->m_hWnd && m_ChildWndSet.find(hMouseWnd) == m_ChildWndSet.end())
			{
				m_bIsInWindow = false;
				InvalidateAll();
			}
			else
			{
				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, pT->m_hWnd };
				::TrackMouseEvent(&tme);
			}
		}
		else
		{
			if (!m_bIsInWindow)
			{
				m_bIsInWindow = true;
				InvalidateAll();
			}
		}
		return 0;
	}


	LRESULT OnSetAlwaysOnOrOff(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SetAlwaysOn(wParam != 0);
		return 0;
	}

private:

	typedef std::map<HWND, bool> ChildWindowSet;
	ChildWindowSet m_ChildWndSet;

};
