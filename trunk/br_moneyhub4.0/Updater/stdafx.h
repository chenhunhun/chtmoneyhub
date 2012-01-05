// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200


// windows headers
#include <windows.h>
#include <Uxtheme.h>
#include <commctrl.h>
#include <comutil.h>

// atl headers
#include <atlbase.h>
#include <atlstr.h>
#include <atlimage.h>
#include <atlapp.h>
#include <atlwinex.h>
#include <atlwinx.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlcoll.h>
#include <atlcrack.h>
#include <atlddx.h>
#include <atlgdix.h>
#include <atlcoll.h>
#include <atlgdi.h>
#include <atlwinex.h>
#include <atluser.h>


#include <tuodwmapi.h>

#include "UpdaterApp.h"

//extern CAppModule _Module;
extern CUpdaterApp _Module;

//#include <atlwin.h>
#include "..//Utils/RecordProgram/RecordProgram.h"
#include "myError.h"



#include <string>
#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

extern tstring _UpdateUrl;
extern tstring _TempCachePath;


extern const UINT WM_TASKBARCREATED;
extern const UINT WM_DOWNLOADOK;
extern const UINT WM_DOWNLOADSTOP;
extern const UINT WM_TRAYICON;
extern const UINT WM_BROADCAST_QUIT;
extern const UINT WM_UPDATERETRY;
extern const UINT WM_SETPROGRESSPOS;

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif