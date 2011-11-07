#pragma once

#include <string>

extern char HardDriveSerialNumber[1024];
extern char HardDriveModelNumber[1024];

int ReadPhysicalDriveInNTWithAdminRights();
int ReadIdeDriveAsScsiDriveInNT();
int ReadPhysicalDriveInNTWithZeroRights();
int ReadPhysicalDriveInNTUsingSmart();

class CHardDisk
{
public:
	CHardDisk(void)
	{
		if (ReadPhysicalDriveInNTWithAdminRights())
			return;

		if (ReadIdeDriveAsScsiDriveInNT())
			return;

		if (ReadPhysicalDriveInNTWithZeroRights())
			return;

		ReadPhysicalDriveInNTUsingSmart();
	}


public:
	std::string GetSerialNumber()
	{
		return strlen(HardDriveSerialNumber) > 0 ? HardDriveSerialNumber : "Unknown...";
	}

	std::string GetModelNumber()
	{
		return strlen(HardDriveModelNumber) > 0 ? HardDriveModelNumber : "Unknown...";
	}
};

