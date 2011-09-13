#pragma once
#include "../../Util/ThreadStorageManager.h"


class CBigButtonMenu : public CWindowImpl<CBigButtonMenu>, public CFSMUtil
{

public:

	CBigButtonMenu(FrameStorageStruct *pFS);
	~CBigButtonMenu();

	HWND CreateBigButtonMenu(HWND hFrameWnd);

	void ShowMenu();
	void HideMenu();

private:

	// message

	BEGIN_MSG_MAP_EX(CAddrDropDownList)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONUP(OnLButtonUp)
	END_MSG_MAP()

	// message handler
	BOOL OnEraseBkgnd(CDCHandle dc) { return TRUE; }
	void OnPaint(CDCHandle dc, RECT rect);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);


	int m_iCurrentSelection;
	ButtonStatus m_eDeleteButtonStatus;


	DECLARE_WND_CLASS(_T("MH_TuoAddrDropDown"))
};
