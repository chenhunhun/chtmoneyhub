#include "stdafx.h"
#include "USBHardWareDetector.h"
#include "USBControl.h"
//#include "../Moneyhub_Agent/Skin/CoolMessageBox.h"
//#include "AliPayTestor.h"
#include <algorithm>
#pragma once

wstring CUSBHardWareDetector::drvName[26] = {L"A:",L"B:",L"C:",L"D:",L"E:",L"F:",L"G:",L"H:",L"I:",L"J:",L"K:",L"L:",L"M:",L"N:",L"O:",
L"P:",L"Q:",L"R:",L"S:",L"T:",L"U:",L"V:",L"W:",L"X:",L"Y:",L"Z:"};

CUSBHardWareDetector* CUSBHardWareDetector::m_Instance = NULL;

CUSBHardWareDetector* CUSBHardWareDetector::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CUSBHardWareDetector();
	return m_Instance;
}

CUSBHardWareDetector::CUSBHardWareDetector()
{
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs2);
	
}
CUSBHardWareDetector::~CUSBHardWareDetector()
{
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_cs2);
}

//检测hardware的情况
USBKeyInfo* CUSBHardWareDetector::CheckUSBHardWare(USBHardwareInfo& hardinfo)
{
	// 根据VID和PID找到支持的关键数据
	hardinfo.eig.hasMid = false;
	USBKeyInfo* psusb = CheckSupportUSB(hardinfo);
	if(psusb)// 找到了支持的usb的数据
	{
		CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"CheckSupportUSB: psusb != NULL");
		if(psusb->bSupport)//检测该usb设备是否支持该系统
		{
			return psusb;
			// 检测到支持该usbkey设备
			// 查找是否
		}
		else
		{			
			std::wstring msg;
			msg = L"检测到" + (psusb->hardware.finaninstitution) + L"的" + (psusb->hardware.goodsname) + L"型号USBKEY，但在您的系统上暂时不支持使用，请在USBKEY推荐系统使用。";
			MessageBox( NULL, msg.c_str(), L"财金汇用户提示",MB_OK);
		}

	}
	return NULL;
}

USBKeyInfo* CUSBHardWareDetector::CheckUSBHardWare(int vid, int pid, DWORD mid)//无提示的检测，直接获得检测结果
{
	USBKeyInfo* psusb = CheckSupportUSB(vid, pid, mid);
	if(psusb)// 找到了支持的usb的数据
	{
		if(psusb->bSupport)//检测该usb设备是否支持该系统
		{
			return psusb;
		}

	}
	return NULL;
}

USBKeyInfo* CUSBHardWareDetector::CheckSupportUSB(int vid, int pid, DWORD mid)
{
	list<USBKeyInfo>* pUsb = CUSBControl::GetInstance()->GetUSBInfo();
	list<USBKeyInfo>::iterator ite = pUsb->begin();
	for(;ite != pUsb->end(); ite ++)
	{
		if((pid == (*ite).hardware.eig.ProductID) && (vid == (*ite).hardware.eig.VendorID))//检查VID和PID，如果相同
		{
			if(mid == 0)
			{
				return (&(*ite));
			}
			else
			{
				if(mid == (*ite).hardware.eig.m_midDword)
				{
					return (&(*ite));
				}
			}
		}
	}
	return NULL;
}

bool CUSBHardWareDetector::InitLogicDriver()
{
	DWORD allDisk = ::GetLogicalDrives();  //返回一个32位整数，将他转换成二进制后，表示磁盘,最低位为A盘	
	m_allDisk = allDisk;

	if( allDisk != 0 )
	{
		USES_CONVERSION;
		::EnterCriticalSection(&m_cs);
		for (int i = 0; i < 32 ; i ++)     // 遍历磁盘
		{
			if ((allDisk & 1) == 1)
			{
				UINT type = ::GetDriveType(drvName[i].c_str());
				if ( type == DRIVE_CDROM )
				{
					DWORD flag;
					WCHAR info[MAX_PATH + 1] = {0};
					int err = 0;
					if( ::GetVolumeInformationW(drvName[i].c_str(), info, MAX_PATH + 1, 0, 0, &flag, 0, 0) != 0)  //判断驱动是否准备就绪
					{
						if((flag & FILE_READ_ONLY_VOLUME) == FILE_READ_ONLY_VOLUME)//必须是只读的
						{
							string volumnname(W2A(info));
							m_logicdriver.insert(std::make_pair(i, volumnname));
						}
					}
				}
			}

			allDisk = allDisk >> 1;
		}
		::LeaveCriticalSection(&m_cs);
	}
	return true;
}
bool CUSBHardWareDetector::RecheckLogicDriver()//更新U盘
{
	DWORD allDisk = ::GetLogicalDrives();  //返回一个32位整数，将他转换成二进制后，表示磁盘,最低位为A盘
	DWORD tpDisk = m_allDisk;
	m_allDisk = allDisk;
	if(allDisk > tpDisk)//说明有磁盘增加
	{
		allDisk = tpDisk ^ allDisk;//获得增加的磁盘
		if( allDisk != 0 )
		{
			USES_CONVERSION;
			for (int i = 0; i < 32 ; i ++)     // 遍历磁盘
			{
				if ((allDisk & 1) == 1)
				{
					UINT type = ::GetDriveTypeW(drvName[i].c_str());
					if ( type == DRIVE_CDROM )
					{
						DWORD flag;
						WCHAR info[MAX_PATH + 1] = {0};
						if( ::GetVolumeInformationW(drvName[i].c_str(), info, MAX_PATH + 1, 0, 0, &flag, 0, 0) != 0)  //判断驱动是否准备就绪
						{
							if((flag & FILE_READ_ONLY_VOLUME) == FILE_READ_ONLY_VOLUME)//必须是只读的
							{
								string volumnname(W2A(info));
								::EnterCriticalSection(&m_cs);
								m_logicdriver.insert(std::make_pair(i, volumnname));
								::LeaveCriticalSection(&m_cs);
							}
						}

					}
				}

				allDisk = allDisk >> 1;
			}
		}
	}
	else if(allDisk < tpDisk)//说明有磁盘减少
	{
		allDisk = tpDisk ^ allDisk;//获得增加的磁盘
		if( allDisk != 0 )
		{
			for (int i = 0; i < 32 ; i ++)     // 遍历磁盘
			{
				if ((allDisk & 1) == 1)
				{
					::EnterCriticalSection(&m_cs);
					std::map<int , string>::iterator ite = m_logicdriver.find(i);
					if(ite != m_logicdriver.end())
						m_logicdriver.erase(ite);
					::LeaveCriticalSection(&m_cs);
				}

				allDisk = allDisk >> 1;
			}
		}
	}
	if(allDisk == allDisk)
		return false;

	return true;
}
bool CUSBHardWareDetector::CheckLogicDriver(string& vname)//检查U盘
{
	char diskPath[5] = {0};
	std::map<int ,std::string>::iterator ite = m_logicdriver.begin();
	::EnterCriticalSection(&m_cs);
	for(;ite != m_logicdriver.end(); ite ++)
		if((*ite).second == vname)
		{
			::LeaveCriticalSection(&m_cs);
			return true;
		}

	::LeaveCriticalSection(&m_cs);
	return false;
}
//如果找到了支持的usb，那么返回支持的usb的数据，如果没有，返回NULL
USBKeyInfo* CUSBHardWareDetector::CheckSupportUSB(USBHardwareInfo& hardinfo)
{
	list<USBKeyInfo>* pUsb = CUSBControl::GetInstance()->GetUSBInfo();
	list<USBKeyInfo>::iterator ite = pUsb->begin();
	for(;ite != pUsb->end(); ite ++)
	{
		if((hardinfo.eig.ProductID == (*ite).hardware.eig.ProductID) && (hardinfo.eig.VendorID == (*ite).hardware.eig.VendorID))//检查VID和PID，如果相同
		{
			//支付盾目前的这个dll会导致无响应，目前去掉该部分处理
			// 暂时在这里需要对支付盾进行额外处理，后期改为dll
			// 该VID和PID不需要mid

			if((*ite).hardware.eig.hasMid == false && ((*ite).hardware.volumnname.size() <= 0))
			{
				return (&(*ite));
			}
			else if((*ite).hardware.eig.hasMid == false && ((*ite).hardware.volumnname.size() > 0))
			{
				//等1.5s后获得卷标
				Sleep(1500);//这个超时还需要再改变
				string vname = (*ite).hardware.volumnname;
				RecheckLogicDriver();//重新扫描增加的磁盘
				bool ret = CheckLogicDriver(vname);
				if(ret == true)
				{
					return (&(*ite));
				}
				else
					continue;
			}
			else //检测mid，检测
			{
				if(hardinfo.eig.hasMid == false)//第一次碰到检测一遍mid
				{
					if(CheckMid(hardinfo.eig, (*ite).hardware.eig))//根据资料直接找到了正确的mid
					{
						return (&(*ite));
					}
					else// 由于加载的原因或是其他原因，没找到mid，那么继续进行下一个查找
					{
						continue;
					}
				}
				else //上一次已经正确找到了mid,只是和上一个不一致
				{
					if(hardinfo.eig.m_midDword == (*ite).hardware.eig.m_midDword)
					{
						return (&(*ite));
					}
				}
			}
		}
	}
	return NULL;
}
// 检测mid,包含我们资料中的mid相关资料dataeig，根据该资料进行mid检测，返回的mid存储在acteig中，返回检测得到的mid值，检测到该mid为输入的mid值，那么直接返回true；
// 否则继续检测mid

typedef DWORD	(*GetNumberMIDFuc)();
typedef CHAR	(*GetStringMIDFuc)();
bool CUSBHardWareDetector::CheckMid(USBEigenvalue& acteig, USBEigenvalue& dataeig)
{
	acteig.hasMid = false;//开始检测失败

	if(dataeig.hasMid == false)
		return false;

	HMODULE hModule;
	hModule = LoadLibraryW(dataeig.midDLLName.c_str());
	if(hModule == NULL)
		return false;

	USES_CONVERSION;
	string fuc = W2A(dataeig.midFucName.c_str()); 
	GetNumberMIDFuc GetNumberMID = NULL;
	GetNumberMID = (GetNumberMIDFuc)::GetProcAddress(hModule, fuc.c_str());//获取函数
	if(GetNumberMID != NULL)
	{
		acteig.m_midDword = GetNumberMID();

		if(acteig.m_midDword == 0)// 如果没找到，mid值返回0
		{
			acteig.hasMid = true;			//成功获取到了mid
			if(acteig.m_midDword == dataeig.m_midDword)//如果mid一致，那么说明已经找到了设备
			{
				FreeLibrary(hModule);
				return true;
			}
		}
	}
	

	FreeLibrary(hModule);
	return false;
}