// SysListParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SysListReader.h"
#include "..\..\Security\BankLoader\BankLoader.h"

//std::vector<CString> g_WhiteList;

bool GetRootKeyAndSubPath(const CStringA& strRegPath, HKEY& hRootKey, CStringA& strSubPath)
{
	bool ret = false;
	if (_strnicmp(strRegPath, "HKLM", 4) == 0)
	{
		hRootKey = HKEY_LOCAL_MACHINE;
		strSubPath = strRegPath.Mid(5);
		ret = true;
	}
	else if (_strnicmp(strRegPath, "HKCU", 4) == 0)
	{
		hRootKey = HKEY_CURRENT_USER;
		strSubPath = strRegPath.Mid(5);
		ret = true;
	}
	else if (_strnicmp(strRegPath, "HKCR", 4) == 0)
	{
		hRootKey = HKEY_CLASSES_ROOT;
		strSubPath = strRegPath.Mid(5);
		ret = true;
	}
	else if (_strnicmp(strRegPath, "HKCC", 4) == 0)
	{
		hRootKey = HKEY_CURRENT_CONFIG;
		strSubPath = strRegPath.Mid(5);
		ret = true;
	}
	else if (_strnicmp(strRegPath, "HKU", 3) == 0)
	{
		hRootKey = HKEY_USERS;
		strSubPath = strRegPath.Mid(4);
		ret = true;
	}

	return ret;
}

void EnumDir(CStringA resToken, int nEnumSubdir, std::vector<CString>& vecFiles)
{
	int nEndSlash = resToken.ReverseFind('\\');
	if (nEndSlash == -1)
		return;

	USES_CONVERSION;

	CStringA path = resToken.Mid(0, nEndSlash);
	CStringA findname = resToken.Mid(nEndSlash + 1);

	// file
	WIN32_FIND_DATAA fd;
	memset(&fd, 0, sizeof(WIN32_FIND_DATAA));
	HANDLE hFind = FindFirstFileA(resToken, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CStringA subname = fd.cFileName;
			if (subname != "." && subname != "..")
			{
				CStringA fname = path + "\\" + subname;
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
				}
				else
					vecFiles.push_back(A2CT(fname));
			}
		} while (FindNextFileA(hFind, &fd) != 0);

		FindClose(hFind);
	}

	// directory
	if (nEnumSubdir > 0)
	{
		hFind = FindFirstFileA(path + "\\*.*", &fd);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				CStringA subname = fd.cFileName;
				if (subname != "." && subname != "..")
				{
					CStringA fname = path + "\\" + subname;
					if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						EnumDir(fname + "\\" + findname, nEnumSubdir, vecFiles);
				}
			} while (FindNextFileA(hFind, &fd) != 0);

			FindClose(hFind);

		}
	}
}

void EnumRegistry(CStringA strRegPath, std::vector<CStringA>& vecOutRegPaths)
{
	int nStar = strRegPath.Find('*');
	if (nStar != -1)
	{
		// multi-*
		if (nStar != (strRegPath.GetLength() - 1) && strRegPath.Find('*', nStar + 1) != -1)
			return;
	}
	else
	{
		vecOutRegPaths.push_back(strRegPath);
		return;
	}

	CStringA strLeftPart, strRightPart;
	
	strLeftPart = strRegPath.Mid(0, nStar);
	strRightPart = strRegPath.Mid(nStar + 1);

	if (strLeftPart.IsEmpty())
		return;

	if (strLeftPart.GetAt(strLeftPart.GetLength() - 1) != '\\')
		return;

	if (!strRightPart.IsEmpty() && strRightPart.GetAt(0) != '\\')
		return;

	strLeftPart.TrimRight('\\');
	strRightPart.TrimLeft('\\');

	HKEY hRootKey = NULL;
	CStringA strSubKey;
	if (!GetRootKeyAndSubPath(strLeftPart, hRootKey, strSubKey))
		return;

	HKEY hSubKey = NULL;
	if (RegOpenKeyExA(hRootKey,	strSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
	{
		CHAR achKey[255];
		for (DWORD i = 0; ; i++)
		{
			DWORD cbName = 255;
			if (ERROR_SUCCESS == RegEnumKeyExA(hSubKey, i, achKey, &cbName, NULL, NULL, NULL, NULL))
				vecOutRegPaths.push_back(strLeftPart + "\\" + achKey + "\\" + strRightPart);
			else
				break;
		}

		RegCloseKey(hSubKey);
	}
}

bool QueryRegistryValue(const CStringA strRegPath, CStringA& strValue)
{
	HKEY hRootKey = NULL;
	CStringA strSubPath;
	if (!GetRootKeyAndSubPath(strRegPath, hRootKey, strSubPath))
		return false;

	int nVal = strSubPath.ReverseFind('\\');
	CStringA strItem = strSubPath.Mid(nVal + 1);
	strSubPath = strSubPath.Mid(0, nVal);

	if (strItem == ".")
		strItem = "";

	HKEY hSubKey = NULL;
	if (RegOpenKeyExA(hRootKey,	strSubPath, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
	{
		CHAR szFile[1024];
		DWORD BufferSize = 1024;
		LONG ret = RegQueryValueExA(hSubKey, strItem, NULL, NULL, (LPBYTE)szFile, &BufferSize);
		RegCloseKey(hSubKey);

		if (ret == ERROR_SUCCESS)
			strValue = szFile;

		return ret == ERROR_SUCCESS;
	}

	return false;
}

void DoOnlyFileName(CStringA resToken)
{
	HMODULE hModule = LoadLibraryA(resToken);
	if (hModule != NULL)
	{
		TCHAR szModulePath[MAX_PATH + 1];
		GetModuleFileName(hModule, szModulePath, MAX_PATH);
		g_sysModuleNameList.push_back(szModulePath);
		FreeLibrary(hModule);
	}
}

void DoFilePathName(CStringA resToken)
{
	CHAR expName[MAX_PATH + 1];
	ExpandEnvironmentStringsA(resToken, expName, MAX_PATH);
	resToken = expName;

	std::vector<CString>* pvecFiles = new std::vector<CString>;

	if (resToken.Find('*') != -1)
	{
		int nEnumSubdir = 0;
		int nLength = resToken.GetLength();
		if (nLength > 2 && resToken.Mid(nLength - 2).MakeLower() == "|s")
		{
			nEnumSubdir = 1;
			resToken = resToken.Mid(0, nLength - 2);
		}
		else if (nLength > 2 && resToken.Mid(nLength - 2).MakeLower() == "|a")
		{
			nEnumSubdir = 2;
			resToken = resToken.Mid(0, nLength - 2);
		}

		//std::vector<CString> vecFiles;
		EnumDir(resToken, nEnumSubdir, *pvecFiles);
	}
	else
	{
		USES_CONVERSION;

		pvecFiles->push_back(A2W(resToken));
	}

	g_sysModuleNameList.insert(g_sysModuleNameList.end(), (*pvecFiles).begin(), (*pvecFiles).end());
	delete pvecFiles;
}

void DoRegistryPath(CStringA resToken)
{	
	int nBrace = -1;
	int nBraceCnt = 1;
	for (int i = 2; i < resToken.GetLength(); i++)
	{
		if (resToken.GetAt(i) == '{')
			nBraceCnt++;
		else if (resToken.GetAt(i) == '}')
		{
			nBraceCnt--;
			if (nBraceCnt == 0)
			{
				nBrace = i;
				break;
			}
		}
	}

	if (nBrace == -1)
		return;

	USES_CONVERSION;

	bool bIsPath = true;
	CStringA strRegPath = resToken.Mid(2, nBrace - 2);
	int nLength = strRegPath.GetLength();
	if (nLength > 2 && strRegPath.Mid(nLength - 2).MakeLower() == "|p")
	{
		bIsPath = true;
		strRegPath = strRegPath.Mid(0, nLength - 2);
	}
	else if (nLength > 2 && strRegPath.Mid(nLength - 2).MakeLower() == "|f")
	{
		bIsPath = false;
		strRegPath = strRegPath.Mid(0, nLength - 2);
	}

	std::vector<CStringA> vecOutRegPaths;
	EnumRegistry(strRegPath, vecOutRegPaths);

	int cnt = vecOutRegPaths.size();
	for (int i = 0; i < cnt; i++)
	{
		CStringA strFilePath;
		if (QueryRegistryValue(vecOutRegPaths.at(i), strFilePath))
		{
			if (!bIsPath)
			{
				strFilePath.Replace('/', '\\');
				int nEndSlash = strFilePath.ReverseFind('\\');
				if (nEndSlash == -1)
					continue;

				strFilePath = strFilePath.Mid(0, nEndSlash);
			}

			strFilePath += resToken.Mid(nBrace + 1);			
			DoFilePathName(strFilePath);
		}
	}
}

void DoRegistryFile(CStringA resToken)
{
	int nBrace = -1;
	int nBraceCnt = 1;
	for (int i = 2; i < resToken.GetLength(); i++)
	{
		if (resToken.GetAt(i) == '[')
			nBraceCnt++;
		else if (resToken.GetAt(i) == ']')
		{
			nBraceCnt--;
			if (nBraceCnt == 0)
			{
				nBrace = i;
				break;
			}
		}
	}

	if (nBrace == -1)
		return;

	USES_CONVERSION;

	CStringA strRegPath = resToken.Mid(2, nBrace - 2);

	std::vector<CStringA> vecOutRegPaths;
	EnumRegistry(strRegPath, vecOutRegPaths);

	int cnt = vecOutRegPaths.size();
	for (int i = 0; i < cnt; i++)
	{
		CStringA strFilePath;
		if (QueryRegistryValue(vecOutRegPaths.at(i), strFilePath))
		{
			CHAR expName[MAX_PATH + 1];
			ExpandEnvironmentStringsA(strFilePath, expName, MAX_PATH);
			strFilePath = expName;

			if (strFilePath.Find('\\') != -1)
				DoFilePathName(strFilePath);
			else
				DoOnlyFileName(strFilePath);
		}
	}
}

void SplitSysListContent(const CStringA& strContent)
{
	USES_CONVERSION;

	int curPos = 0;
	CStringA resToken = strContent.Tokenize("\r\n", curPos);
	while (resToken != "")
	{
		resToken.Trim();

		if (!resToken.IsEmpty() && resToken.GetAt(0) != '#')
		{
			int nPoundKey = resToken.Find('#');
			if (nPoundKey != -1)
				resToken = resToken.Mid(0, nPoundKey);

			resToken.Replace('/', '\\');

			// (1) 注册表
			if (resToken.GetAt(0) == '$')
			{
				if (resToken.GetLength() > 3)
				{
					if (resToken.GetAt(1) == '{')
						DoRegistryPath(resToken);
					else if (resToken.GetAt(1) == '[')
						DoRegistryFile(resToken);
				}
			}
			// (2) 单独文件
			else if (resToken.GetAt(0) == '@')
				DoOnlyFileName(resToken.Mid(1));

			// (3) 其他
			else
				DoFilePathName(resToken);
		}

		resToken = strContent.Tokenize("\r\n", curPos);
	}
}

bool ReadSysList_Plus(const CStringA& strContent)
{
	SplitSysListContent(strContent);
	return true;
}
