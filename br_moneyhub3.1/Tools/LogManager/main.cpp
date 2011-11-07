#include "export.h"
#include "LogManager.h"
#include "stdarg.h"

int LogManager::WriteLog(const char* filename, char* fmt, ...)
{
	va_list ap;
    va_start(ap, fmt);
	int result = CLogManager::GetInstance()->WriteLog(filename, fmt, ap);
    va_end(ap);
	return result;
}
