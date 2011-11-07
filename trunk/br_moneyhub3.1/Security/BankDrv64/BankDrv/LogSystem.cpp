#include "LogSystem.h"
#include "DriverDefine.h"

bool initializeLog()
{
	wchar_t buf[255];
	if(true == QueryReg(REGDRIVERPATH,L"ImagePath",buf,sizeof(buf)) )
	{
		wchar_t *p = wcsrchr(buf,L'\\');
		if(p != NULL)
		{
			*((p-buf+1)+buf) = L'\0';
			//wcscat_s(buf,sizeof(255),L"sysLog.txt");
			RtlMoveMemory(buf+wcslen(buf),L"syslog.txt",24);

			LogInitialize(LOG_TYPE_DEBUG,buf);
			return true;
		}
	}

	return false;
}


void uninitializeLog()
{
	LogUninitialize();
}
//void _cdecl WriteSysLog(ULONG iLevel, NTSTRSAFE_PWSTR format,...)
bool _cdecl wirteLog(ULONG iLevel, NTSTRSAFE_PWSTR format,...)
{	
	return true;
}