#pragma once

#include <string>
#include <map>

#include "stdarg.h"

class CLogManager
{
	typedef std::map<std::string, FILE* > FileMap;
	FileMap m_filemap;

	FILE* FindAndCreateLog(const std::string& filename);
	FILE* CreateLog(const std::string& filename);

	CLogManager(void);
	~CLogManager(void);

	static CLogManager* m_Instance;
public:
	static CLogManager* GetInstance();

	int WriteLog(const std::string& filename, char* fmt, va_list ap);
};
