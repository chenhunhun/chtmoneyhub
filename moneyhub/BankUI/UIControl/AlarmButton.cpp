
#include "stdafx.h"
#include "AlarmButton.h"
#include "../Skin/SkinManager.h"

CAlarmButton::CAlarmButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pAlarmButton = this;
}

void CAlarmButton::CreateButton(HWND hParent)
{
	CRect rc(0, 0, s()->AlarmButton()->GetWidth(), s()->AlarmButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CAlarmButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CAlarmButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->AlarmButton()->DrawButton(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CAlarmButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->AlarmButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CAlarmButton::OnClick(int nIndex, POINT pt)
{
	return 0;
}

