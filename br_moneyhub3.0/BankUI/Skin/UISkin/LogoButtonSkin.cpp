
#include "stdafx.h"
#include "LogoButtonSkin.h"
#include "../SkinManager.h"

CLogoButtonSkin::CLogoButtonSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("logo"), m_bitmap);
}

void CLogoButtonSkin::Refresh()
{
	m_bitmap.LoadFromFile();
}

void CLogoButtonSkin::DrawLogo(CDCHandle dc, const RECT &rc, UINT uButtonStatus)
{
	//GetWebData()->GetLogoPng().ResetCacheDC();
	//m_bitmap.ResetCacheDC();

	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth(), uYPos + m_bitmap.GetHeight() };
	RECT rcSrc = { 0, 0, m_bitmap.GetWidth(), m_bitmap.GetHeight() };
	m_bitmap.Draw(dc, rcDest, rcSrc);
}