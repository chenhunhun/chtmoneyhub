#include "stdafx.h"
#include "Util.h"
#include "ResourceManager.h"
#include "../../Encryption/SHA1/sha.h"
#include "../../Encryption/CHKFile/CHK.h"

std::string CResourceManager::GetRootXMLFile(LPTSTR pfn)
{
#ifdef SINGLE_PROCESS
	//return GetFileContent(Website_None, _T(""), _T("info.xml"), false);
	return GetFileContent(Website_Bank, pfn, _T("info.xml"), false);
#else
	//return GetFileContent(Website_None, _T(""), _T("info.chk"), true);  
	return GetFileContent(Website_Bank, pfn, _T("info.chk"), true); 
#endif
}

std::string CResourceManager::GetFileContent(WebsiteType eWebsiteType, LPCTSTR lpszSiteID, LPCTSTR lpszFileName, bool bCHK)
{	
	std::wstring strPath = GetFilePath(eWebsiteType, lpszSiteID, lpszFileName);
	HANDLE hFile = ::CreateFile(strPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
		return "";

	
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	std::string strFileContent;
	unsigned char *pData = new unsigned char[dwFileSize + 1];
	::ReadFile(hFile, pData, dwFileSize, &dwFileSize, NULL);
	pData[dwFileSize] = 0;

	if (bCHK)
	{
		char* content = new char[dwFileSize + 1];
		int contentLength = unPackCHK(pData, dwFileSize, (unsigned char *)content);
		if (contentLength < 0)
		{
			delete []content;
			delete []pData;

			CloseHandle(hFile);

			return "";
		}

		content[contentLength] = '\0';
		strFileContent = content;
		delete []content;
	}
	else
	{
		strFileContent = (char *)pData;
	}

	delete []pData;
	::CloseHandle(hFile);

	return strFileContent;
}

std::wstring CResourceManager::GetFilePath(WebsiteType eWebsiteType, LPCTSTR lpszSiteID, LPCTSTR lpszFileName)
{
	//由于UAC问题，此路径有可能放到%appdata%中
	//std::wstring strPath = GetModulePath();
	//USES_CONVERSION;
	//std::wstring strPath = A2W(getAppdataPath());
	std::wstring strPath = GetModulePath();


	strPath += _T("\\BankInfo");
	if (eWebsiteType != Website_None)
	{
		static LPCTSTR lpszCategoryName[] = { NULL, _T("banks") };
		strPath += _T("\\");
		strPath += lpszCategoryName[eWebsiteType];
	}
	if (lpszSiteID[0])
	{
		strPath += _T("\\");
		strPath += lpszSiteID;
	}
	strPath += _T("\\");
	strPath += lpszFileName;
	return strPath;
}

//////////////////////////////////////////////////////////////////////////

CResourceManager *g_pResManager = NULL;

CResourceManager* CResourceManager::_()
{
	return g_pResManager;
}

void CResourceManager::Initialize()
{
	g_pResManager = new CResourceManager();
}

