#pragma once
#include "TuoImage.h"

class CSkinManager;

class CFavoriteBarSkin
{

public:

	CFavoriteBarSkin(CSkinManager *pSkin);

	void Refresh();

	int GetButtonWidth(LPCTSTR lpszText, bool bDropDown);
	int GetSingleIconWidth() const;

	void DrawButtonBack(CDCHandle dc, const RECT &rcBtn, HICON hIcon, UINT uStatus, LPCTSTR szText, bool bDropDown, bool bAero);
	void DrawHeaderIcon(CDCHandle dc, const RECT &rcBtn, int iIndex, UINT uStatus);

private:

	CTuoImage m_bmpToolbarDropdown;
	CTuoImage m_bmpFavorBarIcon;
};
