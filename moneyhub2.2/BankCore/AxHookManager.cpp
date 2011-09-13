/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  AxHookManager.cpp
 *      说明：  控件钩子管理
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#include "stdafx.h"
#include "AxUI.h"
#include "AxHookManager.h"
#include "../BankUI/UIControl/CoolMessageBox.h"

CAxHookManager::CAxHookManager()
{
	::TlsSetValue(sm_dwTLSIndex, this);
}


void CAxHookManager::Hook()
{
	m_hAxHook = ::SetWindowsHookEx(WH_CALLWNDPROC, AxWndProc, 0, ::GetCurrentThreadId());
}

void CAxHookManager::Unhook()
{
	if(m_hAxHook)
 		::UnhookWindowsHookEx(m_hAxHook);
 	m_hAxHook = NULL;
}


DWORD CAxHookManager::sm_dwTLSIndex = 0;

void CAxHookManager::Initialize()
{
	sm_dwTLSIndex = ::TlsAlloc();
}
CAxHookManager* CAxHookManager::Get()
{
	return (CAxHookManager*)::TlsGetValue(sm_dwTLSIndex);
}

//////////////////////////////////////////////////////////////////////////
// CShellEmbeddingHook

class CShellEmbeddingHook : public CWindowImpl<CShellEmbeddingHook>
{

public:

	CShellEmbeddingHook(HWND hShellEmbedding)
	{
		SubclassWindow(hShellEmbedding);
	}

	void OnFinalMessage(HWND hwnd)
	{
		delete this;
	}

	BEGIN_MSG_MAP(CShellEmbeddingHook)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
	END_MSG_MAP()

	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		WINDOWPOS *pos = (WINDOWPOS*)lParam;
		if ((pos->flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE))
		{
			HWND hParent = GetParent();
			RECT rcClient;
			::GetClientRect(hParent, &rcClient);
			pos->x = 0;
			pos->y = 0;
			pos->cx = rcClient.right;
			pos->cy = rcClient.bottom;
		}
		return 0;
	}
};


//////////////////////////////////////////////////////////////////////////
// CIEServerHook

class CIEServerHook : public CWindowImpl<CIEServerHook>
{

public:

	CIEServerHook(HWND hIEServer)
	{
		m_uMsgGetObj = ::RegisterWindowMessage( _T("WM_HTML_GETOBJECT") );
		SubclassWindow(hIEServer);
	}

	void OnFinalMessage(HWND hwnd)
	{
		delete this;
	}

	BEGIN_MSG_MAP(CIEServerHook)
		MESSAGE_HANDLER(WM_GETOBJECT, OnGetObject)
		MESSAGE_HANDLER(m_uMsgGetObj, OnGetObject)
	END_MSG_MAP()

	LRESULT OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		return 0;
	}

	UINT m_uMsgGetObj;
};

//////////////////////////////////////////////////////////////////////////
// CWebModalDialog

UINT WM_HTMLDLG_WEBCREATED = ::RegisterWindowMessage(_T("WM_HTMLDLG_WEBCREATED"));

class CWebModalDialog : public CWindowImpl<CWebModalDialog>
{

public:

	CWebModalDialog(HWND hWebModalDlg)
	{
		m_hStatusBar = NULL;
		m_hWebCtrl = NULL;

		SubclassWindow(hWebModalDlg);
	}

	void OnFinalMessage(HWND hwnd)
	{
		if (m_hIcon)
			::DeleteObject(m_hIcon);

		if (m_fontTitle)
			m_fontTitle.DeleteObject();

		delete this;
	}

	BEGIN_MSG_MAP(hWebModalDlg)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MSG_WM_NCPAINT(OnNcPaint)
		MSG_WM_NCMOUSEMOVE(OnNcMouseMove)
		MSG_WM_NCACTIVATE(OnNcActivate)
		MSG_WM_NCLBUTTONDOWN(OnNcLButtonDown)
		MSG_WM_NCLBUTTONUP(OnNcLButtonUp)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		MESSAGE_HANDLER(WM_NCHITTEST, OnHitTest)
		MESSAGE_HANDLER(WM_SHOWWINDOW,OnShowWindow)
		MESSAGE_HANDLER(WM_HTMLDLG_WEBCREATED, OnHtmlDlgWebCreated)
	END_MSG_MAP()

private:
	CRect m_rcButtons;

	CRect m_rcTitleBar;
	CRect m_rcClient;

	CTuoImage m_imgDlgSysCloseBtn;
	CTuoImage m_imgDlgBack;

	bool m_IsFirst;
	CString m_strTitle;
	CFont m_fontTitle;

	int m_btnCloseState;// 0,正常，1，鼠标停留，2，鼠标按下。
	HICON m_hIcon;

public:

	void OnNcLButtonUp(UINT nFlags, CPoint point)
	{
		RECT rcWnd;
		GetWindowRect(&rcWnd);
		point.Offset( - rcWnd.left,- rcWnd.top);
		if(m_rcButtons.PtInRect(point))
		{
			PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
		}
	}

	void OnNcLButtonDown(UINT nHitTest, CPoint point)
	{  
		RECT rcWnd;
		GetWindowRect(&rcWnd);
		point.Offset( - rcWnd.left,- rcWnd.top);
		int oState = m_btnCloseState;
		if(m_rcButtons.PtInRect(point))
		{
			m_btnCloseState = 2;
			CWindowDC dc(m_hWnd);
			AltDrawImagePart(dc, m_rcButtons, m_imgDlgSysCloseBtn, m_btnCloseState, 4);
		}
		else
		{
			m_btnCloseState = oState;
			CWindowDC dc(m_hWnd);
			AltDrawImagePart(dc, m_rcButtons, m_imgDlgSysCloseBtn, m_btnCloseState, 4);
			DefWindowProc();
		}
	}

	void OnNcMouseMove(UINT nHitTest, CPoint point)
	{
		RECT rcWnd;
		GetWindowRect(&rcWnd);
		point.Offset( - rcWnd.left,- rcWnd.top);
		if(m_rcButtons.PtInRect(point))
		{
			m_btnCloseState = 1;
			CWindowDC dc(m_hWnd);
			AltDrawImagePart(dc, m_rcButtons, m_imgDlgSysCloseBtn, m_btnCloseState, 4);
		}
		else
		{
			m_btnCloseState = 0;
			CWindowDC dc(m_hWnd);
			AltDrawImagePart(dc, m_rcButtons, m_imgDlgSysCloseBtn, m_btnCloseState, 4);
		}
	}

	BOOL OnNcActivate(BOOL bActive)
	{
		OnNcPaint(NULL);

		return TRUE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		if (wParam == FALSE)
			return 0;

		LPNCCALCSIZE_PARAMS calc = (LPNCCALCSIZE_PARAMS)lParam;
		RECT &rc = ((LPNCCALCSIZE_PARAMS)lParam)->rgrc[0];

		CRect rcWnd;
		GetWindowRect(&rcWnd);

		rc.top -= ::GetSystemMetrics(SM_CYFRAME) - 1;
		rc.top += DLG_CAPTION_HEIGHT + 1;

		rc.bottom += ::GetSystemMetrics(SM_CYFRAME) - 1;
		rc.bottom -= 2;

		rc.left -= ::GetSystemMetrics(SM_CXFRAME) - 1;
		rc.left += 2;

		rc.right += ::GetSystemMetrics(SM_CXFRAME) - 1;
		rc.right -= 2;

		return 0;
	}

	LRESULT OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		ScreenToClient(&pt);
		pt.y += DLG_CAPTION_HEIGHT;

		if (m_rcTitleBar.PtInRect(pt))
			return HTCAPTION;
		else if (m_rcClient.PtInRect(pt))
			return HTCLIENT;

		return 0;
	}

	static BOOL CALLBACK EnumChildFunc(HWND hwnd, LPARAM lParam)
	{
		CWebModalDialog* pThis = (CWebModalDialog*)lParam;

		TCHAR szClassName[560] = { 0 };
		GetClassName(hwnd, szClassName, _countof(szClassName));
		if (_tcsicmp(szClassName, _T("msctls_statusbar32")) == 0)
		{
			pThis->m_hStatusBar = hwnd;
		}
		else if (_tcsicmp(szClassName, _T("Internet Explorer_Server")) == 0)
		{
			pThis->m_hWebCtrl = hwnd;
		}

		return TRUE;
	}

	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if(m_IsFirst)
		{
			m_IsFirst = false;
			TCHAR szTitle[560] = { 0 };
			GetWindowText(szTitle, _countof(szTitle));
			TCHAR *p = _tcsstr(szTitle, _T("-- "));
			if (p != NULL) *p = '\0';

			m_strTitle = _T("财金汇 - ");
			m_strTitle += szTitle;

			SetWindowText(m_strTitle);

			CRect rect;
			GetWindowRect(&rect);

			HRGN hRgn = ::CreateRoundRectRgn(0, 0, rect.Width() + 1, rect.Height() + 1, 5, 5);
			SetWindowRgn(hRgn);
			DeleteObject(hRgn);

			m_rcTitleBar.SetRect(0, 0, rect.Width(), DLG_CAPTION_HEIGHT);
			m_rcClient.SetRect(0, DLG_CAPTION_HEIGHT, rect.Width(), rect.Height() - DLG_CAPTION_HEIGHT);
		}

		return 1;
	}

	LRESULT OnHtmlDlgWebCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		EnumChildWindows(m_hWnd, EnumChildFunc, (LPARAM)this);

		if (m_hStatusBar != NULL)
		{
			::SendMessage(m_hStatusBar, SB_SETMINHEIGHT, 1, 0L);
		}

		return 0;
	}

	void OnNcPaint(CRgn rgn)
	{
		CRect rcWnd, rcTitle;
		GetWindowRect(&rcWnd);

		CWindowDC wnd_dc(m_hWnd);

		CBitmap bmp;
		bmp.CreateCompatibleBitmap(wnd_dc, rcWnd.Width(), rcWnd.Height());
		CDC dc = CreateCompatibleDC(wnd_dc);
		dc.SelectBitmap(bmp);

		// 取得标题栏的位置
		rcTitle.left = 0;
		rcTitle.top = 0;
		rcTitle.right = rcWnd.right - rcWnd.left;
		rcTitle.bottom = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXDLGFRAME);

// 		// 画灰黑边
// 		CRect rcAcTitle0;
// 		rcAcTitle0.left = rcTitle .left;
// 		rcAcTitle0.top = 0;
// 		rcAcTitle0.right = rcTitle .right;
// 		rcAcTitle0.bottom = rcTitle .bottom;
// 		dc.FillSolidRect(&rcAcTitle0,RGB(96,101,109));
// 		// 画白色条
// 		CRect rcAcTitle1;
// 		rcAcTitle1.left = rcTitle .left + 1;
// 		rcAcTitle1.top = 1;
// 		rcAcTitle1.right = rcTitle .right - 1;
// 		rcAcTitle1.bottom = rcTitle .bottom;
// 
// 		dc.FillSolidRect(&rcAcTitle1,RGB(157,215,255));
// 		// 画里面的条
// 		CRect rcAcTitle2;
// 		rcAcTitle2.left = rcTitle .left + 2;
// 		rcAcTitle2.top = 2;
// 		rcAcTitle2.right = rcTitle .right - 2;
// 		rcAcTitle2.bottom = rcTitle .bottom;
// 		dc.FillSolidRect(&rcAcTitle2,RGB(62,145,200));

		AltDrawDialogBack(dc, rcWnd, &m_imgDlgBack);

// 		// 画左边框
// 		CRect rcAcBorderL;
// 		rcAcBorderL.left = 0;
// 		rcAcBorderL.top = rcTitle.bottom;
// 		rcAcBorderL.right = GetSystemMetrics(SM_CXDLGFRAME);
// 		rcAcBorderL.bottom = rcWnd.bottom - rcWnd.top;
// 
// 		//画灰线
// 		dc.FillSolidRect(&rcAcBorderL,RGB(96,101,109));
// 		CRect rcLLine(rcAcBorderL);
// 		rcLLine.left = 1;
// 		dc.FillSolidRect(&rcLLine,RGB(247,252,255));
// 
// 
// 		// 画右边框
// 		CRect rcAcBorderR;
// 		rcAcBorderR.left = rcTitle.right - GetSystemMetrics(SM_CXDLGFRAME);
// 		rcAcBorderR.top = rcTitle.bottom;
// 		rcAcBorderR.right = rcTitle.right;
// 		rcAcBorderR.bottom = rcWnd.bottom - rcWnd.top;
// 		dc.FillSolidRect(&rcAcBorderR,RGB(96,101,109));
// 		CRect rcRLine(rcAcBorderR);
// 		rcRLine.right -= 1;
// 		dc.FillSolidRect(&rcRLine,RGB(247,252,255));
// 
// 
// 		// 画底框
// 		CRect rcAcBorderB;
// 		rcAcBorderB.left = 0;
// 		rcAcBorderB.top = rcWnd.bottom - rcWnd.top  - GetSystemMetrics(SM_CXDLGFRAME);
// 		rcAcBorderB.right = rcWnd.right - rcWnd.left;
// 		rcAcBorderB.bottom = rcWnd.bottom - rcWnd.top;
// 		dc.FillSolidRect(&rcAcBorderB,RGB(96,101,109));
// 		CRect rcBLine(rcAcBorderB);
// 		rcBLine.bottom -= 1;
// 		rcBLine.right -= 1;
// 		rcBLine.left += 1;
// 		dc.FillSolidRect(&rcBLine,RGB(247,252,255));


		// 画icon
		
		//::DrawIconEx(dc,GetSystemMetrics(SM_CXDLGFRAME),GetSystemMetrics(SM_CYDLGFRAME),m_hIcon, GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYDLGFRAME), GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYDLGFRAME), 0, NULL, DI_NORMAL);

		// 写文字
		CRect rcDraw(rcTitle);
// 		rcDraw.left = GetSystemMetrics(SM_CXDLGFRAME) + GetSystemMetrics(SM_CYCAPTION) + 3;
// 		rcDraw.right = rcTitle.right - 2 - m_imgDlgSysCloseBtn .GetWidth() / 3;
// 		rcDraw.top = rcTitle.top + GetSystemMetrics(SM_CXDLGFRAME) ;
// 		rcDraw.bottom =  rcTitle .bottom;

		//rcDraw.OffsetRect(10, 0);
		rcDraw.DeflateRect(10, 0, m_imgDlgSysCloseBtn.GetWidth() / 4 + 10, 0);
		rcDraw.OffsetRect(0, -1);

		SetBkMode(dc, TRANSPARENT);
		SelectObject(dc, m_fontTitle);
		SetTextColor(dc, RGB(255, 255, 255));
		DrawText(dc, m_strTitle, -1, &rcDraw, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS);


		// 画关闭按钮
		// 计算关闭按钮的位置，位图大小为15 X 15
		// 画ICON
		m_rcButtons.left = rcTitle.right - 1 - m_imgDlgSysCloseBtn .GetWidth() / 4;
		m_rcButtons.top= rcTitle.top + 2;
		m_rcButtons.right = rcTitle.right - 1;
		m_rcButtons.bottom = m_rcButtons.top + m_imgDlgSysCloseBtn.GetHeight();
		AltDrawImagePart(dc, m_rcButtons, m_imgDlgSysCloseBtn, m_btnCloseState, 4); // 初始化关闭按钮
		
		// 标题栏
		::BitBlt(wnd_dc, 0, 0, rcWnd.Width(), DLG_CAPTION_HEIGHT + 1, dc, 0, 0, SRCCOPY);
		// 左右
		::BitBlt(wnd_dc, 0, 0, 2, rcWnd.Height(), dc, 0, 0, SRCCOPY);
		::BitBlt(wnd_dc, rcWnd.Width() - 2, 0, 2, rcWnd.Height(), dc, rcWnd.Width() - 2, 0, SRCCOPY);
		// 底部
		::BitBlt(wnd_dc, 0, rcWnd.Height() - 2, rcWnd.Width(), 2, dc, 0, rcWnd.Height() - 2, SRCCOPY);

		// 这里需要加条件
		
// 		::InvalidateRect(m_hStatusBar, NULL, TRUE);
// 		::UpdateWindow(m_hWebCtrl);
		
		return;
	}

	bool LoadSkins()
	{
		if (NULL == m_imgDlgBack)
			m_imgDlgBack.LoadFromFile(_T("dialog_back.png"), true);
		if (NULL == m_imgDlgSysCloseBtn)
			m_imgDlgSysCloseBtn.LoadFromFile(_T("dialog_sysclose_btn.png"), true);

		return true;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		ModifyStyle(WS_CAPTION, 0);

		m_IsFirst = true;
		m_btnCloseState = 0;// 初始化鼠标状态
		DefWindowProc(uMsg, wParam, lParam);

		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(NULL);
		m_hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
			IMAGE_ICON, 64, 64, LR_DEFAULTCOLOR);

		LoadSkins();		

		if (NULL == m_fontTitle)
			m_fontTitle.CreateFont(13, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));

		return 0;
	}

protected:
	HWND m_hStatusBar;
	HWND m_hWebCtrl;
};


//////////////////////////////////////////////////////////////////////////
// AxWndProc

LRESULT CAxHookManager::AxWndProc(int iCode, WPARAM wParam, LPARAM lParam)
{
	if (iCode == HC_ACTION)
	{
		CWPSTRUCT *pcwp = (CWPSTRUCT*)lParam;
		if (pcwp->message == WM_CREATE)
		{
			CREATESTRUCT *pCS = (CREATESTRUCT*)lParam;

			TCHAR sz[128];
			::GetClassName(pcwp->hwnd, sz, _countof(sz));
			if (_tcscmp(sz, _T("Internet Explorer_Server")) == 0)
			{
				HWND hParent = GetParent(pcwp->hwnd);
				if (hParent && ::IsWindow(hParent))
				{
					TCHAR szClassName[128] = { 0 };
					::GetClassName(hParent, szClassName, _countof(szClassName));
					if (_tcscmp(szClassName, _T("Internet Explorer_TridentDlgFrame")) == 0)
						::PostMessage(hParent, WM_HTMLDLG_WEBCREATED, 0, 0);
				}

				CREATESTRUCT *pcs = (CREATESTRUCT*)pcwp->lParam;
				new CIEServerHook(pcwp->hwnd);
			}
			else if (_tcscmp(sz, _T("Shell Embedding")) == 0)
			{
				CAxHookManager *pAxHook = Get();
				CREATESTRUCT *pcs = (CREATESTRUCT*)pcwp->lParam;
				new CShellEmbeddingHook(pcwp->hwnd);
			}
			else if (_tcscmp(sz, _T("Internet Explorer_TridentDlgFrame")) == 0)
			{
				// 以后可以考虑Hook Dispatch->Invoke 函数
				// 或者访问IDispatchEx接口

				CREATESTRUCT *pcs = (CREATESTRUCT*)pcwp->lParam;
				new CWebModalDialog(pcwp->hwnd);
			}
		}
	}

	return ::CallNextHookEx(NULL, iCode, wParam, lParam);
}
