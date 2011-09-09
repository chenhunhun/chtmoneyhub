#pragma once


// 下面的status是给skin用的
enum ButtonStatus
{
	Btn_MouseOut = 0,
	Btn_MouseOver = 1,
	Btn_MouseDown = 2,
	Btn_Disabled = 3,
	Btn_Addition = 4
};

class CTuoImage
{

public:

	CTuoImage();
	~CTuoImage();

	void ResetCacheDC();

	operator HBITMAP() const { return m_hBitmap; }
	HBITMAP SplitBitmapHandleOut();				// 将bitmap句柄的生命周期终结，移交给其他人管理

	bool LoadFromFile(bool bPNG = true);
	bool LoadFromFile(LPCTSTR lpszFileName, bool bShortName = false);
	void CreateDirect(void *pData, DWORD dwSize);
	void DestroyBitmap();

	int GetWidth() const { return m_iWidth; }
	int GetHeight() const { return m_iHeight; }
	bool IsNull() const { return m_hBitmap == NULL; }
	void ForceUseBitlbt() { m_bUseBitblt = true; }		// 一般layered windows中使用的图片要调用这个函数

	BOOL Draw(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight) const;

	BOOL Draw(HDC hDestDC) const
	{
		return Draw(hDestDC, 0, 0, m_iWidth, m_iHeight, 0, 0, m_iWidth, m_iHeight);
	}
	BOOL Draw(HDC hDestDC, const RECT &rectDest, const RECT &rectSrc) const
	{
		return Draw(hDestDC, rectDest.left, rectDest.top, rectDest.right - rectDest.left, rectDest.bottom - rectDest.top, rectSrc.left, rectSrc.top, rectSrc.right - rectSrc.left, rectSrc.bottom - rectSrc.top);
	}
	BOOL Draw(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc) const
	{
		return Draw(hDestDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nDestWidth, nDestHeight);
	}

	BOOL BitBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight) const;
	BOOL BitBlt(HDC hDestDC, const RECT &rectDest, const RECT &rectSrc) const
	{
		return BitBlt(hDestDC, rectDest.left, rectDest.top, rectDest.right - rectDest.left, rectDest.bottom - rectDest.top, rectSrc.left, rectSrc.top, rectSrc.right - rectSrc.left, rectSrc.bottom - rectSrc.top);
	}

	std::wstring m_strFileName;

private:

	int m_iWidth;
	int m_iHeight;
	bool m_bUseBitblt;

	HBITMAP m_hBitmap;
	HDC m_hBufferDC;
};

//////////////////////////////////////////////////////////////////////////

class CTuoIcon : public CIcon
{
public:

	bool LoadFromFile();

	std::wstring m_strFileName;
};

//////////////////////////////////////////////////////////////////////////

struct ThreadCacheDC
{
	static DWORD g_dwThreadCacheDCIndex;
	static DWORD g_dwThreadCacheDCIndexMagicNumber;

	static const int g_iThreadCacheDCMagicNumber = 0x19840718;


	static inline HDC GetThreadCacheDC()
	{
		if (::TlsGetValue(g_dwThreadCacheDCIndexMagicNumber) != (LPVOID)g_iThreadCacheDCMagicNumber)
			return NULL;
		return (HDC)::TlsGetValue(g_dwThreadCacheDCIndex);
	}

	static void InitializeThreadCacheDC();		// 在启动时主线程只调用一次

	static void CreateThreadCacheDC();			// 每个线程启动时调用一次
	static void DestroyThreadCacheDC();		// 每个线程结束时调用一次
};

HRGN CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF clrTrans, LPCRECT lprcBounds);
