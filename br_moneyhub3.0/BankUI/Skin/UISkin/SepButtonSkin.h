#pragma once

#include "../TuoImage.h"

class CSkinManager;

class CSepButtonSkin
{
public:
	CSepButtonSkin(CSkinManager *pSkin);

public:
	void Refresh();

	int GetWidth() const { return m_bitmap.GetWidth(); }
	int GetHeight() const { return m_bitmap.GetHeight(); }

	//	void DrawButton(CDCHandle dc, const RECT &rect) const;
	void DrawLogo(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const;

	HBITMAP GetMaskBitmap() const { return m_bitmapMask; }

private:
	CTuoImage m_bitmap;
	CTuoImage m_bitmapMask;
};