#include "stdafx.h"
#include "../MainFrame.h"
#include "CategoryItem.h"
#include "CategoryCtrl.h"


void CTuotuoCategoryCtrl::UpdateLayout()
{
	int iCateCount = m_CateItems.size();

	GetClientRect(&m_rcTabItemArea);

	// m_rcTabItemAreaDraw 不包含左右移动按钮区域
	m_rcTabItemAreaDraw = m_rcTabItemArea;

	if (iCateCount == 0 || m_rcTabItemArea.right <= m_rcTabItemArea.left)
	{
		m_rcTabItemArea.left = m_rcTabItemArea.right = 0;
		m_rcTabItemAreaDraw = m_rcTabItemArea;
		Invalidate();

		return;
	}

	// test if the tab bar can fit all the item WITHOUT scroll
	int iMaxLength = m_rcTabItemArea.right - m_rcTabItemArea.left - s()->Category()->GetCategoryRightOverlap() - 4;

	if ((int)m_CateItems.size() * CATEGORY_TEXT_MAX_WIDTH > iMaxLength)
		m_bShowScrollBtn = true;
	else
		m_bShowScrollBtn = false;

	if (m_bShowScrollBtn)
	{
		m_rcTabItemArea.left += s()->Category()->GetCategoryScrollButtonWidth();
		m_rcTabItemArea.right -= s()->Category()->GetCategoryScrollButtonWidth();
	}

 	m_rcTabItemAreaDraw.left = m_rcTabItemArea.left;
 	m_rcTabItemAreaDraw.right = m_rcTabItemArea.right;

	SetScrollOffsetImmediately(GetValidScrollPos(m_iScrollOffset, m_bOverflowLeft, m_bOverflowRight));

	Invalidate();
}


void CTuotuoCategoryCtrl::ScrollToEnsureVisibleAfterScroll(bool bLeft)
{
	if (bLeft)
	{
		// 当用户向左滚动时，我们保证在滚动结束后最左边的一个tab能显示完全
		int iLeft = - m_iScrollOffset / m_iCurrentWidth;
		if (m_iScrollOffset % m_iCurrentWidth != 0)
			SetScrollOffsetImmediately(GetValidScrollPos(-iLeft * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
		else if (m_bOverflowLeft)
			SetScrollOffsetImmediately(GetValidScrollPos(-(iLeft - 1) * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
	}
	else
	{
		// 当用户向右滚动时，我们保证在滚动结束后最右边的一个tab能显示完全
		int iRight = (- m_iScrollOffset + m_rcTabItemArea.right - m_rcTabItemArea.left) / m_iCurrentWidth + 1;
		SetScrollOffsetImmediately(GetValidScrollPos(-iRight * m_iCurrentWidth + m_rcTabItemArea.right - m_rcTabItemArea.left, m_bOverflowLeft, m_bOverflowRight));
	}
}

//////////////////////////////////////////////////////////////////////////

CCategoryItem* CTuotuoCategoryCtrl::HitTest(const POINT &pt, CategoryPosition *pos) const
{
	if (!::PtInRect(&m_rcTabItemAreaDraw, pt))
		return NULL;

	int iMouseX = pt.x - m_iScrollOffset - m_rcTabItemArea.left;
	int iLeft = iMouseX < 0 ? -1 : (iMouseX / m_iCurrentWidth);
	if (iLeft < 0 || iLeft >= (int)m_CateItems.size())
		return NULL;

	iMouseX -= iLeft * (m_iCurrentWidth - 1);
	CCategoryItem *pItem = m_CateItems[iLeft];
	if (!pos)
		return pItem;

	*pos = CatePos_TabItem;
	if (pItem->GetWebData() == START_PAGE_WEBSITE_DATA ||
		pItem->GetWebData()->IsNoClose())
		return pItem;

	int iRightMargin = m_iCurrentWidth - s()->Category()->GetCategoryMargin();
	if (iMouseX > iRightMargin)
		return pItem;

	if (m_eTabCloseButton == CloseBtn_AlwaysDisplay || (pItem == m_pSelectedItem && m_eTabCloseButton == CloseBtn_ActiveTabDisplay))
	{
		iRightMargin -= s()->Category()->GetCategoryCloseButtonWidth();
		if (iMouseX >= iRightMargin)
		{
			int iY = (s()->Category()->GetCategoryHeight() - s()->Category()->GetCategoryCloseButtonHeight()) / 2 + m_rcTabItemAreaDraw.top;
			if (pt.y >= iY && pt.y <= iY + s()->Category()->GetCategoryCloseButtonHeight())
				*pos = CatePos_TabItemClose;
			return pItem;
		}
	}

	return pItem;
}

void CTuotuoCategoryCtrl::EnsureVisible(CCategoryItem *pItem)
{
// 	if (m_bClosingTabs)
// 	{
// 		if ((int)pItem->GetIndex() * m_iCurrentWidth + m_iScrollOffset < 0)
// 			SetScrollOffsetAnimation(m_iScrollOffset + m_iCurrentWidth);
// 		else if (((int)pItem->GetIndex() + 1) * m_iCurrentWidth + m_iScrollOffset > m_rcTabItemArea.right - m_rcTabItemArea.left)
// 			SetScrollOffsetAnimation(m_iScrollOffset - m_iCurrentWidth);
// 	}
// 	else 
		
		//if (m_bOverflowLeft || m_bOverflowRight)
	{
		if ((int)pItem->GetIndex() * m_iCurrentWidth + m_iScrollOffset < 0)
			SetScrollOffsetImmediately(GetValidScrollPos(-(int)pItem->GetIndex() * m_iCurrentWidth, m_bOverflowLeft, m_bOverflowRight));
		else if (((int)pItem->GetIndex() + 1) * m_iCurrentWidth + m_iScrollOffset > m_rcTabItemArea.right - m_rcTabItemArea.left)
			SetScrollOffsetImmediately(GetValidScrollPos(-((int)pItem->GetIndex() + 1) * m_iCurrentWidth + m_rcTabItemArea.right - m_rcTabItemArea.left, m_bOverflowLeft, m_bOverflowRight));
	}
}

//////////////////////////////////////////////////////////////////////////

int CTuotuoCategoryCtrl::GetValidScrollPos(int iOriginalScrollPos, bool &bOverflowLeft, bool &bOverflowRight)
{
	int iCateCount = m_CateItems.size();
	int iMaxScrollNeeded = m_rcTabItemArea.right - m_rcTabItemArea.left - iCateCount * m_iCurrentWidth;
	if (iMaxScrollNeeded >= 0) // 可以完全放下
	{
		bOverflowLeft = bOverflowRight = false;
		return 0;
	}

	// 放不下，左边有空
	if (iOriginalScrollPos > 0)
		iOriginalScrollPos = 0;

	// 
	bOverflowLeft = iOriginalScrollPos < 0;

	// 右边有空，对齐右边
	if (iOriginalScrollPos < iMaxScrollNeeded)
		iOriginalScrollPos = iMaxScrollNeeded;

	bOverflowRight = iOriginalScrollPos > iMaxScrollNeeded;

	return iOriginalScrollPos;
}

void CTuotuoCategoryCtrl::SetScrollOffsetImmediately(int iOffset)
{
	m_iScrollOffset = iOffset;
	m_iScrollOffsetAni = m_iScrollOffset;

	Invalidate();
}

/*
void CTuotuoCategoryCtrl::SetScrollOffsetAnimation(int iOffset)
{
	if (iOffset != m_iScrollOffset)
	{
		m_iScrollOffsetOld = m_iScrollOffset;
		m_iScrollOffset = iOffset;
		m_iScrollOffsetAniFrame = 0;
		InvalidateRect(&m_rcTabItemAreaDraw);
	}
}
*/

void CTuotuoCategoryCtrl::SetScrollDir(int iDir)
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
			SetTimer(Timer_ScrollBegin, CATEGORY_SCROLL_BEGIN_INTERVAL);
		m_iScrollingDir = iDir;
	}
}
