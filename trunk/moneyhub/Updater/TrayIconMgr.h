#pragma once

#include <string>

class CTrayIconMgr
{
public:
	CTrayIconMgr();
	virtual ~CTrayIconMgr();

public:
	BOOL Create(HWND hWnd, UINT *pIcons, UINT cIcons, UINT uMsg);
	void Remove();

	BOOL ShowIcon(int iIndex);
	void TestIcon();

	BOOL ShowBalloon(LPCTSTR szTitle, LPCTSTR szInfo, DWORD dwIcon = NIIF_INFO);

protected:
	DWORD Shell32Version();
	void LoadIcons(UINT* pIcons, UINT cIcons);
	void DestroyIcons();
	BOOL InitTrayIcon();

protected:
	UINT m_uNIDSize;		
	int m_iCurIcon;	

	tstring m_strTip;

	UINT m_uNotifyMsg;
	UINT m_nID;

	UINT m_cIcons;
	HICON *m_pIcons;
	
	HWND m_hWnd;
};

extern CTrayIconMgr _TrayIconMgr;
