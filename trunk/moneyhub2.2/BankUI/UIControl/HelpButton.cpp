
#include "stdafx.h"
#include "HelpButton.h"
#include "../Skin/SkinManager.h"
#include "../Util/Util.h"
#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"
#include "../../Utils/HardwareID/genhwid.h"
#include "../../Utils/sn/SNManager.h"
#include "../../Utils/Config/HostConfig.h"
#include "Version.h"

CHelpButton::CHelpButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS)
{
	pFS->pHelpButton = this;
}

void CHelpButton::CreateButton(HWND hParent)
{
	CRect rc(0, 0, s()->HelpButton()->GetWidth(), s()->HelpButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);

	// 	HBITMAP hBitmap = s()->RefreshButton()->GetMaskBitmap();
	// 	HRGN hRgn = CreateRegionFromBitmap(hBitmap, 0xff0000, NULL);
	// 	::SetWindowRgn(hWnd, hRgn, TRUE);
}

void CHelpButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CHelpButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	s()->HelpButton()->DrawButton(hDC, rc, uCurrentStatus);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CHelpButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->HelpButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CHelpButton::OnClick(int nIndex, POINT pt)
{
	char szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));
	//sprintf_s(szBuf, sizeof(szBuf), "v=%s", GenHWID2(false).c_str(), ProductVersion_All);
	//sprintf_s(szBuf, sizeof(szBuf), "http://www.finantech.cn:6010/help.php?code=%s", aes_encode(szBuf).c_str());
	USES_CONVERSION;
	sprintf_s(szBuf, sizeof(szBuf), "%shelp.php?MoneyhubUID=%s&v=%s&SN=%s",W2CA(CHostContainer::GetInstance()->GetHostName(kWeb).c_str()), GenHWID2().c_str(), ProductVersion_All,CSNManager::GetInstance()->GetSN().c_str());

	::CreateNewPage_0(FS()->MainFrame()->GetFrameWnd(), A2CT(szBuf), TRUE, false);

	return 0;
}
