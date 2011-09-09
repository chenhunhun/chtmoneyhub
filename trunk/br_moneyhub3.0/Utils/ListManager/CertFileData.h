#pragma once
#include <string>
using namespace std;

class CRequirement;
class CWebsiteData;


class CCertFileData
{

public:

	CCertFileData(const CRequirement *pRequire, const CWebsiteData *pWebsiteData);

	bool InstallCert();


private:

	void WriteCertificate(wstring& szData);
	const CRequirement *m_pRequire;
	const CWebsiteData *m_pWebsiteData;

};
