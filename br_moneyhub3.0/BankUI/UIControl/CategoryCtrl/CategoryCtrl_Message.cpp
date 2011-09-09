#include "stdafx.h"
#include "../MainFrame.h"
#include "../MDIClient.h"
#include "../ChildFrm.h"
#include "../FrameBorder/TitleBar.h"
#include "../FloatingTip/ToolTip.h"
#include "CategoryCtrl.h"
#include "CategoryItem.h"


int CTuotuoCategoryCtrl::OnCreate(LPCREATESTRUCT /* lpCreateStruct */)
{
	LRESULT lRes = DefWindowProc();
	ATLASSERT(SUCCEEDED(lRes));

	return lRes;
}

void CTuotuoCategoryCtrl::OnDestroy()
{
}


void CTuotuoCategoryCtrl::OnSize(UINT nType, CSize size)
{
	if (size.cx < 100)
		return;

	int iOffsetTop = size.cy - s()->Category()->GetCategoryHeight();
	if (iOffsetTop < 0)
		iOffsetTop = 0;

	m_rcScrollLLeft.left = 0;
	m_rcScrollLLeft.right = m_rcScrollLLeft.left + s()->Category()->GetCategoryScrollButtonWidth();
	m_rcScrollRRight.right = size.cx;
	m_rcScrollRRight.left = size.cx - s()->Category()->GetCategoryScrollButtonWidth();

	int iYPos = (s()->Category()->GetCategoryHeight() - s()->Category()->GetCategoryScrollButtonHeight()) / 2 + iOffsetTop - 2;
	m_rcScrollLLeft.top = m_rcScrollRRight.top = iYPos;
	m_rcScrollLLeft.bottom = m_rcScrollRRight.bottom = iYPos + s()->Category()->GetCategoryScrollButtonHeight();

	UpdateLayout();
}

//////////////////////////////////////////////////////////////////////////
// mouse messages

void CTuotuoCategoryCtrl::OnLButtonDown(UINT /* nFlags */, CPoint ptCursor)
{
	//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);

	if (::PtInRect(&m_rcTabItemAreaDraw, ptCursor))
	{
		CategoryPosition pos;
		CCategoryItem *pItem = HitTest(ptCursor, &pos);
		if (pItem)
		{
			SetMousePosAndStatus(pos, Btn_MouseDown);
			if (pItem != m_pSelectedItem && pos == CatePos_TabItem)
				FS()->MDI()->ActiveCategory(pItem);
			else if (pItem == m_pSelectedItem)
				EnsureVisible(pItem);
		}
	}
	else if (m_bOverflowLeft && ::PtInRect(&m_rcScrollLLeft, ptCursor))
	{
		SetMousePosAndStatus(CatePos_ScrollLLeftBtn, Btn_MouseDown);
		SetScrollDir(-1);
	}
	else if (m_bOverflowRight && ::PtInRect(&m_rcScrollRRight, ptCursor))
	{
		SetMousePosAndStatus(CatePos_ScrollRRightBtn, Btn_MouseDown);
		SetScrollDir(1);
	}
	else
		SetMousePosAndStatus(CatePos_Unknown, Btn_MouseOut);

	if (m_ePosition != CatePos_Unknown)
		SetCapture();
}

void CTuotuoCategoryCtrl::OnLButtonUp(UINT /* nFlags */, CPoint ptCursor)
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

	if (m_ePosition == CatePos_TabItemClose)
	{
		CategoryPosition pos;
		CCategoryItem *pItem = HitTest(ptCursor, &pos);
		if (pItem == m_pHotItem && pos == CatePos_TabItemClose)
		{
			//m_bClosingTabs = true;
			FS()->MDI()->CloseCategory(pItem);
		}
	}
	else if (m_ePosition == CatePos_ScrollLLeftBtn)
	{
		SetScrollDir(0);
		ScrollToEnsureVisibleAfterScroll(true);
	}
	else if (m_ePosition == CatePos_ScrollRRightBtn)
	{
		SetScrollDir(0);
		ScrollToEnsureVisibleAfterScroll(false);
	}
	SetMousePosAndStatus(m_ePosition, Btn_MouseOut);
}

void CTuotuoCategoryCtrl::OnLButtonDblClk(UINT /* nFlags */, CPoint ptCursor)
{
}

void CTuotuoCategoryCtrl::OnRButtonUp(UINT /* nFlags */, CPoint ptCursor)
{
	CCategoryItem *pItem = HitTest(ptCursor, NULL);
	ClientToScreen(&ptCursor);
}

void CTuotuoCategoryCtrl::OnMButtonUp(UINT /* nFlags */, CPoint ptCursor)
{
	CCategoryItem *pItem = HitTest(ptCursor, NULL);
	if (pItem)
		FS()->MDI()->CloseCategory(pItem);
}

//////////////////////////////////////////////////////////////////////////

void CTuotuoCategoryCtrl::OnMouseMove(UINT /* nFlags */, CPoint ptCursor)
{
	if (!m_bMouseInWindow)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd };
		if (::TrackMouseEvent(&tme))
			m_bMouseInWindow = true;
	}

	// 如果鼠标是按下的时候移动，则显示效果仿照windows标准按钮的方式
	// 即：滑到其他按钮上没反应，滑到当前按钮上显示“按下”的状态
	if (GetCapture() == m_hWnd)
	{
		if (m_ePosition == CatePos_TabItem)
		{
		}
		else if (m_ePosition == CatePos_TabItemClose)
		{
			CategoryPosition pos;
			CCategoryItem *pItem = HitTest(ptCursor, &pos);
			SetMousePosAndStatus(CatePos_TabItemClose, pItem == m_pHotItem && pos == CatePos_TabItemClose ? Btn_MouseDown : Btn_MouseOut);
		}
		else if (m_ePosition == CatePos_ScrollLLeftBtn)
		{
			if (::PtInRect(&m_rcScrollLLeft, ptCursor))
			{
				SetMousePosAndStatus(CatePos_ScrollLLeftBtn, Btn_MouseDown);
				SetScrollDir(-1);
			}
			else
			{
				SetMousePosAndStatus(CatePos_ScrollLLeftBtn, Btn_MouseOut);
				SetScrollDir(0);
			}
		}
		else if (m_ePosition == CatePos_ScrollRRightBtn)
		{
			if (::PtInRect(&m_rcScrollRRight, ptCursor))
			{
				SetMousePosAndStatus(CatePos_ScrollRRightBtn, Btn_MouseDown);
				SetScrollDir(1);
			}
			else
			{
				SetMousePosAndStatus(CatePos_ScrollRRightBtn, Btn_MouseOut);
				SetScrollDir(0);
			}
		}
		return;
	}

	CCategoryItem *pNewHotItem = NULL;
	if (::PtInRect(&m_rcTabItemAreaDraw, ptCursor))
	{
		CategoryPosition pos;
		CCategoryItem *pItem = HitTest(ptCursor, &pos);
		if (pItem)
		{
			SetMousePosAndStatus(pos, Btn_MouseOver);
			pNewHotItem = pItem;
//			CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, (DWORD)(HWND)pNewHotItem->GetTabView());
		}
		else
		{
			SetMousePosAndStatus(CatePos_Unknown, Btn_MouseOut);
//			CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
		}
	}
	else if (m_bOverflowLeft && ::PtInRect(&m_rcScrollLLeft, ptCursor))
	{
		SetMousePosAndStatus(CatePos_ScrollLLeftBtn, Btn_MouseOver);
		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}
	else if (m_bOverflowRight && ::PtInRect(&m_rcScrollRRight, ptCursor))
	{
		SetMousePosAndStatus(CatePos_ScrollRRightBtn, Btn_MouseOver);
		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}
	else
	{
		SetMousePosAndStatus(CatePos_Unknown, Btn_MouseOut);
		//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}

	if (pNewHotItem != m_pHotItem)
	{
		m_pHotItem = pNewHotItem;
		InvalidateRect(&m_rcTabItemAreaDraw);
	}
}

void CTuotuoCategoryCtrl::OnMouseLeave()
{
	m_bMouseInWindow = false;
	if (m_pHotItem != NULL)
	{
		m_pHotItem = NULL;
		InvalidateRect(&m_rcTabItemAreaDraw);
	}
	SetMousePosAndStatus(CatePos_Unknown, Btn_MouseOut);
	//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
}

BOOL CTuotuoCategoryCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (!m_bOverflowLeft && !m_bOverflowRight)
		return TRUE;

	int iNewOffset = GetValidScrollPos(m_iScrollOffset + zDelta, m_bOverflowLeft, m_bOverflowRight);
	SetScrollOffsetImmediately(iNewOffset);
	//CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

void CTuotuoCategoryCtrl::OnCaptureChanged(CWindow /* wnd */)
{
}


LRESULT CTuotuoCategoryCtrl::OnGetToolTipInfo(LPNMHDR pnmh)
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
				if (pChild->GetItem()->m_pCategory->m_pWebData->IsNoSubTab() == true)
					pnmdsp->szText = pChild->GetItem()->GetText();
				else if (pChild->GetItem()->m_pCategory->m_pWebData)
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
