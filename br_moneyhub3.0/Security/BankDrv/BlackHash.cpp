

#include "ntddk.h"
#include "BlackHash.h"
#include "LogSystem.h"

#define		BLACK_HASH_BUCKET_SIZE		256

struct SECURE_HASH
{
	unsigned char Hash[HASH_SIZEB];
	struct SECURE_HASH* next;
};


//      =false, g_blackHashDataF can use .  =true  g_blackHashDataS can use
bool           g_bBlackHash = false;
SECURE_HASH*   g_blackHashDataF[BLACK_HASH_BUCKET_SIZE] = {0};
SECURE_HASH*   g_blackHashDataS[BLACK_HASH_BUCKET_SIZE] = {0};

//添加指针
SECURE_HASH ** g_blackHash = g_blackHashDataS;//初始化数据存于S中
//查询指针
SECURE_HASH ** g_queryHashB = g_blackHashDataF; // 这里应该初始化为F，防止在初始化的时候添加过程中用S进行过滤

bool            g_isReceiveB = false;
KMUTEX	    	g_blackHashMutex;
/**
* hash 函数
* @param hash 文件的hash值。
* return hash值。
*/
inline UCHAR HashFuncB(const unsigned char* hash)
{
	return hash[0];
}


void initialMutexB()
{
	::KeInitializeMutex(&g_blackHashMutex,0);
}
void  releaseMutexB()
{
	::KeReleaseMutex(&g_blackHashMutex,FALSE);
}

void  getBlackTable()
{
	if(g_bBlackHash == false)
	{	
		//DbgPrint("queryTable is balckS,next store is blackF\n");
		g_blackHash = g_blackHashDataF;
		KeWaitForSingleObject(&g_blackHashMutex,Executive,KernelMode,FALSE,NULL);
		g_queryHashB = g_blackHashDataS;
		releaseMutexB();

		g_bBlackHash = true;
	}
	else
	{
		//DbgPrint("queryTable is balckF,next store is blackS\n");
		g_blackHash = g_blackHashDataS;
		KeWaitForSingleObject(&g_blackHashMutex,Executive,KernelMode,FALSE,NULL);
		g_queryHashB = g_blackHashDataF;
		releaseMutexB();

		g_bBlackHash = false;
	}
}

bool AddBlackHash(const unsigned char* hash)
{
	if(!g_isReceiveB)
	{
		g_isReceiveB = true;
		///DbgPrint("begin store blackTable \n");
	}
	
	SECURE_HASH* secuHash = (SECURE_HASH*)ExAllocatePoolWithTag(PagedPool, sizeof(SECURE_HASH), 'knab');
	if(secuHash == NULL)
		return false;

	memcpy(secuHash->Hash, hash, HASH_SIZEB);

	UCHAR id = HashFuncB(hash);
	secuHash->next = g_blackHash[id];
	g_blackHash[id] = secuHash;

// 	DbgPrint ("AddBlackHash %2d: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", id, 
// 		hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], 
// 		hash[11], hash[12], hash[13], hash[14], hash[15]) ;


	return true;
}

// bool isRestartB()
// {
// 	ULONG num=0;
// 
// 	KeWaitForSingleObject(&g_blackHashMutex,Executive,KernelMode,FALSE,NULL);
// 	for(int i=0 ; i<BLACK_HASH_BUCKET_SIZE ; i++)
// 	{
// 		//SECURE_HASH* secuHash = g_secuHash[i];
// 		SECURE_HASH* secuHash = g_queryHashB[i];
// 		while(secuHash)
// 		{
// 			num ++;
// 			if(num > 30)
// 			{
// 				releaseMutexB();
// 				return true;
// 			}
// 
// 			secuHash = secuHash->next;
// 		}
// 	}
// 
// 	releaseMutexB();
// 	return false;
// }
//添加延时验证，如果超过 MAXELPASETIMERFORCHECK 则退出查询
#define     MAXELPASETIMERFORCHECKB    20 
bool IsHashBlack(const unsigned char* hash)
{
	KeWaitForSingleObject(&g_blackHashMutex,Executive,KernelMode,FALSE,NULL);

	UCHAR id = HashFuncB(hash);
	bool bReturn = false;

	SECURE_HASH* secuHash = g_queryHashB[id];
	unsigned __int64   uiStartTimer = __rdtsc();

	while(secuHash)
	{
		if(RtlCompareMemory(secuHash->Hash, hash, HASH_SIZEB) == HASH_SIZEB)
		{
			bReturn = true;
			break;
		}

		if( ((__rdtsc() - uiStartTimer) / (1000 * 1000 * 1000)) >= MAXELPASETIMERFORCHECKB )
		{
			WriteSysLog(LOG_TYPE_INFO,L"black elapse the large timer 6 seconds\n ");
			bReturn = false;
			break;
		}
		secuHash = secuHash->next;
	}

	releaseMutexB();
	return true == bReturn? true:false;
}

bool clearB(SECURE_HASH ** p)
{
	SECURE_HASH ** secuHashForDelete = p;

	if(g_isReceiveB)
		g_isReceiveB = false;
	

	for(int i = 0; i < BLACK_HASH_BUCKET_SIZE; i ++)
	{
		SECURE_HASH* secuHash = secuHashForDelete[i];
		SECURE_HASH* nextHash;

		secuHashForDelete[i] = NULL;

		while(secuHash)
		{
			nextHash = secuHash->next;
			ExFreePool(secuHash);
			secuHash = nextHash;
		}
	}
	return true;
}

bool ClearBlackHash(ULONG index)
{
	if(index ==1)
	{
		//DbgPrint("clear blackTable\n");
		return clearB(g_blackHash);
	}
	else if(index == 2)
	{
		//DbgPrint("clear black ALL\n");
		return clearB(g_blackHash) && clearB(g_queryHashB) ;
	}
	else
		return false;
}
