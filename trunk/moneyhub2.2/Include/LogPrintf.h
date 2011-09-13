#ifndef __LOGPRINTF_H__
#define __LOGPRINTF_H__

//////////////////////////////////////////////////////////////////////////
//LogPrintfW 无argulist，字符串长度无限制 宽字符
//LogPrintfA 无argulist，字符串长度无限制 窄字符
//LogPrintf2W 有argulist，字符串长度限制950个字符 宽字符
//LogPrintf2A 有argulist，字符串长度限制950个字符 窄字符
//////////////////////////////////////////////////////////////////////////

//#define _SHOWLOG_
#define _SHOWLOG_3
#define _SHOWLOG_4
#define _SHOWSTARTEND_
#include <time.h>
#include <Shlwapi.h>
#include <shlobj.h>

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma comment(lib, "shlwapi.lib")

#define SE_MAX_PATH		260

//////////////////////////////////////////////////////////////////////////

static int iIndex = 0;
static const char *szStartEndLogFileName = "BankStartEnd";
static const char *szLogFileName = "BankLog";
static const char* szDLLogFileName = "BankLogDL" ;

_declspec(selectany) BOOL g_bEnableStartEndLog = -1 ;
_declspec(selectany) BOOL g_bEnableLog1 = -1 ;
_declspec(selectany) BOOL g_bEnableLog2 = -1 ;
_declspec(selectany) BOOL g_bEnableLog3 = -1 ;
_declspec(selectany) BOOL g_bEnableLog4 = -1 ;
_declspec(selectany) BOOL g_bEnableLogDL = -1 ;

static void InitLog(BOOL* pBool, LPCTSTR lpKey) 
{
	if ( *pBool!=-1 )
		return ;

	*pBool = FALSE ;

	HKEY hHKCU = NULL;
	DWORD dwErr = ::RegOpenCurrentUser(KEY_READ,&hHKCU);
	if( ERROR_SUCCESS == dwErr )
	{
		HKEY hSection = NULL;
		dwErr = ::RegOpenKey(hHKCU,_T("Software\\Bank\\"),&hSection);
		if ( ERROR_SUCCESS == dwErr )
		{
			DWORD dwValue = 0 ;
			DWORD dwSize = sizeof(DWORD);
			DWORD dwRtType = 0 ;
			dwErr = ::RegQueryValueEx(hSection,lpKey,NULL,&dwRtType,(LPBYTE)&dwValue,&dwSize);
			if ( ERROR_SUCCESS == dwErr && dwRtType==REG_DWORD )
				*pBool = 0==dwValue?FALSE:TRUE ;
			::RegCloseKey(hSection) ;
		}
		::RegCloseKey(hHKCU) ;
	}
}

//////////////////////////////////////////////////////////////////////////
// modified by cjj

static FILE* GetFilePointer(const char* szFileName)
{
	setlocale( LC_ALL, ".ACP" ); 

	char buf[SE_MAX_PATH] = { 0 };
	char szLogName[SE_MAX_PATH] = { 0 };
	SHGetSpecialFolderPathA(NULL, buf, CSIDL_APPDATA, TRUE);
	strcat(buf, "\\Bank_logs\\");

	if (!PathFileExistsA(buf) && !CreateDirectoryA(buf, NULL))
		return NULL;

	sprintf_s(szLogName, "%s%s%d.txt", buf, szFileName, iIndex);

	int iRetry = 0;
	FILE *pf = NULL;
	while (1) 
	{
		pf = fopen(szLogName, "a+");
		if (!pf)
		{
			// 就试10次，不行就退，别挂死了
			if (++iRetry >= 10)	
			{
				ATLASSERT(0);
				return NULL; 
			}
			iIndex ++;

			sprintf_s(szLogName, "%s%s%d.txt", buf, szFileName, iIndex);
		}
		else
			break;
	}
	return pf;
}

static void PrintCurrentTime(FILE* pf) 
{
	time_t ltime;
	time(&ltime);
	struct tm *today = localtime(&ltime);
	fprintf(pf, "\r\n%04d-%02d-%02d %02d:%02d:%02d ", today->tm_year+1900 , today->tm_mon+1, today->tm_mday, today->tm_hour, today->tm_min, today->tm_sec);
}

static void LogSetaskStartEnd(bool bStart, bool bSuccess, DWORD dwPid, int iCoreType)
{
	InitLog(&g_bEnableStartEndLog, _T("EnableStartEndLog")) ;

	if(!g_bEnableStartEndLog)
		return;

	FILE *pf = GetFilePointer(szStartEndLogFileName);
	if (pf == NULL)
		return;

	PrintCurrentTime(pf) ;

	fprintf(pf, "%s %s pid=0x%08x core=%d", bStart ? "StartProcess" : "EndProcess", bSuccess ? "Success" : "Failed", dwPid, iCoreType);

	fclose(pf);
}

static void LogMainStartEnd(bool bStart)
{
	InitLog(&g_bEnableStartEndLog, _T("EnableStartEndLog")) ;

	if(!g_bEnableStartEndLog)
		return;

	FILE *pf = GetFilePointer(szStartEndLogFileName);
	if (pf == NULL)
		return;

	PrintCurrentTime(pf) ;

	fprintf(pf, "%s", bStart ? "Start MainProcess" : "End MainProcess");

	fclose(pf) ;

}

static void LogUnExcepted(int iUnExcepted)
{
	InitLog(&g_bEnableStartEndLog, _T("EnableStartEndLog")) ;

	if(!g_bEnableStartEndLog)
		return;
	
	FILE *pf = GetFilePointer(szStartEndLogFileName);
	if (pf == NULL)
		return;

	PrintCurrentTime(pf) ;

	if (iUnExcepted == 2)
		fprintf(pf, "ax Hung");
	else if (3 == iUnExcepted)
		fprintf(pf, "ax crash");
	else if (4 == iUnExcepted)
		fprintf(pf, "ax create");

	fclose(pf) ;
}

// Log1 is used to print driver information.
static void LogPrintfA(LPCSTR Format, ...)
{
	InitLog(&g_bEnableLog1, _T("EnableLog1")) ;
	if(!g_bEnableLog1)
		return;

	FILE *p = GetFilePointer(szLogFileName);
	if (p == NULL)
		return;

	PrintCurrentTime(p) ;

	va_list arglist;
	va_start( arglist, Format);
	vfprintf(p, Format, arglist);
	va_end( arglist);

	fclose(p) ;

}


static void LogPrintf2A(LPCSTR Format, ...)
{
	InitLog(&g_bEnableLog2, _T("EnableLog2")) ;
	if(!g_bEnableLog2)
		return;

	FILE *p = GetFilePointer(szLogFileName);
	if (p == NULL)
		return;

	PrintCurrentTime(p) ;

	va_list arglist;
	va_start( arglist, Format);
	vfprintf(p, Format, arglist);
	va_end( arglist);

	fclose(p) ;

}

static void LogPrintf3A(LPCSTR Format, ...)
{
	InitLog(&g_bEnableLog3, _T("EnableLog3")) ;
	if(!g_bEnableLog3)
		return;

	FILE *p = GetFilePointer(szLogFileName);
	if (p == NULL)
		return;

	PrintCurrentTime(p) ;

	va_list arglist;
	va_start( arglist, Format);
	vfprintf(p, Format, arglist);
	va_end( arglist);

	fclose(p) ;

}

static void LogPrintf4A(LPCSTR Format, ...)
{
	InitLog(&g_bEnableLog4, _T("EnableLog4")) ;
	if(!g_bEnableLog4)
		return;

	FILE *p = GetFilePointer(szLogFileName);
	if (p == NULL)
		return;

	PrintCurrentTime(p) ;

	va_list arglist;
	va_start( arglist, Format);
	vfprintf(p, Format, arglist);
	va_end( arglist);

	fclose(p) ;

}

static void LogPrintf4W(LPCWSTR Format, ...)
{
	InitLog(&g_bEnableLog4, _T("EnableLog4")) ;
	if(!g_bEnableLog4)
		return ;

	FILE *p = GetFilePointer(szLogFileName);
	if (p == NULL)
		return;

	PrintCurrentTime(p) ;

	va_list arglist;
	va_start( arglist, Format);
	vfwprintf(p, Format, arglist);
	va_end( arglist);

	fclose(p) ;

}

static void LogPrintfDLW(LPCWSTR Format, ...)
{
	InitLog(&g_bEnableLogDL, _T("EnableLogDL")) ;
	if(!g_bEnableLogDL)
		return ;

	FILE *p = GetFilePointer(szDLLogFileName);
	if (p == NULL)
		return;

	PrintCurrentTime(p) ;

	va_list arglist;
	va_start( arglist, Format);
	vfwprintf(p, Format, arglist);
	va_end( arglist);

	fclose(p) ;

}

#pragma warning(pop)

#endif//__LOGPRINTF_H__
