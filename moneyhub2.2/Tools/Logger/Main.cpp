#include "ILog.h"
#include "LogConst.h"

void main()
{
	LogInitialize(LOG_TYPE_WARN,L"log.data",50000,2);

	int i=0;
	for(;i<1000;i++)
	{
		WriteSysLog(LOG_TYPE_ERROR,"%d%s",i,"this is a a test for write file!");
		WriteSysLog(LOG_TYPE_DEBUG,"%s","this is a a test for write file!");
		WriteSysLog(LOG_TYPE_WARN,"%s","this is a a test for write file!");
		WriteSysLog(LOG_TYPE_INFO,"%s","this is a a test for write file!");
	}

	LogUninitialize();

	LogInitialize(LOG_TYPE_ALL,L"log.data",50000,1);

	i=2000;
	for(;i<3000;i++)
	{
		WriteSysLog(LOG_TYPE_ERROR,"%s","this is a a test for write file!");
		WriteSysLog(LOG_TYPE_DEBUG,"%s","this is a a test for write file!");
		WriteSysLog(LOG_TYPE_WARN,"%s","this is a a test for write file!");
		WriteSysLog(LOG_TYPE_INFO,"%d%s",i,"this is a a test for write file!");
	}

	LogUninitialize();
}