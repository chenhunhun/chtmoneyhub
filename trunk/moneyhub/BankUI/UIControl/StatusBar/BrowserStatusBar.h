#pragma once
#include "../FrameBorder/SizingBorder.h"
#include "../../Util/ThreadStorageManager.h"


class CBrowserStatusBarCtrl : public CWindowImpl<CBrowserStatusBarCtrl>, public CSizingBorder<CBrowserStatusBarCtrl>, public CFSMUtil
{
public:
	CBrowserStatusBarCtrl(FrameStorageStruct *pFS);

	void CreateBrowserStatusBar(HWND hParent);
	void SetPageProgress(int iProgress);
	void SetStatusText(LPCTSTR lpszStatusText, bool bIgnoreUnderline);
	void SetStatusIcon(int iIcon);

private:
	// message
	BEGIN_MSG_MAP_EX(CBrowserStatusBarCtrl)
		CHAIN_MSG_MAP(CSizingBorder<CBrowserStatusBarCtrl>)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_SIZE(OnSize)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

	// message handler

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnPaint(CDCHandle dc, RECT rect);
	void OnSize(UINT nType, CSize size);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnTimer(UINT_PTR nIDEvent);

	// member

	bool m_bShowProgress;
	int m_iProgress;

	int m_iIconIndex;
	CRect m_rcPageIcon;

	std::tstring m_strText;
	bool m_bIgnoreUnderline;

public:

	DECLARE_WND_SUPERCLASS(_T("MH_TuotuoStatusbar"), GetWndClassName())
};
