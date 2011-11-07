#pragma once

#include "../TuoImage.h"

class CSkinManager;

class CBigButtonSkin
{
public:
	CBigButtonSkin(CSkinManager *pSkin);

public:
	void Refresh();

	int GetWidth() const { return 50; }
	int GetHeight() const { return 50; }
	
	void DrawButton(CDCHandle dc, const RECT &rect) const;

private:
	CTuoImage m_bitmap;
};