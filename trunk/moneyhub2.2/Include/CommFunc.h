#pragma once
#include "CommStruct.h"
#include "CommDef.h"

#define TEXT_NO					((LPCTSTR)0)
#define TEXT_1					((LPCTSTR)1)

#define IS_VALID_URL(url)		(((LONG_PTR)(url)) > 0)


inline HWND SendGlobalCreatePageInfo(HWND hFrame, HWND hCurrent, CreateNewWebPageData *pNewPage)
{
	DWORD dwProcessID = 0;
	::GetWindowThreadProcessId(hFrame, &dwProcessID);
	if (dwProcessID != ::GetCurrentProcessId())
	{
		ATLASSERT(pNewPage->dwSize);
		COPYDATASTRUCT cds = { WM_GLOBAL_CREATE_NEW_WEB_PAGE, pNewPage->dwSize, pNewPage };
		return (HWND)::SendMessage(hFrame, WM_COPYDATA, (WPARAM)hCurrent, (LPARAM)&cds);
	}
	else
		return (HWND)::SendMessage(hFrame, WM_GLOBAL_CREATE_NEW_WEB_PAGE, 0, (LPARAM)pNewPage);
}

inline HWND SendGlobalGetExistPageInfo(HWND hFrame, HWND hCurrent, CreateNewWebPageData *pNewPage)
{
	DWORD dwProcessID = 0;
	::GetWindowThreadProcessId(hFrame, &dwProcessID);
	if (dwProcessID != ::GetCurrentProcessId())
	{
		ATLASSERT(pNewPage->dwSize);
		COPYDATASTRUCT cds = { WM_GLOBAL_GET_EXIST_WEB_PAGE, pNewPage->dwSize, pNewPage };
		return (HWND)::SendMessage(hFrame, WM_COPYDATA, (WPARAM)hCurrent, (LPARAM)&cds);
	}
	else
		return (HWND)::SendMessage(hFrame, WM_GLOBAL_GET_EXIST_WEB_PAGE, 0, (LPARAM)pNewPage);
}

inline CreateNewWebPageData* CreateNewWebPageDataByText(LPCTSTR lpszText)
{
	DWORD dwTextLen = IS_VALID_URL(lpszText) ? (_tcslen(lpszText) + 1) * sizeof(TCHAR) : 0;
	DWORD dwStructSize = sizeof(CreateNewWebPageData) + dwTextLen;
	CreateNewWebPageData *pNewPageData = (CreateNewWebPageData*)malloc(dwStructSize);
	memset(pNewPageData, 0, sizeof(CreateNewWebPageData));
	pNewPageData->dwSize = dwStructSize;
	memcpy((BYTE*)pNewPageData + sizeof(CreateNewWebPageData), lpszText, dwTextLen);
	return pNewPageData;
}

//////////////////////////////////////////////////////////////////////////

inline HWND CreateNewPage_0(HWND hCurrentWnd, LPCTSTR lpszURL, BOOL bShowImmediately, bool bNoClose = false)
{
	HWND hFrameWnd = ::GetRootWindow(hCurrentWnd);
	ATLASSERT(hFrameWnd);
	if (bShowImmediately)
		::SetForegroundWindow(hFrameWnd);

	CreateNewWebPageData *pNewPageData = CreateNewWebPageDataByText(lpszURL);
	pNewPageData->iCategory = 0;
	pNewPageData->lpszURL = IS_VALID_URL(lpszURL) ? TEXT_1 : lpszURL;
	pNewPageData->bShowImmediately = bShowImmediately;
	pNewPageData->hCreateFromMainFrame = hFrameWnd;
	pNewPageData->bNoClose = bNoClose;
	HWND hRetWnd = SendGlobalCreatePageInfo(hFrameWnd, hCurrentWnd, pNewPageData);
	free(pNewPageData);
	return hRetWnd;
}

inline void CreateNewPage_1(HWND hCurrentWnd, LPCTSTR lpszURL)
{
	HWND hFrameWnd = ::GetRootWindow(hCurrentWnd);
	ATLASSERT(hFrameWnd);
	::SetForegroundWindow(hFrameWnd);

	CreateNewWebPageData *pNewPageData = CreateNewWebPageDataByText(lpszURL);
	pNewPageData->iCategory = 1;
	pNewPageData->lpszURL = IS_VALID_URL(lpszURL) ? TEXT_1 : lpszURL;
	pNewPageData->bShowImmediately = TRUE;
	pNewPageData->hCreateFromMainFrame = hFrameWnd;
	SendGlobalCreatePageInfo(hFrameWnd, hCurrentWnd, pNewPageData);
	free(pNewPageData);
}

inline HWND CreateNewPage_2(HWND hCurrentWnd, LPCTSTR lpszURL, BOOL bShowImmediately, HWND hComeFromChildFrame)
{
	//ATLASSERT(IS_VALID_URL(lpszURL));
	HWND hFrameWnd = ::GetRootWindow(hCurrentWnd);
	ATLASSERT(hFrameWnd);
	if (bShowImmediately)
		::SetForegroundWindow(hFrameWnd);

	CreateNewWebPageData *pNewPageData = CreateNewWebPageDataByText(lpszURL);
	pNewPageData->iCategory = 2;
	pNewPageData->lpszURL = IS_VALID_URL(lpszURL) ? TEXT_1 : lpszURL;
	pNewPageData->bShowImmediately = bShowImmediately;
	pNewPageData->hCreateFromChildFrame = hComeFromChildFrame;
	pNewPageData->hCreateFromMainFrame = hFrameWnd;
	HWND hRetWnd = SendGlobalCreatePageInfo(hFrameWnd, hCurrentWnd, pNewPageData);
	free(pNewPageData);
	return hRetWnd;
}

inline HWND CreateNewPage_3_NewWindow(HWND hCurrentWnd, LPCTSTR lpszDisplayURL, BOOL bShowImmediately, HWND hComeFromChildFrame)
{
	HWND hFrameWnd = ::GetRootWindow(hCurrentWnd);
	ATLASSERT(hFrameWnd);
	if (bShowImmediately)
		::SetForegroundWindow(hFrameWnd);

	CreateNewWebPageData *pNewPageData = CreateNewWebPageDataByText(lpszDisplayURL);
	pNewPageData->iCategory = 2;
	pNewPageData->lpszURL = URL_NO_NAVIGATE;
	pNewPageData->bShowImmediately = bShowImmediately;
	pNewPageData->hCreateFromChildFrame = hComeFromChildFrame;
	pNewPageData->hCreateFromMainFrame = hFrameWnd;
	HWND hRetWnd = SendGlobalCreatePageInfo(hFrameWnd, hCurrentWnd, pNewPageData);
	free(pNewPageData);
	return hRetWnd;
}

inline HWND CreateNewPage_3_ExistWindow(HWND hCurrentWnd, LPCTSTR lpszDisplayURL, HWND hComeFromChildFrame)
{
	HWND hFrameWnd = ::GetRootWindow(hCurrentWnd);
	ATLASSERT(hFrameWnd);
	::SetForegroundWindow(hFrameWnd);

	CreateNewWebPageData *pNewPageData = CreateNewWebPageDataByText(lpszDisplayURL);
	pNewPageData->iCategory = 2;
	pNewPageData->lpszURL = URL_NO_NAVIGATE;
	pNewPageData->bShowImmediately = TRUE;
	pNewPageData->hCreateFromChildFrame = hComeFromChildFrame;
	pNewPageData->hCreateFromMainFrame = hFrameWnd;
	HWND hRetWnd = SendGlobalGetExistPageInfo(hFrameWnd, hCurrentWnd, pNewPageData);
	free(pNewPageData);
	return hRetWnd;
}

inline HWND CreateNewPage_3_NewWindow_WithReferer(HWND hCurrentWnd, LPCTSTR lpszDisplayURL, BOOL bShowImmediately, HWND hComeFromChildFrame)
{
	HWND hFrameWnd = ::GetRootWindow(hCurrentWnd);
	ATLASSERT(hFrameWnd);
	if (bShowImmediately)
		::SetForegroundWindow(hFrameWnd);

	CreateNewWebPageData *pNewPageData = CreateNewWebPageDataByText(lpszDisplayURL);
	pNewPageData->iCategory = 3;
	pNewPageData->lpszURL = URL_WITH_REFERER;
	pNewPageData->bShowImmediately = bShowImmediately;
	pNewPageData->hCreateFromChildFrame = hComeFromChildFrame;
	pNewPageData->hCreateFromMainFrame = hFrameWnd;
	HWND hRetWnd = SendGlobalCreatePageInfo(hFrameWnd, hCurrentWnd, pNewPageData);
	free(pNewPageData);
	return hRetWnd;
}


inline void CreateNewTabAlways(HWND hCurrentWnd, LPCTSTR lpszURL, BOOL bShowImmediately)
{
	HWND hFrameWnd = ::GetRootWindow(hCurrentWnd);
	ATLASSERT(hFrameWnd);
	if (bShowImmediately)
		::SetForegroundWindow(hFrameWnd);

	CreateNewWebPageData *pNewPageData = CreateNewWebPageDataByText(lpszURL);
	pNewPageData->iCategory = -1;
	pNewPageData->lpszURL = IS_VALID_URL(lpszURL) ? TEXT_1 : lpszURL;
	pNewPageData->bShowImmediately = bShowImmediately;
	pNewPageData->hCreateFromMainFrame = hFrameWnd;
	SendGlobalCreatePageInfo(hFrameWnd, hCurrentWnd, pNewPageData);
	free(pNewPageData);
}


inline BOOL IsWindowVisible2(HWND hWnd)
{
	return (::GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE) != 0;
}
