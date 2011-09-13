/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  ILogger.cpp
 *      说明：  日志抽象接口类实现文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.08.06	范振兴	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#include "ILogger.h"

ILogger::ILogger(void)
{
	m_level = LOG_TYPE_ALL;
}

ILogger::~ILogger(void)
{
}

void ILogger::SetLevel(int level)
{
	m_level = level;
};

int ILogger::GetLevel()
{
	return m_level;
};
