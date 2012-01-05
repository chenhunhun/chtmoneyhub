#pragma once
#include <tuofunc.h>
#include "TuoImage.h"

#define ICON_WIDTH					16
#define TOOLBAR_MARGIN				4
#define TOOLBAR_ICON_TEXT_SEP		2
#define TOOLBAR_TEXT_DROPDOWN_SEP	2


class CCommonSkin
{

public:

	CCommonSkin(CSkinManager *pSkin);

	void Refresh(bool bReload);

	HCURSOR GetNormalCursor() const		{ return m_hcurNormal; }

	HFONT GetDefaultFont() const	{ return m_font; }
	HFONT GetBoldFont() const		{ return m_fontBold; }
	HFONT GetTitleFont() const		{ return m_fontTitle; }
	HFONT GetAddrBarFont() const		{ return CSkinProperty::GetDWORD(Skin_Property_AddressBar_EditCtrl_Use_Big_Font) ? m_bigFontInEditCtrl : m_fontEditBoxSmall; }
	HFONT GetSearchBarFont() const		{ return CSkinProperty::GetDWORD(Skin_Property_SearchBar_EditCtrl_Use_Big_Font) ? m_bigFontInEditCtrl : m_fontEditBoxSmall; }
	HFONT GetFindBarFont() const		{ return m_fontEditBoxSmall; }
	HFONT GetAddrListFont() const		{ return m_fontAddressBarList; }
	HFONT GetAddrListFontBold() const		{ return m_fontAddressBarListBold; }
	HFONT GetMenuAccFont() const		{ return m_fontAcc; }

	HICON GetDefaultPageIcon() const { return m_icoDefaultPageIcon; }
	HICON GetFolderOpen(bool bOpen) const { return bOpen ? m_icoFolderOpen : m_icoFolderClose; }

	HICON GetMainIcon() const		{ return m_icoMainIcon; }
	HICON GetVideoIcon() const		{ return m_icoVideoIcon; }

	int GetButtonMaskHeight() const		{ return m_bmpButtonBack.GetHeight(); }

	void DrawButtonBack(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus);
	void DrawButtonBack(CDCHandle dc, const RECT &rcBtn, UINT uStatus);

	void DrawIcon(CDCHandle dc, HICON hIcon, int x, int y, bool bAero);
	void DrawMainIcon(CDCHandle dc, int x, int y);

public:

	CIcon m_icoMainIcon, m_icoVideoIcon;
	CTuoIcon m_icoDefaultPageIcon, m_icoHome, m_icoFolderClose, m_icoFolderOpen, m_icoInprivateOverlay;

	CFont m_font, m_fontBold, m_fontTitle, m_bigFontInEditCtrl, m_fontAddressBarList, m_fontAddressBarListBold, m_fontAcc, m_fontEditBoxSmall;

	CTuoImage m_bmpButtonBack;
	CTuoImage m_bmpMainIcon;

	HCURSOR m_hcurNormal;

	CTuoPatchDraw m_patchDraw;
};
