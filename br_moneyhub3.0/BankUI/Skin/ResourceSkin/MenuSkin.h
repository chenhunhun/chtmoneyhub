#pragma once
#include "../TuoImage.h"

#define MENU_MARGIN_DELTA			2

#define MENU_SEP_HEIGHT				9


class CMenuSkin
{

public:

	CMenuSkin(CSkinManager *pSkin);

	void Refresh();

	int GetMenuHeight()				{ return m_imgMenuBack.GetHeight(); }
	int GetMenuMarginDelta() const	{ return MENU_MARGIN_DELTA; }
	static int GetMenuPadding()		{ return 2; }


	// 画边框背景（掏空的）, WindowDC, (0,0)为菜单左上角
	void DrawMenuBorder(CDCHandle dc, HDC dcBack, const RECT &rect, bool bJustDrawBorder, bool bEraseBg);
	// 画里边儿背景, ClientDc, (0,0)坐标距离菜单左上角有一定距离
	void DrawMenuCoreBackgroundWithClientDC(CDCHandle dc, HDC dcBack, const RECT &rcClient);
	// 画两列菜单中间的分割线
	void DrawColumnSepLine(CDCHandle dc, const RECT &rcClient, int iLineX);

	void DrawMenuItemSelectionBack(CDCHandle dc, const RECT &rect);
	void DrawMenuItemText(CDCHandle dc, UINT uMenuRes, WORD wMenuID, const CString &lpszMenuString, const RECT &rect, CBitmapHandle hBitmap, CBitmapHandle hBitmapMask, bool bEnabled, bool bBold);
	void DrawMenuSep(CDCHandle dc, const RECT &rect);

	void DrawMenuNonClient(CDCHandle dc, const RECT &rect);

private:

	CTuoImage m_imgMenuBack;
	CTuoImage m_imgMenuIcon;
	CTuoImage m_imgMenuSep;
};
