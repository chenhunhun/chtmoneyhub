
#include "stdafx.h"
#include "LoginButton.h"
#include "../Skin/SkinManager.h"
//#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
//#include "ChildFrm.h"
//#include "MainFrame.h"
//#include "../../Utils/HardwareID/genhwid.h"
//#include "../../Utils/sn/SNManager.h"
#include "../../BankData/BankData.h"
#include "Version.h"

CLoginButton::CLoginButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pLoginButton = this;
}

void CLoginButton::CreateButton(HWND hParent)
{
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
	/*CString strFileName;
	GetModuleFileName(NULL, strFileName.GetBuffer(MAX_PATH), MAX_PATH);
	strFileName.ReleaseBuffer();

	strFileName = strFileName.Left(strFileName.ReverseFind('\\'));
	strFileName += L"\\Html\\LoadAndLogin\\register.html";

	::CreateNewPage_0(m_hWnd, strFileName, FALSE);*/
	
	return 0;
}
