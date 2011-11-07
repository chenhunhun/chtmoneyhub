#include "stdafx.h"
#include "../../SkinManager/SkinManager.h"
#include "Menu.h"


#define MENU_EXTRA_WIDTH			20


CTuotuoMenu* CTuotuoMenu::sm_pTopMenuWnd = NULL;
CTuotuoMenu::PopupMenuMap CTuotuoMenu::sm_PopupMenuMap;
CTuotuoMenu::TrackPopupMenuFlagVector CTuotuoMenu::sm_TrackFlags;
CTuotuoMenu::TrackPopupMenuHMenuVector CTuotuoMenu::sm_TrackPopupMenuHMenuVector;
CTuotuoMenu::PopupMenuVec CTuotuoMenu::sm_PopupMenuVector;


CTuotuoMenu::CTuotuoMenu(HMENU hMenu, HWND hWnd) : m_bUseTrackParams(false)
{
	sm_pTopMenuWnd = this;
	m_hMenu = hMenu;
	if (hWnd)
		SubclassWindow(hWnd);
	sm_PopupMenuMap.insert(std::make_pair(hMenu, this));
	sm_PopupMenuVector.push_back(this);

	if (!sm_TrackFlags.empty())
	{
		if (sm_TrackPopupMenuHMenuVector[sm_TrackPopupMenuHMenuVector.size() - 1] == m_hMenu)
			m_bUseTrackParams = true;
		m_TrackParams = sm_TrackFlags[sm_TrackFlags.size() - 1];
	}
	else
		memset(&m_TrackParams, 0, sizeof(TrackPopupMenuParams));

	int iLen = GetMenuItemCount();
	MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_FTYPE | MIIM_CHECKMARKS | MIIM_BITMAP };
	for (int i = 0; i < iLen; i++)
	{
		GetMenuItemInfo(i, TRUE, &mii);
		mii.fType |= MFT_OWNERDRAW;
		if (mii.hbmpItem)
		{
			mii.hbmpChecked = mii.hbmpItem;
			mii.hbmpItem = 0;
		}
		SetMenuItemInfo(i, TRUE, &mii);
	}
}

//////////////////////////////////////////////////////////////////////////

LRESULT CTuotuoMenu::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return 0;
}

void CTuotuoMenu::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
	if (!(lpWndPos->flags & SWP_NOMOVE))
	{
		if (m_bUseTrackParams)
		{
			RECT rc;
			GetWindowRect(&rc);
			rc.right -= rc.left;
			rc.bottom -= rc.top;

			if ((m_TrackParams.dwFlags & (TPM_CENTERALIGN | TPM_RIGHTALIGN)) == 0)		// 处理左对齐的问题
			{
				HMONITOR hMon = ::MonitorFromPoint(CPoint(lpWndPos->x, lpWndPos->y), MONITOR_DEFAULTTONEAREST);
				MONITORINFO mi = { sizeof(MONITORINFO) };
				::GetMonitorInfo(hMon, &mi);
				if (m_TrackParams.pt.x + rc.right > mi.rcWork.right)
				{
					lpWndPos->x = mi.rcWork.right - rc.right;
					if (m_TrackParams.rcExclude.top < m_TrackParams.rcExclude.bottom)
					{
						if (m_TrackParams.pt.y + rc.bottom > mi.rcWork.bottom)
							lpWndPos->y = m_TrackParams.rcExclude.top - rc.bottom;
					}
				}
			}
		}
	}
	DefWindowProc();
}

//////////////////////////////////////////////////////////////////////////

void CTuotuoMenu::OnPrint(CDCHandle dc, UINT uFlags)
{
	CheckSize();
	RECT rcWnd;
	GetWindowRect(&rcWnd);
	ss()->Menu()->DrawMenuBorder(dc, m_backDC, rcWnd, false, false);
	DefWindowProc();
	ss()->Menu()->DrawMenuBorder(dc, m_backDC, rcWnd, true, false);
}

void CTuotuoMenu::OnNcPaint(CRgn rgn)
{
	CheckSize();
	RECT rcWnd;
	GetWindowRect(&rcWnd);
	CWindowDC dc(m_hWnd);
	dc.SelectClipRgn(rgn);
	ss()->Menu()->DrawMenuBorder((HDC)dc, m_backDC, rcWnd, false, false);
}

BOOL CTuotuoMenu::OnEraseBkgnd(CDCHandle dc)
{
	RECT rcWnd;
	GetWindowRect(&rcWnd);
	ss()->Menu()->DrawMenuBorder((HDC)dc, m_backDC, rcWnd, false, true);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void CTuotuoMenu::CheckSize()
{
	if (m_backDC == NULL)
	{
		CWindowDC dc(m_hWnd);
		CRect rcWnd;
		GetWindowRect(&rcWnd);
		m_backDC.CreateCompatibleDC(dc);
		m_backBMP.CreateCompatibleBitmap(dc, rcWnd.Width(), rcWnd.Height());
		m_backDC.SelectBitmap(m_backBMP);
		RECT rcDraw = { 0, 0, rcWnd.Width(), rcWnd.Height() };
		ss()->Menu()->DrawMenuNonClient((HDC)m_backDC, rcDraw);
	}
}

void CTuotuoMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasure)
{
	CTuotuoMenu *pMenu = sm_pTopMenuWnd;

	TCHAR szCaption[512] = { 0 };
	MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_ID | MIIM_DATA | MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU | MIIM_STATE };
	mii.dwTypeData = szCaption;
	mii.cch = 511;
	pMenu->GetMenuItemInfo(lpMeasure->itemID, FALSE, &mii);

	CClientDC dc(pMenu->m_hWnd);
	lpMeasure->itemWidth = GetMenuItemWidth(dc, szCaption, mii.wID, pMenu->m_TrackParams.uMenuRes, mii.hSubMenu != NULL);
	LONG uState = pMenu->GetMenuState(lpMeasure->itemID, MF_BYCOMMAND);
	lpMeasure->itemHeight = (uState & MF_SEPARATOR) && (uState & MF_POPUP) == 0 ? MENU_SEP_HEIGHT : ss()->Menu()->GetMenuHeight();
}

void CTuotuoMenu::DrawItem(LPDRAWITEMSTRUCT lpDraw)
{
	PopupMenuMap::iterator it = sm_PopupMenuMap.find((HMENU)lpDraw->hwndItem);
	if (it == sm_PopupMenuMap.end())
		return;
	CTuotuoMenu *pMenu = it->second;

	MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_FTYPE | MIIM_STRING | MIIM_STATE | MIIM_CHECKMARKS };
	TCHAR szCaption[512];
	mii.dwTypeData = szCaption;
	mii.cch = 511;
	BOOL b = pMenu->GetMenuItemInfo(lpDraw->itemID, FALSE, &mii);
	if (b == FALSE)
	{
		POINT pt = { lpDraw->rcItem.left + 2, lpDraw->rcItem.top + 2 };
		pMenu->ClientToScreen(&pt);
		int iPos = pMenu->MenuItemFromPoint(NULL, pt);
		pMenu->GetMenuItemInfo(iPos, TRUE, &mii);
	}

	if (mii.fType & MFT_MENUBREAK)
	{
		RECT rcClient;
		pMenu->GetClientRect(&rcClient);
		ss()->Menu()->DrawColumnSepLine(lpDraw->hDC, rcClient, lpDraw->rcItem.left - 3);
	}

	ss()->Menu()->DrawMenuCoreBackgroundWithClientDC(lpDraw->hDC, pMenu->m_backDC, lpDraw->rcItem);
	if (mii.fType & MFT_SEPARATOR)
		ss()->Menu()->DrawMenuSep(lpDraw->hDC, lpDraw->rcItem);
	else
	{
		if (lpDraw->itemState & ODS_SELECTED && ((mii.fState & MFS_GRAYED) != MFS_GRAYED))
			ss()->Menu()->DrawMenuItemSelectionBack(lpDraw->hDC, lpDraw->rcItem);
		if (mii.fState & MFS_CHECKED)
			mii.hbmpChecked = (HBITMAP)((mii.fType & MFT_RADIOCHECK) ? -1 : -2);
		ss()->Menu()->DrawMenuItemText(lpDraw->hDC, pMenu->m_TrackParams.uMenuRes, lpDraw->itemID, szCaption, lpDraw->rcItem, mii.hbmpChecked, mii.hbmpUnchecked, (mii.fState & MFS_GRAYED) == 0, (mii.fState & MFS_DEFAULT) != 0);
	}
}

//////////////////////////////////////////////////////////////////////////

void CTuotuoMenu::OnFinalMessage(HWND /* hWnd */)
{
	sm_PopupMenuMap.erase(m_hMenu);
	for (int i = sm_PopupMenuVector.size() - 1; i >= 0; i--)
		if (sm_PopupMenuVector[i] == this)
		{
			sm_PopupMenuVector.erase(sm_PopupMenuVector.begin() + i);
			break;
		}
	delete this;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL OnMenuSelect(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

void OnMenuLoop(HWND hWnd, bool bIsEnterMenuLoop)
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int g_iCurrentMenuMaxAllowedHeight = 0;

void AdjustMenuPos(int &iX, int &iY, DWORD dwFlags, SIZE szMenu, LPRECT rcExclude);
SIZE GetMenuWidthAndHeight(CMenuHandle menu, HWND hWnd);


BOOL TuoTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm, UINT uMenuRes)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

int GetMenuItemWidth(CDCHandle dc, LPCTSTR lpszCaption, WORD wID, DWORD dwMenuResID, bool bContainSubMenu)
{
	dc.SelectFont(s()->Common()->GetDefaultFont());
	const TCHAR *p = _tcschr(lpszCaption, '\n');
	CSize sz, szAcc;
	if (p)
		dc.GetTextExtent(lpszCaption, p - lpszCaption, &sz);
	else
		dc.GetTextExtent(lpszCaption, -1, &sz);

	if (dwMenuResID != 0)
	{
	}

	if (bContainSubMenu)
		sz.cx += 5;

	if (sz.cx > MENU_TEXT_MAX_WIDTH)
		sz.cx = MENU_TEXT_MAX_WIDTH;

	return sz.cx + MENU_EXTRA_WIDTH;
}

int GetMenuHeight(CMenuHandle menu)
{
	int iHeight = 0;
	int iLen = menu.GetMenuItemCount();
	MENUITEMINFO mii = { sizeof(MENUITEMINFO), MIIM_FTYPE };
	for (int i = 0; i < iLen; i++)
	{
		menu.GetMenuItemInfo(i, TRUE, &mii);
		iHeight += ((mii.fType & MF_SEPARATOR) && (mii.fType & MF_POPUP) == 0) ? MENU_SEP_HEIGHT : ss()->Menu()->GetMenuHeight();
	}
	return iHeight;
}

void CalcMenuMaxAllowedHeight(HWND hWnd, LPTPMPARAMS lptpm)
{
	HMONITOR hMon = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(MONITORINFO) };
	::GetMonitorInfo(hMon, &mi);

	if (lptpm)
	{
		int iScreenHeight = mi.rcWork.bottom - mi.rcWork.top;
		if (mi.rcWork.bottom - lptpm->rcExclude.bottom >= lptpm->rcExclude.top - mi.rcWork.top)
			g_iCurrentMenuMaxAllowedHeight = mi.rcWork.bottom - lptpm->rcExclude.bottom;
		else
			g_iCurrentMenuMaxAllowedHeight = lptpm->rcExclude.top - mi.rcWork.top;
	}
	else		// 如果是子菜单，则按全屏高度计算也可以
		g_iCurrentMenuMaxAllowedHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
	g_iCurrentMenuMaxAllowedHeight -= 10;
}
