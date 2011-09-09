#include "stdafx.h"
#include "../Skin/SkinManager.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "BrowserToolbar.h"


CBrowserToolBarCtrl::CBrowserToolBarCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS)
{
	FS()->pToolbar = this;
}


void CBrowserToolBarCtrl::CreateBrowserToolbar(HWND hParent)
{
	int iMaxHeight = 0, iTotalWidth = 0;
	for (int i = 0; i < TOOLBAR_BTN_MAX; i++)
	{
		iMaxHeight = max(iMaxHeight, s()->Toolbar()->GetButtonHeight(i));
		iTotalWidth += s()->Toolbar()->GetButtonWidth(i);
	}

	HWND hWnd = Create(hParent, CRect(s()->Toolbar()->GetBigButtonWidth(), 0, iTotalWidth, iMaxHeight), NULL, WS_CHILD /*| WS_VISIBLE*/);
	ATLASSERT(hWnd && "Create main toolbar failed.");
}


void CBrowserToolBarCtrl::SetButtonState(bool bBackEnabled, bool bForwardEnabled)
{
	SetEnable(ID_TOOLBAR_BACK, bBackEnabled);
	SetEnable(ID_TOOLBAR_FORWARD, bForwardEnabled);
}

void CBrowserToolBarCtrl::Refresh()
{
	SetButtonWidth(ID_TOOLBAR_BACK, s()->Toolbar()->GetButtonWidth(ID_TOOLBAR_BACK));
	SetButtonWidth(ID_TOOLBAR_FORWARD, s()->Toolbar()->GetButtonWidth(ID_TOOLBAR_FORWARD));
	SetButtonWidth(ID_TOOLBAR_REFRESH, s()->Toolbar()->GetButtonWidth(ID_TOOLBAR_REFRESH));

	int iMaxHeight = 0;
	for (int i = 0; i < TOOLBAR_BTN_MAX; i++)
		iMaxHeight = max(iMaxHeight, s()->Toolbar()->GetButtonHeight(i));
	SetHeight(iMaxHeight);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CBrowserToolBarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn[] =
	{
		{ ToolType_Normal, false, ID_TOOLBAR_BACK, s()->Toolbar()->GetButtonWidth(ID_TOOLBAR_BACK) },
		{ ToolType_Normal, false, ID_TOOLBAR_FORWARD, s()->Toolbar()->GetButtonWidth(ID_TOOLBAR_FORWARD) },
		{ ToolType_Normal, true, ID_TOOLBAR_REFRESH, s()->Toolbar()->GetButtonWidth(ID_TOOLBAR_REFRESH) },
	};
	AddButtons(tbBtn, _countof(tbBtn));

	// 注意，这里需要显式地调用一下刷新函数，因为在后期这个toolbar就没有机会再改变大小了
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CBrowserToolBarCtrl::OnGetDispInfo(LPNMHDR pnmh)
{
	return 0;
}


LRESULT CBrowserToolBarCtrl::OnClick(int nIndex, POINT pt)
{
	if (FS()->TabItem() == NULL)
		return 0;
	switch (nIndex)
	{
		case ID_TOOLBAR_BACK:
			FS()->TabItem()->GetChildFrame()->DoNavigateBack();
			break;
		case ID_TOOLBAR_FORWARD:
			FS()->TabItem()->GetChildFrame()->DoNavigateForward();
			break;
		case ID_TOOLBAR_REFRESH:
			FS()->TabItem()->GetChildFrame()->DoNavigateRefresh();
			break;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// override

void CBrowserToolBarCtrl::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CBrowserToolBarCtrl::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->Toolbar()->DrawButton(hDC, rc, iIndex, uCurrentStatus);
}
