#pragma once

#include <vector>

typedef BOOL (WINAPI *ENUMPROCESSES)(DWORD* pProcessIds, DWORD cb, DWORD* pBytesReturned);
typedef BOOL (WINAPI *ENUMPROCESSMODULES)(HANDLE hProcess, HMODULE* lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD (WINAPI *GETMODULEBASENAME)(HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize);

class CBankProcessMgr
{
public:
	CBankProcessMgr(void);
	~CBankProcessMgr(void);

public:
	int GetProcList(LPCTSTR szName, std::vector<HANDLE>& vecProcess);
	void TermProcList(const std::vector<HANDLE>& vecProcess);

private:
	static HMODULE m_hDll;

	static ENUMPROCESSES m_fEnumProcesses;
	static ENUMPROCESSMODULES m_fEnumProcessModules;
	static GETMODULEBASENAME m_fGetModuleBaseName;
};
