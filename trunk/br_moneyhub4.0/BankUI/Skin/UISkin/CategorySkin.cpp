#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"


CCategorySkin::CCategorySkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("cate_item"), m_bmpTabItem);
	pSkin->RegisterFileName(_T("cate_btn_scrollleft"), m_bmpTabScrollLeft);
	pSkin->RegisterFileName(_T("cate_btn_scrollright"), m_bmpTabScrollRight);
	pSkin->RegisterFileName(_T("cate_btn_close"), m_bmpTabClose);
	pSkin->RegisterFileName(_T("cate_btn_new"), m_bmpNewTab);
}

void CCategorySkin::Refresh()
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

	m_iTabLoadingFrame = m_bmpTabLoading.GetWidth() / m_bmpTabLoading.GetHeight();

	ATLASSERT(m_bmpTabScrollLeft.GetWidth() == m_bmpTabScrollRight.GetWidth());
	ATLASSERT(m_bmpTabScrollLeft.GetHeight() == m_bmpTabScrollRight.GetHeight());
}

//////////////////////////////////////////////////////////////////////////

void CCategorySkin::DrawCategory(CDCHandle dc, const RECT &rcTab, ButtonStatus eStatus)
{
	RECT rcTabNew = rcTab;
	rcTabNew.left -= GetCategoryLeftOverlap();
	rcTabNew.right += GetCategoryRightOverlap();
	int iWidth = m_bmpTabItem.GetWidth() / 3;
	int iBegin = iWidth * eStatus;
//	CSkinManager::DrawExtends(dc, rcTabNew, m_bmpTabItem, iBegin, iBegin + iWidth);
	CSkinManager::DrawOverlayExtendsPart(dc, rcTabNew, m_bmpTabItem, iWidth / 2 - 5, iWidth / 2 - 5, iBegin, iBegin + iWidth);
}


void CCategorySkin::DrawCategoryScrollButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus, int iType)
{
	CSkinManager::DrawImagePart(dc, rcBtn, iType == 0 ? m_bmpTabScrollLeft : m_bmpTabScrollRight, eStatus, 4);
}

void CCategorySkin::DrawCategoryCloseButton(CDCHandle dc, const RECT &rcBtn, ButtonStatus eStatus)
{
	CSkinManager::DrawImagePart(dc, rcBtn, m_bmpTabClose, eStatus, 3);
}
