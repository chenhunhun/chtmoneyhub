// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0502	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0603	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <Winsock2.h>
#include <Windows.h>


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

#include <UrlMon.h>
#include <WinInet.h>
#include <tlogstg.h>
#include <MsHTML.h>
#include <mshtmhst.h>
#include <mshtmdid.h>
#include <mshtmcid.h>
#include <ObjSafe.h>
#include <shlobj.h>
#include <comutil.h>
#include <ocmm.h>

#include <exdispid.h>
#include <dispex.h>
#include <comutil.h>
#include <mshtmlc.h>
#include <WinInet.h>
#include <GdiPlus.h>
#include <ShlGuid.h>
#include <wininet.h>


#include <set>
#include <queue>
#include <string>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <iomanip>
#include "MyError.h"
#include "../Utils/RecordProgram/RecordProgram.h"

#include "Message.h"
#include "CommFunc.h"

using namespace ATL;

// Bugµ÷ÊÔÐÅÏ¢
//#ifndef OFFICIAL_VERSION
	#ifndef MY_OWN_LOG_INCLUDE_DEFINE_tag
	#define MY_OWN_LOG_INCLUDE_DEFINE_tag
	#include "..\Utils\RunLog\RunLog.h"
	#endif
//#endif