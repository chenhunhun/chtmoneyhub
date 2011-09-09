
#include "stdafx.h"
#include "CoolMenuHook.h"
#include "Draw.h"
#include "..\Skin\SkinManager.h"

const TCHAR MoneyMenuOldProc[] = _T("MoneyMenuOldProc");
std::map<HWND, CCoolMenuHook*> CCoolMenuHook::m_WndMenuMap;
HHOOK CCoolMenuHook::m_hMenuHook = NULL;
HWND CCoolMenuHook::m_hFrameWnd = NULL;
BOOL CCoolMenuHook::m_bHookTheMenu = FALSE;

CCoolMenuHook::CCoolMenuHook(HWND hWnd) : m_hWnd(hWnd)
{

}

CCoolMenuHook::~CCoolMenuHook ()
{
    WNDPROC oldWndProc = (WNDPROC)::GetProp(m_hWnd, MoneyMenuOldProc);
    if (oldWndProc != NULL)
    {
        ::SetWindowLong(m_hWnd, GWL_WNDPROC, (DWORD)(ULONG)oldWndProc);
        ::RemoveProp(m_hWnd, MoneyMenuOldProc);
    }

	m_WndMenuMap.erase(m_hWnd);
}

void CCoolMenuHook::InstallHook(HWND hFrameWnd)
{
    if (m_hMenuHook == NULL)
    {
		m_hFrameWnd = hFrameWnd;
        m_hMenuHook = ::SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcHook, NULL, ::GetCurrentThreadId());
    }
}

void CCoolMenuHook::UnInstallHook()
{
	std::map<HWND, CCoolMenuHook*>::iterator it = m_WndMenuMap.begin();
	for (; it != m_WndMenuMap.end(); ++it)
	{
		CCoolMenuHook* pMenuWndHook = it->second;
		delete pMenuWndHook;
	}
	m_WndMenuMap.clear();

    if (m_hMenuHook != NULL)
    {
        ::UnhookWindowsHookEx(m_hMenuHook);
    }
}

CCoolMenuHook* CCoolMenuHook::GetMenuHook(HWND hwnd)
{
	std::map<HWND, CCoolMenuHook*>::iterator it = m_WndMenuMap.find(hwnd);
	if (it != m_WndMenuMap.end())
	{
		return it->second;
	}

    return NULL;
}

CCoolMenuHook* CCoolMenuHook::AddMenuHook(HWND hwnd)
{
	std::map<HWND, CCoolMenuHook*>::iterator it = m_WndMenuMap.find(hwnd);
	if (it != m_WndMenuMap.end())
	{
		return it->second;
	}

	CCoolMenuHook* pWnd = new CCoolMenuHook(hwnd);
	if (pWnd != NULL)
	{
		m_WndMenuMap[hwnd] = pWnd;
	}

	return pWnd;
}

LRESULT CALLBACK CCoolMenuHook::CallWndProcHook(int code, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT* pStruct = (CWPSTRUCT*)lParam;
	
    while (code == HC_ACTION)
    {
        HWND hWnd = pStruct->hwnd;

        if (pStruct->message != WM_CREATE && pStruct->message != 0x01E2)
        {
            break;
        }

        TCHAR strClassName[10];
        int Count = ::GetClassName(hWnd, strClassName, sizeof(strClassName) / sizeof(strClassName[0]));
        if (Count != 6 || _tcscmp(strClassName, _T("#32768")) != 0)
        {
            break;
        }

        if (::GetProp(pStruct->hwnd, MoneyMenuOldProc) != NULL)
        {
            break;
        }
        
		if (!m_bHookTheMenu)
		{
			break;
		}

		AddMenuHook(pStruct->hwnd);

        WNDPROC oldWndProc = (WNDPROC)(long)::GetWindowLong(pStruct->hwnd, GWL_WNDPROC);
        if (oldWndProc == NULL)
        {
            break;
        }
		
        ATLASSERT(oldWndProc != MoneyMenuProc);

		if (!SetProp(pStruct->hwnd, MoneyMenuOldProc, oldWndProc))
        {
            break;
        }

        if (!SetWindowLong(pStruct->hwnd, GWL_WNDPROC,(DWORD)(ULONG)MoneyMenuProc))
        {
            ::RemoveProp(pStruct->hwnd, MoneyMenuOldProc);
            break;
        }
        break;
    }
    return CallNextHookEx (m_hMenuHook, code, wParam, lParam);
}

LRESULT CALLBACK CCoolMenuHook::MoneyMenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldWndProc = (WNDPROC)::GetProp(hWnd, MoneyMenuOldProc);
    CCoolMenuHook* pWnd = NULL;
	
    switch (uMsg)
    {
		case WM_SIZE:
			{
				LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
				if ((pWnd = GetMenuHook(hWnd)) != NULL)
				{
					pWnd->OnSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				}
				return lResult;
			}
			break;
		case WM_PRINT:
			{
				LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
				if ((pWnd = GetMenuHook(hWnd)) != NULL)
				{
					pWnd->OnPrint((HDC)wParam);
				}
				return lResult;
			}
			break;
		case WM_NCPAINT:
			{
				if ((pWnd = GetMenuHook(hWnd)) != NULL)
				{
					pWnd->OnNcPaint();
					return 0;
				}
			}
			break;

		case WM_SHOWWINDOW:
			{
				if ((pWnd = GetMenuHook(hWnd)) != NULL)
				{
					pWnd->OnShowWindow(wParam != NULL);
				}
			}
			break;
		case WM_NCDESTROY:
			{
				if ((pWnd = GetMenuHook(hWnd)) != NULL)
				{
					pWnd->OnNcDestroy();
				}
			}
			break;
    }

    return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
}

void CCoolMenuHook::OnSize(int, int)
{
	CRect rc;
	GetWindowRect(m_hWnd, &rc);
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, rc.Width() + 1, rc.Height() + 1, 4, 4);
	::SetWindowRgn(m_hWnd, hRgn, TRUE);
	DeleteObject(hRgn);

	return;
}


void CCoolMenuHook::OnNcPaint()
{
	OnPrint(GetWindowDC(m_hWnd));
}

void CCoolMenuHook::OnPrint(HDC hDC)
{
	CRect rc;
	GetWindowRect(m_hWnd, &rc);
    rc.OffsetRect(-rc.TopLeft());

	s()->CoolMenuSkin()->DrawBackgroud(hDC, rc);
}

void CCoolMenuHook::OnNcDestroy()
{
	m_WndMenuMap.erase(m_hWnd);
	delete this;
}

void CCoolMenuHook::OnShowWindow(BOOL bShow)
{
    if (!bShow)
    {
		m_WndMenuMap.erase(m_hWnd);
        delete this;
    }
}

BOOL CCoolMenuHook::TrackPopupMenuEx(HMENU hmenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
{
	m_bHookTheMenu = TRUE;

	BOOL bRet = ::TrackPopupMenuEx(hmenu, fuFlags, x, y, hwnd, lptpm);

	m_bHookTheMenu = FALSE;

	return bRet;
}