#include "StdAfx.h"
#include "windows.h"
#include "Psapi.h"
#include "HistoryManagerXP.h"
#include "../../Utils/detours/detours.h"

#include "shellapi.h"
#include "atlstr.h"
#include <accctrl.h>
#include <aclapi.h>

CHistoryManagerXP* CHistoryManagerXP::m_Instance = NULL;

#define	REG_TEMP_FOLDER "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders"

#define TEMP_KEY_COUNT	(4)
LPCWSTR g_TempKeyXp[TEMP_KEY_COUNT] = { L"Cache", L"Cookies", L"History", L"Recent" };

std::wstring g_TempRegKeyDir;

//////////////////////////////////////////////////////////////////////////

typedef LONG NTSTATUS;

#define STATUS_INFO_LEN_MISMATCH 0xC0000004
#define ObjectNameInformation (1)

typedef unsigned long OBJECT_INFORMATION_CLASS;

typedef struct
{
	USHORT Length;
	USHORT MaxLen;
	USHORT *Buffer;
}UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_NAME_INFORMATION { // Information Class 1
	UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef NTSTATUS (WINAPI *NTQUERYOBJECT)(IN HANDLE ObjectHandle,IN OBJECT_INFORMATION_CLASS ObjectInformationClass,OUT PVOID ObjectInformation,IN ULONG ObjectInformationLength,OUT PULONG ReturnLength);
NTQUERYOBJECT NtQueryObject = NULL;

//////////////////////////////////////////////////////////////////////////
// (1) CreateFile

HANDLE (WINAPI *XpNativeCreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
								   DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileW;

HANDLE WINAPI XpDetourCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
								DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	if (lpFileName == NULL)
		return XpNativeCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	if (_wcsnicmp(lpFileName, CHistoryManagerXP::GetInstance()->m_strRedirectPath, CHistoryManagerXP::GetInstance()->m_strRedirectPath.GetLength()) == 0)
	{
		lpSecurityAttributes = NULL;
		dwFlagsAndAttributes = dwFlagsAndAttributes & (~FILE_ATTRIBUTE_HIDDEN) & (~FILE_ATTRIBUTE_READONLY) & (~FILE_ATTRIBUTE_SYSTEM);
	}

	return XpNativeCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

//////////////////////////////////////////////////////////////////////////
// (5) SetFileAttributes

BOOL (WINAPI *XpNativeSetFileAttributesW)(LPCWSTR lpFileName, DWORD dwFileAttributes) = SetFileAttributesW;

BOOL WINAPI XpDetourSetFileAttributesW(LPCWSTR lpFileName, DWORD dwFileAttributes)
{
	if (lpFileName == NULL)
		return XpNativeSetFileAttributesW(lpFileName, dwFileAttributes);

	if (_wcsnicmp(lpFileName, CHistoryManagerXP::GetInstance()->m_strRedirectPath, CHistoryManagerXP::GetInstance()->m_strRedirectPath.GetLength()) == 0)
		return TRUE;

	return XpNativeSetFileAttributesW(lpFileName, dwFileAttributes);
}

//////////////////////////////////////////////////////////////////////////
// (8) CreateDirectory

BOOL (WINAPI *XpNativeCreateDirectoryW)(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryW;

BOOL WINAPI XpDetourCreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	if (lpPathName == NULL)
		return XpNativeCreateDirectoryW(lpPathName, lpSecurityAttributes);

	if (_wcsnicmp(lpPathName, CHistoryManagerXP::GetInstance()->m_strRedirectPath, CHistoryManagerXP::GetInstance()->m_strRedirectPath.GetLength()) == 0)
		lpSecurityAttributes = NULL;

	return XpNativeCreateDirectoryW(lpPathName, lpSecurityAttributes);
}

//////////////////////////////////////////////////////////////////////////
// (9) CreateDirectoryEx

BOOL (WINAPI *XpNativeCreateDirectoryExW)(LPCWSTR lpTemplateDirectory, LPCWSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryExW;

BOOL WINAPI XpDetourCreateDirectoryExW(LPCWSTR lpTemplateDirectory, LPCWSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	if (lpNewDirectory == NULL)
		return XpNativeCreateDirectoryExW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

	if (_wcsnicmp(lpNewDirectory, CHistoryManagerXP::GetInstance()->m_strRedirectPath, CHistoryManagerXP::GetInstance()->m_strRedirectPath.GetLength()) == 0)
		lpSecurityAttributes = NULL;

	return XpNativeCreateDirectoryExW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);
}

//////////////////////////////////////////////////////////////////////////
// Registry functions

typedef LONG (WINAPI * RegQueryValueExWFunc)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
RegQueryValueExWFunc XpNativeRegQueryValueExW = NULL;
LONG WINAPI XpDetourRegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	DWORD dwBufLen = 0;
	if (lpcbData != NULL)
		dwBufLen = *lpcbData;

	LONG nRet = XpNativeRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
	if (hKey == NULL || lpValueName == NULL || (lpData == NULL && lpcbData == NULL))
		return nRet;

	std::wstring strRegKeyDir = CHistoryManagerXP::GetInstance()->GetRegKeyDirByHandle(hKey);
	if (_wcsicmp(strRegKeyDir.c_str(), g_TempRegKeyDir.c_str()) != 0)
		return nRet;

	for (int i = 0; i < TEMP_KEY_COUNT; i++)
	{
		if (_wcsicmp(lpValueName, g_TempKeyXp[i]) == 0)
		{
			std::wstring strPath = CHistoryManagerXP::GetInstance()->m_strRedirectPath + lpValueName;
			*lpcbData = (strPath.size() + 1) * 2;

			if (lpData != NULL)
			{
				if (dwBufLen < *lpcbData)
					nRet = ERROR_MORE_DATA;
				else
				{
					nRet = ERROR_SUCCESS;
					memcpy(lpData, strPath.c_str(), *lpcbData);
				}
			}

			break;
		}
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////

typedef LONG (WINAPI * RegQueryValueWFunc)(HKEY hKey, LPCWSTR lpSubKey, LPWSTR lpValue, PLONG lpcbValue);
RegQueryValueWFunc XpNativeRegQueryValueW = NULL;
LONG WINAPI XpDetourRegQueryValueW(HKEY hKey, LPCWSTR lpSubKey, LPWSTR lpValue, PLONG lpcbValue)
{
	DWORD dwBufLen = 0;
	if (lpcbValue != NULL)
		dwBufLen = *lpcbValue;

	LONG nRet = XpNativeRegQueryValueW(hKey, lpSubKey, lpValue, lpcbValue);
	if (hKey == NULL || lpSubKey == NULL || (lpValue == NULL && lpcbValue == NULL))
		return nRet;

	std::wstring strRegKeyDir = CHistoryManagerXP::GetInstance()->GetRegKeyDirByHandle(hKey);
	if (_wcsicmp(strRegKeyDir.c_str(), g_TempRegKeyDir.c_str()) != 0)
		return nRet;

	for (int i = 0; i < TEMP_KEY_COUNT; i++)
	{
		if (_wcsicmp(lpSubKey, g_TempKeyXp[i]) == 0)
		{
			std::wstring strPath = CHistoryManagerXP::GetInstance()->m_strRedirectPath + lpSubKey;
			*lpcbValue = (strPath.size() + 1) * 2;

			if (lpValue != NULL)
			{
				if ((LONG)dwBufLen < *lpcbValue)
					nRet = ERROR_MORE_DATA;
				else
				{
					nRet = ERROR_SUCCESS;
					memcpy(lpValue, strPath.c_str(), *lpcbValue);
				}
			}

			break;
		}
	}

	return nRet;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CHistoryManagerXP* CHistoryManagerXP::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CHistoryManagerXP();

	return m_Instance;
}

CHistoryManagerXP::CHistoryManagerXP(void)
{
	LPWSTR lpszTempPath = new WCHAR[MAX_PATH + 1];
	int nLength = GetTempPathW(MAX_PATH, lpszTempPath);
	if (nLength > MAX_PATH)
	{
		delete[] lpszTempPath;
		lpszTempPath = new WCHAR[nLength + 1];
		GetTempPathW(nLength, lpszTempPath);
	}

	lpszTempPath[nLength] = '\0';
	CStringW strTempPath = lpszTempPath;	
	delete[] lpszTempPath;

	strTempPath.Replace(L'/', L'\\');
	if (strTempPath[strTempPath.GetLength() - 1] != '\\')
		strTempPath += L"\\";

	m_strWinTemp = strTempPath;

	strTempPath += L"BankTemp";
	if (!CreateDirectoryW(strTempPath, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return;

	SetFileAttributesW(strTempPath, FILE_ATTRIBUTE_HIDDEN);

	LPWSTR pBuf = NULL;

	// short path
	pBuf = m_strRedirectPath.GetBuffer(1024);
	GetShortPathNameW(strTempPath, pBuf, 1024);
	m_strRedirectPath.ReleaseBuffer();
	m_strRedirectPath += L"\\";
	m_strRedirectPath.MakeLower();
}

CHistoryManagerXP::~CHistoryManagerXP(void)
{
}

BOOL CHistoryManagerXP::Init()
{
	PrepareTempDirs();

	HMODULE hKernel32 = ::LoadLibrary(_T("kernel32.dll"));
	if (NULL == ::GetProcAddress(hKernel32, "RegCreateKeyExW"))
		hKernel32 = ::LoadLibrary(_T("advapi32.dll"));

	if (hKernel32)
	{
		XpNativeRegQueryValueExW = (RegQueryValueExWFunc)::GetProcAddress(hKernel32, "RegQueryValueExW");
		XpNativeRegQueryValueW = (RegQueryValueWFunc)::GetProcAddress(hKernel32, "RegQueryValueW");
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)XpNativeCreateFileW, XpDetourCreateFileW);
	DetourAttach(&(PVOID&)XpNativeSetFileAttributesW, XpDetourSetFileAttributesW);
	DetourAttach(&(PVOID&)XpNativeCreateDirectoryW, XpDetourCreateDirectoryW);
	DetourAttach(&(PVOID&)XpNativeCreateDirectoryExW, XpDetourCreateDirectoryExW);
	DetourAttach(&(PVOID&)XpNativeRegQueryValueExW, XpDetourRegQueryValueExW);
	DetourAttach(&(PVOID&)XpNativeRegQueryValueW, XpDetourRegQueryValueW);

	return NO_ERROR == DetourTransactionCommit();
}

BOOL CHistoryManagerXP::Finalize()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach(&(PVOID&)XpNativeCreateFileW, XpDetourCreateFileW);
	DetourDetach(&(PVOID&)XpNativeSetFileAttributesW, XpDetourSetFileAttributesW);
	DetourDetach(&(PVOID&)XpNativeCreateDirectoryW, XpDetourCreateDirectoryW);
	DetourDetach(&(PVOID&)XpNativeCreateDirectoryExW, XpDetourCreateDirectoryExW);
	DetourDetach(&(PVOID&)XpNativeRegQueryValueExW, XpDetourRegQueryValueExW);
	DetourDetach(&(PVOID&)XpNativeRegQueryValueW, XpDetourRegQueryValueW);

	return NO_ERROR == DetourTransactionCommit();
}

// 清除临时文件
BOOL CHistoryManagerXP::CleanHistory()
{
	CStringW dir = m_strRedirectPath; 
	return DeleteDir(dir.TrimRight(L'\\'));
}

BOOL CHistoryManagerXP::CleanUpdate()
{
 	CStringW dir = m_strWinTemp + L"BankUpdate";
 	return DeleteDir(dir);

	return TRUE;
}

// 删除目录及文件
BOOL CHistoryManagerXP::DeleteDir(CStringW dir)
{
	SHFILEOPSTRUCTW FileOp; 
	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR; 
	FileOp.hNameMappings = NULL; 
	FileOp.hwnd = NULL; 
	FileOp.lpszProgressTitle = NULL; 
	FileOp.pFrom = dir; 
	FileOp.pTo = NULL; 
	FileOp.wFunc = FO_DELETE; 
	return SHFileOperationW(&FileOp) == 0;
}

BOOL CHistoryManagerXP::PrepareTempDirs()
{
	HMODULE hDll = LoadLibraryW(L"ntdll.dll");
	NtQueryObject = (NTQUERYOBJECT)GetProcAddress(hDll,"NtQueryObject");
	//FreeLibrary(hDll);

	HKEY hTempRegKey;
	RegOpenKeyExA(HKEY_CURRENT_USER, REG_TEMP_FOLDER, 0, KEY_QUERY_VALUE, &hTempRegKey);
	g_TempRegKeyDir = GetRegKeyDirByHandle(hTempRegKey);
	RegCloseKey(hTempRegKey);

	return TRUE;
}

std::wstring CHistoryManagerXP::GetRegKeyDirByHandle(HANDLE hHandle)
{
	HANDLE hHeap = GetProcessHeap();
	DWORD dwSize = 0;
	POBJECT_NAME_INFORMATION pName = (POBJECT_NAME_INFORMATION)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 0x1000);   
	NTSTATUS ns = NtQueryObject(hHandle, ObjectNameInformation, (PVOID)pName, 0x1000, &dwSize);
	DWORD i = 1;
	while (ns == STATUS_INFO_LEN_MISMATCH)
	{
		pName = (POBJECT_NAME_INFORMATION)HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, (LPVOID)pName, 0x1000 * i);
		ns = NtQueryObject(hHandle, ObjectNameInformation, (PVOID)pName, 0x1000, NULL);
		i++;
	}

	std::wstring strDir;
	if (pName->Name.Buffer && pName->Name.Length > 0)
	{
		DWORD dwLength = pName->Name.Length + sizeof(USHORT);
		LPBYTE pBuffer = new BYTE[dwLength];
		memset(pBuffer, 0, dwLength);
		memcpy(pBuffer, pName->Name.Buffer, dwLength);
		std::wstring strDir = (LPWSTR)pName->Name.Buffer;
		delete []pBuffer;
	}

	HeapFree(hHeap,0,pName);

	return strDir;
}
