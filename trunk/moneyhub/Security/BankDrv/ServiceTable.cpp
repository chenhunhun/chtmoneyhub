/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  ServiceTable.cpp
*      说明：  SSDT操作。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.07.03	曹家鑫
*-----------------------------------------------------------*
*/

#include "ntddk.h"
#include "ServiceTable.h"
#include "MemoryProtect.h"

typedef struct _SYSTEMSERVICEDESCRIPTORTABLE
{
	PVOID*   ServiceTableBase;
	PULONG   ServiceCounterTableBase;
	ULONG    NumberOfService;
	ULONG    ParamTableBase;
}SYSTEMSERVICEDESCRIPTORTABLE,*PSYSTEMSERVICEDESCRIPTORTABLE;

extern "C" PSYSTEMSERVICEDESCRIPTORTABLE KeServiceDescriptorTable;


/**
*   设置钩子服务函数
*/
PVOID UpdateService(ULONG ServiceID, PVOID ServiceProc)
{
	if(ServiceID >= KeServiceDescriptorTable->NumberOfService)
		return NULL;

	ULONG OldAttr;

	DisableWriteProtect(&OldAttr);
	PVOID PreviousProc = KeServiceDescriptorTable->ServiceTableBase[ServiceID];
	KeServiceDescriptorTable->ServiceTableBase[ServiceID] = ServiceProc;
	EnableWriteProtect(OldAttr);
	
	KdPrint(("Service Update: 0x%08x -> 0x%08x\n", PreviousProc, ServiceProc));

	return PreviousProc;
}

/**
*  检查钩子状态函数
*/
bool _checkHookSafeMapV(PVOID pFun,ULONG id)
{
	if(id >= KeServiceDescriptorTable->NumberOfService)
		return false;

	ULONG OldAttr;
	bool bReturn=false;

	//DisableWriteProtect(&OldAttr);
	PVOID PreviousProc = KeServiceDescriptorTable->ServiceTableBase[id];
	bReturn = PreviousProc==pFun? true:false;
	//EnableWriteProtect(OldAttr);

	return bReturn;
}