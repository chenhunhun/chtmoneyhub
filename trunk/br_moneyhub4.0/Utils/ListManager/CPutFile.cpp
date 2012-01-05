#include "stdafx.h"
#include "CPutFile.h"
#include "Windows.h"
#include "ResourceManager.h"
#include "../RecordProgram/RecordProgram.h"
#include "../getOSInfo/GetOSInfo.h"
#pragma comment(lib,"Kernel32.lib") 
CPutFile::CPutFile(const CWebsiteData *pWebsiteData):m_pWebsiteData(pWebsiteData)
{
}


//void CPutFile::AddFile(wstring name, wstring path)
//{
//	
//}
//replace为强制替换标记，如果规定目录下的文件
typedef BOOL (WINAPI *Wow64DisableWow64FsRedirectionFun) (__out PVOID *OldValue);
typedef BOOL (WINAPI *Wow64RevertWow64FsRedirectionFun) (__in PVOID OlValue);

void CPutFile::CheckFile(wstring name, wstring path, bool replace)
{		
	HMODULE hKernel32 = ::LoadLibrary(_T("Kernel32.dll"));
	PVOID OldValue;
	BOOL bRet = FALSE;
	if(CGetOSInfo::getInstance()->isX64())
	{
		Wow64DisableWow64FsRedirectionFun pWow64DisableWow64FsRedirection = NULL;
		if (hKernel32)
		{
			pWow64DisableWow64FsRedirection = (Wow64DisableWow64FsRedirectionFun)::GetProcAddress(hKernel32, "Wow64DisableWow64FsRedirection");
		}
		if(pWow64DisableWow64FsRedirection != NULL)
			bRet = pWow64DisableWow64FsRedirection(&OldValue);
	}

	wstring fullPath;
	fullPath = path + L"\\" + name;

	WCHAR expName[MAX_PATH] ={0};
	ExpandEnvironmentStringsW(fullPath.c_str(), expName, MAX_PATH);

	if(::PathFileExistsW(expName) == FALSE)// 判断文件是否存在
	{
		CRecordProgram::GetInstance ()->RecordCommonInfo(L"PutFile", 1001, CRecordProgram::GetInstance ()->GetRecordInfo(L"%s文件不存在", expName));
		wstring file = CResourceManager::_()->GetFilePath(m_pWebsiteData->GetWebsiteType(), m_pWebsiteData->GetID(), name.c_str());

		::CopyFileW(file.c_str(), expName , TRUE);
		
		DWORD re = ::GetLastError();
		CRecordProgram::GetInstance ()->RecordCommonInfo(L"PutFile", 1001, CRecordProgram::GetInstance ()->GetRecordInfo(L"替换%s文件结果:%d", file.c_str(),re));
		if(re == ERROR_ACCESS_DENIED)
		{
			CRecordProgram::GetInstance ()->RecordCommonInfo(L"PutFile", 1001, CRecordProgram::GetInstance ()->GetRecordInfo(L"%s文件放入失败", file.c_str()));
			USES_CONVERSION;
			string appid;
			if(m_pWebsiteData)
			{
				USES_CONVERSION;
				appid = CFavBankOperator::GetBankIDOrBankName(W2A(m_pWebsiteData->GetID()),false);
				CWebsiteData::StartUAC(A2W(appid.c_str()));
			}
		}
	}
	else
	{
		if(replace == true)//强制替换
		{
			DWORD oLength = 0, nLength = 0;
			HANDLE hFile = CreateFileW(expName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				oLength = GetFileSize(hFile, NULL);
				CloseHandle(hFile);
			}		
			
			CRecordProgram::GetInstance ()->RecordCommonInfo(L"PutFile", 1001, CRecordProgram::GetInstance ()->GetRecordInfo(L"%s文件已经存在", expName));
			wstring file = CResourceManager::_()->GetFilePath(m_pWebsiteData->GetWebsiteType(), m_pWebsiteData->GetID(), name.c_str());

			hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				nLength = GetFileSize(hFile, NULL);
				CloseHandle(hFile);
			}
			
			if((oLength != 0) && (oLength != nLength))
			{
				::CopyFileW(file.c_str(), expName , FALSE);//

				DWORD re = ::GetLastError();
				CRecordProgram::GetInstance ()->RecordCommonInfo(L"PutFile", 1001, CRecordProgram::GetInstance ()->GetRecordInfo(L"替换%s文件结果:%d", file.c_str(),re));
				if(re == ERROR_ACCESS_DENIED)
				{
					USES_CONVERSION;
					string appid;
					if(m_pWebsiteData)
					{
						appid = CFavBankOperator::GetBankIDOrBankName(W2A(m_pWebsiteData->GetID()),false);
						CWebsiteData::StartUAC(A2W(appid.c_str()));
					}
				}
			}
		}
	}
	if(CGetOSInfo::getInstance()->isX64())
		if(bRet == TRUE)
		{
			Wow64RevertWow64FsRedirectionFun pWow64RevertWow64FsRedirection = NULL;
			if (hKernel32)
			{
				pWow64RevertWow64FsRedirection = (Wow64RevertWow64FsRedirectionFun)::GetProcAddress(hKernel32, "Wow64RevertWow64FsRedirection");
			}
			if(pWow64RevertWow64FsRedirection != NULL)
				pWow64RevertWow64FsRedirection(OldValue);
		}
	return;
}
void CPutFile::CheckExe(wstring installname, wstring name, int type)
{
	if(type == 2)//file
	{
		WCHAR path[MAX_PATH] = {0};
		ExpandEnvironmentStringsW(installname.c_str(), path, MAX_PATH);

		if(::PathFileExistsW(path) == TRUE)
			return;
	}
	else if(type == 1)//reg
	{
		HKEY rootkey;
		size_t nEndKeyName = installname.find_first_of(']', 1);
		std::wstring strKeyName = installname.substr(1, nEndKeyName - 1);

		size_t nEnd = strKeyName.find_first_of('\\');
		std::wstring strRootKey = strKeyName.substr(0, nEnd);

		if(strRootKey == L"HKEY_CURRENT_USER")
			rootkey = HKEY_CURRENT_USER;
		else if (strRootKey == _T("HKEY_LOCAL_MACHINE"))
			rootkey = HKEY_LOCAL_MACHINE;
		else if(strRootKey == _T("HKEY_CLASSES_ROOT"))
			rootkey = HKEY_CLASSES_ROOT;

		std::wstring strKey = strKeyName.substr(nEnd + 1, wstring::npos);

		HKEY hKey = NULL;                               // 操作键句柄

		if (ERROR_SUCCESS == ::RegOpenKeyExW(rootkey, strKey.c_str(), 0, KEY_READ, &hKey))   
		{   
			::RegCloseKey(hKey);
			return;
		} 
	}
	else
		return;

	wstring file = CResourceManager::_()->GetFilePath(m_pWebsiteData->GetWebsiteType(), m_pWebsiteData->GetID(), name.c_str());

	OSVERSIONINFO os = { sizeof(OSVERSIONINFO) };
	::GetVersionEx(&os);
	if(os.dwMajorVersion >= 6)
	{
		SHELLEXECUTEINFOW shExecInfo;
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;//SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI; 
		shExecInfo.hwnd = NULL;
		shExecInfo.lpVerb = L"runas";
		shExecInfo.lpFile = LPWSTR(file.c_str());
		shExecInfo.lpParameters = NULL;
		shExecInfo.lpDirectory = NULL;
		shExecInfo.nShow = SW_SHOWNORMAL;
		shExecInfo.hInstApp = NULL;

		if (!ShellExecuteExW(&shExecInfo))
		{			
			int err = GetLastError();
			CRecordProgram::GetInstance()->FeedbackError(L"PutFile", err,
				CRecordProgram::GetInstance()->GetRecordInfo(L"CPutFile创建安装进程%s失败！", file.c_str()));
		}
		else
			WaitForSingleObject (shExecInfo.hProcess, INFINITE); 
	}
	else
	{
		STARTUPINFOW si;
		memset (&si, 0, sizeof (STARTUPINFOW));
		si.wShowWindow = SW_HIDE;
		si.cb = sizeof (STARTUPINFOW);
		PROCESS_INFORMATION pi;
		memset (&pi, 0, sizeof (PROCESS_INFORMATION));	

		if (!CreateProcess(NULL, LPWSTR(file.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			CRecordProgram::GetInstance()->FeedbackError(L"PutFile", 1200,
				CRecordProgram::GetInstance()->GetRecordInfo(L"CPutFile创建安装进程%s失败！", file.c_str()));
			return;
		}
		else
			WaitForSingleObject (pi.hProcess, INFINITE); 
		CloseHandle (pi.hThread);
		CloseHandle (pi.hProcess);
	}
	return;
}
