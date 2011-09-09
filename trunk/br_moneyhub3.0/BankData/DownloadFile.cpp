
#include "stdafx.h"
#include <atlstr.h>
#include <shlobj.h>
#include "DownloadFile.h"
#include "../Utils/PostData/postData.h"
#include "../Utils/Config/HostConfig.h"
#pragma comment(lib, "Urlmon.lib")

/*
  hardcode太多了
*/

#define COUPON_PAGE_URL		_T("coupon.php?id=%d")
#define COUPON_LIMAGE_URL	_T("getimage.php?id=%d&type=1")
#define COUPON_MIMAGE_URL	_T("getimage.php?id=%d&type=2")
#define COUPON_SIMAGE_URL	_T("getimage.php?id=%d&type=3")
#define COUPON_MAKESURE_URL _T("makesure.php")

#define COUPON_PAGE_FILE	_T("\\MoneyHub\\Coupons\\%d.htm")
#define COUPON_LIMAGE_FILE	_T("\\MoneyHub\\Coupons\\%dL.jpg")
#define COUPON_MIMAGE_FILE	_T("\\MoneyHub\\Coupons\\%dM.jpg")
#define COUPON_SIMAGE_FILE	_T("\\MoneyHub\\Coupons\\%dS.jpg")

CDownloadFile::CDownloadFile(int style, std::string sn, int id)
{
	m_nStyle = style;
	m_Sn = sn;
	m_nId = id;
	m_url = CHostContainer::GetInstance()->GetHostName(kBenefit);

	m_sPageUrl = m_url + COUPON_PAGE_URL;
	m_sLImageUrl = m_url + COUPON_LIMAGE_URL;
	m_sMImageUrl = m_url + COUPON_MIMAGE_URL;
	m_sSImageUrl = m_url + COUPON_SIMAGE_URL;
	m_sMakesureUrl = m_url + COUPON_MAKESURE_URL;
}

bool CDownloadFile::Start()
{
	CMyWaitCursor waitcursor;

	TCHAR szBuf[1024];
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szBuf);
	CString strAppDataPath = szBuf;

	CreateDirectory(strAppDataPath + "\\MoneyHub\\Coupons", NULL);

	bool ret = true;
	ret = ret && GetFile(strAppDataPath, m_sPageUrl.c_str(), COUPON_PAGE_FILE, m_nStyle, m_nId);	
	ModifyHtmlContent(strAppDataPath, COUPON_PAGE_FILE, m_nId, m_Sn);

	ret = ret && GetFile(strAppDataPath, m_sLImageUrl.c_str(), COUPON_LIMAGE_FILE, m_nStyle, m_nId);
	ret = ret && GetFile(strAppDataPath, m_sMImageUrl.c_str(), COUPON_MIMAGE_FILE, m_nStyle, m_nId);
	ret = ret && GetFile(strAppDataPath, m_sSImageUrl.c_str(), COUPON_SIMAGE_FILE, m_nStyle, m_nId);

	int reInt = ret ? 1 : 0;
	FeedBackSaveCoupon(reInt);// 下载图片结果要向服务器端反馈，好让服务器端能够获得该结果，1表示成功，0表示失败

	return ret;
}
void CDownloadFile::FeedBackSaveCoupon(int& ret)
{
	CStringA strFile;
	strFile.Format("Feedback=%d,%d,%d", m_nStyle, m_nId, ret);

	CPostData::getInstance()->PostData(m_sMakesureUrl.c_str(), (LPVOID)strFile.GetString(), strFile.GetLength());
}
bool CDownloadFile::Delete()
{
	CMyWaitCursor waitcursor;

	TCHAR szBuf[1024];
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szBuf);
	CString strAppDataPath = szBuf;

	DeleteOneFile(strAppDataPath, m_sPageUrl.c_str(), COUPON_PAGE_FILE, m_nId);
	DeleteOneFile(strAppDataPath, m_sLImageUrl.c_str(), COUPON_LIMAGE_FILE, m_nId);
	DeleteOneFile(strAppDataPath, m_sMImageUrl.c_str(), COUPON_MIMAGE_FILE, m_nId);
	DeleteOneFile(strAppDataPath, m_sSImageUrl.c_str(), COUPON_SIMAGE_FILE, m_nId);

	return true;
}

bool CDownloadFile::GetFile(LPCTSTR lpszAppDataPath, LPCTSTR lpszSrc, LPCTSTR lpszDest, int nStyle, int nId)
{
	CString strUrl, strFile;

	strUrl.Format(lpszSrc, nStyle);
	strFile.Format(lpszDest, nId);

	strFile = CString(lpszAppDataPath) + strFile;
	DeleteFile(strFile);

	return SUCCEEDED(URLDownloadToFile(NULL, strUrl, strFile, 0, 0));
}

void CDownloadFile::ModifyHtmlContent(LPCTSTR lpszAppDataPath, LPCTSTR lpszFile, int nId, std::string nSn)
{
	CString strFile;
	strFile.Format(lpszFile, nId);
	strFile = CString(lpszAppDataPath) + strFile;

	HANDLE hFile = CreateFile(strFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwLength = GetFileSize(hFile, NULL);
		CStringA strContent;
		LPSTR lpBuffer = strContent.GetBuffer(dwLength + 1);

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			strContent.ReleaseBuffer();
			CloseHandle(hFile);
			return;
		}

		lpBuffer[dwLength] = 0;
		strContent.ReleaseBuffer();

		// Big Image
		int nTag_Begin = strContent.Find("<!-- coupon_pic_big -->", 0);
		int nTag_End = strContent.Find("<!-- /coupon_pic_big -->", 0);

		if (nTag_Begin >= 0 && nTag_End > nTag_Begin)
		{
			CStringA strId;
			strId.Format("%d", nId);

			CStringA str = strContent.Mid(0, nTag_Begin + strlen("<!-- coupon_pic_big -->"));
			str += "<img id=\"coupon_pic_big\" src=\"" + strId + "L.jpg\" alt=\"\" style=\"border:1px solid #407da7;\" />";
			str += strContent.Mid(nTag_End);

			strContent = str;
		}
		
		ModifyCssHtml(nSn, strContent);

		DWORD dwWrite = 0;
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		WriteFile(hFile, strContent, strContent.GetLength(), &dwWrite, NULL);
		SetFilePointer(hFile, strContent.GetLength(), NULL, FILE_BEGIN);
		SetEndOfFile(hFile);

		CloseHandle(hFile);
	}
}

void CDownloadFile::ModifyCssHtml(std::string nSn, CStringA& strContent)
{
	CStringA strOldHtml = "<!-- txtHint --><!-- /txtHint -->";
	CStringA strId;
	strId.Format("%s", nSn.c_str());
	CStringA strNewHtml = strId;
	strContent.Replace(strOldHtml, strNewHtml);
}

void CDownloadFile::DeleteOneFile(LPCTSTR lpszAppDataPath, LPCTSTR lpszSrc, LPCTSTR lpszDest, int nId)
{
	CString strFile;

	strFile.Format(lpszDest, nId);

	strFile = CString(lpszAppDataPath) + strFile;
	DeleteFile(strFile);
}
