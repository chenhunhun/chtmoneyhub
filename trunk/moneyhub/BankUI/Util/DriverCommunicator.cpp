#include "StdAfx.h"
#include "DriverCommunicator.h"


#include "../..//Security/BankLoader/export.h"

HANDLE g_brEventHand=NULL;

#define     REGNAME           L"SOFTWARE\\BANKID"
#define     REGNAMEANTORUN    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define     REGKEYNAME        L"MoneyHub"

bool CDriverCommunicator::m_isOk=false;
CDriverCommunicator::CDriverCommunicator(void)
{
}

CDriverCommunicator::~CDriverCommunicator(void)
{
}


unsigned __stdcall  threadShowMess (void * p)
{
	//CDriverCommunicator * pThis=(CDriverCommunicator *)p;
    MessageBoxA(NULL,"系统安全检测中，请稍候!","提示",MB_OK);
	return 1;
}


void CDriverCommunicator::show()
{
	CloseHandle((HANDLE)_beginthreadex(NULL,NULL,threadShowMess,0,NULL,NULL) );

	DWORD begin=GetTickCount();
	HWND hand=NULL;
	MSG msg={0};

	while (1)
	{
		if( (GetTickCount() - begin) > 2 * 1000 )
		{	
		    PostQuitMessage(1);
		    break;
		}
	}
}

void CDriverCommunicator::communicationDriver()
{
	g_brEventHand =CreateEvent(NULL,FALSE,FALSE,L"BRENENT");

	_SecuCheck.SetEventsFunc(voidFun, this);
	_SecuCheck.Start();

}

/**
       empty function
*/
void CDriverCommunicator::voidFun(CheckStateEvent ev, DWORD dw, LPCTSTR lpszInfo, LPVOID lp)
{
	if (((int)dw) < 0)
	{
		SetEvent(g_brEventHand);
		m_isOk=false;
		return;
	}

	if( ev == CHK_END )
		SetEvent(g_brEventHand);

	m_isOk=true;
	return ;
}

/**
*        check register for send data
*/
REGSTATUS CDriverCommunicator::isSendData() const
{
	HKEY  hKey=NULL,hSubKey=NULL;
	DWORD index;

	if(RegCreateKeyExW(HKEY_LOCAL_MACHINE,REGNAME,0,0,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&hSubKey,&index) == ERROR_SUCCESS)
	{
		if( REG_CREATED_NEW_KEY == index)
			return CD_SUCESSFUL;
		else
			return CD_ALLREADYEXISTING;
	}
		
	return CD_ERROR;
}
/**
*        check driver's status for sending data
*/
bool CDriverCommunicator::isSendDataWithDriver() const
{
	return BankLoader::returnDriverStatus(DRIVER_RECEIVED)==1 ? true:false ;
}
/**
*         set autoRun with OS
*/
REGSTATUS CDriverCommunicator::setAutoRun()     const
{
	HKEY key=NULL,hSubKey=NULL;
	wchar_t buf[255]={0},lPath[255]={0};
	DWORD dType=0,dSize=sizeof(buf);

	this->getCurrentPath(lPath);
	if (ERROR_SUCCESS == ::SHGetValueW(HKEY_LOCAL_MACHINE, REGNAMEANTORUN, 
		REGKEYNAME, &dType, (byte*)buf, &dSize)
		)
	{
		if( (dType == REG_SZ) && (!wcscmp(lPath,buf)) )
		{
			return CD_ALLREADYEXISTING;
		}
		else
		{
			goto MYSETREG;
		}
	}
	else
	{
		//////////////////////
MYSETREG:
		if( ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE,REGNAMEANTORUN,0,KEY_ALL_ACCESS,&hSubKey) )
		{	
			if( ERROR_SUCCESS == RegSetValueExW(hSubKey,REGKEYNAME,0,REG_SZ,(byte*)lPath,sizeof(lPath) ) )
			{
				RegCloseKey(hSubKey);
				return CD_SUCESSFUL;
			}
			else
			{
				goto REGERROR;
			}
		}
		else
		{
			goto REGERROR;
		}
		///////////////////////////
	}
	
REGERROR:
	if(hSubKey)
		RegCloseKey(hSubKey);
	if(true == this->runWithUAC(1))
		exit(1);

	return CD_ERROR;

}


/**
*
*
*/
bool CDriverCommunicator::deleteAutoRun()         const
{
	HKEY  hSubKey = NULL;

	if( ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE,REGNAMEANTORUN,0,KEY_ALL_ACCESS,&hSubKey) )
	{
		if(ERROR_SUCCESS ==	RegDeleteValueW(hSubKey,REGKEYNAME) )
		{
			RegCloseKey(hSubKey);
			return true;
		}
	}

	if(hSubKey != NULL)    
		RegCloseKey(hSubKey);
	return false;
}
/**
*            
*/
void CDriverCommunicator::getCurrentPath(LPCWSTR lPath)  const
{
	wchar_t tmpPath[255]={0};

	GetModuleFileNameW(NULL,(LPWCH)lPath,255 );

	wcscat_s(tmpPath,L"\"");
	wcscat_s(tmpPath,lPath);
	wcscat_s(tmpPath,L"\"");
	wcscat_s(tmpPath,L" -br");
	wcscpy_s((wchar_t *)lPath,255,tmpPath);
	*((wchar_t *)lPath+wcslen(tmpPath))=L'\0';
}


/**
*
*/
void CDriverCommunicator::sendData()
{
	BankLoader::setSecuModuleBR();
}

void CDriverCommunicator::SendBlackList()
{
	// 发送黑名单
	BankLoader::SendBlackListToDriver();
}

/**
*
*
*/

DWORD GetStyle(HWND m_hWnd)
{
	ATLASSERT(::IsWindow(m_hWnd));
	return (DWORD)::GetWindowLong(m_hWnd, GWL_STYLE);
}


bool CDriverCommunicator::runWithUAC(BYTE index)    const
{
	wchar_t tmpPath[255]={0};

	GetModuleFileNameW(NULL,(LPWCH)tmpPath,255 );
	SHELLEXECUTEINFOW shExecInfo;

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

	//shExecInfo.fMask = NULL;
	shExecInfo.fMask = NULL;//SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI; 
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = L"runas";
	//shExecInfo.lpFile = L"notepad.exe";
	shExecInfo.lpFile = tmpPath;
	shExecInfo.lpParameters = NULL;
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_MAXIMIZE;
	shExecInfo.hInstApp = NULL;

	if(index == 1)//reg
	{
		if(IDOK == MessageBoxW(NULL,L"系统启动项被篡改，需要提权操作，请点击允许.",L"财金汇",MB_OKCANCEL))
		{
			ShellExecuteExW(&shExecInfo);
			//this->setAutoRun();
			return true;
		}
		else
			return false;
	}
	else if(index == 2)//service
	{
		if(IDOK == MessageBoxW(NULL,L"系统内核服务被篡改，需要提权操作，请点击允许.",L"财金汇",MB_OKCANCEL))
		{
			ShellExecuteExW(&shExecInfo);
			return true;
		}
		else
			 return false;
	}
	else
		return false;

}