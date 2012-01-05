#pragma once
#include "../../Skin/SkinManager.h"
#include "MouseObjContainer.h"

#define COMBO_ICON_TEXT_MARGIN			3
#define COMBO_TEXT_DROPDOWN_MARGIN		3


template <class T>
class ATL_NO_VTABLE CEditContainerBase : public CWindowImpl<T>, public CMouseObjContainer<T>
{

public:

	CEditContainerBase() : m_hEditControl(NULL), m_bUseIcon(true) {}

	void SetUseIcon(bool bUseIcon)
	{
		m_bUseIcon = bUseIcon;
	}
	void SetEditControlIcon(HICON hIcon)
	{
		if (m_ico)
			m_ico.DestroyIcon();
		m_ico = ::CopyIcon(hIcon);
		Invalidate();
	}
	void SetEditControlWnd(HWND hWnd)
	{
		m_hEditControl = hWnd;
		RegisterMouseObjChild(m_hEditControl, false);
	}
	void SetDropDownButtonStatus(bool bIsDown)
	{
		m_bIsPushDownButtonDown = bIsDown;
		Invalidate();
	}

private:

	BEGIN_MSG_MAP_EX(T)
		CHAIN_MSG_MAP(CMouseObjContainer<T>)

		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER_EX(WM_USER + 0x000f, OnMouseMoveChild)
	END_MSG_MAP()


	void OnPaint(CDCHandle dc, RECT rect)
	{
		GetClientRect(&rect);
		if (rect.right <= rect.left)
			return;

		CMemoryDC memDC(dc, rect);
		T *pT = static_cast<T*>(this);
		pT->PaintBackground(memDC, rect);
		s()->Combo()->DrawComboEditBackground((HDC)memDC, rect, GetStatus(), m_iContainerType);
		int iY = (s()->Combo()->GetComboBackgroundHeight(m_iContainerType) - ICON_WIDTH) / 2;
		if (m_bUseIcon)
			s()->Common()->DrawIcon((HDC)memDC, m_ico, iY, iY, pT->IsTransparentDraw());
		pT->PaintAddition(memDC, rect);

		// 由于richedit是用WS_EX_TRANSPARENT模式创建的，所以在edit container重绘时，要把edit control的内容添加到这里
		::SendMessage(m_hEditControl, WM_PAINT, (WPARAM)(HDC)memDC, 1);
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		if (::IsWindow(m_hEditControl))
		{
			RECT rcClient;
			GetClientRect(&rcClient);
			T *pT = static_cast<T*>(this);
			int iLeft, iRight;
			pT->GetLeftAndRightMargin(iLeft, iRight);
			int iIconMargin = (s()->Combo()->GetComboBackgroundHeight(m_iContainerType) - ICON_WIDTH) / 2;
			int iY = (s()->Combo()->GetComboBackgroundHeight(m_iContainerType) - s()->Combo()->GetComboEditHeight(m_iContainerType)) / 2 + s()->Combo()->GetComboEditTopOffset(m_iContainerType);
			int iLeftDefaultMargin = iIconMargin + (m_bUseIcon ? ICON_WIDTH + COMBO_ICON_TEXT_MARGIN : 0);
			int iRightDefaultMargin = m_iContainerType == 0 ? (s()->Combo()->GetComboDropdownTriangleButtonWidth() + COMBO_TEXT_DROPDOWN_MARGIN) : iY;
			::SetWindowPos(m_hEditControl, NULL, iLeftDefaultMargin + iLeft, iY, rcClient.right - iRight - iLeft - iLeftDefaultMargin - iRightDefaultMargin, s()->Combo()->GetComboEditHeight(m_iContainerType), SWP_NOZORDER);
		}
		return 0;
	}

	BOOL OnEraseBkgnd(CDCHandle dc) { return TRUE; }

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		::SetFocus(m_hEditControl);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	void PaintBackground(CDCHandle dc, RECT rc) {}
	void PaintAddition(CDCHandle dc, RECT rc) {}
	void GetLeftAndRightMargin(int &left, int &right) { left = right = 0; }
	bool IsTransparentDraw() const	{ return false; }


	HWND m_hEditControl;
	CIcon m_ico;

	bool m_bUseIcon;
};
