#pragma once
#include <AclAPI.h>
#include <ShlObj.h>
#include <algorithm>
#include "../../Include/ConvertBase.h"
#include "../../ThirdParty/detours/detours.h"


enum RedirectDirType
{
	Redirect_None,
	Redirect_Cache,
	Redirect_Cookie
};

struct IEDirRedirctData
{
	DWORD dwDirType;
	RedirectDirType eType;
	DWORD dwPathLen;
	wchar_t szPath[MAX_PATH];
} g_IERedirectDirData[] =
{
	{ CSIDL_COOKIES, Redirect_Cookie, 0 },
	{ CSIDL_RECENT, Redirect_Cache, 0 },
	{ CSIDL_HISTORY, Redirect_Cache, 0 },
	{ CSIDL_INTERNET_CACHE, Redirect_Cache, 0 },
};

TCHAR g_szPath[MAX_PATH] = { 0 };

inline void ToLowerSrc(std::wstring & d)
{
	std::transform (d.begin(), d.end(), d.begin(), (int(*)(int))tolower);
}

bool Redirect(LPCWSTR lpszFileName, std::wstring &strRedirect)
{
	std::wstring strFileName = lpszFileName;
	ToLowerSrc(strFileName);

	for (int i = 0; i < _countof(g_IERedirectDirData); i++)
	{
		if (g_IERedirectDirData[i].eType == Redirect_None)
			continue;

		if (strFileName.length() >= g_IERedirectDirData[i].dwPathLen && wcsncmp(strFileName.c_str(), g_IERedirectDirData[i].szPath, g_IERedirectDirData[i].dwPathLen) == 0)
		{
			switch (g_IERedirectDirData[i].eType)
			{
				case Redirect_Cookie:
					strRedirect = std::wstring(g_szPath) + L"\\" + (lpszFileName + g_IERedirectDirData[i].dwPathLen);
					break;
				case Redirect_Cache:
					strRedirect = std::wstring(g_szPath) + L"\\" + (lpszFileName + g_IERedirectDirData[i].dwPathLen);
					break;
			}
			return true;
		}
	}
	return false;
}

bool Redirect(LPCSTR lpszFileName, std::wstring &strRedirect)
{
	strRedirect = AToW(lpszFileName, -1, CP_Ansi);
	return Redirect(strRedirect.c_str(), strRedirect);
}

//////////////////////////////////////////////////////////////////////////

HANDLE (WINAPI * OldCreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileA;
HANDLE (WINAPI * OldCreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileW;
HANDLE WINAPI TuoCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	std::wstring strFileName;
	Redirect(lpFileName, strFileName);
	return OldCreateFileW(strFileName.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
HANDLE WINAPI TuoCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	std::wstring strFileName;
	if (Redirect(lpFileName, strFileName))
		return OldCreateFileW(strFileName.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	else
		return OldCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HFILE (WINAPI * OldOpenFile)(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) = OpenFile;
HFILE WINAPI TuoOpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
	std::wstring strFileName = AToW(lpFileName, -1, CP_Ansi);
	if (Redirect(strFileName.c_str(), strFileName))
		return OldOpenFile(WToA(strFileName, CP_Ansi).c_str(), lpReOpenBuff, uStyle);
	else
		return OldOpenFile(lpFileName, lpReOpenBuff, uStyle);
}

DWORD (WINAPI * OldGetFileAttributesA)(LPCSTR lpFileName) = GetFileAttributesA;
DWORD (WINAPI * OldGetFileAttributesW)(LPCWSTR lpFileName) = GetFileAttributesW;
DWORD WINAPI TuoGetFileAttributesA(LPCSTR lpFileName)
{
	std::wstring strFileName;
	if (Redirect(lpFileName, strFileName))
		return TRUE;
	else
		return OldGetFileAttributesW(strFileName.c_str());
}
DWORD WINAPI TuoGetFileAttributesW(LPCWSTR lpFileName)
{
	std::wstring strFileName;
	if (Redirect(lpFileName, strFileName))
		return TRUE;
	else
		return OldGetFileAttributesW(lpFileName);
}

BOOL (WINAPI * OldGetFileAttributesExA)(LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation) = GetFileAttributesExA;
BOOL (WINAPI * OldGetFileAttributesExW)(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation) = GetFileAttributesExW;
BOOL WINAPI TuoGetFileAttributesExA(LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
{
	std::wstring strFileName;
	Redirect(lpFileName, strFileName);
	return OldGetFileAttributesExW(strFileName.c_str(), fInfoLevelId, lpFileInformation);
}
BOOL WINAPI TuoGetFileAttributesExW(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
{
	std::wstring strFileName;
	if (Redirect(lpFileName, strFileName))
		return OldGetFileAttributesExW(strFileName.c_str(), fInfoLevelId, lpFileInformation);
	else
		return OldGetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);
}

BOOL (WINAPI * OldSetFileAttributesA)(LPCSTR lpFileName, DWORD dwFileAttributes) = SetFileAttributesA;
BOOL (WINAPI * OldSetFileAttributesW)(LPCWSTR lpFileName, DWORD dwFileAttributes) = SetFileAttributesW;
BOOL WINAPI TuoSetFileAttributesA(LPCSTR lpFileName, DWORD dwFileAttributes)
{
	std::wstring strFileName;
	Redirect(lpFileName, strFileName);
	return OldSetFileAttributesW(strFileName.c_str(), dwFileAttributes);
	return TRUE;
}
BOOL WINAPI TuoSetFileAttributesW(LPCWSTR lpFileName, DWORD dwFileAttributes)
{
	return TRUE;
	std::wstring strFileName;
	if (Redirect(lpFileName, strFileName))
		return OldSetFileAttributesW(strFileName.c_str(), dwFileAttributes);
	else
		return OldSetFileAttributesW(lpFileName, dwFileAttributes);
}

HANDLE (WINAPI * OldFindFirstFileA)(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) = FindFirstFileA;
HANDLE (WINAPI * OldFindFirstFileW)(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) = FindFirstFileW;
HANDLE WINAPI TuoFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
	std::wstring strFileName = AToW(lpFileName, -1, CP_Ansi);
	if (Redirect(strFileName.c_str(), strFileName))
		return OldFindFirstFileA(WToA(strFileName, CP_Ansi).c_str(), lpFindFileData);
	else
		return OldFindFirstFileA(lpFileName, lpFindFileData);
}
HANDLE WINAPI TuoFindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData)
{
	std::wstring strFileName;
	if (Redirect(lpFileName, strFileName))
		return OldFindFirstFileW(strFileName.c_str(), lpFindFileData);
	else
		return OldFindFirstFileW(lpFileName, lpFindFileData);
}

HANDLE (WINAPI * OldFindFirstFileExA)(LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExA;
HANDLE (WINAPI * OldFindFirstFileExW)(LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExW;
HANDLE WINAPI TuoFindFirstFileExA(LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{
	std::wstring strFileName = AToW(lpFileName, -1, CP_Ansi);
	if (Redirect(strFileName.c_str(), strFileName))
		return OldFindFirstFileExA(WToA(strFileName, CP_Ansi).c_str(), fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
	else
		return OldFindFirstFileExA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
}
HANDLE WINAPI TuoFindFirstFileExW(LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{
	std::wstring strFileName;
	if (Redirect(lpFileName, strFileName))
		return OldFindFirstFileExW(strFileName.c_str(), fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
	else
		return OldFindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
}

BOOL (WINAPI * OldCreateDirectoryA)(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryA;
BOOL (WINAPI * OldCreateDirectoryW)(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryW;
BOOL WINAPI TuoCreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	std::wstring strFileName;
	Redirect(lpPathName, strFileName);
	return OldCreateDirectoryW(strFileName.c_str(), lpSecurityAttributes);
}
BOOL WINAPI TuoCreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	std::wstring strFileName;
	if (Redirect(lpPathName, strFileName))
		return OldCreateDirectoryW(strFileName.c_str(), lpSecurityAttributes);
	else
		return OldCreateDirectoryW(lpPathName, lpSecurityAttributes);
}

BOOL (WINAPI * OldCreateDirectoryExA)(LPCSTR lpTemplateDirectory, LPCSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryExA;
BOOL (WINAPI * OldCreateDirectoryExW)(LPCWSTR lpTemplateDirectory, LPCWSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryExW;
BOOL WINAPI TuoCreateDirectoryExA(LPCSTR lpTemplateDirectory, LPCSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	std::wstring strFileName = AToW(lpNewDirectory, -1, CP_Ansi);
	if (Redirect(strFileName.c_str(), strFileName))
		return OldCreateDirectoryExA(lpTemplateDirectory, WToA(strFileName, CP_Ansi).c_str(), lpSecurityAttributes);
	else
		return OldCreateDirectoryExA(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);
}
BOOL WINAPI TuoCreateDirectoryExW(LPCWSTR lpTemplateDirectory, LPCWSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	std::wstring strFileName;
	if (Redirect(lpNewDirectory, strFileName))
		return OldCreateDirectoryExW(lpTemplateDirectory, strFileName.c_str(), lpSecurityAttributes);
	else
		return OldCreateDirectoryExW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);
}

DWORD (WINAPI * OldSearchPathA)(LPCSTR lpPath, LPCSTR lpFileName, LPCSTR lpExtension, DWORD nBufferLength, LPSTR lpBuffer, LPSTR* lpFilePart) = SearchPathA;
DWORD (WINAPI * OldSearchPathW)(LPCWSTR lpPath, LPCWSTR lpFileName, LPCWSTR lpExtension, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR* lpFilePart) = SearchPathW;
DWORD WINAPI TuoSearchPathA(LPCSTR lpPath, LPCSTR lpFileName, LPCSTR lpExtension, DWORD nBufferLength, LPSTR lpBuffer, LPSTR* lpFilePart)
{
	std::wstring strFileName = AToW(lpFileName, -1, CP_Ansi);
	if (Redirect(strFileName.c_str(), strFileName))
		return OldSearchPathA(WToA(strFileName, CP_Ansi).c_str(), lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
	else
		return OldSearchPathA(lpFileName, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
}
DWORD WINAPI TuoSearchPathW(LPCWSTR lpPath, LPCWSTR lpFileName, LPCWSTR lpExtension, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR* lpFilePart)
{
	std::wstring strFileName;
	if (lpPath &&  Redirect(lpPath, strFileName))
		return OldSearchPathW(strFileName.c_str(), lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
	else
		return OldSearchPathW(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
}

BOOL (WINAPI * OldDeleteFileA)(LPCSTR lpFileName) = DeleteFileA;
BOOL (WINAPI * OldDeleteFileW)(LPCWSTR lpFileName) = DeleteFileW;
BOOL WINAPI TuoDeleteFileA(LPCSTR lpFileName)
{
	std::wstring strFileName;
	Redirect(lpFileName, strFileName);
	return OldDeleteFileW(strFileName.c_str());
}
BOOL WINAPI TuoDeleteFileW(LPCWSTR lpFileName)
{
	std::wstring strFileName;
	if (Redirect(lpFileName, strFileName))
		return OldDeleteFileW(strFileName.c_str());
	else
		return OldDeleteFileW(lpFileName);
}

DWORD (WINAPI * OldSetNamedSecurityInfoA)(LPSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl) = SetNamedSecurityInfoA;
DWORD (WINAPI * OldSetNamedSecurityInfoW)(LPWSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl) = SetNamedSecurityInfoW;
DWORD WINAPI TuoSetNamedSecurityInfoA(LPSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl)
{
	std::wstring strFileName;
	Redirect(pObjectName, strFileName);
	return OldSetNamedSecurityInfoW((LPWSTR)strFileName.c_str(), ObjectType, SecurityInfo, psidOwner, psidGroup, pDacl, pSacl);
}
DWORD WINAPI TuoSetNamedSecurityInfoW(LPWSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl)
{
	std::wstring strFileName;
	if (Redirect(pObjectName, strFileName))
		return OldSetNamedSecurityInfoW((LPWSTR)strFileName.c_str(), ObjectType, SecurityInfo, psidOwner, psidGroup, pDacl, pSacl);
	else
		return OldSetNamedSecurityInfoW(pObjectName, ObjectType, SecurityInfo, psidOwner, psidGroup, pDacl, pSacl);
}

BOOL (WINAPI * OldGetDiskFreeSpaceExA)(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes) = GetDiskFreeSpaceExA;
BOOL (WINAPI * OldGetDiskFreeSpaceExW)(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes) = GetDiskFreeSpaceExW;
BOOL WINAPI TuoGetDiskFreeSpaceExA(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	std::wstring strFileName;
	Redirect(lpDirectoryName, strFileName);
	return OldGetDiskFreeSpaceExW(strFileName.c_str(), lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
}
BOOL WINAPI TuoGetDiskFreeSpaceExW(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	std::wstring strFileName;
	if (Redirect(lpDirectoryName, strFileName))
		return OldGetDiskFreeSpaceExW(strFileName.c_str(), lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
	else
		return OldGetDiskFreeSpaceExW(lpDirectoryName, lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
}

//////////////////////////////////////////////////////////////////////////

void RedirectInit()
{
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, g_szPath);
	_tcscat_s(g_szPath, _T("\\Bank"));
	::CreateDirectory(g_szPath, NULL);
	_tcscat_s(g_szPath, _T("\\Data"));
	::CreateDirectory(g_szPath, NULL);

	for (int i = 0; i < _countof(g_IERedirectDirData); i++)
	{
		::SHGetFolderPath(NULL, g_IERedirectDirData[i].dwDirType, NULL, SHGFP_TYPE_CURRENT, g_IERedirectDirData[i].szPath);
		g_IERedirectDirData[i].dwPathLen = _tcslen(g_IERedirectDirData[i].szPath);
		_tcslwr_s(g_IERedirectDirData[i].szPath);
	}

	DetourTransactionBegin();
	DetourUpdateThread(::GetCurrentThread());

	DetourAttach((PVOID*)&OldCreateFileA, TuoCreateFileA);
	DetourAttach((PVOID*)&OldCreateFileW, TuoCreateFileW);
	DetourAttach((PVOID*)&OldOpenFile, TuoOpenFile);
	DetourAttach((PVOID*)&OldGetFileAttributesA, TuoGetFileAttributesA);
	DetourAttach((PVOID*)&OldGetFileAttributesW, TuoGetFileAttributesW);
	DetourAttach((PVOID*)&OldGetFileAttributesExA, TuoGetFileAttributesExA);
	DetourAttach((PVOID*)&OldGetFileAttributesExW, TuoGetFileAttributesExW);
	DetourAttach((PVOID*)&OldSetFileAttributesA, TuoSetFileAttributesA);
	DetourAttach((PVOID*)&OldSetFileAttributesW, TuoSetFileAttributesW);
	DetourAttach((PVOID*)&OldFindFirstFileA, TuoFindFirstFileA);
	DetourAttach((PVOID*)&OldFindFirstFileW, TuoFindFirstFileW);
	DetourAttach((PVOID*)&OldFindFirstFileExA, TuoFindFirstFileExA);
	DetourAttach((PVOID*)&OldFindFirstFileExW, TuoFindFirstFileExW);
	DetourAttach((PVOID*)&OldCreateDirectoryA, TuoCreateDirectoryA);
	DetourAttach((PVOID*)&OldCreateDirectoryW, TuoCreateDirectoryW);
	DetourAttach((PVOID*)&OldCreateDirectoryExA, TuoCreateDirectoryExA);
	DetourAttach((PVOID*)&OldCreateDirectoryExW, TuoCreateDirectoryExW);
	DetourAttach((PVOID*)&OldSearchPathA, TuoSearchPathA);
	DetourAttach((PVOID*)&OldSearchPathW, TuoSearchPathW);
	DetourAttach((PVOID*)&OldDeleteFileA, TuoDeleteFileA);
	DetourAttach((PVOID*)&OldDeleteFileW, TuoDeleteFileW);
	DetourAttach((PVOID*)&OldSetNamedSecurityInfoA, TuoSetNamedSecurityInfoA);
	DetourAttach((PVOID*)&OldSetNamedSecurityInfoW, TuoSetNamedSecurityInfoW);
	DetourAttach((PVOID*)&OldGetDiskFreeSpaceExA, TuoGetDiskFreeSpaceExA);
	DetourAttach((PVOID*)&OldGetDiskFreeSpaceExW, TuoGetDiskFreeSpaceExW);

	DetourTransactionCommit();
}
