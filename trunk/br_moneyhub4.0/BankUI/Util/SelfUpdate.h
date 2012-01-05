#pragma once

//////////////////////////////////////////////////////////////////////////
// CCheckForUpdate class

class CCheckForUpdate
{
protected:
	CCheckForUpdate(HWND hParent);
	bool IsChecking(bool bAuto);

public:
	bool Check(bool bAuto);

	static void ClearInstallFlag();

public:
	static CCheckForUpdate* CreateInstance(HWND hParent);
	static int AddRef();
	static void Release();

protected:
	HWND m_hParentWnd;
	static int m_nAddRef;
};


//////////////////////////////////////////////////////////////////////////
// CInstallUpdatePack class

class CInstallUpdatePack
{
public:
	CInstallUpdatePack();

public:
	/*
	检查是否有下载完毕需要更新的安装包
	*/
	bool Check();
	void Setup();

	/*
	检查注册表，是否用户将下载模式更改为每次启动财金汇主程序启动updata
	*/
	bool setupUpdateWithMH();
protected:
	bool m_bNeedRun;

#ifdef _UNICODE
	std::wstring m_strCmdline;
#else
	std::string m_strCmdline;
#endif

	//static HANDLE m_hUpdateRunMutex;
};
