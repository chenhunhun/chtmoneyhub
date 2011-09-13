#include "StdAfx.h"
#include <Shlwapi.h>
#include "ServiceControl.h"

#pragma comment(lib,"shlwapi")

//moneyhub.exe句柄
extern HANDLE g_terminateHandle;
//停止服务标示
bool          g_isStopService=false;

//设置服务名称以及服务描述
CServiceControl::CServiceControl(LPCTSTR lpServiceName,LPCTSTR lpServieDescription)
{
	_tcscpy_s(m_tzServiceName,MAX_PATH,lpServiceName);
	_tcscpy_s(m_tzServieDescription,MAX_PATH,lpServieDescription);
	memset(m_tzLogPathName,0,sizeof(m_tzLogPathName));
	//设置日志路径
	//SetLogFileName(NULL);

}

CServiceControl::~CServiceControl(void)
{
}

// 判断是否安装了服务
bool CServiceControl::IsInstall(void)
{
	SC_HANDLE	hSCM = NULL;
	SC_HANDLE	hService = NULL;

	hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return false;
	}

	hService = OpenService(hSCM,m_tzServiceName,SERVICE_QUERY_CONFIG);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		return false;
	}


	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return true;
}

// 安装服务
bool CServiceControl::Install(void)
{
	SC_HANDLE	hSCM = NULL;
	SC_HANDLE	hService = NULL;
	TCHAR		tzFilePathName[MAX_PATH] = {0};

	if (IsInstall())
	{
		return true;
	}
	
	hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return false;
	}
	SC_LOCK	scLock = LockServiceDatabase(hSCM);

	GetModuleFileName(NULL,tzFilePathName,MAX_PATH);

	hService = CreateService(hSCM,m_tzServiceName,m_tzServiceName,SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,SERVICE_AUTO_START,SERVICE_ERROR_NORMAL,
		tzFilePathName,NULL,NULL,NULL,NULL,NULL);

	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		return false;
	}


	//修改服务描述
	SERVICE_DESCRIPTION ServiceDescription = {0};
	ServiceDescription.lpDescription = m_tzServieDescription;

	if (0 == ChangeServiceConfig2(hService,SERVICE_CONFIG_DESCRIPTION,&ServiceDescription))
	{
		;
	}


	CloseServiceHandle(hService);

	UnlockServiceDatabase(scLock);
	CloseServiceHandle(hSCM);
	return true;
}

// 启动服务
bool CServiceControl::Start(void)
{
	SC_HANDLE	hSCM = NULL;
	SC_HANDLE	hService = NULL;
	SERVICE_STATUS	ServiceStatus = {0};
	DWORD		dwWaitTime = 0;


	//打开服务
	hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return false;
	}
	hService = OpenService(hSCM,m_tzServiceName,SERVICE_QUERY_STATUS|SERVICE_START);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		return false;
	}

	//查询是否在运行
	QueryServiceStatus(hService,&ServiceStatus);
	if (ServiceStatus.dwCurrentState == SERVICE_RUNNING
		|| ServiceStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return true;
	}

	//启动服务
	if (!StartService(hService,0,NULL))
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return false;
	}

	//查询状态，是否已经启动完毕
	QueryServiceStatus(hService,&ServiceStatus);
	dwWaitTime = ServiceStatus.dwWaitHint / 10;
	if (dwWaitTime < 1000)
	{
		dwWaitTime = 1000;
	}
	else if(dwWaitTime > 10000)
	{
		dwWaitTime = 10000;
	}

    //////////////循环等待
	while(ServiceStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		Sleep(dwWaitTime);

		QueryServiceStatus(hService,&ServiceStatus);

	}
    //////////////

	if (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		;
	}
	else
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return false;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return true;
}

// 停止服务
bool CServiceControl::Stop(void)
{
	
		SC_HANDLE	hSCM = NULL;
		SC_HANDLE	hService = NULL;
		SERVICE_STATUS	ServiceStatus = {0};
		DWORD		dwWaitTime = 0;

		hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
		if (hSCM == NULL)
		{
			return false;
		}

		hService = OpenService(hSCM,m_tzServiceName,SERVICE_QUERY_STATUS|SERVICE_STOP);
		if (hService == NULL)
		{
			CloseServiceHandle(hSCM);
			return false;
		}

		//查询是否在运行
		QueryServiceStatus(hService,&ServiceStatus);
		if (ServiceStatus.dwCurrentState == SERVICE_STOPPED
			|| ServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return true;
		}

		//现在开始停止

		if (!ControlService(hService,SERVICE_CONTROL_STOP,&ServiceStatus))
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return false;
		}

		//再查询状态
		QueryServiceStatus(hService,&ServiceStatus);
		dwWaitTime = ServiceStatus.dwWaitHint / 10;
		if (dwWaitTime < 1000)
		{
			dwWaitTime = 1000;
		}
		else if (dwWaitTime > 10000)
		{
			dwWaitTime = 10000;
		}

		//////////////循环等待
		while (ServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
		{
			Sleep(dwWaitTime);
			QueryServiceStatus(hService,&ServiceStatus);
		}
		/////////////////////

		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);

		if (ServiceStatus.dwCurrentState == SERVICE_STOPPED)
		{
			return true;
		}
		else
		{
			return false;
		}
	
}

// 卸载服务
bool CServiceControl::UnInstall(void)
{
	if(g_isStopService == true)
	{
		SC_HANDLE hSCM = NULL;
		SC_HANDLE hService = NULL;
		SERVICE_STATUS	ServiceStatus = {0};

		//
		//检测是否安装
		if (!IsInstall())
		{
			return true;
		}

		//先停止服务
		if (!Stop())
		{
			return false;
		}
		//打开服务
		hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
		if (hSCM == NULL)
		{
			return false;
		}

		hService = OpenService(hSCM,m_tzServiceName,SERVICE_STOP|DELETE);
		if (hService == NULL)
		{
			CloseServiceHandle(hSCM);
			return false;
		}

		//开始卸载。
		if (!DeleteService(hService))
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return false;
		} 

		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return true;
	}
	return false;
}


// 查询服务状态
DWORD CServiceControl::QueryStatus(void)
{
	SC_HANDLE	hSCM = NULL;
	SC_HANDLE	hService = NULL;
	SERVICE_STATUS	ServiceStatus = {0};

	hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return false;
	}
	hService = OpenService(hSCM,m_tzServiceName,SERVICE_QUERY_STATUS);
	if (hService == NULL)
	{
		CloseServiceHandle(hSCM);
		return false;
	}

	//查询服务状态
	if (QueryServiceStatus(hService,&ServiceStatus))
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return false;
	}


	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return ServiceStatus.dwCurrentState;
}
