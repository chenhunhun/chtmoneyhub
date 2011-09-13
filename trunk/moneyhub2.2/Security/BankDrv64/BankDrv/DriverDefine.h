#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <NTDDK.h>
#ifdef __cplusplus
}
#endif 

/////////////special
#include "LogSystem.h"

#define REGDRIVERPATH    L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\MoneyHubPrt64"
#define BEGINOPERATEREG  L"userinit.exe"
#define SYSTEMNAME       "System"
/////////////

//#define  MYDBG
#ifndef  MYDBG
#define kdP( _x_ )  DbgPrint _x_
#else
#define kdP( _x_ )  
#endif

#define kdNew( _s )	ExAllocatePoolWithTag(PagedPool, _s, 'SYSQ')
#define kdfree(_p)	ExFreePoolWithTag(_p, 'SYSQ')
//#define kdFree( _p )	ExFreePool(_p)



BOOLEAN Sleep(ULONG MillionSecond);

