
#include "stdafx.h"
#include "CoolMessageBox.h"
#include "SaveFavProgress.h"
#include "ShowInfoBar.h"
DWORD WINAPI _threadShowMessage(LPVOID lp);

DWORD WINAPI  _threadShowWaitDLG(LPVOID lp)
{
	ThreadCacheDC::CreateThreadCacheDC();

	if (NULL == g_pSkin)
		new CSkinManager();

	g_pSkin->RefreshProgressSkin();
	
	MsgData *pMD = (MsgData*)lp;

	if(pMD == NULL)
		return 0;

	HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	if(pMD->type == 1)
	{
		CCoolMessageBox dlg(pMD->info.c_str(), L"财金汇", pMD->wtype);
		pMD->res = dlg.DoModal();
	}
	else
	{
		USES_CONVERSION;
		wstring waid = pMD->info.substr(0, 4);
		wstring msg = pMD->info.substr(4);
		string aid( W2A(waid.c_str()) );

		CSaveFavProgressDlg dlg(aid, msg);
		int res = dlg.DoModal();
	}

	ThreadCacheDC::DestroyThreadCacheDC();
	return 1;
}

CShowInfoDlg* g_hWndInfoDlg = NULL;
DWORD WINAPI  _threadShowInfoDLG(LPVOID lp)
{
	if(g_hWndInfoDlg != NULL)
		return 0;
	ThreadCacheDC::CreateThreadCacheDC();

	LPBILLSHOWINFO pInfo = (LPBILLSHOWINFO)lp;
	LPCTSTR lpText = pInfo->info.c_str();

	USES_CONVERSION;
	CShowInfoDlg tip(pInfo->pNotifyInterface, lpText); // 默认时显示 "正在导入账单，请稍候......"
	g_hWndInfoDlg = &tip;
	tip.SetType(pInfo->type);
	if(pInfo->type == 0)
	{
		HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
		tip.DoModal(hMainFrame);
	}
	else
	{
		tip.DoModal();//正在获取数据的记录
	}
	delete pInfo;

	g_hWndInfoDlg = NULL;
	ThreadCacheDC::DestroyThreadCacheDC();
	return 1;
}

CWindow* g_hJSWndInfoDlg = NULL;
DWORD WINAPI  _threadShowJSInfoDLG(LPVOID lp)
{
	int i = (int)lp;

	if(g_hJSWndInfoDlg != NULL)
		return 0;
	ThreadCacheDC::CreateThreadCacheDC();

	if(i == 0)
	{
		LPCTSTR lpText = L"正在处理数据......";
		CShowInfoDlg tip(NULL, lpText); 
		g_hJSWndInfoDlg = &tip;
		tip.DoModal();
	}
	else
	{
		CWaitPicDlg tip; 
		g_hJSWndInfoDlg = &tip;
		tip.DoModal();
	}

	g_hJSWndInfoDlg = NULL;

	ThreadCacheDC::DestroyThreadCacheDC();
	return 1;
}

void WINAPI  _endShowInfoDLGthread()
{
	if (NULL != g_hWndInfoDlg)
	{
		::SendMessage(g_hWndInfoDlg->m_hWnd, WM_FINISH_GET_BILL, 0, 0);
		g_hWndInfoDlg = NULL;
	}
}

void WINAPI _endShowJSInfoDLG()
{
	if (NULL != g_hJSWndInfoDlg)
	{
		::SendMessage(g_hJSWndInfoDlg->m_hWnd, WM_CLOSE, 0, 0);
		g_hJSWndInfoDlg = NULL;
	}

}

int WINAPI mhMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	CCoolMessageBox dlg(lpText, lpCaption, uType);
	return dlg.DoModal(hWnd);
}

int WINAPI mhShowMessage(HWND hWnd, LPCTSTR lpText)
{
	DWORD dw;
	WCHAR *pmsg = new WCHAR[256];
	wcscpy_s(pmsg, 256, lpText);
	CloseHandle(CreateThread(NULL, 0, _threadShowMessage, (LPVOID)pmsg, 0, &dw));
	return 0;
}

DWORD WINAPI _threadShowMessage(LPVOID lp)
{
	wstring str( (WCHAR*)lp );
	HWND hWnd = NULL;
	CShowMessageDlg *pdlg = new CShowMessageDlg(str.c_str());
	HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
	if(hMainFrame)
	{
		hWnd = hMainFrame;
	}
	pdlg->Create(hWnd);
	pdlg->ShowWindow(SW_SHOW);

	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return 0;
}
