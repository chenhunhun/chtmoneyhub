#include "StdAfx.h"
#include "VerifyCache.h"
#include "windows.h"
#include "atlstr.h"
#include "assert.h"
#include "stdio.h"

CVerifyCache::CVerifyCache(void)
{
}

CVerifyCache::~CVerifyCache(void)
{
}

/*
   create file
*/
bool CVerifyCache::CreateDirectoryRecursively(const CStringW& dirpath) const
{
	int len = dirpath.ReverseFind(L'\\');
	CStringW LastDir = dirpath.Left(len);

	if(PathFileExistsW(LastDir) == FALSE)
		CreateDirectoryRecursively(LastDir);

	CreateDirectoryW(dirpath, NULL);

	return true;
}

CStringW CVerifyCache::tranlatePath(const CStringW& filepath) const
{
	WCHAR tempPath[MAX_PATH];
	GetTempPathW(MAX_PATH, tempPath);
	CStringW path = CStringW(tempPath) + CStringW(L"BankCache\\") + filepath[0] + filepath.Right(filepath.GetLength() - 2);
	//abandon the second Wchar
	return path;
}

//////////////////////////////////////////////////////////////////////////
// When IsFileVerified returns false it means unknown.
//////////////////////////////////////////////////////////////////////////
bool CVerifyCache::IsFileVerified(const CStringW& filepath, const void* sha, int shalen, bool& bVerified) const
{
	CStringW cachepath = tranlatePath(filepath) + L".ch";

	HANDLE hFile = CreateFileW(cachepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	byte* cachesha = new byte[shalen];
	assert(cachesha);

	DWORD read;
	ReadFile(hFile, cachesha, shalen, &read, NULL);
	if(read != shalen)
	{
		CloseHandle(hFile);
		return false;
	}

	if(memcmp(cachesha, sha, shalen) != 0)
	{
		CloseHandle(hFile);
		return false;
	}

	ReadFile(hFile, &bVerified, sizeof(bool), &read, NULL);

	CloseHandle(hFile);

	return true;
}

bool CVerifyCache::CreateCache(const CStringW& filepath, const void* sha, int shalen, bool bVerified) const
{
	/*if(filepath.GetLength() < 2)
		return false;
	
	CStringW cachepath = tranlatePath(filepath) + ".ch";
	int len = cachepath.ReverseFind('\\');
	CreateDirectoryRecursively(cachepath.Left(len));

	HANDLE hFile = CreateFileW(cachepath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD written;
	WriteFile(hFile, sha, shalen, &written, NULL);

	WriteFile(hFile, &bVerified, sizeof(bool), &written, NULL);

	CloseHandle(hFile);*/

	return true;
}

