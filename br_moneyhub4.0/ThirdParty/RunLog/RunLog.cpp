/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  RunLog.cpp
 *      说明：  日志接口声明文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2011.02.21	郑鹏	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */

#include "stdafx.h"
#include "RunLog.h"

CRunLog* CRunLog::m_pInsatance = NULL;

CRunLog::CRunLog()
{
	//1、获取环境变量路径
	// 将%%标志的路径，展开成一个完整路径
	WCHAR szAppDataPath[MAX_PATH + 1];
	ExpandEnvironmentStringsW(L"%APPDATA%\\MoneyHub", szAppDataPath, MAX_PATH);
	::CreateDirectoryW(szAppDataPath, NULL);
	m_MoneyHubLog.LogInitialize(LOG_WRITE_LEVEL,szAppDataPath,LOG_FILE_NAME,LOG_FILE_SIZE, LOG_WRITE_STRATEGY, true);
}

CRunLog::~CRunLog()
{
	m_MoneyHubLog.LogUninitialize();
}

CRunLog* CRunLog::GetInstance()
{	
	if(!m_pInsatance)
		m_pInsatance = new CRunLog();

	return m_pInsatance;
}

CMoneyhubLog* CRunLog::GetLog()
{
	return &m_MoneyHubLog;
}

