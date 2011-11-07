
#pragma once

#include <wininet.h>

class CUrlParts  
{
public:
	CUrlParts();
	~CUrlParts();

	bool SplitUrl(BSTR bUrl);

	inline INTERNET_SCHEME GetInternetScheme() const { return lnScheme; }
	inline LPCTSTR GetScheme() const { return szScheme; }
	inline LPCTSTR GetHostName() const { return szHostName; }
	inline LPCTSTR GetUserName() const { return szUserName; }
	inline LPCTSTR GetPassword() const { return szPassword; }
	inline LPCTSTR GetUrlPath() const { return szUrlPath; }
	inline LPCTSTR GetExtraInfo() const { return szExtraInfo; }
	inline LPCTSTR GetFileName() const { return szFileName; }
	inline LPCTSTR GetFileExtension() const { return szFileExtension; }

	BSTR GetFileNameAsBSTR() const
	{
		CComBSTR m_str1(L"");
		if (dwFileName > 0)
		{
			m_str1.Empty();
			m_str1 = szFileName;
		}

		return m_str1.Copy();
	}

	BSTR GetFileExtensionAsBSTR() const
	{
		CComBSTR m_str1(L"");
		if (dwFileName > 0)
		{
			m_str1.Empty();
			m_str1 = szFileExtension;
		}

		return m_str1.Copy();
	}

	inline DWORD GetSchemeLen() { return dwScheme; }
	inline DWORD GetHostNameLen() { return dwHostName; }
	inline DWORD GetUserNameLen() { return dwUserName; }
	inline DWORD GetPasswordLen() { return dwPassword; }
	inline DWORD GetUrlPathLen() { return dwUrlPath; }
	inline DWORD GetExtraInfoLen() { return dwExtraInfo; }
	inline DWORD GetFileNameLen() { return dwFileName; }
	inline DWORD GetFileExtensionLen() { return dwFileExtension; }
	inline DWORD GetPort() { return dwPort; }
	void ResetBuffers();

private:
	bool Allocated;

	LPTSTR szScheme;
	LPTSTR szHostName;
	LPTSTR szUserName;
	LPTSTR szPassword;
	LPTSTR szUrlPath;
	LPTSTR szExtraInfo;
	LPTSTR szFileName;
	LPTSTR szFileExtension;

	DWORD dwScheme;
	DWORD dwHostName;
	DWORD dwUserName;
	DWORD dwPassword;
	DWORD dwUrlPath;
	DWORD dwExtraInfo;
	DWORD dwFileName;
	DWORD dwFileExtension;
	DWORD dwPort;
	
	INTERNET_SCHEME lnScheme;
	
	
	bool AllocateBuffers(int iNum);
};
