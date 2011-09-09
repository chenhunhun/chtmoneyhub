#include "RegOperation.h"
//#include "ntddk.h"
#include "Driver.h"
#include "LogSystem.h"
/**
* 文件名称:RegOperation.cpp                                                 
* 作    者:毕海
* 完成日期:2010-9-19
*/
//#define REG_NAME1 L"\\Registry\\Machine\\Software\\bh\\Item"
/**
*	1     create   succesful
*	2     already  existing
*   0     error
*/
#define  CONTEXTFORAUTORUN    L""C:\Program Files\MoneyHub\MoneyHub.exe" -br"

bool  getAppPath(wchar_t * buf , ULONG len)
{
	wchar_t tmpBuf[255] = {0};
	if(true == QueryReg(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\MoneyHubPrt",L"ImagePath",buf,len) )
	{
		wchar_t *q = wcschr(buf,L'\\');
		if(q)
		{
			wchar_t *r = wcschr(q+1,L'\\');
			if( r && wcschr(q,'?') )
			{
				wcscpy(tmpBuf,r+1);
				*(tmpBuf+wcslen(r+1)) = L'\0';
				wcscpy(buf,tmpBuf);
			}
		}

		wchar_t *p = wcsrchr(buf,L'\\');
		if(p != NULL)
		{
			*( p+1 ) = L'\0';	
			
			wcscpy(tmpBuf,PROGRAMNAME);
			//RtlMoveMemory(buf+wcslen(buf),PROGRAMNAME,wcslen(PROGRAMNAME)*2);
			wcscat(buf,tmpBuf);

			return true;
		}
	}

	return false;
}

#pragma INITCODE
ULONG CreateReg( PCWSTR pKey) 
{
	UNICODE_STRING RegUnicodeString;
	HANDLE hRegister;

	RtlInitUnicodeString( &RegUnicodeString, pKey);
	
	OBJECT_ATTRIBUTES objectAttributes;
	
	InitializeObjectAttributes(&objectAttributes,
							&RegUnicodeString,
							OBJ_CASE_INSENSITIVE,
							NULL, 
							NULL );
	ULONG ulResult;
	
	NTSTATUS ntStatus = ZwCreateKey( &hRegister,
							KEY_ALL_ACCESS,
							&objectAttributes,
							0,
							NULL,
							REG_OPTION_NON_VOLATILE,
							&ulResult);

	ZwClose(hRegister);

	if (NT_SUCCESS(ntStatus))
	{
		if(ulResult==REG_CREATED_NEW_KEY)
		{
			KdPrint(("The register item is created\n"));
			return 1;
		}else if(ulResult==REG_OPENED_EXISTING_KEY)
		{
			KdPrint(("The register item has been created,and now is opened\n"));
			return 2;
		}
	}

	return 0;
}
/**
*	删除注册表某项
*/
#pragma INITCODE
bool DeleteItemReg(const PCWSTR pKey )
{
	UNICODE_STRING RegUnicodeString;
	HANDLE hRegister;

	RtlInitUnicodeString( &RegUnicodeString, pKey);
	
	OBJECT_ATTRIBUTES objectAttributes;
	
	InitializeObjectAttributes(&objectAttributes,
							&RegUnicodeString,
							OBJ_CASE_INSENSITIVE,
							NULL, 
							NULL );
	
	NTSTATUS ntStatus = ZwOpenKey( &hRegister,KEY_ALL_ACCESS,&objectAttributes);

	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ZwDeleteKey(hRegister);
		ZwClose(hRegister);

		if (NT_SUCCESS(ntStatus))
		{
			KdPrint(("Delete the item successfully\n"));
			return true;
		}else 
		{
			//if( ntStatus == STATUS_ACCESS_DENIED || ntStatus == STATUS_INVALID_HANDLE )
			//KdPrint(("Maybe the item has sub item to delete\n"));
			return false;	
		}	
	}

	return false;
}
/*
*     删除注册表项
*/
bool DeleteItemKey(const PCWSTR pKey, const PCWSTR pSubKey)
{
	NTSTATUS ntStatus;
	ntStatus = RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE,pKey,pSubKey);

	if (NT_SUCCESS(ntStatus))
		return true;
	else
		return false;
}
/**
*      设置注册表项
*/
#pragma INITCODE
bool SetReg(const PCWSTR pKey , const PCWSTR pSetKey)
{
	UNICODE_STRING RegUnicodeString;
	HANDLE hRegister;
	bool   bReturn = false;

	RtlInitUnicodeString( &RegUnicodeString, pKey);
	
	OBJECT_ATTRIBUTES objectAttributes;
	
	InitializeObjectAttributes(&objectAttributes,
							&RegUnicodeString,
							OBJ_CASE_INSENSITIVE,
							NULL, 
							NULL );

	NTSTATUS ntStatus = ZwOpenKey( &hRegister,KEY_ALL_ACCESS,&objectAttributes);

	if (NT_SUCCESS(ntStatus))
	{
		UNICODE_STRING ValueName;

		RtlInitUnicodeString( &ValueName, pSetKey);

		wchar_t  strValue[255]={0};
		if( true == getAppPath(strValue,sizeof(strValue)) )
		{
			ntStatus = ZwSetValueKey(hRegister,
				&ValueName,
				0,
				REG_SZ,
				strValue,
				wcslen(strValue)*2 );

			ZwClose(hRegister);
			if (NT_SUCCESS(ntStatus))
				bReturn = true;
			else
				bReturn = false;
		}
			
		ZwClose(hRegister);
	}
	else
		WriteSysLog(LOG_TYPE_INFO,L"error :%s",L"open key!");

	if( true == bReturn)
		return true;
	else	
		return false;
}

/**
*	查询注册表项
*/
#pragma INITCODE
bool QueryReg(const PCWSTR pKey, const PCWSTR pQueryKey, PCWSTR pBuf, ULONG size)
{
	UNICODE_STRING RegUnicodeString;
	HANDLE hRegister;

	RtlInitUnicodeString( &RegUnicodeString, pKey);

	OBJECT_ATTRIBUTES objectAttributes;

	InitializeObjectAttributes(&objectAttributes,
		&RegUnicodeString,
		OBJ_CASE_INSENSITIVE,
		NULL, 
		NULL );

	NTSTATUS ntStatus = ZwOpenKey( &hRegister,KEY_ALL_ACCESS,&objectAttributes);

	if (NT_SUCCESS(ntStatus))
	{
		UNICODE_STRING ValueName;

		RtlInitUnicodeString( &ValueName, pQueryKey);

		ULONG ulSize;
		ntStatus = ZwQueryValueKey(hRegister,   // get buffer size
			&ValueName,
			KeyValuePartialInformation ,
			NULL,
			0,
			&ulSize);

		if (ntStatus==STATUS_OBJECT_NAME_NOT_FOUND || ulSize==0)
		{
			ZwClose(hRegister);
			KdPrint(("The item is not exist\n"));
			return false ;
		}

		PKEY_VALUE_PARTIAL_INFORMATION pvpi = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(PagedPool,ulSize);

		ntStatus = ZwQueryValueKey(hRegister,
			&ValueName,
			KeyValuePartialInformation ,
			pvpi,
			ulSize,
			&ulSize);

		if (!NT_SUCCESS(ntStatus))
		{
			ZwClose(hRegister);
			KdPrint(("Read regsiter error\n"));
			return false;
		}
		//                type 
		if(pBuf != NULL && pvpi->Type == REG_EXPAND_SZ)
		{
			if(size <= pvpi->DataLength)
			{
				ZwClose(hRegister);
				KdPrint(("The item is not exist\n"));
				return false;
			}
			RtlMoveMemory((PVOID)pBuf,(PVOID)pvpi->Data,pvpi->DataLength);
		}

		ExFreePool(pvpi);
	}

	ZwClose(hRegister);
	return true;
}
