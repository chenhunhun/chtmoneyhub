#include "StdAfx.h"
#include "FilterModuleInX64.h"
#include "../ThirdParty/Detours/APIHook.h"
#include "string"
#include "../Security/Authentication/encryption/MD5.h"

CFilterModuleInX64 g_filterModuleInX64App;

bool CFilterModuleInX64::m_bRunOnce = false;      //
typedef std::map<std::wstring, RETURNTYPE> FILTERMODULEPATH;
FILTERMODULEPATH             g_filteredModulePath;//已过滤模块路径
std::vector<std::wstring>    g_enviromentPath;    //dll搜索路径
HANDLE                       g_mutexLoadLibrary;  //用于多个过滤函数之间的同步

HANDLE                       g_handleWaitSecuMap;    //等待传入名单(白) ...("WAITSECUMAP")
HANDLE                       g_handleWaitBlackMap;   //等待传入名单(黑) ...("WAITBLACKMAP")


CheckModuleX64 g_secuHashX64A;
CheckModuleX64 g_secuHashX64B;
UINT32         g_secuIndex = 0;

CheckModuleX64 g_blackHashX64A;
CheckModuleX64 g_blackHashX64B;
UINT32         g_blackIndex = 0;

bool addWBHash(CheckModuleX64 &c, bool b);

CFilterModuleInX64::CFilterModuleInX64(void)
{
}

CFilterModuleInX64::~CFilterModuleInX64(void)
{
}

/**
*  等待白名单传入线程
*/
unsigned _stdcall waitSecuThread ( void * p)
{
	while(1)
	{
		WaitForSingleObject(g_handleWaitSecuMap,INFINITE);
		WaitForSingleObject(g_mutexLoadLibrary,INFINITE);
		
		if(g_secuIndex == 0 || g_secuIndex == 2)
		{
			addWBHash(g_secuHashX64A, true);
//OutputDebugStringW(L"is secuhash send to Driver! change tableA");
			g_secuIndex = 1;
		}
		else 
		{
			addWBHash(g_secuHashX64B, true);
//OutputDebugStringW(L"is secuhash send to Driver! change tableB");
			g_secuIndex = 2;
		}
		ReleaseMutex(g_mutexLoadLibrary);

	}
}

/**
*  等待黑名单传入线程
*/
unsigned _stdcall waitBlackThread ( void * p)
{
	while(1)
	{
		WaitForSingleObject(g_handleWaitBlackMap,INFINITE);
		WaitForSingleObject(g_mutexLoadLibrary,INFINITE);

		if(g_blackIndex == 0 || g_blackIndex == 2)
		{
			addWBHash(g_blackHashX64A, false);
//OutputDebugStringW(L"is blackhash send to Driver! change tableA");
			g_blackIndex = 1;
		}
		else 
		{
			addWBHash(g_blackHashX64B, false);
//OutputDebugStringW(L"is blackhash send to Driver! change tableB");
			g_blackIndex = 2;
		}

		ReleaseMutex(g_mutexLoadLibrary);

	}
}
/**
*    写入黑白名单
*    b = true   secuHash
*        false  blackHash
*/
bool addWBHash(CheckModuleX64 &c, bool b)
{
	HANDLE mphd = NULL;
	if(b)
		mphd=OpenFileMappingW(FILE_MAP_READ, FALSE, L"SHARESECUHASH");
	else
		mphd=OpenFileMappingW(FILE_MAP_READ, FALSE, L"SHAREBLACKHASH");

	if(mphd)
	{
		LPVOID lpMapAddr = MapViewOfFile(mphd,FILE_MAP_READ,0,0,0);
		if(lpMapAddr)
		{
			//将列表放入待查杀列表内存映射文件中
			DWORD num,i;
			memcpy(&num,lpMapAddr,sizeof(ULONG));
			if(num > 100000 || num < 0)
				return false;

// char buf[MAX_PATH] = {0};////D
// unsigned char *hash = (unsigned char *) lpMapAddr + sizeof(ULONG);
// sprintf(buf,"addWBHash : Lengths: %d  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
// 		num, hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], 
// 		hash[11], hash[12], hash[13], hash[14], hash[15]) ;
// 
// OutputDebugStringA(buf);////D


			char * unPackBuf = (char*)lpMapAddr;
			unPackBuf += sizeof(ULONG);
			for( i = 0;i <= num-HASH64 ; i += HASH64)
			{
				checkModuleX64 vi = {0};
				CopyMemory(vi.md5, unPackBuf + i , HASH64);
				c.insert(vi);
/*				OutputDebugStringW(L"dd");//////D*/
			}

			::UnmapViewOfFile(lpMapAddr);
		}
		::CloseHandle(mphd);
	}
// 	else 
// 		OutputDebugStringW(L"mapview is error!");


 	return 0;
}
/**
*  把DLL寻址路径添加进数组
*/
bool  addEnviromentPath()
{
	g_enviromentPath.clear();
	///////////////////////////////////////////////add execute file path
	wchar_t wcsName[MAX_PATH] = {0};
	GetModuleFileNameW(NULL,wcsName,_countof(wcsName) );
	wchar_t *pTmp = wcsrchr(wcsName,L'\\');
	if(pTmp)
	{
		*(pTmp) = L'\0';
		g_enviromentPath.push_back(wcsName);
	}
	////////////////////////////////////////////////add enviroment path
	wchar_t wcsBuf[3*MAX_PATH] = {0};

	ExpandEnvironmentStringsW(L"%Path%", wcsBuf,_countof(wcsBuf) );
	UINT len = wcslen(wcsBuf);

	for(int i=0, j=i; i<len; i++)
	{	
		if( *(wcsBuf+i) == L';')
		{
			wchar_t *p = NULL;
			if( p = wcschr(wcsBuf+j,L';') )
				*p =L'\0';

			if( *(wcsBuf+j+wcslen(wcsBuf+j)-1) == L'\\' )				
				*(wcsBuf+j+wcslen(wcsBuf+j)-1) = L'\0';

			g_enviromentPath.push_back(wcsBuf+j);

			j = i+1;
			if( p )
				*p = L';';
		}
		if( i == len-1)
		{
			if( *(wcsBuf+j+wcslen(wcsBuf+j)-1) == L'\\' )
				*(wcsBuf+j+wcslen(wcsBuf+j)-1) = L'\0';

			g_enviromentPath.push_back(wcsBuf+j);
		}
	}

	return g_enviromentPath.size()>1 ? true:false;
}
/**
*  过滤模块
*/
UCHAR * g_shareMemBuf;
HANDLE  g_shareMemHandle;
RETURNTYPE  checkModule(wchar_t * wcsP)
{
	WaitForSingleObject(g_mutexLoadLibrary, INFINITE);

	RETURNTYPE returnType = ISGRAY;
	CheckModuleX64 *secuCheck = NULL;
	CheckModuleX64 *blackCheck = NULL;

	////选择相应的容器
	if( 0 != g_secuIndex )
	{
		if( 1 == g_secuIndex )
		{
			secuCheck = &g_secuHashX64A;
		}
		else if(2 == g_secuIndex)
		{
			secuCheck = &g_secuHashX64B;
		}
	}

	if( 0 != g_blackIndex )
	{
		if( 1 == g_blackIndex )
		{
			blackCheck = &g_blackHashX64A;
		}
		else if ( 2 == g_blackIndex )
		{
			blackCheck = &g_blackHashX64B;
		}
	}
	//判断文件全路径
	std::wstring wstrPath;

	if( 0 == _waccess_s(wcsP, 0) )
	{
		wstrPath = wcsP;
	}
	else
	{
		for(int i=0; i<g_enviromentPath.size(); i++)
		{
			wstrPath = g_enviromentPath[i];
			wstrPath += L"\\";
			wstrPath += wcsP;
			//经过vsita 64bits 测试，在LoadLibraryA的时候，ws2_32.dll dnsapi.dll iphlpapi.dll三个模块无后缀,默认添加dll后缀
			if( !wcsrchr(wcsP, L'.') )
				wstrPath += L".dll";
			//
				
			if( 0 == _waccess_s(wstrPath.c_str(), 0) )
				break;
		}
	}
		
	//黑名单过滤
	checkModuleX64 hash;
	CMD5 md5;

	FILE* fp;
	USES_CONVERSION;
	fopen_s(&fp, W2A(wstrPath.c_str()), "rb");
	if( NULL != fp )
	{
		FILTERMODULEPATH::iterator it = g_filteredModulePath.find(wstrPath.c_str());
		if( it == g_filteredModulePath.end() )
		{
			while(!feof(fp))
			{
				unsigned char buffer[1024];
				int size = fread(buffer, 1, 1024, fp);
				md5.MD5Update(buffer, size);
			}
			md5.MD5Final(hash.md5);

			CheckModuleX64::iterator itHash;
			if( blackCheck )
			{
				itHash = blackCheck->find(hash);
				if( itHash != blackCheck->end() )
				{
// 					OutputDebugStringW(L"         is black modules");///D
// 					OutputDebugStringW(wstrPath.c_str());///D
					checkModuleX64 &vi = *itHash;
// char buf[MAX_PATH] = {0};////D
// unsigned char *hash1 = hash.md5;
// sprintf(buf," address is : %x  addWBHash kernel made:  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
// 	hash1, hash1[0], hash1[1], hash1[2], hash1[3], hash1[4], hash1[5], hash1[6], hash1[7], hash1[8], hash1[9], hash1[10], hash1[11], hash1[12], hash1[13], hash1[14], hash1[15]) ;
// 
// OutputDebugStringA(buf);////D
// hash1 = vi.md5;
// sprintf(buf,"table address is : %x addWBHash kernel made:  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", 
// 		hash1, hash1[0], hash1[1], hash1[2], hash1[3], hash1[4], hash1[5], hash1[6], hash1[7], hash1[8], hash1[9], hash1[10], hash1[11], hash1[12], hash1[13], hash1[14], hash1[15]) ;
// 
// OutputDebugStringA(buf);////D
					returnType = ISBLACK;
				}
				else if( secuCheck )
				{
					itHash = secuCheck->find(hash);

					if( itHash != secuCheck->end() )
						returnType = ISWHITE;
				}
				else
				{
					HANDLE eventHandle = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"GRAYHANDLE");
					if( eventHandle )
					{
						//共享灰名单数据
						if( g_shareMemBuf )//释放内存空间
							UnmapViewOfFile(g_shareMemBuf);
						if( g_shareMemHandle )
							CloseHandle(g_shareMemHandle);

						g_shareMemHandle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(ULONG)+MAX_PATH*sizeof(wchar_t)+HASH64, L"SHAREGRAYMEM");

						if( NULL == g_shareMemHandle )
							OutputDebugStringW(L"create 64bits mapping handle for gray  error!");
				
						g_shareMemBuf = (UCHAR*)MapViewOfFile(g_shareMemHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
						if( !g_shareMemBuf)
							OutputDebugStringW(L"MapViewOfFile 64bits mapping error!");


						*(ULONG*)g_shareMemBuf = 1;
						CopyMemory(g_shareMemBuf+sizeof(ULONG), wstrPath.c_str() , /*sizeof(wstrPath.c_str())*/ sizeof(wchar_t)*(wcslen(wstrPath.c_str())+1));
						CopyMemory(g_shareMemBuf+sizeof(ULONG)+sizeof(wchar_t)*MAX_PATH, hash.md5, HASH64);
						//
/*OutputDebugStringW((wchar_t*)(g_shareMemBuf+sizeof(ULONG)));*/
						SetEvent(eventHandle);
						CloseHandle(eventHandle);
						returnType = ISGRAY;
					}
				}
			}
			if( ISWHITE == returnType )
				g_filteredModulePath.insert(std::make_pair(wstrPath.c_str(), ISWHITE) );
			else if( ISBLACK == returnType )
				g_filteredModulePath.insert(std::make_pair(wstrPath.c_str(), ISBLACK) );
			else
				g_filteredModulePath.insert(std::make_pair(wstrPath.c_str(), ISGRAY) );	
		}
		else
		{
			returnType = it->second;
		}

		fclose(fp);
	}
	else
	{
		if( 0 != _waccess_s(wstrPath.c_str(), 0) )
		{
			OutputDebugStringW(L"file is not fount with enviromentPath!");
			OutputDebugStringW(wstrPath.c_str());
		}
	}

	ReleaseMutex(g_mutexLoadLibrary);

	return returnType;
}
///////////////////////////////////////////////////////////////////////////////////
typedef HMODULE  (  WINAPI *pLoadLibraryExA )(    
			          LPCSTR lpLibFileName,         
			          HANDLE hFile,                 
			          DWORD dwFlags                 
			   ) ;
pLoadLibraryExA pLLEAFun  = NULL;

HMODULE _stdcall detourLoadLibraryExA( LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags ) 
{
	USES_CONVERSION;
	return ISBLACK == checkModule(A2W(lpLibFileName) )? NULL : pLLEAFun(lpLibFileName, hFile, dwFlags);
}
///////
// typedef HMODULE  (  WINAPI *pLoadLibraryExW )(    
// 	 LPCWSTR lpLibFileName,
// 	 HANDLE hFile,
// 	 DWORD dwFlags                
// 	) ;
// static pLoadLibraryExW pLLEWFun  = NULL;
// 
// HMODULE _stdcall detourLoadLibraryExW( LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags ) 
// {
// 	OutputDebugStringW(L"         detour LoadLibraryExW Fun!\n" );
// 	wchar_t buf[255];
// 	wsprintf(buf,L"address:%x  name:%ws",pLLEWFun ,lpLibFileName);
// 	OutputDebugStringW(buf);
// 	return pLLEWFun(lpLibFileName, hFile, dwFlags);
// }
///////
typedef HMODULE  (  WINAPI *pLoadLibraryW )(    
	LPCWSTR lpLibFileName             
	) ;
pLoadLibraryW pLLWFun  = NULL;

HMODULE _stdcall detourLoadLibraryW( LPCWSTR lpLibFileName ) 
{
	return ISBLACK == checkModule( (wchar_t *)lpLibFileName)? NULL : pLLWFun(lpLibFileName);
}
///////
typedef HMODULE  (  WINAPI *pLoadLibraryA )(    
	LPCSTR lpLibFileName           
	) ;
pLoadLibraryA pLLAFun  = NULL;

HMODULE _stdcall detourLoadLibraryA( LPCSTR lpLibFileName ) 
{
	USES_CONVERSION;
	return ISBLACK == checkModule(A2W(lpLibFileName) )? NULL : pLLAFun(lpLibFileName);
}
///////////////////////////////////////////////////////////////////////////////////

bool CFilterModuleInX64::InitializeHook()
{
	if( m_bRunOnce )
		return 1;
	else
		m_bRunOnce = true;
    
/*	OutputDebugStringW(L"                 core is start !  install hook for 64bits!");/////////////////D*/

	addEnviromentPath();

	if(!( g_mutexLoadLibrary = CreateMutexW(NULL, FALSE, NULL)) )
		OutputDebugStringW(L"hook loadlibrary mutex creating is error!");

	//wait thread create
	g_handleWaitSecuMap  = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"WAITSECUMAP");
	g_handleWaitBlackMap = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"WAITBLACKMAP");
	while( NULL == g_handleWaitSecuMap || NULL == g_handleWaitBlackMap)
	{
		OutputDebugStringW(L"wait WBMap event is error!");

		g_handleWaitSecuMap  =CreateEventW(NULL,FALSE,FALSE,L"WAITSECUMAP");
		g_handleWaitBlackMap  =CreateEventW(NULL,FALSE,FALSE,L"WAITBLACKMAP");
	}
/*OutputDebugStringW(L"wait WBMap event is successFul!");////D*/
	CloseHandle( (HANDLE)_beginthreadex(0, 0, waitSecuThread, 0, 0, 0) );//守候白名单线程
	CloseHandle( (HANDLE)_beginthreadex(0, 0, waitBlackThread, 0, 0, 0) );//守候黑名单线程
	///

	bool bReturn = true;
	HINSTANCE h = LoadLibraryW(L"Kernel32.dll");
	if(h)
	{
		pLLEAFun= (pLoadLibraryExA)GetProcAddress(h, "LoadLibraryExA");
		//pLLEWFun= (pLoadLibraryExW)GetProcAddress(h, "LoadLibraryExW");
		pLLAFun = (pLoadLibraryA)GetProcAddress(h,"LoadLibraryA");
		pLLWFun= (pLoadLibraryW)GetProcAddress(h, "LoadLibraryW");

		if( !pLLAFun || !pLLWFun || !pLLEAFun  /*|| !pLLEWFun*/)
			bReturn = false;
	}
	else
		bReturn = false;


	if( NO_ERROR != Detours::DetourTransactionBegin() || NO_ERROR != Detours::DetourUpdateThread( ::GetCurrentThread() ))      
		bReturn = false;

	
	Detours::DetourAttach((PVOID *)& pLLEAFun, detourLoadLibraryExA);
	//Detours::DetourAttach((PVOID *)& pLLEWFun, detourLoadLibraryExW);
	Detours::DetourAttach((PVOID *)&pLLAFun,   detourLoadLibraryA);	
	Detours::DetourAttach((PVOID *)& pLLWFun,  detourLoadLibraryW);

	return Detours::DetourTransactionCommit() == NO_ERROR;
}

