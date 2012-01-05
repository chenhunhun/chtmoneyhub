#include "StdAfx.h"
#include <tchar.h>
#include "DriverManager.h"
#include "../common/DriverDefine.h"
#include "../../Encryption/Communication/comm.h"
#include "BankLoader.h"
#include "../../Utils/getOSInfo/GetOSInfo.h"

CDriverManager::CDriverManager(void)
{
}

CDriverManager::~CDriverManager(void)
{
}

bool CDriverManager::InstallDriver()
{
	BOOL ret = m_DriverManager.InstallDriver();
	return FALSE == ret ? false:true;
}

bool CDriverManager::StartDriver()
{
	m_DriverManager.RemoveDriver();
	if(m_DriverManager.InstallDriver() == FALSE)
		return false;

	if(m_DriverManager.LoadDriver() == FALSE)
		return false;

	return true;
}

bool CDriverManager::StopDriver()
{
	m_DriverManager.UnloadDriver();
	return true;
}

bool CDriverManager::RemoveDriver()
{
	m_DriverManager.UnloadDriver();
	BOOL ret = m_DriverManager.RemoveDriver();
	return FALSE == ret ? false:true;
}

BOOL CDriverManager::CheckIsDriverLoaded() const
{
	return m_DriverManager.CheckIsDriverLoaded();
}


bool CDriverManager::SetFilterProcessID(UINT32 PID)
{
	if( this->isX64(IOCTL_SET_PROTECT_PID, NULL) )//compatible
		return 1;

	PID_INFO PIDInfo;
	DWORD dwBytesReturned;
	unsigned char buffer[256];
	int length;

	PIDInfo.count = 1;
	PIDInfo.currentTime = __rdtsc();
	PIDInfo.PID[0] = PID;
	
	length=DownloadPack((unsigned char *)&PIDInfo, sizeof(PID_INFO), buffer);

	BOOL ret = ::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_SET_PROTECT_PID, buffer, length, NULL, 0, &dwBytesReturned, NULL);
	
	return FALSE == ret ? false:true;
}

bool CDriverManager::GetFilterProcessIDs(PPID_INFO PIDs, UINT32 size)
{
// 	if( this->isX64(IOCTL_GET_PROTECT_PIDS, NULL) )//compatible
// 		return 1;


	DWORD dwBytesReturned;
	unsigned char buffer[6000];

	::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_GET_PROTECT_PIDS, NULL, 0, buffer, 6000, &dwBytesReturned, NULL);

	if (dwBytesReturned>0)
	{
		int size;

		size=UploadUnPack(buffer,dwBytesReturned,(PUCHAR)PIDs);

		return size > 0 ? true:false;
	}

	return false;
}


//add by bh
bool CDriverManager::checkHookSafe()
{
	if( this->isX64(IOCTL_GET_PROTECT_PIDS, NULL) )//compatible
		return 1;

	DWORD dwBytesReturned;
	unsigned char buffer[400];

	::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_GET_PROTECT_PIDS, NULL, 0, buffer, 400, &dwBytesReturned, NULL);
    
	return dwBytesReturned == 0 ? false:true;
}
/**
*         
*/
int CDriverManager::returnDriverStatus(MYDRIVERSTATUS type)
{
	if( this->isX64(IOCTL_GET_DRIVER_STATUS, NULL) )//compatible
		return 1;

	DWORD dwBytesReturned;
	unsigned char buffer[6000];
	unsigned char PIDs[255]={0};
	MYDRIVERSTATUS ll=type;

	unsigned char encryptBuf[255];
	ULONG enLen=DownloadPack((PUCHAR)&ll,4,encryptBuf);

	::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_GET_DRIVER_STATUS, (LPVOID)encryptBuf, enLen, buffer, 6000, &dwBytesReturned, NULL);

	if (dwBytesReturned>0)
	{
		int size;

		size=UploadUnPack(buffer,dwBytesReturned,(PUCHAR)PIDs);

		if (size>0)
		{
			return *(int *)PIDs ;
		}
	}
	
	return 0;
}
//

bool CDriverManager::SetSecuMD5(void* secuMD5, ULONG length,bool type)
{
	LPVOID par[3];
	par[0] = &secuMD5;
	par[1] = &length;
	par[2] = &type;

	//char buf[MAX_PATH] = {0};
	unsigned char *hash = (unsigned char *) secuMD5;

	/*sprintf(buf,"SetSecuMD5: lengs: %d %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
		length, hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], 
		hash[11], hash[12], hash[13], hash[14], hash[15]) ;*/

	//OutputDebugStringA(buf);////D

	if( this->isX64(type?IOCTL_SET_BLACK_MD5:IOCTL_SET_SECU_MD5,  par) )//compatible
		return true;

	//add by bh
	PUCHAR  tmpBuf=new UCHAR[length+sizeof(ULONG)+8];
	if(!tmpBuf)
		return false;

	//*((ULONG *)(tmpBuf) )=length;
	memcpy((PVOID)tmpBuf,(PVOID)&length,sizeof(ULONG) );
	memcpy((PVOID)(tmpBuf+sizeof(ULONG)),(PVOID)secuMD5,length);
    //*((__int64 *)(tmpBuf+length+sizeof(ULONG)) )=__rdtsc();
	__int64 tmpT=__rdtsc();
	memcpy((PVOID)(tmpBuf+length+sizeof(ULONG) ),&tmpT,sizeof(__int64) );
	memcpy((LPVOID)g_kernelHash,(LPVOID)tmpBuf,length+sizeof(ULONG)+8);
	//

	DWORD dwBytesReturned;
	unsigned char * buffer=new unsigned char[length+256+sizeof(ULONG)+8];

	BOOL ret;
	int packLength;

	packLength=DownloadPack((unsigned char *)tmpBuf, length+sizeof(ULONG)+8, buffer);

	if(type)
		ret= ::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_SET_BLACK_MD5, buffer, packLength, NULL, NULL, &dwBytesReturned, NULL);
	else
		ret= ::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_SET_SECU_MD5, buffer, packLength, NULL, NULL, &dwBytesReturned, NULL);

	delete []buffer;
	delete []tmpBuf;
	
	return FALSE == ret ? false:true;
}

bool CDriverManager::Safeguard()
{
	if( this->isX64(IOCTL_GET_PROTECT_PIDS,  NULL) )//compatible
		return true;

	if (!CDriverLoader::CheckDriverImagePath())
		return false;

	return CheckProcessId();
}

bool CDriverManager::CheckProcessId()
{
	/*
		64位取消驱动保护
	*/
	if( this->isX64(IOCTL_GET_PROTECT_PIDS, NULL) )//compatible
		return true;

	const UINT32 PIDInfoLen = sizeof(PID_INFO) + sizeof(UINT32) * 1024;
	PPID_INFO pPIDs = (PPID_INFO)new char[PIDInfoLen];
	ZeroMemory(pPIDs, PIDInfoLen);

	// ret 0 失败
	bool bOK = GetFilterProcessIDs(pPIDs, PIDInfoLen) == 1;

	if (bOK)
	{
		__int64 elaspedTime = __rdtsc() - pPIDs->currentTime;
		if ((elaspedTime >= COMMUNICATE_TIME_LIMIT) || (elaspedTime <=COMMUNICATE_TIME_DOWN))
			bOK = false;
		if (bOK)
		{
			bool PIDAdded = false;
			for(int i = 0; i < (int)pPIDs->count; i++)
			{
				if(pPIDs->PID[i] == g_dwProcessId)
				{
					PIDAdded = true;
					break;
				}
			}

			bOK = PIDAdded;
		}
	}

	delete pPIDs;

	if (!bOK)
	{
		MessageBox(NULL, _T("内核进程出错，程序中止运行"), _T("财金汇"), MB_OK | MB_ICONSTOP);
		exit(-1);
	}

	return bOK;
}


bool CDriverManager::SetHandle(HANDLE& ev)
{
	LPVOID par[1];
	par[0] = &ev;

	if( this->isX64(IOCTL_SET_HANDLE, par) )//compatible
		return true;

	DWORD dwBytesReturned;
	unsigned char buffer[6000];

	HANDLE hEvent = ev;

	unsigned char * Ebuffer=new unsigned char[sizeof(hEvent)+256];
	int packLength;
	packLength=DownloadPack((unsigned char *)&hEvent, sizeof(hEvent), Ebuffer);

	bool ret = ::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_SET_HANDLE, Ebuffer,packLength, buffer, sizeof(buffer), &dwBytesReturned, NULL) == FALSE ? false : true;

	delete []Ebuffer;
	return ret;
}

bool CDriverManager::GetGrayFile(void *pFileData,DWORD size)
{
	LPVOID par[2];
	par[0] = &pFileData;
	par[1] = &size;

	if( this->isX64(IOCTL_RETURN_DATA, par ) )//compatible
		return true;

	DWORD dwBytesReturned;
	unsigned char buffer[6000];

	bool ret = ::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_RETURN_DATA, NULL,NULL, buffer, sizeof(buffer), &dwBytesReturned, NULL) == FALSE ? false : true;

	int len;
	if( dwBytesReturned > 0 )
		len = UploadUnPack(buffer,dwBytesReturned,(PUCHAR)pFileData);

	return ret;
}

bool CDriverManager::CheckDriver()
{
	return m_DriverManager.CheckDriver();
}

bool CDriverManager::SetKernelContinue(bool b)
{
	DWORD dwBytesReturned;
	if( CGetOSInfo::getInstance()->isX64()  )//compatible// 64位先不要该策略
		return true;

	unsigned char * Ebuffer=new unsigned char[256];
	memset(Ebuffer, 0, 256);
	char result = (b) ? 1 : 0;
	int packLength = sizeof(char);
	packLength = DownloadPack((unsigned char *)&result, sizeof(result), Ebuffer);

	::DeviceIoControl(m_DriverManager.GetDriver(), IOCTL_SET_FILTERRESULT, (LPVOID)Ebuffer, packLength, NULL, 0, &dwBytesReturned, NULL);

	delete[] Ebuffer;
	return 0;
}


/**
*   判断系统类型，同时兼容32bits & 64bits
*   2011-3-24 暂时取消64bits驱动保护
*/
BOOL CDriverManager::isX64(UINT32 index, LPVOID par)
{
	UINT32* p = (UINT32*) par;

	if( CGetOSInfo::getInstance()->isX64() )
	{
		//64bits下取消驱动保护
		return TRUE;
		/////////////////////////////////////
		/////////////////////////////////////
		char buf[MAX_PATH]= {0};
		BOOL bReturn = TRUE;
		switch(index)
		{
		case IOCTL_SET_PROTECT_PID:
		case IOCTL_GET_PROTECT_PIDS:
		case IOCTL_SET_UP_UNLOAD:
			//bReturn = FALSE;//如果64bits加回驱动保护，去掉注释即可
			break;
		case IOCTL_SET_BLACK_MD5:
		case IOCTL_SET_SECU_MD5:
			 //OutputDebugStringW(L"isX64: send  wb hash!");
			 //SetSecuMD5X64(*(void**)(*p), *(ULONG*)(*(p+1)), *(bool*)(*(p+2))/*void* secuMD5, ULONG length,bool type*/);
			break;

		case IOCTL_GET_DRIVER_STATUS://is't receive list
			 //OutputDebugStringW(L"isX64:get driver status!");
			 //returnDriverStatusX64(*(MYDRIVERSTATUS*)*p/*MYDRIVERSTATUS type*/);
			break;

		case IOCTL_SET_HANDLE:
			 //OutputDebugStringW(L"isX64:set handles!");
			 //SetHandleX64((HANDLE&)*(HANDLE*)(*p)/*HANDLE& ev*/);
			break;

		case IOCTL_RETURN_DATA:
			 //OutputDebugStringW(L"isX64:get gray file");
			 //GetGrayFileX64(*(void **)(*p), *(DWORD*)*(p+1)/*void *pFileData,DWORD size*/);
			break;

		default:
			break;
		}
		return bReturn;
	}

	return FALSE;
}


UCHAR* g_mappingBuf;//
HANDLE g_shareMem64Handle;
bool CDriverManager::SetSecuMD5X64(void* secuMD5, ULONG length,bool type)
{
	char buf[MAX_PATH] = {0};////D
	unsigned char *hash = (unsigned char *) secuMD5;
	//sprintf(buf,"SetSecuMD5X64 : Lengths: %d  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
	//	length, hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], 
	//	hash[11], hash[12], hash[13], hash[14], hash[15]) ;

	//OutputDebugStringA(buf);////D

	PUCHAR  tmpBuf=new UCHAR[length+sizeof(ULONG)];
	if(!tmpBuf)
		return false;

	memcpy((PVOID)tmpBuf,(PVOID)&length,sizeof(ULONG) );
	memcpy((PVOID)(tmpBuf+sizeof(ULONG)),(PVOID)secuMD5,length);

 	BOOL ret = TRUE;

	//mapping for 64bits os
	HANDLE secuEventHandle = NULL, blackEventHandle = NULL;
	secuEventHandle  = CreateEventW(NULL,FALSE,FALSE,L"WAITSECUMAP");
	blackEventHandle  = CreateEventW(NULL,FALSE,FALSE,L"WAITBLACKMAP");
	if( NULL == secuEventHandle || NULL == blackEventHandle)
	{
		OutputDebugStringW(L"create 64bits event handle error!");
		return false;
	}
	
	if( g_mappingBuf )//释放内存空间
		UnmapViewOfFile(g_mappingBuf);
	if( g_shareMem64Handle )
		CloseHandle(g_shareMem64Handle);

	if(type)
		g_shareMem64Handle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, length+sizeof(ULONG), L"SHAREBLACKHASH");
	else
		g_shareMem64Handle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, length+sizeof(ULONG), L"SHARESECUHASH");
if(type)
OutputDebugStringW(L"map SHAREBLACKHASH");
else
OutputDebugStringW(L"map SHARESECUHASH");

	if( NULL == g_shareMem64Handle )
	{
		OutputDebugStringW(L"create 64bits mapping handle error!");
		return false;
	}

	g_mappingBuf = (UCHAR*)MapViewOfFile(g_shareMem64Handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if( !g_mappingBuf)
	{
		OutputDebugStringW(L"MapViewOfFile 64bits mapping error!");
		return false;
	}
	UCHAR *p = new UCHAR[length+sizeof(ULONG)];
	*(unsigned long*)p = length;
	CopyMemory(p+sizeof(ULONG), secuMD5, length);

	CopyMemory(g_mappingBuf, p, length+sizeof(ULONG) );

	OutputDebugStringW(L"create mappingview is successful!");////D
	//

	if(type)
		SetEvent(blackEventHandle);
	else
		SetEvent(secuEventHandle);

	if(type)
		OutputDebugStringW(L"                          send black hash");
	else
		OutputDebugStringW(L"                          send secuHash!");

	delete []tmpBuf;
	
	return FALSE == ret ? false:true;
}


int CDriverManager::returnDriverStatusX64(MYDRIVERSTATUS type)
{
	return 1;
}

bool CDriverManager::SetHandleX64(HANDLE& ev)
{
	return true;
}

bool CDriverManager::GetGrayFileX64(void *pFileData,DWORD size)
{
	//共享灰名单

	return true;
}