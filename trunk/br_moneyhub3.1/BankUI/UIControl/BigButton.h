#pragma once
#include "../Util/ThreadStorageManager.h"
#include "BaseClass/TuoToolbar.h"
#include "FloatingTip/BigButtonMenu.h"


class CBigButton : public CTuoToolBarCtrl<CBigButton>, public CFSMUtil
{

public:

	CBigButton(FrameStorageStruct *pFS);


	void CreateBigButton(HWND hParent);

	// override

	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

private:

	// message

	BEGIN_MSG_MAP_EX(CBrowserToolBarCtrl)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)

		CHAIN_MSG_MAP(CTuoToolBarCtrl<CBigButton>)

		MSG_WM_CREATE(OnCreate)
		MSG_WM_TIMER(OnTimer)
		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnGetDispInfo)

		TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	// message handler

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnTimer(UINT_PTR nIDEvent);

	LRESULT OnGetDispInfo(LPNMHDR pnmh);

	LRESULT OnClick(int nIndex, POINT pt);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	CBigButtonMenu m_BigButtonMenu;

public:

	DECLARE_WND_CLASS_NODRAW(_T("MH_TuotuoToolbar"))
};
