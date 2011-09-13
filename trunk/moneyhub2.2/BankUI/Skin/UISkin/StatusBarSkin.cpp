#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"


#define TOOL_ICON_FRAME					15


CStatusBarSkin::CStatusBarSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("statusbar_bg"), m_bmpStatusBarBack);
	pSkin->RegisterFileName(_T("statusbar_bg_secure"), m_bmpStatusBarBack_Secure);
	pSkin->RegisterFileName(_T("statusbar_icon"), m_bmpStatusToolIcon);
	pSkin->RegisterFileName(_T("statusbar_icon_page"), m_bmpPageStatusIcon);
	pSkin->RegisterFileName(_T("statusbar_icon_ani_passport"), m_bmpStatusToolAni[0]);
	pSkin->RegisterFileName(_T("statusbar_icon_ani_download"), m_bmpStatusToolAni[1]);
	pSkin->RegisterFileName(_T("statusbar_icon_split"), m_bmpStatusSplit);
	pSkin->RegisterFileName(_T("statusbar_progressbar"), m_bmpStatusProgress);
	pSkin->RegisterFileName(_T("statusbar_progressbar2"), m_bmpStatusProgress2);
}

void CStatusBarSkin::Refresh()
{
	m_bmpStatusBarBack.LoadFromFile();
	m_bmpStatusBarBack_Secure.LoadFromFile();
	m_bmpStatusToolIcon.LoadFromFile();
	m_bmpPageStatusIcon.LoadFromFile();
	m_bmpStatusToolAni[0].LoadFromFile();
	m_bmpStatusToolAni[1].LoadFromFile();

	m_bmpStatusSplit.LoadFromFile();
	m_bmpStatusProgress.LoadFromFile();
	m_bmpStatusProgress2.LoadFromFile();
}

//////////////////////////////////////////////////////////////////////////

RECT CStatusBarSkin::AdjustToolRect(const RECT &rcBtn, int iIconIndex)
{
	int iX = (rcBtn.right - rcBtn.left - GetStatusToolWidth()) / 2;
	int iY = (s()->Common()->GetButtonMaskHeight() - m_bmpStatusToolIcon.GetHeight()) / 2;
	RECT rcIcon = { rcBtn.left + iX, rcBtn.top + iY, rcBtn.left + iX + GetStatusToolWidth(), rcBtn.top + iY + m_bmpStatusToolIcon.GetHeight() };
	return rcIcon;
}

int CStatusBarSkin::GetStatusBarHeight(bool bIsWindowMax)
{
	return m_bmpStatusBarBack.GetHeight();
}

int CStatusBarSkin::GetPageStatusIconWidth()
{
	return m_bmpPageStatusIcon.GetWidth() / 8;
}

int CStatusBarSkin::GetPageStatusIconHeight()
{
	return m_bmpPageStatusIcon.GetHeight();
}

int CStatusBarSkin::GetStatusToolWidth()
{
	return m_bmpStatusToolIcon.GetWidth() / TOOL_ICON_FRAME;
}

//////////////////////////////////////////////////////////////////////////

void CStatusBarSkin::DrawStatusPageIcon(CDCHandle dc, const RECT &rect, int iIndex)
{
	int iWidth = m_bmpPageStatusIcon.GetWidth() / 8;
	RECT rcDest = { rect.left, rect.top, rect.left + iWidth, rect.top + m_bmpPageStatusIcon.GetHeight() };
	RECT rcSrc = { iWidth * iIndex, 0, iWidth * iIndex + iWidth, m_bmpPageStatusIcon.GetHeight() };
	m_bmpPageStatusIcon.Draw(dc, rcDest, rcSrc);
}

void CStatusBarSkin::DrawStatusSplit(CDCHandle dc, const RECT &rect)
{
	int iWidth = m_bmpStatusSplit.GetWidth();
	int iHeight = m_bmpStatusSplit.GetHeight();

	RECT rcDest = { rect.left, rect.top, rect.left + iWidth, rect.top + iHeight };
	RECT rcSrc = { 0, 0, iWidth, iHeight };
	m_bmpStatusSplit.Draw(dc, rcDest, rcSrc);
}

void CStatusBarSkin::DrawStatusProgress(CDCHandle dc, const RECT& rect, DWORD dwProgress)
{
	int iWidth = m_bmpStatusProgress.GetWidth();
	int iHeight = m_bmpStatusProgress.GetHeight();

	RECT rcDest = { rect.left, rect.top, rect.left + iWidth, rect.top + iHeight };
	RECT rcSrc = { 0, 0, iWidth, iHeight };
	m_bmpStatusProgress.Draw(dc, rcDest, rcSrc);
	
 	RECT rcSrcOverlay = { 0, 0, 9, 8 };
 	RECT rcDestCenter = { 0, 0, dwProgress * 128 / 100 + 9, 8 };
 	OffsetRect(&rcDestCenter, rect.left + 5, rect.top + 4);
	CSkinManager::DrawExtends(dc, rcDestCenter, m_bmpStatusProgress2, 0, 9);

	//128
}

void CStatusBarSkin::DrawCheckProgress(CDCHandle dc, const RECT& rect, DWORD dwProgress)
{
	int iWidth = m_bmpStatusProgress.GetWidth();
	int iHeight = m_bmpStatusProgress.GetHeight();

	RECT rcDest = { rect.left, rect.top, rect.left + 240, rect.top + iHeight };
	CSkinManager::DrawExtends(dc, rcDest, m_bmpStatusProgress, 0, iWidth);

	RECT rcSrcOverlay = { 0, 0, 9, 8 };
	RECT rcDestCenter = { 0, 0, dwProgress * 221 / 100 + 9, 8 };
	OffsetRect(&rcDestCenter, rect.left + 5, rect.top + 4);
	CSkinManager::DrawExtends(dc, rcDestCenter, m_bmpStatusProgress2, 0, 9);
}

void CStatusBarSkin::DrawStatusBarBack(CDCHandle dc, const RECT &rect, bool bIsMax, int iDrawType, bool bSecure)
{
	CTuoImage* pImg = bSecure ? &m_bmpStatusBarBack_Secure : &m_bmpStatusBarBack;

	// iDrawType = 0表示标签不在下面，状态栏的skin只有一种情况，=1表示tab在下面，且tab显示，=2表示tab在下面，但是tab隐藏
	// m_bmpStatusBarBack在iDrawType=0情况下，只有一组图片，在iDrawType=1/2时，有两组图片，分别用在tab显示和不显示的情况下
	if (bIsMax)
	{
		int iSrcStart = iDrawType == 0 ? (pImg->GetWidth() / 2) : (iDrawType == 1 ? (pImg->GetWidth() / 4) : (pImg->GetWidth() / 4) * 3);
		RECT rcSrc = { iSrcStart, 0, iSrcStart + 1, pImg->GetHeight() };
		pImg->Draw(dc, rect, rcSrc);
	}
	else
	{
		int iLeft = iDrawType == 2 ? (pImg->GetWidth() / 2) : 0;
		int iSize = iDrawType == 0 ? pImg->GetWidth() : pImg->GetWidth() / 2;
		CSkinManager::DrawExtends(dc, rect, *pImg, iLeft, iLeft + iSize);
	}
}

void CStatusBarSkin::DrawStatusBarBackPart(CDCHandle dc, const RECT &rect, HWND hCurrentWnd, CWindow wndStatusBar, int iDrawType)
{
	RECT rcWnd;
	::GetWindowRect(hCurrentWnd, &rcWnd);
	wndStatusBar.ScreenToClient(&rcWnd);

	// iDrawType = 0表示标签不在下面，状态栏的skin只有一种情况，=1表示tab在下面，且tab显示，=2表示tab在下面，但是tab隐藏
	// m_bmpStatusBarBack在iDrawType=0情况下，只有一组图片，在iDrawType=1/2时，有两组图片，分别用在tab显示和不显示的情况下
	int iSrcStart = iDrawType == 0 ? (m_bmpStatusBarBack.GetWidth() / 2) : (iDrawType == 1 ? (m_bmpStatusBarBack.GetWidth() / 4) : (m_bmpStatusBarBack.GetWidth() / 4) * 3);
	RECT rcSrc = { iSrcStart, rcWnd.top, iSrcStart + 1, rcWnd.top + rect.bottom - rect.top };
	m_bmpStatusBarBack.Draw(dc, rect, rcSrc);
}


void CStatusBarSkin::DrawStatusBarTool(CDCHandle dc, const RECT &rcBtn, int iIconIndex, UINT uStatus)
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uStatus);
	s()->Common()->DrawButtonBack(dc, rcBtn, btnStatus);

	// draw the icon
	int iX = (rcBtn.right - rcBtn.left - GetStatusToolWidth()) / 2;
	int iY = (s()->Common()->GetButtonMaskHeight() - m_bmpStatusToolIcon.GetHeight()) / 2 + (btnStatus == Btn_MouseDown ? 1 : 0);
	RECT rcIcon = { rcBtn.left + iX, rcBtn.top + iY, rcBtn.left + iX + GetStatusToolWidth(), rcBtn.top + iY + m_bmpStatusToolIcon.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcIcon, m_bmpStatusToolIcon, iIconIndex, TOOL_ICON_FRAME);
}

void CStatusBarSkin::DrawStatusBarToolWithText(CDCHandle dc, const RECT &rcBtn, int iIconIndex, UINT uStatus, LPCTSTR lpszText)
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uStatus);
	s()->Common()->DrawButtonBack(dc, rcBtn, btnStatus);

	// draw the icon
	int iX = 5;
	int iY = (s()->Common()->GetButtonMaskHeight() - m_bmpStatusToolIcon.GetHeight()) / 2 + (btnStatus == Btn_MouseDown ? 1 : 0);
	RECT rcIcon = { rcBtn.left + iY, rcBtn.top + iY, rcBtn.left + iY + GetStatusToolWidth(), rcBtn.top + iY + m_bmpStatusToolIcon.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcIcon, m_bmpStatusToolIcon, iIconIndex, TOOL_ICON_FRAME);

	RECT rcText = {
		rcBtn.left + iY + GetStatusToolWidth() + 1,
		(btnStatus == Btn_MouseDown ? 1 : 0),
		rcBtn.right - TOOLBAR_MARGIN,
		rcBtn.bottom + (btnStatus == Btn_MouseDown ? 1 : 0)
	};
	dc.SetTextColor(CSkinProperty::GetColor(Skin_Property_Statusbar_Text_Color));
	dc.SelectFont(s()->Common()->GetDefaultFont());

	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(lpszText, -1, &rcText, DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_VCENTER);
}
