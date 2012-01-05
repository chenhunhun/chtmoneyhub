
#pragma once

#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"
#include "../Skin/TuoImage.h"

class CSepButton : public CTuoToolBarCtrl<CSepButton>, public CFSMUtil
{
public:
	CSepButton(FrameStorageStruct *pFS);
	~CSepButton();

	DECLARE_WND_CLASS_NODRAW(_T("MH_SepButton"))

public:
	void CreateButton(HWND hParent, LPCTSTR lpName, UINT nBtnClickMessage = 0);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);
	int GetButtonWidth() const;
	int GetButtonHeight() const;

protected:
	BEGIN_MSG_MAP_EX(CSepButton)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CSepButton>)
		MSG_WM_CREATE(OnCreate);
	//TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//LRESULT OnClick(int nIndex, POINT pt);
	LRESULT OnMouseMove(int nIndex, POINT pt);
private:
	CTuoImage m_bitmap;
	HWND m_hParentWnd;
	UINT m_nMessage;

};