
#include "stdafx.h"
#include "SecuCheckDlg.h"
#include "CheckList.h"

#include "../Skin/SkinManager.h"
#include "MainFrame.h"
#include "CoolMessageBox.h"
#define CHECK_EVENT 0x81

bool CSecuCheckDlg::m_bShow = true;
bool CSecuCheckDlg::m_bInstall = false;
bool CSecuCheckDlg::m_bEnd = false;
CSecuCheckDlg::CSecuCheckDlg(bool bInstall,bool bShow)
{
	m_bInstall = bInstall;
	m_bShow = bShow; 
	m_dwProgressPos = 0;
	m_dwPercent = 0;
}

LRESULT CSecuCheckDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(NULL);
	HICON hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	if (!m_bShow)
	{
		MoveWindow(100, 100, 1, 1, TRUE);
		ShowWindow(SW_HIDE);
	}

	SetWindowText(SECU_DLG_TITLE);

	CRect rc;
	GetWindowRect(&rc);
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, rc.Width() + 1, rc.Height() + 1, 4, 4);
	SetWindowRgn(hRgn, TRUE);

	SetClassLongPtr(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE) | CS_DROPSHADOW);

	if (m_bShow)
	{
		CenterWindow(GetParent());

		CMenu menu = GetSystemMenu(FALSE);
		menu.ModifyMenu(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);

		m_dwProgressPos = 0;
	}

	SetTimer(1, 200, NULL);

	if(!m_bInstall)
	{
		m_bEnd = false;
		m_dwPercent = 0;
		m_dwProgressPos = 0;
		SetTimer(CHECK_EVENT,100,NULL);
	}
	return TRUE;
}

void CSecuCheckDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		KillTimer(1);
	
		_SecuCheck.SetEventsFunc(_CheckMgrCallback, this);
		if(m_bInstall == true)
		{
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"启动安装检测");
			_SecuCheck.Start(0); // 安装时检查
		}
		else
		{
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"启动启动检测");

			_SecuCheck.Start(1);// 2秒后检查
		}

		return;
	}
	else if(nIDEvent == CHECK_EVENT)
	{
		m_dwProgressPos += 6;
		m_dwPercent += 6;

		if(m_dwProgressPos > 100 || m_dwPercent > 100)
		{
			m_dwProgressPos = 100;
			m_dwPercent = 100;

			if(m_bEnd == true)
				EndDialog(IDOK);
		}
		Invalidate();
	}
}

LRESULT CSecuCheckDlg::OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_bShow)
	{
		CMemDC dc((HDC)wParam);
		RECT rect;
		GetClientRect(&rect);
		s()->Toolbar()->DrawCheckDlgBack(dc, rect);

		dc.SetBkMode(TRANSPARENT);	

		dc.SelectFont(s()->Common()->GetAddrListFontBold());
		dc.SetTextColor(RGB(61, 98, 123));
		::TextOut(dc, 142, 36, m_strInfo, _tcslen(m_strInfo));

		dc.SelectFont(s()->Common()->GetDefaultFont());
		CString str;
		str.Format(_T("已完成%d%%"), m_dwPercent);
		::TextOut(dc, 144, 62, str, str.GetLength());

		RECT rc = { 140, 84, 0, 0 };
		s()->StatusBar()->DrawCheckProgress(dc, rc, m_dwProgressPos);
	}

	return 0;
}

LRESULT CSecuCheckDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}


LRESULT CSecuCheckDlg::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_bShow)
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
	return 0;
}

void CSecuCheckDlg::_CheckMgrCallback(CheckStateEvent ev, DWORD dw, LPCTSTR lpszInfo, LPVOID lp)
{
	CSecuCheckDlg* pThis = (CSecuCheckDlg *)lp;
	if(m_bInstall)
	{
		if (((int)dw) < 0)
		{
			if (lpszInfo != NULL)
			{
			}
		}
	}

	if(m_bShow)
	{
		if (((int)dw) < 0)
		{
			pThis->KillTimer(CHECK_EVENT);
			pThis->ShowWindow(SW_HIDE);
			if(lpszInfo != NULL)
				mhMessageBox(NULL, lpszInfo, SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
			pThis->EndDialog(IDCANCEL);
			return;
		}
		if(((int)dw) > 100)//最大数量为100限制
			dw = 100;

		pThis->m_strInfo = _T("财金汇正在进行安全扫描…");

		switch (ev)
		{
		case CHK_START:
			break;

		case CHK_SELFMODULES:
			pThis->m_dwProgressPos = dw;
			break;

		case CHK_SELFDATAFILES:
			pThis->m_dwProgressPos = dw;
			break;

		case CHK_SELFURLLIST:
			pThis->m_dwProgressPos = dw;
			break;

		case CHK_SELFSYSLIST:
			pThis->m_dwProgressPos = dw;
			break;

		case CHK_SYSLISTHASH:
			pThis->m_dwProgressPos = dw;
			break;

		case CHK_DRVIMAGEPATH:
			pThis->m_dwProgressPos = dw;
			break;

		case CHK_WINSXSDLL:
			pThis->m_dwProgressPos = dw;
			break;

		case CHK_CLOUDLISTHASH:
			pThis->m_dwProgressPos = dw;
			break;

		case CHK_SECURITYCHECK:
			pThis->m_dwProgressPos = dw;
			pThis->m_dwPercent = dw;
			break;
		// 安装的时候不再发送列表
		case CHK_ERROR:
			pThis->EndDialog(IDOK);
			break;

		case CHK_END:
			if(!CSecuCheckDlg::m_bInstall)
			{
				CSecuCheckDlg::m_bEnd = true;
				return;
			}
			else
			{
				if(pThis->m_dwPercent < 100)
					pThis->m_dwPercent = 100;
				pThis->EndDialog(IDOK);				
				return;
			}
		}

		if (dw >= 100)
		{
			static int nStep = 0;
			pThis->m_dwPercent = 100;
		}

		pThis->Invalidate();
	}
	else
	{
		if (((int)dw) < 0)
		{
			pThis->ShowWindow(SW_HIDE);
			if(lpszInfo != NULL)
				mhMessageBox(NULL, lpszInfo, SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
			pThis->EndDialog(IDCANCEL);
			return;
		}

		if (ev == CHK_END)
		{
			pThis->EndDialog(IDOK);
		}

	}
}

LRESULT CSecuCheckDlg::OnSetShowIndex(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_SETSHOWINDEX)
	{
		m_bShow = true;
		CMenu menu = GetSystemMenu(FALSE);
		menu.ModifyMenu(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	}

	return 1;
}

void CSecuCheckDlg::OnShowWindow(BOOL bShowing, int nReason)
{
	if (bShowing)
	{
		CRect rc;
		GetWindowRect(&rc);		
		SetWindowPos(HWND_TOPMOST,rc.left,rc.top,rc.Width(),rc.Height(),SWP_NOSIZE);
	}
}