#include "StdAfx.h"
#include "postData.h"
#include "UrlCrack.h"
#include "..//CryptHash/base64.h"


#define MONEYHUB_USERAGENT _T("Moneyhub/2.0")

CPostData* CPostData::m_instance = NULL;
std::tstring m_strUrl;
std::tstring m_strSaveFile;


CPostData * CPostData::getInstance()
{
	if(m_instance == NULL)
		m_instance = new CPostData();
	return m_instance;
}


CPostData::CPostData()
{
	USES_CONVERSION;
	m_strHWID = A2CT(GenHWID2().c_str());
}

void CPostData::sendData(IN LPCTSTR lpszUrl, IN LPSTR pData, IN DWORD dwLen)
{
	std::string allXmlStr = pData;
	DWORD dwBufLen = strlen(allXmlStr.c_str()) + 32;
	LPSTR lpszXmlInfo = new char[dwBufLen];
	
	strcpy_s(lpszXmlInfo,dwBufLen,allXmlStr.c_str());

	int dwSize = strlen(lpszXmlInfo) * 2 + 1;
	unsigned char* pszOut = new unsigned char[dwSize];
	base64_encode((LPBYTE)lpszXmlInfo, strlen(lpszXmlInfo), pszOut, &dwSize);
	pszOut[dwSize] = 0;


	std::string info = "xml=";
	info += UrlEncode((char *)pszOut);

	delete []pszOut;
	delete []lpszXmlInfo;


	PostData(lpszUrl,(LPVOID)info.c_str(),info.size() );
}


std::string CPostData::UrlEncode(const std::string& src)   
{   
	static char hex[] = "0123456789ABCDEF";   
	std::string dst;   

	for (size_t i = 0; i < src.size(); i++)   
	{   
		unsigned char ch = src[i];   
		if (isalnum(ch))   
		{   
			dst += ch;   
		}   
		else  
			if (src[i] == ' ')   
			{   
				dst += '+';   
			}   
			else  
			{   
				unsigned char c = static_cast<unsigned char>(src[i]);   
				dst += '%';   
				dst += hex[c / 16];   
				dst += hex[c % 16];   
			}   
	}   
	return dst;   
}  

int CPostData::PostData(IN LPCTSTR lpszUrl, IN LPVOID lpPostData, IN DWORD dwPostDataLength, IN LPCTSTR lpszSaveFile /*, CUpdateMgr* pUpdateMgr*/)
{
	m_strUrl = lpszUrl;//

	ATLASSERT(dwPostDataLength && lpPostData);
	m_lpPostData = lpPostData;
	m_dwPostDataLength = dwPostDataLength;

	//m_strSaveFile = lpszSaveFile;

	m_hInetSession = NULL;
	m_hInetConnection = NULL;
	m_hInetFile = NULL;
	m_hSaveFile = INVALID_HANDLE_VALUE;

	m_ui64FileSize = 0;
	m_ui64TotalRead = 0;

	return TransferDataPost();
}

void CPostData::CloseHandles()
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
int CPostData::TransferDataPost()
{
	CUrlCrack url;
	if (!url.Crack(m_strUrl.c_str()))
		return ERR_URLCRACKERROR;

	m_hInetSession = ::InternetOpen(MONEYHUB_USERAGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
		return ERR_NETWORKERROR;


	DWORD dwTimeOut = 60000;
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);

	m_hInetConnection = ::InternetConnect(m_hInetSession, url.GetHostName(), url.GetPort(), NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD)this);
	if (m_hInetConnection == NULL)
	{
		CloseHandles();
		return ERR_NETWORKERROR;
	}


	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;

	m_hInetFile = HttpOpenRequest(m_hInetConnection, _T("POST"), url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, (DWORD)this);
	if (m_hInetFile == NULL)
	{
		CloseHandles();
		return ERR_NETWORKERROR;
	}


	HttpAddRequestHeaders(m_hInetFile, _T("Content-Type: application/x-www-form-urlencoded\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	TCHAR szHeaders[1024];
	_stprintf_s(szHeaders, _countof(szHeaders), _T("MoneyhubUID: %s\r\n"), m_strHWID.c_str());
	HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	BOOL bSend = ::HttpSendRequest(m_hInetFile, NULL, 0, m_lpPostData, m_dwPostDataLength);
	if (!bSend)
	{
		CloseHandles();
		return ERR_NETWORKERROR;
	}

	CloseHandles();

	return ERR_SUCCESS;
}