#include "stdafx.h"
#include "hdisk.h"

#include <windows.h>


char HardDriveSerialNumber[1024];
char HardDriveModelNumber[1024];

//////////////////////////////////////////////////////////////////////////

#define MAX_IDE_DRIVES 16

#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition

#define  IDE_ATAPI_IDENTIFY  0xA1
#define  IDE_ATA_IDENTIFY    0xEC

#pragma pack(1)

typedef struct _IDENTIFY_DATA {
	USHORT GeneralConfiguration;            // 00 00
	USHORT NumberOfCylinders;               // 02  1
	USHORT Reserved1;                       // 04  2
	USHORT NumberOfHeads;                   // 06  3
	USHORT UnformattedBytesPerTrack;        // 08  4
	USHORT UnformattedBytesPerSector;       // 0A  5
	USHORT SectorsPerTrack;                 // 0C  6
	USHORT VendorUnique1[3];                // 0E  7-9
	USHORT SerialNumber[10];                // 14  10-19
	USHORT BufferType;                      // 28  20
	USHORT BufferSectorSize;                // 2A  21
	USHORT NumberOfEccBytes;                // 2C  22
	USHORT FirmwareRevision[4];             // 2E  23-26
	USHORT ModelNumber[20];                 // 36  27-46
	UCHAR  MaximumBlockTransfer;            // 5E  47
	UCHAR  VendorUnique2;                   // 5F
	USHORT DoubleWordIo;                    // 60  48
	USHORT Capabilities;                    // 62  49
	USHORT Reserved2;                       // 64  50
	UCHAR  VendorUnique3;                   // 66  51
	UCHAR  PioCycleTimingMode;              // 67
	UCHAR  VendorUnique4;                   // 68  52
	UCHAR  DmaCycleTimingMode;              // 69
	USHORT TranslationFieldsValid:1;        // 6A  53
	USHORT Reserved3:15;
	USHORT NumberOfCurrentCylinders;        // 6C  54
	USHORT NumberOfCurrentHeads;            // 6E  55
	USHORT CurrentSectorsPerTrack;          // 70  56
	ULONG  CurrentSectorCapacity;           // 72  57-58
	USHORT CurrentMultiSectorSetting;       //     59
	ULONG  UserAddressableSectors;          //     60-61
	USHORT SingleWordDMASupport : 8;        //     62
	USHORT SingleWordDMAActive : 8;
	USHORT MultiWordDMASupport : 8;         //     63
	USHORT MultiWordDMAActive : 8;
	USHORT AdvancedPIOModes : 8;            //     64
	USHORT Reserved4 : 8;
	USHORT MinimumMWXferCycleTime;          //     65
	USHORT RecommendedMWXferCycleTime;      //     66
	USHORT MinimumPIOCycleTime;             //     67
	USHORT MinimumPIOCycleTimeIORDY;        //     68
	USHORT Reserved5[2];                    //     69-70
	USHORT ReleaseTimeOverlapped;           //     71
	USHORT ReleaseTimeServiceCommand;       //     72
	USHORT MajorRevision;                   //     73
	USHORT MinorRevision;                   //     74
	USHORT Reserved6[50];                   //     75-126
	USHORT SpecialFunctionsEnabled;         //     127
	USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;

#pragma pack()

typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;

typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

BYTE IdOutCmd[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
void PrintIdeInfo(int drive, DWORD diskdata[256]);
BOOL DoIDENTIFY(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				PDWORD lpcbBytesReturned);
char *flipAndCodeBytes(const char* str, int pos, int flip, char * buf);


int ReadPhysicalDriveInNTWithAdminRights(void)
{
	int done = FALSE;
	int drive = 0;

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		char driveName[256];
		sprintf_s(driveName, sizeof(driveName), "\\\\.\\PhysicalDrive%d", drive);

		hPhysicalDriveIOCTL = CreateFileA(driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
		}
		else
		{
			GETVERSIONOUTPARAMS VersionParams;
			DWORD cbBytesReturned = 0;

			memset((void*)&VersionParams, 0, sizeof(VersionParams));

			if (!DeviceIoControl(hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL, 0, &VersionParams, sizeof(VersionParams), &cbBytesReturned, NULL))
			{         
			}

			if (VersionParams.bIDEDeviceMap <= 0)
			{
			}
			else
			{
				BYTE bIDCmd = 0;
				SENDCMDINPARAMS scip;
				bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

				memset(&scip, 0, sizeof(scip));
				memset(IdOutCmd, 0, sizeof(IdOutCmd));

				if (DoIDENTIFY(hPhysicalDriveIOCTL, 
					&scip, 
					(PSENDCMDOUTPARAMS)&IdOutCmd, 
					(BYTE)bIDCmd,
					(BYTE)drive,
					&cbBytesReturned))
				{
					DWORD diskdata[256];
					int ijk = 0;
					USHORT *pIdSector = (USHORT *)((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer;

					for (ijk = 0; ijk < 256; ijk++)
						diskdata[ijk] = pIdSector[ijk];

					PrintIdeInfo(drive, diskdata);

					done = TRUE;
				}
			}

			CloseHandle(hPhysicalDriveIOCTL);
		}
	}

	return done;
}

#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE

int ReadIdeDriveAsScsiDriveInNT (void)
{
	int done = FALSE;
	int controller = 0;

	for (controller = 0; controller < 16; controller++)
	{
		HANDLE hScsiDriveIOCTL = 0;
		char   driveName [256];

		sprintf_s(driveName, sizeof(driveName), "\\\\.\\Scsi%d:", controller);

		hScsiDriveIOCTL = CreateFileA(driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			int drive = 0;

			for (drive = 0; drive < 2; drive++)
			{
				char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
				SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
				SENDCMDINPARAMS *pin =
					(SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
				DWORD dummy;

				memset (buffer, 0, sizeof (buffer));
				p -> HeaderLength = sizeof (SRB_IO_CONTROL);
				p -> Timeout = 10000;
				p -> Length = SENDIDLENGTH;
				p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				//strncpy_s((char *) p -> Signature, 8, "SCSIDISK", 8);
				memcpy(p->Signature, "SCSIDISK", 8);

				pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin -> bDriveNumber = drive;

				if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
					buffer,
					sizeof (SRB_IO_CONTROL) +
					sizeof (SENDCMDINPARAMS) - 1,
					buffer,
					sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
					&dummy, NULL))
				{
					SENDCMDOUTPARAMS *pOut =
						(SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
					IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
					if (pId -> sModelNumber [0])
					{
						DWORD diskdata [256];
						int ijk = 0;
						USHORT *pIdSector = (USHORT *) pId;

						for (ijk = 0; ijk < 256; ijk++)
							diskdata [ijk] = pIdSector [ijk];

						PrintIdeInfo (controller * 2 + drive, diskdata);

						done = TRUE;
					}
				}
			}
			CloseHandle (hScsiDriveIOCTL);
		}
	}

	return done;
}

int ReadPhysicalDriveInNTWithZeroRights (void)
{
	int done = FALSE;
	int drive = 0;

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		char driveName [256];
		sprintf_s(driveName, sizeof(driveName), "\\\\.\\PhysicalDrive%d", drive);

		hPhysicalDriveIOCTL = CreateFileA(driveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
		}
		else
		{
			STORAGE_PROPERTY_QUERY query;
			DWORD cbBytesReturned = 0;
			char buffer [10000];

			memset ((void *) & query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;

			memset (buffer, 0, sizeof (buffer));

			if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
				& query,
				sizeof (query),
				& buffer,
				sizeof (buffer),
				& cbBytesReturned, NULL) )
			{         
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
				char serialNumber [1000];
				char modelNumber [1000];
				char vendorId [1000];
				char productRevision [1000];

				flipAndCodeBytes(buffer,
					descrip -> VendorIdOffset,
					0, vendorId );
				flipAndCodeBytes(buffer,
					descrip -> ProductIdOffset,
					0, modelNumber );
				flipAndCodeBytes(buffer,
					descrip -> ProductRevisionOffset,
					0, productRevision );
				flipAndCodeBytes(buffer,
					descrip -> SerialNumberOffset,
					1, serialNumber );

				if (0 == HardDriveSerialNumber [0] && (isalnum(serialNumber[0]) || isalnum(serialNumber[19])))
			 {
				 strcpy_s(HardDriveSerialNumber, sizeof(HardDriveSerialNumber), serialNumber);
				 strcpy_s(HardDriveModelNumber, sizeof(HardDriveModelNumber), modelNumber);
				 done = TRUE;
			 }
				// Get the disk drive geometry.
				memset (buffer, 0, sizeof(buffer));
				if ( ! DeviceIoControl (hPhysicalDriveIOCTL,
					IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
					NULL,
					0,
					&buffer,
					sizeof(buffer),
					&cbBytesReturned,
					NULL))
				{
				}
				else
				{         
					DISK_GEOMETRY_EX* geom = (DISK_GEOMETRY_EX*) &buffer;
					int fixed = (geom->Geometry.MediaType == FixedMedia);
					__int64 size = geom->DiskSize.QuadPart;

				}
			}
			else
		 {
		 }

			CloseHandle (hPhysicalDriveIOCTL);
		}
	}

	return done;
}

int ReadPhysicalDriveInNTUsingSmart(void)
{
	int done = FALSE;
	int drive = 0;

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		char driveName[256];
		sprintf_s(driveName, sizeof(driveName), "\\\\.\\PhysicalDrive%d", drive);

		hPhysicalDriveIOCTL = CreateFileA(driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 
			NULL, OPEN_EXISTING, 0, NULL);

		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
		}
		else
		{
			GETVERSIONINPARAMS GetVersionParams;
			DWORD cbBytesReturned = 0;

			memset((void*)&GetVersionParams, 0, sizeof(GetVersionParams));
			if (!DeviceIoControl(hPhysicalDriveIOCTL, SMART_GET_VERSION,
				NULL, 0,
				&GetVersionParams, sizeof (GETVERSIONINPARAMS),
				&cbBytesReturned, NULL))
			{         
			}
			else
			{
				ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
				PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS)malloc(CommandSize);

				Command->irDriveRegs.bCommandReg = 0xEC;
				DWORD BytesReturned = 0;
				if (!DeviceIoControl(hPhysicalDriveIOCTL, 
					SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
					Command, CommandSize,
					&BytesReturned, NULL))
				{
				} 
				else
				{
					DWORD diskdata [256];
					USHORT *pIdSector = (USHORT *)(PIDENTIFY_DATA)((PSENDCMDOUTPARAMS)Command)->bBuffer;

					for (int ijk = 0; ijk < 256; ijk++)
						diskdata [ijk] = pIdSector [ijk];

					//PrintIdeInfo(drive, diskdata);
					done = TRUE;
				}

				CloseHandle(hPhysicalDriveIOCTL);
				free(Command);
			}
		}
	}

	return done;
}


char *ConvertToString(DWORD diskdata[256], int firstIndex, int lastIndex, char* buf)
{
	int index = 0;
	int position = 0;

	for (index = firstIndex; index <= lastIndex; index++)
	{
		buf[position++] = (char)(diskdata[index] / 256);
		buf[position++] = (char)(diskdata[index] % 256);
	}

	buf[position] = '\0';

	for (index = position - 1; index > 0 && isspace(buf[index]); index--)
		buf [index] = '\0';

	return buf;
}

void PrintIdeInfo(int drive, DWORD diskdata[256])
{
	char serialNumber[1024];
	char modelNumber[1024];
	char revisionNumber[1024];
	char bufferSize [32];

	__int64 sectors = 0;
	__int64 bytes = 0;

	ConvertToString(diskdata, 10, 19, serialNumber);
	ConvertToString(diskdata, 27, 46, modelNumber);
	ConvertToString(diskdata, 23, 26, revisionNumber);
	sprintf_s(bufferSize, sizeof(bufferSize), "%u", diskdata[21] * 512);

	if (0 == HardDriveSerialNumber[0] && (isalnum(serialNumber [0]) || isalnum(serialNumber[19])))
	{
		strcpy_s(HardDriveSerialNumber, sizeof(HardDriveSerialNumber), serialNumber);
		strcpy_s(HardDriveModelNumber, sizeof(HardDriveModelNumber), modelNumber);
	}
}

BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				 PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command.
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP -> irDriveRegs.bFeaturesReg = 0;
	pSCIP -> irDriveRegs.bSectorCountReg = 1;
	//pSCIP -> irDriveRegs.bSectorNumberReg = 1;
	pSCIP -> irDriveRegs.bCylLowReg = 0;
	pSCIP -> irDriveRegs.bCylHighReg = 0;

	// Compute the drive number.
	pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

	// The command can either be IDE identify or ATAPI identify.
	pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
	pSCIP -> bDriveNumber = bDriveNum;
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

	return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		(LPVOID) pSCIP,
		sizeof(SENDCMDINPARAMS) - 1,
		(LPVOID) pSCOP,
		sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL) );
}

char * flipAndCodeBytes (const char * str,
						 int pos,
						 int flip,
						 char * buf)
{
	int i;
	int j = 0;
	int k = 0;

	buf [0] = '\0';
	if (pos <= 0)
		return buf;

	if ( ! j)
	{
		char p = 0;

		// First try to gather all characters representing hex digits only.
		j = 1;
		k = 0;
		buf[k] = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = tolower(str[i]);

			if (isspace(c))
				c = '0';

			++p;
			buf[k] <<= 4;

			if (c >= '0' && c <= '9')
				buf[k] |= (unsigned char) (c - '0');
			else if (c >= 'a' && c <= 'f')
				buf[k] |= (unsigned char) (c - 'a' + 10);
			else
			{
				j = 0;
				break;
			}

			if (p == 2)
			{
				int a = (unsigned)(unsigned char)buf[k];
				if (buf[k] != '\0' && ! isprint(a))
				{
					j = 0;
					break;
				}
				++k;
				p = 0;
				buf[k] = 0;
			}

		}
	}

	if ( ! j)
	{
		// There are non-digit characters, gather them as is.
		j = 1;
		k = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = str[i];

			if ( ! isprint(c))
			{
				j = 0;
				break;
			}

			buf[k++] = c;
		}
	}

	if ( ! j)
	{
		// The characters are not there or are not printable.
		k = 0;
	}

	buf[k] = '\0';

	if (flip)
		// Flip adjacent characters
		for (j = 0; j < k; j += 2)
		{
			char t = buf[j];
			buf[j] = buf[j + 1];
			buf[j + 1] = t;
		}

		// Trim any beginning and end space
		i = j = -1;
		for (k = 0; buf[k] != '\0'; ++k)
		{
			if (! isspace(buf[k]))
			{
				if (i < 0)
					i = k;
				j = k;
			}
		}

		if ((i >= 0) && (j >= 0))
		{
			for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
				buf[k - i] = buf[k];
			buf[k - i] = '\0';
		}

		return buf;
}
