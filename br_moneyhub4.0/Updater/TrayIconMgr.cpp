
#include "stdafx.h"
#include "TrayIconMgr.h"

CTrayIconMgr _TrayIconMgr;

CTrayIconMgr::CTrayIconMgr()
{
	m_pIcons = NULL;
	m_iCurIcon = 0;
	m_uNIDSize = Shell32Version() >= 5 ? sizeof(NOTIFYICONDATA) : NOTIFYICONDATA_V1_SIZE;
}

CTrayIconMgr::~CTrayIconMgr()
{
	DestroyIcons();
}

BOOL CTrayIconMgr::Create(HWND hWnd, UINT *pIcons, UINT cIcons, UINT uMsg)
{
	LoadIcons(pIcons, cIcons);
	m_hWnd = hWnd;
	m_uNotifyMsg = uMsg;

	return InitTrayIcon();
}

void CTrayIconMgr::Remove()
{
	NOTIFYICONDATA data;

	data.cbSize = m_uNIDSize;
	data.hWnd = m_hWnd;
	data.uID = m_nID;

	Shell_NotifyIcon(NIM_DELETE, &data);
}

BOOL CTrayIconMgr::ShowIcon(int iIndex)
{
	if (m_pIcons == NULL)
		return FALSE;

	m_iCurIcon = iIndex;

	NOTIFYICONDATA data;
	data.cbSize = m_uNIDSize;
	data.hIcon = m_pIcons[iIndex];
	data.hWnd = m_hWnd;
	data.uFlags = NIF_ICON;
	data.uID = m_nID;

	return Shell_NotifyIcon(NIM_MODIFY, &data);
}

void CTrayIconMgr::TestIcon()
{
	if (!ShowIcon(m_iCurIcon))
	{
		InitTrayIcon();
		ShowIcon(m_iCurIcon);
	}
}

BOOL CTrayIconMgr::ShowBalloon(LPCTSTR szTitle, LPCTSTR szInfo, DWORD dwIcon)
{
	if (Shell32Version() < 5)
		return FALSE;

	NOTIFYICONDATA data;

	data.cbSize = m_uNIDSize;
	data.hWnd = m_hWnd;
	data.uID = m_nID;
	data.uFlags = NIF_INFO;

	if (_tcslen(szInfo) > 255)
	{
		_tcsncpy_s(data.szInfo, szInfo, 255);
		data.szInfo[255] = 0;
	}
	else
		_tcscpy_s(data.szInfo, szInfo);

	if (_tcslen(szTitle) > 63)
	{
		_tcsncpy_s(data.szInfoTitle, szTitle, 63);
		data.szInfoTitle[63] = 0;
	}
	else
		_tcscpy_s(data.szInfoTitle, szTitle);

	data.dwInfoFlags = dwIcon;
	data.uTimeout = UINT_MAX;

	return Shell_NotifyIcon(NIM_MODIFY, &data);
}

//////////////////////////////////////////////////////////////////////////
// inner functions

typedef HRESULT (CALLBACK *DllGetVersionFunc)(DLLVERSIONINFO *);

DWORD CTrayIconMgr::Shell32Version()
{
	static int dwVer = 0;

	// 只读取一次Shell32 version
	if (dwVer == 0)
	{
		HMODULE hLib = LoadLibrary(_T("shell32.dll"));
		if (hLib == NULL)
			return 0;

		DllGetVersionFunc func = (DllGetVersionFunc)GetProcAddress(hLib, "DllGetVersion");
		if (func == NULL)
		{
			FreeLibrary(hLib);
			return 0;
		}

		DLLVERSIONINFO info;
		info.cbSize = sizeof(info);
		func(&info);
		FreeLibrary(hLib);

		dwVer = info.dwMajorVersion;
	}

	return dwVer;
}

void CTrayIconMgr::LoadIcons(UINT *pIcons, UINT cIcons)
{
	DestroyIcons();

	m_pIcons = new HICON[cIcons];
	m_cIcons = cIcons;

	for (UINT i = 0; i < cIcons; i++)
	{
		m_pIcons[i] = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(pIcons[i]), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	}
}

void CTrayIconMgr::DestroyIcons()
{
	if (m_pIcons)
	{
		for (UINT i = 0; i < m_cIcons; i++)
			::DestroyIcon(m_pIcons[i]);

		delete[] m_pIcons;
		m_pIcons = NULL;
	}
}

BOOL CTrayIconMgr::InitTrayIcon()
{
	if (m_pIcons == NULL)
		return FALSE;

	NOTIFYICONDATA data;
	data.cbSize = m_uNIDSize;
	data.hIcon = m_pIcons[0];
	data.hWnd = m_hWnd;
	_tcscpy_s(data.szTip, m_strTip.c_str());
	data.uCallbackMessage = m_uNotifyMsg;
	data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	data.uID = 0;

	m_nID = 0;

	return Shell_NotifyIcon(NIM_ADD, &data);
}  