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
#include "..//Utils/Config//HostConfig.h"
#include "..\BankUI\UIControl\CoolMessageBox.h" // mhMessageBox
#include <vector>
#include <algorithm>
#include <atlcomtime.h>
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

			// 检验系统时间
			pThis->CheckSystemTime();

			//启动usb监控线程
			CUSBMonitor::GetInstance()->StartUSBMonitor();
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_INIT, L"AxUI启动了USB监控线程");

			break;
		}
		Sleep(200);
	}
	return 0;
}

void CAxUI::CheckSystemTime(void)
{
	wstring wstrHWID = CA2W(GenHWID2().c_str());
	CDownloadThread downloadThread;
	downloadThread.DownLoadInit(wstrHWID.c_str(), (CHostContainer::GetInstance()->GetHostName(kPUserServerTime)).c_str(), (LPSTR)"");
	char chTemp[1024] = {0};
	DWORD dwRead = 0;
	if (ERR_SUCCESS != downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"获取服务器时间失败");
		return;
	}
	else
	{
		string strRead = chTemp;
		if(strRead.size() <= 0)
			return;
		// 读取KEK
		string strTag = MY_PARAM_END_TAG;
		strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
		string strServerTime = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));
		if (strServerTime.length() <= 0)
			return;

		COleDateTime oleTServer;
		oleTServer.ParseDateTime(CA2W(strServerTime.c_str()));
		COleDateTime oleTCurrent = COleDateTime::GetCurrentTime();
		COleDateTimeSpan oleTSpan1 = oleTServer - oleTCurrent;
		COleDateTimeSpan oleTSpan2 = oleTCurrent - oleTServer;
		if(oleTSpan1.GetTotalMinutes() > 30 || oleTSpan2.GetTotalMinutes() > 30)
		{
			::mhMessageBox(NULL, L"检测到您的系统时间与标准时间有较大误差，这将影响的同步功能的正常使用，推荐您进行修改!", L"财金汇", MB_OK | MB_ICONINFORMATION);
		}
	}
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

LRESULT CAxUI::OnResendVerifyMail(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 重发邮件
	string strParam = "xml=" + CBankData::GetInstance()->m_CurUserInfo.struserid + MY_PARAM_END_TAG;

	string strHWID = GenHWID2();
	wstring wstrHWID = CA2W(strHWID.c_str());
	string strSN = CSNManager::GetInstance()->GetSN();

	/*strParam += strSN;
	strParam += MY_PARAM_END_TAG;

	strParam += strHWID;
	strParam += MY_PARAM_END_TAG;*/
	
	strParam += CBankData::GetInstance()->m_CurUserInfo.strstoken;
	strParam += MY_PARAM_END_TAG;

	CDownloadThread downloadThread;
	downloadThread.DownLoadInit(wstrHWID.c_str(), (CHostContainer::GetInstance()->GetHostName(kPSendMailVerify)).c_str(), (LPSTR)strParam.c_str());
	char chTemp[1024] = {0};
	DWORD dwRead = 0;
	if (ERR_SUCCESS != downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead))
	{
		return S_FALSE;
	}

	string strBack = chTemp;
#define RESEND_VERIFY_MAIL "48" // 邮件重发成功
	if(strBack.find(RESEND_VERIFY_MAIL) != string::npos)
	{
		return S_OK;
	}
	else if (strBack.find(MY_MAIL_VERIFY_SUCC) != string::npos)
	{
		// 用户已经论证了邮件
		return atoi(MY_MAIL_VERIFY_SUCC);
	}
	
	return S_FALSE;
}

LRESULT CAxUI::UserAutoLoad(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	string strUserID, strMail, strEtk, strMailVerify;
	USERSTATUS emStatus = emNotLoad;
	CBankData::GetInstance()->ReadNeedAutoLoadUser(strUserID, strMail, strEtk);

	string strParam = "xml=" + strUserID + MY_PARAM_END_TAG;


	string strHWID = GenHWID2();
	wstring wstrHWID = CA2W(strHWID.c_str());
	string strSN = CSNManager::GetInstance()->GetSN();

	/*strParam += strSN;
	strParam += MY_PARAM_END_TAG;

	strParam += strHWID;
	strParam += MY_PARAM_END_TAG;*/

	string strStoken;
	int nStatus = 0;
#define ERR_BUT_NOT_SHOW 10000 // 出错，但不用在界面上弹出显示

	if (strEtk.length() > 0)
	{
		// 将Etk解密成stoken
		char chTek[33] = {0};
		string strTekMake = strSN + strHWID;
		UserDataASH256((unsigned char*)strTekMake.c_str(), strTekMake.length(), (unsigned char*)chTek);

		int nLen = (strEtk.length() +1) / 2;

		char* pStoken = new char[nLen + 1];
		char* pUEtk = new char[nLen + 1];
		memset(pStoken, 0, nLen + 1);
		memset(pUEtk, 0, nLen + 1);

		FormatDecVal(strEtk.c_str(), (char*)pUEtk, nLen);

		nLen = UserDataASE256D((unsigned char *)pUEtk, nLen, (unsigned char *)chTek, (unsigned char*)pStoken);

		strStoken = pStoken;
		delete []pUEtk;
		delete []pStoken;

#define SERVER_STOKEN_LENGTH 84 // 服务器端stoken的长度
		if (nLen <= 0 || strStoken.length() != SERVER_STOKEN_LENGTH)
		{
#define DENCRYPT_PWD_ERR 108
			nStatus = DENCRYPT_PWD_ERR;
		}
	}

	strParam += strStoken;
	strParam += MY_PARAM_END_TAG;


	CDownloadThread downloadThread;
	downloadThread.DownLoadInit(wstrHWID.c_str(), (CHostContainer::GetInstance()->GetHostName(kPAutoLogon)).c_str(), (LPSTR)strParam.c_str());
	char chTemp[1024] = {0};
	DWORD dwRead = 0;
	int nBack = downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead);
	if (ERR_SUCCESS != nBack || 0 == dwRead)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"自动登陆失败");

		// 设置成不提示状态
		if (0 == nStatus)
			nStatus = ERR_BUT_NOT_SHOW;
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

		// 读取邮件认证信息
		strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
		strMailVerify = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

		// 将KEK EDEKl转换成实际值
		char kekVal[33] = {0};
		char edekVal[33] = {0};
		char keyVal[33] = {0};
		int nBack = 0;
		FormatDecVal(strKEK.c_str(), kekVal, nBack);
		FormatDecVal(strEDEK.c_str(), edekVal, nBack);

		UserDataASE256D((unsigned char*)edekVal, 32, (unsigned char*)kekVal, (unsigned char*)keyVal);

		// 使用USERID命名数据库
		string strData = strUserID;
		strData += ".dat";

		// 将该用户的库设置成当前数据库,并用密码打开
		CBankData::GetInstance()->SetCurrentUserDB((LPSTR)strData.c_str(), keyVal, 32);

		// 通知UI更新当前用户库
		string strSendUI = strData;
		strSendUI += MY_PARAM_END_TAG;
		string strPWD;
		FormatHEXString(keyVal, 32, strPWD);
		strSendUI += strPWD;
		strSendUI += MY_PARAM_END_TAG;
		::SendMessage(g_hMainFrame, WM_SETTEXT, WM_NOTIFYUI_UPDATE_USER_DB, (LPARAM)strSendUI.c_str());


		// 更改当前用户信息
		CBankData::GetInstance()->m_CurUserInfo.strstoken = strStoken;
		CBankData::GetInstance()->m_CurUserInfo.strmail = strMail;
		CBankData::GetInstance()->m_CurUserInfo.struserid = strUserID;
		CBankData::GetInstance()->m_CurUserInfo.emUserStatus = emUserNet;
		emStatus = emUserNet;
	}
	else
	{
		// 清空MAIL，发送消息时表示登录失败
#define MAIL_OR_PWD_ERR 106 // 邮箱或密码错误
		if (0 == nStatus)
			nStatus = MAIL_OR_PWD_ERR;
	}

	if (nStatus > 0 || strUserID.length() <= 0)
	{
		// 自动登录失败，清空相数据
		strUserID = MONHUB_GUEST_USERID;
		strStoken.c_str();
		strMail.c_str();
	}

	// 更新当前用户信息及界面显示信息
	UpdateUserInfo(strStoken.c_str(), strMail.c_str(), strUserID.c_str(), (int)emStatus, false, (char*)strMailVerify.c_str());

	// 显示出错信息
	if (nStatus > 0)
	{
		::PostMessage(g_hMainFrame, WM_SHOW_USER_DLG, nStatus, MY_TAG_LOAD_DLG);
	}

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

		// 通知UI更新当前用户库
		string strSendUI = "Guest.dat";
		strSendUI += MY_PARAM_END_TAG;
		strSendUI += strPw;
		strSendUI += MY_PARAM_END_TAG;
		::SendMessage(g_hMainFrame, WM_SETTEXT, WM_NOTIFYUI_UPDATE_USER_DB, (LPARAM)strSendUI.c_str());

	// 检验当前用户是否是自动登录用户，如果是，则清空etk，将自动登录的状态去掉
	string strUserID = CBankData::GetInstance()->m_CurUserInfo.struserid;	
	string strSQL = "select userid from datUserInfo where userid = '";
	strSQL += strUserID;
	strSQL += "' and autoload = 1";
	string strQueryVal = CBankData::GetInstance()->QuerySQL(strSQL, "DataDB");
	if (strQueryVal.find(strUserID) != string::npos)
	{
		strSQL = "update datUserInfo set autoload = 0, etk = '' where userid = '";
		strSQL += strUserID;
		strSQL += "'";
		CBankData::GetInstance()->QuerySQL(strSQL, "DataDB");
	}

	// 更新当前用户信息及界面显示信息
	UpdateUserInfo("", "", MONHUB_GUEST_USERID, emNotLoad, false);

	return 0;
}

void CAxUI::UpdateUserInfo(const char* pStoken, const char* pMail, const char* pUserId, const int nUserStatus, bool bChangeMail, char* pVerifyMail)
{
	ATLASSERT(NULL != pStoken && NULL != pMail && NULL != pUserId);
	if (NULL == pStoken || NULL == pMail || NULL == pUserId)
		return;

	// 更改内核用户信息
	CBankData::GetInstance()->m_CurUserInfo.strstoken = pStoken;
	CBankData::GetInstance()->m_CurUserInfo.strmail = pMail;
	CBankData::GetInstance()->m_CurUserInfo.struserid = pUserId;
	CBankData::GetInstance()->m_CurUserInfo.emUserStatus = (USERSTATUS)nUserStatus;
	if (emNotLoad == nUserStatus) // 如果用户是退出
		CBankData::GetInstance()->m_CurUserInfo.emChangeStatus = emUserNoChange;


	// 更改邮箱时不必为JS添加这些参数
	if (!bChangeMail)
	{
		// 为JS添加参数
		CExternalDispatchImpl::AddJSParam("ReloadStart", "1");
		CExternalDispatchImpl::AddJSParam("ReloadTools", "1");
		CExternalDispatchImpl::AddJSParam("ReloadFinance", "1");
		CExternalDispatchImpl::AddJSParam("ReloadSet", "1");
		CExternalDispatchImpl::AddJSParam("ReloadReport", "1");
		CExternalDispatchImpl::AddJSParam("ReloadProduct", "1");
	}

	char userStatus[3] = {0};
	// 通知UI更新当前用户信息（同步在UI中要使用这些值）
	string strMesParam = pStoken;
	strMesParam += MY_PARAM_END_TAG;
	strMesParam += pMail;
	strMesParam += MY_PARAM_END_TAG;
	strMesParam += pUserId;
	strMesParam += MY_PARAM_END_TAG;
	if (NULL != pVerifyMail)
		strMesParam += pVerifyMail;
	strMesParam += MY_PARAM_END_TAG;
	strMesParam += itoa(nUserStatus, userStatus, 10);
	strMesParam += MY_PARAM_END_TAG;

	::SendMessage(g_hMainFrame, WM_SETTEXT, WM_UPDATE_USER_STATUS, (LPARAM)strMesParam.c_str());
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


































