#include "stdafx.h"
#include "Util.h"
#include "../MainFrame.h"
#include "CategoryCtrl.h"
#include "CategoryItem.h"
#include "../../../Utils/ListManager/URLList.h"


CTuotuoCategoryCtrl::CTuotuoCategoryCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_bMouseInWindow(false), m_iCurrentWidth(CATEGORY_TEXT_MAX_WIDTH),
		m_pSelectedItem(NULL), m_pHotItem(NULL),
		m_bShowScrollBtn(0),
		m_iScrollOffset(0), m_iScrollingDir(0), m_bOverflowLeft(false), m_bOverflowRight(false),
		m_eButtonStatus(Btn_MouseOut), m_ePosition(CatePos_Unknown),
		m_iScrollOffsetAni(0), m_iScrollOffsetAniFrame(FRAME_COUNT), m_iScrollOffsetOld(0)
{
	m_eTabCloseButton = CloseBtn_AlwaysDisplay; //CloseBtn_ActiveTabDisplay;

	FS()->pCate = this;

	m_pRCIndex[CatePos_Unknown] = NULL;
	m_pRCIndex[CatePos_ScrollLLeftBtn] = &m_rcScrollLLeft;
	m_pRCIndex[CatePos_ScrollRRightBtn] = &m_rcScrollRRight;
	m_pRCIndex[CatePos_TabItem] = &m_rcTabItemAreaDraw;
	m_pRCIndex[CatePos_TabItemClose] = &m_rcTabItemAreaDraw;
}

//////////////////////////////////////////////////////////////////////////

CCategoryItem* CTuotuoCategoryCtrl::FindOrCreateItem(LPCTSTR lpszURL, CTabItem *pItem, bool bForceCreate, int iBillFlag)
{
	const CWebsiteData *pData;

	if(!bForceCreate)
	{
		pData = CURLList::GetInstance()->GetData(L"", lpszURL);	
		for (size_t i = 0; i < m_CateItems.size(); i++)
			if (m_CateItems[i]->m_pWebData == pData)
			{
				//AddATabVisitIndex (i); // gao 用户虽然创建了TAB项，但不一定访问了，所以不用添加
				return m_CateItems[i];
			}
	}
	else
	{
		pData = CURLList::GetInstance()->GetData(L"getbill", lpszURL);
		for (size_t i = 0; i < m_CateItems.size(); i++)
			if (m_CateItems[i]->m_pWebData == pData)
			{
				//AddATabVisitIndex (i); // gao 用户虽然创建了TAB项，但不一定访问了，所以不用添加
				return m_CateItems[i];
			}
	}


	CCategoryItem *pCateItem = new CCategoryItem(this);
	if(bForceCreate)
	{
		const CWebsiteData *pTData = CURLList::GetInstance()->GetData(L"", lpszURL);
		pCateItem->SetShowInfo( 1 );
		if(pTData != NULL)
			pCateItem->SetShowInfo(pTData->GetName());
		else
			pCateItem->SetShowInfo(L"金融机构");
	}
	
	pCateItem->m_pWebData = pData;

	//AddATabVisitIndex (m_CateItems.size ());

	InsertItem(-1, pCateItem);
	
	return pCateItem;
}

CCategoryItem* CTuotuoCategoryCtrl::FindExistItem(LPCTSTR lpszURL, int iBillFlag)
{
	const CWebsiteData *pData;

	if(iBillFlag == 0)
		pData = CURLList::GetInstance()->GetData(L"", lpszURL);
	else
		pData = CURLList::GetInstance()->GetData(L"getbill", lpszURL);
	
	for (size_t i = 0; i < m_CateItems.size(); i++)
		if (m_CateItems[i]->m_pWebData == pData)
			return m_CateItems[i];

	return NULL;
}

void CTuotuoCategoryCtrl::InsertItem(size_t nInsertTo, CCategoryItem *pItem)
{
	ATLASSERT(pItem);

	// we assume, that any new tab appears on the right
	if (m_CateItems.empty())
	{
		m_CateItems.push_back(pItem);
		pItem->SetIndex(0);
		pItem->SetPosImmediately(0);
		pItem->SetNewWidthImmediately();
	}
	else
	{
		if (nInsertTo == -1)
			nInsertTo = m_CateItems.size();
		ATLASSERT(nInsertTo >= 0 && nInsertTo <= m_CateItems.size());
		m_CateItems.insert(m_CateItems.begin() + nInsertTo, pItem);
		for (size_t i = nInsertTo; i < m_CateItems.size(); i++)
		{
			m_CateItems[i]->SetIndex(i);
			m_CateItems[i]->SetPosImmediately(i * m_iCurrentWidth);
		}
		pItem->SetNewWidthImmediately();
	}

	EnsureVisible(pItem);
	UpdateLayout();
}


void CTuotuoCategoryCtrl::DeleteItem(int iIndex)
{
	CCategoryItem *pItem = m_CateItems[iIndex];

	if (pItem == m_pSelectedItem)
		m_pSelectedItem = NULL;

	m_CateItems.erase(m_CateItems.begin() + iIndex);
	for (size_t i = iIndex; i < m_CateItems.size(); i++)
	{
		m_CateItems[i]->SetIndex(i);
		m_CateItems[i]->SetPosImmediately(i * m_iCurrentWidth);
	}

	//if (m_nListVistorIndex.size () > 0)
	m_nListVistorIndex.remove (pItem); // 弹出要删除当前Tab索引

	UpdateLayout();
}

void CTuotuoCategoryCtrl::DeleteItemExcept(CCategoryItem *pItem)
{
	m_CateItems.clear();
	if (pItem)
	{
		m_CateItems.push_back(pItem);
		pItem->SetIndex(0);
		pItem->SetPosImmediately(0);
	}
	if (pItem != m_pSelectedItem)
		m_pSelectedItem = NULL;

	// 这个函数暂时没用，如果用了也要处理m_nListVistorIndex
	m_nListVistorIndex.clear ();
	m_nListVistorIndex.push_back (pItem);

	UpdateLayout();
}

void CTuotuoCategoryCtrl::SelectItem(CCategoryItem *pItem)
{
	if (pItem == NULL)
	{
		m_pSelectedItem = NULL;
		return;
	}
	if (pItem == m_pSelectedItem)
		return;
	m_pSelectedItem = pItem;

	AddATabVisitIndex (pItem); // 记录用户操作过的TAB项指针
		

	EnsureVisible(m_pSelectedItem);
	InvalidateRect(&m_rcTabItemAreaDraw);
}

void CTuotuoCategoryCtrl::ToggleItem()
{
	if (m_CateItems.size() == 0)
		return;

	CategoryItemVector::const_iterator it = m_CateItems.begin();
	for (; it != m_CateItems.end(); ++it)
	{
		if (*it == m_pSelectedItem)
			break;
	}

	CCategoryItem *pItem;
	if (++it == m_CateItems.end())
		pItem = *m_CateItems.begin();
	else
		pItem = *it;

	FS()->MDI()->ActiveCategory(pItem);
}

void CTuotuoCategoryCtrl::SetMousePosAndStatus(CategoryPosition ePos, ButtonStatus btnStatus)
{
	if (ePos == m_ePosition)
	{
		if (btnStatus != m_eButtonStatus)
		{
			m_eButtonStatus = btnStatus;
			if (m_ePosition != CatePos_Unknown)
				InvalidateRect(m_pRCIndex[m_ePosition]);
		}
	}
	else
	{
		CategoryPosition oldPos = m_ePosition;
		m_ePosition = ePos;
		m_eButtonStatus = btnStatus;
		if (oldPos != CatePos_Unknown)
			InvalidateRect(m_pRCIndex[oldPos]);
		if (m_ePosition != CatePos_Unknown)
			InvalidateRect(m_pRCIndex[m_ePosition]);
	}
}


void CTuotuoCategoryCtrl::InvalidateTab(CCategoryItem *pItem, CategoryInvalidateType eType)
{
	int iTabLeft = pItem->m_iXPos + m_iScrollOffset;
	RECT rc;
	GetClientRect(&rc);
	switch (eType)
	{
		case CateInv_Tab:
			rc.left = m_rcTabItemArea.left + iTabLeft;
			rc.right = rc.left + m_iCurrentWidth;
			break;
		case CateInv_Icon:
			rc.left = m_rcTabItemArea.left + iTabLeft + s()->Category()->GetCategoryMargin();
			rc.right = rc.left + ICON_WIDTH;
			break;
		case CateInv_Text:
			rc.left = m_rcTabItemArea.left + iTabLeft + s()->Category()->GetCategoryMargin() + ICON_WIDTH + CATEGORY_IMAGE_TEXT_MARGIN;
			rc.right = rc.left + m_iCurrentWidth - CATEGORY_TEXT_CLOSE_MARGIN;
			break;
		default:
			ATLASSERT(0);
			break;
	}
	InvalidateRect(&rc);
}

void CTuotuoCategoryCtrl::InvalidateTabs(CategoryInvalidateType eType)
{
	// 只更新显示出来的tab
	int iRightBorder = m_rcTabItemArea.right - m_rcTabItemArea.left - m_iScrollOffsetAni;
	size_t nScrollLeft = - m_iScrollOffsetAni / m_iCurrentWidth;
	size_t nScrollRight = iRightBorder / m_iCurrentWidth + (iRightBorder % m_iCurrentWidth == 0 ? 0 : 1);
	size_t nPaintRight = min(nScrollRight, m_CateItems.size() - 1);

	RECT rc;
	GetClientRect(&rc);
	switch (eType)
	{
		case CateInv_Icon:
			for (size_t i = nScrollLeft == 0 ? 0 : nScrollLeft - 1; i <= nPaintRight; i++)
			{
				int iTabLeft = m_CateItems[i]->m_iXPos + m_iScrollOffset;
				rc.left = m_rcTabItemArea.left + iTabLeft + s()->Category()->GetCategoryMargin();
				rc.right = rc.left + ICON_WIDTH;
				InvalidateRect(&rc);
			}
			break;
		default:
			ATLASSERT(0);
			break;
	}
}

//CCategoryItem* CTuotuoCategoryCtrl::GetLeftOrRight(CCategoryItem *pItem, bool bLeft, bool bRoll)
//{
//	if (bLeft)
//	{
//		if (pItem->GetIndex() != 0)
//			return m_CateItems[pItem->GetIndex() - 1];
//		else
//			return bRoll ? m_CateItems[m_CateItems.size() - 1] : (m_CateItems.size() == 1 ? NULL : m_CateItems[1]);
//	}
//	else
//	{
//		if (pItem->GetIndex() != m_CateItems.size() - 1)
//			return m_CateItems[pItem->GetIndex() + 1];
//		else
//			return bRoll ? m_CateItems[0] : (m_CateItems.size() == 1 ? NULL : m_CateItems[m_CateItems.size() - 2]);
//	}
//}

// 得到最近选中的Tab
CCategoryItem* CTuotuoCategoryCtrl::GetLastActiveItem(void)
{
	if (m_nListVistorIndex.size () < 0)
		return m_CateItems[0];

	CCategoryItem* pTempItem = m_nListVistorIndex.back (); 
	ATLASSERT (NULL != pTempItem);
	if ( NULL == pTempItem)
		pTempItem = m_CateItems[0];

	return pTempItem;
}
//////////////////////////////////////////////////////////////////////////

void CTuotuoCategoryCtrl::DoPaint(CDCHandle dc, const RECT &rcClip) const
{
	RECT rcClient;
	GetClientRect(&rcClient);

	dc.SetBkMode(TRANSPARENT);
	s()->Toolbar()->DrawRebarBackPart(dc, rcClient, m_hWnd);

	if (m_CateItems.empty())
		return;

	int iOffSetTop = rcClient.bottom - s()->Category()->GetCategoryHeight();
	if (iOffSetTop < 0)
		iOffSetTop = 0;
	rcClient.top += iOffSetTop;

	// draw scroll button
	if (m_bOverflowLeft || m_bOverflowRight/* || (m_bClosingTabs && m_iLastLayout == 2)*/)
	{
		if (m_bOverflowLeft)
		{
			s()->Category()->DrawCategoryScrollButton(dc, m_rcScrollLLeft, m_ePosition == CatePos_ScrollLLeftBtn ? m_eButtonStatus : Btn_MouseOut, 0);
		}
		else
		{
			s()->Category()->DrawCategoryScrollButton(dc, m_rcScrollLLeft, Btn_Disabled, 0);
		}

		if (m_bOverflowRight)
			s()->Category()->DrawCategoryScrollButton(dc, m_rcScrollRRight, m_ePosition == CatePos_ScrollRRightBtn ? m_eButtonStatus : Btn_MouseOut, 1);
		else
			s()->Category()->DrawCategoryScrollButton(dc, m_rcScrollRRight, Btn_Disabled, 1);
	}

	// draw sidebar button
	// draw "new tab" button
	//s()->Category()->DrawNewTabButton(dc, m_rcNewTabBtn, m_ePosition == POS_NewTabBtn ? m_eButtonStatus : Btn_MouseOut);

	dc.IntersectClipRect(&m_rcTabItemAreaDraw);
	dc.SetViewportOrg(m_iScrollOffsetAni + m_rcTabItemArea.left, 0);

	// we draw the selected item at the last
	dc.SelectFont(s()->Common()->GetDefaultFont());
	ButtonStatus eTabCloseBtnStatus = m_ePosition == CatePos_TabItemClose ? m_eButtonStatus : Btn_MouseOut;
	int iRightBorder = m_rcTabItemArea.right - m_rcTabItemArea.left - m_iScrollOffsetAni;
	size_t nScrollLeft = - m_iScrollOffsetAni / m_iCurrentWidth;
	size_t nScrollRight = iRightBorder / m_iCurrentWidth + (iRightBorder % m_iCurrentWidth == 0 ? 0 : 1);
	size_t nPaintRight = min(nScrollRight, m_CateItems.size() - 1);

	// 	for (size_t i = nScrollLeft == 0 ? 0 : nScrollLeft - 1; i <= nPaintRight; i++)
	// 		if (m_CateItems[i] != m_pSelectedItem)
	// 			m_CateItems[i]->DoPaint(dc, m_CateItems[i] == m_pHotItem ? Btn_MouseOver : Btn_MouseOut, m_CateItems[i] == m_pHotItem ? m_ePosition : CatePos_TabItem, m_eButtonStatus, m_eTabCloseButton == CloseBtn_AlwaysDisplay, iOffSetTop, false);
	int nLowIndex = nScrollLeft == 0 ? 0 : nScrollLeft - 1;
	int nHighIndex = nPaintRight;
	for (int i = nHighIndex; i >= nLowIndex; i--)
		if (m_CateItems[i] != m_pSelectedItem)
			m_CateItems[i]->DoPaint(dc, m_CateItems[i] == m_pHotItem ? Btn_MouseOver : Btn_MouseOut, m_CateItems[i] == m_pHotItem ? m_ePosition : CatePos_TabItem, m_eButtonStatus, m_eTabCloseButton == CloseBtn_AlwaysDisplay, iOffSetTop);

	dc.SelectFont(s()->Common()->GetBoldFont());
	if (m_pSelectedItem && CATEGORY_MATCH_INTERVAL(nScrollLeft, m_pSelectedItem->GetIndex(), nScrollRight))
		m_pSelectedItem->DoPaint(dc, Btn_MouseDown, m_pSelectedItem == m_pHotItem ? m_ePosition : CatePos_TabItem, m_eButtonStatus, m_eTabCloseButton != CloseBtn_AlwaysHide, iOffSetTop);

	dc.SelectClipRgn(NULL);
	dc.SetViewportOrg(0, 0);
}

bool CTuotuoCategoryCtrl::IsSecure()
{
	bool bIsSecure = false;
	if (m_pSelectedItem && m_pSelectedItem->ShowSecureColor())
		bIsSecure = true;

	return bIsSecure;
}


void CTuotuoCategoryCtrl::AddATabVisitIndex(CCategoryItem* pItem)
{
	ATLASSERT (NULL != pItem);
	if (NULL == pItem)
		return;

	if (m_nListVistorIndex.size () <= 0)
	{
		m_nListVistorIndex.push_back (pItem);
		return;
	}
	// 如果最近选择的项和最后一项相同，不添加
	////if (nIndex == m_nListVistorIndex.back ())
	//	return;

	m_nListVistorIndex.remove (pItem); // 去掉相同的

	m_nListVistorIndex.push_back (pItem);
}