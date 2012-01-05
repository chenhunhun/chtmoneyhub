#include "LogSystem.h"

bool initializeLog()
{
	wchar_t buf[255];
	if(true == QueryReg(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\MoneyHubPrt",L"ImagePath",buf,sizeof(buf)) )
	{
		wchar_t *p = wcsrchr(buf,L'\\');
		if(p != NULL)
		{
			*((p-buf+1)+buf) = L'\0';
			//wcscat_s(buf,sizeof(255),L"sysLog.txt");
			RtlMoveMemory(buf+wcslen(buf),L"syslog.txt",24);
#ifdef OFFICIAL_VERSION
			LogInitialize(LOG_TYPE_INFO,buf);
#else
			LogInitialize(LOG_TYPE_DEBUG,buf);
#endif
			return true;
		}
		else
			return false;
	}
	else
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