/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  FileLogger.h
 *      说明：  文件日志抽象类声明文件。
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
#include "ILogger.h"
#include <string>
#include "windows.h"
using namespace std;

#define CHECK_TIME			20	//当写入文件到达一定次数后检测文件大小

/**
* 文件日志写入的接口及抽象类
* 继承自Ilogger类
*/
class CFileLogger:public ILogger
{
public:
	CFileLogger(void);
	~CFileLogger(void);
protected:
	long m_filesize;		//设置记录文件大小
	//为了防止中文路径错误，在程序中涉及到文件路径名称都使用宽字节
	wstring m_file;			//记录应该存储的文件名称
	wstring m_directory;	//存储日志文件夹全路径
	wstring m_currentfile;	//记录实际存储全路径文件名
	HANDLE	m_filehandle;		//实际打开的文件
	CRITICAL_SECTION m_cs;	//控制同进程内同时调用该函数引起的资源竞争
	
public:
	/**
	* 设置日志最大文件大小。
	* @param size：long 文件大小。
	*/
	void SetFileSize(long size);

	/**
	* 设置日志文件名称。
	* @param filename：wstring 文件名称。
	*/
	void SetFileName(const wstring& filename);
	

protected:

	/**
	* 向文件中写入日志信息的函数。
	* @param info：CLogInfo 日志信息。
	* @return bool 写入是否成功
	* 直接向已经打开的m_logfile文件中写入消息
	*/
	bool WriteData(const CLogInfo& info);

	/**
	* 检测文件大小的函数。
	* @param file：fstream 文件流。
	* @return bool 判断是否符合规定大小，符合返回true，不符合返回false
	* 检测文件大小是否符合规定的函数，在使用该函数时请确保file文件流已经打开
	*/
	bool CheckFileSize(HANDLE& hFile);
};
/**
* 单文件日志策略写入类
* 继承自CFileLogger类
*/
class COneFileStrategyLogger:public CFileLogger
{
public:
	~COneFileStrategyLogger(void);
	/**
	* 构造函数。
	* @param logdirectory：wstring 日志文件夹。
	* @param logfilename：wstring 日志文件。
	* @param size：long 日志最大文件大小。
	* 初始化参数。
	*/
	COneFileStrategyLogger(wstring logdirectory = L".\\Logs",wstring logfilename = L"log",long size = 10000000);
public:
	/**
	* 写入日志信息函数。
	* @param CLogInfo& 信息类结构。
	* 当向系统内写入信息时，调用该函数写入,重载了ILogger类的该函数。
	*/
	bool Write(const CLogInfo& info);
private:
	int m_time;		//记录向该文件写入次数，当次数到达一定次数时才进行检查大小的操作，然后清空该参数
};
/**
* 双文件日志策略写入类
* 继承自CFileLogger类
*/
class CTwoFileStrategyLogger:public CFileLogger
{
public:
	~CTwoFileStrategyLogger(void);
	/**
	* 构造函数。
	* @param logdirectory：wstring 日志文件夹。
	* @param logfilename：wstring 日志文件。
	* @param size：long 日志最大文件大小。
	* 初始化参数。
	*/
	CTwoFileStrategyLogger(wstring logdirectory = L".\\Logs",wstring logfilename = L"log",long size = 10000000);

public:
	/**
	* 写入日志信息函数。
	* @param CLogInfo& 信息类结构。
	* 当向系统内写入信息时，调用该函数写入,重载了ILogger类的该函数。
	*/
	bool Write(const CLogInfo& info);
private:
	int m_time;		//记录向该文件写入次数，当次数到达一定次数时才进行检查大小的操作，然后清空该参数
	int m_current;	//记录当前使用的文件名的数字部分
private:
	void GetFileName();
};

//第3个策略尚未完成
/*class CMutiLevelFileStrategyLogger:public CFileLogger
{
public:
	~CMutiLevelFileStrategyLogger(void);
	CMutiLevelFileStrategyLogger(wstring logfilename = L"log",long size = 10000000);

public:
	bool Write(const CLogInfo& info);
};*/
