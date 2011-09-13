#include "stdafx.h"
#include "USBSoftWareDetector.h"
#include "Tlhelp32.h"
#include <algorithm>

#pragma once

CUSBSoftWareDetector* CUSBSoftWareDetector::m_Instance = NULL;

CUSBSoftWareDetector* CUSBSoftWareDetector::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CUSBSoftWareDetector();
	return m_Instance;
}

CUSBSoftWareDetector::CUSBSoftWareDetector()
{
}
CUSBSoftWareDetector::~CUSBSoftWareDetector()
{
}

bool CUSBSoftWareDetector::CheckFile(list<wstring>& fname)							// 检测文件是否存在作为标准
{
	list<wstring>::iterator ite = fname.begin();
	for(;ite != fname.end(); ite ++)
	{
		if(::PathFileExistsW(ite->c_str()) == FALSE)
			return false;
	}
	return true;

}
bool CUSBSoftWareDetector::CheckRegInfo(list<RegInfo>& reginfo)		// 检测注册表是否存在
{

	list<RegInfo>::iterator ite = reginfo.begin();
	for(;ite != reginfo.end(); ite ++)
	{
		HKEY hKey = NULL;
		if (ERROR_SUCCESS != ::RegOpenKeyExW(ite->rootkey,ite->subkey.c_str(),NULL, KEY_QUERY_VALUE,&hKey))
		{
			return false;
		}
		if(hKey)
			::RegCloseKey(hKey);
	}
	return true;
}

bool CUSBSoftWareDetector::CheckUSBSoftWare(USBSoftwareInfo& softinfo, bool bNeedRepair, bool bNeedRestart)//检测并修复software的运行情况
{
	if(!bNeedRepair)//不需要修复
	{
		if(CheckFile(softinfo.files) && CheckKeyFile(softinfo.programinfo, bNeedRepair) && CheckRegInfo(softinfo.reginfo))
			return true;
		return false;	
	}
	else
		CheckKeyFile(softinfo.programinfo, bNeedRepair,bNeedRestart);
	return true;
}

bool CUSBSoftWareDetector::CheckKeyFile(std::map<std::wstring,ProgramType>& programinfo, bool bNeedRepair, bool bNeedRestart)
{
	std::map<wstring, ProgramType>::iterator ite = programinfo.begin();
	for(; ite != programinfo.end() ; ite ++)
	{
		int ret = true;
		if((*ite).second == pNormal && bNeedRepair)
			ret = CheckProgramIsRunning((*ite).first , bNeedRestart);
		else if((*ite).second == pDriver)
			ret = CheckIsDriverInstalled((*ite).first);
		else if((*ite).second == pService)
			ret = CheckServiceIsWork((*ite).first, bNeedRepair);

		if(ret == false)
			return false;
	}
	return true;	
}
bool CUSBSoftWareDetector::CheckIsDriverInstalled(const wstring& dname)
{
	SC_HANDLE hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		int error = GetLastError();
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckDriver OpenSCManager 失败:%d", error));
		return false;
	}

	SC_HANDLE hSCService = OpenServiceW(hSCManager, dname.c_str(), SERVICE_ALL_ACCESS);
	if (hSCService != NULL) 
	{
		int error = GetLastError();
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckDriver OpenService %s失败::%d",dname.c_str(), error));
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		return true;
	}

	CloseServiceHandle(hSCManager);
	return false;
}

bool CUSBSoftWareDetector::CheckServiceIsWork(const wstring& sname, bool bNeedRepair)
{
	SC_HANDLE hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		int error = GetLastError();
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckService OpenSCManager 失败:%d", error));
		return false;
	}

	SC_HANDLE hSCService = OpenServiceW(hSCManager, sname.c_str(), SERVICE_ALL_ACCESS);
	if (hSCService == NULL) 
	{
		int error = GetLastError();
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckService OpenService %s 失败:%d", sname.c_str(), error));

		CloseServiceHandle(hSCManager);
		return false;
	}
	if(!bNeedRepair)//不需要修复，直接返回正确
	{
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		return true;
	}


	// 查询服务状态
	bool scret = false;
	SERVICE_STATUS ss;
	memset(&ss, 0, sizeof(ss));
	BOOL bret = QueryServiceStatus(hSCService,	&ss);
	if(bret)
	{
		DWORD dwErr = ss.dwWin32ExitCode;
		DWORD dwState = ss.dwCurrentState;

		if (dwState == SERVICE_RUNNING)
		{                 
			// 服务在运行
			scret = true;
		}
		else
		{
			LPQUERY_SERVICE_CONFIG ServicesInfo = NULL;
			ServicesInfo = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 64*1024);//申请足够的内存

			DWORD nRequire=0;

			if(!::QueryServiceConfigW(hSCService,ServicesInfo, 64*1024,&nRequire))
			{
				int error = GetLastError();
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckService QueryServiceConfig  %s 失败:%d", sname.c_str(), error));
				LocalFree(ServicesInfo);
				CloseServiceHandle(hSCService);
				CloseServiceHandle(hSCManager);
				return scret;
			}
			// 如果该服务被禁用，那么改为手动
			if(ServicesInfo->dwStartType == SERVICE_DISABLED)
			{
				SC_LOCK sclLock;    
				sclLock = LockServiceDatabase(hSCService);
				if (! ChangeServiceConfig(hSCService, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, NULL,    
					NULL, NULL, NULL, NULL, NULL, NULL) )                // display name: no change   
				{
					int error = GetLastError();
					CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckService ChangeServiceConfig  %s 失败:%d", sname.c_str(), error));
					UnlockServiceDatabase(sclLock);
					LocalFree(ServicesInfo);
					CloseServiceHandle(hSCService);
					CloseServiceHandle(hSCManager);
					return scret;
				}
				UnlockServiceDatabase(sclLock);
			}
			LocalFree(ServicesInfo);   

			// 重启该服务
			bret = StartService(hSCService, 0, NULL);
			if(bret) 
			{
				scret = true;
			}
			else
			{
				int error = GetLastError();
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckService StartService  %s 失败:%d", sname.c_str(), error));
			}
		}
	}

	CloseServiceHandle(hSCService);
	CloseServiceHandle(hSCManager);
	return scret;	
}

bool CUSBSoftWareDetector::CheckProgramIsRunning(const wstring& pname, bool bNeedRestart)//全大写的程序文件名称
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"Begin Repair Program");
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, pname.c_str());
	if(pname.c_str() == L"")
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"CheckProgramIsRunning NULL");
		return true;
	}
	GetPriviledge();
	
	size_t tsize = pname.find_last_of('\\');
	if(tsize == std::wstring::npos)
		return true;

	wstring program = pname.substr(tsize + 1, pname.size() - tsize - 1);//在全路径名称中截取最后一段的程序名称

	HANDLE hProcessSnap = NULL;
	BOOL   bRet         = FALSE;
	PROCESSENTRY32 pe32 = {0};

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		int error = GetLastError();
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckProgramIsRunning CreateToolhelp32Snapshot失败:%d",error));
		return FALSE;
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);


	if(Process32First(hProcessSnap,&pe32))
	{
		do
		{
			wstring exename(pe32.szExeFile);
			transform(exename.begin(), exename.end(), exename.begin(), towupper);
			if(exename == program)
			{
				if(!bNeedRestart)
				{
					CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"exename :%s;program:%s", exename.c_str(),program.c_str()));
					CloseHandle(hProcessSnap);
					return true;
				}
				else
				{
					HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,false,pe32.th32ProcessID);
					if(hProcess)
					{
						CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"Terminate;program:%s", exename.c_str()));
						::TerminateProcess(hProcess,0);
						::CloseHandle(hProcess);
					}
				}
				break;
			}
		}while(Process32Next(hProcessSnap,&pe32));


		// 是否应该启动该进程
		STARTUPINFO si;	
		PROCESS_INFORMATION pi;	
		ZeroMemory( &pi, sizeof(pi) );	
		ZeroMemory( &si, sizeof(si) );	
		si.cb = sizeof(si);	
		// 打开进程	
		if(CreateProcessW(NULL, (LPWSTR)pname.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{	
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"修复进程成功");
			CloseHandle(pi.hProcess);		
			CloseHandle(pi.hThread);	
			return true;
		}
		else
		{
			DWORD error = GetLastError();
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"修复进程%s失败:%d", pname.c_str(), error));
		}
		return false;
	}

	CloseHandle(hProcessSnap);
	return false;

}


bool CUSBSoftWareDetector::GetPriviledge()
{
	HANDLE hProcessToken = NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hProcessToken)) 
	{ 
		return false; 
	}

	TOKEN_PRIVILEGES tp={0};
	LUID luid={0};  
	if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid))  
	{ 
		return false; 
	}  
	tp.PrivilegeCount = 1;  
	tp.Privileges[0].Luid = luid;

	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  

	// Enable the privilege
	AdjustTokenPrivileges(hProcessToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);  

	if(GetLastError() != ERROR_SUCCESS)  
	{
		return false;  
	}
	return true;

}


