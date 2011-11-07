// BankLoaderTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../BankLoader/export.h"
#include "..//..//common/DriverDefine.h"


#include "../../../Encryption/Communication/comm.h"

int _tmain(int argc, _TCHAR* argv[])
{
	/*printf("Loading\n");

	HANDLE hProcess;
	DWORD PID;
	if(BankLoader::LoadProcess("C:\\Program Files\\Internet Explorer\\IEXPLORE.EXE", hProcess, PID) == false)
		return 0;

	while(BankLoader::IsFinished() == false)
	{
		Sleep(1000);
		//printf(".");
	}

	printf("\n");*/


	////////////////////////////////////////////
	HANDLE m_hDriver = CreateFileA("\\\\.\\" "MoneyHubPrt", GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if(m_hDriver == NULL)
	{
		printf("ceatefile is error! \n");
		return -1;
	}


	DWORD dwBytesReturned;
	unsigned char buffer[6000];
	unsigned char PIDs[255]={0};
	MYDRIVERSTATUS ll=DRIVER_RECEIVED;

	unsigned char encryptBuf[255];
	ULONG enLen=DownloadPack((PUCHAR)&ll,4,encryptBuf);

	::DeviceIoControl(m_hDriver, IOCTL_GET_DRIVER_STATUS, (LPVOID)encryptBuf, enLen, buffer, 6000, &dwBytesReturned, NULL);

	if (dwBytesReturned>0)
	{
		int size;

		size=UploadUnPack(buffer,dwBytesReturned,(PUCHAR)PIDs);


		if (size>0)
		{
			printf("%d\n",*(int *)PIDs);
		} else {
			printf("error!\n");
		}
	}

	getchar();
	////////////////////////////////////////////

	return 0;
}

