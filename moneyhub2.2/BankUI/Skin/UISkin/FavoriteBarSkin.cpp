#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"


#define TOOLBAR_MAX_WIDTH			80
#define	FAVORBAR_ICON_FRAME			2


CFavoriteBarSkin::CFavoriteBarSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("favorbar_symbol_dropdown"), m_bmpToolbarDropdown);
	pSkin->RegisterFileName(_T("favorbar_icon"), m_bmpFavorBarIcon);
}


void CFavoriteBarSkin::Refresh()
{
	m_bmpToolbarDropdown.LoadFromFile();
	m_bmpFavorBarIcon.LoadFromFile();
}

//////////////////////////////////////////////////////////////////////////

int CFavoriteBarSkin::GetButtonWidth(LPCTSTR lpszText, bool bDropDown)
{
	CClientDC dc(::GetDesktopWindow());
	SIZE sz;
	dc.SelectFont(s()->Common()->GetDefaultFont());
	::GetTextExtentPoint32(dc, lpszText, _tcslen(lpszText), &sz);
	return min(sz.cx, TOOLBAR_MAX_WIDTH) + TOOLBAR_MARGIN * 2 + TOOLBAR_ICON_TEXT_SEP + ICON_WIDTH + (bDropDown ? TOOLBAR_TEXT_DROPDOWN_SEP + m_bmpToolbarDropdown.GetWidth() : 0);
}

int CFavoriteBarSkin::GetSingleIconWidth() const
{
	return m_bmpFavorBarIcon.GetWidth() / FAVORBAR_ICON_FRAME;
}

//////////////////////////////////////////////////////////////////////////

void CFavoriteBarSkin::DrawButtonBack(CDCHandle dc, const RECT &rcBtn, HICON hIcon, UINT uStatus, LPCTSTR szText, bool bDropDown, bool bAero)
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uStatus);
	s()->Common()->DrawButtonBack(dc, rcBtn, btnStatus);

	// draw the icon
	int iSep = (rcBtn.bottom + rcBtn.top - ICON_WIDTH) / 2;
	if (bDropDown)
		hIcon = s()->Common()->GetFolderOpen(false);
	s()->Common()->DrawIcon(dc, hIcon, TOOLBAR_MARGIN + rcBtn.left, iSep + ((btnStatus == Btn_MouseDown) ? 1 : 0), bAero);

	// ||<- margin ->|<= icon =>|<- icon_text_sep ->|<= text =>|<- margin ->||
	// ||<- margin ->|<= icon =>|<- icon_text_sep ->|<= text =>|<- dropdown margin->|<= drop_down =>|<- margin ->||
	RECT rcText = {
		TOOLBAR_MARGIN + ICON_WIDTH + TOOLBAR_ICON_TEXT_SEP + rcBtn.left,
		1 + (btnStatus == Btn_MouseDown ? 1 : 0),
		rcBtn.right - TOOLBAR_MARGIN - (bDropDown ? TOOLBAR_TEXT_DROPDOWN_SEP + m_bmpToolbarDropdown.GetWidth() : 0),
		1 + rcBtn.bottom + (btnStatus == Btn_MouseDown ? 1 : 0)
	};
	dc.SelectFont(s()->Common()->GetDefaultFont());

	if (bAero)
	{
		dc.SetTextColor(CSkinProperty::GetColor(Skin_Property_Toolbar_Text_Aero_Color));
		bool bIsGlow = (CSkinProperty::GetDWORD(Skin_Property_Toolbar_Text_Aero_Glow) >> (btnStatus < 3 ? btnStatus : 0)) & 1;
		CSkinManager::DrawGlowText(dc, szText, -1, &rcText, DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_VCENTER | DT_NOPREFIX, bIsGlow);
	}
	else
	{
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(CSkinProperty::GetColor(Skin_Property_Toolbar_Text_Color));
		dc.DrawText(szText, -1, &rcText, DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_VCENTER | DT_NOPREFIX);
	}
	if (bDropDown)
	{
		int iY = (rcBtn.bottom + rcBtn.top - m_bmpToolbarDropdown.GetHeight()) / 2 + (btnStatus == Btn_MouseDown ? 2 : 1);
		RECT rcDest = { rcBtn.right - TOOLBAR_MARGIN - m_bmpToolbarDropdown.GetWidth(),
			iY, rcBtn.right - TOOLBAR_MARGIN, iY + m_bmpToolbarDropdown.GetHeight() };
		RECT rcSrc = { 0, 0, m_bmpToolbarDropdown.GetWidth(), m_bmpToolbarDropdown.GetHeight() };
		m_bmpToolbarDropdown.Draw(dc, rcDest, rcSrc);
	}
}


void CFavoriteBarSkin::DrawHeaderIcon(CDCHandle dc, const RECT &rc, int iIndex, UINT uButtonStatus)
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	s()->Common()->DrawButtonBack(dc, rc, btnStatus);

	int iX = (rc.right + rc.left - GetSingleIconWidth()) / 2;
	int iY = (rc.bottom + rc.top - m_bmpFavorBarIcon.GetHeight()) / 2 + (btnStatus == Btn_MouseDown ? 1 : 0);
	RECT rcIcon = { iX, iY, iX + GetSingleIconWidth(), iY + m_bmpFavorBarIcon.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcIcon, m_bmpFavorBarIcon, iIndex, FAVORBAR_ICON_FRAME);
}
