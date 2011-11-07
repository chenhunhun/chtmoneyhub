#include "stdafx.h"
#include "ConvertBase.h"
#include "Util.h"
#include "../../ThirdParty/tinyxml/tinyxml.h"
#include "ResourceManager.h"
#include "ListManager.h"
#include "../../Encryption/SHA1/sha.h"
#include "urlencode.h"
#include "Util.h"
#include "../../BankUI/Util/Util.h"
#include "../../BankCore/MyError.h"
#include "..\RecordProgram\RecordProgram.h"
#include "windows.h"
#pragma comment(lib,"Kernel32.lib")

std::vector<std::wstring> uacPopVec;
void CListManager::GetSystemVersion()
{ 
	//初始化系统对应对列
	m_sysTypeList.insert(make_pair(L"All",sAll));
	m_sysTypeList.insert(make_pair(L"UnSupported",sUnSupported));
	m_sysTypeList.insert(make_pair(L"WinNT",sWinNT));
	m_sysTypeList.insert(make_pair(L"Win2000",sWin2000));
	m_sysTypeList.insert(make_pair(L"WinXP",sWinXP));
	m_sysTypeList.insert(make_pair(L"Win2003",sWin2003));
	m_sysTypeList.insert(make_pair(L"WinVista",sWinVista));
	m_sysTypeList.insert(make_pair(L"Win2008",sWin2008));
	m_sysTypeList.insert(make_pair(L"Win7",sWin7));

	m_tArchite = 32;
	SYSTEM_INFO info;                                   //用SYSTEM_INFO结构判断64位AMD处理器 
    GetSystemInfo(&info);                               //调用GetSystemInfo函数填充结构 
    OSVERSIONINFOEXA os; 
    os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEXA);  /*在调用函数前必须用sizeof(OSVERSIONINFOEXA)填充dwOSVersionInfoSize结构成员*/ 
    if(GetVersionExA((OSVERSIONINFOA *)&os))                  /*调用GetVersionEx函数OSVERSIONINFOEX结构必须将指针类型强制转换*/
    { 
        //下面根据版本信息判断操作系统名称 
        switch(os.dwMajorVersion){                        //判断主版本号 
            case 4: 
                switch(os.dwMinorVersion){                //判断次版本号 
                    case 0: 
                        if(os.dwPlatformId==VER_PLATFORM_WIN32_NT) 
                            m_tSystem = sWinNT;                //1996年7月发布 
                        else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) 
                            m_tSystem = sUnSupported; 
                        break; 
                    case 10: 
                        m_tSystem = sUnSupported; 
                        break; 
                    case 90: 
                        m_tSystem = sUnSupported;
                        break; 
                } 
                break; 
            case 5: 
                switch(os.dwMinorVersion){               //再比较dwMinorVersion的值 
                    case 0: 
						m_tSystem = sWin2000;                  //1999年12月发布 
                        break; 
                    case 1: 
						m_tSystem = sWinXP;                  //2001年8月发布 
                        break; 
                    case 2: 
                        if(os.wProductType==VER_NT_WORKSTATION)
						{
							m_tSystem = sWinXP; 
						}
                        else if(GetSystemMetrics(SM_SERVERR2)==0) 
							m_tSystem = sWin2003;                                   //2003年3月发布 
                        else if(GetSystemMetrics(SM_SERVERR2)!=0) 
							m_tSystem = sWin2003;
                        break; 
                } 
                break; 
            case 6: 
                switch(os.dwMinorVersion){ 
                    case 0: 
                        if(os.wProductType==VER_NT_WORKSTATION)/*
                        VER_NT_WORKSTATION是桌面系统 */
							m_tSystem = sWinVista;
                        else
							m_tSystem = sWin2008;                            
                        break; 
                    case 1: 
                        if(os.wProductType==VER_NT_WORKSTATION) 
							m_tSystem = sWin7;
                        else 
							m_tSystem = sWin2008; 
                        break; 
                }
                break; 
			default: 
				{
					CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_XML_FILE, L"GetSystemVersion判定版本时发生异常！");
					m_tSystem = sUnSupported; 
				}
        } 
    }
	else
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_XML_FILE, L"GetSystemVersion时发生异常！");
	}

	BOOL bIsWow64 = FALSE;
	::IsWow64Process(GetCurrentProcess(),&bIsWow64);
	if(bIsWow64 == TRUE)
		m_tArchite = 64;

	return;
}


#define BANKDIRECTORY       L"BankInfo\\banks\\"
#define BANKNOTRAVERSE      L"main"

CListManager::CListManager(bool bBankCore) :m_bBankCore(bBankCore), m_hRegQueryMutex(NULL)// gao 2010-12-13
{
	m_result = false;
	//获取当前系统的版本信息
	GetSystemVersion();
 
	// 读取配置文件
	std::string strXMLData = CResourceManager::_()->GetRootXMLFile(_T("main"));
	
	TiXmlDocument xmlDoc;
	xmlDoc.Parse(strXMLData.c_str());
	if (xmlDoc.Error())
	{
		if (m_bBankCore)
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_XML_FILE, L"CListManager中GetRootXMLFile发生异常！");
		else
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_UI, ERR_XML_FILE, L"CListManager中GetRootXMLFile发生异常！");

		return;
	}
	const TiXmlNode *pRoot = xmlDoc.FirstChild("main");

	if (!pRoot)
	{
		if (m_bBankCore)
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_XML_FILE, L"CListManager中XML文件不存在根结点！");
		else
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_UI, ERR_XML_FILE, L"CListManager中XML文件不存在根结点！");
		return;
	}

	// 对文件内容进行遍历，
	const TiXmlNode *pReq = pRoot->FirstChild("requirements"); 
	if(pReq != NULL)
	{
		if (m_bBankCore)
		{
			CWebsiteData *pWebsiteData = new CWebsiteData;
			pWebsiteData->m_strID = L"main";
			pWebsiteData->m_eWebsiteType = Website_Bank;
			pWebsiteData->m_strName = L"IE";
			pWebsiteData->m_bNoClose = false;
			pWebsiteData->m_bHasSubTab = false;
			for (const TiXmlNode *pRequire = pReq->FirstChild("require"); pRequire != NULL; pRequire = pReq->IterateChildren("require", pRequire))
			{
				CRequirement *pReq = new CRequirement(pRequire, pWebsiteData);
				pWebsiteData->m_Requirements.push_back(pReq);
			}

			m_WebsiteData.insert(std::make_pair(pWebsiteData->m_strID, pWebsiteData)); // 插入
		}
	}

	///////////////////////////银行子文件 info.mchk 单独遍历
	std::wstring wcsPath = getModulePathForSearch();
	wcsPath += BANKDIRECTORY;	

	m_pFavBkOper = new CFavBankOperator ();
	ATLASSERT (NULL != m_pFavBkOper);

	m_result = true;
	// 遍历Bank目录下的所有文件夹，并分析了各个银选择的info.mchk文件
	if (m_bBankCore)
	{
		if (!traverseBankInfo((LPWSTR)wcsPath.c_str()))
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_XML_FILE, L"CListManager中traverseBankInfo错误！");
			return;// gao 2010-12-13
		}
	}
	////////////////////////////



	for (const TiXmlNode* pDownloader = pRoot->FirstChild("downloader"); pDownloader != NULL; pDownloader = pRoot->IterateChildren("downloader", pDownloader))
	{
		for (const TiXmlNode* pFilter = pDownloader->FirstChild("filter"); pFilter != NULL; pFilter = pDownloader->IterateChildren("filter", pFilter))
		{
			const TiXmlElement* pToElement = pFilter->ToElement();
			m_DldFilterData.Add(pToElement->GetText());
		}
	}

	// 计算HMAC
	CalculateHMac(m_hmac);
}

CListManager::~CListManager()
{
	if (NULL != m_pFavBkOper)
	{
		delete m_pFavBkOper;
		m_pFavBkOper = NULL;
	}
}

std::wstring CListManager::getModulePathForSearch()
{
	std::wstring wcsPath ;
	wchar_t      wPath[255];

// 	ExpandEnvironmentStringsW(L"%appdata%",wPath,_countof(wPath) );
// 	wcsPath = wPath;
// 	wcsPath += L"\\MoneyHub\\";
	GetModuleFileNameW(NULL,wPath,_countof(wPath) );
	*( wcsrchr(wPath,L'\\')+1 ) = L'\0';
	wcsPath = wPath;

	return wcsPath;
}


bool CListManager::traverseBankInfo(LPWSTR path, LPWSTR parentPath) // gao 2010-12-13
{
	WIN32_FIND_DATAW  fw;
	
	std::wstring  wcsPath = path ;
	wcsPath += L"*.*";
	
	HANDLE hFind= FindFirstFileW(wcsPath.c_str(),&fw);
	if(hFind == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		if(wcscmp(fw.cFileName,L".") == 0 || wcscmp(fw.cFileName,L"..") == 0 || wcscmp(fw.cFileName,L".svn") == 0)
			continue;
 
		if(fw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::wstring wStr =   path;
			wStr              +=  fw.cFileName;
			wStr              +=  L"\\";

			std::wstring wP  ;
			if(parentPath == NULL)
				wP = fw.cFileName;
			else
			{
				wP = parentPath;
				wP += L'\\';
				wP += fw.cFileName;
			}

			if (!traverseBankInfo((LPWSTR)wStr.c_str(), (LPWSTR)wP.c_str()))
				return false;
		}
		else  
		{
			if(wcscmp(fw.cFileName,L"info.mchk") != 0)
				continue;

			std::string strBankID = CFavBankOperator::GetBankIDOrBankName (WToA(parentPath), false); // 要获取的银行ID，所以传入false
			if (m_pFavBkOper->IsInFavBankSet (strBankID))
			{
				AddANewFavBank (parentPath);
			}
			
		}
	}
	while( FindNextFile(hFind,&fw) );  

	FindClose(hFind);

	return true;
}


//////////////////////////////////////////////////////////////////////////

static CListManager *g_pListManager = NULL;

CListManager* CListManager::_()
{
	return g_pListManager;
}

void CListManager::Initialize(bool bBankCore) // gao 2010-12-13
{
	g_pListManager = new CListManager(bBankCore);
}

//////////////////////////////////////////////////////////////////////////


void CWebsiteData::StartUAC(std::wstring appid)
{
	std::vector<std::wstring>::iterator result = find( uacPopVec.begin( ), uacPopVec.end( ), appid ); //查找3
	if ( result == uacPopVec.end( ) ) //没找到
		uacPopVec.push_back(appid);
	else //找到
		return;

	WCHAR szPath[MAX_PATH] ={0};
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	::PathRemoveFileSpecW(szPath);

	wstring path(szPath);
	path += L"\\MoneyHub.exe -startuac ";

	path += appid.c_str();

	STARTUPINFO si;	
	PROCESS_INFORMATION pi;	
	ZeroMemory( &pi, sizeof(pi) );	
	ZeroMemory( &si, sizeof(si) );	
	si.cb = sizeof(si);	
	//带参数打开
	if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		//WaitForSingleObject(pi.hProcess, INFINITE); //等待完成
		CloseHandle( pi.hProcess );		
		CloseHandle( pi.hThread );		
	}
}



CImage& CWebsiteData::GetLogo() const
{
	if (bmpLogo.IsNull())
	{
		TCHAR szPath[MAX_PATH];
		_stprintf_s(szPath, _T("%s\\Config\\UILogos\\%s.jpg"), ::GetModulePath(), m_strID.c_str());

		Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromFile(szPath);
		if (pBitmap)
		{
			if (pBitmap->GetLastStatus() == Gdiplus::Ok)
			{
				HBITMAP hBitmap = NULL;
				pBitmap->GetHBITMAP(NULL, &hBitmap);
				bmpLogo.Attach(hBitmap);
			}
		}
	}
	return bmpLogo;
}

void CListManager::CalculateHMac(unsigned char* pHMAC) const
{
	std::wstring strDomains = L"magiccode";

	WebDataMap::const_iterator it = m_WebsiteData.begin();
	for (; it != m_WebsiteData.end(); ++it)
		strDomains += it->first;

	unsigned char key[16];
	memset(key,0x3e,16);
	HMAC_SHA1((unsigned char *)(strDomains.c_str()), strDomains.size() * 2, key, 16, pHMAC);
}

void CListManager::CheckHMAC() const
{
	unsigned char md[20];
	CalculateHMac(md);

	if (memcmp(md, m_hmac, 20) != 0)
	{
		MessageBox(NULL, _T("URL被篡改了，程序中止运行"), _T("财金汇"), MB_OK | MB_ICONSTOP);
		exit(-1);
	}
}

CTuoImage& CWebsiteData::GetLogoPng(bool bSelected) const
{
	if (bSelected)
	{
		if (m_bmpLogoPng.IsNull())
		{
			TCHAR szPath[MAX_PATH];
			_stprintf_s(szPath, _T("%s\\Config\\UILogos\\%s.png"), ::GetModulePath(), m_strID.c_str());
			m_bmpLogoPng.LoadFromFile(szPath);
		}

		return m_bmpLogoPng;
	}
	else
	{
		if (m_bmpLogoPng_gray.IsNull())
		{
			TCHAR szPath[MAX_PATH];
			_stprintf_s(szPath, _T("%s\\Config\\UILogos\\%s2.png"), ::GetModulePath(), m_strID.c_str());
			m_bmpLogoPng_gray.LoadFromFile(szPath);
		}

		return m_bmpLogoPng_gray;
	}
}

CString CListManager::GetModulePath()
{
	TCHAR szPath[1024];
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	*(_tcsrchr(szPath, '\\') + 1) = 0;

	return szPath;
}

CString CListManager::GetAppData()
{
	TCHAR szBuf[1024];
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szBuf);
	
	CString strAppData = szBuf;
	strAppData += "\\MoneyHub\\";

	return strAppData;
}

// 去掉参数，锚，Decode
CString CListManager::ExtractClearDomain(LPCTSTR lpszUrl)
{
	USES_CONVERSION;

	//////////////////////////////////////////////////////////////////////////
	// local file
	CString strUrl = lpszUrl;
	strUrl.Replace('\\', '/');

	std::string std_url = CT2A(strUrl);
	std::string std_dec_url = UrlDecodeString(std_url);
	strUrl = A2CT(std_dec_url.c_str());

	if (_tcsnicmp(strUrl, _T("file:///"), _tcslen(_T("file:///"))) == 0)
		strUrl = strUrl.Mid(_tcslen(_T("file:///")));
	else if (_tcsnicmp(strUrl, _T("file://"), _tcslen(_T("file://"))) == 0)
		strUrl = strUrl.Mid(_tcslen(_T("file://")));

	CString strModulePath = GetModulePath() + "Html\\";
	strModulePath.Replace('\\', '/');

	if (strModulePath.GetLength() < strUrl.GetLength()
		&& _tcsnicmp(strModulePath, strUrl, strModulePath.GetLength()) == 0)
	{
		CString strDomain = strUrl.Mid(strModulePath.GetLength());
		int nEnd = strDomain.Find('/');
		if (nEnd != -1)
			strDomain = strDomain.Mid(0, nEnd);

		return strDomain;
	}

	CString strAppData = GetAppData();
	strAppData.Replace('\\', '/');

	if (strAppData.GetLength() < strUrl.GetLength()
		&& _tcsnicmp(strAppData, strUrl, strAppData.GetLength()) == 0)
	{
		CString strDomain = strUrl.Mid(strAppData.GetLength());
		int nEnd = strDomain.Find('/');
		if (nEnd != -1)
			strDomain = strDomain.Mid(0, nEnd);

		return strDomain;
	}

	//////////////////////////////////////////////////////////////////////////
	// web

	//CString strClearUrl(A2CT(UrlDecodeString(std::string(CT2A(lpszUrl))).c_str()));
	CString strClearUrl(lpszUrl);
	strClearUrl.MakeLower();
	strClearUrl.Trim();

	if (_tcsnicmp(strClearUrl, _T("http://"), 7) == 0)
		strClearUrl = strClearUrl.Mid(7);
	else if (_tcsnicmp(strClearUrl, _T("https://"), 8) == 0)
		strClearUrl = strClearUrl.Mid(8);
	else
		return _T("");

	if (_tcsnicmp(strClearUrl, _T("www."), 4) == 0)
		strClearUrl = strClearUrl.Mid(4);

	int nQuestMark = strClearUrl.Find('?');
	int nPoundMark = strClearUrl.Find('#');
	if (nQuestMark != -1 || nPoundMark != -1)
	{
		int nEnd = -1;
		if (nQuestMark != -1)
			nEnd = nQuestMark;
		if (nPoundMark != -1 && nEnd != -1)
			nEnd = min(nPoundMark, nEnd);
		else if (nPoundMark != -1 && nEnd == -1)
			nEnd = nPoundMark;

		strClearUrl = strClearUrl.Mid(0, nEnd);
	}

	return strClearUrl.TrimRight('/');
}

//bool CListManager::IsHomePage(LPCTSTR lpszUrl)
//{
//	CString strUrl = lpszUrl;
//	strUrl.Replace('\\', '/');
//
//	int nQuestMark = strUrl.Find('?');
//	int nPoundMark = strUrl.Find('#');
//	if (nQuestMark != -1 || nPoundMark != -1)
//	{
//		int nEnd = -1;
//		if (nQuestMark != -1)
//			nEnd = nQuestMark;
//		if (nPoundMark != -1 && nEnd != -1)
//			nEnd = min(nPoundMark, nEnd);
//		else if (nPoundMark != -1 && nEnd == -1)
//			nEnd = nPoundMark;
//
//		strUrl = strUrl.Mid(0, nEnd);
//	}
//
//	if (_tcsicmp((LPCTSTR)strUrl + _tcslen(strUrl) - 26, _T("/Html/StartPage/index.html")) == 0)
//		return true;
//	else
//		return false;
//}

int CListManager::GetDefaultPageIndex(LPCTSTR lpszUrl)
{
	CString strUrl = lpszUrl;
	strUrl.Replace('\\', '/');

	int nQuestMark = strUrl.Find('?');
	int nPoundMark = strUrl.Find('#');
	if (nQuestMark != -1 || nPoundMark != -1)
	{
		int nEnd = -1;
		if (nQuestMark != -1)
			nEnd = nQuestMark;
		if (nPoundMark != -1 && nEnd != -1)
			nEnd = min(nPoundMark, nEnd);
		else if (nPoundMark != -1 && nEnd == -1)
			nEnd = nPoundMark;

		strUrl = strUrl.Mid(0, nEnd);
	}

	CString strPartUrl;

	strPartUrl = _T("/Html/StartPage/index.html");
	if (_tcsicmp((LPCTSTR)strUrl + _tcslen(strUrl) - strPartUrl.GetLength(), strPartUrl) == 0)
		return kStartPage;

	strPartUrl = _T("/Html/ToolsPage/index.html");
	if (_tcsicmp((LPCTSTR)strUrl + _tcslen(strUrl) - strPartUrl.GetLength(), strPartUrl) == 0)
		return kToolsPage;

	/*strPartUrl = _T("/Html/CouponPage/index.html");
	if (_tcsicmp((LPCTSTR)strUrl + _tcslen(strUrl) - strPartUrl.GetLength(), strPartUrl) == 0)
		return kCouponPage;*/

	strPartUrl = _T("/Html/FinancePage/index.html");
	if (_tcsicmp((LPCTSTR)strUrl + _tcslen(strUrl) - strPartUrl.GetLength(), strPartUrl) == 0)
		return kFinancePage;

	strPartUrl = _T("/Html/FinancePage/report.html");
	if (_tcsicmp((LPCTSTR)strUrl + _tcslen(strUrl) - strPartUrl.GetLength(), strPartUrl) == 0)
		return kReportPage;

	strPartUrl = _T("/Html/FinancePage/set.html");
	if (_tcsicmp((LPCTSTR)strUrl + _tcslen(strUrl) - strPartUrl.GetLength(), strPartUrl) == 0)
		return kSetPage;

//	strPartUrl = _T("/Html/ProductPage/index.html");
//	if (_tcsicmp((LPCTSTR)strUrl + _tcslen(strUrl) - strPartUrl.GetLength(), strPartUrl) == 0)
//		return kProductPage;


	return kPageInvalid;
}

void CListManager::NavigateWebPage(LPCTSTR lpszUrl)
{
	bool bDefBrowser = false;

	HKEY hKey = NULL;
	if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT,_T("http\\shell\\open\\command"), &hKey))
	{
		TCHAR szValue[200];
		DWORD dwSize = sizeof(szValue);
		DWORD dwType = REG_SZ;
		LONG ret = RegQueryValueEx(hKey, NULL, NULL, &dwType, (LPBYTE)szValue, &dwSize);
		if (ret == ERROR_MORE_DATA || ret == ERROR_SUCCESS)
			bDefBrowser = true;
		RegCloseKey(hKey);
	}

	//MessageBoxW(NULL,lpszUrl,L"网页检测",MB_OK);
	if (bDefBrowser)
		::ShellExecute(NULL, _T("open"), lpszUrl, NULL, NULL, SW_SHOW);
	else
		::ShellExecute(NULL, _T("open"), _T("iexplore.exe"), lpszUrl, NULL, SW_SHOW);

	//::ShellExecute(NULL, _T("open"), lpszUrl, NULL, NULL, SW_SHOW);
}

// void CListManager::NavigateWebPage(BSTR bstrURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers)
// {
// 	IWebBrowser2* pWebBrowser2 = NULL;
// 	CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pWebBrowser2);
// 	if (pWebBrowser2)
// 	{
// // 		VARIANT vEmpty;
// // 		VariantInit(&vEmpty);
// // 		BSTR bstrURL = SysAllocString(L"http://microsoft.com");
// 
// 		HRESULT hr = pWebBrowser2->Navigate(bstrURL, Flags, TargetFrameName, PostData, Headers);
// 		if (SUCCEEDED(hr))
// 		{
// 			pWebBrowser2->put_Visible(VARIANT_TRUE);
// 		}
// 		else
// 		{
// 			pWebBrowser2->Quit();
// 		}
// 
// 		SysFreeString(bstrURL);
// 		pWebBrowser2->Release();
// 	}
// }

void CListManager::PrintWebPage(LPCTSTR lpszUrl)
{
	CString strUrl = lpszUrl;
	strUrl += "#print";

	::ShellExecute(NULL, _T("open"), _T("iexplore.exe"), strUrl, NULL, SW_SHOW);

/*
	USES_CONVERSION;

	IWebBrowser2* pWebBrowser2 = NULL;
	CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pWebBrowser2);
	if (pWebBrowser2)
	{
		VARIANT vEmpty;
		VariantInit(&vEmpty);
		BSTR bstrURL = SysAllocString(CT2OLE(strUrl));
		HRESULT hr = pWebBrowser2->Navigate(bstrURL, &vEmpty, &vEmpty, &vEmpty, &vEmpty);
		if (SUCCEEDED(hr))
		{
			pWebBrowser2->put_Visible(VARIANT_TRUE);
			//pWebBrowser2->ExecWB(OLECMDID_PRINT, OLECMDEXECOPT_PROMPTUSER, 0, 0);
		}
		else
		{
			pWebBrowser2->Quit();
		}

		SysFreeString(bstrURL);
		pWebBrowser2->Release();
	}
*/
}


// 读取Core程序所需的数据 // gao 2010-12-16
void CListManager::ReadBankCoreNeedInfo(const TiXmlNode *pNode, CWebsiteData *pWebsiteData)
{
	ATLASSERT (NULL != pNode && NULL != pWebsiteData);
	if (NULL == pNode || NULL == pWebsiteData)
		return;

	const TiXmlNode *pRequirements = pNode->FirstChild("requirements");
	if (pRequirements)
	{
		// 找到系统类型
		for (const TiXmlNode *pSystem = pRequirements->FirstChild("system"); pSystem != NULL; pSystem = pRequirements->IterateChildren("system", pSystem))
		{
			std::wstring system = AToW(pSystem->ToElement()->Attribute("type"));
			std::wstring cpu = AToW(pSystem->ToElement()->Attribute("cpu"));

			map<wstring,SystemType>::iterator itr;

			itr = m_sysTypeList.find(system);
			if(itr != m_sysTypeList.end())
			{
				if((*itr).second == m_tSystem || (*itr).second == sAll)//符合当前系统才行,根据系统类型进行安装
				{
					int ncpu = 32;
					if(cpu == L"0")
						ncpu = m_tArchite;
					else if(cpu == L"64")
						ncpu = 64;

					if(ncpu == m_tArchite)
					{// 控件元素
						for (const TiXmlNode *pRequire = pSystem->FirstChild("require"); pRequire != NULL; pRequire = pSystem->IterateChildren("require", pRequire))
						{
							CRequirement *pReq = new CRequirement(pRequire, pWebsiteData);
							pWebsiteData->m_Requirements.push_back(pReq);
						}
					}

				}
			}

		}
	}

}


void CListManager::RegQueryLock()
{
	if (NULL == m_hRegQueryMutex)
	{
		m_hRegQueryMutex = CreateMutex (NULL, FALSE, NULL);
		ATLASSERT (NULL != m_hRegQueryMutex);
	}
	
	if (NULL != m_hRegQueryMutex)
		WaitForSingleObject (m_hRegQueryMutex, INFINITE);
}

void CListManager::RegQueryUnlock()
{
	if (NULL != m_hRegQueryMutex)
		ReleaseMutex (m_hRegQueryMutex);
}


bool CListManager::AddANewFavBank(LPWSTR lpParam, bool bBankName)
{
	if (NULL == lpParam)
		return false;

	std::wstring strPath = lpParam;
	if (!bBankName)
	{
		// 将BankID转换成BankName
		std::string strTp = CW2A (strPath.c_str ());
		strTp = CFavBankOperator::GetBankIDOrBankName (strTp);
		strPath = CA2W(strTp.c_str ());
	}

	// 读取配置文件
	std::string strXMLData = CResourceManager::_()->GetRootXMLFile((LPWSTR)strPath.c_str ());

	TiXmlDocument xmlDoc;
	xmlDoc.Parse(strXMLData.c_str());
	if (xmlDoc.Error())
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_XML_FILE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"AddANewFavBank中GetRootXMLFile %s 发生异常！", strPath.c_str()));

		return false;
	}

	const TiXmlNode *pRoot = xmlDoc.FirstChild("main");
	if (!pRoot)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_XML_FILE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"AddANewFavBank中 %s 中没有根结点！", strPath.c_str()));
		return false;
	}

	for (const TiXmlNode *pCategory = pRoot->FirstChild("category"); pCategory != NULL; pCategory = pRoot->IterateChildren("category", pCategory))
	{
		WebsiteType eWebsiteType;
		std::string strType = pCategory->ToElement()->Attribute("name");
		if (strType == "banks")
			eWebsiteType = Website_Bank;
		else
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_XML_FILE, L"AddANewFavBank中对XML文件内容进行遍历时异常！");
			ATLASSERT(0);
		}

		for (const TiXmlNode *pSite = pCategory->FirstChild("site"); pSite != NULL; pSite = pCategory->IterateChildren("site", pSite))
		{
			CWebsiteData *pWebsiteData = new CWebsiteData;
			pWebsiteData->m_strID = AToW(pSite->ToElement()->Attribute("id"));
			WebDataMap::iterator it = m_WebsiteData.find (pWebsiteData->m_strID);
			if (it == m_WebsiteData.end()) // 如果不存在
			{
				pWebsiteData->m_strName = AToW(pSite->ToElement()->Attribute("name"));
				pWebsiteData->m_bNoClose = false;
				pWebsiteData->m_eWebsiteType = eWebsiteType;
				const TiXmlNode *pDomainList = pSite->FirstChild("domains");
				if (pDomainList)
				{
					for (const TiXmlNode *pDomain = pDomainList->FirstChild("domain"); pDomain != NULL; pDomain = pDomainList->IterateChildren("domain", pDomain))
					{
						std::wstring strDomain = AToW(pDomain->ToElement()->Attribute("name"));
						pWebsiteData->m_urllist.push_back(strDomain);
					}
					ReadBankCoreNeedInfo (pSite, pWebsiteData);	
					RegQueryLock ();
					m_WebsiteData.insert(std::make_pair(pWebsiteData->m_strID, pWebsiteData)); // 插入					
					RegQueryUnlock ();

										
				}
				else
				{
					delete pWebsiteData;
					pWebsiteData = NULL;
				}
			}
			else
			{
				delete pWebsiteData;
				pWebsiteData = NULL;
			}
		}
	}
	return true;

}
bool CListManager::CheckCom(string appid)
{
	if(appid == "")
	{
		CWebsiteData* pWebsiteData = NULL;

		WebDataMap::const_iterator it = m_WebsiteData.begin();
		for(;it != m_WebsiteData.end();it ++)
		{
			pWebsiteData = (it->second);

			ReqList::iterator rite = pWebsiteData->GetReqList()->begin();
			for(;rite != pWebsiteData->GetReqList()->end();rite ++)
			{
				CRequirement *pRequirement = (*rite);

				if (pRequirement->GetType() != Require_Class)
					continue;
				else
					pRequirement->CheckComInfo();
			}
		}
		return true;
	}
	else
	{
		USES_CONVERSION;
		CWebsiteData* pWebsiteData = NULL;
		std::wstring strBankName = A2W(CFavBankOperator::GetBankIDOrBankName(appid).c_str());
		WebDataMap::const_iterator it = m_WebsiteData.find(strBankName);
		if(it != m_WebsiteData.end())
		{
			pWebsiteData = (it->second);
			
			ReqList::iterator rite = pWebsiteData->GetReqList()->begin();
			for(;rite != pWebsiteData->GetReqList()->end();rite ++)
			{
				CRequirement *pRequirement = (*rite);

				if (pRequirement->GetType() != Require_Class)
					continue;
				else
					pRequirement->CheckComInfo();
			}
		}
		return true;
	}
	return true;
}
bool CListManager::DeleteAFavBank(string appid)
{
	if ("" == appid)
		return false;

	USES_CONVERSION;
	std::wstring strBankName = A2W(CFavBankOperator::GetBankIDOrBankName(appid).c_str());

	CWebsiteData* pWebsiteData = NULL;
	m_pFavBkOper->UpDateFavBankID(appid, false);

	RegQueryLock ();
	WebDataMap::const_iterator it = m_WebsiteData.find(strBankName);
	if(it != m_WebsiteData.end())
	{
		pWebsiteData = (it->second);
		m_WebsiteData.erase(it);
	}
	RegQueryUnlock ();

	if(pWebsiteData != NULL)
		delete pWebsiteData;
	return true;
}

CFavBankOperator* CListManager::GetFavBankOper(void)
{
	return m_pFavBkOper;
}

void CListManager::UpdateHMac(void)
{
	CalculateHMac(m_hmac);
}