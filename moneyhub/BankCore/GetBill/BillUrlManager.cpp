#include "stdafx.h"
#include "BillUrlManager.h"
#include "../../BankData/BankData.h"
#include "ConvertBase.h"
#include "../../Encryption/CHKFile/CHK.h"
#include <list>
#include "../MyError.h"

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

map<int, wstring>* CBillUrlManager::GetUrlMap(string aid, int type)
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
					return &(site->url);
			}
		}
	}
	return NULL;
}

wstring CBillUrlManager::GetBillUrl(string aid, int type, int step)
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
				{
					map<int, wstring>::iterator mite;
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

	path += L"\\Config\\BillUrl.chk";

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
		bData.id = pSite->ToElement()->Attribute("id");
		bData.name = AToW(pSite->ToElement()->Attribute("name"));
		bData.dll = AToW(pSite->ToElement()->Attribute("dll"));
		
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
	map<DWORD, BillData*>::iterator ite;
	ite = m_billTid.find(pid);

	if(ite != m_billTid.end())
	{
		return ite->second;
	}
	return NULL;
}

bool CBillUrlManager::DeleteBillTid(DWORD pid)
{
	map<DWORD, BillData*>::iterator ite;
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

void CBillUrlManager::FreeDll()
{
	if(m_urldll != NULL)
		FreeLibrary(m_urldll);
	m_urldll = NULL;
}
// 动态库中处理网页内容，模仿操作的接口
typedef int (WINAPI * FetchBillFunc)(IWebBrowser2* pWebBrowser, BillData* pData, int step, std::list<BILLRECORD*>** plRecords);
FetchBillFunc pFetchBillFunc = NULL;
// 动态库中释放在动态库中申请的内存的接口
typedef int (WINAPI * FreeMemoryFunc)(std::list<BILLRECORD*>** plRecords);
FreeMemoryFunc pFreeMemoryFunc = NULL;

int CBillUrlManager::GetBill(IWebBrowser2* pWebBrowser, BillData* pData, int step)
{
	USES_CONVERSION;
	if(pWebBrowser == NULL)
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
		}
		else
			return 0;
	}



	std::list<BILLRECORD*>* plRecords;
	int state = 0;
	if(pFetchBillFunc)
		state = pFetchBillFunc(pWebBrowser, pData, step, &plRecords);
	else
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_GET_BILL, L"pFetchBillFunc == NULL");
	// 状态为99表明是最后一步并且获取账单成功，当执行完该步骤后，进行存储写入数据库的行为
	if(state == 99)
	{
		std::list<BILLRECORD*>::iterator ite = (plRecords)->begin();
		for(;ite != (plRecords)->end(); ite ++)
		{
			if((*ite) != NULL)
			{
				CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_GET_BILL, L"账单开始写入数据库");
				WriteBillRecordToDB((*(*ite)));	
			}
		}
		if(pFreeMemoryFunc)
			pFreeMemoryFunc(&plRecords);
		
		if(m_urldll != NULL)
			FreeLibrary(m_urldll);
		m_urldll = NULL;
	}
	return state;

	
}

void CBillUrlManager::WriteBillRecordToDB(BILLRECORD& TRecord)
{
	CBankData::GetInstance()->InsertGetBillData(TRecord);
	return;
}