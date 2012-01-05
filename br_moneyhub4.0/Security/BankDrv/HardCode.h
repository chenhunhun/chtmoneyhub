#pragma once

#include "ntddk.h"

extern ULONG g_NtMapViewOfSectionID;

extern ULONG g_Offset_SegmentInSection;
extern ULONG g_Offset_ControlAreaInSegment;
extern ULONG g_Offset_FileObjectInControlArea;

extern RTL_OSVERSIONINFOW g_SysVersion;

/**
* 根据系统版本初始化硬编码偏移。
* return 返回是否成功。
*/
bool InitHardCode();
