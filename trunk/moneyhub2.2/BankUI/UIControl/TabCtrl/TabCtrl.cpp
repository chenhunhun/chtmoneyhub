#include "stdafx.h"
#include "../MainFrame.h"
#include "TabCtrl.h"
#include "TabItem.h"


CTuotuoTabCtrl::CTuotuoTabCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_bMouseInWindow(false), m_iCurrentWidth(TAB_TEXT_MAX_WIDTH),
		m_pSelectedItem(NULL), m_pHotItem(NULL), m_nDragToPos(-2), m_pDraggingItem(NULL), m_bDraggingSource(false),
		m_bClosingTabs(false), m_iLastLayout(0),
		m_iScrollOffset(0), m_iScrollingDir(0), m_bOverflowLeft(false), m_bOverflowRight(false),
		m_eButtonStatus(Btn_MouseOut), m_ePosition(POS_Unknown),
		m_iScrollOffsetAni(0), m_iScrollOffsetAniFrame(FRAME_COUNT), m_iScrollOffsetOld(0)
{
	m_eTabCloseButton = CloseBtn_ActiveTabDisplay;

	FS()->pTab = this;

	m_bDisplayResourceIcon = true;

	m_ptDragOrigin.x = m_ptDragOrigin.y = 0;
	m_pRCIndex[POS_Unknown] = NULL;
	m_pRCIndex[POS_ScrollLLeftBtn] = &m_rcScrollLLeft;
	m_pRCIndex[POS_ScrollRRightBtn] = &m_rcScrollRRight;
	m_pRCIndex[POS_TabItem] = &m_rcTabItemAreaDraw;
	m_pRCIndex[POS_TabItemClose] = &m_rcTabItemAreaDraw;
}

void CTuotuoTabCtrl::UpdateDisplayResourceIcon()
{
}

void CTuotuoTabCtrl::OnSettingUpdate()
{
}

//////////////////////////////////////////////////////////////////////////

void CTuotuoTabCtrl::InsertItem(size_t nInsertTo, CTabItem *pItem)
{
	ATLASSERT(pItem);
	m_bClosingTabs = false;

	// we assume, that any new tab appears on the right
	if (m_TabItems.empty())
	{
		m_TabItems.push_back(pItem);
		pItem->SetIndex(0);
		pItem->ChangeItemParent(this);
		pItem->SetPosImmediately(0);
		pItem->SetNewWidthImmediately();
	}
	else
	{
		if (nInsertTo == -1)
			nInsertTo = m_TabItems.size();
		ATLASSERT(nInsertTo >= 0 && nInsertTo <= m_TabItems.size());
		m_TabItems.insert(m_TabItems.begin() + nInsertTo, pItem);
		for (size_t i = nInsertTo; i < m_TabItems.size(); i++)
		{
			m_TabItems[i]->SetIndex(i);
			m_TabItems[i]->SetPosImmediately(i * m_iCurrentWidth);
		}
		pItem->ChangeItemParent(this);
		pItem->SetNewWidthImmediately();
	}
	EnsureVisible(pItem);

	UpdateLayout();
}


void CTuotuoTabCtrl::DeleteItem(int iIndex)
{
	CTabItem *pItem = m_TabItems[iIndex];

	if (pItem == m_pSelectedItem)
		m_pSelectedItem = NULL;

	//pItem->FinishAnimation(this);
	m_TabItems.erase(m_TabItems.begin() + iIndex);
	for (size_t i = iIndex; i < m_TabItems.size(); i++)
	{
		m_TabItems[i]->SetIndex(i);
		//m_TabItems[i]->SetPos(i * m_iCurrentWidth);
		m_TabItems[i]->SetPosImmediately(i * m_iCurrentWidth);
	}

	UpdateLayout();
}

void CTuotuoTabCtrl::DeleteAllItems()
{
	m_TabItems.clear();
	m_pSelectedItem = NULL;
	UpdateLayout();
}


void CTuotuoTabCtrl::SelectItem(CTabItem *pItem)
{
	if (pItem == NULL)
	{
		m_pSelectedItem = NULL;
		return;
	}
	if (pItem == m_pSelectedItem)
		return;
	m_pSelectedItem = pItem;
	EnsureVisible(m_pSelectedItem);
	InvalidateRect(&m_rcTabItemAreaDraw);
}

CTabItem* CTuotuoTabCtrl::GetLeftOrRight(CTabItem *pItem, bool bLeft, bool bRoll)
{
	if (bLeft)
	{
		if (pItem->GetIndex() != 0)
			return m_TabItems[pItem->GetIndex() - 1];
		else
			return bRoll ? m_TabItems[m_TabItems.size() - 1] : (m_TabItems.size() == 1 ? NULL : m_TabItems[1]);
	}
	else
	{
		if (pItem->GetIndex() != m_TabItems.size() - 1)
			return m_TabItems[pItem->GetIndex() + 1];
		else
			return bRoll ? m_TabItems[0] : (m_TabItems.size() == 1 ? NULL : m_TabItems[m_TabItems.size() - 2]);
	}
}


void CTuotuoTabCtrl::SetMousePosAndStatus(Position ePos, ButtonStatus btnStatus)
{
	if (ePos == m_ePosition)
	{
		if (btnStatus != m_eButtonStatus)
		{
			m_eButtonStatus = btnStatus;
			if (m_ePosition != POS_Unknown)
				InvalidateRect(m_pRCIndex[m_ePosition]);
		}
	}
	else
	{
		Position oldPos = m_ePosition;
		m_ePosition = ePos;
		m_eButtonStatus = btnStatus;
		if (oldPos != POS_Unknown)
			InvalidateRect(m_pRCIndex[oldPos]);
		if (m_ePosition != POS_Unknown)
			InvalidateRect(m_pRCIndex[m_ePosition]);
	}
}

/*
void CTuotuoTabCtrl::IncOrDecAnimationCounter(bool bAddRef, CTabItem *pItem, TabInvalidateType eType)
{
	if (pItem && (pItem->GetIndex() >= m_TabItems.size() || m_TabItems[pItem->GetIndex()] != pItem) && m_pDraggingItem != pItem)
	{
//		ATLASSERT(0);
		m_AniSet.erase(pItem);
		return;
	}
	ATLASSERT(pItem == NULL || (pItem && !::IsBadReadPtr(pItem, sizeof(CTabItem))));

	size_t nOld = m_AniSet.size();
	AnimationMap::iterator it = m_AniSet.find(pItem);
	if (bAddRef)
	{
		if (it == m_AniSet.end())
			m_AniSet.insert(std::make_pair(pItem, eType));
		else
			it->second |= eType;
	}
	else
	{
		ATLASSERT(it != m_AniSet.end());
		it->second &= ~eType;
		if (it->second == 0)
			m_AniSet.erase(it);
	}
	if (m_AniSet.size() == 0)
	{
//		ATLTRACE(" -------- end\n");
		KillTimer(Timer_Update);
	}
	else if (nOld == 0)
	{
//		ATLTRACE("ani ---------- begin -----------");
		SetTimer(Timer_Update, 10);
	}
}
*/

void CTuotuoTabCtrl::NotifyAndGetTabIconPos(CTabItem *pItem, RECT &rc)
{
	GetClientRect(&rc);
	EnsureVisible(pItem);
	int iTabLeft = pItem->m_iXPos + m_iScrollOffset;
	rc.left = m_rcTabItemArea.left + iTabLeft + s()->Tab()->GetTabMargin();
	rc.right = rc.left + ICON_WIDTH;
}


void CTuotuoTabCtrl::InvalidateTab(CTabItem *pItem, TabInvalidateType eType)
{
	int iTabLeft = pItem->m_iXPos + m_iScrollOffset;
	RECT rc;
	GetClientRect(&rc);
	switch (eType)
	{
		case TabInv_Tab:
			rc.left = m_rcTabItemArea.left + iTabLeft;
			rc.right = rc.left + m_iCurrentWidth;
			break;
		case TabInv_Icon:
			rc.left = m_rcTabItemArea.left + iTabLeft + s()->Tab()->GetTabMargin();
			rc.right = rc.left + ICON_WIDTH;
			break;
		case TabInv_Text:
			rc.left = m_rcTabItemArea.left + iTabLeft + s()->Tab()->GetTabMargin() + ICON_WIDTH + IMAGE_TEXT_MARGIN;
			rc.right = rc.left + m_iCurrentWidth - TEXT_CLOSE_MARGIN;
			break;
		default:
			ATLASSERT(0);
			break;
	}
	InvalidateRect(&rc);
}

void CTuotuoTabCtrl::InvalidateTabs(TabInvalidateType eType)
{
	// 只更新显示出来的tab
	int iRightBorder = m_rcTabItemArea.right - m_rcTabItemArea.left - m_iScrollOffsetAni;
	size_t nScrollLeft = - m_iScrollOffsetAni / m_iCurrentWidth;
	size_t nScrollRight = iRightBorder / m_iCurrentWidth + (iRightBorder % m_iCurrentWidth == 0 ? 0 : 1);
	size_t nPaintRight = min(nScrollRight, m_TabItems.size() - 1);

	RECT rc;
	GetClientRect(&rc);
	switch (eType)
	{
		case TabInv_Icon:
			for (size_t i = nScrollLeft == 0 ? 0 : nScrollLeft - 1; i <= nPaintRight; i++)
			{
				int iTabLeft = m_TabItems[i]->m_iXPos + m_iScrollOffset;
				rc.left = m_rcTabItemArea.left + iTabLeft + s()->Tab()->GetTabMargin();
				rc.right = rc.left + ICON_WIDTH;
				InvalidateRect(&rc);
			}
			break;
		default:
			ATLASSERT(0);
			break;
	}
}

//////////////////////////////////////////////////////////////////////////

void CTuotuoTabCtrl::DoPaint(CDCHandle dc, const RECT &rcClip) const
{
	RECT rcClient;
	GetClientRect(&rcClient);

	dc.SetBkMode(TRANSPARENT);
	s()->Toolbar()->DrawRebarBackPart(dc, rcClient, m_hWnd);

	int iOffSetTop = rcClient.bottom - s()->Tab()->GetTabHeight();
	if (iOffSetTop < 0)
		iOffSetTop = 0;
	rcClient.top += iOffSetTop;

	// draw scroll button
	if (m_bOverflowLeft || m_bOverflowRight || (m_bClosingTabs && m_iLastLayout == 2))
	{
		if (m_bOverflowLeft)
		{
			s()->Tab()->DrawTabScrollButton(dc, m_rcScrollLLeft, m_ePosition == POS_ScrollLLeftBtn ? m_eButtonStatus : Btn_MouseOut, 0);
		}
		else
		{
			s()->Tab()->DrawTabScrollButton(dc, m_rcScrollLLeft, Btn_Disabled, 0);
		}

		if (m_bOverflowRight)
			s()->Tab()->DrawTabScrollButton(dc, m_rcScrollRRight, m_ePosition == POS_ScrollRRightBtn ? m_eButtonStatus : Btn_MouseOut, 1);
		else
			s()->Tab()->DrawTabScrollButton(dc, m_rcScrollRRight, Btn_Disabled, 1);
	}

	// draw sidebar button
	// draw "new tab" button
	//s()->Tab()->DrawNewTabButton(dc, m_rcNewTabBtn, m_ePosition == POS_NewTabBtn ? m_eButtonStatus : Btn_MouseOut);

	// draw tabs
	if (GetItemCountInDraggingMode() == 0)
		return;

	dc.IntersectClipRect(&m_rcTabItemAreaDraw);
	dc.SetViewportOrg(m_iScrollOffsetAni + m_rcTabItemArea.left, 0);

	// we draw the selected item at the last
	dc.SelectFont(s()->Common()->GetDefaultFont());
	ButtonStatus eTabCloseBtnStatus = m_ePosition == POS_TabItemClose ? m_eButtonStatus : Btn_MouseOut;
	int iRightBorder = m_rcTabItemArea.right - m_rcTabItemArea.left - m_iScrollOffsetAni;
	size_t nScrollLeft = - m_iScrollOffsetAni / m_iCurrentWidth;
	size_t nScrollRight = iRightBorder / m_iCurrentWidth + (iRightBorder % m_iCurrentWidth == 0 ? 0 : 1);
	size_t nPaintRight = min(nScrollRight, m_TabItems.size() - 1);

	if (m_nDragToPos == -2)
	{
		for (size_t i = nScrollLeft == 0 ? 0 : nScrollLeft - 1; i <= nPaintRight; i++)
			if (m_TabItems[i] != m_pSelectedItem)
				m_TabItems[i]->DoPaint(dc, m_TabItems[i] == m_pHotItem ? Btn_MouseOver : Btn_MouseOut, m_TabItems[i] == m_pHotItem ? m_ePosition : POS_TabItem, m_eButtonStatus, m_eTabCloseButton == CloseBtn_AlwaysDisplay, iOffSetTop, false);
		dc.SelectFont(s()->Common()->GetBoldFont());
		if (m_pSelectedItem && TAB_MATCH_INTERVAL(nScrollLeft, m_pSelectedItem->GetIndex(), nScrollRight))
			m_pSelectedItem->DoPaint(dc, Btn_MouseDown, m_pSelectedItem == m_pHotItem ? m_ePosition : POS_TabItem, m_eButtonStatus, m_eTabCloseButton != CloseBtn_AlwaysHide, iOffSetTop, false);

		dc.SelectClipRgn(NULL);
		dc.SetViewportOrg(0, 0);
	}
	else		// 拖动状态
	{
		if (m_bDraggingSource)			// 正常状态显示 || 在自己的框架里面拖动
		{
			for (size_t i = nScrollLeft; i <= nPaintRight; i++)
				if (m_TabItems[i] != m_pSelectedItem)
					m_TabItems[i]->DoPaint(dc, Btn_MouseOut, POS_TabItem, Btn_MouseOut, m_eTabCloseButton != CloseBtn_AlwaysHide, iOffSetTop, false);

			dc.SelectClipRgn(NULL);
			dc.SetViewportOrg(0, 0);
			dc.SelectFont(s()->Common()->GetBoldFont());

			if (m_pDraggingItem)
				m_pSelectedItem->DoPaint(dc, Btn_MouseDown, POS_TabItem, Btn_MouseOut, m_eTabCloseButton != CloseBtn_AlwaysHide, iOffSetTop, false);
		}
		else												// 拖到别人的框架里面了
		{
			for (size_t i = nScrollLeft; i <= nPaintRight; i++)
				m_TabItems[i]->DoPaint(dc, Btn_MouseOut, POS_TabItem, Btn_MouseOut, m_eTabCloseButton == CloseBtn_AlwaysDisplay, iOffSetTop, false);

			dc.SelectClipRgn(NULL);
			dc.SetViewportOrg(0, 0);
			dc.SelectFont(s()->Common()->GetBoldFont());

			if (m_pDraggingItem)
				m_pDraggingItem->DoPaint(dc, Btn_MouseDown, POS_TabItem, Btn_MouseOut, m_eTabCloseButton != CloseBtn_AlwaysHide, iOffSetTop, false);
		}
	}
}

/*
void CTuotuoTabCtrl::RedrawImmediately(const RECT *rc)
{
	if (!IsWindowVisible())
		return;
	CClientDC dc(m_hWnd);
	if (rc)
	{
		CRgn hRgn;
		hRgn.CreateRectRgnIndirect(rc);
		dc.SelectClipRgn(hRgn);
		SendMessage(WM_PAINT, (WPARAM)(HDC)dc, 0);
		dc.SelectClipRgn(NULL);
	}
	else
		SendMessage(WM_PAINT, (WPARAM)(HDC)dc, 0);
}
*/