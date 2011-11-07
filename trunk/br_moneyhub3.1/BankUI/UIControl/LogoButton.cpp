
#include "stdafx.h"
#include "LogoButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"

CLogoButton::CLogoButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pLogoButton = this;
}

void CLogoButton::CreateLogoButton(HWND hParent)
{
	HWND hWnd = Create(hParent, CRect(0, 0, s()->LogoButton()->GetWidth(), s()->LogoButton()->GetHeight()), NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CLogoButton::DrawBackground(HDC hDC, const RECT &rc)
{
	// 要改成BigButton的背景
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CLogoButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->LogoButton()->DrawLogo(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CLogoButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->LogoButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}
