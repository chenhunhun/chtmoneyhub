
#pragma once
#include <string>
using namespace std;
class CDownloadFile
{
public:
	CDownloadFile(int style, std::string sn, int id);

public:
	bool Start();
	bool Delete();

protected:
	bool GetFile(LPCTSTR lpszAppDataPath, LPCTSTR lpszSrc, LPCTSTR lpszDest, int nStyle, int nId);
	void ModifyHtmlContent(LPCTSTR lpszAppDataPath, LPCTSTR lpszFile, int nId, std::string nSn);
	void ModifyCssHtml(std::string nId, CStringA& strContent);

	void DeleteOneFile(LPCTSTR lpszAppDataPath, LPCTSTR lpszSrc, LPCTSTR lpszDest, int nId);
	void FeedBackSaveCoupon(int &ret);
	
private:
	int m_nStyle;
	std::string m_Sn;
	int m_nId;
	wstring m_url;
	wstring m_sPageUrl;
	wstring m_sLImageUrl;
	wstring m_sMImageUrl;
	wstring m_sSImageUrl;
	wstring m_sMakesureUrl;
};


class CMyWaitCursor
{
public:
	HCURSOR m_hWaitCursor;
	HCURSOR m_hOldCursor;
	bool m_bInUse;

	CMyWaitCursor() : m_hOldCursor(NULL), m_bInUse(false)
	{
		m_hWaitCursor = ::LoadCursor(NULL, IDC_WAIT);
		Set();
	}

	~CMyWaitCursor()
	{
		Restore();
	}

	// Methods
	bool Set()
	{
		if(m_bInUse)
			return false;
		m_hOldCursor = ::SetCursor(m_hWaitCursor);
		m_bInUse = true;
		return true;
	}

	bool Restore()
	{
		if(!m_bInUse)
			return false;
		::SetCursor(m_hOldCursor);
		m_bInUse = false;
		return true;
	}
};
