
#include "stdafx.h"
#include "BackButtonSkin.h"
#include "../SkinManager.h"

CBackButtonSkin::CBackButtonSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("back_button"), m_bitmap);
	pSkin->RegisterFileName(_T("back_button_mask"), m_bitmapMask);
}

void CBackButtonSkin::Refresh()
{
	m_bitmap.LoadFromFile();
	m_bitmapMask.LoadFromFile();
}

void CBackButtonSkin::DrawButton(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth() / 4, uYPos + m_bitmap.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap, btnStatus, 4);
}