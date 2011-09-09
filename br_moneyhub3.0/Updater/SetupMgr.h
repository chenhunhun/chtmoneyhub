#pragma once

#include <vector>

struct XmlConfig
{
	tstring _sLocalPath;
	tstring _sCabFile;
};

struct XmlBinary
{
	tstring _sExec;
	bool _bIsCritical;
	tstring _sCabFile;
};

class CSetupMgr
{
public:
	CSetupMgr();

public:
	bool SetupConfig();
	bool SetupBinary();

protected:
	bool ReadXml();

protected:
	tstring m_strListXml;
	std::vector<XmlConfig> m_vecConfigs;
	std::vector<XmlBinary> m_vecBinaries;
};