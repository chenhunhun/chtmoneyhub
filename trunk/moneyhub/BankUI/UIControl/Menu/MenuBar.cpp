#include "stdafx.h"
#include "../../SkinManager/SkinManager.h"
#include "MenuBar.h"


CTuotuoMenuBarCtrl::CTuotuoMenuBarCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_hWndFocus(NULL), m_bDropDownWithFirstSelection(false), m_bVisibleBeforeGetFocus(false), m_bIsAltKeyPressed(false), m_bHasFocus(false)
{
	FS()->pCmdBar = this;
}

void CTuotuoMenuBarCtrl::Refresh()
{
	SetHeight(s()->MenuBar()->GetMenuBarBackHeight());
}

//////////////////////////////////////////////////////////////////////////

void CTuotuoMenuBarCtrl::CreateCommandBarCtrl(HWND hParent)
{
	HWND hWnd = Create(hParent, CRect(0, 0, 1, s()->MenuBar()->GetMenuBarBackHeight()), NULL, WS_CHILD, 0, ID_VIEW_TOOL_MENU);
	ATLASSERT(hWnd && "Failed to create Commandbar");
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CTuotuoMenuBarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// override

void CTuotuoMenuBarCtrl::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Rebar()->DrawRebarBackPart(hDC, rc, m_hWnd, GetLineNumberInRebar(), FS()->ReBar());
}

void CTuotuoMenuBarCtrl::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
/*	TCHAR szString[512];
	CMenuItemInfo mii;
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = szString;
	mii.cch = 512;
	m_hMenu.GetMenuItemInfo(iIndex, TRUE, &mii);

	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uCurrentStatus);
	s()->MenuBar()->DrawMenuBarBack(hDC, szString, rc, btnStatus, sl()->Border() == Border_AdvanceAero && GetLineNumberInRebar() < sl()->RebarAero());
*/
}
