#include "stdafx.h"
#include "ShowJsChild.h"
#include "../Util/ProcessManager.h"


#define CHECK_VALID_ITEM()			do { if (m_pItem == NULL) return 0; } while (0)

CShowJSChild::CShowJSChild(HWND hWnd, LPCTSTR lpszWindowPath, int nErrCode) :m_nErrCode(nErrCode)
{
//	CString strURL = lpszWindowPath ? (lpszWindowPath[0] ? lpszWindowPath : _T("about:blank")) : _T("about:blank");

	m_tcsHtmlName = lpszWindowPath;
	CRect rc;
	::GetClientRect(hWnd, &rc);
	rc.DeflateRect(2, 2);
	rc.top = 34;
	rc.bottom = 315;
	
	HWND hChildFrame = Create(hWnd, rc, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	ATLASSERT(hChildFrame && "Create Childframe failed.");

	CProcessManager::_()->CreateNewWebPage(m_hWnd);
	::ShowWindow(hChildFrame, true);

}

//////////////////////////////////////////////////////////////////////////

void CShowJSChild::OptionalDestroy(int iType, bool isTrueClose)
{
	SetWindowLongPtr(GWLP_USERDATA, NULL);
	m_iDestroyType = iType;

	if(isTrueClose)
		DestroyWindow();
}

//////////////////////////////////////////////////////////////////////////

LRESULT CShowJSChild::PostMessage2AxWnd(UINT uMsg, WPARAM wParam, LPARAM lParam)
{/*
	if (m_pItem && m_pItem->GetAxControl().IsWindow())
		return m_pItem->GetAxControl().PostMessage(uMsg, wParam, lParam);*/
	return 0;
}

LRESULT CShowJSChild::PostTextToAxWnd(UINT Msg, LPCTSTR lpszStr)
{
//	if (m_pItem && m_pItem->GetAxControl().IsWindow())
//		MainUIFrame::PostProcessMessageStringWithPrefix(m_pItem->GetAxControl(), Msg, lpszStr);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// override

void CShowJSChild::OnFinalMessage(HWND /* hWnd */)
{
//	ATLASSERT(m_pItem == NULL);
	delete this;
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CShowJSChild::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetWindowLongPtr(GWLP_USERDATA, (LONG_PTR)this);
	return 0;
}

void CShowJSChild::OnDestroy()
{
}

void CShowJSChild::OnClose()
{
	//FS()->MDI()->ClosePage(m_pItem);
}

void CShowJSChild::OnShowWindow(BOOL bShowing, int nReason)
{
	/*if(bShowing == TRUE)
		if (m_pItem && m_pItem->GetAxControl())
			m_pItem->GetAxControl().PostMessage(WM_SHOWWINDOW, (WPARAM)bShowing, (LPARAM)nReason);*/
}

void CShowJSChild::OnSize(UINT nType, CSize size)
{
	//if (m_pItem && m_pItem->GetAxControl())
	//	m_pItem->GetAxControl().SetWindowPos(NULL, 0, 0, size.cx, size.cy, /*SWP_NOMOVE |*/ SWP_NOZORDER);
}

//////////////////////////////////////////////////////////////////////////

LRESULT CShowJSChild::OnNotifyCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{

	HWND hAxHwnd = (HWND)lParam;
	TCHAR szPath[1024];
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	TCHAR *p = _tcsrchr(szPath, '\\');
	
	*p = '\0';
	std::wstring wstrURL = szPath;
	wstrURL += L"\\Html\\LoadAndLogin\\";
	wstrURL += m_tcsHtmlName;


	if (NULL != hAxHwnd)
	{
		::SendMessage(hAxHwnd, WM_SETTEXT, WM_AX_NAVIGATE, (LPARAM)wstrURL.c_str());

		// 在界面上弹出出错信息
		if (0 != m_nErrCode)
			::PostMessage(hAxHwnd, WM_AX_CALL_JS_SHOW_TEXT, m_nErrCode, 0);
	}

	return 0;
}

