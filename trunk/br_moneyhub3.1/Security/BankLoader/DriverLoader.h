#pragma once

#include "windows.h"
#include <string>

#define			DRIVER_NAME				"MoneyHubPrt"
#define         DRIVER_NAMEX64          "MoneyHubPrt64"

class CDriverLoader{
	std::string	m_DriverPath;
	std::string m_CurrentPath;
	static std::string m_drivername;
	HANDLE		m_hDriver;

public:
	CDriverLoader();

	BOOL	InitializeDriver() const;

	BOOL	InstallDriver() const;
	BOOL	CheckIsDriverInstalled() const;
	BOOL	RemoveDriver() const;

	BOOL	LoadDriver() const;
	BOOL	CheckIsDriverLoaded() const;
	BOOL	UnloadDriver();

	BOOL	StartDriver();
	BOOL	StopDriver();

	HANDLE	GetDriver(bool isUnload=false);
	BOOL	CloseDriver();
	bool	CheckDriver();//检测驱动状态，最多2分钟


	static  bool    m_showMessOnce;
	static bool	CheckDriverImagePath(bool bNotSilent=true);

private:
	static void InitialDriverName();

	/**
	判断当前系统版本
	*/
	BOOL isX64() const;
};
