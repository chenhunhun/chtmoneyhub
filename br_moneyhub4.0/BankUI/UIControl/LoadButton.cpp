
#include "stdafx.h"
#include "LoadButton.h"
#include "../Skin/SkinManager.h"
#include "ShowJSFrame.h"
#include "../../Utils/SecurityCache/comm.h"

CLoadButton::CLoadButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS), m_hParentWnd(NULL)//, m_pShowDlg(NULL)
{
	pFS->pLoadButton = this;
}

void CLoadButton::CreateButton(HWND hParent)
{
	m_hParentWnd = hParent;
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
	::SendMessage(m_hParentWnd, WM_SHOW_USER_DLG, 0, MY_TAG_LOAD_DLG);

	return 0;
}

void CLoadButton::EndShowDialog(void)
{
	/*if (NULL != m_pShowDlg)
	{
		::PostMessage(m_pShowDlg->m_hWnd, END_SHOW_DIALOG, 0, 0);
	}*/
}
