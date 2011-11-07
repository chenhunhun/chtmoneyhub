
#include "stdafx.h"
#include "HttpDownloader.h"
#include "UpdateMgr.h"
#include "..\Utils\PostData\UrlCrack.h"
#include "DownloadOperation.h"

#define REPEATNUM               6
#define BREAKRETURNBACKBYTE     20  //kb
#define MYWIRTEFILE             FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA | FILE_WRITE_EA | STANDARD_RIGHTS_WRITE | SYNCHRONIZE 

#pragma comment(lib, "wininet.lib")

#define MONEYHUB_USERAGENT _T("Moneyhub/2.0")

CHttpDownloader _HttpDownloader;

CHttpDownloader::CHttpDownloader()
{
	USES_CONVERSION;
	m_strHWID = A2CT(GenHWID2().c_str());

	m_wcsOriginalFileName = L"error";
}

int CHttpDownloader::GetData(LPCTSTR lpszUrl, LPCTSTR lpszSaveFile, CUpdateMgr* pUpdateMgr)
{
	m_repeatNum = 0 ;//重复五次
	m_strUrl = lpszUrl;

	m_lpPostData = NULL;
	m_dwPostDataLength = 0;

	m_strSaveFile = lpszSaveFile;
	m_pUpdateMgr = pUpdateMgr;

	m_hInetSession = NULL;
	m_hInetConnection = NULL;
	m_hInetFile = NULL;
	m_hSaveFile = INVALID_HANDLE_VALUE;

	m_ui64FileSize = 0;
	m_ui64TotalRead = 0;

	return TransferDataGet();
}

int CHttpDownloader::PostData(LPCTSTR lpszUrl, LPVOID lpPostData, DWORD dwPostDataLength, LPCTSTR lpszSaveFile, CUpdateMgr* pUpdateMgr)
{
	m_strUrl = lpszUrl;

	ATLASSERT(dwPostDataLength && lpPostData);
	m_lpPostData = lpPostData;
	m_dwPostDataLength = dwPostDataLength;

	m_strSaveFile = lpszSaveFile;
	m_pUpdateMgr = pUpdateMgr;

	m_hInetSession = NULL;
	m_hInetConnection = NULL;
	m_hInetFile = NULL;
	m_hSaveFile = INVALID_HANDLE_VALUE;

	m_ui64FileSize = 0;
	m_ui64TotalRead = 0;

	return TransferDataPost();
}

#define NEED_STOP	if (m_pUpdateMgr && m_pUpdateMgr->m_bNeedStop) { \
						CloseHandles(); \
						return ERR_STOPPED; }


int CHttpDownloader::TransferDataGet()
{
	CUrlCrack url;
	if (!url.Crack(m_strUrl.c_str()))
		return ERR_URLCRACKERROR;

	NEED_STOP;

	//LPCTSTR lpszUserAgent = _T("Mozilla/5.0 (compatible; MSIE 6.0; Windows NT 5.1)");
	//m_hInetSession = ::InternetOpen(lpszUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	m_hInetSession = ::InternetOpen(MONEYHUB_USERAGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
		return ERR_NETWORKERROR;

	NEED_STOP;

	DWORD dwTimeOut = 60000;
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);

	m_hInetConnection = ::InternetConnect(m_hInetSession, url.GetHostName(), url.GetPort(), NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD)this);
	if (m_hInetConnection == NULL)
	{
		//服务器连接错误，反馈 
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_SERVICE, MY_ERROR_DESCRIPT_SERVICE);

		CloseHandles();
		return ERR_NETWORKERROR;
	}

	NEED_STOP;

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;

	m_hInetFile = HttpOpenRequest(m_hInetConnection, NULL, url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD /*| INTERNET_FLAG_DONT_CACHE*/ | INTERNET_FLAG_KEEP_CONNECTION, (DWORD)this);
	if (m_hInetFile == NULL)
	{
		CloseHandles();
		return ERR_NETWORKERROR;
	}

	NEED_STOP;

	getBreakDownloadName();

	TCHAR szHeaders[100];
	//_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %08X%08X%08X%08X\r\n"), m_hwid.dw1, m_hwid.dw2, m_hwid.dw3, m_hwid.dw4);
	_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %s\r\n"), m_strHWID.c_str());
	HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	HttpAddRequestHeaders(m_hInetFile, _T("User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	_stprintf_s(szHeaders,_countof(szHeaders),_T("Range: bytes=%d-"), isBreakPointFile(m_strSaveFile.c_str()) );
  	HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD_IF_NEW ) ;

	BOOL bSend = ::HttpSendRequest(m_hInetFile, NULL, 0, NULL, 0);
	if (!bSend)
	{
		CloseHandles();
		return ERR_NETWORKERROR;
	}

	NEED_STOP;

	TCHAR szStatusCode[32];
	DWORD dwInfoSize = sizeof(szStatusCode);
	if (!HttpQueryInfo(m_hInetFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		CloseHandles();
		return ERR_FILENOTFOUND;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);
		if (nStatusCode != HTTP_STATUS_PARTIAL_CONTENT && nStatusCode != HTTP_STATUS_OK)
		{
			CloseHandles();
			return ERR_FILENOTFOUND;
		}
	}


	TCHAR szContentLength[32];
	dwInfoSize = sizeof(szContentLength);
	if (::HttpQueryInfo(m_hInetFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
	{
		m_ui64FileSize = (UINT64)_ttoi64(szContentLength) + isBreakPointFile(m_strSaveFile.c_str());
		if (m_pUpdateMgr)
			m_pUpdateMgr->SetProgressVal(m_ui64FileSize, 0);
	}
	else 
	{
		CloseHandles();
		return ERR_FILENOTFOUND;
	}

	NEED_STOP;

	return this->downLoadBreakpointFile();
}

int CHttpDownloader::TransferDataPost()
{
	CUrlCrack url;
	if (!url.Crack(m_strUrl.c_str()))
		return ERR_URLCRACKERROR;

	NEED_STOP;

	//LPCTSTR lpszUserAgent = _T("Mozilla/5.0 (compatible; MSIE 6.0; Windows NT 5.1)");
	//m_hInetSession = ::InternetOpen(lpszUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	m_hInetSession = ::InternetOpen(MONEYHUB_USERAGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
		return ERR_NETWORKERROR;

	NEED_STOP;

	DWORD dwTimeOut = 60000;
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);

	m_hInetConnection = ::InternetConnect(m_hInetSession, url.GetHostName(), url.GetPort(), NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD)this);
	if (m_hInetConnection == NULL)
	{
		//服务器连接错误，反馈 
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_SERVICE, MY_ERROR_DESCRIPT_SERVICE);
		CloseHandles();
		return ERR_NETWORKERROR;
	}

	NEED_STOP;

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;

	m_hInetFile = HttpOpenRequest(m_hInetConnection, _T("POST"), url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, (DWORD)this);
	if (m_hInetFile == NULL)
	{
		CloseHandles();
		return ERR_NETWORKERROR;
	}

	NEED_STOP;

	HttpAddRequestHeaders(m_hInetFile, _T("Content-Type: application/x-www-form-urlencoded\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	HttpAddRequestHeaders(m_hInetFile, _T("User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	TCHAR szHeaders[1024];
	//_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %08X%08X%08X%08X\r\n"), m_hwid.dw1, m_hwid.dw2, m_hwid.dw3, m_hwid.dw4);
	_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %s\r\n"), m_strHWID.c_str());
	HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	BOOL bSend = ::HttpSendRequest(m_hInetFile, NULL, 0, m_lpPostData, m_dwPostDataLength);
	if (!bSend)
	{
		CloseHandles();
		return ERR_NETWORKERROR;
	}

	NEED_STOP;

	TCHAR szStatusCode[32];
	DWORD dwInfoSize = sizeof(szStatusCode);
	if (!HttpQueryInfo(m_hInetFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		CloseHandles();
		return ERR_FILENOTFOUND;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);
		if (nStatusCode != HTTP_STATUS_OK)
		{
			CloseHandles();
			return ERR_FILENOTFOUND;
		}
	}

	NEED_STOP;
	
	m_hSaveFile = CreateFile(m_strSaveFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hSaveFile == INVALID_HANDLE_VALUE)
	{
		//xml文件创建失败
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_FILE, MY_ERROR_DESCRIPT_FILE);
		CloseHandles();
		return ERR_DISKERROR;
	}
	
	NEED_STOP;

	LPBYTE lpszData = NULL;
	DWORD dwSize = 0;

	while (true)   
	{   
		NEED_STOP;

		if (!InternetQueryDataAvailable(m_hInetFile, &dwSize, 0, 0))   
			break;   

		lpszData = new BYTE[dwSize];   
		DWORD dwDownloaded = 0;

		if (!InternetReadFile(m_hInetFile, (LPVOID)lpszData, dwSize, &dwDownloaded))   
		{   
			delete []lpszData;
			CloseHandles();
			return ERR_FILENOTFOUND;  
		}   
		else   
		{   
			DWORD dwBytesWritten = 0;
			if (!WriteFile(m_hSaveFile, lpszData, dwDownloaded, &dwBytesWritten, NULL))
			{
				//xml文件写入失败
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_EEEOR_ID_FILER, MY_ERROR_DESCRIPT_FILER);

				CloseHandles();
				return ERR_DISKERROR;
			}

			delete []lpszData;   

			if (dwDownloaded == 0)   
				break;   
		}   
	}

	CloseHandles();

	return ERR_SUCCESS;
}

void CHttpDownloader::CloseHandles()
{
	if (m_hInetConnection)
	{
		InternetCloseHandle(m_hInetConnection);
		m_hInetConnection = NULL;
	}

	if (m_hInetFile)
	{
		InternetCloseHandle(m_hInetFile);
		m_hInetFile = NULL;
	}

	if (m_hInetSession)
	{
		InternetCloseHandle(m_hInetSession);
		m_hInetSession = NULL;
	}

	if (m_hSaveFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hSaveFile);
		m_hSaveFile = INVALID_HANDLE_VALUE;
	}
}

#define DOWNLOADADDNAME   L".mh"
#include "..//Utils//CryptHash/base64.h"
void CHttpDownloader::getBreakDownloadName()
{
	///  one 
// 	if( 0 == wcscmp(m_wcsOriginalFileName.c_str(), L"error") )
// 	{
// 		int dwLen =0;
// 		wchar_t uBuf[1000] = {0};
// 
// 		base64_encode(( unsigned char *)m_strSaveFile.c_str(),m_strSaveFile.size() , ( unsigned char *)uBuf, &dwLen);
// 
// 		m_wcsOriginalFileName = L"error";
// 
// 		if( wcsrchr(m_strSaveFile.c_str(), L'\\') )
// 		{
// 			m_wcsOriginalFileName = m_strSaveFile;
// 
// 			wchar_t wcsName[MAX_PATH] = {0};
// 			wcscpy_s(wcsName, _countof(wcsName), m_strSaveFile.c_str() );
// 			*( wcsrchr(wcsName, L'\\')+1 ) = L'\0';
// 			m_strSaveFile = wcsName;
// 
// 			m_strSaveFile += uBuf;
// 			m_strSaveFile += L".mh";
// 		}
// 	}	 
	///
	if( wcsstr(m_strSaveFile.c_str(), DOWNLOADADDNAME) )
	{
		return ;
	}

	m_wcsOriginalFileName = L"error";

	if( wcsrchr(m_strSaveFile.c_str(), L'.') )
	{
		m_wcsOriginalFileName = m_strSaveFile;

		wchar_t wcsName[MAX_PATH] = {0};
		wcscpy_s(wcsName, _countof(wcsName), m_strSaveFile.c_str() );
		*(wcsrchr(wcsName, L'.')) = L'\0';
		m_strSaveFile = wcsName;

		m_strSaveFile += DOWNLOADADDNAME;
	}
}


UINT64  CHttpDownloader::isBreakPointFile(std::wstring wcsFile)
{
	UINT64 uI64Num =0;
	wchar_t wcsNum[255];
	UINT64  uI64Return =0 ;
	LARGE_INTEGER largeSize;
	largeSize.QuadPart = 0;
	DWORD   dwRegType = 0, dwReturnSize = sizeof(wcsNum);
 
	HANDLE hFile = CreateFile(wcsFile.c_str(),MYWIRTEFILE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if( hFile != INVALID_HANDLE_VALUE)	
	{
		GetFileSizeEx(hFile,&largeSize);

		uI64Num = largeSize.QuadPart;
		if( uI64Num > BREAKRETURNBACKBYTE*1024 )
			uI64Return = uI64Num - BREAKRETURNBACKBYTE*1024;
	}

	CloseHandle(hFile);
	return uI64Return;
}

#define DOWNLOADLEN   ((1024)*(60))
bool CHttpDownloader::downLoadBreakpointFile()
{
	DWORD dwBytesRead = 0;
	//char szReadBuf[DOWNLOADLEN];
	char *szReadBuf = new char[DOWNLOADLEN];
	if( !szReadBuf )
		return false;

	ZeroMemory(szReadBuf, DOWNLOADLEN);
	DWORD dwBytesToRead = DOWNLOADLEN;

	wchar_t wcsNum[255];
	DWORD   dwRegType = 0, dwReturnSize = sizeof(wcsNum), dwFilePoint;
	UINT64  uI64Num = 0;
	bool    bIsBreak = false;

	uI64Num = isBreakPointFile(m_strSaveFile.c_str());
	
	dwFilePoint = (DWORD)(uI64Num);

	if( dwFilePoint > 0 )//续传文件
	{
		bIsBreak = true;
		m_hSaveFile = CreateFile(m_strSaveFile.c_str(), MYWIRTEFILE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (m_hSaveFile == INVALID_HANDLE_VALUE)//断点文件不存在
		{
			m_hSaveFile = CreateFile(m_strSaveFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		else
		{
			if(INVALID_SET_FILE_POINTER == SetFilePointer(m_hSaveFile,dwFilePoint,NULL,FILE_BEGIN) )
			{
				CloseHandles();
				delete []szReadBuf;
				return ERR_DISKERROR;
			}

			m_ui64TotalRead = dwFilePoint;
		}
	}
	else//重新下载
	{
		m_hSaveFile = CreateFile(m_strSaveFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if ( INVALID_HANDLE_VALUE == m_hSaveFile )
	{
		CloseHandles();
		delete []szReadBuf;
		return ERR_DISKERROR;
	}

	do
	{
		NEED_STOP;

		if (!::InternetReadFile(m_hInetFile, szReadBuf, dwBytesToRead, &dwBytesRead))
		{
			CloseHandles();

			if(m_repeatNum < REPEATNUM)
			{
				m_repeatNum ++;
				DWORD dwBegin = GetTickCount();		
				while( GetTickCount() - dwBegin < 1000 * m_repeatNum * 20)
					Sleep(20000);
					
				delete []szReadBuf;
				return this->TransferDataGet() == 0 ? false:true ;
			}
			else
			{
				delete []szReadBuf;
				return ERR_FILENOTFOUND;
			}
		}
		else if (dwBytesRead)
		{
			DWORD dwBytesWritten = 0;
			if (!WriteFile(m_hSaveFile, szReadBuf, dwBytesRead, &dwBytesWritten, NULL))
			{
				//文件写入错误，反馈 
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_EEEOR_ID_FILER, MY_ERROR_DESCRIPT_FILER);

				CloseHandles();
				delete []szReadBuf;
				return ERR_DISKERROR;
			}

			m_ui64TotalRead += dwBytesRead;

			if (m_pUpdateMgr)
				m_pUpdateMgr->SetProgressVal(m_ui64FileSize, m_ui64TotalRead);
		}
	} 
	while (dwBytesRead);
   
	CloseHandles();	

	if( !MoveFileExW(m_strSaveFile.c_str(), m_wcsOriginalFileName.c_str() , MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED ) )
	{
		CDownloadOperation don;
		don.deleteAllUpdataFile();
	}

	m_wcsOriginalFileName = L"error";

	delete []szReadBuf;
	return ERR_SUCCESS;
}

