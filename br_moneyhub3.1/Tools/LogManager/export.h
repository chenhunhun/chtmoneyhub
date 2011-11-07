#pragma once

#ifdef LOGMANAGER_EXPORTS
#define LOGMANAGER_API __declspec(dllexport)
#else
#define LOGMANAGER_API __declspec(dllimport)
#endif


#ifdef _DEBUG
#define WRITE_LOG(filename, fmt, args, ...)	LogManager::WriteLog(filename, fmt, ##args)
#else
#define WRITE_LOG(filename, fmt, args, ...)
#endif

namespace LogManager
{
	LOGMANAGER_API int WriteLog(const char* filename, char* fmt, ...);
}