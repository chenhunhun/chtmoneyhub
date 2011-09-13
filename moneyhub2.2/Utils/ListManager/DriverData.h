#pragma once

class CRequirement;
class CWebsiteData;
#include "Windows.h"

class CDriverData
{

public:

	CDriverData(const CRequirement *pRequire, const CWebsiteData *pWebsiteData);

	void SetDriverInfo(LPCTSTR lpszServiceName);

	bool CheckDriverIsWorking() const;
	bool InstallDriver();

private:

	const CRequirement *m_pRequire;
	const CWebsiteData *m_pWebsiteData;

	std::wstring m_strServiceName;
};
