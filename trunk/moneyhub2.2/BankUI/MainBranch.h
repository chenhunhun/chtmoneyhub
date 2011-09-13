/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  main.cpp
 *      说明：  主进程分类执行及功能类的头文件
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#pragma once
#include "Windows.h"
#include <string>
#include <list>
#include <map>
using namespace std;

class CMainBranch
{
public:
	CMainBranch(void);
	~CMainBranch(void);
private:
	UINT_PTR  m_returnTimer;
public:
	// 判断MoneyHub正在运行并切换到其界面的函数
	BOOL	IsAlreadyRunning();
	// 关闭在IsAlreadyRunning中开启的内核对象，要在调用IsAlreadyRunning后退出程序时调用
	void	CloseHandle();
	// 检测访问自身进程权限的句柄
	bool	CheckToken();
	// 获得当前界面显示位置
	bool	GetFramePos(int& nShowWindow,RECT& rcWnd,DWORD& dwMax);
	// 检测是否显示向导
	void	CheckGuide(HWND& hFrame);	

public:
	// 开启内核进程
	void	RunIECoreProcess(LPCTSTR lpstrCmdLine);
	// 卸载流程
	int		UnInstall();
	// 安装流程
	int		Install();
	// 设置显示首页
	bool	PopSetPage();
	// 检测IE kernel进程
	bool	CheckIECoreProcess();
	// 关闭ie进程的函数
	bool	TerminateIECore();

	bool InitManagerList();

	bool RunUAC(LPCTSTR lpstrCmdLine);

	bool Shell(LPCTSTR lpstrCmdLine);

public:
	// 升级检测
	bool	UpdateCheck();
	// 安全检测
	bool	SecurityCheck();

	// 安装时的检测
	int		InstallCheck();

	bool	CheckPop(bool bCehck = true);

	bool	IsPopAlreadyRunning();


public:
	// 检测挂钩用的函数
	static	VOID CALLBACK CheckHookProc(HWND hwnd , UINT uMsg , UINT_PTR idEvent , DWORD dwTime);
	// 开启定时检测挂钩和注册表的函数
	void	StartMonitor();
	// 关闭检测
	void	StopMonitor();
private:
	// 将wstring转换为string类型的函数
	string ws2s(const wstring& ws);

	DWORD GetIEVersion();
};

extern CMainBranch g_AppBranch;
