/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  AxUI.cpp
 *      说明：  控件界面
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#include "stdafx.h"
#include "AxUI.h"
#include "../Security/BankProtector/export.h"
#include "../BankUI/Util/CleanHistory.h"
#include "ExternalDispatchImpl.h"
#include "..\BankData\BankData.h"
#include "../Utils/UserBehavior/UserBehavior.h"
#include "BankData/USBMonitor.h"
#include "../Utils/ListManager/ListManager.h"
#include <vector>
#include <algorithm>
using namespace std;

CAxUI::CAxUI() :m_hTag(NULL)
{
	Create(HWND_MESSAGE); // call WM_CREATE
}


//////////////////////////////////////////////////////////////////////////

int CAxUI::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DWORD dwThreadID;
	CloseHandle(CreateThread(NULL, 0, _threadInit, this, 0, &dwThreadID));
	return 0;
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI ThreadProcCreateAxControl(LPVOID lpParameter);

DWORD WINAPI CAxUI::_threadInit(LPVOID lp)
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"AxUI开始初始化");

	CAxUI* pThis = (CAxUI*)lp;
	if(lp == NULL)
		return 0;

	while (pThis->m_hTag == NULL)
	{
		pThis->m_hTag = CExternalDispatchImpl::m_hFrame[0];
		if (NULL != pThis->m_hTag)
		{

			// 先初始化USB监控
			CUSBMonitor::GetInstance()->InitParam();
			pThis->CheckDownloadFile();

			//启动usb监控线程
			CUSBMonitor::GetInstance()->StartUSBMonitor();
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"AxUI启动了USB监控线程");

			break;
		}
		Sleep(200);
	}
	return 0;
}

LRESULT CAxUI::OnCreateNewPage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"AxUI 创建了新页");
	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(NULL, 0, ThreadProcCreateAxControl, (LPVOID)lParam, NULL, &dwThreadID);
	::CloseHandle(hThread);
	return 0;
}

LRESULT CAxUI::OnExit(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CleanHistory();
	CUserBehavior::GetInstance ()->Action_SendDataToServerWhenExit (); // 发送数据到服务器端

	
	//CBankData::GetInstance()->ExitWriteCurUserTpFile2Db();

	::TerminateProcess(::GetCurrentProcess(), 0);

	return 0;
}

void CAxUI::CheckDownloadFile(void)
{
	CBkInfoDownloadManager *pTemp = CBkInfoDownloadManager::GetInstance ();
	pTemp->SetProgressNotifyHwnd (CExternalDispatchImpl::m_hFrame[0]);
	if (NULL != pTemp)
		pTemp->FinishDLBreakFile (); // 继续下载用户下载未完成的银行控件
}
//取消收藏
LRESULT CAxUI::OnCancelAddFav(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(CExternalDispatchImpl::m_hFrame[0])
		::PostMessageW(CExternalDispatchImpl::m_hFrame[0], WM_AX_FRAME_DELETEFAV, 0, lParam);

	LONG tlfid = lParam;
	char tfid[6] = {0};

	memcpy(tfid, (void*)&tlfid, 4);
	USES_CONVERSION;
	std::wstring appId(A2W(tfid));
	vector<wstring>::iterator ite = find( uacPopVec.begin() , uacPopVec.end() , appId) ;
	if(ite != uacPopVec.end())
		uacPopVec.erase(ite);
	return 0;
}

//LRESULT CAxUI::OnDownLoadBankInfo (UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	std::string strTp = (LPSTR)lParam;
//
//	if (NULL == m_pBkDownloadManager)
//	{
//		HWND hwnd = (HWND)wParam;
//		CString cstrT;
//		cstrT.Format (L"accept hwnd = %d", wParam);
//		
//		m_pBkDownloadManager = new CBkInfoDownloadManager();
//	}
//
//	// 检验是否已经安装
//	if (m_pBkDownloadManager->IsSetupAlready ((char*)strTp.c_str ()))
//		return S_FALSE; // 如果安装了就不下载
//
//	// 执行下载
//	m_pBkDownloadManager->DownloadBankCtrl ((char*)strTp.c_str ());
//	return S_OK;
//}


//LRESULT CAxUI::OnUSBChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	// 转换参数
//	DWORD mid = (DWORD)wParam;
//	LONG fid = (LONG)lParam;
//	int nVid = fid & 0xffffffff; // 低字节
//	int nPid = fid >> 32; // 高字节
//
//	CBankData* pBankData = CBankData::GetInstance();
//
//	// 读取到USB在数据库中保存的记录
//	USBRECORD usbRecord;
//	if (!pBankData->GetAUSBRecord (nVid, nPid, mid, usbRecord))
//		return S_FALSE;
//
//	if (_SETUP_FINISH == usbRecord.status)
//		return S_OK;
//
//	if (_DOWNLOAD_FINISH == usbRecord.status)// 如果已经下载好了，进行安装
//	{
//		STARTUPINFO si;	
//		PROCESS_INFORMATION pi;	
//		ZeroMemory( &pi, sizeof(pi) );	
//		ZeroMemory( &si, sizeof(si) );	
//		si.cb = sizeof(si);	
//		// 进行安装
//		if(CreateProcessW(NULL, NULL/*(LPWSTR)path.c_str()*/, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) // 路径还没有确定
//		{		
//			CloseHandle( pi.hProcess );		
//			CloseHandle( pi.hThread );		
//		}
//	} 
//
//	if (usbRecord.status >= 0 && usbRecord.status < _DOWNLOAD_FINISH) // 如果没有下载或下载未完成，创建下载
//	{
//		/*if (NULL == m_pBkDownloadManager)
//		{
//			if(CExternalDispatchImpl::m_hFrame[0])
//				m_pBkDownloadManager = new CBkInfoDownloadManager(CExternalDispatchImpl::m_hFrame[0]);
//		}*/
//		CBkInfoDownloadManager *pTemp = CBkInfoDownloadManager::GetInstance ();
//
//		if (NULL != pTemp)
//		{
//
//			std::string strXml = usbRecord.xml;
//
//			// 执行下载
//			pTemp->DownloadBankCtrl (usbRecord.fav, NULL, NULL, (LPVOID)strXml.c_str (), strXml.size ());
//		}
//	}
//
//	return 0;
//}