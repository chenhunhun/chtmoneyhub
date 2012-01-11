#include "stdafx.h"
#include "HostConfig.h"

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

bool CHostContainer::IsUrlInUrlError(wstring url)
{
	if(find(m_urlError.begin(), m_urlError.end(), url) == m_urlError.end())
		return false;
	else
		return true;

}

bool CHostContainer::ReadUrlData(const TiXmlNode *pErrorHtml)
{
	const TiXmlNode *pSite = pErrorHtml->FirstChild("site");
	if( pSite != NULL ) 
	{
		wstring id =  AToW(pSite->ToElement()->Attribute("id"));
		if(id == L"urlerror")
		{
			const TiXmlNode *pDomainList = pSite->FirstChild("domains");
			if (pDomainList)
			{
				for (const TiXmlNode *pDomain = pDomainList->FirstChild("domain"); pDomain != NULL; pDomain = pDomainList->IterateChildren("domain", pDomain))
				{
					std::wstring strDomain = AToW(pDomain->ToElement()->Attribute("name"));
					m_urlError.push_back(strDomain);
				}
			}

		}
	}
	return true;
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

	path += L"\\Config\\info.mchk";

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

		const TiXmlNode *pJsWeb = pType->FirstChild("jsweb");
		if(pJsWeb)
		{
			wstring jsweb = AToW(pJsWeb->FirstChild()->Value());
			m_host.insert(make_pair(kJsWeb,jsweb));
		}

		const TiXmlNode *pWeb = pType->FirstChild("web");
		if(pWeb)
		{
			wstring web = AToW(pWeb->FirstChild()->Value());
			m_host.insert(make_pair(kWeb,web));
		}

		const TiXmlNode *pHelp = pType->FirstChild("help");
		if(pHelp)
		{
			wstring help = AToW(pHelp->FirstChild()->Value());
			m_host.insert(make_pair(kHelp,help));
		}
		const TiXmlNode *pFeedback = pType->FirstChild("feedback");
		if(pFeedback)
		{
			wstring feedback = AToW(pFeedback->FirstChild()->Value());
			m_host.insert(make_pair(kFeedback,feedback));
		}
		const TiXmlNode *pUninstall = pType->FirstChild("uninstall");
		if(pUninstall)
		{
			wstring uninstall = AToW(pUninstall->FirstChild()->Value());
			m_host.insert(make_pair(kUninstall,uninstall));
		}
		const TiXmlNode *pDownloadmode = pType->FirstChild("downloadmode");
		if(pDownloadmode)
		{
			wstring downloadmode = AToW(pDownloadmode->FirstChild()->Value());
			m_host.insert(make_pair(kDownloadMode,downloadmode));
		}
		
		const TiXmlNode *pPdownloadinstall = pType->FirstChild("pdownloadinstall");
		if(pPdownloadinstall)
		{
			wstring pdownloadinstall = AToW(pPdownloadinstall->FirstChild()->Value());
			m_host.insert(make_pair(kPDownloadInstall,pdownloadinstall));
		}

		//8
		const TiXmlNode *pPupgrade = pType->FirstChild("pupgrade");
		if(pPupgrade)
		{
			wstring pupgrade = AToW(pPupgrade->FirstChild()->Value());
			m_host.insert(make_pair(kPUpgrade,pupgrade));
		}

		const TiXmlNode *pPukey = pType->FirstChild("pukey");
		if(pPukey)
		{
			wstring pukey = AToW(pPukey->FirstChild()->Value());
			m_host.insert(make_pair(kPUkey,pukey));
		}

		const TiXmlNode *pPgetfile = pType->FirstChild("pgetfile");
		if(pPgetfile)
		{
			wstring pgetfile = AToW(pPgetfile->FirstChild()->Value());
			m_host.insert(make_pair(kPGetFile,pgetfile));
		}

		const TiXmlNode *pPdatafeedback = pType->FirstChild("pdatafeedback");
		if(pPdatafeedback)
		{
			wstring pdatafeedback = AToW(pPdatafeedback->FirstChild()->Value());
			m_host.insert(make_pair(kPDataFeedback,pdatafeedback));
		}

		//12  pautologon
		const TiXmlNode *pPautologon = pType->FirstChild("pautologon");
		if(pPautologon)
		{
			wstring pautologon = AToW(pPautologon->FirstChild()->Value());
			m_host.insert(make_pair(kPAutoLogon,pautologon));
		}

		const TiXmlNode *pPmanulogon= pType->FirstChild("pmanulogon");
		if(pPmanulogon)
		{
			wstring pmanulogon = AToW(pPmanulogon->FirstChild()->Value());
			m_host.insert(make_pair(kPManuLogon,pmanulogon));
		}

		const TiXmlNode *pPbeforeregistration = pType->FirstChild("pbeforeregistration");
		if(pPbeforeregistration)
		{
			wstring pbeforeregistration = AToW(pPbeforeregistration->FirstChild()->Value());
			m_host.insert(make_pair(kPBeforeRegistration,pbeforeregistration));
		}
		const TiXmlNode *pPsendmailverify = pType->FirstChild("psendmailverify");
		if(pPsendmailverify)
		{
			wstring psendmailverify = AToW(pPsendmailverify->FirstChild()->Value());
			m_host.insert(make_pair(kPSendMailVerify,psendmailverify));
		}
		const TiXmlNode *pPregistration = pType->FirstChild("pregistration");
		if(pPregistration)
		{
			wstring pregistration = AToW(pPregistration->FirstChild()->Value());
			m_host.insert(make_pair(kPRegistration,pregistration));
		}
		// 17
		const TiXmlNode *pPgetlist = pType->FirstChild("pgetlist");
		if(pPgetlist)
		{
			wstring pgetlist = AToW(pPgetlist->FirstChild()->Value());
			m_host.insert(make_pair(kPGetList,pgetlist));
		}
		const TiXmlNode *pPgetdownloaddata = pType->FirstChild("pgetdownloaddata");
		if(pPgetdownloaddata)
		{
			wstring pgetdownloaddata = AToW(pPgetdownloaddata->FirstChild()->Value());
			m_host.insert(make_pair(kPGetDownloadData,pgetdownloaddata));
		}
		const TiXmlNode *pPgetuploaddata = pType->FirstChild("pgetuploaddata");
		if(pPgetuploaddata)
		{
			wstring pgetuploaddata = AToW(pPgetuploaddata->FirstChild()->Value());
			m_host.insert(make_pair(kPGetUploadData,pgetuploaddata));
		}
		const TiXmlNode *pPchangemail = pType->FirstChild("pchangemail");
		if(pPchangemail)
		{
			wstring pchangemail = AToW(pPchangemail->FirstChild()->Value());
			m_host.insert(make_pair(kPChangeMail,pchangemail));
		}

		// 21
		const TiXmlNode *pPchangepassword = pType->FirstChild("pchangepassword");
		if(pPchangepassword)
		{
			wstring pchangepassword = AToW(pPchangepassword->FirstChild()->Value());
			m_host.insert(make_pair(kPChangePassword,pchangepassword));
		}

		const TiXmlNode *pPmailverify = pType->FirstChild("pmailverify");
		if(pPmailverify)
		{
			wstring pmailverify = AToW(pPmailverify->FirstChild()->Value());
			m_host.insert(make_pair(kPMailVerify,pmailverify));
		}

		const TiXmlNode *pPuserunlock = pType->FirstChild("puserunlock");
		if(pPuserunlock)
		{
			wstring puserunlock = AToW(pPuserunlock->FirstChild()->Value());
			m_host.insert(make_pair(kPUserUnlock,puserunlock));
		}

		const TiXmlNode *pServerTime = pType->FirstChild("pusersevertime");
		if(pServerTime)
		{
			wstring servertime = AToW(pServerTime->FirstChild()->Value());
			m_host.insert(make_pair(kPUserServerTime,servertime));
		}

		const TiXmlNode *pSendMail = pType->FirstChild("psendfindmail");
		if(pSendMail)
		{
			wstring sendmail = AToW(pSendMail->FirstChild()->Value());
			m_host.insert(make_pair(kPSendFindMail,sendmail));
		}
		
		const TiXmlNode *pCheckOPT = pType->FirstChild("pcheckopt");
		if(pCheckOPT)
		{
			wstring checkopt = AToW(pCheckOPT->FirstChild()->Value());
			m_host.insert(make_pair(kPCheckOPT,checkopt));
		}
		
		const TiXmlNode *pInitPwd = pType->FirstChild("pinitpwd");
		if(pInitPwd)
		{
			wstring initpwd = AToW(pInitPwd->FirstChild()->Value());
			m_host.insert(make_pair(kPInitPassword,initpwd));
		}

		const TiXmlNode *pexchangerate = pType->FirstChild("exchangerate");
		if(pexchangerate)
		{
			wstring exchangerate = AToW(pexchangerate->FirstChild()->Value());
			m_host.insert(make_pair(kExchangeRate,exchangerate));
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

		const TiXmlNode *pErrorHtml = pRoot->FirstChild("errorhtml"); 
		//读取errorhtml处理的配置文件
		if(pErrorHtml)
		{
			ReadUrlData(pErrorHtml);
		}

	}

	return true;
}
void CHostContainer::Init(VECTORNPBNAME *pVvecNPB)
{
	if(!GetAllHostName(pVvecNPB))//如果初始化失败，采用默认值，默认值为正式版的地址
	{
		CRecordProgram::GetInstance()->FeedbackError(L"Common", 0, L"读Host文件错误");
		exit(0);
	}
}