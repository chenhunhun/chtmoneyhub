#pragma once
#include "ListManager.h"

class CURLList
{
private:
	CURLList(void);
	~CURLList(void);

	static CURLList* m_Instance;
public:
	static CURLList* GetInstance();

public:
	const CWebsiteData* GetData(LPCTSTR /*lpszDomain*/, LPCTSTR lpszUrl) const;

	WebDataMap m_WebsiteData;

	bool ReadData(const TiXmlNode *pUrlList);
};
