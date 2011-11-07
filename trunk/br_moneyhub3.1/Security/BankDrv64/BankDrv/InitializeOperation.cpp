#include "InitializeOperation.h"
#include "FilterClass.h"
#include "ProcessFilter.h"
#include "LogSystem.h"


ULONG  g_processNameOffset=0;


/**
* 获得进程名称偏移
*/
bool getProcessNameOffset()
{
	PEPROCESS curproc;

	bool bReturn = false;

	curproc = PsGetCurrentProcess();

	for(int i=0; i< 4096; i++)
	{
		//tolower();
		if( !strncmp( SYSTEMNAME, (PCHAR) curproc + i, strlen(SYSTEMNAME) ))
		{  
			g_processNameOffset = i;
			bReturn = true;
		}
	}

	return bReturn;
}


bool _init()
{
	if( !initializeLog() )
		return false;

	initialMutexAddPID();

	if( !getProcessNameOffset() )
		return false;

	if( !NT_SUCCESS(PsSetCreateProcessNotifyRoutine(OnProcessQuit, FALSE) ) )
		return false;
	
	if( !loadFilter() )
		return false;

	return true;
}


void _quit()
{
	unloadFilter();
	PsSetCreateProcessNotifyRoutine(OnProcessQuit, TRUE);
}