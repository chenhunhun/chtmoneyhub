#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"


#define MENU_LEFT_PART_WIDTH		28
#define MENU_MARGIN					5


CMenuSkin::CMenuSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("menu_selection_bg"), m_imgMenuBack);
	pSkin->RegisterFileName(_T("menu_icon"), m_imgMenuIcon);
	pSkin->RegisterFileName(_T("menu_sep"), m_imgMenuSep);
}

void CMenuSkin::Refresh()
{
	m_imgMenuBack.LoadFromFile();
	m_imgMenuIcon.LoadFromFile();
	m_imgMenuSep.LoadFromFile();
}

//////////////////////////////////////////////////////////////////////////

void CMenuSkin::DrawMenuBorder(CDCHandle dc, HDC dcBack, const RECT &rect, bool bJustDrawBorder, bool bEraseBg)
{
	RECT rcClient = { 0, 0, rect.right - rect.left, rect.bottom - rect.top };
	if (bEraseBg)
	{
		dc.BitBlt(0, 0, rcClient.right, rcClient.bottom, dcBack, 5, 5, SRCCOPY);
		return;
	}
	if (bJustDrawBorder)
	{
		CRgn rgn0, rgn1;
		rgn0.CreateRectRgnIndirect(&rcClient);
		RECT rcCore = { MENU_MARGIN, MENU_MARGIN, rcClient.right - MENU_MARGIN, rcClient.bottom - MENU_MARGIN };
		rgn1.CreateRectRgnIndirect(&rcCore);
		rgn0.CombineRgn(rgn1, RGN_XOR);
		dc.SelectClipRgn(rgn0);
		dc.BitBlt(0, 0, rcClient.right, rcClient.bottom, dcBack, 0, 0, SRCCOPY);
		dc.SelectClipRgn(NULL);
	}
	else
		dc.BitBlt(0, 0, rcClient.right, rcClient.bottom, dcBack, 0, 0, SRCCOPY);
}

void CMenuSkin::DrawMenuCoreBackgroundWithClientDC(CDCHandle dc, HDC dcBack, const RECT &rcClient)
{
	dc.BitBlt(rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		dcBack, rcClient.left + MENU_MARGIN, rcClient.top + MENU_MARGIN, SRCCOPY);
}

void CMenuSkin::DrawColumnSepLine(CDCHandle dc, const RECT &rcClient, int iLineX)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, CSkinProperty::GetColor(Skin_Property_Menu_Vertical_Sep_Color));
	dc.SelectPen(pen);
	dc.MoveTo(iLineX, rcClient.top + GetMenuMarginDelta());
	dc.LineTo(iLineX, rcClient.bottom - GetMenuMarginDelta());
}

void CMenuSkin::DrawMenuItemSelectionBack(CDCHandle dc, const RECT &rect)
{
	if (rect.bottom - rect.top == m_imgMenuBack.GetHeight())
		CSkinManager::DrawExtends(dc, rect, m_imgMenuBack, 0, m_imgMenuBack.GetWidth());
}

void CMenuSkin::DrawMenuItemText(CDCHandle dc, UINT uMenuRes, WORD wMenuID, const CString &lpszMenuString, const RECT &rect, CBitmapHandle hBitmap, CBitmapHandle hBitmapMask, bool bEnabled, bool bBold)
{
	if (hBitmap == (HBITMAP)-1 || hBitmap == (HBITMAP)-2)
	{
		int iWidth = m_imgMenuIcon.GetWidth() / 2;
		int iBegin = iWidth * (-(int)(HBITMAP)hBitmap - 1);
		int iY = (rect.bottom - rect.top - ICON_WIDTH) / 2;
		RECT rcDest = { rect.left + iY + 1, rect.top + iY, rect.left + iY + iWidth + 1, rect.top + iY + iWidth };
		RECT rcSrc = { iBegin, 0, iBegin + iWidth, m_imgMenuIcon.GetHeight() };
		m_imgMenuIcon.Draw(dc, rcDest, rcSrc);
	}
	else if ( (hBitmap && ::GetObjectType(hBitmap) == OBJ_BITMAP) || 
              (hBitmapMask && ::GetObjectType(hBitmapMask) == OBJ_BITMAP) )
	{
		ICONINFO ii = { TRUE, 8, 8, hBitmapMask, hBitmap };
		CIcon ico;
		ico.CreateIconIndirect(&ii);

		int iY = (rect.bottom - rect.top - ICON_WIDTH) / 2;
		ico.DrawIconEx(dc, rect.left + iY + 1, rect.top + iY, ICON_WIDTH, ICON_WIDTH);
	}

	CString strMenuText = lpszMenuString;
	int n = strMenuText.Find('\n');
	if (n >= 0)
		strMenuText = strMenuText.Mid(0, n);

	TCHAR szAcc[128] = { 0 };
	n = strMenuText.Find('\t');
	if (n >= 0)
	{
		_tcscpy_s(szAcc, strMenuText.Mid(n + 1));
		strMenuText = strMenuText.Mid(0, n);
	}

	RECT rcText = rect;
	rcText.left += ICON_WIDTH + 6;
	rcText.right -= 5;

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(CSkinProperty::GetColor(bEnabled ? Skin_Property_Menu_Text_Color : Skin_Property_Menu_Disabled_Text_Color));
	dc.SelectFont(bBold ? s()->Common()->GetBoldFont() : s()->Common()->GetDefaultFont());
	dc.DrawText(strMenuText, strMenuText.GetLength(), &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

	if (szAcc[0])
	{
		dc.SetTextColor(CSkinProperty::GetColor(Skin_Property_Menu_Shortcut_Text_Color));
		dc.SelectFont(s()->Common()->GetMenuAccFont());
		dc.DrawText(szAcc, -1, &rcText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
}


void CMenuSkin::DrawMenuSep(CDCHandle dc, const RECT &rect)
{
	int iY = (rect.bottom - rect.top - m_imgMenuSep.GetHeight()) / 2;
	RECT rcDest = { rect.left, rect.top + iY, rect.right, rect.top + iY + m_imgMenuSep.GetHeight() };
	RECT rcSrc = { 0, 0, m_imgMenuSep.GetWidth(), m_imgMenuSep.GetHeight() };
	m_imgMenuSep.Draw(dc, rcDest, rcSrc);
}

//////////////////////////////////////////////////////////////////////////

void CMenuSkin::DrawMenuNonClient(CDCHandle dc, const RECT &rect)
{
	CMemoryDC dcBuf(dc, rect);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, CSkinProperty::GetColor(Skin_Property_Menu_Border_Color));
	CBrush brush;
	brush.CreateSolidBrush(CSkinProperty::GetColor(Skin_Property_Menu_Background_Color));
	dcBuf.SelectPen(pen);
	dcBuf.SelectBrush(brush);

	dcBuf.Rectangle(&rect);
}
