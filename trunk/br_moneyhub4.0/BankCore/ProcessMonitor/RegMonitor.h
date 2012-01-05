#pragma once

class CRegDir;


class CRegKeyManager
{

public:

	static void Initialize();
//	bool ReadEncryptJSFileList();
	bool IsEncryptFile(LPSTR lpPathStr);

	CRegKeyManager();
	~CRegKeyManager();

	LRESULT OnOpenKey(LRESULT lRes, HKEY hRootKey, LPCTSTR lpszSubKey, PHKEY phReturnedKey);
	void CloseKey(HKEY hKey);

private:

	HKEY CreateNewKey(HKEY hRootKey, LPCTSTR lpszSubKey, HKEY hSubKey);

	HKEY GetRegFullPath(HKEY hSubKey, std::wstring &strPath) const;


	typedef std::map<HKEY, std::pair<HKEY, std::wstring>> HKeyMap;
	HKeyMap m_HKeyMap;
	typedef std::map<HKEY, const CRegDir*> VirtualKeyMap;
	VirtualKeyMap m_VirtualKeyMap;

	mutable CRITICAL_SECTION m_cs;
//	std::list<std::string> m_listFileName;
};
