#include "stdafx.h"
#include "info.h"
#pragma once

using namespace std;


class CUSBSoftWareDetector
{
private:
	CUSBSoftWareDetector();
	~CUSBSoftWareDetector();


	static CUSBSoftWareDetector* m_Instance;
public:
	static CUSBSoftWareDetector* GetInstance();

	bool CheckUSBSoftWare(USBSoftwareInfo& softinfo, bool bNeedRepair = false, bool bNeedRestart = false);//检测并修复software的运行情况,如果没有安装返回false，安装了返回true

	//bool CheckVesion();		// 检测版本，如果版本低的话，需要更新，需要和服务器通信
private:
	bool CheckFile(list<wstring>& fname);								// 检测文件是否存在作为标准
	bool CheckRegInfo(list<RegInfo>& reginfo);							// 检测注册表是否存在
	bool CheckKeyFile(std::map<std::wstring,ProgramType>& programinfo, bool bNeedRepair, bool bNeedRestart = false);

	bool CheckIsDriverInstalled(const wstring& dname);						//检测是否安装了驱动

	bool CheckServiceIsWork(const wstring& sname, bool bNeedRepair = false);	//这里需要单独处理，因为如果服务被禁止了，启动服务

	bool GetPriviledge();

	bool CheckProgramIsRunning(const wstring& pname, bool bNeedRestart = false);							//检测并启动关键进程
};
