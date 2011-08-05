#pragma once

#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

class CLogoButton : public CTuoToolBarCtrl<CLogoButton>, public CFSMUtil
{
public:
	CLogoButton(FrameStorageStruct *pFS);

	DECLARE_WND_CLASS_NODRAW(_T("MH_LogoButton"))

public:
	void CreateLogoButton(HWND hParent);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

protected:
	BEGIN_MSG_MAP_EX(CLogoButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CLogoButton>)
		MSG_WM_CREATE(OnCreate);
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
};