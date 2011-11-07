//#pragma message(__FILE__ "(1) : warning C000: #include common/common.h")    //#include "common/common.h"
#include <windows.h>
#include <stdio.h>
#include "genhwid.h"
//#pragma warning(disable: 4100)

int __stdcall WinMain( HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine,
			int nCmdShow
			)
{
	HWIDSTRUCT hwid = {0};
	GenHWID(hwid);
	//char hwidstr[255];
	//wsprintf(hwidstr, "Your hardware fingerprint is %08X%08X%08X%08X",hwid.dw1,hwid.dw2,hwid.dw3,hwid.dw4);
	//MessageBox(0,hwidstr,"Info",0);
	return 0;
}