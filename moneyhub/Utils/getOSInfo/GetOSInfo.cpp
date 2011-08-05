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
#include "windows.h"
#pragma comment(lib,"Kernel32.lib")


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
	BOOL  bIsWow64 = false;
	::IsWow64Process(GetCurrentProcess(),&bIsWow64);
	return bIsWow64;
}