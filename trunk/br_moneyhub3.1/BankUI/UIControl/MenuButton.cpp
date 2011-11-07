
#include "stdafx.h"
#include "MenuButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"

UINT WM_SHOWHELPMENU = RegisterWindowMessage(_T("MenuButton_MSG"));

CMenuButton::CMenuButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pMenuButton = this;
}

void CMenuButton::CreateButton(HWND hParent)
{
	HWND hWnd = Create(hParent, CRect(0, 0, s()->MenuButton()->GetWidth(), s()->MenuButton()->GetHeight()), NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CMenuButton::DrawBackground(HDC hDC, const RECT &rc)
{
	// 要改成BigButton的背景
	//s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CMenuButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->MenuButton()->DrawButton(hDC, rc, uCurrentStatus);


}

//////////////////////////////////////////////////////////////////////////
// message handler

int CMenuButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->MenuButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CMenuButton::OnClick(int nIndex, POINT pt)
{
//	FS()->TabItem()->GetChildFrame()->DoShowSSLStatus();

	FS()->MainFrame()->PostMessage(WM_SHOWHELPMENU);

	return 0;
}
