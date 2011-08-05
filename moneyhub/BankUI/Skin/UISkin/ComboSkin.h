#pragma once
#include "SkinLoader.h"
#include "TuoImage.h"

class CSkinManager;

class CComboSkin
{

public:

	CComboSkin(CSkinManager *pSkin);

	void Refresh();

	int GetComboBackgroundHeight(int iType) const		{ return m_bmpComboBackground[iType].GetHeight(); }
	int GetComboEditHeight(int iType) const				{ return m_iEditControlHeight[iType]; }
	int GetComboEditTopOffset(int iType) const			{ return m_iEditControlTopOffset[iType]; }
	int GetComboDropdownItemBackgroundHeight() const { return m_bmpDropdownButtonBack.GetHeight() + 3; }
	int GetComboDropdownTriangleButtonWidth() const	{ return CSkinProperty::GetDWORD(Skin_Property_AddressBar_DropdownArrow_Width); }
	int GetSearchBarDropdownArrowWidth() const	{ return m_bmpSearchBarDropdown.GetWidth(); }
	int GetWidthOfAddrBarText(CDCHandle dc, CString &lpszText);

	BOOL GetPointerOnDeleteButton(const RECT& rcItem, POINT& pt, int iBig);

	void DrawComboEditBackground(CDCHandle dc, const RECT &rcCombo, ButtonStatus eStatus, int iContainerType);
	void DrawComboEditBackgroundPart(CDCHandle dc, const RECT &rcEdit, int iYOffsetCombo, ButtonStatus eStatus, int iContainerType, bool bUseBlit);
	void DrawComboEditTipText(CDCHandle dc, const RECT &rcComboEdit, LPCTSTR lpszText, bool bAero);
	void DrawComboDropDownListBackground(CDCHandle dc, const RECT &rcDropDown);
	void DrawComboDropDownListItem(CDCHandle dc, const RECT &rcItem, ButtonStatus eStatus, LPCTSTR lpszStr);
	void DrawSearchBarDropDownArrow(CDCHandle dc, int x, int y);
	void DrawSearchTipText(CDCHandle dc, const RECT &rcSearch, LPCTSTR lpszText, bool bAero);
	
	static std::wstring GenerateMask(const std::wstring &strSrc, const std::wstring &strSearchStr);

private:

	static void MaskSearchString(const std::wstring &strKey, std::wstring &strMask);
	static void DrawTextWithBold(HDC hDC, const std::wstring &strSrc, const std::wstring &strMask, const RECT &rc, UINT uFlag);

	void GetDeleteButtonRect(const RECT& rcItem, int iBig, RECT& rcDel);


	CTuoImage m_bmpComboBackground[3];
	CTuoImage m_bmpDropdownButtonBack;
	CTuoImage m_imgDropdownIcon;
	CTuoImage m_imgDropdownCloseIcon;

	CTuoImage m_bmpSearchBarDropdown;

	CTuoIcon m_icoDefaultSearch;

	int m_iEditControlHeight[3];			// 地址栏、搜索栏和查找栏的edit控件的高度
	int m_iEditControlTopOffset[3];			// 地址栏、搜索栏和查找栏的edit控件的上移调整距离
};
