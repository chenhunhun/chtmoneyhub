#include "stdafx.h"
#include "UserLoadInfoDlg.h"


CUserLoadInfoDlg::CUserLoadInfoDlg(HWND hParent, LPCTSTR lpBitPath):
m_hParent(hParent),m_wstrBitPath(lpBitPath)//, m_pBtnRetry(NULL), m_pBtnChange(NULL)
{
}
CUserLoadInfoDlg::~CUserLoadInfoDlg(void)
{
	//::DeleteObject(m_Font);

	/*if (NULL != m_pBtnChange)
		delete m_pBtnChange;
	m_pBtnChange = NULL;

	if (NULL != m_pBtnRetry)
		delete m_pBtnRetry;
	m_pBtnRetry = NULL;*/
}

void CUserLoadInfoDlg::ShowMenuWindow(CPoint pt)
{
	CRect rect;
	::GetClientRect(m_hWnd, &rect);

	// 指定窗口的位置
	::MoveWindow(this->m_hWnd, pt.x - 20, pt.y, rect.Width(), rect.Height(),false); 

	::ShowWindow(m_hWnd, true);
}


LRESULT CUserLoadInfoDlg::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CUserLoadInfoDlg::OnActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (WA_INACTIVE == wParam)
	{
		::ShowWindow(m_hWnd, false);
	}
	return 0;
}

LRESULT CUserLoadInfoDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (NULL == m_imgNoBtnSel)
		m_imgNoBtnSel.LoadFromFile(m_wstrBitPath.c_str(), true);

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

	//m_Font.CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));
	//
	//CRect rectBtn(20, 180, 105, 207);
	//m_pBtnRetry = new CButton();
	//m_pBtnRetry->Create(m_hWnd, rectBtn, _T("重发邮件"), WS_CHILD|WS_VISIBLE, 0, 7000);
	//m_pBtnRetry->SetFont(m_Font);
	////m_pBtnRetry->SetColor(RGB(120, 133, 142));

	//CRect rectBtn2(125, 180, 205, 207);
	//m_pBtnChange = new CButton();
	//m_pBtnChange->Create(m_hWnd, rectBtn2, _T("更改邮箱"), WS_CHILD|WS_VISIBLE, 0, 7001);
	//m_pBtnChange->SetFont(m_Font);

	//SetFocus();
	//ApplyButtonSkin(7000);

	return TRUE;
}

LRESULT CUserLoadInfoDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	/*HBITMAP hBitmap = m_imgNoBtnSel.operator HBITMAP();

	BITMAP	bm;
	::GetObject(hBitmap, sizeof(BITMAP), &bm);
	int nWidth = bm.bmWidth;
	int nHeight = bm.bmHeight;*/


	//HDC hhMemDC = ::CreateCompatibleDC(NULL);
	//BITMAPINFOHEADER bmi = {sizeof(BITMAPINFOHEADER), nWidth, nHeight, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
	//void* pBits32 = NULL;

	//// 创建位图部件(Section)（用来复制位图）
	//HBITMAP hBmpSection = ::CreateDIBSection(hhMemDC, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &pBits32, NULL, 0);
	//if (hBmpSection == NULL)
	//{
	//	::DeleteDC(hhMemDC);
	//	return 0;
	//}

	//// 将位图选中到DC中
	//HBITMAP hDefBmp = (HBITMAP)::SelectObject(hhMemDC, hBmpSection);

	//HDC hTmpDC = ::CreateCompatibleDC(NULL);
	//HBITMAP hTmpBmp = (HBITMAP)::SelectObject(hTmpDC, (HBITMAP)hBitmap);
	//// 将原来的位图数据复制到新生成的位图中
	//::BitBlt(hhMemDC, 0, 0, nWidth, nHeight, hTmpDC, 0, 0, SRCCOPY);


	//// 将位图的数据拷贝到客户DC中去
	//::BitBlt(dc, 0, 0, nWidth, nHeight, hhMemDC, 0, 0, SRCCOPY);


	//::DeleteDC(hTmpDC);
	//::DeleteDC(hhMemDC);
	//::DeleteObject(hBmpSection);
	m_imgNoBtnSel.Draw(dc, 0, 0, m_imgNoBtnSel.GetWidth(), m_imgNoBtnSel.GetHeight(), 0, 0, m_imgNoBtnSel.GetWidth(), m_imgNoBtnSel.GetHeight());
	return 0;
}

LRESULT CUserLoadInfoDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDOK);
	return 0;
}

void CUserLoadInfoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect1(19, 182, 106, 207); // 重发邮件区域
	CRect rect2(122, 182, 209, 207); // 更改邮箱区域
	CRect rect3(223, 189, 269, 200); // 我知道了区域

	if (rect1.PtInRect(point))
	{
		::MessageBox(NULL, L"重发邮件区域", L"重发邮件区域", MB_OK);
	}
	else if (rect2.PtInRect(point))
	{
		::MessageBox(NULL, L"更改邮箱区域", L"更改邮箱区域", MB_OK);
	}
	else if (rect3.PtInRect(point))
	{
		::MessageBox(NULL, L"我知道了区域", L"我知道了区域", MB_OK);
	}
}


HRGN CUserLoadInfoDlg::CreateRegionFromBitmap(HBITMAP hBitmap, COLORREF clrTrans, LPCRECT lprcBounds)
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