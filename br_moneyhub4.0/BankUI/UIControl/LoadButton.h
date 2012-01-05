#pragma once

#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

class CLoadButton : public CTuoToolBarCtrl<CLoadButton>, public CFSMUtil
{
public:
	CLoadButton(FrameStorageStruct *pFS);

	DECLARE_WND_CLASS_NODRAW(_T("MH_LoadButton"))

public:
	void CreateButton(HWND hParent);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);
	void EndShowDialog(void);

protected:
	BEGIN_MSG_MAP_EX(CLoadButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CLoadButton>)
		MSG_WM_CREATE(OnCreate);
	TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnClick(int nIndex, POINT pt);
private:
	HWND m_hParentWnd;
//	CWindow*		m_pShowDlg;
};