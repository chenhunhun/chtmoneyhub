#pragma once
#include "../Util/ThreadStorageManager.h"
#include "Message.h"

class CTabItem;


#define WM_ITEM_SHOW_LOGIN_DIALOG_ASYNC		(WM_USER + 0x345)

class CShowJSChild : public CWindowImpl<CShowJSChild>
{

public:

	CShowJSChild(HWND hParentWnd, LPCTSTR lpszWindowName);


	//CTabItem* GetItem() const				{ return m_pItem; }

	void OptionalDestroy(int iType, bool isTrueClose = true);

	void DoNavigateBack();
	void DoNavigateForward();
	void DoNavigateRefresh();
	void DoShowSSLStatus();
	//HWND GetHwnd(){return m_hWnd;}

private:

	LRESULT PostMessage2AxWnd(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);
	LRESULT SendMessage2AxWndTimeout(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0, LRESULT iDefReturnValue = 0, DWORD dwTimeout = 100);
	LRESULT PostTextToAxWnd(UINT Msg, LPCTSTR lpszStr);

	// override

	virtual void OnFinalMessage(HWND /* hWnd */);

	// message

	BEGIN_MSG_MAP_EX(CShowJSChild)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)

		HANDLE_TUO_SETTEXT()
		HANDLE_TUO_COPYDATA()

		MESSAGE_HANDLER(WM_ITEM_NOTIFY_CREATED, OnNotifyCreated)
		

	END_MSG_MAP()

	// message handler
	void OnShowWindow(BOOL bShowing, int nReason);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnClose();
	void OnSize(UINT nType, CSize size);

	LRESULT OnNotifyCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:

//	CTabItem *m_pItem;

	wstring m_tcsHtmlName;
	int m_iDestroyType;

	bool m_bCreatingAxControl;				// =true表示正在创建AxControl过程中，随后会收到WM_ITEM_NOTIFY_CREATE_AXCONTROL消息
	bool m_bForuceNoNavigate;				// =true表示在创建之后强制不navigate，主要用于点击链接打开窗口的情况

public:

	DECLARE_WND_CLASS_NODRAW(_T("Bank_ChildFrame"))
};
