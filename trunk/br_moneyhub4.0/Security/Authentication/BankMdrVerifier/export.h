#pragma once

#include "atlstr.h"

namespace BankMdrVerifier
{
	/**
	*Init是按路径初始化
	*InitCheck是按数据初始化
	*/
	const bool Init(const char* filename);
	const bool InitCheck(const char * hash,int hashLen);

	bool VerifyModule(CStringW filepath);

	ULONG GetBankModuleNumber();

	bool FillFileHashes(unsigned char* hashes, int maxlen);

	bool CleanUp();
};