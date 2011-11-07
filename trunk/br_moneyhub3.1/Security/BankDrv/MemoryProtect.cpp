/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  MemoryProtect.cpp
*      说明：  CPU写保护操作。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.07.03	曹家鑫
*-----------------------------------------------------------*
*/

#include "MemoryProtect.h"

VOID DisableWriteProtect(PULONG pOldAttr)
{
	ULONG uAttr;
	_asm
	{
		push	eax;
		mov		eax, cr0;
		mov		uAttr, eax;
		and		eax, 0FFFEFFFFh;
		mov		cr0, eax;
		pop		eax;
	};

	*pOldAttr = uAttr;
}

VOID EnableWriteProtect( ULONG uOldAttr )
{
	_asm
	{
		push eax;
		mov   eax, uOldAttr;
		mov   cr0, eax;
		pop   eax;
	};
}