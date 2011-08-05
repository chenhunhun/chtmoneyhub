// DigsigGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../encryption/SHA512.h"
#include "../encryption/MD5.h"
#include "../encryption/AES.h"
//#include "../encryption/RSA.h"
#include "../encryption/Encryption.h"

#include "../../../ThirdParty/tinyxml/tinyxml.h"

#include "CHKWriter.h"
#include "../../../Encryption/CHKFile/CHK.h"
#include "..//..//../Utils/CryptHash/base64.h"

CCHKWriter g_chkWriter;

bool GenerateDigSig(const char* filename)
{
	CMD5 md5;
	//uint8_t hash[SHA512_HASH_SIZE];

	printf("Generating: %s ", filename);

	FILE* fp;
	fopen_s(&fp,filename, "rb");
	if(fp == NULL)
	{
		printf("failed\n");
		return false;
	}

	while(!feof(fp))
	{
		unsigned char buffer[1024];
		int size = fread(buffer, 1, 1024, fp);
		md5.MD5Update(buffer, size);
	}

	fclose(fp);

	unsigned char final[MD5_HASH_SIZE];
	md5.MD5Final(final);

	g_chkWriter.AddFileVerify(filename, final, MD5_HASH_SIZE);

	printf("success\n");

	return true;
}

void GenerateDigSigs()
{
	TiXmlDocument *myDocument = new TiXmlDocument("VerifyList.xml");
	myDocument->LoadFile();

	TiXmlElement *VerifyList = myDocument->RootElement();

	for(TiXmlElement *Binary = VerifyList->FirstChildElement();
		Binary; Binary = Binary->NextSiblingElement())
	{
		TiXmlAttribute *IDAttribute = Binary->FirstAttribute();
		GenerateDigSig(IDAttribute->Value());
	}
}

void copyData(unsigned char * p , int len)
{
	HANDLE hGlobalMemory = GlobalAlloc(GHND, len + 1); 
	LPBYTE lpGlobalMemory = (LPBYTE)GlobalLock(hGlobalMemory);
	for (int i = 0; i < len; i++) 
		memcpy(lpGlobalMemory+i,p+i,1);

	GlobalUnlock(hGlobalMemory); 
	
	OpenClipboard(NULL); 
	::EmptyClipboard(); 
	::SetClipboardData(CF_TEXT, hGlobalMemory); 
	::CloseClipboard(); 
}

int main (int argc, char *argv[])
{
	GenerateDigSigs();

	int hashlen1;
	unsigned char* pHash1 = (unsigned char*)g_chkWriter.GetFileHashes(hashlen1);

	int hashlen = hashlen1 + 4;
	unsigned char* pHash = (unsigned char*)malloc(hashlen);

	int cnt = hashlen1 / MD5_HASH_SIZE;
	memcpy(pHash, &cnt, 4);
	memcpy(pHash + 4, pHash1, hashlen1);

	// ²ÉÓÃadamËã·¨

	int totalLength = hashlen + 512;
	unsigned char *total = (unsigned char *)malloc(totalLength);

	if (total != NULL)
	{
		totalLength = packCHK(pHash, hashlen, total);

		//copy data 
		unsigned char * base_content = (unsigned char * )malloc(totalLength*2);
		int iReturnSize = 0;
		base64_encode(total,totalLength,base_content,&iReturnSize);
		copyData(base_content,iReturnSize);
		

		char module_path[MAX_PATH + 1];
		GetModuleFileNameA(NULL, module_path, MAX_PATH);
		*(strrchr(module_path, '\\') + 1) = 0;
		strcat_s(module_path,261, "Authen.chk");

		FILE* fp;
		fopen_s(&fp,module_path, "wb");
		fwrite(total, 1, totalLength,fp);
		fclose(fp);

		FILE* ffp;
		fopen_s(&ffp,"base64.txt", "wb");
		fwrite(base_content, 1, iReturnSize,ffp);
		fclose(ffp);

		free(base_content);

		free(total);
	}
	else
	{
		printf("error memory");
	}
}

