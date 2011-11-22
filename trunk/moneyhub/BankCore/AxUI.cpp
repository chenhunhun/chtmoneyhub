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
#include "../Utils/HardwareID/genhwid.h" // 读取硬件ID
#include "../Utils/SN/SNManager.h" // 读取SN
#include "../Utils/SecurityCache/comm.h"
#include <vector>
#include <algorithm>
using namespace std;

extern HWND g_hMainFrame;
#define  MH_SENDSTARTDELAYEVENT (0xfe02)

CAxUI::CAxUI() :m_hTag(NULL), m_pbilldata(NULL)
{
	Create(HWND_MESSAGE); // call WM_CREATE
}


//////////////////////////////////////////////////////////////////////////

int CAxUI::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CExternalDispatchImpl::m_hAxui = m_hWnd;
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
	//CUserBehavior::GetInstance ()->Action_SendDataToServerWhenExit (); // 发送数据到服务器端//防止服务器处理慢导致退出缓慢

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

LRESULT CAxUI::UserAutoLoad(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	string strUserID, strMail, strStoken;
	CBankData::GetInstance()->ReadNeedAutoLoadUser(strUserID, strMail, strStoken);

	string strParam = "xml=" + strUserID + MY_PARAM_END_TAG;


	string strHWID = GenHWID2();
	wstring wstrHWID = CA2W(strHWID.c_str());
	string strSN = CSNManager::GetInstance()->GetSN();

	strParam += strSN;
	strParam += MY_PARAM_END_TAG;

	strParam += strHWID;
	strParam += MY_PARAM_END_TAG;

	
	strParam += strStoken;
	strParam += MY_PARAM_END_TAG;

	CDownloadThread downloadThread;
	downloadThread.DownLoadInit(wstrHWID.c_str(), L"http://moneyhub.ft.com/server/auto_log_on.php", (LPSTR)strParam.c_str());
	char chTemp[1024] = {0};
	DWORD dwRead = 0;
	if (ERR_SUCCESS != downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"自动登陆失败");

		// 登录失败，发送一个空的账号
		::SendMessage(g_hMainFrame, WM_SETTEXT, WM_FINIHS_AUTO_LOAD, (LPARAM)L"");

		// 设置登录失败的状态
		return 0;
	}

	// 取出返回结果，KEK，EDEK
	string strRead = chTemp;
	string strSub = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

#define MY_AUTO_LAOD_SUCC			 "61"
	int nParam = -1;
	if (strSub.find(MY_AUTO_LAOD_SUCC) != string::npos)
	{
		// 读取KEK
		string strTag = MY_PARAM_END_TAG;
		strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
		string strKEK = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

		// 读取EDEK
		strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
		string strEDEK = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

		// 将KEK EDEKl转换成实际值
		char kekVal[33] = {0};
		char edekVal[33] = {0};
		char keyVal[33] = {0};
		int nBack = 0;
		FormatDecVal(strKEK.c_str(), kekVal, nBack);
		FormatDecVal(strEDEK.c_str(), edekVal, nBack);

		UserEdekUnPack((unsigned char*)edekVal, 32, (unsigned char*)kekVal, (unsigned char*)keyVal);

		// 使用USERID命名数据库
		string strData = strUserID;
		strData += ".dat";

		// 将该用户的库设置成当前数据库,并用密码打开
		CBankData::GetInstance()->SetCurrentUserDB((LPSTR)strData.c_str(), keyVal, 32);

		// 更改当前用户信息
		CBankData::GetInstance()->m_CurUserInfo.strstoken = strStoken;
		CBankData::GetInstance()->m_CurUserInfo.strmail = strMail;
		CBankData::GetInstance()->m_CurUserInfo.struserid = strUserID;
	}
	else
	{
		// 清空MAIL，发送消息时表示登录失败
		strMail.clear();
	}


	// 通知UI更新当前用户信息（同步在UI中要使用这些值）
	string strMesParam = strStoken + MY_PARAM_END_TAG;
	strMesParam += strMail;
	strMesParam += MY_PARAM_END_TAG;
	strMesParam += strUserID;
	strMesParam += MY_PARAM_END_TAG;

	::SendMessage(g_hMainFrame, WM_SETTEXT, WM_UPDATE_USER_STATUS, (LPARAM)strMesParam.c_str());

	/*CString cstrMail = CA2W(strMail.c_str());
	::SendMessage(g_hMainFrame, WM_SETTEXT, WM_FINIHS_AUTO_LOAD, (LPARAM)(LPCTSTR)cstrMail);*/
	return 0;
}

LRESULT CAxUI::OnLoadUserQuit(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	string strPw;
	int nLen = 0;

// 发布版本，用以下密码解密
#ifdef OFFICIAL_VERSION
	strPw = "NCrFT2RIeD0NY2wHOI8W";
	nLen = 20;
#endif
	// 将数据库转移至访客数据库
	CBankData::GetInstance()->SetCurrentUserDB("Guest.dat", (LPSTR)strPw.c_str(), nLen);

	// 检验当前用户是否是自动登录用户，如果是，则清空stoken，否则不更新
	string strUserID = CBankData::GetInstance()->m_CurUserInfo.struserid;	
	string strSQL = "select userid from datUserInfo where userid = '";
	strSQL += strUserID;
	strSQL += "' and autoload = 1";
	string strQueryVal = CBankData::GetInstance()->QuerySQL(strSQL, "DataDB");
	if (strQueryVal.find(strUserID) != string::npos)
	{
		strSQL = "update datUserInfo set autoload = 0, stoken = '' where userid = '";
		strSQL += strUserID;
		strSQL += "'";
		CBankData::GetInstance()->QuerySQL(strSQL, "DataDB");
	}

	// 更改当前用户信息
	CBankData::GetInstance()->m_CurUserInfo.strstoken.clear();
	CBankData::GetInstance()->m_CurUserInfo.strmail.clear();
	CBankData::GetInstance()->m_CurUserInfo.struserid = "Guest";



	// 通知UI更新当前用户信息（同步在UI中要使用这些值）
	string strMesParam = "Guest";
	strMesParam += MY_PARAM_END_TAG;
	strMesParam += MY_PARAM_END_TAG;
	strMesParam += MY_PARAM_END_TAG;
	//::PostMessage(g_hMainFrame, WM_SETTEXT, WM_NOTICE_UI_UPDATE_USERINFO, (LPARAM)strMesParam.c_str());

	::SendMessage(g_hMainFrame, WM_SETTEXT, WM_UPDATE_USER_STATUS, (LPARAM)L"");
	return 0;
}

//LRESULT CAxUI::OnShowUserDlg(UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	if(CExternalDispatchImpl::m_hFrame[0])
//		::PostMessageW(CExternalDispatchImpl::m_hFrame[0], WM_AX_SHOW_USER_DLG, 0, lParam);
//	OutputDebugStringA("send message ksksk");
//	return 0;
//}

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
//} wParam, LPARAM lParam)

LRESULT CAxUI::OnGetAllBill(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char *pInfo = (char*)lParam;
	if(pInfo == NULL)
		return 0;

	m_pbilldata = pInfo;
	return 0;
}

LRESULT CAxUI::OnSendToBillPage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pbilldata != NULL)
		SetTimer(MH_SENDSTARTDELAYEVENT, 1 * 1000 ,NULL);	
	return 0;
}

LRESULT CAxUI::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(wParam == MH_SENDSTARTDELAYEVENT)
	{
		KillTimer(MH_SENDSTARTDELAYEVENT);
		::PostMessageW(CExternalDispatchImpl::m_hFrame[2], WM_AX_GET_ALL_BILL, NULL, (LPARAM)&m_pbilldata);
	}
	return 0;
}


































