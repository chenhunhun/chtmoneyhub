
#include "stdafx.h"
#include "TipsButton.h"
#include "../Skin/SkinManager.h"
#include "../UIControl/MainFrame.h"

CTipsButton::CTipsButton():m_statenumber(4)
{
	//m_bitmap.m_strFileName = filename;
	//m_statenumber = 4;
}

void CTipsButton::CreateButton(HWND hParent,UINT msg)
{
	CRect rc(0, 0, GetWidth(), GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
	m_msg = msg;
}

void CTipsButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CTipsButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	DrawButton(hDC, rc, uCurrentStatus);
}

void CTipsButton::SetButtonState(bool bEnabled)
{
	SetEnable(0, bEnabled);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CTipsButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, false, 0, GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();
	return 1;
}

LRESULT CTipsButton::OnClick(int nIndex, POINT pt)
{
	::PostMessageW(GetParent(),m_msg,0,0);
	return 0;
}

void CTipsButton::Refresh()
{
	m_bitmap.LoadFromFile();
}

void CTipsButton::DrawButton(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth() / m_statenumber, uYPos + m_bitmap.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap, btnStatus, m_statenumber);
}

void CTipsButton::SetBitmapFileName(wstring fname,int statenumber)
{
	m_bitmap.m_strFileName = fname;
	m_statenumber = statenumber;
}
