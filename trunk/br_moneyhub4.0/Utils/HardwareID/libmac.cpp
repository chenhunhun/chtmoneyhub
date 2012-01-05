#include <windows.h>
#include "libmac.h"
void FreeMACResult(MACSTRUCT *result)
{
	HeapFree (GetProcessHeap(),0,result);
}

// ban VMWARE macs and DUN macs

static unsigned char VMWAREMAC[6]   = {0x00, 0x50, 0x56, 0xC0, 0, 0};
static unsigned char DUNMAC[6]      = {0x44, 0x45, 0x53, 0x54, 0, 0};
static unsigned char NULLMAC[6]     = {0, 0, 0, 0, 0, 0};
static unsigned char FFMAC[6]       = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static unsigned char TAPMAC[6]      = {0, 0xFF, 0, 0, 0, 0};
static unsigned char MINIPORTMAC[6] = {0, 0, 0, 0x52, 0x41, 0x53};

enum MACLEVEL {MAC_TRUSTABLE= 0, MAC_UNTRUSTABLE = -1, MAC_NOUSE = -2 , MAC_BAD = -3};

static MACLEVEL FilterMAC(MACSTRUCT &dst, MACSTRUCT &src, MACLEVEL oldlevel)
{
	MACLEVEL ret = MAC_BAD;
	if (memcmp(&src,VMWAREMAC,5) == 0)
		ret = MAC_NOUSE;
	else if (memcmp(&src,DUNMAC,5) == 0)
		ret = MAC_NOUSE;
	else if (memcmp(((BYTE*)&src)+3,MINIPORTMAC+3, 3) == 0)
		ret = MAC_NOUSE;
	else if (memcmp(&src,NULLMAC,6) == 0)
		ret = MAC_BAD;
	else if (memcmp(&src,FFMAC,6) == 0)
		ret = MAC_BAD;
	else if (memcmp(&src,TAPMAC,2) == 0) 
		ret = MAC_UNTRUSTABLE;
	else 
		ret = MAC_TRUSTABLE;
	if (ret > oldlevel) {
		MoveMemory(&dst,&src,6);
		return ret;
	}
	else 
		return oldlevel;
}

GETMACRESULT GetFirstMAC(MACSTRUCT &mac)
{
	int num;
	MACSTRUCT *macs;
	MACLEVEL level = MAC_BAD;
	if (GetAdapterMACviaSNMP(num,macs) == MAC_OK && num > 0) 
	{
		for (int i=0;i<num && level < MAC_TRUSTABLE;i++) 
			level = FilterMAC(mac,macs[i],level);
	} 
	FreeMACResult(macs);
	if (level == MAC_TRUSTABLE)
		return MAC_OK;

	if (GetAdapterMACviaNETBIOS(num,macs) == MAC_OK && num > 0) 
	{
		for (int i=0;i<num && level < MAC_TRUSTABLE;i++) 
			level = FilterMAC(mac,macs[i],level);
	} 
	FreeMACResult(macs);
	if (level == MAC_BAD)
		return MAC_NOCARD;
	else
		return MAC_OK;
}



















