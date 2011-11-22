#pragma once

#include "../resource/Resource.h"
#include "../UIControl/FloatingTip/Structs.h"
#include "../Skin/SkinManager.h"
#include "../UIControl/AltSkinClasses.h"
#include "../../BankData/BankData.h"
#include <list>
using namespace std;

typedef std::map<char*, DWORD> SelectMonthNode;
#define CHECKBOX_SHOW_CHECKED	0x00000010 // 显示时以选中的状态显示

class CAccountSelectDlg : public CDialogImpl<CAccountSelectDlg>, public CDialogSkinMixer<CAccountSelectDlg>
{
public:
	CAccountSelectDlg(IN OUT list<SELECTINFONODE>* pAccountNode);
	~CAccountSelectDlg();

public:
	enum { IDD = IDD_DLG_BILL_SELECT };

	BEGIN_MSG_MAP(CAccountSelectDlg)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		CHAIN_MSG_MAP(CDialogSkinMixer<CAccountSelectDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtlColorStatic)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDOK, OnCancel)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	//void OnSetAllCheckButtonStatus(DWORD dwStatus);
	//void OnLButtonDown(UINT nFlags, CPoint point) ;
	//void OnLButtonUp(UINT nFlags, CPoint point) ;
	//void OnMouseMove(UINT nFlags, CPoint point) ;

private:
	list<SELECTINFONODE>*			m_pAccountNode;
	std::map<CButton*, string>		m_mapBtn;
	list<CStatic*>					m_listStatic;
	HBRUSH							m_bkBrush;
	HBRUSH							m_bkBrush2;
	CFont							m_font;
	vector<HWND>					m_radiolist;
	HWND							m_title;

	CFont							m_tFont;
	
};