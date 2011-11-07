#include "libmac.h"
#include <windows.h>
#include <nb30.h>
typedef UCHAR  (WINAPI *pNetbios)(PNCB pncb);

static pNetbios m_NetBios = NULL;
static HINSTANCE m_hInst = NULL;

bool InitNETBIOSFunctions()
{
	if (m_hInst == NULL) 
	{
		m_hInst = LoadLibraryA("netapi32.dll");
		if (m_hInst < (HINSTANCE) HINSTANCE_ERROR) 
		{
			m_hInst = NULL;
			return false;
		}
	}
	if (m_NetBios == NULL) 
	{
		m_NetBios = (pNetbios) GetProcAddress(m_hInst, "Netbios");
		if (m_NetBios == NULL)
			return false;
	}
	return true;
}

bool GetAdapterInfo(int nAdapterNum, MACSTRUCT &mac)
{
    // Reset the LAN adapter so that we can begin querying it 
    NCB Ncb;
	ZeroMemory(&Ncb, sizeof(NCB));
    Ncb.ncb_command = NCBRESET;
    Ncb.ncb_lana_num = (BYTE)nAdapterNum;
    if (m_NetBios(&Ncb) != NRC_GOODRET) 
        return false;
    // Prepare to get the adapter status block 
    ZeroMemory(&Ncb, sizeof(NCB));
    Ncb.ncb_command = NCBASTAT;
    Ncb.ncb_lana_num = (BYTE)nAdapterNum;
	char *p = (char *) Ncb.ncb_callname;
	*p++ = '*';
	*p = '\0';
    struct ASTAT {
        ADAPTER_STATUS adapt;
        NAME_BUFFER NameBuff[30];
    } Adapter;
	ZeroMemory(&Adapter,  sizeof(Adapter));
    Ncb.ncb_buffer = (unsigned char *)&Adapter;
    Ncb.ncb_length = sizeof(Adapter);
    
    // Get the adapter's info and, if this works, return it in standard,
    // colon-delimited form.
    if (m_NetBios(&Ncb) == NRC_GOODRET) {
		MoveMemory(&mac,&Adapter.adapt.adapter_address,6);
        return true;
    }
    else return false;
}


GETMACRESULT GetAdapterMACviaNETBIOS(int &num, MACSTRUCT *& maclist)
{
	num = 0;
	maclist = (MACSTRUCT *)HeapAlloc(GetProcessHeap(),0,num * sizeof(MACSTRUCT));
	GETMACRESULT ret = MAC_UNKNOWN;
	if (!InitNETBIOSFunctions())
		return ret;
    LANA_ENUM AdapterList;
    NCB Ncb;
	ZeroMemory(&Ncb,  sizeof(NCB));
    Ncb.ncb_command = NCBENUM;
    Ncb.ncb_buffer = (unsigned char *)&AdapterList;
    Ncb.ncb_length = sizeof(AdapterList);
    if (NRC_GOODRET != m_NetBios(&Ncb))
		return ret;
	
	for (int i = 0; i < AdapterList.length; ++i)
	{
		MACSTRUCT mac;
		if (GetAdapterInfo(AdapterList.lana[i],mac)) 
		{
			MACSTRUCT *temp = (MACSTRUCT *)HeapReAlloc(GetProcessHeap(),0,maclist,(num +1) * sizeof (MACSTRUCT));
			if (temp) 
			{
				maclist = temp;
				MoveMemory((void *)(maclist + num),&mac,6);
				num ++;
			} 
			else 
			{
				// realloc ¾ÓÈ»Ê§°ÜÁË! ¸Ï½ô·µ»Ø
				break;
			}
		}
	}
	if (num > 0) 
		ret = MAC_OK;
	else 
		ret = MAC_NOCARD;
	return ret;
}