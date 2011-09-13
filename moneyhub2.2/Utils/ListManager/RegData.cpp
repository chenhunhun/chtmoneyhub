#include "stdafx.h"
#include "StringHelper.h"
#include "ResourceManager.h"
#include "RegData.h"


CRegItem::CRegItem(LPCTSTR lpszName, LPCTSTR lpszValue, const CWebsiteData *pWebsite) : m_uRegValueType(REG_NONE), m_iDataLength(0), m_lpszString(NULL)
{
	m_strItemName = lpszName;
	if (_tcsnicmp(lpszValue, _T("hex("), 4) == 0)
	{
		std::wstring strValue = lpszValue;
		size_t nTypeEnd = strValue.find_first_of(')', 4);
		if (nTypeEnd != std::wstring::npos)
		{
			m_uRegValueType = _tcstol(strValue.substr(4, nTypeEnd - 4).c_str(), NULL, 16);
			ATLASSERT(m_uRegValueType == REG_QWORD || m_uRegValueType == REG_EXPAND_SZ || m_uRegValueType == REG_MULTI_SZ);
			StrVecW strVec;
			if (::SplitStringW(lpszValue + nTypeEnd + 2, strVec, ',') > 0)
			{
				m_pBinary = new BYTE[strVec.size()];
				for (size_t i = 0; i < strVec.size(); i++)
					m_pBinary[i] = (BYTE)_tcstol(strVec[i].c_str(), NULL, 16);
			}
			m_iDataLength = strVec.size();
		}
	}
	else if (_tcsnicmp(lpszValue, _T("hex:"), 4) == 0)
	{
		m_uRegValueType = REG_BINARY;
		StrVecW strVec;
		if (::SplitStringW(lpszValue + 4, strVec, ',') > 0)
		{
			m_pBinary = new BYTE[strVec.size()];
			for (size_t i = 0; i < strVec.size(); i++)
				m_pBinary[i] = (BYTE)_tcstol(strVec[i].c_str(), NULL, 16);
		}
		m_iDataLength = strVec.size();
	}
	else if (_tcsnicmp(lpszValue, _T("dword:"), 6) == 0)
	{
		m_uRegValueType = REG_DWORD;
		m_dwDword = _tcstol(lpszValue + 6, NULL, 16);
		m_iDataLength = sizeof(DWORD);
	}
	else
	{
		ATLASSERT(lpszValue[0] == '"');
		m_uRegValueType = REG_SZ;
		std::wstring strValue = lpszValue;
		TrimString(strValue, BLANKS_WITH_QUOTATION);
		if (_tcsncmp(strValue.c_str(), _T("%M%"), 3) == 0)
		{
			// 获取文件实际存储的文件夹位置，替代虚拟文件中的符号
			strValue = CResourceManager::_()->GetFilePath(pWebsite->GetWebsiteType(), pWebsite->GetID(), strValue.c_str() + 4);
		}
		m_lpszString = _tcsdup(strValue.c_str());

///////////////////////////
// 		if(/*_tcsstr(m_lpszString,_T(".dll")) &&*/ _tcsstr(m_lpszString,_T("\\")))
// 		{
// 			
// 			HINSTANCE hIns = LoadLibrary(m_lpszString);
// 
// 			FARPROC lpDllEntryPoint; 
// 			if(hIns)
// 			{
// 				lpDllEntryPoint = GetProcAddress(hIns,"DllRegisterServer"); //DllRegisterServer
		/*
		S_OK 

		The registry entries were created successfully. 

		SELFREG_E_TYPELIB 

		The server was unable to complete the registration of all the type libraries used by its classes. 

		SELFREG_E_CLASS 

		The server was unable to complete the registration of all the object classes. 


		*/
// 				if(lpDllEntryPoint)			
// 				{
// 					if(/*_tcsstr(m_lpszString,_T("ali")) && _tcsstr(m_lpszString,_T("dll")) &&*/ _tcsstr(m_lpszString,_T("\\")) )
// 					{
// 						if(S_OK == (*lpDllEntryPoint)() )
// 							;//MessageBox(NULL,m_lpszString,_T("succef"),MB_OK);
// 						else
// 							;//MessageBox(NULL,m_lpszString,_T("error"),MB_OK);
// 				
// 					}
// 				}
// 			}
// 		}
/////////////////////////////

		m_iDataLength = (strValue.length() + 1) * sizeof(TCHAR);
	}
}

void CRegItem::WriteItemToReg(HKEY hKey) const
{
	// 写入实际注册表
	if(m_uRegValueType == REG_DWORD)
		::RegSetValueEx(hKey, m_strItemName.c_str(), 0, REG_DWORD, (const BYTE *)&m_dwDword, sizeof(DWORD));
	else
		::RegSetValueEx(hKey, m_strItemName.c_str(), 0, m_uRegValueType, m_pBinary, m_iDataLength);
}

//////////////////////////////////////////////////////////////////////////

CRegDir* CRegDir::CreateRegDirs(LPCTSTR lpszPath)
{
	LPCTSTR p = _tcschr(lpszPath, '\\');
	if (p)
	{
		std::wstring strRegKeyName = std::wstring(lpszPath, p - lpszPath);
		std::transform(strRegKeyName.begin(), strRegKeyName.end(), strRegKeyName.begin(), tolower);
		RegDirMap::iterator it = m_RegSubDirs.find(strRegKeyName);
		if (it != m_RegSubDirs.end())
			return it->second->CreateRegDirs(p + 1);
		else
		{
			CRegDir *pRegDir = new CRegDir();
			m_RegSubDirs.insert(std::make_pair(strRegKeyName, pRegDir));
			return pRegDir->CreateRegDirs(p + 1);
		}
	}
	else
	{
		std::wstring strRegKeyName = lpszPath;
		std::transform(strRegKeyName.begin(), strRegKeyName.end(), strRegKeyName.begin(), tolower);
		RegDirMap::iterator it = m_RegSubDirs.find(strRegKeyName);
		if (it != m_RegSubDirs.end())
			return it->second;
		else
		{
			CRegDir *pRegDir = new CRegDir();
			m_RegSubDirs.insert(std::make_pair(strRegKeyName, pRegDir));
			return pRegDir;
		}
	}
	return NULL;
}
// 递归获得要查找的键
const CRegDir* CRegDir::GetRegDirs(LPCTSTR lpszPath) const
{
	LPCTSTR p = _tcschr(lpszPath, '\\');
	if (p)
	{
		std::wstring strRegKeyName = std::wstring(lpszPath, p - lpszPath);
		std::transform(strRegKeyName.begin(), strRegKeyName.end(), strRegKeyName.begin(), tolower);
		RegDirMap::const_iterator it = m_RegSubDirs.find(strRegKeyName);
		if (it != m_RegSubDirs.end())
			return it->second->GetRegDirs(p + 1);
	}
	else
	{
		std::wstring strRegKeyName = lpszPath;
		std::transform(strRegKeyName.begin(), strRegKeyName.end(), strRegKeyName.begin(), tolower);
		RegDirMap::const_iterator it = m_RegSubDirs.find(strRegKeyName);
		if (it != m_RegSubDirs.end())
			return it->second;
	}
	return NULL;
}

// 为键创建值项的函数
void CRegDir::CreateRegItem(LPCTSTR lpszKey, LPCTSTR lpszValue, const CWebsiteData *pWebsite)
{
	if (lpszKey)
	{
		std::wstring strKey = lpszKey;
		std::transform(strKey.begin(), strKey.end(), strKey.begin(), tolower);
		m_RegValues.insert(std::make_pair(strKey, new CRegItem(strKey.c_str(), lpszValue, pWebsite)));
	}
	else
		m_RegValues.insert(std::make_pair(_T(""), new CRegItem(_T(""), lpszValue, pWebsite)));
}

// 将键值写入实际注册表键中的虚拟值项
void CRegDir::WriteAllItemsToReg(HKEY hKey) const
{
	for (RegItemMap::const_iterator it = m_RegValues.begin(); it != m_RegValues.end(); it++)
		it->second->WriteItemToReg(hKey);
}

//////////////////////////////////////////////////////////////////////////
// 根据文件生成虚拟注册表
void CRegData::LoadRegData(const std::wstring &szData, const CWebsiteData *pWebsite)
{
	CRegDir *pCurrentDir = NULL;
	std::wstring strLine;
	for (size_t n = 0; n < szData.length();)
	{
		size_t nLineEnd = szData.find_first_of('\n', n);
		std::wstring strCurrentLine;
		if (nLineEnd == std::wstring::npos)
		{
			strCurrentLine = szData.substr(n);
			n = szData.length();
		}
		else
		{
			strCurrentLine = szData.substr(n, nLineEnd - n - 1);
			n = nLineEnd + 1;
		}
		TrimString(strCurrentLine);
		if (strCurrentLine.empty())
			continue;
		if (strCurrentLine[strCurrentLine.length() - 1] == '\\')
		{
			// 说明这一行没完，要找到一行不以“\”结尾的行，然后把每行的内容 拼起来
			strCurrentLine.erase(strCurrentLine.length() - 1);
			TrimString(strCurrentLine);
			strLine += strCurrentLine;
			continue;
		}
		strLine += strCurrentLine;

		if (strLine[0] == '[')
		{
			// 这一行是key的名称
			size_t nEndKeyName = strLine.find_first_of(']', 1);
			std::wstring strKeyName = strLine.substr(1, nEndKeyName - 1);
			TrimString(strKeyName);
			pCurrentDir = CreateRegDirs(strKeyName.c_str());
		}
		else if (strLine[0] == '!')		// 表示这个键值强制替换系统的
			pCurrentDir->SetForceReplace();
		else if (pCurrentDir)
		{
			// 这一行是key目录下面item的数据
			size_t nEqual = strLine.find_first_of('=');
			if (nEqual != std::wstring::npos)
			{
				std::wstring strValue = strLine.substr(nEqual + 1);
				TrimString(strValue);
				if (strLine[0] == '@')
					pCurrentDir->CreateRegItem(NULL, strValue.c_str(), pWebsite);
				else
				{
					std::wstring strKeyName = strLine.substr(0, nEqual);
					TrimString(strKeyName, BLANKS_WITH_QUOTATION);
					pCurrentDir->CreateRegItem(strKeyName.c_str(), strValue.c_str(), pWebsite);
				}
			}
		}
		strLine.clear();
	}
}
// 查找键值
const CRegDir* CRegData::GetRegDirs(LPCTSTR lpszPath, HKEY hRootKey) const
{
	ATLASSERT(((UINT)hRootKey & 0x80000000) && "hRootKey must be the predefined HKEY value");
	RegRootDirMap::const_iterator it = m_RegRootDirs.find(hRootKey);
	if (it == m_RegRootDirs.end())
		return NULL;
	return it->second->GetRegDirs(lpszPath);
}

// 在初始化时被创建
CRegDir* CRegData::CreateRegDirs(LPCTSTR lpszPath)
{
	LPCTSTR p = _tcschr(lpszPath, '\\');
	if (p)
	{
		HKEY hRootKeyType = 0;
		std::wstring strRootKey = std::wstring(lpszPath, p - lpszPath);
		std::transform(strRootKey.begin(), strRootKey.end(), strRootKey.begin(), toupper);
		if (strRootKey == _T("HKEY_CLASSES_ROOT"))
			hRootKeyType = HKEY_CLASSES_ROOT;
		else if (strRootKey == _T("HKEY_CURRENT_USER"))
			hRootKeyType = HKEY_CURRENT_USER;
		else if (strRootKey == _T("HKEY_LOCAL_MACHINE"))
			hRootKeyType = HKEY_LOCAL_MACHINE;
		else
			ATLASSERT(0);

		RegRootDirMap::iterator it = m_RegRootDirs.find(hRootKeyType);
		if (it != m_RegRootDirs.end())
			return it->second->CreateRegDirs(p + 1);
		else
		{
			CRegDir *pRegDir = new CRegDir();
			m_RegRootDirs.insert(std::make_pair(hRootKeyType, pRegDir));
			return pRegDir->CreateRegDirs(p + 1);
		}
	}

	return NULL;
}
