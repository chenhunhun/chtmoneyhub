#pragma once
#include "../BaseClass/TuoToolbar.h"
#include "../BaseClass/MouseObjChild.h"
#include "AddrComboBox.h"


class CAddressBarCtrl : public CTuoToolBarCtrl<CAddressBarCtrl>, public CMouseObjChild<CAddressBarCtrl>, public CFSMUtil
{

public:

	CAddressBarCtrl(FrameStorageStruct *pFS);

	void CreateAddressBar(HWND hWndParent);
	void RefreshCoreType(int iCoreType);
	void Refresh();

	// override

	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

	bool CheckIsMouseOver(POINT pt);

private:

	// message

	BEGIN_MSG_MAP_EX(CAddressBarCtrl)
		CHAIN_MSG_MAP(CMouseObjChild<CAddressBarCtrl>)

		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)

		CHAIN_MSG_MAP(CTuoToolBarCtrl<CAddressBarCtrl>)

		MSG_WM_CREATE(OnCreate)
		MSG_WM_SETFOCUS(OnSetFocus)

		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnGetDispInfo)
		TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	// message handler

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnWindowPosChanged(LPWINDOWPOS lpWndPos);
	void OnSetFocus(CWindow wndOld);

	LRESULT OnGetDispInfo(LPNMHDR pnmh);
	LRESULT OnClick(int nIndex, POINT pt);


	CAddressComboBoxCtrl m_combo;

	int m_iCoreOfCurrentPage;		// 当前页面所使用的内核

public:

	DECLARE_WND_CLASS_NODRAW(_T("MH_TuotuoAddressBar"))
};
