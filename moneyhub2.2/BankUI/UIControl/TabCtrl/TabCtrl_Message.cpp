#include "stdafx.h"
#include "../MainFrame.h"
#include "../MDIClient.h"
#include "../ChildFrm.h"
#include "../FrameBorder/TitleBar.h"
#include "../FloatingTip/ToolTip.h"
#include "../CategoryCtrl/CategoryItem.h"
#include "TabCtrl.h"
#include "TabItem.h"


int CTuotuoTabCtrl::OnCreate(LPCREATESTRUCT /* lpCreateStruct */)
{
	LRESULT lRes = DefWindowProc();
	ATLASSERT(SUCCEEDED(lRes));
	RegisterDragDrop();

	return lRes;
}

void CTuotuoTabCtrl::OnDestroy()
{
}


void CTuotuoTabCtrl::OnSize(UINT nType, CSize size)
{
	if (size.cx < 100)
		return;

	int iOffsetTop = size.cy - s()->Tab()->GetTabHeight();
	if (iOffsetTop < 0)
		iOffsetTop = 0;

	m_rcScrollLLeft.left = 0;
	m_rcScrollLLeft.right = m_rcScrollLLeft.left + s()->Tab()->GetTabScrollButtonWidth();
	m_rcScrollRRight.right = size.cx;
	m_rcScrollRRight.left = size.cx - s()->Tab()->GetTabScrollButtonWidth();

	int iYPos = (s()->Tab()->GetTabHeight() - s()->Tab()->GetTabScrollButtonHeight()) / 2 + iOffsetTop - 2;
	m_rcScrollLLeft.top = m_rcScrollRRight.top = iYPos;
	m_rcScrollLLeft.bottom = m_rcScrollRRight.bottom = iYPos + s()->Tab()->GetTabScrollButtonHeight();

	UpdateLayout();
}

//////////////////////////////////////////////////////////////////////////
// mouse messages

void CTuotuoTabCtrl::OnLButtonDown(UINT /* nFlags */, CPoint ptCursor)
{
	//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);

	if (::PtInRect(&m_rcTabItemAreaDraw, ptCursor))
	{
		Position pos;
		CTabItem *pItem = HitTest(ptCursor, &pos);
		if (pItem)
		{
			SetMousePosAndStatus(pos, Btn_MouseDown);
			m_ptDragOrigin = ptCursor;
			if (pItem != m_pSelectedItem && pos == POS_TabItem)
				FS()->MDI()->ActivePage(pItem);
			else if (pItem == m_pSelectedItem)
				EnsureVisible(pItem);
		}
	}
	else if (m_bOverflowLeft && ::PtInRect(&m_rcScrollLLeft, ptCursor))
	{
		SetMousePosAndStatus(POS_ScrollLLeftBtn, Btn_MouseDown);
		SetScrollDir(-1);
	}
	else if (m_bOverflowRight && ::PtInRect(&m_rcScrollRRight, ptCursor))
	{
		SetMousePosAndStatus(POS_ScrollRRightBtn, Btn_MouseDown);
		SetScrollDir(1);
	}
	else
		SetMousePosAndStatus(POS_Unknown, Btn_MouseOut);

	if (m_ePosition != POS_Unknown)
		SetCapture();
}

void CTuotuoTabCtrl::OnLButtonUp(UINT /* nFlags */, CPoint ptCursor)
{
	if (::GetCapture() != m_hWnd)
		return;
	::ReleaseCapture();

	// 检查鼠标是否已经移出了tab栏
	RECT rcClient;
	GetClientRect(&rcClient);
	if (!::PtInRect(&rcClient, ptCursor))
	{
		OnMouseLeave();
		return;
	}

	if (m_ePosition == POS_TabItemClose)
	{
		Position pos;
		CTabItem *pItem = HitTest(ptCursor, &pos);
		if (pItem == m_pHotItem && pos == POS_TabItemClose)
		{
			m_bClosingTabs = true;
			FS()->MDI()->ClosePage(pItem);
		}
	}
	else if (m_ePosition == POS_ScrollLLeftBtn)
	{
		SetScrollDir(0);
		ScrollToEnsureVisibleAfterScroll(true);
	}
	else if (m_ePosition == POS_ScrollRRightBtn)
	{
		SetScrollDir(0);
		ScrollToEnsureVisibleAfterScroll(false);
	}
	SetMousePosAndStatus(m_ePosition, Btn_MouseOut);
}

void CTuotuoTabCtrl::OnLButtonDblClk(UINT /* nFlags */, CPoint ptCursor)
{
}

void CTuotuoTabCtrl::OnRButtonUp(UINT /* nFlags */, CPoint ptCursor)
{
	CTabItem *pItem = HitTest(ptCursor, NULL);
	ClientToScreen(&ptCursor);
//	FS()->MDI()->ShowContextMenu(pItem, ptCursor);
}

void CTuotuoTabCtrl::OnMButtonUp(UINT /* nFlags */, CPoint ptCursor)
{
	CTabItem *pItem = HitTest(ptCursor, NULL);
	if (pItem)
		FS()->MDI()->ClosePage(pItem);
}

//////////////////////////////////////////////////////////////////////////

void CTuotuoTabCtrl::OnMouseMove(UINT /* nFlags */, CPoint ptCursor)
{
	if (!m_bMouseInWindow)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd };
		if (::TrackMouseEvent(&tme))
			m_bMouseInWindow = true;
	}

	// 拖动过程中，由ole的drag负责响应鼠标消息，我们这里不再响应
	if (m_nDragToPos != -2)
		return;

	// 如果鼠标是按下的时候移动，则显示效果仿照windows标准按钮的方式
	// 即：滑到其他按钮上没反应，滑到当前按钮上显示“按下”的状态
	if (GetCapture() == m_hWnd)
	{
		if (m_ePosition == POS_TabItem)
		{
			if ((abs(m_ptDragOrigin.x - ptCursor.x) > BEGIN_DRAGGING_OFFSET || abs(m_ptDragOrigin.y - ptCursor.y) > BEGIN_DRAGGING_OFFSET))
			{
				//::ReleaseCapture();
				//DraggingItem(ptCursor.x, ptCursor.y);
			}
		}
		else if (m_ePosition == POS_TabItemClose)
		{
			Position pos;
			CTabItem *pItem = HitTest(ptCursor, &pos);
			SetMousePosAndStatus(POS_TabItemClose, pItem == m_pHotItem && pos == POS_TabItemClose ? Btn_MouseDown : Btn_MouseOut);
		}
		else if (m_ePosition == POS_ScrollLLeftBtn)
		{
			if (::PtInRect(&m_rcScrollLLeft, ptCursor))
			{
				SetMousePosAndStatus(POS_ScrollLLeftBtn, Btn_MouseDown);
				SetScrollDir(-1);
			}
			else
			{
				SetMousePosAndStatus(POS_ScrollLLeftBtn, Btn_MouseOut);
				SetScrollDir(0);
			}
		}
		else if (m_ePosition == POS_ScrollRRightBtn)
		{
			if (::PtInRect(&m_rcScrollRRight, ptCursor))
			{
				SetMousePosAndStatus(POS_ScrollRRightBtn, Btn_MouseDown);
				SetScrollDir(1);
			}
			else
			{
				SetMousePosAndStatus(POS_ScrollRRightBtn, Btn_MouseOut);
				SetScrollDir(0);
			}
		}
		return;
	}

	// hit test
	CTabItem *pNewHotItem = NULL;
	if (::PtInRect(&m_rcTabItemAreaDraw, ptCursor))
	{
		Position pos;
		CTabItem *pItem = HitTest(ptCursor, &pos);
		if (pItem)
		{
			SetMousePosAndStatus(pos, Btn_MouseOver);
			pNewHotItem = pItem;
			//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, (DWORD)(HWND)pNewHotItem->GetTabView());
		}
		else
		{
			SetMousePosAndStatus(POS_Unknown, Btn_MouseOut);
			//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
		}
	}
	else if (m_bOverflowLeft && ::PtInRect(&m_rcScrollLLeft, ptCursor))
	{
		SetMousePosAndStatus(POS_ScrollLLeftBtn, Btn_MouseOver);
		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}
	else if (m_bOverflowRight && ::PtInRect(&m_rcScrollRRight, ptCursor))
	{
		SetMousePosAndStatus(POS_ScrollRRightBtn, Btn_MouseOver);
		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}
	else
	{
		SetMousePosAndStatus(POS_Unknown, Btn_MouseOut);
		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}

	if (pNewHotItem != m_pHotItem)
	{
		m_pHotItem = pNewHotItem;
		InvalidateRect(&m_rcTabItemAreaDraw);
	}
}

void CTuotuoTabCtrl::OnMouseLeave()
{
	if (m_bClosingTabs)
	{
		m_bClosingTabs = false;
		UpdateLayout();
	}
	m_bMouseInWindow = false;
	if (m_pHotItem != NULL)
	{
		m_pHotItem = NULL;
		InvalidateRect(&m_rcTabItemAreaDraw);
	}
	SetMousePosAndStatus(POS_Unknown, Btn_MouseOut);
	//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
}

BOOL CTuotuoTabCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (!m_bOverflowLeft && !m_bOverflowRight)
		return TRUE;

	int iNewOffset = GetValidScrollPos(m_iScrollOffset + zDelta, m_bOverflowLeft, m_bOverflowRight);
	//SetScrollOffsetAnimation(iNewOffset);
	SetScrollOffsetImmediately(iNewOffset);
	//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

void CTuotuoTabCtrl::OnCaptureChanged(CWindow /* wnd */)
{
//	SetMousePosAndStatus(POS_Unknown, Btn_MouseOut);
//	if (IS_DRAGGING())
//		DragItemCancel();
}


LRESULT CTuotuoTabCtrl::OnGetToolTipInfo(LPNMHDR pnmh)
{
	LPNMSTTDISPINFO pnmdsp = (LPNMSTTDISPINFO)pnmh;
	pnmdsp->dwFlags = STTDSPF_TEXT;
	if (pnmdsp->pdwIndex >= 1 && pnmdsp->pdwIndex <= 5)
		pnmdsp->szText = _T("");
	else if (pnmdsp->pdwIndex)
	{
		HWND hChildFrame = (HWND)pnmdsp->pdwIndex;
		if (::IsWindow(hChildFrame))
		{
			CChildFrame *pChild = (CChildFrame*)::GetWindowLongPtr(hChildFrame, GWLP_USERDATA);
			if (pChild)
			{
				if (pChild->GetItem()->m_pCategory->GetWebData() == START_PAGE_WEBSITE_DATA)
					pnmdsp->szText = _T("我的首页");
				else if (pChild->GetItem()->m_pCategory->GetWebData())
					pnmdsp->szText = pChild->GetItem()->GetText();
				else
					pnmdsp->szText = _T("");
			}
			else
				pnmdsp->szText = _T("");
		}
		else
			pnmdsp->szText = _T("");
	}
	else
		pnmdsp->szText = _T("");

	return 0;
}

LRESULT CTuotuoTabCtrl::OnOleDragOverTab(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	ScreenToClient(&pt);
	DragItemMoving(pt);
	return 0;
}

LRESULT CTuotuoTabCtrl::OnOleSetDraggingItem(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DragGlobalData *pData = (DragGlobalData*)wParam;
	if (pData->pDraggingItem == NULL)
	{
		DragItemOut();
		Invalidate();
	}
	else
		DragItemEnter(pData->pDraggingItem);
	return 0;
}

LRESULT CTuotuoTabCtrl::OnOleSetDragTo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DragGlobalData *pData = (DragGlobalData*)wParam;
	DragItemEnd_Dest();
	return 0;
}
