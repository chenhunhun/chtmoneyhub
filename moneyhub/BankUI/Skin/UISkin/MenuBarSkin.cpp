#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"


CMenuBarSkin::CMenuBarSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("menubar_btn_bg"), m_menuButtonBack);
}

void CMenuBarSkin::Refresh()
{
	m_menuButtonBack.LoadFromFile();
}

//////////////////////////////////////////////////////////////////////////

void CMenuBarSkin::DrawMenuBarBack(CDCHandle dc, LPCTSTR lpszText, const RECT &rcBtn, ButtonStatus btnStatus, bool bUnderAero)
{
	int iFullSize = m_menuButtonBack.GetWidth() / 3;
	int iBegin = iFullSize * btnStatus;
	int iY = (rcBtn.bottom + rcBtn.top - m_menuButtonBack.GetHeight()) / 2;
	RECT rcDest = { rcBtn.left, iY, rcBtn.right, iY + m_menuButtonBack.GetHeight() };
	CSkinManager::DrawExtends(dc, rcDest, m_menuButtonBack, iBegin, iBegin + iFullSize);

	RECT rcText = rcBtn;
	if (btnStatus == Btn_MouseDown)
	{
		rcText.top ++;
		rcText.bottom ++;
	}

	dc.SelectFont(s()->Common()->GetDefaultFont());
	if (bUnderAero)
	{
		dc.SetTextColor(CSkinProperty::GetColor(Skin_Property_Toolbar_Text_Aero_Color));
		bool bIsGlow = (CSkinProperty::GetDWORD(Skin_Property_Toolbar_Text_Aero_Glow) >> (btnStatus < 3 ? btnStatus : 0)) & 1;
		CSkinManager::DrawGlowText(dc, lpszText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE, bIsGlow);
	}
	else
	{
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(CSkinProperty::GetColor(Skin_Property_Toolbar_Text_Color));
		dc.DrawText(lpszText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
}
