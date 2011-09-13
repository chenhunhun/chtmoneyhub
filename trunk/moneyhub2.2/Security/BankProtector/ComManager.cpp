#include "StdAfx.h"
#include "windows.h"
#include "Psapi.h"
#include "ComManager.h"
#include "../../Utils/detours/detours.h"
#include "stdio.h"
#include "objbase.h"

#include <stdio.h>
#include <assert.h>
#include "atlstr.h"

CComManager* CComManager::m_Instance = NULL;

CComManager* CComManager::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CComManager();
	return m_Instance;
}

typedef HRESULT (STDAPICALLTYPE *DLLGETCLASSOBJECT)(REFCLSID rclsid ,REFIID riid,void **ppv);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static LONG (WINAPI *NativeRegCreateKeyA)(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult) = RegCreateKeyA;
static LONG (WINAPI *NativeRegCreateKeyW)(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult) = RegCreateKeyW;

LONG WINAPI DetourRegCreateKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult)
{
	printf("RegCreateKeyA: %s\n", lpSubKey);
	return NativeRegCreateKeyA(hKey, lpSubKey, phkResult);
}

LONG WINAPI DetourRegCreateKeyW(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult)
{
	printf("RegCreateKeyW: %ws\n", lpSubKey);
	return NativeRegCreateKeyW(hKey, lpSubKey, phkResult);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static HRESULT (STDAPICALLTYPE *NativeCoCreateInstance)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID * ppv) = CoCreateInstance;

HRESULT STDAPICALLTYPE DetourCoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID * ppv)
{
	printf("CoCreateInstance\n");
	return NativeCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

static HRESULT (STDAPICALLTYPE *NativeCoGetClassObject)(REFCLSID rclsid, DWORD dwClsContext, LPVOID pServerInfo, REFIID riid, LPVOID * ppv) = CoGetClassObject;

HRESULT STDAPICALLTYPE DetourCoGetClassObject(REFCLSID rclsid, DWORD dwClsContext, LPVOID pServerInfo, REFIID riid, LPVOID * ppv)
{
	const char* objpath = CComManager::GetInstance()->FindComPath(rclsid);
	if(objpath)
	{
		HMODULE hModule = LoadLibraryA(objpath);
		DLLGETCLASSOBJECT pGetClassObject = (DLLGETCLASSOBJECT)GetProcAddress(hModule, "DllGetClassObject");
		//PVOID pClass;
		return pGetClassObject(rclsid, riid, (void**)ppv);
	}

	return NativeCoGetClassObject(rclsid, dwClsContext, pServerInfo, riid, ppv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CComManager::CComManager(void)
{
	CHAR szExePath[MAX_PATH];
	DWORD ret = GetModuleFileNameExA(GetCurrentProcess(), NULL, szExePath, MAX_PATH);
	assert(ret <= MAX_PATH);

	CStringA strExePath = CStringA(szExePath);

	char* pLastSlash = strrchr(szExePath, '\\');
	if(pLastSlash == NULL)
	{
		assert(false);
		return;
	}
	strcpy_s(pLastSlash, MAX_PATH,"\\COM\\*");
	char* comdir_ptr = pLastSlash + strlen("\\COM");

	WIN32_FIND_DATAA fd;
	memset(&fd, 0, sizeof(WIN32_FIND_DATAA));
	HANDLE hFile = FindFirstFileA(szExePath, &fd);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;

			// Check if it is a COM name
			if(strlen(fd.cFileName) != 38)
				continue;

			COM_CLSID clsid;
			CLSIDFromString((LPOLESTR)(const WCHAR*)CStringW(fd.cFileName), &clsid.clsid);

			sprintf_s(comdir_ptr,(MAX_PATH - 5), "\\%s\\*", fd.cFileName);
			char* com_ptr = comdir_ptr + strlen(comdir_ptr) - 1;

			WIN32_FIND_DATAA Comfd;
			memset(&Comfd, 0, sizeof(WIN32_FIND_DATAA));
			HANDLE hComFile = FindFirstFileA(szExePath, &Comfd);
			if(hComFile == INVALID_HANDLE_VALUE)
				continue;
			do
			{
				if(strcmp(Comfd.cFileName, ".") == 0 || strcmp(Comfd.cFileName, "..") == 0)
					continue;
				strcpy_s(com_ptr,MAX_PATH, Comfd.cFileName);
				m_comlist.insert(std::make_pair(clsid, CStringA(szExePath)));

			}while(FindNextFileA(hComFile, &Comfd));

			FindClose(hComFile);
			
		}while(FindNextFileA(hFile, &fd));

		FindClose(hFile);
	}
}

CComManager::~CComManager(void)
{
}

const char* CComManager::FindComPath(const CLSID& clsid) const
{
	COM_CLSID com_clsid;
	memcpy((void*)&com_clsid.clsid, (void*)&clsid, sizeof(CLSID));
	ComList::const_iterator it = m_comlist.find(com_clsid);
	if(it == m_comlist.end())
		return NULL;
	
	return (const char*)it->second;
}


bool CComManager::Init()
{
	//DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)NativeRegCreateKeyA, DetourRegCreateKeyA);
	DetourAttach(&(PVOID&)NativeRegCreateKeyW, DetourRegCreateKeyW);
	//DetourAttach(&(PVOID&)NativeCoCreateInstance, DetourCoCreateInstance);
	DetourAttach(&(PVOID&)NativeCoGetClassObject, DetourCoGetClassObject);

	ULONG error = DetourTransactionCommit();

	return error == NO_ERROR;
}


bool CComManager::Finalize()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach(&(PVOID&)NativeRegCreateKeyA, DetourRegCreateKeyA);
	DetourDetach(&(PVOID&)NativeRegCreateKeyW, DetourRegCreateKeyW);
	//DetourDetach(&(PVOID&)NativeCoCreateInstance, DetourCoCreateInstance);
	DetourDetach(&(PVOID&)NativeCoGetClassObject, DetourCoGetClassObject);

	ULONG error = DetourTransactionCommit();
	return error == NO_ERROR;
}
