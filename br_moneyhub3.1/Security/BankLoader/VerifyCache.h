#pragma once

#include "atlstr.h"

class CVerifyCache
{
	bool CreateDirectoryRecursively(const CStringW& dirpath) const;

	CStringW tranlatePath(const CStringW& filepath) const;
public:
	CVerifyCache(void);
	~CVerifyCache(void);

	bool IsFileVerified(const CStringW& filepath, const void* sha, int shalen, bool& bVerified) const;
	bool CreateCache(const CStringW& filepath, const void* sha, int shalen, bool bVerified) const;
};
