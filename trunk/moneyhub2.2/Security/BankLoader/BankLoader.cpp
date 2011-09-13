#include "stdafx.h"
#include "BankLoader.h"
#include "windows.h"
#include <assert.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include "VerifyCache.h"
#include "../../Security/Authentication/encryption/MD5.h"
#include "../Authentication/ModuleVerifier/export.h"
#include "../../Include/LogPrintf.h"
#include "../../Encryption/CHKFile/CHK.h"
#include "../../Utils/getOSInfo/GetOSInfo.h"

SysModuleList g_sysModuleNameList;
ModuleList g_moduleHashList;
ModuleList g_blackHashList;
DWORD g_dwProcessId;


#pragma comment(lib,"psapi.lib")

////////////////////look up dll
#pragma data_seg(".coremoneyHubShared")
//UINT32 g_kernelProcessId = 0; 
unsigned char g_kernelHash[400000] ={0};
#pragma data_seg()

#pragma comment(linker, "/SECTION:.coremoneyHubShared,RWS")
////////////////////
#define __NOUAC__
//#define DISABLE_DRIVER

CBankLoader* CBankLoader::m_Instance = NULL;

CBankLoader* CBankLoader::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CBankLoader();
	return m_Instance;
}


CBankLoader::CBankLoader() : m_bLoadFinished(false)
{
	ModuleVerifier::Init();
}

CBankLoader::~CBankLoader() 
{
	ModuleVerifier::CleanUp();
}

bool CBankLoader::InstallAndStartDriver()
{
	return m_DriverLoader.StartDriver();
}

bool CBankLoader::UnInstallDriver()
{
	return m_DriverLoader.RemoveDriver();
}

//////////////////////////////////////////////////////////////////////////
bool CBankLoader::LoadProcess(const CString& cmdline, HANDLE& hProcess, DWORD& PID)
{
#ifndef DISABLE_DRIVER
	if(m_DriverLoader.CheckIsDriverLoaded() == false)
	{
		LogPrintfA("Driver Not Loaded: %d", GetLastError());
		return false;
	}
#endif

	DWORD TID;
	m_bLoadFinished = false;

	LoaderInfo* loaderinfo = new LoaderInfo;
	loaderinfo->bankLoader = this;
	loaderinfo->cmdline = cmdline;

	LogPrintfA("Create Thread");
	if(CreateThread(NULL, 0, LoadProc_S, (LPVOID)loaderinfo, 0, &TID) == NULL)
	{
		delete loaderinfo;
		LogPrintfA("CreateThread Failed: %d", GetLastError());

		m_bLoadFinished = true;
		return false;
	}

	return true;
}

DWORD WINAPI CBankLoader::LoadProc_S(LPVOID param)
{
	LoaderInfo* loaderinfo = (LoaderInfo*)param;
	CBankLoader* This = loaderinfo->bankLoader;

	DWORD ret = This->LoadProc(loaderinfo);
	delete loaderinfo;
	return ret;
}

CStringW CBankLoader::TranslateName(const WCHAR* wcsFileName)
{
	WCHAR expName[MAX_PATH];
	ExpandEnvironmentStringsW(wcsFileName, expName, MAX_PATH);
	return CStringW(expName).MakeLower();
}

CStringW CBankLoader::GetDevName(const WCHAR* wcsFileName)
{
	WCHAR devName[MAX_PATH];
	WCHAR drvName[3];
	drvName[0] = wcsFileName[0];
	drvName[1] = wcsFileName[1];
	drvName[2] = 0;
	QueryDosDeviceW(drvName, devName, MAX_PATH);

	CStringW devpath = CStringW(wcsFileName).MakeLower();
	devpath = devName + devpath.Right(devpath.GetLength() - 2);
	devpath.MakeLower();

	return devpath;
}

CString CBankLoader::GetFileName(const WCHAR* wcsFilePath)
{
	if(wcsFilePath == NULL || *wcsFilePath == 0)
		return CString();

	if(*(wcsFilePath + 1) == L':')
		return CString(wcsFilePath);

	const WCHAR* pLastSlash = wcsrchr(wcsFilePath, L'\\');
	if(pLastSlash == NULL)
		return CString();

	return CString(pLastSlash + 1);
}
/**
*
*   get lssass.exe PID
*/
bool CBankLoader::getPriviledge()
{
	HANDLE hProcessToken = NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hProcessToken)) 
	{ 
		return FALSE; 
	}

	TOKEN_PRIVILEGES tp={0};
	LUID luid={0};  
	if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid))  
	{ 
		return FALSE; 
	}  
	tp.PrivilegeCount = 1;  
	tp.Privileges[0].Luid = luid;

	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  

	// Enable the privilege
	AdjustTokenPrivileges(hProcessToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);  

	if(GetLastError() != ERROR_SUCCESS)  
	{
		return FALSE;  
	}
	return TRUE;

}
/**
*		判断lsass.exe 和 spoolsv.exe，如果有两个重名，则认为存在威胁
*/
bool CBankLoader::returnLsassId(wchar_t *p, unsigned long * pId)
{
	this->getPriviledge();
	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hand == INVALID_HANDLE_VALUE)
		return false;
	
	PROCESSENTRY32W pew = {sizeof(PROCESSENTRY32W)};
	bool bHaveOther=false;
	Process32FirstW(hand,&pew );

	wchar_t buf[500] = {0},tmp[500]={0} ;
	ExpandEnvironmentStringsW(L"%systemroot%",buf,sizeof(buf));
	std::wstring wcsPath = buf;
	wcsPath += L"\\system32\\";
	wcsPath += p;
	wcscpy_s(buf,_countof(buf),wcsPath.c_str() );	
 	
	do{
		_wcslwr_s(pew.szExeFile, 260);

		if(!wcscmp(pew.szExeFile,p))
		{
#ifndef  __NOUAC__
			HANDLE h = OpenProcess(PROCESS_ALL_ACCESS,false,pew.th32ProcessID);
			if(h)
			{
				GetModuleFileNameExW(h,NULL,tmp,sizeof(tmp) );
				_wcslwr(tmp);
				_wcslwr(buf);
				if(!wcscmp(tmp,buf))
				{
					*pId = pew.th32ProcessID;
					CloseHandle(hand);
					return true;
				}
			}
			CloseHandle(h);
#else

			if(bHaveOther == true)
			{
				CloseHandle(hand);
				*pId = 0; 
				return false;
			}
			else
			{
				bHaveOther=true;
				*pId = pew.th32ProcessID;
			}
#endif
		}

	}while(Process32NextW(hand,&pew));

	CloseHandle(hand);
	return bHaveOther;	
}

DWORD CBankLoader::LoadProc(LoaderInfo* loaderinfo)
{

	ULONG length = 0;

	// create the target process
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFOA));
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

	StartupInfo.cb = sizeof(STARTUPINFOA);

	LogPrintfA("CreateProcess");
	//将待查杀列表放入内存映射文件中，创建的时候进行继承
	if(CreateProcess(NULL, (LPWSTR)(LPCWSTR)loaderinfo->cmdline, NULL, NULL, TRUE, 
		CREATE_SUSPENDED, NULL, NULL, &StartupInfo, &ProcessInfo))
	{
		LogPrintfA("CreateProcess success");

#ifndef DISABLE_DRIVER

		m_DriverLoader.SetFilterProcessID(ProcessInfo.dwProcessId);
		//add by bh
		DWORD pId=0;
		if(false == returnLsassId(L"lsass.exe",&pId))
		{
			MessageBoxW(NULL,L"微软安全策略未通过验证，财金汇将停止运行！",L"财金汇",MB_OK);
			TerminateThread(ProcessInfo.hThread, 0);
			exit(0);
		}
		m_DriverLoader.SetFilterProcessID( pId );

		if(false == returnLsassId(L"spoolsv.exe",&pId))
			;//MessageBoxW(NULL,L"用户未开 printf spoolsv 服务！",L"财金汇",MB_OK);
		else
			m_DriverLoader.SetFilterProcessID( pId );
		// 

		// Check if the PID is added in the kernel filter list.
		const UINT32 PIDInfoLen = sizeof(PID_INFO) + sizeof(UINT32) * 1023;
		PPID_INFO pPIDs = (PPID_INFO)new char[PIDInfoLen];
		ZeroMemory(pPIDs, PIDInfoLen);
		if( !CGetOSInfo::getInstance()->isX64() )
		{
			if(m_DriverLoader.GetFilterProcessIDs(pPIDs, PIDInfoLen) == 1 )
			{
					__int64 elaspedTime = __rdtsc() - pPIDs->currentTime;
					if(elaspedTime >= COMMUNICATE_TIME_LIMIT)
					{
						LogPrintfA("GetFilterProcessIDs exceeds the time limit: %I64d\n", elaspedTime);
						delete[] pPIDs;
						TerminateThread(ProcessInfo.hThread, 0);
						return 0;
					}

					bool PIDAdded = false;
					for(UINT32 i = 0; i < pPIDs->count; i++)
					{
						if(pPIDs->PID[i] == ProcessInfo.dwProcessId)
						{
							PIDAdded = true;
							break;
						}
					}
						
					delete[] pPIDs;

					if(PIDAdded == false)
					{
						LogPrintfA("PID not added in the filter list: %d\n", ProcessInfo.dwProcessId);
						TerminateThread(ProcessInfo.hThread, 0);
						return 0;
					}
			}
			else
			{
				MessageBox(NULL, _T("内核进程出错，程序中止运行!"), _T("财金汇"), MB_OK | MB_ICONSTOP);
				delete[] pPIDs;
				TerminateThread(ProcessInfo.hThread, 0);
				return 0;
			}
		}
#endif

		g_dwProcessId = ProcessInfo.dwProcessId;
		ResumeThread(ProcessInfo.hThread);
	}
	else
		LogPrintfA("CreateProcess failed");

	m_bLoadFinished = true;
	return 0;
}

bool CBankLoader::IsFinished()
{
	return m_bLoadFinished;
}

bool CBankLoader::Safeguard()
{
	return m_DriverLoader.Safeguard();
}

//////////////////////////////////////////////////////////////////////////  by bh
bool CBankLoader::setSecuModHashBR()
{
	ULONG length = 0;
	length = MD5_HASH_SIZE * g_moduleHashList.size();
	// Verify bank modules
	ULONG BankModuleNumber = ModuleVerifier::GetBankModuleNumber();
	if(BankModuleNumber)
	{
		const ULONG hashsize = BankModuleNumber * MD5_HASH_SIZE;
		unsigned char* hashBuf = new unsigned char[hashsize+length];

		ModuleVerifier::FillFileHashes(hashBuf, hashsize);

		ULONG index;

		index=hashsize;

		for(ModuleList::const_iterator it = g_moduleHashList.begin(); it != g_moduleHashList.end(); it++)
		{
			memcpy((void *)(hashBuf+index),it->md,MD5_HASH_SIZE);
			index += MD5_HASH_SIZE;
		}

		if(m_DriverLoader.SetSecuMD5((void*)hashBuf, hashsize+length) == false)
		{
			LogPrintfA("Set secure hash failed");

			delete[] hashBuf;
			return false;
		}


		delete[] hashBuf;
	}

	return true;
}

bool CBankLoader::SendBlackHashListToDriver()
{
	ULONG length = 0;
	length = MD5_HASH_SIZE * g_blackHashList.size();
	unsigned char* hashBuf = new unsigned char[length];
	ULONG index = 0;
	for(ModuleList::const_iterator it = g_blackHashList.begin(); it != g_blackHashList.end(); it++)
	{
		memcpy((void *)(hashBuf+index),it->md,MD5_HASH_SIZE);
		index += MD5_HASH_SIZE;
	}

	if(m_DriverLoader.SetSecuMD5((void*)hashBuf, length, true) == false)
	{
		delete[] hashBuf;
		return false;
	}
	delete[] hashBuf;

	return true;
}

bool CBankLoader::SendReferenceEvent(HANDLE& ev)
{
	return m_DriverLoader.SetHandle(ev);
}

bool CBankLoader::GetGrayFile(void *pFileData,DWORD size)
{
	return m_DriverLoader.GetGrayFile(pFileData,size);
}

bool CBankLoader::CheckDriver()//检测驱动状态的函数，最多检测2分钟
{
	return m_DriverLoader.CheckDriver();
}