#include "stdafx.h"
#include "URLList.h"
#include "ConvertBase.h"

CURLList* CURLList::m_Instance = NULL;


CURLList* CURLList::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CURLList();
	return m_Instance;
}

CURLList::CURLList(void)
{
	BillFlag.clear();
}

CURLList::~CURLList(void)
{
}

bool CURLList::ReadData(const TiXmlNode *pUrlList)
{
	m_WebsiteData.clear();
	WebsiteType eWebsiteType;
	std::string strType = pUrlList->ToElement()->Attribute("name");
	if (strType == "banks")
		eWebsiteType = Website_Bank;
	else
		ATLASSERT(0);

	for (const TiXmlNode *pSite = pUrlList->FirstChild("site"); pSite != NULL; pSite = pUrlList->IterateChildren("site", pSite))
	{
		CWebsiteData *pWebsiteData = new CWebsiteData;
		pWebsiteData->m_strID = AToW(pSite->ToElement()->Attribute("id"));
		WebDataMap::iterator it = m_WebsiteData.find (pWebsiteData->m_strID);
		if (it == m_WebsiteData.end()) // 如果不存在
		{
			pWebsiteData->m_strName = AToW(pSite->ToElement()->Attribute("name"));

			pWebsiteData->m_bNoClose = false;
			if (pSite->ToElement()->Attribute("noclose") != NULL)
			{
				CStringW strNoClose = AToW(pSite->ToElement()->Attribute("noclose")).c_str();
				strNoClose.Trim();
				if (strNoClose.CompareNoCase(L"true") == 0)
					pWebsiteData->m_bNoClose = true;
			}

			pWebsiteData->m_bHasSubTab = false;
			if (pSite->ToElement()->Attribute("noSubTab") != NULL)
			{
				CStringW strNoSubTab = AToW(pSite->ToElement()->Attribute("noSubTab")).c_str();
				strNoSubTab.Trim();
				if (strNoSubTab.CompareNoCase(L"true") == 0)
					pWebsiteData->m_bHasSubTab = true;
			}

			pWebsiteData->m_eWebsiteType = eWebsiteType;
			const TiXmlNode *pDomainList = pSite->FirstChild("domains");
			if (pDomainList)
			{
				for (const TiXmlNode *pDomain = pDomainList->FirstChild("domain"); pDomain != NULL; pDomain = pDomainList->IterateChildren("domain", pDomain))
				{
					std::wstring strDomain = AToW(pDomain->ToElement()->Attribute("name"));
					pWebsiteData->m_urllist.push_back(strDomain);
				}
				m_WebsiteData.insert(std::make_pair(pWebsiteData->m_strID, pWebsiteData));
			}
			else
			{
				delete pWebsiteData;
				pWebsiteData = NULL;
			}
		}
		else
		{			
			MessageBox(NULL, pWebsiteData->m_strID.c_str(), L"Info配置文件ID重复", MB_OK);
			delete pWebsiteData;
			pWebsiteData = NULL;

		}

	}
	return true;
}
const CWebsiteData* CURLList::GetData(wstring szDomain, LPCTSTR lpszUrl) const
{
	CString strClearUrl = CListManager::ExtractClearDomain(lpszUrl);
	
	WebDataMap::const_iterator it = m_WebsiteData.begin();

	if(szDomain.size() == 7)
	{
		it = m_WebsiteData.find(szDomain);
		if(it != m_WebsiteData.end())
			return it->second;
	}
		
	for (; it != m_WebsiteData.end(); ++it)
	{
		CWebsiteData* pWebsiteData = it->second;

		list<wstring>::iterator lite =  pWebsiteData->m_urllist.begin();
		for(; lite != pWebsiteData->m_urllist.end(); lite ++)
		{
			CString strDomain = lite->c_str();		

			int nPattern = strDomain.Find(_T("/*"));
			if (nPattern == -1)
			{
				if (strDomain.Mid(0, 2) != _T("*."))
				{
					if (strDomain.CompareNoCase(strClearUrl) == 0)
						return pWebsiteData;
				}
				else
				{
					CString strDomain1 = strDomain.Mid(1);
					CString strClearUrl1 = _T(".");
					strClearUrl1 += strClearUrl;

					if (strDomain.CompareNoCase((LPCTSTR)strClearUrl + strClearUrl.GetLength() - strDomain1.GetLength()) == 0)
						return pWebsiteData;
				}
			}
			else 
			{
				strDomain = strDomain.Mid(0, nPattern);
				if (strDomain.Mid(0, 2) != _T("*."))
				{
					if (_tcsnicmp(strDomain, strClearUrl, strDomain.GetLength()) == 0)
						return pWebsiteData;
				}
				else
				{
					CString strDomain1 = strDomain.Mid(1);
					CString strClearUrl1 = _T(".");
					strClearUrl1 += strClearUrl;

					strDomain1.MakeLower();
					strClearUrl1.MakeLower();

					if (strClearUrl1.Find(strDomain1) != -1)
						return pWebsiteData;
				}
			}
		}

	}

	return NULL;
}

