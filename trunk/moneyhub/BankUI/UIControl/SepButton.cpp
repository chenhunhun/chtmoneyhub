
#include "stdafx.h"
#include "SepButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"

CSepButton::CSepButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pSepButton = this;
}

void CSepButton::CreateLogoButton(HWND hParent)
{
	HWND hWnd = Create(hParent, CRect(0, 0, s()->SepButton()->GetWidth(), s()->SepButton()->GetHeight()), NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CSepButton::DrawBackground(HDC hDC, const RECT &rc)
{
	// 要改成BigButton的背景
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CSepButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->SepButton()->DrawLogo(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CSepButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->SepButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}
