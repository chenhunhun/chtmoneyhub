
#include "stdafx.h"
#include "BigButtonSkin.h"
#include "../SkinManager.h"

CMenuButtonSkin::CMenuButtonSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("menubtn"), m_bitmap);
}

void CMenuButtonSkin::Refresh()
{
	m_bitmap.LoadFromFile();
}

void CMenuButtonSkin::DrawButton(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth() / 3, uYPos + m_bitmap.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap, btnStatus, 3);
}

