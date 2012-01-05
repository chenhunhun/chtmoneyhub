#pragma once


#include "SecurityCheck.h"

enum REGSTATUS
{
	CD_ALLREADYEXISTING = 0,
	CD_SUCESSFUL,
	CD_ERROR,
};


class CDriverCommunicator
{
public:
	CDriverCommunicator(void);
	~CDriverCommunicator(void);

private:
	/**
	*得到当前路径，带命令行参数
	*/
	void getCurrentPath(LPCWSTR lPath)   const;
public:
	/**
	*检测各种信息，做好与驱动通讯前的准备工作
	*/
	void communicationDriver();

	/**
	*通过回调实现多线程同步
	*/
	static void voidFun(CheckStateEvent ev, DWORD dw, LPCTSTR lpszInfo, LPVOID lp);
	/**
	*判断是否已将白名单传送给驱动
	*/
	REGSTATUS      isSendData()       const;

	bool      isSendDataWithDriver() const;
	/**
	*设置用户态程序随OS启动
	*/
	REGSTATUS      setAutoRun()       const;

	bool           deleteAutoRun()    const;

	/**
	*与驱动通信，传递白名单
	*/
	void sendData();
	void SendBlackList();

	static bool m_isOk;
	/*
	*自动关闭提示窗口
	*/
	void show();

	/*
	*提权启动
	*/
	bool runWithUAC(BYTE index)          const;
};
