// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER			0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE		0x0501
#define _RICHEDIT_VER	0x0200



//#include "UpdaterApp.h"


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

#include <time.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>
#include <queue>

#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES


extern CAppModule _Module;

// 下面的status是给skin用的
enum ButtonStatus
{
	Btn_MouseOut = 0,
	Btn_MouseOver = 1,
	Btn_MouseDown = 2,
	Btn_Disabled = 3,
	Btn_Addition = 4
};

// 下面的status是给tuo toolbar用的
#define BTN_STATUS_DISABLED			0x00
#define BTN_STATUS_MOUSEOUT			0x01
#define BTN_STATUS_MOUSEOVER		0x02
#define BTN_STATUS_MOUSEDOWN		0x03
#define BTN_STATUS_MASK				0x0f

#define BTN_STATUS_CHECKED			0x10
#define BTN_STATUS_DROPDOWN			0x20
#define BTN_STATUS_SPECIAL			0x40

#define INVALID_ITEM		-1
#define NM_DROPDOWN			(NM_FIRST - 50)
#include "MyError.h"
#include "../Utils/RecordProgram/RecordProgram.h"
//#include "atltypes.h"

#include <string>

// Bug调试信息
//#ifndef OFFICIAL_VERSION
	#ifndef MY_OWN_LOG_INCLUDE_DEFINE_tag
	#define MY_OWN_LOG_INCLUDE_DEFINE_tag
	#include "..\Utils\RunLog\RunLog.h"
	#endif
//#endif

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif