#include "ntddk.h"
#include "FilterCache.h"
#include "LogSystem.h"

typedef struct _FILTER_ITEM
{
	UNICODE_STRING FilePath;
	bool Filtered;

	struct _FILTER_ITEM* next;
}FILTER_ITEM, *PFILTER_ITEM;

typedef struct _PROCESS_FILTERS
{
	ULONG PID;
	FILTER_ITEM* RootFilter;

	struct _PROCESS_FILTERS* next;
}PROCESS_FILTERS, *PPROCESS_FILTERS;

#define WHITELISTLIMIT     40*1024*1024

PROCESS_FILTERS     g_ProcessFilters={0};
KMUTEX              g_ProcessFiltersMutex;

PFILTER_ITEM		g_BlackCache = NULL;
KMUTEX				g_BlackCacheMutex;

bool InitFilterCache()
{
	g_ProcessFilters.RootFilter = NULL;
	g_ProcessFilters.PID = (ULONG)(-1);
	g_ProcessFilters.next = NULL;
	::KeInitializeMutex(&g_ProcessFiltersMutex,0);

	return true;
}

bool ClearFilterCache()
{
	///////////////////add by bh
	::KeWaitForSingleObject(&g_ProcessFiltersMutex,Executive,KernelMode,FALSE,NULL);

	for(PPROCESS_FILTERS p=g_ProcessFilters.next  ;p;  )
	{
		for(PFILTER_ITEM q=p->RootFilter ;q;  )
		{
		    PFILTER_ITEM prevQ=q->next;
			ExFreePoolWithTag(q->FilePath.Buffer,'knaB');
			ExFreePoolWithTag(q,'knaB');
			q=prevQ;
		}

		PPROCESS_FILTERS prevP=p->next;
		ExFreePoolWithTag(p,'knaB');
		p=prevP;
	}

	g_ProcessFilters.next = NULL;
	::KeReleaseMutex(&g_ProcessFiltersMutex,FALSE);
	///////////////////end
	return true;
}

PPROCESS_FILTERS FindProcessFilter(ULONG PID)
{
	PPROCESS_FILTERS ProcessFilter = g_ProcessFilters.next;
	PPROCESS_FILTERS tmpFilter=NULL;

	while(ProcessFilter)
	{
		if(ProcessFilter->PID == PID)
		{	
		//////////////////add by bh for accelerate
			if(tmpFilter)
			{
				tmpFilter->next=ProcessFilter->next;
				ProcessFilter->next=g_ProcessFilters.next;
				g_ProcessFilters.next=ProcessFilter;
			}
			break;
		}
		tmpFilter=ProcessFilter;
		//////////////////end 
		ProcessFilter = ProcessFilter->next;
	}

	//KdPrint(("FindProcessFilter. PID: %d  Return: %08x\n", PID, ProcessFilter));
	return ProcessFilter;
}

PPROCESS_FILTERS InsertProcessFilter(ULONG PID)
{
	PPROCESS_FILTERS ProcessFilter = (PPROCESS_FILTERS)ExAllocatePoolWithTag(PagedPool, 
		sizeof(PROCESS_FILTERS), 'knaB');

	if(ProcessFilter == NULL)
		return NULL;

	ProcessFilter->PID = PID;
	ProcessFilter->RootFilter = NULL;

	::KeWaitForSingleObject(&g_ProcessFiltersMutex,Executive,KernelMode,FALSE,NULL);
	ProcessFilter->next = g_ProcessFilters.next;
	g_ProcessFilters.next = ProcessFilter;
	::KeReleaseMutex(&g_ProcessFiltersMutex,FALSE);

	return ProcessFilter;
}

bool SetModuleFilter(ULONG PID, PUNICODE_STRING filepath, bool filtered)
{
	PPROCESS_FILTERS ProcessFilter = FindProcessFilter(PID);
	if(ProcessFilter == NULL)
	{
		ProcessFilter = InsertProcessFilter(PID);
		if(ProcessFilter == NULL)
			return false;
	}

	PFILTER_ITEM FilterItem = (PFILTER_ITEM)ExAllocatePoolWithTag(PagedPool, 
		sizeof(FILTER_ITEM), 'knaB');

	if(FilterItem == NULL)
		return false;

	FilterItem->Filtered = filtered;

	PVOID Buffer = (PVOID)ExAllocatePoolWithTag(PagedPool, filepath->MaximumLength, 'knaB');
	if(NULL==Buffer)
		return false;

	RtlInitEmptyUnicodeString(&FilterItem->FilePath, (PWSTR)Buffer, filepath->MaximumLength);
	RtlCopyUnicodeString(&FilterItem->FilePath, filepath);

	FilterItem->next = ProcessFilter->RootFilter;
	ProcessFilter->RootFilter= FilterItem;

	return true;
}
/************************************************************************/
/*           过滤函数，根据PID 和 filePath参数进行过滤
               返回值：MODULE_UNKNOWN  未知PID
			           MODULE_FILTERED 过滤掉HookDll.dll(黑名单加速过滤)
					   MODULE_PASS     PID+filePath准许通过，加速		   
*/
/************************************************************************/
ULONG GetModuleFilter(ULONG PID, PUNICODE_STRING filepath)
{
	PPROCESS_FILTERS ProcessFilter = FindProcessFilter(PID);
	if(ProcessFilter == NULL)
		return MODULE_UNKNOWN;

	if(wcsstr(filepath->Buffer, L"HOOKDLL.DLL"))//只有大写
		return MODULE_FILTERED;

	PFILTER_ITEM PrevFilterItem = NULL;
	PFILTER_ITEM FilterItem = ProcessFilter->RootFilter;
	while(FilterItem)
	{
		if(RtlCompareUnicodeString(&FilterItem->FilePath, filepath, FALSE) == 0)
		{
			// Change the item to the first one.    for accelerate
			if(PrevFilterItem != NULL)
			{
				PrevFilterItem->next = FilterItem->next;
				FilterItem->next = ProcessFilter->RootFilter;
				ProcessFilter->RootFilter = FilterItem;
			}

			if(FilterItem->Filtered)
				return MODULE_FILTERED;
			else
				return MODULE_PASSED;
		}

		PrevFilterItem=FilterItem;//add by bh
		FilterItem = FilterItem->next;
	}

	return MODULE_UNKNOWN;
}


/************************************************************************/
/*     
*/
/************************************************************************/
bool InitBlackCache()
{
	::KeInitializeMutex(&g_BlackCacheMutex,0);
	g_BlackCache = NULL;
	return true;
}

bool AddBlackCache(const PUNICODE_STRING filepath)
{
	PFILTER_ITEM tmp = (PFILTER_ITEM)ExAllocatePoolWithTag(PagedPool, 
		sizeof(FILTER_ITEM), 'BBuf');
	if( NULL == tmp)
		return false;

	tmp->FilePath.Buffer = (PWCH)ExAllocatePoolWithTag(PagedPool, 
		filepath->MaximumLength, 'DBuf');

	if(NULL == tmp->FilePath.Buffer)
	{
		ExFreePoolWithTag(tmp,'BBuf');
		return false;
	}
	tmp->FilePath.MaximumLength = filepath->MaximumLength;//***必须初始化
	RtlCopyUnicodeString(&tmp->FilePath, filepath);


	//开始添加，需要互斥
	::KeWaitForSingleObject(&g_BlackCacheMutex,Executive,KernelMode,FALSE,NULL);
	tmp->next = g_BlackCache;
	g_BlackCache = tmp;
	//KdPrint(("InsertProcessFilter1:%ws\n", filepath->Buffer));
	::KeReleaseMutex(&g_BlackCacheMutex,FALSE);
	
	return true;
}

bool IsInBlackCache(const PUNICODE_STRING filepath)
{
	::KeWaitForSingleObject(&g_BlackCacheMutex,Executive,KernelMode,FALSE,NULL);
	UINT count = 0;

	if(NULL == g_BlackCache)
	{
		::KeReleaseMutex(&g_BlackCacheMutex,FALSE);
		return false;
	}
	//KdPrint(("ISbufferFilter:%ws\n", filepath->Buffer));
	
	PFILTER_ITEM current = g_BlackCache;
	while(current != NULL)
	{
		if(RtlCompareUnicodeString(&current->FilePath, filepath, FALSE) == 0)
		{
			::KeReleaseMutex(&g_BlackCacheMutex,FALSE);
			return true;
		}

		if(count > 500)  
		{
			WriteSysLog(LOG_TYPE_INFO,L"elapse the large timer 500 counter\n ");
			break;
		}

		current = current->next;
		count ++;
	}

	::KeReleaseMutex(&g_BlackCacheMutex,FALSE);
	return false;
	

}

bool ClearBlackCache()
{
	if(NULL != g_BlackCache )
	{
		::KeWaitForSingleObject(&g_BlackCacheMutex,Executive,KernelMode,FALSE,NULL);
		PFILTER_ITEM current = g_BlackCache;
		for(; current != NULL;)
		{
			PFILTER_ITEM next = current->next;
			ExFreePoolWithTag(current->FilePath.Buffer,'DBuf');
			ExFreePoolWithTag(current,'BBuf');
			current = next; 
		}

		g_BlackCache = NULL;
		::KeReleaseMutex(&g_BlackCacheMutex,FALSE);
	}
	return true;
}