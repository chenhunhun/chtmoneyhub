#include "StdAfx.h"
#include "FileVersionInfo.h"

#pragma comment(lib, "version.lib")

CFileVersionInfo::CFileVersionInfo()
{
	Reset();
}

CFileVersionInfo::~CFileVersionInfo()
{
}

BOOL CFileVersionInfo::GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId, DWORD &dwId, BOOL bPrimaryEnough/*= FALSE*/)
{
	for (LPWORD lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData) + unBlockSize; lpwData += 2)
	{
		if (*lpwData == wLangId)
		{
			dwId = *((DWORD*)lpwData);
			return TRUE;
		}
	}

	if (!bPrimaryEnough)
		return FALSE;

	for (LPWORD lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
	{
		if (((*lpwData)&0x00FF) == (wLangId & 0x00FF))
		{
			dwId = *((DWORD*)lpwData);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFileVersionInfo::Create(HMODULE hModule /*= NULL*/)
{
	TCHAR szPath[_MAX_PATH + 1];
	GetModuleFileName(hModule, szPath, _MAX_PATH);
	return Create(szPath);
}

BOOL CFileVersionInfo::Create(LPCTSTR lpszFileName)
{
	Reset();

	DWORD dwHandle;
	DWORD dwFileVersionInfoSize = GetFileVersionInfoSize((LPTSTR)lpszFileName, &dwHandle);
	if (dwFileVersionInfoSize == 0)
		return FALSE;

	LPVOID lpData = (LPVOID)new BYTE[dwFileVersionInfoSize];
	if (!lpData)
		return FALSE;

	try
	{
		if (!GetFileVersionInfo((LPTSTR)lpszFileName, dwHandle, dwFileVersionInfoSize, lpData))
			throw FALSE;

		// catch default information
		LPVOID lpInfo;
		UINT unInfoLen;
		if (VerQueryValue(lpData, _T("\\"), &lpInfo, &unInfoLen))
		{
			ATLASSERT(unInfoLen == sizeof(m_FileInfo));
			if (unInfoLen == sizeof(m_FileInfo))
				memcpy(&m_FileInfo, lpInfo, unInfoLen);

		}

		// find best matching language and codepage
		VerQueryValue(lpData, _T("\\VarFileInfo\\Translation"), &lpInfo, &unInfoLen);

		DWORD	dwLangCode = 0;
		if (!GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, FALSE))
		{
			if (!GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, TRUE))
			{
				if (!GetTranslationId(lpInfo, unInfoLen, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), dwLangCode, TRUE))
				{
					if (!GetTranslationId(lpInfo, unInfoLen, MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL), dwLangCode, TRUE))
						// use the first one we can get
						dwLangCode = *((DWORD*)lpInfo);
				}
			}
		}

		//CString	strSubBlock;
		//strSubBlock.Format(_T("\\StringFileInfo\\%04X%04X\\"), dwLangCode&0x0000FFFF, (dwLangCode&0xFFFF0000)>>16);

		TCHAR sb[1000];
		_stprintf_s(sb, _T("\\StringFileInfo\\%04X%04X\\"), dwLangCode & 0x0000FFFF, (dwLangCode & 0xFFFF0000) >> 16);
		tstring strSubBlock = sb;
		
		
		tstring strItem = strSubBlock + _T("CompanyName");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strCompanyName = (LPCTSTR)lpInfo;
		
		strItem = strSubBlock + _T("FileDescription");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strFileDescription = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("FileVersion");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strFileVersion = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("InternalName");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strInternalName = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("LegalCopyright");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strLegalCopyright = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("OriginalFileName");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strOriginalFileName = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("ProductName");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strProductName = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("ProductVersion");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strProductVersion = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("Comments");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strComments = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("LegalTrademarks");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strLegalTrademarks = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("PrivateBuild");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strPrivateBuild = (LPCTSTR)lpInfo;

		strItem = strSubBlock + _T("SpecialBuild");
		if (VerQueryValue(lpData, (LPTSTR)(LPCTSTR)strItem.c_str(), &lpInfo, &unInfoLen))
			m_strSpecialBuild = (LPCTSTR)lpInfo;

		delete[] lpData;
	}
	catch (BOOL)
	{
		delete[] lpData;
		return FALSE;
	}

	return TRUE;
}

WORD CFileVersionInfo::GetFileVersion(int nIndex) const
{
	if (nIndex == 0)
		return (WORD)(m_FileInfo.dwFileVersionLS & 0x0000FFFF);
	else if (nIndex == 1)
		return (WORD)((m_FileInfo.dwFileVersionLS & 0xFFFF0000) >> 16);
	else if (nIndex == 2)
		return (WORD)(m_FileInfo.dwFileVersionMS & 0x0000FFFF);
	else if (nIndex == 3)
		return (WORD)((m_FileInfo.dwFileVersionMS & 0xFFFF0000) >> 16);
	else
		return 0;
}

WORD CFileVersionInfo::GetProductVersion(int nIndex) const
{
	if (nIndex == 0)
		return (WORD)(m_FileInfo.dwProductVersionLS & 0x0000FFFF);
	else if (nIndex == 1)
		return (WORD)((m_FileInfo.dwProductVersionLS & 0xFFFF0000) >> 16);
	else if (nIndex == 2)
		return (WORD)(m_FileInfo.dwProductVersionMS & 0x0000FFFF);
	else if (nIndex == 3)
		return (WORD)((m_FileInfo.dwProductVersionMS & 0xFFFF0000) >> 16);
	else
		return 0;
}

DWORD CFileVersionInfo::GetFileFlagsMask() const
{
	return m_FileInfo.dwFileFlagsMask;
}

DWORD CFileVersionInfo::GetFileFlags() const
{
	return m_FileInfo.dwFileFlags;
}

DWORD CFileVersionInfo::GetFileOs() const
{
	return m_FileInfo.dwFileOS;
}

DWORD CFileVersionInfo::GetFileType() const
{
	return m_FileInfo.dwFileType;
}

DWORD CFileVersionInfo::GetFileSubtype() const
{
	return m_FileInfo.dwFileSubtype;
}

FILETIME CFileVersionInfo::GetFileDate() const
{
	FILETIME ft;
	ft.dwLowDateTime = m_FileInfo.dwFileDateLS;
	ft.dwHighDateTime = m_FileInfo.dwFileDateMS;
	
	return ft;
}

tstring CFileVersionInfo::GetCompanyName() const
{
	return m_strCompanyName;
}

tstring CFileVersionInfo::GetFileDescription() const
{
	return m_strFileDescription;
}

tstring CFileVersionInfo::GetFileVersion() const
{
	return m_strFileVersion;
}

tstring CFileVersionInfo::GetInternalName() const
{
	return m_strInternalName;
}

tstring CFileVersionInfo::GetLegalCopyright() const
{
	return m_strLegalCopyright;
}

tstring CFileVersionInfo::GetOriginalFileName() const
{
	return m_strOriginalFileName;
}

tstring CFileVersionInfo::GetProductName() const
{
	return m_strProductName;
}

tstring CFileVersionInfo::GetProductVersion() const
{
	return m_strProductVersion;
}

tstring CFileVersionInfo::GetComments() const
{
	return m_strComments;
}

tstring CFileVersionInfo::GetLegalTrademarks() const
{
	return m_strLegalTrademarks;
}

tstring CFileVersionInfo::GetPrivateBuild() const
{
	return m_strPrivateBuild;
}

tstring CFileVersionInfo::GetSpecialBuild() const
{
	return m_strSpecialBuild;
}

void CFileVersionInfo::Reset()
{
	ZeroMemory(&m_FileInfo, sizeof(m_FileInfo));
	m_strCompanyName.empty();
	m_strFileDescription.empty();
	m_strFileVersion.empty();
	m_strInternalName.empty();
	m_strLegalCopyright.empty();
	m_strOriginalFileName.empty();
	m_strProductName.empty();
	m_strProductVersion.empty();
	m_strComments.empty();
	m_strLegalTrademarks.empty();
	m_strPrivateBuild.empty();
	m_strSpecialBuild.empty();
}


