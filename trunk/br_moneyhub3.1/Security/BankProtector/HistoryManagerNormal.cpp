#include "StdAfx.h"
#include "windows.h"
#include "Psapi.h"
#include "HistoryManagerNormal.h"
#include "../../ThirdParty/detours/detours.h"

#include "shellapi.h"
#include "atlstr.h"
#include <accctrl.h>
#include <AclAPI.h>

// 特别注意不要循环Redirect
// CreateFileA() => thunk => CreateFileW() 
// 也不要放在AppData下

CHistoryManager* CHistoryManager::m_Instance = NULL;

#define TEMP_KEY_COUNT	(4)

class CTempPath
{
public:
	std::wstring strRegKey;
	std::wstring strShortTempDir;
	std::wstring strLongTempDir;
	std::vector<std::wstring> vecShortParts;
	std::vector<std::wstring> vecLongParts;
} g_TempPaths[TEMP_KEY_COUNT];

LPCWSTR g_TempKey[TEMP_KEY_COUNT] = { L"Cache", L"Cookies", L"History", L"Recent" };

#define	REG_TEMP_FOLDER "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders"

//////////////////////////////////////////////////////////////////////////
// (1) CreateFile

HANDLE (WINAPI *NativeCreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
								   DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileA;
HANDLE (WINAPI *NativeCreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
								   DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileW;

HANDLE WINAPI DetourCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	if (lpFileName == NULL)
		return NativeCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	CStringA strFileName = CHistoryManager::GetInstance()->Redirect(CStringA(lpFileName));
	return NativeCreateFileA(strFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI DetourCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
								DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	if (lpFileName == NULL)
		return NativeCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	CStringW strFileName = CHistoryManager::GetInstance()->Redirect(CStringW(lpFileName));
	return NativeCreateFileW(strFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

//////////////////////////////////////////////////////////////////////////
// (2) OpenFile

HFILE (WINAPI *NativeOpenFile)(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) = OpenFile;

HFILE WINAPI DetourOpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
	if (lpFileName == NULL)
		return NativeOpenFile(lpFileName, lpReOpenBuff, uStyle);

	CStringA strFileName = CHistoryManager::GetInstance()->Redirect(CStringA(lpFileName));
	return NativeOpenFile(strFileName, lpReOpenBuff, uStyle);
}

//////////////////////////////////////////////////////////////////////////
// (3) GetFileAttributes

DWORD (WINAPI *NativeGetFileAttributesA)(LPCSTR lpFileName) = GetFileAttributesA;
DWORD (WINAPI *NativeGetFileAttributesW)(LPCWSTR lpFileName) = GetFileAttributesW;

DWORD WINAPI DetourGetFileAttributesA(LPCSTR lpFileName)
{
	if (lpFileName == NULL)
		return NativeGetFileAttributesA(lpFileName);

	CStringA strFileName = CHistoryManager::GetInstance()->Redirect(CStringA(lpFileName));
	return NativeGetFileAttributesA(strFileName);
}

DWORD WINAPI DetourGetFileAttributesW(LPCWSTR lpFileName)
{
	if (lpFileName == NULL)
		return NativeGetFileAttributesW(lpFileName);

	CStringW strFileName = CHistoryManager::GetInstance()->Redirect(CStringW(lpFileName));
	return NativeGetFileAttributesW(strFileName);
}

//////////////////////////////////////////////////////////////////////////
// (4) GetFileAttributesEx

BOOL (WINAPI *NativeGetFileAttributesExA)(LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation) = GetFileAttributesExA;
BOOL (WINAPI *NativeGetFileAttributesExW)(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation) = GetFileAttributesExW;

BOOL WINAPI DetourGetFileAttributesExA(LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
{
	if (lpFileName == NULL)
		return NativeGetFileAttributesExA(lpFileName, fInfoLevelId, lpFileInformation);

	CStringA strFileName = CHistoryManager::GetInstance()->Redirect(CStringA(lpFileName));
	return NativeGetFileAttributesExA(strFileName, fInfoLevelId, lpFileInformation);
}

BOOL WINAPI DetourGetFileAttributesExW(LPCWSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation)
{
	if (lpFileName == NULL)
		return NativeGetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);

	CStringW strFileName = CHistoryManager::GetInstance()->Redirect(CStringW(lpFileName));
	return NativeGetFileAttributesExW(strFileName, fInfoLevelId, lpFileInformation);
}

//////////////////////////////////////////////////////////////////////////
// (5) SetFileAttributes

BOOL (WINAPI *NativeSetFileAttributesA)(LPCSTR lpFileName, DWORD dwFileAttributes) = SetFileAttributesA;
BOOL (WINAPI *NativeSetFileAttributesW)(LPCWSTR lpFileName, DWORD dwFileAttributes) = SetFileAttributesW;

BOOL WINAPI DetourSetFileAttributesA(LPCSTR lpFileName, DWORD dwFileAttributes)
{
	if (lpFileName == NULL)
		return NativeSetFileAttributesA(lpFileName, dwFileAttributes);

	CStringA strFileName = CHistoryManager::GetInstance()->Redirect(CStringA(lpFileName));
	return NativeSetFileAttributesA(strFileName, dwFileAttributes);
}

BOOL WINAPI DetourSetFileAttributesW(LPCWSTR lpFileName, DWORD dwFileAttributes)
{
	if (lpFileName == NULL)
		return NativeSetFileAttributesW(lpFileName, dwFileAttributes);

	CStringW strFileName = CHistoryManager::GetInstance()->Redirect(CStringW(lpFileName));
	return NativeSetFileAttributesW(strFileName, dwFileAttributes);
}

//////////////////////////////////////////////////////////////////////////
// (6) FindFirstFile

HANDLE (WINAPI *NativeFindFirstFileA)(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) = FindFirstFileA;
HANDLE (WINAPI *NativeFindFirstFileW)(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) = FindFirstFileW;

HANDLE WINAPI DetourFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
	if (lpFileName == NULL)
		return NativeFindFirstFileA(lpFileName, lpFindFileData);

	CStringA strFileName = CHistoryManager::GetInstance()->Redirect(CStringA(lpFileName));
	return NativeFindFirstFileA(strFileName, lpFindFileData);
}

HANDLE WINAPI DetourFindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData)
{
	if (lpFileName == NULL)
		return NativeFindFirstFileW(lpFileName, lpFindFileData);

	CStringW strFileName = CHistoryManager::GetInstance()->Redirect(CStringW(lpFileName));
	return NativeFindFirstFileW(strFileName, lpFindFileData);
}

//////////////////////////////////////////////////////////////////////////
// (7) FindFirstFileEx

HANDLE (WINAPI *NativeFindFirstFileExA)(LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, 
										FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExA;
HANDLE (WINAPI *NativeFindFirstFileExW)(LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, 
										FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExW;

HANDLE WINAPI DetourFindFirstFileExA(LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, 
									 FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{
	if (lpFileName == NULL)
		return NativeFindFirstFileExA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

	CStringA strFileName = CHistoryManager::GetInstance()->Redirect(CStringA(lpFileName));
	return NativeFindFirstFileExA(strFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
}

HANDLE WINAPI DetourFindFirstFileExW(LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, 
									 FINDEX_SEARCH_OPS fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{
	if (lpFileName == NULL)
		return NativeFindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

	CStringW strFileName = CHistoryManager::GetInstance()->Redirect(CStringW(lpFileName));
	return NativeFindFirstFileExW(strFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);
}

//////////////////////////////////////////////////////////////////////////
// (8) CreateDirectory

BOOL (WINAPI *NativeCreateDirectoryA)(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryA;
BOOL (WINAPI *NativeCreateDirectoryW)(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryW;

BOOL WINAPI DetourCreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	if (lpPathName == NULL)
		return NativeCreateDirectoryA(lpPathName, lpSecurityAttributes);

	CStringA strPathName = CHistoryManager::GetInstance()->Redirect(CStringA(lpPathName));
	return NativeCreateDirectoryA(strPathName, lpSecurityAttributes);
}

BOOL WINAPI DetourCreateDirectoryW(LPCWSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	if (lpPathName == NULL)
		return NativeCreateDirectoryW(lpPathName, lpSecurityAttributes);

	CStringW strPathName = CHistoryManager::GetInstance()->Redirect(CStringW(lpPathName));
	return NativeCreateDirectoryW(strPathName, lpSecurityAttributes);
}

//////////////////////////////////////////////////////////////////////////
// (9) CreateDirectoryEx

BOOL (WINAPI *NativeCreateDirectoryExA)(LPCSTR lpTemplateDirectory, LPCSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryExA;
BOOL (WINAPI *NativeCreateDirectoryExW)(LPCWSTR lpTemplateDirectory, LPCWSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes) = CreateDirectoryExW;

BOOL WINAPI DetourCreateDirectoryExA(LPCSTR lpTemplateDirectory, LPCSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	if (lpNewDirectory == NULL)
		return NativeCreateDirectoryExA(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

	CStringA strNewDirectory = CHistoryManager::GetInstance()->Redirect(CStringA(lpNewDirectory));
	return NativeCreateDirectoryExA(lpTemplateDirectory, strNewDirectory, lpSecurityAttributes);
}

BOOL WINAPI DetourCreateDirectoryExW(LPCWSTR lpTemplateDirectory, LPCWSTR lpNewDirectory, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	if (lpNewDirectory == NULL)
		return NativeCreateDirectoryExW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

	CStringW strNewDirectory = CHistoryManager::GetInstance()->Redirect(CStringW(lpNewDirectory));
	return NativeCreateDirectoryExW(lpTemplateDirectory, strNewDirectory, lpSecurityAttributes);
}

//////////////////////////////////////////////////////////////////////////
// (10) SearchPath

DWORD (WINAPI *NativeSearchPathA)(LPCSTR lpPath, LPCSTR lpFileName, LPCSTR lpExtension, DWORD nBufferLength, LPSTR lpBuffer, LPSTR* lpFilePart) = SearchPathA;
DWORD (WINAPI *NativeSearchPathW)(LPCWSTR lpPath, LPCWSTR lpFileName, LPCWSTR lpExtension, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR* lpFilePart) = SearchPathW;

DWORD WINAPI DetourSearchPathA(LPCSTR lpPath, LPCSTR lpFileName, LPCSTR lpExtension, DWORD nBufferLength, LPSTR lpBuffer, LPSTR* lpFilePart)
{
	if (lpPath == NULL)
		return NativeSearchPathA(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

	CStringA strPath = CHistoryManager::GetInstance()->Redirect(CStringA(lpPath));
	return NativeSearchPathA(strPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
}

DWORD WINAPI DetourSearchPathW(LPCWSTR lpPath, LPCWSTR lpFileName, LPCWSTR lpExtension, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR* lpFilePart)
{
	if (lpPath == NULL)
		return NativeSearchPathW(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

	CStringW strPath = CHistoryManager::GetInstance()->Redirect(CStringW(lpPath));
	return NativeSearchPathW(strPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);
}

//////////////////////////////////////////////////////////////////////////
// (11) DeleteFile

BOOL (WINAPI *NativeDeleteFileA)(LPCSTR lpFileName) = DeleteFileA;
BOOL (WINAPI *NativeDeleteFileW)(LPCWSTR lpFileName) = DeleteFileW;

BOOL WINAPI DetourDeleteFileA(LPCSTR lpFileName)
{
	if (lpFileName == NULL)
		return NativeDeleteFileA(lpFileName);

	CStringA strFileName = CHistoryManager::GetInstance()->Redirect(CStringA(lpFileName));
	return NativeDeleteFileA(strFileName);
}

BOOL WINAPI DetourDeleteFileW(LPCWSTR lpFileName)
{
	if (lpFileName == NULL)
		return NativeDeleteFileW(lpFileName);

	CStringW strFileName = CHistoryManager::GetInstance()->Redirect(CStringW(lpFileName));
	return NativeDeleteFileW(strFileName);
}

//////////////////////////////////////////////////////////////////////////
// (12) SetNamedSecurityInfo

DWORD (WINAPI *NativeSetNamedSecurityInfoA)(LPSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl) = SetNamedSecurityInfoA;
DWORD (WINAPI *NativeSetNamedSecurityInfoW)(LPWSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl) = SetNamedSecurityInfoW;

DWORD WINAPI DetourSetNamedSecurityInfoA(LPSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl)
{
	if (pObjectName == NULL)
		return NativeSetNamedSecurityInfoA(pObjectName, ObjectType, SecurityInfo, psidOwner, psidGroup, pDacl, pSacl);

	CStringA strObjectName = CHistoryManager::GetInstance()->Redirect(CStringA(pObjectName));
	return NativeSetNamedSecurityInfoA((LPSTR)(LPCSTR)strObjectName, ObjectType, SecurityInfo, psidOwner, psidGroup, pDacl, pSacl);
}

DWORD WINAPI DetourSetNamedSecurityInfoW(LPWSTR pObjectName, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl)
{
	if (pObjectName == NULL)
		return NativeSetNamedSecurityInfoW(pObjectName, ObjectType, SecurityInfo, psidOwner, psidGroup, pDacl, pSacl);

	CStringW strObjectName = CHistoryManager::GetInstance()->Redirect(CStringW(pObjectName));
	return NativeSetNamedSecurityInfoW((LPWSTR)(LPCWSTR)strObjectName, ObjectType, SecurityInfo, psidOwner, psidGroup, pDacl, pSacl);
}

//////////////////////////////////////////////////////////////////////////
// (13) GetDiskFreeSpaceEx

BOOL (WINAPI *NativeGetDiskFreeSpaceExA)(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes) = GetDiskFreeSpaceExA;
BOOL (WINAPI *NativeGetDiskFreeSpaceExW)(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes) = GetDiskFreeSpaceExW;

BOOL WINAPI DetourGetDiskFreeSpaceExA(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	if (lpDirectoryName == NULL)
		return NativeGetDiskFreeSpaceExA(lpDirectoryName, lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);

	CStringA strDirectoryName = CHistoryManager::GetInstance()->Redirect(CStringA(lpDirectoryName));
	return NativeGetDiskFreeSpaceExA(strDirectoryName, lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
}

BOOL WINAPI DetourGetDiskFreeSpaceExW(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	if (lpDirectoryName == NULL)
		return NativeGetDiskFreeSpaceExW(lpDirectoryName, lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);

	CStringW strDirectoryName = CHistoryManager::GetInstance()->Redirect(CStringW(lpDirectoryName));
	return NativeGetDiskFreeSpaceExW(strDirectoryName, lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
}

//////////////////////////////////////////////////////////////////////////
// (14) PathFileExists

BOOL (WINAPI *NativePathFileExistsA)(LPCSTR pszPath) = PathFileExistsA;
BOOL (WINAPI *NativePathFileExistsW)(LPCWSTR pszPath) = PathFileExistsW;

BOOL WINAPI DetourPathFileExistsA(LPCSTR pszPath)
{
	if (pszPath == NULL)
		return NativePathFileExistsA(pszPath);

	CStringA strPath = CHistoryManager::GetInstance()->Redirect(CStringA(pszPath));
	return NativePathFileExistsA(strPath);
}

BOOL WINAPI DetourPathFileExistsW(LPCWSTR pszPath)
{
	if (pszPath == NULL)
		return NativePathFileExistsW(pszPath);

	CStringW strPath = CHistoryManager::GetInstance()->Redirect(CStringW(pszPath));
	return NativePathFileExistsW(strPath);
}

//////////////////////////////////////////////////////////////////////////
// (15) SetCurrentDirectory

BOOL (WINAPI *NativeSetCurrentDirectoryA)(LPCSTR lpPathName) = SetCurrentDirectoryA;
BOOL (WINAPI *NativeSetCurrentDirectoryW)(LPCWSTR lpPathName) = SetCurrentDirectoryW;

BOOL WINAPI DetourSetCurrentDirectoryA(LPCSTR lpPathName)
{
	if (lpPathName == NULL)
		return NativeSetCurrentDirectoryA(lpPathName);

	CStringA strPathName = CHistoryManager::GetInstance()->Redirect(CStringA(lpPathName));
	return NativeSetCurrentDirectoryA(strPathName);
}

BOOL WINAPI DetourSetCurrentDirectoryW(LPCWSTR lpPathName)
{
	if (lpPathName == NULL)
		return NativeSetCurrentDirectoryW(lpPathName);

	CStringW strPathName = CHistoryManager::GetInstance()->Redirect(CStringW(lpPathName));
	return NativeSetCurrentDirectoryW(strPathName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CHistoryManager* CHistoryManager::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CHistoryManager();

	return m_Instance;
}

CHistoryManager::CHistoryManager(void)
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

CHistoryManager::~CHistoryManager(void)
{
}

BOOL CHistoryManager::Init()
{
	PrepareTempDirs();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)NativeCreateFileA, DetourCreateFileA);
	DetourAttach(&(PVOID&)NativeCreateFileW, DetourCreateFileW);
	DetourAttach(&(PVOID&)NativeOpenFile, DetourOpenFile);
	DetourAttach(&(PVOID&)NativeGetFileAttributesA, DetourGetFileAttributesA);
	DetourAttach(&(PVOID&)NativeGetFileAttributesW, DetourGetFileAttributesW);
	DetourAttach(&(PVOID&)NativeGetFileAttributesExA, DetourGetFileAttributesExA);
	DetourAttach(&(PVOID&)NativeGetFileAttributesExW, DetourGetFileAttributesExW);
	DetourAttach(&(PVOID&)NativeSetFileAttributesA, DetourSetFileAttributesA);
	DetourAttach(&(PVOID&)NativeSetFileAttributesW, DetourSetFileAttributesW);
	DetourAttach(&(PVOID&)NativeFindFirstFileA, DetourFindFirstFileA);
	DetourAttach(&(PVOID&)NativeFindFirstFileW, DetourFindFirstFileW);
	DetourAttach(&(PVOID&)NativeFindFirstFileExA, DetourFindFirstFileExA);
	DetourAttach(&(PVOID&)NativeFindFirstFileExW, DetourFindFirstFileExW);
	DetourAttach(&(PVOID&)NativeCreateDirectoryA, DetourCreateDirectoryA);
	DetourAttach(&(PVOID&)NativeCreateDirectoryW, DetourCreateDirectoryW);
	DetourAttach(&(PVOID&)NativeCreateDirectoryExA, DetourCreateDirectoryExA);
	DetourAttach(&(PVOID&)NativeCreateDirectoryExW, DetourCreateDirectoryExW);
	DetourAttach(&(PVOID&)NativeSearchPathA, DetourSearchPathA);
	DetourAttach(&(PVOID&)NativeSearchPathW, DetourSearchPathW);
	DetourAttach(&(PVOID&)NativeDeleteFileA, DetourDeleteFileA);
	DetourAttach(&(PVOID&)NativeDeleteFileW, DetourDeleteFileW);
	DetourAttach(&(PVOID&)NativeSetNamedSecurityInfoA, DetourSetNamedSecurityInfoA);
	DetourAttach(&(PVOID&)NativeSetNamedSecurityInfoW, DetourSetNamedSecurityInfoW);
	DetourAttach(&(PVOID&)NativeGetDiskFreeSpaceExA, DetourGetDiskFreeSpaceExA);
	DetourAttach(&(PVOID&)NativeGetDiskFreeSpaceExW, DetourGetDiskFreeSpaceExW);
	DetourAttach(&(PVOID&)NativePathFileExistsA, DetourPathFileExistsA);
	DetourAttach(&(PVOID&)NativePathFileExistsW, DetourPathFileExistsW);
	DetourAttach(&(PVOID&)NativeSetCurrentDirectoryA, DetourSetCurrentDirectoryA);
	DetourAttach(&(PVOID&)NativeSetCurrentDirectoryW, DetourSetCurrentDirectoryW);

	return NO_ERROR == DetourTransactionCommit();
}

BOOL CHistoryManager::Finalize()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach(&(PVOID&)NativeCreateFileA, DetourCreateFileA);
	DetourDetach(&(PVOID&)NativeCreateFileW, DetourCreateFileW);
	DetourDetach(&(PVOID&)NativeOpenFile, DetourOpenFile);
	DetourDetach(&(PVOID&)NativeGetFileAttributesA, DetourGetFileAttributesA);
	DetourDetach(&(PVOID&)NativeGetFileAttributesW, DetourGetFileAttributesW);
	DetourDetach(&(PVOID&)NativeGetFileAttributesExA, DetourGetFileAttributesExA);
	DetourDetach(&(PVOID&)NativeGetFileAttributesExW, DetourGetFileAttributesExW);
	DetourDetach(&(PVOID&)NativeSetFileAttributesA, DetourSetFileAttributesA);
	DetourDetach(&(PVOID&)NativeSetFileAttributesW, DetourSetFileAttributesW);
	DetourDetach(&(PVOID&)NativeFindFirstFileA, DetourFindFirstFileA);
	DetourDetach(&(PVOID&)NativeFindFirstFileW, DetourFindFirstFileW);
	DetourDetach(&(PVOID&)NativeFindFirstFileExA, DetourFindFirstFileExA);
	DetourDetach(&(PVOID&)NativeFindFirstFileExW, DetourFindFirstFileExW);
	DetourDetach(&(PVOID&)NativeCreateDirectoryA, DetourCreateDirectoryA);
	DetourDetach(&(PVOID&)NativeCreateDirectoryW, DetourCreateDirectoryW);
	DetourDetach(&(PVOID&)NativeCreateDirectoryExA, DetourCreateDirectoryExA);
	DetourDetach(&(PVOID&)NativeCreateDirectoryExW, DetourCreateDirectoryExW);
	DetourDetach(&(PVOID&)NativeSearchPathA, DetourSearchPathA);
	DetourDetach(&(PVOID&)NativeSearchPathW, DetourSearchPathW);
	DetourDetach(&(PVOID&)NativeDeleteFileA, DetourDeleteFileA);
	DetourDetach(&(PVOID&)NativeDeleteFileW, DetourDeleteFileW);
	DetourDetach(&(PVOID&)NativeSetNamedSecurityInfoA, DetourSetNamedSecurityInfoA);
	DetourDetach(&(PVOID&)NativeSetNamedSecurityInfoW, DetourSetNamedSecurityInfoW);
	DetourDetach(&(PVOID&)NativeGetDiskFreeSpaceExA, DetourGetDiskFreeSpaceExA);
	DetourDetach(&(PVOID&)NativeGetDiskFreeSpaceExW, DetourGetDiskFreeSpaceExW);
	DetourDetach(&(PVOID&)NativePathFileExistsA, DetourPathFileExistsA);
	DetourDetach(&(PVOID&)NativePathFileExistsW, DetourPathFileExistsW);
	DetourDetach(&(PVOID&)NativeSetCurrentDirectoryA, DetourSetCurrentDirectoryA);
	DetourDetach(&(PVOID&)NativeSetCurrentDirectoryW, DetourSetCurrentDirectoryW);

	return NO_ERROR == DetourTransactionCommit();
}

// 清除临时文件
BOOL CHistoryManager::CleanHistory()
{
	CStringW dir = m_strRedirectPath; 
	return DeleteDir(dir.TrimRight(L'\\'));
}

BOOL CHistoryManager::CleanUpdate()
{
	CStringW dir = m_strWinTemp + L"BankUpdate";
	return DeleteDir(dir);
}

// 重新定向
// 注意不要empty string
CStringA CHistoryManager::Redirect(CStringA strFileName)
{
	return CStringA(Redirect(CStringW(strFileName)));
}

CStringW CHistoryManager::Redirect(CStringW strFileName)
{
	bool bRedirect = false;
	CStringW strRedirect;
	
	strFileName.Replace(L'/', L'\\');

	bool bLeadSlash = false;
	if (strFileName.GetLength() >= 4 && _wcsnicmp(strFileName, L"\\\\?\\", 4) == 0)
		bLeadSlash = true;

	std::vector<std::wstring> vecParts;
	std::wstring strPath = strFileName;
	transform(strPath.begin(), strPath.end(), strPath.begin(), tolower);
	SplitPath(strPath, vecParts);

	for (int i = 0; i < TEMP_KEY_COUNT; i++)
	{
		int nPartCount = g_TempPaths[i].vecShortParts.size();
		if (nPartCount == 0 || (int)vecParts.size() < nPartCount)
			continue;

		bool bSame = true;
		for (int j = 0; j < nPartCount; j++)
		{
			if (g_TempPaths[i].vecShortParts[j] != vecParts[j] && g_TempPaths[i].vecLongParts[j] != vecParts[j])
			{
				bSame = false;
				break;
			}
		}
	
		if (bSame)
		{
			bRedirect = true;

			if (bLeadSlash)
				strRedirect = L"\\\\?\\";

			strRedirect += m_strRedirectPath;
			strRedirect += g_TempPaths[i].strRegKey.c_str();

			for (std::vector<std::wstring>::size_type j = nPartCount; j < vecParts.size(); j++) // gao
			{
				strRedirect += L"\\";
				strRedirect += vecParts[j].c_str();
			}

			if (strFileName[strFileName.GetLength() - 1] == L'\\')
				strRedirect += L'\\';
				
			break;
		}
	}

	return bRedirect ? strRedirect : strFileName;
}

// 删除目录及文件
BOOL CHistoryManager::DeleteDir(CStringW dir)
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

BOOL CHistoryManager::PrepareTempDirs()
{
	for (int i = 0; i < TEMP_KEY_COUNT; i++)
	{
		HKEY key;
		if (RegOpenKeyExA(HKEY_CURRENT_USER, REG_TEMP_FOLDER, 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
			continue;

		g_TempPaths[i].strRegKey = g_TempKey[i];

		WCHAR szPath[1024];
		DWORD Size = 1024;
		if (RegQueryValueExW(key, g_TempPaths[i].strRegKey.c_str(), NULL, NULL, (LPBYTE)szPath, &Size) != ERROR_SUCCESS)
		{
			RegCloseKey(key);
			continue;
		}

		CStringW strPath = szPath;
		strPath.Replace(L'/', L'\\');
		if (strPath[strPath.GetLength() - 1] != L'\\')
			strPath += L'\\';

		//////////////////////////////////////////////////////////////////////////
		// Create Redirect Directory

		CStringW strTemp = m_strRedirectPath + CStringW(g_TempPaths[i].strRegKey.c_str());
		if (!CreateDirectoryW(strTemp, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
			continue;
		SetFileAttributesW(strTemp, FILE_ATTRIBUTE_HIDDEN);
		
		//////////////////////////////////////////////////////////////////////////
		// 

 		WCHAR szDir[1024], szShort[1024], szLong[1024];
 		ExpandEnvironmentStringsW((LPWSTR)(LPCWSTR)strPath, (LPWSTR)szDir, _countof(szDir));
 		GetShortPathNameW(szDir, szShort, _countof(szShort));
		GetLongPathNameW(szDir, szLong, _countof(szLong));
		
		_wcslwr_s(szShort);
		_wcslwr_s(szLong);

		g_TempPaths[i].strShortTempDir = szShort;
		g_TempPaths[i].strLongTempDir = szLong;

		SplitPath(g_TempPaths[i].strShortTempDir, g_TempPaths[i].vecShortParts);
		SplitPath(g_TempPaths[i].strLongTempDir, g_TempPaths[i].vecLongParts);

		RegCloseKey(key);
	}

	return true;
}

void CHistoryManager::SplitPath(const std::wstring& strPath, std::vector<std::wstring>& vec)
{
	LPCWSTR pBegin = strPath.c_str();
	if (strPath.size() >= 4 && _wcsnicmp(pBegin, L"\\\\?\\", 4) == 0)
		pBegin += 4;

	CStringW str = pBegin;
	int curPos = 0;
	CStringW resToken = str.Tokenize(L"\\", curPos);
	while (resToken != L"")
	{
		resToken.Trim(_T(" "));
		std::wstring token = resToken;
		vec.push_back(token);
		resToken = str.Tokenize(L"\\", curPos);
	}
}