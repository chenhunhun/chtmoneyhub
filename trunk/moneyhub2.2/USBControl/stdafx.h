// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define PASSIVE_LEVEL 0
#include "windows.h"
#include "targetver.h"
#ifndef WINVER
	#define WINVER _WIN32_WINNT_WIN7 //假设我们的软件只是运行在这些高级版本上，用于编译
#endif
//
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

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

#include <string>
#include <list>
#include <map>
#include <set>
#include "MyError.h"
#include "../Utils/RecordProgram/RecordProgram.h"
// Bug调试信息
//#ifndef OFFICIAL_VERSION
	#ifndef MY_OWN_LOG_INCLUDE_DEFINE_tag
	#define MY_OWN_LOG_INCLUDE_DEFINE_tag
	#include "..\Utils\RunLog\RunLog.h"
	#endif
//#endif
// TODO: reference additional headers your program requires here
