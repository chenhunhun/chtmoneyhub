#include <windows.h>
#include "libcpu.h"

UINT GetCPUID()
{
	BYTE szCpu[16]  = { 0 };
	UINT uCpuID     = 0U;
#ifndef _WIN64	
	__try 
	{
		_asm 
		{
				mov eax, 0
				cpuid
				mov dword ptr szCpu[0], ebx
				mov dword ptr szCpu[4], edx
				mov dword ptr szCpu[8], ecx
				mov eax, 1
				cpuid
				mov uCpuID, edx
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		uCpuID = 0;
	}
#endif
	return uCpuID;	
}