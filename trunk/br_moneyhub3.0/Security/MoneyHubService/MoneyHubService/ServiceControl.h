#pragma once
#include <WTypes.h>

class CServiceControl
{
public:
	
	//设置服务名称以及服务描述
	CServiceControl(LPCTSTR lpServiceName,LPCTSTR lpServieDescription);
	virtual ~CServiceControl(void);

public:
	// 安装服务
	bool Install(void);
	// 启动服务
	bool Start(void);
	// 停止服务
	bool Stop(void);
	// 卸载服务
	bool UnInstall(void);
	// 查询服务状态
	DWORD QueryStatus(void);


private:
	// 判断是否安装了服务
	TCHAR m_tzServiceName[MAX_PATH];
	TCHAR m_tzLogPathName[MAX_PATH];
	bool IsInstall(void);
private:
	TCHAR m_tzServieDescription[MAX_PATH];
};
