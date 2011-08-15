
#include "stdafx.h"
#include "CustomSite.h"
#include "WebBrowserCore.h"
#include "AxControl.h"

#include "ExternalDispatchImpl.h"
#include "InternetSecurityManagerImpl.h"
#include "DownloadManagerImpl.h"

CCustomSite::CCustomSite(CAxControl *pAxControl) : m_pAxControl(pAxControl)
{
	m_pExternalDispatchImpl = new CExternalDispatchImpl(m_pAxControl);
	m_pExternalDispatchImpl->AddRef();

	m_pInternetSecurityManager = new CInternetSecurityManagerImpl(m_pAxControl);
	m_pInternetSecurityManager->AddRef();
}

CCustomSite::~CCustomSite()
{
	delete m_pExternalDispatchImpl;
	m_pExternalDispatchImpl = NULL;

	delete m_pInternetSecurityManager;
	m_pInternetSecurityManager = NULL;
}

// IUnknown

ULONG CCustomSite::AddRef()
{
	return 1;
}

ULONG CCustomSite::Release()
{
	return 1;
}

HRESULT CCustomSite::QueryInterface(const IID &riid, void **ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL ;

	if (riid == IID_IUnknown || riid == IID_IOleClientSite)
		*ppvObject = (IOleClientSite*)this; 
	else if (riid == IID_IOleCommandTarget) 
		*ppvObject = (IOleCommandTarget*)this; 
	else if (riid == IID_IDocHostUIHandler || riid == IID_IDocHostUIHandler2) 
		*ppvObject = (IDocHostUIHandler2*)this; 
	else if (riid == IID_IDocHostShowUI)
		*ppvObject = (IDocHostShowUI*)this;
	else if (riid == IID_IServiceProvider)
		*ppvObject = (IServiceProvider*)this ;
	else if (riid == IID_IOleInPlaceSite)
		*ppvObject = (IOleInPlaceSite*)this; 

	if (*ppvObject != NULL)
	{
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE ;
}

// IOleClientSite

HRESULT CCustomSite::SaveObject(void)
{
	ATLASSERT(0);

	return E_NOTIMPL ;
}

HRESULT CCustomSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	ATLASSERT(0);

	return E_NOTIMPL ;
}

HRESULT CCustomSite::GetContainer(IOleContainer **ppContainer)
{
	*ppContainer = NULL;

	return E_NOINTERFACE;
}

HRESULT CCustomSite::ShowObject(void)
{
	return NOERROR;
}

HRESULT CCustomSite::OnShowWindow(BOOL fShow)
{
	ATLASSERT(0);

	return E_NOTIMPL ;
}

HRESULT CCustomSite::RequestNewObjectLayout(void)
{
	ATLASSERT(0);

	return E_NOTIMPL ;
}


// IOleCommandTarget

HRESULT CCustomSite::QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT *pCmdText)
{
	return E_NOTIMPL ;
}

HRESULT CCustomSite::Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
{
	HRESULT hr = OLECMDERR_E_UNKNOWNGROUP ;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{
		switch (nCmdID) 
		{
		case OLECMDID_SHOWSCRIPTERROR: // 屏蔽脚本错误的对话框
			{
				pvaOut->vt = VT_BOOL ;
				pvaOut->boolVal = VARIANT_TRUE ;

				return S_OK ;
			}
			break ;

		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}

	return hr;
}


//IDocHostUIHandler

HRESULT CCustomSite::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return S_OK;
}

HRESULT CCustomSite::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO) ;
	pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE;
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT ;

	return S_OK ;
}

HRESULT CCustomSite::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	return S_OK ;
}

HRESULT CCustomSite::HideUI(void)
{
	return S_OK ;
}

HRESULT CCustomSite::UpdateUI(void)
{
	return S_OK ;
}

HRESULT CCustomSite::EnableModeless(BOOL fEnable)
{
	return S_OK;
}

HRESULT CCustomSite::OnDocWindowActivate(BOOL fActivate) 
{
	return S_OK;
}

HRESULT CCustomSite::OnFrameWindowActivate(BOOL fActivate)
{
	return S_OK;
}

HRESULT CCustomSite::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	return S_OK;
}

HRESULT CCustomSite::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return S_FALSE;
}

HRESULT CCustomSite::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return S_FALSE;
}

HRESULT CCustomSite::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return S_FALSE;
}

HRESULT CCustomSite::GetExternal(IDispatch **ppDispatch)
{
	if (m_pExternalDispatchImpl)
	{
		m_pExternalDispatchImpl->AddRef() ;
		*ppDispatch = m_pExternalDispatchImpl ;

		return S_OK ;
	}

	return E_NOTIMPL ;
}

HRESULT CCustomSite::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	*ppchURLOut = NULL;

	return S_FALSE;
}

HRESULT CCustomSite::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet) 
{
	*ppDORet = NULL;

	return S_FALSE;
}


// IDocHostUIHandler2

HRESULT CCustomSite::GetOverrideKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return E_NOTIMPL;
}


// IDocHostShowUI
HRESULT CCustomSite::ShowMessage(HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT *plResult)
{
	if(m_pAxControl->m_isGetBill)  // 屏蔽info对话框		*plResult = IDOK;	else    	*plResult = ::MessageBox(hwnd, lpstrText, L"财金汇", dwType);

	return S_OK;
}

HRESULT CCustomSite::ShowHelp(HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand, DWORD dwData, POINT ptMouse, IDispatch *pDispatchObjectHit)
{
	return E_NOTIMPL;
}


// IServiceProvider

HRESULT CCustomSite::QueryService(REFGUID guidService, REFIID riid, void **ppvObject)
{
	if (!ppvObject)
		return E_POINTER;

	if (guidService == SID_SInternetSecurityManager)
	{
		if (m_pInternetSecurityManager)
		{
			m_pInternetSecurityManager->AddRef();
			*ppvObject = m_pInternetSecurityManager;

			return S_OK ;
		}
	}
	else if (guidService == SID_SDownloadManager)
	{
		HWND hCloseWnd = NULL;
		CComPtr<IDispatch> pDispatch;
		if (SUCCEEDED(m_pAxControl->m_pCore->m_pWebBrowser2->get_Document(&pDispatch)))
		{
			CComPtr<IHTMLDocument2> pDoc2;
			if (SUCCEEDED(pDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2)))
			{
				CComPtr<IHTMLElement> pBody;   
				if (SUCCEEDED(pDoc2->get_body(&pBody)))
				{
					CComBSTR bstr;
					pBody->get_innerHTML(&bstr);

					if (bstr.Length() == 0)
						hCloseWnd = m_pAxControl->m_hChildFrame;
				}
			}
		}

		CDownloadManagerImpl* pDownloadManager = new CDownloadManagerImpl(m_pAxControl->m_hChildFrame);
		pDownloadManager->SetCloseWnd(hCloseWnd);
		return pDownloadManager->QueryInterface(IID_IDownloadManager, ppvObject);
	}

	*ppvObject = NULL;
	return E_NOINTERFACE;
}


// IOleWindow

HRESULT CCustomSite::GetWindow(HWND *phwnd)
{
	*phwnd = m_pAxControl->m_hWnd;		// 返回包含这个browser object的窗口

	return S_OK;
}

HRESULT CCustomSite::ContextSensitiveHelp(BOOL fEnterMode)
{ 
	ATLASSERT(0);

	return E_NOTIMPL;
}


// IOleInPlaceSite : IOleWindow

HRESULT CCustomSite::CanInPlaceActivate()
{
	return S_OK;
}

HRESULT CCustomSite::OnInPlaceActivate()
{
	return S_OK;
}

HRESULT CCustomSite::OnUIActivate()
{
	return S_OK;
}

HRESULT CCustomSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*ppDoc = 0;

	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = m_pAxControl->m_hWnd;
	lpFrameInfo->haccel = 0;
	lpFrameInfo->cAccelEntries = 0;

	return S_OK;
}

HRESULT CCustomSite::Scroll(SIZE scrollExtant)
{ 
	ATLASSERT(0);

	return E_NOTIMPL;
}

HRESULT CCustomSite::OnUIDeactivate(BOOL fUndoable)
{
	return S_OK;
}

HRESULT CCustomSite::OnInPlaceDeactivate()
{
	return S_OK;
}

HRESULT CCustomSite::DiscardUndoState()
{
	ATLASSERT(0);

	return E_NOTIMPL;
}

HRESULT CCustomSite::DeactivateAndUndo()
{
	ATLASSERT(0);

	return E_NOTIMPL;
}

HRESULT CCustomSite::OnPosRectChange(LPCRECT lprcPosRect)
{
	CComPtr<IOleInPlaceObject> spOleInPlaceObject;
	if (SUCCEEDED(m_pAxControl->m_pCore->m_pOleObject->QueryInterface(IID_IOleInPlaceObject, (void**)&spOleInPlaceObject)))
		spOleInPlaceObject->SetObjectRects(lprcPosRect, lprcPosRect);

	return S_OK;
}


// IOleInPlaceUIWindow

HRESULT CCustomSite::GetBorder(LPRECT lprectBorder)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CCustomSite::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CCustomSite::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CCustomSite::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
	return S_OK;
}


// IOleInPlaceFrame
HRESULT CCustomSite::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CCustomSite::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	return S_OK;
}

HRESULT CCustomSite::RemoveMenus(HMENU hmenuShared)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}

HRESULT CCustomSite::SetStatusText(LPCOLESTR pszStatusText)
{
	return S_OK;
}

HRESULT CCustomSite::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	ATLASSERT(0);
	return E_NOTIMPL;
}
