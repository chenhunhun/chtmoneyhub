#include "stdafx.h"
#include "../../Util/Config.h"
#include "AddrComboBox.h"
#include "AddrDropDownList.h"


CAddrDropDownList* CAddrDropDownList::sm_pCurrentHookingAddrDropDown = NULL;


CAddrDropDownList::CAddrDropDownList(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_iDisplayDataCount(0), m_iRealDataCount(0), m_iLastSeq(-1), m_bBigDropDown(true), m_iCurrentSelection(-1), m_eDeleteButtonStatus(Btn_MouseOut), m_hMsgHook(NULL)
{
	FS()->pAddrDropDownList = this;
	m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
	UpdateDropDownListStyle();
}

CAddrDropDownList::~CAddrDropDownList()
{
	if (m_hMsgHook)
		::UnhookWindowsHookEx(m_hMsgHook);
}


HWND CAddrDropDownList::CreateDropDownList(CAddressComboBoxCtrl *pComboBox)
{
	m_pComboBox = pComboBox;
	return Create(pComboBox->m_hWnd, NULL, NULL, WS_POPUP | WS_BORDER, WS_EX_NOACTIVATE);
}

void CAddrDropDownList::UpdateDropDownListStyle()
{
	m_bBigDropDown = g_bAddrBarDropDownIsBig;
	m_iPageDisplaySize = m_bBigDropDown ? 8 : 12;
}

void CAddrDropDownList::ShowDropDownList()
{
	if (m_iDisplayDataCount == 0)
	{
		::ReleaseCapture();
		return;
	}

	RECT rcParent;
	m_pComboBox->GetWindowRect(&rcParent);

	int iDelta = s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_RANGE | SIF_POS | SIF_PAGE };
	si.nMin = 0;
	si.nPos = 0;
	if (m_iDisplayDataCount > m_iPageDisplaySize)
	{
		si.nMax = m_iDisplayDataCount * iDelta - 1;
		si.nPage = m_iPageDisplaySize * iDelta;
	}
	else
	{
		si.nMax = m_iDisplayDataCount * iDelta;
		si.nPage = si.nMax + 1;
	}
	SetScrollInfo(SB_VERT, &si, TRUE);

	int iDisplayHeight = m_iDisplayDataCount > m_iPageDisplaySize ? m_iPageDisplaySize : m_iDisplayDataCount;
	int iHeight = iDisplayHeight * iDelta + 1;

	RECT rcWorkArea;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	if (iHeight + rcParent.top + 30 > rcWorkArea.bottom)
		SetWindowPos(HWND_TOP, rcParent.left, rcParent.top - iHeight - 1, rcParent.right - rcParent.left, iHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE);
	else
		SetWindowPos(HWND_TOP, rcParent.left, rcParent.bottom + 1, rcParent.right - rcParent.left, iHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE);
	Invalidate();

	if (!m_hMsgHook)
	{
		sm_pCurrentHookingAddrDropDown = this;
		m_hMsgHook = ::SetWindowsHookEx(WH_MOUSE, MouseProc, NULL, ::GetCurrentThreadId());
	}
}

void CAddrDropDownList::HideDropDownList(bool bAcceptNewSuggestion)
{
	::UnhookWindowsHookEx(m_hMsgHook);
	m_hMsgHook = NULL;
	sm_pCurrentHookingAddrDropDown = NULL;

	if (!bAcceptNewSuggestion)
		m_iLastSeq = -1;
	m_iCurrentSelection = -1;
	ShowWindow(SW_HIDE);
	m_pComboBox->OnDropDownListHide();
}


void CAddrDropDownList::GenSearchRequest(LPCTSTR lpszSearchText)
{
}


bool CAddrDropDownList::CheckURLIsInSuggestionList(LPCTSTR lpszURL) const
{
	return false;
}

//////////////////////////////////////////////////////////////////////////

void CAddrDropDownList::SetSelection(int iSelection)
{
}

void CAddrDropDownList::OnMouseClickWhenDropDown(HWND hWnd, POINT pt)
{
	if (hWnd == m_hWnd)
		return;
	if (hWnd == m_pComboBox->m_hWnd)
	{
		RECT rcComboBox;
		m_pComboBox->GetWindowRect(&rcComboBox);
		rcComboBox.left = rcComboBox.right - s()->Combo()->GetComboDropdownTriangleButtonWidth();
		if (::PtInRect(&rcComboBox, pt))
			return;
	}
	HideDropDownList(false);
}

ButtonStatus CAddrDropDownList::GetDelButtonStatus(const RECT& rcItem, CPoint& pt)
{
	if( s()->Combo()->GetPointerOnDeleteButton(rcItem, pt, m_bBigDropDown) )
	{
		return Btn_MouseOver;
	}
	else
	{
		return Btn_MouseOut;
	}
}

void CAddrDropDownList::RemoveDropDownSelection(int iSel)
{
}

//////////////////////////////////////////////////////////////////////////
// message handler

void CAddrDropDownList::OnPaint(CDCHandle dc, RECT rect)
{
	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_POS };
	GetScrollInfo(SB_VERT, &si);

	CMemoryDC memDC(dc, rect);

	RECT rcClient;
	GetClientRect(&rcClient);
	int iDelta = s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
	for (int i = 0, y = -si.nPos; i < m_iDisplayDataCount; i++, y += iDelta)
	{
		RECT rcItem = { 0, y, rcClient.right, y + iDelta };
		int iBackColor = 0;
		if (i == 0 && m_pComboBox->IsInAutoCompletionMode())
			iBackColor = 2;
		else if (m_LocalData[i].iType == 1)
			iBackColor = 1;
		else
			iBackColor = 0;
		s()->Combo()->DrawComboDropDownListItem(memDC, rcItem, m_iCurrentSelection == i ? Btn_MouseOver : Btn_MouseOut,
			m_LocalData[i].strURL, m_LocalData[i].strTitle, m_LocalData[i].strURLMask, m_LocalData[i].strTitleMask,
			(HICON)m_LocalData[i].hSiteIcon, m_bBigDropDown, iBackColor, m_eDeleteButtonStatus);
	}
}


void CAddrDropDownList::OnMouseMove(UINT nFlags, CPoint point)
{
	if (point == m_ptLastMousePos)
		return;
	m_ptLastMousePos = point;

	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_POS };
	GetScrollInfo(SB_VERT, &si);

	int iDelta = s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
	int iSel = (point.y + si.nPos) / iDelta;

	RECT rcClient;
	GetClientRect(&rcClient);
	
	RECT rcItem = { 0, iSel*iDelta,  rcClient.right, (iSel + 1) * iDelta };
	CPoint pt = point;
	pt.y += si.nPos;

	ButtonStatus eStatus = GetDelButtonStatus(rcItem, pt);

	if (iSel != m_iCurrentSelection)
	{
		m_iCurrentSelection = iSel;
		m_eDeleteButtonStatus = eStatus;

		Invalidate();
	}
	else
	{
		if( m_eDeleteButtonStatus != eStatus )
		{
			m_eDeleteButtonStatus = eStatus;
			Invalidate();
		}
	}
}

void CAddrDropDownList::OnLButtonUp(UINT nFlags, CPoint point)
{
	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_POS };
	GetScrollInfo(SB_VERT, &si);

	int iDelta = s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
	int iSelection = (point.y + si.nPos) / iDelta;

	RECT rcClient;
	GetClientRect(&rcClient);

	RECT rcItem = { 0, iSelection*iDelta,  rcClient.right, (iSelection + 1) * iDelta };
	CPoint pt = point;
	pt.y += si.nPos;

	// 点到X上了
	if (Btn_MouseOver == GetDelButtonStatus(rcItem, pt))
	{
		RemoveDropDownSelection(iSelection);
	}
	else
	{
		FS()->AddrCombo()->StartNavigate(1 + m_LocalData[iSelection].iType, 0, m_LocalData[iSelection].strURL);
	}

}

BOOL CAddrDropDownList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_RANGE | SIF_POS };
	GetScrollInfo(SB_VERT, &si);
	si.nPos -= zDelta / 40 * s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
	if (zDelta > 0)
	{
		if (si.nPos > si.nMax)
			si.nPos = si.nMax;
	}
	else
	{
		if (si.nPos < 0)
			si.nPos = 0;
	}
	SetScrollInfo(SB_VERT, &si);
	Invalidate();
	return TRUE;
}

void CAddrDropDownList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar)
{
	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_RANGE | SIF_POS };
	GetScrollInfo(SB_VERT, &si);
	int iOldPos = si.nPos;
	switch (nSBCode)
	{
		case SB_TOP:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case SB_LINEUP:
			si.nPos -= s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
			if (si.nPos < 0)
				si.nPos = 0;
			break;
		case SB_LINEDOWN:
			si.nPos += s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
			if (si.nPos > si.nMax)
				si.nPos = si.nMax;
			break;
		case SB_PAGEUP:
			si.nPos -= 5 * s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
			if (si.nPos < 0)
				si.nPos = 0;
			break;
		case SB_PAGEDOWN:
			si.nPos += 5 * s()->Combo()->GetComboDropdownItemBackgroundHeight(m_bBigDropDown);
			if (si.nPos > si.nMax)
				si.nPos = si.nMax;
			break;
		case SB_THUMBTRACK:
			si.nPos = nPos;
			break;
	}
	if (iOldPos != si.nPos)
	{
		si.fMask = SIF_POS;
		SetScrollInfo(SB_VERT, &si, TRUE);
		Invalidate();
	}
}

void CAddrDropDownList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_UP: case VK_PRIOR:
			if (m_iCurrentSelection == 0)
			{
				SetSelection(-1);
				m_pComboBox->SendMessage(WM_SETTEXT, 2, NULL);
			}
			else
			{
				int iSel = m_iCurrentSelection == -1 ? m_iDisplayDataCount - 1 : m_iCurrentSelection - (nChar == VK_PRIOR ? m_iPageDisplaySize - 1 : 1);
				SetSelection(iSel < 0 ? 0 : iSel);
				m_pComboBox->SendMessage(WM_SETTEXT, 1, (LPARAM)(LPCTSTR)m_LocalData[m_iCurrentSelection].strURL);
			}
			Invalidate();
			break;
		case VK_DOWN: case VK_NEXT:
			if (m_iCurrentSelection == m_iDisplayDataCount - 1)
			{
				SetSelection(-1);
				m_pComboBox->SendMessage(WM_SETTEXT, 2, NULL);
			}
			else
			{
				int iSel = m_iCurrentSelection == -1 ? 0 : m_iCurrentSelection + (nChar == VK_NEXT ? m_iPageDisplaySize - 1 : 1);
				SetSelection(iSel >= m_iDisplayDataCount ? m_iDisplayDataCount - 1 : iSel);
				m_pComboBox->SendMessage(WM_SETTEXT, 1, (LPARAM)(LPCTSTR)m_LocalData[m_iCurrentSelection].strURL);
			}
			Invalidate();
			break;
	}
}

//////////////////////////////////////////////////////////////////////////

LRESULT CAddrDropDownList::OnSetDropDownContent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	return 1;
}

LRESULT CAddrDropDownList::OnAddDropDownContent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////


LRESULT CAddrDropDownList::MouseProc(int iCode, WPARAM wParam, LPARAM lParam)
{
	if (iCode == HC_ACTION && sm_pCurrentHookingAddrDropDown)
	{
		MOUSEHOOKSTRUCT *pms = (MOUSEHOOKSTRUCT*)lParam;
		if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN
			|| wParam == WM_NCLBUTTONDOWN || wParam == WM_NCRBUTTONDOWN || wParam == WM_NCMBUTTONDOWN)
			sm_pCurrentHookingAddrDropDown->OnMouseClickWhenDropDown(pms->hwnd, pms->pt);
	}

	return ::CallNextHookEx(NULL, iCode, wParam, lParam);
}
