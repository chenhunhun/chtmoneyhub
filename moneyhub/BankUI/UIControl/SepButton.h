#pragma once

#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

class CSepButton : public CTuoToolBarCtrl<CSepButton>, public CFSMUtil
{
public:
	CSepButton(FrameStorageStruct *pFS);

	DECLARE_WND_CLASS_NODRAW(_T("MH_SepButton"))

public:
	void CreateLogoButton(HWND hParent);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

protected:
	BEGIN_MSG_MAP_EX(CSepButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CSepButton>)
		MSG_WM_CREATE(OnCreate);
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
};