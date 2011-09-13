
#include "stdafx.h"
#include "DownloadProgressDlg.h"

CDownloadProgressDlg::CDownloadProgressDlg(ThreadDataStruct* pTds)
: m_bFinished(false), m_bFailed(false), m_bCanceled(false)
{
	m_pInputTds = pTds;

	m_tds.hClient = pTds->hClient;
	m_tds.hCreateEvent = pTds->hCreateEvent;
	m_tds.strFileName = pTds->strFileName;
	m_tds.strUrl = pTds->strUrl;
	m_tds.bHasContentLength = pTds->bHasContentLength;
}

CDownloadProgressDlg::~CDownloadProgressDlg()
{
}

LRESULT CDownloadProgressDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	SetWindowText(_T("文件下载"));

	m_pbPercent = GetDlgItem(IDC_PROGRESS1);
	m_pbPercent.SetRange(0, 100);

	ApplyButtonSkin(IDCANCEL);
	ApplyStaticSkin(IDC_STATIC_UNKNOWNLENGTH);
	ApplyStaticSkin(IDC_STATIC_STATUS);
	ApplyStaticSkin(IDC_STATIC_FILENAME);
	
	//if (m_pBSCB->IsContentLength())
	if (m_tds.bHasContentLength)
	{
		m_bIsContentLength = true;
		::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTH), SW_HIDE);
		m_pbPercent.ShowWindow(SW_SHOW);
	}
	else
	{
		m_bIsContentLength = false;
		SetDlgItemText(IDC_STATIC_UNKNOWNLENGTH, _T("未知文件大小"));
		::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTH), SW_SHOW);
		m_pbPercent.ShowWindow(SW_HIDE);
	}

	SetDlgItemText(IDC_STATIC_FILENAME, m_tds.strFileName);

	m_pInputTds->hProgDialog = m_hWnd;
	SetEvent(m_tds.hCreateEvent);

	return TRUE;
}

LRESULT CDownloadProgressDlg::OnDownloadStatus(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	ProgStruct* pps = (ProgStruct*)wParam;
	VARIANT_BOOL* pCancel = (VARIANT_BOOL*)lParam;

	LONG lProgress = pps->lProgress;
	LONG lProgressMax = pps->lProgressMax;

	if (m_bIsContentLength)
	{
		if (lProgressMax > 0)
		{
			m_pbPercent.SetPos(lProgress * 100 / lProgressMax);
		}
	}
	else
	{
		if (lProgress != 0)
		{
			CString strText;
			strText.Format(_T("未知文件大小(已经下载%d字节)"), lProgress);
			SetDlgItemText(IDC_STATIC_UNKNOWNLENGTH, strText);
			::InvalidateRect(GetDlgItem(IDC_STATIC_UNKNOWNLENGTH), NULL, TRUE);
		}		
	}

	if (m_bCanceled)
	{
		*pCancel = VARIANT_TRUE;
		EndDialog(IDCANCEL);
	}

	return 0;
}

LRESULT CDownloadProgressDlg::OnDownloadFinish(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (m_bCanceled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	m_bFinished = true;
	SetDlgItemText(IDC_STATIC_STATUS, _T("文件下载完成！"));
	::InvalidateRect(GetDlgItem(IDC_STATIC_STATUS), NULL, TRUE);
	EndDialog(IDOK);

	return 0;
}

LRESULT CDownloadProgressDlg::OnDownloadFailed(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (m_bCanceled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	m_bFailed = true;
	SetDlgItemText(IDC_STATIC_STATUS, _T("文件下载失败！"));
	::InvalidateRect(GetDlgItem(IDC_STATIC_STATUS), NULL, TRUE);
	SetDlgItemText(IDCANCEL, _T("确定"));

	return 0;
}

void CDownloadProgressDlg::UpdateFileName(CString strFileName)
{
	SetDlgItemText(IDC_STATIC_FILENAME, strFileName);
	::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAME), NULL, TRUE);
}

bool CDownloadProgressDlg::DoSysClose()
{
	if (!m_bFailed && !m_bFinished)
	{
		m_bCanceled = true;
		::EnableWindow(GetDlgItem(IDCANCEL), FALSE);
	}
	else
	{
		EndDialog(IDCANCEL);
	}

	return true;
}

LRESULT CDownloadProgressDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoSysClose();

	return 0;
}