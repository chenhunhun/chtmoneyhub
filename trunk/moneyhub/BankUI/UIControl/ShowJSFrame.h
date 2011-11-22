#pragma once

#include "../resource/Resource.h"
#include "../Util/ThreadStorageManager.h"
#include "../UIControl/FloatingTip/Structs.h"
#include "../Skin/SkinManager.h"
#include "../UIControl/AltSkinClasses.h"
#include "../Util/ThreadStorageManager.h"
#include "ShowJSChild.h"

#define END_SHOW_DIALOG	WM_USER + 100

class CShowJSFrameDlg : public CDialogImpl<CShowJSFrameDlg>, public CDialogSkinMixer<CShowJSFrameDlg>
{
public:
	CShowJSFrameDlg(LPCTSTR lpszWindowName, LPCTSTR lpszWindowPath);
	~CShowJSFrameDlg();

public:
	enum { IDD = IDD_DLG_BILL_SELECT };

	BEGIN_MSG_MAP(CShowJSFrameDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CShowJSFrameDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(END_SHOW_DIALOG, CloseDialog)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT CloseDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	CShowJSChild *		m_pChild;
	wstring				m_tcsHtmlName;
	wstring				m_tcsHtmlPath;
};