#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"


CMainFrameBorderSkin::CMainFrameBorderSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("mainframe_btn_close"), m_imgBtnClose);
	pSkin->RegisterFileName(_T("mainframe_btn_restore"), m_imgBtnRestore);
	pSkin->RegisterFileName(_T("mainframe_btn_min"), m_imgBtnMin);
	pSkin->RegisterFileName(_T("mainframe_btn_max"), m_imgBtnMax);
	pSkin->RegisterFileName(_T("mainframe_border"), m_imgBorder);
	pSkin->RegisterFileName(_T("mainframe_border_secure"), m_imgBorder_Secure);
}


void CMainFrameBorderSkin::Refresh()
{
	m_imgBtnClose.LoadFromFile();
	m_imgBtnRestore.LoadFromFile();
	m_imgBtnMin.LoadFromFile();
	m_imgBtnMax.LoadFromFile();
	m_imgBorder.LoadFromFile();
	m_imgBorder_Secure.LoadFromFile();

	ATLASSERT(m_imgBtnRestore.GetWidth() == m_imgBtnMax.GetWidth());
	ATLASSERT(m_imgBtnClose.GetHeight() == m_imgBtnRestore.GetHeight() && m_imgBtnRestore.GetHeight() == m_imgBtnMax.GetHeight() && m_imgBtnMax.GetHeight() == m_imgBtnMin.GetHeight());
}

//////////////////////////////////////////////////////////////////////////

void CMainFrameBorderSkin::DrawButton(CDCHandle dc, const RECT &rc, int iButtonType, UINT uButtonStatus)
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	CTuoImage *imgs[] = { &m_imgBtnMin, &m_imgBtnMax, &m_imgBtnClose, &m_imgBtnRestore };
	UINT uYPos = CSkinProperty::GetDWORD(Skin_Property_Frame_System_Button_Top_Margin) != -1 ? CSkinProperty::GetDWORD(Skin_Property_Frame_System_Button_Top_Margin) : (rc.bottom + rc.top - imgs[iButtonType]->GetHeight()) / 2 + 1;
	RECT rcDest = { rc.left, uYPos, rc.left + imgs[iButtonType]->GetWidth() / 3, uYPos + imgs[iButtonType]->GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, *imgs[iButtonType], btnStatus, 3);
}


void CMainFrameBorderSkin::DrawBorder(CDCHandle dc, const RECT &rc, bool bDrawBottomBorder, bool bSecure)
{
	CTuoImage* pImg = bSecure ? &m_imgBorder_Secure : &m_imgBorder;

	if (rc.top < rc.bottom)
	{
		if (bDrawBottomBorder)
		{
			RECT rcDestLeft = { 0, rc.top, GetBorderWidth(), rc.bottom };
			CSkinManager::DrawExtendsVertical(dc, rcDestLeft, *pImg, 0, pImg->GetHeight(), 0, GetBorderWidth(), pImg->GetHeight() / 2);
			RECT rcDestRight = { rc.right - GetBorderWidth(), rc.top, rc.right, rc.bottom };
			CSkinManager::DrawExtendsVertical(dc, rcDestRight, *pImg, 0, pImg->GetHeight(), pImg->GetWidth() - GetBorderWidth(), pImg->GetWidth(), pImg->GetHeight() / 2);
			RECT rcDestBottom = { rc.left + GetBorderWidth(), rc.bottom - GetBorderWidth(), rc.right - GetBorderWidth(), rc.bottom };
			CSkinManager::DrawExtends(dc, rcDestBottom, *pImg, GetBorderWidth(), pImg->GetWidth() - GetBorderWidth(), pImg->GetHeight() - GetBorderWidth(), pImg->GetHeight(), (pImg->GetWidth() - 2 * GetBorderWidth()) / 2);
		}
		else
		{
			RECT rcDest = { 0, rc.top, GetBorderWidth(), rc.bottom };
			CSkinManager::DrawExtendsVertical(dc, rcDest, *pImg, 0, pImg->GetHeight() - GetBorderWidth(), 0, GetBorderWidth(), (pImg->GetHeight() - GetBorderWidth()) / 2);
			RECT rcDest2 = { rc.right - GetBorderWidth(), rc.top, rc.right, rc.bottom };
			CSkinManager::DrawExtendsVertical(dc, rcDest2, *pImg, 0, pImg->GetHeight() - GetBorderWidth(), pImg->GetWidth() - GetBorderWidth(), pImg->GetWidth(), (pImg->GetHeight() - GetBorderWidth()) / 2);
		}
	}
}


void CMainFrameBorderSkin::DrawTitleBar(CDCHandle dc, const RECT &rc, LPCTSTR lpszTitle, bool bActive, bool bIsMax)
{
	int iIconWidth = 30;
	
	int iY = (rc.bottom + rc.top - iIconWidth) / 2;
	RECT rcText = { rc.left + iY + 4 + iIconWidth, rc.top + 2, rc.right, rc.bottom + 1 };
	//dc.SelectFont(s()->Common()->GetTitleFont());
	dc.SelectFont(s()->Common()->GetBoldFont());

	dc.SetBkMode(TRANSPARENT);
	//dc.SetTextColor(CSkinProperty::GetColor(bActive ? Skin_Property_Frame_Border_Title_Activate_Color : Skin_Property_Frame_Border_Title_Deactivate_Color));
	dc.SetTextColor(RGB(255, 255, 255));
	dc.DrawText(lpszTitle, -1, &rcText, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	//ico.DrawIconEx(dc, rc.left + iY, iY, ICON_WIDTH, ICON_WIDTH);
	s()->Common()->DrawMainIcon(dc, rc.left + iY, iY);
}
