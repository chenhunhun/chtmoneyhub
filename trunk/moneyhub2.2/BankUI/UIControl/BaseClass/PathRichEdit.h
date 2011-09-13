#pragma once

__declspec(selectany) HDC g_hReplacePaintDC = NULL;
__declspec(selectany) PAINTSTRUCT g_replacePaintStruct;


HDC (WINAPI * OldBeginPaint)(HWND hwnd, LPPAINTSTRUCT lpPaint) = BeginPaint;
HDC WINAPI MineBeginPaint(HWND hwnd, LPPAINTSTRUCT lpPaint)
{
	if (g_hReplacePaintDC == NULL)
		return OldBeginPaint(hwnd, lpPaint);
	*lpPaint = g_replacePaintStruct;
	lpPaint->hdc = g_hReplacePaintDC;
	return g_hReplacePaintDC;
}


BOOL (WINAPI *OldEndPaint)(HWND hWnd, const PAINTSTRUCT *lpPaint) = EndPaint;
BOOL WINAPI MineEndPaint(HWND hWnd, const PAINTSTRUCT *lpPaint)
{
	if (g_hReplacePaintDC == NULL)
		return OldEndPaint(hWnd, lpPaint);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

#include "../../../Utils/APIHook.h"

void DoPatchRichEditHook()
{
	LONG lRes = Detours::DetourTransactionBegin();
	ATLASSERT(lRes == NO_ERROR);
	lRes = Detours::DetourUpdateThread(::GetCurrentThread());
	ATLASSERT(lRes == NO_ERROR);
	lRes = Detours::DetourAttach((PVOID*)&OldEndPaint, MineEndPaint);
	ATLASSERT(lRes == NO_ERROR);
	lRes = Detours::DetourAttach((PVOID*)&OldBeginPaint, MineBeginPaint);
	ATLASSERT(lRes == NO_ERROR);
	lRes = Detours::DetourTransactionCommit();
	ATLASSERT(lRes == NO_ERROR);
}
