#include "stdafx.h"
#include "info.h"
#pragma once
#include <list>
using namespace std;


class CUSBControl
{
private:
	CUSBControl();
	~CUSBControl();

	list<USBKeyInfo> m_usbinfo;

	static CUSBControl* m_Instance;
public:
	static CUSBControl* GetInstance();

	list<USBKeyInfo>* GetUSBInfo(){return &m_usbinfo;}


	void BeginUSBControl();
private:
	bool InitUSBInfo();

public:
	static DWORD WINAPI _threadUSBTest(LPVOID lp);
};