/************************************************************************
* 文件名称:Driver.h                                                 
* 
* 
*************************************************************************/
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif 

#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

#define		DEVICE_NAME		L"\\Device\\MoneyHubPrt"
#define		DOS_NAME		L"\\DosDevices\\MoneyHubPrt"

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))

typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice;
	UNICODE_STRING ustrDeviceName;	//设备名称
	UNICODE_STRING ustrSymLinkName;	//符号链接名
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

// 函数声明

NTSTATUS CreateDevice (IN PDRIVER_OBJECT pDriverObject);
VOID DDKUnload (IN PDRIVER_OBJECT pDriverObject);
NTSTATUS DDKDispatchRoutine(IN PDEVICE_OBJECT pDevObj,
								 IN PIRP pIrp);

NTSTATUS DDKPower(IN PDEVICE_OBJECT pDevObj,
				  IN PIRP pIrp);
NTSTATUS DispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP IRP);