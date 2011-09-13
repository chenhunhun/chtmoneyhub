
#pragma once

class CAxControl;

class CInternetSecurityManagerImpl : public IInternetSecurityManager
{
public:
	CInternetSecurityManagerImpl(CAxControl *pAxControl);
	~CInternetSecurityManagerImpl();

public:
	static bool Initialize();
	static void Uninitialize();

public:
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IInternetSecurityManager
	STDMETHOD(SetSecuritySite)(IInternetSecurityMgrSite *pSite);
	STDMETHOD(GetSecuritySite)(IInternetSecurityMgrSite **ppSite);
	STDMETHOD(MapUrlToZone)(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags);
	STDMETHOD(GetSecurityId)(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved);
	STDMETHOD(ProcessUrlAction)(LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved);
	STDMETHOD(QueryCustomPolicy)(LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved);
	STDMETHOD(SetZoneMapping)(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags);
	STDMETHOD(GetZoneMappings)(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags);

private:
	CAxControl* m_pAxControl;
	static std::vector<std::wstring> _vecRestricted;
};
