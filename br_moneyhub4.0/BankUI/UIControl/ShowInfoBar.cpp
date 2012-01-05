#include "stdafx.h"
#include "../Skin/SkinManager.h"
#include "TabCtrl/TabCtrl.h"
#include "BrowserToolbar.h"
#include "ShowInfoBar.h"


CShowInfoBar::CShowInfoBar(FrameStorageStruct *pFS) : CFSMUtil(pFS),m_fontText(NULL),m_fontText2(NULL)
{
	m_strText2 = L"通信过程中，所有数据均按银行系统要求进行加密，您的信息也不会发送给任何第三方，请放心使用";
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
		m_fontText.CreateFont(20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));
	if (NULL == m_fontText2)
		m_fontText2.CreateFont(13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));

	m_crTextColor = RGB(61, 98, 123);
	m_strText = L"测试";
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

	if(m_state == eShowInfo)
		dc.FillSolidRect(&rc, RGB(204, 255, 204));
	else
		dc.FillSolidRect(&rc, RGB(237, 244, 249));

	if(m_state == eShowInfo)
	{	
		RECT rcClient1(rcClient),rcClient2(rcClient);
		rcClient1.top += 5;
		rcClient1.bottom -= 20;
		rcClient2.top += 40;
		rcClient2.bottom -= 5; 
		dc.SelectFont(m_fontText);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(m_crTextColor);
		dc.DrawText(m_strText.GetString(), -1, (LPRECT)&rcClient1, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_CENTER);

		dc.SelectFont(m_fontText2);
		dc.DrawText(m_strText2.GetString(), -1, (LPRECT)&rcClient2, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_CENTER);
	}
}
