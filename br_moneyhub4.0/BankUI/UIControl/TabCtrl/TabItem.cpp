#include "stdafx.h"
#include "../ChildFrm.h"
#include "Util.h"
#include "../CategoryCtrl/CategoryItem.h"
#include "TabItem.h"

#define LOADING_ICON_INTERVAL		50

CTabItem::CTabItem(bool bNoClose) : m_bNoClose(bNoClose), m_pTabCtr(NULL),
		m_pChildFrame(NULL), m_hIcon(NULL), m_iPageWeight(0), m_iCoreType(-1), m_iCoreTypeSource(0), m_bSwitchCoreUsed(false),
		m_iLoadingProgressFrame(0), m_dwLastUpdateLoadingProgressTime(0), m_iLoadingProgress(0), m_iStatusIcon(0),
		m_bIsBackEnabled(false), m_bIsForwardEnabled(false), m_ePageType(PageType_Blank), m_eSpecialPageType(SpecialPage_Null),
		m_bFindHiLight(false), m_bFindCaseSensitive(false), m_iTotalFind(-1), m_iCurrentFind(-1), m_bIsFinding(false),
		m_nIndex(-1), m_iSearchEngine(-1), m_iZoom(GetInitZoomValue()),
		m_iXPos(0), m_iXPosAni(0), m_iXPosAniFrame(FRAME_COUNT), m_iWidth(0), m_iWidthAni(0), m_iWidthAniFrame(FRAME_COUNT), m_iWidthOld(0),
		m_iExceptionCount(0), m_dwLastExceptionCode(0), m_dwRequestNum(0),
		m_hThumbnailProxy(NULL), m_eStartPageType(StartPage_Null), m_iAdditionalDataType(-1), m_bDisplayAutofillTip(false),
		m_pCategory(NULL), m_bIsSecure(false)
{
}

void CTabItem::ChangeItemParent(CTuotuoTabCtrl *pTabCtr)
{
	if (m_pTabCtr == pTabCtr)
		return;
/*
	if (m_iWidthAniFrame < FRAME_COUNT)
	{
		if (m_pTabCtr)
			m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_WidthChange);
		if (pTabCtr)
			pTabCtr->IncOrDecAnimationCounter(true, this, TabInv_WidthChange);
	}
	if (m_iXPosAniFrame < FRAME_COUNT)
	{
		if (m_pTabCtr)
			m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_PosChange);
		if (pTabCtr)
			pTabCtr->IncOrDecAnimationCounter(true, this, TabInv_PosChange);
	}
	if (m_iLoadingProgressFrame >= 0)
	{
		if (m_pTabCtr)
			m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_Icon);
		if (pTabCtr)
			pTabCtr->IncOrDecAnimationCounter(true, this, TabInv_Icon);
	}
*/
	ATLASSERT(m_pTabCtr == NULL || m_pTabCtr->m_AniSet.find(this) == m_pTabCtr->m_AniSet.end());
	m_pTabCtr = pTabCtr;
}


void CTabItem::DoPaint(CDCHandle dc, ButtonStatus eTabStatus, Position btnPos, ButtonStatus eBtn, bool bDrawCloseBtn, int iDeltaHeight, bool bAero)
{
	RECT rcItem = { m_iXPosAni, iDeltaHeight, m_iXPosAni + m_iWidthAni, iDeltaHeight + s()->Tab()->GetTabHeight() };

	s()->Tab()->DrawTab(dc, rcItem, eTabStatus, 0);

	int iXPos;

	// if this item is selected, draw Close btn
	if (!m_bNoClose && bDrawCloseBtn)
	{
		int iY = (s()->Tab()->GetTabHeight() - s()->Tab()->GetTabCloseButtonHeight() + CSkinProperty::GetDWORD(Skin_Property_Tabbar_Text_Top_Margin)) / 2 + iDeltaHeight;
		RECT rcClose = { rcItem.right - s()->Tab()->GetTabCloseButtonWidth() - s()->Tab()->GetTabMargin(), iY,
			rcItem.right - s()->Tab()->GetTabMargin(), iY + s()->Tab()->GetTabCloseButtonHeight() };
		s()->Tab()->DrawTabCloseButton(dc, rcClose, btnPos == POS_TabItemClose ? eBtn : Btn_MouseOut);
		iXPos = rcClose.left;
	}
	else
		iXPos = rcItem.right - s()->Tab()->GetTabMargin();

	// Draw the image.

	if (eTabStatus == Btn_MouseDown)
		dc.SetTextColor(RGB(255, 255, 255));
	else
		dc.SetTextColor(RGB(29, 126, 191));

	RECT rcText = { rcItem.left + s()->Tab()->GetTabMargin() + 14 + IMAGE_TEXT_MARGIN, 1, iXPos - TEXT_CLOSE_MARGIN, s()->Tab()->GetTabHeight() - 1 };
	dc.DrawText(GetText(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_NOPREFIX);

	int iIconHeight = 0;
	//POINT pt = { rcItem.left + s()->Tab()->GetTabMargin(), (s()->Tab()->GetTabHeight() - iIconHeight) / 2 + iDeltaHeight };
	POINT pt = { rcItem.left, rcItem.top };
	CRect rcMiniLock(pt.x, pt.y, pt.x + 15, pt.y + 16);
	OffsetRect(&rcMiniLock, 6, 7);
	if (m_bIsSecure)
		s()->Tab()->DrawTabMiniLockButton(dc, rcMiniLock, (ButtonStatus)0);
	else
		s()->Tab()->DrawTabMiniPageButton(dc, rcMiniLock, (ButtonStatus)0);
}

bool CTabItem::NeedRefreshLoadingIcon() const
{
	if (m_iLoadingProgressFrame >= 0)
	{
		if (::GetTickCount() - m_dwLastUpdateLoadingProgressTime > LOADING_ICON_INTERVAL)
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

void CTabItem::SetToDefault()
{
	m_pChildFrame = NULL;
	m_wndAxControl = NULL;
	m_hThumbnailProxy = NULL;
	m_iCoreType = -1;

/*
	if (m_iLoadingProgressFrame < 0 && m_pTabCtr)
		m_pTabCtr->IncOrDecAnimationCounter(true, this, TabInv_Icon);
*/
	m_iLoadingProgressFrame = 0;
	SetIcon(NULL);

	++m_dwRequestNum ;

	m_sDatabaseURL = _T("");
	m_iStatusIcon = 0;
	m_iLoadingProgress = 100;
	m_bIsBackEnabled = m_bIsForwardEnabled = false;

	m_iSearchEngine = -1;
	m_strSearchString = _T("");
	m_bSearchHilight = false;

	m_strFindText = _T("");
	m_iTotalFind = m_iCurrentFind = -1;

	m_iFontSize = 0;
	m_iZoom = GetInitZoomValue(); 

	for (int i = 0; i < RESOURCE_TYPE_COUNT; i++)
		m_strResourceTitle[i].clear();

	m_strBlockedFloatingElements.clear();
	m_strBlockedPopups.clear();
}

void CTabItem::SetAxControl(HWND hAxWnd)
{
	m_wndAxControl = hAxWnd;
	if (hAxWnd == NULL)
	{
		m_dwAxThreadID = 0;
		m_strFindText = _T("");
		m_iTotalFind = m_iCurrentFind = -1;
		for (int i = 0; i < RESOURCE_TYPE_COUNT; i++)
			m_strResourceTitle[i].clear();
		m_strBlockedFloatingElements.clear();
		m_strBlockedPopups.clear();
	}
	else
	{
		m_dwAxThreadID = ::GetWindowThreadProcessId(m_wndAxControl, NULL);
	}
}


CWindow CTabItem::GetTabView() const
{
	return m_pChildFrame->m_hWnd;
}

bool CTabItem::CanBeOverwrite() const
{
	// 如果是空白页，则这个页面可以被覆盖（即可以被重复利用）
	if (m_ePageType == PageType_Blank)
		return true;
	// 另外，弱智黎志要求se:tab也要被重复利用
	if (m_ePageType == PageType_SpecialPage && m_eSpecialPageType == SpecialPage_TabStartPage)
		return true;
	return false;
}

bool CTabItem::IsCloseBorder() const
{
	// 如果返回true，表示这个是关闭的边界，即如果只剩这一个标签，就不允许再关闭了（只允许替换其内容）
	if (m_ePageType == PageType_Blank)
		return true;
	if (m_ePageType == PageType_SpecialPage && m_eSpecialPageType == SpecialPage_StartPage)
		return true;
	return false;
}

void CTabItem::SetURLText(LPCTSTR lpszURL)
{
	m_sURL = lpszURL;
	std::wstring strDomain = ExtractTopDomain(lpszURL);

#ifndef SINGLE_PROCESS
		CListManager::_()->CheckHMAC();

#endif
	if (m_pTabCtr)
		m_pTabCtr->InvalidateTab(this, TabInv_Tab);
}

void CTabItem::SetText(LPCTSTR sNewText)
{
	m_sText = sNewText;
	if (m_pTabCtr)
		m_pTabCtr->InvalidateTab(this, TabInv_Tab);
}



LPCTSTR CTabItem::GetText() const
{
	return m_sText.c_str();
}

bool CTabItem::IsPageViewSource() const
{
	return _tcsnicmp(m_sText.c_str(), _T("view-source:"), 12) == 0;
}

void CTabItem::SetIcon(HICON hIcon)
{
	m_hIcon = hIcon;
	m_strLastIconName.clear();
	if (m_pTabCtr)
		m_pTabCtr->InvalidateTab(this, TabInv_Icon);
}

bool CTabItem::LoadIconFromURL(LPCTSTR lpszURL, HWND hCallbackWnd)
{
	return true;
}



void CTabItem::SetProgress(int iProgress)
{
	m_iLoadingProgress = iProgress;
	if (m_iLoadingProgress == 100)
	{
		if (m_iLoadingProgressFrame >= 0)
		{
			m_iLoadingProgressFrame = -1;
			if (m_pTabCtr)
			{
				//m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_Icon);
				m_pTabCtr->InvalidateTab(this, TabInv_Icon);
			}
		}
	}
	else
	{
		if (m_iLoadingProgressFrame == -1)
		{
			m_iLoadingProgressFrame = 0;
			//if (m_pTabCtr)
			//	m_pTabCtr->IncOrDecAnimationCounter(true, this, TabInv_Icon);
		}
	}
}

void CTabItem::SetAdditinalData(LPCTSTR lpszAdditionalData, int iType)
{
	m_iAdditionalDataType = iType;
	if (iType >= 0)
		m_sAdditionalData = lpszAdditionalData;
	else
		m_sAdditionalData.clear();
}

void CTabItem::SetMainToolbarStatus(int iType, bool bEnabled)
{
	if (iType == 0)
		m_bIsBackEnabled = bEnabled;
	else if (iType == 1)
		m_bIsForwardEnabled = bEnabled;
}

void CTabItem::SetSearchBarStatus(LPCTSTR lpszSearchString, short iHiLight, short iSearchEngine, bool bForce)
{
	if (lpszSearchString)
		m_strSearchString = lpszSearchString;
	if (iHiLight >= 0 || bForce)
		m_bSearchHilight = iHiLight != 0;
	if (iSearchEngine >= 0 || bForce)
		m_iSearchEngine = iSearchEngine;
}

DWORD CTabItem::GetInitZoomValue()
{
	return 100;
}

int CTabItem::SetResourceStringList(LPCTSTR szList)
{
	std::tstring str = szList;
	size_t p;
	size_t iReturnCount = 0;
	size_t iLastPos = 0;
	std::tstring strTitle;
	int iType = 0;
	m_strResourceTitle[iType].clear();
	while ((p = str.find('\n', iLastPos)) != -1)
	{
		iReturnCount ++;
		if (iReturnCount == 1)
			iType = _ttoi(str.substr(iLastPos, p - iLastPos).c_str());
		else if (iReturnCount % 2 == 0)
			strTitle = str.substr(iLastPos, p - iLastPos);
		else
		{
			strTitle.append(_T("\n"));
			strTitle.append(str.substr(iLastPos, p - iLastPos));
			m_strResourceTitle[iType].push_back(strTitle);
		}
		iLastPos = p + 1;
	}
	if (m_pTabCtr)
		m_pTabCtr->InvalidateTab(this, TabInv_Text);
	return iType;
}

//////////////////////////////////////////////////////////////////////////

void CTabItem::SetNewWidth()
{
	if (m_pTabCtr && m_iWidth != m_pTabCtr->m_iCurrentWidth)
	{
		//if (m_iWidthAniFrame == FRAME_COUNT)
		//	m_pTabCtr->IncOrDecAnimationCounter(true, this, TabInv_WidthChange);
		m_iWidthOld = m_iWidthAni;
		m_iWidthAniFrame = 0;
		m_iWidth = m_pTabCtr->m_iCurrentWidth;
	}
}

void CTabItem::SetNewWidthImmediately()
{
	if (m_pTabCtr)
	{
		//if (m_iWidthAniFrame != FRAME_COUNT)
		//	m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_WidthChange);
		m_iWidth = m_iWidthAni = m_pTabCtr->m_iCurrentWidth;
		m_iWidthAniFrame = FRAME_COUNT;
	}
}

void CTabItem::SetPos(int iPos)
{
	if (m_iXPos != iPos)
	{
		//if (m_pTabCtr && m_iXPosAniFrame == FRAME_COUNT)
		//	m_pTabCtr->IncOrDecAnimationCounter(true, this, TabInv_PosChange);
		m_iXPosAniFrame = 0;
		m_iXPosOld = m_iXPosAni;
		m_iXPos = iPos;
	}
}
void CTabItem::SetPosImmediately(int iPos)
{
	//if (m_pTabCtr && m_iXPosAniFrame != FRAME_COUNT)
	//	m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_PosChange);
	m_iXPos = m_iXPosAni = iPos;
	m_iXPosAniFrame = FRAME_COUNT;
}


void CTabItem::UpdateAnimateXPos()
{
	if (m_iXPosAniFrame < FRAME_COUNT)
	{
		m_iXPosAni = m_iXPosOld * (1000 - g_iMotionPath[m_iXPosAniFrame]) / 1000 + m_iXPos * g_iMotionPath[m_iXPosAniFrame] / 1000;
		m_iXPosAniFrame ++;
		//if (m_pTabCtr && m_iXPosAniFrame == FRAME_COUNT)
		//	m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_PosChange);
	}
}
void CTabItem::UpdateAnimateWidth()
{
	if (m_pTabCtr && m_iWidthAniFrame < FRAME_COUNT)
	{
		m_iWidthAni = m_iWidthOld * (1000 - g_iMotionPath[m_iWidthAniFrame]) / 1000 + m_pTabCtr->m_iCurrentWidth * g_iMotionPath[m_iWidthAniFrame] / 1000;
		m_iWidthAniFrame ++;
		//if (m_iWidthAniFrame == FRAME_COUNT)
		//	m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_WidthChange);
	}
}


/*
void CTabItem::FinishAnimation(CTuotuoTabCtrl *pOPTab)
{
	if (pOPTab != m_pTabCtr)
	{
		ATLASSERT(pOPTab->m_AniSet.find(this) == pOPTab->m_AniSet.end());
		return;
	}
	if (m_iWidthAniFrame < FRAME_COUNT)
	{
		m_iWidthAniFrame = FRAME_COUNT;
		m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_WidthChange);
	}
	if (m_iXPosAniFrame < FRAME_COUNT)
	{
		m_iXPosAniFrame = FRAME_COUNT;
		m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_PosChange);
	}
	if (m_iLoadingProgressFrame >= 0)
	{
		m_iLoadingProgressFrame = -1;
		m_pTabCtr->IncOrDecAnimationCounter(false, this, TabInv_Icon);
	}
	ATLASSERT(m_pTabCtr && m_pTabCtr->m_AniSet.find(this) == m_pTabCtr->m_AniSet.end());
	m_pTabCtr = NULL;
}*/

