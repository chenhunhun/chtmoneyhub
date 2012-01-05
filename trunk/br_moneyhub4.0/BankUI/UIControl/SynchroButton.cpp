
#include "stdafx.h"
#include "SynchroButton.h"
#include "../Skin/SkinManager.h"
#include "../Synchro/SynchroDlg.h"
#include "ShowJSFrame.h"

#include "TabCtrl/TabItem.h"
#include "ChildFrm.h"
#include "MainFrame.h"

CSynchroButton::CSynchroButton(FrameStorageStruct *pFS)
: CFSMUtil(pFS), m_bKeepSelected(false), m_hParentWnd(NULL)
{
	pFS->pSynchroButton = this;
}

void CSynchroButton::CreateButton(HWND hParent)
{
	ATLASSERT(hParent);
	m_hParentWnd = hParent;
	CRect rc(0, 0, s()->SynchroButton()->GetWidth(), s()->SynchroButton()->GetHeight());

	HWND hWnd = Create(hParent, rc, NULL, WS_CHILD | WS_VISIBLE);
	ATLASSERT(hWnd);
}

void CSynchroButton::DrawBackground(HDC hDC, const RECT &rc)
{
	s()->Toolbar()->DrawRebarBackPart(hDC, rc, m_hWnd);
}

void CSynchroButton::DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus)
{
	if (!m_bKeepSelected)
		s()->SynchroButton()->DrawButton(hDC, rc, 1);
	else
		s()->SynchroButton()->DrawButton(hDC, rc, 0); // 有数据要同步
}

void CSynchroButton::KeepOnSelect(bool bSelect)
{
	m_bKeepSelected = bSelect;
	CRect rc(0, 0, s()->SynchroButton()->GetWidth(), s()->SynchroButton()->GetHeight());
	InvalidateRect(&rc, false);
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CSynchroButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ToolData tbBtn = { ToolType_Normal, true, 0, s()->SynchroButton()->GetWidth() };
	AddButtons(&tbBtn, 1);
	RefreshAfterAddButtonOrSizeChange();

	return 1;
}

LRESULT CSynchroButton::OnClick(int nIndex, POINT pt)
{
	CBankData* pBankData = CBankData::GetInstance();
	std::string  strd = CBankData::GetInstance()->m_CurUserInfo.struserid;
	if (CBankData::GetInstance()->m_CurUserInfo.struserid == MONHUB_GUEST_USERID)
		::SendMessage(GetParent() ,WM_SHOW_USER_DLG, 0, MY_TAG_LOAD_DLG);
	if (CBankData::GetInstance()->m_CurUserInfo.emUserStatus == emUserLocal)
		::SendMessage(GetParent() ,WM_SHOW_USER_DLG, 3, MY_TAG_LOAD_DLG);
	if (CBankData::GetInstance()->m_CurUserInfo.emUserStatus == emUserNet)
	{
		CSynchroDlg dlg(0);
		if(dlg.DoModal() == IDOK)
		{
			KeepOnSelect(false);
			::PostMessage(m_hParentWnd, WM_AX_INIT_SYNCHRO_BTN, 0, 0);
		}
        FS()->TabItem()->GetChildFrame()->DoNavigateRefresh();
		//if(pBankData->m_CurUserInfo.strstoken == "" || pBankData->m_CurUserInfo.strmail == "")
		{
			if (CBankData::GetInstance()->m_CurUserInfo.emUserStatus == emNotLoad)
			{
	    		CBankData::GetInstance()->CloseDB();
				::PostMessage(FS()->MainFrame()->m_IeWnd, WM_AX_LOAD_USER_QUIT, 0, 0);
				::SendMessage(GetParent() ,WM_SHOW_USER_DLG, 3, MY_TAG_LOAD_DLG);
				if (CBankData::GetInstance()->m_CurUserInfo.emUserStatus == emUserNet)
				{
					CSynchroDlg dlg(0);
					if(dlg.DoModal() == IDOK)
					{
						KeepOnSelect(false);
						::PostMessage(m_hParentWnd, WM_AX_INIT_SYNCHRO_BTN, 0, 0);
					}
					FS()->TabItem()->GetChildFrame()->DoNavigateRefresh();
				}
			}
		}
	}
	else
	{
		//WCHAR sInfo[256] = { 0 }; 
		//swprintf(sInfo, 256, L"当前网络故障或登录后同步!");
		//MessageBoxW(sInfo, L"财金汇",  MB_OK);
	}
	return 0;
}

