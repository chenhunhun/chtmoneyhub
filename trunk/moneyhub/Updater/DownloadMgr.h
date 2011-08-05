#pragma once

#include <windows.h>
#include <wininet.h>

#include <string>

enum {
	ERR_URLCRACKERROR = 1,
	ERR_NETWORKERROR = 2, 
	ERR_FILENOTFOUND = 3, 
	ERR_DISKERROR = 4, 
	ERR_FATALERROR = 5,
};

class CDownloadMgr
{
public:
	CDownloadMgr();
	~CDownloadMgr();

public:
	UINT64 GetBytesCount() const;
	UINT64 GetBytesDownloaded() const;

protected:
	UINT64 m_ui64BytesCount;
	UINT64 m_ui64BytesDownloaded;

public:
	void Start(LPCTSTR lpszUrl, LPCTSTR lpszSave);
	void Stop();
	bool IsRunning();

protected:
	static DWORD WINAPI _threadWinInet(LPVOID lpParam);
	void DownloadFile();
	void HandleError(int nIndex);

protected:
	bool m_bNeedStop;
	bool m_bRunning;
	std::tstring m_strUrl;
	std::tstring m_strSave;

	int m_nErrorCode;
};