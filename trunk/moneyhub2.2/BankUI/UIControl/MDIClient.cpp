#include "stdafx.h"
#include "Util.h"
#include "../../Security/BankLoader/BankLoader.h"
#include "../../Utils/ListManager/ListManager.h"
#include "../../Utils/ListManager/URLList.h"
#include "TabCtrl/TabItem.h"
#include "CategoryCtrl/CategoryItem.h"
#include "MainFrame.h"
#include "ChildFrm.h"
#include "MDIClient.h"
#include "..//HookKeyboard.h"
#include "..\Util\SecurityCheck.h"
#include <TlHelp32.h>

bool CMDIClient::m_bSafe = true;
CMDIClient::CMDIClient(CTuotuoTabCtrl &TabCtrl, CTuotuoCategoryCtrl &cateCtrl, FrameStorageStruct *pFS) : m_TabCtrl(TabCtrl), m_CateCtrl(cateCtrl), CFSMUtil(pFS)
{
	FS()->pMDIClient = this;
}


HWND CMDIClient::CreateTabMDIClient(HWND hParent)
{
	HWND hWnd = Create(hParent, CRect(s()->MainFrame()->GetBorderWidth(), 0, 1, 1), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	ATLASSERT(hWnd && "Create CMDIClient failed.");

	m_TabCtrl.m_wndMDIClient = m_hWnd;

	return m_hWnd;
}

void CMDIClient::ClosePage(CTabItem *pItem)
{
	ATLASSERT(pItem);
	if (pItem->m_pCategory->GetWebData() == START_PAGE_WEBSITE_DATA)
		return;

	if (m_TabCtrl.GetTabItems().size() == 1)
	{
		// 只剩一个，则关闭category
		CloseCategory(pItem->m_pCategory);
		return;
	}

	CTabItem *pNewItem = FS()->TabItem();
	if (pItem == pNewItem)
	{
		pNewItem = m_TabCtrl.GetLeftOrRight(pItem, true, false);
		ActivePage(pNewItem);
	}
	pItem->m_pCategory->m_TabItems.erase(pItem->m_pCategory->m_TabItems.begin() + pItem->GetIndex());
	pItem->m_pCategory->Redraw();
	m_TabCtrl.DeleteItem(pItem->GetIndex());
	pItem->GetChildFrame()->OptionalDestroy(0);
}

void CMDIClient::CloseCategory(CCategoryItem *pCateItem)
{
	if (pCateItem->GetWebData() == START_PAGE_WEBSITE_DATA)
		return;
	
	if (pCateItem->GetWebData()->IsNoClose())
		return;

	m_CateCtrl.DeleteItem(pCateItem->GetIndex());
	CCategoryItem *pNewCate = m_CateCtrl.GetLastActiveItem ();
	ActiveCategory(pNewCate);

	for (size_t i = 0; i < pCateItem->m_TabItems.size(); i++)
		pCateItem->m_TabItems[i]->GetChildFrame()->OptionalDestroy(0);
	delete pCateItem;
}


void CMDIClient::ActiveCategory(CCategoryItem *pCateItem)
{
	if( !m_bSafe )
		return ;


	if (m_CateCtrl.GetCurrentSelection() != pCateItem)
	{
		m_TabCtrl.DeleteAllItems();
		for (size_t i = 0; i < pCateItem->m_TabItems.size(); i++)
			m_TabCtrl.InsertItem(i, pCateItem->m_TabItems[i]);
		m_CateCtrl.SelectItem(pCateItem);
		if (pCateItem->m_pSelectedItem)
			ActivePage(pCateItem->m_pSelectedItem);
		else if (!pCateItem->m_TabItems.empty())
			ActivePage(pCateItem->m_TabItems[0]);

		FS()->MainFrame()->RecalcClientSize(-1, -1);
	}
}
/**
*关闭内核
*/

bool CMDIClient::getPriviledge()
{
	HANDLE hProcessToken = NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hProcessToken)) 
	{ 
		return FALSE; 
	}

	TOKEN_PRIVILEGES tp={0};
	LUID luid={0};  
	if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid))  
	{ 
		return FALSE; 
	}  
	tp.PrivilegeCount = 1;  
	tp.Privileges[0].Luid = luid;

	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  

	// Enable the privilege
	AdjustTokenPrivileges(hProcessToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);  

	if(GetLastError() != ERROR_SUCCESS)  
	{
		return FALSE;  
	}
	return TRUE;

}

bool CMDIClient::killKernel(bool bKill)
{
	bool bReturn = false;
	wchar_t p[260] = {L"moneyhub.exe"};
	this->getPriviledge();

	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hand != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W pew = {sizeof(PROCESSENTRY32W)};
		bool bHaveOther=false;
		Process32FirstW(hand,&pew );	

		do{
			_wcslwr_s(pew.szExeFile, 260);

			if(!wcscmp(pew.szExeFile,p) && GetCurrentProcessId()!=pew.th32ProcessID)
			{
				HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pew.th32ProcessID);
				if( h )
				{
					if( TerminateProcess(h, 1) )
						bReturn = true;

					CloseHandle(h);
				}
			}

		}while(Process32NextW(hand,&pew));


		CloseHandle(hand);	
	}

	return bReturn ;
}

/**
* check integrity
*/
HANDLE g_threadHandleCheckIntegrity;
unsigned _stdcall _threadForCheckIntegrity( void * p)
{
	CMDIClient * pMC = (CMDIClient*) p;

	_SecuCheck.SetEventsFunc(NULL, NULL);
	if( !_SecuCheck.CheckSelfDataFiles(true, true) )
	{
// 		HWND hWnd = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
// 		if(hWnd)
// 			SendMessage(hWnd, WM_MYDANGEROUS, 0, 0);
		pMC->m_bSafe = false;
		pMC->killKernel();
		::MessageBoxW(NULL, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描", L"财金汇安全提示", MB_OK);
		exit(-1);
	}

	return 0;
}

void CMDIClient::ActivePage(CTabItem *pItem)
{
	if(!m_bSafe)
	{
		return ;
	}

	CTabItem *pSelectedItem = FS()->pCurrentItem;
	if (pSelectedItem == pItem)
		return;
	FS()->pCurrentItem = pItem;

	if (m_CateCtrl.GetCurrentSelection() != pItem->m_pCategory)
	{
		m_TabCtrl.DeleteAllItems();
		for (size_t i = 0; i < pItem->m_pCategory->m_TabItems.size(); i++)
			m_TabCtrl.InsertItem(i, pItem->m_pCategory->m_TabItems[i]);
		m_CateCtrl.SelectItem(pItem->m_pCategory);
		//m_CateCtrl.Invalidate();
		FS()->MainFrame()->RecalcClientSize(-1, -1);
	}

	pItem->m_pCategory->m_pSelectedItem = pItem;

	m_TabCtrl.SelectItem(pItem);

	RECT rcClient;
	GetClientRect(&rcClient);

	if (pSelectedItem)
	{
		pItem->GetChildFrame()->SetWindowPos(NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		pItem->GetChildFrame()->ShowWindow(SW_SHOW);
		pSelectedItem->GetChildFrame()->ShowWindow(SW_HIDE);
	}
	else
	{
		pItem->GetChildFrame()->SetWindowPos(NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		pItem->GetChildFrame()->ShowWindow(SW_SHOW);
	}

	//FS()->MainFrame()->RecalcClientSize(-1, -1);

	SyncCurrentPageInfo();

	// 刷新页面 fanzhenxing add
	// 刷新优惠券和提醒、首页3个目录，其他不刷新
	std::tstring toolstr = _T("Html\\ToolsPage"); 
	std::tstring Mainstr = _T("Html/StartPage"); 

	std::tstring  ms = pItem->GetURLText();

	CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_COMMON_PROCESS, CRecordProgram::GetInstance()->GetRecordInfo(L"用户点击如下链接:%s", ms.c_str ()));

	if((ms.find(toolstr) != std::tstring::npos) || (ms.find(Mainstr) != std::tstring::npos))
	{

		//TerminateThread(g_threadHandleCheckIntegrity,-1);
		//CloseHandle(g_threadHandleCheckIntegrity);
		//g_threadHandleCheckIntegrity = (HANDLE)_beginthreadex(0, 0, _threadForCheckIntegrity, this, 0, 0) ;
	}

	wcscpy_s(g_noHookfilterUrl,3000,pItem->GetURLText());
	*(g_noHookfilterUrl + 2999) = L'\0';
}

void CMDIClient::SyncCurrentPageInfo(bool bUpdateStatusBarOnly)
{
	CTabItem *pItem = FS()->TabItem();
	if (pItem)
	{
		CWindow wndParent(GetParent());
		if (!bUpdateStatusBarOnly)
		{
			FS()->MainFrame()->UpdateTitle();
			FS()->MainFrame()->UpdateSSLState();
			//FS()->ToolBar()->SetButtonState(pItem->GetMainToolbarBackStatus(), pItem->GetMainToolbarForwardStatus());
			FS()->BackButton()->SetButtonState(pItem->GetMainToolbarBackStatus() != 0);
			FS()->ForwardButton()->SetButtonState(pItem->GetMainToolbarForwardStatus() != 0);


			/*
			FS()->AddrBar()->RefreshCoreType(pItem->GetCoreType());
			FS()->AddrCombo()->UpdateIcon(pItem->GetIcon());
			FS()->AddrCombo()->UpdateURLText(pItem->GetURLText(), true);
			if (!pItem->GetChildFrame()->NeedAutoSetFocusToPage() && ::GetForegroundWindow() == FS()->MainFrame()->m_hWnd)		// 如果不需要把焦点放到页面区，则要把地址栏全选
				FS()->AddrCombo()->DoSelectAddressComboBar();
			for (int i = 0; i < RESOURCE_TYPE_COUNT; i++)
				FS()->AddrCombo()->UpdateResourceStatus(i, pItem->GetResourceCount(i));

			wndParent.SendMessage(WM_MAIN_SET_TITLE_AND_URL, 0, (LPARAM)pItem->GetText());
			FS()->ToolBar()->SetButtonState(pItem->GetMainToolbarBackStatus(), pItem->GetMainToolbarForwardStatus());
			FS()->SearchBar()->SetSearchBarData(pItem->GetSearchBarString(), pItem->GetSearchBarHiLight(), pItem->GetSearchBarEngine());
			FS()->Status()->SetPageProgress(pItem->GetProgress());
			FS()->Status()->SetStatusIcon(pItem->GetStatusIcon());
			FS()->StatusTool()->UpdateZoom(pItem->GetZoom());
			FS()->StatusTool()->UpdateAdBlockStatus(pItem->GetBlockedPopups().size());
			FS()->StatusTool()->UpdateAdBlockStatus(pItem->GetBlockedCount());
			FS()->FindBar()->SetFindData(pItem->GetFindString(), pItem->GetFindHiLight(), pItem->GetFindCaseSensitive());
			FS()->FindBar()->SetFindPosition(pItem->GetCurrentFind(), pItem->GetTotalFind());
		*/}
		
		FS()->Status()->SetPageProgress(pItem->GetProgress());
		FS()->Status()->SetStatusText(pItem->GetStatusText(), true);
	}
}

//////////////////////////////////////////////////////////////////////////
// message handler

void CMDIClient::OnDestroy()
{
	FS()->pCurrentItem = NULL;

	for (size_t i = 0; i < m_CateCtrl.GetTabItems().size(); i++)
		for (size_t j = 0; j < m_CateCtrl.GetTabItems()[i]->m_TabItems.size(); j++)
			m_CateCtrl.GetTabItems()[i]->m_TabItems[j]->GetChildFrame()->OptionalDestroy(0);
}

void CMDIClient::OnSize(UINT nType, CSize size)
{
	if (FS()->TabItem())
	{
		FS()->TabItem()->GetChildFrame()->SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);
		FS()->TabItem()->GetChildFrame()->PostMessage(WM_SIZE, 0, MAKELPARAM(size.cx, size.cy));
	}
}

// 创建一个新的页面标签页
LRESULT CMDIClient::OnCreateNewWebPage(UINT /* uMsg */, WPARAM wParam, LPARAM lParam)
{
	if( !m_bSafe )
		return NULL;

#ifndef SINGLE_PROCESS
	CBankLoader::GetInstance()->Safeguard();
#endif


	CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"创建新的页面");

	CreateNewWebPageData *pNewPage = (CreateNewWebPageData*)lParam;

	std::tstring strURL = _T("about:blank");
	std::tstring strAdditionalInfo;
	if (IS_VALID_URL(pNewPage->lpszURL))
	{
		if (pNewPage->dwSize > 0)			// >0表示url的指针只是一个flag，不是真实地址，需要转换
			strURL = (LPCTSTR)((BYTE*)pNewPage + sizeof(CreateNewWebPageData));
		else
			strURL = pNewPage->lpszURL;
	}
	else if (pNewPage->lpszURL == URL_NO_NAVIGATE)		// 点击链接打开新页面，此时不去navigate url，只是显示出来
	{
		if (pNewPage->dwSize > 0)
			strURL = (LPCTSTR)((BYTE*)pNewPage + sizeof(CreateNewWebPageData));
		else
			strURL = _T(" ");
	}
	else if (pNewPage->lpszURL == URL_WITH_REFERER)
		strURL = (LPCTSTR)((BYTE*)pNewPage + sizeof(CreateNewWebPageData));
	else if (pNewPage->lpszURL == URL_WITH_POSTDATA)
	{
		strURL = (LPCTSTR)((BYTE*)pNewPage + sizeof(CreateNewWebPageData));
		size_t nPos = strURL.find('\n');
		if (nPos != std::wstring::npos)
		{
			strAdditionalInfo = strURL.substr(nPos + 1);
			strURL = strURL.substr(0, nPos);
		}
	}

	//std::wstring strDomain = ExtractTopDomain(strURL.c_str());

#ifndef SINGLE_PROCESS
	CListManager::_()->CheckHMAC();
#endif

	const CWebsiteData *pData;
	if (CListManager::IsHomePage(strURL.c_str()))
		pData = START_PAGE_WEBSITE_DATA;
	else
		pData = CURLList::GetInstance()->GetData(NULL, strURL.c_str());

	if (pData == NULL)
	{
		if (_tcsnicmp(strURL.c_str(), _T("http"), 4) == 0)
		{
			//::ShellExecute(NULL, _T("open"), strURL.c_str(), strURL.c_str(), NULL, SW_SHOW);
			CListManager::NavigateWebPage(strURL.c_str());
			
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"NavigateWebPage创建新页面");
			CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_COMMON_PROCESS, strURL.c_str());
			return 0;
		}
	}

	std::tstring strChildFrameUrl;
	CTabItem *pCreateFromItem = NULL;
	if (::IsWindow(pNewPage->hCreateFromChildFrame))
	{
		CChildFrame *pChildFrame = (CChildFrame*)::GetWindowLongPtr(pNewPage->hCreateFromChildFrame, GWLP_USERDATA);
		//ATLASSERT(!::IsBadReadPtr(pChildFrame, sizeof(CChildFrame)));
		if (pChildFrame && !::IsBadReadPtr(pChildFrame, sizeof(CChildFrame)))
		{
			pCreateFromItem = pChildFrame->GetItem();
			strChildFrameUrl = pCreateFromItem->GetURLText();
		}
	}

	HWND hReturnChildFrame = NULL;

	CTabItem *pItem = new CTabItem(pNewPage->bNoClose);
	CCategoryItem *pCate = m_CateCtrl.FindOrCreateItem(strURL == _T("about:blank") ? strChildFrameUrl.c_str() : strURL.c_str(), pCreateFromItem);
	pCate->m_TabItems.push_back(pItem);
	pItem->m_pCategory = pCate;

	DWORD dwProcessID = 0;
	if (pCreateFromItem)
		::GetWindowThreadProcessId(pCreateFromItem->GetAxControl(), &dwProcessID);
	CChildFrame *pChild = new CChildFrame(FS(), m_hWnd, strURL.c_str(), pItem, pNewPage->lpszURL == URL_NO_NAVIGATE, false, dwProcessID, strChildFrameUrl);

	if (m_CateCtrl.GetCurrentSelection() == pCate)
		m_TabCtrl.InsertItem(m_TabCtrl.GetTabItems().size(), pItem);

	if (pNewPage->bShowImmediately || m_TabCtrl.GetCurrentSelection() == NULL)
		ActivePage(pItem);

	hReturnChildFrame = pChild->m_hWnd;

	return (LRESULT)hReturnChildFrame;
}

LRESULT CMDIClient::OnGetExistWebPage(UINT /* uMsg */, WPARAM wParam, LPARAM lParam)
{
	if( !m_bSafe )
		return NULL;

	CreateNewWebPageData *pNewPage = (CreateNewWebPageData*)lParam;
	
	if (pNewPage->lpszURL != URL_NO_NAVIGATE || pNewPage->dwSize == 0)
		return NULL;

	std::tstring strURL = (LPCTSTR)((BYTE*)pNewPage + sizeof(CreateNewWebPageData));

	HWND hReturnChildFrame = NULL;

	CCategoryItem *pCate = m_CateCtrl.FindExistItem(strURL.c_str());
	if (pCate == NULL)
		return NULL;

	for (size_t i = 0; i < pCate->m_TabItems.size(); ++i)
	{
		CTabItem* pTabItem = pCate->m_TabItems[i];
		if (_tcsicmp(pTabItem->GetURLText(), strURL.c_str()) == 0)
		{
			ActivePage(pTabItem);
			return (LRESULT)pTabItem->GetChildFrame()->m_hWnd;
		}
	}

	return NULL;
}