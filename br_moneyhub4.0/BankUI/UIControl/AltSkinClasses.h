
#pragma once

#include <vector>

/*
	fyc: (Alternative Skin Classes) 这是试验性的皮肤类，会不停改进
	// 贴图方式还是太垃圾
*/


#define DLG_CAPTION_HEIGHT	(32)

static BOOL AltDrawImage(const CTuoImage* pImage, HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight)
{
	ATLASSERT(hDestDC != NULL);
	ATLASSERT(nDestWidth > 0);
	ATLASSERT(nDestHeight > 0);
	ATLASSERT(nSrcWidth > 0);
	ATLASSERT(nSrcHeight > 0);

	HDC hDC = ::CreateCompatibleDC(hDestDC);
	::SelectObject(hDC, (HBITMAP)*pImage);
	BOOL bRet = ::StretchBlt(hDestDC, xDest, yDest, nDestWidth, nDestHeight, hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY);
	DeleteDC(hDC);

	return bRet;
}

static void AltDrawImagePart(CDCHandle dc, const RECT &rcDest, const CTuoImage &image, int iCurrentFrame, int iTotalFrame)
{
	ATLASSERT(image.GetWidth() % iTotalFrame == 0);
	
	int iWidth = image.GetWidth() / iTotalFrame;
	
	ATLASSERT(rcDest.right - rcDest.left == iWidth);
	ATLASSERT(rcDest.bottom - rcDest.top == image.GetHeight());
	
	int iLeft = iWidth * iCurrentFrame;
	RECT rcSrc = { iLeft, 0, iLeft + iWidth, image.GetHeight() };
	AltDrawImage(&image, dc, rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top, rcSrc.left, rcSrc.top, rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top);
}

static void AltDrawDialogBack(HDC hDstDC, const RECT& rcDest, const CTuoImage* pImage)
{
	ATLASSERT(hDstDC != NULL);
	ATLASSERT(pImage != NULL);

	int nDstWidth = rcDest.right - rcDest.left;
	int nDstHeight = rcDest.bottom - rcDest.top;
	int nImgWidth = pImage->GetWidth();
	int nImgHeight = pImage->GetHeight();

	ATLASSERT(nDstWidth >= nImgWidth);
	ATLASSERT(nDstHeight >= nImgHeight);

	int nHalfImgWidth = nImgWidth / 2;
	int nHalfImgHeight = nImgHeight / 2;

	RECT rcImgPart;
	RECT rcDstPart;

	::SetRect(&rcImgPart, 0, 0, nHalfImgWidth, nHalfImgHeight);
	::SetRect(&rcDstPart, 0, 0, nHalfImgWidth, nHalfImgHeight);
	pImage->Draw(hDstDC, rcDstPart, rcImgPart);

	if (nDstWidth > nHalfImgWidth)
	{
		::SetRect(&rcImgPart, nImgWidth - nHalfImgWidth, 0, nImgWidth, nHalfImgHeight);
		::SetRect(&rcDstPart, nDstWidth - nHalfImgWidth, 0, nDstWidth, nHalfImgHeight);
		pImage->Draw(hDstDC, rcDstPart, rcImgPart);
	}

	if (nDstHeight > nHalfImgHeight)
	{
		::SetRect(&rcImgPart, 0, nImgHeight - nHalfImgHeight, nHalfImgWidth, nImgHeight);
		::SetRect(&rcDstPart, 0, nDstHeight - nHalfImgHeight, nHalfImgWidth, nDstHeight);
		pImage->Draw(hDstDC, rcDstPart, rcImgPart);
	}

	if (nDstWidth > nHalfImgWidth && nDstHeight > nHalfImgHeight)
	{
		::SetRect(&rcImgPart, nImgWidth - nHalfImgWidth, nImgHeight - nHalfImgHeight, nImgWidth, nImgHeight);
		::SetRect(&rcDstPart, nDstWidth - nHalfImgWidth, nDstHeight - nHalfImgHeight, nDstWidth, nDstHeight);
		pImage->Draw(hDstDC, rcDstPart, rcImgPart);
	}

	if (nDstWidth > nImgWidth)
	{
		::SetRect(&rcImgPart, nHalfImgWidth, 0, nHalfImgWidth + 1, nHalfImgHeight);
		::SetRect(&rcDstPart, nHalfImgWidth, 0, nDstWidth - nHalfImgWidth, nHalfImgHeight);
		pImage->Draw(hDstDC, rcDstPart, rcImgPart);

		::SetRect(&rcImgPart, nHalfImgWidth, nImgHeight - nHalfImgHeight, nHalfImgWidth + 1, nImgHeight);
		::SetRect(&rcDstPart, nHalfImgWidth, nDstHeight - nHalfImgHeight, nDstWidth - nHalfImgWidth, nDstHeight);
		pImage->Draw(hDstDC, rcDstPart, rcImgPart);
	}

	if (nDstHeight > nImgHeight)
	{
		::SetRect(&rcImgPart, 0, nHalfImgHeight, nHalfImgWidth, nHalfImgHeight + 1);
		::SetRect(&rcDstPart, 0, nHalfImgHeight, nHalfImgWidth, nDstHeight - nHalfImgHeight);
		pImage->Draw(hDstDC, rcDstPart, rcImgPart);

		::SetRect(&rcImgPart, nImgWidth - nHalfImgWidth, nHalfImgHeight, nImgWidth, nHalfImgHeight + 1);
		::SetRect(&rcDstPart, nDstWidth - nHalfImgWidth, nHalfImgHeight, nDstWidth, nDstHeight - nHalfImgHeight);
		pImage->Draw(hDstDC, rcDstPart, rcImgPart);
	}

	if (nDstWidth > nImgWidth && nDstHeight > nImgHeight)
	{
		::SetRect(&rcImgPart, nHalfImgWidth, nHalfImgHeight, nHalfImgWidth + 1, nHalfImgHeight + 1);
		::SetRect(&rcDstPart, nHalfImgWidth, nHalfImgHeight, nDstWidth - nHalfImgWidth, nDstHeight - nHalfImgHeight);
		pImage->Draw(hDstDC, rcDstPart, rcImgPart);
	}
}

static void AltDrawDialogTitle(HDC hDstDC, const RECT& rcDest, LPCTSTR lpszTitle, HFONT hFont)
{
	CDCHandle dc(hDstDC);

	dc.SetTextColor(RGB(255, 255, 255));
	dc.SetBkMode(TRANSPARENT);
	dc.SelectFont(hFont);

	CRect rcDraw(rcDest);
	rcDraw.OffsetRect(10, 1);

	dc.DrawText(lpszTitle, _tcslen(lpszTitle), &rcDraw, DT_SINGLELINE | DT_VCENTER);
}

static HFONT AltGetDefaultFont()
{
	// 同步??
	static CFont m_font;
	if (m_font.IsNull())
		m_font.CreateFont(13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));

	return (HFONT)m_font;
}

// CSkinButton

typedef CWinTraits<WS_CHILD | WS_VISIBLE | BS_OWNERDRAW> SkinButtonTraits;
class CSkinButton : public CWindowImpl<CSkinButton, CWindow, SkinButtonTraits>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CButton::GetWndClassName())

	CSkinButton() :	m_bTracking(false),	m_bButtonDown(false), 
		m_pImageList(NULL), m_iImageCount(0)
	{
		m_crFace = RGB(0x3E, 0x63, 0x7C);
		m_crBack = RGB(0xFF, 0xFF, 0xFF);
		m_crSelected = RGB(0xFF, 0xFF, 0xFF);
		m_crDisabled = RGB(0x87, 0x9F, 0xB0);
	}

	CSkinButton& operator=(const HWND& hControl)
	{
		this->SubclassWindow(hControl);
		this->ModifyStyle(0, SkinButtonTraits::GetWndStyle(0));

		return *this;
	}

	void ApplySkin(CTuoImage* pImageList, int iImageCount)
	{
		ATLASSERT(pImageList != NULL && iImageCount > 0);

		m_pImageList = pImageList;
		m_iImageCount = iImageCount;

		RECT rcWnd = { 0, 0, m_pImageList->GetWidth() / m_iImageCount, m_pImageList->GetHeight() };
		SetWindowPos(NULL, &rcWnd, SWP_NOZORDER | SWP_NOMOVE);
	}

	virtual BOOL DrawSkin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) { return FALSE; }

	BEGIN_MSG_MAP(CSkinButton)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_pImageList == NULL)
		{
			bHandled = FALSE;
			return 0;
		}

		return 1;
	}

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (DrawSkin(uMsg, wParam, lParam, bHandled))
			return 0;

		if (m_pImageList == NULL)
		{
			bHandled = FALSE;
			return 0;
		}

		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
		CRect rect(lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
		CMemoryDC dc(lpdis->hDC, rect);

		RECT rcDraw = { rect.left, rect.top, 
			rect.left + m_pImageList->GetWidth() / m_iImageCount, 
			rect.top + m_pImageList->GetHeight() };

		BOOL bDrawBack = TRUE;
		DWORD dwTextColor = m_crFace;

		UINT itemState = lpdis->itemState;
		if (itemState & ODS_DISABLED && m_iImageCount >= 4)
		{
			CSkinManager::DrawImagePart(dc, rcDraw, *m_pImageList, 3, m_iImageCount);
			dwTextColor = m_crDisabled;
		}
		else if (itemState & ODS_SELECTED && m_iImageCount >= 3)
		{	
			CSkinManager::DrawImagePart(dc, rcDraw, *m_pImageList, 2, m_iImageCount);
			dwTextColor = m_crSelected;
			bDrawBack = FALSE;
		}
		else
		{
			if (m_bTracking && m_iImageCount >= 2)
			{
				CSkinManager::DrawImagePart(dc, rcDraw, *m_pImageList, 1, m_iImageCount);
			}
			else
			{
				CSkinManager::DrawImagePart(dc, rcDraw, *m_pImageList, 0, m_iImageCount);
			}
		}

		CRect rcBack(rcDraw);
		rcBack.OffsetRect(0, 1);

		CString strText;
		GetWindowText(strText);

		dc.SetBkMode(TRANSPARENT);
		dc.SelectFont(AltGetDefaultFont());

		if (bDrawBack)
		{
			dc.SetTextColor(m_crBack);
			::DrawText(dc, strText, -1, &rcBack, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}

		dc.SetTextColor(dwTextColor);
		::DrawText(dc, strText, -1, &rcDraw, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_bTracking)
		{
			TRACKMOUSEEVENT t = {sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_LEAVE, m_hWnd, 0};
			if (::_TrackMouseEvent(&t))
				m_bTracking = false;
		}

		m_bButtonDown = true;
		bHandled = false;

		return 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bButtonDown = false;
		bHandled = false;

		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if ((m_bButtonDown) && (::GetCapture() == m_hWnd))
		{
			POINT p2 = { LOWORD(lParam), HIWORD(lParam) };
			::ClientToScreen(m_hWnd, &p2);
			HWND hHotWnd = ::WindowFromPoint(p2);

			bool Pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
			bool bNeedPressed = (hHotWnd == m_hWnd);
			if (Pressed != bNeedPressed)
			{
				SetState(bNeedPressed ? TRUE : FALSE);
				Invalidate();
			}
		}
		else 
		{
			if (!m_bTracking)
			{
				TRACKMOUSEEVENT t = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
				if (::_TrackMouseEvent(&t))
				{
					m_bTracking = true;
					Invalidate();
				}
			}
		}

		bHandled = false;

		return 0;
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bTracking = false;
		Invalidate();

		return 0;
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::GetCapture() == m_hWnd)
		{
			::ReleaseCapture();
			m_bButtonDown = false;
		}

		bHandled = false;

		return 0;
	}

protected:
	UINT GetState() const
	{
		return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0L);
	}

	void SetState(BOOL bHighlight)
	{
		::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0L);
	}

protected:
	BOOL m_bTracking;
	BOOL m_bButtonDown;

	CTuoImage* m_pImageList;
	int m_iImageCount;

	DWORD m_crFace;
	DWORD m_crBack;
	DWORD m_crSelected;
	DWORD m_crDisabled;
};


// CSkinStatic

class CSkinStatic : public CWindowImpl<CSkinStatic>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CStatic::GetWndClassName())

	CSkinStatic() : m_pBackImage(NULL)
	{
	}

	CSkinStatic& operator=(const HWND& hControl)
	{
		this->SubclassWindow(hControl);
		return *this;
	}

	void ApplySkin(CTuoImage* pBackImage)
	{
		m_pBackImage = pBackImage;
	}
  
 	BEGIN_MSG_MAP(CSkinStatic)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CPaintDC paintdc(m_hWnd);

		CRect rect;
		GetClientRect(&rect);

		if (true)
		{
			CMemoryDC dc(paintdc, rect);

			CBrush brBackground;
			brBackground.CreateSolidBrush(RGB(0xf7, 0xfc, 0xff));
			dc.FillRect(&rect, brBackground);
			
			if (m_pBackImage != NULL)
			{
				CRect rcPos;
				GetWindowRect(&rcPos);
				GetParent().ScreenToClient(&rcPos);
				AltDrawImage(m_pBackImage, dc, 0, 0, rect.Width(), rect.Height(), rcPos.left, rcPos.top, rect.Width(), rect.Height());
			}

			CString strText;
			GetWindowText(strText);

			dc.SelectFont(AltGetDefaultFont());
			dc.SetTextColor(RGB(61, 98, 123));
			::DrawText(dc, strText, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
		}

		return 0;
	}

protected:
	CTuoImage* m_pBackImage;
};


// CSkinProgressBar

class CSkinProgressCtrl : public CWindowImpl<CSkinProgressCtrl, CWindow>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CProgressBarCtrl::GetWndClassName())

	CSkinProgressCtrl() : m_pBackImage(NULL), m_pBarImage(NULL)
	{
	}

	CSkinProgressCtrl& operator=(const HWND& hControl)
	{
		this->SubclassWindow(hControl);
		return *this;
	}

	void ApplySkin(CTuoImage* pBackImage, CTuoImage* pBarImage)
	{
		m_pBackImage = pBackImage;
		m_pBarImage = pBarImage;
	}

	BEGIN_MSG_MAP(CSkinProgressCtrl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CPaintDC paintdc(m_hWnd);

		if (m_pBackImage == NULL || m_pBarImage == NULL)
		{
			bHandled = FALSE;
			return 0;
		}
		else
		{
			CRect rect;
			GetWindowRect(&rect);

// 			CMemDC dc(paintdc);
// 			m_pBackImage->BitBlt(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, rect.Width(), rect.Height());
		}

		return 0;
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

protected:
	CTuoImage* m_pBackImage;
	CTuoImage* m_pBarImage;
};


// CSkinListTabCtrl

typedef CWinTraits<WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_OWNERDRAW> SkinListTabCtrlTraits;
class CSkinListTabCtrl : public CWindowImpl<CSkinListTabCtrl, CWindow, SkinListTabCtrlTraits>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CStatic::GetWndClassName())

	CSkinListTabCtrl() : m_pBackImage(NULL), m_pTabItemImages(NULL), m_iTabItemImagesCount(0),
		m_nSelectedTab(0), m_nHoverTab(-1), m_bTracking(false)
	{
	}

	CSkinListTabCtrl& operator=(const HWND& hControl)
	{
		this->SubclassWindow(hControl);
		this->ModifyStyle(0, SkinListTabCtrlTraits::GetWndStyle(0));

		return *this;
	}

	void ApplySkin(CTuoImage* pBackImage, CTuoImage* pTabItemImages, int iTabItemImagesCount)
	{
		m_pBackImage = pBackImage;
		m_pTabItemImages = pTabItemImages;
		m_iTabItemImagesCount = iTabItemImagesCount;
	}

	BEGIN_MSG_MAP(CSkinListTabCtrl)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
	END_MSG_MAP()

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
		CRect rect(lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
		CMemoryDC dc(lpdis->hDC, rect);

		if (m_pBackImage == NULL)
		{
			CBrush brBackground;
			brBackground.CreateSolidBrush(RGB(222, 239, 248));
			dc.FillRect(&rect, brBackground);
		}
		else
		{
			CRect rcPos;
			GetWindowRect(&rcPos);
			GetParent().ScreenToClient(&rcPos);
			m_pBackImage->BitBlt(dc, 0, 0, rect.Width(), rect.Height(), rcPos.left, rcPos.top, rect.Width(), rect.Height());
		}
		
		dc.SetBkMode(TRANSPARENT);
		dc.SelectFont(AltGetDefaultFont());

		CRect rcTabItem = rect, rcImage, rcText;
		rcTabItem.DeflateRect(5, 5, 5, 0);
		rcTabItem.bottom = rcTabItem.top + 22;

		CBrush brTabItem;
		brTabItem.CreateSolidBrush(RGB(40, 115, 167));
		for (size_t i = 0; i < m_vecListTabs.size(); i++)
		{
			if (m_nSelectedTab == (int)i)
			{
				dc.SetTextColor(RGB(255, 255, 255));
				dc.FillRect(rcTabItem, brTabItem);
			}
			else
			{
				if (m_nHoverTab == (int)i)
					dc.SetTextColor(RGB(50, 120, 167));
				else
					dc.SetTextColor(RGB(0, 70, 117));
			}


			int nImageWidth = m_pTabItemImages->GetWidth() / m_iTabItemImagesCount;
			int nImageHeight = m_pTabItemImages->GetHeight();

			rcImage.SetRect(rcTabItem.left + 10, rcTabItem.top, rcTabItem.left + 50, rcTabItem.bottom);
			CPoint ptCenter = rcImage.CenterPoint();
			rcImage.left = ptCenter.x - nImageWidth / 2;
			rcImage.top = ptCenter.y - nImageHeight / 2;
			rcImage.right = rcImage.left + nImageWidth;
			rcImage.bottom = rcImage.top + nImageHeight;

			CSkinManager::DrawImagePart(dc, rcImage, *m_pTabItemImages, i, m_iTabItemImagesCount);

			rcText = rcTabItem;
			rcText.left += 50;
			dc.DrawText(m_vecListTabs[i], -1, &rcText, DT_VCENTER | DT_SINGLELINE);

			rcTabItem.OffsetRect(0, 32);
		}
			
		return 0;
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CRect rect;
		GetClientRect(&rect);

		CRect rcTabItem = rect;
		rcTabItem.DeflateRect(5, 5, 5, 0);
		rcTabItem.bottom = rcTabItem.top + 22;

		int nSelectedTab = -1;
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		for (size_t i = 0; i < m_vecListTabs.size(); i++)
		{
			if (rcTabItem.PtInRect(pt))
			{
				nSelectedTab = i;
				break;
			}

			rcTabItem.OffsetRect(0, 32);
		}
		
		if (nSelectedTab != -1 && m_nSelectedTab != nSelectedTab)
		{
			m_nSelectedTab = nSelectedTab;
			Invalidate();

			::PostMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), 0), (LPARAM)m_hWnd);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_bTracking)
		{
			CRect rect;
			GetClientRect(&rect);

			CRect rcTabItem = rect;
			rcTabItem.DeflateRect(5, 5, 5, 0);
			rcTabItem.bottom = rcTabItem.top + 22;

			int nHoverTab = -1;
 			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			for (size_t i = 0; i < m_vecListTabs.size(); i++)
			{
				if (rcTabItem.PtInRect(pt))
				{
					nHoverTab = i;
					break;
				}

				rcTabItem.OffsetRect(0, 32);
			}

			if (m_nHoverTab != nHoverTab)
			{
				m_nHoverTab = nHoverTab;
				Invalidate();
			}
		}
		else
		{
			TRACKMOUSEEVENT t = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
			if (::_TrackMouseEvent(&t))
			{
				m_bTracking = true;
				Invalidate();
			}
		}

		bHandled = false;

		return 0;
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bTracking = false;
		Invalidate();

		return 0;
	}

public:
	int AddTab(CString strText, bool bRedraw = false)
	{
		m_vecListTabs.push_back(strText);
		if (bRedraw)
			Invalidate();

		return m_vecListTabs.size();
	}

	int GetCurTab() const
	{
		return m_nSelectedTab;
	}

	void SetCurTab(size_t nIndex) // gao
	{	
		ATLASSERT (nIndex >= 0 && nIndex < m_vecListTabs.size ());
		if (nIndex < 0 || nIndex >= m_vecListTabs.size ())
			return;
		m_nSelectedTab = nIndex;
		Invalidate ();
	}

protected:
	CTuoImage* m_pBackImage;
	CTuoImage* m_pTabItemImages;
	int m_iTabItemImagesCount;

	std::vector<CString> m_vecListTabs;
	int m_nSelectedTab;
	int m_nHoverTab;

	bool m_bTracking;
};


template<class T>
class CDialogSkinMixer
{
public:
	~CDialogSkinMixer()
	{
		for (size_t i = 0; i < m_vecButtons.size(); i++)
		{
			CSkinButton* pButton = m_vecButtons[i];
			if (pButton != NULL)
			{
				if (pButton->IsWindow())
					pButton->UnsubclassWindow();
				delete pButton;
			}
		}

		for (size_t i = 0; i < m_vecStatics.size(); i++)
		{
			CSkinStatic* pStatic = m_vecStatics[i];
			if (pStatic != NULL)
			{
				if (pStatic->IsWindow())
					pStatic->UnsubclassWindow();
				delete pStatic;
			}
		}

		m_vecButtons.clear();
		m_vecStatics.clear();
	}

public:
	BEGIN_MSG_MAP(CDialogSkinMixer)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDC_SYSCLOSE, OnSysClose)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		
		T* pThis = static_cast<T*>(this);

		m_crTextColor = RGB(61, 98, 123);

		if (NULL == m_fontTitle)
			m_fontTitle.CreateFont(13, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));
		if (NULL == m_fontText)
			m_fontText.CreateFont(13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));

		if (NULL == m_imgDlgBack)
			m_imgDlgBack.LoadFromFile(_T("dialog_back.png"), true);
		if (NULL == m_imgDlgSysCloseBtn)
			m_imgDlgSysCloseBtn.LoadFromFile(_T("dialog_sysclose_btn.png"), true);
		if (NULL == m_imgDlgButton)
			m_imgDlgButton.LoadFromFile(_T("button_back.png"), true);

		pThis->PreCreate();

		CRect rect;
		pThis->GetWindowRect(&rect);
		m_rcTitleBar.SetRect(0, 0, rect.Width(), DLG_CAPTION_HEIGHT);

		HRGN hRgn = ::CreateRoundRectRgn(0, 0, rect.Width() + 1, rect.Height() + 1, 5, 5);
		pThis->SetWindowRgn(hRgn);
		DeleteObject(hRgn);

		if (NULL == m_btnSysClose)
		{
			m_btnSysClose.Create(pThis->m_hWnd, &rect, _T(""), 0, 0, IDC_SYSCLOSE);
			m_btnSysClose.ApplySkin(&m_imgDlgSysCloseBtn, 4);
		}

		CRect rcClose;
		m_btnSysClose.GetWindowRect(&rcClose);
		int w = rcClose.Width();
		int h = rcClose.Height();
		rcClose.SetRect(rect.Width() - w - 1, 2, rect.Width() - 1, h + 2);
		m_btnSysClose.MoveWindow(&rcClose);

		return TRUE;
	}

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pThis = static_cast<T*>(this);

		CRect rect;
		pThis->GetWindowRect(rect);

		CString strTitle;
		pThis->GetWindowText(strTitle);

		CMemDC dc((HDC)wParam);
		AltDrawDialogBack(dc, rect, &m_imgDlgBack);
		AltDrawDialogTitle(dc, m_rcTitleBar, strTitle, (HFONT)m_fontTitle);

		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		T* pThis = static_cast<T*>(this);

		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if (m_rcTitleBar.PtInRect(pt))
		{
			pThis->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
			return 0;
		}

		return 0;
	}

	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		TCHAR szClassName[256];
		if (GetClassName((HWND)lParam, szClassName, _countof(szClassName)))
		{
			if ((_tcsicmp(szClassName, CStatic::GetWndClassName()) == 0) || (_tcsicmp(szClassName, CLinkCtrl::GetWndClassName()) == 0))
			{
				CDCHandle dc((HDC)wParam);
				dc.SetTextColor(m_crTextColor);
				dc.SetBkMode(TRANSPARENT);

				return LRESULT(GetStockObject(NULL_BRUSH)); 
			}
		}

		return NULL;
	}

	LRESULT OnSysClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pThis = static_cast<T*>(this);

		if (!pThis->DoSysClose())
			pThis->EndDialog(IDCANCEL);

		return 0;
	}

public:
	void ApplyButtonSkin(UINT nID)
	{
		T* pThis = static_cast<T*>(this);
		
		HWND hButton = pThis->GetDlgItem(nID);
		if (hButton != NULL)
		{
			CSkinButton* pButton = new CSkinButton;
			*pButton = hButton;
			pButton->ApplySkin(&m_imgDlgButton, 4);

			m_vecButtons.push_back(pButton);
		}
	}

	void ApplyStaticSkin(UINT nID)
	{
		T* pThis = static_cast<T*>(this);

		HWND hStatic = pThis->GetDlgItem(nID);
		if (hStatic != NULL)
		{
			CSkinStatic* pStatic = new CSkinStatic;
			*pStatic = hStatic;
			//pStatic->ApplySkin(&m_imgDlgButton, 4);

			m_vecStatics.push_back(pStatic);
		}

	}

	bool DoSysClose()
	{
		return false;
	}

	void PreCreate()
	{
	}

protected:
	DWORD m_crTextColor;
	DWORD m_crBackColor;

	CFont m_fontTitle;
	CFont m_fontText;

	CTuoImage m_imgDlgBack;
	CTuoImage m_imgDlgSysCloseBtn;
	CTuoImage m_imgDlgButton;

	CRect m_rcTitleBar;

	CSkinButton m_btnSysClose;
	std::vector<CSkinButton*> m_vecButtons;
	std::vector<CSkinStatic*> m_vecStatics;
};

//checkbox的自绘
typedef CWinTraits<WS_CHILD | WS_VISIBLE | BS_OWNERDRAW> SkinCheckBoxTraits;
class CSkinCheckBox : public CWindowImpl<CSkinCheckBox, CWindow, SkinCheckBoxTraits>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CButton::GetWndClassName())

	CSkinCheckBox() :	m_bTracking(false),	m_bButtonDown(false), 
		m_pImageList(NULL), m_iImageCount(0), m_bSelected(false)
	{

	}

	CSkinCheckBox& operator=(const HWND& hControl)
	{
		this->SubclassWindow(hControl);
		this->ModifyStyle(0, SkinCheckBoxTraits::GetWndStyle(0));

		return *this;
	}


	void ApplySkin(CTuoImage* pImageList, int iImageCount)
	{
		ATLASSERT(pImageList != NULL && iImageCount > 0);

		m_pImageList = pImageList;
		m_iImageCount = iImageCount;


		RECT rcWnd = { 0, 0, m_pImageList->GetWidth() / m_iImageCount, m_pImageList->GetHeight() };
		SetWindowPos(NULL, &rcWnd, SWP_NOZORDER | SWP_NOMOVE);
	}

	virtual BOOL DrawSkin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) { return FALSE; }

	BEGIN_MSG_MAP(CSkinCheckBox)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_pImageList == NULL)
		{
			bHandled = FALSE;
			return 0;
		}

		return 1;
	}

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (DrawSkin(uMsg, wParam, lParam, bHandled))
			return 0;

		if (m_pImageList == NULL)
		{
			bHandled = FALSE;
			return 0;
		}

		LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
		CRect rect(lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem.right, lpdis->rcItem.bottom);
		CMemoryDC dc(lpdis->hDC, rect);

		RECT rcDraw = { rect.left, rect.top, 
			rect.left + m_pImageList->GetWidth() / m_iImageCount, 
			rect.top + m_pImageList->GetHeight() };

		BOOL bDrawBack = TRUE;
		UINT itemState = lpdis->itemState;
		if(m_bSelected)
		{
			if (itemState & ODS_DISABLED && m_iImageCount >= 8)
			{
				AltDrawImagePart(dc, rcDraw, *m_pImageList, 3 + (m_iImageCount / 2), m_iImageCount);
			}
			else if (itemState & ODS_SELECTED && m_iImageCount >= 6)
			{	
				AltDrawImagePart(dc, rcDraw, *m_pImageList, 2 + (m_iImageCount / 2), m_iImageCount);
				bDrawBack = FALSE;
			}
			else
			{
				if (m_bTracking && m_iImageCount >= 4)
				{
					AltDrawImagePart(dc, rcDraw, *m_pImageList, 1 + (m_iImageCount / 2), m_iImageCount);
				}
				else
				{
					AltDrawImagePart(dc, rcDraw, *m_pImageList, 0 + (m_iImageCount / 2), m_iImageCount);
				}
			}
		}
		else
		{
			if (itemState & ODS_DISABLED && m_iImageCount >= 8)
			{
				AltDrawImagePart(dc, rcDraw, *m_pImageList, 3, m_iImageCount);
			}
			else if (itemState & ODS_SELECTED && m_iImageCount >= 6)
			{	
				AltDrawImagePart(dc, rcDraw, *m_pImageList, 2, m_iImageCount);
				bDrawBack = FALSE;
			}
			else
			{
				if (m_bTracking && m_iImageCount >= 4)
				{
					AltDrawImagePart(dc, rcDraw, *m_pImageList, 1, m_iImageCount);
				}
				else
				{
					AltDrawImagePart(dc, rcDraw, *m_pImageList, 0, m_iImageCount);
				}
			}

		}

		return 0;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_bTracking)
		{
			TRACKMOUSEEVENT t = {sizeof(TRACKMOUSEEVENT), TME_CANCEL | TME_LEAVE, m_hWnd, 0};
			if (::_TrackMouseEvent(&t))
				m_bTracking = false;
		}

		m_bSelected = !m_bSelected;

		m_bButtonDown = true;
		bHandled = false;

		return 0;
	}

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bButtonDown = false;
		bHandled = false;

		return 0;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if ((m_bButtonDown) && (::GetCapture() == m_hWnd))
		{
			POINT p2 = { LOWORD(lParam), HIWORD(lParam) };
			::ClientToScreen(m_hWnd, &p2);
			HWND hHotWnd = ::WindowFromPoint(p2);

			bool Pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
			bool bNeedPressed = (hHotWnd == m_hWnd);
			if (Pressed != bNeedPressed)
			{
				SetState(bNeedPressed ? TRUE : FALSE);
				Invalidate();
			}
		}
		else 
		{
			if (!m_bTracking)
			{
				TRACKMOUSEEVENT t = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
				if (::_TrackMouseEvent(&t))
				{
					m_bTracking = true;
					Invalidate();
				}
			}
		}

		bHandled = false;

		return 0;
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bTracking = false;
		Invalidate();

		return 0;
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::GetCapture() == m_hWnd)
		{
			::ReleaseCapture();
			m_bButtonDown = false;
		}

		bHandled = false;

		return 0;
	}
	void SetSelectedState(bool bSelected)
	{
		m_bSelected = bSelected;
		Invalidate();
	}

	bool GetSelectedState(){return m_bSelected;}


protected:
	UINT GetState() const
	{
		return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0L);
	}

	void SetState(BOOL bHighlight)
	{
		::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0L);
	}

protected:
	BOOL m_bTracking;
	BOOL m_bButtonDown;
	bool m_bSelected;

	CTuoImage* m_pImageList;
	int m_iImageCount;
};