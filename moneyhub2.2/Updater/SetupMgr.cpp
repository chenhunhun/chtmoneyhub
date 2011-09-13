
#include "stdafx.h"
#include "SetupMgr.h"
#include "../utils/tinyxml/tinyxml.h"

CSetupMgr::CSetupMgr()
{
	m_strListXml = _TempCachePath + _T("\\list.xml");
}

bool CSetupMgr::SetupConfig()
{
	return true;
}

bool CSetupMgr::SetupBinary()
{
	return true;
}

bool CSetupMgr::ReadXml()
{

	return true;
}