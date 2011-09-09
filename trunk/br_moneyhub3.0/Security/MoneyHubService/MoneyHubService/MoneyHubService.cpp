// StartTimeService.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <Shlwapi.h>
#include "ServiceControl.h"

#pragma comment(lib,"shlwapi")


//////////////////////////////////////////////////////////////////////////export
TCHAR	tzServiceName[] = _T("MoneyHubService");
TCHAR	tzServiceDescription[] = _T("加速财金汇启动速度，关闭该服务将严重影响财金汇的启动速度！");
//////////////////////////////////////////////////////////////////////////


SERVICE_STATUS	ServiceStatus = {0};
SERVICE_STATUS_HANDLE	hStatus = NULL;
HANDLE	hEventEnd = NULL;	//服务是否结束事件
HANDLE  g_terminateHandle=NULL;
extern bool   g_isStopService;

//声明服务函数
void WINAPI ServiceMain();
void WINAPI ServiceCtrlHandle(DWORD dwOpcode);
//线程函数
DWORD	WINAPI ThreadMain(LPVOID pParam);

CServiceControl CSerCtrl(tzServiceName,tzServiceDescription);;

int _tmain(int argc, LPTSTR argv);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	if( (_tcsncmp(lpstrCmdLine, _T("install"), 7) == 0) 
		|| (_tcsncmp(lpstrCmdLine, _T("uninstall"), 9) == 0) 
		|| (_tcsncmp(lpstrCmdLine, _T("start"), 5) == 0)
		|| (_tcsncmp(lpstrCmdLine, _T("stop"), 4) == 0)  )

		return _tmain(2,lpstrCmdLine);

	else
		return _tmain(1,lpstrCmdLine);
}


int _tmain(int argc, LPTSTR argv)
{
	if (argc == 2)
	{
		//安装
		if (_tcsicmp(argv,_T("install")) == 0 )
		{
			if (CSerCtrl.Install())
			{
				return 1;
			}
		}
		//卸载
		if (_tcsicmp(argv,_T("uninstall")) == 0 )
		{
			g_isStopService=true;
			if (CSerCtrl.UnInstall())
			{
				return 1;
			}
		}
		//启动
		if (_tcsicmp(argv,_T("start")) == 0 )
		{
			if (CSerCtrl.Start())
			{
				return 1;
			}
		}

		//停止
		if (_tcsicmp(argv,_T("stop")) == 0 )
		{
			if (CSerCtrl.Stop())
			{
				return 1;
			}
		}

	}
	else
	{
		//启动服务
		SERVICE_TABLE_ENTRY ServiceTable[] = 
		{
			{tzServiceName,(LPSERVICE_MAIN_FUNCTION)ServiceMain},
			{NULL,NULL}
		};

		if (StartServiceCtrlDispatcher(ServiceTable) == 0)
		{
			return 0;
		}
	}

	return 0;
}

void WINAPI ServiceMain()
{
	HANDLE	hThread = NULL;
	DWORD	dwThreadID = 0;

	//DebugBreak();
	//初始化
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwServiceType = SERVICE_WIN32;

	//完成Handler函数的注册
	hStatus = RegisterServiceCtrlHandler(tzServiceName,(LPHANDLER_FUNCTION)ServiceCtrlHandle);

	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		return;
	}

	//报告正在启动
	SetServiceStatus(hStatus,&ServiceStatus);

	//创建一个事件进行同步
	hEventEnd = CreateEvent(NULL,TRUE,FALSE,NULL);
	if (hEventEnd == NULL)
	{
		return;
	}
	ResetEvent(hEventEnd);

	//报告启动完毕
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus,&ServiceStatus);

	//创建运行线程
	hThread = CreateThread(NULL,0,ThreadMain,NULL,NULL,&dwThreadID);
	if (!hThread)
	{
		SetEvent(hEventEnd);
	}
	CloseHandle(hThread);

	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;

	SetServiceStatus(hStatus,&ServiceStatus);

	//等待事件退出,结束主线程
	WaitForSingleObject(hEventEnd,INFINITE);
	CloseHandle(hEventEnd);
}

void WINAPI ServiceCtrlHandle(DWORD dwOpcode)
{
	switch (dwOpcode)
	{
	case SERVICE_CONTROL_STOP:
		//结束moneyhub.exe
		if(g_terminateHandle != NULL)
			TerminateProcess(g_terminateHandle,0);
		//开始停止
		ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(hStatus,&ServiceStatus);
		//设置停止信号
		SetEvent(hEventEnd);
		//停止
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus,&ServiceStatus);

		CloseHandle(hStatus);

		break;
	case SERVICE_CONTROL_PAUSE:
		ServiceStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
		SetServiceStatus(hStatus,&ServiceStatus);
		ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		SetServiceStatus(hStatus,&ServiceStatus);
		break;
	case SERVICE_CONTROL_CONTINUE:
		ServiceStatus.dwCurrentState = SERVICE_CONTINUE_PENDING;
		SetServiceStatus(hStatus,&ServiceStatus);
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		SetServiceStatus(hStatus,&ServiceStatus);
		break;

	case SERVICE_CONTROL_INTERROGATE:	//检索更新状态的时
		break;

	case SERVICE_CONTROL_SHUTDOWN:
		//结束moneyhub.exe
		if(g_terminateHandle != NULL)
			TerminateProcess(g_terminateHandle,0);

		ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(hStatus,&ServiceStatus);

		//设置停止信号
		SetEvent(hEventEnd);

		break;
	default:
		ServiceStatus.dwCurrentState = dwOpcode;
		SetServiceStatus(hStatus,&ServiceStatus);
		break;

	}
}
/************************************************************************/
/*                          工作线程                                    */
/************************************************************************/
DWORD WINAPI ThreadMain(LPVOID pParam)
{
	//WinExec("MoneyHub.exe -br",SW_HIDE);
	STARTUPINFOA ls;
	PROCESS_INFORMATION lp;
	ZeroMemory(&ls, sizeof(ls));
	ls.cb = sizeof(ls);
	ZeroMemory(&lp, sizeof(lp));
	
	CreateProcessA(NULL,"MoneyHub.exe -br",NULL,NULL,FALSE,0,NULL,NULL,&ls,&lp);
	g_terminateHandle=lp.hProcess;
	WaitForSingleObject(lp.hProcess,INFINITE);
	g_terminateHandle=NULL;

	return 0;
}