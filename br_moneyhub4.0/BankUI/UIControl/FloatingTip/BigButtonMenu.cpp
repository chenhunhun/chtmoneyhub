#include "stdafx.h"
#include "../../Skin/SkinManager.h"
#include "../BigButton.h"
#include "BigButtonMenu.h"


CBigButtonMenu::CBigButtonMenu(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_iCurrentSelection(-1), m_eDeleteButtonStatus(Btn_MouseOut)
{
}

CBigButtonMenu::~CBigButtonMenu()
{
}


HWND CBigButtonMenu::CreateBigButtonMenu(HWND hFrameWnd)
{
	return Create(hFrameWnd, NULL, NULL, WS_POPUP | WS_BORDER, WS_EX_NOACTIVATE);
}

void CBigButtonMenu::ShowMenu()
{
	RECT rcParent;
	FS()->BigButton()->GetWindowRect(&rcParent);

	SetWindowPos(HWND_TOP, rcParent.left, rcParent.bottom - 5, 120, 1 + s()->Combo()->GetComboDropdownItemBackgroundHeight(), SWP_SHOWWINDOW | SWP_NOACTIVATE);
	Invalidate();
}

void CBigButtonMenu::HideMenu()
{
	m_iCurrentSelection = -1;
	ShowWindow(SW_HIDE);
}

//////////////////////////////////////////////////////////////////////////
// message handler

void CBigButtonMenu::OnPaint(CDCHandle dc, RECT rect)
{
	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_POS };
	GetScrollInfo(SB_VERT, &si);

	CMemoryDC memDC(dc, rect);

	RECT rcClient;
	GetClientRect(&rcClient);
	int iDelta = s()->Combo()->GetComboDropdownItemBackgroundHeight();
	for (int i = 0; i < 1; i++)
	{
		RECT rcItem = { 0, i * iDelta, rcClient.right, i * iDelta + iDelta };
		s()->Combo()->DrawComboDropDownListItem(memDC, rcItem, m_iCurrentSelection == i ? Btn_MouseOver : Btn_MouseOut, _T("¸öÐÔÒ³"));
	}
}


void CBigButtonMenu::OnMouseMove(UINT nFlags, CPoint point)
{
	int iDelta = s()->Combo()->GetComboDropdownItemBackgroundHeight();
	int iSel = point.y / iDelta;

	if (iSel != m_iCurrentSelection)
	{
		m_iCurrentSelection = iSel;
		Invalidate();
	}
}

void CBigButtonMenu::OnLButtonUp(UINT nFlags, CPoint point)
{
	int iDelta = s()->Combo()->GetComboDropdownItemBackgroundHeight();
	int iSelection = point.y / iDelta;

//	FS()->AddrCombo()->StartNavigate(1 + m_LocalData[iSelection].iType, 0, m_LocalData[iSelection].strURL);
}
