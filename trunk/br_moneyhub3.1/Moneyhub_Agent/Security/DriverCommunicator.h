#pragma once
#include <set>
#include <string>
using namespace std;

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

public:
	/**
	*检测各种信息，做好与驱动通讯前的准备工作
	*/
	void communicationDriver();

	/**
	*判断是否已将白名单传送给驱动
	*/
	REGSTATUS      isSendData()       const;

	bool      isSendDataWithDriver() const;


	/**
	*与驱动通信，传递白名单
	*/
	void sendData();
	void SendBlackList();

	bool CheckDriver();
	// 发送事件句柄给驱动
	bool SendReferenceEvent(HANDLE& ev);

	// 获得灰名单
	bool GetGrayFile(set<wstring>& file);//暂时只用文件名，不用md5

	static bool m_isOk;

	//反馈查询结果给内核
	bool SetKernelContinue(bool b);

};
