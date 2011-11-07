/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  FileLog.h
 *      说明：  驱动日志处理函数接口库声明文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.08.06	范振兴	初始版本

 *  开发环境：
 *  Visual Studio 2008+WinDDK\7600.16385.1
 *	 Include库:WinDDK\7600.16385.1\inc\ddk;D:\WinDDK\7600.16385.1\inc\crt;WinDDK\7600.16385.1\inc\api
 *	 Lib库:WinDDK\7600.16385.1\lib\wxp\i386\
 *			hal.lib int64.lib ntoskrnl.lib ntstrsafe.lib
 *
 *-----------------------------------------------------------*
 */
#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>


/**
* 驱动日志处理函数接口库，主要包含五个接口
* LogInitialize:初始化函数
* WriteSysLog:写日志接口
* LogUninitialize:清除日志参数
* SetLogLevel:设置日志等级
* GetLogLevel:获取日志等级

* 此外提供了函数供调用:
* GetLocalTime():获得当前系统时间

* 调用示例：
* LogInitialize(LOG_TYPE_DEBUG);

* WriteSysLog(LOG_TYPE_DEBUG, "%d%s",12,"test");
* WriteSysLog(LOG_TYPE_DEBUG, "%s","test");

* LogUninitialize();
*/



/**
* 初始化文件日志。
* @param loglevel:ULONG,日志能够记录信息的最大等级，请参考LogConst.h文件内等级。
* @param logfilename:WCHAR*,记录全工作路径:！！其路径内所有用到的文件夹请提前创建好，在这里不创建，否则会导致记日志错误。
* @param filesize:LONGLONG,允许一个日志文件的最大长度。
* @param strategy:USHORT,日志策略，1为单文件，2为双文件互换记录。如果为单文件，那么记录的日志名就用输入用户名，如果为双文件，那么日志文件名后自动+00或01。
* 在使用该日志模块时在写日志之前要调用该函数进行初始化，在中间任何地方都可以对日志文件进行写入日志信息，当写完日志后，不再调用该函数写日志，那么要调用LogUninitialize进行清理。
*/

void LogInitialize(ULONG loglevel,WCHAR* logfilename = L"\\Device\\HarddiskVolume1\\MoneyHubDriver.log",LONGLONG filesize = 10000000,USHORT strategy = 1);
/**
* 写日志函数
* @param iLevel:ULONG,要写入日志信息的等级，请参考LogConst.h文件内等级。
* @param format:NTSTRSAFE_PSTR*,输入信息的格式，此格式参考printf。
* @param ...:变参,参考printf，要写入的信息。
* 写日志信息接口，写日志信息大小请不要超过规定长度的大小！最大暂时为256字节！！
*/
void _cdecl WriteSysLog(ULONG iLevel, NTSTRSAFE_PSTR format,...);

/**
* 清除日志参数
* 无参数
*/
void LogUninitialize();

/**
* 设置系统内日志记录等级
* @param loglevel:ULONG,日志能够记录信息的最大等级。
*/
void SetLogLevel(ULONG loglevel);

/**
* 获得系统内日志记录等级
* @return ULONG 函数返回能够记录信息的最大等级。
*/
ULONG GetLogLevel();

/**
* 获取当前系统时间
* @return TIME_FIELDS 返回当前系统的时间。
*/
TIME_FIELDS GetLocalTime();


//   以下为本库内私有函数，外部不能调用！！

/**
* 获取当前文件名称
* @param pfactfilename:PUNICODE_STRING,需要提前定义，在本函数内进行了空间申请，申请完由调用者释放！！。
* @return USHORT 返回转换的结果:1表示成功，0表示失败。
* 根据g_CurrentFileNumber和g_logFile共同生成了新的文件名。
*/
USHORT GetLogFileName(OUT PUNICODE_STRING pfactfilename);

/**
* 检测文件大小并做后处理，如果文件超过规定大小重新创建文件
* @param pRecreateFilename:PUNICODE_STRING,当文件大小超过规定之后要用的文件名，重新创建的文件名称
* @param strategy:USHORT 重建的方针，1为Repalce，2为Open。
* @return NTSTATUS 执行该函数的结果，成功返回STATUS_SUCCESS
* 根据需要进行重建方针的选择，在日志系统初始化时一般采用1或2，当写入日志文件中判断大小采用1.
*/
NTSTATUS CheckLogFileSizeAndReCreateLogFile(PUNICODE_STRING pRecreateFilename,USHORT strategy = 1);

