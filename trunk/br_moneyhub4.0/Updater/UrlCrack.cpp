#include "StdAfx.h"
#include "UrlCrack.h"

/*
CUrlCrack::CUrlCrack()
{
	m_wPort = 0;
}

BOOL CUrlCrack::Crack(LPCTSTR pszUrl)
{
	DWORD urlLen = _tcslen(pszUrl) * 2;
	TCHAR *pszCanUrl = new TCHAR[urlLen];

	if (!InternetCanonicalizeUrl(pszUrl, pszCanUrl, &urlLen, ICU_BROWSER_MODE))
	{
		delete[] pszCanUrl;

		if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
		{
			pszCanUrl = new TCHAR[urlLen + 1];
			if (!InternetCanonicalizeUrl(pszUrl, pszCanUrl, &urlLen, ICU_BROWSER_MODE))
			{
				delete[] pszCanUrl;
				return FALSE;
			}
		}
		else
			return FALSE;
	}

	URL_COMPONENTS url_comp;
	ZeroMemory(&url_comp, sizeof(url_comp));
	url_comp.dwStructSize = sizeof(url_comp);

	url_comp.lpszHostName = m_szHost;
	url_comp.lpszPassword = m_szPassword;
	url_comp.lpszScheme = m_szScheme;
	url_comp.lpszUrlPath = m_szPath;
	url_comp.lpszUserName = m_szUser;

	url_comp.dwHostNameLength = UP_HOSTNAME_SIZE;
	url_comp.dwPasswordLength = UP_PASSWORD_SIZE;
	url_comp.dwSchemeLength = UP_SCHEME_SIZE;
	url_comp.dwUrlPathLength = UP_PATH_SIZE;
	url_comp.dwUserNameLength = UP_USERNAME_SIZE;

	if (!InternetCrackUrl(pszCanUrl, urlLen, 0, &url_comp))
	{
		delete[] pszCanUrl;
		return FALSE;
	}

	delete[] pszCanUrl;

	if (url_comp.nScheme != INTERNET_SCHEME_HTTP)
		return FALSE;

	m_wPort = url_comp.nPort;

	return TRUE;
}

LPCTSTR CUrlCrack::GetPath()
{
	return m_szPath;
}

LPCTSTR CUrlCrack::GetHostName()
{
	return m_szHost;
}

INTERNET_PORT CUrlCrack::GetPort()
{
	return m_wPort;
}
*/