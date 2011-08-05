
#include "stdafx.h"
#include "SSLLockButtonSkin.h"
#include "../SkinManager.h"

CSSLLockButtonSkin::CSSLLockButtonSkin(CSkinManager *pSkin)
{
	// small ssl icon
	pSkin->RegisterFileName(_T("ssl_lock"), m_bitmap);
}

void CSSLLockButtonSkin::Refresh()
{
	m_bitmap.LoadFromFile();
}

void CSSLLockButtonSkin::DrawButton(CDCHandle dc, const RECT &rect) const
{
	UINT uYPos = (rect.bottom + rect.top - m_bitmap.GetHeight()) / 2;
	RECT rcDest = { rect.left, uYPos, rect.right, uYPos + GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap, 0, 1);
}
