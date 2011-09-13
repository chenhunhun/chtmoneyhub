/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  SecuHash.cpp
*      说明：  Hash白名单存储。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明
*	1.0.0	2010.07.03	曹家鑫 
*-----------------------------------------------------------*
*/


#include "ntddk.h"
#include "SecuHash.h"
#include "LogSystem.h"

#define		SECURE_HASH_BUCKET_SIZE		256

struct SECURE_HASH
{
	unsigned char Hash[HASH_SIZE];
	struct SECURE_HASH* next;
};


//      =false,is g_secuHashDataF can use .  =true is g_secuHashDataS can use
bool           g_bSechHash = false;
// 采用二级的hash将原hash值分散在不同的桶中，加快查找速度。
SECURE_HASH*   g_secuHashDataF[SECURE_HASH_BUCKET_SIZE] = {0};
SECURE_HASH*   g_secuHashDataS[SECURE_HASH_BUCKET_SIZE] = {0};
//SECURE_HASH* g_secuHash[SECURE_HASH_BUCKET_SIZE] = {0};
//添加指针
SECURE_HASH ** g_secuHash = g_secuHashDataS;//初始化数据存于S中
//查询指针
SECURE_HASH ** g_queryHash = g_secuHashDataF; // 这里应该初始化为F，防止在初始化的时候添加过程中用S进行过滤，这种情况应该不会发生

bool            g_isReceive = false;
KMUTEX	    	g_secuHashMutex;
/**
* hash 函数
* @param hash 文件的hash值。
* return hash值。
*/
inline UCHAR HashFunc(const unsigned char* hash)
{
	return hash[0];
}


void initialMutex()
{
	::KeInitializeMutex(&g_secuHashMutex,0);
}
void  releaseMutex()
{
	::KeReleaseMutex(&g_secuHashMutex,FALSE);
}

void  getSecuTable()
{
	if(g_bSechHash == false)
	{	
		g_secuHash = g_secuHashDataF;
		KeWaitForSingleObject(&g_secuHashMutex,Executive,KernelMode,FALSE,NULL);
		g_queryHash = g_secuHashDataS;
		releaseMutex();
		//DbgPrint("         change white table: g_secuHashDataF \n");
		g_bSechHash = true;
	}
	else
	{
		g_secuHash = g_secuHashDataS;
		KeWaitForSingleObject(&g_secuHashMutex,Executive,KernelMode,FALSE,NULL);
		g_queryHash = g_secuHashDataF;
		releaseMutex();
		//DbgPrint("         change white table: g_secuHashDataS \n");
		g_bSechHash = false;
	}
}

bool AddSecureHash(const unsigned char* hash)
{
	//
	if(!g_isReceive)
		g_isReceive = true;
	//
	SECURE_HASH* secuHash = (SECURE_HASH*)ExAllocatePoolWithTag(PagedPool, sizeof(SECURE_HASH), 'knab');
	if(secuHash == NULL)
		return false;

	memcpy(secuHash->Hash, hash, HASH_SIZE);

	UCHAR id = HashFunc(hash);
	secuHash->next = g_secuHash[id];
	g_secuHash[id] = secuHash;

// 	DbgPrint ("AddSecuHash %2d: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", id, 
// 		hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], 
// 		hash[11], hash[12], hash[13], hash[14], hash[15]) ;

	
	return true;
}

bool isRestart()
{
	ULONG num=0;

	KeWaitForSingleObject(&g_secuHashMutex,Executive,KernelMode,FALSE,NULL);
	for(int i=0 ; i<SECURE_HASH_BUCKET_SIZE ; i++)
	{
		//SECURE_HASH* secuHash = g_secuHash[i];
		SECURE_HASH* secuHash = g_queryHash[i];
		while(secuHash)
		{
			num ++;
			if(num > 30)
			{
				releaseMutex();
				return true;
			}

			secuHash = secuHash->next;
		}
	}
	releaseMutex();
	return false;
}
//添加延时验证，如果超过 MAXELPASETIMERFORCHECK 则退出查询
#define     MAXELPASETIMERFORCHECK    20 
bool IsHashSecure(const unsigned char* hash)
{
	KeWaitForSingleObject(&g_secuHashMutex,Executive,KernelMode,FALSE,NULL);

	UCHAR id = HashFunc(hash);
	bool bReturn = false;
	
	//SECURE_HASH* secuHash = g_secuHash[id];
	SECURE_HASH* secuHash = g_queryHash[id];
	unsigned __int64   uiStartTimer = __rdtsc();

	while(secuHash)
	{
		if(RtlCompareMemory(secuHash->Hash, hash, HASH_SIZE) == HASH_SIZE)
		{
			bReturn = true;
			break;
		}
			
		if( ((__rdtsc() - uiStartTimer) / (1000 * 1000 * 1000)) >= MAXELPASETIMERFORCHECK )
		{
			WriteSysLog(LOG_TYPE_DEBUG,L"elapse the large timer 6 seconds\n ");
			bReturn = false;
			break;
		}
		secuHash = secuHash->next;
	}
	
	releaseMutex();
	return true == bReturn? true:false;
}

bool clear(SECURE_HASH ** p)
{
	//
	SECURE_HASH ** g_secuHashForDelete = p;

	if(g_isReceive)
		g_isReceive = false;
	//

	for(int i = 0; i < SECURE_HASH_BUCKET_SIZE; i ++)
	{
		SECURE_HASH* secuHash = g_secuHashForDelete[i];
		SECURE_HASH* nextHash;

		g_secuHashForDelete[i] = NULL;

		while(secuHash)
		{
			nextHash = secuHash->next;
			ExFreePool(secuHash);
			secuHash = nextHash;
		}
	}
	return true;
}

bool ClearHash(ULONG index)
{
	if(index ==1)
	{
		return clear(g_secuHash);
	}
	else if(index == 2)
	{
		return clear(g_secuHash) && clear(g_queryHash) ;
	}
	else
		return false;
}