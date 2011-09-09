
#include "stdafx.h"
#include "SSLLockButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"

CSSLLockButton::CSSLLockButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pSSLLockButton = this;
}

void CSSLLockButton::CreateLockButton(HWND hParent)
{
	HWND hWnd = Create(hParent, CRect(0, 0, s()->SSLLockButton()->GetWidth(), s()->SSLLockButton()->GetHeight()), NULL, WS_CHILD/* | WS_VISIBLE*/);
	ATLASSERT(hWnd);
}

void CSSLLockButton::DrawBackground(HDC hDC, const RECT &rc)
{
	//rc.bottom += 3;
	RECT rc2 = rc;
	rc2.bottom += 3;

	// 要改成BigButton的背景
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CSSLLockButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->SSLLockButton()->DrawButton(hDC, rc);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CSSLLockButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->SSLLockButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CSSLLockButton::OnClick(int nIndex, POINT pt)
{
	FS()->TabItem()->GetChildFrame()->DoShowSSLStatus();

	return 0;
}
