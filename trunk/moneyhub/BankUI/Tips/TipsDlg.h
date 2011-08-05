#pragma once

#include "../resource/Resource.h"
#include "../Util/ThreadStorageManager.h"
#include "atlctrlx.h"
#include "atlwinex.h"
#include "../UIControl/FloatingTip/Structs.h"
#include "../../ThirdParty/tinyxml/tinyxml.h"
#include "../Skin/SkinManager.h"
#include "../UIControl/AltSkinClasses.h"


#define MAX_SHOWPAGE 20

class CTipsDlg : public CDialogImpl<CTipsDlg>, public CDialogSkinMixer<CTipsDlg>
{
public:
	CTipsDlg(int page);
	~CTipsDlg();

public:
	enum { IDD = IDD_DIALOG_TIPS };

	BEGIN_MSG_MAP(CTipsDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CTipsDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		COMMAND_ID_HANDLER(IDC_PREPAGE, OnPrePage)
		COMMAND_ID_HANDLER(IDC_NEXTPAGE, OnNextPage)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPrePage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNextPage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	void OnMouseMove(UINT nFlags, CPoint point) ;

protected:
	int m_page;//记录第几页的属性

	UINT m_nFilesPerType;

	CStatic m_tipsString;
	CStatic m_titleString;
	CStatic m_pic;
	CStatic m_guideString;

	CString m_strInfo;

	CTuoImage m_imgCheckBox;	//checkbox的图片
	CTuoImage m_showList[MAX_SHOWPAGE];		//显示的图片
	std::wstring m_showtitle[MAX_SHOWPAGE];
	std::wstring m_showstring[MAX_SHOWPAGE];
	int TOTAL_PAGE;

	bool    m_bChecked;			//记录复选框选中状态
	CRect	m_rcCheckBox ;		// 复选框图片区域
	CRect	m_rcCheckBoxRgn ;	// 复选框区域
	DWORD	m_dwCheckState ;	// 复选框当前状态

	void GetConfig();

public:
	void Refresh();
	bool IsVista();
};