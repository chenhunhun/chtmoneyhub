#pragma once

#include "../resource/Resource.h"
#include "../Util/ThreadStorageManager.h"
#include "../UIControl/FloatingTip/Structs.h"
#include "../Skin/SkinManager.h"
#include "../UIControl/AltSkinClasses.h"
#include "../../BankData/BankData.h"

#define CHECKBOX_SHOW_ONE_LINE	0x00000001 // 该复选框在新的一行单独显示


#define STATIC_CTRL_ID_SEL_ALL			WM_USER + 1000
#define STATIC_CTRL_ID_SEL_NON			WM_USER + 1001

class CMyCheckBox
{
public:
	CMyCheckBox();
	bool Create(HWND hParent, CRect& rect, int nID, const char* pStr, bool bCheck);
	void GetCtrlRectWithoutStatic(CRect& rect);
	void GetCtrlRect(CRect& rect);
	void ButtonClick();
	void SetCheckStatues(DWORD bStatues); 
	DWORD GetCheckStatues(void);
	bool IsChecked();
	int GetCtrlID();
private:
	int			m_nID;
	bool		m_bChecked;
	DWORD		m_dwStatue;
	CRect		m_rectNoStatic;
	CStatic*	m_pStatic;
	CRect		m_rectWhole;

	CFont		m_font;
	
};

typedef std::list<CMyCheckBox*> MyCheckBoxList;
//typedef std::map<char*, DWORD> SelectMonthNode;


class CMonthSelectDlg : public CDialogImpl<CMonthSelectDlg>, public CDialogSkinMixer<CMonthSelectDlg>
{
public:
	CMonthSelectDlg(IN OUT list<SELECTINFONODE>* pMonthNode, IN char* pDlgText = NULL, IN char* pTitleText = NULL);
	~CMonthSelectDlg();

public:
	enum { IDD = IDD_DLG_BILL_SELECT };

	BEGIN_MSG_MAP(CMonthSelectDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CMonthSelectDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
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

	void OnSetAllCheckButtonStatus(DWORD dwStatus);
	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	void OnMouseMove(UINT nFlags, CPoint point) ;

private:
	list<SELECTINFONODE>*		m_pMonthNode;
	MyCheckBoxList				m_CheckBoxArr;
	CTuoImage					m_imgCheckBox;	//checkbox的图片
//	CStatic*					m_pStaticeTitle; // 标题页
	std::string					m_strDlgText;
	std::string					m_strTitleText;
	CStatic*					m_pStaticSelAll; // 全选
	CRect						m_cRectSelAll;
	CStatic*					m_pStaticSelNon; // 全不选
	CRect						m_cRectSelNon;
	CFont						m_sFont;//显示文字的字体
};