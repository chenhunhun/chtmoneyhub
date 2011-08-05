/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  hookSST.cpp
 *      说明：  一个内核挂钩类。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.08.27	毕海	初始版本
 *	
 *-----------------------------------------------------------*
 */

#include "hookSST.h"

#include "ProcessFilter.h"
#include "..//common/DriverDefine.h"
#include "..//common/VMProtectDDK.h"
#include "ServiceTable.h"
#include "LogSystem.h"

extern LONG g_HookCounter ;
extern ULONG  g_processNameOffset;

////////////////////////////////////////////////////////////////////////////
#pragma pack(push,1)	
typedef struct _SYSTEMSERVICEDESCRIPTORTABLE
{
	PVOID*   ServiceTableBase;
	PULONG   ServiceCounterTableBase;
	ULONG    NumberOfService;
	ULONG    ParamTableBase;
}SYSTEMSERVICEDESCRIPTORTABLE,*PSYSTEMSERVICEDESCRIPTORTABLE;
#pragma pack(pop)

extern "C" PSYSTEMSERVICEDESCRIPTORTABLE KeServiceDescriptorTable;

/////////////////////////////////////////////////////////////////////////////////
extern  "C" NTSYSAPI NTSTATUS NTAPI ZwOpenProcess(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId OPTIONAL
	);

typedef NTSTATUS (NTAPI *pNtOpenProcess)(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId OPTIONAL
	);

PVOID     pOriNtOpenProcess=NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////@1

PMDL   m_MDL = NULL;
PVOID *m_Mapped;


#define SYSTEMSERVICE(_function)  KeServiceDescriptorTable->ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)]
#define SYSCALL_INDEX(_Function) *(PULONG)((PUCHAR)_Function+1)
#define HOOK_SYSCALL(_Function, _Hook, _Orig )  \
	_Orig = (PVOID) InterlockedExchange( (PLONG) &m_Mapped[SYSCALL_INDEX(_Function)], (LONG) _Hook)
#define UNHOOK_SYSCALL(_Function, _Hook )  \
	InterlockedExchange( (PLONG) &m_Mapped[SYSCALL_INDEX(_Function)], (LONG) _Hook)

 

ULONG  g_openProcessId = 0;
/////////////////////////////////////////////////////////////////////////*NTSTATUS
NTSTATUS NTAPI MyNtOpenProcess(
							   OUT PHANDLE ProcessHandle,
							   IN ACCESS_MASK DesiredAccess,
							   IN POBJECT_ATTRIBUTES ObjectAttributes,
							   IN PCLIENT_ID ClientId OPTIONAL
							   );

/**
*  设置钩子函数
*
*/
NTSTATUS  sstHook_OpenProcess()
{
    if(m_MDL == NULL)
	{
		m_MDL = MmCreateMdl(NULL,KeServiceDescriptorTable->ServiceTableBase,KeServiceDescriptorTable->NumberOfService*4);

		if(!m_MDL)
			return STATUS_UNSUCCESSFUL;

		MmBuildMdlForNonPagedPool(m_MDL);
		m_MDL->MdlFlags = m_MDL->MdlFlags | MDL_MAPPED_TO_SYSTEM_VA;
		m_Mapped = (PVOID *)MmMapLockedPages(m_MDL, KernelMode);
		HOOK_SYSCALL(ZwOpenProcess,MyNtOpenProcess,pOriNtOpenProcess);
		g_openProcessId = (ULONG)SYSTEMSERVICE(ZwOpenProcess);

		return STATUS_SUCCESS;
	}

	UpdateService(SYSCALL_INDEX(ZwOpenProcess),(PVOID)MyNtOpenProcess);
	return STATUS_SUCCESS;
}

/**
*  还原钩子函数
*
*/
NTSTATUS  sstUnhook_OpenProcess()
{
	g_openProcessId = 0;

	UpdateService(SYSCALL_INDEX(ZwOpenProcess),pOriNtOpenProcess );

	if(m_MDL)
	{
		MmUnmapLockedPages(m_Mapped,m_MDL);
		IoFreeMdl(m_MDL);
	}

	return STATUS_SUCCESS;
}
/**
*   判断当前进程是否允许放过
*/
bool  isPassProcess()
{
	UINT32 otherPid[6] = {0};
	bool bReturn = false;
	int count = GetPidOther(otherPid,sizeof(otherPid)/sizeof(UINT32) );

	for(int i=0; i<count; i++)
	{
		if( (UINT32)PsGetCurrentProcessId() == otherPid[i] )
		{
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
	int iNum = 3;

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
					bReturn = true;
				}

				break;
			}
		}
	}

	return bReturn;
}
/**
*   钩子函数，进行进程保护
*/
ULONG g_tmp = 0;
#define KILLKERNEL                (0x0001)

#define PROCESS_CREATE_PROCESS    (0x0080)
#define PROCESS_CREATE_THREAD     (0x0002)
#define VMOPERATION               (0x0008)
#define VMREAD                    (0x0010)
#define VMWRITE                   (0x0020)

NTSTATUS NTAPI MyNtOpenProcess(
							   OUT PHANDLE ProcessHandle,
							   IN ACCESS_MASK DesiredAccess,
							   IN POBJECT_ATTRIBUTES ObjectAttributes,
							   IN PCLIENT_ID ClientId OPTIONAL
							   )
{
	//VMProtectBegin("MHVMP");
	//VMProtectBeginVirtualization("MHVMP");
	VMProtectBeginUltra("MHVMP");
	InterlockedIncrement(&g_HookCounter);
	bool bLeave=true;

	if( (KILLKERNEL != DesiredAccess) )
	{
		if( (DesiredAccess&PROCESS_CREATE_THREAD) || (DesiredAccess&VMOPERATION) || (DesiredAccess&VMWRITE) || (DesiredAccess&VMREAD))
		{
			if(ClientId->UniqueProcess > 0)
				if( isProtectProcess((UINT32)ClientId->UniqueProcess) && !isPassProcess() )
					bLeave =false;
		}
	}
		
	if( !bLeave)
	{
		PEPROCESS  p=PsGetCurrentProcess();

		ANSI_STRING ascallCode;
		RtlInitAnsiString(&ascallCode,(char *)p+g_processNameOffset);
		UNICODE_STRING  uni;
		RtlAnsiStringToUnicodeString(&uni,&ascallCode,true);
		if( g_tmp != (ULONG)PsGetCurrentProcessId() )
			;//WriteSysLog(LOG_TYPE_DEBUG,L"filter process Name: %s    PID : %d",uni.Buffer,PsGetCurrentProcessId());

		g_tmp = (ULONG)PsGetCurrentProcessId();
		RtlFreeUnicodeString(&uni);

		InterlockedDecrement(&g_HookCounter);
		return STATUS_ACCESS_DENIED;
	}
	else
	{
		InterlockedDecrement(&g_HookCounter);
		return ((pNtOpenProcess) pOriNtOpenProcess)(ProcessHandle,DesiredAccess,ObjectAttributes,ClientId);
	}
	VMProtectEnd();
}

/**
*
*
*/
bool _checkHookSafeOpenP()
{
	//return (g_openProcessId == (ULONG)SYSTEMSERVICE(ZwOpenProcess)) ? true : false ;
	if( g_openProcessId == (ULONG)SYSTEMSERVICE(ZwOpenProcess) )
		return true;
	else 
		return false;
}