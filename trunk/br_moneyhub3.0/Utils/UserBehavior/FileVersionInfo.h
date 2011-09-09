/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  FileVersionInfo.h
 *      说明：  获得moneyhub软件版本信息类的声明文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.27	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#pragma once

#include <xstring>

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

	std::wstring GetCompanyName() const;
	std::wstring GetFileDescription() const;
	std::wstring GetFileVersion() const;
	std::wstring GetInternalName() const;
	std::wstring GetLegalCopyright() const;
	std::wstring GetOriginalFileName() const;
	std::wstring GetProductName() const;
	std::wstring GetProductVersion() const;
	std::wstring GetComments() const;
	std::wstring GetLegalTrademarks() const;
	std::wstring GetPrivateBuild() const;
	std::wstring GetSpecialBuild() const;

	// implementation helpers
protected:
	virtual void Reset();
	BOOL GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId, DWORD &dwId, BOOL bPrimaryEnough = FALSE);

protected:
	VS_FIXEDFILEINFO m_FileInfo;

	std::wstring m_strCompanyName;
	std::wstring m_strFileDescription;
	std::wstring m_strFileVersion;
	std::wstring m_strInternalName;
	std::wstring m_strLegalCopyright;
	std::wstring m_strOriginalFileName;
	std::wstring m_strProductName;
	std::wstring m_strProductVersion;
	std::wstring m_strComments;
	std::wstring m_strLegalTrademarks;
	std::wstring m_strPrivateBuild;
	std::wstring m_strSpecialBuild;
};
