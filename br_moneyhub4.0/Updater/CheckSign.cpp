#include "stdafx.h"
#include "CheckSign.h"

CCheckSign::CCheckSign(const std::vector<tstring>& vecSign)
{
	m_bIsInitOK = false;

	if (vecSign.size() == 0)		
		return;

	for (size_t i = 0; i < vecSign.size(); i++)
	{
		m_bIsInitOK = BankMdrVerifier::Init(CT2A(vecSign[i].c_str()));

		// 留给以后多文件情况...
		break;
	}
}

CCheckSign::~CCheckSign()
{
	BankMdrVerifier::CleanUp();
}

bool CCheckSign::CheckFile(LPCTSTR lpszFile)
{
#ifdef _UNICODE
	CStringW str(lpszFile);
	return BankMdrVerifier::VerifyModule(str);
#else
#error "why use ansi???"
	USES_CONVERSION;
	std::wstring wstr(CT2W(lpszFile));
	CStringW str = wstring.c_str();
	return BankMdrVerifier::VerifyModule(str);
#endif
}


// 外部接口API, Hash验证
bool CheckSignature(const std::vector<tstring>& vecSign, const std::vector<tstring>& vecFile)
{
	CCheckSign sign(vecSign);

	if (!sign.IsInitOK())
	{
		return false;
	}
	for (size_t i = 0; i < vecFile.size(); i++)
	{
		if (!sign.CheckFile(vecFile[i].c_str()))
			return false;
	}

	return true;
}
