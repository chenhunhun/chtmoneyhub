/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  FileLogger.cpp
 *      说明：  文件日志抽象类实现文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.08.06	范振兴	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#include "FileLogger.h"
#include "LogConst.h"
#include <stdio.h>
//#pragma warning(disable:4996)

CFileLogger::CFileLogger(void)
{
	InitializeCriticalSection(&m_cs);
}

CFileLogger::~CFileLogger(void)
{
	DeleteCriticalSection(&m_cs);
}
/**
* 设置日志最大文件大小。
*/
void CFileLogger::SetFileSize(long size)
{
	m_filesize = size;
}
/**
* 设置日志文件名称。
*/
void CFileLogger::SetFileName(const wstring& filename)
{
	m_file = filename;
}
/**
* 向文件中写入日志信息的函数。
*/
bool CFileLogger::WriteData(const CLogInfo& info)
{
	wchar_t cinfo[MAX_INFO_LENGTH]={0};
	swprintf(cinfo,MAX_INFO_LENGTH,L"%04d-%02d-%02d %02d:%02d:%02d.%03d\t%d", info.time.wYear, info.time.wMonth, info.time.wDay, 
		info.time.wHour, info.time.wMinute, info.time.wSecond, info.time.wMilliseconds,info.pid);
	wstring sinfo(cinfo);

	sinfo += L"\t" + info.typeinfo + L"\t" + info.info + L"\r\n";


	char *pdata = (char*)sinfo.c_str();
	if(m_filehandle != INVALID_HANDLE_VALUE)
	{
		SetFilePointer(m_filehandle, 0, NULL, FILE_END);
		DWORD wl;
		if(!WriteFile(m_filehandle,pdata,(sinfo.length())*sizeof(wchar_t),&wl,NULL))
			return false;
		return true;
	}

	return false;
}
/**
* 检测文件大小的函数。
*/
bool CFileLogger::CheckFileSize(HANDLE& hFile)
{
	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	long dwFileSize = GetFileSize(hFile, NULL);

	if(dwFileSize == 0)
	{
		wchar_t uhead = 0xFEFF;
		DWORD dwLength;
		WriteFile(hFile,&uhead,sizeof(wchar_t),&dwLength,NULL);
	}
	if(dwFileSize >= m_filesize)
	{
		return false;
	}
	else
		return true;
}
/**
* 构造函数。
* 初始化参数。
*/
COneFileStrategyLogger::COneFileStrategyLogger(wstring logdirectory,wstring logfilename,long size)
{
	m_directory = logdirectory + L"\\";
	m_time = 0;
	m_file = logfilename;
	m_filesize = size;
	m_currentfile = m_directory + m_file;
	//为了防止出现中文错误,第一个参数统一使用宽字节
	m_filehandle = CreateFileW(m_currentfile.c_str(),GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以追加方式打开文件
	if(m_filehandle != INVALID_HANDLE_VALUE)
	{
		SetFilePointer(m_filehandle, 0, NULL, FILE_END);
	}
	//_s(&m_plogfile,);//以追加方式打开文件

	else
	{			
		//第一次打开失败，那么打开临时文件	
		m_currentfile = m_directory + L"~logtemp.dat";
		m_filehandle = CreateFileW(m_currentfile.c_str(),GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以追加方式打开文件
		if(m_filehandle != INVALID_HANDLE_VALUE)
		{
			SetFilePointer(m_filehandle, 0, NULL, FILE_END);
		}
		else
			return;
	}

	if(!CheckFileSize(m_filehandle))
	{
		CloseHandle(m_filehandle);

		m_filehandle = CreateFileW(m_currentfile.c_str(),GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以新建方式打开文件
		if(m_filehandle == INVALID_HANDLE_VALUE)
			return;

		SetFilePointer(m_filehandle, 0, NULL, FILE_END);
		wchar_t uhead = 0xFEFF;
		DWORD dwLength;
		WriteFile(m_filehandle,&uhead,sizeof(wchar_t),&dwLength,NULL);
	}
}

COneFileStrategyLogger::~COneFileStrategyLogger(void)
{
	CloseHandle(m_filehandle);
}

bool COneFileStrategyLogger::Write(const CLogInfo& info)
{
	//每写入一定次数后
	if(info.type > this->m_level)//在本系统内检测写入信息的级别
		return false;

	if(m_filehandle == INVALID_HANDLE_VALUE)
		return false;
	//当开始操作文件时开始锁定，防止其他线程写入
	EnterCriticalSection(&m_cs);
	if(m_time >= CHECK_TIME)
	{
		if(!CheckFileSize(m_filehandle))
		{
			CloseHandle(m_filehandle);
			m_filehandle = CreateFileW(m_currentfile.c_str(),GENERIC_WRITE | GENERIC_READ, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以追加方式打开文件
			if(m_filehandle == INVALID_HANDLE_VALUE)
			{
				LeaveCriticalSection(&m_cs);
				return false;
			}

			SetFilePointer(m_filehandle, 0, NULL, FILE_END);
			wchar_t uhead = 0xFEFF;
			DWORD dwLength;
			WriteFile(m_filehandle,&uhead,sizeof(wchar_t),&dwLength,NULL);

		}
		m_time = 0;
	}

	bool res = WriteData(info);
	if(res)
	{
		m_time ++;
	}
	LeaveCriticalSection(&m_cs);

	return res;
}

CTwoFileStrategyLogger::~CTwoFileStrategyLogger(void)
{
	::CloseHandle(m_filehandle);
}

/**
* 构造函数。
* 初始化参数。
*/
CTwoFileStrategyLogger::CTwoFileStrategyLogger(wstring logdirectory,wstring logfilename,long size)
{
	m_directory = logdirectory + L"\\";
	m_current = 0;
	m_time = 0;
	m_file = logfilename;
	m_filesize = size;

	GetFileName();
	//为了防止出现中文处理错误,第一个参数统一使用宽字节
	m_filehandle = CreateFileW(m_currentfile.c_str(),GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以追加方式打开文件
	if(m_filehandle != INVALID_HANDLE_VALUE)
	{
		SetFilePointer(m_filehandle, 0, NULL, FILE_END);
	}

	else
	{
		//打开第一个文件失败，那么打开临时文件
		m_file = L"~logtemp.d";
		GetFileName();
		m_filehandle = CreateFileW(m_currentfile.c_str(),GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以追加方式打开文件
		if(m_filehandle != INVALID_HANDLE_VALUE)
		{
			SetFilePointer(m_filehandle, 0, NULL, FILE_END);
		}
		else
			return;

	}
	//如果打开日志文件成功，测试日志文件大小，如果符合大小，那么就用该文件，否则换日志文件
	if(!CheckFileSize(m_filehandle))
	{
		::CloseHandle(m_filehandle);
		m_current = (m_current > 0) ? 0 : 1;

		GetFileName();
		m_filehandle = CreateFileW(m_currentfile.c_str(),GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以追加方式打开文件
		if(m_filehandle == INVALID_HANDLE_VALUE)
			return;
		SetFilePointer(m_filehandle, 0, NULL, FILE_END);
		wchar_t uhead = 0xFEFF;
		DWORD dwLength;
		WriteFile(m_filehandle,&uhead,sizeof(wchar_t),&dwLength,NULL);
	}
}

bool CTwoFileStrategyLogger::Write(const CLogInfo& info)
{
	if(info.type > this->m_level)//在本系统内检测写入信息的级别
		return false;

	if(m_filehandle == INVALID_HANDLE_VALUE)
		return false;
	//当开始操作文件时开始锁定，防止其他线程写入
	EnterCriticalSection(&m_cs);
	if(m_time >= CHECK_TIME)
	{
		if(!CheckFileSize(m_filehandle))
		{
			::CloseHandle(m_filehandle);
			m_current = (m_current > 0)? 0:1;
			GetFileName();			

			m_filehandle = CreateFileW(m_currentfile.c_str(),GENERIC_WRITE | GENERIC_READ, 
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以追加方式打开文件
			if(m_filehandle == INVALID_HANDLE_VALUE)
			{
				LeaveCriticalSection(&m_cs);
				return false;
			}
			SetFilePointer(m_filehandle, 0, NULL, FILE_END);
			wchar_t uhead = 0xFEFF;
			DWORD dwLength;
			WriteFile(m_filehandle,&uhead,sizeof(wchar_t),&dwLength,NULL);
		}
		m_time = 0;
	}

	bool res = WriteData(info);
	if(res)
	{
		m_time ++;
	}
	LeaveCriticalSection(&m_cs);

	return res;

}

void CTwoFileStrategyLogger::GetFileName()
{
	wchar_t filename[MAX_PATH];
	wsprintfW(filename,L"%s%s%02d",m_directory.c_str(),m_file.c_str(),m_current);
	wstring ftmp(filename);
	m_currentfile = ftmp;
}