#include "stdafx.h"
#include "BkInfoDownload.h"
#include "../Utils/CryptHash/base64.h"
#include "../ThirdParty/tinyxml/tinyxml.h"
#include "../Utils/Config/HostConfig.h"
#include "../Utils/HardwareID/genhwid.h"
#include "../BankUI/Util/SecurityCheck.h"
#include "../Utils/FavBankOperator/FavBankOperator.h"
#include "../USBControl/USBCheckor.h"
#include "..\Security\Authentication\BankMdrVerifier\export.h"
#include "../Utils/ListManager/ListManager.h"
#include "../ExternalDispatchImpl.h"
#include "../../USBControl/USBHardWareDetector.h"
#include "../../BankUI/UIControl/CoolMessageBox.h"

#define _DownLoadPath  _T("BankDownLoad\\")
#define _SetUpSubPath _T("BankInfo\\banks\\")


CNotifyFavProgress* CNotifyFavProgress::m_Instance = NULL;
CNotifyFavProgress::CNotifyFavProgress():m_hwndNotify(NULL)
{
}

void CNotifyFavProgress::SetFavProgress(string appId, int progress, nsDownStates dstate, bool allowState)
{
	if(m_hwndNotify)
	{
		LPDOWNLOADSTATUS pstatus = new DOWNLOADSTATUS;
		pstatus->appId = appId;

		char temp[100] = {0};
		sprintf_s(temp, 100 , "%d", progress);
		string stp(temp);

		pstatus->progress = stp;

		switch(dstate)
		{
		case nsDownloading:pstatus->status = "正在下载";
			break;
		case nsInstalling:pstatus->status = "正在安装";
			break;
		default:pstatus->status = "";
			break;
		}

		switch(allowState)
		{
		case true:pstatus->logo = "1";
			break;
		default:pstatus->logo = "0";
			break;
		}
		
		::PostMessage (m_hwndNotify, WM_AX_FRAME_CHANGE_PROGRESS, (WPARAM)pstatus, 0);

	}
}
void CNotifyFavProgress::SetFavProgress(string appId, int progress)
{
	//if(progress == )
}
void CNotifyFavProgress::SetProgressNotifyHwnd(HWND hNotify)
{
	m_hwndNotify = hNotify;
}

void CNotifyFavProgress::CancelFav(string appId)//取消收藏
{
	if(m_hwndNotify)
	{
		LPARAM param;
		memcpy(&param , appId.c_str() ,sizeof(param));
		::PostMessage (m_hwndNotify, WM_AX_FRAME_DELETEFAV, 0, param);
	}
}

void CNotifyFavProgress::AddFav(string appId)// 增加收藏
{

	if(m_hwndNotify)
	{
		LPARAM param;
		memcpy(&param , appId.c_str() ,sizeof(param));
	
		::PostMessage (m_hwndNotify, WM_AX_FRAME_ADDFAV, 0, param);
	}
}

CNotifyFavProgress* CNotifyFavProgress::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CNotifyFavProgress();
	return m_Instance;
}



CBkInfoDownloadManager* CBkInfoDownloadManager::m_staticInstance = NULL;
IBankDownInterface* CBkInfoDownloadManager::m_staticpICheckFile = NULL;

CBkInfoDownloadManager::CBkInfoDownloadManager() :m_bAddBkCtrl(false)
{
	// 初始化临界区
	InitializeCriticalSection(&m_cs);

	ReadAcquiesceSetupPath ();
	// 暂时用来测试，要调用GetHWIDs
	m_strHWID = CA2W(GenHWID2().c_str ());
	DWORD dwThreadID = 0;
	CloseHandle (CreateThread (NULL, 0, CheckThreadProc, (LPVOID)this, 0, &dwThreadID));
}

CBkInfoDownloadManager::~CBkInfoDownloadManager()
{
	DeleteCriticalSection(&m_cs);
}

// 读取文件进度
int CBkInfoDownloadManager::ReadDownLoadPercent(LPSTR lpBankID)
{
	ATLASSERT (NULL != lpBankID);
	if (NULL == lpBankID)
		return ERR_UNKNOW;

	CDownloadManagerThread* pTempThread = FindBankCtrlDLManager(lpBankID);
	if (NULL == pTempThread)
		return 200; // 表示安装完成

	return pTempThread->GetAverageDownLoadProcess (); // 读取进度
}

void CBkInfoDownloadManager::MyBankCtrlDownload(LPSTR lpBankID)
{
	ATLASSERT (NULL != lpBankID);
	if (NULL == lpBankID)
		return;

	//OutputDebugString(L"收藏银行———内核");
	//OutputDebugString(CA2W (lpBankID));

	// 如果已经安装了，不进行下载
	if (IsSetupAlready (lpBankID))
	{
		NotifyCoreBankCtrlDLFinish (lpBankID);

		// 将数据库中的数据标记成已经安装
		CBankData::GetInstance ()->SaveFav (lpBankID, 200);
		// 将数据库中的数据标记成已经安装
		CNotifyFavProgress::GetInstance()->SetFavProgress(lpBankID, 200, nsNULL, true);
		return;
	}


	// 得到ID的下载控件对象指针
	CDownloadManagerThread* pTempThread = FindBankCtrlDLManager(lpBankID);

	if (NULL == pTempThread)
	{
		pTempThread = new CDownloadManagerThread (); // 创建一个管理对象

		::EnterCriticalSection(&m_cs);
		m_BankManager.insert (std::make_pair (lpBankID, pTempThread)); // 将新生成的加入到map
		::LeaveCriticalSection (&m_cs);

	}
	else // 检验该银行的是否正在下载
	{
		if (pTempThread->AddBankCtrlAlready ())
		{
			return;
		}
	}


	// 禁用界面
	CNotifyFavProgress::GetInstance()->SetFavProgress(lpBankID, 0, nsDownloading, false);
	CBankData::GetInstance ()->SaveFav (lpBankID, 0);

	// 得到下载的临时路径
	if (m_wstrDLTempPath.empty ())
		m_wstrDLTempPath = GetTempCachePath ();
	std::wstring strXmlPath = m_wstrDLTempPath;
	std::wstring strCtrlPath = m_wstrDLTempPath;	
	std::string strBankName = CFavBankOperator::GetBankIDOrBankName (lpBankID);
	strXmlPath += CA2W (strBankName.c_str ());
	strXmlPath += L".xml";
	strCtrlPath += CA2W (strBankName.c_str ());
	strCtrlPath += L".cab";

	// 读取老的校验码
	std::string strOldCk;
	std::list<std::wstring> TpList;
	if (!CheckBankCtrlXml(strXmlPath.c_str (), TpList, strOldCk)) 
	{
		strOldCk.clear ();
	}

	// 构建和服务器通讯的xml文件
	std::string strTemp;
	ConstructBkCtrlCommunicateXml (lpBankID, strTemp);

	std::wstring downPath = CHostContainer::GetInstance()->GetHostName(kPDownloadInstall);
	DOWN_LOAD_PARAM_NODE myNode;
	myNode.bCreateThread = false;
	myNode.strHWID = m_strHWID;
	myNode.strSendData = strTemp;
	//myNode.dwPostDataLength = strTemp.size ();
	myNode.strSaveFile = strXmlPath;
	myNode.strUrl = downPath;



	std::string strBkIDTp = lpBankID;
	while (true && CExternalDispatchImpl::IsInUserFavBankList(strBkIDTp))
	{
		// 从服务器端下载检验码和下载连接（采用阻塞方式下载，没有创建新的线程下载,因为要校验码）
		if (!pTempThread->CreateDownLoadTask (&myNode))// (m_strHWID.c_str (), downPath.c_str (), strXmlPath.c_str (), (LPVOID)strTemp.c_str (), strTemp.size (), false)
		{
			//OutputDebugString(L"从服务器下载XML数据失败！———内核");
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_DOWNLOAD_XML_CTRL, L"MyBankCtrlDownload从服务器下载XML数据失败！");
			Sleep (5000);
			continue;
		}

		// 检验验证码，并得到下载连接
		std::string strNewCk;
		std::list<std::wstring> UrlList;
		if (!CheckBankCtrlXml(strXmlPath.c_str (), UrlList, strNewCk))
		{
			//OutputDebugString(L"校验下载的XML数据失败！———内核");
			CRecordProgram::GetInstance( )->FeedbackError(MY_ERROR_PRO_CORE, ERR_READ_XML_CTRL, L"MyBankCtrlDownload校验下载的XML数据失败！");
			Sleep (5000);
			continue;
		}

		// 如果没有校验码就不比较了
		if (!strOldCk.empty ()) 
		{
			// 两个校验码不一样说明服务器更新了
			if (strOldCk != strNewCk) 
			{
				std::wstring strTp;
				CDownloadThread::TranslanteToBreakDownloadName (strXmlPath, strTp);
				DeleteFile (strTp.c_str ()); // 清除掉以前下好的临时文件
			}
		}

		std::list<std::wstring>::iterator it;
		if (UrlList.empty ())
		{
			//OutputDebugString(L"读取到下载连接数据为空！———内核");
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_DOWNLOAD_URL_NOFOUND_CTRL, CRecordProgram::GetInstance()->GetRecordInfo(L"MyBankCtrlDownload读取%s下载连接数据为空！", strXmlPath.c_str()));
			
		}
		for (it = UrlList.begin (); it != UrlList.end (); it ++) // 创建下载任务
		{
			DOWN_LOAD_PARAM_NODE myNode;
			myNode.bCreateThread = true;
			myNode.strHWID = m_strHWID;
			if (!m_bAddBkCtrl)
				myNode.emKind = emBkActiveX;// 下载的是银行控件
			else
			{
				myNode.emKind = emUSBBkActiveX; // 是由USBKEY插入而引起的下载的银行控件
				m_bAddBkCtrl = false;
			}
			
			myNode.strSaveFile = strCtrlPath;
			myNode.strSetupPath = m_wstrSetupPath;
			myNode.bSetupDlFile = true;
			myNode.strUrl = (*it);

			pTempThread->CreateDownLoadTask (&myNode);
		}

		// 已经成功添加下载任务
		if (UrlList.size() > 0)
			return;

		// 5秒后重新链接下载
		Sleep(5000);
	}
}

void CBkInfoDownloadManager::MyBankUsbKeyDownload(LPUSBRECORD pUsbNode) // 下载USBKEY// LPSTR lpBankID, LPVOID lpVoid, int nSize
{
	std::string strBankID = pUsbNode->fav;
	std::string strXml = pUsbNode->xml;
	std::string strVer = pUsbNode->ver;

	//检测银行控件是否已经安装
	CBankData* pBankData = CBankData::GetInstance();

	// 2者条件满足一个就进行安装
	bool bAddBkCtrl = false;
	CBkInfoDownloadManager* pManager = CBkInfoDownloadManager::GetInstance ();
	bool bSetup = pManager->IsSetupAlready (pUsbNode->fav);
	bool bExist = pBankData->IsFavExist(strBankID);
	if ((!bSetup) || (!bExist))
	{
		m_bAddBkCtrl = true; // 标记下载的银行控件是USBKEY触发的
		CNotifyFavProgress::GetInstance()->AddFav(pUsbNode->fav);//调用js开始安装
		
		bAddBkCtrl = true;
	}

	// 得到USBKEY保存的名字，VID+PID+MID
	std::wstring strSaveName;
	wchar_t wchTemp[20] = {0};
	_itow_s (pUsbNode->vid, wchTemp, 19, 10);
	strSaveName += wchTemp;
	memset (wchTemp, 0, sizeof (wchTemp));
	_itow_s (pUsbNode->pid, wchTemp, 19, 10);
	strSaveName += wchTemp;
	memset (wchTemp, 0, sizeof (wchTemp));
	_itow_s (pUsbNode->mid, wchTemp, 19, 10);
	strSaveName += wchTemp;

	// 得到ID的下载控件对象指针
	CDownloadManagerThread* pTempThread = FindBankCtrlDLManager((LPSTR)strBankID.c_str ());
	if (NULL == pTempThread)
	{
		pTempThread = new CDownloadManagerThread (); // 创建一个管理对象

		::EnterCriticalSection(&m_cs);
		m_BankManager.insert (std::make_pair (strBankID.c_str (), pTempThread)); // 将新生成的加入到map
		::LeaveCriticalSection (&m_cs);
	}
	std::wstring strXmlPath, strCtrlPath;
	if (m_wstrDLTempPath.empty ())
		m_wstrDLTempPath = GetTempCachePath (); // 得到下载的临时路径

	strXmlPath = m_wstrDLTempPath;
	strCtrlPath = m_wstrDLTempPath;

	strXmlPath += strSaveName;
	strXmlPath += L".xml";
	strCtrlPath += strSaveName;
	strCtrlPath += L".exe";

	//做base64编码
	DWORD dwBufLen = strlen(strXml.c_str()) + 32;
	LPSTR lpszXmlInfo = new char[dwBufLen];

	strcpy_s(lpszXmlInfo,dwBufLen,strXml.c_str());

	int dwSize = strlen(lpszXmlInfo) * 2 + 1;
	unsigned char* pszOut = new unsigned char[dwSize];
	base64_encode((LPBYTE)lpszXmlInfo, strlen(lpszXmlInfo), pszOut, &dwSize);
	pszOut[dwSize] = 0;


	strXml = "xml=";
	strXml += UrlEncode((char *)pszOut);

	// 从服务器端下载检验码和下载连接（采用阻塞方式下载，没有创建新的线程下载,因为要校验码）
	std::wstring downPath = CHostContainer::GetInstance()->GetHostName(kPUkey);
	DOWN_LOAD_PARAM_NODE myNode;
	myNode.bCreateThread = false;
	myNode.strHWID = m_strHWID;
	myNode.strSendData = strXml;
	//myNode.dwPostDataLength = strXml.size ();
	myNode.strSaveFile = strXmlPath;
	myNode.strUrl = downPath;

	
	if (!pTempThread->CreateDownLoadTask (&myNode))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_DOWNLOAD_XML_USBKEY, L"MyBankUsbKeyDownload从服务器端下载XML文件失败！");
	}

	// 检验验证码，并得到下载连接
	std::string strNewCk;
	std::string strVersion;
	std::list<std::wstring> UrlList;
	if (!CheckUsbKeyXml(strXmlPath.c_str (), UrlList, strNewCk, strVersion))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_READ_XML_USBKEY, CRecordProgram::GetInstance()->GetRecordInfo(L"MyBankUsbKeyDownload解析%s数据失败！",strXmlPath.c_str ()));
	}

	bool bUpData = false;
	// 进行版本比较
	int nVerComp = MyTwoVersionCompare (strVersion, strVer);
	// 表示要弹出版本要更新的提示
	if (nVerComp > 0)
		bUpData = true;
	if(strVer == DEFAULT_INSTALL_VERSION)
	{
		bUpData = false;
	}


	// 检验该usb控件是否已经安装，没有安装就下载安装
	bool bCheck = m_staticpICheckFile->CheckServerXmlFile (bUpData, *pUsbNode);
	if(bCheck == false)
		return;
	int nStatus = pUsbNode->status; // 用这个来判定是否安装
	if (200 == nStatus)
	{
		// 如果银行控件已经收藏
		if (bAddBkCtrl == false || bSetup == true || bUpData == false)
		{
			USBKeyInfo* pusb = CUSBHardWareDetector::GetInstance()->CheckUSBHardWare(pUsbNode->vid, pUsbNode->pid, pUsbNode->mid);//从内存文件中获得相关信息

			if(!pusb)
				return;

			wstring msg;
			msg = pusb->hardware.goodsname + L"软件安装正常，可以使用";
			mhShowMessage( GetActiveWindow(), msg.c_str());

			return;
		}
	}
 
	// 更新USBKEY的版本
	CBankData::GetInstance ()->UpdateUSB (pUsbNode->vid, pUsbNode->pid, pUsbNode->mid, strVersion);


	if (UrlList.size() <= 0)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_DOWNLOAD_URL_NOFOUND_USBKEY, L"MyBankUsbKeyDownload没有检测到下载链接！");
	}

	std::list<std::wstring>::iterator it;
	for (it = UrlList.begin (); it != UrlList.end (); it ++) // 创建下载任务
	{
		DOWN_LOAD_PARAM_NODE myNode;
		myNode.bCreateThread = true;
		myNode.strHWID = m_strHWID;
		myNode.emKind = emUsbKey; // 下载USBKEY
		
		myNode.strSetupPath = m_wstrSetupPath;
		myNode.bSetupDlFile = true;
		myNode.strSaveFile = strCtrlPath;
		myNode.strUrl = (*it);

		// 初始化USBKEY特有的参数
		myNode.dwUsbKeyParam.nMid = pUsbNode->mid;
		myNode.dwUsbKeyParam.nPid = pUsbNode->pid;
		myNode.dwUsbKeyParam.nVid = pUsbNode->vid;
		myNode.dwUsbKeyParam.strVersion = pUsbNode->ver;


		pTempThread->CreateDownLoadTask (&myNode);
		
	}

}

// 新增一个下载任务
void CBkInfoDownloadManager::DownloadBankCtrl(LPSTR lpBankID, LPCTSTR lpszUrl, LPCTSTR lpszSaveFile, LPVOID lpPostData, DWORD dwPostDataLength)
{
}

// 查找银行相应的管理指针
CDownloadManagerThread* CBkInfoDownloadManager::FindBankCtrlDLManager(LPSTR lpBankID)
{
	ATLASSERT (NULL != lpBankID);
	if (NULL == lpBankID)
		return NULL;
	
	std::map<std::string, CDownloadManagerThread*>::const_iterator it;

	::EnterCriticalSection(&m_cs);


	it = m_BankManager.find (lpBankID);
	if (m_BankManager.end () == it)
	{
		::LeaveCriticalSection (&m_cs);
		return NULL;
	}

	::LeaveCriticalSection (&m_cs);

	return it->second;
}

// 解析服务器端的返回的数据
bool CBkInfoDownloadManager::ParseBkCtrlListContent(const char* pContent, std::list<std::wstring>& UrlList, std::string& strCheckCode)
{
	ATLASSERT (NULL != *pContent);
	if (NULL == *pContent)
		return false;

	USES_CONVERSION;
	UrlList.clear();

	TiXmlDocument xmlDoc;
	xmlDoc.Parse(pContent); // 参数是文件内容

	if (xmlDoc.Error())
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, xmlDoc.Error(), L"TiXmlDocument读ParseBkCtrlListContent");
		return false;
	}

	const TiXmlNode* pRoot = xmlDoc.FirstChild("moneyhub"); // ANSI string 
	if (NULL == pRoot)
		return false;

	const TiXmlNode* pStatus = pRoot->FirstChild("status");
	if (NULL == pStatus)
		return false;

	const TiXmlNode* pStatusId = pStatus->FirstChild("id");
	if (NULL == pStatusId)
		return false;

	const TiXmlElement* pStatusIdElement = pStatusId->ToElement();
	const char* pText = pStatusIdElement->GetText();
	DWORD dwStatusId = pText != NULL ? atoi(pText) : 0; // ID

	//const TiXmlNode* pStatusMsg = pStatus->FirstChild("message");
	//if (NULL != pStatusMsg)
	//{
	//	const TiXmlElement* pStatusMsgElement = pStatusMsg->ToElement();
	//	pText = pStatusMsgElement->GetText();
	//	//m_strStatusMsg = pText != NULL ? AToW(pStatusMsgElement->GetText()) : L""; // message type
	//}

	if (dwStatusId != 300) // ID type
		return false;

	const TiXmlNode* pModules = pRoot->FirstChild("modules");
	if (NULL == pModules)
		return false;

	for (const TiXmlNode *pModule = pModules->FirstChild("module"); pModule != NULL; pModule = pModules->IterateChildren("module", pModule))
	{
		const TiXmlElement* pModuleElement = pModule->ToElement();
		std::string strType = "";
		if (pModuleElement->Attribute("type"))
			strType = pModuleElement->Attribute("type");

		bool bCriticalPack = _stricmp(strType.c_str(), "critical") == 0;

		const TiXmlNode* pModuleName = pModule->FirstChild("name");
		if (NULL == pModuleName)
			continue;

		const TiXmlElement* pModuleNameElement = pModuleName->ToElement();
		//get base64
		pText = pModuleNameElement->Attribute("check");
		strCheckCode = pText; // 得到校验码
		if (!CheckCheckCode (strCheckCode))
			return false;

		pText = pModuleNameElement->GetText();
		std::string strVersionName = pText;
		std::wstring name = pText != NULL ? A2W(pModuleNameElement->GetText()) : L""; // name


		const TiXmlNode* pModulePath = pModule->FirstChild("path");
		if (NULL == pModulePath)
			continue;

		const TiXmlElement* pModulePathElement = pModulePath->ToElement();
		pText = pModulePathElement->GetText();
		std::wstring url = pText != NULL ? A2W(pModulePathElement->GetText()) : L"";

		if (!url.empty())
		{
			// 添加到下载列表中
			UrlList.push_back (url);
		}
	}

	if (UrlList.size() == 0)
	{
		return false;
	}
	return true;
}

bool CBkInfoDownloadManager::ParseUSBListContent(const char* pContent, std::list<std::wstring>& UrlList, std::string& strVersion, std::string& strCheck)
{
	UrlList.clear();
	strVersion.clear ();
	strCheck.clear ();
	ATLASSERT (NULL != pContent);
	if (NULL == pContent)
		return false;

	USES_CONVERSION;

	TiXmlDocument xmlDoc;
	xmlDoc.Parse(pContent); // 参数是文件内容

	if (xmlDoc.Error())
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, xmlDoc.Error(), L"TiXmlDocument读ParseUSBListContent");
		return false;
	}

	const TiXmlNode* pRoot = xmlDoc.FirstChild("moneyhub"); // ANSI string 
	if (NULL == pRoot)
		return false;

	const TiXmlNode* pNode = pRoot->FirstChild ("usbinfo");
	if (NULL == pNode)
		return false;

	for (; pNode != NULL; pNode = pRoot->IterateChildren("usbinfo", pNode))
	{
		const TiXmlNode* pUrlNode = pNode->FirstChild ("url");
		if (pUrlNode == NULL)
			continue;

		const TiXmlElement* pUrlEle = pUrlNode->ToElement ();
		const char* pText = pUrlEle->GetText ();
		std::wstring url = pText != NULL ? A2W(pUrlEle->GetText()) : L"";
		UrlList.push_back (url);

		const TiXmlNode* pVerNode = pNode->FirstChild ("ver");
		if (pVerNode == NULL)
			continue;

		const TiXmlElement* pVerEle = pVerNode->ToElement ();
		pText = pVerEle->GetText ();
		strVersion = pText != NULL ? pVerEle->GetText() : "";
		


		const TiXmlNode* pCheckNode = pNode->FirstChild ("checknode");
		if (pCheckNode == NULL)
			continue;

		const TiXmlElement* pCheckEle = pCheckNode->ToElement ();
		pText = pCheckEle->GetText ();
		strCheck = pText != NULL ? pCheckEle->GetText() : "";

		CheckCheckCode(strCheck);
	
	}

	return true;
}

// 构建下载BankCtrl和服务器通讯的xml文件
bool CBkInfoDownloadManager::ConstructBkCtrlCommunicateXml(LPSTR lpBankID, std::string& info)
{
	ATLASSERT (NULL != lpBankID);
	if (NULL == lpBankID)
		return false;

	LPSTR szXmlInfoPattern = "<?xml version=\"1.0\" encoding=\"utf-8\"?><moneyhub><modules><module><name>";
	std::string allXmlStr = szXmlInfoPattern;
	std::string strBankName = CFavBankOperator::GetBankIDOrBankName (lpBankID);
	allXmlStr += strBankName;
	allXmlStr += "</name></module></modules></moneyhub>";
	
/////////////////////////////////////////////////////////////////////////
	DWORD dwBufLen = strlen(allXmlStr.c_str()) + 32;
	LPSTR lpszXmlInfo = new char[dwBufLen];

	strcpy_s(lpszXmlInfo,dwBufLen,allXmlStr.c_str());

	int dwSize = strlen(lpszXmlInfo) * 2 + 1;
	unsigned char* pszOut = new unsigned char[dwSize];
	base64_encode((LPBYTE)lpszXmlInfo, strlen(lpszXmlInfo), pszOut, &dwSize);
	pszOut[dwSize] = 0;

	
	info = "xml=";
	info += UrlEncode((char *)pszOut);

	delete []pszOut;
	delete []lpszXmlInfo;

	return true;
}

// 构建下载USB和服务器进行通讯的xml文件
bool CBkInfoDownloadManager::ReadUSBCommunicateXml(const std::string& strIn, std::string& info)
{
	if (strIn.empty ())
		return false;

	USBRECORD usbRecord;
	memset (&usbRecord, 0, sizeof (USBRECORD));

	std::string strTp = strIn, strPid, strVid, strMid;
	size_t nIndex = 0;
	nIndex = strTp.find (PID_MARK_STRING); // 得到PID起始位置
	if (nIndex < 0)
		return false;

	strTp = strTp.substr (nIndex + 1 + PID_MARK_STRING.length (), strTp.length ());
	nIndex = strTp.find (VID_MARK_STRING); // 得到VID的起始位置
	if (nIndex < 0)
		return false;
	strPid = strTp.substr (0, nIndex);

	strTp = strTp.substr (nIndex + 1 + VID_MARK_STRING.length (), strTp.length ());
	nIndex = strTp.find (MID_MARK_STRING);
	if (nIndex < 0)
		return false;
	strVid = strTp.substr (0, nIndex);

	strMid = strTp.substr (nIndex + MID_MARK_STRING.length (), strTp.length ());

	if (strPid.empty () || strVid.empty () || strMid.empty ())
		return false;
	


	// 读取在数据库的记录
	CBankData* pBankData = CBankData::GetInstance();
	if (pBankData->GetAUSBRecord (atoi (strVid.c_str ()), atoi(strPid.c_str ()), atoi(strMid.c_str ()), usbRecord))
	{
		info = usbRecord.xml;
		return true;
	}
	else
		return false;
}

std::string CBkInfoDownloadManager::UrlEncode(const std::string& src)   
{   
    static char hex[] = "0123456789ABCDEF";   
    std::string dst;   
  
    for (size_t i = 0; i < src.size(); i++)   
    {   
        unsigned char ch = src[i];   
        if (isalnum(ch))   
        {   
            dst += ch;   
        }   
        else  
            if (src[i] == ' ')   
            {   
                dst += '+';   
            }   
            else  
            {   
                unsigned char c = static_cast<unsigned char>(src[i]);   
                dst += '%';   
                dst += hex[c / 16];   
                dst += hex[c % 16];   
            }   
    }   
    return dst;   
}

bool CBkInfoDownloadManager::CheckBankCtrlXml(LPCTSTR lpPath, std::list<std::wstring>& UrlList, std::string& strCheckCode)
{
	strCheckCode.clear ();

	ATLASSERT (NULL != lpPath);
	if (NULL == lpPath)
		return false;

	HANDLE hFile = CreateFile(lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwLowSize = GetFileSize(hFile, NULL);
	if (dwLowSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);

		return false;
	}

	DWORD dwRead = 0;
	char* pContent = new char[dwLowSize + 1];
	if (!ReadFile(hFile, pContent, dwLowSize, &dwRead, NULL))
	{
		delete[] pContent;
		CloseHandle(hFile);

		return false;
	}

	CloseHandle(hFile);
	*(pContent+dwLowSize) = '\0';


	if (!ParseBkCtrlListContent(pContent, UrlList, strCheckCode)) // 解析银行控件xml文件
	{
		delete[] pContent;
		return false;
	}

	delete[] pContent;

	return true;
}

bool CBkInfoDownloadManager::CheckUsbKeyXml(LPCTSTR lpPath, std::list<std::wstring>& UrlList, std::string& strCheckCode, std::string& strVersion)
{
	UrlList.clear ();
	strCheckCode.clear ();
	strVersion.clear ();

	ATLASSERT (NULL != lpPath);
	if (NULL == lpPath)
		return false;

	HANDLE hFile = CreateFile(lpPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwLowSize = GetFileSize(hFile, NULL);
	if (dwLowSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);

		return false;
	}

	DWORD dwRead = 0;
	char* pContent = new char[dwLowSize + 1];
	if (!ReadFile(hFile, pContent, dwLowSize, &dwRead, NULL))
	{
		delete[] pContent;
		CloseHandle(hFile);

		return false;
	}

	CloseHandle(hFile);
	*(pContent+dwLowSize) = '\0';

	if (!ParseUSBListContent(pContent, UrlList, strVersion, strCheckCode)) // 解析下载USBKEY时服务器返回的xml文件
	{
		delete[] pContent;
		return false;
	}

	delete[] pContent;

	return true;
}

// 校验码校验
bool CBkInfoDownloadManager::CheckCheckCode(const std::string& strCheck)
{
	unsigned char content[4000]={0};
	int dwReturnSize = 0;

	//for(std::vector<std::string>::size_type i = 0; i < m_checkBase64.size (); i ++) // gao

	base64_decode((unsigned char *)strCheck.c_str(), strCheck.size(),content,&dwReturnSize);	

	*(content + dwReturnSize) = 0;

	if(!BankMdrVerifier::InitCheck((const char *)content,dwReturnSize) )
	{
		return false;
	}

	return true;

}

// 得到下载的临时目录
std::wstring CBkInfoDownloadManager::GetTempCachePath()
{
	char* pTempPath = new char[MAX_PATH + 1];
	memset (pTempPath, 0, MAX_PATH + 1);

	// 得Application Data路径
	SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, pTempPath);

	std::wstring path = CA2W(pTempPath);
	if (path[path.size() - 1] != '\\')
		path += _T("\\");

	path += _T("MoneyHub\\");
	::CreateDirectoryA(CW2A(path.c_str ()), NULL);


	path += _DownLoadPath;
	::CreateDirectoryA(CW2A(path.c_str ()), NULL);

	delete []pTempPath;

	return path;
}

DWORD CBkInfoDownloadManager::CheckThreadProc(LPVOID lpParam)
{
	CBkInfoDownloadManager *pTemp = (CBkInfoDownloadManager*)lpParam;
	while(true)
	{

		// 查看任务是否完成
		
		std::map<std::string, CDownloadManagerThread*>::const_iterator cstIt;

		::EnterCriticalSection(&(pTemp->m_cs));
		for (cstIt = pTemp->m_BankManager.begin (); cstIt != pTemp->m_BankManager.end (); cstIt ++)
		{

			CDownloadManagerThread* pFinish = (*cstIt).second;
			ATLASSERT (NULL != pFinish);
			if (NULL == pFinish)
				continue;

			if (!pFinish->HasDownloadTask ())
				continue;

			bool bUseable = false;
			nsDownStates emStatus = nsNULL;
			int nPercent = 0;
			
			MY_DOWNLOAD_KIND emMyKind = emOther;


			CDownloadAndSetupThread* pDlThread = pFinish->SetPercentAndGetAFinishDownloadThread ((LPSTR)(*cstIt).first.c_str());

			if (NULL != pDlThread)
			{
				emMyKind = pDlThread->GetDownloadKind ();
				if (pDlThread->IsCancled ())
				{
					if (emBkActiveX & emMyKind)
					{
						// 取消银行收藏并删除数据库中的记录
						//CNotifyFavProgress::GetInstance()->CancelFav ((*cstIt).first);
					}
					else if (emBkActiveX  & emUsbKey)
					{
						// 取消USBKEY中的记录
						CBankData* pBankData = CBankData::GetInstance ();
						pBankData->DeleteUSB(pDlThread->m_dlUSBParam.nVid, pDlThread->m_dlUSBParam.nPid, pDlThread->m_dlUSBParam.nMid);//上次安装完了才删除
					}
				}
				else if (pDlThread->IsFinished ())
				{

					if (emBkActiveX & emMyKind)
					{
						pTemp->NotifyCoreBankCtrlDLFinish ((LPSTR)(*cstIt).first.c_str ());
						// 启用银行，设置状态
						nPercent = 200;
						bUseable = true;
					}
					else
					{
						nPercent = 200;
					}

				}
			}

			if (NULL != CNotifyFavProgress::GetInstance()->m_hwndNotify && NULL != pFinish) // 获取并发送进度
			{

				if (nPercent < 200)
					nPercent = pFinish->GetAverageDownLoadProcess ();
			
				std::string strID;
				strID = (*cstIt).first;


				bUseable = pFinish->IsBankCtrlSetup ();
				if (nPercent < 100)
				{
					emStatus = nsDownloading; // 正在下载
				}
				else if (nPercent == 100)
				{
					emStatus = nsInstalling; // 正在安装
				}
				
				if(nPercent == 200)
				{
					// 下载并安装完成
					if (emBkActiveX & emMyKind)
						CBankData::GetInstance ()->SaveFav ((*cstIt).first, 200);
					else if(emUsbKey & emMyKind)
						CBankData::GetInstance ()->UpdateUSB (pDlThread->m_dlUSBParam.nVid, pDlThread->m_dlUSBParam.nPid, pDlThread->m_dlUSBParam.nMid, 200);
				}
				// 如果是USBKEY的话得另外设置进度

				// fan 
				CNotifyFavProgress::GetInstance()->SetFavProgress((*cstIt).first, nPercent, emStatus, bUseable);

			}

			if (NULL != pDlThread)
			{
				if (pFinish->IsBankCtrlSetup ()) // 银行控件已经安装
				{
					// 下的是USBKEY并且已经安装完成
					if (pDlThread->IsFinished () && emUSBBkActiveX == emMyKind)
					{
						DWORD dwThread = 0;
						// 新创建一个线程弹出消息
						//CloseHandle (CreateThread (NULL, 0, ShowUSBFinishThreadProc, NULL, 0, &dwThread));
					}
				}
			}

			if (pFinish->AllDownloadThreadExit ())
			{
				//if (pFinish->IsBankCtrlCancled ())
				//	CNotifyFavProgress::GetInstance()->CancelFav ((*cstIt).first);

				if (pFinish->IsBankCtrlSetup ()) // 银行控件已经安装
				{
					// 下载并安装完成
				
					CBankData::GetInstance ()->SaveFav ((*cstIt).first, 200);
					
					// fan 
					CNotifyFavProgress::GetInstance()->SetFavProgress((*cstIt).first, nPercent, emStatus, bUseable);

				}
				pFinish->DeleteAllDownLoadThread ();

			}

		}

		::LeaveCriticalSection (&(pTemp->m_cs));

		// 如果完成就进行安装，并修改进度
		Sleep (1000);
	}
}


void CBkInfoDownloadManager::ReadAcquiesceSetupPath()
{
	TCHAR szPath[MAX_PATH];

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
		ATLASSERT (false);
        return;
    }

	m_wstrSetupPath = szPath;
	size_t nIndex = m_wstrSetupPath.rfind ('\\');
	ATLASSERT (nIndex >= 0 && nIndex < m_wstrSetupPath.length ());
	if (nIndex < 0 || nIndex >= m_wstrSetupPath.length ())
		return;

	m_wstrSetupPath = m_wstrSetupPath.substr (0, nIndex);
	if (m_wstrSetupPath.rfind ('\\') != m_wstrSetupPath.length () - 1)
		m_wstrSetupPath += L"\\";

	m_wstrSetupPath += _SetUpSubPath;

}

void CBkInfoDownloadManager::CheckDownloadBreakFile(void)
{
	m_wstrDLTempPath = GetTempCachePath(); // 得到临时目录
	CString cstrTp = m_wstrDLTempPath.c_str ();
	cstrTp += "\\*.*";
	WIN32_FIND_DATA findData;
	memset(&findData, 0, sizeof(WIN32_FIND_DATAW));
	HANDLE hFFind = FindFirstFile (cstrTp , &findData);

	std::list<std::wstring> downList;
	if (INVALID_HANDLE_VALUE != hFFind)
	{
		do
		{
			std::wstring strFN = findData.cFileName;
			if (strFN == L"." || strFN == L"..")
				continue;

			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// 判断xml文件和临时文件是不是成对的
				if (!IsXmlBreakFileExistInPair (m_wstrDLTempPath.c_str (), strFN.c_str ()))
				{
					// 不是的删除
					DeleteFile ((m_wstrDLTempPath + strFN).c_str ());
					continue;
				}

				// 继续下载未完成的任务
				if (strFN.find (L".xml") == strFN.length () - 4)
				{
					strFN = strFN.substr (0, strFN.find (L".xml"));
					downList.push_back (strFN);
					//DownloadBankCtrl (strFN.c_str ());
				}
			}
			
		}
		while (FindNextFile (hFFind, &findData));
	}

	FindClose (hFFind);

	std::list<std::wstring>::const_iterator it;
	for (it = downList.begin (); it != downList.end (); it ++)
	{
		DownloadBankCtrl (CW2A((*it).c_str ()));
	}
}

bool CBkInfoDownloadManager::IsXmlBreakFileExistInPair(LPCTSTR lpPath, LPCTSTR lpFileName)
{
	ATLASSERT (NULL != lpPath);
	if (NULL == lpPath)
		return false;

	std::wstring strPath = lpPath;
	std::wstring strFileName = lpFileName;

	if (strPath.length () < 4 || strPath.empty ())
		return false;

	if (strFileName.find (L".xml") == strFileName.length () - 4) // 表明是xml文件
	{
		strPath += strFileName;

		std::wstring strTp;
		if (!CDownloadThread::TranslanteToBreakDownloadName (strPath, strTp)) // 转换成临时文件的路径
			return false;
		
		HANDLE hFile = CreateFile (strTp.c_str (), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			CloseHandle (hFile);
			return true;
		}
		else
			return false;
	}
	else if (strFileName.find (L".mh") == strFileName.length () - 3) // 表明是临时文件
	{

		// 得到临时文件对应的xml文件名
		strFileName = strFileName.substr (0, strFileName.length () - 3);
		if (strFileName.find (L".") <= 0)
			return false;

		strFileName = strFileName.substr (strFileName.find (L".") + 1, strFileName.length ());
		strFileName += L".xml";

		// 得到全路径
		strPath += strFileName;
		
		HANDLE hFile = CreateFile (strPath.c_str (), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			CloseHandle (hFile);
			return true;
		}
		else
			return false;
	}
	else
		return false;


}

// 检验是否已经安装了
bool CBkInfoDownloadManager::IsSetupAlready(LPSTR lpBankID)
{
	ATLASSERT (NULL != lpBankID);
	if (NULL == lpBankID)
		return false;

	wchar_t message[MSG_BUF_LEN];
	////验证银行控件完整性
	std::string strBankPath = CW2A (m_wstrSetupPath.c_str ()); 
	
	std::string strBankName = CFavBankOperator::GetBankIDOrBankName (lpBankID);
	strBankPath += strBankName;

	CSecurityCheck temp;
	std::string filePath;
	filePath = strBankPath + "\\" + "bank.mchk" ;

	return (temp.VerifyMoneyHubList(strBankPath.c_str(), filePath.c_str(), message) < 0)? false:true;
}

void CBkInfoDownloadManager::CancleDownload(LPSTR lpBankID) // 用户取消下载
{
	ATLASSERT (NULL != lpBankID);
	if (NULL == lpBankID)
		return;

	CDownloadManagerThread* pFindManager = FindBankCtrlDLManager(lpBankID);
	if (NULL == pFindManager)
		return;

	pFindManager->CancleAllDownload ();
	
	// 清空管理线程中的记录
	pFindManager->DeleteAllDownLoadThread();
}

void CBkInfoDownloadManager::PauseDownload(LPSTR lpBankID) // 用户暂停下载
{
	ATLASSERT (NULL != lpBankID);
	if (NULL == lpBankID)
		return;

	CDownloadManagerThread* pFindManager = FindBankCtrlDLManager(lpBankID);
	if (NULL == pFindManager)
		return;

	pFindManager->PauseAllDownload ();

}

void CBkInfoDownloadManager::SetCheckFun(IBankDownInterface* pObject)
{
	m_staticpICheckFile = pObject;
}

DWORD WINAPI CBkInfoDownloadManager::DownloadBkUSBThreadProc(LPVOID lpParam)
{
	// 得到用户没有下载完成的USBKEY控件
	CBankData* pBankData = CBankData::GetInstance();
	std::vector<LPUSBRECORD> vec;
	pBankData->GetAllUsb (vec);
	std::vector<LPUSBRECORD>::const_iterator cstItUsb;

	CBkInfoDownloadManager *pTemp = CBkInfoDownloadManager::GetInstance ();
	for (cstItUsb = vec.begin (); cstItUsb != vec.end (); cstItUsb ++)
	{
		LPUSBRECORD lpTempNode = *cstItUsb;
		ATLASSERT (lpTempNode != NULL);
		if (NULL == lpTempNode)
			continue;

		// 如果没有成功安装
		if (lpTempNode->status < 200)
		{
			pTemp->MyBankUsbKeyDownload (lpTempNode);
		}

		// 在这释放内存
		delete lpTempNode;
		lpTempNode = NULL;
	}
	return 0;
}

// 下载没有下载完成的或破坏的收藏银行控件
void CBkInfoDownloadManager::FinishDLBreakFile(void)
{
	std::list<std::string> strFav;
	CBankData* pBankData = CBankData::GetInstance();

	// 得到用户收藏的未下载完成的银行控件
	pBankData->GetFav (strFav);
	std::list<std::string>::const_iterator cstIt;
	DWORD dwThreadID = 0;
	for (cstIt = strFav.begin (); cstIt != strFav.end (); cstIt ++)
	{
		DWORD dwThreadID = 0;
		char* appid = new char[20];
		memset(appid, 0, 20);
		strcpy_s(appid, 20, (*cstIt).c_str ());

		CloseHandle (CreateThread (NULL, 0, CExternalDispatchImpl::DownloadBkCtrlThreadProc,(LPVOID)appid, 0, &dwThreadID)); 
		//MyBankCtrlDownload ((LPSTR)(*cstIt).c_str ());
	}

//	CloseHandle (CreateThread (NULL, 0, DownloadBkUSBThreadProc, 0, 0, &dwThreadID));
}

CBkInfoDownloadManager* CBkInfoDownloadManager::GetInstance(void)
{
	if (NULL == m_staticInstance)
		m_staticInstance = new CBkInfoDownloadManager ();

	return m_staticInstance;
}

void CBkInfoDownloadManager::SetProgressNotifyHwnd(HWND hNotify) // 设置下载进度通知的句柄
{
	CNotifyFavProgress::GetInstance()->SetProgressNotifyHwnd(hNotify);
}

int CBkInfoDownloadManager::MyTwoVersionCompare(std::string& strVer1, std::string& strVer2)
{
	// 假设传入的是标准的如：1.0.0.0
	std::set<std::string> setStr1, setStr2;
	CFavBankOperator::SeparateStringByChar (setStr1, strVer1, '.');
	CFavBankOperator::SeparateStringByChar (setStr2, strVer2, '.');
	int nVer1 = 0, nVer2 = 0;
	int nPower = 1000;

	std::set<std::string>::const_iterator cstIt1;
	std::set<std::string>::const_iterator cstIt2;
	for (cstIt1 = setStr1.begin (), cstIt2 = setStr2.begin (); cstIt1 != setStr1.end () && cstIt2 != setStr2.end (); cstIt1 ++, cstIt2 ++)
	{
		nVer1 += atoi((*cstIt1).c_str ())*nPower;
		nVer2 += atoi((*cstIt2).c_str ())*nPower;
		if (nPower > 10)
			nPower = nPower / 10;
	}

	if (nVer1 > nVer2)
		return 1;
	else if (nVer1 == nVer2)
		return 0;
	else
		return -1;
}

void CBkInfoDownloadManager::NotifyCoreBankCtrlDLFinish(LPSTR lpBankID)
{
	ATLASSERT (NULL != lpBankID);
	if (NULL == lpBankID)
		return;

	std::string strBkID;
	strBkID = lpBankID;

	CListManager::_()->AddANewFavBank ((LPWSTR)CA2W(strBkID.c_str ()), false);
	CListManager::_()->UpdateHMac ();

	CFavBankOperator* pOper = CListManager::_()->GetFavBankOper();
	ATLASSERT (NULL != pOper);
	if (NULL == pOper)
		return;
	pOper->UpDateFavBankID (strBkID);

	CListManager::_()->CheckCom(strBkID);//尝试创建该对象，以确定是否已经将控件正确加入

	//int nTp;
	//CFavBankOperator::MyTranslateBetweenBankIDAndInt (strBkID, nTp);// 第三参数默认为true,表示将bankID转换成int

		// 用于通知主框架，用户添加收藏银行
	//SendMessage(g_hMainFrame, WM_FAV_BANK_CHANGE, 0, (LPARAM)nTp); // gao 通知框架用户收藏银行发生变化
}

IBankDownInterface* CBkInfoDownloadManager::GetInterfaceInstance(void)
{
	return m_staticpICheckFile;
}

DWORD WINAPI CBkInfoDownloadManager::ShowUSBFinishThreadProc(LPVOID lpVoid)
{
	// 等待JSP更新界面后才弹出对话框
	HWND hFrame = ::FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	::MessageBox (hFrame, L"USBKEY软件安装正常，可以使用！", L"提示", MB_OK | MB_ICONINFORMATION);
	return 0;
}