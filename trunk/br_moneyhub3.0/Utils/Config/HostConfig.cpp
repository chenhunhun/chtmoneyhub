#include "stdafx.h"
#include "HostConfig.h"
#include "../../ThirdParty/tinyxml/tinyxml.h"
#include "../Encryption/CHKFile/CHK.h"
#include "ConvertBase.h"
#include "../ListManager/URLList.h"
#include "../RecordProgram/RecordProgram.h"


CHostContainer* CHostContainer::m_Instance = NULL;


CHostContainer* CHostContainer::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CHostContainer();
	return m_Instance;
}

CHostContainer::CHostContainer()
{
	Init(NULL);
}
CHostContainer::~CHostContainer()
{
}

wstring CHostContainer::GetHostName(webconfig host)
{
	map<webconfig,wstring>::iterator ite = m_host.find(host);
	if(ite != m_host.end())
		return (*ite).second;
	return L"";
}
std::string CHostContainer::GetFileContent(wstring strPath,bool bCHK)
{	
	HANDLE hFile = ::CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)\
	{
		CRecordProgram::GetInstance()->FeedbackError(L"Common", GetLastError(), L"读Host文件错误");
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
			CRecordProgram::GetInstance()->FeedbackError(L"Common", GetLastError(), L"读Host文件unPackCHK错误");
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

bool CHostContainer::GetAllHostName(VECTORNPBNAME *pVvecNPB )
{
	wchar_t lpPath[255] = {0};
	GetModuleFileNameW(NULL,lpPath,sizeof(lpPath));
	::PathRemoveFileSpecW(lpPath);

	wstring path(lpPath);

	path += L"\\Config\\info.chk";

	string info = GetFileContent(path.c_str(),true);
	if(info == "")
		return false;

	TiXmlDocument xmlDoc;

	xmlDoc.Parse(info.c_str()); 

	if (xmlDoc.Error())
	{
		CRecordProgram::GetInstance()->FeedbackError(L"Common", xmlDoc.Error(), L"TiXmlDocument读Host");
		return false;
	}

	const TiXmlNode* pRoot = xmlDoc.FirstChild("main"); // ANSI string 
	if (NULL == pRoot)
		return false;

	if(!pVvecNPB)
	{
		const TiXmlNode* pHost = pRoot->FirstChild("host");
		if (NULL == pHost)
			return false;

		std::string strType = pHost->ToElement()->Attribute("type");
		
		const TiXmlNode* pType = pHost->FirstChild(strType.c_str());

		if(NULL == pType)
			return false;

		const TiXmlNode *pWeb = pType->FirstChild("web");
		if(pWeb)
		{
			wstring web = AToW(pWeb->FirstChild()->Value());
			m_host.insert(make_pair(kWeb,web));
		}

		const TiXmlNode *pDownload = pType->FirstChild("download");
		if(pWeb)
		{
			wstring download = AToW(pDownload->FirstChild()->Value());
			m_host.insert(make_pair(kDownload,download));
		}
		const TiXmlNode *pFeedback = pType->FirstChild("feedback");
		if(pWeb)
		{
			wstring feedback = AToW(pFeedback->FirstChild()->Value());
			m_host.insert(make_pair(kFeedback,feedback));
		}
		const TiXmlNode *pAdv = pType->FirstChild("adv");
		if(pWeb)
		{
			wstring adv = AToW(pAdv->FirstChild()->Value());
			m_host.insert(make_pair(kAdv,adv));
		}
		const TiXmlNode *pBenefit = pType->FirstChild("benefit");
		if(pWeb)
		{
			wstring benefit = AToW(pBenefit->FirstChild()->Value());
			m_host.insert(make_pair(kBenefit,benefit));
		}
	}

	///add by bh for get need hookProtect bank name
	const TiXmlNode *pNPB = pRoot->FirstChild("npb");
	if( pNPB && pVvecNPB)
	{
		const TiXmlElement *VerifyList = pNPB->ToElement();
		for(const TiXmlElement *Binary = VerifyList->FirstChildElement();
			Binary; Binary = Binary->NextSiblingElement())
		{
			const TiXmlAttribute *IDAttribute = Binary->FirstAttribute();
			if(IDAttribute)
			{
				pVvecNPB->push_back(AToW(IDAttribute->Value()));
			}
 		}
	}

	if(!pVvecNPB)
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(L"Common", 3000, L"读取urllist");
		const TiXmlNode *pURLList = pRoot->FirstChild("urllist"); 
		if(pURLList)
		{
			CURLList::GetInstance()->ReadData(pURLList);
		}
	}

	return true;
}
void CHostContainer::Init(VECTORNPBNAME *pVvecNPB)
{
	if(!GetAllHostName(pVvecNPB))//如果初始化失败，采用默认值，默认值为正式版的地址
	{
		m_host.insert(make_pair(kWeb,L"http://www.moneyhub.cn/"));
		m_host.insert(make_pair(kDownload,L"http://download.moneyhub.cn/"));
		m_host.insert(make_pair(kFeedback,L"http://cloud.moneyhub.cn/"));
		m_host.insert(make_pair(kAdv,L"adv.moneyhub.cn"));
		m_host.insert(make_pair(kBenefit,L"http://benefit.moneyhub.cn/"));
	}

}