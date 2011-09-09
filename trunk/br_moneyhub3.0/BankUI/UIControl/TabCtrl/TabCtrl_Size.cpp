#include "stdafx.h"
#include "../MainFrame.h"
#include "TabItem.h"
#include "TabCtrl.h"


void CTuotuoTabCtrl::UpdateLayout()
{
	int iTabCount = GetItemCountInDraggingMode();

	GetClientRect(&m_rcTabItemArea);
	// 在全屏状态下，tab的高度要填充整个窗口，以便接受鼠标事件
	if ((FS()->MainFrame()->GetStyle() & WS_MAXIMIZE) == 0)
		m_rcTabItemArea.top = m_rcTabItemArea.bottom - s()->Tab()->GetTabHeight();
	m_rcTabItemAreaDraw = m_rcTabItemArea;
	if (iTabCount == 0 || m_rcTabItemArea.right <= m_rcTabItemArea.left)
	{
		m_rcTabItemArea.left = m_rcTabItemArea.right = 0;
		m_rcTabItemAreaDraw = m_rcTabItemArea;
		Invalidate();
		return;
	}

	m_rcTabItemArea.left += 0 + s()->Tab()->GetTabLeftOverlap();

/*
	if (m_bClosingTabs)
	{
		switch (m_iLastLayout)
		{
			case 0:
			case 1:
				break;
			case 2:
				m_rcTabItemArea.left += s()->Tab()->GetTabScrollButtonWidth();
				m_rcTabItemArea.right -= s()->Tab()->GetTabScrollButtonWidth();
				break;
		}
		m_rcTabItemArea.right += s()->Tab()->GetTabRightOverlap();
		m_rcTabItemAreaDraw.left = m_rcTabItemArea.left - s()->Tab()->GetTabLeftOverlap();
		m_rcTabItemAreaDraw.right = m_rcTabItemArea.right + s()->Tab()->GetTabRightOverlap();

		if (m_iScrollOffset >= 0)
			m_bOverflowLeft = false;
		else if (m_iScrollOffset < 0)
			m_bOverflowLeft = true;

		int iRight = iTabCount * m_iCurrentWidth;
		if (iRight + m_iScrollOffset + m_rcTabItemArea.left > m_rcTabItemArea.right)
			m_bOverflowRight = true;
		else
			m_bOverflowRight = false;

		Invalidate();
		return;
	}
*/
	// test if the tab bar can fit all the item WITHOUT scroll
	int iNewOffset;
	int iNewWidth = m_iCurrentWidth;
	int iMaxLength = m_rcTabItemArea.right - m_rcTabItemArea.left - s()->Tab()->GetTabRightOverlap() - 4;

	if ((int)m_TabItems.size() * TAB_TEXT_MAX_WIDTH > iMaxLength)
	{
		iNewWidth = TAB_TEXT_MAX_WIDTH;
		m_iLastLayout = 2;
		iNewOffset = m_iScrollOffset;
	}
	else
	{
		iNewWidth = TAB_TEXT_MAX_WIDTH;
		m_iLastLayout = 0;
		iNewOffset = 0;
	}

	if (m_iCurrentWidth != iNewWidth)
	{
		m_iCurrentWidth = iNewWidth;
		int iLeftPos = 0;
		for (size_t i = 0; i < m_TabItems.size(); i++)
		{
			m_TabItems[i]->SetNewWidth();
			//m_TabItems[i]->SetPos(iLeftPos);
			m_TabItems[i]->SetPosImmediately(iLeftPos);
			iLeftPos += m_iCurrentWidth;
		}
	}
	if (m_pDraggingItem)
		m_pDraggingItem->SetNewWidthImmediately();

	switch (m_iLastLayout)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			m_rcTabItemArea.left += s()->Tab()->GetTabScrollButtonWidth();
			m_rcTabItemArea.right -= s()->Tab()->GetTabScrollButtonWidth();
			break;
	}
	m_rcTabItemArea.right -= s()->Tab()->GetTabRightOverlap();
	m_rcTabItemAreaDraw.left = m_rcTabItemArea.left - s()->Tab()->GetTabLeftOverlap();
	m_rcTabItemAreaDraw.right = m_rcTabItemArea.right + s()->Tab()->GetTabRightOverlap();

	//SetScrollOffsetAnimation(GetValidScrollPos(m_iScrollOffset, m_bOverflowLeft, m_bOverflowRight));
	SetScrollOffsetImmediately(GetValidScrollPos(m_iScrollOffset, m_bOverflowLeft, m_bOverflowRight));

	Invalidate();
}


void CTuotuoTabCtrl::ScrollToEnsureVisibleAfterScroll(bool bLeft)
{
	if (bLeft)
	{
		// 当用户向左滚动时，我们保证在滚动结束后最左边的一个tab能显示完全
		int iLeft = - m_iScrollOffset / m_iCurrentWidth;
		if (m_iScrollOffset % m_iCurrentWidth != 0)
			//SetScrollOffsetAnimation(GetValidScrollPos(-iLeft * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
			SetScrollOffsetImmediately(GetValidScrollPos(-iLeft * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
		else if (m_bOverflowLeft)
			//SetScrollOffsetAnimation(GetValidScrollPos(-(iLeft - 1) * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
			SetScrollOffsetImmediately(GetValidScrollPos(-(iLeft - 1) * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
	}
	else
	{
		// 当用户向右滚动时，我们保证在滚动结束后最右边的一个tab能显示完全
		int iRight = (- m_iScrollOffset + m_rcTabItemArea.right - m_rcTabItemArea.left) / m_iCurrentWidth + 1;
		//SetScrollOffsetAnimation(GetValidScrollPos(-iRight * m_iCurrentWidth + m_rcTabItemArea.right - m_rcTabItemArea.left, m_bOverflowLeft, m_bOverflowRight));
		SetScrollOffsetImmediately(GetValidScrollPos(-iRight * m_iCurrentWidth + m_rcTabItemArea.right - m_rcTabItemArea.left, m_bOverflowLeft, m_bOverflowRight));
	}
}

//////////////////////////////////////////////////////////////////////////

CTabItem* CTuotuoTabCtrl::HitTest(const POINT &pt, Position *pos) const
{
	if (!::PtInRect(&m_rcTabItemAreaDraw, pt))
		return NULL;

	int iMouseX = pt.x - m_iScrollOffset - m_rcTabItemArea.left;
	int iLeft = iMouseX < 0 ? -1 : (iMouseX / m_iCurrentWidth);
	if (iLeft < 0 || iLeft >= (int)m_TabItems.size())
		return NULL;

	iMouseX -= iLeft * m_iCurrentWidth;
	CTabItem *pItem = m_TabItems[iLeft];
	if (!pos)
		return pItem;

	*pos = POS_TabItem;
	int iRightMargin = m_iCurrentWidth - s()->Tab()->GetTabMargin();
	if (iMouseX > iRightMargin)
		return pItem;

	if (!pItem->IsNoClose() && (m_eTabCloseButton == CloseBtn_AlwaysDisplay || (pItem == m_pSelectedItem && m_eTabCloseButton == CloseBtn_ActiveTabDisplay)))
	{
		iRightMargin -= s()->Tab()->GetTabCloseButtonWidth();
		if (iMouseX >= iRightMargin)
		{
			int iY = (s()->Tab()->GetTabHeight() - s()->Tab()->GetTabCloseButtonHeight()) / 2 + m_rcTabItemAreaDraw.top;
			if (pt.y >= iY && pt.y <= iY + s()->Tab()->GetTabCloseButtonHeight())
				*pos = POS_TabItemClose;
			return pItem;
		}
	}

	return pItem;
}

void CTuotuoTabCtrl::EnsureVisible(CTabItem *pItem)
{
	if (m_bClosingTabs)
	{
		if ((int)pItem->GetIndex() * m_iCurrentWidth + m_iScrollOffset < 0)
			//SetScrollOffsetAnimation(m_iScrollOffset + m_iCurrentWidth);
			SetScrollOffsetImmediately(m_iScrollOffset + m_iCurrentWidth);
		else if (((int)pItem->GetIndex() + 1) * m_iCurrentWidth + m_iScrollOffset > m_rcTabItemArea.right - m_rcTabItemArea.left)
			//SetScrollOffsetAnimation(m_iScrollOffset - m_iCurrentWidth);
			SetScrollOffsetImmediately(m_iScrollOffset - m_iCurrentWidth);
	}
	else if (m_bOverflowLeft || m_bOverflowRight)
	{
		if ((int)pItem->GetIndex() * m_iCurrentWidth + m_iScrollOffset < 0)
			//SetScrollOffsetAnimation(GetValidScrollPos(-(int)pItem->GetIndex() * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
			SetScrollOffsetImmediately(GetValidScrollPos(-(int)pItem->GetIndex() * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
		else if (((int)pItem->GetIndex() + 1) * m_iCurrentWidth + m_iScrollOffset > m_rcTabItemArea.right - m_rcTabItemArea.left)
			//SetScrollOffsetAnimation(GetValidScrollPos(-((int)pItem->GetIndex() + 1) * m_iCurrentWidth + m_rcTabItemArea.right - m_rcTabItemArea.left, m_bOverflowLeft, m_bOverflowRight));
			SetScrollOffsetImmediately(GetValidScrollPos(-((int)pItem->GetIndex() + 1) * m_iCurrentWidth + m_rcTabItemArea.right - m_rcTabItemArea.left, m_bOverflowLeft, m_bOverflowRight));
	}
}

//////////////////////////////////////////////////////////////////////////

int CTuotuoTabCtrl::GetValidScrollPos(int iOriginalScrollPos, bool &bOverflowLeft, bool &bOverflowRight)
{
	int iTabCount = GetItemCountInDraggingMode();
	int iMaxScrollNeeded = m_rcTabItemArea.right - m_rcTabItemArea.left - iTabCount * m_iCurrentWidth;
	if (iMaxScrollNeeded >= 0)
	{
		bOverflowLeft = bOverflowRight = false;
		return 0;
	}

	if (iOriginalScrollPos > 0)
		iOriginalScrollPos = 0;
	bOverflowLeft = iOriginalScrollPos < 0;

	if (iOriginalScrollPos < iMaxScrollNeeded)
		iOriginalScrollPos = iMaxScrollNeeded;
	bOverflowRight = iOriginalScrollPos > iMaxScrollNeeded;

	return iOriginalScrollPos;
}

void CTuotuoTabCtrl::SetScrollOffsetImmediately(int iOffset)
{
	m_iScrollOffset = iOffset;
	m_iScrollOffsetAni = m_iScrollOffset;
/*
	if (m_iScrollOffsetAniFrame < FRAME_COUNT)
	{
		m_iScrollOffsetAniFrame = FRAME_COUNT;
		IncOrDecAnimationCounter(false, NULL, TabInv_Scroll);
	}
*/
	Invalidate();
}
/*
void CTuotuoTabCtrl::SetScrollOffsetAnimation(int iOffset)
{
	if (iOffset != m_iScrollOffset)
	{
		m_iScrollOffsetOld = m_iScrollOffset;
		m_iScrollOffset = iOffset;
		if (m_iScrollOffsetAniFrame == FRAME_COUNT)
			IncOrDecAnimationCounter(true, NULL, TabInv_Scroll);
		m_iScrollOffsetAniFrame = 0;
		InvalidateRect(&m_rcTabItemAreaDraw);
	}
}
*/

void CTuotuoTabCtrl::SetScrollDir(int iDir)
{
	if (iDir == 0)
	{
		if (m_iScrollingDir != 0)
		{
			KillTimer(Timer_ScrollBegin);
			KillTimer(Timer_Scroll);
		}
		m_iScrollingDir = 0;
	}
	else
	{
		if (m_iScrollingDir == 0)
			SetTimer(Timer_ScrollBegin, TAB_SCROLL_BEGIN_INTERVAL);
		m_iScrollingDir = iDir;
	}
}
