
#include "stdafx.h"
#include "TextButton.h"
#include "../Skin/SkinManager.h"

#define TEXT_FORMAT_LEN 27 // 用户名定长
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
		m_TextFont.CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));

		if (m_wstrShow.length() > TEXT_FORMAT_LEN)
		{
			m_wstrShow = m_wstrShow.substr(0, TEXT_FORMAT_LEN - 3);
			m_wstrShow += L"...";
		}
		else if(m_wstrShow.length() < TEXT_FORMAT_LEN)
		{
			int nLen = (TEXT_FORMAT_LEN - m_wstrShow.length()) / 2;

			// 让不足长度的居中显示
			wstring strT;
			// 补充前面的空格
			for(int n = 0; n < nLen; n ++)
			{
				strT += L" ";
			}
			strT += m_wstrShow;
			// 补充后面的空格
			int nLeft = TEXT_FORMAT_LEN - strT.length();
			for(int n = 0; n < nLen; n ++)
			{
				strT += L" ";
			}

			m_wstrShow = strT; 
		}

		m_wstrShow += L"▼";

		double dTextWidth = 6.0 * (TEXT_FORMAT_LEN + 2);

		m_nHeight = 12;
		m_nWidth = (int)dTextWidth;
	}
	else if (nType == TEXT_BTN_USER_LOADINT) // 显示的是中文
	{
		m_TextFont.CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));

		double dTextWidth = 12.0 * (m_wstrShow.length()) - 18; // 18是因为...是英文输入，但按中文输入计算长度的

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