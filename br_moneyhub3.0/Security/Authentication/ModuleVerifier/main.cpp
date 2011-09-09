#include "stdafx.h"
#include "export.h"
#include "windows.h"

#include "SysModVerify.h"
#include "../BankMdrVerifier/export.h"

static CSysModVerify* g_pSysModVerify = NULL;

bool ModuleVerifier::Init()
{
	char szAuthenPath[MAX_PATH + 1];
	GetModuleFileNameA(NULL, szAuthenPath, MAX_PATH);
	*(strrchr(szAuthenPath, '\\') + 1) = 0;
	strcat_s(szAuthenPath,MAX_PATH + 1, "authen.chk");

	g_pSysModVerify = new CSysModVerify();
	//BankMdrVerifier::Init("authen.chk");
	BankMdrVerifier::Init(szAuthenPath);
	return true;
}

bool ModuleVerifier::CleanUp()
{
	delete g_pSysModVerify;

	return true;
}


bool ModuleVerifier::IsModuleVerified(const CStringW& filepath)
{
	if (g_pSysModVerify->IsModuleVerified(filepath))
		return true;
	else if (BankMdrVerifier::VerifyModule(filepath))
		return true;

	return false;
}

bool ModuleVerifier::IsSysModuleVerified(const CStringW& filepath)
{
	if (g_pSysModVerify->IsModuleVerified(filepath))
		return true;

	return false;
}

ULONG ModuleVerifier::GetBankModuleNumber()
{
	return BankMdrVerifier::GetBankModuleNumber();
}

bool ModuleVerifier::FillFileHashes(unsigned char* hashes, int maxlen)
{
	return BankMdrVerifier::FillFileHashes(hashes, maxlen);
}
