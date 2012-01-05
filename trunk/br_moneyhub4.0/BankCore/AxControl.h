/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  AxControl.cpp
 *      说明：  控件控制
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#include <vector>
#pragma once

class CWebBrowserCore;
class CCustomSite;
class CWebBrowserEventsManager;


class CAxControl : public CWindowImpl<CAxControl>
{
	friend class CCustomSite;
	friend class CWebBrowserEventsManager;

public:

	CAxControl(HWND hChildFrame);
	~CAxControl();

	void OnFinalMessage(HWND);
	LRESULT CreateIEServer();

	BEGIN_MSG_MAP(CAxControl)
		HANDLE_TUO_SETTEXT()
		HANDLE_TUO_COPYDATA()

		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)

		MESSAGE_HANDLER(WM_AX_INIT_SYNCHRO_BTN, OnInitSynchroBtnStatus)
		MESSAGE_HANDLER(WM_AX_FRAME_SETALARM, OnFrameSetAlarm)
		MESSAGE_HANDLER(WM_AX_FRAME_CHANGE_PROGRESS, OnFrameChangeProgress)
		MESSAGE_HANDLER(WM_AX_TOOLS_CHANGE, OnToolsChange)
		//MESSAGE_HANDLER(WM_AX_COUPON_CHANGE, OnCouponChange)
		MESSAGE_HANDLER(WM_AX_FRAME_ADDFAV, OnFrameAddFav)
		MESSAGE_HANDLER(WM_AX_FRAME_DELETEFAV, OnFrameDelFav)

		MESSAGE_HANDLER(WM_AX_GET_BILL, OnGetBill)

		MESSAGE_HANDLER(WM_AX_CHANGE_SETTINT_STATUS, ChangeSettingShowStatus)
		MESSAGE_HANDLER(WM_CHANGE_FIRST_PAGE_SHOW, OnChangeFirtsPageShow)
		MESSAGE_HANDLER(WM_AX_SHOW_INFO_DLG, OnShowInfoDlg)
		MESSAGE_HANDLER(WM_AX_END_INFO_DLG, OnEndInfoDlg)
		MESSAGE_HANDLER(WM_AX_GET_ALL_BILL, OnGetAllBill)//获得了所有账单

		MESSAGE_HANDLER(WM_AX_ACCOUNT_SELECT, OnAccountSelect)
		MESSAGE_HANDLER(WM_AX_CALL_JS_TABACTIVE, OnCallJSTabActive)
		MESSAGE_HANDLER(WM_AX_CALL_JS_SHOW_TEXT, OnCallJSShowText)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)

		MESSAGE_HANDLER(WM_AX_NAVIGATE, OnNavigate)
		MESSAGE_HANDLER(WM_AX_GOBACK, OnGoBack)
		MESSAGE_HANDLER(WM_AX_GOFORWARD, OnGoForward)
		MESSAGE_HANDLER(WM_AX_REFRESH, OnRefresh)
		MESSAGE_HANDLER(WM_AX_SSLSTATUS, OnSSLStatus)
		MESSAGE_HANDLER(WM_AX_GET_WEBBROWSER2_CROSS_THREAD, OnGetMarshalWebBrowser2CrossThread)
	END_MSG_MAP()


	void OnShowWindow(BOOL bShowing, int nReason);
	void OnDestroy();
	void OnClose();
	void OnSize(UINT nType, CSize size);

	LRESULT OnNavigate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGoBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGoForward(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSSLStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMarshalWebBrowser2CrossThread(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnInitSynchroBtnStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCallJSShowText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT ChangeSettingShowStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFrameSetAlarm(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFrameAddFav(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFrameChangeProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnToolsChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCouponChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFrameDelFav(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowInfoDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled); // 导出账单时弹出的提示对话框
	LRESULT OnEndInfoDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChangeFirtsPageShow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAccountSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetAllBill(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCallJSTabActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT CloseBillList(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetBill(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	HWND m_hChildFrame;
	CWebBrowserEventsManager *m_pEventsManager;
	CCustomSite *m_pCustomSite;

	CComVariant CallJScript(std::string strFunc,std::vector<std::string>& paramVec, int nType = 0);
public:
	CWebBrowserCore *m_pCore;

private:
	bool m_isGetBill;// 判断是否是记账功能下的处理
	wstring m_url;
	int m_time;
	int m_nErrCode;

};
