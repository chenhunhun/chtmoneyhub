#pragma once
#include "../../Util/ThreadStorageManager.h"
#include "../../Skin/UISkin/TabSkin.h"
#include "../FrameBorder/SizingBorder.h"

enum Position
{
	POS_Unknown,
	POS_ScrollLLeftBtn,
	POS_ScrollRRightBtn,
	POS_TabItem,
	POS_TabItemClose,
};

enum TabInvalidateType
{
	TabInv_Tab = 0,
	TabInv_Icon = 1,
	TabInv_Text = 2,
	TabInv_WidthChange = 4,
	TabInv_PosChange = 8,
	TabInv_Scroll = 16
};

#define WM_TAB_OLE_DRAG_OVER			(WM_USER + 0xcccc)
#define WM_TAB_OLE_SET_DRAGGING_ITEM	(WM_USER + 0xcccd)
#define WM_TAB_OLE_SET_DRAG_TO			(WM_USER + 0xccce)


class CTabItem;
class CTabDropTarget;

typedef struct tagNMCTCITEM
{
	NMHDR hdr;
	CTabItem *pItem;	// Item
	POINT pt;			// Client Coordinates
	DWORD_PTR dwParam;
} NMCTCITEM, *LPNMCTCITEM;

struct DragGlobalData
{
	CTabItem *pDraggingItem;
	HWND *hDragToFrame;
	DWORD dwProcessID;
};

#define TAB_TEXT_MAX_WIDTH				130


#define IMAGE_TEXT_MARGIN				5
#define TEXT_CLOSE_MARGIN				2


#define TAB_SCROLL_BEGIN_INTERVAL		100
#define TAB_SCROLL_INTERVAL				10
#define UPDATE_INTERVAL					5

#define BEGIN_DRAGGING_OFFSET			5

#define SCROLL_DELTA_DRAGGING			4
#define TAB_SCROLL_DELTA				16

#define TAB_MATCH_INTERVAL(a, b, c)			((a) <= (b) && (b) < (c))


__declspec(selectany) int g_iMotionPath[] = { 210, 395, 555, 691, 802, 889, 950, 988, 1000 };
#define FRAME_COUNT		(_countof(g_iMotionPath))



typedef std::vector<CTabItem*> TabItemVector;



class CTuotuoTabCtrl : public CWindowImpl<CTuotuoTabCtrl>, public COffscreenDrawRect<CTuotuoTabCtrl>, public CSizingBorder<CTuotuoTabCtrl>, public CFSMUtil
{

public:

	CTuotuoTabCtrl(FrameStorageStruct *pFS);

	// export function

	void UpdateDisplayResourceIcon();
	void DoPaint(CDCHandle dc, const RECT &rcClient) const;
	CTabItem* GetCurrentSelection() const { return m_pSelectedItem; }

	// 选择指定tab右边或左边的，bRoll=true表示如果到头了就从另外一端开始
	CTabItem* GetLeftOrRight(CTabItem *pItem, bool bLeft, bool bRoll);

	void OnSettingUpdate();

	//void IncOrDecAnimationCounter(bool bAddRef, CTabItem *pItem, TabInvalidateType eType);
	int GetDraggingItemIndex() const { return m_iDraggingItemIndex; }

	// tab item operations

	void InsertItem(size_t nIndex, CTabItem *pItem);
	void DeleteItem(int iIndex);
	void DeleteAllItems();
	void SelectItem(CTabItem *pItem);

	const TabItemVector& GetTabItems() const { return m_TabItems; }

	void NotifyAndGetTabIconPos(CTabItem *pItem, RECT &rc);		// 将指定tab显示出来，并返回其icon中心的位置

	void InvalidateTab(CTabItem *pItem, TabInvalidateType eType);
	void InvalidateTabs(TabInvalidateType eType);

private:

	CTabItem* HitTest(const POINT &pt, Position *pos) const;
	void EnsureVisible(CTabItem *pItem);

	void SetMousePosAndStatus(Position ePos, ButtonStatus btnStatus);

	// TabCtrl_Size : layout management

	void UpdateLayout();
	void ScrollToEnsureVisibleAfterScroll(bool bLeft);

	void SetScrollOffsetImmediately(int iOffset);
	//void SetScrollOffsetAnimation(int iOffset);

	void SetScrollDir(int iDir);
	int GetValidScrollPos(int iOriginalScrollPos, bool &bOverflowLeft, bool &bOverflowRight);

	// TabCtrl_Drag : drag operations

	void RegisterDragDrop();
	void DraggingItem(int iMouseX, int iMouseY);
	void DragItemEnter(CTabItem *pItem);
	void DragItemOut();
	void DragItemMoving(POINT ptCursor);
	void DragItemEnd_Source(HWND hTargetFrame);
	void DragItemEnd_Dest();		// 返回值为拖到哪儿了
	void DragItemCancel();

	size_t GetItemCountInDraggingMode() const;		// 在拖动过程中获取tab的个数

	// message

	BEGIN_MSG_MAP_EX(CTuotuoTabCtrl)
		CHAIN_MSG_MAP(COffscreenDrawRect<CTuotuoTabCtrl>)
		CHAIN_MSG_MAP(CSizingBorder<CTuotuoTabCtrl>)

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
		//MSG_WM_TIMER(OnTimer)
		MSG_WM_CAPTURECHANGED(OnCaptureChanged)

		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnGetToolTipInfo)

		MESSAGE_HANDLER_EX(WM_TAB_OLE_DRAG_OVER, OnOleDragOverTab)
		MESSAGE_HANDLER_EX(WM_TAB_OLE_SET_DRAGGING_ITEM, OnOleSetDraggingItem)
		MESSAGE_HANDLER_EX(WM_TAB_OLE_SET_DRAG_TO, OnOleSetDragTo)
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

	//void OnTimer(UINT_PTR nIDEvent);

	void OnCaptureChanged(CWindow /* wnd */);

	LRESULT OnGetToolTipInfo(LPNMHDR pnmh);

	LRESULT OnOleDragOverTab(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnOleSetDraggingItem(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnOleSetDragTo(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

	CWindow m_wndMDIClient;

	bool m_bMouseInWindow;
	bool m_bDisplayResourceIcon;

	TabItemVector m_TabItems;
	CTabItem *m_pSelectedItem, *m_pHotItem;

	// dragging data
	size_t m_nDragToPos;
	POINT m_ptDragOrigin;
	int m_iBeginDragItemOffset;
	CTabItem *m_pDraggingItem;
	int m_iDraggingItemIndex;		// 用来保存拖动的那个item的index，因为拖动完之后这个index就不可读了
	bool m_bDraggingSource;			// 拖动过程中，此tab是否是原先将标签拖出去的那个tab
	CTabDropTarget *m_pDropTarget;

	// position and size
	RECT m_rcTabItemArea, m_rcTabItemAreaDraw;
	RECT m_rcScrollLLeft;			// the "<-" button on the left of the tab bar
	RECT m_rcScrollRRight;			// the "->" button on the right of the tab bar
	RECT *m_pRCIndex[12];
	ButtonStatus m_eButtonStatus;
	Position m_ePosition;

	short m_iCurrentWidth;

	// scroll data
	bool m_bOverflowLeft, m_bOverflowRight;		// tab bar overflow direction
	int m_iScrollingDir;		// scrolling direction, -1 = left, 0 = no scroll, 1 = right
	int m_iScrollOffset;
	int m_iScrollOffsetAni;
	int m_iScrollOffsetAniFrame;
	int m_iScrollOffsetOld;

	// closing tab
	bool m_bClosingTabs;	// the user is CLOSING tabs, if this value is true, we DO NOT readjust the size of each tab
	int m_iLastLayout;

	typedef std::map<CTabItem*, int> AnimationMap;
	AnimationMap m_AniSet;

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

	friend class CTabItem;

	DECLARE_WND_CLASS_EX(_T("MH_TuotuoTabCtrl"), CS_DBLCLKS, COLOR_WINDOW)
};
