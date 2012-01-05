#pragma once
#include "../Util/ThreadStorageManager.h"
#include "BaseClass/TuoToolbar.h"

#define ID_TOOLBAR_BACK				0
#define ID_TOOLBAR_FORWARD			1
#define ID_TOOLBAR_REFRESH			2

#define TOOLBAR_BTN_MAX				3


class CBrowserToolBarCtrl : public CTuoToolBarCtrl<CBrowserToolBarCtrl>, public CFSMUtil
{

public:

	CBrowserToolBarCtrl(FrameStorageStruct *pFS);


	void CreateBrowserToolbar(HWND hParent);

	void SetButtonState(bool bBackEnabled, bool bForwardEnabled);
	void Refresh();

	// override

	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

private:

	// message

	BEGIN_MSG_MAP_EX(CBrowserToolBarCtrl)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CBrowserToolBarCtrl>)

		MSG_WM_CREATE(OnCreate)
		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnGetDispInfo)

		TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	// message handler

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnGetDispInfo(LPNMHDR pnmh);

	LRESULT OnClick(int nIndex, POINT pt);

public:

	DECLARE_WND_CLASS_NODRAW(_T("MH_TuotuoToolbar"))
};
