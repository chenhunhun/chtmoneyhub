
#include "stdafx.h"
#include "SepButtonSkin.h"
#include "../SkinManager.h"

CSepButtonSkin::CSepButtonSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("Sep_button"), m_bitmap);
}

void CSepButtonSkin::Refresh()
{
	m_bitmap.LoadFromFile();
	//m_bitmapMask.LoadFromFile();
}

void CSepButtonSkin::DrawLogo(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const
{
	/*ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth(), uYPos + m_bitmap.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap, btnStatus, 1);*/
	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth(), uYPos + m_bitmap.GetHeight() };
	RECT rcSrc = { 0, 0, m_bitmap.GetWidth(), m_bitmap.GetHeight() };
	m_bitmap.Draw(dc, rcDest, rcSrc);
}