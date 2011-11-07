
#include "stdafx.h"
#include "DownloadManagerImpl.h"
#include "AxControl.h"

#include "..\Utils\PostData\UrlCrack.h"
#include "..\Utils\ListManager\ListManager.h"

#include "Downloader\DownloadOptionDlg.h"
#include "Downloader\DownloadProgressDlg.h"
#include "Downloader\UrlParts.h"
#include <string>
using namespace std;
//////////////////////////////////////////////////////////////////////////
// Events

UINT WM_URLDOWNLOADFROMURL_STATUS = RegisterWindowMessage(_T("WM_URLDOWNLOADFROMURL_STATUS"));
UINT WM_URLDOWNLOADFROMURL_FINISH = RegisterWindowMessage(_T("WM_URLDOWNLOADFROMURL_FINISH"));
UINT WM_URLDOWNLOADFROMURL_FAILED = RegisterWindowMessage(_T("WM_URLDOWNLOADFROMURL_FAILED"));

unsigned __stdcall _threadDownloadProxy(void* pData)
{
	ThreadDataStruct* pTds = (ThreadDataStruct *)pData;
	HWND hClient = pTds->hClient;

	CDownloadProgressDlg dlg(pTds);
	dlg.DoModal(hClient);

	_endthreadex(0);

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// CAsyncDownloadHelper

CAsyncDownloadHelper::CAsyncDownloadHelper(HWND hClient, HWND hCloseWnd)
: m_hClient(hClient), m_hCloseWnd(hCloseWnd)
{
}

STDMETHODIMP CAsyncDownloadHelper::BeforeNavigate2(IDispatch *pDispatch, VARIANT *URL, VARIANT *Flags, VARIANT *TargetFrameName,
												   VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *bCancel)
{
	USES_CONVERSION;

	std::string strUrl = OLE2A(URL->bstrVal);
	bool bIsFtp;
	std::string strFileName;
	std::string strExt;

	if (!IsDownloadUrl(strUrl, bIsFtp, strFileName, strExt))
		return E_FAIL;

	CDownloadManagerImpl pDownloadMgr(m_hClient);
	pDownloadMgr.SetCloseWnd(m_hCloseWnd);
	pDownloadMgr.DownloadUrlAsync(OLE2CT(URL->bstrVal));

	*bCancel = VARIANT_TRUE;

	return S_OK;
}

bool CAsyncDownloadHelper::IsDownloadUrl(std::string strUrl, bool& bIsFtp, std::string& strFileName, std::string& strExt)
{
	USES_CONVERSION;

	CUrlCrack urlcrack;
	if (!urlcrack.Crack(A2CT(strUrl.c_str())))
		return false;

	std::string strScheme = CT2A(urlcrack.GetScheme());
	if (!IsFilterScheme(strScheme))
		return false;

	char szFileName[MAX_PATH + 1];
	bIsFtp = _stricmp(strScheme.c_str(), "ftp") == 0;
	if (!GetFileNameFromUrl(strUrl, bIsFtp, szFileName, MAX_PATH))
		return false;

	strFileName = szFileName;

	char* pszExt = szFileName + strlen(szFileName);
	while (pszExt > szFileName)
	{
		if (*pszExt == '.')
		{
			pszExt++;
			break;
		}

		pszExt--;
	}

	if (pszExt == szFileName)
		return false;

	strExt = pszExt;

	if (!CListManager::_()->GetDldFilter()->IsMatch(pszExt))
		return false;

	return true;
}

bool CAsyncDownloadHelper::IsFilterScheme(std::string strScheme)
{
	return _stricmp(strScheme.c_str(), "http") == 0 || 
		_stricmp(strScheme.c_str(), "ftp") == 0 || 
		_stricmp(strScheme.c_str(), "https") == 0;
}

bool CAsyncDownloadHelper::GetFileNameFromUrl(const std::string& strUrl, bool bIsFTP, char* pszBuf, unsigned int uSize)
{
	*pszBuf = 0;

	int len = strUrl.size();
	if (len == 0)
		return false;

	int end = bIsFTP ? (len - 1) : (strcspn(strUrl.c_str(), "?=#") - 1);
	if (end < 0)
		return false;

	if (end >= len)
		end = len - 1;

	int pos = end;

	while (pos && strUrl[pos] != '/' && strUrl[pos] != '\\')
		pos--;

	if (strUrl[pos] != '/' && strUrl[pos] != '\\')
		return false;

	if ((unsigned int)(len - pos) > uSize)
		return false;

	strcpy_s(pszBuf, uSize, strUrl.c_str() + pos + 1);
	pszBuf[end - pos] = 0;

	if (strlen(pszBuf) == 0)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////
//CDownloadProxy
/////////////////////////////////////////////////////////////////////

//Constructor
CBindStatusCallback::CBindStatusCallback()
: m_cRef(0)
{
	m_pPrevBSCB = NULL;
	m_pBindCtx = NULL;
	m_pBinding = NULL;
	m_pStream = NULL;
	m_pStreamBind = NULL;

	m_cbOldSize = 0;
	m_hFile = INVALID_HANDLE_VALUE;

	m_hProgDialog = NULL;
	m_bContentLength = TRUE;

	m_bCanceled = FALSE;
	m_bFailed = FALSE;

	m_bOpenFile = FALSE;
}

CBindStatusCallback::~CBindStatusCallback()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
	}

	if (m_bCanceled || m_bFailed)
		DeleteFile(m_fFullSavePath);

	if (m_hCloseWnd)
		::PostMessage(m_hCloseWnd, WM_ITEM_AUTOCLOSE, 0, 0);
}

STDMETHODIMP CBindStatusCallback::QueryInterface(REFIID riid, void ** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;
	
	if (riid == __uuidof(IMoneyhubDownload) || riid == IID_IUnknown)
	{
		*ppvObject = (IMoneyhubDownload*)this;
	}
	else if (riid == __uuidof(IMoneyhubEventsSink))
	{
		*ppvObject = (IMoneyhubEventsSink*)this;
	}
	else if (riid == IID_IBindStatusCallback)
	{
       *ppvObject = (IBindStatusCallback*)this;
	}
	else if (riid == IID_IHttpNegotiate)
	{
		*ppvObject = (IHttpNegotiate*)this;
	}

	if (NULL != *ppvObject)
	{
        AddRef();
        return S_OK;
    }

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CBindStatusCallback::AddRef()
{
	return ++m_cRef;
}

ULONG STDMETHODCALLTYPE CBindStatusCallback::Release()
{
	ULONG nTemp = --m_cRef;
	if (nTemp == 0)
		delete this;

	return nTemp;
}

void CBindStatusCallback::ReleaseInterfaces()
{
	if (m_pBinding)
	{
		m_pBinding->Release();
		m_pBinding = NULL;
	}

	if (m_pStream)
	{
		m_pStream->Release();
		m_pStream = NULL;
	}

	if (m_pStreamBind)
	{
		m_pStreamBind->Release();
		m_pStreamBind = NULL;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	if (m_pPrevBSCB && m_pBindCtx)
	{
		LPOLESTR oParam = L"_BSCB_Holder_";
		m_pBindCtx->RegisterObjectParam(oParam, (IUnknown*)m_pPrevBSCB);

		m_pPrevBSCB->Release();
		m_pPrevBSCB = NULL;

		m_pBindCtx->Release();
		m_pBindCtx = NULL;
	}
}

// IMoneyhubControl
STDMETHODIMP CBindStatusCallback::InitByClient(BSTR lpUrl, HWND hClient, HWND hCloseWnd)
{
	m_fUrl = lpUrl;
	m_hClient = hClient;
	m_hCloseWnd = hCloseWnd;

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::InitByUser(HWND hClient, HWND hCloseWnd)
{
	m_fUrl = _T("");
	m_hClient = hClient;
	m_hCloseWnd = hCloseWnd;

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::CancelDownload()
{
	m_bCanceled = TRUE;

	if (m_pBinding)
		m_pBinding->Abort();

	ReleaseInterfaces();

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::IsDownloading(VARIANT_BOOL* pbResult)
{
	return S_OK;
}

STDMETHODIMP CBindStatusCallback::IsContentLength(VARIANT_BOOL* pbResult)
{
	if (pbResult)
		*pbResult = m_bContentLength ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::HoldDefaultBSCB(IBindStatusCallback* pBSCB)
{
	if (m_pPrevBSCB != NULL)
		m_pPrevBSCB->Release();
	
	m_pPrevBSCB = pBSCB;

	if (pBSCB)
		pBSCB->AddRef();

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::HoldDefaultBC(IBindCtx* pBC)
{
	if (m_pBindCtx != NULL)
		m_pBindCtx->Release();

	m_pBindCtx = pBC;

	if (pBC)
		pBC->AddRef();

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::HoldDefaultStream(IStream* pStream)
{
	if (m_pStreamBind != NULL)
		m_pStreamBind->Release();

	m_pStreamBind = pStream;

	if (pStream)
		pStream->AddRef();

	return S_OK;
}

// IMoneyhubEventsSink
STDMETHODIMP CBindStatusCallback::OnFileDownloadEx(BSTR sURL, BSTR sFilename, BSTR sExt, BSTR sExtraHeaders, BSTR sRedirURL, VARIANT_BOOL *SendProgressEvents, VARIANT_BOOL *bStopDownload, BSTR *sPathToSave)
{
	USES_CONVERSION;
	CString strFileName(OLE2CT(sFilename));
	CString strUrl(OLE2CT(sURL));

	CDownloadOptionDlg dlg(strFileName, strUrl);
	if (IDCANCEL != dlg.DoModal(m_hClient))
	{
		CComBSTR bstr(dlg.m_strFullFilePath);
		bstr.CopyTo(sPathToSave);

		ThreadDataStruct tds;
		tds.hClient = m_hClient;
		tds.hCreateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		tds.strFileName = strFileName;
		tds.strUrl = strUrl;
		
		VARIANT_BOOL vBoolean;
		IsContentLength(&vBoolean);
		tds.bHasContentLength = (vBoolean == VARIANT_TRUE) ? TRUE : FALSE;

		tds.hProgDialog = NULL;

		unsigned int threadid = 0;
		CloseHandle((HANDLE)_beginthreadex(NULL, NULL, _threadDownloadProxy, (void *)&tds, 0, &threadid));
		AtlWaitWithMessageLoop(tds.hCreateEvent);
		CloseHandle(tds.hCreateEvent);

		m_hProgDialog = tds.hProgDialog;
	}
	else
	{
		*bStopDownload = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::OnFileDLProgress(BSTR sURL, ULONG lProgress, ULONG lProgressMax, VARIANT_BOOL* CancelDl)
{
	ProgStruct ps = { lProgress, lProgressMax };
	::SendMessage(m_hProgDialog, WM_URLDOWNLOADFROMURL_STATUS, (WPARAM)&ps, (LPARAM)CancelDl);

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::OnFileDLEndDownload(BSTR sURL, BSTR sSavedFilePath, VARIANT_BOOL* pbOpenFile)
{
	::SendMessage(m_hProgDialog, WM_URLDOWNLOADFROMURL_FINISH, 0, 0);

	if (IDYES == ::MessageBox(m_hClient, _T("文件下载完成，是否打开或运行此文件？"), _T("财金汇"), MB_YESNO | MB_ICONQUESTION))
		*pbOpenFile = VARIANT_TRUE;
	else
		*pbOpenFile = VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::OnFileDLDownloadError(BSTR sURL, BSTR sErrorMsg, BSTR sSavedFilePath)
{
	::SendMessage(m_hProgDialog, WM_URLDOWNLOADFROMURL_FAILED, 0, 0);

	return S_OK;
}

// IBindStatusCallback
STDMETHODIMP CBindStatusCallback::OnStartBinding(DWORD dwReserved, IBinding* pib)
{
	if (m_pBinding)
		m_pBinding->Release();

	m_pBinding = pib;
	if (m_pBinding)
		m_pBinding->AddRef();

	if (m_pPrevBSCB)
		m_pPrevBSCB->OnStopBinding(HTTP_STATUS_OK, NULL);

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::GetPriority(LONG* pnPriority)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBindStatusCallback::OnLowResource(DWORD reserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBindStatusCallback::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
{ 
	USES_CONVERSION;
	VARIANT_BOOL vbCancelDL = VARIANT_FALSE;
	
	if (ulStatusCode == BINDSTATUS_BEGINDOWNLOADDATA)
	{
		CUrlParts parts;
		m_fFullSavePath.Empty();
		m_fFileName.Empty();
		m_fFileExt.Empty();
		m_fUrl.Empty();

		m_fUrl = szStatusText;
		if (parts.SplitUrl(m_fUrl))
		{
			if (parts.GetFileNameLen() > 0)
			{
				m_fFileName = parts.GetFileNameAsBSTR();
			}

			if (parts.GetFileExtensionLen() > 0)
			{
				m_fFileExt =  parts.GetFileExtensionAsBSTR(); 
			}
		}

		parts.ResetBuffers();

		if (m_pPrevBSCB)
		{
			if (m_pBinding)
			{
				CComPtr<IWinInetHttpInfo> pWinInetHttpInfo;
				CComPtr<IBinding> pBinding = m_pBinding;
				HRESULT hr = pBinding->QueryInterface(IID_IWinInetHttpInfo, (void**)&pWinInetHttpInfo);
				if (SUCCEEDED(hr) && pWinInetHttpInfo)
				{
					DWORD size = 0;
					DWORD flags = 0;
					hr = pWinInetHttpInfo->QueryInfo(HTTP_QUERY_CONTENT_DISPOSITION, 0, &size, &flags, 0);
					if (SUCCEEDED(hr))
					{
						LPSTR pbuf = new char[size + 1];
						if (pbuf)
						{
							pbuf[size] = '\0';
							hr = pWinInetHttpInfo->QueryInfo(HTTP_QUERY_CONTENT_DISPOSITION, pbuf, &size, &flags, 0);//不是直接跑到该位置的文件名需要利用该函数获得真正的函数名
							if (SUCCEEDED(hr) && (size > 0) && (pbuf[0] != '\0'))
							{
								string dstr(pbuf);
								size_t dpos = dstr.find("filename");
								if(dpos != string::npos)
								{
									string sdstr = dstr.substr(dpos + 10);
									size_t ddpos = sdstr.find("\";");
									string fname = sdstr.substr(0, ddpos);

									m_fFileName.Empty();
									m_fFileName.Append(fname.c_str());
									m_fFileExt.Empty();

									size_t spos = fname.find_last_of('.');
									if(spos != string::npos)
									{
										string pext = fname.substr(spos + 1);
										m_fFileExt.Append(pext.c_str());
									}
								}
								//int i = size - 1;
								//while (( i > -1) && (pbuf[i] != '='))
								//{
								//	if (pbuf[i] == '.')
								//	{
								//		//LPSTR pext = &pbuf[i];
								//		CStringA pext = &pbuf[i];
								//		pext.TrimRight("\"' ");
								//		m_fFileExt.Empty();
								//		m_fFileExt.Append(pext);
								//	}

								//	--i;
								//}

								//if (i > 0)
								//{
								//	//LPSTR pname = &pbuf[i+1];
								//	CStringA pname = &pbuf[i+1];
								//	pname.Trim("\"' ");
								//	m_fFileName.Empty();
								//	m_fFileName.Append(pname);
								//}
							}

							delete[] pbuf;
						}
					}
				}
			}
		}

		if (m_pBinding)
		{
			CComPtr<IWinInetHttpInfo> pWinInetHttpInfo;
			CComPtr<IBinding> pBinding = m_pBinding;
			HRESULT hr = pBinding->QueryInterface(IID_IWinInetHttpInfo, reinterpret_cast<void**>(&pWinInetHttpInfo));
			if ((SUCCEEDED(hr)) && (pWinInetHttpInfo))
			{
				DWORD size = 0;
				DWORD flags = 0;
				hr = pWinInetHttpInfo->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, 0, &size, &flags, 0);
				if (SUCCEEDED(hr))
				{
					LPSTR pbuf = new char[size+1];
					if (pbuf)
					{
						pbuf[size] = '\0';
						hr = pWinInetHttpInfo->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, pbuf, &size, &flags, 0);
						if ((SUCCEEDED(hr)) && (size > 0) && (pbuf[0] != '\0'))
						{
							if (!strstr(pbuf, "Content-Length: "))
								m_bContentLength = FALSE;
						}
					}
				}
			}
		}

		if (m_fFileName.Length() == 0)
			m_fFileName = L"";

		if (m_fFileExt.Length() == 0)
			m_fFileExt = L"";

		CComBSTR fPathToSave = L"";

		IMoneyhubEventsSink* pEvents = NULL;
		if (SUCCEEDED(this->QueryInterface(__uuidof(IMoneyhubEventsSink), (void**)&pEvents)))
		{
			pEvents->OnFileDownloadEx(m_fUrl, m_fFileName, m_fFileExt, NULL, NULL, NULL, &vbCancelDL, &fPathToSave);
			pEvents->Release();
		}

		// Cancel
		if (vbCancelDL == VARIANT_TRUE)
		{
			CancelDownload();
			return S_OK;
		}

		if (fPathToSave.Length() > 0)
		{
			m_fFullSavePath = fPathToSave;		
		}
		else
		{
			if(m_fFileName.Length() > 0)
			{
				m_fFullSavePath = m_fFileName;
			}
			else
			{
				TCHAR szTempPath[MAX_PATH + 1];
				GetTempPath(_countof(szTempPath), szTempPath);
				TCHAR szTempFileName[MAX_PATH + 1];
				GetTempFileName(szTempPath, _T("mhdl"), 0, szTempFileName);

				m_fFullSavePath = szTempFileName;
			}
		}

	}
	else if (ulStatusCode == BINDSTATUS_REDIRECTING)
	{
	}
	else if ((ulStatusCode == BINDSTATUS_DOWNLOADINGDATA))
	{
		IMoneyhubEventsSink* pEvents = NULL;
		if (SUCCEEDED(this->QueryInterface(__uuidof(IMoneyhubEventsSink), (void**)&pEvents)))
		{
			pEvents->OnFileDLProgress(m_fUrl, ulProgress, ulProgressMax, &vbCancelDL);
			pEvents->Release();
		}
	}
	else if ((ulStatusCode == BINDSTATUS_ENDDOWNLOADDATA))
	{
		if (!m_bCanceled)
		{
			VARIANT_BOOL vbOpenFile = VARIANT_FALSE;
			IMoneyhubEventsSink* pEvents = NULL;
			if (SUCCEEDED(this->QueryInterface(__uuidof(IMoneyhubEventsSink), (void**)&pEvents)))
			{
				pEvents->OnFileDLEndDownload(m_fUrl, m_fFullSavePath, &vbOpenFile);
				pEvents->Release();
			}

			m_bOpenFile = vbOpenFile == VARIANT_TRUE;
		}
	}

	if (m_pPrevBSCB)
	{
		if (ulStatusCode == BINDSTATUS_CONTENTDISPOSITIONATTACH)
			return S_OK;

		m_pPrevBSCB->OnProgress(ulProgress, ulProgressMax, ulStatusCode, szStatusText);
	}

	if (vbCancelDL == VARIANT_TRUE)
	{
		CancelDownload();
	}

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::OnStopBinding(HRESULT hresult, LPCWSTR szError)
{
	if (m_pBinding)
	{
		m_pBinding->Release();
		m_pBinding = NULL;
	}

	if (hresult && hresult != E_ABORT)
	{
		IMoneyhubEventsSink* pEvents = NULL;
		if (SUCCEEDED(this->QueryInterface(__uuidof(IMoneyhubEventsSink), (void**)&pEvents)))
		{
			pEvents->OnFileDLDownloadError(m_fUrl, NULL, m_fFullSavePath);
			pEvents->Release();
		}

		m_bFailed = TRUE;
	}

	ReleaseInterfaces();

	if (m_bOpenFile)
	{
		int re = (int)::ShellExecute(NULL, _T("open"), m_fFullSavePath, NULL, NULL, SW_SHOWNORMAL);
		if(re == SE_ERR_ACCESSDENIED)
		{
			WCHAR szPath[MAX_PATH] ={0};
			::GetModuleFileName(NULL, szPath, _countof(szPath));
			::PathRemoveFileSpecW(szPath);

			wstring path(szPath);
			path += L"\\MoneyHub.exe -ShellExecute ";

			path += m_fFullSavePath;

			STARTUPINFO si;	
			PROCESS_INFORMATION pi;	
			ZeroMemory( &pi, sizeof(pi) );	
			ZeroMemory( &si, sizeof(si) );	
			si.cb = sizeof(si);	
			//带参数打开
			if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{	
				::CloseHandle( pi.hProcess );		
				::CloseHandle( pi.hThread );		
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::GetBindInfo(DWORD* grfBINDF, BINDINFO* pbindinfo)
{
	if (pbindinfo == NULL || pbindinfo->cbSize == 0 || grfBINDF == NULL)
		return E_INVALIDARG;

	*grfBINDF = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_PULLDATA | BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE;

	DWORD cbSize = pbindinfo->cbSize = sizeof(BINDINFO);

	memset(pbindinfo,0,cbSize);
	pbindinfo->cbSize = cbSize;

	pbindinfo->dwBindVerb = BINDVERB_GET;
	pbindinfo->szExtraInfo = NULL;

	memset(&pbindinfo->stgmedData, 0, TYMED_NULL);

	pbindinfo->grfBindInfoF = 0;
	pbindinfo->szCustomVerb = NULL;

	pbindinfo->dwOptions = 0;
	pbindinfo->dwOptionsFlags = 0;
	pbindinfo->dwReserved = 0;

	return S_OK;
}

STDMETHODIMP CBindStatusCallback::OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pformatetc, STGMEDIUM* pstgmed)
{
	HRESULT hr = S_OK;
	
    if (BSCF_FIRSTDATANOTIFICATION & grfBSCF)
    {
        if (m_pStream == NULL &&  pstgmed->tymed == TYMED_ISTREAM)
	    {
			m_pStream = pstgmed->pstm;
			if (m_pStream)
			{
				m_pStream->AddRef();
				if (m_fFullSavePath.Length() > 0)
				{
					USES_CONVERSION;
					m_hFile = CreateFile(OLE2T(m_fFullSavePath), GENERIC_WRITE, 
						FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (m_hFile == INVALID_HANDLE_VALUE)
					{
						goto Error;
					}
				}
				else
				{
					goto Error;
				}
			}
    	}
    }

    if (m_pStream && dwSize > m_cbOldSize)
	{
        DWORD dwRead = dwSize - m_cbOldSize;
        DWORD dwActuallyRead = 0;
		DWORD dwWritten = 0;

		if (dwRead > 0)
		do
		{
			LPSTR pNewstr = new char[dwRead + 1];
			if (!pNewstr)
				goto Error;

			hr = m_pStream->Read(pNewstr, dwRead, &dwActuallyRead);
			pNewstr[dwActuallyRead] = '\0';
			if (dwActuallyRead > 0)
			{
				m_cbOldSize += dwActuallyRead;
				WriteFile(m_hFile, pNewstr, dwActuallyRead, &dwWritten, NULL);
			}

			delete[] pNewstr;

		} while (!(hr == E_PENDING || hr == S_FALSE) && SUCCEEDED(hr));
	}

	// Clean up
	if (BSCF_LASTDATANOTIFICATION & grfBSCF)
	{
        if (m_pStream)
		{
            m_pStream->Release();
			m_pStream = NULL;

			if (m_hFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hFile);
				m_hFile = INVALID_HANDLE_VALUE;
			}
		}

		hr = S_OK;
	}

	return hr;

Error:
	CancelDownload();

	return S_FALSE;
}

STDMETHODIMP CBindStatusCallback::OnObjectAvailable(REFIID riid,IUnknown* punk)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBindStatusCallback::BeginningTransaction(LPCWSTR szURL, LPCWSTR szHeaders, DWORD dwReserved, LPWSTR *pszAdditionalHeaders)
{
	m_fUrl = szURL;
	return S_OK;
}

STDMETHODIMP CBindStatusCallback::OnResponse(DWORD dwResponseCode, LPCWSTR szResponseHeaders, LPCWSTR szRequestHeaders, LPWSTR *pszAdditionalRequestHeaders)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// CDownloadManagerImpl

CDownloadManagerImpl::CDownloadManagerImpl(HWND hClient)
: m_cRef(0), m_hClient(hClient), m_hCloseWnd(NULL)
{
}

CDownloadManagerImpl::~CDownloadManagerImpl()
{
}

// IUnknown

STDMETHODIMP_(ULONG) CDownloadManagerImpl::AddRef()
{		
//	return ++m_cRef;

	return Increment(&m_cRef);
}

STDMETHODIMP_(ULONG) CDownloadManagerImpl::Release()
{
// 	ULONG nTemp = --m_cRef;
// 	if (nTemp == 0)
// 		delete this;

	LONG nTemp = Decrement(&m_cRef);
	if (nTemp == 0)
		delete this;

	return nTemp;
}

STDMETHODIMP CDownloadManagerImpl::QueryInterface(REFIID riid, void **ppvObject)
{
	if (!ppvObject)
		return E_POINTER;

	*ppvObject = NULL;

	if (riid == IID_IUnknown || riid == IID_IDownloadManager)
		*ppvObject = (IDownloadManager*)this ;

	if (*ppvObject != NULL)
	{
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

// IDownloadManager

STDMETHODIMP CDownloadManagerImpl::Download(IMoniker *pmk, IBindCtx *pBC, DWORD dwBindVerb, LONG grfBINDF, 
											BINDINFO *pBindInfo, LPCOLESTR pszHeaders, LPCOLESTR pszRedir, UINT uiCP)
{
	CBindStatusCallback *pBSCBObj = new CBindStatusCallback();
	IMoneyhubDownload *pMoneyhubDownload = NULL;
	pBSCBObj->QueryInterface(__uuidof(IMoneyhubDownload), (void**)&pMoneyhubDownload);
	pMoneyhubDownload->InitByUser(m_hClient, m_hCloseWnd);

	CComPtr<IBindStatusCallback> pBSCB;
	pMoneyhubDownload->QueryInterface(IID_IBindStatusCallback, (void**)&pBSCB);

	IBindStatusCallback *pDefaultBSCB = NULL;
	HRESULT hr = RegisterBindStatusCallback(pBC, pBSCB, &pDefaultBSCB, 0L);

	if (FAILED(hr) && pDefaultBSCB)
	{
		LPOLESTR oParam = L"_BSCB_Holder_";
		hr = pBC->RevokeObjectParam(oParam);
		if (SUCCEEDED(hr))
		{
			hr = RegisterBindStatusCallback(pBC, pBSCB, 0, 0L);
			if (SUCCEEDED(hr))
			{
				pMoneyhubDownload->HoldDefaultBSCB(pDefaultBSCB);
				pMoneyhubDownload->HoldDefaultBC(pBC);
			}
		}
	}

	pBSCB.Release();

	if (SUCCEEDED(hr))
	{
		CComPtr<IStream> pStream;
		hr = pmk->BindToStorage(pBC, 0, IID_IStream, (void**)&pStream);
		pMoneyhubDownload->HoldDefaultStream(pStream);
		pStream.Release();
	}

	pMoneyhubDownload->Release();
// 	if (pDefaultBSCB) pDefaultBSCB->Release();
// 	if (pBC) pBC->Release();

	return hr;
}

BOOL CDownloadManagerImpl::DownloadUrlAsync(LPCTSTR pszUrl)
{	
	IMoniker* pMoniker = NULL;
	IBindCtx* pBC = NULL;
	IStream* pStream = NULL;

	CBindStatusCallback *pBSCBObj = new CBindStatusCallback;
	IMoneyhubDownload *pMoneyhubDownload = NULL;
	pBSCBObj->QueryInterface(__uuidof(IMoneyhubDownload), (void**)&pMoneyhubDownload);

	USES_CONVERSION;
	pMoneyhubDownload->InitByClient(CT2OLE(pszUrl), m_hClient, m_hCloseWnd);

	IBindStatusCallback* pBSCB = NULL;
	pBSCBObj->QueryInterface(IID_IBindStatusCallback, (void**)&pBSCB);

	HRESULT hr = CreateURLMonikerEx(NULL, pszUrl, &pMoniker, URL_MK_UNIFORM);
	if (FAILED(hr))
	{
		pMoneyhubDownload->Release();
		pBSCB->Release();
		return FALSE;
	}

	hr = CreateAsyncBindCtx(0, pBSCB, NULL, &pBC);
	if (FAILED(hr))
	{
		pMoniker->Release();
		pMoneyhubDownload->Release();
		pBSCB->Release();

		return FALSE;
	}

	hr = pMoniker->BindToStorage(pBC, NULL, IID_IStream, (void**)&pStream);
	if (FAILED(hr))
	{
		pMoniker->Release();
		pMoneyhubDownload->Release();
		pBSCB->Release();
		pBC->Release();
		
		return FALSE;
	}

	if (pStream)
	{
		pMoneyhubDownload->HoldDefaultStream(pStream);
		pStream->Release();
	}

	if (pBC)
	{
		pMoneyhubDownload->HoldDefaultBC(pBC);
		pBC->Release();
	}

	pMoniker->Release();
	pMoneyhubDownload->Release();
	pBSCB->Release();
	pBC->Release();

	return TRUE;
}

void CDownloadManagerImpl::SetCloseWnd(HWND hWnd)
{
	m_hCloseWnd = hWnd;
}