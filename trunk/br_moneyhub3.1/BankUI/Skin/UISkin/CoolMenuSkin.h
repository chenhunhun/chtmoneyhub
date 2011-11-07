
#pragma once

#include "../TuoImage.h"

class CSkinManager;

class CCoolMenuSkin
{
public:
	CCoolMenuSkin(CSkinManager *pSkin);

	void Refresh();

	int GetWidth() const	{ return m_imgMenuBackground.GetWidth() / 3; }
	int GetHeight() const	{ return m_imgMenuBackground.GetHeight(); }

	void DrawBackgroud(CDCHandle dc, const RECT &rc);

public:
	CTuoImage m_imgMenuBackground;
};
