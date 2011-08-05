#include <windows.h>
#include "hdsnutil.h"

char * flipAndCodeBytes (char * str, char *out)
{
	
	int num = lstrlenA (str);
	lstrcpyA (out, "");
	for (int i = 0; i < num; i += 4)
	{
		for (int j = 1; j >= 0; j--)
		{
			int sum = 0;
			for (int k = 0; k < 2; k++)
			{
				sum *= 16;
				switch (str [i + j * 2 + k])
				{
				case '0': sum += 0; break;
				case '1': sum += 1; break;
				case '2': sum += 2; break;
				case '3': sum += 3; break;
				case '4': sum += 4; break;
				case '5': sum += 5; break;
				case '6': sum += 6; break;
				case '7': sum += 7; break;
				case '8': sum += 8; break;
				case '9': sum += 9; break;
				case 'a': sum += 10; break;
				case 'b': sum += 11; break;
				case 'c': sum += 12; break;
				case 'd': sum += 13; break;
				case 'e': sum += 14; break;
				case 'f': sum += 15; break;
				}
			}
			if (sum > 0) 
			{
				char sub [2];
				sub [0] = (char) sum;
				sub [1] = 0;
				lstrcatA (out, sub);
			}
		}
	}
	
	return out;
}


bool DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PTUOTUO_SENDCMDINPARAMS pSCIP,
                 PTUOTUO_SENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
                 PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command.
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP -> irDriveRegs.bFeaturesReg = 0;
	pSCIP -> irDriveRegs.bSectorCountReg = 1;
	pSCIP -> irDriveRegs.bSectorNumberReg = 1;
	pSCIP -> irDriveRegs.bCylLowReg = 0;
	pSCIP -> irDriveRegs.bCylHighReg = 0;
	
	// Compute the drive number.
	pSCIP -> irDriveRegs.bDriveHeadReg = (BYTE) (0xA0 | ((bDriveNum & 1) << 4));
	
	// The command can either be IDE identify or ATAPI identify.
	pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
	pSCIP -> bDriveNumber = bDriveNum;
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	
	return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		(LPVOID) pSCIP,
		sizeof(TUOTUO_SENDCMDINPARAMS) - 1,
		(LPVOID) pSCOP,
		sizeof(TUOTUO_SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL)  != 0);
}

void TrimSerial(char *string)
{
	char *begin=0, *end=0;
	// trim non-humanreadable char from harddisk serial
	for (begin = string; *begin; begin ++)
		if (IsCharAlphaNumeric(*begin)) 
			break;
		for (end = begin + lstrlenA(begin) ; end >begin; end --)
			if (IsCharAlphaNumeric(*end))  
				break;
			else
				*end = '\0';
	MoveMemory(string, begin, lstrlenA(begin));
}

void GenSerial (DWORD diskdata [256], char *serial, int size)
{
	char string1 [1024] = {0};
	//  copy the hard drive serial number to the buffer
	lstrcpyA (string1, ConvertToString (diskdata, 10, 19));
	TrimSerial(string1);
	MoveMemory(serial,string1,size);		

}


char *ConvertToString (DWORD diskdata [256], int firstIndex, int lastIndex)
{
	static char string [1024] = {0};
	int index = 0;
	int position = 0;
	
	//  each integer has two characters stored in it backwards
	for (index = firstIndex; index <= lastIndex; index++)
	{
		//  get high byte for 1st character
		string [position] = (char) (diskdata [index] / 256);
		position++;
		
		//  get low byte for 2nd character
		string [position] = (char) (diskdata [index] % 256);
		position++;
	}
	
	//  end the string 
	string [position] = '\0';
	
	//  cut off the trailing blanks
	for (index = position - 1; index > 0 && ' ' == string [index]; index--)
		string [index] = '\0';
	
	return string;
}
