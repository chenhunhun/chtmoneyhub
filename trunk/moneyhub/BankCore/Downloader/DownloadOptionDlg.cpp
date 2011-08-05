
#include "stdafx.h"
#include "DownloadOptionDlg.h"
#include "Util.h"
#include "..\..\BankUI\UIControl\CoolMessageBox.h"

CDownloadOptionDlg::CDownloadOptionDlg(CString strFileName, CString strURL)
: m_strFileName(strFileName), m_strURL(strURL), m_strFullFilePath(_T("")), m_bFileNameModified(false)
{
}

LRESULT CDownloadOptionDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	SetWindowText(_T("ÎÄ¼þÏÂÔØ"));

	SetDlgItemText(IDC_STATIC_FILENAME, m_strFileName);
	SetDlgItemText(IDC_STATIC_URL, m_strURL);

	ApplyButtonSkin(IDC_SAVE);
	ApplyButtonSkin(IDCANCEL);

	return TRUE;
}

LRESULT CDownloadOptionDlg::OnButtonSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(FALSE, NULL, m_strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER);
	if (IDOK == dlg.DoModal())
	{
		m_strFullFilePath = dlg.m_szFileName;

		if (_tcsicmp(dlg.m_szFileTitle, m_strFileName) == 0)
			m_bFileNameModified = false;
		else
			m_bFileNameModified = true;

		EndDialog(IDC_SAVE);
	}

	return 0;
}

LRESULT CDownloadOptionDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return 0;
}
