#pragma once

#include "CoolMessageBox.h"
#include "AltSkinClasses.h"
#include <Message.h>
#include <atltime.h>

class CUserLoadInfoDlg : public CDialogImpl<CUserLoadInfoDlg>//, public CDialogSkinMixer<CMenuDlg>
{
public:
	enum { IDD = IDD_DLG_MENU };

	BEGIN_MSG_MAP(CUserLoadInfoDlg)
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
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

public:
	CUserLoadInfoDlg(HWND hParent, LPCTSTR lpBitPath);
	~CUserLoadInfoDlg(void);
	void ShowMenuWindow(CPoint pt);
	void KeepWindowForSeconds(int nSecond); // 保留窗口（多少）秒
	bool IsWindowShow(void); // 窗口是否显示
	void InitSendMailTime(void); // 重置发送邮件时间


private:
	HRGN CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF clrTrans, LPCRECT lprcBounds);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnTimer(UINT_PTR nIDEvent);
	
protected:
	wstring	m_wstrBitPath; // 菜单图片名称
	CTuoImage m_imgNoBtnSel; // 菜单背景图图片
	HWND m_hParent;
	CTime m_LastTime;
	BOOL m_bTimerTag; // 标记是否启动保留对话框的功能
	BOOL m_bShow; // 标记对话框是否显示
};
