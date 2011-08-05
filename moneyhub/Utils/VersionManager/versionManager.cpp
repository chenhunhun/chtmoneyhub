#include "StdAfx.h"
#include "versionManager.h"
#include <string>
#include "FileVersionInfo.h"
#include "../../ThirdParty/tinyxml/tinyxml.h"
#include "..//Encryption/CHKFile/CHK.h"
#include "..//..//include//ConvertBase.h"

VMMAPDEF g_mapVersionVM;
#define  BANKINFOPATH        L"BankInfo\\banks\\*.*"
#define  BANKINFOPATHNOFIND  L"BankInfo\\banks\\"



versionManager::versionManager(void):m_bEnName(true)
{
	m_mapEnChName.insert(std::make_pair(L"Main", L"主程序") );
    m_mapEnChName.insert(std::make_pair(L"Html", L"页面内容") );
	m_mapEnChName.insert(std::make_pair(L"Config", L"配置信息") );
}

versionManager::~versionManager(void)
{
}


versionManager * versionManager::m_instance = NULL;

versionManager * versionManager::getHinstance()
{
	if(!m_instance)
		m_instance = new versionManager();

	return m_instance;
}


std::wstring versionManager::getModulePath()
{
	wchar_t wszPath[MAX_PATH];
	std::wstring wcsPath;
	
	::GetModuleFileName(NULL, wszPath, MAX_PATH);
	TCHAR* pSlash = _tcsrchr(wszPath, '\\');
	*pSlash = '\0';

	wcsPath = wszPath;
	return wcsPath;
}

bool versionManager::getAllVersion(VMMAPDEF& mapVersion, MONEYHUBVERSION index, bool bEnName)
{
	bool bReturn = true;
	m_bEnName = bEnName;

	mapVersion.clear();

	switch(index)
	{
	case ALLVERSION:
		getMainModuleVersion(mapVersion);
		getBankModuleVersion(mapVersion);
		getOtherModuleVersion(mapVersion);
		break;
	case MAINVERSION:
		getMainModuleVersion(mapVersion);
		break;
	case BANKVERSION:
		getBankModuleVersion(mapVersion);
		break;
	case OTHERVERSION:
		getOtherModuleVersion(mapVersion);
		break;

	default: 
		break;
	}

	return bReturn;
}




void versionManager::getMainModuleVersion(VMMAPDEF& mapVersion)
{
	std::string   strVersion = "";
	TCHAR szBankPath[1000];

	_tcscpy_s(szBankPath, getModulePath().c_str());
	_tcscat_s(szBankPath, _T("\\MoneyHub.exe"));

	CFileVersionInfo vinfo;
	if (vinfo.Create(szBankPath))
	{
		tstring strFileVersion = vinfo.GetFileVersion();
		replace(strFileVersion.begin(), strFileVersion.end(), ',', '.');
		strFileVersion.erase(remove(strFileVersion.begin(), strFileVersion.end(), ' '), strFileVersion.end());
		strVersion = std::string(CT2A(strFileVersion.c_str()));
	}
	else 
		return ;

	USES_CONVERSION;
	if(m_bEnName)
		mapVersion.insert(std::make_pair(L"Main",A2W(strVersion.c_str()) ) );
	else
	{
	    VMMAPDEF::iterator it = m_mapEnChName.find(L"Main");
		if( it != m_mapEnChName.end() )
			mapVersion.insert(std::make_pair(it->second.c_str(), A2W(strVersion.c_str()) ) );
	}
}

void versionManager::getBankModuleVersion(VMMAPDEF& mapVersion)
{
	std::wstring wsModulePath = getModulePath();
	wsModulePath += L"\\";
	wsModulePath += BANKINFOPATH;

	if(false == traverseFile((LPWSTR)wsModulePath.c_str(), NULL, mapVersion) )
		return ;
}


void versionManager::getOtherModuleVersion(VMMAPDEF& mapVersion)
{
	char vs[255];
	std::vector <std::wstring> vecWcspath;

	std::wstring wcsPath = getModulePath();
	std::wstring wcsTmp = wcsPath + L"\\Html\\info.chk";
	vecWcspath.push_back(wcsTmp);
	wcsTmp = wcsPath + L"\\Config\\info.chk";
	vecWcspath.push_back(wcsTmp);

	USES_CONVERSION;
	for(size_t i=0; i<vecWcspath.size(); i++)
	{
		if(  getVersionFromChk(vs,W2A(vecWcspath[i].c_str())) )
		{
			wchar_t wcsModuleName[MAX_PATH] = {0};
			wcscpy_s(wcsModuleName, _countof(wcsModuleName), vecWcspath[i].c_str() );
			*( wcsrchr(wcsModuleName, L'\\') ) = L'\0';
			wchar_t *p = wcsrchr(wcsModuleName, L'\\');

			if(m_bEnName)
				mapVersion.insert(std::make_pair(p+1, A2W(vs) ));
			else
			{
				VMMAPDEF::iterator it = m_mapEnChName.find(p+1);
				if( it != m_mapEnChName.end() )
					mapVersion.insert(std::make_pair(it->second.c_str(), A2W(vs) ));
			}
		}
	}

	return ;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
bool  versionManager::traverseFile(LPWSTR path, LPWSTR wName, VMMAPDEF& mapVersion )
{
	USES_CONVERSION;
	WIN32_FIND_DATAW  fw;
	HANDLE hFind= FindFirstFileW(path,&fw);

	if(hFind == INVALID_HANDLE_VALUE)
		return false;
	LPVOID p = (LPVOID)&fw;

	do
	{
		if(wcscmp(fw.cFileName,L".") == 0 || wcscmp(fw.cFileName,L"..") == 0 || wcscmp(fw.cFileName,L".svn") == 0)
			continue;

		if(fw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::wstring wStr = getModulePath();
			wStr += L"\\";
			wStr += BANKINFOPATHNOFIND;
			wStr += fw.cFileName ;
			wStr += L"\\*.*";

			if(false == traverseFile((LPWSTR)wStr.c_str(), fw.cFileName, mapVersion))
				return false;
		}
		else  
		{
			if(0 == wcscmp(fw.cFileName,L"info.chk"))//analysis version from info.chk
			{
				std::wstring wsXmlPath = getModulePath();
				wsXmlPath += L"\\";
				wsXmlPath += BANKINFOPATHNOFIND;

				if(!wName)
					continue;

				wsXmlPath += wName;
				wsXmlPath += L'\\';
				wsXmlPath += fw.cFileName;

				char vs[255] = {0};
				char szBankName[MAX_PATH] = {0};

				if(false == getVersionFromChk(vs,W2A(wsXmlPath.c_str()), szBankName) )//得到版本号	
					continue;

				if(m_bEnName)
					mapVersion.insert(std::make_pair(wName, A2W(vs) ));
				else
				{
					mapVersion.insert(std::make_pair(A2W(szBankName), A2W(vs) ));
				}

				FindClose(hFind);
				return true;
			}
		}

		p=(LPVOID)fw.cFileName;
	}
	while( FindNextFile(hFind,&fw) );  

	FindClose(hFind);

	return true;
}



bool versionManager::getVersionFromChk(LPSTR vs , LPCSTR lpCHKFileName, LPSTR szBankName)
{
	//char * content= NULL;//content[8000];
	char content[80000] = {0};
	UINT dwSize = 0;
	if(false == chkToXml(lpCHKFileName,content,&dwSize))
	{
		return false;
	}

	TiXmlDocument xmlDoc;

	xmlDoc.Parse((const char*)content); 

	if (xmlDoc.Error())
	{
		return false;
	}

	const TiXmlNode* pRoot = xmlDoc.FirstChild("main"); // ANSI string 
	if (NULL == pRoot)
	{
		return false;
	}

	const TiXmlNode* pStatus = pRoot->FirstChild("category");
	if (NULL == pStatus)
	{
		return false;
	}

	const TiXmlNode* pStatusId = pStatus->FirstChild("site");
	if (NULL == pStatusId)//为Html或者Config的info.chk，亦或者出错
	{
		const TiXmlElement* pCGElement = pStatus->ToElement();
		if(NULL == pCGElement)
		{
			return false;
		}

		if( pCGElement->Attribute("version") != 0 )
			strcpy_s(vs,20, pCGElement->Attribute("version") );
		else
		{
			return false;
		}

		return true;
	}

	const TiXmlElement* pStatusIdElement = pStatusId->ToElement();
	if(NULL == pStatusIdElement)
	{
		return false;
	}

	if(szBankName)
	{
		USES_CONVERSION;
		if( pStatusIdElement->Attribute("name") != 0 )
		{
			strcpy_s(szBankName,255, W2A(AToW(pStatusIdElement->Attribute("name")).c_str()) );
		}
		else
		 	return false;
	}

	if( pStatusIdElement->Attribute("version") != 0 )
		strcpy_s(vs,20, pStatusIdElement->Attribute("version") );
	else
		return false;

	return true;
}




bool versionManager::chkToXml(LPCSTR lpChkFileName , LPSTR  lpContentXml, UINT *pLen)
{
	// 读文件
	HANDLE hFile = CreateFileA(lpChkFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	*pLen = 0 ;

	if (hFile != INVALID_HANDLE_VALUE)
	{
		// 读取文件长度
		DWORD dwLength = GetFileSize(hFile, NULL);
		unsigned char* lpBuffer = new unsigned char[dwLength + 1];

		if (lpBuffer == NULL)
		{
			CloseHandle(hFile);
			return false;
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			delete []lpBuffer;
			CloseHandle(hFile);
			return false;
		}
		CloseHandle(hFile);

		unsigned char* content = new unsigned char[dwRead];

		if (content == NULL)
		{
			delete []lpBuffer;
			return false;
		}

		int contentLength = unPackCHK(lpBuffer, dwRead, content);
		delete []lpBuffer;

		if (contentLength < 0)
		{
			return false;
		}

		//lpContentXml = new char[contentLength];
		memcpy(lpContentXml,content,contentLength );
		*(lpContentXml+contentLength) = '\0';

		*pLen = contentLength;
		delete []content;
		return true;

	}
	else
	{
		return false;
	}
}
