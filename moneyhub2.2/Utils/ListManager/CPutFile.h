
#pragma once
#include <string>
class CRequirement;

class CWebsiteData;

using namespace std;

class CPutFile
{

public:

	CPutFile(const CWebsiteData *pWebsiteData);

	void CheckFile(wstring name, wstring path, bool replace = false);
	void CheckExe(wstring installname, wstring name, wstring path, bool replace = false);
private:

	const CWebsiteData *m_pWebsiteData;
	void PutFile(wstring name, wstring path);
};
