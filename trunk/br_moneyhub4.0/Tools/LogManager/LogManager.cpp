#include "LogManager.h"
#include "windows.h"

CLogManager* CLogManager::m_Instance = NULL;

CLogManager::CLogManager(void)
{
}

CLogManager::~CLogManager(void)
{
}

CLogManager* CLogManager::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CLogManager();
	return m_Instance;
}

int CLogManager::WriteLog(const std::string& filename, char* fmt, va_list ap)
{
	FILE* fp = FindAndCreateLog(filename);

	SYSTEMTIME time;
	GetSystemTime(&time);
	fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d:%03d]  ", time.wYear, time.wMonth, time.wDay, 
		time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

    vfprintf(fp, fmt, ap);
	return 0;
}

FILE* CLogManager::CreateLog(const std::string& filename)
{
	FILE* fp = fopen(filename.c_str(), "w");
	m_filemap[filename] = fp;
	return fp;
}

FILE* CLogManager::FindAndCreateLog(const std::string& filename)
{
	if(m_filemap.find(filename) == m_filemap.end())
		return CreateLog(filename);

	return m_filemap[filename];
}
