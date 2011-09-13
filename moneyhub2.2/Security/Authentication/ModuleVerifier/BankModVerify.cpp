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

/*
CBankModVerify::CBankModVerify(void)
{
	ReadCHK();
}

CBankModVerify::~CBankModVerify(void)
{
}

bool CBankModVerify::ReadCHK()
{
	bool result = false;
	unsigned char* pText = CHKReader::GetCHKBuffer("authen.chk");
	if(pText == NULL)
		goto FAILED;

	//////////////////////////////////////////////////////////////////////////
	// Get current directory
	TCHAR szModName[MAX_PATH];
	TCHAR* pExeDir = szModName;

	DWORD size = GetModuleFileNameEx(GetCurrentProcess(), NULL, szModName, MAX_PATH);
	if(size > MAX_PATH)
		goto FAILED;

	TCHAR* pLastSlash = _tcsrchr(szModName, _T('\\'));
	if(pLastSlash == NULL)
		goto FAILED;

	*(pLastSlash + 1) = 0;

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
}*/