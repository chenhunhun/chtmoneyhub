#include "DriverDefine.h"

BOOLEAN Sleep(ULONG MillionSecond)
{
	NTSTATUS st;
	LARGE_INTEGER DelayTime;
	DelayTime = RtlConvertLongToLargeInteger(-10000*MillionSecond);
	st=KeDelayExecutionThread( KernelMode, FALSE, &DelayTime );
	return (NT_SUCCESS(st));
}