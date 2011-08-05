#include "stdafx.h"
#include "ChkSnManager.h"

#include "windows.h"
#include "Shlwapi.h"
#pragma comment(lib,"Shlwapi.lib")
char border[] = "<BORDER>";

CChkSnManager _ChkSnManager;
CChkSnManager::CChkSnManager()
{
	bKInit = false;
	bSInit = false;
}
void CChkSnManager::GetKey()
{
	if(bKInit)
		return;
	WCHAR swPath[MAX_PATH] = {0};
	::GetModuleFileNameW(NULL, swPath, MAX_PATH);

	::PathRemoveFileSpecW(swPath);
	::PathRemoveFileSpecW(swPath);

	wstring str(swPath);
	str += L"\\data.dz";//ÃÜÔ¿Ãû³Æ¹Ì¶¨Îªdata.dz
	
	HANDLE hFile = CreateFileW(str.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		return;

	DWORD dwLength = GetFileSize(hFile, NULL);
	if(dwLength <= 0)
	{
		CloseHandle(hFile);
		return;
	}
	unsigned char* lpBuffer = new unsigned char[dwLength + 1];

	if (lpBuffer == NULL)
	{
		CloseHandle(hFile);
		return;
	}

	DWORD dwRead = 0;
	if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
	{
		delete []lpBuffer;
		CloseHandle(hFile);
		return;
	}
	CloseHandle(hFile);

	int  lenBorder= strlen(border);
	int bitLen;

	unsigned char *pdata = lpBuffer;
	memcpy(&bitLen, pdata, sizeof(bitLen));
	pdata += sizeof(bitLen);

	memcpy(&snstruct.n, pdata, sizeof(snstruct.n));
	pdata += sizeof(snstruct.n) + lenBorder;

	memcpy(&snstruct.e, pdata, sizeof(snstruct.e));
	pdata += sizeof(snstruct.e) + lenBorder;

	memcpy(&snstruct.d, pdata, sizeof(snstruct.d));
	pdata += sizeof(snstruct.d) + lenBorder;

	memcpy(&snstruct.p, pdata, sizeof(snstruct.p));
	pdata += sizeof(snstruct.p) + lenBorder;

	memcpy(&snstruct.q, pdata, sizeof(snstruct.q));
	pdata += sizeof(snstruct.q) + lenBorder;

	memcpy(&snstruct.kdp, pdata, sizeof(snstruct.kdp));
	pdata += sizeof(snstruct.kdp) + lenBorder;

	memcpy(&snstruct.kdq, pdata, sizeof(snstruct.kdq));
	pdata += sizeof(snstruct.kdq) + lenBorder;

	memcpy(&snstruct.A, pdata, sizeof(snstruct.A));

	delete []lpBuffer;
	bKInit = true;
}
