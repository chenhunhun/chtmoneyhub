/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  ProcessFilter.cpp
*      说明：  过滤进程ID存储。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.07.03	曹家鑫 
*-----------------------------------------------------------*
*/


#include "ntddk.h"
#include "ProcessFilter.h"
#include "LogSystem.h"
#include "FilterCache.h"
#include "RegOperation.h"
#include "MyDefine.h"

#define		MAX_FILTER_NUMBER		256
#define  BEGINPID    0              //允许访问起点


extern ULONG  g_processNameOffset;

KMUTEX   g_addPIDMutex;
ULONG g_ProcessFilterID[MAX_FILTER_NUMBER] = {0};
int g_ProcessFilterCount = 0;

BOOL AddProtectPID(ULONG PID)
{
	if(g_ProcessFilterCount >= MAX_FILTER_NUMBER - 1)
		return FALSE;

	KeWaitForSingleObject(&g_addPIDMutex,Executive,KernelMode,FALSE,NULL);

	if( PID == 0 )
	{
		g_ProcessFilterCount = 0;	
	}
	else
	{
		g_ProcessFilterID[g_ProcessFilterCount] = PID;
		g_ProcessFilterCount++;
	}

	KeReleaseMutex(&g_addPIDMutex,FALSE);
	return TRUE;
}

BOOL RemoveProtectPID(ULONG PID)
{

	if(g_ProcessFilterCount <= 0)
		return FALSE;

	for(int i = 0; i < g_ProcessFilterCount; i++)
	{
		if(g_ProcessFilterID[i] == PID)
		{
			if(i == 0)//is ie core
			{
				//每次启动财金汇重新记录日志
				deleteLogFileA();
				for(int j=0; j<g_ProcessFilterCount; j++)
				{
					g_ProcessFilterID[j] = 0;
				}
				g_ProcessFilterCount =0;
			}
			else
			{
				if(i == g_ProcessFilterCount - 1)
				{
					g_ProcessFilterID[i] = 0;
					g_ProcessFilterCount--;
				}
				else
				{
					g_ProcessFilterID[i] = g_ProcessFilterID[g_ProcessFilterCount - 1];
					g_ProcessFilterID[g_ProcessFilterCount - 1] = 0;
					g_ProcessFilterCount--;
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL IsProcessProtected(ULONG PID)
{
 	if( (g_ProcessFilterID[0] == PID) || (g_ProcessFilterID[1] == PID) )
	{
		return TRUE;
	}
	else
		return FALSE;
}

VOID OnProcessQuit(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create)
{
	if(Create == TRUE)
	{
		PEPROCESS  p=PsGetCurrentProcess();
		
		ANSI_STRING ascallCode;
		RtlInitAnsiString(&ascallCode,(char *)p+g_processNameOffset);
		UNICODE_STRING  uni;
		RtlAnsiStringToUnicodeString(&uni,&ascallCode,true);	
		_wcslwr(uni.Buffer);
		
		if(RtlCompareMemory(BEGINOPERATEREG,uni.Buffer,uni.Length))
		{		
			if(SetReg(AUTORUN,REGNAME) == false)
				WriteSysLog(LOG_TYPE_DEBUG,L" is userinital create reg : %s","error");
		}
		RtlFreeUnicodeString(&uni);
		return;
	}
    
	KdPrint(("OnProcessQuit\n"));

	if(IsProcessProtected((ULONG)ProcessId))
		RemoveProtectPID((ULONG)ProcessId);
}

UINT32 GetPIDNumber()
{
	return g_ProcessFilterCount;
}

UINT32 GetKernelPIDs(UINT32* pPIDs, UINT32 count)
{
	pPIDs[0] = g_ProcessFilterID[1];
	if(g_ProcessFilterID[1] > 0)
		return 1;
	else
		return 0;
}
/**
*   返回允许访问的进程PID
*/
UINT32 GetPidOther(UINT32 *pPids, UINT32 count)
{
	int i = BEGINPID;
	UINT32 limit = min(g_ProcessFilterCount, count);
	
	for( int j = 0; i< limit; i ++)
		pPids[j++] = g_ProcessFilterID[i];

	return limit - BEGINPID ;
}
/**
*
*/
bool getPPidNum(UINT32 *uiPPid, UINT32 count, UINT32 iNum)
{
	if( iNum > count || !iNum)  return false;

	UINT32 limit = min(iNum,g_ProcessFilterCount);

	for(int i = 0 ; i< limit; i ++)
		uiPPid[i] = g_ProcessFilterID[i];	
	
	return true;
}

/**
*
*/
VOID initialMutexAddPID()
{
	KeInitializeMutex(&g_addPIDMutex,0);
}