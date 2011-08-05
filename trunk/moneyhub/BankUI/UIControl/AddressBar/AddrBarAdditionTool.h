#pragma once
#include "../../Util/ThreadStorageManager.h"
#include "../BaseClass/TuoToolbar.h"
#include "../BaseClass/MouseObjChild.h"


class CAddrAdditionTool
	: public CTuoToolBarCtrl<CAddrAdditionTool>,
	  public CMouseObjChild<CAddrAdditionTool>,
	  public CFSMUtil
{

public:

	CAddrAdditionTool(FrameStorageStruct *pFS);

	void CreateAddressBarAdditionTool(HWND hWndParent);

	void UpdateResourceStatus(int iResType, int iCount);

	LRESULT OnDropDown(int nIndex, POINT pt);

	void Refresh();

	// override

	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

private:

	// message

	BEGIN_MSG_MAP_EX(CAddrAdditionTool)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CAddrAdditionTool>)
		CHAIN_MSG_MAP(CMouseObjChild<CAddrAdditionTool>)

		MSG_WM_CREATE(OnCreate)

		NOTIFY_CODE_HANDLER_EX(TTN_GETDISPINFO, OnGetDispInfo)
		TUO_COMMAND_CODE_HANDLER_EX(NM_DROPDOWN, OnDropDown)
	END_MSG_MAP()

	// message handler

	int OnCreate(LPCREATESTRUCT lpCreateStruct);

	LRESULT OnGetDispInfo(LPNMHDR pnmh);

	// members

	int m_iResCount[RESOURCE_TYPE_COUNT];

public:

	DECLARE_WND_CLASS_NODRAW(_T("MH_TuotuoAddrAdditionalToolBar"))
};
