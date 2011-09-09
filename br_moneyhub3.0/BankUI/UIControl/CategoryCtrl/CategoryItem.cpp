#include "stdafx.h"
#include "../ChildFrm.h"
#include "Util.h"
#include "CategoryItem.h"
#include "../TabCtrl/TabItem.h"

#define LOADING_ICON_INTERVAL		50
#define CATEGORY_ITEM_OVERLAP		6

CCategoryItem::CCategoryItem(CTuotuoCategoryCtrl *pTabCtrl)
: m_pCateCtrl(pTabCtrl)
, m_pWebData(0)
, m_nIndex(-1), m_bShowInfo(0)
, m_iXPos(0), m_iWidth(0)
, m_pSelectedItem(NULL)
{
	m_relateHwnd = NULL;
}

void CCategoryItem::DoPaint(CDCHandle dc, ButtonStatus eTabStatus, CategoryPosition btnPos, ButtonStatus eBtn, bool bDrawCloseBtn, int iDeltaHeight)
{
	// m_iXPos: x-value
	// iDeltaHeight: y-value
	// CATEGORY_ITEM_OVERLAP 重叠宽度

	RECT rcItem = { m_iXPos, iDeltaHeight, m_iXPos + m_iWidth + CATEGORY_ITEM_OVERLAP, iDeltaHeight + s()->Category()->GetCategoryHeight() };

	// 画背景
	s()->Category()->DrawCategory(dc, rcItem, eTabStatus);

	int iXTitlePos;

	// if this item is selected, draw Close btn
	// 画关闭图标
	if (bDrawCloseBtn && !GetWebData()->IsNoClose())
	{
		// 关闭图标的y坐标
		int iYCloseBtn = (s()->Category()->GetCategoryHeight() - s()->Category()->GetCategoryCloseButtonHeight()) / 2 + iDeltaHeight;

		RECT rcCloseBtn = { rcItem.right - s()->Category()->GetCategoryCloseButtonWidth() - s()->Category()->GetCategoryMargin(), iYCloseBtn,
			rcItem.right - s()->Category()->GetCategoryMargin(), iYCloseBtn + s()->Category()->GetCategoryCloseButtonHeight() };

		OffsetRect(&rcCloseBtn, -CATEGORY_ITEM_OVERLAP, 0);

		s()->Category()->DrawCategoryCloseButton(dc, rcCloseBtn, btnPos == CatePos_TabItemClose ? eBtn : Btn_MouseOut);
		iXTitlePos = rcCloseBtn.left;
	}
	else // 不画关闭图标
		iXTitlePos = rcItem.right - s()->Category()->GetCategoryMargin();


	bool bSelectedItem = false;
	if (m_pCateCtrl && m_pCateCtrl->GetCurrentSelection() == this)
		bSelectedItem = true;

	// 画logo图和Title
	// Draw the image.
	if (GetWebData() != NULL)
	{
		// 		int iIconWidth = GetWebData()->GetLogo().GetWidth();
		// 		int iIconHeight = GetWebData()->GetLogo().GetHeight();
		// 		POINT pt = { rcItem.left + s()->Category()->GetCategoryMargin(), (s()->Category()->GetCategoryHeight() - iIconHeight) / 2 + iDeltaHeight };
		// 		GetWebData()->GetLogo().Draw(dc, pt.x, pt.y + 3);

		// png icon
		int iIconWidth = GetWebData()->GetLogoPng(bSelectedItem).GetWidth();
		int iIconHeight = GetWebData()->GetLogoPng(bSelectedItem).GetHeight();
		POINT pt = { rcItem.left + s()->Tab()->GetTabMargin(), (s()->Tab()->GetTabHeight() - iIconHeight) / 2 + iDeltaHeight };
		CRect rcDest(pt.x, pt.y, pt.x + iIconWidth, pt.y + iIconHeight);
		rcDest.OffsetRect(0, 1);
		CRect rcSrc(0, 0, iIconWidth, iIconHeight);
		GetWebData()->GetLogoPng(bSelectedItem).ResetCacheDC(); // CacheDC没有正确设置，这里重新设置一下
		GetWebData()->GetLogoPng(bSelectedItem).Draw(dc, rcDest, rcSrc);

		if (ShowSecureLock())
		{
			int iIconHeight = 0;
			//POINT pt = { rcItem.left + s()->Tab()->GetTabMargin(), (s()->Tab()->GetTabHeight() - iIconHeight) / 2 + iDeltaHeight };
			POINT pt = { rcItem.left, rcItem.top };
			CRect rcMiniLock(pt.x, pt.y, pt.x + 15, pt.y + 16);
			OffsetRect(&rcMiniLock, 12, 10);
			s()->Tab()->DrawTabMiniLockButton(dc, rcMiniLock, (ButtonStatus)0);
		}

		dc.SetTextColor(eTabStatus == Btn_MouseDown ? RGB(16, 93, 145) : RGB(29, 126, 191));
		RECT rcText = { rcItem.left + s()->Category()->GetCategoryMargin() + 20 + CATEGORY_IMAGE_TEXT_MARGIN, 6, iXTitlePos - CATEGORY_TEXT_CLOSE_MARGIN, s()->Category()->GetCategoryHeight() };
		OffsetRect(&rcText, -1, -2);
		dc.DrawText(GetWebData()->GetName(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_NOPREFIX);
	}
}


//////////////////////////////////////////////////////////////////////////

void CCategoryItem::SetText(LPCTSTR sNewText)
{
	m_sText = sNewText;
	if (m_pCateCtrl)
		m_pCateCtrl->InvalidateTab(this, CateInv_Tab);
}

LPCTSTR CCategoryItem::GetText() const
{
	return m_sText.c_str();
}

//////////////////////////////////////////////////////////////////////////

void CCategoryItem::SetNewWidthImmediately()
{
	if (m_pCateCtrl)
		m_iWidth = m_pCateCtrl->m_iCurrentWidth;
}

void CCategoryItem::SetPosImmediately(int iPos)
{
	m_iXPos = iPos;
}

bool CCategoryItem::ShowSecureLock()
{
	bool bIsSecure = false;

	std::vector<CTabItem*>::iterator it = m_TabItems.begin();
	for (; it != m_TabItems.end(); ++it)
	{
		if ((*it)->GetSecure())
		{
			bIsSecure = true;
			break;
		}
	}
	
	return bIsSecure;
}

bool CCategoryItem::ShowSecureColor()
{
	bool bIsSecure = false;

	if (m_pSelectedItem && m_pSelectedItem->GetSecure())
		bIsSecure = true;

	return bIsSecure;
}