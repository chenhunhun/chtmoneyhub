#pragma once

#include "../TuoImage.h"

class CSkinManager;

class CSSLLockButtonSkin
{
public:
	CSSLLockButtonSkin(CSkinManager *pSkin);

public:
	void Refresh();

	int GetWidth() const { return 30; }
	int GetHeight() const { return 30; }

	void DrawButton(CDCHandle dc, const RECT &rect) const;

private:
	CTuoImage m_bitmap;
};