#pragma once

class CGetOSInfo
{
public:
	CGetOSInfo();
	~CGetOSInfo();

public:
	static CGetOSInfo* m_hIns;
	static CGetOSInfo* getInstance();

public:
	BOOL isX64();
};