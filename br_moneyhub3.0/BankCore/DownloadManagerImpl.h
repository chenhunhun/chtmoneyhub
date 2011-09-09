
#pragma once

#include <vector>

class CBindStatusCallback;
class CAsyncDownloadHelper;
class CDownloadManagerImpl;

//////////////////////////////////////////////////////////////////////////
// Register messages

extern UINT WM_URLDOWNLOADFROMURL_STATUS;
extern UINT WM_URLDOWNLOADFROMURL_FINISH;
extern UINT WM_URLDOWNLOADFROMURL_FAILED;


//////////////////////////////////////////////////////////////////////////
// Progress struct

typedef struct  
{
	ULONG lProgress;
	ULONG lProgressMax;

} ProgStruct;

//////////////////////////////////////////////////////////////////////////
// Thread data struct

typedef struct  
{
	HWND hClient;
	HANDLE hCreateEvent;
	CString strFileName;
	CString strUrl;
	BOOL bHasContentLength;

	// out
	HWND hProgDialog;

} ThreadDataStruct;

//////////////////////////////////////////////////////////////////////////
// IDownloadManager GUID & Interface

DEFINE_GUID(IID_IDownloadManager, 0x988934a4, 0x064b, 0x11d3, 0xbb, 0x80, 0x0, 0x10, 0x4b, 0x35, 0xe7, 0xf9);
#define SID_SDownloadManager IID_IDownloadManager    

MIDL_INTERFACE("988934A4-064B-11D3-BB80-00104B35E7F9")
IDownloadManager : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Download(IMoniker *pmk, IBindCtx *pbc, DWORD dwBindVerb, LONG grfBINDF,
		BINDINFO *pBindInfo, LPCOLESTR pszHeaders, LPCOLESTR pszRedir, UINT uiCP) = 0;
};

//////////////////////////////////////////////////////////////////////////
// CAsyncDownloadHelper

class CAsyncDownloadHelper
{
public:
	CAsyncDownloadHelper(HWND hClient, HWND hCloseWnd);

public:
	STDMETHOD(BeforeNavigate2)(IDispatch *, VARIANT *URL, VARIANT *Flags, VARIANT *TargetFrameName,
		VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *bCancel);

public:
	static bool IsDownloadUrl(std::string strUrl, bool& bIsFtp, std::string& strFileName, std::string& strExt);
	static bool IsFilterScheme(std::string strScheme);
	static bool GetFileNameFromUrl(const std::string& strUrl, bool bIsFTP, char* pszBuf, unsigned int uSize);
		
protected:
	HWND m_hClient;
	HWND m_hCloseWnd;
};

//////////////////////////////////////////////////////////////////////////
// CBindStatusCallback

MIDL_INTERFACE("83BC707F-5286-4659-A1A5-82FC34865894")
IMoneyhubDownload : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE InitByClient(BSTR lpUrl, HWND hClient, HWND hCloseWnd) = 0;
	virtual HRESULT STDMETHODCALLTYPE InitByUser(HWND hClient, HWND hCloseWnd) = 0;
	virtual HRESULT STDMETHODCALLTYPE CancelDownload() = 0;
	virtual HRESULT STDMETHODCALLTYPE IsDownloading(VARIANT_BOOL* pbResult) = 0;
	virtual HRESULT STDMETHODCALLTYPE IsContentLength(VARIANT_BOOL* pbResult) = 0;
	virtual HRESULT STDMETHODCALLTYPE HoldDefaultBSCB(IBindStatusCallback* pBSCB) = 0;
	virtual HRESULT STDMETHODCALLTYPE HoldDefaultBC(IBindCtx* pBC) = 0;
	virtual HRESULT STDMETHODCALLTYPE HoldDefaultStream(IStream* pStream) = 0;
};

MIDL_INTERFACE("F1EE5C4D-EDF4-4bc5-9C7F-93E0282A4A3A")
IMoneyhubEventsSink : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE OnFileDownloadEx(BSTR sURL, BSTR sFilename, BSTR sExt, BSTR sExtraHeaders, BSTR sRedirURL, VARIANT_BOOL *SendProgressEvents, VARIANT_BOOL *bStopDownload, BSTR *sPathToSave) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnFileDLProgress(BSTR sURL, ULONG lProgress, ULONG lProgressMax, VARIANT_BOOL *CancelDl) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnFileDLEndDownload(BSTR sURL, BSTR sSavedFilePath, VARIANT_BOOL* pbOpenFile) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnFileDLDownloadError(BSTR sURL, BSTR sErrorMsg, BSTR sSavedFilePath) = 0;
};

class CBindStatusCallback : public IMoneyhubDownload, public IMoneyhubEventsSink, public IBindStatusCallback, public IHttpNegotiate
{
public:
	CBindStatusCallback();
	~CBindStatusCallback();

public:
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IMoneyhubControl
	STDMETHOD(InitByClient)(BSTR lpUrl, HWND hClient, HWND hCloseWnd);
	STDMETHOD(InitByUser)(HWND hClient, HWND hCloseWnd);
	STDMETHOD(CancelDownload)();
	STDMETHOD(IsDownloading)(VARIANT_BOOL* pbResult);
	STDMETHOD(IsContentLength)(VARIANT_BOOL* pbResult);
	STDMETHOD(HoldDefaultBSCB)(IBindStatusCallback* pBSCB);
	STDMETHOD(HoldDefaultBC)(IBindCtx* pBC);
	STDMETHOD(HoldDefaultStream)(IStream* pStream);

	// IMoneyhubEventsSink
	STDMETHOD(OnFileDownloadEx)(BSTR sURL, BSTR sFilename, BSTR sExt, BSTR sExtraHeaders, BSTR sRedirURL, VARIANT_BOOL *SendProgressEvents, VARIANT_BOOL *bStopDownload, BSTR *sPathToSave);
	STDMETHOD(OnFileDLProgress)(BSTR sURL, ULONG lProgress, ULONG lProgressMax, VARIANT_BOOL* CancelDl);
	STDMETHOD(OnFileDLEndDownload)(BSTR sURL, BSTR sSavedFilePath, VARIANT_BOOL* pbOpenFile);
	STDMETHOD(OnFileDLDownloadError)(BSTR sURL, BSTR sErrorMsg, BSTR sSavedFilePath);

	//IBindStatusCallback
	STDMETHOD(OnStartBinding)(DWORD dwReserved, IBinding* pib);
	STDMETHOD(GetPriority)(LONG* pnPriority);
	STDMETHOD(OnLowResource)(DWORD reserved);
	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText);
	STDMETHOD(OnStopBinding)(HRESULT hresult,LPCWSTR szError);
	STDMETHOD(GetBindInfo)(DWORD *grfBINDF, BINDINFO * pbindinfo);
	STDMETHOD(OnDataAvailable)(DWORD grfBSCF, DWORD dwSize, FORMATETC* pformatetc, STGMEDIUM* pstgmed);
	STDMETHOD(OnObjectAvailable)(REFIID riid,IUnknown* punk);

	// IHttpNegotiate methods
	STDMETHOD(BeginningTransaction)(LPCWSTR szURL, LPCWSTR szHeaders, DWORD dwReserved, LPWSTR *pszAdditionalHeaders);
	STDMETHOD(OnResponse)(DWORD dwResponseCode, LPCWSTR szResponseHeaders, LPCWSTR szRequestHeaders, LPWSTR *pszAdditionalRequestHeaders);

protected:
	void ReleaseInterfaces();

protected:
	LONG	m_cRef;

public:
	IBindStatusCallback *m_pPrevBSCB;
	IBindCtx			*m_pBindCtx;
	IBinding			*m_pBinding;
	IStream				*m_pStream;
	IStream				*m_pStreamBind;

protected:
	HWND		m_hClient;
	HWND		m_hProgDialog;
	CComBSTR	m_fFileName;
	CComBSTR	m_fFileExt;
	CComBSTR	m_fUrl;
	CComBSTR	m_fFullSavePath;
	DWORD		m_cbOldSize;
	HANDLE		m_hFile;
	BOOL		m_bContentLength;
	HWND		m_hCloseWnd;
	BOOL		m_bCanceled;
	BOOL		m_bFailed;
	BOOL		m_bOpenFile;
};


//////////////////////////////////////////////////////////////////////////
// CDownloadManagerImpl

class CDownloadManagerImpl : public IDownloadManager, public CComMultiThreadModel
{
public:
	CDownloadManagerImpl(HWND hClient);
	~CDownloadManagerImpl();

public:
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDownloadManager
	STDMETHOD(Download)(IMoniker *pmk, IBindCtx *pbc, DWORD dwBindVerb, LONG grfBINDF, BINDINFO *pBindInfo, LPCOLESTR pszHeaders, LPCOLESTR pszRedir, UINT uiCP);

public:
	BOOL DownloadUrlAsync(LPCTSTR pszUrl);
	void SetCloseWnd(HWND hWnd);

private:
	LONG	m_cRef;
	HWND	m_hClient;
	HWND	m_hCloseWnd;
};
