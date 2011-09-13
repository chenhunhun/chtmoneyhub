
#include "stdafx.h"
#include "ForwardButtonSkin.h"
#include "../SkinManager.h"

CForwardButtonSkin::CForwardButtonSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("forward_button"), m_bitmap);
	pSkin->RegisterFileName(_T("forward_button_mask"), m_bitmapMask);
}

void CForwardButtonSkin::Refresh()
{
	m_bitmap.LoadFromFile();
	m_bitmapMask.LoadFromFile();
}

void CForwardButtonSkin::DrawButton(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth() / 4, uYPos + m_bitmap.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap, btnStatus, 4);
}