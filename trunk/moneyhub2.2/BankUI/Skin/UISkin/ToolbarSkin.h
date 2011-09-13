#pragma once
#include "TuoImage.h"


struct RebarDrawData
{
	int iLastWidth, iLastHeight;
	int iFirstLineHeight;		// 这个变量主要是给tab在最顶上的时候使用，平常一般都是0
	bool bLastIsMax;
	bool bLastIsSecure;

	CDC bufferDC;
	CBitmap bufferBitmap;

	RebarDrawData() : iLastHeight(0), iLastWidth(0), bLastIsMax(false), iFirstLineHeight(0)
	{
		bufferDC.CreateCompatibleDC(NULL);
	}

	void Reset()
	{
		iLastHeight = iLastWidth = 0;
		bLastIsMax = false;
		bLastIsSecure = false;
		if (bufferBitmap)
			bufferBitmap.DeleteObject();
	}
};


const extern int g_iButtonFrame[];

#define REBAR_CHEVRON_FRAME			3

class CSkinManager;

class CToolbarSkin
{

public:

	CToolbarSkin(CSkinManager *pSkin);

	void Refresh();

	int GetBigButtonWidth() const			{ return 50; }

	int GetToolbarHeight() const			{ return max(m_bitmap[0].GetHeight(), max(m_bitmap[1].GetHeight(), m_bitmap[2].GetHeight())); }
	int GetButtonWidth(UINT uID) const		{ return m_bitmap[uID].GetWidth() / g_iButtonFrame[uID]; }
	int GetButtonHeight(UINT uID) const		{ return m_bitmap[uID].GetHeight(); }
	int GetDropDownArrowWidth() const		{ return CSkinProperty::GetDWORD(Skin_Property_Toolbar_UndoButton_DropdownArrow_Width); }

	void DrawButton(CDCHandle dc, const RECT &rect, UINT uID, UINT uButtonStatus) const;
	void DrawToolBarSep(CDCHandle dc, const RECT &rect) const;

	void DrawRebarBack(CDCHandle dc, const RECT &rect, bool bIsMax, bool bSecure);
	void DrawRebarBackPart(CDCHandle dc, const RECT &rect, HWND hWnd);

	void DrawCheckDlgBack(CDCHandle dc, const RECT &rect);
	void DrawAuthenDlgBack(CDCHandle dc, const RECT &rect);
	void DrawAuthenDlgCloseBtn(CDCHandle dc, const RECT &rect, UINT iCurrentFrame); 
	void DrawAuthenDlgButton(CDCHandle dc, const RECT &rect, UINT iCurrentFrame, LPCTSTR lpszName);

	void DrawSettingDlgBack(CDCHandle dc, const RECT &rect);
	void DrawMessageBoxBack(CDCHandle dc, const RECT &rect);
	void DrawMessageBoxIcon(CDCHandle dc, const RECT &rect, UINT iCurr);

private:

	CTuoImage m_bitmap[3];

	CTuoImage m_bmpRebarBack;
	CTuoImage m_bmpRebarBack_Secure;
	RebarDrawData m_rebarData;

	CTuoImage m_bmpCheckDlgBack;

public:
	CTuoImage m_bmpAuthenDlgBack;
	CTuoImage m_bmpDlgCloseBtn;
	CTuoImage m_bmpDlgButtonBack;
	CTuoImage m_bmpListTabCtrlBack;
	CTuoImage m_bmpListTabCtrlItems;

	CTuoImage m_bmpSettingDlgBack;
	CTuoImage m_bmpMessageBoxBack;
	CTuoImage m_bmpMessageBoxIcons;
};
