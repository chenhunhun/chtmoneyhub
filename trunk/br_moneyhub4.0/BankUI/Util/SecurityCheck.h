#pragma once
#include "../../Utils/CloudCheck/CloudCheckor.h"
#include "../../Utils/CloudCheck/CloudFileSelector.h"
#include "../../Utils/SecurityCache/SecuCache.h"
#include <string>
#include <set>
using namespace std;


#define MSG_BUF_LEN (256)

enum CheckStateEvent
{
	CHK_ERROR = -2,
	CHK_START = 0,
	CHK_SELFMODULES,
	CHK_SELFDATAFILES,
	CHK_SELFURLLIST,
	CHK_SELFSYSLIST,
	CHK_SYSLISTHASH,
	CHK_DRVIMAGEPATH,
	CHK_WINSXSDLL,
	CHK_CLOUDLISTHASH,
	CHK_SECURITYCHECK,
	CHK_END,
};

typedef void (*CHECKEVENTFUNC)(CheckStateEvent ev, DWORD dw, LPCTSTR lpszInfo, LPVOID lp);
// 财金汇安全检测类，抽象类CProcessShow是为了显示所需
class CSecurityCheck: public CProcessShow
{
public:
	CSecurityCheck();

public:
	// 启动安全检测的函数
	void Start(int bCheckType = 0);
	// 对自身模块进行检测的函数
	bool CheckSelfModules();
	// 对自身数据文件进行检测的函数
	bool CheckSelfDataFiles(bool bCheckBank = true, bool bThreadCheck = false);
	// 检测URL白名单文件的函数
	bool CheckSelfUrlList();
	// 读取系统查询文件的函数
	bool CheckSelfSysList();
	// 检测驱动状态
	bool CheckDriverStatus();
	// 独立检测gdi对象，看是否存在gdi版本过旧的状况
	bool ExtraCheckGdiplus();

	int VerifyMoneyHubList(const char *path, const char* lpCHKFileName, wchar_t *message);

protected:
	// 验证银行控件完整性
	bool  CheckBankDataFiles(LPSTR path,wchar_t * message);

	// 根据验证文件验证自身模块模块
	int VerifySelfModules(const char* lpCHKFileName, wchar_t *message);
	// 
	int CheckMoneyHubList(unsigned char *buffer, int length, const wchar_t *path, wchar_t *message);
	
	int VerifyUrlList(const char* lpCHKFileName, wchar_t *message);
	int VerifySysList(const char* lpCHKFileName, wchar_t *message);
	// 验证云文件的模块
	int VerifyCloudList(const char* lpCHKFileName, wchar_t *message,CCloudFileSelector& cselector);
	// 重载CProcessShow的函数
	void Update(int i);
	// 获取云查杀文件的内容并进行文件数据的函数
	void SplitCloudListContent(const CStringA& strContent,CCloudFileSelector& cselector);
	// 读取系统文件列表文件的函数
	void ReadSysList(const CStringA& strContent);




public:
	void SetEventsFunc(CHECKEVENTFUNC pfn, LPVOID lpData);

protected:
	// 向安全检测窗口发送检测进度事件的函数
	void Event(CheckStateEvent cse, DWORD prog_or_error, const wchar_t* lpszInfo, const char* lpszErrNum);
	// 正常启动2秒钟的检测
	static DWORD WINAPI _threadCheck(LPVOID lp);
	// 安装时进行安全检测的线程
	static DWORD WINAPI _threadInstallCheck(LPVOID lp);
	// 进行缓存检查的线程
	static DWORD WINAPI _threadCacheCheck(LPVOID lp);

private:
	set<wstring> m_files;
	CHECKEVENTFUNC m_fnCheckEventFunc;
	LPVOID m_lpData;
	// 下面2个是辅助进行安全检测显示进度的记录变量
	static int ProcessPos;
	static int ProcessHelp;//辅助参数

	// 记录需要进行检测的总数量
	int m_listnumber;
	// 记录检测到的数量
	int m_nowcheck;
	// 用于在检测中同步
	CRITICAL_SECTION m_cs;
public:
	bool CheckWhiteListCache();// 生成白名单
	bool CheckBlackListCache();// 生成黑名单
	bool CloudCheck(set<wstring>* files,bool& flag);
	bool CheckCache();
};

extern CSecurityCheck _SecuCheck;