#include "stdafx.h"
#include "../Config.h"
#include "SkinLoader.h"
#include "TuoImageStream.h"
#include "TuoImage.h"


CTuoImage::CTuoImage() : m_iWidth(0), m_iHeight(0), m_bUseBitblt(true), m_hBitmap(NULL)
{
	m_hCurrentThreadDC = ThreadCacheDC::GetThreadCacheDC();
}

CTuoImage::~CTuoImage()
{
	if (m_hBitmap)
		::DeleteObject(m_hBitmap);
}

void CTuoImage::ResetCacheDC()
{
	m_hCurrentThreadDC = ThreadCacheDC::GetThreadCacheDC();
}

HBITMAP CTuoImage::SplitBitmapHandleOut()
{
	HBITMAP hBitmap = m_hBitmap;
	m_hBitmap = NULL;
	return hBitmap;
}


bool CTuoImage::LoadFromFile(bool bPNG)
{
	std::wstring strPath;
	if (bPNG)
		strPath = g_strSkinDir + _T("\\") + m_strFileName + _T(".png");
	else
		strPath = g_strSkinDir + _T("\\") + m_strFileName + _T(".ico");

	Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromFile(strPath.c_str());
	if (pBitmap)
	{
		if (pBitmap->GetLastStatus() == Gdiplus::Ok)
		{
			if (m_hBitmap)
				::DeleteObject(m_hBitmap);
			pBitmap->GetHBITMAP(NULL, &m_hBitmap);
		}

		BITMAP bmpData;
		::GetObject(m_hBitmap, sizeof(BITMAP), &bmpData);
		m_iWidth = bmpData.bmWidth;
		m_iHeight = bmpData.bmHeight;
		m_bUseBitblt = bmpData.bmBitsPixel < 32;

		delete pBitmap;

		return true;
	}
	return false;
}

bool CTuoImage::LoadFromFile(LPCTSTR lpszFileName, bool bShortName)
{
	std::wstring strFileName = lpszFileName;

	if (bShortName)
	{
		if (g_strSkinDir.size() == 0)
		{
			TCHAR szPath[MAX_PATH] = { 0 };
			::GetModuleFileName(NULL, szPath, _countof(szPath));
			TCHAR *p = _tcsrchr(szPath, '\\');
			if (p) *p = 0;

			g_strSkinDir = szPath;
			g_strSkinDir += _T("\\Skin\\");
		}

	}

	strFileName = g_strSkinDir + strFileName;
	//Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromFile(lpszFileName);
	Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromFile(strFileName.c_str());
	if (pBitmap)
	{
		if (pBitmap->GetLastStatus() == Gdiplus::Ok)
		{
			if (m_hBitmap)
				::DeleteObject(m_hBitmap);
			pBitmap->GetHBITMAP(NULL, &m_hBitmap);
		}

		BITMAP bmpData;
		::GetObject(m_hBitmap, sizeof(BITMAP), &bmpData);
		m_iWidth = bmpData.bmWidth;
		m_iHeight = bmpData.bmHeight;
		m_bUseBitblt = bmpData.bmBitsPixel < 32;

		delete pBitmap;

		return true;
	}
	return false;
}


void CTuoImage::CreateDirect(void *pData, DWORD dwSize)
{
	static bool bInited = false;
	if (!bInited)
	{
		bInited = true;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	}

	CTuoImageStream stream(pData, dwSize);
	Gdiplus::Bitmap bmp(&stream);
	bmp.GetHBITMAP(Gdiplus::Color(0xff, 0xff, 0xff), &m_hBitmap);
	ATLASSERT(m_hBitmap);

	BITMAP bmpData;
	::GetObject(m_hBitmap, sizeof(BITMAP), &bmpData);
	m_iWidth = bmpData.bmWidth;
	m_iHeight = bmpData.bmHeight;
	m_bUseBitblt = true;
}

void CTuoImage::DestroyBitmap()
{
	if (m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}


BOOL CTuoImage::Draw(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight) const
{
	ATLASSUME(m_hBitmap != NULL);
	ATLASSERT(hDestDC != NULL);
	ATLASSERT(nDestWidth > 0);
	ATLASSERT(nDestHeight > 0);
	ATLASSERT(nSrcWidth > 0);
	ATLASSERT(nSrcHeight > 0);

	::SelectObject(m_hCurrentThreadDC, m_hBitmap);

	if (m_bUseBitblt)
		return ::StretchBlt(hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hCurrentThreadDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 0xff;
	bf.AlphaFormat = AC_SRC_ALPHA;
	return ::AlphaBlend(hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hCurrentThreadDC, xSrc, ySrc, nSrcWidth, nSrcHeight, bf);
}

BOOL CTuoImage::BitBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight) const
{
	ATLASSUME(m_hBitmap != NULL);
	ATLASSERT(hDestDC != NULL);
	ATLASSERT(nDestWidth > 0);
	ATLASSERT(nDestHeight > 0);
	ATLASSERT(nSrcWidth > 0);
	ATLASSERT(nSrcHeight > 0);

	::SelectObject(m_hCurrentThreadDC, m_hBitmap);
	return ::StretchBlt(hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hCurrentThreadDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY);
}

//////////////////////////////////////////////////////////////////////////

bool CTuoIcon::LoadFromFile()
{
	CIconHandle ico;
	ico.LoadIcon(m_strFileName.c_str(), 16, 16, LR_LOADFROMFILE);
	if (ico == NULL)
		return false;
	m_hIcon = ico;
	return true;
}

//////////////////////////////////////////////////////////////////////////

DWORD ThreadCacheDC::g_dwThreadCacheDCIndex = 0;
DWORD ThreadCacheDC::g_dwThreadCacheDCIndexMagicNumber = 0;

void ThreadCacheDC::InitializeThreadCacheDC()
{
	g_dwThreadCacheDCIndex = ::TlsAlloc();
	g_dwThreadCacheDCIndexMagicNumber = ::TlsAlloc();
	ATLASSERT(g_dwThreadCacheDCIndex != TLS_OUT_OF_INDEXES);
	ATLASSERT(g_dwThreadCacheDCIndexMagicNumber != TLS_OUT_OF_INDEXES);
}


void ThreadCacheDC::CreateThreadCacheDC()
{
	CWindowDC dc(::GetDesktopWindow());
	HDC hDC = ::CreateCompatibleDC(dc);
	::TlsSetValue(g_dwThreadCacheDCIndex, hDC);
	::TlsSetValue(g_dwThreadCacheDCIndexMagicNumber, (LPVOID)g_iThreadCacheDCMagicNumber);
}

void ThreadCacheDC::DestroyThreadCacheDC()
{
	HDC hDC = GetThreadCacheDC();
	::DeleteDC(hDC);
	::TlsSetValue(g_dwThreadCacheDCIndex, 0);
	::TlsSetValue(g_dwThreadCacheDCIndexMagicNumber, 0);
}

//////////////////////////////////////////////////////////////////////////
// 不规则区域

HRGN CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF clrTrans, LPCRECT lprcBounds)
{
	HRGN hRgn = NULL;

	if (hBitmap == NULL)
		return NULL;

	BITMAP	bm;
	::GetObject(hBitmap, sizeof(BITMAP), &bm);
	int nWidth = bm.bmWidth;
	int nHeight = bm.bmHeight;

	HDC hhMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFOHEADER bmi = {sizeof(BITMAPINFOHEADER), nWidth, nHeight, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
	void* pBits32 = NULL;

	// 创建位图部件(Section)，获取位数据
	HBITMAP hBmpSection = ::CreateDIBSection(hhMemDC, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &pBits32, NULL, 0);
	if (hBmpSection == NULL)
	{
		::DeleteDC(hhMemDC);
		return NULL;
	}

	HBITMAP hDefBmp = (HBITMAP)::SelectObject(hhMemDC, hBmpSection);
	HDC hTmpDC = ::CreateCompatibleDC(NULL);

	BITMAP bm32;
	::GetObject(hBmpSection, sizeof(BITMAP), &bm32);
	bm.bmWidthBytes = ((bm.bmWidthBytes + 31) & (~31) ) >> 3;

	HBITMAP hTmpBmp = (HBITMAP)::SelectObject(hTmpDC, (HBITMAP)hBitmap);
	::BitBlt(hhMemDC, 0, 0, nWidth, nHeight, hTmpDC, 0, 0, SRCCOPY);
	::SelectObject(hTmpDC, hTmpBmp);
	::DeleteDC(hTmpDC);

	// 区域矩形个数的步长值设为2000
#define ALLOC_UNIT	2000

	DWORD nMaxRects = ALLOC_UNIT;
	RGNDATA* pData = (RGNDATA*)malloc(sizeof(RGNDATAHEADER) + (sizeof(RECT) * nMaxRects));
	if (pData == NULL)
	{
		::SelectObject(hhMemDC, hDefBmp);
		::DeleteObject(hBmpSection);
		::DeleteDC(hhMemDC);
		return NULL;
	}

	// 初始化区域数据结构
	pData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pData->rdh.iType = RDH_RECTANGLES;
	pData->rdh.nCount = 0;
	pData->rdh.nRgnSize = 0;
	::SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

	// 创建区域
	BYTE *p32 = (BYTE*)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
	RECT rcBounds = { 0, 0, nWidth, nHeight };
	if (lprcBounds)
		rcBounds = *lprcBounds;

	for (int y = rcBounds.top; y < rcBounds.bottom; y++)
	{
		for (int x = rcBounds.left; x < rcBounds.right; x++)
		{
			int x0 = x;
			LONG* p = (LONG*)p32 + x;
			while (x < nWidth)
			{	// 指定要滤掉的颜色
				if (((*p) & 0xFFFFFF) == ((LONG)clrTrans & 0xFFFFFF))
					break;
				p++;
				x++;
			}

			if (x > x0)	// 表明有非滤掉的颜色
			{
				if (pData->rdh.nCount >= nMaxRects)
				{	// 如果区域结构中不能再容纳新的矩形数据，则再追加内存
					_ASSERTE(pData->rdh.dwSize == sizeof(RGNDATAHEADER));
					nMaxRects += ALLOC_UNIT;
					RGNDATA* p = (RGNDATA*)realloc((LPVOID)pData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * nMaxRects));
					if (p == NULL)
					{
						if (hRgn != NULL)
							::DeleteObject(hRgn);
						::DeleteObject(::SelectObject(hhMemDC, hDefBmp));
						::DeleteDC(hhMemDC);
						return NULL;
					}
					_ASSERTE(p->rdh.dwSize == sizeof(RGNDATAHEADER));
					pData = p;
				}

				// 刷新区域结构中的数据
				RECT* pr = (RECT*)&pData->Buffer;
				::SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
				if (x0 < pData->rdh.rcBound.left)
					pData->rdh.rcBound.left = x0;
				if (y < pData->rdh.rcBound.top)
					pData->rdh.rcBound.top = y;
				if (x > pData->rdh.rcBound.right)
					pData->rdh.rcBound.right = x;
				if (y+1 > pData->rdh.rcBound.bottom)
					pData->rdh.rcBound.bottom = y+1;
				pData->rdh.nCount++;

				// 因为创建区域的函数ExtCreateRegion()要求区域数据结构中的矩形个数不能超过4000个，
				// 所以这里取极限值的一半。
				if (pData->rdh.nCount == 2000)
				{
					HRGN hTmpRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * nMaxRects), pData);
					if (hRgn != NULL)
					{
						::CombineRgn(hRgn, hRgn, hTmpRgn, RGN_OR);
						::DeleteObject(hTmpRgn);
					}
					else
						hRgn = hTmpRgn;
					pData->rdh.nCount = 0;
					::SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
				}
			}
		}
		// 指向下一行的首地址（DIB扫描行是从下到上）
		p32 -= bm32.bmWidthBytes;
	}

	// 如果整张位图全是指定的要滤掉的颜色，或者区域矩形个数少于
	// 2000，则由以下代码完成创建区域的工作
	HRGN hTmpRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * nMaxRects), pData);
	if (hRgn != NULL)
	{
		::CombineRgn(hRgn, hRgn, hTmpRgn, RGN_OR);
		::DeleteObject(hTmpRgn);
	}
	else
		hRgn = hTmpRgn;

	::free((void*)pData);
	::DeleteObject(::SelectObject(hhMemDC, hDefBmp));
	::DeleteDC(hhMemDC);

	return hRgn;
}