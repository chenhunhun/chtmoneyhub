#pragma once
#include <WinInet.h>



//	各种Scheme
//	http://en.wikipedia.org/wiki/URI_scheme#Generic_syntax
static LPCWSTR szURIScheme[] = { L"http://", L"http:\\\\", L"https:\\\\", L"https://", L"www.", L"mms://", L"view-source:",
L"adiumxtra://", L"aim:", L"afp:/", L"aw://", 
L"bolo://", L"callto:", L"chrome://", L"content://", L"cvs://", L"doi:", L"ed2k://", 
L"feed://", L"finger://", L"fish://", L"gg:", L"gizmoproject://", L"iax2:", L"irc://", L"ircs://", 
L"jar:", L"keyparc://", L"lastfm://", L"ldaps://", L"msnim:", L"mvn:", L"notes://",
L"psyc:", L"paparazzi:", L"rmi://", L"rsync://", L"secondlife://", L"sgn://", L"skype://", L"ssh://",
L"sftp://", L"smb://", L"sms:", L"soldat://", L"steam://", L"svn://", L"teamspeak://", L"thunder://", L"unreal://",
L"ut2004://", L"ventrilo://", L"webcal://", L"wtai://", L"wyciwyg://", L"xfire:",
L"xri://", L"Msgrs:", L"tencent://"};

// 顶级域名列表从维基百科取得
// http://en.wikipedia.org/wiki/Generic_top-level_domain
static LPCWSTR szTopLevelDomain[] = { L".ac", L".aero", L".arpa", L".asia", L".biz", L".cat", L".com", 
L".coop", L".edu", L".gov", L".info", L".int", L".jobs", 
L".mil", L".mobi", L".museum", L".name", L".net", L".org",
L".pro", L".tel", L".travel"};

// 国家顶级域名
static LPCWSTR szCountryDomain[] = {L".ad", L".ae", L".af", L".ag", L".ai", L".al", L".am", L".an", 
L".ao", L".aq", L".ar", L".as", L".at", L".au", L".aw", L".ax", L".az", L".ba", L".bb", L".bd", L".be",
L".bf", L".bg", L".bh", L".bi", L".bj", L".bm", L".bn", L".bo", L".br", L".bs", L".bt", L".bv", L".bw",
L".by", L".bz", L".ca", L".cc", L".cd", L".cf", L".cg", L".ch", L".ci", L".ck", L".cl", L".cm", L".cn",
L".co", L".cr", L".cu", L".cv", L".cx", L".cy", L".cz", L".de", L".dj", L".dk", L".dm", L".do", L".dz",
L".ec", L".ee", L".eg", L".er", L".es", L".et", L".eu", L".fi", L".fj", L".fk", L".fm", L".fo", L".fr",
L".ga", L".gb", L".gd", L".ge", L".gf", L".gg", L".gh", L".gi", L".gl", L".gm", L".gn", L".gp", L".gq",
L".gr", L".gs", L".gt", L".gu", L".gw", L".gy", L".hk", L".hm", L".hn", L".hr", L".ht", L".hu", L".id",
L".ie", L".il", L".im", L".in", L".io", L".iq", L".ir", L".is", L".it", L".je", L".jm", L".jo", L".jp",
L".ke", L".kg", L".kh", L".ki", L".km", L".kn", L".kp", L".kr", L".kw", L".ky", L".kz", L".la", L".lb",
L".lc", L".li", L".lk", L".lr", L".ls", L".lt", L".lu", L".lv", L".ly", L".ma", L".mc", L".md", L".me",
L".mg", L".mh", L".mk", L".ml", L".mm", L".mn", L".mo", L".mp", L".mq", L".mr", L".ms", L".mt", L".mu",
L".mv", L".mw", L".mx", L".my", L".mz", L".na", L".nc", L".ne", L".nf", L".ng", L".ni", L".nl", L".no",
L".np", L".nr", L".nu", L".nz", L".om", L".pa", L".pe", L".pf", L".pg", L".ph", L".pk", L".pl", L".pm", 
L".pn", L".pr", L".ps", L".pt", L".pw", L".py", L".qa", L".re", L".ro", L".rs", L".ru", L".rw", L".sa",
L".sb", L".sc", L".sd", L".se", L".sg", L".sh", L".si", L".sj", L".sk", L".sl", L".sm", L".sn", L".so", 
L".sr", L".st", L".su", L".sv", L".sy", L".sz", L".tc", L".td", L".tf", L".tg", L".th", L".tj", L".tk",
L".tl", L".tm", L".tn", L".to", L".tp", L".tr", L".tt", L".tv", L".tw", L".tz", L".ua", L".ug", L".uk",
L".us", L".uy", L".uz", L".va", L".vc", L".ve", L".vg", L".vi", L".vn", L".vu", L".wf", L".ws", L".ye",
L".yt", L".yu", L".za", L".zm", L".zw", L".nt"};



#define	UNKNOWN_URL                 long(-1)
#define VALID_URL					long(0)
#define FILE_SCHEME_KEY				long(1)
#define SCRIPT_SCHEME_KEY			long(2)
#define SEARCH_DATA_KEY				long(3)
#define FTP_SCHEME_KEY				long(4)
#define DIRECTORY_EXIST             long(5)
#define NOT_FIND                    long(6)
#define NETWORK_FILE_PATH           long(7)
#define ABOUT_BLANK                 long(8)
#define SE_RESERVE					long(9)

static long GetKindOfNavigateUrl(LPCTSTR lpszKey)
{
	if (!lpszKey || !lpszKey[0])
		return UNKNOWN_URL ;

	if (::PathIsDirectory(lpszKey))
		return DIRECTORY_EXIST ;
	if (::PathIsNetworkPath(lpszKey))
		return NETWORK_FILE_PATH ;

	if (!_wcsnicmp(L"about:", lpszKey, 6))
		return ABOUT_BLANK ;

	if (!_wcsnicmp(L"se:", lpszKey, 3))
		return SE_RESERVE ;

	if (!_wcsnicmp(L"javascript:", lpszKey, 11)  ||
		!_wcsnicmp(L"vbscript:", lpszKey, 9) ||
		!_wcsnicmp(L"mailto:", lpszKey, 7))
		return SCRIPT_SCHEME_KEY ;

	if (!_wcsnicmp(L"localhost", lpszKey, 9))
		return VALID_URL ;
	if (!_wcsnicmp(L"ftp", lpszKey, 3))
		return FTP_SCHEME_KEY ;

	// 判断是不是本地文件
	// WuJian. 
	// 处理一下file://的情况
	LPCTSTR lpszFileKey;
	if (_wcsnicmp(lpszKey, L"file://", 7) == 0)
		lpszFileKey = lpszKey + 7;
	else
		lpszFileKey = lpszKey;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fd = { 0 };
	if ((hFile = ::FindFirstFile(lpszFileKey, &fd)) != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFile);
		
		CString cstrUrl(lpszFileKey);
		if (_tcscmp(fd.cFileName, _T(".")) == 0  || _tcscmp(fd.cFileName, _T("..")) == 0)
			return SEARCH_DATA_KEY;
		if (cstrUrl.GetLength() > 5)
		{
			// 如果是本地文件，但又是个网页类型，也去navigate
			CString cstrKey = CString(lpszFileKey).Right(5).MakeLower();
			static const WCHAR* szHtmlExt[] = {L".html", L".htm", L".xml", L".mht" };
			for (int i = 0; i < _countof(szHtmlExt); i++)
				if (wcsstr(cstrKey, szHtmlExt[i]))
					return VALID_URL;
		}

		// 长度小于5或者后缀名不为html等的作为本地文件处理
		return FILE_SCHEME_KEY;
	}

	TCHAR szUrlPath[512] = { 0 };
	TCHAR szScheme[32] = { 0 };
	URL_COMPONENTS urlComponents = { 0 };
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
	urlComponents.lpszUrlPath = szUrlPath;
	urlComponents.dwUrlPathLength = 512;
	urlComponents.lpszScheme = szScheme;
	urlComponents.dwSchemeLength = 32;
	WIN32_FIND_DATA wfd = { 0 } ; 
	if (::InternetCrackUrl(lpszKey, 0, 0, &urlComponents) && urlComponents.nScheme == INTERNET_SCHEME_FILE)
		if (INVALID_HANDLE_VALUE != FindFirstFile(lpszKey, &wfd))
			return FILE_SCHEME_KEY ;
		else if (::PathIsDirectory(szUrlPath))
			return DIRECTORY_EXIST ;
		else
			return NOT_FIND ;


	// 判断Scheme
	CString szUrl(lpszKey);
	szUrl.MakeLower();

	for (int i = 0; i < _countof(szURIScheme); i++)
	{
		if (_wcsnicmp(szUrl, szURIScheme[i], wcslen(szURIScheme[i])) == 0)
			return VALID_URL;
	}


	// 把URL的host部分提取出来
	CString szHost;

	// 从左开始找第一个/或者:
	int iHostEnd = szUrl.FindOneOf(L":/");
	szHost = iHostEnd != -1 ? szUrl.Left(iHostEnd) : szUrl;

	if (szHost.IsEmpty())
		return SEARCH_DATA_KEY;

	// 去掉最后的空格
	int iLen = szHost.GetLength();
	while (szHost[iLen-1] == ' ')
	{
		iLen--;
	}
	szHost = szHost.Left(iLen);

	// 先看域名，由于目前最长的域名是.travel，所以取最后8位出来
	CString szPart = szHost.GetLength() > 8 ? szHost.Right(8) : szHost;
	for (int i = 0; i < _countof(szTopLevelDomain); i++)
	{
		if (wcsstr(szPart, szTopLevelDomain[i]))
			return VALID_URL;
	}


	static std::set<CString> setDomain;
	if (setDomain.empty())
	{
		for (int i = 0; i < _countof(szCountryDomain); i++)
		{
			setDomain.insert(szCountryDomain[i]);
		}
	}

	// 再看国家顶级域名，取后三位
	if (szHost.GetLength() > 3)
	{
		szPart = szHost.Right(3);
		if (setDomain.find(szPart) != setDomain.end())
			return VALID_URL;
	}

	return SEARCH_DATA_KEY ;
}

static std::wstring ExtractTopDomain(LPCTSTR lpszUrl)
{
	std::wstring wstrDomain = lpszUrl;
	transform(wstrDomain.begin(), wstrDomain.end(), wstrDomain.begin(), tolower);
	std::wstring::size_type n1 = std::wstring::npos;
	std::wstring::size_type n2 = std::wstring::npos;
	std::wstring::size_type n_ = std::wstring::npos;

	n_ = wstrDomain.find(L':') ;
	if (std::wstring::npos != n_)
		if (wcsncmp(wstrDomain.c_str(), L"http", 4))
		{
			wstrDomain = L"" ;
			return wstrDomain ;
		}

		for (int i = 0; i < _countof(szTopLevelDomain); ++i)
		{
			std::wstring::size_type n2 = wstrDomain.find(szTopLevelDomain[i]) ;
			if (std::wstring::npos == n2)
				continue ;
			long l = wcslen(szTopLevelDomain[i]) ;
			bool bc = true ;
			do 
			{
				if (n2 + l >= wstrDomain.length())
				{
					bc = false ;
					break ;
				}
				if (L'/' == wstrDomain[n2 + l])
				{
					bc = false ;
					break ;
				}
				if (L':' == wstrDomain[n2 + l])
				{
					bc = false ;
					break ;
				}
				if (L'.' != wstrDomain[n2 + l])
					break ;
				std::wstring wstrNation = wstrDomain.substr(n2 + l, 3) ;
				for (int i = 0; i < _countof(szCountryDomain); ++i)
				{
					if (std::wstring::npos != wstrNation.find(szCountryDomain[i]))
					{
						bc = false ;
						break ;
					}
				}
			} 
			while (0);

			if (bc)
				continue ;

			n_ = wstrDomain.rfind(L'.', n2 - 1) ;
			if (std::wstring::npos != n_)
				wstrDomain = wstrDomain.substr(n_ + 1) ;
			n2 = wstrDomain.find(szTopLevelDomain[i]) ;

			n_ = wstrDomain.rfind(L'/', n2 - 1) ;
			if (std::wstring::npos != n_)
				wstrDomain = wstrDomain.substr(n_ + 1) ;

			n2 = wstrDomain.find(szTopLevelDomain[i]) ;
			n_ = wstrDomain.find(L'/', n2) ;
			if (std::wstring::npos != n_)
				wstrDomain = wstrDomain.substr(0, n_) ;

			// 如果有端口号，则去掉端口号
			n_ = wstrDomain.rfind(L':');
			if (std::wstring::npos != n_)
				wstrDomain = wstrDomain.substr(0, n_);

			return wstrDomain ;
		}
		for (int i = 0; i < _countof(szCountryDomain); ++i)
		{
			std::wstring::size_type n1 = wstrDomain.find(szCountryDomain[i]) ;
			if (std::wstring::npos == n1)
				continue ;
			long l = wcslen(szCountryDomain[i]) ;
			if (n1 + l < wstrDomain.length() && L'/' != wstrDomain[n1 + l])
				continue ;

			n_ = wstrDomain.rfind(L'.', n1 - 1) ;
			if (std::wstring::npos != n_)
				wstrDomain = wstrDomain.substr(n_ + 1) ;
			n1 = wstrDomain.find(szCountryDomain[i]) ;
			n_ = wstrDomain.rfind(L'/', n1 - 1) ;
			if (std::wstring::npos != n_)
				wstrDomain = wstrDomain.substr(n_ + 1) ;

			n1 = wstrDomain.find(szCountryDomain[i]) ;
			n_ = wstrDomain.find(L'/', n1) ;
			if (std::wstring::npos != n_)
				wstrDomain = wstrDomain.substr(0, n_) ;

			return wstrDomain ;
		}

		wstrDomain = L"" ;
		return wstrDomain;
}

//////////////////////////////////////////////////////////////////////////


__declspec(selectany) TCHAR szPath[MAX_PATH] = { 0 };
inline LPCTSTR GetModulePath()
{
	if (szPath[0] == 0)
	{
		::GetModuleFileName(NULL, szPath, _countof(szPath));
		TCHAR *p = _tcsrchr(szPath, '\\');
		if (p)
			*p = 0;
	}
	return szPath;
}


__declspec(selectany) char  szPathAD[MAX_PATH] = {0};
inline char * getAppdataPath()
{
	ExpandEnvironmentStringsA("%appdata%",szPathAD,_countof(szPathAD) );
	strcat_s(szPathAD,_countof(szPathAD),"\\MoneyHub");

	return szPathAD;
}
/*
static bool GlobalIsEbankUrl(LPCTSTR lpszUrl)
{
#ifdef _UNICODE
	std::wstring sUrl = lpszUrl;
#else
	std::string sUrl = lpszUrl;
#endif
	if (_tcsicmp(sUrl.c_str() + sUrl.length() - 20, _T("StartPage\\index.html")) == 0)
	{
		return true;
	}
	else
	{
		if (_tcsicmp(sUrl.c_str(), _T("http://www.boc.cn")) == 0
			|| _tcsicmp(sUrl.c_str(), _T("http://www.boc.cn/")) == 0
			|| _tcsnicmp(sUrl.c_str(), _T("http://www.boc.cn/#"), _tcslen(_T("http://www.boc.cn/#"))) == 0)
		{
			return true;
		}
		else if (_tcsnicmp(sUrl.c_str(), _T("res://"), _tcslen(_T("res://"))) == 0)
		{
			return true;
		}
		else if (_tcsnicmp(sUrl.c_str(), _T("https://ebs.boc.cn"), _tcslen(_T("https://ebs.boc.cn"))) == 0)
		{
			return true;
		}
	}

	return false;
}*/