
#include "stdafx.h"
#include "LoadButton.h"
#include "../Skin/SkinManager.h"
//#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
//#include "ChildFrm.h"
//#include "MainFrame.h"
//#include "../../Utils/HardwareID/genhwid.h"
//#include "../../Utils/sn/SNManager.h"
//#include "../../Utils/Config/HostConfig.h"
#include "Version.h"

CLoadButton::CLoadButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pLoadButton = this;
}

void CLoadButton::CreateButton(HWND hParent)
{
	CRect rc(0, 0, s()->LoadButton()->GetWidth(), s()->LoadButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CLoadButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CLoadButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->LoadButton()->DrawButton(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CLoadButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->LoadButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CLoadButton::OnClick(int nIndex, POINT pt)
{
	return 0;
}
