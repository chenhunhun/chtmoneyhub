/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  DriverFrame.cpp
*      说明：  驱动框架。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.07.03	曹家鑫
*-----------------------------------------------------------*
*/

#include "ntddk.h"
#include "ServiceTable.h"
#include "MapViewOfSection.h"
#include "../common/DriverDefine.h"
#include "ProcessFilter.h"
#include "SecuModule.h"
#include "SecuHash.h"
#include "HardCode.h"
#include "FilterCache.h"
#include "utils/comm.h"


#include "writeFileForTest.h"
//////////////////////////////////////////////////////////////////////////

#define		DEVICE_NAME		L"\\Device\\MoneyHubPrt"
#define		DOS_NAME		L"\\DosDevices\\MoneyHubPrt"

//////////////////////////////////////////////////////////////////////////

extern LONG g_HookCounter;

ULONG  g_tmpB=0;  //add by bh
__int64    g_globalTime=0;

void DriverUnload(PDRIVER_OBJECT DriverObject);

//////////////////////////////////////////////////////////////////////////

NTSTATUS DispatchCreate(PDEVICE_OBJECT DriverObject, PIRP IRP) 
{
	IRP->IoStatus.Status = 0;
	IRP->IoStatus.Information = 0;
	IoCompleteRequest(IRP, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT DriverObject, PIRP IRP) 
{
	IRP->IoStatus.Status = 0;
	IRP->IoStatus.Information = 0;
	IoCompleteRequest(IRP, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP IRP) 
{
	KdPrint(("==>DriverDeviceControl\n"));

	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	ULONG tmpLen=0;
	PIO_STACK_LOCATION pIoStackIrp = IoGetCurrentIrpStackLocation(IRP);

	switch (pIoStackIrp->Parameters.DeviceIoControl.IoControlCode) 
	{
		case IOCTL_SET_PROTECT_PID:
			KdPrint(("IOCTL_SET_PROTECT_PID\n"));

			{
				unsigned char pUnPack[256];
				int unPackLength;

				unPackLength=DownloadUnPack((unsigned char *)IRP->AssociatedIrp.SystemBuffer,pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength,pUnPack);

				if (unPackLength>0)
				{
					PPID_INFO pInputBuffer = (PPID_INFO)pUnPack;
					int iStringLength = unPackLength;

					if(iStringLength != sizeof(PID_INFO)) // The PID should contain exactly 1 member.
						break;

					__int64 elapsedTime = __rdtsc() - pInputBuffer->currentTime;

					KdPrint(("IOCTL_SET_PROTECT_PID elapsed time: %I64d.\n", elapsedTime));

					if((elapsedTime > COMMUNICATE_TIME_LIMIT)||(elapsedTime <=COMMUNICATE_TIME_DOWN))
					{
						KdPrint(("IOCTL_SET_PROTECT_PID exceeds time limit.\n"));
					} else {
					
					// 加入进程 ID
						AddProtectPID(pInputBuffer->PID[0]);
					}

					ntStatus = STATUS_SUCCESS;
				}
			}
			break;

		case IOCTL_GET_PROTECT_PIDS:
			KdPrint(("IOCTL_GET_PROTECT_PIDS\n"));

			if (IRP->MdlAddress) 
			{
				PPID_INFO pUserBuffer = (PPID_INFO)MmGetSystemAddressForMdlSafe(IRP->MdlAddress, NormalPagePriority);
				ULONG OutputLength = pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength;
	
				ULONG PIDLength = OutputLength - sizeof(PID_INFO) + sizeof(UINT32);//1025*sizeof(unsigned int),last one for another

				// add by bh
				PPID_INFO tmpBuf=(PPID_INFO)ExAllocatePoolWithTag(PagedPool,OutputLength-sizeof(UINT32),'bnak');
				if(!tmpBuf)
					return ntStatus;
				
				///judge safe
				KdPrint(("entry check hook safe!\n"));
				if(checkHookSafe())
				{
					KdPrint(("              safe!\n"));
					tmpBuf->count = GetPIDs(tmpBuf->PID, PIDLength / sizeof(UINT32));
					tmpBuf->currentTime = __rdtsc();
					ULONG bufLength=sizeof(PID_INFO)+tmpBuf->count*sizeof(UINT32);
					tmpLen = UploadPack((PUCHAR)tmpBuf , bufLength , (PUCHAR)pUserBuffer);
				}
				else
				{
					KdPrint( ("           unfalse\n"));
					RtlZeroMemory(tmpBuf,OutputLength-sizeof(UINT32));
					tmpLen=0;
				}
				///
				//pUserBuffer->count = GetPIDs(pUserBuffer->PID, PIDLength / sizeof(UINT32));
			    //pUserBuffer->currentTime = __rdtsc();
				ExFreePoolWithTag(tmpBuf,'bnak');
				/////  end 

				ntStatus = STATUS_SUCCESS;
			}
			break;

		case IOCTL_SET_SECU_PATHS:
			KdPrint(("IOCTL_SET_SECU_PATHS\n"));
			{
				/////////////////add by bh
				if(!g_tmpB)
					setStartTime();
				////////////////end 

				/*PUCHAR pInputBuffer = (PUCHAR)IRP->AssociatedIrp.SystemBuffer;
				int iStringLength = pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength;

				ClearSecurePaths();

				ULONG index = 0;
				while(index < iStringLength)
				{
					ULONG length = *(PULONG)((ULONG)pInputBuffer + index);
					index += 4;
					if(index + length >= iStringLength)
						break;

					AddSecurePath((WCHAR*)((ULONG)pInputBuffer + index), length);

					index += length * 2 + 2;
				}*/

				ntStatus = STATUS_SUCCESS;
			}
			break;

		case IOCTL_SET_SECU_MD5:
			KdPrint(("IOCTL_SET_SECU_MD5\n"));
			{
				PUCHAR pInputBuffer;
				int iStringLength = pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength;

				unsigned char * pUnPack=(UCHAR *)ExAllocatePoolWithTag(PagedPool,iStringLength,'knab');
				int unPackLength;

				unPackLength=DownloadUnPack((unsigned char *)IRP->AssociatedIrp.SystemBuffer,pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength,pUnPack);
	
				//add by bh
				//iStringLength=*((ULONG*)pUnPack );
				RtlCopyBytes((PVOID)&iStringLength,(PVOID)pUnPack,sizeof(ULONG) );
				pInputBuffer=pUnPack+sizeof(ULONG);

				//if(!g_globalTime)
				//g_globalTime=*((__int64 *)(pInputBuffer+iStringLength) );
				RtlCopyBytes((PVOID)&g_globalTime,(PVOID)(pInputBuffer+iStringLength),8 );


				__int64 elapseTime=__rdtsc()-g_globalTime;
	
				if( (elapseTime<COMMUNICATE_TIME_LIMIT) && (elapseTime>=COMMUNICATE_TIME_DOWN)  )
				{
					KdPrint( ("entry elapse check!           ") );
					if (unPackLength>0)
					{KdPrint( ("entry length check!           ") );
						ClearHash();
						
						for(int i = 0; i <= iStringLength - HASH_SIZE; i += HASH_SIZE)
							AddSecureHash(pInputBuffer + i);

						ntStatus = STATUS_SUCCESS;
					}
				}
			
				ExFreePoolWithTag(pUnPack,'knab');
				//
			}
			break;

		case IOCTL_SET_UP_UNLOAD:
			KdPrint(("IOCTL_SET_UP_UNLOAD\n"));
			DeviceObject->DriverObject->DriverUnload = DriverUnload;
			ntStatus = STATUS_SUCCESS;
			break;
	}

	IRP->IoStatus.Status = 0;
	IRP->IoStatus.Information = tmpLen ;
	IoCompleteRequest(IRP, IO_NO_INCREMENT);
	KdPrint(("<==DriverDeviceControl\n"));
	return ntStatus;
}

void DriverUnload(PDRIVER_OBJECT DriverObject) 
{
	DbgPrint("==> DrvUnload\n");

	UNICODE_STRING DeviceName, DosDeviceName;
	PDEVICE_OBJECT DeviceObject = NULL;

	// Unhook NtMapViewOfSection
	UnHookNtMapViewOfSection();

	// 等待所有函数退出
	while(g_HookCounter)
	{
		LARGE_INTEGER interval;
		interval.QuadPart = -10 * 1000 * 1000;
		KeDelayExecutionThread(KernelMode, FALSE, &interval);
	}

	// 取消进程回调
	PsSetCreateProcessNotifyRoutine(OnProcessQuit, TRUE);

	RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&DosDeviceName, DOS_NAME);

	// 删除 Symbolic Link
	if (STATUS_SUCCESS != IoDeleteSymbolicLink(&DosDeviceName)) {
		KdPrint(("[E] Failed: IoDeleteSymbolicLink\n"));
	}

	// 删除 Device
	::IoDeleteDevice(DriverObject->DeviceObject);
	DbgPrint("<== DrvUnload\n");
}

BOOLEAN DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	DbgPrint("==> Driver Entry\n");

	NTSTATUS status = STATUS_SUCCESS;

	UNICODE_STRING DeviceName, DosDeviceName;
	PDEVICE_OBJECT DeviceObject = NULL;

	RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&DosDeviceName, DOS_NAME);

	// 创建 Device Object
	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);	

	if(!NT_SUCCESS(status))
		return STATUS_UNSUCCESSFUL;

	// 创建 SymbolicLink
	IoCreateSymbolicLink(&DosDeviceName, &DeviceName);

	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;

	// 初始化硬编码操作，如果失败则拒绝加载驱动
	if(InitHardCode() == false)
	{
		KdPrint(("System Version Unrecognized\n"));
		return STATUS_UNSUCCESSFUL;
	}

	// 注册进程回调函数
	PsSetCreateProcessNotifyRoutine(OnProcessQuit, FALSE);

	// Hook NtMapViewOfSection
	HookNtMapViewOfSection();

	//DriverObject->DriverUnload = DriverUnload;

	DriverObject->Flags |= DO_BUFFERED_IO;

	DriverObject->Flags &= (~DO_DEVICE_INITIALIZING);

	DbgPrint("<== Driver Entry\n");
	return STATUS_SUCCESS;
}