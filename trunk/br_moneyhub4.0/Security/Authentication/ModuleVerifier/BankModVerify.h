#pragma once

#include <map>
#include <set>
#include "../encryption/md5.h"
#include "atlstr.h"

/*
struct VerifyInfo
{
	unsigned char md5[MD5_HASH_SIZE];

	bool operator <(const VerifyInfo& vi) const
	{
		return memcmp(md5, vi.md5, MD5_HASH_SIZE) < 0;
	}
};

class CBankModVerify
{
	typedef std::set<VerifyInfo> ModuleHash;
	ModuleHash m_moduleHash;
	ModuleHash::const_iterator m_curr_it;

	bool ReadCHK();

public:
	CBankModVerify(void);
	~CBankModVerify(void);

	bool VerifyModule(CStringW filepath) const;
	ULONG GetBankModuleNumber() const;
	bool FillFileHashes(unsigned char* hashes, int maxlen) const;
};
*/