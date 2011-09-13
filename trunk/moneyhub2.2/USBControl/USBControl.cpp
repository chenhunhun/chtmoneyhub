#include "stdafx.h"
#include "USBControl.h"
#include "USBDevice.h"
#include "USBInfoFile.h"
#pragma once

CUSBControl* CUSBControl::m_Instance = NULL;

CUSBControl* CUSBControl::GetInstance()
{
	if(m_Instance == NULL)
		m_Instance = new CUSBControl();
	return m_Instance;
}

CUSBControl::CUSBControl()
{
}
CUSBControl::~CUSBControl()
{
}

bool CUSBControl::InitUSBInfo()
{
	//从文件获得usb的信息
	CUSBInfoFileManager::GetInstance()->InitUSBManegerList(m_usbinfo);
	return true;
}

void CUSBControl::BeginUSBControl()
{
	InitUSBInfo();
	DWORD dwThreadID;
	::CloseHandle(::CreateThread(NULL, 0, _threadUSBTest, NULL, NULL, &dwThreadID));
}

DWORD WINAPI CUSBControl::_threadUSBTest(LPVOID lp)
{
	DWORD threadid = ::GetCurrentThreadId();
	CAxUSBControl axui;


	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_ID_TESTUSB, L"启动了USB监控线程");

	// 由于要获得usb的设备更新消息，所以这里要注册为顶层窗口
	if(axui.Create(NULL, 0 , L"Moneyhub_USBKEY_Test", WS_OVERLAPPEDWINDOW) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	// 消息循环，等待新的usb插入的消息
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return 0;
}