#pragma once

#include "../TuoImage.h"

class CSkinManager;

class CForwardButtonSkin
{
public:
	CForwardButtonSkin(CSkinManager *pSkin);

public:
	void Refresh();

	int GetWidth() const { return m_bitmap.GetWidth() / 4; }
	int GetHeight() const { return m_bitmap.GetHeight(); }
	
//	void DrawButton(CDCHandle dc, const RECT &rect) const;
	void DrawButton(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const;

	HBITMAP GetMaskBitmap() const { return m_bitmapMask; }

private:
	CTuoImage m_bitmap;
	CTuoImage m_bitmapMask;
};