
#include "stdafx.h"
#include "TextButton.h"
#include "../Skin/SkinManager.h"

CTextButton::CTextButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS), m_hParnetWnd(NULL)
{
	pFS->pTextButton = this;
}

CTextButton::~CTextButton()
{
	::DeleteObject(m_TextFont);
	::DestroyWindow(m_hWnd);
}

void CTextButton::CreateButton(HWND hParent, LPCTSTR lpShow, int nType)
{

	ATLASSERT(NULL != hParent && NULL != lpShow);
	m_hParnetWnd = hParent;
	m_wstrShow = lpShow;
	m_nType = nType;
	if (m_nType == TEXT_BTN_USER_NAME)
	{
		m_TextFont.CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("ËÎÌå"));

		m_wstrShow += L" ¨‹";

		double dTextWidth = 5.7 * (m_wstrShow.length() + 2);

		m_nHeight = 12;
		m_nWidth = (int)dTextWidth;
	}
	else if (nType == TEXT_BTN_USER_LOADINT)
	{
		m_TextFont.CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("ËÎÌå"));

		double dTextWidth = 6.0 * (m_wstrShow.length() + 1);

		m_nHeight = 12;
		m_nWidth = (int)dTextWidth;
	}
	CRect rc(0, 0, m_nWidth, m_nHeight);

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CTextButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CTextButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	//s()->TextButton()->DrawButton(hDC, rc, uCurrentStatus);
	CDCHandle dc(hDC);
	HGDIOBJ hObj = ::SelectObject(dc, m_TextFont);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(120, 133, 142));
	dc.TextOut(0, 0, m_wstrShow.c_str());
	::SelectObject(dc, hObj);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CTextButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, m_nWidth };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CTextButton::OnClick(int nIndex, POINT pt)
{
	if (m_nType == TEXT_BTN_USER_NAME)
	{
		::PostMessage(m_hParnetWnd, WM_SHOW_USER_CLICKED_MENU, 0, 0);
	}

	return 0;
}

int CTextButton::GetTextButtonHeight()
{
	return m_nHeight;
}

int CTextButton::GetTextButtonWidth()
{
	return m_nWidth;
}

void CTextButton::ReadButtonText(std::wstring& strBtn)
{
	strBtn = m_wstrShow;
}