#pragma once

#include "../resource/Resource.h"
#include "../Util/ThreadStorageManager.h"
#include "atlctrlx.h"
#include "atlwinex.h"
#include "../UIControl/FloatingTip/Structs.h"
#include "../../ThirdParty/tinyxml/tinyxml.h"
#include "../Skin/SkinManager.h"
#include "../UIControl/AltSkinClasses.h"

#define CHECKBOX_SHOW_ONE_LINE	0x00000001 // 该复选框在新的一行单独显示
#define CHECKBOX_SHOW_CHECKED	0x00000010 // 显示时以选中的状态显示
#define CHECKBOX_SHOW_UNCHECKED	0x00000100 // 显示时以没有选中的状态显示

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
	
};

typedef std::list<CMyCheckBox*> MyCheckBoxList;
typedef std::map<char*, DWORD> SelectMonthNode;

class CMonthSelectDlg : public CDialogImpl<CMonthSelectDlg>, public CDialogSkinMixer<CMonthSelectDlg>
{
public:
	CMonthSelectDlg(IN OUT SelectMonthNode* pMonthNode, IN char* pDlgText = NULL, IN char* pTitleText = NULL);
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
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	void OnMouseMove(UINT nFlags, CPoint point) ;

private:
	SelectMonthNode*	m_pMonthNode;
	MyCheckBoxList		m_CheckBoxArr;
	CTuoImage			m_imgCheckBox;	//checkbox的图片
	CStatic*			m_pStaticeTitle; // 标题页
	std::string			m_strDlgText;
	std::string			m_strTitleText;
};