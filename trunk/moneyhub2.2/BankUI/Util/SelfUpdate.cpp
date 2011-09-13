
#include "stdafx.h"
#include "SelfUpdate.h"
#include "../UIControl/CoolMessageBox.h"

#define _UPDATE_WND_CAPTION _T("财金汇在线升级");
UINT WM_SHOWUPDATEMAINWND = RegisterWindowMessage(_T("BankUpdateShowMainWnd"));

//////////////////////////////////////////////////////////////////////////
// simple registry functions
DWORD GetRegValue(LPCTSTR lpszItem)
{
	DWORD dwType;
	DWORD dwReturnBytes = sizeof(DWORD);
	DWORD dwUpdate = 0;

	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), lpszItem, &dwType, &dwUpdate, &dwReturnBytes))
		return 0;

	return dwUpdate;
}

void SetRegValue(LPTSTR lpszItem, DWORD dwVal)
{
	ATLASSERT(lpszItem);
	if (ERROR_SUCCESS != ::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), lpszItem, REG_DWORD, &dwVal, sizeof(DWORD)))
		lpszItem[0] = '\0';
}

//////////////////////////////////////////////////////////////////////////

CCheckForUpdate* _pCheckForUpdate = NULL;
int CCheckForUpdate::m_nAddRef = 0;

CCheckForUpdate* CCheckForUpdate::CreateInstance(HWND hParent)
{
	if (_pCheckForUpdate == NULL)
		_pCheckForUpdate = new CCheckForUpdate(hParent);
	
	_pCheckForUpdate->AddRef();

	return _pCheckForUpdate;
}

int CCheckForUpdate::AddRef()
{
	return ++m_nAddRef;
}

void CCheckForUpdate::Release()
{
	if (--m_nAddRef == 0)
	{
		delete _pCheckForUpdate;
		_pCheckForUpdate = NULL;
	}
}

CCheckForUpdate::CCheckForUpdate(HWND hParent)
: m_hParentWnd(hParent)
{
}

bool CCheckForUpdate::IsChecking(bool bAuto)
{
	HWND hWnd = FindWindow(NULL, _T("财金汇在线升级"));
	if (!bAuto && (hWnd != NULL))
	{
		ShowWindow(hWnd, SW_SHOW);
		if (IsIconic(hWnd))
			ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
	}

	return hWnd != NULL;
}

bool CCheckForUpdate::Check(bool bAuto)
{
	if (IsChecking(bAuto))
		return false;

	if (GetRegValue(_T("InstallFlag")))
	{
		if (!bAuto)
			mhMessageBox(m_hParentWnd, 
			_T("最新版本安装包已下载，请关闭财金汇重新运行"),
			_T("在线升级"),
			MB_OK | MB_ICONEXCLAMATION);

		return false;
	}

	TCHAR szFile[MAX_PATH];
	::GetModuleFileName(NULL, szFile, _countof(szFile));
	TCHAR* pSlash = _tcsrchr(szFile, '\\');
	*(pSlash + 1) = 0;
	_tcscat_s(szFile, _T("Moneyhub_Updater.exe"));

	TCHAR szCmdLine[2048];
	if (bAuto)
		_stprintf_s(szCmdLine, _T("\"%s\" /h /p %d"), szFile, m_hParentWnd);
	else
		_stprintf_s(szCmdLine, _T("\"%s\" /p %d"), szFile, m_hParentWnd);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	BOOL bRet = ::CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return !!bRet;
}

void CCheckForUpdate::ClearInstallFlag()
{
	SetRegValue(_T("InstallFlag"), 0);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CInstallUpdatePack::CInstallUpdatePack()
: m_bNeedRun(false)
{
}

bool CInstallUpdatePack::Check()
{
	m_bNeedRun = GetRegValue(_T("InstallFlag")) != 0;
	if (!m_bNeedRun)
		return false;

	//tstring 
	DWORD dwType;
	TCHAR szCmdline[1024];
	DWORD dwReturnBytes = sizeof(szCmdline);

	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), 
						_T("InstallPack"), &dwType, szCmdline, &dwReturnBytes)
						|| dwType != REG_SZ)
		return false;

	m_strCmdline = szCmdline;
	//SetRegValue(_T("InstallFlag"), 0);	

	//非主程序升级
	if(0 == _tcscmp(szCmdline,_T("modules") ) )
		return true;

// 	wchar_t * sp = wcsrchr(szCmdline,'/');
// 	if(!sp)         
// 		return false;
// 	*(szCmdline + (sp - szCmdline) -1) = L'\0';
	CString strExe = szCmdline;
	strExe.Trim('"');

	if (_taccess_s(strExe, 0) != 0)
		return false;

	return true;
}

void CInstallUpdatePack::Setup()
{
	TCHAR szFile[MAX_PATH];
	::GetModuleFileName(NULL, szFile, _countof(szFile));
	TCHAR* pSlash = _tcsrchr(szFile, '\\');
	*(pSlash + 1) = 0;
	_tcscat_s(szFile, _T("Moneyhub_Updater.exe"));

	TCHAR szCmdLine[2048];
	_stprintf_s(szCmdLine, _T("\"%s\" /i /h"), szFile);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	::CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}


bool CInstallUpdatePack::setupUpdateWithMH()
{
	BOOL bAutoRun = FALSE;

	DWORD dwType;
	DWORD dwValue;
	DWORD dwReturnBytes = sizeof(DWORD);

	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("AutoRun"), &dwType, &dwValue, &dwReturnBytes))
	{
		bAutoRun = TRUE;
	}
	else if (dwValue == 1)
	{
		bAutoRun = TRUE;
	}

	if(bAutoRun == FALSE)
	{
		WCHAR szPath[MAX_PATH] ={0};
		::GetModuleFileName(NULL, szPath, _countof(szPath));
		::PathRemoveFileSpecW(szPath);

		wstring path(szPath);
		path += L"\\Moneyhub_Updater.exe /h";

		STARTUPINFO si;	
		PROCESS_INFORMATION pi;	
		ZeroMemory( &pi, sizeof(pi) );	
		ZeroMemory( &si, sizeof(si) );	
		si.cb = sizeof(si);	
		//带参数打开	
		if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{	
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"UI升级");
			::CloseHandle( pi.hProcess );		
			::CloseHandle( pi.hThread );		
		}
		else
		{
			int error = GetLastError();
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_COMMON_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"UI升级失败:%d", error));
		}
	}
	return true;

}