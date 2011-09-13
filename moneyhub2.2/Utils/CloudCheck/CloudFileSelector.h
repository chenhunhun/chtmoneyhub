#pragma once
#include <set>
#include <string>
#include "Shlwapi.h"
#include <windows.h>
#pragma comment(lib, "Advapi32.lib")
using namespace std;


class CCloudFileSelector
{
public:
	CCloudFileSelector(void);
	~CCloudFileSelector(void);
private:
	set<wstring> m_cloudfile;

	wstring m_java;
	int m_sysversion;


	//wchar_t m_cloudcheckfile[MAX_PATH];
	
public:
	void AddWhiteList(int tag,wstring filename);
	void AddFolder(wstring folder);

	void GetAllFiles();
	set<wstring>* GetFiles();
	void ClearFiles();

	void AddRegFolder(wstring regflag,wstring keyname,int flag);
	void AddRegFile(wstring regflag,wstring key,wstring file,int flag);
	void AddExtensionsFile(wstring folder,wstring extensions);
private:
	//获得要查找的Java文件
	bool GetJavaInstallDirectory();
	//获得输入法文件
	bool GetInputFiles();
	//获得打印机驱动文件
	bool GetPrintDriver();
	//以下三个函数分别获得不同打印机驱动的部分
	bool GetPrintEnvironments();
	bool GetPrintMonitors();
	bool GetPrintProviders();
	bool GetValue(wchar_t* para,wchar_t* value);
	bool GetStringFiles(const wchar_t* str,int len,const wstring& dir);

	//获得Win7文件
	bool GetWinVer();

	bool GetIEFiles();


	//获得桌面主题文件
	bool GetThemeFiles();

	bool GetSharedDLLs();
	void EnumDir(wstring resToken);
};
