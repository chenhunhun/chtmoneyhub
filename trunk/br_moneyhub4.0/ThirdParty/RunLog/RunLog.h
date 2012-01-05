/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  RunLog.h
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
#pragma once
#ifndef MY_OWN_LOG_INCLUDE_DEFINE_tag
#define MY_OWN_LOG_INCLUDE_DEFINE_tag
#endif

#include "ILog.h"

#ifndef OFFICIAL_VERSION
#define LOG_WRITE_LEVEL LOG_TYPE_DEBUG		//测试版用
#define LOG_FILE_SIZE 10000000				//文件大小为10M
#else
#define LOG_WRITE_LEVEL LOG_TYPE_INFO		//正式版用
#define LOG_FILE_SIZE 1000000				//文件大小为10M
#endif
#define LOG_FILE_NAME L"Run.log"			//日志文件相对路径
#define LOG_WRITE_STRATEGY 1               //写日志策略

class CRunLog
{
public:
	//得到该类的一个实例
	static CRunLog* GetInstance();

	//写log文件
	CMoneyhubLog* GetLog();
private:
	CRunLog();
	~CRunLog();
private:
	CMoneyhubLog m_MoneyHubLog;
	static CRunLog* m_pInsatance;
};
