#include "stdafx.h"
#include "UserLoadInfoDlg.h"

#define SEND_MAIL_KEEP_TIME 3

CUserLoadInfoDlg::CUserLoadInfoDlg(HWND hParent, LPCTSTR lpBitPath):
m_hParent(hParent),m_wstrBitPath(lpBitPath), m_bTimerTag(false), m_bShow(false)//, m_pBtnRetry(NULL), m_pBtnChange(NULL)
{
}
CUserLoadInfoDlg::~CUserLoadInfoDlg(void)
{
}

void CUserLoadInfoDlg::ShowMenuWindow(CPoint pt)
{
	CRect rect;
	::GetClientRect(m_hWnd, &rect);

	bool bBack = SetWindowPos(HWND_TOP, pt.x - 20, pt.y, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	
	m_bShow = true;
	::ShowWindow(m_hWnd, m_bShow);
}


LRESULT CUserLoadInfoDlg::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

void CUserLoadInfoDlg::InitSendMailTime(void)
{
	CTime timePast(1970, 3, 19, 22, 15, 0);
	m_LastTime = timePast;
}

LRESULT CUserLoadInfoDlg::OnActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (WA_INACTIVE == wParam && !m_bTimerTag)
	{
		m_bShow = false;
		::ShowWindow(m_hWnd, m_bShow);
	}
	return 0;
}

void CUserLoadInfoDlg::KeepWindowForSeconds(int nSecond)
{
	if (false == m_bTimerTag)
	{
		SetTimer(1, 1000 * nSecond, NULL);
		m_bTimerTag = true;
	}
}

void CUserLoadInfoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		m_bTimerTag = false;
		m_bShow = false;
		::ShowWindow(m_hWnd, m_bShow);
		KillTimer(1);
	}
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

	return TRUE;
}

LRESULT CUserLoadInfoDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	m_imgNoBtnSel.Draw(dc, 0, 0, m_imgNoBtnSel.GetWidth(), m_imgNoBtnSel.GetHeight(), 0, 0, m_imgNoBtnSel.GetWidth(), m_imgNoBtnSel.GetHeight());
	return 0;
}

LRESULT CUserLoadInfoDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDOK);
	return 0;
}

void CUserLoadInfoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rect1(19, 200, 106, 225); // 重发邮件区域
	CRect rect2(122, 200, 209, 225); // 更改邮箱区域
	CRect rect3(223, 207, 269, 218); // 我知道了区域

	if (rect1.PtInRect(point) || rect2.PtInRect(point) || rect3.PtInRect(point))
	{
		// 将鼠标设置成手型
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
	}
}

void CUserLoadInfoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect1(19, 200, 106, 225); // 重发邮件区域
	CRect rect2(122, 200, 209, 225); // 更改邮箱区域
	CRect rect3(223, 207, 269, 218); // 我知道了区域


	if (rect1.PtInRect(point))
	{
		// 将鼠标设置成手型
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));

		// 启动几秒以后自动关闭功能
		if (false == m_bTimerTag)
		{
			SetTimer(1, 1000 * SEND_MAIL_KEEP_TIME, NULL);
			m_bTimerTag = true;
		}

		int n = m_LastTime.GetYear() ;
		if (m_LastTime.GetYear() == 1970)
		{
			m_LastTime = CTime::GetCurrentTime();
		}
		else
		{
			CTime tNow = CTime::GetCurrentTime();
			CTimeSpan timeSpan = tNow - m_LastTime;
			int nMin = timeSpan.GetMinutes();
			if (nMin < 5) // 5分钟之内只能重发一次
				return;

			m_LastTime = tNow;
		}
		::SendMessage(m_hParent, WM_RESEND_VERIFY_MAIL, 0, 0);
		

		//ShowWindow(FALSE);
	}
	else if (rect2.PtInRect(point))
	{
		// 将鼠标设置成手型
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		::SendMessage(m_hParent, WM_USER_INFO_MENU_CLICKED, MY_USER_INFO_MENU_CLICK_MAILCHANGE, 0);
	}
	else if (rect3.PtInRect(point))
	{
		// 将鼠标设置成手型
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		m_bShow = false;
		ShowWindow(m_bShow);
	}
}

bool CUserLoadInfoDlg::IsWindowShow(void)
{
	return m_bShow;
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