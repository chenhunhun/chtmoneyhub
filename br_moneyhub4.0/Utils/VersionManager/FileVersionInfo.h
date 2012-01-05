#pragma once

#include <string>
using namespace std;

class CFileVersionInfo
{
public:
	CFileVersionInfo(void);
	~CFileVersionInfo(void);

	// operations
public:
	BOOL Create(HMODULE hModule = NULL);
	BOOL Create(LPCTSTR lpszFileName);

	// attribute operations
public:
	WORD GetFileVersion(int nIndex) const;
	WORD GetProductVersion(int nIndex) const;
	DWORD GetFileFlagsMask() const;
	DWORD GetFileFlags() const;
	DWORD GetFileOs() const;
	DWORD GetFileType() const;
	DWORD GetFileSubtype() const;
	FILETIME GetFileDate() const;

	tstring GetCompanyName() const;
	tstring GetFileDescription() const;
	tstring GetFileVersion() const;
	tstring GetInternalName() const;
	tstring GetLegalCopyright() const;
	tstring GetOriginalFileName() const;
	tstring GetProductName() const;
	tstring GetProductVersion() const;
	tstring GetComments() const;
	tstring GetLegalTrademarks() const;
	tstring GetPrivateBuild() const;
	tstring GetSpecialBuild() const;

	// implementation helpers
protected:
	virtual void Reset();
	BOOL GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId, DWORD &dwId, BOOL bPrimaryEnough = FALSE);

protected:
	VS_FIXEDFILEINFO m_FileInfo;

	tstring m_strCompanyName;
	tstring m_strFileDescription;
	tstring m_strFileVersion;
	tstring m_strInternalName;
	tstring m_strLegalCopyright;
	tstring m_strOriginalFileName;
	tstring m_strProductName;
	tstring m_strProductVersion;
	tstring m_strComments;
	tstring m_strLegalTrademarks;
	tstring m_strPrivateBuild;
	tstring m_strSpecialBuild;
};
