#include "stdafx.h"
#include "ShowJSFrame.h"



CShowJSFrameDlg::CShowJSFrameDlg(LPCTSTR lpszWindowName, LPCTSTR lpszWindowPath):m_pChild(NULL)
{
	m_tcsHtmlName = lpszWindowName;
	m_tcsHtmlPath = lpszWindowPath;
}

CShowJSFrameDlg::~CShowJSFrameDlg()
{
	if (NULL != m_pChild)
		delete m_pChild;

	m_pChild = NULL;
}



LRESULT CShowJSFrameDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	::SetWindowText(m_hWnd, m_tcsHtmlName.c_str());
	CShowJSChild *m_pChild = new CShowJSChild(m_hWnd, m_tcsHtmlPath.c_str());
	CRect rect;
	GetClientRect(&rect);
	MoveWindow(0, 0, rect.Width(), 317);
	//::SetWindowPos(m_hWnd, NULL, 0, 0, rect.Width() - 5, 327, SWP_NOSIZE | SWP_NOOWNERZORDER);
	::ShowWindow(GetDlgItem(IDOK), false);
	CenterWindow(GetParent());
	return 0;
}

LRESULT CShowJSFrameDlg::CloseDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(0);
	return 0;
}