
#pragma once

#include <map>

class CCoolMenuHook
{
public:
    CCoolMenuHook(HWND hWnd);
	~CCoolMenuHook();

public:
	void OnSize(int x, int y);
	void OnNcPaint();
	void OnPrint(HDC hDC);
	void OnShowWindow(BOOL bShow);
	void OnNcDestroy();

public:
	static CCoolMenuHook* AddMenuHook(HWND hwnd);
	static CCoolMenuHook* GetMenuHook(HWND hwnd);

protected:
	HWND m_hWnd;

//////////////////////////////////////////////////////////////////////////
public:
	static void InstallHook(HWND hFrameWnd);
	static void UnInstallHook();
	static LRESULT CALLBACK MoneyMenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CallWndProcHook(int code, WPARAM wParam, LPARAM lParam);
	static BOOL TrackPopupMenuEx(HMENU hmenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm);

public:
	static std::map<HWND, CCoolMenuHook*> m_WndMenuMap;
    static HHOOK m_hMenuHook;
	static HWND m_hFrameWnd;
	static BOOL m_bHookTheMenu;
};
