#include "stdafx.h"
#include "export.h"
#include "ComManager.h"
#include "IEDirRedirector.h"
#include "HistoryManagerXP.h"
#include "HistoryManagerNormal.h"


bool BankProtector::Init()
{
	//CComManager::GetInstance()->Init();
	DWORD dwVersion = GetVersion(); 
	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

 	if (dwMajorVersion < 5 || (dwMajorVersion == 5 && dwMinorVersion < 2))
		CHistoryManagerXP::GetInstance()->Init();
	else
		CHistoryManager::GetInstance()->Init();

	return true;
}

bool BankProtector::CleanHistory()
{
	BOOL bRet;

	DWORD dwVersion = GetVersion(); 
	DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if (dwMajorVersion < 5 || (dwMajorVersion == 5 && dwMinorVersion < 2))
		bRet = CHistoryManagerXP::GetInstance()->CleanHistory();
	else
		bRet = CHistoryManager::GetInstance()->CleanHistory();

	return bRet ? true : false;
}

bool BankProtector::Finalize()
{
	//CComManager::GetInstance()->Finalize();
	//CHistoryManager::GetInstance()->Finalize();
	return true;
}