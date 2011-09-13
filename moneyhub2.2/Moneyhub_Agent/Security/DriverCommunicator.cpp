#include "StdAfx.h"
#include "DriverCommunicator.h"
#include "../stdafx.h"

#include "../../Security/BankLoader/export.h"
#include "../../Utils/getOSInfo/GetOSInfo.h"

HANDLE g_brEventHand=NULL;

#define     REGNAME           L"SOFTWARE\\BANKID"
#define     REGNAMEANTORUN    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define     REGKEYNAME        L"MoneyHub"

bool CDriverCommunicator::m_isOk=false;
CDriverCommunicator::CDriverCommunicator(void)
{
}

CDriverCommunicator::~CDriverCommunicator(void)
{
}

// 发送事件句柄给驱动
bool CDriverCommunicator::SendReferenceEvent(HANDLE& ev)
{
	return BankLoader::SendReferenceEvent(ev);
}

bool CDriverCommunicator::CheckDriver()
{
	return BankLoader::CheckDriver();
}

// 获得灰名单
bool CDriverCommunicator::GetGrayFile(set<wstring>& file)
{
	const int size = 6000;//暂时够用了
	char sfile[size];

	if( CGetOSInfo::getInstance()->isX64() )//64bits OS
	{
		HANDLE mphd = NULL;
		mphd = OpenFileMappingW(FILE_MAP_READ, FALSE, L"SHAREGRAYMEM");

		if(mphd)
		{
			LPVOID lpMapAddr = MapViewOfFile(mphd,FILE_MAP_READ,0,0,0);
			if(lpMapAddr)
			{
				file.insert( (wchar_t*)((UCHAR*)lpMapAddr+sizeof(ULONG)) );
				/*char buf[MAX_PATH] = {0};////D
				unsigned char *hash1 = (UCHAR*)lpMapAddr+sizeof(ULONG)+sizeof(wchar_t)*2;
				sprintf(buf," address is : %x  get gray hash is:  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
					hash1, hash1[0], hash1[1], hash1[2], hash1[3], hash1[4], hash1[5], hash1[6], hash1[7], hash1[8], hash1[9], hash1[10], hash1[11], hash1[12], hash1[13], hash1[14], hash1[15]) ;
				OutputDebugStringA(buf);*/
				return true;
			}
			else
			{
				int error = GetLastError();
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DRIVER_COM, CRecordProgram::GetInstance()->GetRecordInfo(L"MapViewOfFile SHAREGRAYMEM error:%d", error));
				return false;
			}
		}
		else
		{
			int error = GetLastError();
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DRIVER_COM, CRecordProgram::GetInstance()->GetRecordInfo(L"OpenFileMapping SHAREGRAYMEM error:%d", error));
			return false;
		}
		
	}
	else
	{
		if(BankLoader::GetGrayFile((void*)sfile,size))
		{
			DWORD num = 0;
			memcpy(&num, sfile ,sizeof(DWORD));
			char* p = sfile + sizeof(DWORD);

			for(DWORD dwIndex = 0; dwIndex < num; dwIndex ++) // gao
			{
				WCHAR strfile[MAX_PATH] = { 0 };
				memcpy((void*)strfile, p,sizeof(strfile));
				file.insert(strfile);
				p = p + sizeof(strfile) + 16;
			}
			return true;
		}
		else
			return false;
	}
}

REGSTATUS CDriverCommunicator::isSendData() const
{
	HKEY  hKey=NULL,hSubKey=NULL;
	DWORD index;

	if(RegCreateKeyExW(HKEY_LOCAL_MACHINE,REGNAME,0,0,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&hSubKey,&index) == ERROR_SUCCESS)
	{
		if( REG_CREATED_NEW_KEY == index)
			return CD_SUCESSFUL;
		else
			return CD_ALLREADYEXISTING;
	}
		
	return CD_ERROR;
}
/**
*        check driver's status for sending data
*/
bool CDriverCommunicator::isSendDataWithDriver() const
{
	return BankLoader::returnDriverStatus(DRIVER_RECEIVED) == 1 ? true:false ;
}

          
/**
*
*/
void CDriverCommunicator::sendData()
{
	BankLoader::setSecuModuleBR();
}

void CDriverCommunicator::SendBlackList()
{
	// 发送黑名单
	BankLoader::SendBlackListToDriver();
}
