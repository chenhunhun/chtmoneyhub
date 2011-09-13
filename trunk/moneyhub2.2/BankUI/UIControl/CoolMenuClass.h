
#pragma once

#include <vector>
#include "Draw.h"

#define MAX_MENU_ITEM_TEXT_LENGTH       100
#define IMGPADDING                      6
#define TEXTPADDING                     8
#define OBM_CHECK                       32760

template <class T>
class CCoolMenuClass
{
private:
	SIZE m_szBitmap;
	SIZE m_szButton;

	CFont m_fontMenu;               // used internally, only to measure text
	int m_cxExtraSpacing;
	COLORREF m_clrMask;

	std::vector<HMENU> m_vecMenuHandle;

protected:
	struct MenuItemData	            // menu item data
	{
		LPTSTR lpstrText;
		UINT fType;
		UINT fState;
		int iImage;
	};

protected:
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
	{
		MenuItemData* pmd = (MenuItemData*)lpMeasureItemStruct->itemData;

		if (pmd->fType & MFT_SEPARATOR)   // separator - use half system height and zero width
		{
			lpMeasureItemStruct->itemHeight = 0;
			lpMeasureItemStruct->itemWidth  = 0;
		}
		else
		{
			// Compute size of text - use DrawText with DT_CALCRECT
			CWindowDC dc(NULL);
			CFont fontBold;
			HFONT hOldFont = NULL;
			if (pmd->fState & MFS_DEFAULT)
			{
				// Need bold version of font
				LOGFONT lf = { 0 };
				m_fontMenu.GetLogFont(lf);
				lf.lfWeight += 200;
				fontBold.CreateFontIndirect(&lf);
				ATLASSERT(fontBold.m_hFont != NULL);
				hOldFont = dc.SelectFont(fontBold);
				fontBold.DeleteObject();
			}
			else
				hOldFont = dc.SelectFont(m_fontMenu);

			RECT rcText = { 0, 0, 0, 0 };
			dc.DrawText(pmd->lpstrText, -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);

			int cx = rcText.right - rcText.left;

			dc.SelectFont(hOldFont);

			LOGFONT lf = { 0 };
			m_fontMenu.GetLogFont(lf);
			int cy = lf.lfHeight;
			if (cy < 0)
				cy = -cy;
			const int cyMargin = 10;
			cy += cyMargin + 1;

			lpMeasureItemStruct->itemHeight = cy;

			// Width is width of text plus some
			cx += 4;                    // L/R margin for readability
			cx += 1;                    // space between button and menu text
			cx += 2 * m_szButton.cx;    // button width
			cx += m_cxExtraSpacing;     // extra between item text and accelerator keys

			// Windows adds 1 to returned value
			cx -= ::GetSystemMetrics(SM_CXMENUCHECK) - 1;
			lpMeasureItemStruct->itemWidth = cx;   // we are done
		}
	}

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		MenuItemData* pmd = (MenuItemData*)lpDrawItemStruct->itemData;
		CDCHandle dc = lpDrawItemStruct->hDC;
		RECT& rcItem = lpDrawItemStruct->rcItem;
		LPCRECT pRect = &rcItem;
		BOOL bDisabled = lpDrawItemStruct->itemState & ODS_GRAYED;
		BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
		BOOL bChecked = lpDrawItemStruct->itemState & ODS_CHECKED;
		COLORREF crBackImg = CLR_NONE;
		CDCHandle* pDC = &dc; 

		if (bSelected && !bDisabled)
		{
			COLORREF crHighLight = ::GetSysColor(COLOR_HIGHLIGHT);
			CPenDC pen(*pDC, crHighLight);
			CBrushDC brush(*pDC, RGB(204, 212, 225));

			CRect rc(pRect);
			rc.bottom -= 1;

			pDC->Rectangle(rc);
		}
		else
		{ 
			// Draw the menu item background
			CRect rc(pRect);
			rc.bottom -= 1;

			pDC->FillSolidRect(rc, RGB(247, 252, 255));
		}

		// Menu item is a separator
		if (pmd->fType & MFT_SEPARATOR)
		{
		}
		else
		{   
			// Draw the text
			CRect rc(pRect);
			rc.bottom -= 1;

			CString sCaption = pmd->lpstrText;
			int nTab = sCaption.Find('\t');

			if (nTab >= 0)
			{
				sCaption = sCaption.Left (nTab);
			}
			pDC->SetTextColor(RGB(62, 99, 124));
			pDC->SetBkMode(TRANSPARENT);

			CBoldDC bold(*pDC, (lpDrawItemStruct->itemState & ODS_DEFAULT) != 0);

			rc.left = m_szBitmap.cx + IMGPADDING + TEXTPADDING;
			pDC->DrawText(sCaption, sCaption.GetLength(), rc, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

			if (nTab >= 0)
			{    
				rc.right -= TEXTPADDING + 4;
				pDC->DrawText(pmd->lpstrText + nTab + 1, _tcslen(pmd->lpstrText + nTab + 1), rc, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);
			}

			// Draw background and border around the check mark
			if (bChecked)
			{
				COLORREF crHighLight = ::GetSysColor(COLOR_HIGHLIGHT);
				CPenDC pen(*pDC, crHighLight);
				CBrushDC brush(*pDC, crBackImg = bDisabled ? HLS_TRANSFORM(::GetSysColor (COLOR_3DFACE), +73, 0) :
					(bSelected ? HLS_TRANSFORM(crHighLight, +50, -50) : HLS_TRANSFORM(crHighLight, +70, -57)));

				pDC->Rectangle(CRect(pRect->left + 1, pRect->top + 1, pRect->left + m_szButton.cx - 2, pRect->bottom - 1));
			}

			if (bChecked)
			{
				// Draw the check mark
				rc.left  = pRect->left + 5;
				rc.right = rc.left + m_szBitmap.cx + IMGPADDING;
				pDC->SetBkColor(crBackImg);
				HBITMAP hBmp = LoadBitmap(NULL, MAKEINTRESOURCE(OBM_CHECK));
				BOOL bRet = pDC->DrawState(CPoint(rc.left,rc.top + 3), CSize(rc.Size()), hBmp, DSS_NORMAL, (HBRUSH)NULL);
				DeleteObject(hBmp);
			}
		}

		if (ID_HELP_ABOUT != lpDrawItemStruct->itemID)
		{
			CRect rc(pRect);
			CPenDC pen(*pDC, RGB(103, 170, 216));
			pDC->MoveTo(rc.left, rc.bottom - 1);
			pDC->LineTo(rc.right, rc.bottom - 1);
		}
	}

public:
	CCoolMenuClass()
	{
		m_cxExtraSpacing = 0;
		m_clrMask = RGB(192, 192, 192);

		m_szBitmap.cx = 16;
		m_szBitmap.cy = 15;

		m_szButton.cx = m_szBitmap.cx + 6;
		m_szButton.cy = m_szBitmap.cy + 6;
	}

	~CCoolMenuClass()
	{
		m_fontMenu.DeleteObject();
	}

	// Note: do not forget to put CHAIN_MSG_MAP in your message map.
	BEGIN_MSG_MAP(CCoolMenuClass)
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
	END_MSG_MAP()

	void InitCoolMenuSettings()
	{
		// Set up the font
		NONCLIENTMETRICS info = { 0 };
		info.cbSize = sizeof(info);
		BOOL bRet = ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
		ATLASSERT(bRet);

		if (bRet)
		{
			LOGFONT logfont = { 0 };
			if (m_fontMenu.m_hFont != NULL)
				m_fontMenu.GetLogFont(logfont);

			if (logfont.lfHeight != info.lfMenuFont.lfHeight ||
				logfont.lfWidth != info.lfMenuFont.lfWidth ||
				logfont.lfEscapement != info.lfMenuFont.lfEscapement ||
				logfont.lfOrientation != info.lfMenuFont.lfOrientation ||
				logfont.lfWeight != info.lfMenuFont.lfWeight ||
				logfont.lfItalic != info.lfMenuFont.lfItalic ||
				logfont.lfUnderline != info.lfMenuFont.lfUnderline ||
				logfont.lfStrikeOut != info.lfMenuFont.lfStrikeOut ||
				logfont.lfCharSet != info.lfMenuFont.lfCharSet ||
				logfont.lfOutPrecision != info.lfMenuFont.lfOutPrecision ||
				logfont.lfClipPrecision != info.lfMenuFont.lfClipPrecision ||
				logfont.lfQuality != info.lfMenuFont.lfQuality ||
				logfont.lfPitchAndFamily != info.lfMenuFont.lfPitchAndFamily ||
				lstrcmp(logfont.lfFaceName, info.lfMenuFont.lfFaceName) != 0)
			{
				HFONT hFontMenu = ::CreateFontIndirect(&info.lfMenuFont);
				ATLASSERT(hFontMenu != NULL);
				if (hFontMenu != NULL)
				{
					if (m_fontMenu.m_hFont != NULL)
						m_fontMenu.DeleteObject();
					m_fontMenu.Attach(hFontMenu);
					static_cast<T*>(this)->SetFont(m_fontMenu);
				}
			}
		}

		// Check if we need extra spacing for menu item text
		CWindowDC dc(static_cast<T*>(this)->m_hWnd);
		HFONT hFontOld = dc.SelectFont(m_fontMenu);
		RECT rcText = { 0, 0, 0, 0 };
		dc.DrawText(_T("\t"), -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
		if ((rcText.right - rcText.left) < 4)
		{
			::SetRectEmpty(&rcText);
			dc.DrawText(_T("x"), -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
			m_cxExtraSpacing = rcText.right - rcText.left;
		}
		else
			m_cxExtraSpacing = 0;

		dc.SelectFont(hFontOld);
	}

	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// System menu, do nothing
		if ((BOOL)HIWORD(lParam))   
		{
			bHandled = FALSE;
			return 1;
		}

		CMenuHandle menuPopup = (HMENU)wParam;
		ATLASSERT(menuPopup.m_hMenu != NULL);

		TCHAR szString[MAX_MENU_ITEM_TEXT_LENGTH];
		BOOL bRet = FALSE;

		for (int i = 0; i < menuPopup.GetMenuItemCount(); i++)
		{
			CMenuItemInfo mii;
			mii.cch = MAX_MENU_ITEM_TEXT_LENGTH;
			mii.fMask = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
			mii.dwTypeData = szString;
			bRet = menuPopup.GetMenuItemInfo(i, TRUE, &mii);
			ATLASSERT(bRet);

			if (!(mii.fType & MFT_OWNERDRAW))   // not already an ownerdraw item
			{
				MenuItemData * pMI = new MenuItemData;
				ATLASSERT(pMI != NULL);

				if (pMI)
				{
					// Make this menu item an owner-drawn
					mii.fType |= MFT_OWNERDRAW;

					pMI->fType = mii.fType;
					pMI->fState = mii.fState;

					pMI->lpstrText = new TCHAR[lstrlen(szString) + 1];
					ATLASSERT(pMI->lpstrText != NULL);

					if (pMI->lpstrText != NULL)
						lstrcpy(pMI->lpstrText, szString);
					mii.dwItemData = (ULONG_PTR)pMI;

					bRet = menuPopup.SetMenuItemInfo(i, TRUE, &mii);
					ATLASSERT(bRet);
				}
			}
		}

		m_vecMenuHandle.push_back(menuPopup.m_hMenu);

		return 0;
	}

	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// Check if a menu is closing, do a cleanup
		if (HIWORD(wParam) == 0xFFFF && lParam == NULL)   // Menu closing
		{
			// Restore the menu items to the previous state for all menus that were converted
			HMENU hMenu = NULL;
			//while ((hMenu = m_stackMenuHandle.Pop()) != NULL)
			while (m_vecMenuHandle.size() > 0)
			{
				hMenu = m_vecMenuHandle.back();
				m_vecMenuHandle.pop_back();

				CMenuHandle menuPopup = hMenu;
				ATLASSERT(menuPopup.m_hMenu != NULL);
				// Restore state and delete menu item data
				BOOL bRet = FALSE;
				for (int i = 0; i < menuPopup.GetMenuItemCount(); i++)
				{
					CMenuItemInfo mii;
					mii.fMask = MIIM_DATA | MIIM_TYPE;
					bRet = menuPopup.GetMenuItemInfo(i, TRUE, &mii);
					ATLASSERT(bRet);

					MenuItemData * pMI = (MenuItemData*)mii.dwItemData;
					if (pMI != NULL)
					{
						mii.fMask = MIIM_DATA | MIIM_TYPE | MIIM_STATE;
						mii.fType = pMI->fType;
						mii.dwTypeData = pMI->lpstrText;
						mii.cch = lstrlen(pMI->lpstrText);
						mii.dwItemData = NULL;

						bRet = menuPopup.SetMenuItemInfo(i, TRUE, &mii);
						ATLASSERT(bRet);

						delete [] pMI->lpstrText;
						delete pMI;
					}
				}
			}
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPMEASUREITEMSTRUCT lpMeasureItemStruct = (LPMEASUREITEMSTRUCT)lParam;
		MenuItemData * pmd = (MenuItemData*)lpMeasureItemStruct->itemData;

		if (lpMeasureItemStruct->CtlType == ODT_MENU && pmd != NULL)
			MeasureItem(lpMeasureItemStruct);
		else
			bHandled = FALSE;

		return (LRESULT)TRUE;
	}

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT)lParam;

		MenuItemData * pMI = (MenuItemData*)lpDrawItemStruct->itemData;
		if (lpDrawItemStruct->CtlType == ODT_MENU && pMI != NULL)     // only owner-drawn menu item
			DrawItem(lpDrawItemStruct);
		else
			bHandled = FALSE;

		return TRUE;
	}
};