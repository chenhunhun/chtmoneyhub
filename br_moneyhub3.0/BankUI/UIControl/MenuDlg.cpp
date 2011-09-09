#include "stdafx.h"
#include "MenuDlg.h"


CMenuDlg::CMenuDlg(HWND hParent):m_hParent(hParent), m_nBtnSelIndex(0), m_nBtnLastSel(0)
{
}
CMenuDlg::~CMenuDlg(void)
{
}

void CMenuDlg::ShowMenuWindow(CPoint pt)
{
	m_nBtnSelIndex = -1;
	m_nBtnLastSel = -1;
	CRect rect;
	::GetClientRect(m_hWnd, &rect);

	// 指定窗口的位置
	::MoveWindow(this->m_hWnd, pt.x - 20, pt.y, rect.Width(), rect.Height(),false); 

	::ShowWindow(m_hWnd, true);
}

void CMenuDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nBtnSelIndex >= 1 || m_nBtnSelIndex <= 4)
	{
		::ShowWindow(m_hWnd, false);
		ATLASSERT (NULL != m_hParent);
		::PostMessage(m_hParent, WM_MY_MENU_CLICKED, m_nBtnSelIndex, 0);
	}
}

void CMenuDlg::OnMouseMove(UINT /* nFlags */, CPoint ptCursor)
{
	int nBeginHight = 14;

	int nBtnHight = 25;

	m_nBtnSelIndex = (ptCursor.y + nBtnHight - 1 - 14)/nBtnHight;

	if (m_nBtnSelIndex > 4 || m_nBtnSelIndex < 1)
		m_nBtnSelIndex = 0;

	if (m_nBtnLastSel != m_nBtnSelIndex)
		Invalidate();// 让客户区域进行重绘


}


LRESULT CMenuDlg::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//EndDialog(0);
	return 0;
}

LRESULT CMenuDlg::OnActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (WA_INACTIVE == wParam)
	{
		::ShowWindow(m_hWnd, false);
	}
	return 0;
}

LRESULT CMenuDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (NULL == m_imgNoBtnSel)
		m_imgNoBtnSel.LoadFromFile(_T("NoSel.png"), true);

	HBITMAP hBitmap = m_imgNoBtnSel.operator HBITMAP();
	if (hBitmap)
	{
		// 根据位图创建客户区域，屏蔽掉红色
		HRGN hRgn = CreateRegionFromBitmap(hBitmap, 0xff0000, NULL);
		int nVal = ::SetWindowRgn(this->m_hWnd, hRgn, true);
	}

	long   lExStyle   =   ::GetWindowLong(m_hWnd,   GWL_EXSTYLE); 
	lExStyle   &=   ~WS_EX_APPWINDOW;
	lExStyle   |=   WS_EX_TOOLWINDOW;
	::SetWindowLong(m_hWnd,   GWL_EXSTYLE,   lExStyle);

	return TRUE;
}

LRESULT CMenuDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	int nBeginHight = 14;

	int nBtnHight = 25;

	// 如果选中的按钮没有发生改变，就没有必要进行重绘
	if (m_nBtnSelIndex == m_nBtnLastSel && m_nBtnSelIndex != -1)
		return 0;

	HBITMAP hBitmap = m_imgNoBtnSel.operator HBITMAP();

	BITMAP	bm;
	::GetObject(hBitmap, sizeof(BITMAP), &bm);
	int nWidth = bm.bmWidth;
	int nHeight = bm.bmHeight;

	HDC hhMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFOHEADER bmi = {sizeof(BITMAPINFOHEADER), nWidth, nHeight, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
	void* pBits32 = NULL;

	// 创建位图部件(Section)（用来复制位图）
	HBITMAP hBmpSection = ::CreateDIBSection(hhMemDC, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &pBits32, NULL, 0);
	if (hBmpSection == NULL)
	{
		::DeleteDC(hhMemDC);
		return 0;
	}

	// 将位图选中到DC中
	HBITMAP hDefBmp = (HBITMAP)::SelectObject(hhMemDC, hBmpSection);

	HDC hTmpDC = ::CreateCompatibleDC(NULL);
	HBITMAP hTmpBmp = (HBITMAP)::SelectObject(hTmpDC, (HBITMAP)hBitmap);
	// 将原来的位图数据复制到新生成的位图中
	::BitBlt(hhMemDC, 0, 0, nWidth, nHeight, hTmpDC, 0, 0, SRCCOPY);

	BYTE* lpBits = NULL;

	if (m_nBtnSelIndex > 0 && m_nBtnSelIndex < 5)
	{
		// 因为GetDIBits 得到的图片是逆过来的
		DWORD dwLoopYB = bm.bmHeight - (nBeginHight + (m_nBtnSelIndex - 1) * (nBtnHight + 1));
		
		PBITMAPINFO bmpInf;  
		int nPaletteSize=0;   
		
		bmpInf = (PBITMAPINFO)LocalAlloc(LPTR,sizeof(BITMAPINFOHEADER) +(bm.bmWidth+7)/8*bm.bmHeight*bm.bmBitsPixel);
		BYTE* buf = ((BYTE*)bmpInf) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*nPaletteSize;  

		bmpInf->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);   
		bmpInf->bmiHeader.biWidth = bm.bmWidth;   
		bmpInf->bmiHeader.biHeight = bm.bmHeight;   
		bmpInf->bmiHeader.biPlanes = bm.bmPlanes;   
		bmpInf->bmiHeader.biBitCount = bm.bmBitsPixel;   
		bmpInf->bmiHeader.biCompression = BI_RGB;   
		bmpInf->bmiHeader.biSizeImage = 0;//(bm.bmWidth+7)/8*bm.bmHeight*bm.bmBitsPixel;

		// 得到位图的数据
		int nLine = GetDIBits(hhMemDC, hBmpSection, 0,(UINT)bm.bmHeight, buf, bmpInf, DIB_RGB_COLORS);

		int nOffset = 0;
		if(bmpInf->bmiHeader.biBitCount == 32)   
		{   
			// 根据用户鼠标的位置，对位图进行选中着色
			for(int i = dwLoopYB - nBtnHight; i < dwLoopYB; i++)   
			{   
				nOffset = i * nWidth * 4;   
				for(int j=0; j<bm.bmWidth; j++)   
				{   
					int b = buf[nOffset+j*4];   
					int g = buf[nOffset+j*4+1];   
					int r = buf[nOffset+j*4+2]; 

					buf[nOffset+j*4] = b * 0.875;
					buf[nOffset+j*4 + 1] = g * 0.885;
					buf[nOffset+j*4 + 2] = r * 0.895;

				}   
			}
		}

		// 将修改后的位图数据写入到位图中
		SetDIBits(hhMemDC, hBmpSection, 0,(UINT)bm.bmHeight, buf, bmpInf, DIB_RGB_COLORS);
		///////////////////////////////////////////////////////////22222222222222222
		/*switch(m_nBtnSelIndex)
		{
		case 1:
			if (NULL != m_imgSelBtn1)
				hBitmap = m_imgSelBtn1.operator HBITMAP();
			break;

		case 2:
			if (NULL != m_imgSelBtn1)
				hBitmap = m_imgSelBtn2.operator HBITMAP();
			break;

		case 3:
			if (NULL != m_imgSelBtn1)
				hBitmap = m_imgSelBtn3.operator HBITMAP();
			break;

		case 4:
			if (NULL != m_imgSelBtn1)
				hBitmap = m_imgSelBtn4.operator HBITMAP();
			break;

		default:
			break;
		}

		::GetObject(hBitmap, sizeof(BITMAP), &bm);
		nWidth = bm.bmWidth;
		nHeight = bm.bmHeight;

		::SelectObject(hTmpDC, (HBITMAP)hBitmap);*/

		//::BitBlt(dc, 0, dwLoopYB, nWidth, nHeight, hTmpDC, 0, 0, SRCCOPY);
		m_nBtnLastSel = m_nBtnSelIndex;
		LocalFree(bmpInf);

		//////////////////////////////////////////////////////11111111111
		//DWORD		dwLoopYB = 0, dwLoopYE = 0, dwLoopXB = 0, dwLoopXE = 0;
		//m_nBtnLastSel = m_nBtnSelIndex;
		//dwLoopYB = nBeginHight + (m_nBtnSelIndex - 1) * (nBtnHight + 1) + 1;
		//dwLoopYE = nBtnHight + dwLoopYB;
		//dwLoopXB = 1;

		//dwLoopXE = m_imgNoBtnSel.GetWidth() - 1;

		//DWORD		dwLoopY = 0, dwLoopX = 0;
		//COLORREF	crPixel = 0;
		//BYTE		byNewPixel = 0;

		//for (dwLoopY = dwLoopYB; dwLoopY < dwLoopYE; dwLoopY++)
		//{
		//	for (dwLoopX = dwLoopXB; dwLoopX < dwLoopXE; dwLoopX++)
		//	{
		//		crPixel = ::GetPixel(dc, dwLoopX, dwLoopY);
		//		//::SetPixel(dc, dwLoopX, dwLoopY, RGB(GetRValue(crPixel) * 0.895, GetGValue(crPixel) * 0.885, GetBValue(crPixel) * 0.875));

		//	} // for
		//} // for

	}
	else
	{
		m_nBtnLastSel = 0;
	}

	// 将位图的数据拷贝到客户DC中去
	::BitBlt(dc, 0, 0, nWidth, nHeight, hhMemDC, 0, 0, SRCCOPY);

	if (NULL !=  lpBits)
		delete []lpBits;

	::DeleteDC(hTmpDC);
	::DeleteDC(hhMemDC);
	::DeleteObject(hBmpSection);
	return 0;
}

LRESULT CMenuDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDOK);
	return 0;
}


HRGN CMenuDlg::CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF clrTrans, LPCRECT lprcBounds)
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
						// 将两个区域合并成一个区域
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