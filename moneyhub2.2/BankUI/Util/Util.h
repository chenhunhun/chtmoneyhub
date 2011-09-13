#pragma once

enum TopPageStruct
{
	kStartPage	= 0,
	kToolsPage	= 1,
	kCouponPage	= 2,
	kPageCount	= 3,
	kPageInvalid = -1,
};

static void SetTopPage(enum TopPageStruct tps)
{
	DWORD dwNewValue = (DWORD)tps;
	::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Agent"), _T("TopPage"), REG_DWORD, &dwNewValue, sizeof(DWORD));
};

static int GetTopPage(bool bClear = true)
{
	DWORD dwType;
	DWORD dwReturnBytes = sizeof(DWORD);
	DWORD dwTopPage;
	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Agent"), _T("TopPage"), &dwType, &dwTopPage, &dwReturnBytes))
	{
		dwTopPage = 0;
	}

	if (bClear)
	{
		SetTopPage(kStartPage);
	}

	return dwTopPage;
}

static void AccessDefaultPage(HWND hWnd)
{
	TCHAR szPath[1024];
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	TCHAR *p = _tcsrchr(szPath, '\\');

	int dwTopPage = GetTopPage() % kPageCount;

	// 我的首页
 	_tcscpy_s(p, 100, _T("\\Html\\StartPage\\index.html"));
 	::CreateNewPage_0(hWnd, szPath, dwTopPage == 0, true);

	// 提醒
	_tcscpy_s(p, 100, _T("\\Html\\ToolsPage\\index.html"));
	::CreateNewPage_0(hWnd, szPath, dwTopPage == 1, true);

	// 优惠券
	/*_tcscpy_s(p, 100, _T("\\Html\\CouponPage\\index.html"));
	::CreateNewPage_0(hWnd, szPath, dwTopPage == 2, true);*/


#ifdef SINGLE_PROCESS
	// 测试页面
	_tcscpy_s(p, 100, _T("\\Html\\ToolsPage\\test.html"));
	::CreateNewPage_0(hWnd, szPath, FALSE);

	::CreateNewPage_0(hWnd, _T("http://www.boc.cn"), FALSE);
#endif

}

static void SwitchTopPage(HWND hWnd, enum TopPageStruct tps)
{
	TCHAR szPath[1024];
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	TCHAR *p = _tcsrchr(szPath, '\\');

	int dwTopPage = tps % kPageCount;

	if (tps == kStartPage)
	{
		_tcscpy_s(p, 100, _T("\\Html\\StartPage\\index.html"));
	}
	else if (tps == kToolsPage)
	{
		_tcscpy_s(p, 100, _T("\\Html\\ToolsPage\\index.html"));
	}
	else if (tps == kCouponPage)
	{
		_tcscpy_s(p, 100, _T("\\Html\\CouponPage\\index.html"));
	}

	CreateNewPage_3_ExistWindow(hWnd, szPath, NULL);
};