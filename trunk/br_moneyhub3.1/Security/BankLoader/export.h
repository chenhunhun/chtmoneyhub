#pragma once

#include "atlstr.h"
#include "windows.h"

#include "..\.\common\DriverDefine.h"

namespace BankLoader
{
	bool LoadProcess(const CString& cmdline, HANDLE& hProcess, DWORD& PID);
	bool IsFinished();

	bool InstallAndStartDriver();
	bool UnInstallDriver();

	//add by bh 8 9
	bool setSecuModuleBR();//发送白名单

	bool SendBlackListToDriver();//发送黑名单

	int returnDriverStatus(MYDRIVERSTATUS type);

	bool  checkHook();

	bool SendReferenceEvent(HANDLE& ev);//ev为输入参数，为驱动通知应用程序有灰名单文件

	bool GetGrayFile(void *pFileData, DWORD size);

	bool CheckDriver();//单独为pop定义的检测驱动状态的函数

	bool SendProtectId(UINT32 id);
	bool SetKernelContinue(bool b);
};