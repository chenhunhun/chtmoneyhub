#include "stdafx.h"
#include "USBMonitor.h"
#include "BkInfoDownload.h"
#include "../stdafx.h"
#include "../../BankData/BankData.h"
#include "../../USBControl/USBHardWareDetector.h"
#include "../../USBControl/USBSoftWareDetector.h"
#include "../../BankUI/UIControl/CoolMessageBox.h"

bool CBankDownInterface::CheckServerXmlFile(bool bUpdate,  USBRECORD& ur)
{
	USBKeyInfo* pusb = CUSBHardWareDetector::GetInstance()->CheckUSBHardWare(ur.vid ,ur.pid, ur.mid);//从内存文件中获得相关信息
	if(!pusb)
		return false;
	//存在两次提示的问题，根据版本可以判断是否再次提示
	if(bUpdate)
	{
		wstring msg;
		msg = L"检测到" + pusb->hardware.finaninstitution + L"的USBKEY需要更新，是否进行更新？";
		HWND hFrame = ::FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
		if(IDYES != MessageBox( hFrame, msg.c_str(), L"财金汇提示",MB_YESNO))
			return false;
	}


	return true;
}

bool CBankDownInterface::USBFinalTest(int vid, int pid, DWORD mid, bool& bSetup)
{
	USBKeyInfo* pusb = CUSBHardWareDetector::GetInstance()->CheckUSBHardWare(vid ,pid, mid);//从内存文件中获得相关信息

	if(!pusb)
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, L"USBFinalTest pusb = NULL");
		return true;
	}

	bool bInstall = CUSBSoftWareDetector::GetInstance()->CheckUSBSoftWare(pusb->software, false);//检测版本是否正常
	if(bInstall)
	{			
		CUSBSoftWareDetector::GetInstance()->CheckUSBSoftWare(pusb->software, true);
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, L"USBFinalTest开始修复");
		
		if(pusb->hardware.financeid == "e001")
		{
			bSetup = true;
			CBankData::GetInstance ()->UpdateUSB (pusb->hardware.eig.VendorID, pusb->hardware.eig.ProductID, 0, 200);
			wstring alipaymsg;
			alipaymsg = L"请拔出支付盾，然后重新插入，以完成安装过程";
			HWND hFrame = ::FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
			MessageBox( hFrame, alipaymsg.c_str(), L"财金汇提示",MB_OK );
		}

		wstring msg;
		msg = pusb->hardware.goodsname + L"软件安装正常，可以使用";
		
		mhShowMessage( GetActiveWindow(), msg.c_str());
			return true;
	}
	else
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, L"USBFinalTestAxUI安装失败");
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, pusb->hardware.goodsname.c_str());
		wstring msg;
		msg = pusb->hardware.goodsname + L"软件安装失败，是否重试？";
		HWND hFrame = ::FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
		if(IDYES == MessageBox( hFrame, msg.c_str(), L"财金汇提示",MB_YESNO))
			return false;
		else
			return true;
	}
}

CUSBMonitor* CUSBMonitor::m_Instance = NULL;

CUSBMonitor* CUSBMonitor::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CUSBMonitor();
	return m_Instance;
}

CUSBMonitor::CUSBMonitor()
{
}
CUSBMonitor::~CUSBMonitor()
{
}
void CUSBMonitor::InitParam()
{
	//申请接口对象，完成设置
	IBankDownInterface* pBdi = new CBankDownInterface;
	CUSBCheckor::GetInstance()->SetUSBTask(this);
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, L"USBMonitor::InitParam");
	CBkInfoDownloadManager::GetInstance()->SetCheckFun(pBdi);
}

bool CUSBMonitor::StartUSBMonitor()
{
	// InitParam();
	CUSBControl::GetInstance()->BeginUSBControl();
	return true;
}

bool CUSBMonitor::AddUSBTask(int vid, int pid, DWORD mid)
{
	CBankData* pBankData = CBankData::GetInstance();
	

	USBRECORD usbrecord;
	// 以前安装过
	if(pBankData->GetAUSBRecord(vid, pid, mid, usbrecord))
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_USBKEY_CHECK, CRecordProgram::GetInstance()->GetRecordInfo(L"增加usb下载任务vid %d pid %d",vid,pid));
		CBkInfoDownloadManager::GetInstance()->MyBankUsbKeyDownload(&usbrecord);//增加usb下载任务
		
	}
	else
		return false;

	return true;
}

