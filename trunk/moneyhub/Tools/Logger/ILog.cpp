/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  ILog.cpp
 *      说明：  日志接口实现文件。
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
#include "FileLogger.h"
#include "ILog.h"
#include "LogConst.h"

ILogger *g_logger = NULL;

/**
* 初始化文件日志。
* 在使用该日志模块时在写日志之前要调用该函数进行初始化，在中间任何地方都可以对日志文件进行写入日志信息，当写完日志后，不再调用该函数写日志，那么要调用LogUninitialize进行清理。
*/
void LogInitialize(int level,wchar_t* logdirectory,wchar_t* logfilename,long size,int strategy)
{
	if(strategy == 1)
		g_logger = new COneFileStrategyLogger(logdirectory,logfilename,size);
	else
		g_logger = new CTwoFileStrategyLogger(logdirectory,logfilename,size);

	if(g_logger)
		g_logger -> SetLevel(level);
}

/**
* 清除日志参数
* 无参数
*/
void LogUninitialize()
{
	if(g_logger)
		delete g_logger;
	g_logger = NULL;
}

/**
* 写日志函数
* 写日志信息接口，写日志信息大小请不要超过规定长度的大小！最大暂时为256字节！！
*/
void WriteSysLog(int iLevel, char *format,... )
{ 

	CLogInfo msg;
	msg.type = iLevel;
	GetLocalTime(&msg.time);
	msg.pid = ::GetCurrentProcessId();
	switch(iLevel)
	{
		case LOG_TYPE_ERROR:	msg.typeinfo = "ERROR";break;
		case LOG_TYPE_WARN:		msg.typeinfo = "WARNING";break;
		case LOG_TYPE_INFO:		msg.typeinfo = "INFO";break;
		default:				msg.typeinfo = "DEBUG";break;
	}
    char strTemp[MAX_INFO_LENGTH];
    memset(strTemp, 0, sizeof(strTemp));
    char *pTemp = strTemp;
	//合成信息
 	va_list args; 
    va_start(args,format); 
    vsprintf(pTemp,format,args); 
    va_end(args); 

	string stemp(strTemp);
	msg.info = stemp;

    if (g_logger) 
    { 
		g_logger->Write(msg);
    } 

}

/**
* 设置系统内日志记录等级
* @param loglevel:int,日志能够记录信息的最大等级。
*/
void SetLogLevel(int level)
{
	if(g_logger)
	{
		g_logger->SetLevel(level);
	}
}

/**
* 获得系统内日志记录等级
*/
int GetLogLevel()
{
	if(g_logger)
	{
		return g_logger->GetLevel();
	}
	else
		return -1;
}