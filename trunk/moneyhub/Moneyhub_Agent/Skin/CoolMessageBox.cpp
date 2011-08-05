
#include "stdafx.h"
#include "CoolMessageBox.h"

int WINAPI mhMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	CCoolMessageBox dlg(lpText, lpCaption, uType);
	return dlg.DoModal(hWnd);
}