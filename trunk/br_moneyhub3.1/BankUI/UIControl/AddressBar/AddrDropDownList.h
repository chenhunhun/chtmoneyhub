#pragma once


struct AddrListDataLocal
{
	int iType;
	CString strTitle;
	CString strURL;

	CString strTitleMask;
	CString strURLMask;
	HICON hSiteIcon;
	int iRank;
};

struct AddrListData
{
	int iType;
	std::wstring strTitle;
	std::wstring strURL;
	int iRank ;
};


class CAddressComboBoxCtrl;

class CAddrDropDownList : public CWindowImpl<CAddrDropDownList>, public CFSMUtil
{

public:

	CAddrDropDownList(FrameStorageStruct *pFS);
	~CAddrDropDownList();

	HWND CreateDropDownList(CAddressComboBoxCtrl *pComboBox);
	void UpdateDropDownListStyle();

	void ShowDropDownList();
	void HideDropDownList(bool bAcceptNewSuggestion);

	void GenSearchRequest(LPCTSTR lpszSearchText);
	int GetCurrentSelection() const		{ return m_iCurrentSelection; }
	bool CheckURLIsInSuggestionList(LPCTSTR lpszURL) const;			// 为了pingback使用，检查一下给定的url是否在下拉列表的suggestion里面

	ButtonStatus GetDelButtonStatus(const RECT& rcItem, CPoint& pt);

	void RemoveDropDownSelection(int iSel);

private:

	void SetSelection(int iSelection);
	void OnMouseClickWhenDropDown(HWND hWnd, POINT pt);

	static LRESULT CALLBACK MouseProc(int iCode, WPARAM wParam, LPARAM lParam);

	// message

	BEGIN_MSG_MAP_EX(CAddrDropDownList)
		MSG_WM_MOUSEACTIVATE(OnMouseActivate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_VSCROLL(OnVScroll)
		MSG_WM_KEYDOWN(OnKeyDown)
	END_MSG_MAP()

	// message handler

	int OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message) { return MA_NOACTIVATE; }
	BOOL OnEraseBkgnd(CDCHandle dc) { return TRUE; }
	void OnPaint(CDCHandle dc, RECT rect);

	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	LRESULT OnSetDropDownContent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnAddDropDownContent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);

	// member

	CAddressComboBoxCtrl *m_pComboBox;

	AddrListData m_AddrListData[20];
	AddrListData m_AddrListSearchData[5];

	AddrListDataLocal m_LocalData[25];
	int m_iDisplayDataCount;			// 实际显示的条数
	int m_iRealDataCount;			// 真正有效的条数（用于当搜索结果返回之后是否覆盖原有结果）
	int m_iLastSeq;

	bool m_bBigDropDown;
	int m_iPageDisplaySize;
	int m_iCurrentSelection;
	ButtonStatus m_eDeleteButtonStatus;

	POINT m_ptLastMousePos;

	typedef std::set<std::tstring> StrSet;
	StrSet m_LocalURLSet;


	HHOOK m_hMsgHook;
	static CAddrDropDownList* sm_pCurrentHookingAddrDropDown;


	DECLARE_WND_CLASS(_T("MH_TuoAddrDropDown"))
};
