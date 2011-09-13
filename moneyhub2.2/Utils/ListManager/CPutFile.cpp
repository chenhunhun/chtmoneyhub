#include "stdafx.h"
#include "CPutFile.h"
#include "Windows.h"
#include "ResourceManager.h"
#include "../RecordProgram/RecordProgram.h"
CPutFile::CPutFile(const CWebsiteData *pWebsiteData):m_pWebsiteData(pWebsiteData)
{
}


//void CPutFile::AddFile(wstring name, wstring path)
//{
//	
//}
//replace为强制替换标记，如果规定目录下的文件
void CPutFile::CheckFile(wstring name, wstring path, bool replace)
{
	wstring fullPath;
	fullPath = path + L"\\" + name;

	WCHAR expName[MAX_PATH] ={0};
	ExpandEnvironmentStringsW(fullPath.c_str(), expName, MAX_PATH);

	if(::PathFileExistsW(expName) == FALSE)// 判断文件是否存在
	{
		wstring file = CResourceManager::_()->GetFilePath(m_pWebsiteData->GetWebsiteType(), m_pWebsiteData->GetID(), name.c_str());

		::CopyFileW(file.c_str(), expName , TRUE);
		
		DWORD re = ::GetLastError();
		if(re == ERROR_ACCESS_DENIED)
		{	
			USES_CONVERSION;
			string appid;
			if(m_pWebsiteData)
			{
				USES_CONVERSION;
				appid = CFavBankOperator::GetBankIDOrBankName(W2A(m_pWebsiteData->GetID()),false);
				CWebsiteData::StartUAC(A2W(appid.c_str()));
			}
		}
	}
	else
	{
		if(replace == true)//强制替换
		{
			wstring file = CResourceManager::_()->GetFilePath(m_pWebsiteData->GetWebsiteType(), m_pWebsiteData->GetID(), name.c_str());

			::CopyFileW(file.c_str(), expName , FALSE);//
			DWORD re = ::GetLastError();
			if(re == ERROR_ACCESS_DENIED)
			{
				USES_CONVERSION;
				string appid;
				if(m_pWebsiteData)
				{
					USES_CONVERSION;
					appid = CFavBankOperator::GetBankIDOrBankName(W2A(m_pWebsiteData->GetID()),false);
					CWebsiteData::StartUAC(A2W(appid.c_str()));
				}
			}
		}
	}
	return;
}
void CPutFile::CheckExe(wstring installname, wstring name, wstring path, bool replace)
{
	return;
}
