
#include "stdafx.h"
#include "LoginButton.h"
#include "../Skin/SkinManager.h"
//#include "ShowJSFrame.h"

CLoginButton::CLoginButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS), m_hParentWnd(NULL)//, m_pShowDlg(NULL)
{
	pFS->pLoginButton = this;
}

void CLoginButton::CreateButton(HWND hParent)
{
	m_hParentWnd = hParent;
	CRect rc(0, 0, s()->LoginButton()->GetWidth(), s()->LoginButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CLoginButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CLoginButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->LoginButton()->DrawButton(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CLoginButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->LoginButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CLoginButton::OnClick(int nIndex, POINT pt)
{
	::SendMessage(m_hParentWnd, WM_SHOW_USER_DLG, 0, MY_TAG_REGISTER_DLG);

	return 0;
}

void CLoginButton::EndShowDialog(void)
{
	/*if (NULL != m_pShowDlg)
	{
		::PostMessage(m_pShowDlg->m_hWnd, END_SHOW_DIALOG, 0, 0);
	}*/
}
