#pragma once

#include "../TuoImage.h"

class CSkinManager;

class CLogoButtonSkin
{
public:
	CLogoButtonSkin(CSkinManager *pSkin);

public:
	void Refresh();

	int GetWidth() const { return m_bitmap.GetWidth(); }
	int GetHeight() const { return m_bitmap.GetHeight(); }
	
//	void DrawButton(CDCHandle dc, const RECT &rect) const;
	void DrawLogo(CDCHandle dc, const RECT &rc, UINT uButtonStatus);

private:
	CTuoImage m_bitmap;
};