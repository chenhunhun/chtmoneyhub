// EncryptJSFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <windows.h>
#include <iostream>
#include "../../Utils/SecurityCache/comm.h"

void EncrypJsFileData(const char* lpszCHKFile, unsigned char * buffer, int length)
{
	unsigned char *total;
	int totalLength;

	totalLength = length + 128;
	total = (unsigned char *)malloc (totalLength);

	if (total != NULL)
	{
		FILE *fp;

		totalLength = JSFilePack(buffer,length,total);

		fp = fopen(lpszCHKFile, "wb");
		fwrite(total,1,totalLength,fp);
		fclose(fp);

		free(total);
	}
}

void EncrypJsFile(const char* lpszFile,const char* lpszCHKFile)
{
	printf("%s\n", lpszFile); 
	unsigned char * buffer;
	HANDLE hFile = CreateFileA(lpszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "failed to open .mhs file" << std::endl;
		return;
	}

	DWORD dwLen = GetFileSize(hFile, NULL);
	DWORD dwRead = 0;
	buffer = new unsigned char[dwLen];
	if (!ReadFile(hFile, buffer, dwLen, &dwRead, NULL))
	{
		std::cout << "failed to read .mhs file" << std::endl;

		CloseHandle(hFile);

		return;
	}

	CloseHandle(hFile);
	EncrypJsFileData(lpszCHKFile, buffer, dwLen);
	delete []buffer;
}

// 遍历JS文件
void TraversalPath(const char* lpszPath,const char * lpszFilePath, FILE *fp)
{
	std::string dir = lpszPath;
	dir += "\\*.*";

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFileA(dir.c_str(), &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE)
		return;
	do{
		std::string fn = FindFileData.cFileName;

		if ((strcmp(fn.c_str(), ".") != 0) && (strcmp(fn.c_str(), "..") != 0) && (strcmp(fn.c_str(), ".svn") != 0))
		{
			//为每个单独升级模块制作xxx.mchk完整性效验文件
			/*if( isSpecial(FindFileData.cFileName) )
				continue;*/

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
				// 如果是.mhs后缀的文件，进行加密
				if (fn.find(".mhs") == fn.length() - 4)
				{
					/*std::string subFile = lpszPath;
					subFile = subFile + "\\" + fn;*/
					std::string filePath = lpszPath;
					filePath = filePath + "\\" + fn;
					std::string strTemp = filePath + "tp";

					EncrypJsFile(filePath.c_str(), strTemp.c_str());
					DeleteFileA(filePath.c_str());
					MoveFileA(strTemp.c_str(), filePath.c_str());

					
				}

			}
		}
	}while (FindNextFileA(hFind, &FindFileData) != 0);


	FindClose(hFind);
}

#define HTMLFILEPATH "\\Html"
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		std::cout << "Please input root dir!" << std::endl;
		return 0;
	}

	std::cout << "start Encrypt .mhs file..." << std::endl;
	std::string strFileList = argv[1];//..Release
	

    //为各个银行生成相应的info.mchk文件
	std::string strPath = strFileList;
	strPath += HTMLFILEPATH;
	

	FILE * fp = fopen(strPath.c_str(), "w");
	TraversalPath(strPath.c_str(), "", fp);
	//fclose(fp);

	//getchar();

	return 0;
}

