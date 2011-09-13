#include "StdAfx.h"
#include "LookUpHash.h"
#include "..\\Security\\Authentication\encryption\md5.h"
#include "tlhelp32.h"
#include "..\\Security\\BankLoader\BankLoader.h"
#include <set>
using namespace std;




#define TIMEELSPACE      3000

HANDLE   g_lookupThreadHandle = NULL ;
HWND LookUpHash::m_mainHwnd=NULL;


LookUpHash::LookUpHash(void):m_isAdd(false)
{
	for(int i = 0; i < SECURE_HASH_BUCKET_SIZE; i++)
	{
		m_secuHash[i] = NULL;
	}
	m_elpseTime=GetTickCount();
	//ZeroMemory(m_secuHash,sizeof(m_secuHash));
}

LookUpHash::~LookUpHash(void)
{
}


inline UCHAR LookUpHash::HashFunc(const unsigned char* hash)
{
	return hash[0];
}

bool LookUpHash::AddSecureHash(const unsigned char* hash)
{
	SECURE_HASH* secuHash = (SECURE_HASH*)new SECURE_HASH;
	if(secuHash == NULL)
		return false;

	memcpy(secuHash->Hash, hash, HASH_SIZE);

	UCHAR id = HashFunc(hash);
	secuHash->next = m_secuHash[id];
	m_secuHash[id] = secuHash;

	m_isAdd = true;
	return true;
}

bool LookUpHash::IsHashSecure(const unsigned char* hash)
{
	UCHAR id = HashFunc(hash);

	SECURE_HASH* secuHash = m_secuHash[id];
	while(secuHash)
	{
		if(memcmp(secuHash->Hash, hash, HASH_SIZE) == 0)
			return true;

		secuHash = secuHash->next;
	}

	return false;
}

bool LookUpHash::ClearHash()
{
	for(int i = 0; i < SECURE_HASH_BUCKET_SIZE; i++)
	{
		SECURE_HASH* secuHash = m_secuHash[i];
		SECURE_HASH* nextHash;

		m_secuHash[i] = NULL;

		while(secuHash)
		{
			nextHash = secuHash->next;
			free(secuHash);
			secuHash = nextHash;
		}
	}
	m_isAdd = false;
	return true;
}


bool LookUpHash::IsAdd()
{
	return m_isAdd;
}

///////////////////////////////////////////////////////////////////////////////////////////////

bool GenerateHash(const wchar_t* filename,unsigned char * final)
{
	//CMD5 md5;
	////md5.MD5Salt();

	//FILE* fp = _wfopen(filename, L"rb");
	//if(fp == NULL)
	//{
	//	return false;
	//}

	//while(!feof(fp))
	//{
	//	unsigned char buffer[1024];
	//	int size = fread(buffer, 1, 1024, fp);
	//	md5.MD5Update(buffer, size);
	//}

	//fclose(fp);

	////unsigned char final[MD5_HASH_SIZE];
	//md5.MD5Final(final);

	//return true;

	// gao
	ATLASSERT (NULL != filename && NULL != final);
	if (NULL == filename || NULL == final)
		return false;

	// 打开文件
	HANDLE hFile = CreateFile (filename,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	// 获取文件大小
	LARGE_INTEGER liSize;
	BOOL bRet = GetFileSizeEx (hFile, &liSize);
	if (FALSE == bRet && liSize.QuadPart <= 0)
	{
		CloseHandle (hFile);
		return false;
	}

	CMD5 md5;
	DWORD dwRead = 0;
	unsigned char buffer[1025];
	LARGE_INTEGER liRead;
	liRead.QuadPart = 0;

	while (liRead.QuadPart < liSize.QuadPart)
	{
		memset (buffer, 0,1024);
		// 读文件
		bRet = ReadFile (hFile, buffer, 1024, &dwRead, NULL);
		if (FALSE == bRet)
		{
			CloseHandle (hFile);
			return false;
		}

		// 统计读了多少字节
		liRead.QuadPart += 1024;

		md5.MD5Update (buffer, dwRead);
	}

	// 关闭文件句柄
	CloseHandle (hFile);

	md5.MD5Final(final);

	return true;

}

unsigned _stdcall lookupDll( void * p)
{
	LookUpHash luh;

	while(1)
	{
		DWORD pId = GetCurrentProcessId();	

		Sleep(TIMEELSPACE);
		
		//md5 encrypt
		UINT len = 0;
		if(*(int*)g_kernelHash == 0)
		{
			continue;
		}

		len = *(ULONG *)g_kernelHash;

		if( !luh.IsAdd() )
		{
			for(UINT i=0; i<=len-16; i+=16) // gao
			{			
				luh.AddSecureHash( (const unsigned char *)((char *)g_kernelHash+sizeof(ULONG)+i) );
			}
		}
	
		HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPALL,pId);

		if(hand == INVALID_HANDLE_VALUE)
		{
			goto MYERROR;
		}
		MODULEENTRY32W  mew = {sizeof(MODULEENTRY32W) };

		unsigned char final[MD5_HASH_SIZE];
		if(false  == Module32FirstW(hand,&mew) )
		{
			CloseHandle(hand);
			continue;
		}
		else
		{
			if(GenerateHash(mew.szExePath,final) )
			{
				if(!luh.IsHashSecure(final) )
				{
					CloseHandle(hand);
					goto MYERROR;
				}
			}
		}

		while(false != Module32NextW(hand,&mew) )
		{
			if(GenerateHash(mew.szExePath,final) )
			{
				if(!luh.IsHashSecure(final) )
				{
					CloseHandle(hand);
					goto MYERROR;
				}
			}
		}

		CloseHandle(hand);
	}

MYERROR:
	if(IDYES == MessageBoxW(NULL,L"检测到有非法入侵，为保证安全，请允许财金汇退出！",L"财金汇",MB_YESNO) )
	{
		luh.ClearHash();
		SendMessageW(LookUpHash::m_mainHwnd,WM_CLOSE,NULL,NULL);
		exit(-1);
	}
	else	
	{
		luh.ClearHash();
		g_lookupThreadHandle = (HANDLE)_beginthreadex(NULL,NULL,lookupDll,NULL,0,NULL);
	}

	return 1;
}

bool CheckLoadModule(LookUpHash& luh)
{
	DWORD pId = GetCurrentProcessId();
	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPALL,pId);

	if(hand == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	MODULEENTRY32W  mew = {sizeof(MODULEENTRY32W)};

	unsigned char final[MD5_HASH_SIZE];
	if(false  == Module32FirstW(hand,&mew) )
	{
		CloseHandle(hand);
		return false;
	}
	else
	{
		if(GenerateHash(mew.szExePath,final) )
		{
			if(luh.IsHashSecure(final) )
			{
				CloseHandle(hand);
				return false;
			}
		}
	}

	while(false != Module32NextW(hand,&mew) )
	{
		if(GenerateHash(mew.szExePath,final) )
		{
			if(luh.IsHashSecure(final) )
			{
				CloseHandle(hand);
				return false;
			}
		}
	}

	CloseHandle(hand);
	return true;
}