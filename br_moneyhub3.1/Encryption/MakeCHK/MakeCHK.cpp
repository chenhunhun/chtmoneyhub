// MakeCHK.cpp : Defines the entry point for the console application.
//
/**
*   单独校验的文件夹共三个：分别是 BankInfo、Html、Config
*	                                                    2010-11-30
*/
#include "stdafx.h"
#include <windows.h>
#include <string>
#include <iostream>

#include "../SHA1/sha.h"
#include "../CHKFile/CHK.h"

char g_szSpecialName[][255]={ "BankInfo","Thumbs.db","Config","Html",  "bank.mchk","MoneyHub.mchk","config.mchk","html.mchk","usbkeyinfo.xml","syslog.txt","end", "BillUrl.xml"};

// std::string  getAppDataBank()
// {	
// 	std::string wcsPath ;
// 	char      wPath[255];
// 
// 	ExpandEnvironmentStringsA("%appdata%",wPath,_countof(wPath) );
// 	wcsPath = wPath;
// 	wcsPath += "\\MoneyHub\\BankInfo\\banks";
// 
// 	return wcsPath;
// }

/**
*
*/
bool  isSpecial(char * pSzPathName)
{
	for( int i=0; 0 != strcmp(g_szSpecialName[i],"end"); i++) 
	{
		if( 0 == strcmp(g_szSpecialName[i], pSzPathName) )
		{
			OutputDebugStringA(pSzPathName);
			return true;
		}
	}

	return false;
}
// 目录遍历
//
void TraversalPath(const char* lpszPath,const char * lpszFilePath, FILE *fp)
{
	std::string dir = lpszPath;
	dir += "\\*.*";

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(dir.c_str(), &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE)
		return;
	do{
		std::string fn = FindFileData.cFileName;

		if ((_tcscmp(fn.c_str(), ".") != 0) && (_tcscmp(fn.c_str(), "..") != 0) && (_tcsicmp(fn.c_str(), ".svn") != 0) && (_tcsicmp(fn.c_str(), "BillUrl.xml") != 0)
			&& (_tcsicmp(fn.c_str(), "info.xml") != 0) && (_tcsicmp(fn.c_str(), "syslist.txt") != 0) && (_tcsicmp(fn.c_str(), "CloudCheck.ini") != 0) && (_tcsicmp(fn.c_str(), "BlackList.txt") != 0))
		{
			//为每个单独升级模块制作xxx.mchk完整性效验文件
			if( isSpecial(FindFileData.cFileName) )
				continue;

			// 子目录
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string subDir = lpszPath;
				subDir = subDir + "\\" + fn;
				std::string filePath = lpszFilePath;
				filePath = filePath + "\\" + fn;

				// 迭代遍历子目录
				TraversalPath(subDir.c_str(),filePath.c_str(),fp);
			}
			else // 文件
			{
				std::string subFile = lpszPath;
				subFile = subFile + "\\" + fn;
				std::string filePath = lpszFilePath;
				filePath = filePath + "\\" + fn;

				if(  !isSpecial(FindFileData.cFileName) )
				{
					// 输出文件名
					printf("%s\n", subFile.c_str());
					printf("%s\n", filePath.c_str());
	
					fprintf(fp,"%s\n", subFile.c_str());
					fprintf(fp,"%s\n", filePath.c_str());
				}
			}
		}
	}while (FindNextFile(hFind, &FindFileData) != 0);


	FindClose(hFind);
}
// 生成校验数据的函数
int MakeMoneyHubContent(const char* lpszFileList, unsigned char ** buffer)
{
	FILE *fp;
	char fullName[256];
	char relativeName[256];
	unsigned char md[20];

	SHA_CTX	c;
	unsigned char *content;
	int length;
	unsigned char *ptr;
	int current;

	unsigned char readBuffer[4096];
	
	int nameLength;

	length = 10240;
	content = (unsigned char *)malloc(length);
	if (content == NULL)
	{
		return -1;
	}
	current = 4;
	ptr = content + 4;

	fp = fopen(lpszFileList,"r");
	while (!feof(fp))
	{
		// 从文件读入白名单名字
		fscanf(fp,"%s",fullName);
		fscanf(fp,"%s",relativeName);

		FILE *fContent;
		fContent = fopen(fullName,"rb");
		if (fContent!=NULL)
		{
			int readLength;
			SHA1_Init(&c);
			while (1)
			{
				readLength = fread(readBuffer,1,4096,fContent);
				if (readLength <= 0)
				{
					break;
				}
				SHA1_Update(&c,(const void *)readBuffer,readLength);
			}
			fclose(fContent);
			// 生成校验数据放入md中
			SHA1_Final(md, &c);
			nameLength = strlen(relativeName);
			if ((current + nameLength + 20 + 2) > length)
			{
				unsigned char *tempBuffer;

				tempBuffer=(unsigned char *)malloc(length+10240);
				if (tempBuffer!=NULL)
				{
					memcpy(tempBuffer, content, length);
					length += 10240;
					free(content);
					content = tempBuffer;
					ptr = content+current;
				} 
				else
				{
					free (content);
					fclose(fContent);
					fclose(fp);
					return -2;
				}
			}
			ptr[0] = nameLength & 0xff;
			ptr[1] = (nameLength >> 8) & 0xff;
			ptr += 2;
			current += 2;
			memcpy(ptr, relativeName,nameLength);
			ptr += nameLength;
			current += nameLength;
			memcpy(ptr,md,20);
			ptr += 20;
			current += 20;
		}
	}
	fclose(fp);


	ptr = content;
	current = current - 4;
	ptr[0] = current&0xff;
	ptr[1] =( current&0xff00) >> 8;
	ptr[2] = (current&0xff0000) >> 16;
	ptr[3] = (current&0xff000000) >> 24;

	*buffer = content;

	return current + 4;
}

void MakeMoneyHubCHK(const char* lpszCHKFile, unsigned char * buffer, int length)
{
	unsigned char *total;
	int totalLength;

	totalLength = length + 512;
	total = (unsigned char *)malloc (totalLength);

	if (total != NULL)
	{
		FILE *fp;

		totalLength = packCHK(buffer,length,total);

		fp = fopen(lpszCHKFile, "wb");
		fwrite(total,1,totalLength,fp);
		fclose(fp);

		free(total);
	}
}
void MakeFileCHK(const char* lpszFile,const char* lpszCHKFile)
{
	unsigned char * buffer;
	HANDLE hFile = CreateFileA(lpszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "failed to open info.xml" << std::endl;
		return;
	}

	DWORD dwLen = GetFileSize(hFile, NULL);
	DWORD dwRead = 0;
	buffer = new unsigned char[dwLen];
	if (!ReadFile(hFile, buffer, dwLen, &dwRead, NULL))
	{
		std::cout << "failed to read info.xml" << std::endl;

		CloseHandle(hFile);

		return;
	}

	CloseHandle(hFile);
	MakeMoneyHubCHK(lpszCHKFile, buffer, dwLen);
	delete []buffer;
}

/**
*            为每个银行单独制作相应info.mchk
*
*/
#define  BANKINFOPATH        "BankInfo\\banks\\*.*"
#define  BANKINFOPATHNOFIND  "BankInfo\\banks\\"
void makeBankChk(LPSTR strP,LPSTR strParentP = NULL)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFileA(strP, &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do{
		std::string fn = FindFileData.cFileName;

		if ((_tcscmp(fn.c_str(), ".") != 0) && (_tcscmp(fn.c_str(), "..") != 0) && (_tcsicmp(fn.c_str(), ".svn") != 0) )
		{
				// 子目录
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char buf[255];
					strcpy_s(buf,_countof(buf),strP);
					char *p = strrchr(buf,'\\');
					
					std::string strPath;
					std::string strPPath;

					if(p)
					{
						*(p+1) = '\0';
						strPath = buf;

						strPath += FindFileData.cFileName;
						strPath += "\\";
						strPath += "*.*";
					
						if(strParentP != NULL)
						{
							strPPath = strParentP;
							strPPath += "\\";
							strPPath += FindFileData.cFileName;
						}
						else
							strPPath = FindFileData.cFileName;

						makeBankChk((LPSTR)strPath.c_str(),(LPSTR)strPPath.c_str());
					}
				}
				else 
				{
					if(0 == strcmp(FindFileData.cFileName,"info.xml"))
					{
						char buf[255];
						strcpy_s(buf,_countof(buf),strP);
						char *p = strrchr(buf,'\\');
						std::string strPath;

						if(p)
						{
							*(p+1) = '\0';
							strPath = buf;

							std::string bankList = strPath + "\\info.xml";
							std::string bankListCHK = strPath + "\\info.mchk";

							MakeFileCHK(bankList.c_str(),bankListCHK.c_str());
						}
					}
				}
		}
	}while (FindNextFile(hFind, &FindFileData) != 0);


	FindClose(hFind);

}

/**
*为Html和Config生成效验文件
*     filepath:      为该目录下文件生成chk效验,format: "c:\\dell"
      storeFileName：存放文件目录的文件路径
	  chkName：      chk文件名称
*/
void makeCheckChk(LPSTR filePath,  LPSTR stroreFileName , LPSTR chkName)
{	
	std::string   strPath = filePath;

	FILE * fp = fopen(stroreFileName, "w");
	TraversalPath(strPath.c_str(), "", fp);
	fclose(fp);

	unsigned char * buffer;
	// 生成校验数据
	int ret = MakeMoneyHubContent(stroreFileName, &buffer);

	// 保存数据CHK
	std::string strCHKFile = strPath;
	strCHKFile += "\\";
	strCHKFile += chkName;

	MakeMoneyHubCHK(strCHKFile.c_str(), buffer, ret);

	delete []buffer;
}

/**
*  为每个银行生成完整性效验信息  bank.mchk
*/
void makeBankCheckChk(LPSTR filePath,LPSTR path,LPSTR parentPath, FILE* p = NULL)
{
	std::string  dir = path;
	dir += "\\*.*";

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFileA(dir.c_str(), &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE)
		return;

	do{
		std::string fn = FindFileData.cFileName;

		if ((_tcscmp(fn.c_str(), ".") != 0) && (_tcscmp(fn.c_str(), "..") != 0) && (_tcsicmp(fn.c_str(), ".svn") != 0) )
		{
			// 子目录
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string   strPath = path;
				strPath += '\\';
				strPath += FindFileData.cFileName;

				makeCheckChk((LPSTR)strPath.c_str(), filePath, "bank.mchk");
			}
		}
	}while (FindNextFile(hFind, &FindFileData) != 0);

	FindClose(hFind);
}



int _tmain(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Please input root dir!" << std::endl;
		return 0;
	}

	std::cout << "start make CHK file..." << std::endl;
	std::string strFileList = argv[1];//..Release
	
	//unsigned char * buffer; //gao
	//int ret; //gao

    //为各个银行生成相应的info.mchk文件
	std::string dir = strFileList;
	dir += "\\";
	dir += BANKINFOPATH;
	makeBankChk((LPSTR)dir.c_str());
	
	//为Html和Config生成相应的info.mchk文件
	dir = strFileList;
	dir += "\\Html\\*.*";
	makeBankChk((LPSTR)dir.c_str());

	dir = strFileList;
	dir += "\\Config\\*.*";
	makeBankChk((LPSTR)dir.c_str());

	// 根目录
	char szRootDir[MAX_PATH + 1];
	GetModuleFileName(NULL, szRootDir, MAX_PATH);
	*(strrchr(szRootDir, '\\') + 1) = '\0';
			
	// 生成syslist.mchk
	std::string strSyslist2 = strFileList + "\\Config\\syslist.txt";
	std::string strSyslistCHK2 = strFileList + "\\Config\\syslist.mchk";

	MakeFileCHK(strSyslist2.c_str(),strSyslistCHK2.c_str());

	std::string strCloudlist2 = strFileList + "\\Config\\CloudCheck.ini";
	std::string strCloudlistCHK2 = strFileList + "\\Config\\CloudCheck.mchk";

	MakeFileCHK(strCloudlist2.c_str(),strCloudlistCHK2.c_str());

	std::string strBlackListlist2 = strFileList + "\\Config\\BlackList.txt";
	std::string strBlackListlCHK2 = strFileList + "\\Config\\BlackList.mchk";

	MakeFileCHK(strBlackListlist2.c_str(),strBlackListlCHK2.c_str());

	std::string strusbkeyinfoxml = strFileList + "\\Config\\usbkeyinfo.xml";
	std::string strusbkeyinfoCHK = strFileList + "\\Config\\usbkeyinfo.mchk";

	MakeFileCHK(strusbkeyinfoxml.c_str(),strusbkeyinfoCHK.c_str());

	std::string strBillUrlxml = strFileList + "\\Config\\BillUrl.xml";
	std::string strBillUrlCHK = strFileList + "\\Config\\BillUrl.mchk";

	MakeFileCHK(strBillUrlxml.c_str(),strBillUrlCHK.c_str());
	// 生成列表
	char szFileList[MAX_PATH + 1];
	strcpy_s(szFileList, szRootDir);
	strcat_s(szFileList, "MoneyHubList.txt");

	makeCheckChk((LPSTR)strFileList.c_str(),szFileList,"MoneyHub.mchk");


	//为各个银行生成完整性效验
	std::string strMakeCHk = strFileList;
	strMakeCHk += "\\BankInfo\\banks";
	//strFileList   = getAppDataBank();
	makeBankCheckChk(szFileList,(LPSTR)strMakeCHk.c_str(), "", NULL);

	//为Html和Config生成效验文件
	strMakeCHk =   strFileList;
	strMakeCHk +=  "\\Html";
	makeCheckChk((LPSTR)strMakeCHk.c_str(),szFileList,"html.mchk");

	strMakeCHk =   strFileList;
	strMakeCHk +=  "\\Config";
	makeCheckChk((LPSTR)strMakeCHk.c_str(),szFileList,"config.mchk");
}