#include "stdafx.h"
#include "../TabCtrl/TabItem.h"
#include "AddressBar.h"


#define ID_ADDRBAR_PLACEHOLDER		0
#define ID_ADDRBAR_SWITCH_CORE		1
#define ID_ADDRBAR_GO				2

#define ADDRBAR_ICON_PREFIX			5


CAddressBarCtrl::CAddressBarCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_combo(pFS)
{
	m_iCoreOfCurrentPage = 1;
	FS()->pAddressBar = this;
}


void CAddressBarCtrl::CreateAddressBar(HWND hWndParent)
{
	int iHeight = s()->Toolbar()->GetButtonHeight(ADDRBAR_ICON_PREFIX + ID_ADDRBAR_GO);
	if (iHeight < s()->Combo()->GetComboBackgroundHeight(0))
		iHeight = s()->Combo()->GetComboBackgroundHeight(0);
	if (iHeight < s()->Toolbar()->GetCoreSwitchBtnHeight())
		iHeight = s()->Toolbar()->GetCoreSwitchBtnHeight();
	HWND hWnd = Create(hWndParent, CRect(0, 0, 1, iHeight), NULL, WS_CHILD | WS_CLIPCHILDREN, 0, ID_VIEW_TOOL_ADDR);
	ATLASSERT(hWnd && "Create address bar failed.");
}

void CAddressBarCtrl::RefreshCoreType(int iCoreType)
{
	if (iCoreType == -1)
	{
		if (GetEnable(ID_ADDRBAR_SWITCH_CORE))
		{
			SetEnable(ID_ADDRBAR_SWITCH_CORE, false);
			Invalidate();
		}
	}
	else
	{
		if (!GetEnable(ID_ADDRBAR_SWITCH_CORE) || m_iCoreOfCurrentPage != iCoreType)
		{
			SetEnable(ID_ADDRBAR_SWITCH_CORE, true);
			m_iCoreOfCurrentPage = iCoreType;
			Invalidate();
		}
	}
}

void CAddressBarCtrl::Refresh()
{
	SetButtonWidth(ID_ADDRBAR_GO, s()->Toolbar()->GetButtonWidth(ADDRBAR_ICON_PREFIX + ID_ADDRBAR_GO));
	SetButtonWidth(ID_ADDRBAR_SWITCH_CORE, s()->Toolbar()->GetCoreSwitchBtnWidth());

	m_combo.Refresh();

	int iHeight = s()->Toolbar()->GetButtonHeight(ADDRBAR_ICON_PREFIX + ID_ADDRBAR_GO);
	if (iHeight < s()->Combo()->GetComboBackgroundHeight(0))
		iHeight = s()->Combo()->GetComboBackgroundHeight(0);
	if (iHeight < s()->Toolbar()->GetCoreSwitchBtnHeight())
		iHeight = s()->Toolbar()->GetCoreSwitchBtnHeight();
	SetHeight(iHeight);

//	RECT rc;
//	GetClientRect(&rc);
//	SetWindowPos(NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER | SWP_NOMOVE);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CAddressBarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_combo.CreateComboBox(m_hWnd);
	m_combo.RegisterMouseObjChild(m_hWnd, true);

	ToolData tbBtn[] =
	{
		{ ToolType_Null, false, ID_ADDRBAR_PLACEHOLDER, 0 },
		{ ToolType_Normal, false, ID_ADDRBAR_SWITCH_CORE, s()->Toolbar()->GetCoreSwitchBtnWidth() },
		{ ToolType_Normal, true, ID_ADDRBAR_GO, s()->Toolbar()->GetButtonWidth(ADDRBAR_ICON_PREFIX + ID_ADDRBAR_GO) },
	};
	AddButtons(tbBtn, _countof(tbBtn));

	return 1;
}

void CAddressBarCtrl::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
{
	if ((lpWndPos->flags & SWP_NOSIZE) == 0)
	{
		int iWidth = lpWndPos->cx - GetButtonWidth(ID_ADDRBAR_GO) - GetButtonWidth(ID_ADDRBAR_SWITCH_CORE);
		int iY = (lpWndPos->cy - s()->Combo()->GetComboBackgroundHeight(0)) / 2;
		m_combo.SetWindowPos(NULL, 0, iY, iWidth, s()->Combo()->GetComboBackgroundHeight(0), SWP_NOZORDER);
		SetButtonWidth(ID_ADDRBAR_PLACEHOLDER, iWidth, true);
	}
}


void CAddressBarCtrl::OnSetFocus(CWindow wndOld)
{
	m_combo.m_editCtrl.SetFocus();
}

LRESULT CAddressBarCtrl::OnGetDispInfo(LPNMHDR pnmh)
{
	return 0;
}

LRESULT CAddressBarCtrl::OnClick(int nIndex, POINT pt)
{
	if (nIndex == ID_ADDRBAR_GO)
		FS()->AddrCombo()->StartNavigate(0, 0, NULL);
	else if (nIndex == ID_ADDRBAR_SWITCH_CORE)
	{
		m_iCoreOfCurrentPage = 1 - m_iCoreOfCurrentPage;
		Invalidate();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// override

void CAddressBarCtrl::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Rebar()->DrawRebarBackPart(hDC, rc, m_hWnd, GetLineNumberInRebar(), FS()->ReBar());
}

void CAddressBarCtrl::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	if (iIndex == ID_ADDRBAR_GO)
	{
		if (CSkinProperty::GetDWORD(Skin_Property_AddressBar_Sync_State) == 2 && uCurrentStatus == BTN_STATUS_MOUSEOUT && m_combo.IsInWindow())
			uCurrentStatus = BTN_STATUS_MOUSEOUT | BTN_STATUS_SPECIAL;
		s()->Toolbar()->DrawButton(hDC, rc, ADDRBAR_ICON_PREFIX + iIndex, uCurrentStatus);
	}
	else if (iIndex == ID_ADDRBAR_SWITCH_CORE)
	{
		if (uCurrentStatus == BTN_STATUS_DISABLED)
			uCurrentStatus = BTN_STATUS_MOUSEOUT;
		if (CSkinProperty::GetDWORD(Skin_Property_AddressBar_Sync_State) >= 1 && uCurrentStatus == BTN_STATUS_MOUSEOUT && m_combo.IsInWindow())
			uCurrentStatus = BTN_STATUS_MOUSEOUT | BTN_STATUS_SPECIAL;
		s()->Toolbar()->DrawCoreSwitchButton(hDC, rc, m_iCoreOfCurrentPage, uCurrentStatus);
	}
}

bool CAddressBarCtrl::CheckIsMouseOver(POINT pt)
{
	RECT rcL;
	switch (CSkinProperty::GetDWORD(Skin_Property_AddressBar_Sync_State))
	{
		case 0: return false;
		case 1: GetButtonRect(ID_ADDRBAR_SWITCH_CORE, &rcL); break;
		case 2: GetButtonRect(ID_ADDRBAR_GO, &rcL); break;
	}
	return pt.x <= rcL.right;
}
