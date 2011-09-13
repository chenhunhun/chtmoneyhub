#pragma once

#include "DriverLoader.h"
#include "../common/DriverDefine.h"

class CDriverManager
{
	CDriverLoader m_DriverManager;


public:
	CDriverManager(void);
	~CDriverManager(void);

	bool InstallDriver();

	bool StartDriver();
	bool StopDriver();
	BOOL CheckIsDriverLoaded() const;

	bool RemoveDriver();

	bool SetFilterProcessID(UINT32 PID);
	bool GetFilterProcessIDs(PPID_INFO PIDs, UINT32 size);

	bool SetSecuMD5(void* secuMD5, ULONG length,bool type = false);// false为白名单，true为黑名单

	bool Safeguard();
	bool CheckProcessId();

	bool checkHookSafe();
	int  returnDriverStatus(MYDRIVERSTATUS type);

	bool SetHandle(HANDLE& ev);
	bool GetGrayFile(void *pFileData,DWORD size);
	bool CheckDriver();
private:
	BOOL isX64(UINT32 index, LPVOID par);
	
	bool SetSecuMD5X64(void* secuMD5, ULONG length,bool type = false);// false为白名单，true为黑名单
	int  returnDriverStatusX64(MYDRIVERSTATUS type);
	bool SetHandleX64(HANDLE& ev);
	bool GetGrayFileX64(void *pFileData,DWORD size);

};
