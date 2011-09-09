#include "stdafx.h"
#include "info.h"
#pragma once
#define DEFAULT_INSTALL_VERSION		"1.0.0.0"
#define DEFAULT_BEGIN_VERSION		"2.0.0.0"
#define MONEYHUB_USBKEY_URL			"ukeyfiles/ukey.php"
#define  XMLUSBINFO					"<?xml version=\"1.0\" encoding=\"utf-8\"?><moneyhub><usbinfo><financeid>%s</financeid><inmodel>%s</inmodel><sys>%s</sys><ver>%s</ver></usbinfo></moneyhub>"
#define BANKID_SEPARATE_CHAR		'&'
const std::string PID_MARK_STRING = "PID_";
const std::string VID_MARK_STRING = "VID_";
const std::string MID_MARK_STRING = "MID_";

#define _SETUP_FINISH		200


class IUSBTaskInterface
{
public:
	virtual bool AddUSBTask(int vid, int pid, DWORD mid) = 0;
};

class CUSBCheckor
{
private:
	CUSBCheckor();
	~CUSBCheckor();

	static CUSBCheckor* m_Instance;
public:
	static CUSBCheckor* GetInstance();
public:
	bool CheckUSBInfo(USBHardwareInfo& hardinfo);

	bool SetSystem(SystemType ostype, int osarch)
	{
		m_osType  = ostype; m_osArchi = osarch;
		return true;
	}

	bool SetUSBTask(IUSBTaskInterface *task)
	{
		m_task = task;
		return true;
	}
private:
	bool AddRecord(USBKeyInfo* pusb, string ver, int status = 0);

	bool RemindUser(USBKeyInfo* pusb);//提示用户
	SystemType m_osType;
	int		m_osArchi;

	IUSBTaskInterface *m_task;

};