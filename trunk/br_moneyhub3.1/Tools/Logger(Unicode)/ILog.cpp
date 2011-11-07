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

CMoneyhubLog::CMoneyhubLog()
{
	//初始化互斥量句柄资源 郑鹏 2011.2.14 16:34 Add Begin
	m_hFileMutex = NULL;
	//初始化互斥量句柄资源 郑鹏 2011.2.14 16:34 Add End
}

CMoneyhubLog::~CMoneyhubLog()
{
	if(m_logger != NULL)
		LogUninitialize();

	//释放互斥量句柄资源 郑鹏 2011.2.14 16:34 Add Begin
	if (NULL != m_hFileMutex)
	{
		CloseHandle(m_hFileMutex);
		m_hFileMutex = NULL;
	}
	//释放互斥量句柄资源 郑鹏 2011.2.14 16:34 Add End
}
/**
* 初始化文件日志。
* 在使用该日志模块时在写日志之前要调用该函数进行初始化，在中间任何地方都可以对日志文件进行写入日志信息，当写完日志后，不再调用该函数写日志，那么要调用LogUninitialize进行清理。
*/
void CMoneyhubLog::LogInitialize(int level,wchar_t* logdirectory,wchar_t* logfilename,long size,int strategy,bool bSynchronize)
{
	if(strategy == 1)
		m_logger = new COneFileStrategyLogger(logdirectory,logfilename,size);
	else
		m_logger = new CTwoFileStrategyLogger(logdirectory,logfilename,size);

	if(m_logger)
	{
		ILogger * p = (ILogger *)m_logger;
		p -> SetLevel(level);
	}

	//创建互斥量句柄资源 郑鹏 2011.2.14 16:34 Add Begin
	if (bSynchronize)
	{
		m_hFileMutex = CreateMutexW(NULL,false,logfilename);
	}
	if (NULL == m_hFileMutex) //创建互斥对象失败
	{
		
	}
	//创建互斥量句柄资源 郑鹏 2011.2.14 16:34 Add End
}

/**
* 清除日志参数
* 无参数
*/
void CMoneyhubLog::LogUninitialize()
{
	if(m_logger)
	{
		ILogger * p = (ILogger *)m_logger;
		delete p;
	}
	m_logger = NULL;
}

/**
* 写日志函数
* 写日志信息接口，写日志信息大小请不要超过规定长度的大小！最大暂时为256个字符！！
*/
void CMoneyhubLog::WriteSysLog(int iLevel, wchar_t *format,... )
{ 
	CLogInfo msg;
	msg.type = iLevel;
	GetLocalTime(&msg.time);
	msg.pid = ::GetCurrentProcessId();
	switch(iLevel)
	{
		case LOG_TYPE_ERROR:	msg.typeinfo = L"ERROR";break;
		case LOG_TYPE_WARN:		msg.typeinfo = L"WARNING";break;
		case LOG_TYPE_INFO:		msg.typeinfo = L"INFO";break;
		default:				msg.typeinfo = L"DEBUG";break;
	}
    wchar_t strTemp[MAX_INFO_LENGTH];
    memset(strTemp, 0, sizeof(strTemp));
    wchar_t *pTemp = strTemp;
	//合成信息
 	va_list args; 
    va_start(args,format); 
    vswprintf(pTemp,MAX_INFO_LENGTH,format,args); 
    va_end(args); 

	wstring stemp(strTemp);
	msg.info = stemp;

    if (m_logger) 
    { 
		ILogger * p = (ILogger *)m_logger;
		//根据互斥量句柄判断是否进行同步 郑鹏 2011.2.14 17:00 Add Begin
		if (NULL != m_hFileMutex)
		{
			WaitForSingleObject(m_hFileMutex,INFINITE);
		}
		//根据互斥量句柄判断是否进行同步 郑鹏 2011.2.14 17:00 Add End

		p->Write(msg);

		//释放互斥对象 郑鹏 2011.2.14 17:00 Add Begin
		if (NULL != m_hFileMutex)
		{
			ReleaseMutex(m_hFileMutex);
		}
		//释放互斥对象 郑鹏 2011.2.14 17:00 Add End
    } 

}

/**
* 设置系统内日志记录等级
* @param loglevel:int,日志能够记录信息的最大等级。
*/
void CMoneyhubLog::SetLogLevel(int level)
{
	if(m_logger)
	{
		ILogger * p = (ILogger *)m_logger;
		p -> SetLevel(level);
	}
}

/**
* 获得系统内日志记录等级
*/
int CMoneyhubLog::GetLogLevel()
{
	if(m_logger)
	{
		ILogger * p = (ILogger *)m_logger;
		return p->GetLevel();
	}
	else
		return -1;
}