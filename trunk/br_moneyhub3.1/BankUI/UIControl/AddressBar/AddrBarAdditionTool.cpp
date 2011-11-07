#include "stdafx.h"
#include "../../Util/Config.h"
#include "../../Skin/SkinManager.h"
#include "AddrComboBox.h"
#include "AddrBarAdditionTool.h"


CAddrAdditionTool::CAddrAdditionTool(FrameStorageStruct *pFS) : CFSMUtil(pFS)
{
	m_iResCount[0] = m_iResCount[1] = 0;
}


void CAddrAdditionTool::CreateAddressBarAdditionTool(HWND hWndParent)
{
	HWND hWnd = Create(hWndParent, CRect(0, 0, 1, s()->Tab()->GetResourceButtonMaxHeight()), NULL, WS_VISIBLE | WS_CHILD);
	ATLASSERT(hWnd && "Create address bar additional tool failed.");
}


void CAddrAdditionTool::UpdateResourceStatus(int iResType, int iCount)
{
	m_iResCount[iResType] = iCount;
	ClearAllButtons();

	for (int i = 0; i < _countof(m_iResCount); i++)
	{
		if (m_iResCount[i] != 0)
		{
			ToolData tbBtn = { ToolType_Dropdown, true, i, s()->Tab()->GetResourceButtonWidth(i) };
			AddButton(&tbBtn);
		}
	}
	RefreshAfterAddButtonOrSizeChange();
	Invalidate();
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CAddrAdditionTool::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

LRESULT CAddrAdditionTool::OnGetDispInfo(LPNMHDR pnmh)
{
	return 0;
}

LRESULT CAddrAdditionTool::OnDropDown(int nIndex, POINT pt)
{
	return 0;
}

void CAddrAdditionTool::Refresh()
{
	for (int i = 0; i < GetButtonCount(); i++)
		SetButtonWidth(i, s()->Tab()->GetResourceButtonWidth(GetData(i)));
	RefreshAfterAddButtonOrSizeChange();
}

//////////////////////////////////////////////////////////////////////////
// override

void CAddrAdditionTool::DrawBackground(HDC hDC, const RECT &rc)
{
	if (rc.left == rc.right)
		return;
	s()->Rebar()->DrawRebarBackPart(hDC, rc, m_hWnd, GetLineNumberInRebar(), FS()->ReBar());
	RECT rcClient;
	GetWindowRect(&rcClient);
	FS()->AddrCombo()->ScreenToClient(&rcClient);
	s()->Combo()->DrawComboEditBackgroundPart(hDC, rc, rcClient.top, FS()->AddrCombo()->GetStatus(), 0, false);
}

void CAddrAdditionTool::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	int iY = (rc.bottom + rc.top - s()->Tab()->GetResourceButtonHeight(GetData(iIndex))) / 2;
	RECT rcDest = { rc.left, iY, rc.right, iY + s()->Tab()->GetResourceButtonHeight(GetData(iIndex)) };
	s()->Tab()->DrawResourceButton(hDC, rcDest, CSkinManager::FromTuoToolStatus(uCurrentStatus), GetData(iIndex));
}
