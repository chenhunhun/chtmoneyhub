#pragma once

#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

class CSynchroButton : public CTuoToolBarCtrl<CSynchroButton>, public CFSMUtil
{
public:
	CSynchroButton(FrameStorageStruct *pFS);

	DECLARE_WND_CLASS_NODRAW(_T("MH_SynchroButton"))

public:
	void CreateButton(HWND hParent);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);
	void KeepOnSelect(bool bSelect);

protected:
	BEGIN_MSG_MAP_EX(CLoginButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CSynchroButton>)
		MSG_WM_CREATE(OnCreate);
	TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnClick(int nIndex, POINT pt);
private:
	bool		m_bKeepSelected;
	HWND		m_hParentWnd;
};