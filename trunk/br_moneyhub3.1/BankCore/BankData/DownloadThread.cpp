#include "stdafx.h"
#include "DownloadThread.h"
#include "BkInfoDownload.h" // USB安装好要弹出显示时用
#include "..\Utils\PostData\UrlCrack.h"
#include "..\ThirdParty\CabExtract\CabExtract.h"
#include "..\Security\Authentication\BankMdrVerifier\export.h"
#include "..\Utils\ExceptionHandle\ExceptionHandle.h"
#include "..\Utils\RecordProgram\RecordProgram.h"
#include "../MyError.h"

#define REPEATNUM               6 // 重试6次
#define BREAKRETURNBACKBYTE     20  //kb
#define MYWIRTEFILE             FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA | FILE_WRITE_EA | STANDARD_RIGHTS_WRITE | SYNCHRONIZE 

#pragma comment(lib, "wininet.lib")

#define MONEYHUB_USERAGENT _T("Moneyhub/2.0")



CDownloadThread::CDownloadThread()
{
	m_hInetSession = NULL;
	m_hInetConnection = NULL;
	m_hInetFile = NULL;
	m_hSaveFile = INVALID_HANDLE_VALUE;

	m_ui64FileSize = 0;
	m_ui64TotalRead = 0;

	m_dwPostDataLength = 0;

	m_bCancle = false;

	m_bDLThreadState = emInit;

	m_repeatNum = 0 ;//重复五次

	m_bRetryWait = false;
	//InitializeCriticalSection(&m_cs);
}

CDownloadThread::~CDownloadThread()
{
	//DeleteCriticalSection(&m_cs);
}

void CDownloadThread::DownLoadInit(LPCTSTR lpszHWID, LPCTSTR lpszUrl, LPCTSTR lpszSaveFile, LPSTR lpPostData)
{
	
	m_strSaveFile = lpszSaveFile;
	m_strDownURl = lpszUrl;
	m_strHWID = lpszHWID;
	m_strSendData = lpPostData;
	m_dwPostDataLength = m_strSendData.size ();
}

int CDownloadThread::DownLoadData()
{
	int nVal = 0;
	CExceptionHandle::MapSEtoCE();
	try
	{
		// 检验线程是否已经退出
	//	::EnterCriticalSection(&m_cs);
		m_bDLThreadState = emBegin;

		OutputDebugString(m_strSaveFile.c_str());
		OutputDebugString(m_strDownURl.c_str());

		ATLASSERT (!m_strSaveFile.empty () && !m_strDownURl.empty ());
		if (m_strSaveFile.empty () || m_strDownURl.empty ())
		{
			//::LeaveCriticalSection (&m_cs);
			
			return ERR_UNKNOW;
		}


		if (m_strSendData.empty () || m_dwPostDataLength <= 0)
			nVal = TransferDataGet();
		else
			nVal = TransferDataPost ();

		if (ERR_SUCCESS != nVal)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_CATCH,
			CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadData中Get或Post数据时出错！BackValue = %d", nVal));
		}

		if (m_bCancle)
		{
			m_bDLThreadState = emCancled;
		}
		else
		{
			m_bDLThreadState = emFinished;
		}

	//	::LeaveCriticalSection (&m_cs);

	}

	catch(CExceptionHandle eH)
	{
	//	::LeaveCriticalSection (&m_cs);

		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_CATCH,
			CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadData捕获的异常！LastErrCode = %d", GetLastError()));

		std::string strErr;
		eH.SetThreadName ("Download Thread Error");
		eH.RecordException ();
		
	}

	return nVal;
}

#define NEED_STOP	if (m_bCancle) { \
					CloseHandles(); \
					return ERR_STOPPED; \
					}


int CDownloadThread::TransferDataGet()
{
	CUrlCrack url;
	if (!url.Crack(m_strDownURl.c_str()))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_URLCRACKERROR,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的url = %s Crack 异常！", m_strDownURl.c_str()));
		return ERR_URLCRACKERROR;
	}

	NEED_STOP;
	
	m_hInetSession = ::InternetOpen(MONEYHUB_USERAGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
	{
		NEED_STOP;
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTOPEN,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的InetOpen异常！LastErrCode = %d", GetLastError()));
		return ERR_INTOPEN;
	}
	
	NEED_STOP;
	
	DWORD dwTimeOut = 60000;
	if (!InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_SETOPTION,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
	}
	if (!InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_SETOPTION,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的INTERNET_OPTION_CONTROL_SEND_TIMEOUT异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
	}
	if (!InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_SETOPTION,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的INTERNET_OPTION_SEND_TIMEOUT异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
	}
	if (!InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_SETOPTION,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的INTERNET_OPTION_RECEIVE_TIMEOUT异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
	}
	if (!InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_SETOPTION,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的INTERNET_OPTION_CONNECT_TIMEOUT异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
	}
	
	m_hInetConnection = ::InternetConnect(m_hInetSession, url.GetHostName(), url.GetPort(), NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD)this);
	if (m_hInetConnection == NULL)
	{
		NEED_STOP;
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTCONNECT,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的连接异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
		CloseHandles();
		return ERR_INTCONNECT;
	}
	
	NEED_STOP;

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;
	
	m_hInetFile = HttpOpenRequest(m_hInetConnection, NULL, url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD /*| INTERNET_FLAG_DONT_CACHE*/ | INTERNET_FLAG_KEEP_CONNECTION, (DWORD)this);
	if (m_hInetFile == NULL)
	{
		NEED_STOP;
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTOPENREQ,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的连接异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
		CloseHandles();
		return ERR_INTOPENREQ;
	}

	NEED_STOP;

//	m_wcsOriginalFileName = m_strSaveFile;
	TranslanteToBreakDownloadName(m_strSaveFile, m_wcsBreakFileName);
	//GetBreakDownloadName();
	
	TCHAR szHeaders[100];
	
	_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %s\r\n"), m_strHWID.c_str());
	HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	HttpAddRequestHeaders(m_hInetFile, _T("User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	
	_stprintf_s(szHeaders,_countof(szHeaders),_T("Range: bytes=%d-"), IsBreakPointFile(m_wcsBreakFileName.c_str()) );
  	HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD_IF_NEW ) ;
	
	BOOL bSend = ::HttpSendRequest(m_hInetFile, NULL, 0, NULL, 0);
	if (!bSend)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTSENDREQ,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的发送请求异常！LastErrCode = %d", GetLastError()));
//		OutputDebugString(strErr);

		CloseHandles();
		return ERR_INTSENDREQ;
	}

	NEED_STOP;
	
	TCHAR szStatusCode[32];
	DWORD dwInfoSize = sizeof(szStatusCode);
	if (!HttpQueryInfo(m_hInetFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTQUREYINFO,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的HttpQueryInfo异常！LastErrCode = %d", GetLastError()));
//		OutputDebugString(strErr);
		CloseHandles();
		return ERR_INTQUREYINFO;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);
		if (nStatusCode != HTTP_STATUS_PARTIAL_CONTENT && nStatusCode != HTTP_STATUS_OK)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTFILENOTFOUND,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的!= HTTP_STATUS_OK异常！LastErrCode = %d", GetLastError()));

//		OutputDebugString(strErr);
			CloseHandles();
			return ERR_INTFILENOTFOUND;
		}
	}


	TCHAR szContentLength[32];
	dwInfoSize = sizeof(szContentLength);
	if (::HttpQueryInfo(m_hInetFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
	{
		m_ui64FileSize = (UINT64)_ttoi64(szContentLength) + IsBreakPointFile(m_wcsBreakFileName.c_str());
		m_ui64TotalRead = 0;
	}
	else 
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTFILENOTFOUND,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataGet的HttpQueryInfo异常！LastErrCode = %d", GetLastError()));
//		OutputDebugString(strErr);
		CloseHandles();
		return ERR_INTFILENOTFOUND;
	}

	NEED_STOP;

	return DownLoadBreakpointFile();
}


int CDownloadThread::TransferDataPost()
{
	CUrlCrack url;
	if (!url.Crack(m_strDownURl.c_str()))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_URLCRACKERROR,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的url = %s Crack 异常！", m_strDownURl.c_str()));
		return ERR_URLCRACKERROR;
	}

	NEED_STOP;

	m_hInetSession = ::InternetOpen(MONEYHUB_USERAGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTOPEN,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的InternetOpen异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
		return ERR_INTOPEN;
	}

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
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTCONNECT,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的InternetConnect异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
		CloseHandles();
		return ERR_INTCONNECT;
	}

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;

	m_hInetFile = HttpOpenRequest(m_hInetConnection, _T("POST"), url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, (DWORD)this);
	if (m_hInetFile == NULL)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTOPENREQ,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的HttpOpenRequest异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
		CloseHandles();
		return ERR_INTOPENREQ;
	}

	NEED_STOP;
	
	HttpAddRequestHeaders(m_hInetFile, _T("Content-Type: application/x-www-form-urlencoded\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	HttpAddRequestHeaders(m_hInetFile, _T("User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	HttpAddRequestHeaders(m_hInetFile, _T("Accept-Language: zh-CN"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	TCHAR szHeaders[1024];
	
	_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %s\r\n"), m_strHWID.c_str());
	HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	BOOL bSend = ::HttpSendRequest(m_hInetFile, NULL, 0, (LPVOID)m_strSendData.c_str (), m_dwPostDataLength);
	if (!bSend)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTSENDREQ,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的HttpSendRequest异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
		CloseHandles();
		return ERR_INTSENDREQ;
	}

	NEED_STOP;
	
	TCHAR szStatusCode[32];
	DWORD dwInfoSize = sizeof(szStatusCode);
	if (!HttpQueryInfo(m_hInetFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTQUREYINFO,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的HttpQueryInfo异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
		CloseHandles();
		return ERR_INTQUREYINFO;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);
		if (nStatusCode != HTTP_STATUS_OK)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTFILENOTFOUND,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的HttpQueryInfo异常！LastErrCode = %d", GetLastError()));
			//OutputDebugString(strErr);
			
			CloseHandles();
			return ERR_INTFILENOTFOUND;
		}
	}

	NEED_STOP;
	
	m_hSaveFile = CreateFile(m_strSaveFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hSaveFile == INVALID_HANDLE_VALUE)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_CREATEFILE,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的CreateFile异常！LastErrCode = %d", GetLastError()));
		//OutputDebugString(strErr);
		CloseHandles();
		return ERR_CREATEFILE;
	}
	
	NEED_STOP;

	LPBYTE lpszData = NULL;
	DWORD dwSize = 0;
	
	while (true)   
	{   
		NEED_STOP;

		if (!InternetQueryDataAvailable(m_hInetFile, &dwSize, 0, 0))
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTQUERYDATAAVAILABLE,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的InternetQueryDataAvailable异常！LastErrCode = %d", GetLastError()));
			//OutputDebugString(strErr);
			// ERR_INTQUERYDATAAVAILABLE
			break;
		}

		lpszData = new BYTE[dwSize];   
		DWORD dwDownloaded = 0;

		if (!InternetReadFile(m_hInetFile, (LPVOID)lpszData, dwSize, &dwDownloaded))   
		{   
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTREADFILE,
			CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的InternetReadFile异常！LastErrCode = %d", GetLastError()));
			//OutputDebugString(strErr);
			delete []lpszData;
			CloseHandles();
			return ERR_INTREADFILE;  
		}   
		else   
		{   
			DWORD dwBytesWritten = 0;
			if (!WriteFile(m_hSaveFile, lpszData, dwDownloaded, &dwBytesWritten, NULL))
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_WRITEFILE,
					CRecordProgram::GetInstance()->GetRecordInfo(L"TransferDataPost的WriteFile异常！LastErrCode = %d", GetLastError()));
				//OutputDebugString(strErr);
				CloseHandles();
				return ERR_WRITEFILE;
			}

			delete []lpszData;   

			if (dwDownloaded == 0)   
				break;   
		}   
	}

	CloseHandles();
	
	return ERR_SUCCESS;
}

void CDownloadThread::CloseHandles()
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

//#define DOWNLOADADDNAME   L"%moneyhubDownload%.mh"
#include "..//Utils//CryptHash/base64.h"


bool CDownloadThread::TranslanteToBreakDownloadName(const std::wstring& strPath, std::wstring& strDesc)
{
	if (strPath.empty ())
		return false;
	strDesc.clear ();

	if( wcsrchr(strPath.c_str(), L'\\') )
	{
		wchar_t wcsName[MAX_PATH] = {0};
		wcscpy_s(wcsName, _countof(wcsName), strPath.c_str() );
		*( wcsrchr(wcsName, L'\\')+1 ) = L'\0';
		strDesc = wcsName;

		// 得到文件名称
		std::wstring strTp = strPath;
		strTp = strTp.substr(strDesc.length (), strPath.length ());

		int dwLen =0;
		wchar_t uBuf[1000] = {0};
		base64_encode(( unsigned char *)strTp.c_str(),strTp.size(), ( unsigned char *)uBuf, &dwLen);

		if (strTp.find (L".") > 0)
			strTp = strTp.substr (0, strTp.find (L"."));

		strDesc += uBuf;
		strDesc += L".";
		strDesc += strTp;
		strDesc += L".mh";

		return true;
	}
	
	return false;
}


UINT64  CDownloadThread::IsBreakPointFile(std::wstring wcsFile)
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
	else
	{
		DWORD dwErr = GetLastError ();
	}

	CloseHandle(hFile);
	return uI64Return;
}

int CDownloadThread::DownLoadBreakpointFile()
{
	
	DWORD dwBytesRead = 0;
	char szReadBuf[1024];
	DWORD dwBytesToRead = sizeof(szReadBuf);

	wchar_t wcsNum[255];
	DWORD   dwRegType = 0, dwReturnSize = sizeof(wcsNum), dwFilePoint;
	UINT64  uI64Num = 0;
	bool    bIsBreak = false;

	uI64Num = IsBreakPointFile(m_wcsBreakFileName.c_str());
	
	dwFilePoint = (DWORD)(uI64Num);

	if( dwFilePoint > 0 )//续传文件
	{
		NEED_STOP;
		bIsBreak = true;
		m_hSaveFile = CreateFile(m_wcsBreakFileName.c_str(), MYWIRTEFILE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (m_hSaveFile == INVALID_HANDLE_VALUE)//断点文件不存在
		{
			
			m_hSaveFile = CreateFile(m_wcsBreakFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		else
		{
			if(INVALID_SET_FILE_POINTER == SetFilePointer(m_hSaveFile,dwFilePoint,NULL,FILE_BEGIN) )
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_SETFILEPOINTER,
					CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadBreakpointFile的SetFilePointer异常！LastErrCode = %d", GetLastError()));
	
				CloseHandles();
				return ERR_SETFILEPOINTER;
			}

			m_ui64TotalRead = dwFilePoint;
		}
	}
	else//重新下载
	{
		NEED_STOP;
		m_hSaveFile = CreateFile(m_wcsBreakFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if ( INVALID_HANDLE_VALUE == m_hSaveFile )
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_CREATEFILE,
			CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadBreakpointFile的CreateFile异常！LastErrCode = %d", GetLastError()));
		CloseHandles();
		CloseHandles();
		return ERR_CREATEFILE;
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
				m_bRetryWait = true; // 重试等待

				// 等待十秒后重试
				int nTemp = 50;
				while(nTemp >= 0)
				{
					nTemp --;
					Sleep(200);
					NEED_STOP;
				}
				
				m_bRetryWait = false;
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_INTREADFILE,
					CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadBreakpointFile中的InternetReadFile异常！LastErrCode = %d", GetLastError()));
				
				return this->TransferDataGet() == 0 ? false:true ;
			}
			else
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_OUTOFTIME,
					CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadBreakpointFile下载超时！LastErrCode = %d", GetLastError()));
				
				return ERR_OUTOFTIME; // 下载超时
			}
		}
		else if (dwBytesRead)
		{
			
			DWORD dwBytesWritten = 0;
			if (!WriteFile(m_hSaveFile, szReadBuf, dwBytesRead, &dwBytesWritten, NULL))
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_WRITEFILE,
					CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadBreakpointFile的WriteFile异常！LastErrCode = %d", GetLastError()));

				CloseHandles();
				return ERR_WRITEFILE;
			}

			m_ui64TotalRead += dwBytesRead;
		}
	} 
	while (dwBytesRead);
   
	CloseHandles();	

	NEED_STOP;
	if( !MoveFileExW(m_wcsBreakFileName.c_str(), m_strSaveFile.c_str() , MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED ) )
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_RENAMEFILE,
					CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadBreakpointFile的MoveFileExW异常！LastErrCode = %d", GetLastError()));
		NEED_STOP;
	}

	m_wcsBreakFileName.clear ();

		
	return ERR_SUCCESS;
}

void CDownloadThread::ReadDownloadPercent(UINT64& i64FileSize, UINT64& i64ReadSize)
{
	i64FileSize = m_ui64FileSize;
	i64ReadSize = m_ui64TotalRead;
}

int CDownloadThread::GetDownloadPercent(void)
{
	if (m_ui64TotalRead > m_ui64FileSize)
		return 0;

	if (m_ui64TotalRead <= 0)
		return 0;

	return (m_ui64TotalRead * 100 / m_ui64FileSize);
}


void CDownloadThread::CancleDownload(void) // 用户取消下载
{
	m_bCancle = true;

	// 检验是否已经退出
	//::EnterCriticalSection(&m_cs);

	//::LeaveCriticalSection (&m_cs);
}

//void CDownloadThread::PauseDownload(void) // 用户暂停下载
//{
//	m_bPause = true;
//
//	// 检验是否已经退出
//	::EnterCriticalSection(&m_cs);
//	::LeaveCriticalSection (&m_cs);
//
//	// 删除临时文件
//}

//bool CDownloadOperate::IsCancled(void)
//{
//	return m_bCancle;
//
//	
//}

//bool CDownloadOperate::IsPaused(void)
//{
//	return m_bPause;
//}

int CDownloadThread::GetThreadState(void)
{
	return m_bDLThreadState;
}

LPCTSTR CDownloadThread::GetDownloadURL(void)
{
	return m_strDownURl.c_str ();
}

LPCTSTR CDownloadThread::GetDownloadStorePath(void)
{
	return m_strSaveFile.c_str ();
}

//// 下载线程实体
//DWORD WINAPI CDownloadThread::ThreadProc(LPVOID lpParam)
//{
//	CDownloadThread* pDownOper = (CDownloadThread*)lpParam;
//	ATLASSERT (NULL != pDownOper);
//	if (NULL == pDownOper)
//		return 0;
//
//	return pDownOper->DownLoadData ();
//}

//void CDownloadThread::BeginDownloadByThread(bool bCreateThread)
//{
//	m_bCreateThread = bCreateThread;
//	if (bCreateThread)
//	{
//		DWORD dwThreadID = 0;
//		CloseHandle (CreateThread (NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadID)); // 启动新线程下载
//	}
//	else
//		DownLoadData (); // 用原本的线程下载
//
//}

LPCTSTR CDownloadThread::GetBreakFilePath(void)
{
	return m_wcsBreakFileName.c_str ();
}


/////////////////////////////////////////
CDownloadAndSetupThread::CDownloadAndSetupThread() : m_emDlKind(emOther), m_bSetupDlFile(false), m_bSetupFinish(false), m_bCancled(false)
{
	//InitializeCriticalSection(&m_cs);
}

CDownloadAndSetupThread::~CDownloadAndSetupThread()
{
	//DeleteCriticalSection(&m_cs);
}

void CDownloadAndSetupThread::DownLoadAndSetupDlFile(PDOWN_LOAD_PARAM_NODE pDownloadParam)
{

	m_emDlKind = pDownloadParam->emKind;
	m_bSetupDlFile = pDownloadParam->bSetupDlFile;
	m_strDlPath = pDownloadParam->strSaveFile;
	m_strSetupPath = pDownloadParam->strSetupPath;

	m_bThreadExcute = false;

	// 记录下下载USBKEY时的参数
	if (emUsbKey == m_emDlKind)
		m_dlUSBParam = pDownloadParam->dwUsbKeyParam;

	// 初使化下载参数
	DownLoadInit (pDownloadParam->strHWID.c_str (), pDownloadParam->strUrl.c_str (),
		pDownloadParam->strSaveFile.c_str (), (LPSTR)pDownloadParam->strSendData.c_str ());

	
	if (pDownloadParam->bCreateThread == false) // 不创建新线程
	{
		DownLoadData ();

		SetupDlFileOrNotNeed(m_emDlKind, m_bSetupDlFile);

	}
	else // 创建下载线程
	{
		DWORD dwThreadID = 0;

		CloseHandle (CreateThread (NULL, 0, DLAndSetupThreadProc, (LPVOID)this, 0, &dwThreadID));
	}


}

void CDownloadAndSetupThread::SetupDlFileOrNotNeed(MY_DOWNLOAD_KIND emKind, bool bSetup)
{
	if (!bSetup)
		return;

	if (emOther == emKind)
		return;

	// 进行文件校验，并给出相应的提示
	while (!CheckDlFileAndShowErrMeg(GetDownloadStorePath(), emKind))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_CHECK_DOWNLOAD_FILE,
			CRecordProgram::GetInstance()->GetRecordInfo(L"DownLoadAndSetupDlFile检验已经下载的%s文件失败！", GetDownloadStorePath()));

		if (m_bCancled)
			break;
	}

	// 取消
	if (!m_bCancled)
	{
		// 进行安装
		if (!SetupDownloadFile (emKind, GetDownloadStorePath(), m_strSetupPath.c_str ()))
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_SETUP_DOWNLOAD_FILE,
				CRecordProgram::GetInstance()->GetRecordInfo( L"DownLoadAndSetupDlFile将文件%s安装到%s时失败！", GetDownloadStorePath(), m_strSetupPath.c_str ()));
		}
	}
}

bool CDownloadAndSetupThread::SetupDownloadFile(MY_DOWNLOAD_KIND emKind,LPCTSTR lpPath, LPCTSTR lpSetUp)
{
	if (emOther == emKind)
		return false;

	if (emBkActiveX & emKind)
	{
		// 如果要加安装失败的提示在这加
		bool bBackVal = SetupBankControl (lpPath, lpSetUp);
		if (!bBackVal)
			return false;
		else
		{
			DeleteFile (lpPath);
			return true;
		}
	}

	if (emUsbKey == emKind)
	{
		// 如果要加安装失败的提示在这加
		bool bBackVal =  SetupBankUsbKey (lpPath, lpSetUp);
		if (!bBackVal)
		{
			//OutputDebugString(L"111---166");
			return false;
		}
		else
		{
			// 安装成功后删除
			DeleteFile (lpPath);
			return true;
		}
		
	}

	return false;
}

bool CDownloadAndSetupThread::SetupBankControl(LPCTSTR lpPath, LPCTSTR lpSetUp) // 银行控件安装
{
	ATLASSERT (NULL != lpPath);
	if (NULL == lpPath)
		return false;

	std::wstring strPath = lpPath;
	std::wstring strSetup = lpSetUp;

	if (strPath.rfind (L".cab") == strPath.length () - 4) // 安装cab文件
	{
		std::wstring strTemp = wcsrchr(strPath.c_str(), '\\') + 1;
		strTemp = strTemp.substr (0, strTemp.rfind(L".cab"));
		
		CCabExtract tt;
		if(tt.ExtractFile(strPath.c_str(), strSetup.c_str()))
		{
			strSetup += strTemp;
			strSetup += L"\\info.mchk";

			// 检验解压是否成功
			int nTime = 0;
			HANDLE hFile = NULL;
			do
			{
				Sleep (100);
				nTime ++;
				hFile = CreateFile (strSetup.c_str (), NULL, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			}
			while (INVALID_HANDLE_VALUE == hFile && nTime < 10);
			CloseHandle (hFile);

			m_bSetupFinish = true; // 标记安装完成
			if (nTime >= 10)
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_SETUP_CAB_OUTOFTIME,
					CRecordProgram::GetInstance()->GetRecordInfo(L"安装文件%s超时失败！", lpPath));
				return false;
			}

			return true;
		}
	}
	return false;
}

bool CDownloadAndSetupThread::SetupBankUsbKey(LPCTSTR lpPath, LPCTSTR lpSetUp) // 银行USBKEY安装
{
	ATLASSERT (NULL != lpPath);
	if (NULL == lpPath)
		return false;

	std::wstring strPath = lpPath;

	if (strPath.rfind (L".exe") == strPath.length () - 4) // 安装exe文件
	{
		OSVERSIONINFO os = { sizeof(OSVERSIONINFO) };
		::GetVersionEx(&os);
		if(os.dwMajorVersion >= 6)
		{
			SHELLEXECUTEINFOW shExecInfo;
			shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;//SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI; 
			shExecInfo.hwnd = NULL;
			shExecInfo.lpVerb = L"runas";
			shExecInfo.lpFile = LPWSTR(lpPath);
			shExecInfo.lpParameters = NULL;
			shExecInfo.lpDirectory = NULL;
			shExecInfo.nShow = SW_SHOWNORMAL;
			shExecInfo.hInstApp = NULL;

			// 用户取消
			if (true == m_bCancled)
			{
				return true;
			}

			if (!ShellExecuteExW(&shExecInfo))
			{			
				int err = GetLastError();
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, err,
					CRecordProgram::GetInstance()->GetRecordInfo(L"SetupBankUsbKey创建安装进程%s失败！", lpPath));
			}
			else
				WaitForSingleObject (shExecInfo.hProcess, INFINITE); // 等待usbkey.exe安装完成才返回

			while (!CBkInfoDownloadManager::GetInterfaceInstance ()->USBFinalTest (m_dlUSBParam.nVid, m_dlUSBParam.nPid, m_dlUSBParam.nMid, m_bSetupFinish))
			{	
				if (!ShellExecuteExW(&shExecInfo))
				{
					CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, GetLastError(),
					CRecordProgram::GetInstance()->GetRecordInfo(L"SetupBankUsbKey创建安装进程%s失败！", lpPath));
				}
				else
					WaitForSingleObject (shExecInfo.hProcess, INFINITE); // 等待usbkey.exe安装完成才返回
			}
		}
		else
		{
			STARTUPINFOW si;
			memset (&si, 0, sizeof (STARTUPINFOW));
			si.wShowWindow = SW_HIDE;
			si.cb = sizeof (STARTUPINFOW);
			PROCESS_INFORMATION pi;
			memset (&pi, 0, sizeof (PROCESS_INFORMATION));	

			// 用户取消
			if (true == m_bCancled)
			{
				return true;
			}

			if (!CreateProcess(NULL, LPWSTR(lpPath), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_CREATEPROCESS,
					CRecordProgram::GetInstance()->GetRecordInfo(L"SetupBankUsbKey创建安装进程失败！", lpPath));
				m_bSetupFinish = true;
				return false;
			}
			else
				WaitForSingleObject (pi.hProcess, INFINITE); // 等待usbkey.exe安装完成才返回

			while (!CBkInfoDownloadManager::GetInterfaceInstance ()->USBFinalTest (m_dlUSBParam.nVid, m_dlUSBParam.nPid, m_dlUSBParam.nMid, m_bSetupFinish))
			{
				memset (&si, 0, sizeof (STARTUPINFOW));
				memset (&pi, 0, sizeof (PROCESS_INFORMATION));	
				if (!CreateProcess(NULL, LPWSTR(lpPath), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_CREATEPROCESS,
						CRecordProgram::GetInstance()->GetRecordInfo(L"SetupBankUsbKey创建安装进程失败！", lpPath));
					m_bSetupFinish = true;
					return false;
				}
				else
					WaitForSingleObject (pi.hProcess, INFINITE); // 等待usbkey.exe安装完成才返回
			}
			CloseHandle (pi.hThread);
			CloseHandle (pi.hProcess);
		}
		

		m_bSetupFinish = true; // 标记安装完成

		return true;
	}
	
	return false;
}

// 文件校验
bool CDownloadAndSetupThread::CheckDownLoadFile(LPCTSTR lpPath)
{
	//std::list<std::wstring>::const_iterator it;
	//for(it = UrlList.begin (); it != UrlList.end (); it ++)
	{
#ifdef _UNICODE
		CStringW str(lpPath);
		if(!BankMdrVerifier::VerifyModule(str) )
		{
			return false;
		}
#else
#error "why use ansi???"
		USES_CONVERSION;
		std::wstring wstr(CT2W(m_vecLocalFiles[j].c_str()));
		CStringW str = wstring.c_str();
		if(!BankMdrVerifier::VerifyModule(str) )
		{
			return false;
		}
#endif
	}
	return true;

}

bool CDownloadAndSetupThread::CheckDlFileAndShowErrMeg(LPCTSTR lpPath, MY_DOWNLOAD_KIND emKind)
{
	//OutputDebugString(L"111---168");
	// 弹出提示校验文件失败的提示框
	if (!CheckDownLoadFile (lpPath))
	{
		if (emKind == emOther)
			return false;

		//OutputDebugString(L"111---169");
		int nBack = IDRETRY;//::MessageBox (NULL, L"下载银行软件失败，是否重试？", L"银行软件下载", MB_OK | MB_RETRYCANCEL);
		if (IDRETRY == nBack && !m_bCancled) // 重下
		{
			//OutputDebugString(L"111---170");
			Sleep(5000);
			DownLoadData ();
			
		}
		return false;
		//else
		//{
		//	//OutputDebugString(L"111---171");
		//	m_bCancled = true;
		//	if (emBkActiveX & emKind)
		//	{ 
		//		// 取消时调用用户界面的取消功能
		//		return false;
		//	}
		//}

	}
	return true;
}

// 利用线程进行下载安装
DWORD WINAPI CDownloadAndSetupThread::DLAndSetupThreadProc(LPVOID lpParam)
{
	ATLASSERT (NULL != lpParam);
	if (NULL == lpParam)
		return 0;

	CDownloadAndSetupThread *pTemp = (CDownloadAndSetupThread*)lpParam;
	if (NULL == pTemp)
		return 0;
	
	// 标记有线程在执行
	//::EnterCriticalSection (&(pTemp->m_cs));

	// 执行下载
	pTemp->DownLoadData ();

	// 如果要安装则执行安装，如果不用安装则跳过
	pTemp->SetupDlFileOrNotNeed(pTemp->m_emDlKind, pTemp->m_bSetupDlFile);

	//::LeaveCriticalSection (&(pTemp->m_cs));

	if(pTemp->m_bCancled)
		delete pTemp;

	return 0;
}


bool CDownloadAndSetupThread::IsFinished(void)
{
	return m_bSetupFinish;
}

bool CDownloadAndSetupThread::IsCancled(void)
{
	return m_bCancled;
}

MY_DOWNLOAD_KIND CDownloadAndSetupThread::GetDownloadKind(void)
{
	return m_emDlKind;
}


void CDownloadAndSetupThread::CancleDownloadAndSetup(void)
{
	m_bCancled = true;
	CancleDownload ();
}