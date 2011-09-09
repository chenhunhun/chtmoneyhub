#include "stdafx.h"
#include "AddressBar.h"
#include "AddrComboBox.h"
#include "AddrComboBoxEdit.h"


CAddressComboBoxEditCtrl::CAddressComboBoxEditCtrl(FrameStorageStruct *pFS, CBaseProperties *pBaseProperties) : CNormalEditControlBase(pBaseProperties), CFSMUtil(pFS) {}


void CAddressComboBoxEditCtrl::SetText(LPCTSTR lpszText)
{
	SetWindowText(lpszText);
	int iLen = _tcslen(lpszText);
	SetSel(iLen, iLen);
}

bool CAddressComboBoxEditCtrl::SelfPaint(CDCHandle dc, const RECT &rc)
{
	return false;
}

bool CAddressComboBoxEditCtrl::IsTransparentDraw() const
{
	return sl()->Border() == Border_AdvanceAero && FS()->AddrBar()->GetLineNumberInRebar() < sl()->RebarAero();
}

void CAddressComboBoxEditCtrl::DrawContainerBackground(CDCHandle dc, const RECT &rc)
{
	s()->Rebar()->DrawRebarBackPart(dc, rc, m_hWnd, FS()->AddrBar()->GetLineNumberInRebar(), FS()->ReBar());
}

//////////////////////////////////////////////////////////////////////////
// message handler

LRESULT CAddressComboBoxEditCtrl::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//m_bCanAutoComplete = true;
	if (wParam == VK_RETURN)
	{
		m_bCanAutoComplete = false;
		// CTRL = 001, SHIFT = 010, ALT = 100
		// ctrl + enter --> 1
		// shift + enter --> 2
		// ctrl + shift + enter --> 3
		// alt + enter --> 4
		// ctrl + alt + enter --> 5
		// shift + alt + enter --> 6
		// ctrl + alt + shift + enter --> 7
		int iType = 0;
		if (::GetKeyState(VK_CONTROL) < 0)
			iType |= 1;
		if (::GetKeyState(VK_SHIFT) < 0)
			iType |= 2;
		if (::GetAsyncKeyState(VK_MENU) < 0)
			iType |= 4;
		FS()->AddrCombo()->StartNavigate(0, iType, NULL);
		return 0;
	}

	if (::GetKeyState(VK_SHIFT) < 0)
	{
		switch (wParam)
		{
			case VK_SHIFT:
				DefWindowProc();
				return 0;
			case VK_INSERT:
				if (::IsClipboardFormatAvailable(CF_TEXT))
				{
					m_bCanAutoComplete = false;
					DefWindowProc();
					FS()->AddrCombo()->OnEditBoxUserChange();
				}
				return 0;
		}
	}

	if (::GetKeyState(VK_CONTROL) < 0)
	{
		switch (wParam)
		{
			case 'A':			// ctrl + a
				DefWindowProc();
				GetParent().SendMessage(WM_KEYDOWN, 1, 0);
				break;
			case 'C':			// ctrl + c
			case VK_INSERT:
				DefWindowProc();
				break;
			case 'V':		// ctrl + v
				if (::IsClipboardFormatAvailable(CF_TEXT))
				{
					m_bCanAutoComplete = false;
					DefWindowProc();
					FS()->AddrCombo()->OnEditBoxUserChange();
					if (::GetKeyState(VK_SHIFT) < 0)
						FS()->AddrCombo()->StartNavigate(0, 0, NULL);
				}
				break;
			case 'X':		// ctrl + x
				m_bCanAutoComplete = false;
				DefWindowProc();
				FS()->AddrCombo()->OnEditBoxUserChange();
				break;
			case 'Z':		// ctrl + z
				m_bCanAutoComplete = false;
				DefWindowProc();
				FS()->AddrCombo()->OnEditBoxUserChange();
				break;
			case VK_LEFT:
			case VK_RIGHT:
				DefWindowProc();
				break;
			case VK_CONTROL:
				DefWindowProc();
				break;
		}
		return 0;
	}
	switch (wParam)
	{
		case VK_UP: case VK_DOWN: case VK_PRIOR: case VK_NEXT: case VK_ESCAPE:
			GetParent().SendMessage(uMsg, wParam, lParam);
			return 0;
		case VK_LEFT: case VK_RIGHT: case VK_HOME: case VK_END:
			DefWindowProc();
			GetParent().SendMessage(uMsg, wParam, lParam);
			break;
		case VK_DELETE:
		{
			m_bCanAutoComplete = false;
			long iSelBegin, iSelEnd;
			GetSel(iSelBegin, iSelEnd);
			int iLen = GetWindowTextLength();
			if (iSelEnd == iLen && (iSelBegin == iLen || iSelBegin == 0))
			{
				int iSel = FS()->AddrCombo()->GetDropDownSelection();
				if (iSel >= 0)
				{
					FS()->AddrDropdown()->RemoveDropDownSelection(iSel);
					return 0;
				}
			}
			DefWindowProc();
			FS()->AddrCombo()->OnEditBoxUserChange();
			break;
		}
		default:
			DefWindowProc();
			break;
	}
	return 0;
}

LRESULT CAddressComboBoxEditCtrl::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// [TuotuoXP] 
	if (wParam >= 32 && wParam <= 126)
		m_bCanAutoComplete = true;
	switch (wParam)
	{
		case VK_ESCAPE: case VK_RETURN: return 0;
		case VK_BACK:
			m_bCanAutoComplete = false;
			break;
	}
	DefWindowProc();
	if (::GetKeyState(VK_SHIFT) >= 0 && ::GetKeyState(VK_CONTROL) >= 0)
		FS()->AddrCombo()->OnEditBoxUserChange();
	return 0;
}


void CAddressComboBoxEditCtrl::OnKillFocus(CWindow wndFocus)
{
	DefWindowProc();
	FS()->AddrCombo()->AddrEditKillFocus();
}



LRESULT CAddressComboBoxEditCtrl::OnIMENotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	LRESULT l = DefWindowProc();
	if (wParam == IMN_OPENCANDIDATE)
		GetParent().SendMessage(WM_IME_NOTIFY, 0, lParam);
	return l;
}

LRESULT CAddressComboBoxEditCtrl::OnIMEStartComposition(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	LRESULT l = DefWindowProc();
	GetWindowText(m_strBeforeIME);
	return l;
}

LRESULT CAddressComboBoxEditCtrl::OnIMEEndComposition(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	m_bCanAutoComplete = true;
	LRESULT l = DefWindowProc();
	CString strNow;
	GetWindowText(strNow);
	if (strNow.Compare(m_strBeforeIME))
		FS()->AddrCombo()->OnEditBoxUserChange();
	else
		GetParent().SendMessage(WM_IME_NOTIFY, 1);
	return l;
}


void CAddressComboBoxEditCtrl::OnContextMenu(CWindow wnd, CPoint point)
{
}
