#pragma once
#include "ListManager.h"


class CResourceManager
{

public:

	std::string GetRootXMLFile(LPTSTR pfn = NULL);
	std::string GetFileContent(WebsiteType eWebsiteType, LPCTSTR lpszSiteID, LPCTSTR lpszFileName, bool bCHK=false);
	/**
	*         return //BankInfo//banks//$param2//$param3
	*/
	std::wstring GetFilePath(WebsiteType eWebsiteType, LPCTSTR lpszSiteID, LPCTSTR lpszFileName);

	static CResourceManager* _();
	static void Initialize();
	
private:
};
