#include "stdafx.h"
#include "../../Util/Config.h"
#include "AddressBar.h"
#include "AddrComboBox.h"

#define TIMER_WAIT_SEARCH_RESULT	101
#define TIMEOUT_WAIT_SEARCH_RESULT	1000


CAddressComboBoxCtrl::CAddressComboBoxCtrl(FrameStorageStruct *pFS) : CFSMUtil(pFS), m_editCtrl(pFS, this), m_DropDown(pFS), m_tool(pFS), m_bAlreadyAutoComplete(false), m_bUserTyping(false)
{
	FS()->pAddrCombo = this;
	SetContainerType(0);

	m_WaitSearch.uTimer = 0;
	ResetWaitSearch();
}


void CAddressComboBoxCtrl::CreateComboBox(HWND hParent)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	HWND hWnd = Create(hParent, NULL, NULL, dwStyle);
	ATLASSERT(hWnd && "Create address bar failed.");
}


void CAddressComboBoxCtrl::AutoCompleteUserInput()
{
}


void CAddressComboBoxCtrl::UpdateIcon(HICON hIcon)
{
	SetEditControlIcon(hIcon);
}

void CAddressComboBoxCtrl::UpdateURLText(LPCTSTR lpszURL, bool bForce)
{
	m_strCurrentURL = lpszURL;
	bool bChangeText = bForce || ::GetFocus() != m_editCtrl;
	if (!bChangeText)
	{
		long l0, l1;
		m_editCtrl.GetSel(l0, l1);
		if (l0 == 0 && l1 == m_editCtrl.GetTextLength())
			bChangeText = true;
	}
	if (bChangeText)
	{
		m_bAlreadyAutoComplete = false;
		m_strTextBeforeAutoCompletion = m_strCurrentURL;
		m_editCtrl.SetText(m_strCurrentURL);
		m_DropDown.HideDropDownList(false);
	}
}

void CAddressComboBoxCtrl::UpdateResourceStatus(int iResType, int iCount)
{
	m_tool.UpdateResourceStatus(iResType, iCount);
	SendMessage(WM_SIZE);
}

void CAddressComboBoxCtrl::UpdateDisplayResourceIcon()
{
	m_tool.ShowWindow(g_bDisplayVideoPopIcon ? SW_SHOW : SW_HIDE);
	SendMessage(WM_SIZE);
}

void CAddressComboBoxCtrl::DoSelectAddressComboBar()
{
	if (::IsWindowVisible2(GetParent()))
	{
		if (::GetFocus() != m_editCtrl)
			m_editCtrl.SetFocus();
		else
			m_editCtrl.SetSelAll();
	}
}

void CAddressComboBoxCtrl::DoShowDropDownList()
{
	if (::IsWindowVisible2(GetParent()))
	{
		if (m_DropDown.IsWindowVisible())
			m_DropDown.HideDropDownList(false);
		else
		{
			m_DropDown.GenSearchRequest(_T(""));
			m_editCtrl.SetFocus();
		}
	}
}

void CAddressComboBoxCtrl::OnDropDownListHide()
{
	SetDropDownButtonStatus(false);
}

//////////////////////////////////////////////////////////////////////////
// callback

void CAddressComboBoxCtrl::PaintBackground(CDCHandle dc, RECT rc)
{
	s()->Rebar()->DrawRebarBackPart(dc, rc, m_hWnd, FS()->AddrBar()->GetLineNumberInRebar(), FS()->ReBar());
}

void CAddressComboBoxCtrl::GetLeftAndRightMargin(int &left, int &right)
{
	left = 0;
	if (::IsWindowVisible2(m_tool))
		right = m_tool.GetFullWidth() + 2;
	else
		right = 0;
}

bool CAddressComboBoxCtrl::IsTransparentDraw() const
{
	return sl()->Border() == Border_AdvanceAero && FS()->AddrBar()->GetLineNumberInRebar() < sl()->RebarAero();
}


void CAddressComboBoxCtrl::GetSearchRecommend(std::wstring& strUrl, std::wstring& strWord)
{
	strUrl = m_strSearchRecommendUrl;
	strWord = m_strSearchRecommendWord;
}

BOOL CAddressComboBoxCtrl::IsSearchComplete(const std::wstring& strWord)
{
	return ( 0 == strWord.compare(m_strSearchRecommendWord) );
}



void CAddressComboBoxCtrl::StartNavigate(int iComeFromSource, int iShortcutKeyType, LPCTSTR lpszText)
{
}

void CAddressComboBoxCtrl::OnEditBoxUserChange()
{
}

void CAddressComboBoxCtrl::AddrEditKillFocus()
{
	m_bUserTyping = false;
	if (m_DropDown.IsWindow())
		m_DropDown.HideDropDownList(false);
}

int CAddressComboBoxCtrl::GetDropDownSelection()
{
	return m_DropDown.IsWindowVisible() ? m_DropDown.GetCurrentSelection() : -1;
}

void CAddressComboBoxCtrl::IMENotify(bool bStartIME)
{
	if (m_DropDown.IsWindow())
	{
		if (bStartIME)
			m_DropDown.HideDropDownList(false);
		else if (::GetFocus() == m_editCtrl)
			m_DropDown.ShowDropDownList();
	}
}


void CAddressComboBoxCtrl::PrepareQuickComplete(bool bForce)
{
}

void CAddressComboBoxCtrl::Refresh()
{
	m_tool.Refresh();
	RECT rcWnd;
	m_editCtrl.GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	int iY = (s()->Combo()->GetComboBackgroundHeight(0) - s()->Tab()->GetResourceButtonMaxHeight()) / 2 + 1;
	RECT rcResTool = { rcWnd.right + 1, iY, rcWnd.right + 1 + m_tool.GetFullWidth(), iY + s()->Tab()->GetResourceButtonMaxHeight() };
	m_tool.SetWindowPos(NULL, &rcResTool, SWP_NOZORDER);

	CString str;
	long lSelStart, lSelEnd;
	m_editCtrl.GetSel(lSelStart, lSelEnd);
	m_editCtrl.GetWindowText(str);
	m_editCtrl.SetWindowText(_T(""));
	m_editCtrl.SetFont(s()->Common()->GetAddrBarFont());
	m_editCtrl.SetWindowText(str);
	m_editCtrl.SetSel(lSelStart, lSelEnd);
}

//////////////////////////////////////////////////////////////////////////

void CAddressComboBoxCtrl::ResetWaitSearch()
{
	if( m_WaitSearch.uTimer )
	{
		KillTimer(m_WaitSearch.uTimer);
	}

	m_WaitSearch.uTimer = 0;
	m_WaitSearch.uStart = 0;
	m_WaitSearch.uWeight = 0;
	m_WaitSearch.strWord = L"";

}

void CAddressComboBoxCtrl::OpenRecommend()
{
}

void CAddressComboBoxCtrl::OpenPage(LPCTSTR url, int iWeight)
{
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CAddressComboBoxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_tool.CreateAddressBarAdditionTool(m_hWnd);
	RegisterMouseObjChild(m_tool, true);

	SetEditControlWnd(m_editCtrl.CreateEditControl(m_hWnd));
	m_editCtrl.LimitText(1280);
	m_editCtrl.SetFont(s()->Common()->GetAddrBarFont());
	m_DropDown.CreateDropDownList(this);

	SendMessage(WM_SIZE);

	return 1;
}

void CAddressComboBoxCtrl::OnDestroy()
{
	//m_DropDown.DestroyWindow();
}

void CAddressComboBoxCtrl::OnSize(UINT nType, CSize size)
{
	RECT rcWnd;
	m_editCtrl.GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	int iY = (s()->Combo()->GetComboBackgroundHeight(0) - s()->Tab()->GetResourceButtonMaxHeight()) / 2 + 1;
	RECT rcResTool = { rcWnd.right + 1, iY, rcWnd.right + 1 + m_tool.GetFullWidth(), iY + s()->Tab()->GetResourceButtonMaxHeight() };
	m_tool.SetWindowPos(NULL, &rcResTool, SWP_NOZORDER);
	m_DropDown.HideDropDownList(false);
}

void CAddressComboBoxCtrl::OnLButtonDownAndDblClk(UINT nFlags, CPoint point)
{
	RECT rect;
	GetClientRect(&rect);
	if (point.x >= rect.right - s()->Combo()->GetComboDropdownTriangleButtonWidth())
	{
		if (!m_DropDown.IsWindowVisible())
		{
			m_bAlreadyAutoComplete = false;
			SetDropDownButtonStatus(true);
			m_DropDown.GenSearchRequest(_T(""));
			m_editCtrl.SetFocus();
			SetCapture();
		}
		else
			m_DropDown.HideDropDownList(false);
	}
}

void CAddressComboBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
}

void CAddressComboBoxCtrl::OnCaptureChanged(CWindow wnd)
{
	SetDropDownButtonStatus(m_DropDown.IsWindowVisible() != FALSE);
}


LRESULT CAddressComboBoxCtrl::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	switch (wParam)
	{
		case VK_ESCAPE:
			if (m_DropDown.IsWindowVisible())
			{
				m_DropDown.HideDropDownList(false);
				if (IsInAutoCompletionMode())		// 如果是补全状态下按esc，则要删除掉补全部分
				{
					m_bAlreadyAutoComplete = false;
					m_editCtrl.SetText(m_strTextBeforeAutoCompletion);
					m_editCtrl.SetSel(m_strTextBeforeAutoCompletion.GetLength(), m_strTextBeforeAutoCompletion.GetLength());
				}
			}
			else
			{
				m_editCtrl.SetText(m_strCurrentURL);
				m_editCtrl.SetSelAll();
			}
			m_bUserTyping = false;
			return 0;
		case 1:			// ctrl + a
		case VK_LEFT: case VK_RIGHT: case VK_HOME: case VK_END:		// 取消补全状
			m_bAlreadyAutoComplete = false;
			m_DropDown.Invalidate();
			break;
		case VK_DOWN: case VK_NEXT:
		case VK_UP:  case VK_PRIOR: 
			if (!m_DropDown.IsWindowVisible())
			{
				// 如果按下的时候列表没有显示，则去对当前地址栏的内容作suggestion
				CString str;
				m_editCtrl.GetWindowText(str);
				m_DropDown.GenSearchRequest(str);
			}
			else
				m_DropDown.SendMessage(uMsg, wParam, lParam);
			bHandled = TRUE;
			return 0;
	}
	return 0;
}

LRESULT CAddressComboBoxCtrl::OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 只有DropDown那个窗口会用settext消息的方式进来
	LPCTSTR lpsz = (LPCTSTR)lParam;
	if (wParam == 1)
	{
		m_bAlreadyAutoComplete = false;
		m_editCtrl.SetText(lpsz);
		m_editCtrl.SetSelAll();
	}
	else if (wParam == 2)			// 当dropdown选择-1的时候发送此消息
	{
		m_editCtrl.SetText(m_strTextBeforeAutoCompletion);
		AutoCompleteUserInput();
		if (!IsInAutoCompletionMode())
			m_editCtrl.SetSel(m_strTextBeforeAutoCompletion.GetLength(), m_strTextBeforeAutoCompletion.GetLength());
	}

	return 0;
}

LRESULT CAddressComboBoxCtrl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// message from outside

LRESULT CAddressComboBoxCtrl::OnSetDropDownContent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
/*	if (m_DropDown.SendMessage(uMsg, wParam, lParam))
	{
		AddressBarSearchRequestData *pRequest = (AddressBarSearchRequestData*)wParam;
		if (pRequest->strQueryString.length() > 0)
		{
			long iSelBegin, iSelEnd;
			m_editCtrl.GetSel(iSelBegin, iSelEnd);
			int iLen = m_editCtrl.GetTextLength();
			if (iSelBegin == iLen && iSelEnd == iLen)
				AutoCompleteUserInput();
		}
	}
*/
	return 0;
}

LRESULT CAddressComboBoxCtrl::OnAddDropDownContent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}


LRESULT CAddressComboBoxCtrl::OnSetSearchRecommend(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
