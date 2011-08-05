#pragma once

class CWebsiteData;

class CRegItem
{

public:

	// 注册表键值项的结构
	CRegItem(LPCTSTR lpszName, LPCTSTR lpszValue, const CWebsiteData *pWebsite);

	void WriteItemToReg(HKEY hKey) const;

private:
	// 值名称
	std::wstring m_strItemName;
	// 值类型
	UINT m_uRegValueType;
	// 值数据存储位置
	union
	{
		LPCTSTR m_lpszString;
		BYTE *m_pBinary;
		DWORD m_dwDword;
	};
	// 值长度
	int m_iDataLength;
};

//////////////////////////////////////////////////////////////////////////

class CRegDir
{

public:

	CRegDir() : m_bForceReplace(false) {}

	CRegDir* CreateRegDirs(LPCTSTR lpszPath);
	const CRegDir* GetRegDirs(LPCTSTR lpszPath) const;

	void CreateRegItem(LPCTSTR lpszKey, LPCTSTR lpszValue, const CWebsiteData *pWebsite);

	void WriteAllItemsToReg(HKEY hKey) const;
	void WriteCurrentKey(HKEY hKey) const;

	void SetForceReplace() { m_bForceReplace = true; }
	bool ForceReplaceSystemValue() const { return m_bForceReplace; }

private:
	// 注册表项的键的值项
	typedef std::map<std::wstring, CRegItem*> RegItemMap;
	RegItemMap m_RegValues;

	// 注册表的键的子键
	typedef std::map<std::wstring, CRegDir*> RegDirMap;
	RegDirMap m_RegSubDirs;

	bool m_bForceReplace;
};

//////////////////////////////////////////////////////////////////////////

class CRegData
{

public:

	void LoadRegData(const std::wstring &szData, const CWebsiteData *pWebsite);

	const CRegDir* GetRegDirs(LPCTSTR lpszPath, HKEY hRootKey) const;

private:

	CRegDir* CreateRegDirs(LPCTSTR lpszPath);

	typedef std::map<HKEY, CRegDir*> RegRootDirMap;
	RegRootDirMap m_RegRootDirs;
};
