#pragma once
#include "../../Util/ThreadStorageManager.h"
#include "../BaseClass/TuoToolbar.h"
#include "../Menu/Menu.h"
#include "../FrameBorder/SizingBorder.h"


class CTitleBarCtrl : public CTuoToolBarCtrl<CTitleBarCtrl>, public CSizingBorder<CTitleBarCtrl>, public CFSMUtil
{

public:

	CTitleBarCtrl(FrameStorageStruct *pFS);
	~CTitleBarCtrl();

	void CreateTitleBar(HWND hParent);

	// override

	void DrawBackground(HDC hDC, const RECT &rc);

private:

	// message

	BEGIN_MSG_MAP_EX(CTitleBarCtrl)
		CHAIN_MSG_MAP(CSizingBorder<CTitleBarCtrl>)

		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)

		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_NCLBUTTONUP(OnNcLButtonUp)

		CHAIN_MSG_MAP(CTuoToolBarCtrl<CTitleBarCtrl>)		// 这个必须放在最底下，因为鼠标消息要被上面的截掉
	END_MSG_MAP()

	// message handler

	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnNcLButtonUp(UINT nHitTest, CPoint point);

	// member

	HHOOK m_hSysMenuHook, m_hSysMenuHook2;
	static CTitleBarCtrl* sm_pCurrentHookingTitlebar;

	bool m_bDoubleClkDouble;

public:

	DECLARE_WND_CLASS_NODRAW(_T("MH_TuotuoTitlebar"))
};
