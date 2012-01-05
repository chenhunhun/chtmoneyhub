#pragma once
#include "ListManager.h"
#include <string>
#include <vector>
using namespace std;
class CURLList
{
private:
	CURLList(void);
	~CURLList(void);

	static CURLList* m_Instance;
public:
	static CURLList* GetInstance();

public:
	const CWebsiteData* GetData(wstring szDomain, LPCTSTR lpszUrl) const;

	WebDataMap m_WebsiteData;

	bool ReadData(const TiXmlNode *pUrlList);

	vector<int> BillFlag;
};
