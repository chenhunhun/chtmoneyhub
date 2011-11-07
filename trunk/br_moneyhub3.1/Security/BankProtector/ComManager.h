#pragma once

#include <map>
#include "atlstr.h"

struct COM_CLSID
{
	CLSID clsid;
	bool operator <(const struct COM_CLSID& clsid2) const
	{
		if(memcmp((const char*)&clsid, (const char*)&clsid2.clsid, sizeof(CLSID)) < 0)
			return true;
		return false;
	}
};

class CComManager
{
	typedef std::map<COM_CLSID, CStringA> ComList;
	ComList m_comlist;

	bool FillCLSID(COM_CLSID* clsid, const char* str) const;

	CComManager(void);
	~CComManager(void);

	static CComManager* m_Instance;
public:
	static CComManager* GetInstance();

	bool Init();
	bool Finalize();

	const char* FindComPath(const CLSID& clsid) const;
};
