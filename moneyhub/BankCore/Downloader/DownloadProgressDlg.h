
#pragma once

#include "../../BankUI/Resource/resource.h"
#include "../DownloadManagerImpl.h"
#include "../../BankUI/UIControl/CoolMessageBox.h"

class CDownloadProgressDlg : public CDialogImpl<CDownloadProgressDlg>, public CDialogSkinMixer<CDownloadProgressDlg>
{
public:
	CDownloadProgressDlg(ThreadDataStruct* pTds);
	~CDownloadProgressDlg();

public:
	enum { IDD = IDD_DIALOG_DOWNLOADPROGRESS };

	BEGIN_MSG_MAP(CDownloadProgressDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CDownloadProgressDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_URLDOWNLOADFROMURL_STATUS, OnDownloadStatus)
		MESSAGE_HANDLER(WM_URLDOWNLOADFROMURL_FINISH, OnDownloadFinish)
		MESSAGE_HANDLER(WM_URLDOWNLOADFROMURL_FAILED, OnDownloadFailed)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDownloadStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDownloadFinish(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDownloadFailed(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

public:
	void UpdateFileName(CString strFileName);
	bool DoSysClose();

protected:
	//CBindStatusCallback* m_pBSCB;
	ThreadDataStruct m_tds;
	ThreadDataStruct* m_pInputTds;

	bool m_bFinished;
	bool m_bFailed;
	bool m_bIsContentLength;

	bool m_bCanceled;

protected:
//	CProgressBarCtrl	m_pbPercent;
	int					m_nPercent;
	CRect				m_rectProgress;
};