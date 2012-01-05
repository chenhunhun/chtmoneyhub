#pragma once

#include "../resource/Resource.h"
#include "../Util/ThreadStorageManager.h"
#include "../Util/SecurityCheck.h"

#define	WM_SETSHOWINDEX	(WM_USER+0x103)
#define	SECU_DLG_TITLE _T("财金汇安全验证")

/*
 
 检测步骤:

 (1) 财金汇组件检测
 (2) 财金汇数据文件检查
 (3) 第三方组件检测
 (4) 系统组件检测

*/

class CSecuCheckDlg : public CDialogImpl<CSecuCheckDlg>
{
public:
	CSecuCheckDlg(bool bInstall = false,bool bShow = true);

public:
	enum { IDD = IDD_DIALOG_SECUCHECK };

	BEGIN_MSG_MAP(CSecuCheckDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MESSAGE_HANDLER(WM_SETSHOWINDEX, OnSetShowIndex)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void OnShowWindow(BOOL bShowing, int nReason);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnTimer(UINT_PTR nIDEvent) ;
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSetShowIndex(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	static void _CheckMgrCallback(CheckStateEvent ev, DWORD dw, LPCTSTR lpszInfo, LPVOID lp);

protected:
	static bool m_bShow;
	static bool m_bInstall;
	static bool m_bEnd;

	UINT m_nFileCount;
	UINT m_nCurFile;

	UINT m_nTypeCount;
	UINT m_nCurType;

	UINT m_nFilesPerType;

	DWORD m_dwProgressPos;
	DWORD m_dwPercent;
	CString m_strInfo;
};