#include "stdafx.h"
#include "USBCheckor.h"
#include "USBHardWareDetector.h"
#include "USBSoftWareDetector.h"
#include "..\BankData\BankData.h"
#include <Message.h>

CUSBCheckor* CUSBCheckor::m_Instance = NULL;

CUSBCheckor* CUSBCheckor::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CUSBCheckor();
	return m_Instance;
}

CUSBCheckor::CUSBCheckor():m_task(NULL)
{
}
CUSBCheckor::~CUSBCheckor()
{
}
// 检测
bool CUSBCheckor::CheckUSBInfo(USBHardwareInfo& hardinfo)
{
	CRecordProgram* pRecord = CRecordProgram::GetInstance();
	USBKeyInfo* pusb = CUSBHardWareDetector::GetInstance()->CheckUSBHardWare(hardinfo);
	pRecord->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"CheckUSBHardWare");
	if(pusb == NULL)
	{
		pRecord->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"CheckUSBHardWare Pusb = NULL");
		return false;
	}
	
	//bool bAlipay = true;
	//// 支付盾单独加，暂时先不做
	//if(pusb->hardware.financeid == "e001")//对支付宝要进行特殊处理
	//{
	//	//说明支付盾安装正确
	//	bAlipay = hardinfo.eig.hasMid;//支付宝不正常的话，bAlipay = false;		
	//}
	
	DWORD mid;
	// 检测升级
	if(pusb->hardware.eig.hasMid == true)
		mid = pusb->hardware.eig.m_midDword;
	else
		mid = 0;

	CBankData* pBankData = CBankData::GetInstance();
	// 先检测数据库里有没有，来判JSP获取一个事件———内核断我们是否安装过
	bool isExist = pBankData->IsUsbExist(pusb->hardware.eig.VendorID, pusb->hardware.eig.ProductID ,mid);
	// 软件状态检测	
	// 检测安装状态,如果已经安装了，那么根据是否升级来处理是否修复
	bool bInstall = CUSBSoftWareDetector::GetInstance()->CheckUSBSoftWare(pusb->software, false);
	pRecord->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"CheckUSBHardWare no repair");

	USES_CONVERSION;
	if(!bInstall)//if(!bInstall && bAlipay)	//没安装或者是关键位置的配置信息被损坏
	{
		if(RemindUser(pusb))//这个位置要提示用户是否安装
			return true;

		if(isExist)//我们安装过，但被损坏了,先删除信息
		{
			USBRECORD usbRecord;
			if (pBankData->GetAUSBRecord (pusb->hardware.eig.VendorID, pusb->hardware.eig.ProductID, mid, usbRecord))
			{
				if (usbRecord.status == _SETUP_FINISH)
				{
					pBankData->DeleteUSB(pusb->hardware.eig.VendorID, pusb->hardware.eig.ProductID ,mid);//上次安装完了才删除
					//点了确定
					string version = DEFAULT_INSTALL_VERSION;//"暂时放入安装版本1.0.0.0",肯定要进行安装的信息
					AddRecord(pusb, version);
				}
			}			
		}
		else
		{
			//点了确定
			string version = DEFAULT_INSTALL_VERSION;//"暂时放入安装版本1.0.0.0",肯定要进行安装的信息
			AddRecord(pusb, version);
		}
	}

	else	//安装过
	{		
		// 再修复一遍
		bool rRepair = CUSBSoftWareDetector::GetInstance()->CheckUSBSoftWare(pusb->software, true);
		if(!rRepair)//修复失败，需要重新安装
		{
			if(RemindUser(pusb))//这个位置要提示用户是否安装
				return true;

			if(isExist)
			{
				//这里应该区别对待，如果里面的状态为200，才应该该为0，否则应该不变，暂时没做
				USBRECORD usbRecord;
				if (pBankData->GetAUSBRecord (pusb->hardware.eig.VendorID, pusb->hardware.eig.ProductID, mid, usbRecord))
				{
					if (usbRecord.status == _SETUP_FINISH)
					{
						pBankData->DeleteUSB(pusb->hardware.eig.VendorID, pusb->hardware.eig.ProductID ,mid);//上次安装完了才删除
						//点了确定
						string version = DEFAULT_INSTALL_VERSION;//"暂时放入安装版本1.0.0.0",肯定要进行安装的信息
						AddRecord(pusb, version);
					}
					else
						pRecord->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"CheckUSBHardWare  state isn't _SETUP_FINISH");
				}

			}
			else //如果数据库里没有，那么
			{
				string version = DEFAULT_INSTALL_VERSION;// 安装
				AddRecord(pusb, version);
			}
		}
		else
		{
			//这里相当于用户点了ok键
			if(pusb->software.bNeedUpdate)
			{
				// 如果存在，并且正常运行，那我们不管，继续执行；只有不存在，我们记录下该版本,以后有新版本我们再更新
				if(!isExist)
				{
					
					pRecord->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, 
						pRecord->GetRecordInfo(L"CheckUSBHardWare  USB数据更新:%d__%d", pusb->hardware.eig.VendorID,pusb->hardware.eig.ProductID));

					string version = DEFAULT_BEGIN_VERSION;
					AddRecord(pusb, version, _SETUP_FINISH);//更新安装
				}
			}// 还是要进行银行控件下载任务的检测
			//else
			//{
				//wstring msg = pusb->hardware.finaninstitution + L" USBKey可以正常使用!";
				//MessageBox(NULL, msg.c_str(), L"财金汇", MB_OK);
				//return true;
			//}


		}		
	}

	// 
	// 执行到这里，说明必然要做增加收藏和usbkey软件的相应安装
	// 添加收藏

	if(m_task)
	{
		pRecord->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, 
			pRecord->GetRecordInfo(L"CheckUSBHardWare  执行下载USB任务:%d__%d", pusb->hardware.eig.VendorID,pusb->hardware.eig.ProductID));

		// 执行下载，到服务器端下载USBKEY的安装程序
		m_task->AddUSBTask(pusb->hardware.eig.VendorID, pusb->hardware.eig.ProductID, mid);
	}
	else
		pRecord->FeedbackError(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"m_task = NULL");

	return true;
}
// 点击确定返回false，取消返回true
bool CUSBCheckor::RemindUser(USBKeyInfo* pusb)
{
	wstring msg;
	msg = L"检测到" + (pusb->hardware.finaninstitution) + L"的USBKEY，需要安装相关软件，是否安装？";
	HWND hFrame = ::FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	if(IDYES != MessageBox( hFrame, msg.c_str(), L"财金汇用户提示",MB_YESNO))
		return true;
	return false;
}

// 如果返回失败，说明我们自己目录下的下载失败了，那么需要调用备用下载地址，即银行的下载地址下载usbkey的驱动。
bool CUSBCheckor::AddRecord(USBKeyInfo* pusb, string ver, int status)
{
	// 
	CBankData* pBankData = CBankData::GetInstance();
	USES_CONVERSION;

	CHAR sys[256] = { 0 };
	switch(m_osType)
	{
	case sWinXP:
		sprintf_s(sys, 256, "WinXP%d",m_osArchi);
		break;
	case sWinVista:
		sprintf_s(sys, 256, "WinVista%d",m_osArchi);
		break;

	case sWin7:
		sprintf_s(sys, 256, "Win7%d",m_osArchi);
		break;
	default:
		sprintf_s(sys, 256, "Normal");
		break;
	}

	// 向服务器端获得USBkey的版本信息
	char cxml[2560] = {0};
	sprintf_s(cxml, 2560, XMLUSBINFO, (char*)pusb->hardware.financeid.c_str(),  W2A(pusb->hardware.inmodel.c_str()), (char*)sys, ver.c_str());
	string xml(cxml);	

	DWORD mid;
	// 检测升级
	if(pusb->hardware.eig.hasMid == true)
		mid = pusb->hardware.eig.m_midDword;
	else
		mid = 0;

	pBankData->AddUSB(pusb->hardware.eig.VendorID, pusb->hardware.eig.ProductID, mid, 
		pusb->hardware.financeid, xml, W2A(pusb->software.backurl.c_str()), ver, status);

	return true;
}