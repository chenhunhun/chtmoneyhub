#pragma once
#include "TabCtrl/TabCtrl.h"


class CMDIClient : public CWindowImpl<CMDIClient>, public CFSMUtil
{

public:

	CMDIClient(CTuotuoTabCtrl &TabCtrl, CTuotuoCategoryCtrl &cateCtrl, FrameStorageStruct *pFS);

	HWND CreateTabMDIClient(HWND hParent);

	void ClosePage(CTabItem *pItem);
	void CloseCategory(CCategoryItem *pItem);
	void HideCategory(CCategoryItem *pCateItem);
	void ActiveCategory(CCategoryItem *pCateItem);
	void ActivePage(CTabItem *pItem);
	void SyncCurrentPageInfo(bool bUpdateStatusBarOnly = false);

private:

	// message

	BEGIN_MSG_MAP_EX(CMDIClient)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)

		MESSAGE_HANDLER_EX(WM_GLOBAL_CREATE_NEW_WEB_PAGE, OnCreateNewWebPage)
		MESSAGE_HANDLER_EX(WM_GLOBAL_GET_EXIST_WEB_PAGE, OnGetExistWebPage)
	END_MSG_MAP()

	// message handler

	void OnDestroy();
	void OnSize(UINT nType, CSize size);

	LRESULT OnCreateNewWebPage(UINT /* uMsg */, WPARAM wParam, LPARAM /* lParam */);
	LRESULT OnGetExistWebPage(UINT /* uMsg */, WPARAM wParam, LPARAM /* lParam */);

	// member

	CTuotuoTabCtrl &m_TabCtrl;
	CTuotuoCategoryCtrl &m_CateCtrl;

public:
	bool killKernel(bool bKill = true);
	static bool m_bSafe;
private:
	bool getPriviledge();
	
public:

	DECLARE_WND_CLASS(_T("MH_MDIClient"))
};
