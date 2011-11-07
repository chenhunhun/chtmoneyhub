
#include "stdafx.h"
#include "ForwardButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"

CForwardButton::CForwardButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pForwardButton = this;
}

void CForwardButton::CreateButton(HWND hParent)
{
	CRect rc(0, 0, s()->ForwardButton()->GetWidth(), s()->ForwardButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);

	HBITMAP hBitmap = s()->ForwardButton()->GetMaskBitmap();
	HRGN hRgn = CreateRegionFromBitmap(hBitmap, 0xff0000, NULL);
	::SetWindowRgn(hWnd, hRgn, TRUE);
}

void CForwardButton::DrawBackground(HDC hDC, const RECT &rc)
{
	// 要改成BigButton的背景
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CForwardButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->ForwardButton()->DrawButton(hDC, rc, uCurrentStatus);
}


void CForwardButton::SetButtonState(bool bEnabled)
{
	SetEnable(0, bEnabled);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CForwardButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, false, 0, s()->ForwardButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CForwardButton::OnClick(int nIndex, POINT pt)
{
	FS()->TabItem()->GetChildFrame()->DoNavigateForward();
	return 0;
}
