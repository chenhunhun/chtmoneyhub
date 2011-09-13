#include "stdafx.h"
#include <fstream>
#include "ConvertBase.h"
#include "Util.h"
#include "../Utils/ListManager/URLList.h"
#include "../Utils/Config/HostConfig.h"
#include "AxControl.h"
#include "CustomSite.h"
#include "WebBrowserEventsManager.h"
#include "DownloadManagerImpl.h"
#include "../Utils/HardwareID/genhwid.h"
#include "../BankUI/Util/Util.h"
#include "../BankUI/UIControl/CoolMessageBox.h"
#include "../Utils/UserBehavior/UserBehavior.h"
#include "GetBill/BillUrlManager.h"
#include "ExternalDispatchImpl.h"
#include "BankData\BkInfoDownload.h"
#include "MyError.h"

#define MODE_UNDEFINED                     ((DWORD)0)
#define MODE_HANDINPUT                     ((DWORD)1)
#define MODE_SELECTDROPLIST                ((DWORD)2)
#define MODE_OTHERS                        ((DWORD)500)

#define  BEGIN_GET_BILL_STEPcmbchina	2
#define  BEGIN_GET_BILL_STEPecitic	6
#define  FINISH_GET_BILL_STEP	99
BOOL CWebBrowserEventsManager::AdviseBrowserEvents(IWebBrowser2* pWebBrowser2)
{
	m_pWebBrowser2 = pWebBrowser2 ;
	HRESULT hr = AtlAdvise(m_pWebBrowser2, (IUnknown*)this, DIID_DWebBrowserEvents2, &m_dwCookie);
	return SUCCEEDED(hr) ;
}

BOOL CWebBrowserEventsManager::UnadviseBrowserEvents()
{
	if (!m_pWebBrowser2 || !m_dwCookie)
	{
		ATLASSERT(FALSE) ;
		return FALSE ;
	}

	HRESULT hr = AtlUnadvise(m_pWebBrowser2, DIID_DWebBrowserEvents2, m_dwCookie) ;
	return SUCCEEDED(hr) ;
}

CWebBrowserEventsManager::CWebBrowserEventsManager(CAxControl *pAxControl): m_pAxControl(pAxControl)
{
	m_error = false;
	m_pBillData = NULL;
	m_isGetBill = false;
	m_step = 1;
	m_hmid = 0;

	m_dwCookie = 0 ;
	m_pCustomSite = NULL ;
	m_dwPercentage = 0 ;
	m_dwZoomPage = 100 ;
	m_bForceBackOpen = FALSE ;
	m_bShowTitle = FALSE ;
	m_bInsertTitle = FALSE ;
	m_cstrFindString = L"" ;
	m_dwTotalFindString = 0 ;
	m_dwCurrentFindString = -1 ;
	m_dwLastSubmitTickCount = 0;
	m_lOldStatus = -1 ;
	m_pDispDocument = NULL ;
	m_pWebBrowser2 = NULL ;
	m_dwNewestBeforeNavTick = GetTickCount() ;
	m_hTempWriteHTMLData = NULL;
	m_cstrOriUrl = L"" ;
	m_cstrLastUrl = L"" ;
	m_cstrConnectedUrl = L"" ;
	m_dwAxUserActionTick = GetTickCount() - 200 ;
	m_bPostDataInTopFrame = FALSE ;
	m_bPostDataInAnyFrame = FALSE ;
	m_bFormCheck = FALSE ;
	m_adv = CHostContainer::GetInstance()->GetHostName(kAdv);
}

CWebBrowserEventsManager::~CWebBrowserEventsManager()
{
	if (m_hTempWriteHTMLData)
		::GlobalFree(m_hTempWriteHTMLData);
}

ULONG CWebBrowserEventsManager::AddRef()
{
	return 1;
}

ULONG CWebBrowserEventsManager::Release()
{
	return 1;
}


HRESULT CWebBrowserEventsManager::QueryInterface(const IID &riid, void **ppv)
{
	if (!ppv)
	{
		return E_POINTER ;
	}

	if (IID_IUnknown == riid)
	{
		AddRef();
		*ppv = (IUnknown*)this ;
		return S_OK ;
	}
	else if (IID_IDispatch == riid)
	{
		AddRef();
		*ppv = (IDispatch*)this ;
		return S_OK ;
	}

	return E_NOTIMPL ;
}


BOOL CWebBrowserEventsManager::SetCustomSite(CCustomSite* pCustomSite)
{
	if (!pCustomSite)
	{
		return FALSE ;
	}
	m_pCustomSite = pCustomSite ;
	return TRUE ;
}

void CWebBrowserEventsManager::SetWindowOpenMode(BOOL bForceBackOpen)
{
	m_bForceBackOpen = bForceBackOpen ;
}

HRESULT CWebBrowserEventsManager::GetTypeInfoCount(UINT* pctinfo)
{
	return E_NOTIMPL ;
}

HRESULT CWebBrowserEventsManager::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppInfo)
{
	return E_NOTIMPL ;
}

HRESULT CWebBrowserEventsManager::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	return E_NOTIMPL ;
}

HRESULT CWebBrowserEventsManager::Invoke(DISPID dispIdMember, const IID &riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	switch(dispIdMember)
	{
	case DISPID_TITLECHANGE :
		return OnTitleChange(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_STATUSTEXTCHANGE :
		return OnStatuesChange(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_NEWWINDOW2 :
		return OnNewWindow2(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_NEWWINDOW3:
		return OnNewWindow3(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);

	case DISPID_BEFORENAVIGATE2 :
		return OnBeforeNavigate2(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_NAVIGATECOMPLETE2 :
		return OnNavigateComplete2(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_DOCUMENTCOMPLETE :
		return OnDocumentComplete(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_PROGRESSCHANGE :
		return OnProgressChange(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_WINDOWCLOSING :
		return OnWindowClosing(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_COMMANDSTATECHANGE :
		return OnCommandStateChange(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_NAVIGATEERROR :
		return OnNavigateError(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;

	case DISPID_SETSECURELOCKICON :
		return OnSetSecureLockIcon(riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr) ;
	}
	
	return E_FAIL ;
}


HRESULT CWebBrowserEventsManager::OnCommandStateChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if(m_isGetBill)
		if(m_step > 1)
			return S_OK;

	if (pDispParams->cArgs != 2)
	{
		return E_INVALIDARG ;
	}

	if (CSC_NAVIGATEBACK == pDispParams->rgvarg[1].lVal)
	{
		::PostMessage(m_pAxControl->m_hChildFrame, WM_ITEM_SET_MAIN_TOOLBAR, MAKEWPARAM(0, pDispParams->rgvarg[0].boolVal), 0);
	}
	else if (CSC_NAVIGATEFORWARD == pDispParams->rgvarg[1].lVal)
	{
		::PostMessage(m_pAxControl->m_hChildFrame, WM_ITEM_SET_MAIN_TOOLBAR, MAKEWPARAM(1, pDispParams->rgvarg[0].boolVal), 0);
	}

	return S_OK ;
}

HRESULT CWebBrowserEventsManager::OnWindowClosing(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	return S_OK ;
}

HRESULT CWebBrowserEventsManager::OnStatuesChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	// 如果是导入账单页，阻止更新界面信息的改变
	if(m_isGetBill)
		if(m_step > 1)
			return S_OK;

	if (pDispParams->cArgs < 1)
	{
		return E_FAIL ;
	}

	// 过滤掉一些未知信息不显示在状态栏里
	if(pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != NULL)
	{
		if (!wcsncmp(L"javascript:", pDispParams->rgvarg[0].bstrVal, 11)   ||
			!wcsncmp(L"vbscript:", pDispParams->rgvarg[0].bstrVal, 9)      ||
			!wcsncmp(L"mailto:", pDispParams->rgvarg[0].bstrVal, 7)        ||
			!wcsncmp(L"about:", pDispParams->rgvarg[0].bstrVal, 6)         ||
			!wcsncmp(L"res:", pDispParams->rgvarg[0].bstrVal, 4)		  ||
			!wcsncmp(L"http:", pDispParams->rgvarg[0].bstrVal, 5)         ||
			!wcsncmp(L"file:", pDispParams->rgvarg[0].bstrVal, 5))
		{
			return S_OK ;
		}
	}

	wstring sinfo(pDispParams->rgvarg[0].bstrVal);
	size_t n = sinfo.find(L"http");
	if (n != std::wstring::npos)
	{
		sinfo = sinfo.substr(0,  n - 1);
	}

	n = sinfo.find(L"file");
	if (n != std::wstring::npos)
	{
		sinfo = sinfo.substr(0,  n - 1);
	}
	if(sinfo != L"")
		::SendMessage(m_pAxControl->m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_STATUS, (LPARAM)sinfo.c_str());

	return S_OK ;
}

// OnNewWindow3 在 XP SP2 或更高版本才有，但 OnNewWindow2 对于某些 flash 弹出的链接拿不到对应的 URL
// 因此，如果是 XP SP2 或更高版本，我们用 OnNewWindow3.
//
HRESULT CWebBrowserEventsManager::OnNewWindow2(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	return S_FALSE;
}

HRESULT CWebBrowserEventsManager::OnNewWindow3(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if (pDispParams->cArgs == 5)
		return OnNewWindow(pDispParams->rgvarg[0].bstrVal, pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[4].ppdispVal, pDispParams->rgvarg[3].pboolVal);
	return S_FALSE;
}

// wanyong 2009-1-23
// 发现现在的做法有点小问题
// 参考 http://www.archivum.info/microsoft.public.inetsdk.programming.webbrowser_ctl/2006-04/msg00015.html
//         http://www.archivum.info/microsoft.public.inetsdk.programming.webbrowser_ctl/2006-04/msg00016.html
//
// msdn上也表明了（如下），正确的做法是先创建个隐藏的没有navigate的webbrowser实例即可，然后做跨线程或跨进程的marshal
// ------------------------------------------------
// From DWebBrowserEvents2::NewWindow3 Event
// The application that processes this notification can respond in one of three ways. 
// 
// * Create a new, hidden, nonnavigated WebBrowser or InternetExplorer object that is returned in ppDisp. 
//			Upon return from this event, the object that raised this event will then configure and navigate (including a BeforeNavigate2 event) 
//			the new object to the target location.
// * Cancel the navigation by setting Cancel to VARIANT_TRUE.
// * Do nothing and do not set ppDisp to any value. This will cause the object that raised the event to create a new InternetExplorer 
//			object to handle the navigation.
// ------------------------------------------------
//
std::wstring CWebBrowserEventsManager::Replace( const std::wstring& orignStr, const std::wstring& oldStr, const std::wstring& newStr)
{   
	size_t pos = 0;
	wstring tempStr = orignStr;
	wstring::size_type newStrLen = newStr.length();
	wstring::size_type oldStrLen = oldStr.length();
	while(true)
	{   
		pos = tempStr.find(oldStr, pos);
		if (pos == wstring::npos) 
			break;   
		tempStr.replace(pos, oldStrLen, newStr);
		pos += newStrLen;  
	}  
	return tempStr;   
} 
HRESULT CWebBrowserEventsManager::OnNewWindow(LPCTSTR lpszUrl, LPCTSTR lpszReferer, IDispatch **&ppDisp, VARIANT_BOOL *&Cancel)
{
	// 如果是脚本，则在本窗口内执行
	if (GetKindOfNavigateUrl(lpszUrl) == SCRIPT_SCHEME_KEY)
	{
		return S_OK;
	}

	if (!::IsWindow(m_pAxControl->m_hChildFrame))
	{
		*Cancel = TRUE;
		return S_OK;
	}

	if(_tcsnicmp(lpszUrl, _T("about:blank"), 11) == 0)
	{
		if(lpszReferer != NULL)//对于采用windows.open(url,"_blank")这种打开新网页的处理，例如中国银行
			lpszUrl = lpszReferer;
	}

	//广告抛到我们自己的页面		
	std::wstring  url(lpszUrl);
	size_t dpos = url.find(m_adv);

	if(dpos != std::wstring::npos)
	{
		size_t spos = url.find(L"http", dpos);
		if(spos != std::wstring::npos)
		{
			std::wstring suburl = url.substr(spos);

			std::wstring turl = Replace(suburl, L"%3A", L":");
			url = Replace(turl, L"%2F", L"/");
			lpszUrl = url.c_str();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 
	std::wstring strDomain = ExtractTopDomain(lpszUrl);
	const CWebsiteData *pData = CURLList::GetInstance()->GetData(L"", lpszUrl);

	if (pData == NULL)
	{
		if (_tcsnicmp(lpszUrl, _T("http"), 4) == 0)
		{
			*Cancel = TRUE;
			CListManager::NavigateWebPage(lpszUrl);

/*
			IWebBrowser2* pWebBrowser2 = NULL;
			CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pWebBrowser2);
			if (pWebBrowser2)
			{
				pWebBrowser2->get_Application(ppDisp);
				pWebBrowser2->Release();
			}
*/

			return S_OK;
		}
	}


	//////////////////////////////////////////////////////////////////////////

	bool bOpenBackground = false;
	// Control 键优先级最高，如果按下就一定在后台打开
	if (GetAsyncKeyState(VK_CONTROL) < 0 || m_bForceBackOpen)
		bOpenBackground = true;
	// Shift 优先级次高
	else if (GetAsyncKeyState(VK_SHIFT) < 0)
		bOpenBackground = false;


	//////////////////////////////////////////////////////////////////////////

	HWND hChildFrame = NULL;
	
	enum TopPageStruct tps = (enum TopPageStruct)CListManager::GetDefaultPageIndex(lpszUrl);
	if (tps != kPageInvalid)
	{
		HWND hFrameWnd = ::GetRootWindow(m_pAxControl->m_hChildFrame);
		SwitchTopPage(hFrameWnd, tps);

		*Cancel = TRUE;
		return S_OK;
	}
	else if (pData == NULL)
	{
		if (_tcsnicmp(lpszUrl, _T("about:blank"), 11) != 0)
		{
			*Cancel = TRUE;
			CListManager::NavigateWebPage(lpszUrl);

			return S_OK;
		}
	}

	HWND hAxControl = NULL;
	if(m_isGetBill)
	{
		bOpenBackground = true;
		if(m_step > 1)
			if (_tcsnicmp(lpszUrl, _T("about:blank"), 11) == 0)
			{
				*Cancel = TRUE;
				return S_OK;
			}
	}
	if (hChildFrame == NULL)
	{
		if (bOpenBackground)
		{
			hChildFrame = (HWND)CreateNewPage_3_NewWindow(m_pAxControl->m_hChildFrame, lpszUrl, FALSE, m_pAxControl->m_hChildFrame, m_isGetBill);
			m_bForceBackOpen = FALSE;
		}
		else
			hChildFrame = (HWND)CreateNewPage_3_NewWindow(m_pAxControl->m_hChildFrame, lpszUrl, TRUE, m_pAxControl->m_hChildFrame, m_isGetBill);
	}

	if (hChildFrame == NULL)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, CRecordProgram::GetInstance()->GetRecordInfo(L"OnNewWindow-CreateNewPage_3_ExistWindow-2失败:%s",lpszUrl));
		*Cancel = TRUE;
		return S_OK;
	}


	for (int i = 0; i < 400; ++i)
	{
		hAxControl = (HWND)::SendMessage(hChildFrame, WM_ITEM_GET_AX_CONTROL_WND, 0, 0);
		if (hAxControl)
			break ;
		Sleep(10) ;
	}


	bool bSuccess = false;
	if (hAxControl != NULL)
	{
		DWORD dwProcessIDDescendant = 0;
		GetWindowThreadProcessId(hAxControl, &dwProcessIDDescendant) ;
		if (dwProcessIDDescendant == GetCurrentProcessId())
		{		
			LPARAM lParam = 0;
			WPARAM isGetBill = 0;

			if(m_isGetBill)
			{
				lParam = (LPARAM)m_pBillData;
				isGetBill = 2 *100 + m_step;
			}
			// 如果是导入账单页，那么将导入账单的数据和进行到的步骤通知创建的页面
			IStream* pStream = (IStream*)::SendMessageBlock(hAxControl, WM_AX_GET_WEBBROWSER2_CROSS_THREAD, isGetBill, lParam) ;
			if (pStream)
			{
				CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_STATE, CRecordProgram::GetInstance()->GetRecordInfo(L"OnNewWindow-浏览页面%s成功", lpszUrl));
				CComPtr<IWebBrowser2> spWebBrowser2;
				if (SUCCEEDED(CoGetInterfaceAndReleaseStream(pStream, IID_IWebBrowser2, (void**)&spWebBrowser2)) && spWebBrowser2)
				{	
					spWebBrowser2->put_RegisterAsDropTarget(VARIANT_FALSE);

					// 招商银行js需要识别窗口名
					if (_tcsstr(lpszReferer, _T("ebank.cmbchina.com")) != NULL)
						spWebBrowser2->put_RegisterAsBrowser(VARIANT_TRUE);

					if (SUCCEEDED(spWebBrowser2->get_Application(ppDisp)) && ppDisp)
						return S_OK;
					else
						CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, CRecordProgram::GetInstance()->GetRecordInfo(L"OnNewWindow-get_Application失败:%s",lpszUrl));
				}
				else
				{
					CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, CRecordProgram::GetInstance()->GetRecordInfo(L"OnNewWindow-CoGetInterfaceAndReleaseStream失败:%s",lpszUrl));
				}
			}
		}
	}
	if (!bSuccess)
	{
		*Cancel = TRUE;
	}

	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnTitleChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if (pDispParams->cArgs < 1)
		return E_FAIL ;
	// 如果是导入账单页，阻止更改界面的显示
	if(m_isGetBill)
		if(m_step > 1)
			return S_OK;

	if (!m_bShowTitle && pDispParams->rgvarg[0].bstrVal[0])
	{
		::SendMessage(m_pAxControl->m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_TAB_TEXT, (LPARAM)pDispParams->rgvarg[0].bstrVal);
		m_cstrTitle = pDispParams->rgvarg[0].bstrVal ;

		if (0 == m_bInsertTitle && m_cstrConnectedUrl.GetLength() > 0)
		{
			CString cstr = m_cstrConnectedUrl ;
			cstr += L"\n\n" ;
			cstr += pDispParams->rgvarg[0].bstrVal ;
		}
		if (m_cstrConnectedUrl.GetLength() > 0)
			if (m_cstrConnectedUrl.Compare(pDispParams->rgvarg[0].bstrVal))
				if (wcsncmp(pDispParams->rgvarg[0].bstrVal, L"http", 4))
					m_bInsertTitle = TRUE ;
	}
	return S_OK ;
}


HRESULT CWebBrowserEventsManager::OnBeforeNavigate2(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	m_error = false;
	if(pDispParams->cArgs < 7)
	{
		return E_FAIL;
	}
	// 在该处处理导入账单页面开始进行和中间关键步骤的起始处理
	if(m_isGetBill)
	{
		std::string  sss= m_pBillData->aid;
		map<int, wstring>* purlmap =  CBillUrlManager::GetInstance()->GetUrlMap(m_pBillData->aid, m_pBillData->type);

		if( sss == "a007" )
		{
			CString   strtmpqq;
       		CString   strtmp   =   (   CString   ) pDispParams->rgvarg[5].pvarVal->bstrVal;
				map<int, wstring>::iterator ite= purlmap->find(3);
				wstring url = ite->second;
				if(strtmp == url.c_str() )
						::PostMessage(m_pAxControl->m_hChildFrame,WM_GETTING_BILL, 0, 0);

		}
		else if( purlmap!= NULL)
		{
			map<int, wstring>::iterator ite= purlmap->begin();
			for(;ite != purlmap->end(); ite ++)
			{
				wstring url = ite->second;
				if(url.size() > 0)
					if(wcsncmp(url.c_str(), pDispParams->rgvarg[5].pvarVal->bstrVal, url.size()) == 0)
					{
						CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_GET_BILL, CRecordProgram::GetInstance()->GetRecordInfo(L"导入账单进入关键页面%d:%s", m_step, url.c_str()));
						if(ite->first > m_step)
							m_step = ite->first;
						else
							if(ite->first > 1)
								m_step ++;
						// 锁定当前Tab页面
						if(m_step == BEGIN_GET_BILL_STEPcmbchina && url.find(_T("cmbchina"),0) <100)
							::PostMessage(m_pAxControl->m_hChildFrame,WM_GETTING_BILL, 0, 0);

						return S_OK;
					}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// Download Catch

	BOOL bClosed = FALSE;
	IDispatch* pDispatch = pDispParams->rgvarg[6].pdispVal;
	if (pDispatch && m_pWebBrowser2)
	{
		CComPtr<IUnknown> pUnk;
		CComPtr<IDispatch> pWBDisp;

		HRESULT hr = m_pWebBrowser2->QueryInterface(IID_IUnknown, (void**)&pUnk);
		HRESULT hr2 = pUnk->QueryInterface(IID_IDispatch, (void**)&pWBDisp);
		if (SUCCEEDED(hr) && SUCCEEDED(hr2) && pDispatch && pDispatch == pWBDisp)
		{
			BSTR bstrUrl;
			m_pWebBrowser2->get_LocationURL(&bstrUrl);
			if (SysStringLen(bstrUrl) == 0)
				bClosed = TRUE;
			SysFreeString(bstrUrl);
		}
	}

	HWND hCloseWnd = bClosed ? m_pAxControl->m_hChildFrame : NULL;
	CAsyncDownloadHelper cHelper(m_pAxControl->m_hChildFrame, hCloseWnd);
	if (SUCCEEDED(cHelper.BeforeNavigate2(pDispParams->rgvarg[6].pdispVal, 
										  pDispParams->rgvarg[5].pvarVal, 
										  pDispParams->rgvarg[4].pvarVal, 
										  pDispParams->rgvarg[3].pvarVal, 
										  pDispParams->rgvarg[2].pvarVal, 
										  pDispParams->rgvarg[1].pvarVal, 
										  pDispParams->rgvarg[0].pboolVal)))
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_STATE, CRecordProgram::GetInstance()->GetRecordInfo(L"DownloadHelper: %s-开始下载", pDispParams->rgvarg[5].pvarVal->bstrVal));
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	CString strUrl(pDispParams->rgvarg[5].pvarVal->bstrVal);
	const CWebsiteData* pData = NULL;

	if((wcsncmp(L"https:", pDispParams->rgvarg[5].pvarVal->bstrVal, 6) == 0))
	{
		USES_CONVERSION;

		pData = CURLList::GetInstance()->GetData(L"", strUrl);
		if(NULL != pData)
		{
			//对访问https页面才进行收藏
			if(wcsncmp(L"https:", pDispParams->rgvarg[5].pvarVal->bstrVal, 6) == 0)
			{
				USES_CONVERSION;
				std::string appid = CFavBankOperator::GetBankIDOrBankName(W2A(pData->GetID()), false);
				if(appid != "")
				{
					if( !CBankData::GetInstance()->IsFavExist(appid))
					{
						CStringW msg;
						msg.Format(_T("检测到%s没有收藏，您要现在进行收藏吗？"), pData->GetName());
						DWORD dwVersion = GetVersion();
						int	res;
						// win7存在当子窗口的处理线程被拥塞后，父窗口也会无响应的状况，所以在这里单独创建线程去显示，否则采用让窗口线程显示询问结果的步骤
						if((DWORD)(LOBYTE(LOWORD(dwVersion))) > 5)
						{
							DWORD dw;
							MsgData md;
							md.type = 1;
							md.info = msg.GetString();
							md.wtype = MB_OKCANCEL;
							HANDLE hThread = CreateThread(NULL, 0, _threadShowWaitDLG, (LPVOID)&md, 0, &dw);

							WaitForSingleObject(hThread, INFINITE);
							//读取用户选择的结果
							res = md.res;
						}
						else
							res = ::SendMessage(m_pAxControl->m_hChildFrame, WM_SETTEXT, WM_ITEM_ASK_SAVE_FAV, (LPARAM)msg.GetString());
						if(res == IDOK)
						{							
							CNotifyFavProgress::GetInstance()->AddFav(appid);//调用js开始安装;
							wstring smsg = A2W(appid.c_str()); 
							msg.Format(_T("%s正在下载%s控件，请稍候......"), smsg.c_str(), pData->GetName());
							//int res = ::SendMessageW(m_pAxControl->m_hChildFrame, WM_SETTEXT, WM_ITEM_TEST_SAVE_FAV, (LPARAM)msg.GetString());

							if((DWORD)(LOBYTE(LOWORD(dwVersion))) > 5)
							{
								DWORD dw;
								MsgData md;
								md.type = 2;
								md.info = msg.GetString();
								md.wtype = MB_OKCANCEL;
								HANDLE hThread = CreateThread(NULL, 0, _threadShowWaitDLG, (LPVOID)&md, 0, &dw);

								WaitForSingleObject(hThread, INFINITE);
							}
							else
								::SendMessageW(m_pAxControl->m_hChildFrame, WM_SETTEXT, WM_ITEM_TEST_SAVE_FAV, (LPARAM)msg.GetString());
						}
					}
				}
			}
		}
	}

	if (IsToppestWindowOfBrowser2(pDispParams->rgvarg[6].pdispVal))
	{
		if (!wcsncmp(L"javascript:", pDispParams->rgvarg[5].pvarVal->bstrVal, 11)   ||
			!wcsncmp(L"vbscript:", pDispParams->rgvarg[5].pvarVal->bstrVal, 9)      ||
			!wcsncmp(L"mailto:", pDispParams->rgvarg[5].pvarVal->bstrVal, 7)        ||
			!wcsncmp(L"about:", pDispParams->rgvarg[5].pvarVal->bstrVal, 6)         ||
			!wcsncmp(L"res:", pDispParams->rgvarg[5].pvarVal->bstrVal, 4))
		{
			return S_OK ;
		}


		if((wcsncmp(L"https:", pDispParams->rgvarg[5].pvarVal->bstrVal, 6) == 0) || 
			(wcsncmp(L"http:", pDispParams->rgvarg[5].pvarVal->bstrVal, 5) == 0) || 
			(wcsncmp(L"file:", pDispParams->rgvarg[5].pvarVal->bstrVal, 5) == 0))
		{
			USES_CONVERSION;

			//我们自己的广告页进行转换
			std::wstring  url(pDispParams->rgvarg[5].pvarVal->bstrVal);
			size_t dpos = url.find(m_adv);


			if(dpos != std::wstring::npos)
			{
				size_t spos = url.find(L"http", dpos);
				if(spos != std::wstring::npos)
				{
					std::wstring suburl = url.substr(spos);
					std::wstring turl = Replace(suburl, L"%3A", L":");
					url = Replace(turl, L"%2F", L"/");
					strUrl = url.c_str();
				}
			}
		}
		if(NULL == pData)
			pData = CURLList::GetInstance()->GetData(L"", strUrl);
		if(NULL == pData)
		{
			*pDispParams->rgvarg[0].pboolVal = VARIANT_TRUE;
			CListManager::NavigateWebPage(strUrl);

			::PostMessage(m_pAxControl->m_hChildFrame, WM_ITEM_AUTOCLOSE, 0, 0);

			return E_FAIL;
		}

		m_cstrLastUrl = pDispParams->rgvarg[5].pvarVal->bstrVal ;
		::SendMessage(m_pAxControl->m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_TAB_URL, (LPARAM)pDispParams->rgvarg[5].pvarVal->bstrVal);
	}

	return S_OK ;
}

HRESULT CWebBrowserEventsManager::OnNavigateComplete2(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if (pDispParams->cArgs < 2)
	{
		return E_FAIL;
	}

	//这里主要是如果该页面为导入账单页面，那么阻止修改界面上的显示消息
	if(m_isGetBill)
		if(m_step > 1)
			return S_OK;

	if (IsToppestWindowOfBrowser2(pDispParams->rgvarg[1].pdispVal))
	{
		if (!wcsncmp(L"javascript:", pDispParams->rgvarg[0].pvarVal->bstrVal, 11)   ||
			!wcsncmp(L"vbscript:", pDispParams->rgvarg[0].pvarVal->bstrVal, 9)      ||
			!wcsncmp(L"mailto:", pDispParams->rgvarg[0].pvarVal->bstrVal, 7)        ||
			!wcsncmp(L"about:", pDispParams->rgvarg[0].pvarVal->bstrVal, 6)          )
		{
			return S_OK ;
		}

		if (0 == m_cstrConnectedUrl.GetLength() || -1 == m_cstrConnectedUrl.Find(L"http://"))
		{
			CString cstrOri = m_cstrOriUrl ;
			CString cstrInput = pDispParams->rgvarg[0].pvarVal->bstrVal ;
			int i = cstrOri.Find(L"://") ;
			if (i > 0)
				cstrOri = cstrOri.Right(cstrOri.GetLength() - 3 - i) ;

			m_cstrConnectedUrl = m_cstrOriUrl ;

			BOOL bSamilarUrl = FALSE ;
			CString cstrLastUrl = m_cstrLastUrl ;
			CString cstrCurUrl = m_cstrConnectedUrl ;
			do 
			{
				i = cstrLastUrl.Find('#') ;
				if (i > 0)
					cstrLastUrl = cstrLastUrl.Left(i) ;
				i = cstrCurUrl.Find('#') ;
				if (i > 0)
					cstrCurUrl = cstrCurUrl.Left(i) ;
				if (0 == cstrCurUrl.Compare(cstrLastUrl))
				{
					bSamilarUrl = TRUE ;
					break ;
				}

				i = cstrLastUrl.Find('?') ;
				if (i > 0)
					cstrLastUrl = cstrLastUrl.Left(i) ;
				i = cstrCurUrl.Find('?') ;
				if (i > 0)
					cstrCurUrl = cstrCurUrl.Left(i) ;
				if (0 == cstrCurUrl.Compare(cstrLastUrl))
				{
					bSamilarUrl = TRUE ;
					break ;
				}
			}
			while (0);
		}

		if (!wcsncmp(L"http:", pDispParams->rgvarg[0].pvarVal->bstrVal, 5)   ||
			!wcsncmp(L"https:", pDispParams->rgvarg[0].pvarVal->bstrVal, 6)  )
		{
			// 加载完页面后检测白名单,//只有当页面为http和https的才检测在不在白名单里
			CString strUrl(pDispParams->rgvarg[0].pvarVal->bstrVal);
			if (NULL == CURLList::GetInstance()->GetData(L"", strUrl))
			{
				CListManager::NavigateWebPage(strUrl);
				::PostMessage(m_pAxControl->m_hChildFrame, WM_ITEM_AUTOCLOSE, 0, 0);
				return S_OK;
			}
		}		

		m_cstrLastUrl = pDispParams->rgvarg[0].pvarVal->bstrVal ;

		::SendMessage(m_pAxControl->m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_TAB_URL, (LPARAM)pDispParams->rgvarg[0].pvarVal->bstrVal);
		m_pDispDocument = pDispParams->rgvarg[1].pdispVal;
	}

	return S_OK ;
}

HRESULT CWebBrowserEventsManager::OnDocumentComplete(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	//将焦点放置在最底层，因为只有当发生该事件时，才能放置焦点，否则里面的窗口还没创建
	LONG style = ::GetWindowLongW(m_pAxControl->m_hWnd, GWL_STYLE);
	if((style & WS_VISIBLE) ==  WS_VISIBLE)
	{
		HWND TChild, MChild, BChild;
		TChild = FindWindowExW(m_pAxControl->m_hWnd , NULL, L"Shell Embedding", NULL);
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

	if (pDispParams->cArgs < 2)
	{
		return E_FAIL;
	}

	int state = 0;
	if(m_isGetBill)
	{
		//////////////////////////////////////////////////////////////////////////
		IWebBrowser2 *thisBrowser	= NULL;
		IDispatch *docDisp			= NULL;
		HRESULT hr = S_OK;
		IDispatch* pBrowser = pDispParams->rgvarg[1].pdispVal;
		hr = pBrowser->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void **>(&thisBrowser));

		std::string  sss= m_pBillData->aid;
		map<int, wstring>* purlmap =  CBillUrlManager::GetInstance()->GetUrlMap(m_pBillData->aid, m_pBillData->type);

		int maxnum = 0; 
		if( sss == "a007" )
		{
			CString   strtmpqq;
       		CString   strtmp   =   (   CString   ) pDispParams->rgvarg[0].pvarVal->bstrVal;
		   // 	if(strtmp.GetLength() > 500)
					//::PostMessage(m_pAxControl->m_hChildFrame,WM_GETTING_BILL, 0, 0);
        	strtmp.Replace(_T("&"),_T(""));
				map<int, wstring>::iterator ite= purlmap->find(3);
				wstring url = ite->second;
						if(strtmp == url.c_str() && url.size() == strtmp.GetLength())
						{
								m_step = 3;
		    					maxnum =65;
						}
				ite= purlmap->find(4);				 url = ite->second;
				strtmpqq = strtmp.Mid(0,59);
						if(strtmpqq == url.c_str() && url.size() > 0)
						{
         						m_step =4;
				    			maxnum =65;
						}
				ite= purlmap->find(5);				 url = ite->second;
				strtmpqq = strtmp.Mid(0,61);
						if(strtmpqq == url.c_str() && url.size() > 0)
						{
         						m_step =5;
				    			maxnum =65;
						}
				ite= purlmap->find(6);				 url = ite->second;
				strtmpqq = strtmp.Mid(0,63);
						if(strtmpqq == url.c_str() && url.size() > 0)
						{
         						m_step =6;
				    			maxnum =65;
						}

					if (SUCCEEDED(hr) && thisBrowser != NULL) 
					{	
						// 判断是否是当前页面加载完成，这里用状态和browser对象来得到完成加载的数据
						if(m_pWebBrowser2 == thisBrowser || maxnum==65 )
						{
							READYSTATE rstate;
							hr = thisBrowser->get_ReadyState(&rstate);
							if(hr == S_OK && rstate == READYSTATE_COMPLETE)
							{
								//开始判断该线程是否是记账功能的相关线程
								if(m_pBillData)
								{
									CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_GET_BILL, CRecordProgram::GetInstance()->GetRecordInfo(L"导入账单处理%d", m_step));
									state = CBillUrlManager::GetInstance()->GetBill(thisBrowser, m_pBillData, m_step);
								}
							}
							maxnum = 0;
						}
               		}	

		}
     	else if (SUCCEEDED(hr) && thisBrowser != NULL) 
		{	
			// 判断是否是当前页面加载完成，这里用状态和browser对象来得到完成加载的数据
			if( m_pWebBrowser2 == thisBrowser)
			{
				READYSTATE rstate;
				hr = thisBrowser->get_ReadyState(&rstate);
				if(hr == S_OK && rstate == READYSTATE_COMPLETE)
				{
					//开始判断该线程是否是记账功能的相关线程
					if(m_pBillData)
					{
						CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_GET_BILL, CRecordProgram::GetInstance()->GetRecordInfo(L"导入账单处理%d", m_step));
						state = CBillUrlManager::GetInstance()->GetBill(thisBrowser, m_pBillData, m_step);

						if(m_pBillData->aid == "a006")
							if(m_step == 4)
								m_step ++;
					}
				}

			}
		}	
		if (thisBrowser) { thisBrowser->Release(); thisBrowser = NULL; }
	}


/* 暂时关闭该部分代码，以后添加到本身就含有js代码的页面中
	//////////////////////////////////////////////////////////////////////////
	// 插入 onerror 事件
	IWebBrowser2 *thisBrowser	= NULL;
	IDispatch *docDisp			= NULL;
	IHTMLDocument3 *doc			= NULL;
	IHTMLElement *elem			= NULL;
	HRESULT hr = S_OK;

	IDispatch* pBrowser = pDispParams->rgvarg[1].pdispVal;
	hr = pBrowser->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void **>(&thisBrowser));
	if (FAILED(hr) || thisBrowser == NULL) goto cleanup;
	hr = thisBrowser->get_Document(&docDisp);
	if (FAILED(hr) || docDisp == NULL) goto cleanup;
	hr = docDisp->QueryInterface(IID_IHTMLDocument3, reinterpret_cast<void**>(&doc));
	if (FAILED(hr) || doc == NULL) goto cleanup;
	if (docDisp) { docDisp->Release(); docDisp = NULL; }

	hr = doc->get_documentElement(&elem);
	if (FAILED(hr) || elem == NULL) goto cleanup;
	hr = elem->insertAdjacentHTML(L"afterBegin", L"&#xa0;<script for='window' event='onerror'>var noOp = null;</script>");
	// Cleanup.
cleanup:
	if (elem) { elem->Release(); elem = NULL; }
	if (thisBrowser) { thisBrowser->Release(); thisBrowser = NULL; }
	if (doc) { doc->Release(); doc = NULL; }
*/
	//////////////////////////////////////////////////////////////////////////
	//

	if (IsToppestWindowOfBrowser2(pDispParams->rgvarg[1].pdispVal))
	{
		if (!wcsncmp(L"javascript:", pDispParams->rgvarg[0].pvarVal->bstrVal, 11)   ||
			!wcsncmp(L"vbscript:", pDispParams->rgvarg[0].pvarVal->bstrVal, 9)      ||
			!wcsncmp(L"mailto:", pDispParams->rgvarg[0].pvarVal->bstrVal, 7)        ||
			!wcsncmp(L"about:", pDispParams->rgvarg[0].pvarVal->bstrVal, 6)          )
		{
			return S_OK ;
		}
		::SendMessage(m_pAxControl->m_hChildFrame, WM_SETTEXT, WM_ITEM_SET_TAB_URL, (LPARAM)pDispParams->rgvarg[0].pvarVal->bstrVal);
	}


	if(state == FINISH_GET_BILL_STEP)
	{
		CBillUrlManager::GetInstance()->FreeDll();
		::SendMessage(m_pAxControl->m_hChildFrame, WM_FINISH_GET_BILL, 0, 0);
		::PostMessage(CExternalDispatchImpl::m_hFrame[2], WM_AX_CLOSE_GET_BILL, 0 , 0);								
	}
	return S_OK ;
}

HRESULT CWebBrowserEventsManager::OnProgressChange(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if (pDispParams->cArgs < 2)
	{
		return E_FAIL;
	}
	if (pDispParams->rgvarg[0].lVal == 0 && pDispParams->rgvarg[1].lVal == 0)
	{
		m_dwPercentage = 0 ;
		return S_OK ;
	}

	if(m_isGetBill)
		if(m_step > 1)
			return S_OK;

	long lMax = pDispParams->rgvarg[0].lVal ;
	long lNow = pDispParams->rgvarg[1].lVal ;

	if (0 == lMax || 0 == lNow)
	{
		::PostMessage(m_pAxControl->m_hChildFrame, WM_ITEM_SET_PAGE_PROGRESS, 100, 0);

		return S_OK ;
	}

	if (-1 == lNow)
	{
		m_dwPercentage = 100;
	}
	else
	{
		m_dwPercentage = (DWORD)((lNow * 100)/lMax) > m_dwPercentage ? (DWORD)((lNow * 100)/lMax) : m_dwPercentage ;
		m_dwPercentage = m_dwPercentage > 100 ? 100 : m_dwPercentage ;

		//m_pAxControl->SetTimer(0x4, 2000);
	}

	::PostMessage(m_pAxControl->m_hChildFrame, WM_ITEM_SET_PAGE_PROGRESS, m_dwPercentage, 0);

	return S_OK ;
}

HRESULT CWebBrowserEventsManager::OnQuit(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnNavigateError(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if (!wcsncmp(L"javascript:", pDispParams->rgvarg[3].pvarVal->bstrVal, 11)   ||
		!wcsncmp(L"vbscript:", pDispParams->rgvarg[3].pvarVal->bstrVal, 9)      ||
		!wcsncmp(L"mailto:", pDispParams->rgvarg[3].pvarVal->bstrVal, 7)        ||
		!wcsncmp(L"about:", pDispParams->rgvarg[3].pvarVal->bstrVal, 6)          )
	{
		return S_OK ;
	}

	m_cstrErrUrl = pDispParams->rgvarg[3].pvarVal->bstrVal ;
	wchar_t* strbom = L"http://www.bankcomm.com/BankCommSite/uploadFiles";
	if(!wcsncmp(strbom, pDispParams->rgvarg[3].pvarVal->bstrVal, wcslen(strbom)))
		return S_OK;
	IDispatch* pDisp = pDispParams->rgvarg[4].pdispVal ;
	if (pDisp == NULL)
		return S_OK;
	if(pDispParams->rgvarg[2].pvarVal->vt == VT_BSTR)
	{
		if(pDispParams->rgvarg[2].pvarVal->bstrVal != NULL && wcslen(pDispParams->rgvarg[2].pvarVal->bstrVal) >= 9)
		{
			if((!wcsncmp(L"newwindow", pDispParams->rgvarg[2].pvarVal->bstrVal, 9)) || (!wcsncmp(L"loginFrame", pDispParams->rgvarg[2].pvarVal->bstrVal, 10)) ||
				(!wcsncmp(L"LoginFrame", pDispParams->rgvarg[2].pvarVal->bstrVal, 10)))// 对于建立新页面的操作，允许
				return S_OK;
		}
	}
	
	// 在记账过程中出现错误，那么通知用户出错。还要有后续处理
	// 关闭当前页还是打开新页都需要考虑
	if(m_isGetBill)
		if(m_step > 1)
			return S_OK;

	// [TuotuoXP] 更改错误页面
	CComPtr<IWebBrowser2> spWebBrowser2;
	HRESULT hr = pDisp->QueryInterface(IID_IWebBrowser2, (void**)&spWebBrowser2);
	if (SUCCEEDED(hr) && spWebBrowser2)
	{
		CComPtr<IDispatch> spDispDoc;
		hr = spWebBrowser2->get_Document(&spDispDoc);
		if (SUCCEEDED(hr) && spDispDoc)
		{
			CComPtr<IHTMLDocument2> spHTMLDocument2;
			hr = spDispDoc->QueryInterface(IID_IHTMLDocument2, (void**)&spHTMLDocument2);
			if (SUCCEEDED(hr) && spHTMLDocument2)
			{
				TCHAR szPath[MAX_PATH];
				_stprintf_s(szPath, _T("%s\\Html\\Error\\ErrorPage.html"), ::GetModulePath());
				std::fstream fs;
				fs.open(szPath, std::ios::in);
				if (fs.good())
				{
					std::stringstream ss;
					ss << fs.rdbuf();
					std::string strContent = ss.str();

					size_t n = strContent.find("$ERRCODE2$");
					if (n != std::string::npos)
					{
						std::stringstream ssnum;
						ssnum << (int)pDispParams->rgvarg[1].pvarVal->uiVal;
						std::wstring url(pDispParams->rgvarg[3].pvarVal->bstrVal);
						std::string bkinfo;
						bkinfo = "URL:" + ws2s(url) + ";";
						bkinfo += "Error:" + ssnum.str() + ";";
						bkinfo += "Filter:" + GetFilterFile() + ";";

						CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, CRecordProgram::GetInstance()->GetRecordInfo(L"NavigateError:%s",bkinfo.c_str()));
						strContent = strContent.replace(n, 10, ssnum.str());
					}

					n = strContent.find("error.png");
					if (n != std::string::npos)
					{
						_stprintf_s(szPath, _T("file:///%s\\Html\\Error\\error.png"), ::GetModulePath());
						strContent = strContent.replace(n, 9, WToA(szPath));
					}

					SAFEARRAY *psaStrings = SafeArrayCreateVector(VT_VARIANT, 0, 1);
					VARIANT *param;
					HRESULT hr = SafeArrayAccessData(psaStrings, (LPVOID*)&param);
					param->vt = VT_BSTR;
					param->bstrVal = ::SysAllocString(AToW(strContent.c_str()).c_str());

					hr = SafeArrayUnaccessData(psaStrings);
					hr = spHTMLDocument2->write(psaStrings);
					if (psaStrings != NULL) 
						SafeArrayDestroy(psaStrings);

					*pDispParams->rgvarg[0].pboolVal = VARIANT_TRUE;
					m_error = true;
				}
			}
		}
	}

	return S_OK ;
}
std::string CWebBrowserEventsManager::GetFilterFile()
{
	TCHAR szPath[MAX_PATH] = { 0 };

	::GetModuleFileName(NULL, szPath, _countof(szPath));
	TCHAR *p = _tcsrchr(szPath, '\\');
	if (p)
		*p = 0;

	std::wstring wcsPath(szPath);
	wcsPath += L"\\syslog.txt";
	std::string info;
	DWORD dwReadNum;
	HANDLE hFile;

	if( hFile = CreateFileW(wcsPath.c_str(),GENERIC_READ,FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL))
	{
		if(hFile == INVALID_HANDLE_VALUE)
			return info;

		const int bufsize = 2 * 1024 * 1024;
		wchar_t* buf = new wchar_t[bufsize];
		if(buf == NULL)
			return info;
		DWORD fsize = bufsize * sizeof(wchar_t);

		if(ReadFile(hFile,buf,fsize,&dwReadNum,NULL))
		{
			wchar_t *p = buf;
			p = wcsstr(buf,L"Fileter Module :");//寻找第一个
			while(p)
			{
				wchar_t *q = wcsstr(p,L"\r\n");

				wchar_t allstr[255] = {0};
				memcpy(allstr,p,(q - p)*2);
				std::wstring tmp(allstr);

				info += "  " + ws2s(tmp);
				p = wcsstr(q,L"Fileter Module :");
			}
		}
		delete[] buf;

		::CloseHandle(hFile);
	}

	return info;
}

std::string CWebBrowserEventsManager::ws2s(const std::wstring& ws)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, NULL, 0, NULL, NULL);
	if (nLen<= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen -1] = 0;
	std::string strTemp(pszDst);
	delete [] pszDst;
	return strTemp;
}

HRESULT CWebBrowserEventsManager::OnSetSecureLockIcon(REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if (pDispParams->cArgs < 1)
		return E_FAIL ;

	if(m_isGetBill)
		if(m_step > 1)
			return S_OK;

	BOOL bSecure = (pDispParams->rgvarg[0].lVal >= secureLockIconMixed) ? TRUE : FALSE;
	::PostMessage(m_pAxControl->m_hChildFrame, WM_ITEM_SET_SSL_STATE, 0, (LPARAM)bSecure);

	return S_OK;
}

BOOL CWebBrowserEventsManager::IsToppestWindowOfBrowser2(IDispatch* pDispatch)
{
	if (!pDispatch || !m_pWebBrowser2)
	{
		return FALSE ;
	}

	CComPtr<IUnknown> pUnk;
	CComPtr<IDispatch> pWBDisp;

	if (FAILED(m_pWebBrowser2->QueryInterface(IID_IUnknown, (void**)&pUnk)) || !pUnk)
	{
		return FALSE ;
	}

	if (FAILED(pUnk->QueryInterface(IID_IDispatch, (void**)&pWBDisp)) || !pWBDisp)
	{
		return FALSE ;
	}

	if (pDispatch == pWBDisp)
	{
		return TRUE ;
	}

	return FALSE ;
}

BOOL CWebBrowserEventsManager::IsToppestWindowOfBrowser(IDispatch* pDispatch)
{
	if (!pDispatch)
	{
		return FALSE ;
	}

	HRESULT hr = S_OK ;
	CComPtr<IWebBrowser2> spWebBrowser2 ;
	hr = pDispatch->QueryInterface(IID_IWebBrowser2, (void**)&spWebBrowser2) ;
	if (FAILED(hr) || !spWebBrowser2)
	{
		return FALSE ;
	}
	CComPtr<IDispatch> spDispDoc ;
	hr = spWebBrowser2->get_Document(&spDispDoc) ;
	if (FAILED(hr) || !spDispDoc)
	{
		return FALSE ;
	}
	CComPtr<IHTMLDocument2> spHTMLDocument2 ;
	hr = spDispDoc->QueryInterface(IID_IHTMLDocument2, (void**)&spHTMLDocument2) ;
	if (FAILED(hr) || !spHTMLDocument2)
	{
		return FALSE ;
	}
	CComPtr<IHTMLWindow2> spHTMLWindow2 ;
	hr = spHTMLDocument2->get_parentWindow(&spHTMLWindow2) ;
	if (FAILED(hr) || !spHTMLWindow2)
	{
		return FALSE ;
	}

	CComPtr<IHTMLWindow2> spTopWindow2 ;
	hr = spHTMLWindow2->get_top(&spTopWindow2) ;

	if (FAILED(hr) || !spHTMLWindow2)
	{
		return FALSE ;
	}
	//////////////////////////////////////////////////////////////////////////
	CComPtr<IObjectIdentity> spIdentity ;
	hr = spHTMLWindow2->QueryInterface(IID_IObjectIdentity, (void**)&spIdentity) ;
	if (FAILED(hr) || !spHTMLWindow2)
	{
		return FALSE ;
	}
	hr = spIdentity->IsEqualObject(spTopWindow2) ;
	if (S_OK == hr)
	{
		m_dwHTMLWindowBaseAddr = *(DWORD*)spHTMLWindow2.p ;
		return TRUE ;
	}

	return FALSE ;
}

BOOL CWebBrowserEventsManager::IsValidHttpUrl(const BSTR bstrUrl)
{
	CString strUrl(bstrUrl);
	strUrl.MakeLower();
	if (strUrl.Left(5) == _T("http:") || strUrl.Left(6) == _T("https:"))
	{
		return TRUE;
	}

	return FALSE;
}
