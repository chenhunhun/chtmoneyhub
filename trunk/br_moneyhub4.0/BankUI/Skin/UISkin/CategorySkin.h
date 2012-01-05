#pragma once
#include "../SkinProperty.h"
#include "../TuoImage.h"

class CSkinManager;

class CCategorySkin
{

public:

	CCategorySkin(CSkinManager *pSkin);

	void Refresh();

	int GetCategoryHeight() const				{ return m_bmpTabItem.GetHeight(); }
	int GetCategoryScrollButtonWidth() const	{ return m_bmpTabScrollLeft.GetWidth() / 4; }
	int GetCategoryScrollButtonHeight() const 	{ return m_bmpTabScrollLeft.GetHeight(); }
	int GetCategoryCloseButtonWidth() const		{ return m_bmpTabClose.GetWidth() / 3; }
	int GetCategoryCloseButtonHeight() const	{ return m_bmpTabClose.GetHeight(); }

	int GetCategoryLeftOverlap() const			{ return CSkinProperty::GetDWORD(Skin_Property_Tabbar_Overlap_Left); }
	int GetCategoryRightOverlap() const			{ return CSkinProperty::GetDWORD(Skin_Property_Tabbar_Overlap_Right); }

	int GetCategoryMargin() const				{ return 5; }

	void DrawCategory(CDCHandle dc, const RECT &rcTab, ButtonStatus eStatus);
	// iType = 0 : left scroll, 1 : right scroll
	void DrawCategoryScrollButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus, int iType);
	void DrawCategoryCloseButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus);

public:

	CTuoImage m_bmpTabBackground;
	CTuoImage m_bmpTabItem;
	CTuoImage m_bmpTabScrollLeft, m_bmpTabScrollRight;

	CTuoImage m_bmpTabClose;
	CTuoImage m_bmpSidebar, m_bmpNewTab;
	CTuoImage m_bmpTabLoading;

	CTuoImage m_bmpResIcon;

	CTuoImage m_imgTabAdditonalIcon;

	int m_iTabLoadingFrame;

};
