#include "stdafx.h"
#include "../SkinManager.h"
#include "../SkinLoader.h"

#define CNT_DEL_ICON				3
#define DROPDOWN_LIST_MARGIN		5
#define ICON_TEXT_SEP				4

#define ADDRBAR_EDIT_TIP_TEXT		0xcccccc		// 地址栏里面，提示“输入文字搜索”几个字的颜色
#define SEARCHBAR_TIP_TEXT			0xcccccc


CComboSkin::CComboSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("addrbar_edit_bg"), m_bmpComboBackground[0]);
	pSkin->RegisterFileName(_T("searchbar_edit_bg"), m_bmpComboBackground[1]);
	pSkin->RegisterFileName(_T("findbar_edit_bg"), m_bmpComboBackground[2]);

	pSkin->RegisterFileName(_T("bigbtn_bg"), m_bmpDropdownButtonBack);

	pSkin->RegisterFileName(_T("addrbar_dropdown_icon_search"), m_imgDropdownIcon);
	pSkin->RegisterFileName(_T("addrbar_dropdown_btn_close"), m_imgDropdownCloseIcon);
	pSkin->RegisterFileName(_T("searchbar_symbol_dropdown"), m_bmpSearchBarDropdown);

	pSkin->RegisterFileName(_T("default_search"), m_icoDefaultSearch);
}


void CComboSkin::Refresh()
{
	m_bmpComboBackground[0].LoadFromFile();
	m_bmpComboBackground[1].LoadFromFile();
	m_bmpComboBackground[2].LoadFromFile();
	m_bmpDropdownButtonBack.LoadFromFile();
	m_imgDropdownIcon.LoadFromFile();
	m_imgDropdownCloseIcon.LoadFromFile();
	m_bmpSearchBarDropdown.LoadFromFile();

	m_icoDefaultSearch.LoadFromFile();

	int iEditHeight[2][2] = { /*小字体*/ { /*xp*/ 14, /* vista/win7 */ 17 }, /*大字体*/ { 18, 21 } };
	m_iEditControlHeight[0] = iEditHeight[CSkinProperty::GetDWORD(Skin_Property_AddressBar_EditCtrl_Use_Big_Font) ? 1 : 0][CSkinLoader::IsVista() ? 1 : 0];
	m_iEditControlHeight[1] = iEditHeight[CSkinProperty::GetDWORD(Skin_Property_SearchBar_EditCtrl_Use_Big_Font) ? 1 : 0][CSkinLoader::IsVista() ? 1 : 0];
	m_iEditControlHeight[2] = iEditHeight[0][CSkinLoader::IsVista() ? 1 : 0];

	m_iEditControlTopOffset[0] = CSkinLoader::IsVista() ? 1 : 0;
	m_iEditControlTopOffset[1] = CSkinLoader::IsVista() ? 1 : 0;
	m_iEditControlTopOffset[2] = CSkinLoader::IsVista() ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////

int CComboSkin::GetWidthOfAddrBarText(CDCHandle dc, CString &lpszText)
{
	dc.SelectFont(s()->Common()->GetAddrBarFont());
	SIZE sz = { 0, 0 };
	::GetTextExtentPoint32(dc, lpszText, lpszText.GetLength(), &sz);
	return sz.cx;
}


BOOL CComboSkin::GetPointerOnDeleteButton(const RECT& rcItem, POINT& pt, int iBig)
{
	RECT rcDelButton = {0};

	GetDeleteButtonRect(rcItem, iBig, rcDelButton);

	return PtInRect(&rcDelButton, pt);
}

void CComboSkin::GetDeleteButtonRect(const RECT& rcItem, int iBig, RECT& rcDelButton)
{	
	int iY = (m_bmpDropdownButtonBack.GetHeight() - m_imgDropdownCloseIcon.GetHeight()) / 2 + 1;
	rcDelButton.left   = rcItem.right - 4 - m_imgDropdownCloseIcon.GetWidth() / CNT_DEL_ICON;
	rcDelButton.top    = rcItem.top + iY;
	rcDelButton.right  = rcItem.right - 4;
	rcDelButton.bottom = rcItem.top + iY + m_imgDropdownCloseIcon.GetHeight();
}

//////////////////////////////////////////////////////////////////////////

void CComboSkin::DrawComboEditBackground(CDCHandle dc, const RECT &rcCombo, ButtonStatus eStatus, int iContainerType)
{
	CTuoImage &img = m_bmpComboBackground[iContainerType];
	static int iFrame[] = { 3, 2, 2 };
	int iSize = img.GetWidth() / iFrame[iContainerType];
	int iBegin = iSize * eStatus;
	RECT rcComboBack = { rcCombo.left, rcCombo.top, rcCombo.right, rcCombo.top + img.GetHeight() };
//	CSkinManager::DrawExtends(dc, rcComboBack, img, iBegin, iBegin + iSize);
	CSkinManager::DrawOverlayExtendsPart(dc, rcComboBack, img, iSize / 2 - 7, iSize / 2 - 7, iBegin, iBegin + iSize);
}

void CComboSkin::DrawComboEditBackgroundPart(CDCHandle dc, const RECT &rcEdit, int iYOffset, ButtonStatus eStatus, int iContainerType, bool bUseBlit)
{
	CTuoImage &img = m_bmpComboBackground[iContainerType];
	static int iFrame[] = { 3, 2, 2 };
	int iSize = img.GetWidth() / iFrame[iContainerType];
	int iBegin = iSize * eStatus;
	RECT rcSrc = { iBegin + iSize / 2, iYOffset, iBegin + iSize / 2 + 1, iYOffset + rcEdit.bottom - rcEdit.top };
	if (bUseBlit)
		img.BitBlt(dc, rcEdit, rcSrc);
	else
		img.Draw(dc, rcEdit, rcSrc);
}

void CComboSkin::DrawComboEditTipText(CDCHandle dc, const RECT &rcComboEdit, LPCTSTR lpszText, bool bAero)
{
	dc.SelectFont(s()->Common()->GetDefaultFont());
	dc.SetTextColor(ADDRBAR_EDIT_TIP_TEXT);
	if (bAero)
		CSkinManager::DrawGlowText(dc, lpszText, -1, (LPRECT)&rcComboEdit, DT_RIGHT | DT_VCENTER | DT_SINGLELINE, false);
	else
	{
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(lpszText, -1, (LPRECT)&rcComboEdit, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
}

void CComboSkin::DrawComboDropDownListBackground(CDCHandle dc, const RECT &rcDropDown)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, CSkinProperty::GetDWORD(Skin_Property_AddressBar_Dropdown_Border_Color));
	dc.SelectPen(pen);
	dc.Rectangle(&rcDropDown);
}

void CComboSkin::DrawComboDropDownListItem(CDCHandle dc, const RECT &rc, ButtonStatus eStatus, LPCTSTR lpszStr)
{
	const int iWidthIconType = 1;
	RECT rcItem = rc;
	dc.FillSolidRect(&rcItem, 0xffffff);
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, CSkinProperty::GetColor(Skin_Property_AddressBar_Dropdown_Seperator_Color));
	dc.SelectPen(pen);
	dc.MoveTo(rcItem.left, rcItem.bottom - 1);
	dc.LineTo(rcItem.right, rcItem.bottom - 1);
	if (eStatus != Btn_MouseOut)
	{
		RECT rcBtn = { rcItem.left + 1, rcItem.top + 1, rcItem.right - 1, rcItem.top + 1 + m_bmpDropdownButtonBack.GetHeight() };
		CSkinManager::DrawExtends(dc, rcBtn, m_bmpDropdownButtonBack, 0, m_bmpDropdownButtonBack.GetWidth());
	}

	dc.SetBkMode(TRANSPARENT);

	RECT rcTopText = { rcItem.left + DROPDOWN_LIST_MARGIN + ICON_WIDTH + ICON_TEXT_SEP,
		rcItem.top, rcItem.right - DROPDOWN_LIST_MARGIN, rcItem.top + 26 };
	dc.SetTextColor(CSkinProperty::GetColor(Skin_Property_AddressBar_Dropdown_Title_Color));
	dc.SelectFont(s()->Common()->GetAddrListFont());
	dc.DrawText(lpszStr, -1, &rcTopText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void CComboSkin::DrawSearchBarDropDownArrow(CDCHandle dc, int x, int y)
{
	RECT rcSrc = { 0, 0, m_bmpSearchBarDropdown.GetWidth(), m_bmpSearchBarDropdown.GetHeight() };
	RECT rcDest = { x + ICON_WIDTH, y + ICON_WIDTH - m_bmpSearchBarDropdown.GetHeight(), x + ICON_WIDTH + m_bmpSearchBarDropdown.GetWidth(), y + ICON_WIDTH };
	m_bmpSearchBarDropdown.Draw(dc, rcDest, rcSrc);
}

void CComboSkin::DrawSearchTipText(CDCHandle dc, const RECT &rcSearch, LPCTSTR lpszText, bool bAero)
{
	dc.SelectFont(s()->Common()->GetDefaultFont());
	dc.SetTextColor(SEARCHBAR_TIP_TEXT);
	if (bAero)
		CSkinManager::DrawGlowText(dc, lpszText, -1, (LPRECT)&rcSearch, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX, false);
	else
	{
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(lpszText, -1, (LPRECT)&rcSearch, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


void CComboSkin::MaskSearchString(const std::wstring &strKey, std::wstring &strMask)
{
	LPWSTR lpFind = ::StrStrIW(strMask.c_str(), strKey.c_str());
	while (lpFind)
	{
		for (size_t i = 0; i < strKey.length(); i++)
			lpFind[i] = '\t';

		lpFind = ::StrStrIW(lpFind + strKey.length(), strKey.c_str());
	}
}

std::wstring CComboSkin::GenerateMask(const std::wstring &strSrc, const std::wstring &strSearchStr)
{
	std::wstring strMask = strSrc;
	int iWordStart = -1;
	for (size_t i = 0; i < strSearchStr.length(); i++)
	{
		if (iWordStart < 0)
		{
			if (strSearchStr[i] == ' ')
				continue;
			else
				iWordStart = i;
		}
		else
		{
			if (strSearchStr[i] == ' ')
			{
				MaskSearchString(strSearchStr.substr(iWordStart, i - iWordStart), strMask);
				iWordStart = -1;
			}
			else
				continue;
		}
	}
	if (iWordStart >= 0)
		MaskSearchString(strSearchStr.substr(iWordStart), strMask);
	return strMask;
}


void CComboSkin::DrawTextWithBold(HDC hDC, const std::wstring &strSrc, const std::wstring &strMask, const RECT &rc, UINT uFlag)
{
	RECT rcDraw = { rc.left, rc.top, rc.right, rc.bottom };
	int iCurrentX = rc.left;

	CDCHandle dc(hDC);

	int iBoldStart = 0, iBoldEnd = 0;
	bool bQuitFlag = false;
	for (;;)
	{
		iBoldStart = strMask.find_first_of('\t', iBoldEnd);
		if (iBoldStart == -1)
		{
			iBoldStart = strMask.length();
			bQuitFlag = true;
		}
		dc.SelectFont(s()->Common()->GetAddrListFont());
		rcDraw.left = iCurrentX;
		rcDraw.right = 0;
		dc.DrawText(strSrc.c_str() + iBoldEnd, iBoldStart - iBoldEnd, &rcDraw, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
		if (rcDraw.right > rc.right)
		{
			rcDraw.right = rc.right;
			rcDraw.top = rc.top;
			rcDraw.bottom = rc.bottom;
			dc.DrawText(strSrc.c_str() + iBoldEnd, iBoldStart - iBoldEnd, &rcDraw, DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_NOCLIP | DT_NOPREFIX | uFlag);
			break;
		}
		if (iCurrentX < rcDraw.right)
			iCurrentX = rcDraw.right;
		rcDraw.top = rc.top;
		rcDraw.bottom = rc.bottom;
		dc.DrawText(strSrc.c_str() + iBoldEnd, iBoldStart - iBoldEnd, &rcDraw, DT_SINGLELINE | DT_NOPREFIX | uFlag);
		if (bQuitFlag)
			break;

		iBoldEnd = strMask.find_first_not_of('\t', iBoldStart);
		if (iBoldEnd == -1)
		{
			iBoldEnd = strMask.length();
			bQuitFlag = true;
		}
		dc.SelectFont(s()->Common()->GetAddrListFontBold());
		rcDraw.left = iCurrentX;
		rcDraw.right = 0;
		dc.DrawText(strSrc.c_str() + iBoldStart, iBoldEnd - iBoldStart, &rcDraw, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
		if (rcDraw.right > rc.right)
		{
			rcDraw.right = rc.right;
			rcDraw.top = rc.top;
			rcDraw.bottom = rc.bottom;
			dc.DrawText(strSrc.c_str() + iBoldStart, iBoldEnd - iBoldStart, &rcDraw, DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_NOCLIP | DT_NOPREFIX | uFlag);
			break;
		}
		if (iCurrentX < rcDraw.right)
			iCurrentX = rcDraw.right;
		rcDraw.top = rc.top;
		rcDraw.bottom = rc.bottom;
		dc.DrawText(strSrc.c_str() + iBoldStart, iBoldEnd - iBoldStart, &rcDraw, DT_SINGLELINE | DT_NOPREFIX | uFlag);
		if (bQuitFlag)
			break;
	}
}
