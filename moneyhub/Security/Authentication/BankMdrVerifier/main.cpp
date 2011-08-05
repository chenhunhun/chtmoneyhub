#include "stdafx.h"
#include "export.h"
#include "BankModVerify.h"

CBankModVerify* g_BankModVeriy = NULL;

const bool BankMdrVerifier::Init(const char* filename)
{
	if(!g_BankModVeriy)
		g_BankModVeriy = new CBankModVerify();
	return g_BankModVeriy->Init(filename);
}

const bool BankMdrVerifier::InitCheck(const char * hash,int hashLen)
{
	if(!g_BankModVeriy)
		g_BankModVeriy = new CBankModVerify();
	return g_BankModVeriy->initCheck(hash,hashLen);
}

bool BankMdrVerifier::VerifyModule(CStringW filepath)
{
	return g_BankModVeriy->VerifyModule(filepath);
}

ULONG BankMdrVerifier::GetBankModuleNumber()
{
	return g_BankModVeriy->GetBankModuleNumber();
}

bool BankMdrVerifier::FillFileHashes(unsigned char* hashes, int maxlen)
{
	return g_BankModVeriy->FillFileHashes(hashes, maxlen);
}

bool BankMdrVerifier::CleanUp()
{
	delete g_BankModVeriy;
	g_BankModVeriy = NULL;
	return true;
}
