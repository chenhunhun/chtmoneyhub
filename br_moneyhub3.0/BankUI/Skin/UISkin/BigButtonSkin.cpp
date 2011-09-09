
#include "stdafx.h"
#include "BigButtonSkin.h"
#include "../SkinManager.h"

CBigButtonSkin::CBigButtonSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("bigbutton"), m_bitmap);
}

void CBigButtonSkin::Refresh()
{
	m_bitmap.LoadFromFile();
}

void CBigButtonSkin::DrawButton(CDCHandle dc, const RECT &rect) const
{
	UINT uYPos = (rect.bottom + rect.top - m_bitmap.GetHeight()) / 2;
	RECT rcDest = { rect.left, uYPos, rect.right, uYPos + GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap, 0, 1);
}
