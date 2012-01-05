#include "stdafx.h"
#include "../Util/ProcessManager.h"
#include "FrameBorder/TitleBar.h"
#include "StatusBar/BrowserStatusBar.h"
#include "TabCtrl/TabItem.h"
#include "MainFrame.h"
#include "ChildFrm.h"
#include "CategoryCtrl/CategoryItem.h"
#include "SaveFavProgress.h"
#include "../Util/Util.h"

#define CHECK_VALID_ITEM()			do { if (m_pItem == NULL) return 0; } while (0)

CChildFrame::CChildFrame(FrameStorageStruct *pFS, HWND hWnd, LPCTSTR lpszWindowName, CTabItem *pItem, bool bForuceNoNavigate, bool bForceCreateInNewProcess, DWORD dwCreateFromPID, std::tstring strChildFrameUrl)
	: CFSMUtil(pFS), m_pItem(pItem), m_bForuceNoNavigate(bForuceNoNavigate), m_iDestroyType(-1), m_bCreatingAxControl(true)	// 这一排是非参数的默认值
{
	CString strURL = lpszWindowName ? (lpszWindowName[0] ? lpszWindowName : _T("about:blank")) : _T("about:blank");

	RECT rc;
	::GetClientRect(hWnd, &rc);
	HWND hChildFrame = Create(hWnd, rc, NULL, WS_CHILD | WS_CLIPCHILDREN);
	ATLASSERT(hChildFrame && "Create Childframe failed.");
	pItem->SetTabView(this);

	//m_pItem->SetURLText(strURL);
	if (strURL != _T("about:blank"))
		m_pItem->SetURLText(strURL);
	else
		m_pItem->SetURLText(strChildFrameUrl.c_str());

	if (m_pItem->GetPageType() == PageType_Normal)
		m_pItem->LoadIconFromURL(m_pItem->GetURLText(), m_hWnd);

	CProcessManager::_()->CreateNewWebPage(m_hWnd);
	FS()->MainFrame()->RecalcClientSize(-1, -1);
}

//////////////////////////////////////////////////////////////////////////

void CChildFrame::OptionalDestroy(int iType, bool isTrueClose)
{
	SetWindowLongPtr(GWLP_USERDATA, NULL);
	m_iDestroyType = iType;

	::ShowWindow(m_pItem->GetAxControl(), SW_HIDE);
	::SetParent(m_pItem->GetAxControl(), NULL);

	
	if(isTrueClose)
		::PostMessage(m_pItem->GetAxControl(), WM_CLOSE, 0, 0);
	if (m_pItem)
	{
		CTabItem *pItem = m_pItem;
		m_pItem = NULL;
		delete pItem;
	}
	if(isTrueClose)
		DestroyWindow();
}

void CChildFrame::DoNavigateBack()
{
	PostMessage2AxWnd(WM_AX_GOBACK);
}

void CChildFrame::DoNavigateForward()
{
	PostMessage2AxWnd(WM_AX_GOFORWARD);
}

void CChildFrame::DoNavigateRefresh()
{
	PostMessage2AxWnd(WM_AX_REFRESH);
}

void CChildFrame::DoShowSSLStatus()
{
	PostMessage2AxWnd(WM_AX_SSLSTATUS);
}

//////////////////////////////////////////////////////////////////////////

LRESULT CChildFrame::PostMessage2AxWnd(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pItem && m_pItem->GetAxControl().IsWindow())
		return m_pItem->GetAxControl().PostMessage(uMsg, wParam, lParam);
	return 0;
}

LRESULT CChildFrame::PostTextToAxWnd(UINT Msg, LPCTSTR lpszStr)
{
//	if (m_pItem && m_pItem->GetAxControl().IsWindow())
//		MainUIFrame::PostProcessMessageStringWithPrefix(m_pItem->GetAxControl(), Msg, lpszStr);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// override

void CChildFrame::OnFinalMessage(HWND /* hWnd */)
{
	ATLASSERT(m_pItem == NULL);
	delete this;
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetWindowLongPtr(GWLP_USERDATA, (LONG_PTR)this);
	return 0;
}

void CChildFrame::OnDestroy()
{
}

void CChildFrame::OnClose()
{
	FS()->MDI()->ClosePage(m_pItem);
}

void CChildFrame::OnShowWindow(BOOL bShowing, int nReason)
{
	if(bShowing == TRUE)
		if (m_pItem && m_pItem->GetAxControl())
			m_pItem->GetAxControl().PostMessage(WM_SHOWWINDOW, (WPARAM)bShowing, (LPARAM)nReason);
}

void CChildFrame::OnSize(UINT nType, CSize size)
{
	if (m_pItem && m_pItem->GetAxControl())
		m_pItem->GetAxControl().SetWindowPos(NULL, 0, 0, size.cx, size.cy, /*SWP_NOMOVE |*/ SWP_NOZORDER);
}
// 获取账单关闭整个页面
LRESULT CChildFrame::OnFinishBill(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if(m_pItem)
		FS()->MDI()->CloseCategory(m_pItem->m_pCategory);
	::PostMessage(FS()->MainFrame()->m_IeWnd, WM_FINISH_GET_BILL, 0 ,0);
	::PostMessage(FS()->MainFrame()->m_hWnd, WM_SWITCHTOPPAGE, (WPARAM)kFinancePage ,0);
	return 0;
}


LRESULT CChildFrame::OnGettingBill(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if(m_pItem)
	{
		m_pItem->m_pCategory->SetShowInfo( 2 );
		FS()->MainFrame()->RecalcClientSize(-1, -1);
	}
	return 0;
}

LRESULT CChildFrame::OnResetGetBill(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if(m_pItem)
	{
		m_pItem->m_pCategory->SetShowInfo( 1 );
		FS()->MainFrame()->RecalcClientSize(-1, -1);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////

LRESULT CChildFrame::OnNotifyCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
	{
		m_pItem->SetAxControl((HWND)lParam);
		if (!m_bForuceNoNavigate)
			::SendMessage(m_pItem->GetAxControl(), WM_SETTEXT, WM_AX_NAVIGATE, (LPARAM)m_pItem->GetURLText());

		FS()->MainFrame()->RecalcClientSize(-1, -1);
	}

	return 0;
}

LRESULT CChildFrame::OnSetMainToolbar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
	{
		m_pItem->SetMainToolbarStatus(LOWORD(wParam), HIWORD(wParam) != FALSE);
		if (::IsWindowVisible2(m_hWnd))
		{
			//FS()->ToolBar()->SetButtonState(m_pItem->GetMainToolbarBackStatus() != 0, m_pItem->GetMainToolbarForwardStatus() != 0);			FS()->BackButton()->SetButtonState(m_pItem->GetMainToolbarBackStatus() != 0);
			FS()->BackButton()->SetButtonState(m_pItem->GetMainToolbarBackStatus() != 0);
			FS()->ForwardButton()->SetButtonState(m_pItem->GetMainToolbarForwardStatus() != 0);
		}
	}
	return 0;
}

LRESULT CChildFrame::OnSetStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
	{
		m_pItem->SetStatusText((LPCTSTR)lParam);
		if (::IsWindowVisible2(m_hWnd))
			FS()->Status()->SetStatusText(m_pItem->GetStatusText(), true);
	}
	return 0;
}

LRESULT CChildFrame::OnSetURL(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
	{
		LPCTSTR lpszURL = (LPCTSTR)lParam;
		m_pItem->SetURLText(lpszURL);
		if (::IsWindowVisible2(m_hWnd))
		{
			FS()->TitleBar()->Invalidate();
			FS()->MainFrame()->UpdateTitle();
		}
	}
	return 0;
}

LRESULT CChildFrame::OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
	{
		LPCTSTR lpszText = (LPCTSTR)lParam;
		m_pItem->SetText(lpszText);
		if (::IsWindowVisible2(m_hWnd))
		{
			FS()->TitleBar()->Invalidate();
			FS()->MainFrame()->UpdateTitle();
		}
	}
	return 0;
}


LRESULT CChildFrame::OnGetAxControl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
		return (LRESULT)m_pItem->GetAxControl().m_hWnd;
	else
		return (LRESULT)NULL;
}

LRESULT CChildFrame::OnSetSSLState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{	
	if (m_pItem)
	{
		m_pItem->SetSecure(lParam != 0);

		if (FS()->TabItem() == m_pItem)
			FS()->MainFrame()->UpdateSSLState();

		m_pItem->m_pCategory->Redraw();
	}

	return 0;
}

LRESULT CChildFrame::OnTabClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
		FS()->MDI()->CloseCategory(m_pItem->m_pCategory);

	return 0;
}

LRESULT CChildFrame::OnAskSaveFav(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LPCTSTR lpszInfo = (LPCTSTR)lParam;
	int res = mhMessageBox(FS()->hMainFrame, lpszInfo, L"财金汇", MB_OKCANCEL);

	return res;
}

LRESULT CChildFrame::OnTestSaveFav(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	USES_CONVERSION;
	LPCTSTR lpszInfo = (LPCTSTR)lParam;
	wstring text((LPCTSTR)lParam);
	wstring waid = text.substr(0, 4);
	wstring msg = text.substr(4, text.length() - 1);
	string aid( W2A(waid.c_str()) );

	CSaveFavProgressDlg dlg(aid, msg);
	int res = dlg.DoModal(FS()->hMainFrame);

	return res;
}
LRESULT CChildFrame::OnAutoClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
		FS()->MDI()->ClosePage(m_pItem);

	return 0;
}

LRESULT CChildFrame::OnSetPageProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if (m_pItem)
	{
		m_pItem->SetProgress(wParam);

		if (FS()->TabItem() == m_pItem)
			FS()->Status()->SetPageProgress(wParam);
	}

	return 0;
}

LRESULT CChildFrame::OnToggleCateCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	FS()->pCate->ToggleItem();

	return 0;
}

