/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  ILog.h
 *      说明：  日志接口声明文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.08.06	范振兴	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */

/**
* 初始化文件日志。
* @param level:int 日志能够记录信息的最大等级，请参考LogConst.h文件内等级。
* @param logdirectory:wchar_t* 日志全工作路径！！其路径内所有用到的文件夹请提前创建好，在这里不创建，否则会导致记日志错误。
* @param logfilename:wchar_t* 日志文件名称
* @param size:long,允许一个日志文件的最大长度。
* @param strategy:int,日志策略，1为单文件，2为双文件互换记录。如果为单文件，那么记录的日志名就用输入用户名，如果为双文件，那么日志文件名后自动+00或01。
* 在使用该日志模块时在写日志之前要调用该函数进行初始化，在中间任何地方都可以对日志文件进行写入日志信息，当写完日志后，不再调用该函数写日志，那么要调用LogUninitialize进行清理。
*/
void LogInitialize(int level,wchar_t* logdirectory = L".\\Logs",wchar_t* logfilename = L"log.d",long size = 10000000,int strategy = 2);

/**
* 写日志函数
* @param iLevel:int,要写入日志信息的等级，请参考LogConst.h文件内等级。
* @param format:char*,输入信息的格式，此格式参考printf。
* @param ...:变参,参考printf，要写入的信息。
* 写日志信息接口，写日志信息大小请不要超过规定长度的大小！最大暂时为256字节！！
*/
void WriteSysLog(int iLevel, char *format,... );

/**
* 清除日志参数
* 无参数
*/
void LogUninitialize();

/**
* 设置系统内日志记录等级
* @param loglevel:int,日志能够记录信息的最大等级。
*/
void SetLogLevel(int level);

/**
* 获得系统内日志记录等级
* @return int 函数返回能够记录信息的最大等级。如果没有初始化日志，那么返回-1
*/
int GetLogLevel();
