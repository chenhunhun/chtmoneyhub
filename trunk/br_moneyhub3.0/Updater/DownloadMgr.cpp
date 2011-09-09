
#include "stdafx.h"
#include "DownloadMgr.h"
#include "UrlParser.h"

#pragma comment(lib, "wininet.lib")

CDownloadMgr::CDownloadMgr()
: m_bNeedStop(false), m_bRunning(false)
{
	m_ui64BytesCount = 0;
	m_ui64BytesDownloaded = 0;
}

CDownloadMgr::~CDownloadMgr()
{
}

UINT64 CDownloadMgr::GetBytesCount() const
{
	return m_ui64BytesCount;
}

UINT64 CDownloadMgr::GetBytesDownloaded() const
{
	return m_ui64BytesDownloaded;
}

void CDownloadMgr::Start(LPCTSTR lpszUrl, LPCTSTR lpszSave)
{
	m_strUrl = lpszUrl;
	m_strSave = lpszSave;

	m_ui64BytesCount = 0;
	m_ui64BytesDownloaded = 0;

	m_bRunning = true;

	DWORD dw;
	CloseHandle(CreateThread(NULL, 0, _threadWinInet, this, 0, &dw));
}

void CDownloadMgr::Stop()
{
	m_bNeedStop = true;
}

bool CDownloadMgr::IsRunning()
{
	return m_bRunning;
}

DWORD WINAPI CDownloadMgr::_threadWinInet(LPVOID lpParam)
{
	CDownloadMgr* pThis = (CDownloadMgr *)lpParam;
	pThis->DownloadFile();
	pThis->m_bRunning = false;

	return 0;
}

void CDownloadMgr::DownloadFile()
{
	CUrlParser url;
	if (!url.Crack(m_strUrl.c_str()))
		HandleError(ERR_URLCRACKERROR);

	HINTERNET  hInternetSession = NULL;
	HINTERNET  hInternetConnection = NULL;
	HINTERNET  hInternetFile = NULL;

	//LPCTSTR lpszUserAgent = _T("BankUpdate");
	LPCTSTR lpszUserAgent = _T("Mozilla/5.0 (compatible; MSIE 6.0; Windows NT 5.1)");
	hInternetSession = ::InternetOpen(lpszUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInternetSession == NULL)
	{
		HandleError(ERR_NETWORKERROR);
		return;
	}

	DWORD dwTimeOut = 4000;
	InternetSetOptionEx(hInternetSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(hInternetSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(hInternetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(hInternetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(hInternetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);

	hInternetConnection = ::InternetConnect(hInternetSession, url.GetHostName(), url.GetPort(), NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD)this);
	if (hInternetConnection == NULL)
	{
		HandleError(ERR_NETWORKERROR);

		InternetCloseHandle(hInternetSession);
	
		return;
	}

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;

	hInternetFile = HttpOpenRequest(hInternetConnection, NULL, url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, (DWORD)this);
	if (hInternetFile == NULL)
	{
		HandleError(ERR_NETWORKERROR);

		InternetCloseHandle(hInternetSession);
		InternetCloseHandle(hInternetConnection);

		return;
	}

	BOOL bSend = ::HttpSendRequest(hInternetFile, NULL, 0, NULL, 0);
	if (!bSend)
	{
		HandleError(ERR_NETWORKERROR);

		InternetCloseHandle(hInternetSession);
		InternetCloseHandle(hInternetConnection);
		InternetCloseHandle(hInternetFile);

		return;
	}

	TCHAR szStatusCode[32];
	DWORD dwInfoSize = sizeof(szStatusCode);
	if (!HttpQueryInfo(hInternetFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		HandleError(ERR_FILENOTFOUND);

		InternetCloseHandle(hInternetSession);
		InternetCloseHandle(hInternetConnection);
		InternetCloseHandle(hInternetFile);

		return;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);
		if (nStatusCode != HTTP_STATUS_OK)
		{
			HandleError(ERR_FILENOTFOUND);

			InternetCloseHandle(hInternetSession);
			InternetCloseHandle(hInternetConnection);
			InternetCloseHandle(hInternetFile);

			return;
		}
	}

	TCHAR szContentLength[32];
	dwInfoSize = sizeof(szContentLength);
	if (::HttpQueryInfo(hInternetFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
	{
		m_ui64BytesCount = (DWORD)_ttoi64(szContentLength);
	}
	else 
	{
		HandleError(ERR_FILENOTFOUND);

		InternetCloseHandle(hInternetSession);
		InternetCloseHandle(hInternetConnection);
		InternetCloseHandle(hInternetFile);

		return;
	}

	LPCTSTR lpszSaveFile = m_strSave.c_str();
	HANDLE hFile = CreateFile(lpszSaveFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		HandleError(ERR_DISKERROR);

		InternetCloseHandle(hInternetSession);
		InternetCloseHandle(hInternetConnection);
		InternetCloseHandle(hInternetFile);

		return;
	}

	DWORD dwBytesRead = 0;

	char szReadBuf[1024];
	DWORD dwBytesToRead = sizeof(szReadBuf);
	UINT ui64TotalBytesRead = 0;
	do
	{
		if (!::InternetReadFile(hInternetFile, szReadBuf, dwBytesToRead, &dwBytesRead))
		{
			HandleError(ERR_FILENOTFOUND);

			InternetCloseHandle(hInternetSession);
			InternetCloseHandle(hInternetConnection);
			InternetCloseHandle(hInternetFile);
	
			CloseHandle(hFile);

			return;
		}
		else if (dwBytesRead)
		{
			DWORD dwBytesWritten = 0;
			if (!WriteFile(hFile, szReadBuf, dwBytesRead, &dwBytesWritten, NULL))
			{
				HandleError(ERR_DISKERROR);

				InternetCloseHandle(hInternetSession);
				InternetCloseHandle(hInternetConnection);
				InternetCloseHandle(hInternetFile);

				CloseHandle(hFile);

				return;
			}

			ui64TotalBytesRead += dwBytesRead;
			m_ui64BytesDownloaded = ui64TotalBytesRead;
		}
	} 
	while (dwBytesRead);

	InternetCloseHandle(hInternetConnection);
	InternetCloseHandle(hInternetFile);
	InternetCloseHandle(hInternetSession);

	CloseHandle(hFile);
}

void CDownloadMgr::HandleError(int nIndex)
{
	m_nErrorCode = nIndex;
}