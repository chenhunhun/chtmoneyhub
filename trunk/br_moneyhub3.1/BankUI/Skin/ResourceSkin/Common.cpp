#include "stdafx.h"
#include "../../Resource/Resource.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"


CCommonSkin::CCommonSkin(CSkinManager *pSkin) : m_patchDraw(ICON_WIDTH, ICON_WIDTH)
{
	pSkin->RegisterFileName(_T("default_btn_bg"), m_bmpButtonBack);
	pSkin->RegisterFileName(_T("default_page"), m_icoDefaultPageIcon);
	pSkin->RegisterFileName(_T("home"), m_icoHome);
	pSkin->RegisterFileName(_T("folder_close"), m_icoFolderClose);
	pSkin->RegisterFileName(_T("folder_open"), m_icoFolderOpen);
	pSkin->RegisterFileName(_T("inprivate_overlay"), m_icoInprivateOverlay);
	pSkin->RegisterFileName(_T("mainicon"), m_bmpMainIcon);
}


void CCommonSkin::Refresh(bool bReload)
{
	if (bReload)
	{
		m_font.DeleteObject();
		m_fontBold.DeleteObject();
		m_fontTitle.DeleteObject();
		m_bigFontInEditCtrl.DeleteObject();
		m_fontAddressBarList.DeleteObject();
		m_fontAddressBarListBold.DeleteObject();
		m_fontEditBoxSmall.DeleteObject();
		m_fontAcc.DeleteObject();
	}

	// 第一维数组表示是否使用vista的微软雅黑字体，第二维表示在各个情况下的字体大小
	LPCTSTR lpszFontName[] = { _T("Tahoma"), _T("微软雅黑") };

	HDC hdc = ::GetDC(NULL);
	int iDI = ::GetDeviceCaps(hdc, LOGPIXELSX);
	::ReleaseDC(NULL, hdc);

	int iFontSizeList[2][5] = {		// 5个值依次是：普通字体，地址栏的大号字体，地址栏下拉列表的大号字体、小号字体，专门给richedit用的小号字体
		{ 13, 18, 16, 14, iDI >= 120 ? 14 : 13 },
		{ 17, 20, 19, 17, 17 }
	};
	int iIndex = CSkinLoader::IsVista() ? 1 : 0;

	m_font.CreateFont(iFontSizeList[iIndex][0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
	m_fontEditBoxSmall.CreateFont(iFontSizeList[iIndex][4], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
	m_fontBold.CreateFont(iFontSizeList[iIndex][0], 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
	m_fontTitle.CreateFont(iFontSizeList[iIndex][0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
	m_bigFontInEditCtrl.CreateFont(iFontSizeList[iIndex][1], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
	m_fontAddressBarList.CreateFont(iFontSizeList[iIndex][2], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
	m_fontAddressBarListBold.CreateFont(iFontSizeList[iIndex][2], 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
	m_fontAcc.CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Verdana"));

	m_bmpButtonBack.LoadFromFile();

	m_bmpMainIcon.LoadFromFile();

	if (m_hcurNormal == NULL)
		m_hcurNormal = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));

	if (m_icoMainIcon.IsNull())
		m_icoMainIcon.LoadIcon(IDI_ICON1, ICON_WIDTH, ICON_WIDTH);
	if (m_icoVideoIcon.IsNull())
		m_icoVideoIcon.LoadIcon(IDI_ICON_VIDEO, ICON_WIDTH, ICON_WIDTH);
}

//////////////////////////////////////////////////////////////////////////

void CCommonSkin::DrawButtonBack(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus)
{
	int iFullSize = m_bmpButtonBack.GetWidth() / 5;
	int iBegin = iFullSize * eStatus;
	CSkinManager::DrawExtends(dc, rcBtn, m_bmpButtonBack, iBegin, iBegin + iFullSize);
}

void CCommonSkin::DrawButtonBack(CDCHandle dc, const RECT &rcBtn, UINT uStatus)
{
	ButtonStatus btnStatus = CSkinManager::ConvertBtnStateToButtonStatus(uStatus);
	DrawButtonBack(dc, rcBtn, btnStatus);
}


void CCommonSkin::DrawIcon(CDCHandle dc, HICON hIcon, int x, int y, bool bAero)
{
	if (hIcon == NULL)
		::DrawIconEx(dc, x, y, GetDefaultPageIcon(), ICON_WIDTH, ICON_WIDTH, 0, NULL, DI_NORMAL);
	else if (hIcon == ICON_HOME)
		::DrawIconEx(dc, x, y, m_icoHome, ICON_WIDTH, ICON_WIDTH, 0, NULL, DI_NORMAL);
	else
	{
		if (bAero)
		{
			m_patchDraw.BeginDraw();
			::DrawIconEx(m_patchDraw.GetDC(0), 0, 0, hIcon, ICON_WIDTH, ICON_WIDTH, 0, NULL, DI_NORMAL);
			::DrawIconEx(m_patchDraw.GetDC(1), 0, 0, hIcon, ICON_WIDTH, ICON_WIDTH, 0, NULL, DI_NORMAL);
			m_patchDraw.EndDraw();
			m_patchDraw.AlphaBlend(dc, x, y);
		}
		else
			::DrawIconEx(dc, x, y, hIcon, ICON_WIDTH, ICON_WIDTH, 0, NULL, DI_NORMAL);
	}
}

void CCommonSkin::DrawMainIcon(CDCHandle dc, int x, int y)
{
	int width = m_bmpMainIcon.GetWidth();
	int height = m_bmpMainIcon.GetHeight();
	RECT rc = { x, y, x + width, y + height };

	CSkinManager::DrawExtends(dc, rc, m_bmpMainIcon, 0, width);
}