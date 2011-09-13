#pragma once

#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

class CHelpButton : public CTuoToolBarCtrl<CHelpButton>, public CFSMUtil
{
public:
	CHelpButton(FrameStorageStruct *pFS);

	DECLARE_WND_CLASS_NODRAW(_T("MH_RefreshButton"))

public:
	void CreateButton(HWND hParent);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

protected:
	BEGIN_MSG_MAP_EX(CHelpButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CHelpButton>)
		MSG_WM_CREATE(OnCreate);
	TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnClick(int nIndex, POINT pt);
};