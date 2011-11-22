#pragma once

#include "CoolMessageBox.h"
#include "AltSkinClasses.h"
#include <Message.h>

class CUserLoadInfoDlg : public CDialogImpl<CUserLoadInfoDlg>//, public CDialogSkinMixer<CMenuDlg>
{
public:
	enum { IDD = IDD_DLG_MENU };

	BEGIN_MSG_MAP(CUserLoadInfoDlg)
		//CHAIN_MSG_MAP(CDialogSkinMixer<CMenuDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActive)
		//MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
		//MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

public:
	CUserLoadInfoDlg(HWND hParent, LPCTSTR lpBitPath);
	~CUserLoadInfoDlg(void);
	void ShowMenuWindow(CPoint pt);

private:
	HRGN CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF clrTrans, LPCRECT lprcBounds);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnLButtonDown(UINT nFlags, CPoint point);
	
protected:
	wstring	m_wstrBitPath; // ²Ëµ¥Í¼Æ¬Ãû³Æ
	CTuoImage m_imgNoBtnSel; // ²Ëµ¥±³¾°Í¼Í¼Æ¬
	HWND m_hParent;
	//CButton* m_pBtnRetry;
	//CButton* m_pBtnChange;
	//CFont m_Font;

};
