/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  GetOSInfo.cpp
*      说明：  获取操作系统信息。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2011.1.12	bh
*-----------------------------------------------------------*
*/


#include "StdAfx.h"
#include "getOSInfo.h"


typedef BOOL  (WINAPI* pIsWow64Process)(
						   __in          HANDLE hProcess,
						   __out         PBOOL Wow64Process
						   );


CGetOSInfo* CGetOSInfo::m_hIns =NULL;

CGetOSInfo* CGetOSInfo::getInstance()
{
	if(!m_hIns)
		m_hIns = new CGetOSInfo();

	return m_hIns;
}

CGetOSInfo::CGetOSInfo()
{

}

CGetOSInfo::~CGetOSInfo()
{

}
/**
*  默认为32bits
*/
BOOL CGetOSInfo::isX64()
{
	BOOL  bIsWow = false;
	HMODULE hDll = GetModuleHandleW(L"Kernel32.dll");

	if(!hDll)
		hDll = LoadLibraryW(L"Kernel32.dll");

	if( hDll )
	{
		pIsWow64Process pFun = (pIsWow64Process)GetProcAddress(hDll, "IsWow64Process");
		
		if( pFun )
			pFun(GetCurrentProcess(), &bIsWow);
	}

	return bIsWow;
}