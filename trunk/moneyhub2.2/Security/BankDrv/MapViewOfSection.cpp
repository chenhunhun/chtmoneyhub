/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  MapViewOfSection.cpp
*      说明：  过滤功能框架。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.07.03	曹家鑫
*-----------------------------------------------------------*
*/

#include "ntddk.h"
#include "MapViewOfSection.h"
#include "ServiceTable.h"
#include "windef.h"
#include "QueryObjectName.h"

#include "ProcessFilter.h"
#include "FilterCache.h"
#include "md5.h"
#include "SecuHash.h"
#include "BlackHash.h"
#include "HardCode.h"
#include "hookSST.h"
#include "LogSystem.h"
#include "SendDataToGUI.h"

#define WRITELOGBH   100
//////////////////////////////////////////////////////////////////////////

#define		FILE_BUFFER_SIZE	(64 * 1024) 

extern "C" POBJECT_TYPE* MmSectionObjectType;

LONG g_HookCounter = 0;
LONG g_MapLock = 0;

//统计钩子失效次数，如超过__次，moneyhub退出
ULONG   g_count=0;

typedef NTSTATUS (NTAPI *NTMAPVIEWOFSECTION)(
	IN HANDLE SectionHandle,
	IN HANDLE ProcessHandle,
	IN OUT PVOID *BaseAddress,
	IN ULONG ZeroBits,
	IN ULONG CommitSize,
	IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
	IN OUT PULONG ViewSize,
	IN SECTION_INHERIT InheritDisposition,
	IN ULONG AllocationType,
	IN ULONG Protect);

static NTMAPVIEWOFSECTION NtMapViewOfSection = NULL;

//////////////////////////////////////////////////////////////////////////

/**
* 先检测黑名单，如果在黑名单内，直接拒，否则放行。
   如果不在黑名单，则检查白名单，在白名单内，无操作。
     如果不在白名单内，则将文件全路径及hash值传递到上层
* @param filename 文件全路径。
* return 如果文件安全则返回true，否则返回false。
*/
bool CheckIsFileHashSecure(const PUNICODE_STRING filename)
{
	/////////////////////////////////////////////

	// 比较已经过滤的文件名
	if(IsInBlackCache(filename) == true)
		return false;
	///////////////////////////////////////

	HANDLE hFile;
	OBJECT_ATTRIBUTES oaFile;
	InitializeObjectAttributes(&oaFile, filename, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	// 读权限打开文件，如果失败则认为文件不安全，返回false。
	IO_STATUS_BLOCK ioStatus;
	NTSTATUS status = ZwOpenFile(&hFile, GENERIC_READ, &oaFile, &ioStatus, FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("VerifyModule: ZwOpenFile: %ws %08x\n", filename->Buffer, status));
		return false;
	}

	unsigned char* fileBuf = (unsigned char*)ExAllocatePoolWithTag(PagedPool, FILE_BUFFER_SIZE, 'knab');
	if(fileBuf == NULL)
	{
		ZwClose(hFile);
		return false;
	}

	// 获取文件hash。
	MD5_CTX md5;
	MD5Init(&md5);
	ULONG sizeAll=0;
	
	FILE_STANDARD_INFORMATION fsi;
	ZwQueryInformationFile(hFile,&ioStatus,&fsi,sizeof(FILE_STANDARD_INFORMATION),FileStandardInformation);


	while(1)
	{
		NTSTATUS status = ZwReadFile(hFile, NULL, NULL, NULL, &ioStatus, fileBuf, 
			FILE_BUFFER_SIZE, NULL, NULL);
		if(!NT_SUCCESS(status))
			break;
		if(ioStatus.Information == 0)
			break;

		sizeAll += ioStatus.Information;
		MD5Update(&md5, fileBuf, ioStatus.Information);
	}
	ExFreePoolWithTag(fileBuf, 'knab');

	unsigned char final[16];
	MD5Final(final, &md5);

	ZwClose(hFile);

	//黑白名单校验
 	bool bOK = IsHashBlack(final);

	if( bOK )
	{
		if(!IsInBlackCache(filename))
		{
			WriteSysLog(LOG_TYPE_DEBUG,L" Fileter Module :%s", filename->Buffer);
			AddBlackCache(filename);
		}
		return false;
	}
	else if( !IsHashSecure(final) )//传递到上层 
	{
		if( setData(filename->Buffer,filename->Length,final,16) )
			setSigned();
	}	
	//
	return true;
}

/**
* 检查文件是否为PE文件
* @param filename 文件路径。
* return 如果文件是PE文件的话否回true，否则返回false。
* 注：现在的方法比较简单，只检查了文件的前两个字节，最好再检查几步。
*/
bool IsPEFile(const PUNICODE_STRING filename)
{
	HANDLE hFile;
	bool bIsPEFile = true;

	OBJECT_ATTRIBUTES oaFile;
	InitializeObjectAttributes(&oaFile, filename, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	IO_STATUS_BLOCK ioStatus;
	NTSTATUS status = ZwOpenFile(&hFile, GENERIC_READ, &oaFile, &ioStatus, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		FILE_SYNCHRONOUS_IO_NONALERT);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("IsPEFile: ZwOpenFile: %ws %08x\n", filename->Buffer, status));
		return true;
	}

	USHORT PESig;
	LARGE_INTEGER offset;
	offset.QuadPart = 0;
	ZwReadFile(hFile, NULL, NULL, NULL, &ioStatus, &PESig, 2, &offset, NULL);

	if(PESig != 'ZM')
		bIsPEFile = false;

	ZwClose(hFile);
	return bIsPEFile;
}

/**
*  过滤核心函数
*
*/
NTSTATUS NTAPI BkMapViewOfSection(IN HANDLE SectionHandle,
								  IN HANDLE ProcessHandle,
								  IN OUT PVOID *BaseAddress,
								  IN ULONG ZeroBits,
								  IN ULONG CommitSize,
								  IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
								  IN OUT PULONG ViewSize,
								  IN SECTION_INHERIT InheritDisposition,
								  IN ULONG AllocationType,
								  IN ULONG Protect)
{
	PVOID Section = NULL;
	ULONG bModuleFilter = MODULE_PASSED;

	// 获取Section Object
	NTSTATUS Status = ObReferenceObjectByHandle (SectionHandle, 0x00000004, //SECTION_MAP_READ
		*MmSectionObjectType, UserMode, (PVOID *)&Section, NULL);

	// 如果失败直接返回系统调用
	if(NT_SUCCESS(Status))
	{
		// 通过硬编码值找到FileObject
		PVOID Segment = *(PVOID*)((ULONG)Section + g_Offset_SegmentInSection);

		if(Segment)
		{
			PVOID ControlArea = *(PVOID*)((ULONG)Segment + g_Offset_ControlAreaInSegment);

			if(ControlArea)
			{
				PVOID FileObject = *(PVOID*)((ULONG)ControlArea + g_Offset_FileObjectInControlArea);

				// 如果是vista或者win7系统，FileObject需要清除最后3个bits
				if((g_SysVersion.dwMajorVersion == 6 && g_SysVersion.dwMinorVersion == 1) // Windows 7
					|| (g_SysVersion.dwMajorVersion == 6 && g_SysVersion.dwMinorVersion == 0)) // Windows Vista

					FileObject = (PVOID)((ULONG)FileObject & 0xFFFFFFF8);

				if(FileObject)
				{
					//KdPrint(("==>BkMapViewOfSection\n"));

					// 获取文件路径
					POBJECT_NAME_INFORMATION ObjectKernelName = BkQueryNameString((PVOID)FileObject);

					if (ObjectKernelName != NULL)
					{
						//检查文件是否是PE文件，如果是做进一步检查，不是的话通过检查。
						if (IsPEFile(&ObjectKernelName->Name))
						{
							// 检查是否是可信文件hash
							if (!CheckIsFileHashSecure(&ObjectKernelName->Name))
							{
								//if( MODULE_FILTERED != GetModuleFilter((ULONG)PsGetCurrentProcessId(),&ObjectKernelName->Name) )
								//SetModuleFilter((ULONG)PsGetCurrentProcessId(),&ObjectKernelName->Name,true);
								bModuleFilter = MODULE_FILTERED;
							} 
						}

						ExFreePool(ObjectKernelName);
					}

					//KdPrint(("<==BkMapViewOfSection\n"));
				}
			}
		}

		ObDereferenceObject(Section);

		if (bModuleFilter == MODULE_FILTERED)
		{	
			//KdPrint(("Module Filtered\n"));
			return STATUS_ACCESS_DENIED;
		}
	}

	// 调用原系统服务
	NTSTATUS result = NtMapViewOfSection(SectionHandle, ProcessHandle, BaseAddress,
		ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition,
		AllocationType, Protect);

	return result;
}



/**
*   钩子函数，进行模块过滤
*/
NTSTATUS NTAPI MyMapViewOfSection(IN HANDLE SectionHandle,
								  IN HANDLE ProcessHandle,
								  IN OUT PVOID *BaseAddress,
								  IN ULONG ZeroBits,
								  IN ULONG CommitSize,
								  IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
								  IN OUT PULONG ViewSize,
								  IN SECTION_INHERIT InheritDisposition,
								  IN ULONG AllocationType,
								  IN ULONG Protect)
{
	NTSTATUS result;

	InterlockedIncrement(&g_HookCounter);

	if (IsProcessProtected((DWORD)PsGetCurrentProcessId()) && (LONG)ProcessHandle == 0xFFFFFFFF)
		result = BkMapViewOfSection(SectionHandle, ProcessHandle, BaseAddress,
		ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition,
		AllocationType, Protect);
	else
		result = NtMapViewOfSection(SectionHandle, ProcessHandle, BaseAddress,
		ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition,
		AllocationType, Protect);

	InterlockedDecrement(&g_HookCounter);

	return result;
}

/**
*  设置钩子和卸载钩子函数
*/
VOID HookSSDT()
{
	NtMapViewOfSection = (NTMAPVIEWOFSECTION)UpdateService(g_NtMapViewOfSectionID, (PVOID)MyMapViewOfSection);
	sstHook_OpenProcess();
}

VOID UnHookSSDT()
{
	UpdateService(g_NtMapViewOfSectionID, (PVOID)NtMapViewOfSection);
	sstUnhook_OpenProcess();
}

/**
*  检查钩子状态函数
*/
bool checkHookSafe()
{
	if(g_count <= 6 )
	{   		
		if(!_checkHookSafeMapV((PVOID)MyMapViewOfSection, g_NtMapViewOfSectionID) )
		{
			UpdateService(g_NtMapViewOfSectionID, (PVOID)MyMapViewOfSection);
			g_count++;
		}
	    if(!_checkHookSafeOpenP())
		{
			sstHook_OpenProcess();
			g_count++;
		}
		return true;
	}
	else
	{
		g_count=0;
		return false;
	}	

}