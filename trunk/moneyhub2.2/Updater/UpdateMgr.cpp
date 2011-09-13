
#include "stdafx.h"
#include "UpdateMgr.h"
#include "HttpDownloader.h"
#include "../utils/tinyxml/tinyxml.h"
#include "CheckSign.h"
#include "ConvertBase.h"
#include "Updater.h"
#include "../include/ConvertBase.h"
CUpdateMgr _UpdateMgr;

// 判断升级包是否包含主程序模块
bool CUpdateMgr::m_isMain = false ;

CUpdateMgr::CUpdateMgr() : m_bRunning(false), m_bNeedStop(false), 
						   m_funcEvent(NULL), m_lpPostData(NULL), m_bCriticalPack(false)
{
}

CUpdateMgr::~CUpdateMgr()
{
	if (m_lpPostData)
		delete []m_lpPostData;
}

void CUpdateMgr::Start(tstring strUrl, LPVOID lpPostData, DWORD dwPostDataLength)
{
	m_bRunning = true;
	m_bNeedStop = false;
	m_strListUrl = strUrl;

	if (lpPostData && dwPostDataLength)
	{
		if (m_lpPostData)
			delete []m_lpPostData;

		m_lpPostData = new BYTE[dwPostDataLength];
		memcpy(m_lpPostData, lpPostData, dwPostDataLength);
		
		m_dwPostDataLength = dwPostDataLength;
	}

	DWORD dw;
	CloseHandle(CreateThread(NULL, 0, _threadUpdate, this, 0, &dw));
}

void CUpdateMgr::Stop()
{
	m_bNeedStop = true;

	// 此处 Sleep 比 WaitForSingleObject 更好
	while (m_bRunning)
		Sleep(100);
}

bool CUpdateMgr::IsRunning() const
{
	return m_bRunning;
}

void CUpdateMgr::SetEventsFunc(UPDATEMGREVENTFUNC func, LPVOID lpVoid)
{
	m_funcEvent = func;
	m_lpParam = lpVoid;
}

void CUpdateMgr::Event(UpdateMgrEvent ume, LPCTSTR info)
{
	if (m_funcEvent)
		m_funcEvent(ume, info, m_lpParam);
}

bool CUpdateMgr::GetUpdateList()		// 下载LIST
{
	Event(UE_RETR_LIST);

	m_strLocalList = _TempCachePath + _T("\\list.xml");
	int nErrCode = _HttpDownloader.PostData(m_strListUrl.c_str(), m_lpPostData, m_dwPostDataLength, m_strLocalList.c_str(), this);	

	if (nErrCode != ERR_SUCCESS)
	{
		ErrCode2Event(nErrCode);
		return false;
	}

	Event(UE_DONE_LIST);

	return true;
}

bool CUpdateMgr::CheckListFile()		// 检查List文件
{
	Event(UE_CHECK_LIST);

	HANDLE hFile = CreateFile(m_strLocalList.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		Event(UE_CHECK_LIST_FAILED);
		return false;
	}

	DWORD dwLowSize = GetFileSize(hFile, NULL);
	if (dwLowSize == INVALID_FILE_SIZE)
	{
		Event(UE_CHECK_LIST_FAILED);
		CloseHandle(hFile);

		return false;
	}

	DWORD dwRead = 0;
	LPBYTE pContent = new BYTE[dwLowSize + 1];
	if (!ReadFile(hFile, pContent, dwLowSize, &dwRead, NULL))
	{
		Event(UE_CHECK_LIST_FAILED);
		delete[] pContent;
		CloseHandle(hFile);

		return false;
	}

	CloseHandle(hFile);
	*(pContent+dwLowSize) = '\0';

	if (!ParseListContent(pContent, dwLowSize))
	{
		//xml文件解析失败
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_PARSE, MY_ERROR_DESCRIPT_PARSE);

		Event(UE_CHECK_LIST_FAILED);
		delete[] pContent;

		return false;
	}
	
	delete[] pContent;

	if (m_dwStatusId == 200)
	{
		Event(UE_AVAIL_YES, m_strStatusMsg.c_str());
	}
	else
	{
		Event(UE_AVAIL_NO, m_strStatusMsg.c_str());
		return false;
	}

	return true;
}

bool CUpdateMgr::GetUpdateFiles()		// 下载数据包
{
	for (size_t i = 0; i < m_vecFileUrls.size(); i++)
	{
		LPCTSTR lpszFullUrl = m_vecFileUrls[i].c_str();
		Event(UE_RETR_FILE);
		const TCHAR* pSlash = _tcsrchr(lpszFullUrl, '/');
		if (pSlash == NULL)
		{
			Event(UE_FATAL_ERROR);
			return false;
		}

		LPCTSTR lpszFileName = pSlash + 1;

		tstring strSaveFile = _TempCachePath;
		strSaveFile += _T("\\");
		strSaveFile += lpszFileName;

		int nErrCode = _HttpDownloader.GetData(lpszFullUrl, strSaveFile.c_str(), this);

		if (nErrCode != ERR_SUCCESS)
		{
			ErrCode2Event(nErrCode);
			return false;
		}

		USES_CONVERSION;

		m_vecLocalFiles.push_back(strSaveFile);
		
		Event(UE_DONE_FILE);
	}

	return true;
}

bool CUpdateMgr::GetSignatureFiles()	// 下载签名
{
	for (size_t i = 0; i < m_vecSignUrls.size(); i++)
	{
		LPCTSTR lpszFullUrl = m_vecSignUrls[i].c_str();
		Event(UE_RETR_SIGN);
		const TCHAR* pSlash = _tcsrchr(lpszFullUrl, '/');
		if (pSlash == NULL)
		{
			Event(UE_FATAL_ERROR);
			return false;
		}

		LPCTSTR lpszFileName = pSlash + 1;

		tstring strSaveFile = _TempCachePath;
		strSaveFile += _T("\\");
		strSaveFile += lpszFileName;

		int nErrCode = _HttpDownloader.GetData(lpszFullUrl, strSaveFile.c_str(), this);

		if (nErrCode != ERR_SUCCESS)
		{
			ErrCode2Event(nErrCode);
			return false;
		}

		m_vecLocalSigns.push_back(strSaveFile);

		Event(UE_DONE_SIGN);
	}

	return true;
}



std::wstring CUpdateMgr::getDownLoadFilePath()
{
	LPWSTR lpszTempPath = new wchar_t[MAX_PATH + 1];
	int nLength = GetTempPathW(MAX_PATH, lpszTempPath);

	while (nLength > MAX_PATH)
	{
		delete[] lpszTempPath;
		lpszTempPath = new wchar_t[nLength + 1];
		nLength = GetTempPath(nLength, lpszTempPath);
	}

	wchar_t tmp[500];
	wcscpy_s(tmp,_countof(tmp),lpszTempPath);	
	GetLongPathNameW(tmp,lpszTempPath,MAX_PATH+1 );
	wcscpy_s(lpszTempPath,MAX_PATH+1,tmp);

	lpszTempPath[nLength] = L'\0';
	std::wstring path = lpszTempPath;	
	delete[] lpszTempPath;

	if (path[path.size() - 1] != L'\\')
		path += L"\\";
	path += L"BankUpdate";
	path += L'\\';
	//CreateDirectory(path.c_str(), NULL);

	return path;
}
int CUpdateMgr::CheckUpdateFiles()
{
	Event(UE_CHECK_FILE);

// 	if (!CheckSignature(m_vecLocalSigns, m_vecLocalFiles) )
// 	{
// 		Event(UE_CHECK_FILE_FAILED);
// 
// 		return false;
// 	}
	std::vector <std::string>  vecLocalSaveFile;
	std::vector <std::wstring>  vecNopassFile;

	unsigned char content[4000]={0};
	int dwReturnSize = 0;

	for(std::vector<std::string>::size_type i = 0; i < m_checkBase64.size (); i ++) // gao
	{
		base64_decode((unsigned char *)m_checkBase64[i].c_str(),m_checkBase64[i].size(),content,&dwReturnSize);	
	
		*(content + dwReturnSize) = 0;

		if(!BankMdrVerifier::InitCheck((const char *)content,dwReturnSize) )
		{
// 			Event(UE_CHECK_FILE_FAILED);
// 			return false;
		}
	}

//	for(std::vector<std::wstring>::size_type j=0 ; j<m_vecLocalFiles.size() ; j++)
	for(std::vector<std::wstring>::size_type j = 0 ; j < m_vecLocalFiles.size () ; j ++) // gao
	{
#ifdef _UNICODE
		CStringW str(m_vecLocalFiles[j].c_str());
		if(!BankMdrVerifier::VerifyModule(str) )
		{
			Event(UE_CHECK_FILE_FAILED);
			vecNopassFile.push_back(m_vecLocalFiles[j] );
			//return false;
		}
		else
		{
			m_vecPassCheck.push_back(m_vecLocalFiles[j].c_str() );
		}
#else
#error "why use ansi???"
		USES_CONVERSION;
		std::wstring wstr(CT2W(m_vecLocalFiles[j].c_str()));
		CStringW str = wstring.c_str();
		if(!BankMdrVerifier::VerifyModule(str) )
		{
			Event(UE_CHECK_FILE_FAILED);
			return false;
		}
#endif
	}

	if( m_vecPassCheck.size() == m_vecLocalFiles.size() && 0 != m_vecPassCheck.size())//all is avalid
		return 2;
	else if( m_vecPassCheck.size() > 0 )//somes is avalid
		return 1;
	else                                //no pass
	{
		//效验错误，反馈
		std::wstring wstrDes = MY_ERROR_DESCRIPT_CHECKCHK;
		for(std::vector<std::wstring>::size_type i = 0; i < vecNopassFile.size(); i++)
		{
			wstrDes += vecNopassFile[i].c_str();
		}

		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_CHECKCHK, wstrDes);
		return 0;
	}

}

DWORD WINAPI CUpdateMgr::_threadUpdate(LPVOID lp)
{
	CUpdateMgr* pThis = (CUpdateMgr *)lp;

	do
	{
		// 服务器上得到返回的要更新的版本XML文件
		if (!pThis->GetUpdateList())
			break;

		// 检验得到的文件是否正确
		if (!pThis->CheckListFile())
			break;

		// 下载要更新的文件
		if (!pThis->GetUpdateFiles())
			break;

//  		if (!pThis->GetSignatureFiles())
//  			break;

		// 检验下载的文件
 		if (!pThis->CheckUpdateFiles())
 			break;

		pThis->Event(UE_EVERYTHING_OK);

	} while (false);

	pThis->m_bRunning = false;

	return 0;
}

void CUpdateMgr::ErrCode2Event(int nErrCode)
{
	switch (nErrCode)
	{
	case ERR_STOPPED:
	case ERR_SUCCESS:
		break;

	case ERR_URLCRACKERROR:
	case ERR_NETWORKERROR:
		Event(UE_INET_ERROR);
		break;

	case ERR_FILENOTFOUND:
	case ERR_DISKERROR:
		Event(UE_FILE_ERROR);
		break;

	case ERR_FATALERROR:
		Event(UE_FATAL_ERROR);
		break;
	}
}

void CUpdateMgr::SetProgressVal(UINT64 uSize, UINT64 uRead)
{
	DWORD dwPos = (DWORD)(uRead * 100 / (uSize ? uSize : 1));
	if (!m_bNeedStop)
		Event(UE_PROGRESS_VALUE, (LPCTSTR)dwPos);
}


/*

先忽略Signture

*/

bool CUpdateMgr::ParseListContent(LPBYTE pContent, DWORD dwSize)
{
	USES_CONVERSION;

	m_vecFileUrls.clear();

	TiXmlDocument xmlDoc;
	xmlDoc.Parse((const char*)pContent); // 参数是文件内容

	if (xmlDoc.Error())
		return false;

	const TiXmlNode* pRoot = xmlDoc.FirstChild("moneyhub"); // ANSI string 
	if (NULL == pRoot)
		return false;

	const TiXmlNode* pStatus = pRoot->FirstChild("status");
	if (NULL == pStatus)
		return false;

	const TiXmlNode* pStatusId = pStatus->FirstChild("id");
	if (NULL == pStatusId)
		return false;

	const TiXmlElement* pStatusIdElement = pStatusId->ToElement();
	const char* pText = pStatusIdElement->GetText();
	m_dwStatusId = pText != NULL ? atoi(pText) : 0;

	const TiXmlNode* pStatusMsg = pStatus->FirstChild("message");
	if (NULL != pStatusMsg)
	{
		const TiXmlElement* pStatusMsgElement = pStatusMsg->ToElement();
		pText = pStatusMsgElement->GetText();
		m_strStatusMsg = pText != NULL ? AToW(pStatusMsgElement->GetText()) : L"";
	}

	if (m_dwStatusId != 200)
		return true;

	const TiXmlNode* pModules = pRoot->FirstChild("modules");
	if (NULL == pModules)
		return false;

	for (const TiXmlNode *pModule = pModules->FirstChild("module"); pModule != NULL; pModule = pModules->IterateChildren("module", pModule))
	{
		const TiXmlElement* pModuleElement = pModule->ToElement();
		std::string strType = "";
		if (pModuleElement->Attribute("type"))
			strType = pModuleElement->Attribute("type");

		bool bCriticalPack = _stricmp(strType.c_str(), "critical") == 0;
		const TiXmlNode* pModuleCheck = pModule->FirstChild("check");
		if( NULL == pModuleCheck)
			continue;
		const TiXmlElement* pModuleCheckElement = pModuleCheck->ToElement();
		pText = pModuleCheckElement->GetText();
		std::string strCheck;
		if( pText )
		{
		    strCheck = pText;
			m_checkBase64.push_back(strCheck);
		}

		USES_CONVERSION;
		const TiXmlNode* pModuleContent = pModule->FirstChild("content");
		if( NULL == pModuleContent)
			continue;
		const TiXmlElement* pModuleContentElement = pModuleContent->ToElement();
		pText = W2A( AToW(pModuleContentElement->GetText()).c_str() );
		if( pText )
		{
			strCheck = pText;
			if( strCheck.size() > 3 )
				m_vecUpgradeContent.push_back(strCheck);
		}


		const TiXmlNode* pModuleName = pModule->FirstChild("name");
		if (NULL == pModuleName)
			continue;

		const TiXmlElement* pModuleNameElement = pModuleName->ToElement();
		pText = pModuleNameElement->GetText();
		std::string strVersionName;
		tstring name;
		if( pText )
		{	
			strVersionName = pText;
			name = pText != NULL ? A2W(pModuleNameElement->GetText()) : L"";
		}

		//get version
		if( !strcmp(pText , "Main") )
		{
			const TiXmlNode* pModuleVersion = pModule->FirstChild("version");
			if(NULL == pModuleVersion)
				continue;

			const TiXmlElement* pModuleVersionElement = pModuleVersion->ToElement();
			pText = pModuleVersionElement->GetText();
			if( pText )
			{
				strCheck = pText;
				g_newVersion.insert(std::make_pair(strVersionName,strCheck));
			}
		}

		const TiXmlNode* pModulePath = pModule->FirstChild("path");
		if (NULL == pModulePath)
			continue;

		const TiXmlElement* pModulePathElement = pModulePath->ToElement();
		pText = pModulePathElement->GetText();
		tstring url = pText != NULL ? A2W(pModulePathElement->GetText()) : L"";

		if (!url.empty())
		{
			if (_tcsicmp(name.c_str(), _T("signature")) == 0)
			{
				m_vecSignUrls.push_back(url);
			}
			else
			{
				if (_tcscmp(name.c_str(), _T("Main")) == 0)
				{
					m_isMain = true;	
					std::vector<tstring>::iterator vecItorBegin = m_vecFileUrls.begin();
					m_vecFileUrls.insert(vecItorBegin,url);
				}
				else
				{
					std::vector<tstring>::iterator vecItorEnd = m_vecFileUrls.end();
					m_vecFileUrls.insert(vecItorEnd,url);
				}
				
				if( bCriticalPack )
					m_bCriticalPack = bCriticalPack;
			}
		}
	}

	if (m_vecSignUrls.size() == 0 && m_vecFileUrls.size() == 0)
	{
		return false;
	}
	return true;
}


int CUpdateMgr::checkFileValid()
{
	m_vecLocalFiles.clear();
	m_vecFileUrls.clear();

	m_strLocalList = _TempCachePath + _T("\\list.xml");

	HANDLE hFile = CreateFile(m_strLocalList.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwLowSize = GetFileSize(hFile, NULL);
	if (dwLowSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);

		return false;
	}

	DWORD dwRead = 0;
	LPBYTE pContent = new BYTE[dwLowSize + 1];
	if (!ReadFile(hFile, pContent, dwLowSize, &dwRead, NULL))
	{
		delete[] pContent;
		CloseHandle(hFile);
		return false;
	}

	CloseHandle(hFile);
	*(pContent+dwLowSize) = '\0';

	if (!ParseListContent(pContent, dwLowSize))
	{
		delete[] pContent;
		return false;
	}

	delete[] pContent;

	this->getLocalFilePathName();

	return CheckUpdateFiles();
}

bool CUpdateMgr::getLocalFilePathName()
{
	for (size_t i = 0; i < m_vecFileUrls.size(); i++)
	{
		LPCTSTR lpszFullUrl = m_vecFileUrls[i].c_str();

		const TCHAR* pSlash = _tcsrchr(lpszFullUrl, '/');
		if (pSlash == NULL)
		{
			return false;
		}

		LPCTSTR lpszFileName = pSlash + 1;

		tstring strSaveFile = _TempCachePath;
		strSaveFile += _T("\\");
		strSaveFile += lpszFileName;

		m_vecLocalFiles.push_back(strSaveFile);
	}

	return true;
}

bool CUpdateMgr::isSafeFile(wchar_t * pWcsFileName)
{
	bool bReturn = false;

	for(std::vector<std::wstring>::size_type i = 0; i < m_vecPassCheck.size (); i ++) // gao
	{
		if( wcsstr(pWcsFileName,m_vecPassCheck[i].c_str()))
		{
			bReturn = true;
			break;
		}
	}

	return bReturn;
}