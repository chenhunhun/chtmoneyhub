#include "stdafx.h"
#include "../../Skin/SkinManager.h"
#include "SystemButtonBar.h"

#define ID_SYSBTN_MIN			1
#define ID_SYSBTN_MAX			2
#define ID_SYSBTN_RESTORE		2
#define ID_SYSBTN_CLOSE			3


CSystemButtonBarCtrl::CSystemButtonBarCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS) {}


void CSystemButtonBarCtrl::CreateSystemButtonBar(HWND hWndParent)
{
	int iFullWidth = s()->MainFrame()->GetMinButtonWidth() + s()->MainFrame()->GetMaxRestoreButtonWidth() + s()->MainFrame()->GetCloseButtonWidth() + CSkinProperty::GetDWORD(Skin_Property_Frame_System_Button_Right_Margin);
	HWND hWnd  = Create(hWndParent, CRect(0, 0, iFullWidth, s()->MainFrame()->GetHeight()), NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd && "Create system button bar failed.");
}

void CSystemButtonBarCtrl::Refresh()
{
	SetButtonWidth(ID_SYSBTN_MIN, s()->MainFrame()->GetMinButtonWidth());
	SetButtonWidth(ID_SYSBTN_MAX, s()->MainFrame()->GetMaxRestoreButtonWidth());
	SetButtonWidth(ID_SYSBTN_CLOSE, s()->MainFrame()->GetCloseButtonWidth() + CSkinProperty::GetDWORD(Skin_Property_Frame_System_Button_Right_Margin));
	SetButtonWidth(0, 0);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CSystemButtonBarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn[] =
	{
		{ ToolType_Null, false, 0, 0 },
		{ ToolType_Normal, true, ID_SYSBTN_MIN, s()->MainFrame()->GetMinButtonWidth() },
		{ ToolType_Normal, true, ID_SYSBTN_MAX, s()->MainFrame()->GetMaxRestoreButtonWidth() },
		{ ToolType_Normal, true, ID_SYSBTN_CLOSE, s()->MainFrame()->GetCloseButtonWidth() + CSkinProperty::GetDWORD(Skin_Property_Frame_System_Button_Right_Margin) }
	};
	AddButtons(tbBtn, _countof(tbBtn));
	RefreshAfterAddButtonOrSizeChange();
	return 1;
}

LRESULT CSystemButtonBarCtrl::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	WINDOWPOS *lpWndPos = (WINDOWPOS*)lParam;
	if ((lpWndPos->flags & SWP_NOSIZE) == 0)
	{
		if (CSkinProperty::GetDWORD(Skin_Property_Frame_System_Button_Top_Margin) != -1)
			lpWndPos->cy = s()->MainFrame()->GetHeight() + CSkinProperty::GetDWORD(Skin_Property_Frame_System_Button_Top_Margin) + 1;
	}
	return 0;
}

LRESULT CSystemButtonBarCtrl::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	WINDOWPOS *lpWndPos = (WINDOWPOS*)lParam;
	if ((lpWndPos->flags & SWP_NOSIZE) == 0)
	{
		int iWidth = lpWndPos->cx - s()->MainFrame()->GetMinButtonWidth() - s()->MainFrame()->GetMaxRestoreButtonWidth() - s()->MainFrame()->GetCloseButtonWidth() - CSkinProperty::GetDWORD(Skin_Property_Frame_System_Button_Right_Margin);
		SetButtonWidth(0, iWidth);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CSystemButtonBarCtrl::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	return 0;
}


LRESULT CSystemButtonBarCtrl::OnClick(int iIndex, POINT pt)
{
	CWindow wndMainFrame = ::GetRootWindow(m_hWnd);
	switch (iIndex)
	{
		case ID_SYSBTN_MIN:
			wndMainFrame.PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
			break;
		case ID_SYSBTN_MAX:
			if (wndMainFrame.GetStyle() & WS_MAXIMIZE)
				wndMainFrame.PostMessage(WM_SYSCOMMAND, SC_RESTORE);
			else
				wndMainFrame.PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
			break;
		case ID_SYSBTN_CLOSE:
			wndMainFrame.PostMessage(WM_SYSCOMMAND, SC_CLOSE);
			break;
	}
	return 0;
}


LRESULT CSystemButtonBarCtrl::OnGetDispInfo(LPNMHDR pnmh)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// override

void CSystemButtonBarCtrl::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CSystemButtonBarCtrl::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	if (iIndex >= 1)
		s()->MainFrame()->DrawButton(hDC, rc, iIndex == ID_SYSBTN_MAX ? ((::GetWindowLong(GetRootWindow(m_hWnd), GWL_STYLE) & WS_MAXIMIZE) ? 3 : 1) : iIndex - 1, uCurrentStatus);
}
