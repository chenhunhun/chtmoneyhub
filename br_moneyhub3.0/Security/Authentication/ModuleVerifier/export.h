#pragma once

#include "atlstr.h"

namespace ModuleVerifier
{
	bool Init();
	bool CleanUp();
	bool IsModuleVerified(const CStringW& filepath);
	bool IsSysModuleVerified(const CStringW& filepath);

	ULONG GetBankModuleNumber();
	bool FillFileHashes(unsigned char* hashes, int maxlen);
};