/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  FilterClass.cpp
*      说明：  保护ID。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2011.01.11	  bh
*-----------------------------------------------------------*
*/
#include "FilterClass.h"
#include "ProcessFilter.h"


PVOID g_registerHandle = NULL;
extern ULONG g_processNameOffset;

/**
*   判断当前进程是否允许放过
*/
bool  isPassProcess()
{
	UINT32 otherPid[6] = {0};
	bool bReturn = false;
	int count = GetPidOther(otherPid,sizeof(otherPid)/sizeof(UINT32) );

	for(int i=0; i<=count; i++)
	{
		if( (UINT32)PsGetCurrentProcessId() == otherPid[i] )
		{
			kdP( ("pass the pid is: %d\n",otherPid[i]) );
			bReturn = true;
			break;
		}
	}

	return bReturn;
}

/**
*
*/
bool isProtectProcess(UINT32 uPPid)
{
	UINT32 uiPPid[6] = {0};
	bool bReturn = false;
	int iNum = 2;

	if( getPPidNum(uiPPid, sizeof(uiPPid)/sizeof(UINT32), iNum) )
	{
		for(int i=0; i<iNum; i++)
		{
			UINT32 seek=(uiPPid[i])&0xfffffffffffffffc;
			if(seek == (UINT32)(uPPid)
				|| (seek+1) == (UINT32)(uPPid)
				|| (seek+2) == (UINT32)(uPPid)
				|| (seek+3) == (UINT32)(uPPid) )
			{
				if( (UINT32)PsGetCurrentProcessId() != uPPid)
				{
					kdP( ("protect the pid is: %d\n",uiPPid[i]) );
					bReturn = true;
				}

				break;
			}
		}
	}

	return bReturn;
}


/**
*
*/
#define PROTECTID 556
OB_PREOP_CALLBACK_STATUS  ObjectPreCallbackFilter( 
	__in PVOID  RegistrationContext,
	__in POB_PRE_OPERATION_INFORMATION  OperationInformation
	)
{
	OB_PREOP_CALLBACK_STATUS obReturn = OB_PREOP_SUCCESS;

	if( OperationInformation->Operation != OB_OPERATION_HANDLE_CREATE )
		return obReturn;

	if( isProtectProcess((UINT)PsGetProcessId( (PEPROCESS)OperationInformation->Object)) && !isPassProcess() )
	//if( PsGetProcessId( (PEPROCESS)OperationInformation->Object)  == (HANDLE)PROTECTID ) 
	{
		PEPROCESS p = PsGetCurrentProcess();
		kdP(("operation process is %s    PID is: %d\n",(char*)p+g_processNameOffset, (UINT32)PsGetCurrentProcessId() ));
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
	}

	return obReturn;
}



/**
*  装载过滤处理模块
*/
bool loadFilter()
{
	NTSTATUS status;

	UNICODE_STRING ustrAltitude;
	RtlInitUnicodeString(&ustrAltitude, L"326666");

	OB_OPERATION_REGISTRATION  oor = {PsProcessType, OB_OPERATION_HANDLE_CREATE, ObjectPreCallbackFilter, NULL};
	
	OB_CALLBACK_REGISTRATION  ocr = {OB_FLT_REGISTRATION_VERSION, 1, ustrAltitude, NULL, &oor };

	status = ObRegisterCallbacks(&ocr, &g_registerHandle);

	if (!NT_SUCCESS(status))
	{
		switch(status)
		{
		case STATUS_FLT_INSTANCE_ALTITUDE_COLLISION :
			WriteSysLog(LOG_TYPE_DEBUG,L" altitude is already exist!\n");
			break;

		case STATUS_INVALID_PARAMETER: 
			WriteSysLog(LOG_TYPE_DEBUG,L" parameter is error!\n");
			break;
		}
		return false;
	}

	kdP(("obRegisterCallbacks is sucessful!\n"));
	return true;
}

/**
*	卸载过滤处理模块
*/
bool unloadFilter()
{
	if( g_registerHandle )
	{
		ObUnRegisterCallbacks(g_registerHandle);
		g_registerHandle = NULL;
	}
	return true;
}


