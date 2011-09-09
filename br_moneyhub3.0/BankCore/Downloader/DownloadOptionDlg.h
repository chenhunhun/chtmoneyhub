
#pragma once

#include "../../BankUI/Resource/resource.h"
#include "../../BankUI/UIControl/CoolMessageBox.h"

class CDownloadOptionDlg : public CDialogImpl<CDownloadOptionDlg>, public CDialogSkinMixer<CDownloadOptionDlg>
{
public:
	enum { IDD = IDD_DIALOG_DOWNLOADOPTION };

	CDownloadOptionDlg(CString strFileName, CString strURL);

	BEGIN_MSG_MAP(CDownloadOptionDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CDownloadOptionDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_SAVE, OnButtonSave)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_SYSCLOSE, OnCancel)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnButtonSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

protected:
	CString m_strFileName;
	CString m_strURL;

public:
	CString m_strFullFilePath;
	bool m_bFileNameModified;
};
