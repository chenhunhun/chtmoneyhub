#pragma once


// #define WINVER			0x0501
// #define _WIN32_WINNT	0x0501
// #define _WIN32_IE		_WIN32_IE_IE60SP2
// #define _RICHEDIT_VER	0x0200

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0603	// Change this to the appropriate value to target other versions of IE.
#endif

#define _WIN32_DCOM 

#ifndef _COMPILE_MSG_
#define _COMPILE_MSG_
// 输出编译消息
#define STR2(x)	   #x
#define STR(x)	STR2(x)
#define CompileMessage(desc) message(__FILE__ "(" STR(__LINE__) "): " #desc)
#endif

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
//#include <atlmisc.h>

#include "Message.h"
#include "CommFunc.h"


#include "myError.h"
#include "../Utils/RecordProgram/RecordProgram.h"


// 下面的status是给tuo toolbar用的
#define BTN_STATUS_DISABLED			0x00
#define BTN_STATUS_MOUSEOUT			0x01
#define BTN_STATUS_MOUSEOVER		0x02
#define BTN_STATUS_MOUSEDOWN		0x03
#define BTN_STATUS_MASK				0x0f

#define BTN_STATUS_CHECKED			0x10
#define BTN_STATUS_DROPDOWN			0x20
#define BTN_STATUS_SPECIAL			0x40

#define NM_DROPDOWN			(NM_FIRST - 50)


#define ICON_DEFAULT_PAGE			(HICON)0
#define ICON_HOME					(HICON)-1

#define RESOURCE_TYPE_COUNT					2

#define INVALID_ITEM		-1

#define tstring basic_string<TCHAR>


#define WM_MAIN_NOTIFY_FILE_CHANGE			(WM_USER + 0x3001)


#ifndef MY_OWN_LOG_INCLUDE_DEFINE_tag
#define MY_OWN_LOG_INCLUDE_DEFINE_tag
#include "..\Utils\RunLog\RunLog.h"
#endif


//////////////////////////////////////////////////////////////////////////

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
