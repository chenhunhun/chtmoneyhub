#include "stdafx.h"
#include "BankCube.h"
#include <string>

HRESULT CBankCube::SendData(BSTR strURL, BSTR strData)
{
	TCHAR szModuleFileName[MAX_PATH];
	::GetModuleFileName(_pModule->GetModuleInstance(), szModuleFileName, _countof(szModuleFileName));
	TCHAR *p = _tcsrchr(szModuleFileName, '\\');
	if (p)
		_tcscpy_s(p, 20, _T("\\MoneyHub.exe"));

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	TCHAR szCmdLine[2048];
	_stprintf_s(szCmdLine, _T("\"%s\" %s"), szModuleFileName, strURL);

	::CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	return S_OK;
}
