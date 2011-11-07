#include "stdafx.h"
#include "info.h"
#include <list>
#include <map>
#pragma once

using namespace std;


class CUSBHardWareDetector
{
private:
	CUSBHardWareDetector();
	~CUSBHardWareDetector();


	static CUSBHardWareDetector* m_Instance;
public:
	static CUSBHardWareDetector* GetInstance();

	USBKeyInfo* CheckUSBHardWare(USBHardwareInfo& hardinfo);//检测hardware的情况

	USBKeyInfo* CheckUSBHardWare(int vid, int pid, DWORD mid);//无提示的检测，直接获得检测结果，core进程检测usb相关信息的状态

private:
	USBKeyInfo* CheckSupportUSB(USBHardwareInfo& hardinfo); 

	USBKeyInfo* CheckSupportUSB(int vid, int pid, DWORD mid);// 无mid的检测，直接返回结果，core进程检测usb相关信息的状态

	bool CheckMid(USBEigenvalue& acteig, USBEigenvalue& dataeig);		// 检测mid


	//list<string> m_checkedlogicdriver;	//存储已经检查过的U盘盘符
	map<int, string> m_logicdriver;	//改变之前的
	DWORD m_allDisk;

	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_cs2;
public:
	bool InitLogicDriver();
	bool RecheckLogicDriver();//更新U盘
	bool CheckLogicDriver(string& vname);//更新U盘
	static wstring drvName[26];
};
