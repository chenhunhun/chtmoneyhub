#pragma once
#include "../tinyxml/tinyxml.h"
#include "RegData.h"
#include "DriverData.h"
#include "CertFileData.h"
#include "FileData.h"
#include "CPutFile.h"

enum RequireType
{
	Require_ActiveX,
	Require_Driver,
	Require_Cert
};


class CWebsiteData;

class CRequirement
{

public:

	CRequirement(const TiXmlNode *pRequire, const CWebsiteData *pWebsiteData);

	LPCTSTR GetID() const { return m_strID.c_str(); }
	RequireType GetType() const { return m_eRequireType; }

	void InstallRequirement();

	const CRegData* GetRegData() const { ATLASSERT(m_eRequireType == Require_ActiveX); return &m_RegData; }
	const CDriverData* GetDriverData() const { ATLASSERT(m_eRequireType == Require_Driver); return &m_DriverData; }
	const CFileData* GetFileData() const { return &m_FileData; }

private:

	std::wstring m_strID;
	RequireType m_eRequireType;

	// 每个站点都有自己的虚拟注册表数据、驱动数据、证书数据和文件数据
	CRegData m_RegData;
	CDriverData m_DriverData;
	CCertFileData m_CertFileData;
	CFileData m_FileData;
	CPutFile m_PutFile;

};
