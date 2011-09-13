#pragma once 

#include <windows.h>
#include <wininet.h>

#include "..\Utils\HardwareID\genhwid.h"

enum 
{
	ERR_STOPPED = -1,
	ERR_SUCCESS = 0,
	ERR_URLCRACKERROR = 1,
	ERR_NETWORKERROR = 2, 
	ERR_FILENOTFOUND = 3, 
	ERR_DISKERROR = 4, 
	ERR_FATALERROR = 5,
};


class  CPostData
{
public:
	CPostData();
protected:


	LPVOID m_lpPostData;
	DWORD m_dwPostDataLength;

	HINTERNET m_hInetSession;
	HINTERNET m_hInetConnection;
	HINTERNET m_hInetFile;
	HANDLE m_hSaveFile;

	std::wstring m_strHWID;

protected:
	UINT64 m_ui64FileSize;
	UINT64 m_ui64TotalRead;

private:
	int TransferDataPost();
	void CloseHandles();
	
	std::string UrlEncode(const std::string& src);   

	static CPostData * m_instance;
public:
	static CPostData * getInstance();
	void sendData(IN LPCTSTR lpszUrl, IN LPSTR pData, IN DWORD dwLen);
	int PostData(IN LPCTSTR lpszUrl, IN LPVOID lpPostData, IN DWORD dwPostDataLength, IN LPCTSTR lpszSaveFile = NULL/*, CUpdateMgr* pUpdateMgr*/);
};