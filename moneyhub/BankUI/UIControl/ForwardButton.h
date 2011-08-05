#pragma once

#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

class CForwardButton : public CTuoToolBarCtrl<CForwardButton>, public CFSMUtil
{
public:
	CForwardButton(FrameStorageStruct *pFS);

	DECLARE_WND_CLASS_NODRAW(_T("MH_ForwardButton"))

public:
	void CreateButton(HWND hParent);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

	void SetButtonState(bool bEnabled);

protected:
	BEGIN_MSG_MAP_EX(CForwardButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CForwardButton>)
		MSG_WM_CREATE(OnCreate);
		TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnClick(int nIndex, POINT pt);
};