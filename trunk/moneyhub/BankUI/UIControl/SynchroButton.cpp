
#include "stdafx.h"
#include "SynchroButton.h"
#include "../Skin/SkinManager.h"
#include "../Synchro/SynchroDlg.h"


CSynchroButton::CSynchroButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pSynchroButton = this;
}

void CSynchroButton::CreateButton(HWND hParent)
{
	CRect rc(0, 0, s()->SynchroButton()->GetWidth(), s()->SynchroButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CSynchroButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CSynchroButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->SynchroButton()->DrawButton(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CSynchroButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->SynchroButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CSynchroButton::OnClick(int nIndex, POINT pt)
{
	CSynchroDlg dlg;
	dlg.DoModal();
	return 0;
}

