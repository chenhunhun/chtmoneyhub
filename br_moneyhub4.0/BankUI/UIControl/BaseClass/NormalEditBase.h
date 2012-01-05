#pragma once
#include "../Menu/Menu.h"
#include "MouseObjContainer.h"
#include "MouseObjChild.h"


extern HDC g_hReplacePaintDC;
extern PAINTSTRUCT g_replacePaintStruct;


template <class T>
class ATL_NO_VTABLE CNormalEditControlBase : public CWindowImpl<T, CRichEditCtrl>, public CMouseObjChild<T>
{

public:

	CNormalEditControlBase(CBaseProperties *pBaseProperties) : m_bRightClickToSetFocus(false), m_pBaseProperties(pBaseProperties) {}

	HWND CreateEditControl(HWND hWndParent)
	{
		return Create(hWndParent, rcDefault, NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, WS_EX_TRANSPARENT);
	}

private:

	// message

	BEGIN_MSG_MAP_EX(T)
		MSG_WM_TUOTUO_MENU()
		CHAIN_MSG_MAP(CMouseObjChild<T>)

		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_CHAR, OnChar)

		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		SetFont(s()->Common()->GetDefaultFont());
		SetTextMode(TM_PLAINTEXT);
		SetEventMask(ENM_CHANGE);
		return 1;
	}

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		SetContainerAlwaysOnOrOff(1);
		if (m_bRightClickToSetFocus)
		{
			m_bRightClickToSetFocus = false;
			SetSelAll();
		}
		PostMessage(EM_SETSEL, 0, -1);
		return 0;
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		m_bRightClickToSetFocus = false;
		SetContainerAlwaysOnOrOff(0);
		SetSel(0, 0);
		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		SetCursor(::LoadCursor(NULL, IDC_IBEAM));
		return 0;
	}

	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		m_bRightClickToSetFocus = true;
		return 0;
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		if (wParam == VK_TAB)
			bHandled = TRUE;
		else if (wParam == VK_INSERT && ::GetKeyState(VK_SHIFT) >= 0 && ::GetKeyState(VK_CONTROL) >= 0)
			bHandled = TRUE;
		return 0;
	}

	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (wParam == VK_TAB)
			bHandled = TRUE;
		else
			bHandled = FALSE;
		return 0;
	}


	void DrawTransparent(CDCHandle dc, const RECT &rc)
	{
		T *pT = static_cast<T*>(this);
		if (pT->SelfPaint(dc, rc))
			return;

		CTuoPatchDraw patchDraw(rc.right, rc.bottom);
		patchDraw.BeginDraw();

		g_replacePaintStruct.fErase = TRUE;
		g_replacePaintStruct.rcPaint = rc;

		g_hReplacePaintDC = patchDraw.GetDC(0);
		DefWindowProc(WM_PAINT, 0, 0);
		g_hReplacePaintDC = patchDraw.GetDC(1);
		DefWindowProc(WM_PAINT, 0, 0);
		g_hReplacePaintDC = NULL;

		patchDraw.EndDraw();
		patchDraw.AlphaBlend(dc, 0, 0);
	}

	void DrawOpaque(CDCHandle dc, const RECT &rc)
	{
		T *pT = static_cast<T*>(this);
		if (!pT->SelfPaint(dc, rc))
		{
			g_replacePaintStruct.fErase = TRUE;
			g_replacePaintStruct.rcPaint = rc;
			g_hReplacePaintDC = dc;
			DefWindowProc(WM_PAINT, 0, 0);
			g_hReplacePaintDC = NULL;
		}
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		T *pT = static_cast<T*>(this);

		RECT rc;
		GetClientRect(&rc);
		RECT rcWnd;
		GetWindowRect(&rcWnd);
		GetParent().ScreenToClient(&rcWnd);

		if (lParam)			// 表示这个paint消息是edit container发来的，意为重绘请求，其中wparam为要求重绘的memdc
		{
			CDCHandle dc((HDC)wParam);
			POINT pt;
			dc.SetViewportOrg(rcWnd.left, rcWnd.top, &pt);
			if (pT->IsTransparentDraw())
				DrawTransparent(dc, rc);
			else
				DrawOpaque(dc, rc);
			dc.SetViewportOrg(pt.x, pt.y, NULL);
			Invalidate();
		}
		else
		{
			CLayeredMemDC memDC;		// 这里不用CMemoryDC的原因是，memorydc最后还要bitblt回去，这里我们不需要这个操作
			memDC.CreateMemDC(rc.right, rc.bottom);
			if (pT->IsTransparentDraw())
			{
				s()->Combo()->DrawComboEditBackgroundPart(memDC, rc, rcWnd.top, m_pBaseProperties->GetStatus(), m_pBaseProperties->m_iContainerType, true);
				DrawTransparent(memDC, rc);
			}
			else
			{
				pT->DrawContainerBackground(memDC, rc);
				s()->Combo()->DrawComboEditBackgroundPart(memDC, rc, rcWnd.top, m_pBaseProperties->GetStatus(), m_pBaseProperties->m_iContainerType, false);
				DrawOpaque(memDC, rc);
			}
			CPaintDC dc(m_hWnd);			// 这句话必需在richedit paint之后写，否则在xp下面会有问题
			dc.BitBlt(0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// overridable function

	bool SelfPaint(CDCHandle dc, const RECT &rc) { return false; }
	bool IsTransparentDraw() const	{ return false; }
	void DrawContainerBackground(CDCHandle dc, const RECT &rc) {}


	bool m_bRightClickToSetFocus;			// 用来记录是否是用户点击右键来激活焦点的

	CBaseProperties *m_pBaseProperties;
};
