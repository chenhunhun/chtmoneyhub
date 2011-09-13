#include "stdafx.h"
#include "ConvertBase.h"
#include "Requirement.h"
#include "ResourceManager.h"
#include "../CryptHash/base64.h"


CRequirement::CRequirement(const TiXmlNode *pRequire, const CWebsiteData *pWebsiteData) : m_DriverData(this, pWebsiteData), m_CertFileData(this, pWebsiteData),m_PutFile(pWebsiteData)
{
	for (const TiXmlNode *pFile = pRequire->FirstChild("file"); pFile != NULL; pFile = pRequire->IterateChildren("file", pFile))
	{
		std::wstring strURL = AToW(pFile->ToElement()->Attribute("name"));
		m_FileData.AddFile(strURL.c_str());
	}

	m_strID = AToW(pRequire->ToElement()->Attribute("name"));
	std::wstring strType = AToW(pRequire->ToElement()->Attribute("type"));
	if (strType == _T("activex"))
	{
		std::wstring strReg = AToW(pRequire->ToElement()->Attribute("reg")) + _T(".reg");
		m_RegData.LoadRegData(AToW(CResourceManager::_()->GetFileContent(pWebsiteData->GetWebsiteType(), pWebsiteData->GetID(), strReg.c_str())), pWebsiteData);
		m_eRequireType = Require_ActiveX;
	}
	else if (strType == _T("driver")) // 驱动 gao
	{
		std::wstring strServiceName = AToW(pRequire->ToElement()->Attribute("service"));
		m_DriverData.SetDriverInfo(strServiceName.c_str());
		m_eRequireType = Require_Driver;
	}
	else if (strType == _T("cert")) // 证书 gao
	{
		m_CertFileData.InstallCert();
		m_eRequireType = Require_Cert;
	}
	else if (strType == _T("file"))//这个先做，需要把正常检测或需要放在固定位置的文件放到应该放的位置
	{
		std::wstring strFileName = AToW(pRequire->ToElement()->Attribute("name"));
		std::wstring strPath = AToW(pRequire->ToElement()->Attribute("path"));
		std::wstring strReplace = AToW(pRequire->ToElement()->Attribute("replace"));
		if(strReplace == L"true")
			m_PutFile.CheckFile(strFileName, strPath, true);
		else
			m_PutFile.CheckFile(strFileName, strPath);
	}
	else if (strType == _T("exe"))//这个先放着，以后再说
	{
		std::wstring strInstallName = AToW(pRequire->ToElement()->Attribute("installName"));
		std::wstring strFileName = AToW(pRequire->ToElement()->Attribute("name"));
		std::wstring strInstallPath = AToW(pRequire->ToElement()->Attribute("installPath"));
		m_PutFile.CheckExe(strInstallName, strFileName, strInstallPath);
	}
	else
		ATLASSERT(0);
}

void CRequirement::InstallRequirement()
{
	switch (m_eRequireType)
	{
		case Require_Driver:
			m_DriverData.InstallDriver();
			break;
		case Require_Cert:
			m_CertFileData.InstallCert();
			break;
	}
}
