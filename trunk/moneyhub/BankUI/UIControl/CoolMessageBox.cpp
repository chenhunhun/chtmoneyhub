
#include "stdafx.h"
#include "CoolMessageBox.h"
#include "SaveFavProgress.h"
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
		CCoolMessageBox dlg(pMD->info.c_str(), L"²Æ½ð»ã", pMD->wtype);
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
