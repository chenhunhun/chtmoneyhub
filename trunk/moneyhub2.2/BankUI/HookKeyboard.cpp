#include "StdAfx.h"
#include "HookKeyboard.h"
#include "..//Utils/Config//HostConfig.h"


#define NUMBEGIN     0x30
#define KEYBEGIN     0x41
#define _WIN32_WINNT 0x0501

/*
*	广大银行、浦发银行、和交通银行，对底层钩子有检测，因此在当前打开页面是广大银行的时候应该卸掉钩子(仅限于XP)，以保证密码输入框的正常使用
*/
std::vector<std::wstring>   g_vecNPBName;//不采取保护的银行
bool                        g_bNPB = false;//只采集一次

wchar_t  g_noHookfilterUrl[3000] = {0};

HHOOK g_hLLKeyboardHook = NULL;
HHOOK g_hLLJournalHook = NULL;

enum OSVERSION
{
	MH_WIN2K = 1,
	MH_WINXP,
	MH_WINVISTA,
	MH_WIN7,
	MH_NONE
};


OSVERSION getOsVersion()
{
	OSVERSIONINFOW ovi = {sizeof(OSVERSIONINFOW)};
	GetVersionEx(&ovi);

	switch(ovi.dwMajorVersion)
	{
		case 5:
			if( 1 == ovi.dwMinorVersion)
				return MH_WINXP;
			else if( 0 == ovi.dwMinorVersion)
			    return MH_WIN2K;
			break;

		case 6:
			if( 0 == ovi.dwMinorVersion)
				return MH_WINVISTA;
			else if( 1 == ovi.dwMinorVersion)
				return MH_WIN7;
			break;

		default:
			break;
	}

	return MH_NONE;
}


bool isQQ()
{
	GUITHREADINFO gti = {sizeof(GUITHREADINFO) };

	if( GetGUIThreadInfo(NULL,&gti) )
	{
		wchar_t wcsName[500];
		GetWindowTextW(gti.hwndActive,wcsName,_countof(wcsName) );
		_wcslwr(wcsName);

		if(wcsstr(wcsName,L"qq"))
		{
			return true;
		}
	}
	return false;
}


bool  isWinxp()
{
	return MH_WINXP == getOsVersion()? true:false;
}

/**
*  return false;   hook
*  reutrn true ;   unHook
*/
bool checkCompatible()
{
	if( isQQ() )
		return true;

// 	if( isWinxp() )
// 		return true;

	for(std::vector<std::wstring>::size_type i=0; i < g_vecNPBName.size() ; i++)
	{
		if( wcsstr(g_noHookfilterUrl, g_vecNPBName[i].c_str()))
		{
			return true;
		}
	}

	return false;
}


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
//	CHAR szDebug[256];

	if (nCode == HC_ACTION)
	{
		PKBDLLHOOKSTRUCT pKeyboardHookStruct = (PKBDLLHOOKSTRUCT) lParam;

		BYTE KeyboardState[256];
		ZeroMemory(KeyboardState, sizeof(KeyboardState));
		GetKeyboardState(KeyboardState);

		KeyboardState[VK_SHIFT] = (BYTE) (GetKeyState(VK_LSHIFT) | GetKeyState(VK_RSHIFT));

		KeyboardState[VK_CAPITAL] = (BYTE) GetKeyState(VK_CAPITAL);

		WORD wChar;

		int iNumChar = ToAscii(pKeyboardHookStruct->vkCode, pKeyboardHookStruct->scanCode, KeyboardState, &wChar, 0);

		GUITHREADINFO gti = {sizeof(GUITHREADINFO) }; 

		if (wParam == WM_KEYDOWN)
		{
			if(0 == GetGUIThreadInfo(NULL,&gti))
			{	
				UINT lparm =0;
				lparm |= 0;
				lparm |= MapVirtualKey(pKeyboardHookStruct->vkCode, 0)<<15;
				lparm |= 1<<29;
				SendMessage(gti.hwndFocus, WM_KEYDOWN, (WPARAM)pKeyboardHookStruct->vkCode, (LPARAM)lParam);
			}
		}

		if (wParam == WM_KEYUP)
		{
			if(0 == GetGUIThreadInfo(NULL,&gti))
			{
				UINT lparm =0;
				lparm |= 0;
				lparm |= MapVirtualKey(pKeyboardHookStruct->vkCode, 0)<<15;
				lparm |= 1<<30;
				SendMessage(gti.hwndFocus, WM_KEYUP, (WPARAM)pKeyboardHookStruct->vkCode, (LPARAM)lParam);
			}
		}

		return NULL;
	}
	else
		return CallNextHookEx(g_hLLKeyboardHook,nCode,wParam,lParam);	
}

LRESULT CALLBACK JournalRecordProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(code < 0)
		return CallNextHookEx(g_hLLJournalHook, code, wParam, lParam);	
	else
		return NULL;
}

bool HOOKKEY::installHook()
{
//	if(g_hLLJournalHook == NULL)
//		;//g_hLLJournalHook = SetWindowsHookEx(WH_JOURNALRECORD,(HOOKPROC)JournalRecordProc, GetModuleHandle(NULL), 0);
	
	if( !checkCompatible() )
	{
		g_hLLKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

		return g_hLLKeyboardHook == NULL ? false:true ;
	}

	return false;
}




#define HOTKEYNUM  6
DWORD g_Hid[HOTKEYNUM] ;

bool  HOOKKEY::disablePrintKey()
{
	bool bR = true;
	
	wchar_t wcsAtomName[255] = L"Amoneyhubatom";
	for(int i=0; i<HOTKEYNUM; i++)
	{
		int * a =  (int *)wcsAtomName;
		*a += i;
		g_Hid[i] = GlobalAddAtomW(wcsAtomName);

		if(g_Hid[i] == 0)  bR=false;
	}

	if(0 == ::RegisterHotKey(NULL,g_Hid[0],MOD_ALT,VK_SNAPSHOT) )
		bR=false;

	if(0 == ::RegisterHotKey(NULL,g_Hid[1],MOD_CONTROL,VK_SNAPSHOT) )
		bR=false;

// 	if(0 == ::RegisterHotKey(NULL,g_Hid[2],MOD_KEYUP,VK_SNAPSHOT) )
// 		bR=false;

	if(0 == ::RegisterHotKey(NULL,g_Hid[3],MOD_SHIFT,VK_SNAPSHOT) )
		bR=false;

	if(0 == ::RegisterHotKey(NULL,g_Hid[4],MOD_WIN,VK_SNAPSHOT) )
		bR=false;

	if(0 == ::RegisterHotKey(NULL,g_Hid[5],0,VK_SNAPSHOT) )
		bR=false;

	return bR == true? true:false;
}


void HOOKKEY::addNPB(/*std::wstring wcsName*/)
{
	if( !g_bNPB)
	{
		g_bNPB = true;
		CHostContainer::GetInstance()->Init(&g_vecNPBName);
	}
}

bool HOOKKEY::uninstallHook(bool b)
{
//	if(b && NULL != g_hLLJournalHook)
//		;//UnhookWindowsHookEx(g_hLLJournalHook);

	if(NULL != g_hLLKeyboardHook)
	{
		if( UnhookWindowsHookEx(g_hLLKeyboardHook) )
		{
			g_hLLKeyboardHook = NULL;
			return true;
		}
	}

	return false;
}


bool    HOOKKEY::EnablePrintKey()
{
	for(int i=0; i<HOTKEYNUM; i++)
	{
		UnregisterHotKey(NULL, g_Hid[i]); 
	}
	return true;

}