#include "stdafx.h"
#include "BillUrlManager.h"
#include "../../BankData/BankData.h"
#include "ConvertBase.h"
#include "../../Encryption/CHKFile/CHK.h"
#include <list>
#include "../MyError.h"
#include "../WebBrowserEventsManager.h"
#include "../ExternalDispatchImpl.h"
#include "../../BankUI/UIControl/ShowBillResultDlg.h"

CBillUrlManager* CBillUrlManager::m_Instance = NULL;

CBillUrlManager* CBillUrlManager::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CBillUrlManager();
	return m_Instance;
}

CBillUrlManager::CBillUrlManager()
{
	m_urldll = NULL;
	m_billTid.clear();
}
CBillUrlManager::~CBillUrlManager()
{
	if(m_urldll)
		::FreeLibrary(m_urldll);
	m_billTid.clear();
}

wstring CBillUrlManager::GetDllName(string aid)
{
	list<BILLURLDATA>::iterator fite;
	for(fite = m_billlist.begin(); fite!= m_billlist.end(); fite ++)
	{
		if(fite->id == aid)
		{
			return fite->dll;
		}
	}
	return L"";
}

int CBillUrlManager::Getislogin(string aid)
{
	list<BILLURLDATA>::iterator fite;
	for(fite = m_billlist.begin(); fite!= m_billlist.end(); fite ++)
	{
		if(fite->id == aid)
		{
			return fite->m_islogin;
		}
	}
	return 0;
}

int CBillUrlManager::Getmode(string aid)
{
	list<BILLURLDATA>::iterator fite;
	for(fite = m_billlist.begin(); fite!= m_billlist.end(); fite ++)
	{
		if(fite->id == aid)
		{
			return fite->m_mode;
		}
	}
	return 0;
}

LPURLLIST CBillUrlManager::GetUrlMap(string aid, int type)
{
	list<BILLURLDATA>::iterator fite;
	for(fite = m_billlist.begin(); fite!= m_billlist.end(); fite ++)
	{
		if(fite->id == aid)
		{
			list<URLLIST>::iterator site = fite->urldata.begin();
			for(;site!= fite->urldata.end(); site ++)
			{
				if(site->type == type)
					return &(*site);
			}
		}
	}
	return NULL;
}

wstring CBillUrlManager::GetBillUrl(string aid, int type, int step, bool& isBeginStep)
{
	isBeginStep = false;
	list<BILLURLDATA>::iterator fite;
	for(fite = m_billlist.begin(); fite!= m_billlist.end(); fite ++)
	{
		if(fite->id == aid)
		{
			list<URLLIST>::iterator site = fite->urldata.begin();
			for(;site!= fite->urldata.end(); site ++)
			{
				vector<int>::iterator bite;				
				bite = find(site->m_beginstep.begin(), site->m_beginstep.end(), step);
				if(bite != site->m_beginstep.end())
					isBeginStep = true;
				if(site->type == type)
				{
					std::map<int, wstring>::iterator mite;
					mite = site->url.find(step);
					if(mite != site->url.end())
						return mite->second;
				}
			}
		}
	}
	return L"";
}

bool CBillUrlManager::Init()
{
	wchar_t lpPath[255] = {0};
	GetModuleFileNameW(NULL,lpPath,sizeof(lpPath));
	::PathRemoveFileSpecW(lpPath);

	wstring path(lpPath);

	path += L"\\Config\\BillUrl.mchk";

	string info = GetFileContent(path.c_str(),true);
	if(info == "")
		return false;

	TiXmlDocument xmlDoc;

	xmlDoc.Parse(info.c_str()); 

	if (xmlDoc.Error())
	{
		CRecordProgram::GetInstance()->FeedbackError(L"Common", xmlDoc.Error(), L"TiXmlDocument读BillUrl");
		return false;
	}

	const TiXmlNode* pRoot = xmlDoc.FirstChild("main"); // ANSI string 
	if (NULL == pRoot)
		return false;

	const TiXmlNode* pUrlList = pRoot->FirstChild("urllist");
	if (NULL == pUrlList)
		return false;

	for (const TiXmlNode *pSite = pUrlList->FirstChild("site"); pSite != NULL; pSite = pUrlList->IterateChildren("site", pSite))
	{
		BILLURLDATA bData;
		bData.m_mode = 0;
		bData.m_islogin = 0;
		bData.id = pSite->ToElement()->Attribute("id");
		bData.name = AToW(pSite->ToElement()->Attribute("name"));
		bData.dll = AToW(pSite->ToElement()->Attribute("dll"));
		if( pSite->ToElement()->Attribute("mode") != NULL)
		{
	     	string tislogin = pSite->ToElement()->Attribute("mode");
			int mislogin = atoi(tislogin.c_str());
			bData.m_mode = mislogin;
		}
		
		if( pSite->ToElement()->Attribute("islogin") != NULL)
		{
	     	string tislogin = pSite->ToElement()->Attribute("islogin");
			int mislogin = atoi(tislogin.c_str());
			bData.m_islogin = mislogin;
		}
		
		for (const TiXmlNode *pType = pSite->FirstChild("type"); pType != NULL; pType = pSite->IterateChildren("type", pType))
		{
			URLLIST uList;
			string ttype = pType->ToElement()->Attribute("type");
			uList.type = atoi(ttype.c_str());

			for (const TiXmlNode *pStep = pType->FirstChild("step"); pStep != NULL; pStep = pType->IterateChildren("step", pStep))
			{		
				string tSeq = pStep->ToElement()->Attribute("seq");
				int seq = atoi(tSeq.c_str());
				wstring url = AToW(pStep->FirstChild()->Value());

				/*if (bData.id == "a003")
					url.replace(url.find(L"//vip."), 6, L"//mybank.");*/

				if (pStep->ToElement()->Attribute("begin") != NULL)
					uList.m_beginstep.push_back(seq);
				uList.url.insert(make_pair(seq, url));
			}
			bData.urldata.push_back(uList);
		}
		m_billlist.push_back(bData);
	}	
	return true;
}
std::string CBillUrlManager::GetFileContent(wstring strPath,bool bCHK)
{	
	HANDLE hFile = ::CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)\
	{
		CRecordProgram::GetInstance()->FeedbackError(L"Common", GetLastError(), L"读BillUrlList文件错误");
		return "";
	}

	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	std::string strFileContent;
	unsigned char *pData = new unsigned char[dwFileSize + 1];
	::ReadFile(hFile, pData, dwFileSize, &dwFileSize, NULL);
	if (bCHK)
	{
		char* content = new char[dwFileSize + 1];
		int contentLength = unPackCHK(pData, dwFileSize, (unsigned char *)content);
		if (contentLength < 0)
		{
			CRecordProgram::GetInstance()->FeedbackError(L"Common", GetLastError(), L"读BillUrlList文件unPackCHK错误");
			delete []content;
			delete []pData;

			CloseHandle(hFile);

			return "";
		}

		content[contentLength] = '\0';
		strFileContent = content;
		delete []content;
	}
	else
	{
		strFileContent = (char *)pData;
	}

	delete []pData;
	::CloseHandle(hFile);

	return strFileContent;
}
bool CBillUrlManager::InsertBillTid(DWORD pid, BillData* pData)
{
	m_billTid.insert(make_pair(pid, pData));
	return true;
}
BillData* CBillUrlManager::GetBillTid(DWORD pid)
{
	std::map<DWORD, BillData*>::iterator ite;
	ite = m_billTid.find(pid);

	if(ite != m_billTid.end())
	{
		return ite->second;
	}
	return NULL;
}

bool CBillUrlManager::DeleteBillTid(DWORD pid)
{
	std::map<DWORD, BillData*>::iterator ite;
	ite = m_billTid.find(pid);
	BillData* pData = NULL;

	if(ite != m_billTid.end())
	{
		pData = ite->second;
		m_billTid.erase(ite);
	}
	if(pData != NULL)
		delete pData;
	return true;
	
}
// 动态库中处理网页内容，模仿操作的接口
typedef int (WINAPI * FetchBillFunc)(IWebBrowser2* pFatherWebBrowser, IWebBrowser2* pChildWebBrowser, BillData* pData, int &step, LPBILLRECORDS plRecords);
FetchBillFunc pFetchBillFunc = NULL;
// 动态库中释放在动态库中申请的内存的接口
typedef int (WINAPI * FreeMemoryFunc)(LPBILLRECORDS plRecords);
FreeMemoryFunc pFreeMemoryFunc = NULL;
typedef void (WINAPI * SetBillStateFunc)(int bState);
SetBillStateFunc pSetBillStateFunc = NULL;

typedef void (WINAPI * SetNotifyWndFunc)(HWND notifyWnd);
SetNotifyWndFunc pSetNotifyWndFunc = NULL;

void CBillUrlManager::FreeDll()
{	
	if(m_urldll != NULL)
	{
		//消除dll申请的内存，否则存在问题！！
		pFreeMemoryFunc = (FreeMemoryFunc)::GetProcAddress(m_urldll, "FreeMemory");
		if(pFreeMemoryFunc != NULL)
			pFreeMemoryFunc(&m_BillRecords);
		FreeLibrary(m_urldll);
	}
	m_urldll = NULL;
}
void  CBillUrlManager::SetGetBillState(BillState bState)
{
	if(m_urldll != NULL)
	{
		//消除dll申请的内存，否则存在问题！！
		pSetBillStateFunc = (SetBillStateFunc)::GetProcAddress(m_urldll, "SetBillState");
		if(pSetBillStateFunc != NULL)
		{
			if(bState == bSNormal)
				pSetBillStateFunc( 0 );
			else if(bState == bSExceedTime)
				pSetBillStateFunc( BILL_EXCEED_MAX_TIME );
			else if(bState == bSCancel)
				pSetBillStateFunc( BILL_CANCEL_GET_BILL );
		}
	}
}
void  CBillUrlManager::SetNotifyWnd(HWND notifyWnd)
{
	if(m_urldll != NULL)
	{
		//消除dll申请的内存，否则存在问题！！
		pSetNotifyWndFunc = (SetNotifyWndFunc)::GetProcAddress(m_urldll, "SetNotifyWnd");
		if(pSetNotifyWndFunc != NULL)
			pSetNotifyWndFunc(notifyWnd);
	}
}

void CBillUrlManager::InitDll()
{
	if(m_urldll != NULL)
	{
		//消除dll申请的内存，否则存在问题！！
		pFreeMemoryFunc = (FreeMemoryFunc)::GetProcAddress(m_urldll, "FreeMemory");
		if(pFreeMemoryFunc != NULL)
			pFreeMemoryFunc(&m_BillRecords);

		pSetNotifyWndFunc = (SetNotifyWndFunc)::GetProcAddress(m_urldll, "SetNotifyWnd");
		if(pSetNotifyWndFunc != NULL)
			pSetNotifyWndFunc( 0 );

		pSetBillStateFunc = (SetBillStateFunc)::GetProcAddress(m_urldll, "SetBillState");
		if(pSetBillStateFunc != NULL)
			pSetBillStateFunc( 0 );

	}

}


int CBillUrlManager::GetBill(IWebBrowser2* pFatherWebBrowser, IWebBrowser2* pChildWebBrowser, BillData* pData, int& step, HWND hAxControl)
{
	USES_CONVERSION;
	if(pFatherWebBrowser == NULL || NULL == pChildWebBrowser)
		return 0;

	if(m_urldll == NULL)
	{
		wstring dll = GetDllName(pData->aid);
		if(dll.size() > 0)
		{
			wchar_t lpPath[255] = {0};
			GetModuleFileNameW(NULL,lpPath,sizeof(lpPath));
			::PathRemoveFileSpecW(lpPath);

			wstring path(lpPath);

			path += L"\\Config\\Bill\\" + dll;
			m_urldll = ::LoadLibraryW(path.c_str());
			if(m_urldll == NULL)
				return 0;

			pFetchBillFunc = (FetchBillFunc)::GetProcAddress(m_urldll, "FetchBillFunc");
			pFreeMemoryFunc = (FreeMemoryFunc)::GetProcAddress(m_urldll, "FreeMemory");
			m_BillRecords.isFinish = false;	//初始化为没导完账单
		}
		else
			return 0;
	}

	int state = 0;
	if(pFetchBillFunc)
		state = pFetchBillFunc(pFatherWebBrowser, pChildWebBrowser, pData, step, &m_BillRecords);
	else
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_GET_BILL, L"pFetchBillFunc == NULL");
	// 状态为99表明是最后一步并且获取账单成功，当执行完该步骤后，进行存储写入数据库的行为
	if(state == BILL_FINISH_STATE && m_BillRecords.isFinish == true)
	{
		if(strlen(m_BillRecords.tag) != 0)//没抓到账号的话，可以由由这里统一去过滤
		{
			//要增加显示结果
			::SendMessage(hAxControl, WM_AX_END_INFO_DLG, 0, 0);//关闭导入账单

			ShowResultDlg();//显示获取的结果
			//组织显示的文字结构
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_GET_BILL, L"账单交给JS");
			SendBillRecordToJS(pData);
			if(pFreeMemoryFunc)
				pFreeMemoryFunc(&m_BillRecords);
			
			if(m_urldll != NULL)
				FreeLibrary(m_urldll);
			m_urldll = NULL;

			state = BILL_ALL_FINISH;
		}
		else
		{
			state = BILL_COM_ERROR;
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_GET_BILL, L"未获取到标签");
		}

	}
	return state;	
}
void CBillUrlManager::ShowResultDlg()
{
	std::list<LPMSGBILLRESULT> data;

	LPMSGBILLRESULT info1 = new MSGBILLRESULT;
	info1->info = L"获取账号";
	info1->result = true;
	data.push_back(info1);

	USES_CONVERSION;


	list<LPBILLRECORD>::iterator bite = m_BillRecords.BillRecordlist.begin();

	for(;bite != m_BillRecords.BillRecordlist.end();bite ++)
	{	
		LPMSGBILLRESULT info2 = new MSGBILLRESULT;
		if((*bite)->type == RMB)
			info2->info = L"获取人民币账户余额";
		else if((*bite)->type == USD)
			info2->info = L"获取美元账户余额";
		info2->result = true;
		if((*bite)->balance == "F")
			info2->result = false;
		data.push_back(info2);
	}
	bite = m_BillRecords.BillRecordlist.begin();
	for(;bite != m_BillRecords.BillRecordlist.end();bite ++)
	{
		int size = 12;
		if((*bite)->type == RMB)
		{
			list<LPMONTHBILLRECORD>::iterator mite = (*bite)->bills.begin();
			for(; mite != (*bite)->bills.end(); mite ++)
			{			
				LPMSGBILLRESULT pinfo = new MSGBILLRESULT;
				if((*mite)->month.size() > 6)
				{
					pinfo->info = L"获取账单";
				}
				else if((*mite)->month.size() == 6)
				{
					wstring tmonth = A2W((*mite)->month.c_str());
					wstring year = tmonth.substr(0, 4);
					wstring month = tmonth.substr(4, 2);
					pinfo->info = L"获取" + year + L"年" + month + L"月账单";
				}

				if((*mite)->m_isSuccess == true)//过滤掉获取失败的月份
				{
					pinfo->result = true;
				}
				else
					pinfo->result = false;

				data.push_back(pinfo);
			}	
		}
	}

	CShowBillResultDlg dlg(&data);
	HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	dlg.DoModal(hMainFrame);

	std::list<LPMSGBILLRESULT>::iterator dite = data.begin();
	for(;dite != data.end(); dite ++)
	{
		if((*dite) != NULL)
			delete (*dite);
	}
	data.clear();

}

void CBillUrlManager::FilterXml(char* pString, int maxlength)
{
	if(pString == NULL)
		return;
	int length = strlen(pString);
	if(length == 0)
		return;
	if(length > maxlength)
		length = maxlength;
	char* pCur = pString;

	for(int i = 0; i < length; i ++)
	{
		if((*pCur) == '\'')
			*pCur = '\"';
		pCur ++;
	}
}
void CBillUrlManager::SendBillRecordToJS(BillData* pData)
{
	string info;

	info = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<Data>";

	//先组原有带入的数据
	char oinfo[2560] = { 0 };
	sprintf_s(oinfo, 2560, "<InputInfo><FinanceId>%s</FinanceId><AccountId>%I64d</AccountId><Tag>%s</Tag><Type>%d</Type></InputInfo>\r\n", pData->aid.c_str(), pData->accountid, pData->tag.c_str(), pData->type);
	info += oinfo;


	// 组现有的树结构
	sprintf_s(oinfo, 2560, "<OutputInfo><FinanceId>%s</FinanceId><AccountId>%I64d</AccountId><Tag>%s</Tag><Type>%d</Type></OutputInfo>\r\n<BillInfo>", m_BillRecords.aid, m_BillRecords.accountid, m_BillRecords.tag, m_BillRecords.type);
	info += oinfo;	

	list<LPBILLRECORD>::iterator bite = m_BillRecords.BillRecordlist.begin();
	for(;bite != m_BillRecords.BillRecordlist.end(); bite ++)
	{
		info += "<BillRecord>";
		if((*bite)->type == RMB)
			sprintf_s(oinfo, 2560, "<Balance>%s</Balance><AType>RMB</AType>\r\n", (*bite)->balance.c_str());
		else
			sprintf_s(oinfo, 2560, "<Balance>%s</Balance><AType>USD</AType>\r\n", (*bite)->balance.c_str());

		info += oinfo;

		list<LPMONTHBILLRECORD>::iterator mite = (*bite)->bills.begin();
		for(; mite != (*bite)->bills.end(); mite ++)
		{
			if((*mite)->m_isSuccess == true)//过滤掉获取失败的月份
			{
				info += "<MonthRecord>";
				sprintf_s(oinfo, 2560, "<Month>%s</Month>\r\n", (*mite)->month.c_str());
				info += oinfo;

				list<LPTRANRECORD>::iterator rite = (*mite)->TranList.begin();
				for(; rite != (*mite)->TranList.end(); rite ++)
				{
					FilterXml((*rite)->Description, 256);
					FilterXml((*rite)->Payee, 256);

					//测试需要，先做成这样，临时的
					if((*rite)->Amount[0] == '-' &&  pData->type == 2)
						sprintf_s(oinfo, 2560, "<Record><Time>%s</Time><Amount>%s</Amount><Des><![CDATA[%s%s]]></Des><Payee></Payee></Record>\r\n", (*rite)->PostDate, (*rite)->Amount, (*rite)->Description, (*rite)->Payee);
					else
						sprintf_s(oinfo, 2560, "<Record><Time>%s</Time><Amount>%s</Amount><Des><![CDATA[%s]]></Des><Payee><![CDATA[%s]]></Payee></Record>\r\n", (*rite)->PostDate, (*rite)->Amount, (*rite)->Description, (*rite)->Payee);

					info += oinfo;
				}
		
				info += "</MonthRecord>\r\n";
			}
		}

		info += "</BillRecord>\r\n";
	}

	info += "</BillInfo></Data>\r\n";

	char* pBData = new char[info.size() + 1];
	if(pBData == NULL)
		return;

	memset(pBData, 0, info.size() + 1);

	memcpy(pBData, info.c_str(), info.size());
	
	::SendMessageW(CExternalDispatchImpl::m_hAxui, WM_AX_GET_ALL_BILL, NULL, (LPARAM)pBData);
	return;
}