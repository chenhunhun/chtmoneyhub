
#include "stdafx.h"
#include "RefreshButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"

CRefreshButton::CRefreshButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pRefreshButton = this;
}

void CRefreshButton::CreateButton(HWND hParent)
{
	CRect rc(0, 0, s()->RefreshButton()->GetWidth(), s()->RefreshButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);

// 	HBITMAP hBitmap = s()->RefreshButton()->GetMaskBitmap();
// 	HRGN hRgn = CreateRegionFromBitmap(hBitmap, 0xff0000, NULL);
// 	::SetWindowRgn(hWnd, hRgn, TRUE);
}

void CRefreshButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CRefreshButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->RefreshButton()->DrawButton(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CRefreshButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->RefreshButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CRefreshButton::OnClick(int nIndex, POINT pt)
{
	FS()->TabItem()->GetChildFrame()->DoNavigateRefresh();
	return 0;
}
