#include <stdafx.h>
#include "SMBiosStructs.h"
#include <comdef.h>
#include <Wbemidl.h>
# pragma comment(lib, "wbemuuid.lib")

SMBiosData::SMBiosData()
{
	m_byMajorVersion = 0;
	m_byMinorVersion = 0;
	m_pbBIOSData = m_pbBIOSDataBuffer = NULL;
	m_dwLen = 0;
}

SMBiosData::~SMBiosData()
{
	if(m_pbBIOSDataBuffer)
		delete []m_pbBIOSDataBuffer;
}

BOOL SMBiosData::GetData(SMBios_TYPE0& stSMBiosType0)
{
	DWORD dwTableSize;
	BYTE* pbData = GetNextTable(NULL,FALSE,0,dwTableSize);
	BYTE* pbSectionStart = pbData;
	
	if(pbData)
	{
		memset(&stSMBiosType0,0,sizeof(SMBios_TYPE0));

		stSMBiosType0.stHeader.bySection = *pbData++;
		stSMBiosType0.stHeader.byLength = *pbData++;
		stSMBiosType0.stHeader.wHandle = *(WORD*)pbData,pbData++,pbData++;
		
		stSMBiosType0.byVendor = *pbData++;
		stSMBiosType0.byBiosVersion = *pbData++;
		stSMBiosType0.wBIOSStartingSegment = *(WORD*)pbData,pbData++,pbData++;
		
		stSMBiosType0.byBIOSReleaseDate = *pbData++;
		stSMBiosType0.byBIOSROMSize = *pbData++;
		for(int j = 0 ; j < 8 ; j++)
			stSMBiosType0.qwBIOSCharacteristics[j] = *pbData++;
		stSMBiosType0.byExtensionByte1 = *pbData++;
		stSMBiosType0.byExtensionByte2 = *pbData++;
		stSMBiosType0.bySystemBIOSMajorRelease = *pbData++;
		stSMBiosType0.bySystemBIOSMinorRelease = *pbData++;
		stSMBiosType0.byEmbeddedFirmwareMajorRelease = *pbData++;
		stSMBiosType0.byEmbeddedFirmwareMinorRelease = *pbData++;

		pbSectionStart += stSMBiosType0.stHeader.byLength;
		
		stSMBiosType0.szVendor = GetString(pbSectionStart,stSMBiosType0.byVendor);
		stSMBiosType0.szBIOSVersion = GetString(pbSectionStart,stSMBiosType0.byBiosVersion);
		stSMBiosType0.szBIOSReleaseDate = GetString(pbSectionStart,stSMBiosType0.byBIOSReleaseDate);
	}
	return (pbData != NULL);
}

BOOL SMBiosData::GetData(SMBios_TYPE1& stSMBiosType1)
{
	DWORD dwTableSize;
	BYTE* pbData = GetNextTable(NULL,FALSE,1,dwTableSize);
	BYTE* pbSectionStart = pbData;
	
	if(pbData)
	{
		memset(&stSMBiosType1,0,sizeof(SMBios_TYPE1));

		if(m_byMajorVersion >=2 && m_byMinorVersion > 0)
		{
			stSMBiosType1.stHeader.bySection = *pbData++;
			stSMBiosType1.stHeader.byLength = *pbData++;
			stSMBiosType1.stHeader.wHandle = *(WORD*)pbData,pbData++,pbData++;
		
			stSMBiosType1.byManufacturer = *pbData++;
			stSMBiosType1.byProductName = *pbData++;
			stSMBiosType1.byVersion = *pbData++;
			stSMBiosType1.bySerialNumber = *pbData++;
		}

		if(m_byMajorVersion >=2 && m_byMinorVersion > 1)
		{
			for(int j = 0 ; j < 16 ; j++)
				stSMBiosType1.byUUID[j] = *pbData++;
			stSMBiosType1.byWakeupType = *pbData++;
		}

		if(m_byMajorVersion >=2 && m_byMinorVersion > 4)
		{
			stSMBiosType1.bySKUNumber = *pbData++;
			stSMBiosType1.byFamily = *pbData++;
		}

		pbSectionStart += stSMBiosType1.stHeader.byLength;
		
		if(stSMBiosType1.byManufacturer)
			stSMBiosType1.szManufacturer = GetString(pbSectionStart,stSMBiosType1.byManufacturer);
		if(stSMBiosType1.byProductName)
			stSMBiosType1.szProductName = GetString(pbSectionStart,stSMBiosType1.byProductName);
		if(stSMBiosType1.byVersion)
			stSMBiosType1.szVersion = GetString(pbSectionStart,stSMBiosType1.byVersion);
		if(stSMBiosType1.bySerialNumber)
			stSMBiosType1.szSerialNumber = GetString(pbSectionStart,stSMBiosType1.bySerialNumber);
		if(stSMBiosType1.bySKUNumber)
			stSMBiosType1.szSKUNumber = GetString(pbSectionStart,stSMBiosType1.bySKUNumber);
		if(stSMBiosType1.byFamily)
			stSMBiosType1.szFamily = GetString(pbSectionStart,stSMBiosType1.byFamily);
	}
	return (pbData != NULL);
}

BOOL SMBiosData::GetData(SMBios_TYPE2& stSMBiosType2)
{
	DWORD dwTableSize;
	BYTE* pbData = GetNextTable(NULL,FALSE,2,dwTableSize);
	BYTE* pbSectionStart = pbData;
	
	if(pbData)
	{
		memset(&stSMBiosType2,0,sizeof(SMBios_TYPE2));

		stSMBiosType2.stHeader.bySection = *pbData++;
		stSMBiosType2.stHeader.byLength = *pbData++;
		stSMBiosType2.stHeader.wHandle = *(WORD*)pbData,pbData++,pbData++;
		
		stSMBiosType2.byManufacturer = *pbData++;
		stSMBiosType2.byProductName = *pbData++;
		stSMBiosType2.byVersion = *pbData++;
		stSMBiosType2.bySerialNumber = *pbData++;
		stSMBiosType2.byAssetTag = *pbData++;
		stSMBiosType2.byFeatureFlags = *pbData++;
		stSMBiosType2.byLocationInChassis = *pbData++;
		stSMBiosType2.wChassisHandle = *(WORD*)pbData,pbData++,pbData++;
		stSMBiosType2.byBoardType = *pbData++;
		stSMBiosType2.byNoOfContainedObjectHandles = *pbData++;

		for(int j = 0 ; j < stSMBiosType2.byNoOfContainedObjectHandles ; j++)
			stSMBiosType2.ContainedObjectHandles[j] = (WORD*)pbData,pbData++,pbData++;

		pbSectionStart += stSMBiosType2.stHeader.byLength;
		
		if(stSMBiosType2.byManufacturer)
			stSMBiosType2.szManufacturer = GetString(pbSectionStart,stSMBiosType2.byManufacturer);
		if(stSMBiosType2.byProductName)
			stSMBiosType2.szProductName = GetString(pbSectionStart,stSMBiosType2.byProductName);
		if(stSMBiosType2.byVersion)
			stSMBiosType2.szVersion = GetString(pbSectionStart,stSMBiosType2.byVersion);
		if(stSMBiosType2.bySerialNumber)
			stSMBiosType2.szSerialNumber = GetString(pbSectionStart,stSMBiosType2.bySerialNumber);
		if(stSMBiosType2.byAssetTag)
			stSMBiosType2.szAssetTag = GetString(pbSectionStart,stSMBiosType2.byAssetTag);
		if(stSMBiosType2.byLocationInChassis)
			stSMBiosType2.szLocationInChassis = GetString(pbSectionStart,stSMBiosType2.byLocationInChassis);
	}
	return (pbData != NULL);
}

BOOL SMBiosData::GetData(SMBios_TYPE3& stSMBiosType3)
{
	DWORD dwTableSize;
	BYTE* pbData = GetNextTable(NULL,FALSE,3,dwTableSize);
	BYTE* pbSectionStart = pbData;
	
	if(pbData)
	{
		memset(&stSMBiosType3,0,sizeof(SMBios_TYPE3));

		if(m_byMajorVersion >=2 && m_byMinorVersion > 0)
		{
			stSMBiosType3.stHeader.bySection = *pbData++;
			stSMBiosType3.stHeader.byLength = *pbData++;
			stSMBiosType3.stHeader.wHandle = *(WORD*)pbData,pbData++,pbData++;
			
			stSMBiosType3.byManufacturer = *pbData++;
			stSMBiosType3.byType = *pbData++;
			stSMBiosType3.byVersion = *pbData++;
			stSMBiosType3.bySerialNumber = *pbData++;
			stSMBiosType3.byAssetTag = *pbData++;
		}

		if(m_byMajorVersion >=2 && m_byMinorVersion > 1)
		{
			stSMBiosType3.byBootupState = *pbData++;
			stSMBiosType3.byPowerSupplyState = *pbData++;
			stSMBiosType3.byThermalState = *pbData++;
			stSMBiosType3.bySecurityStatus = *pbData++;
		}

		if(m_byMajorVersion >=2 && m_byMinorVersion > 3)
		{
			stSMBiosType3.dwOEMdefined = *(DWORD*)pbData,pbData++,pbData++,pbData++,pbData++;
			stSMBiosType3.byHeight = *pbData++;
			stSMBiosType3.byNumberOfPowerCords = *pbData++;
			stSMBiosType3.byContainedElementCount = *pbData++;
			stSMBiosType3.byContainedElementRecordLength = *pbData++;
		}

		pbSectionStart += stSMBiosType3.stHeader.byLength;
		
		if(stSMBiosType3.byManufacturer)
			stSMBiosType3.szManufacturer = GetString(pbSectionStart,stSMBiosType3.byManufacturer);
		if(stSMBiosType3.byVersion)
			stSMBiosType3.szVersion = GetString(pbSectionStart,stSMBiosType3.byVersion);
		if(stSMBiosType3.bySerialNumber)
			stSMBiosType3.szSerialNumber = GetString(pbSectionStart,stSMBiosType3.bySerialNumber);
		if(stSMBiosType3.byAssetTag)
			stSMBiosType3.szAssetTag = GetString(pbSectionStart,stSMBiosType3.byAssetTag);
	}
	return (pbData != NULL);
}

BOOL SMBiosData::GetData(SMBios_TYPE4& stSMBiosType4)
{
	DWORD dwTableSize;
	BYTE* pbData = GetNextTable(NULL,FALSE,4,dwTableSize);
	BYTE* pbSectionStart = pbData;
	
	if(pbData)
	{
		memset(&stSMBiosType4,0,sizeof(SMBios_TYPE4));

		if(m_byMajorVersion >=2 && m_byMinorVersion > 0)
		{
			stSMBiosType4.stHeader.bySection = *pbData++;
			stSMBiosType4.stHeader.byLength = *pbData++;
			stSMBiosType4.stHeader.wHandle = *(WORD*)pbData,pbData++,pbData++;
			
			stSMBiosType4.bySocketDesignation = *pbData++;
			stSMBiosType4.byProcessorType = *pbData++;
			stSMBiosType4.byProcessorFamily = *pbData++;
			stSMBiosType4.byProcessorManufacturer = *pbData++;
			for(int j = 0 ; j < 8; j++)
				stSMBiosType4.qwProcessorID[j] = *pbData++;
			stSMBiosType4.byProcessorVersion = *pbData++;
			stSMBiosType4.byVoltage = *pbData++;

			stSMBiosType4.wExternalClock = *pbData,pbData++,pbData++;
			stSMBiosType4.wMaxSpeed = *pbData,pbData++,pbData++;
			stSMBiosType4.wCurrentSpeed = *pbData,pbData++,pbData++;

			stSMBiosType4.byStatus = *pbData++;
			stSMBiosType4.byProcessorUpgrade = *pbData++;
		}

		if(m_byMajorVersion >=2 && m_byMinorVersion > 1)
		{
			stSMBiosType4.wL1CacheHandle = *(WORD*)pbData,pbData++,pbData++;
			stSMBiosType4.wL2CacheHandle = *(WORD*)pbData,pbData++,pbData++;
			stSMBiosType4.wL3CacheHandle = *(WORD*)pbData,pbData++,pbData++;
		}

		if(m_byMajorVersion >=2 && m_byMinorVersion > 3)
		{
			stSMBiosType4.bySerialNumber = *pbData++;
			stSMBiosType4.byAssetTagNumber = *pbData++;
			stSMBiosType4.byPartNumber = *pbData++;
		}

		pbSectionStart += stSMBiosType4.stHeader.byLength;
		
		if(stSMBiosType4.bySocketDesignation)
			stSMBiosType4.szSocketDesignation = GetString(pbSectionStart,stSMBiosType4.bySocketDesignation);
		if(stSMBiosType4.byProcessorManufacturer)
			stSMBiosType4.szProcessorManufacturer = GetString(pbSectionStart,stSMBiosType4.byProcessorManufacturer);
		if(stSMBiosType4.bySerialNumber)
			stSMBiosType4.szSerialNumber = GetString(pbSectionStart,stSMBiosType4.bySerialNumber);
		if(stSMBiosType4.byAssetTagNumber)
			stSMBiosType4.szAssetTagNumber = GetString(pbSectionStart,stSMBiosType4.byAssetTagNumber);
		if(stSMBiosType4.byPartNumber)
			stSMBiosType4.szPartNumber = GetString(pbSectionStart,stSMBiosType4.byPartNumber);
	}
	return (pbData != NULL);
}

BOOL SMBiosData::GetData(SMBios_TYPE11& stSMBiosType11)
{
	DWORD dwTableSize;
	BYTE* pbData = GetNextTable(NULL,FALSE,11,dwTableSize);
	BYTE* pbSectionStart = pbData;
	
	if(pbData)
	{
		memset(&stSMBiosType11,0,sizeof(SMBios_TYPE11));

		stSMBiosType11.stHeader.bySection = *pbData++;
		stSMBiosType11.stHeader.byLength = *pbData++;
		stSMBiosType11.stHeader.wHandle = *(WORD*)pbData,pbData++,pbData++;
		
		stSMBiosType11.byCountStrings = *pbData++;
		
		pbSectionStart += stSMBiosType11.stHeader.byLength;
		
		for(int j = 0 ; j < stSMBiosType11.byCountStrings ; j++)
			stSMBiosType11.szStrings[j] = GetString(pbSectionStart,j + 1);
	}
	return (pbData != NULL);
}

BYTE* SMBiosData::GetNextTable(BYTE* const pbData,BOOL bIgnoreTableType,BYTE byTableType,DWORD& dwTotalTableSize)
{
	BYTE byCurrentSection = 0;
	BYTE byLengthOfFormattedSection = 0;
	BYTE*	pbNextTable = NULL;
	BYTE*	pbCurrentTable = NULL;
	
	if(pbData)
		pbCurrentTable = pbData;
	else
		pbCurrentTable = m_pbBIOSData;
	int j = pbCurrentTable - m_pbBIOSData;

	for (; j < (int)m_dwLen;)
	{
		byCurrentSection = pbCurrentTable[0];

		if((FALSE == bIgnoreTableType && byCurrentSection == byTableType && pbCurrentTable != pbData) ||
		   (TRUE == bIgnoreTableType && pbCurrentTable != pbData))
		{
			byLengthOfFormattedSection = pbCurrentTable[1];

			BYTE* pUnformattedSection = pbCurrentTable + byLengthOfFormattedSection;

			for(int m = 0 ; ; m++)
			{
				if(pUnformattedSection[m] == 0 && pUnformattedSection[m+1] == 0)
				{
					dwTotalTableSize = byLengthOfFormattedSection + m + 1;
					break;
				}
			}
			pbNextTable = pbCurrentTable;
			break;
		}

		byLengthOfFormattedSection = pbCurrentTable[1];

		BYTE* pUnformattedSection = pbCurrentTable + byLengthOfFormattedSection;

		for(int m = 0 ; ; m++)
		{
			if(pUnformattedSection[m] == 0 && pUnformattedSection[m+1] == 0)
			{
				j = j + byLengthOfFormattedSection + m + 2;
				dwTotalTableSize = byLengthOfFormattedSection + m + 1;
				pbCurrentTable = pbCurrentTable + byLengthOfFormattedSection + m + 2;
				break;
			}
		}

	}
	return pbNextTable;
}

BYTE* SMBiosData::GetTableByIndex(BYTE byIndex,DWORD& dwTotalTableSize)
{
	BYTE	byLengthOfFormattedSection = 0;
	BYTE*	pbNextTable = NULL;
	BYTE*	pbCurrentTable = NULL;
	
	pbCurrentTable = m_pbBIOSData;

	for(int j = 0, nIndex = 0 ; j < (int)m_dwLen; )
	{
		byLengthOfFormattedSection = pbCurrentTable[1];

		BYTE* pUnformattedSection = pbCurrentTable + byLengthOfFormattedSection;

		int m;
		for(m = 0 ; ; m++)
		{
			dwTotalTableSize = 0;
			if(pUnformattedSection[m] == 0 && pUnformattedSection[m+1] == 0)
			{
				j = j + byLengthOfFormattedSection + m + 2;
				dwTotalTableSize = byLengthOfFormattedSection + m + 1;
				break;
			}
		}
		
		if(nIndex == byIndex)
		{
			pbNextTable = pbCurrentTable;
			break;
		}
		else
		{
			pbCurrentTable = pbCurrentTable + byLengthOfFormattedSection + m + 2;
			nIndex++;
		}

	}
	return pbNextTable;
}

char* SMBiosData::GetString(BYTE* pbData,BYTE byIndex)
{
	char* pStr = NULL;
	
	BYTE j = 0;
	do
	{
		pStr = (char*)pbData;
		pbData += (strlen(pStr) + 1);
		j++;
	}
	while(j < byIndex);
	return pStr;
}

BOOL SMBiosData::FetchSMBiosData()
{
	if (FetchSMBiosDataFromWmi())
		return TRUE;

	if (FetchSMBiosDataFromApi())
		return TRUE;

	return FetchSMBiosDataFromRegistry();
}

BOOL SMBiosData::FetchSMBiosDataFromWmi()
{
	BOOL bRet = FALSE;
	HRESULT hres;

	hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
	if (FAILED(hres))
		return FALSE;

	IWbemLocator *pLoc = 0;

	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
		return FALSE;
	}

	IWbemServices *pSvc = 0;
	hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\WMI"), NULL, NULL, 0, NULL, 0, 0, &pSvc );                              

	if (FAILED(hres))
	{
		pLoc->Release();     
		CoUninitialize();
		return FALSE;
	}

	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();     
		CoUninitialize();
		return FALSE;
	}

	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->CreateInstanceEnum(L"MSSMBios_RawSMBiosTables", 0, NULL, &pEnumerator);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();     
		CoUninitialize();
		return FALSE;
	}
	else
	{ 
		do
		{
			IWbemClassObject* pInstance = NULL;
			ULONG dwCount = NULL;

			hres = pEnumerator->Next(WBEM_INFINITE, 1, &pInstance, &dwCount);      
			if (SUCCEEDED(hres))
			{
				VARIANT varBIOSData;
				VariantInit(&varBIOSData);
				CIMTYPE type;

				hres = pInstance->Get(bstr_t("SmbiosMajorVersion"),0,&varBIOSData,&type,NULL);
				if (FAILED(hres))
				{
					VariantClear(&varBIOSData);
				}
				else
				{
					m_byMajorVersion = (BYTE)varBIOSData.iVal;
					VariantInit(&varBIOSData);
					hres = pInstance->Get(bstr_t("SmbiosMinorVersion"),0,&varBIOSData,&type,NULL);
					if (FAILED(hres))
					{
						VariantClear(&varBIOSData);
					}
					else
					{
						m_byMinorVersion = (BYTE)varBIOSData.iVal;
						VariantInit(&varBIOSData);
						hres = pInstance->Get(bstr_t("SMBiosData"),0,&varBIOSData,&type,NULL);
						if (SUCCEEDED(hres))
						{
							if ((VT_UI1 | VT_ARRAY) != varBIOSData.vt)
							{
							}
							else
							{
								SAFEARRAY *parray = NULL;
								parray = V_ARRAY(&varBIOSData);
								BYTE* pbData = (BYTE*)parray->pvData;

								m_dwLen = parray->rgsabound[0].cElements;
								m_pbBIOSData = m_pbBIOSDataBuffer = new BYTE[m_dwLen];
								memcpy(m_pbBIOSData, pbData, m_dwLen);

								bRet = TRUE;
							}
						}

						VariantClear(&varBIOSData);
					}
				}
				break;
			}

		} while (hres == WBEM_S_NO_ERROR);
	}

	pSvc->Release();
	pLoc->Release();     
	CoUninitialize();

	return bRet;
}

BOOL SMBiosData::FetchSMBiosDataFromApi()
{
	typedef UINT (WINAPI *_GetSystemFirmwareTable)(DWORD FirmwareTableProviderSignature, DWORD FirmwareTableID, PVOID pFirmwareTableBuffer, DWORD BufferSize);
	HMODULE hDll = LoadLibraryA("kernel32.dll");
	if (hDll == NULL)
		return FALSE;

	_GetSystemFirmwareTable fnGetSystemFirmwareTable = (_GetSystemFirmwareTable)GetProcAddress(hDll, "GetSystemFirmwareTable");
	if (fnGetSystemFirmwareTable == NULL)
		return FALSE;

	DWORD dwSize = fnGetSystemFirmwareTable('RSMB', 0, NULL, 0);
	if (dwSize < sizeof(RawSMBIOSData))
		return FALSE;

	m_pbBIOSDataBuffer = new BYTE[dwSize];
	fnGetSystemFirmwareTable('RSMB', 0, m_pbBIOSDataBuffer, dwSize);
	
	RawSMBIOSData* pRawData = (RawSMBIOSData *)m_pbBIOSDataBuffer;
	m_byMajorVersion = pRawData->SMBIOSMajorVersion;
	m_byMinorVersion = pRawData->SMBIOSMinorVersion;

	m_dwLen = dwSize - 8;
	m_pbBIOSData = m_pbBIOSDataBuffer + 8;

	return TRUE;
}

BOOL SMBiosData::FetchSMBiosDataFromRegistry()
{
	HKEY hkData;
	LPSTR  lpString = NULL;
	LPBYTE lpData = NULL;
	DWORD  dwType = 0, dwSize = 0;
	LONG lErr = ERROR_SUCCESS;

	if ((lErr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\mssmbios\\Data"),
		0, KEY_QUERY_VALUE, &hkData)) != ERROR_SUCCESS)
		return FALSE;

	if ((lErr = RegQueryValueEx(hkData, TEXT("SMBiosData"), NULL, &dwType, NULL, &dwSize)) == ERROR_SUCCESS)
	{
		if (dwSize == 0 || dwType != REG_BINARY)
			lErr = ERROR_BADKEY;
		else
		{
			m_pbBIOSDataBuffer = new BYTE[dwSize];
			lErr = RegQueryValueEx(hkData, TEXT("SMBiosData"), NULL, NULL, m_pbBIOSDataBuffer, &dwSize);
		}
	}

	RegCloseKey(hkData);

	if (lErr != ERROR_SUCCESS)
	{
		if (m_pbBIOSDataBuffer)
			delete m_pbBIOSDataBuffer;
		m_pbBIOSDataBuffer = NULL;

		return FALSE;
	}

	RawSMBIOSData* pRawData = (RawSMBIOSData *)m_pbBIOSDataBuffer;
	m_byMajorVersion = pRawData->SMBIOSMajorVersion;
	m_byMinorVersion = pRawData->SMBIOSMinorVersion;

	m_dwLen = dwSize - 8;
	m_pbBIOSData = m_pbBIOSDataBuffer + 8;

	return TRUE;
}
