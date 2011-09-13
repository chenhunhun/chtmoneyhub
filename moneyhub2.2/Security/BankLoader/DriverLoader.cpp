#include "stdafx.h"
#include "windows.h"
#include "sfc.h"
#include "DriverLoader.h"
#include "atlstr.h"
#include "../common/DriverDefine.h"

#include "../../Include/LogPrintf.h"
#include "../../Include/showMess.h"
#include "../../Utils/getOSInfo/GetOSInfo.h"

bool CDriverLoader::m_showMessOnce = false ;
std::string   CDriverLoader::m_drivername = "  ";

CDriverLoader::CDriverLoader() : m_hDriver(NULL)
{
	InitialDriverName();

	char DriverPath[1024];
	DWORD size;
	std::string ExePath;
	size = GetModuleFileNameA(NULL, DriverPath, 1024);
	if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		char* pDriverPath = new char[size + 1];
		GetModuleFileNameA(NULL, pDriverPath, size);
		ExePath = pDriverPath;
		delete pDriverPath;
	}
	else
		ExePath = DriverPath;

	const char* dotPointer = ExePath.c_str();
	const char* tmp;
	while(tmp = strchr(dotPointer, '\\'))
		dotPointer = tmp + 1;

	m_DriverPath = std::string(ExePath.c_str(), dotPointer - ExePath.c_str());
	m_CurrentPath = m_DriverPath;
	m_DriverPath += m_drivername.c_str();
	m_DriverPath += ".sys";

	return;
}

BOOL CDriverLoader::InitializeDriver() const
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////
	LogPrintfA("InitializeDriver");

	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schSCManager) 
	{
		LogPrintfA("OpenSCManager failed: %d", GetLastError());
		return FALSE;
	}

	schService = OpenServiceA(schSCManager, m_drivername.c_str(), SERVICE_CHANGE_CONFIG);

	if (schService == NULL)
	{ 
		LogPrintfA("OpenService failed: %d", GetLastError());
		CloseServiceHandle(schSCManager);
		return FALSE;
	}    

	BOOL rCode;
	if (!ChangeServiceConfigA(schService, SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, 
		SERVICE_ERROR_NORMAL, m_DriverPath.c_str(), NULL, NULL, NULL, NULL, NULL, NULL))
	{
		rCode = FALSE;
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_INITIAL, MY_ERROR_DESCRIPT_INITIAL);
	}
	else
		rCode = TRUE;

	if(schService)
		CloseServiceHandle(schService); 
	if(schSCManager)
		CloseServiceHandle(schSCManager);
	return rCode;
}

BOOL CDriverLoader::CheckIsDriverInstalled() const 
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////

	LogPrintfA("CheckIsDriverInstalled");

	SC_HANDLE hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL)
	{
		LogPrintfA("OpenSCManager Failed: %d", GetLastError());
		return FALSE;
	}

	SC_HANDLE hSCService = OpenServiceA(hSCManager, m_drivername.c_str(), SERVICE_ALL_ACCESS);
	if (hSCService != NULL) 
	{
		CloseServiceHandle(hSCService);
		CloseServiceHandle(hSCManager);
		return TRUE;
	}
	else
		LogPrintfA("OpenService Failed: %d", GetLastError());

	return FALSE;
}

BOOL CDriverLoader::InstallDriver() const
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////

	LogPrintfA("InstallDriver");

	SC_HANDLE   schSCManager;
	schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(schSCManager == NULL)
	{
		LogPrintfA("OpenSCManager Failed: %d", GetLastError());
		return FALSE;
	}

	SC_HANDLE schService = CreateServiceA(schSCManager, m_drivername.c_str(), m_drivername.c_str(), SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, 
		/*SERVICE_AUTO_START*/SERVICE_SYSTEM_START, SERVICE_ERROR_NORMAL, m_DriverPath.c_str(), NULL, NULL, NULL, NULL, NULL);

	if (schService == NULL)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_INSTALL, MY_ERROR_DESCRIPT_INSTALL);

		if (schSCManager)
			CloseServiceHandle(schSCManager);

		DWORD err = GetLastError();
		if (err == ERROR_SERVICE_EXISTS) 
			return TRUE;
		else
			return FALSE;
	}

	if (schService)
		CloseServiceHandle(schService);

	if (schSCManager)
		CloseServiceHandle(schSCManager);

	return TRUE;
}

BOOL CDriverLoader::LoadDriver() const
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////

	LogPrintfA("LoadDriver");

	SC_HANDLE   schSCManager;
	schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(schSCManager == NULL)	
	{
		LogPrintfA("OpenSCManager Failed: %d", GetLastError());
		return FALSE;
	}

	SC_HANDLE   schService;
	DWORD       err;

	schService = OpenServiceA(schSCManager, m_drivername.c_str(), SERVICE_ALL_ACCESS);

	if (schService == NULL)
	{
		LogPrintfA("OpenService Failed: %d", GetLastError());

		if (schSCManager)
			CloseServiceHandle(schSCManager);

		return FALSE;
	}

	if (!StartServiceA(schService, 0, NULL))
	{
		err = GetLastError();
		if(err != ERROR_SERVICE_ALREADY_RUNNING)
		{
			//启动驱动失败，反馈
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_START, MY_ERROR_DESCRIPT_START);
		}

		if (schSCManager)
			CloseServiceHandle(schSCManager);

		if (err == ERROR_SERVICE_ALREADY_RUNNING)
			return TRUE;
		else 
			return FALSE;
	}

	if (schService)
		CloseServiceHandle(schService);
	if (schSCManager)
		CloseServiceHandle(schSCManager);
	return TRUE;
}

BOOL CDriverLoader::CheckIsDriverLoaded() const
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////

	LogPrintfA("CheckIsDriverLoaded");

	SC_HANDLE hSCManager = OpenSCManagerA(NULL, NULL, SERVICE_QUERY_STATUS);
	if (hSCManager == NULL)
		return FALSE;

	SC_HANDLE hSCService = OpenServiceA(hSCManager, m_drivername.c_str(), SERVICE_QUERY_STATUS);
	::CloseServiceHandle(hSCManager);
	if (hSCService == NULL)
		return FALSE;

	SERVICE_STATUS_PROCESS ssp;
	DWORD dwSizeNeeded;
	BOOL bRes = ::QueryServiceStatusEx(hSCService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwSizeNeeded);
	::CloseServiceHandle(hSCService);
	if (bRes == FALSE)
		return FALSE;

	return ssp.dwCurrentState == SERVICE_RUNNING;
}

BOOL CDriverLoader::UnloadDriver()
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////

	LogPrintfA("UnloadDriver");

	SC_HANDLE   schSCManager;
	schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(schSCManager == NULL)
	{
		LogPrintfA("OpenSCManager Failed: %d", GetLastError());
		return FALSE;
	}

	BOOLEAN         rCode = TRUE;
	SC_HANDLE       schService;
	SERVICE_STATUS  serviceStatus;

	schService = OpenServiceA(schSCManager, m_drivername.c_str(), SERVICE_ALL_ACCESS);

	if (schService == NULL)
	{
		LogPrintfA("OpenService Failed: %d", GetLastError());
		if (schSCManager)
			CloseServiceHandle(schSCManager);
		return FALSE;
	}

	DWORD dwReturned;
	if(::DeviceIoControl(GetDriver(true), IOCTL_SET_UP_UNLOAD, NULL, 0, NULL, 0, &dwReturned, NULL) == FALSE)
	{
		//卸载驱动失败，反馈

		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_UNINSTALL, MY_ERROR_DESCRIPT_UNINSTALL);

		return FALSE;
	}
//////////////////////////////////////////////////////////////////////////////////////modify by bh 8-5 16:22
	/*if (ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus))
		rCode = TRUE;
	else
	{
		LogPrintfA("ControlService Failed: %d", GetLastError());
		rCode = FALSE;
	}*/
	int laspetime=100;
	while(laspetime>0)
	{
		if(ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus))
		{
			rCode=TRUE;
			break;
		}
		laspetime--;
		Sleep(100);
	}
	if(laspetime<=0 )  
	{
		rCode=FALSE;
		LogPrintfA("wait for driver stopping is error : %d", GetLastError());	
	}
///////////////////////////////////////////////////////////////////////////////////////////
	if (schService)
		CloseServiceHandle (schService);
	if (schSCManager)
		CloseServiceHandle(schSCManager);
	return rCode;
}

BOOL CDriverLoader::RemoveDriver() const
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////

	LogPrintfA("RemoveDriver");

	SC_HANDLE   schSCManager;
	schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(schSCManager == NULL)
	{
		LogPrintfA("OpenSCManager Failed: %d", GetLastError());
		return FALSE;
	}

	SC_HANDLE   schService;
	BOOLEAN     rCode;

	schService = OpenServiceA(schSCManager, m_drivername.c_str(), SERVICE_ALL_ACCESS);

	if (schService == NULL)
	{
		LogPrintfA("OpenService Failed: %d", GetLastError());

		if (schSCManager)
			CloseServiceHandle(schSCManager);
		return FALSE;
	}

	if (DeleteService(schService))
		rCode = TRUE;
	else
	{
		LogPrintfA("DeleteService Failed: %d", GetLastError());
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_DELETE, MY_ERROR_DESCRIPT_DELETE);

		rCode = FALSE;
	}

	SERVICE_STATUS srvStatus;
	QueryServiceStatus(schService, &srvStatus);

	if (schService)
		CloseServiceHandle(schService);
	if (schSCManager)
		CloseServiceHandle(schSCManager);
	return rCode;
}

bool CDriverLoader::CheckDriver()//检测驱动状态，最多2分钟
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////

	int flag = 0;
	while( 1 )
	{
		std::string tmpstr = "\\\\.\\";
		tmpstr             += m_drivername.c_str();
		m_hDriver = CreateFileA( tmpstr.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		if(m_hDriver == NULL || (GetLastError() == 6))
		{			
			flag ++;
		}
		else
			return true;

		if(flag > 7)
			break;
		else
			Sleep(flag*4*1000);
	}
	return false;
}

HANDLE CDriverLoader::GetDriver(bool isUnload)
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return (HANDLE)1;
	///////////////////////

	std::string tmpstr = "\\\\.\\";
	tmpstr             += m_drivername.c_str();

	ReadFile(m_hDriver,0,0,NULL,NULL);
	if(m_hDriver == NULL || (GetLastError() == 6))
		m_hDriver = CreateFileA( tmpstr.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);

	ReadFile(m_hDriver,0,0,NULL,NULL);
	if((GetLastError() == 6) && (m_showMessOnce == false) && (isUnload == false))
	{
		m_showMessOnce = true;
		MessageBoxW(NULL,NONEDRIVEREERROR,L"财金汇",MB_OK | MB_ICONSTOP);
	
		exit(1);
	}
	return m_hDriver;
}

BOOL CDriverLoader::CloseDriver()
{
	/**
	取消64bits驱动保护
	*/
	if( this->isX64() )
		return TRUE;
	///////////////////////

	BOOL ret = CloseHandle(m_hDriver);
	m_hDriver = NULL;
	return ret;
}
/**
  static 
*/
BOOL isOsX64()   
{
	return CGetOSInfo::getInstance()->isX64()? TRUE : FALSE;
}

bool CDriverLoader::CheckDriverImagePath(bool bNotSilent)
{
	/**
	取消64bits驱动保护
	*/
	if( isOsX64() )
		return TRUE;
	///////////////////////

	InitialDriverName();
	bool bOK = false;

	char DriverPath[1024];
	GetModuleFileNameA(NULL, DriverPath, 1024);
	*(strrchr(DriverPath, '\\') + 1) = '\0';

	std::string strDriverPath = "\\??\\";
	strDriverPath += DriverPath;
	strDriverPath += m_drivername.c_str();
	strDriverPath += ".sys";

	DWORD dwType;
	char szImagePath[1024];
	DWORD dwReturnBytes = sizeof(szImagePath);
	std::string regDriverName = "SYSTEM\\CurrentControlSet\\Services\\";
	regDriverName             += m_drivername.c_str();
	if (ERROR_SUCCESS == ::SHGetValueA(HKEY_LOCAL_MACHINE, regDriverName.c_str(), 
		"ImagePath", &dwType, szImagePath, &dwReturnBytes)
		&& dwType == REG_SZ)
	{
		if (_stricmp(strDriverPath.c_str(), szImagePath) == 0)
			bOK = true;
	}

	if ((!bOK) && bNotSilent)
	{
		MessageBox(NULL, NONEDRIVEREERROR , _T("财金汇"), MB_OK | MB_ICONSTOP);
		exit(-1);
	}
	//防止被禁用
	if (ERROR_SUCCESS == ::SHGetValueA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\MoneyHubPrt", 
		"Start", &dwType, szImagePath, &dwReturnBytes)
		&& dwType == REG_DWORD)
	{
		if ( atoi(szImagePath) != 1)
		{
			int data = 1;
			SHSetValueA(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\MoneyHubPrt","Start",REG_DWORD,(LPVOID)&data,4);
			
		}
	}
	//

	return bOK;
}



void CDriverLoader::InitialDriverName()
{
	if( CGetOSInfo::getInstance()->isX64() )
		m_drivername = DRIVER_NAMEX64;
	else
		m_drivername = DRIVER_NAME;
}


/**
*   判断系统类型，同时兼容32bits & 64bits
*   2011-3-24 暂时取消64bits驱动保护
*/
BOOL CDriverLoader::isX64()   const
{
	return CGetOSInfo::getInstance()->isX64()? TRUE : FALSE;
}