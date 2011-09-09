#include "windows.h"
#include "dbt.h"
//#include "usbioctl.h"
#include <string>
#include "info.h"
using namespace std;
#define WM_SHNOTIFY WM_USER + 0x101f
#pragma once
struct USBKeyCommInfo
{
	USBEigenvalue   eig;
	wstring			ctlername;// 记录控制器的名称，方便以后的通信
	ULONG			NumPorts;//  记录该设备在控制器中的端口号
};

class CAxUSBControl : public CWindowImpl<CAxUSBControl>
{

public:

	CAxUSBControl();
	~CAxUSBControl();

	DECLARE_WND_CLASS(_T("MH_USBMoneyhubAxUI"))

	BEGIN_MSG_MAP_EX(CAxUSBControl)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER_EX(WM_DEVICECHANGE, OnDeviceChange)
		//MESSAGE_HANDLER_EX(WM_SHNOTIFY, OnWin7DeviceChange)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		HANDLE_TUO_COPYDATA()
	END_MSG_MAP()
	
	LRESULT OnWin7DeviceChange(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDeviceChange(UINT uMsg, WPARAM wParam, LPARAM lParam);	
	int OnCreate(LPCREATESTRUCT lpCreateStruct);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnExit(UINT uMsg, WPARAM wParam, LPARAM lParam);
	list<USBKeyCommInfo*> m_usbcheckinglist; //测试正在进行测试的usbkey，下次开线程测试就不再进行测试了 //本工程中
	list<USBKeyCommInfo*> m_usbcheckedlist; //已经检测过的不在我们管理名单中的usbkey，下次不再进行检测了
private:
	void TestUSB();//有的USBKEY注册为了系统唯一id，和键盘鼠标一个类型
	bool GetUSBAtt(wstring& usbpath, USBEigenvalue& eig);

	int m_HIDChangeTime;//记录系统发的消息的次数，因为添加设备会发2次7的消息
	HDEVNOTIFY      m_NotifyDevHandle;
	ULONG   m_hNotify; 

	bool m_isInit;//处理usb检测枚举部分的程序
	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_cs2;

	USBEigenvalue m_nUSBEig;//记录新插入的usb设备的vid、pid等关键值
	list<USBKeyCommInfo*> m_usblist;// 存储usb信息的链表，包括控制器名称和端口号

	list<USBKeyCommInfo*> m_actusblist;//存储上一次检测的usb设备的数据信息
	static DWORD WINAPI _threadUSBDevice(LPVOID lp);
private:
	bool bChecking;
	bool bNeedReCheck;
	VOID EnumerateHostControllers();
	wstring GetHCDDriverKeyName(HANDLE  HCD);
	wstring GetRootHubName (HANDLE HostController);
	wstring GetExternalHubName (HANDLE  Hub, ULONG   ConnectionIndex);

	VOID EnumerateHub(wstring HubName);
	VOID EnumerateHubPorts(HANDLE hHubDevice,ULONG NumPorts, wstring nHubDeviceName);
	wstring GetDriverKeyName(HANDLE Hub,ULONG ConnectionIndex);

	void CheckNewUSBDevice(wstring ndname);
	bool CheckCheckedlist(USBKeyCommInfo& usb);
	bool CheckCheckinglist(USBKeyCommInfo& usb);
};