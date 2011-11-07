
#include "stdafx.h"
#include "InternetSecurityManagerImpl.h"

std::vector<std::wstring> CInternetSecurityManagerImpl::_vecRestricted;

// IUnknown

CInternetSecurityManagerImpl::CInternetSecurityManagerImpl(CAxControl *pAxControl)
: m_pAxControl(pAxControl)
{

}

CInternetSecurityManagerImpl::~CInternetSecurityManagerImpl()
{

}

bool CInternetSecurityManagerImpl::Initialize()
{
	// 去掉注册表的受限站点
	/*IInternetSecurityManager *pSecurityMgr = NULL;
	HRESULT hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, 
		IID_IInternetSecurityManager, (void**)&pSecurityMgr);
	if (FAILED(hr) || pSecurityMgr == NULL)
		return false;

	IEnumString* pEnumString = NULL;
	hr = pSecurityMgr->GetZoneMappings(URLZONE_UNTRUSTED, &pEnumString, 0);
	if (FAILED(hr) || pEnumString == NULL)
		return false;

	LPOLESTR pszName = NULL; 
	ULONG count = 0; 
	while ((hr = pEnumString->Next(1, &pszName, &count)) == S_OK) 
		_vecRestricted.push_back(pszName);	
	pEnumString->Release(); 

	for (std::vector<std::wstring>::const_iterator it = _vecRestricted.begin();
		it != _vecRestricted.end(); ++it)
		pSecurityMgr->SetZoneMapping(URLZONE_UNTRUSTED, it->c_str(), SZM_DELETE);

	pSecurityMgr->Release();*/

	return true;
}

void CInternetSecurityManagerImpl::Uninitialize()
{
/*
	IInternetSecurityManager *pSecurityMgr = NULL;
	HRESULT hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, 
		IID_IInternetSecurityManager, (void**)&pSecurityMgr);
	if (FAILED(hr) || pSecurityMgr == NULL)
		return;

	for (std::vector<std::wstring>::const_iterator it = _vecRestricted.begin();
		it != _vecRestricted.end(); ++it)
		pSecurityMgr->SetZoneMapping(URLZONE_UNTRUSTED, it->c_str(), SZM_CREATE);

	pSecurityMgr->Release();
*/
}

// IUnknown

STDMETHODIMP CInternetSecurityManagerImpl::QueryInterface(REFIID riid, void **ppvObject)
{
	if (!ppvObject)
		return E_POINTER ;

	*ppvObject = NULL;

	if (riid == IID_IUnknown)
		*ppvObject = (IUnknown*)this;
	else if (riid == IID_IInternetSecurityManager)
		*ppvObject = (IInternetSecurityManager*)this ;

	if (*ppvObject != NULL)
	{
		AddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}

STDMETHODIMP_(ULONG) CInternetSecurityManagerImpl::AddRef()
{
	return 1;
}

STDMETHODIMP_(ULONG) CInternetSecurityManagerImpl::Release()
{
	return 1;
}


// IInternetSecurityManager

STDMETHODIMP CInternetSecurityManagerImpl::SetSecuritySite(IInternetSecurityMgrSite *pSite)
{
	return INET_E_DEFAULT_ACTION; 
}

STDMETHODIMP CInternetSecurityManagerImpl::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{
	return INET_E_DEFAULT_ACTION; 
}

STDMETHODIMP CInternetSecurityManagerImpl::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags)
{
	*pdwZone = 3;
	return S_OK;
}

STDMETHODIMP CInternetSecurityManagerImpl::GetSecurityId(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved)
{
/*
	static const BYTE wszFileSecId[] = "http:example.com\3\0\0\0";

	if (*pcbSecurityId < sizeof(wszFileSecId)) {
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	}

	memcpy(pbSecurityId, wszFileSecId, sizeof(wszFileSecId));
	*pcbSecurityId = sizeof(wszFileSecId);
	return S_OK;
*/

	return INET_E_DEFAULT_ACTION; 
}

STDMETHODIMP CInternetSecurityManagerImpl::ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
{
	*pPolicy = URLPOLICY_ALLOW;
	return S_OK;

/*
	if((dwAction >= URLACTION_DOWNLOAD_MIN) && (dwAction <= URLACTION_DOWNLOAD_MAX))
		*pPolicy = URLPOLICY_ALLOW;
	else if((dwAction >= URLACTION_ACTIVEX_MIN) && (dwAction <= URLACTION_ACTIVEX_MAX))
		*pPolicy = URLPOLICY_ALLOW;
	else if((dwAction >= URLACTION_SCRIPT_MIN) && (dwAction <= URLACTION_SCRIPT_MAX))
		*pPolicy = URLPOLICY_ALLOW;
	else if((dwAction >= URLACTION_HTML_MIN) && (dwAction <= URLACTION_HTML_MAX))
		*pPolicy = URLPOLICY_ALLOW;
	else if((dwAction >= URLACTION_NETWORK_MIN) && (dwAction <= URLACTION_NETWORK_MAX))
		*pPolicy = URLPOLICY_ALLOW;
	else if((dwAction >= URLACTION_JAVA_MIN) && (dwAction <= URLACTION_JAVA_MAX))
		*pPolicy = URLPOLICY_ALLOW;
	else
		return INET_E_DEFAULT_ACTION;

	return S_OK;
*/
}

STDMETHODIMP CInternetSecurityManagerImpl::QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION; 
}

STDMETHODIMP CInternetSecurityManagerImpl::SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION; 
}

STDMETHODIMP CInternetSecurityManagerImpl::GetZoneMappings(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION; 
}