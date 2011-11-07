#pragma once
#include "../../Utils/SecurityCache/SecuCache.h"
#include <string>
#include <set>
#include <map>
#include <Message.h>
using namespace std;



class CGlobalData
{
private:
	CSecurityCache m_blackcache; // 黑名单
	CSecurityCache m_whitecache; // 白名单

	set<wstring> m_graylist;	//云查杀待查列表
	set<wstring> m_waitlist;    // 待查杀列表
	map<wstring,wstring> m_LogicToDosDevice;//记录本机器内盘符对应关系
	HANDLE m_hProcessMutex;
	HWND  m_frame;
public:
	void Init();
	void Uninit();
private:

	CGlobalData(void);
	~CGlobalData(void);

	static CGlobalData* m_Instance;
public:
	static CGlobalData* GetInstance();

public:
	void SetFramHwnd(HWND fhwnd);
	map<wstring,wstring>* GetLogicDosDeviceMap();
	set<wstring>* GetWaitList();// 云查杀待查列表为未经过微软认证需要进行云查杀的文件
	set<wstring>* GetGrayList();// 需要检测的文件，主要是从驱动获取的灰文件
	CSecurityCache* GetBlackCache();
	CSecurityCache* GetWhiteCache();

	BOOL CloseMoneyHub();// 关闭正在运行的moneyhub是否正在运行
	BOOL IsPopAlreadyRunning(); // 检测Pop是否正在运行

	void ShowCloudMessage();
	void ClearCloudMessage();
	void ShowCloudStatus();
	void NoShowCloudStatus();

//新的安全机制
private:
	std::set<std::wstring>  m_unKnowFileList;
	std::set<std::wstring>  m_whiteFileList;
	std::set<std::wstring>  m_blackFileList;
public:
	set<wstring> * GetBlackFileList();
	set<wstring> * GetWhiteFileList();
	set<wstring> * GetUnKnowFileList();
};