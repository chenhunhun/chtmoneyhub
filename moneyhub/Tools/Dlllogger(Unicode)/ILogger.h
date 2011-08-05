/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  ILogger.h
 *      说明：  日志抽象接口类声明文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.08.06	范振兴	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */

#pragma once
#include "stddef.h"
#include "LogConst.h"
#include "windows.h"
#include <string>
using namespace std;
/**
* 传递日志组成信息接口参数所用到的类结构
* 
*/
class CLogInfo
{
public:
	SYSTEMTIME	time;		//可以获得毫秒级的时间
	DWORD		pid;		//记录进程PID使用
	int			type;		//记录消息类型
	wstring		typeinfo;	//记录消息类型文字
	wstring		info;		//记录消息
};

class ILogger
{
public:
	ILogger(void);
	virtual ~ILogger(void);
protected:
	int m_level;			//记录当前管理器的日志记录级别

public:
	/**
	* 写入日志信息函数。
	* @param CLogInfo& 信息类结构。
	* 当向系统内写入信息时，调用该函数写入。
	*/
	virtual bool Write(const CLogInfo& info) = 0;

public:
	/**
	* 设置管理器的日志记录级别。
	* @param level 日志级别。
	* 设置系统内的可记录的最高日志信息级别。
	*/
	void SetLevel(int level);
	
	/**
	* 获得管理器的日志记录级别。
	* @param level 日志级别。
	* 获得系统内的日志级别。
	*/
	int GetLevel();
};
