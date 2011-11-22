#pragma once

#include <windows.h>
#include <wininet.h>
#include <string>
#include "..\Utils\HardwareID\genhwid.h"

enum {
	ERR_STOPPED = -1,
	ERR_SUCCESS = 0,
	ERR_URLCRACKERROR = 1,
	ERR_NETWORKERROR = 2, 
	ERR_FILENOTFOUND = 3, 
	ERR_DISKERROR = 4, 
	ERR_FATALERROR = 5,
};

class CUpdateMgr;

class CHttpDownloader
{
public:
	CHttpDownloader();

public:
	int GetData(LPCTSTR lpszUrl, LPCTSTR lpszSaveFile, CUpdateMgr* pUpdateMgr);
	int PostData(LPCTSTR lpszUrl, LPVOID lpPostData, DWORD dwPostDataLength, LPCTSTR lpszSaveFile, CUpdateMgr* pUpdateMgr);

protected:
	int TransferDataGet();
	int TransferDataPost();
	void CloseHandles();

	/*
	*判断是否需要断点续传，返回0表示不需要，正数表示在该点处继续下载
	*/
	UINT64 isBreakPointFile(std::wstring wcsFile);
	bool downLoadBreakpointFile();
	bool DownLoadFileNoBreak();
	void getBreakDownloadName();

protected:
	tstring			m_strUrl;
	tstring			m_strSaveFile;
	std::wstring    m_wcsOriginalFileName;
	bool			m_bBreakDownload;

	LPVOID m_lpPostData;
	DWORD m_dwPostDataLength;

	CUpdateMgr* m_pUpdateMgr;

	HINTERNET m_hInetSession;
	HINTERNET m_hInetConnection;
	HINTERNET m_hInetFile;
	HANDLE m_hSaveFile;

	//HWIDSTRUCT m_hwid;
	std::wstring m_strHWID;

protected:
	UINT64 m_ui64FileSize;
	UINT64 m_ui64TotalRead;

	UINT m_repeatNum;
};

extern CHttpDownloader _HttpDownloader;