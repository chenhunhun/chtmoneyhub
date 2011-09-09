#pragma once
#include "../BaseClass/NormalEditBase.h"


class CAddressComboBoxEditCtrl : public CNormalEditControlBase<CAddressComboBoxEditCtrl>, public CFSMUtil
{

public:

	CAddressComboBoxEditCtrl(FrameStorageStruct *pFS, CBaseProperties *pBaseProperties);

	void SetText(LPCTSTR lpszText);

	// override

	bool SelfPaint(CDCHandle dc, const RECT &rc);
	bool IsTransparentDraw() const;
	void DrawContainerBackground(CDCHandle dc, const RECT &rc);

	// message

	BEGIN_MSG_MAP_EX(CAddressComboBoxCtrl)
		MSG_WM_CONTEXTMENU(OnContextMenu)

		CHAIN_MSG_MAP(CNormalEditControlBase<CAddressComboBoxEditCtrl>)

		MSG_WM_KILLFOCUS(OnKillFocus)

		MESSAGE_HANDLER_EX(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER_EX(WM_SYSKEYDOWN, OnKeyDown)
		MESSAGE_HANDLER_EX(WM_CHAR, OnChar)
		MESSAGE_HANDLER_EX(WM_IME_NOTIFY, OnIMENotify)
		MESSAGE_HANDLER_EX(WM_IME_STARTCOMPOSITION, OnIMEStartComposition)
		MESSAGE_HANDLER_EX(WM_IME_ENDCOMPOSITION, OnIMEEndComposition)
	END_MSG_MAP()

	// message handler

	void OnKillFocus(CWindow wndFocus);
	void OnContextMenu(CWindow wnd, CPoint point);

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnIMENotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnIMEStartComposition(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnIMEEndComposition(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);


	CString m_strBeforeIME;

	bool m_bCanAutoComplete;

public:

	DECLARE_WND_SUPERCLASS(_T("MH_TuotuoAddressBarEditCtrl"), GetWndClassName())
};
