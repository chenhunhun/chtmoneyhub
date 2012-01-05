
#include "stdafx.h"
#include "BackButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"

CBackButton::CBackButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pBackButton = this;
}

void CBackButton::CreateButton(HWND hParent)
{
	CRect rc(0, 0, s()->BackButton()->GetWidth(), s()->BackButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);

	HBITMAP hBitmap = s()->BackButton()->GetMaskBitmap();
	HRGN hRgn = CreateRegionFromBitmap(hBitmap, 0xff0000, NULL);
	::SetWindowRgn(hWnd, hRgn, TRUE);
}

void CBackButton::DrawBackground(HDC hDC, const RECT &rc)
{
	// 要改成BigButton的背景
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CBackButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->BackButton()->DrawButton(hDC, rc, uCurrentStatus);
}

void CBackButton::SetButtonState(bool bEnabled)
{
	SetEnable(0, bEnabled);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CBackButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, false, 0, s()->BackButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CBackButton::OnClick(int nIndex, POINT pt)
{
	FS()->TabItem()->GetChildFrame()->DoNavigateBack();
	return 0;
}
