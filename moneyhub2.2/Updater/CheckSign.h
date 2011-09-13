#pragma once

#include <vector>
#include "../Security/Authentication/BankMdrVerifier/export.h"

//#pragma comment(lib, "psapi.lib")

class CCheckSign
{
public:
	CCheckSign(const std::vector<tstring>& vecSign);
	~CCheckSign();

public:
	bool IsInitOK() const { return m_bIsInitOK; }
	bool CheckFile(LPCTSTR lpszFile);

private:
	bool m_bIsInitOK;
};

bool CheckSignature(const std::vector<tstring>& vecSign, const std::vector<tstring>& vecFile);