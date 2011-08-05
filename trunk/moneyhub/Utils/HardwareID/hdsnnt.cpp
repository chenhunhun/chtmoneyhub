#include <windows.h>
#include "libhdsn.h"
#include <stdio.h>
#include <ctype.h>
#pragma warning(disable: 4201)
#include <winioctl.h>
#pragma warning(default: 4201)
#include "hdsnutil.h"

#pragma pack(4)

typedef enum _TUOTUO_STORAGE_QUERY_TYPE {
    TUOTUO_PropertyStandardQuery = 0,          // Retrieves the descriptor
		TUOTUO_PropertyExistsQuery,                // Used to test whether the descriptor is supported
		TUOTUO_PropertyMaskQuery,                  // Used to retrieve a mask of writeable fields in the descriptor
		TUOTUO_PropertyQueryMaxDefined     // use to validate the value
} TUOTUO_STORAGE_QUERY_TYPE, *PTUOTUO_STORAGE_QUERY_TYPE;
typedef enum _TUOTUO_STORAGE_PROPERTY_ID {
    TUOTUO_StorageDeviceProperty = 0,
		TUOTUO_StorageAdapterProperty
} TUOTUO_STORAGE_PROPERTY_ID, *PTUOTUO_STORAGE_PROPERTY_ID;

typedef struct _TUOTUO_STORAGE_PROPERTY_QUERY {
    TUOTUO_STORAGE_PROPERTY_ID PropertyId;
    TUOTUO_STORAGE_QUERY_TYPE QueryType;
    UCHAR AdditionalParameters[1];
	
} TUOTUO_STORAGE_PROPERTY_QUERY, *PTUOTUO_STORAGE_PROPERTY_QUERY;


#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

#ifndef _NTDDSTOR_H_
typedef enum _STORAGE_BUS_TYPE {
    BusTypeUnknown = 0x00,
		BusTypeScsi,
		BusTypeAtapi,
		BusTypeAta,
		BusType1394,
		BusTypeSsa,
		BusTypeFibre,
		BusTypeUsb,
		BusTypeRAID,
		BusTypeMaxReserved = 0x7F
} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;
#endif

#ifndef IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER
#define IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER   CTL_CODE(IOCTL_STORAGE_BASE, 0x304, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif
#define  SENDIDLENGTH  sizeof (TUOTUO_SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition



typedef struct _TUOTUO_STORAGE_DEVICE_DESCRIPTOR {
    ULONG Version;
    ULONG Size;
    UCHAR DeviceType;
    UCHAR DeviceTypeModifier;
    BOOLEAN RemovableMedia;
    BOOLEAN CommandQueueing;
    ULONG VendorIdOffset;
    ULONG ProductIdOffset;
    ULONG ProductRevisionOffset;
    ULONG SerialNumberOffset;
    STORAGE_BUS_TYPE BusType;
    ULONG RawPropertiesLength;
    UCHAR RawDeviceProperties[1];

} TUOTUO_STORAGE_DEVICE_DESCRIPTOR, *PTUOTUO_STORAGE_DEVICE_DESCRIPTOR;

typedef struct _MEDIA_SERAL_NUMBER_DATA {
	ULONG  SerialNumberLength; 
	ULONG  Result;
	ULONG  Reserved[2];
	UCHAR  SerialNumberData[1];
} MEDIA_SERIAL_NUMBER_DATA, *PMEDIA_SERIAL_NUMBER_DATA;

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;


bool GetFirstHDSerial_NT_PhysicalDriveAdmin(char *buf, size_t len)
{
	bool done = false;
	int drive = 0;
	BYTE IdOutCmd [sizeof (TUOTUO_SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
	for (drive = 0; drive < MAX_IDE_DRIVES && !done; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;
		
		char driveName [256];
		
		wsprintfA (driveName, "\\\\.\\PhysicalDrive%d", drive);
		
		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFileA (driveName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		
		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			GETVERSIONOUTPARAMS VersionParams;
			DWORD               cbBytesReturned = 0;
			
			ZeroMemory ((void*) &VersionParams,  sizeof(VersionParams));
			
			if ( ! DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL, 
				0,
				&VersionParams,
				sizeof(VersionParams),
				&cbBytesReturned, NULL) )
			{         
				 continue;
			}
			
			if (VersionParams.bIDEDeviceMap > 0)
			{
				BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
				TUOTUO_SENDCMDINPARAMS  scip;
				bIDCmd = (BYTE)((VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY);
				
				ZeroMemory (&scip, sizeof(scip));
				ZeroMemory (IdOutCmd, sizeof(IdOutCmd));
				
				if ( DoIDENTIFY (hPhysicalDriveIOCTL, 
					&scip, 
					(PTUOTUO_SENDCMDOUTPARAMS)&IdOutCmd, 
					(BYTE) bIDCmd,
					(BYTE) drive,
					&cbBytesReturned))
				{
					DWORD diskdata [256];
					int ijk = 0;
					USHORT *pIdSector = (USHORT *)
						((PTUOTUO_SENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;
					
					for (ijk = 0; ijk < 256; ijk++)
						diskdata [ijk] = pIdSector [ijk];
					
					GenSerial(diskdata,buf,len);
					if (IsCharAlphaNumeric(buf[0])) {
						done = true;
					}
				}
			}
			
			CloseHandle (hPhysicalDriveIOCTL);
		}
	}
	return done;
}

bool GetFirstHDSerial_NT_PhysicalDriveZeroRight(char *buf, size_t len)
{
   bool done = false;
   int drive = 0;

   for (drive = 0; (drive < MAX_IDE_DRIVES) && (!done); drive++)
   {
      HANDLE hPhysicalDriveIOCTL = 0;

      char driveName [256]     = {0};
	  char serialNumber [1000] = {0};
	  
      wsprintfA (driveName, "\\\\.\\PhysicalDrive%d", drive);

         //  Windows NT, Windows 2000, Windows XP - admin rights not required
      hPhysicalDriveIOCTL = CreateFileA (driveName, 0,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_EXISTING, 0, NULL);

      if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
      {
		 TUOTUO_STORAGE_PROPERTY_QUERY query;
         DWORD cbBytesReturned = 0;
		 DWORD dwBufferLen = 10000;
		 char *buffer  = (char *)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,dwBufferLen);
		 
		 if (buffer)
		 {
			 ZeroMemory((void *) & query, sizeof (query));
			 query.PropertyId = TUOTUO_StorageDeviceProperty;
			 query.QueryType = TUOTUO_PropertyStandardQuery;

			 if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
					   & query,
					   sizeof (query),
					   buffer,
					   dwBufferLen,
					   & cbBytesReturned, NULL) )
			 {         
				 TUOTUO_STORAGE_DEVICE_DESCRIPTOR * descrip = (TUOTUO_STORAGE_DEVICE_DESCRIPTOR *) buffer;
				 if (descrip->SerialNumberOffset!=0)
				 {
					 flipAndCodeBytes ( &buffer [descrip -> SerialNumberOffset],serialNumber);
					 TrimSerial(serialNumber);
					 if (IsCharAlphaNumeric(serialNumber[0])) {
						MoveMemory(buf,serialNumber,len);
						 done = true;
					 }
				 }
			 } // if DeviceIoControl 
			 if (!done) {
				 ZeroMemory (buffer, dwBufferLen);
				 if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER,
						   NULL,
						   0,
						   buffer,
						   dwBufferLen,
						   & cbBytesReturned, NULL) )
				 {
					 MEDIA_SERIAL_NUMBER_DATA * mediaSerialNumber = 
									(MEDIA_SERIAL_NUMBER_DATA *) buffer;
					 lstrcpyA (serialNumber, (char *) mediaSerialNumber -> SerialNumberData);
					 TrimSerial(serialNumber);
					 if (IsCharAlphaNumeric(serialNumber[0])) {
						 MoveMemory(buf,serialNumber,len);
						 done = true;
					 }
					 
				 } // if DeviceIoCtl
			 }  // if !done
 			HeapFree(GetProcessHeap(),0,buffer);
		 } // if buffer
         CloseHandle (hPhysicalDriveIOCTL);
      }
   }

   return done;
}

bool GetFirstHDSerial_NT_IDEasSCSI(char *buf, size_t len)
{
	bool done = false;
	int controller = 0;
	
	for (controller = 0; controller < 16 && !done ; controller++)
	{
		HANDLE hScsiDriveIOCTL = 0;
		char   driveName [256];
		
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		wsprintfA (driveName, "\\\\.\\Scsi%d:", controller);
		
		//  Windows NT, Windows 2000, any rights should do
		hScsiDriveIOCTL = CreateFileA (driveName,
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
				TUOTUO_SENDCMDINPARAMS *pin =
					(TUOTUO_SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
				DWORD dummy;
				
				ZeroMemory (buffer,  sizeof (buffer));
				p -> HeaderLength = sizeof (SRB_IO_CONTROL);
				p -> Timeout = 10000;
				p -> Length = SENDIDLENGTH;
				p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				MoveMemory ((char *) p -> Signature, "SCSIDISK", 8);
				
				pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin -> bDriveNumber = (BYTE)drive;
				
				if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
					buffer,
					sizeof (SRB_IO_CONTROL) +
					sizeof (TUOTUO_SENDCMDINPARAMS) - 1,
					buffer,
					sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
					&dummy, NULL))
				{
					TUOTUO_SENDCMDOUTPARAMS *pOut =
						(TUOTUO_SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
					IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
					if (pId -> sModelNumber [0])
					{
						DWORD diskdata [256];
						int ijk = 0;
						USHORT *pIdSector = (USHORT *) pId;
						
						for (ijk = 0; ijk < 256; ijk++)
							diskdata [ijk] = pIdSector [ijk];
						
						GenSerial(diskdata,buf, len);
						if (IsCharAlphaNumeric(buf[0])) {
							done = true;
						}
					}
				}
			}
			CloseHandle (hScsiDriveIOCTL);
		}
	}
	
	return done;
}
