#include "stdafx.h"
#include <tuotheme.h>
#include "SkinManager.h"

// cry ...


CSkinManager *g_pSkin = NULL;
int CSkinManager::m_iDrawFixWithSomeDisplayCard = 10;


CSkinManager::CSkinManager() : m_hTheme(NULL), m_ComboSkin(this), m_MainFrameBorderSkin(this), m_FavoriteSkin(this),
m_MenuBarSkin(this), m_TabSkin(this), m_StatusBarSkin(this),
m_ToolBarSkin(this), m_BigButtonSkin(this), m_SSLLockButtonSkin(this), m_HelpButtonSkin(this),
m_CommonSkin(this), m_MenuSkin(this), m_TooltipSkin(this), m_CategorySkin(this), m_MenuButtonSkin(this),
m_LogoButtonSkin(this), m_BackButtonSkin(this), m_ForwardButtonSkin(this), m_RefreshButtonSkin(this), m_CoolMenuSkin(this), m_SetButtonSkin(this)
{
	g_pSkin = this;
}

void CSkinManager::RefreshAllSkins(bool bReload)
{
	Combo()->Refresh();
	MainFrame()->Refresh();
	FavorBar()->Refresh();
	MenuBar()->Refresh();
	Tab()->Refresh();
	Category()->Refresh();
	Toolbar()->Refresh();
	HelpButton()->Refresh();
	SettingButton()->Refresh();
	BigButton()->Refresh();
	SSLLockButton()->Refresh();
	StatusBar()->Refresh();
	Tooltip()->Refresh();
	MenuButton()->Refresh();
	LogoButton()->Refresh();
	Common()->Refresh(bReload);

	BackButton()->Refresh();
	ForwardButton()->Refresh();
	RefreshButton()->Refresh();

	CoolMenuSkin()->Refresh();
}

void CSkinManager::CreateTheme(HWND hWnd, bool bDestroyOld)
{
}


void CSkinManager::RegisterFileName(LPCTSTR lpszFileName, CTuoImage &image)
{
	image.m_strFileName = lpszFileName;
	m_File2Image.insert(std::make_pair(lpszFileName, &image));
}

void CSkinManager::RegisterFileName(LPCTSTR lpszFileName, CTuoIcon &icon)
{
	icon.m_strFileName = lpszFileName;
	m_File2Icon.insert(std::make_pair(lpszFileName, &icon));
}


bool CSkinManager::RefreshSkinFile(LPCTSTR lpszFileName)
{
	LPCTSTR pEnd = _tcsrchr(lpszFileName, '.');
	if (!pEnd)
		return false;

	std::wstring strFileName = std::wstring(lpszFileName, pEnd - lpszFileName);
	std::transform(strFileName.begin(), strFileName.end(), strFileName.begin(), tolower);
	if (_tcsicmp(pEnd + 1, _T("png")) == 0)
	{
		FileName2ImageMap::iterator it = m_File2Image.find(strFileName);
		if (it != m_File2Image.end())
			return it->second->LoadFromFile();
	}
	else if (_tcsicmp(pEnd + 1, _T("ico")) == 0)
	{
		FileName2IconMap::iterator it = m_File2Icon.find(strFileName);
		if (it != m_File2Icon.end())
			return it->second->LoadFromFile();
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

ButtonStatus CSkinManager::ConvertBtnStateToButtonStatus(UINT uBtnState)
{
	ButtonStatus iIndex = Btn_MouseOut;
	if (uBtnState & CDIS_DISABLED)
		iIndex = Btn_Disabled;
	else if (uBtnState & CDIS_CHECKED)
	{
		if (uBtnState & CDIS_HOT)
			iIndex = (uBtnState & CDIS_SELECTED) ? Btn_MouseDown : Btn_Addition;
		else
			iIndex = Btn_MouseDown;
	}
	else if (uBtnState & CDIS_HOT)
		iIndex = (uBtnState & CDIS_SELECTED) ? Btn_MouseDown : Btn_MouseOver;
	return iIndex;
}

ButtonStatus CSkinManager::FromTuoToolStatus(UINT uTuoToolStatus)
{
	if (uTuoToolStatus == BTN_STATUS_DISABLED)
		return Btn_Disabled;
	else
	{
		if (uTuoToolStatus & BTN_STATUS_DROPDOWN)
			return Btn_Addition;
		else
		{
			switch (uTuoToolStatus & BTN_STATUS_MASK)
			{
				case BTN_STATUS_MOUSEOUT:
					if (uTuoToolStatus & BTN_STATUS_SPECIAL)
						return Btn_Addition;
					else if (uTuoToolStatus & BTN_STATUS_CHECKED)
						return Btn_MouseDown;
					else
						return Btn_MouseOut;
				case BTN_STATUS_MOUSEOVER:
					return (uTuoToolStatus & BTN_STATUS_CHECKED) ? Btn_Addition : Btn_MouseOver;
				case BTN_STATUS_MOUSEDOWN:
					return Btn_MouseDown;
			}
		}
	}
	ATLASSERT(0);
	return Btn_MouseOut;
}

void CSkinManager::DrawExtends(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcLeft, int iSrcRight)
{
	//ATLASSERT(rcDest.bottom - rcDest.top == img.GetHeight());
	DrawExtends(dc, rcDest, img, iSrcLeft, iSrcRight, 0, img.GetHeight(), (iSrcRight - iSrcLeft) / 2);
}

void CSkinManager::DrawExtends(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcLeft, int iSrcRight, int iTop, int iBottom, int iSepPos)
{
	//ATLASSERT(rcDest.right > rcDest.left);
	if (iSrcRight - iSrcLeft > rcDest.right - rcDest.left)
	{
		int iSizeLeft = (rcDest.right - rcDest.left) / 2;
		int iSizeRight = rcDest.right - rcDest.left - iSizeLeft;
		if (iSizeLeft < 1)
		{
			RECT rcDestLeft = { rcDest.left, rcDest.top, rcDest.left + 1, rcDest.bottom };
			RECT rcSrcLeft = { iSrcLeft, iTop, iSrcLeft + 1, iBottom };
			img.Draw(dc, rcDestLeft, rcSrcLeft);
		}
		else
		{
			RECT rcDestLeft = { rcDest.left, rcDest.top, rcDest.left + iSizeLeft, rcDest.bottom };
			RECT rcSrcLeft = { iSrcLeft, iTop, iSrcLeft + iSizeLeft, iBottom };
			img.Draw(dc, rcDestLeft, rcSrcLeft);
			RECT rcDestRight = { rcDest.right - iSizeRight, rcDest.top, rcDest.right, rcDest.bottom };
			RECT rcSrcRight = { iSrcRight - iSizeRight, iTop, iSrcRight, iBottom };
			img.Draw(dc, rcDestRight, rcSrcRight);
		}
	}
	else
	{
		int iSizeLeft = iSepPos;
		int iSizeRight = iSrcRight - iSrcLeft - iSizeLeft - 1;

		RECT rcDestLeft = { rcDest.left, rcDest.top, rcDest.left + iSizeLeft, rcDest.bottom };
		RECT rcSrcLeft = { iSrcLeft, iTop, iSrcLeft + iSizeLeft, iBottom };
		img.Draw(dc, rcDestLeft, rcSrcLeft);
		RECT rcDestMid = { rcDest.left + iSizeLeft, rcDest.top, rcDest.right - iSizeRight, rcDest.bottom };
		RECT rcSrcMid = { iSrcLeft + iSizeLeft, iTop, iSrcLeft + iSizeLeft + 1, iBottom };
		img.Draw(dc, rcDestMid, rcSrcMid);

		if (iSizeRight > 0)
		{
			RECT rcDestRight = { rcDest.right - iSizeRight, rcDest.top, rcDest.right, rcDest.bottom };
			RECT rcSrcRight = { iSrcRight - iSizeRight, iTop, iSrcRight, iBottom };
			img.Draw(dc, rcDestRight, rcSrcRight);
		}
	}
}

void CSkinManager::DrawTileExtends(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcLeft, int iSrcRight, int iTop, int iBottom, int iTile)
{
	if (iSrcRight - iSrcLeft > rcDest.right - rcDest.left)
	{
		int iSizeLeft = (rcDest.right - rcDest.left) / 2;
		int iSizeRight = rcDest.right - rcDest.left - iSizeLeft;
		if (iSizeLeft < 1)
		{
			RECT rcDestLeft = { rcDest.left, rcDest.top, rcDest.left + 1, rcDest.bottom };
			RECT rcSrcLeft = { iSrcLeft, iTop, iSrcLeft + 1, iBottom };
			img.Draw(dc, rcDestLeft, rcSrcLeft);
		}
		else
		{
			RECT rcDestLeft = { rcDest.left, rcDest.top, rcDest.left + iSizeLeft, rcDest.bottom };
			RECT rcSrcLeft = { iSrcLeft, iTop, iSrcLeft + iSizeLeft, iBottom };
			img.Draw(dc, rcDestLeft, rcSrcLeft);
			RECT rcDestRight = { rcDest.right - iSizeRight, rcDest.top, rcDest.right, rcDest.bottom };
			RECT rcSrcRight = { iSrcRight - iSizeRight, iTop, iSrcRight, iBottom };
			img.Draw(dc, rcDestRight, rcSrcRight);
		}
	}
	else
	{
		//int iSizeLeft = iSepPos;
		//int iSizeRight = iSrcRight - iSrcLeft - iSizeLeft - 1;
		int iSizeLeft = (iSrcRight - iSrcLeft - iTile) / 2;
		int iSizeRight = iSrcRight - iSrcLeft - iSizeLeft - iTile - 1;

		RECT rcDestLeft = { rcDest.left, rcDest.top, rcDest.left + iSizeLeft, rcDest.bottom };
		RECT rcSrcLeft = { iSrcLeft, iTop, iSrcLeft + iSizeLeft, iBottom };
		img.Draw(dc, rcDestLeft, rcSrcLeft);

		int i = 0;
		do {
			RECT rcDestMid = { rcDest.left + iSizeLeft, rcDest.top, rcDest.left + iSizeLeft + iTile/*rcDest.right - iSizeRight*/, rcDest.bottom };
			RECT rcSrcMid = { iSrcLeft + iSizeLeft, iTop, iSrcLeft + iSizeLeft + iTile, iBottom };

			OffsetRect(&rcDestMid, i++ * iTile, 0);
			img.Draw(dc, rcDestMid, rcSrcMid);

			if (rcDestMid.right > (rcDest.right - iSizeRight))
				break;

		} while (true);

		if (iSizeRight > 0)
		{
			RECT rcDestRight = { rcDest.right - iSizeRight, rcDest.top, rcDest.right, rcDest.bottom };
			RECT rcSrcRight = { iSrcRight - iSizeRight, iTop, iSrcRight, iBottom };
			img.Draw(dc, rcDestRight, rcSrcRight);
		}
	}
}

void CSkinManager::DrawOverlayExtends(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSepLeft, int iSepRight)
{
	// ºáÏò²ð·ÖÒ»¸öÍ¼Æ¬
	//        |                 |
	//        v                 v
	//  +-----------------------------+
	//  |     |                 |     |
	//  +-----------------------------+
	//        ^                 ^
	//        |                 |
	//     ×ó±ß¾à          ÓÒ±ß¾à

	RECT rcDestLeft = { rcDest.left, rcDest.top, rcDest.left + iSepLeft, rcDest.bottom };
	RECT rcSrcLeft = { 0, 0, iSepLeft, img.GetHeight() };
	img.Draw(dc, rcDestLeft, rcSrcLeft);
	RECT rcDestRight = { rcDest.right - iSepRight, rcDest.top, rcDest.right, rcDest.bottom };
	RECT rcSrcRight = { img.GetWidth() - iSepRight, 0, img.GetWidth(), img.GetHeight() };
	img.Draw(dc, rcDestRight, rcSrcRight);

	if (rcDest.right - rcDest.left > iSepLeft + iSepRight)
	{
		RECT rcDestMid = { rcDest.left + iSepLeft, rcDest.top, rcDest.right - iSepRight, rcDest.bottom };
		RECT rcSrcMid = { iSepLeft, 0, img.GetWidth() - iSepRight, img.GetHeight() };
		img.Draw(dc, rcDestMid, rcSrcMid);
	}
}

void CSkinManager::DrawOverlayExtendsPart(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSepLeft, int iSepRight, int iLeft, int iRight)
{
	RECT rcDestLeft = { rcDest.left, rcDest.top, rcDest.left + iSepLeft, rcDest.bottom };
	RECT rcSrcLeft = { iLeft, 0, iLeft + iSepLeft, img.GetHeight() };
	img.Draw(dc, rcDestLeft, rcSrcLeft);

	if (rcDest.right - rcDest.left < iSepLeft + iSepRight)
		iSepRight = rcDest.right - rcDest.left - iSepLeft;
	RECT rcDestRight = { rcDest.right - iSepRight, rcDest.top, rcDest.right, rcDest.bottom };
	RECT rcSrcRight = { iRight - iSepRight, 0, iRight, img.GetHeight() };
	img.Draw(dc, rcDestRight, rcSrcRight);

	if (rcDest.right - rcDest.left > iSepLeft + iSepRight)
	{
		RECT rcDestMid = { rcDest.left + iSepLeft, rcDest.top, rcDest.right - iSepRight, rcDest.bottom };
		RECT rcSrcMid = { iLeft + iSepLeft, 0, iRight - iSepRight, img.GetHeight() };
		img.Draw(dc, rcDestMid, rcSrcMid);
	}
}


void CSkinManager::DrawExtendsVertical(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcTop, int iSrcBottom)
{
	ATLASSERT(rcDest.right - rcDest.left == img.GetWidth());
	DrawExtends(dc, rcDest, img, iSrcTop, iSrcBottom, 0, img.GetWidth(), (iSrcBottom - iSrcTop) / 2);
}

void CSkinManager::DrawExtendsVertical(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcTop, int iSrcBottom, int iLeft, int iRight, int iSepPos)
{
	ATLASSERT(rcDest.bottom > rcDest.top);
	//ATLASSERT(iSrcBottom - iSrcTop <= rcDest.bottom - rcDest.top);

	if (iSrcBottom - iSrcTop <= rcDest.bottom - rcDest.top)
	{
		int iSizeBottom = iSrcBottom - iSrcTop - iSepPos - 1;

		RECT rcDestTop = { rcDest.left, rcDest.top, rcDest.right, rcDest.top + iSepPos };
		RECT rcSrcTop = { iLeft, iSrcTop, iRight, iSrcTop + iSepPos };
		img.Draw(dc, rcDestTop, rcSrcTop);
		// »­³¤Í¼Æ¬Ê±£¬Òª×ö²¹³¥
		RECT rcDestMid = { rcDest.left, rcDest.top + iSepPos, rcDest.right, rcDest.bottom - iSizeBottom + m_iDrawFixWithSomeDisplayCard };
		RECT rcSrcMid = { iLeft, iSrcTop + iSepPos, iRight, iSrcTop + iSepPos + 1 };
		img.Draw(dc, rcDestMid, rcSrcMid);
		if (iSizeBottom > 0)
		{
			RECT rcDestBottom = { rcDest.left, rcDest.bottom - iSizeBottom, rcDest.right, rcDest.bottom };
			RECT rcSrcBottom = { iLeft, iSrcBottom - iSizeBottom, iRight, iSrcBottom };
			img.Draw(dc, rcDestBottom, rcSrcBottom);
		}
	}
	else
	{
		//RECT rcDest = { rcDest.left, rcDest.top, rcDest.right, rcDest.top + iSepPos };
		RECT rcSrc = { iLeft, iSrcTop, iRight, iSrcTop + rcDest.bottom - rcDest.top };
		img.Draw(dc, rcDest, rcSrc);
	}
}


void CSkinManager::DrawImagePart(CDCHandle dc, const RECT &rcDest, const CTuoImage &img, int iCurrentFrame, int iTotalFrame)
{
	ATLASSERT(img.GetWidth() % iTotalFrame == 0);
	int iWidth = img.GetWidth() / iTotalFrame;
	ATLASSERT(rcDest.right - rcDest.left == iWidth);
	ATLASSERT(rcDest.bottom - rcDest.top == img.GetHeight());
	int iLeft = iWidth * iCurrentFrame;
	RECT rcSrc = { iLeft, 0, iLeft + iWidth, img.GetHeight() };
	img.Draw(dc, rcDest, rcSrc);
}

int CSkinManager::DrawGlowText(CDCHandle hdc, LPCTSTR lpchText, int cchText, LPRECT lprc, UINT format, bool bGlow)
{
	const int iGlowMargin = 10;

	CDC dcMem;
	dcMem.CreateCompatibleDC(hdc);

	CBitmap bmp;
	BITMAPINFO dib = { sizeof(BITMAPINFOHEADER), lprc->right - lprc->left + iGlowMargin + iGlowMargin, lprc->top - lprc->bottom, 1, 32, BI_RGB };
	bmp.CreateDIBSection(hdc, &dib, DIB_RGB_COLORS, NULL, NULL, 0);
	dcMem.SelectBitmap(bmp);

	__DTTOPTS dto = { sizeof(__DTTOPTS) };
	dto.dwFlags = _DTT_TEXTCOLOR | _DTT_COMPOSITED | (bGlow ? _DTT_GLOWSIZE : 0);
	dto.crText = hdc.GetTextColor();
	dto.iGlowSize = 8;

	dcMem.SelectFont(hdc.GetCurrentFont());

	RECT rcText2 = { iGlowMargin, 0, lprc->right - lprc->left + iGlowMargin, lprc->bottom - lprc->top };
	int iRet = ::TuoDrawThemeTextEx(s()->GetTheme(), dcMem, 0, 0, lpchText, cchText, format, &rcText2, &dto);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 0xff;
	bf.AlphaFormat = AC_SRC_ALPHA;
	::AlphaBlend(hdc, lprc->left - iGlowMargin, lprc->top, lprc->right - lprc->left + iGlowMargin + iGlowMargin, lprc->bottom - lprc->top, dcMem, 0, 0, lprc->right - lprc->left + iGlowMargin + iGlowMargin, lprc->bottom - lprc->top, bf);

	return iRet;
}
