/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  AxControl.cpp
 *      说明：  控件控制
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#include "stdafx.h"
#include "ConvertBase.h"
#include "WebBrowserCore.h"
#include "WebBrowserEventsManager.h"
#include "CustomSite.h"
#include "AxControl.h"
#include "../../Utils/ListManager/ListManager.h"
#include "ExternalDispatchImpl.h"
#include "JS/JSParam.h"
#include "GetBill/BillUrlManager.h"
#include "../BankUI/UIControl/CoolMessageBox.h"
#include "../BankUI/UIControl/MonthSelectDlg.h"
#include "../BankUI/UIControl/AccountSelectDlg.h"

CAxControl::CAxControl(HWND hChildFrame) : m_hChildFrame(hChildFrame), m_isGetBill(false),m_time(0)
{
	// 事件回调对象
	m_pEventsManager = new CWebBrowserEventsManager(this);
	
	// Custom Site 对象
	m_pCustomSite = new CCustomSite(this);

	// Web Browser 对象
	m_pCore = new CWebBrowserCore();
}

CAxControl::~CAxControl()
{
	if (m_pEventsManager)
	{
		delete m_pEventsManager;
		m_pEventsManager = NULL;
	}

	if (m_pCore)
	{
		delete m_pCore;
		m_pCore = NULL;
	}

	if (m_pCustomSite)
	{
		delete m_pCustomSite;
		m_pCustomSite = NULL;
	}
}
void CAxControl::OnShowWindow(BOOL bShowing, int nReason)
{
	if(bShowing == TRUE)
	{
		if(m_time == 0)
			if(m_url != L"")
			{
				CComVariant var;
				m_time = 1;
				m_pCore->m_pWebBrowser2->Navigate((BSTR)m_url.c_str(), &var, &var, &var, &var);
			}

		LONG style = ::GetWindowLongW(m_hWnd, GWL_STYLE);
		if((style & WS_VISIBLE) ==  WS_VISIBLE)
		{
			HWND TChild, MChild, BChild;
			TChild = FindWindowExW(m_hWnd , NULL, L"Shell Embedding", NULL);
			if(TChild != NULL)
			{
				MChild = FindWindowExW(TChild, NULL, L"Shell DocObject View", NULL);
				if(MChild != NULL)
				{
					BChild = FindWindowExW(MChild, NULL, L"Internet Explorer_Server", NULL);
					if(BChild != NULL)
					{
						::SetFocus(BChild);
					}
				}
			}
		}
	}
}

LRESULT CAxControl::OnGetBill(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	DWORD pid = ::GetCurrentThreadId();

	BillData *pData = (BillData *)lParam;
	if(pData != NULL)
	{
		CBillUrlManager::GetInstance()->InsertBillTid(pid, pData);
	}

	if(m_pEventsManager)
	{		
		m_pEventsManager->m_pBillData = pData;
		m_pEventsManager->m_isGetBill = true;
	}
	m_isGetBill = true;


	return 0;
}
void CAxControl::OnFinalMessage(HWND)
{
	if(m_isGetBill)
	{
		DWORD pid = ::GetCurrentThreadId();
		CBillUrlManager::GetInstance()->DeleteBillTid(pid);
		CBillUrlManager::GetInstance()->FreeDll();
	}

	delete this;
	PostQuitMessage(0);
}

LRESULT CAxControl::CreateIEServer()
{
	__try
	{
		m_pCore->CreateIEServer(m_hWnd, m_pCustomSite);

		RECT rect;
		GetClientRect(&rect);
		m_pCore->ChangeSize(0, 0, rect.right, rect.bottom);

		if (m_pEventsManager)
			m_pEventsManager->AdviseBrowserEvents(m_pCore->m_pWebBrowser2) ;

		//if (m_pEventsManager)
			//m_pEventsManager->SetCustomSite(m_pCustomSite);

		m_pCore->m_pWebBrowser2->put_RegisterAsDropTarget(VARIANT_FALSE);
		m_pCore->m_pWebBrowser2->put_RegisterAsBrowser(VARIANT_TRUE);

	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CAxControl::OnDestroy()
{
	if(m_pEventsManager)
		m_pEventsManager->UnadviseBrowserEvents();

	if(m_pCore)
		m_pCore->Release();
}

void CAxControl::OnClose()
{
	if(m_pCore)
		m_pCore->m_pWebBrowser2->Stop();
	DestroyWindow();
}

void CAxControl::OnSize(UINT nType, CSize size)
{
	m_pCore->ChangeSize(0, 0, size.cx, size.cy);
}

LRESULT CAxControl::OnGetMarshalWebBrowser2CrossThread(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int isGetBill = (int)wParam;
	if(isGetBill >= 199 && isGetBill <= 300)
	{
		m_isGetBill = true;
		if(m_pEventsManager)
		{
			m_pEventsManager->m_isGetBill = true;
			m_pEventsManager->m_step = isGetBill % 200;
			m_pEventsManager->m_pBillData = (BillData *)lParam;
		}
	}

	try
	{
		// 特别注意不要在单线程的情况下调用这个函数
		IStream* pStream = NULL ;
		HRESULT hr = CoMarshalInterThreadInterfaceInStream(IID_IWebBrowser2, m_pCore->m_pWebBrowser2, &pStream) ;
		if (FAILED(hr) || !pStream)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, L"CoMarshalInterThreadInterfaceInStream error");
			return NULL ;
		}

		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_STATE, L"OnGetMarshalWebBrowser2CrossThread成功");
		return (LRESULT)pStream ;
	}
	catch (...)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, L"OnGetMarshalWebBrowser2CrossThread EXCEPTION");
		ATLASSERT(FALSE) ;
		return NULL ;
	}

	return 0;
}
LRESULT CAxControl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam == MH_STARTDELAYEVENT)
	{
		KillTimer(MH_STARTDELAYEVENT);
		if(m_time == 0)
			if(m_url != L"")
			{
				CComVariant var;
				m_time = 1;
				m_pCore->m_pWebBrowser2->Navigate((BSTR)m_url.c_str(), &var, &var, &var, &var);
			}
	}
	return 0;

}
LRESULT CAxControl::OnNavigate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD tid = ::GetCurrentThreadId();

	CComVariant var;
	if(m_pCore != NULL)
	{
		LPCTSTR strUrl = (LPCTSTR)lParam;;
		int tag = CListManager::GetDefaultPageIndex(strUrl);
		if(tag >= 0 && tag <= 2)
			CExternalDispatchImpl::m_hFrame[tag] = m_hWnd;

		if(tag > 1 && tag < 6)
			if(m_time == 0)
			{
				switch(tag)
				{
				case 2:
					{
						::SendMessage(m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_TAB_TEXT, (LPARAM)L"我的账本");
						//SetTimer(MH_STARTDELAYEVENT, 5 * 1000 ,NULL);//设置5s之后加载管理页
					}
					break;
				case 3:
					::SendMessage(m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_TAB_TEXT, (LPARAM)L"统计报表");
					break;
				case 4:
					{
						::SendMessage(m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_TAB_TEXT, (LPARAM)L"管理");
						SetTimer(MH_STARTDELAYEVENT, 3 * 1000 ,NULL);//设置3s之后加载管理页
					}
					break;
				case 5:
					::SendMessage(m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_TAB_TEXT, (LPARAM)L"精选理财产品");
					break;
				default:
					break;
				}
				m_url = strUrl;
				return 0;
			}

		m_pCore->m_pWebBrowser2->Navigate((BSTR)(LPCTSTR)lParam, &var, &var, &var, &var);
	//记录3个首页的位置(BSTR)(LPCTSTR)lParam
		

	}

	return 0;
}

LRESULT CAxControl::OnGoBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD dwTime = GetTickCount() - m_pEventsManager->m_dwAxUserActionTick;
	if( dwTime < 300)
		return 0;

	m_pEventsManager->m_dwAxUserActionTick = GetTickCount() ;

	try
	{
		m_pCore->m_pWebBrowser2->GoBack();
	}
	catch(...)
	{
		return 0;
	}
	
	return 0 ;
}

LRESULT CAxControl::OnGoForward(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pEventsManager->m_dwAxUserActionTick = GetTickCount() ;
	m_pCore->m_pWebBrowser2->GoForward() ;
	
	return 0 ;
}

LRESULT CAxControl::OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CComVariant var;
	VariantInit(&var);
	var.vt = VT_I4 ;
	var.lVal = wParam;
	if(m_pEventsManager->m_error == false)
	{
		var.lVal = wParam;	
		m_pEventsManager->SetPercentage(0);
		m_pCore->m_pWebBrowser2->Refresh2(&var);
	}
	else
	{
		//如果发生了错误，那么刷新采用直接重新浏览的方式运行，否则不会刷新
		BSTR bstrUrl;
		m_pCore->m_pWebBrowser2->get_LocationURL(&bstrUrl);
		if (SysStringLen(bstrUrl) != 0)
		{
			CComVariant var;
			m_pCore->m_pWebBrowser2->Navigate(bstrUrl, &var, &var, &var, &var);
			SysFreeString(bstrUrl);
		}

	}	
	return 0;
}

LRESULT CAxControl::OnSSLStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	#define SHDVID_SSLSTATUS 33

	IOleCommandTarget *pct;
	if (SUCCEEDED(m_pCore->m_pWebBrowser2->QueryInterface(IID_IOleCommandTarget, (void **)&pct)))
	{
		pct->Exec(&CGID_ShellDocView, SHDVID_SSLSTATUS, 0, NULL, NULL);
		pct->Release();
	}

	return 0;
}

LRESULT CAxControl::OnFrameSetAlarm(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::vector<std::string> paramVec;
	CallJScript("setAlarm", paramVec);
	return 0;
}

LRESULT CAxControl::OnFrameAddFav(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG tlfid = lParam;
	char tfid[6] = {0};

	memcpy(tfid, (void*)&tlfid, 4);
	std::string appId(tfid);
	std::vector<std::string> paramVec;
	paramVec.push_back(appId);
	CallJScript("favAction", paramVec);
	return 0;
}
// 
LRESULT CAxControl::OnFrameChangeProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDOWNLOADSTATUS dStatus = (LPDOWNLOADSTATUS)wParam; 
	if(dStatus == NULL)
		return 0;

	std::vector<std::string> paramVec;
	paramVec.push_back(dStatus->logo);
	paramVec.push_back(dStatus->status);
	paramVec.push_back(dStatus->progress);
	paramVec.push_back(dStatus->appId);

	CallJScript("setDownloadStatus", paramVec);

	delete dStatus;
	dStatus = NULL;
	return 0;
}
//  取消收藏
LRESULT CAxControl::OnFrameDelFav(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG tlfid = lParam;
	char tfid[6] = {0};

	memcpy(tfid, (void*)&tlfid, 4);
	std::string appId(tfid);
	std::vector<std::string> paramVec;
	paramVec.push_back(appId);
	CallJScript("deleteFavAction", paramVec);
	return 0;
}
LRESULT CAxControl::OnToolsChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CComVariant var;
	VariantInit(&var);
	var.vt = VT_I4 ;
	var.lVal = wParam;
	if(m_pCore->m_pWebBrowser2)
	{
		m_pCore->m_pWebBrowser2->Refresh2(&var);
	}
	return 0;
}
LRESULT CAxControl::OnCouponChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::vector<std::string> paramVec;
	CallJScript("init", paramVec, 1);
	return 0;
}

LRESULT CAxControl::OnCallJSTabActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::vector<std::string> paramVec;
	CallJScript("TabActivated", paramVec);
	return 0;
}

LRESULT CAxControl::OnChangeFirtsPageShow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::vector<std::string> paramVec;
	int nParam = (int)lParam;
	if (nParam == MY_STATUE_USER_NOTLOAD)
		CallJScript("beforeLogin", paramVec);
	else if (nParam == MY_STATUE_USER_LAODED)
		CallJScript("afterLogin", paramVec);
	return 0;
}

LRESULT CAxControl::ChangeSettingShowStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CHANGESTATUS emParam = (CHANGESTATUS)lParam;
	CBankData::GetInstance()->m_CurUserInfo.emStatus = emParam;
	return 0;
}

LRESULT CAxControl::OnShowInfoDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD dw = 0;
	HANDLE hThread;
	LPBILLSHOWINFO pInfo = new BILLSHOWINFO;
	pInfo->pNotifyInterface = m_pEventsManager;
	pInfo->type = wParam;
	if(wParam == 0)
		pInfo->info = L"正在登录......";
	else if(wParam == 1)//显示正在导入账单的对话框
		pInfo->info = L"正在导入账单......";
	hThread = CreateThread(NULL, 0, _threadShowInfoDLG, (LPVOID)pInfo, 0, &dw);
	CloseHandle(hThread);

	return true;
}


LRESULT CAxControl::OnEndInfoDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_endShowInfoDLGthread();

	return true;
}

LRESULT CAxControl::OnAccountSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLASSERT(NULL != lParam);
	list<SELECTINFONODE>* pNodeList = (list<SELECTINFONODE>*)lParam;
	if(NULL == pNodeList)
		return 0;
	if (pNodeList->size() < 1)
		return 0;

	HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	if (0 == wParam)
	{
		CMonthSelectDlg dlg(pNodeList);
		return dlg.DoModal(hMainFrame);
	}
	else
	{
		CAccountSelectDlg dlg(pNodeList);
		return dlg.DoModal(hMainFrame);
	}


	return 0;
}


LRESULT CAxControl::OnGetAllBill(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	char **ppInfo = (char**)lParam;
	if(ppInfo == NULL)
		return 0;

	if((*ppInfo) == NULL)
		return 0;
	std::string info(*ppInfo);
	delete (*ppInfo);
	(*ppInfo) = NULL;
	std::vector<std::string> paramVec;
	paramVec.push_back(info);
	CallJScript("returnBillXml", paramVec);
	
	return 0;

}
CComVariant CAxControl::CallJScript(std::string strFunc,std::vector<std::string>& paramVec, int nType)
{
	//Getting IDispatch for Java Script objects
	if(!m_pCore)
		return false;
	
	if(!m_pCore->m_pWebBrowser2)
		return false;

	CComPtr<IDispatch> pDispatch;
	if (SUCCEEDED(m_pCore->m_pWebBrowser2->get_Document(&pDispatch)))
	{
		CComPtr<IHTMLDocument2> pDoc2;
		if (SUCCEEDED(pDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2)))
		{
			// 获得JS脚本
			CComPtr<IDispatch> spScript;
			if(nType == 0)
			{
				if(!SUCCEEDED(pDoc2->get_Script(&spScript)))
					return false;
			}
			else
			{
				CComPtr<IHTMLFramesCollection2> pFrame = NULL;
				if(!SUCCEEDED(pDoc2->get_frames(&pFrame)))
					return false;

				VARIANT sPram;
				sPram.vt = VT_BSTR;
				sPram.bstrVal = L"mainw";

				VARIANT frameOut;//找到iFrame对象
				if(!SUCCEEDED(pFrame->item(&sPram, &frameOut)))
					return false;

				CComPtr<IHTMLWindow2> pRightFrameWindow = NULL;
				if (!SUCCEEDED(V_DISPATCH(&frameOut)->QueryInterface(IID_IHTMLWindow2,
					(void**)&pRightFrameWindow)))
					return false;

				CComPtr<IHTMLDocument2> pRightDoc = NULL;
				if (!SUCCEEDED(pRightFrameWindow->get_document(&pRightDoc)))
					return false;

				if(!SUCCEEDED(pRightDoc->get_Script(&spScript)))
					return false;
			}

			//
			USES_CONVERSION;
			BSTR bstrMember = ::SysAllocString((LPOLESTR)A2COLE(strFunc.c_str()));
			DISPID dispid = NULL;
			HRESULT hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
				LOCALE_SYSTEM_DEFAULT,&dispid);
			if(FAILED(hr))
			{
				::SysFreeString(bstrMember);
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, CRecordProgram::GetInstance()->GetRecordInfo(L"CallJScript:%s GetIDsOfNames error",A2W(strFunc.c_str())));
				return false;
			}

			const int arraySize = paramVec.size();
			//Putting parameters
			DISPPARAMS dispparams;
			memset(&dispparams, 0, sizeof dispparams);
			dispparams.cArgs      = arraySize;
			dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
			dispparams.cNamedArgs = 0;


			std::vector<std::string>::iterator ite = paramVec.begin();
			int i = 0;

			for( ;ite != paramVec.end(); ite ++)
			{
				//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数
				wstring winfo = AToW((*ite), 936);
				dispparams.rgvarg[i].bstrVal = ::SysAllocString((LPOLESTR)winfo.c_str());
				dispparams.rgvarg[i].vt = VT_BSTR;
				i ++;
			}

			EXCEPINFO excepInfo;
			memset(&excepInfo, 0, sizeof excepInfo);
			CComVariant vaResult;
			UINT nArgErr = (UINT) -1;      
			// initialize to invalid arg
			// Call JavaScript function
			hr = spScript->Invoke(dispid,IID_NULL,0,DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

			// 释放所有申请的空间
			for(i = 0; i< arraySize;i ++)
			{

				::SysFreeString(dispparams.rgvarg[i].bstrVal);
				dispparams.rgvarg[i].bstrVal = NULL;
			}
			delete[] dispparams.rgvarg;
			::SysFreeString(bstrMember);
			bstrMember = NULL;

			if(FAILED(hr))
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, CRecordProgram::GetInstance()->GetRecordInfo(L"CallJScript:%s Invoke error",A2W(strFunc.c_str())));
				return false;
			}

			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_STATE, 
				CRecordProgram::GetInstance()->GetRecordInfo(L"CallJScript:%s 成功",A2W(strFunc.c_str())));
			return vaResult;
		}
	}
	return false;
}
