// BankLoader.h
#pragma once 
#include "DriverManager.h"
#include "atlstr.h"
#include <string>

class CBankLoader;

#include <vector>
using namespace std;

struct LoaderInfo
{
	CBankLoader* bankLoader;
	wstring cmdline;
};

struct ModuleItem
{
	bool bVerified;
	unsigned char md[20]; //zjun
};

typedef std::vector<CStringW> SysModuleList;
extern SysModuleList g_sysModuleNameList;

typedef std::vector<ModuleItem> ModuleList;
extern ModuleList g_moduleHashList;
extern ModuleList g_blackHashList;

extern DWORD g_dwProcessId;

//look up dll
//extern  UINT32 g_kernelProcessId;
extern  unsigned char g_kernelHash[400000] ;

class CBankLoader
{
	static DWORD WINAPI LoadProc_S(LPVOID param);
	DWORD LoadProc(LoaderInfo* loaderinfo);

	//get lsass.exe PID
	bool GetProcessId(wchar_t *p, unsigned long * pId);
	bool GetPriviledge();

private:
	bool m_bLoadFinished;

	CBankLoader();
	~CBankLoader();
	static CBankLoader* m_Instance;
public:
	CDriverManager m_DriverLoader;
	static CBankLoader* GetInstance();

	bool InstallAndStartDriver();
	bool UnInstallDriver();

	bool LoadProcess(const CString& exepath, HANDLE& hProcess, DWORD& PID);
	bool IsFinished();

	bool Safeguard();
	bool SetSecuModHashBR();

	bool SendBlackHashListToDriver();

	bool SendReferenceEvent(HANDLE& ev);//ev为输入参数，为驱动通知应用程序有灰名单文件

	bool GetGrayFile(void *pFileData, DWORD size);//pFileData为输入参数，驱动将数据放入该数据段，格式为灰名单个数(DWORD) + 文件1名(MAX_PATH(260)*sizeof(WCHAR)) + md5值( 16 ) + 文件2名 + ...,size为该数据段长度

	bool CheckDriver();//检测驱动状态的函数，最多检测2分钟
};