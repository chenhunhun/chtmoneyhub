#pragma once

#include "../TuoImage.h"

class CSkinManager;

class CMenuButtonSkin
{
public:
	CMenuButtonSkin(CSkinManager *pSkin);

public:
	void Refresh();

	int GetWidth() const { return m_bitmap.GetWidth() / 3; }
	int GetHeight() const { return m_bitmap.GetHeight(); }
	
//	void DrawButton(CDCHandle dc, const RECT &rect) const;
	void DrawButton(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const;

private:
	CTuoImage m_bitmap;
};