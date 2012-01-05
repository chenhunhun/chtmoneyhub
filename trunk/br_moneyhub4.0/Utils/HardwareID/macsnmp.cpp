#include <snmp.h>
#include "libmac.h"

#pragma comment(lib, "ws2_32.lib")

/*
 *	UTIL functions from snmpapi.cpp
 */

SNMPAPI
SNMP_FUNC_TYPE
SnmpUtilOidCpy(
			   OUT AsnObjectIdentifier *DstObjId,
			   IN  AsnObjectIdentifier *SrcObjId
			   )
{
	DstObjId->ids = (UINT *)GlobalAlloc(GMEM_ZEROINIT,SrcObjId->idLength * 
		sizeof(UINT));
	if(!DstObjId->ids){
		SetLastError(1);
		return 0;
	}
	
	MoveMemory(DstObjId->ids,SrcObjId->ids,SrcObjId->idLength*sizeof(UINT));
	DstObjId->idLength = SrcObjId->idLength;
	
	return 1;
}


VOID
SNMP_FUNC_TYPE
SnmpUtilOidFree(
				IN OUT AsnObjectIdentifier *ObjId
				)
{
	GlobalFree(ObjId->ids);
	ObjId->ids = 0;
	ObjId->idLength = 0;
}

SNMPAPI
SNMP_FUNC_TYPE
SnmpUtilOidNCmp(
				IN AsnObjectIdentifier *ObjIdA,
				IN AsnObjectIdentifier *ObjIdB,
				IN UINT                 Len
				)
{
	UINT CmpLen;
	UINT i;
	int  res;
	
	CmpLen = Len;
	if(ObjIdA->idLength < CmpLen)
		CmpLen = ObjIdA->idLength;
	if(ObjIdB->idLength < CmpLen)
		CmpLen = ObjIdB->idLength;
	
	for(i=0;i<CmpLen;i++){
		res = ObjIdA->ids[i] - ObjIdB->ids[i];
		if(res!=0)
			return res;
	}
	return 0;
}

VOID
SNMP_FUNC_TYPE
SnmpUtilVarBindFree(
					IN OUT RFC1157VarBind *VarBind
					)
{
	BYTE asnType;
	// free object name
	SnmpUtilOidFree(&VarBind->name);
	
	asnType = VarBind->value.asnType;
	
	if(asnType==ASN_OBJECTIDENTIFIER){
		SnmpUtilOidFree(&VarBind->value.asnValue.object);
	}
	else if(
        (asnType==ASN_OCTETSTRING) ||
        (asnType==ASN_RFC1155_IPADDRESS) ||
        (asnType==ASN_RFC1155_OPAQUE) ||
        (asnType==ASN_SEQUENCE)){
		if(VarBind->value.asnValue.string.dynamic){
			GlobalFree(VarBind->value.asnValue.string.stream);
		}
	}
	
	VarBind->value.asnType = ASN_NULL;
	
}



/*
 *	UTIL functions from snmpmac.cpp
 */


typedef BOOL(WINAPI * pSnmpExtensionInit) (
        IN DWORD dwTimeZeroReference,
        OUT HANDLE * hPollForTrapEvent,
        OUT AsnObjectIdentifier * supportedView);

typedef BOOL(WINAPI * pSnmpExtensionTrap) (
        OUT AsnObjectIdentifier * enterprise,
        OUT AsnInteger * genericTrap,
        OUT AsnInteger * specificTrap,
        OUT AsnTimeticks * timeStamp,
        OUT RFC1157VarBindList * variableBindings);

typedef BOOL(WINAPI * pSnmpExtensionQuery) (
        IN BYTE requestType,
        IN OUT RFC1157VarBindList * variableBindings,
        OUT AsnInteger * errorStatus,
        OUT AsnInteger * errorIndex);

typedef BOOL(WINAPI * pSnmpExtensionInitEx) (
        OUT AsnObjectIdentifier * supportedView);


    
static HINSTANCE m_hInst = NULL;
static pSnmpExtensionInit m_Init = NULL;
static pSnmpExtensionInitEx m_InitEx = NULL;
static pSnmpExtensionQuery m_Query = NULL;
static pSnmpExtensionTrap m_Trap = NULL;

static   UINT OID_ifEntryType[] = {
        1, 3, 6, 1, 2, 1, 2, 2, 1, 3
    };
static    UINT OID_ifEntryNum[] = {
        1, 3, 6, 1, 2, 1, 2, 1
    };
static    UINT OID_ipMACEntAddr[] = {
        1, 3, 6, 1, 2, 1, 2, 2, 1, 6
    };                          //, 1 ,6 };
static    AsnObjectIdentifier MIB_ifMACEntAddr =
        { sizeof(OID_ipMACEntAddr) / sizeof(UINT), OID_ipMACEntAddr };
static    AsnObjectIdentifier MIB_ifEntryType = {
        sizeof(OID_ifEntryType) / sizeof(UINT), OID_ifEntryType
    };
static    AsnObjectIdentifier MIB_ifEntryNum = {
        sizeof(OID_ifEntryNum) / sizeof(UINT), OID_ifEntryNum
    };

static AsnObjectIdentifier MIB_NULL = {
	0, 0
};

static HANDLE PollForTrapEvent;
static AsnObjectIdentifier SupportedView;

static char NULLMAC[6] =  {0, 0, 0, 0, 0, 0};

/*
 *	初始化 SNMP 使用的函数！ true 成功， false 失败！
 */
bool InitSNMPFunctions ()
{
    WSADATA WinsockData;
    if (WSAStartup(MAKEWORD(2, 0), &WinsockData) != 0) {
        return false;
    }
	if (m_hInst == NULL) 
	{
		m_hInst = LoadLibraryA("inetmib1.dll");
		if (m_hInst < (HINSTANCE) HINSTANCE_ERROR) 
		{
			m_hInst = NULL;
			return false;
		}
	}

	
	if (m_Init == NULL) 
	{
		m_Init = (pSnmpExtensionInit) GetProcAddress(m_hInst, "SnmpExtensionInit");
		if (m_Init == NULL)
			return false;
	}
	m_Init(GetTickCount(), &PollForTrapEvent, &SupportedView);
	
	if (m_InitEx == NULL)
	{
		m_InitEx = (pSnmpExtensionInitEx) GetProcAddress(m_hInst,"SnmpExtensionInitEx");
		if (m_InitEx == NULL)
			return false;
	}
    if (m_Query == NULL)
	{
		m_Query = (pSnmpExtensionQuery) GetProcAddress(m_hInst,	"SnmpExtensionQuery");
		if (m_Query == NULL)
			return false;
	}
    if (m_Trap == NULL) 
	{
		m_Trap = (pSnmpExtensionTrap) GetProcAddress(m_hInst, "SnmpExtensionTrap");
		if (m_Trap == NULL)
			return false;
	}
	return true;
}

/*
 *	public :
 */

GETMACRESULT GetAdapterMACviaSNMP(int &num, MACSTRUCT * &maclist)
{
	num = 0;
	maclist = (MACSTRUCT *)HeapAlloc(GetProcessHeap(),0,num * sizeof(MACSTRUCT));
	GETMACRESULT ret = MAC_UNKNOWN;
    RFC1157VarBindList varBindList;
    RFC1157VarBind varBind[2];
    AsnInteger errorStatus;
    AsnInteger errorIndex;
	if (!InitSNMPFunctions())
		return MAC_UNKNOWN;
    varBindList.list = varBind;
    varBind[0].name = MIB_NULL;
    varBind[1].name = MIB_NULL;
	
    varBindList.len = 1;        /* Only retrieving one item */
    SNMP_oidcpy(&varBind[0].name, &MIB_ifEntryNum);
    if ( m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus,&errorIndex) )
	{
		int loopcount = varBind[0].value.asnValue.number;
		varBindList.len = 2;
		/* Copy in the OID of ifType, the type of interface */
		SNMP_oidcpy(&varBind[0].name, &MIB_ifEntryType);
		/* Copy in the OID of ifPhysAddress, the address */
		SNMP_oidcpy(&varBind[1].name, &MIB_ifMACEntAddr);
		
		for (int i=0; i< loopcount; i++) 
		{
			if (m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus,&errorIndex))
			{
				if (! SNMP_oidncmp(&varBind[0].name, &MIB_ifEntryType,
					MIB_ifEntryType.idLength) &&
					varBind[0].value.asnValue.number == 6 &&
					! SNMP_oidncmp(&varBind[1].name, &MIB_ifMACEntAddr,
					MIB_ifMACEntAddr.idLength) &&
					varBind[1].value.asnValue.address.stream != NULL && 
					memcmp(varBind[1].value.asnValue.address.stream , NULLMAC , 6) != 0 
					)
				{
					MACSTRUCT *temp = (MACSTRUCT *)HeapReAlloc(GetProcessHeap(),0,maclist,(num +1) * sizeof (MACSTRUCT));
 					if (temp) 
					{
						maclist = temp;
						MoveMemory((void *)(maclist + num),varBind[1].value.asnValue.address.stream,6);
						num ++;
					} 
					else 
					{
						// realloc 居然失败了! 赶紧返回
						break;
					}
				}	// 一大托 if 
			} // if (m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus,&errorIndex))
		} // for (int i=0;i<varBind[0].value.asnValue.number ; i++) 
		if (num == 0)
			ret = MAC_NOCARD;
		else 
			ret = MAC_OK;
	}
	else  // m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus,&errorIndex) 
		ret = MAC_UNKNOWN;
    SNMP_FreeVarBind(&varBind[0]);
    SNMP_FreeVarBind(&varBind[1]);
	return ret;
}















