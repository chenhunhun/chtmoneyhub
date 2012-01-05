#include "StdAfx.h"
#include "BankModVerify.h"
#include "windows.h"
#include "Psapi.h"

//#include "../encryption/AES.h"
//#include "../encryption/SHA512.h"
//#include "../encryption/md5.h"
//#include "../encryption/RSA.h"
//#include "../encryption/Encryption.h"
//#include "../CHKReader/export.h"
#include "../../../Encryption/CHKFile/CHK.h"

CBankModVerify::CBankModVerify(void)
{
}

CBankModVerify::~CBankModVerify(void)
{
}

bool CBankModVerify::Init(const char* filename)
{
	return ReadCHK(filename);
}

bool CBankModVerify::ReadCHK(const char* filename)
{
	bool result = false;
	unsigned char* pTextBase = NULL;
//_asm int 3;
	/*
	unsigned char* pText = CHKReader::GetCHKBuffer(filename);
	unsigned char* pTextBase = pText;
	*/

	// adamÀ„∑®
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		goto FAILED;
	
	DWORD dwLength = GetFileSize(hFile, NULL);
	unsigned char* lpBuffer = new unsigned char[dwLength + 1];

	DWORD dwRead = 0;
	BOOL bRet = ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL);
	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	if (!bRet)
	{
		delete []lpBuffer;
		goto FAILED;
	}

	unsigned char* pText = new unsigned char[dwRead];
	pTextBase = pText;
	int contentLength = unPackCHK(lpBuffer, dwRead, pText);
	delete []lpBuffer;

	if (contentLength < 0)
		goto FAILED;

	//////////////////////////////////////////////////////////////////////////
	// CHK file verified, read the file hashes
	int fileNumber = *(int*)pText;
	pText += 4;
	for(int i = 0; i < fileNumber; i++)
	{
		VerifyInfo vi;
		memcpy(vi.md5, pText, MD5_HASH_SIZE);
		pText += MD5_HASH_SIZE;

		m_moduleHash.insert(vi);
	}

	result = true;

FAILED:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	if(pTextBase)
		delete pTextBase;

	return result;
}


bool CBankModVerify::initCheck(const char * hash,int hashLen)
{
	bool result = false;
	unsigned char* pText = new unsigned char[hashLen];
	unsigned char* pTextBase = pText;
	int contentLength = unPackCHK((unsigned char *)hash, hashLen, pText);

	if (contentLength < 0)
		goto FAILEDC;

	int fileNumber = *(int*)pText;
	pText += 4;
	for(int i = 0; i < fileNumber; i++)
	{
		VerifyInfo vi;
		memcpy(vi.md5, pText, MD5_HASH_SIZE);
		pText += MD5_HASH_SIZE;

		m_moduleHash.insert(vi);
	}

	result = true;

FAILEDC:

	if(pTextBase)
		delete pTextBase;

	return result;

}


bool CBankModVerify::VerifyModule(CStringW filepath) const
{
	filepath.MakeLower();

	HANDLE hFile = CreateFileW(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD FileSize = GetFileSize(hFile, NULL);

	unsigned char* FileBuffer = new unsigned char[FileSize];
	DWORD read;
	if(ReadFile(hFile, FileBuffer, FileSize, &read, NULL) || read != FileSize)
	{
		CMD5 md5;
		md5.MD5Update(FileBuffer, FileSize);
		unsigned char FileHash[MD5_HASH_SIZE];
		md5.MD5Final(FileHash);

		delete FileBuffer;

		VerifyInfo vi;
		memcpy(vi.md5, FileHash, MD5_HASH_SIZE);

		ModuleHash::const_iterator it = m_moduleHash.find(vi);

		if(it != m_moduleHash.end())
		{
			CloseHandle(hFile);
			return true;
		}
	}

	CloseHandle(hFile);
	return false;
}

ULONG CBankModVerify::GetBankModuleNumber() const
{
	return m_moduleHash.size();
}

bool CBankModVerify::FillFileHashes(unsigned char* hashes, int maxlen) const
{
	int i = 0;
	for(ModuleHash::const_iterator it = m_moduleHash.begin(); it !=  m_moduleHash.end(); it++)
	{
		const VerifyInfo& vi = *it;
		memcpy(hashes + i * MD5_HASH_SIZE, vi.md5, MD5_HASH_SIZE);
		i++;

		if(i >= maxlen)
			return false;
	}

	return true;
}
