#pragma once

#define		MODULE_WAIT			0
#define		MODULE_UNKNOWN		1
#define		MODULE_PASSED		2
#define		MODULE_FILTERED		3

extern ULONG bModuleFilter;
extern bool g_bPassFilter;
/**
* Hook NtMapViewOfSection
*/
VOID HookSSDT();

/**
* Unhook NtMapViewOfSection
*/
VOID UnHookSSDT();

/**
* check hook safe
*/
bool checkHookSafe();