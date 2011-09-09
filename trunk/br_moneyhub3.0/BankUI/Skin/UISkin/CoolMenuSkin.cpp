
#include "stdafx.h"
#include "CoolMenuSkin.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"

CCoolMenuSkin::CCoolMenuSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("menu_back"), m_imgMenuBackground);
}


void CCoolMenuSkin::Refresh()
{
	m_imgMenuBackground.LoadFromFile();
}

void CCoolMenuSkin::DrawBackgroud(CDCHandle dc, const RECT &rc)
{
	CRect rcDraw = rc;
	int nHalfHeight = m_imgMenuBackground.GetHeight() / 2;
	int nHalfWidth = m_imgMenuBackground.GetWidth() / 2;

	rcDraw.bottom = nHalfHeight;
	CSkinManager::DrawExtends(dc, rcDraw, m_imgMenuBackground, 0, m_imgMenuBackground.GetWidth(), 0, nHalfHeight, nHalfWidth);

	rcDraw.top = rcDraw.bottom;
	rcDraw.bottom = rc.bottom - nHalfHeight;;
	CSkinManager::DrawExtends(dc, rcDraw, m_imgMenuBackground, 0, m_imgMenuBackground.GetWidth(), nHalfHeight - 1, nHalfHeight, nHalfWidth);

	rcDraw.top = rc.bottom - nHalfHeight;
	rcDraw.bottom = rc.bottom;
	CSkinManager::DrawExtends(dc, rcDraw, m_imgMenuBackground, 0, m_imgMenuBackground.GetWidth(), nHalfHeight, m_imgMenuBackground.GetHeight(), nHalfWidth);
}