#pragma once
#include <string>
#include <algorithm>

#define BLANKS						_T(" \r\n\t")
#define BLANKS_WITH_QUOTATION		_T(" \r\n\t\"\'")
#define BLANKS_SLASH				_T("\\")


inline void TrimString(std::wstring &str, LPCTSTR lpszBlankStr = BLANKS)
{
	size_t nBegin = str.find_first_not_of(lpszBlankStr);
	if (nBegin == std::wstring::npos)
	{
		str.clear();
		return;
	}
	size_t nEnd = str.find_last_not_of(lpszBlankStr);
	if (nEnd != str.length() - 1)
		str.erase(nEnd + 1);
	if (nBegin != 0)
		str.erase(0, nBegin);
	return;
}



typedef std::vector<std::string> StrVecA;

inline int SplitStringA(LPCSTR lpszStr, StrVecA &strlist, char chSeparator = '\n', size_t iMaxCount = 0x7fffffff)
{
	strlist.clear();

	int iLast = 0;
	for (int i = 0; lpszStr[i]; i++)
	{
		if (lpszStr[i] == chSeparator)
		{
			strlist.push_back(std::string(lpszStr + iLast, i - iLast));
			if (iMaxCount == strlist.size())
				break;
			iLast = i + 1;
		}
	}
	if (iMaxCount > strlist.size())
		strlist.push_back(lpszStr + iLast);

	return strlist.size();
}


typedef std::vector<std::wstring> StrVecW;

inline int SplitStringW(LPCWSTR lpszStr, StrVecW &strlist, wchar_t chSeparator = '\n', size_t iMaxCount = 0x7fffffff)
{
	strlist.clear();

	int iLast = 0;
	for (int i = 0; lpszStr[i]; i++)
	{
		if (lpszStr[i] == chSeparator)
		{
			strlist.push_back(std::wstring(lpszStr + iLast, i - iLast));
			iLast = i + 1;
			if (iMaxCount == strlist.size() + 1)
				break;
		}
	}
	if (iMaxCount > strlist.size())
		strlist.push_back(lpszStr + iLast);

	return strlist.size();
}
