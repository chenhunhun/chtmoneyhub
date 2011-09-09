#include "stdafx.h"
#include "USBDevice.h"
#include <assert.h>
#include <basetyps.h>
#include "winioctl.h"
#include <usbioctl.h>
#include "info.h"
#include "usbiodef.h"
#include <algorithm>
#include "USBCheckor.h"
#include "USBHardWareDetector.h"
#define MY_STARTDELAYEVENT (0xff03)

#pragma once
#include "USBInfoFile.h"

#define ALLOC(dwBytes) GlobalAlloc(GPTR,(dwBytes))
#define REALLOC(hMem, dwBytes) GlobalReAlloc((hMem), (dwBytes), (GMEM_MOVEABLE|GMEM_ZEROINIT))
#define FREE(hMem)  GlobalFree((hMem))

#define NUM_HCS_TO_CHECK 10

static /*const*/ GUID MYGUID_DEVINTERFACE_USB_DEVICE = 
{ 0xA5DCBF10L, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

DEFINE_GUID( GUID_CLASS_USBHUB,    0xf18a0e88, 0xc30c, 0x11d0, 0x88, 0x15, 0x00, \
			0xa0, 0xc9, 0x06, 0xbe, 0xd8);

CAxUSBControl* pAxControl = NULL;
CAxUSBControl::CAxUSBControl():m_NotifyDevHandle(NULL),m_hNotify(0),bChecking(false),bNeedReCheck(false)
{
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs2);
	pAxControl = this;
}

CAxUSBControl::~CAxUSBControl()
{
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_cs2);
	list<USBKeyCommInfo*>::iterator ite;
	ite = m_usbcheckinglist.begin();
	// 将获得的usb进行对比，如果在我们的key列表中，检测是否需要mid再通信获得mid信息
	for(;ite != m_usbcheckinglist.end(); ite ++)
	{
		if((*ite) != NULL)
			delete (*ite);
	}
	m_usbcheckinglist.clear();
	m_usbcheckedlist.clear();
	if(!m_NotifyDevHandle)
		UnregisterDeviceNotification(m_NotifyDevHandle);
	if(m_hNotify != 0)
		SHChangeNotifyDeregister(m_hNotify); 
}


//////////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI * pChangeWindowMessageFilter)(UINT message, DWORD dwFlag);

int CAxUSBControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//HMODULE hDLL = ::LoadLibraryW(L"user32.dll");	// 动态装载组件

	//if(hDLL)
	//{
	//	pChangeWindowMessageFilter lpChangeWindowMessageFilter = (pChangeWindowMessageFilter)::GetProcAddress( hDLL, "ChangeWindowMessageFilter");	
	//	// "ChangeWindowMessageFilter"函数指针
	//	if(lpChangeWindowMessageFilter)	
	//	{
	//		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"USBControl注册消息ChangeWindowMessageFilter");
	//		lpChangeWindowMessageFilter(WM_DEVICECHANGE, MSGFLT_ADD);
	//	}
	//	FreeLibrary(hDLL);
	//}
	m_usbcheckedlist.clear();
	m_usbcheckinglist.clear();

	//OSVERSIONINFOEX OSVerInfo; 
	//OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	//if(!GetVersionEx((OSVERSIONINFO *)&OSVerInfo)) 
	//{ 
	//	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
	//	GetVersionEx((OSVERSIONINFO *)&OSVerInfo); 
	//} 

	//if(OSVerInfo.dwMajorVersion < 6) // Vista 以上 
	//{ 
	//	DEV_BROADCAST_DEVICEINTERFACE   broadcastInterface; 
	//	broadcastInterface.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	//	broadcastInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	//	memcpy( &(broadcastInterface.dbcc_classguid),
	//		&(MYGUID_DEVINTERFACE_USB_DEVICE),
	//		sizeof(struct _GUID));

	//	m_NotifyDevHandle = RegisterDeviceNotification(m_hWnd,
	//		&broadcastInterface,
	//		DEVICE_NOTIFY_WINDOW_HANDLE);
	//	DWORD error = ::GetLastError();

	//	if(m_NotifyDevHandle == NULL)
	//	{
	//		if(error != ERROR_ACCESS_DENIED) 
	//			SetTimer(MY_STARTDELAYEVENT, 60*1000, NULL);//第一次如果初始化失败，延迟进行初始化
	//		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"USBControl初始化RegisterDeviceNotification失败");
	//	}
	//}
	CUSBHardWareDetector::GetInstance()->InitLogicDriver();
	m_isInit = true;
	TestUSB();

	return 0;
}

LRESULT CAxUSBControl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if(wParam == MY_STARTDELAYEVENT)
	{

		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"USBControl进入延迟进行初始化");

		//延迟2分钟显示
		KillTimer(MY_STARTDELAYEVENT);
		// 在初始化的时候要检测一遍USB设备已经插入与否	
		// Register to receive notification when a USB device is plugged in.
		// 这里注意一定要有注册，否则接收不到有的USB设备插入信息	
		DEV_BROADCAST_DEVICEINTERFACE   broadcastInterface; 
		broadcastInterface.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		broadcastInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

		memcpy( &(broadcastInterface.dbcc_classguid),
			&(MYGUID_DEVINTERFACE_USB_DEVICE),
			sizeof(struct _GUID));

		m_NotifyDevHandle = RegisterDeviceNotification(m_hWnd,
			&broadcastInterface,
			DEVICE_NOTIFY_WINDOW_HANDLE);
		m_isInit = true;
		
		TestUSB();

	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////


LRESULT CAxUSBControl::OnExit(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	::TerminateProcess(::GetCurrentProcess(), 0);
	return 0;
}

//*****************************************************************************
//
// GetHCDDriverKeyName()
//
//*****************************************************************************

wstring CAxUSBControl::GetHCDDriverKeyName(HANDLE  HCD)
{
    BOOL                    success;
    ULONG                   nBytes;
    USB_HCD_DRIVERKEY_NAME  driverKeyName;
    PUSB_HCD_DRIVERKEY_NAME driverKeyNameW;

    driverKeyNameW = NULL;
	// Get the length of the name of the driver key of the HCD
    success = DeviceIoControl(HCD,
							  IOCTL_GET_HCD_DRIVERKEY_NAME,
                              &driverKeyName,
                              sizeof(driverKeyName),
                              &driverKeyName,
                              sizeof(driverKeyName),
                              &nBytes,
                              NULL);
    if (!success)
        return NULL;
    // Allocate space to hold the driver key name
    nBytes = driverKeyName.ActualLength;

    if (nBytes <= sizeof(driverKeyName))
        return NULL;

    driverKeyNameW = (PUSB_HCD_DRIVERKEY_NAME)ALLOC(nBytes);
	ZeroMemory(driverKeyNameW, nBytes);

    if (driverKeyNameW == NULL)
    {
        return NULL;
    }
	// Get the name of the driver key of the device attached to
    // the specified port.
    success = DeviceIoControl(HCD,
                              IOCTL_GET_HCD_DRIVERKEY_NAME,
                              driverKeyNameW,
                              nBytes,
                              driverKeyNameW,
                              nBytes,
                              &nBytes,
                              NULL);
    if (success)
    {
		wstring temp(driverKeyNameW->DriverKeyName);
		FREE(driverKeyNameW);
		return temp;        
    }
	FREE(driverKeyNameW);
	return NULL;
}

wstring CAxUSBControl::GetRootHubName(HANDLE HostController)
{
	BOOL                success;
	ULONG               nBytes;
	USB_ROOT_HUB_NAME   rootHubName;
	PUSB_ROOT_HUB_NAME  rootHubNameW;

	rootHubNameW = NULL;

	// Get the length of the name of the Root Hub attached to the
	// Host Controller
	success = DeviceIoControl(HostController,
		IOCTL_USB_GET_ROOT_HUB_NAME,
		0,
		0,
		&rootHubName,
		sizeof(rootHubName),
		&nBytes,
		NULL);
	if (!success)
	{
		return NULL;
	}
	// Allocate space to hold the Root Hub name
	nBytes = rootHubName.ActualLength;

	rootHubNameW = (PUSB_ROOT_HUB_NAME)ALLOC(nBytes);
	if (rootHubNameW == NULL)
	{
		return NULL;
	}
	// Get the name of the Root Hub attached to the Host Controller
	//
	success = DeviceIoControl(HostController,
		IOCTL_USB_GET_ROOT_HUB_NAME,
		NULL,
		0,
		rootHubNameW,
		nBytes,
		&nBytes,
		NULL);
	if (success)
	{
		wstring temp(rootHubNameW->RootHubName);
		FREE(rootHubNameW);
		return temp;
	}
	return NULL;
}

VOID CAxUSBControl::EnumerateHostControllers()
{
	WCHAR       HCName[16];
    int         HCNum;
    HANDLE      hHCDev;

    wstring       rootHubName;

    // Iterate over some Host Controller names and try to open them.
    for (HCNum = 0; HCNum < NUM_HCS_TO_CHECK; HCNum++)
    {
        wsprintf(HCName, L"\\\\.\\HCD%d", HCNum);
        hHCDev = CreateFileW(HCName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        // If the handle is valid, then we've successfully opened a Host
        // Controller.  Enumerate the Root Hub attached to the Host Controller.
        if (hHCDev != INVALID_HANDLE_VALUE)
        {
            wstring driverKeyName, deviceDesc;
			driverKeyName = GetHCDDriverKeyName(hHCDev);
            rootHubName = GetRootHubName(hHCDev);
            if (rootHubName != L"")
            {
               EnumerateHub( rootHubName);
            }
			CloseHandle(hHCDev);
        }        
     }
}

VOID CAxUSBControl::EnumerateHub(wstring HubName)
{
	HANDLE          hHubDevice;
	wstring         deviceName;
	BOOL            success;
	ULONG           nBytes;
	PUSB_NODE_INFORMATION  info;

	// Initialize locals to not allocated state so the error cleanup routine
	// only tries to cleanup things that were successfully allocated.
	info        = NULL;
	hHubDevice  = INVALID_HANDLE_VALUE;

	// Allocate some space for a USB_NODE_INFORMATION structure for this Hub,
	info = (PUSB_NODE_INFORMATION)ALLOC(sizeof(USB_NODE_INFORMATION));
	if (info == NULL)
	{
		return;
	}
	// Allocate a temp buffer for the full hub device name.
	deviceName = L"\\\\.\\" + HubName;
	// Try to hub the open device
	//
	hHubDevice = CreateFile(deviceName.c_str(),	GENERIC_WRITE,	FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hHubDevice == INVALID_HANDLE_VALUE)
	{
		FREE(info);
		return;
	}
	// Now query USBHUB for the USB_NODE_INFORMATION structure for this hub.
	// This will tell us the number of downstream ports to enumerate, among
	// other things.
	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_NODE_INFORMATION,
		info, sizeof(USB_NODE_INFORMATION), 
		info, sizeof(USB_NODE_INFORMATION),
		&nBytes,
		NULL);
	if (!success)
	{
		CloseHandle(hHubDevice);
		FREE(info);
		return;
	}
	// Now recursively enumrate the ports of this hub.
	EnumerateHubPorts(hHubDevice, info->u.HubInformation.HubDescriptor.bNumberOfPorts, deviceName);

	CloseHandle(hHubDevice);
	FREE(info);
	return;
}


VOID CAxUSBControl::EnumerateHubPorts(HANDLE hHubDevice,ULONG NumPorts,wstring nHubDeviceName)
{
	ULONG		index;
	BOOL        success;

	PUSB_NODE_CONNECTION_INFORMATION    connectionInfo;

	// Port indices are 1 based, not 0 based.
	for (index=1; index <= NumPorts; index++)
	{
		ULONG nBytes;
		// Allocate space to hold the connection info for this port.
		nBytes = sizeof(USB_NODE_CONNECTION_INFORMATION) + sizeof(USB_PIPE_INFO) * 30;
		connectionInfo = (PUSB_NODE_CONNECTION_INFORMATION)ALLOC(nBytes);

		if (connectionInfo == NULL)
			break;
		// Now query USBHUB for the USB_NODE_CONNECTION_INFORMATION structure
		// for this port. 
		connectionInfo->ConnectionIndex = index;
		success = DeviceIoControl(hHubDevice, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION,
			connectionInfo,	nBytes,	connectionInfo,	nBytes,
			&nBytes, NULL);
		if (!success)
		{
			FREE(connectionInfo);
			continue;
		}

		// 把找到的USB设备的配置信息放到list链表中
		if (connectionInfo->ConnectionStatus == DeviceConnected)
		{
			// 输入参数中的nHubDeviceName这个是控制器的名称，用于记录与新插入的usb设备进行
			// 如果是初始化时的枚举，那么将所有设备进行枚举，如果不是，那么只关心后插入的usb设备，其他过滤掉
			// 目前来说得到这些信息已经够用了，已经得到了VID和PID，如果不够用还需要额外得到
			// 描述信息，要继续发送IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION消息
			if(m_isInit)
			{	
				USBKeyCommInfo *pinfo = new USBKeyCommInfo;
				pinfo->eig.VendorID = connectionInfo->DeviceDescriptor.idVendor;
				pinfo->eig.ProductID = connectionInfo->DeviceDescriptor.idProduct;
				//info.eig.SerierlNumber = connectionInfo->DeviceDescriptor.iSerialNumber;
				pinfo->ctlername = nHubDeviceName;
				pinfo->NumPorts = NumPorts;

				//在这里做检测比较好，因为已经在通信了，后来直接还要获得MID
				m_usblist.push_back(pinfo);
			}
			else
			{	
				//m_nUSBEig在这里面记录了新插入的设备的vid，pid；
				if((connectionInfo->DeviceDescriptor.idVendor == m_nUSBEig.VendorID) && (connectionInfo->DeviceDescriptor.idProduct == m_nUSBEig.ProductID))
				{
					USBKeyCommInfo *pinfo = new USBKeyCommInfo;
					pinfo->eig.VendorID = connectionInfo->DeviceDescriptor.idVendor;
					pinfo->eig.ProductID = connectionInfo->DeviceDescriptor.idProduct;
					//info.eig.SerierlNumber = connectionInfo->DeviceDescriptor.iSerialNumber;
					pinfo->ctlername = nHubDeviceName;
					pinfo->NumPorts = NumPorts;
					// 放入新usb设备的控制器及相关信息
					m_usblist.push_back(pinfo);
				}
			}
		}

		// If the device connected to the port is an external hub, get the
		// name of the external hub and recursively enumerate it.
		//
		if (connectionInfo->DeviceIsHub)
		{
			wstring extHubName;
			extHubName = GetExternalHubName(hHubDevice,index);
			if (extHubName != L"")
			{
				EnumerateHub(extHubName);
				// On to the next port
			}
		}

		FREE(connectionInfo);
	}
}

//*****************************************************************************
//
// GetExternalHubName()
//
//*****************************************************************************

wstring CAxUSBControl::GetExternalHubName(HANDLE  Hub, ULONG   ConnectionIndex)
{
    BOOL                        success;
    ULONG                       nBytes;
    USB_NODE_CONNECTION_NAME    extHubName;
    PUSB_NODE_CONNECTION_NAME   extHubNameW;

    extHubNameW = NULL;

    // Get the length of the name of the external hub attached to the
    // specified port.
    //
    extHubName.ConnectionIndex = ConnectionIndex;
    success = DeviceIoControl(Hub,
                              IOCTL_USB_GET_NODE_CONNECTION_NAME,
                              &extHubName,
                              sizeof(extHubName),
                              &extHubName,
                              sizeof(extHubName),
                              &nBytes,
                              NULL);

    if (!success)
    {
		return NULL;
    }
    // Allocate space to hold the external hub name
    //
    nBytes = extHubName.ActualLength;

    if (nBytes <= sizeof(extHubName))
    {
       return NULL;
    }

    extHubNameW = (PUSB_NODE_CONNECTION_NAME)ALLOC(nBytes);

    if (extHubNameW == NULL)
    {
        return NULL;
	}
    extHubNameW->ConnectionIndex = ConnectionIndex;
    success = DeviceIoControl(Hub,
                              IOCTL_USB_GET_NODE_CONNECTION_NAME,
                              extHubNameW,
                              nBytes,
                              extHubNameW,
                              nBytes,
                              &nBytes,
                              NULL);

    if (!success)
    {
        FREE(extHubNameW);
		return NULL;
    }

	wstring temp(extHubNameW->NodeName);
    FREE(extHubNameW);

    return temp;
}
LRESULT CAxUSBControl::OnWin7DeviceChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"接收到Svr的检测信号");

	// win7下由svr通知usb设备变化
	m_isInit = true;//遍历一遍usb
	TestUSB();


	return 0;
}
LRESULT CAxUSBControl::OnDeviceChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_isInit = true;//遍历一遍usb
	TestUSB();
	//switch(wParam)
	//{
	//	case DBT_DEVICEARRIVAL://插入U盘
	//		{
	//		//有的USBKEY注册为了系统唯一id，和键盘鼠标一个类型，需要单独处理
	//		//银行网银U盘一般会注册为CDROM、MASS STORAGE、但也有没有提示的，所以这里需要加判断，用统一的USB判断
	//			PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
	//			if(pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
	//			{
	//				PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
	//				pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;//这里已经获得了插入的USB信息，就不用再枚举了
	//				wstring ndevicename(pDevInf->dbcc_name);

	//				CheckNewUSBDevice(ndevicename);
	//			}
	//		}
	//		break;
	//	case DBT_DEVICEREMOVECOMPLETE://删除U盘不管
	//	default:
	//		break;
	//}
	return 0;
}

DWORD WINAPI CAxUSBControl::_threadUSBDevice(LPVOID lp)
{
	// 多线程要做一个checkinglist，否则会出现重复查询的过程
	USBKeyCommInfo* pusb = (USBKeyCommInfo*)lp;
	if(pusb == NULL)
		return 0;

	::EnterCriticalSection(&pAxControl->m_cs2);
	pAxControl->m_usbcheckedlist.push_back(pusb);
	::LeaveCriticalSection (&pAxControl->m_cs2);

	USBHardwareInfo usb;
	usb.eig.ProductID = pusb->eig.ProductID;
	usb.eig.VendorID = pusb->eig.VendorID;
	if(CUSBCheckor::GetInstance()->CheckUSBInfo(usb) == false)
	{
		::EnterCriticalSection(&pAxControl->m_cs);
		pAxControl->m_usbcheckinglist.push_back(pusb);
		::LeaveCriticalSection (&pAxControl->m_cs);
		::EnterCriticalSection(&pAxControl->m_cs2);
		pAxControl->m_usbcheckedlist.remove(pusb);
		::LeaveCriticalSection (&pAxControl->m_cs2);

	}
	else
	{
		::EnterCriticalSection(&pAxControl->m_cs2);
		pAxControl->m_usbcheckedlist.remove(pusb);
		::LeaveCriticalSection (&pAxControl->m_cs2);
		delete pusb;
	}


	return 0;
}

void CAxUSBControl::TestUSB()
{
	m_usblist.clear();
	EnumerateHostControllers();
	if(m_usblist.size() > m_actusblist.size())//说明有新增usb设备
	{
		//CUSBHardWareDetector::GetInstance()->RecheckLogicDriver();
		USBKeyCommInfo *ptemp;
		list<USBKeyCommInfo *>::iterator lite, aite;
		lite = m_usblist.begin();
		for(;lite != m_usblist.end(); )
		{
			bool tflag = true;
			if((*lite)!= NULL)
			{
				for(aite = m_actusblist.begin(); aite != m_actusblist.end(); aite ++)
				{
					if(((*aite)->eig.VendorID == (*lite)->eig.VendorID) && ((*aite)->eig.ProductID == (*lite)->eig.ProductID) &&((*aite)->ctlername == (*lite)->ctlername))//说明不是新增的
					{
						ptemp = (*lite);
						lite = m_usblist.erase(lite);
						delete ptemp;
						tflag = false;
						break;
					}
				}
			}
			if(tflag)
			{	
				USBKeyCommInfo* pinfo = new USBKeyCommInfo;
				pinfo->eig.VendorID = (*lite)->eig.VendorID;
				pinfo->eig.ProductID = (*lite)->eig.ProductID;
				pinfo->ctlername = (*lite)->ctlername;
				pinfo->NumPorts = (*lite)->NumPorts;
				m_actusblist.push_back(pinfo);
				lite ++;
			}
		}
	}
	else if(m_usblist.size() < m_actusblist.size())//说明usb设备减少
	{
		CUSBHardWareDetector::GetInstance()->RecheckLogicDriver();
		m_actusblist.swap(m_usblist);
		list<USBKeyCommInfo *>::iterator lite = m_usblist.begin();
		for(;lite != m_usblist.end(); lite ++)
		{
			if((*lite) != NULL)
				delete (*lite);
		}
		m_usblist.clear();
		return;		
	}
	else//usb设备没有变化
		return;
	// 将所有的USB信息进行对比
	list<USBKeyCommInfo *>::iterator ite;
	ite = m_usblist.begin();
	// 将获得的usb进行对比，如果在我们的key列表中，检测是否需要mid再通信获得mid信息
	for(;ite != m_usblist.end(); ite ++)
	{
		// 暂时只用USBKeyCommInfo中特征值部分//多线程？
		if((*ite) != NULL)
		{
			USBHardwareInfo usb;
			usb.eig.ProductID = (**ite).eig.ProductID;
			usb.eig.VendorID = (**ite).eig.VendorID;
			::EnterCriticalSection(&m_cs);
			if(CheckCheckedlist((**ite)))
			{
				::LeaveCriticalSection (&m_cs);
				continue;
			}
			::LeaveCriticalSection (&m_cs);

			::EnterCriticalSection(&m_cs2);
			if(CheckCheckinglist((**ite)))
			{
				::LeaveCriticalSection (&m_cs2);
				continue;
			}
			::LeaveCriticalSection (&m_cs2);
		// 先做个单线程查询
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, 
				CRecordProgram::GetInstance()->GetRecordInfo(L"检查到USB设备VID:%d__PID:%d", usb.eig.VendorID , usb.eig.ProductID));

			DWORD dw;
			USBKeyCommInfo* pinfo = new USBKeyCommInfo;
			pinfo->eig.VendorID = (*ite)->eig.VendorID;
			pinfo->eig.ProductID = (*ite)->eig.ProductID;
			pinfo->ctlername = (*ite)->ctlername;
			pinfo->NumPorts = (*ite)->NumPorts;

			CloseHandle(CreateThread(NULL, 0, _threadUSBDevice, (LPVOID)pinfo, 0, &dw));
		}
	}

	ite = m_usblist.begin();
	// 将获得的usb进行对比，如果在我们的key列表中，检测是否需要mid再通信获得mid信息
	for(;ite != m_usblist.end(); ite ++)
	{
		if((*ite) != NULL)
			delete (*ite);
	}
	m_usblist.clear();
}
bool CAxUSBControl::CheckCheckinglist(USBKeyCommInfo& usb)
{
	list<USBKeyCommInfo*>::iterator ite;
	ite = m_usbcheckedlist.begin();
	// 将获得的usb进行对比，如果在我们的key列表中
	for(;ite != m_usbcheckedlist.end(); ite ++)
	{
		if((usb.eig.ProductID == (**ite).eig.ProductID) && (usb.eig.VendorID == (**ite).eig.VendorID)&& (usb.ctlername == (**ite).ctlername))
			return true;
	}
	return false;
	
}
bool CAxUSBControl::CheckCheckedlist(USBKeyCommInfo& usb)
{
	list<USBKeyCommInfo*>::iterator ite;
	ite = m_usbcheckinglist.begin();
	// 将获得的usb进行对比，如果在我们的key列表中
	for(;ite != m_usbcheckinglist.end(); ite ++)
	{
		if((usb.eig.ProductID == (**ite).eig.ProductID) && (usb.eig.VendorID == (**ite).eig.VendorID) && (usb.ctlername == (**ite).ctlername))
			return true;
	}
	return false;
	
}
void CAxUSBControl::CheckNewUSBDevice(wstring ndname)
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, CRecordProgram::GetInstance()->GetRecordInfo(L"CheckNewUSBDevice:%s",ndname.c_str()));
	// 在得到这个设备名称之后如何与该设备通信获得vid和pid呢？
	// 暂时先用枚举所有usb控制器设备，对比VID和PID获得usb控制器，然后与其通信进行处理
	// 以后再看有什么方法直接通过这里传过来的名字直接获得USB控制器
	if(!GetUSBAtt(ndname,m_nUSBEig))
	{
		m_isInit = true;//如果通过名称没有正确获得，那么获取所有的从头处理一遍
	}
	else
	{
		m_isInit = false;
	}

	TestUSB();
}

bool CAxUSBControl::GetUSBAtt(wstring& usbpath, USBEigenvalue& eig)
{
	//先把名称转换为全大写
	transform(usbpath.begin(), usbpath.end(), usbpath.begin(), towupper);
	size_t nvidName = usbpath.find(L"VID_");
	if(nvidName != wstring::npos)
	{
		std::wstring strVidName = usbpath.substr(nvidName + 4, 4);
		strVidName = L"0x" + strVidName;
		USHORT vid = wcstol(strVidName.c_str(), NULL, 16);
		eig.VendorID = vid;
	}
	else
		return false;

	nvidName = usbpath.find(L"PID_");
	if(nvidName != wstring::npos)
	{
		std::wstring strPidName = usbpath.substr(nvidName + 4, 4);
		strPidName = L"0x" + strPidName;
		USHORT pid = wcstol(strPidName.c_str(), NULL, 16);
		eig.ProductID = pid;
	}
	else
		return false;

	nvidName = usbpath.find(L"#", 10);
	if(nvidName != wstring::npos)
	{
		std::wstring strSN = usbpath.substr(nvidName + 1, 14);
		USES_CONVERSION;
		eig.SerierlNumber = W2A(strSN.c_str());
	}
	else
		return false;

	return true;
}


	/*if(NULL == OldSetupDiGetClassDevsW || NULL == OldSetupDiEnumDeviceInterfaces ||
		NULL == OldSetupDiDestroyDeviceInfoList || NULL == OldSetupDiGetDeviceInterfaceDetailW)
		return;
*/

	//GUID  hidGuid;
	//GUID* guid;
	//if(!isSHIDDevice)
	//	guid = (GUID*)(void*)&GUID_DEVINTERFACE_USB_DEVICE;
	//else
	//{
	//	OldHidD_GetHidGuid (&hidGuid);
	//	guid = (GUID*)&hidGuid; //人体光学输入设备，有的usbkey用
	//}

	////   准备查找符合HID规范的USB设备
	//
	//HDEVINFO   hDevInfo = OldSetupDiGetClassDevsW(guid, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE); 
	//int rest = GetLastError();
	//if(ERROR_INVALID_PARAMETER == rest || ERROR_INVALID_FLAGS == rest)
	//	return;

	////   查找USB设备接口 
	//SP_DEVICE_INTERFACE_DATA   strtInterfaceData; 
	//strtInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	//DWORD dwIndex = 0;
	////ZeroMemory(&strtInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));
	////strtInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	//BOOL bRet = FALSE;
	//while(TRUE)
	//{
	//	
	//// 这里同时插入2个usb会产生问题，可能找到的不是第一个，上面的代码解决的就是该问题,现在没有多个的需求，暂时用一个的
	//// 找到第一个符合该usb接口的设备
	//	BOOL   bSuccess = OldSetupDiEnumDeviceInterfaces(hDevInfo, NULL, guid, dwIndex, 
	//		&strtInterfaceData);
	//	if (!bSuccess || strtInterfaceData.Flags != SPINT_ACTIVE)
	//	{
	//		if (GetLastError() == ERROR_NO_MORE_ITEMS)
	//		{
	//			break;
	//		}
	//		continue;
	//	}

	//	dwIndex ++ ;
	//	//   若找到了设备，则读取设备路径名 
	//	PSP_DEVICE_INTERFACE_DETAIL_DATA   strtDetailData; 
	//	DWORD strSize = 0;
	//	DWORD reqSize = 0;
	//	//  获得需要的缓冲区大小
	//	OldSetupDiGetDeviceInterfaceDetailW(hDevInfo,&strtInterfaceData,NULL,0,&strSize,NULL);
	//	//SetupDiGetDeviceInterfaceDetail
	//	reqSize = strSize;
	//	strtDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)new unsigned char[reqSize];
	//	ZeroMemory(strtDetailData, reqSize);
	//	strtDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	//	// 获得路径名称,这里获得的是USB名称，如果要获得更细节的，要获得其下面的USBSTOR的路径进行操作
	//	if (!OldSetupDiGetDeviceInterfaceDetailW(hDevInfo,&strtInterfaceData, 
	//		strtDetailData,strSize,&reqSize,NULL)) 
	//	{ 	
	//		// */"查找设备路径时出错! "
	//		delete strtDetailData;
	//		continue;

	//	}
	//	//
	//	//  中间是与USB设备通信
	//	//   开放与设备的通信
	//	wstring usbpath(strtDetailData->DevicePath);
	//	// 这类设备被系统独占，第2个参数需要设为0
	//	HANDLE   hCom;
	//	if(!isSHIDDevice)
	//		hCom = CreateFileW(usbpath.c_str(), GENERIC_READ | GENERIC_WRITE, 
	//			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
	//	else
	//		hCom = CreateFileW(usbpath.c_str(), 0,
	//			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //人体光学设备需要用这个
	//	delete strtDetailData;
	//	if (hCom == INVALID_HANDLE_VALUE) 
	//	{
	//		continue;
	//	} 
	//	// 暂时可以都用
	//	USBEigenvalue eig;
	//	if(!isSHIDDevice)
	//		GetVIDnPID(usbpath, eig);
	//	else
	//	{
	//		HIDD_ATTRIBUTES deviceAttributes;
	//		if (!OldHidD_GetAttributes(hCom, &deviceAttributes)) //如果调用该函数失败，那么用名称进行处理  
	//		{
	//			GetVIDnPID(usbpath, eig);
	//		}
	//		else
	//		{
	//			eig.ProductID = deviceAttributes.ProductID;
	//			eig.VendorID = deviceAttributes.VendorID;
	//			eig.VersionNumber = deviceAttributes.VersionNumber;
	//		}
	//	}
	//	
	//	//如果打开直接
	//	//// 获得了
	//	//PSTORAGE_DEVICE_DESCRIPTOR pDeviceDesc;
	//	//pDeviceDesc=(PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
	//	//pDeviceDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
	//	////大容量存储类型的是不能通过HidD_GetAttributes获得VID和PID的，下面将通过名称获得
	//	//if(GetDriveProperty(hCom, pDeviceDesc) == FALSE)
	//	//	rest = GetLastError();

	//	//   查询设备标识 



	//		
	//			
	//	// 这个地方获得了usb设备的vid，pid和相应参数存放在strtAttrib
	//	// 根据vid和pid获得设备号，如果不需要获得mid，那么直接向下，如果需要获得mid，则继续进行通信

	//	// 释放资源 
	//	CloseHandle(hCom); 
	//}
	////
	//OldSetupDiDestroyDeviceInfoList(hDevInfo);



//// 获取存储设备类型的具体参数，目前不用,hDevice要用usbstor类型的打开
//BOOL CAxUSBControl::GetDriveProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
//{
//	STORAGE_PROPERTY_QUERY Query;    // 查询输入参数
//	DWORD dwOutBytes;                // IOCTL输出数据长度
//	BOOL bResult;                    // IOCTL返回值
//	// 指定查询方式
//
//	Query.PropertyId = StorageDeviceProperty;
//	Query.QueryType = PropertyStandardQuery;
//	// 用IOCTL_STORAGE_QUERY_PROPERTY取设备属性信息
//	bResult = ::DeviceIoControl(hDevice, // 设备句柄
//		IOCTL_STORAGE_QUERY_PROPERTY,    // 取设备属性信息
//		&Query, sizeof(STORAGE_PROPERTY_QUERY),    // 输入数据缓冲区
//		pDevDesc, pDevDesc->Size,        // 输出数据缓冲区
//		&dwOutBytes,                     // 输出数据长度
//		(LPOVERLAPPED)NULL);             // 用同步I/O    
//
//	return bResult;
//}


