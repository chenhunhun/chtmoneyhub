#include "stdafx.h"
#include "ResourceManager.h"
#include "Requirement.h"
#include "DriverData.h"
#include "../RecordProgram/RecordProgram.h"
#include "../FavBankOperator/FavBankOperator.h"

CDriverData::CDriverData(const CRequirement *pRequire, const CWebsiteData *pWebsiteData) : m_pRequire(pRequire), m_pWebsiteData(pWebsiteData) {}

// 设置驱动信息
void CDriverData::SetDriverInfo(LPCTSTR lpszServiceName,DWORD rstart,DWORD rtype)
{
	m_strServiceName = lpszServiceName;
	if (!CheckDriverIsWorking(rstart, rtype))
		InstallDriver(rstart,rtype);
}
// 检测驱动是否在运行
bool CDriverData::CheckDriverIsWorking(DWORD rstart,DWORD rtype) const
{
	SC_HANDLE hSCHandle = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	if (hSCHandle == NULL)
		return false;
	SC_HANDLE hService = ::OpenService(hSCHandle, m_strServiceName.c_str(), SERVICE_QUERY_STATUS);
	::CloseServiceHandle(hSCHandle);
	if (hService == NULL)
		return false;

	if(rstart >= 3)
	{
		//如果是自动运行的
		::CloseServiceHandle(hService);
		return true;
	}
	SERVICE_STATUS serviceStatus;
	if (::QueryServiceStatus(hService, &serviceStatus))
	{
		CRecordProgram::GetInstance ()->RecordCommonInfo(L"MoneyCore", serviceStatus.dwCurrentState, CRecordProgram::GetInstance ()->GetRecordInfo(L"驱动%s状态:%d", m_strServiceName.c_str(),serviceStatus.dwCurrentState));

		if (serviceStatus.dwCurrentState == SERVICE_RUNNING)
		{
			::CloseServiceHandle(hService);
			return true;
		}
		else
		{
			::StartService(hService, 0, NULL);
			::CloseServiceHandle(hService);
			return true;			
		}
	}
	::CloseServiceHandle(hService);
	return false;
}

// 安装网页中需要驱动的控件
bool CDriverData::InstallDriver(DWORD rstart,DWORD rtype) 
{
	SC_HANDLE hSCHandle = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	if (hSCHandle == NULL)
	{
		return false;
	}
	SC_HANDLE hService = ::OpenService(hSCHandle, m_strServiceName.c_str(), SERVICE_QUERY_STATUS | SERVICE_START);
	::CloseServiceHandle(hSCHandle);
	if (hService == NULL /*|| ::StartService(hService, 0, NULL)== FALSE*/)
	{
		// 没有service，创建一个
		SC_HANDLE hSCHandleNew = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
		if (hSCHandleNew == NULL)
		{
			DWORD re = ::GetLastError();
			if(re == ERROR_ACCESS_DENIED)
			{
				CRecordProgram::GetInstance ()->RecordCommonInfo(L"MoneyCore", 1003, L"安装驱动ERROR_ACCESS_DENIED");
				USES_CONVERSION;
				string appid;
				if(m_pWebsiteData)
				{
					USES_CONVERSION;
					appid = CFavBankOperator::GetBankIDOrBankName(W2A(m_pWebsiteData->GetID()),false);
					CWebsiteData::StartUAC(A2W(appid.c_str()));
				}
			}
			else
				CRecordProgram::GetInstance ()->RecordCommonInfo(L"MoneyCore", 1003, L"安装驱动错误");
			return false;
		}
		std::wstring strFilePath = _T("\\??\\") + CResourceManager::_()->GetFilePath(m_pWebsiteData->GetWebsiteType(), m_pWebsiteData->GetID(), m_pRequire->GetFileData()->GetOneFile());

		
		//hService = ::CreateService(hSCHandleNew, m_strServiceName.c_str(), m_strServiceName.c_str(), SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
		//	SERVICE_SYSTEM_START, SERVICE_ERROR_NORMAL, strFilePath.c_str(), NULL, NULL, NULL, NULL, NULL);
		hService = ::CreateService(hSCHandleNew, m_strServiceName.c_str(), m_strServiceName.c_str(), SERVICE_ALL_ACCESS, rtype,
			rstart, SERVICE_ERROR_NORMAL, strFilePath.c_str(), NULL, NULL, NULL, NULL, NULL);
		::CloseServiceHandle(hSCHandleNew);
		
		if (hService == NULL)
		{
			int error = GetLastError();
			CRecordProgram::GetInstance ()->FeedbackError(L"MoneyCore", 1004, CRecordProgram::GetInstance ()->GetRecordInfo(L"创建驱动%s失败:%d", m_strServiceName.c_str(), error));
			return false;
		}

		// 创建成功，调整驱动的目录，防止系统的事件错误
		std::wstring strkey = L"SYSTEM\\CurrentControlSet\\Services\\" + m_strServiceName;
		DWORD dwLength = (strFilePath.size() + 1) * sizeof(TCHAR);
		::SHSetValue(HKEY_LOCAL_MACHINE,strkey.c_str() , _T("ImagePath"), REG_EXPAND_SZ, strFilePath.c_str(), dwLength);

		if(rstart < 3)
    		::StartService(hService, 0, NULL);
		::CloseServiceHandle(hService);
	}
	else
	{
		if(rstart < 3)
			::StartService(hService, 0, NULL);
		::CloseServiceHandle(hService);
	}
	return true;
}