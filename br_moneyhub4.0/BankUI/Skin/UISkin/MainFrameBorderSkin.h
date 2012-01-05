#pragma once
#include "SkinProperty.h"
#include "TuoImage.h"

class CSkinManager;

class CMainFrameBorderSkin
{

public:

	CMainFrameBorderSkin(CSkinManager *pSkin);

	void Refresh();

	int GetBorderWidth() const				{ return 6; }
	int GetBorderHeight() const				{ return 15; }
	int GetResizeRange() const				{ return 28; }

	int GetCloseButtonWidth() const			{ return m_imgBtnClose.GetWidth() / 3; }
	int GetMaxRestoreButtonWidth() const	{ return m_imgBtnMax.GetWidth() / 3; }
	int GetMinButtonWidth() const			{ return m_imgBtnMin.GetWidth() / 3; }
	int GetHeight() const					{ return m_imgBtnClose.GetHeight(); }

	void DrawButton(CDCHandle dc, const RECT &rc, int iButtonType, UINT uButtonStatus);
	void DrawBorder(CDCHandle dc, const RECT &rc, bool bDrawBottomBorder, bool bSecure);

	void DrawTitleBar(CDCHandle dc, const RECT &rc, LPCTSTR lpszTitle, bool bActive, bool bIsMax);

public:

	CTuoImage m_imgBtnClose, m_imgBtnRestore, m_imgBtnMin, m_imgBtnMax;
	CTuoImage m_imgBorder;
	CTuoImage m_imgBorder_Secure;
};
