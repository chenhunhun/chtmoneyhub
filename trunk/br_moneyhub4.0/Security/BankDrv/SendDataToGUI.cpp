#include "ntddk.h"
#include "SendDataToGUI.h"
#include "LogSystem.h"

PKEVENT pEvent = NULL;
PKEVENT g_pEventFilterGo = NULL;
KEVENT  g_EventFilterGo ;

wchar_t			g_szDataFilePath[260] = {0};
unsigned char   g_uDataHash[16] = {0};

bool setSigned()
{
	if(pEvent)
		KeSetEvent(pEvent, IO_NO_INCREMENT, FALSE);
	return true;
}

bool setData(void * pPath, unsigned int pCLen, void* pHash, unsigned int pSLen)
{
	if( pCLen > DATAPATHLEN || pSLen > DATAHASHLEN )
	{
		WriteSysLog(LOG_TYPE_INFO,L" is: %s","data's len is to long!\n");
		return false;
	}

	RtlZeroMemory(g_szDataFilePath,DATAPATHLEN);
	RtlZeroMemory(g_uDataHash,DATAHASHLEN);

	RtlCopyMemory(g_szDataFilePath,pPath,pCLen);
	RtlCopyMemory(g_uDataHash,pHash,pSLen);
	return true;
}

void getData(void * pPath, void* pHash)
{
	RtlCopyMemory(pPath,g_szDataFilePath,DATAPATHLEN);
	RtlCopyMemory(pHash,g_uDataHash,DATAHASHLEN);
}

bool InitializeFilterGoEvent()
{
	KeInitializeEvent(&g_EventFilterGo, SynchronizationEvent, FALSE);
	g_pEventFilterGo = &g_EventFilterGo;

	return true;
}