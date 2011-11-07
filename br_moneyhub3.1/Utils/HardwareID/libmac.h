#pragma once

/*
 *	
 */

typedef unsigned char MACSTRUCT[6]; 

enum GETMACRESULT {MAC_OK = 0, MAC_NOCARD = -1, MAC_UNKNOWN = -2};

extern GETMACRESULT GetFirstMAC(MACSTRUCT & mac);

extern GETMACRESULT GetAdapterMACviaNETBIOS(int &num, MACSTRUCT *& maclist);

extern GETMACRESULT GetAdapterMACviaSNMP(int &num, MACSTRUCT *& maclist);

extern void FreeMACResult(MACSTRUCT *result);

