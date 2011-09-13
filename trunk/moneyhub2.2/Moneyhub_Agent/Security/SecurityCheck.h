#pragma once
#include "../../Utils/CloudCheck/CloudCheckor.h"
#include "../../Utils/CloudCheck/CloudFileSelector.h"
#include "../../Utils/SecurityCache/SecuCache.h"
#include <string>
#include <set>
using namespace std;


#define MSG_BUF_LEN (256)

// 在泡泡进程中，只检测与缓存和黑白名单有关的，其他函数一概无用
// 财金汇安全检测类，抽象类CProcessShow是为了显示所需
class CSecurityCheck
{
public:
	CSecurityCheck();
	~CSecurityCheck();

public:
	// 启动安全检测的函数
	void Start(int bCheckType = 0);

	// 读取系统查询文件的函数
	bool CheckSelfSysList();
	// 检测驱动状态
	//bool CheckDriverStatus();

protected:
	int VerifySysList(const char* lpCHKFileName, wchar_t *message);
	// 验证云文件的模块
	int VerifyCloudList(const char* lpCHKFileName, wchar_t *message,CCloudFileSelector& cselector);
	// 获取云查杀文件的内容并进行文件数据的函数
	void SplitCloudListContent(const CStringA& strContent,CCloudFileSelector& cselector);
	// 读取系统文件列表文件的函数
	void ReadSysList(const CStringA& strContent);

protected:
	// 正常启动2秒钟的检测
	static DWORD WINAPI _threadCheckAuto(LPVOID lp);
	// 安装时进行安全检测的线程
	static DWORD WINAPI _threadSelfCommunicate(LPVOID lp);
	// 检查安全缓存的线程
	static DWORD WINAPI _threadCacheCheck(LPVOID lp);

	// 获得本次过滤名单
	//bool GetFilterFile(list<wstring>& filterfiles);


private:
	set<wstring> m_files;
	LPVOID m_lpData;
	// 记录需要进行检测的总数量
	int m_listnumber;
	// 记录检测到的数量
	int m_nowcheck;
	// 用于在检测中同步
	CRITICAL_SECTION m_cs;
public:
	bool CheckSercurityCache(bool& isCacheExist);//检测安全缓存状态
	bool CloudCheck(set<wstring>* files,bool& flag);
	bool CheckCache();
	bool CheckBlackListCache();
	bool ReBuildSercurityCache();
};

extern CSecurityCheck _SecuCheckPop;