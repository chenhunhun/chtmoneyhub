/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  AxUI.h
*      说明：  核心线程进程管理类
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.11.17	融信恒通
*-----------------------------------------------------------*
*/
#include "BankData\\BkInfoDownload.h"
#include "Message.h"
#pragma once


class CAxUI : public CWindowImpl<CAxUI>
{

public:
	#define _SETUP_FINISH		200
	#define _DOWNLOAD_FINISH	100
	
	CAxUI();
	// 启动关闭步骤的函数
	void StartQuittingSeTask();

	DECLARE_WND_CLASS(_T("MH_TuoAxUI"))

	BEGIN_MSG_MAP_EX(CAxUI)
 		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER_EX(WM_MAINFRAME_CLOSE, OnExit)

		MESSAGE_HANDLER_EX(WM_MULTI_PROCESS_CREATE_NEW_PAGE, OnCreateNewPage)
		MESSAGE_HANDLER_EX(WM_CANCEL_ADDFAV, OnCancelAddFav)
		MESSAGE_HANDLER_EX(WM_AX_GET_ALL_BILL, OnGetAllBill)
		MESSAGE_HANDLER_EX(WM_FINISH_GET_BILL, OnSendToBillPage)
		MESSAGE_HANDLER_EX(WM_TIMER, OnTimer)
		HANDLE_TUO_COPYDATA()
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// 创建新的页面的函数
	LRESULT OnCreateNewPage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCancelAddFav(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetAllBill(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSendToBillPage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnExit(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void CheckDownloadFile(void);

protected:
	// CBkInfoDownloadManager* m_pBkDownloadManager;
	HWND m_hTag;
	char *m_pbilldata;
	static DWORD WINAPI _threadInit(LPVOID lp);
};
