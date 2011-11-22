
#include "stdafx.h"
#include "SepButton.h"

#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"

CSepButton::CSepButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS), m_hParentWnd(NULL)
{
	pFS->pSepButton = this;
}

CSepButton::~CSepButton()
{
}

void CSepButton::CreateButton(HWND hParent, LPCTSTR lpName, UINT nBtnClickMessage)
{
	m_bitmap.LoadFromFile(lpName, true);
	m_hParentWnd = hParent;
	m_nMessage = nBtnClickMessage;
	HWND hWnd = Create(hParent, CRect(0, 0, m_bitmap.GetWidth(), m_bitmap.GetHeight()), NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CSepButton::DrawBackground(HDC hDC, const RECT &rc)
{
	// 要改成BigButton的背景
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CSepButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	//s()->SepButton()->DrawLogo(hDC, rc, uCurrentStatus);
	CDCHandle dc(hDC);
	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth(), uYPos + m_bitmap.GetHeight() };
	RECT rcSrc = { 0, 0, m_bitmap.GetWidth(), m_bitmap.GetHeight() };
	m_bitmap.Draw(dc, rcDest, rcSrc);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CSepButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, m_bitmap.GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

int CSepButton::GetButtonWidth() const
{
	return m_bitmap.GetWidth();
}

int CSepButton::GetButtonHeight() const
{
	return m_bitmap.GetHeight() ;
}

LRESULT CSepButton::OnClick(int nIndex, POINT pt)
{
	if (0 != m_nMessage)
		::SendMessage(m_hParentWnd, m_nMessage, 0, 0);

	return 0;
}
