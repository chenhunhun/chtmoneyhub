#include "stdafx.h"
#include "ComInfo.h"
#include "Windows.h"

#include "../RecordProgram/RecordProgram.h"

#pragma comment(lib,"Kernel32.lib") 
CComInfo::CComInfo()
{
}
void CComInfo::SetClassId(wstring classid)
{
	::CoInitialize( NULL );		// COM 初始化
	m_strclassid = L"{" + classid + L"}";

	CLSIDFromString((LPOLESTR)m_strclassid.c_str(), &m_classid);

	::CoUninitialize();
}
bool CComInfo::CheckCom()
{		
	::CoInitialize( NULL );		// COM 初始化	
	HRESULT hr;
	int time = 0;
	for(;time < 3 ;time ++ )
	{
		CComPtr < IUnknown > spUnk;	// 由 CLSID 启动组件，并取得 IUnknown 指针
		hr = ::CoCreateInstance( m_classid, NULL, CLSCTX_ALL, IID_IUnknown, (LPVOID *)&spUnk );

		if(hr != S_OK)
		{
			wstring info = m_strclassid + L" Create Failed!";
			CRecordProgram::GetInstance()->RecordCommonInfo(L"BankCom", hr , info.c_str());
			Sleep(500);
		}
		else
		{
			wstring info = m_strclassid + L" Create Success!";
			CRecordProgram::GetInstance()->RecordCommonInfo(L"BankCom", 2000 , info.c_str());
			break;
		}

	}
	::CoUninitialize();
	return hr != S_OK ? false : true;
}
