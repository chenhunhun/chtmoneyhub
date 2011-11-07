/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  HardCode.cpp
*      说明：  硬编码偏移。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.07.03	曹家鑫
*-----------------------------------------------------------*
*/

#include "HardCode.h"
#include "ntddk.h"

//////////////////////////////////////////////////////////////////////////

ULONG g_NtMapViewOfSectionID = 0;

ULONG g_Offset_SegmentInSection = 0;
ULONG g_Offset_ControlAreaInSegment = 0;
ULONG g_Offset_FileObjectInControlArea = 0;


//////////////////////////////////////////////////////////////////////////

RTL_OSVERSIONINFOW g_SysVersion = {0};

bool InitSystemVersion()
{
	g_SysVersion.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

	NTSTATUS status = RtlGetVersion(&g_SysVersion);

	KdPrint(("Major Version: %d\nMinor Version: %d\n", 
		g_SysVersion.dwMajorVersion, g_SysVersion.dwMinorVersion));

	if(!NT_SUCCESS(status))
	{
		KdPrint(("RtlGetVersion failed: %08x\n", status));
		return false;
	}

	return true;
}

bool XPInit()
{
	KdPrint(("Windows XP initialization.\n"));
	g_NtMapViewOfSectionID = 0x006c;

	g_Offset_SegmentInSection = 0x14;
	g_Offset_ControlAreaInSegment = 0;
	g_Offset_FileObjectInControlArea = 0x24;
	return true;
}

bool VistaInit()
{
	KdPrint(("Windows Vista initialization.\n"));
	g_NtMapViewOfSectionID = 0x00b1;

	g_Offset_SegmentInSection = 0x14;
	g_Offset_ControlAreaInSegment = 0;
	g_Offset_FileObjectInControlArea = 0x24;
	return true;
}

bool Win7Init()
{
	KdPrint(("Windows 7 initialization.\n"));
	g_NtMapViewOfSectionID = 0x00a8;

	g_Offset_SegmentInSection = 0x14;
	g_Offset_ControlAreaInSegment = 0;
	g_Offset_FileObjectInControlArea = 0x24;
	return true;
}

/**
*  根据系统版本不同得到相应的硬编码值
*
*/
bool InitHardCode()
{
	if(InitSystemVersion() == false)
		return false;

	if(g_SysVersion.dwMajorVersion == 6 && g_SysVersion.dwMinorVersion == 1)
		return Win7Init();
	if(g_SysVersion.dwMajorVersion == 6 && g_SysVersion.dwMinorVersion == 0)
		return VistaInit();
	else if(g_SysVersion.dwMajorVersion == 5 && g_SysVersion.dwMinorVersion == 1)
		return XPInit();
	else
		return false;
}