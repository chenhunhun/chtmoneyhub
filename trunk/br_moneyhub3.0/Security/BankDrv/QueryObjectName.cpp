/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  QueryObjectName.cpp
*      说明：  获取文件路径。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.07.03	曹家鑫
*-----------------------------------------------------------*
*/


#include "ntddk.h"
#include "undoc.h"

POBJECT_NAME_INFORMATION BkQueryNameString(IN PVOID Object)
{
	ULONG ReturnLength = 1024;

	POBJECT_NAME_INFORMATION ObjectNameInfo = (POBJECT_NAME_INFORMATION)ExAllocatePoolWithTag( PagedPool, ReturnLength, 'knab' );

	if ( ObjectNameInfo != NULL ) 
	{
		RtlZeroMemory(ObjectNameInfo,ReturnLength);//初始化内存为0，防止错误,fanzhenxing 增加
		NTSTATUS Status = ObQueryNameString(Object, ObjectNameInfo, ReturnLength, &ReturnLength);

		if(NT_SUCCESS(Status) && (ObjectNameInfo->Name.Length != 0))
			return( ObjectNameInfo );
		else 
		{
			ExFreePool(ObjectNameInfo);
			return NULL;
		}
	}

	return NULL;
}