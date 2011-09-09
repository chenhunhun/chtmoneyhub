#pragma once
#include "../../Util/ThreadStorageManager.h"
#include "../BaseClass/TuoToolbar.h"
#include "../FrameBorder/SizingBorder.h"


class CSystemButtonBarCtrl : public CTuoToolBarCtrl<CSystemButtonBarCtrl>, public CSizingBorder<CSystemButtonBarCtrl>, public CFSMUtil
{

public:

	CSystemButtonBarCtrl(FrameStorageStruct *pFS);


	void CreateSystemButtonBar(HWND hWndParent);
	void Refresh();

	// override

	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

private:

	// message

	BEGIN_MSG_MAP_EX(CSystemButtonBarCtrl)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)

		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)

		CHAIN_MSG_MAP(CSizingBorder<CSystemButtonBarCtrl>)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CSystemButtonBarCtrl>)

		MSG_WM_CREATE(OnCreate)

		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnGetDispInfo)
		TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	// message handler

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnGetDispInfo(LPNMHDR pnmh);
	LRESULT OnClick(int iIndex, POINT pt);

public:

	DECLARE_WND_CLASS_NODRAW(_T("MH_TuotuoSystemButtonBar"))
};
