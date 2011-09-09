#include "StdAfx.h"
#include "BankProcessMgr.h"
#include <TlHelp32.h>

HMODULE CBankProcessMgr::m_hDll = NULL;
ENUMPROCESSES CBankProcessMgr::m_fEnumProcesses;
ENUMPROCESSMODULES CBankProcessMgr::m_fEnumProcessModules;
GETMODULEBASENAME CBankProcessMgr::m_fGetModuleBaseName;

CBankProcessMgr::CBankProcessMgr(void)
{
	if (m_hDll == NULL)
	{
		m_hDll = LoadLibrary(_T("psapi.dll"));
		if (m_hDll != NULL)
		{
			m_fEnumProcesses = (ENUMPROCESSES)GetProcAddress(m_hDll, "EnumProcesses");
			m_fEnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress(m_hDll, "EnumProcessModules");
			#ifdef _UNICODE
			m_fGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(m_hDll, "GetModuleBaseNameW");
			#else
			m_fGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(m_hDll, "GetModuleBaseNameA");
			#endif

			if (m_fEnumProcesses == NULL || m_fEnumProcessModules == NULL || m_fGetModuleBaseName == NULL)
			{
				FreeLibrary(m_hDll);
				m_hDll = NULL;
			}
		}
	}
}

CBankProcessMgr::~CBankProcessMgr(void)
{
	if (m_hDll)
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
}

int CBankProcessMgr::GetProcList(LPCTSTR szName, std::vector<HANDLE>& vecProcess)
{
	/////////////////////////////////first
	// 	if (m_hDll == NULL)
	// 		return 0;
	// 
	// 	vecProcess.clear();
	// 
	// 	DWORD arrProcesses[1024];
	// 	DWORD dwNeeded;
	// 
	// 	if (!m_fEnumProcesses(arrProcesses, sizeof(arrProcesses), &dwNeeded))
	// 		return 0;
	// 
	// 	DWORD cProcesses = dwNeeded / sizeof(DWORD);
	// 
	// 	for (DWORD i = 0; i < cProcesses; i++)
	// 	{
	// 		if (arrProcesses[i] != 0)
	// 		{
	// 			TCHAR szProcessName[MAX_PATH] = _T("");
	// 			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, arrProcesses[i]);//财金汇安全策略不允许
	// 
	// 			if (NULL != hProcess)
	// 			{
	// 				HMODULE hMod;
	// 				DWORD dwNeeded;
	// 
	// 				if (m_fEnumProcessModules(hProcess, &hMod, sizeof(hMod), &dwNeeded))
	// 				{
	// 					m_fGetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
	// 					if (0 == _tcsicmp(szProcessName, szName))
	// 						vecProcess.push_back(hProcess);
	// 				}
	// 			}
	// 
	// 			CloseHandle(hProcess);
	// 		}
	// 	}
	// 
	// 	return vecProcess.size();
	////////////////////////////////////////second
	if (m_hDll == NULL)
		return 0;

	vecProcess.clear();

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if( INVALID_HANDLE_VALUE == hSnap)
		return 0;

	PROCESSENTRY32W pew = { sizeof(PROCESSENTRY32W) };
	if( Process32FirstW(hSnap, &pew))
	{
		do 
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pew.th32ProcessID);
			if( hProcess )
			{
				if (0 == _tcsicmp(pew.szExeFile, szName))
					vecProcess.push_back(hProcess);
			}
		} while (Process32NextW(hSnap, &pew));
	}

	CloseHandle(hSnap);
	return vecProcess.size();
	/////////////////////////////////////////
}

void CBankProcessMgr::TermProcList(const std::vector<HANDLE>& vecProcess)
{
	for (size_t i = 0; i < vecProcess.size(); i++)
		TerminateProcess(vecProcess[i], 0);
}
