#pragma once

#include "CoolMessageBox.h"
#include "AltSkinClasses.h"
#include <Message.h>

class CMenuDlg : public CDialogImpl<CMenuDlg>//, public CDialogSkinMixer<CMenuDlg>
{
public:
	enum { IDD = IDD_DLG_MENU };

	BEGIN_MSG_MAP(CMenuDlg)
		//CHAIN_MSG_MAP(CDialogSkinMixer<CMenuDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActive)
		MSG_WM_MOUSEMOVE(OnMouseMove)
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
	CMenuDlg(HWND hParent);
	~CMenuDlg(void);
	void ShowMenuWindow(CPoint pt);

private:
	HRGN CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF clrTrans, LPCRECT lprcBounds);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnMouseMove(UINT /* nFlags */, CPoint ptCursor);
	void OnLButtonDown(UINT nFlags, CPoint point);

	
protected:
	//CTuoImage m_imgRngBg;
	CTuoImage m_imgNoBtnSel;

	/*CTuoImage m_imgSelBtn1;
	CTuoImage m_imgSelBtn2;
	CTuoImage m_imgSelBtn3;
	CTuoImage m_imgSelBtn4;*/
	
	HWND m_hParent;
	int m_nBtnSelIndex;
	int m_nBtnLastSel;

};
