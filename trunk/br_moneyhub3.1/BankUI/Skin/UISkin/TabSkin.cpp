#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"


CTabSkin::CTabSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("tab_bg"), m_bmpTabBackground);
	pSkin->RegisterFileName(_T("tab_item"), m_bmpTabItem);
	pSkin->RegisterFileName(_T("tab_btn_scrollleft"), m_bmpTabScrollLeft);
	pSkin->RegisterFileName(_T("tab_btn_scrollright"), m_bmpTabScrollRight);
	pSkin->RegisterFileName(_T("tab_btn_close"), m_bmpTabClose);
	pSkin->RegisterFileName(_T("tab_btn_sidebar"), m_bmpSidebar);
	pSkin->RegisterFileName(_T("tab_btn_new"), m_bmpNewTab);
	pSkin->RegisterFileName(_T("tab_ani_working"), m_bmpTabLoading);
	pSkin->RegisterFileName(_T("tab_icon_status"), m_imgTabAdditonalIcon);
	pSkin->RegisterFileName(_T("video_res_icon"), m_bmpResIcon);
	pSkin->RegisterFileName(_T("mini_lock"), m_bmpMiniLock);
	pSkin->RegisterFileName(_T("mini_page"), m_bmpMiniPage);
}

void CTabSkin::Refresh()
{
	m_bmpTabBackground.LoadFromFile();
	m_bmpTabItem.LoadFromFile();
	m_bmpTabScrollLeft.LoadFromFile();
	m_bmpTabScrollRight.LoadFromFile();
	m_bmpTabClose.LoadFromFile();
	m_bmpSidebar.LoadFromFile();
	m_bmpNewTab.LoadFromFile();
	m_bmpTabLoading.LoadFromFile();
	m_imgTabAdditonalIcon.LoadFromFile();
	m_bmpResIcon.LoadFromFile();
	m_bmpMiniLock.LoadFromFile();
	m_bmpMiniPage.LoadFromFile();

	m_iTabLoadingFrame = m_bmpTabLoading.GetWidth() / m_bmpTabLoading.GetHeight();

	ATLASSERT(m_bmpTabScrollLeft.GetWidth() == m_bmpTabScrollRight.GetWidth());
	ATLASSERT(m_bmpTabScrollLeft.GetHeight() == m_bmpTabScrollRight.GetHeight());
}

//////////////////////////////////////////////////////////////////////////

void CTabSkin::DrawTab(CDCHandle dc, const RECT &rcTab, ButtonStatus eStatus, int iCategory)
{
	RECT rcTabNew = rcTab;
	rcTabNew.left -= GetTabLeftOverlap();
	rcTabNew.right += GetTabRightOverlap();
	int iWidth = m_bmpTabItem.GetWidth() / 3;
	int iBegin = iWidth * (iCategory * 3 + eStatus);
//	CSkinManager::DrawExtends(dc, rcTabNew, m_bmpTabItem, iBegin, iBegin + iWidth);
	CSkinManager::DrawOverlayExtendsPart(dc, rcTabNew, m_bmpTabItem, iWidth / 2 - 5, iWidth / 2 - 5, iBegin, iBegin + iWidth);
}


void CTabSkin::DrawTabScrollButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus, int iType)
{
	CSkinManager::DrawImagePart(dc, rcBtn, iType == 0 ? m_bmpTabScrollLeft : m_bmpTabScrollRight, eStatus, 4);
}

void CTabSkin::DrawTabCloseButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus)
{
	CSkinManager::DrawImagePart(dc, rcBtn, m_bmpTabClose, eStatus, 3);
}

void CTabSkin::DrawSidebarButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus, bool bIsOpen)
{
	CSkinManager::DrawImagePart(dc, rcBtn, m_bmpSidebar, bIsOpen ? eStatus + 3 : eStatus, 6);
}

void CTabSkin::DrawNewTabButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus)
{
	CSkinManager::DrawImagePart(dc, rcBtn, m_bmpNewTab, eStatus, 3);
}

int CTabSkin::DrawTabLoadingIcon(CDCHandle dc, int left, int top, int &iProgress)
{
	RECT rcDest = { left, top, left + m_bmpTabLoading.GetHeight(), top + m_bmpTabLoading.GetHeight() };
	//CSkinManager::DrawImagePart(dc, rcDest, m_bmpTabLoading, iProgress, m_iTabLoadingFrame);
	return (iProgress + 1) % m_iTabLoadingFrame;
}

void CTabSkin::DrawResourceButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus, int iResType)
{
	CSkinManager::DrawImagePart(dc, rcBtn, m_bmpResIcon, eStatus, 3);
}

void CTabSkin::DrawTabAdditionalIcon(CDCHandle dc, int left, int top, int iIconIndex)
{
	RECT rcDest = { left + ICON_WIDTH - m_imgTabAdditonalIcon.GetWidth() / 4, top + ICON_WIDTH - m_imgTabAdditonalIcon.GetHeight(), left + ICON_WIDTH, top + ICON_WIDTH };
	CSkinManager::DrawImagePart(dc, rcDest, m_imgTabAdditonalIcon, iIconIndex, 4);
}

void CTabSkin::DrawTabMiniLockButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus)
{
	CSkinManager::DrawImagePart(dc, rcBtn, m_bmpMiniLock, 0, 1);
}

void CTabSkin::DrawTabMiniPageButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus)
{
	CSkinManager::DrawImagePart(dc, rcBtn, m_bmpMiniPage, 0, 1);
}