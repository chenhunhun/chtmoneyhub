#pragma once
#include "../SkinProperty.h"
#include "../TuoImage.h"

class CSkinManager;

class CTabSkin
{

public:

	CTabSkin(CSkinManager *pSkin);

	void Refresh();

	int GetTabHeight() const				{ return m_bmpTabItem.GetHeight(); }
	int GetTabScrollButtonWidth() const		{ return m_bmpTabScrollLeft.GetWidth() / 4; }
	int GetTabScrollButtonHeight() const 	{ return m_bmpTabScrollLeft.GetHeight(); }
	int GetTabCloseButtonWidth() const		{ return m_bmpTabClose.GetWidth() / 3; }
	int GetTabCloseButtonHeight() const		{ return m_bmpTabClose.GetHeight(); }

	int GetTabLeftOverlap() const			{ return CSkinProperty::GetDWORD(Skin_Property_Tabbar_Overlap_Left); }
	int GetTabRightOverlap() const			{ return CSkinProperty::GetDWORD(Skin_Property_Tabbar_Overlap_Right); }

	int GetTabMargin() const				{ return 5; }

	void DrawTab(CDCHandle dc, const RECT &rcTab, ButtonStatus eStatus, int iCategory);
	// iType = 0 : left scroll, 1 : right scroll
	void DrawTabScrollButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus, int iType);
	void DrawTabCloseButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus);

	void DrawSidebarButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus, bool bIsOpen);
	void DrawNewTabButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus);

	void DrawResourceButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus, int iResType);

	int DrawTabLoadingIcon(CDCHandle dc, int left, int top, int &iProgress);

	void DrawTabAdditionalIcon(CDCHandle dc, int left, int top, int iIconIndex);

	void DrawTabMiniLockButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus);
	void DrawTabMiniPageButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus);

public:

	CTuoImage m_bmpTabBackground;
	CTuoImage m_bmpTabItem;
	CTuoImage m_bmpTabScrollLeft, m_bmpTabScrollRight;

	CTuoImage m_bmpTabClose;
	CTuoImage m_bmpSidebar, m_bmpNewTab;
	CTuoImage m_bmpTabLoading;

	CTuoImage m_bmpResIcon;
	CTuoImage m_bmpMiniLock;
	CTuoImage m_bmpMiniPage;
	CTuoImage m_imgTabAdditonalIcon;

	int m_iTabLoadingFrame;

};
