#include "stdafx.h"
#include "SettingButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"
#include "../../Utils/HardwareID/genhwid.h"
#include "../../Utils/sn/SNManager.h"
#include "../../Utils/Config/HostConfig.h"
#include "Version.h"
#include "SettingDlg.h"
//#include "../../USBControl/USBControl.h"

CSettingButton::CSettingButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pSettingButton = this;
}

void CSettingButton::CreateButton(HWND hParent)
{
	CRect rc(0, 0, s()->SettingButton()->GetWidth(), s()->SettingButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);

}

void CSettingButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CSettingButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->SettingButton()->DrawButton(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CSettingButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->SettingButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CSettingButton::OnClick(int nIndex, POINT pt)
{
	
	CSettingDlg dlg;
	dlg.DoModal();
	//CUSBControl* pCtrl = CUSBControl::GetInstance();
	////pCtrl->InitUSBInfo ();
	//pCtrl->BeginUSBControl ();
	

	return 0;
}
