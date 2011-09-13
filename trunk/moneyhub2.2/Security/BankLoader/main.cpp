#include "stdafx.h"
#include "export.h"
#include "BankLoader.h"

bool BankLoader::LoadProcess(const CString& cmdline, HANDLE& hProcess, DWORD& PID)
{
	return CBankLoader::GetInstance()->LoadProcess(cmdline, hProcess, PID);
}

bool BankLoader::IsFinished()
{
	return CBankLoader::GetInstance()->IsFinished();
}

bool BankLoader::InstallAndStartDriver()
{
	return CBankLoader::GetInstance()->InstallAndStartDriver();
}

bool BankLoader::UnInstallDriver()
{
	return CBankLoader::GetInstance()->UnInstallDriver();
}


bool BankLoader::setSecuModuleBR()
{
	return CBankLoader::GetInstance()->setSecuModHashBR();
}

int BankLoader::returnDriverStatus(MYDRIVERSTATUS type)
{
	return CBankLoader::GetInstance()->m_DriverLoader.returnDriverStatus(type);
}

bool BankLoader::checkHook()
{
	return CBankLoader::GetInstance()->m_DriverLoader.checkHookSafe();
}
// 增加发送黑名单
bool BankLoader::SendBlackListToDriver()
{
	return CBankLoader::GetInstance()->SendBlackHashListToDriver();
}

bool BankLoader::SendReferenceEvent(HANDLE& ev)
{
	return CBankLoader::GetInstance()->SendReferenceEvent(ev);
}

bool BankLoader::GetGrayFile(void *pFileData, DWORD size)
{
	return CBankLoader::GetInstance()->GetGrayFile(pFileData, size);
}

bool BankLoader::CheckDriver()//单独为pop定义的检测驱动状态的函数
{
	return CBankLoader::GetInstance()->CheckDriver();
}


bool BankLoader::SendProtectId(UINT32 id)
{
	return CBankLoader::GetInstance()->m_DriverLoader.SetFilterProcessID(id);
}