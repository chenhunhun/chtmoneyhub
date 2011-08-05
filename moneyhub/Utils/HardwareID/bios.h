#pragma once

#include <string>
#include "SMBiosStructs.h"

class CBIOS
{
public:
	CBIOS()
	{
		if (QueryInfo())
			return;
	}

public:
	std::string GetSystemManufacturer() const
	{
		return m_strSystemManufacturer.size() > 0 ? m_strSystemManufacturer : "Unknown...";
	}

	std::string GetProductName() const
	{
		return m_strProductName.size() > 0 ? m_strProductName : "Unknown...";
	}

	std::string GetUniversalUniqueID() const
	{
		return m_strUniversalUniqueId.size() > 0 ? m_strUniversalUniqueId : "Unknown...";
	}

	std::string GetMotherBoardModel() const
	{
		return m_strMotherboardModel.size() > 0 ? m_strMotherboardModel : "Unknown...";
	}

	std::string GetMotherBoardSerialNumber() const
	{
		return m_strMotherboardSerialNumber.size() > 0 ? m_strMotherboardSerialNumber : "Unknown...";
	}

protected:
	bool QueryInfo()
	{
		SMBiosData rawdata;
		if (!rawdata.FetchSMBiosData())
			return false;

		SMBios_TYPE1 data1;
		rawdata.GetData(data1);

		if( data1.szManufacturer )
			m_strSystemManufacturer = data1.szManufacturer;
		if( data1.szProductName )
			m_strProductName = data1.szProductName;

		char szUUID[128];
		sprintf_s(szUUID, sizeof(szUUID), "%02X%02X%02X%02X-%02X%02X%02X%02X-%02X%02X%02X%02X-%02X%02X%02X%02X",
			data1.byUUID[0], data1.byUUID[1], data1.byUUID[2], data1.byUUID[3],
			data1.byUUID[4], data1.byUUID[5], data1.byUUID[6], data1.byUUID[7],
			data1.byUUID[8], data1.byUUID[9], data1.byUUID[10], data1.byUUID[11],
			data1.byUUID[12], data1.byUUID[13], data1.byUUID[14], data1.byUUID[15]);
		m_strUniversalUniqueId = szUUID;

		SMBios_TYPE2 data2;
		memset((void *)&data2, 0, sizeof(SMBios_TYPE2) );
		rawdata.GetData(data2);
 
		if( !IsBadWritePtr(data2.szProductName, 3) && !IsBadWritePtr(data2.szSerialNumber, 3))
		{
			if(data2.szProductName)
				m_strMotherboardModel = data2.szProductName;

			if(data2.szSerialNumber)
	    		m_strMotherboardSerialNumber = data2.szSerialNumber;
		}

		return true;
	}

protected:
	std::string m_strSystemManufacturer;
	std::string m_strProductName;
	std::string m_strUniversalUniqueId;
	std::string m_strMotherboardModel;
	std::string m_strMotherboardSerialNumber;
};