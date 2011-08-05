#pragma once
#include "../../Util/ThreadStorageManager.h"
#include "../../Skin/UISkin/TabSkin.h"
#include "../FrameBorder/SizingBorder.h"

enum CategoryPosition
{
	CatePos_Unknown,
	CatePos_ScrollLLeftBtn,
	CatePos_ScrollRRightBtn,
	CatePos_TabItem,
	CatePos_TabItemClose,
};

enum CategoryInvalidateType
{
	CateInv_Tab = 0,
	CateInv_Icon = 1,
	CateInv_Text = 2,
	CateInv_WidthChange = 4,
	CateInv_PosChange = 8,
	CateInv_Scroll = 16
};

class CCategoryItem;

#define CATEGORY_TEXT_MAX_WIDTH				135

#define CATEGORY_IMAGE_TEXT_MARGIN			5
#define CATEGORY_TEXT_CLOSE_MARGIN			2

#define CATEGORY_SCROLL_BEGIN_INTERVAL		100
#define CATEGORY_SCROLL_INTERVAL			10

#define CATEGORY_SCROLL_DELTA				16

#define CATEGORY_MATCH_INTERVAL(a, b, c)	((a) <= (b) && (b) < (c))

typedef std::vector<CCategoryItem*> CategoryItemVector;

class CTuotuoCategoryCtrl : public CWindowImpl<CTuotuoCategoryCtrl>, public COffscreenDrawRect<CTuotuoCategoryCtrl>, public CFSMUtil
{

public:

	CTuotuoCategoryCtrl(FrameStorageStruct *pFS);

	// export function

	void DoPaint(CDCHandle dc, const RECT &rcClient) const;
	CCategoryItem* GetCurrentSelection() const { return m_pSelectedItem; }

	// tab item operations
	CCategoryItem* FindOrCreateItem(LPCTSTR lpszURL, CTabItem *pItem, bool bForceCreate = false,int iBillFlag = 0);
	CCategoryItem* FindExistItem(LPCTSTR lpszURL, int iBillFlag = 0);

	void InsertItem(size_t nIndex, CCategoryItem *pItem);
	void DeleteItem(int iIndex);		// 返回删除之后将要选中的那个
	void DeleteItemExcept(CCategoryItem *pItem);
	void SelectItem(CCategoryItem *pItem);
	void ToggleItem();

	const CategoryItemVector& GetTabItems() const { return m_CateItems; }

	void InvalidateTab(CCategoryItem *pItem, CategoryInvalidateType eType);
	void InvalidateTabs(CategoryInvalidateType eType);

	CCategoryItem* GetLastActiveItem(void);// gao 得到用户最近浏览的TAB项指针

	//CCategoryItem* GetLeftOrRight(CCategoryItem *pItem, bool bLeft, bool bRoll);
private:
	CCategoryItem* HitTest(const POINT &pt, CategoryPosition *pos) const;
	void EnsureVisible(CCategoryItem *pItem);

	void SetMousePosAndStatus(CategoryPosition ePos, ButtonStatus btnStatus);

	// TabCtrl_Size : layout management

	void UpdateLayout();
	void ScrollToEnsureVisibleAfterScroll(bool bLeft);

	void SetScrollOffsetImmediately(int iOffset);
	//void SetScrollOffsetAnimation(int iOffset);

	void SetScrollDir(int iDir);
	int GetValidScrollPos(int iOriginalScrollPos, bool &bOverflowLeft, bool &bOverflowRight);

	void AddATabVisitIndex(CCategoryItem* pItem); // gao 记录用户最近浏览项指针
	//int GetLastActiveTabIndex(void); // gao 得到用户最近浏览的索引

	// message

	BEGIN_MSG_MAP_EX(CTuotuoCategoryCtrl)
		CHAIN_MSG_MAP(COffscreenDrawRect<CTuotuoCategoryCtrl>)

		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		MSG_WM_MBUTTONUP(OnMButtonUp)
		MSG_WM_CAPTURECHANGED(OnCaptureChanged)

		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnGetToolTipInfo)
	END_MSG_MAP()

	// message handler

	int OnCreate(LPCREATESTRUCT /* lpCreateStruct */);
	void OnDestroy();
	void OnSize(UINT /* nType */, CSize /* size */);

	// mouse messages

	void OnLButtonDown(UINT /* nFlags */, CPoint ptCursor);
	void OnLButtonUp(UINT /* nFlags */, CPoint ptCursor);
	void OnLButtonDblClk(UINT /* nFlags */, CPoint ptCursor);
	void OnRButtonUp(UINT /* nFlags */, CPoint ptCursor);
	void OnMButtonUp(UINT /* nFlags */, CPoint ptCursor);

	void OnMouseMove(UINT /* nFlags */, CPoint ptCursor);
	void OnMouseLeave();
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void OnCaptureChanged(CWindow /* wnd */);

	LRESULT OnGetToolTipInfo(LPNMHDR pnmh);

public:
	bool IsSecure();

private:
	std::list<CCategoryItem*> m_nListVistorIndex; // gao 记录用户浏览项指针

public:

	CWindow m_wndMDIClient;

	bool m_bMouseInWindow;		// arrow在TabCtrl窗口内

	CategoryItemVector m_CateItems;
	CCategoryItem *m_pSelectedItem, *m_pHotItem;

	// position and size
	RECT m_rcTabItemArea, m_rcTabItemAreaDraw;
	RECT m_rcScrollLLeft;			// the "<-" button on the left of the tab bar
	RECT m_rcScrollRRight;			// the "->" button on the right of the tab bar
	RECT *m_pRCIndex[12];
	ButtonStatus m_eButtonStatus;
	CategoryPosition m_ePosition;

	short m_iCurrentWidth;		// TabItem的宽度

	// scroll data
	bool m_bOverflowLeft, m_bOverflowRight;		// tab bar overflow direction
	int m_iScrollingDir;		// scrolling direction, -1 = left, 0 = no scroll, 1 = right
	int m_iScrollOffset;
	int m_iScrollOffsetAni;
	int m_iScrollOffsetAniFrame;
	int m_iScrollOffsetOld;

	// closing tab
	//bool m_bClosingTabs;	// the user is CLOSING tabs, if this value is true, we DO NOT readjust the size of each tab
	//int m_iLastLayout;
	bool m_bShowScrollBtn;

	enum ButtonToolTipIDs
	{
		ToolTip_ScrollLLeft = 1,
		ToolTip_ScrollRLeft = 2,
		ToolTip_ScrollRRight = 3,
		ToolTip_Sidebar = 4,
		ToolTip_NewTab = 5,
	};

	enum TimerIDs
	{
		Timer_Scroll = 1,
		Timer_ScrollBegin = 2,
		Timer_Update = 3
	};

	enum TabCloseButton
	{
		CloseBtn_AlwaysDisplay = 0,
		CloseBtn_ActiveTabDisplay,
		CloseBtn_AlwaysHide
	};
	TabCloseButton m_eTabCloseButton;

	friend class CCategoryItem;

	DECLARE_WND_CLASS_EX(_T("MoneyhubCategoryCtrl"), CS_DBLCLKS, COLOR_WINDOW)
};
