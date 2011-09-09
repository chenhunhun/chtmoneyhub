#include "stdafx.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "BigButton.h"


CBigButton::CBigButton(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_BigButtonMenu(pFS)
{
	FS()->pBigButton = this;
}


void CBigButton::CreateBigButton(HWND hParent)
{
	HWND hWnd = Create(hParent, CRect(0, 0, s()->BigButton()->GetWidth(), s()->BigButton()->GetHeight()), NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd && "Create main toolbar failed.");
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CBigButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->Toolbar()->GetBigButtonWidth() };
	AddButtons(&tbBtn, 1);

	// 注意，这里需要显式地调用一下刷新函数，因为在后期这个toolbar就没有机会再改变大小了
	RefreshAfterAddButtonOrSizeChange();

#ifdef _USE_BIGBUTTON_MENU_
	m_BigButtonMenu.CreateBigButtonMenu(m_hWnd);
#endif 

	return 1;
}

void CBigButton::OnTimer(UINT_PTR nIDEvent)
{
	POINT pt;
	::GetCursorPos(&pt);
	HWND hWnd = ::WindowFromPoint(pt);
	if (hWnd)
	{
		if (hWnd == m_hWnd || hWnd == m_BigButtonMenu.m_hWnd)
			return;
	}
	KillTimer(nIDEvent);

#ifdef _USE_BIGBUTTON_MENU_
	m_BigButtonMenu.HideMenu();
#endif
}


LRESULT CBigButton::OnGetDispInfo(LPNMHDR pnmh)
{
	return 0;
}


LRESULT CBigButton::OnClick(int nIndex, POINT pt)
{
	AccessDefaultPage(m_hWnd);
	return 0;
}

LRESULT CBigButton::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;

#ifdef _USE_BIGBUTTON_MENU_
	if (!m_BigButtonMenu.IsWindowVisible())
	{
		m_BigButtonMenu.ShowMenu();
		SetTimer(0, 50);
	}
#endif 

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// override

void CBigButton::DrawBackground(HDC hDC, const RECT &rc)
{
	// 要改成BigButton的背景
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CBigButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->BigButton()->DrawButton(hDC, rc);
}
