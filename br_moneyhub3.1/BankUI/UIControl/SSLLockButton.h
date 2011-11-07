#pragma once

#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

class CSSLLockButton : public CTuoToolBarCtrl<CSSLLockButton>, public CFSMUtil
{
public:
	CSSLLockButton(FrameStorageStruct *pFS);

	DECLARE_WND_CLASS_NODRAW(_T("MH_SSLLockButton"))

public:
	void CreateLockButton(HWND hParent);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

protected:
	BEGIN_MSG_MAP_EX(CSSLLockButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CSSLLockButton>)
		
		MSG_WM_CREATE(OnCreate);
		TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnClick(int nIndex, POINT pt);
};
