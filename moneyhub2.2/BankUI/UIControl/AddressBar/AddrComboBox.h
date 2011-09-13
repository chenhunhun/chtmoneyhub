#pragma once
#include "../BaseClass/EditContainer.h"
#include "AddrBarAdditionTool.h"
#include "AddrDropDownList.h"
#include "AddrComboBoxEdit.h"


struct CWaitSearchResultTimer 
{
	DWORD uTimer;
	DWORD uStart;
	DWORD uWeight;
	CString strWord;
};

class CAddressBarCtrl;

class CAddressComboBoxCtrl : public CEditContainerBase<CAddressComboBoxCtrl>, public CFSMUtil
{

public:

	CAddressComboBoxCtrl(FrameStorageStruct *pFS);

	void CreateComboBox(HWND hParent);
	void AutoCompleteUserInput();

	bool IsInAutoCompletionMode() const		{ return m_bAlreadyAutoComplete; }

	void UpdateIcon(HICON hIcon);
	void UpdateURLText(LPCTSTR lpszURL, bool bForce);
	void UpdateResourceStatus(int iResType, int iCount);
	void UpdateDisplayResourceIcon();

	void DoSelectAddressComboBar();
	void DoShowDropDownList();

	void OnDropDownListHide();
	void GetSearchRecommend(std::wstring& strUrl, std::wstring& strWord);
	BOOL IsSearchComplete(const std::wstring& strWord);

	void StartNavigate(int iComeFromSource, int iShortcutKeyType, LPCTSTR lpszText);
	void OnEditBoxUserChange();
	void AddrEditKillFocus();
	int GetDropDownSelection();
	void IMENotify(bool bStartIME);

	void PrepareQuickComplete(bool bForce);
	void Refresh();

	// callback

	void PaintBackground(CDCHandle dc, RECT rc);
	void GetLeftAndRightMargin(int &left, int &right);
	bool IsTransparentDraw() const;

private:

	void ResetWaitSearch();
	void OpenRecommend();
	void OpenPage(LPCTSTR url, int iWeight);

	// message

	BEGIN_MSG_MAP_EX(CAddressComboBoxCtrl)
		CHAIN_MSG_MAP(CEditContainerBase<CAddressComboBoxCtrl>)

		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDownAndDblClk)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDownAndDblClk)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_CAPTURECHANGED(OnCaptureChanged)

		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER_EX(WM_SETTEXT, OnSetText)
		MESSAGE_HANDLER_EX(WM_TIMER, OnTimer)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	// message handler

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnLButtonDownAndDblClk(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnCaptureChanged(CWindow wnd);

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnSetDropDownContent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnAddDropDownContent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);

	LRESULT OnSetSearchRecommend(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	
	// members

	CAddressComboBoxEditCtrl m_editCtrl;

	// 用户是否处于补全状态
	// 用户按上下左右、pageup、pagedown，鼠标点击等任何取消补全选取的操作都会退出补全状态
	bool m_bAlreadyAutoComplete;

	// 用户正在输入、在列表中选择、补全之类，此时不接受browser发来的变更url的请求
	// 当用户按回车浏览url，或者按esc返回原始的url，或者焦点不在地址栏的时候，此值为false
	bool m_bUserTyping;

	CString m_strTextBeforeAutoCompletion;		// 应用自动补全之前用户输入的文字
	CString m_strCurrentURL;

	CAddrDropDownList m_DropDown;
	CAddrAdditionTool m_tool;

	CString m_strSearchRecommendUrl;
	CString m_strSearchRecommendWord;
	CString m_strSearchResultWord;

	std::vector<std::pair<std::wstring, std::wstring>> m_QuickCompleteStr;

	CWaitSearchResultTimer m_WaitSearch;

public:

	DECLARE_WND_CLASS(_T("MH_TuotuoAddressBarComboBox"))
};
