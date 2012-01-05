
#include "stdafx.h"
#include "../stdafx.h"
#include "CleanHistory.h"
#include <windows.h>
#include <string>
#include "..\..\Security\BankProtector\HistoryManagerXP.h"
#include "..\..\Security\BankProtector\HistoryManagerNormal.h"
#include "..\..\Security\BankProtector\export.h"

void CleanHistory()
{
#ifndef SINGLE_PROCESS
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	TCHAR szCmdLine[2048];
	TCHAR szFile[MAX_PATH];
	::GetModuleFileName(NULL, szFile, _countof(szFile));
	_stprintf_s(szCmdLine, _T("\"%s\" -clean"), szFile);

	if(::CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"完成CleanHistory清理");
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		int error = ::GetLastError();
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_COMMON_PROCESS, CRecordProgram::GetInstance()->GetRecordInfo(L"CleanHistory:CreateProcess失败:%d",error));
	}


	// 清除temp文件夹下的所有htm文件，主要为了清除打印记录
	WCHAR expName[MAX_PATH] ={0};
	ExpandEnvironmentStringsW(L"%temp%", expName, MAX_PATH);
	std::wstring infolder(expName);
	std::wstring findname;
	findname =infolder + L"\\*.htm" ;
	// file
	WIN32_FIND_DATAW fd;
	memset(&fd, 0, sizeof(WIN32_FIND_DATAW));
	HANDLE hFind = FindFirstFileW(findname.c_str(), &fd);
	if(INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			std::wstring subname = fd.cFileName;
			if (subname != L"." && subname != L"..")
			{
				std::wstring fname = infolder +L"\\" + subname;
				DeleteFileW(fname.c_str());
			}
		} while (FindNextFileW(hFind, &fd) != 0);
	}

	FindClose(hFind);

#endif
}

void CleanHistoryMain(BOOL bLoop)
{
#ifndef SINGLE_PROCESS
	int cnt = 5; // try 5 times
	while (cnt-- > 0)
	{
		//BOOL bRet = CHistoryManagerXP::GetInstance()->CleanHistory();
		bool bRet = BankProtector::CleanHistory();

		if (!bRet && bLoop)
			Sleep(200);
		else
			break;
	}
#endif
}
