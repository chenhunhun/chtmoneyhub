#pragma once

class CRequirement;
class CWebsiteData;
#include "Windows.h"

class CDriverData
{

public:

	CDriverData(const CRequirement *pRequire, const CWebsiteData *pWebsiteData);

	void SetDriverInfo(LPCTSTR lpszServiceName,DWORD rstart=1,DWORD rtype=1);

	bool CheckDriverIsWorking(DWORD rstart,DWORD rtype) const;
	bool InstallDriver(DWORD rstart,DWORD rtype);

private:

	const CRequirement *m_pRequire;
	const CWebsiteData *m_pWebsiteData;

	std::wstring m_strServiceName;
};
