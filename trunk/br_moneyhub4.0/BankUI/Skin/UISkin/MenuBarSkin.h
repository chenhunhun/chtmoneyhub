#pragma once
#include "TuoImage.h"

class CSkinManager;

class CMenuBarSkin
{

public:

	CMenuBarSkin(CSkinManager *pSkin);

	void Refresh();

	int GetMenuBarBackHeight() const		{ return m_menuButtonBack.GetHeight(); }

	void DrawMenuBarBack(CDCHandle dc, LPCTSTR lpszText, const RECT &rcBtn, ButtonStatus btnStatus, bool bUnderAero);

private:

	CTuoImage m_menuButtonBack;

};
