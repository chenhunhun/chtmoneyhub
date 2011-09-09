#include "stdafx.h"
#include "../Skin/SkinManager.h"
#include "TabCtrl/TabCtrl.h"
#include "BrowserToolbar.h"
#include "ShowInfoBar.h"


CShowInfoBar::CShowInfoBar(FrameStorageStruct *pFS) : CFSMUtil(pFS),m_fontText(NULL)
{
	FS()->pShowInfoBar = this;
	m_state = eShowInfo;
}
CShowInfoBar::~CShowInfoBar()
{
}

void CShowInfoBar::CreateShowInfoBar(HWND hParent)
{
	HWND hWnd  = Create(hParent, CRect(0, 0, 10, FS()->ToolBar()->GetHeight()), NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd && "Create main titlebar failed.");

	if (NULL == m_fontText)
		m_fontText.CreateFont(13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));
	m_crTextColor = RGB(61, 98, 123);
	m_strText = L"²âÊÔ";
	/*RECT rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);

	HRGN hRgn = ::CreateRoundRectRgn(0, 0, rect.right - rect.left + 1, rect.bottom - rect.top + 1, 4, 4);
	SetWindowRgn(hRgn);
	DeleteObject(hRgn);*/

}

//////////////////////////////////////////////////////////////////////////
// message handler

LRESULT CShowInfoBar::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// override

void CShowInfoBar::DrawBackground(HDC hDC, const RECT &rc)
{
	CDCHandle dc(hDC);
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
	CWindow wndMain = ::GetRootWindow(m_hWnd);

	RECT rcClient;
	GetClientRect(&rcClient);
	dc.SelectFont(m_fontText);

	if(m_state == eShowInfo)
		dc.FillSolidRect(&rc, RGB(204, 255, 204));
	else
		dc.FillSolidRect(&rc, RGB(237, 244, 249));

	if(m_state == eShowInfo)
	{
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(m_crTextColor);
		dc.DrawText(m_strText.GetString(), -1, (LPRECT)&rcClient, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_CENTER);
	}
}
