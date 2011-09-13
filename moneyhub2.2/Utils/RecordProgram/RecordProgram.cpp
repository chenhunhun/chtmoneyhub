#include "stdafx.h"
#include "RecordProgram.h"
#include "..\RunLog\RunLog.h"
#include "..\UserBehavior\UserBehavior.h"

CRecordProgram::CRecordProgram()
{

}

CRecordProgram::~CRecordProgram()
{

}


CRecordProgram * CRecordProgram::m_instance = NULL;

CRecordProgram* CRecordProgram::GetInstance()
{
	if( NULL == m_instance )
		m_instance = new CRecordProgram();

	return m_instance;
}


bool CRecordProgram::FeedbackError(wstring program, DWORD error, std::wstring wcsDes)
{
	bool bRet = true;

	wchar_t cinfo[20]= { 0 };
	swprintf(cinfo, 20, L"0x%08x", error);
	wstring wscTmp(cinfo);
	wscTmp  = program + L"-" + wscTmp + L"-" + wcsDes;

	USES_CONVERSION;
	CRunLog::GetInstance ()->GetLog ()->WriteSysLog (LOG_TYPE_ERROR, L"%ws", wscTmp.c_str() );
	CUserBehavior::GetInstance()->Action_SendErrorInfo( W2A(cinfo), W2A(wscTmp.c_str()) );

	return bRet;
}

bool CRecordProgram::RecordDebugInfo(wstring program, DWORD debug, wstring wcsDes)
{
	wchar_t cinfo[20]= { 0 };
	swprintf(cinfo, 20, L"0x%08x", debug);
	wstring wscTmp(cinfo);
	wscTmp  = program + L"-" + wscTmp + L"-" + wcsDes;

	CRunLog::GetInstance ()->GetLog ()->WriteSysLog (LOG_TYPE_DEBUG, L"%ws", wscTmp.c_str());
	return true;
}
bool CRecordProgram::RecordCommonInfo(wstring program, DWORD common, wstring wcsDes)
{
	wchar_t cinfo[20]= { 0 };
	swprintf(cinfo, 20, L"0x%08x", common);
	wstring wscTmp(cinfo);
	wscTmp  = program + L"-" + wscTmp + L"-" + wcsDes;

	CRunLog::GetInstance ()->GetLog ()->WriteSysLog (LOG_TYPE_INFO, L"%ws", wscTmp.c_str());
	return true;
}
bool CRecordProgram::RecordWarnInfo(wstring program, DWORD warn, wstring wcsDes)
{
	wchar_t cinfo[20]= { 0 };
	swprintf(cinfo, 20, L"0x%08x", warn);
	wstring wscTmp(cinfo);
	wscTmp  = program + L"-" + wscTmp + L"-" + wcsDes;

	CRunLog::GetInstance ()->GetLog ()->WriteSysLog (LOG_TYPE_WARN, L"%ws", wscTmp.c_str());
	return true;
}

wstring CRecordProgram::GetRecordInfo(wchar_t *format, ...)
{
	wchar_t strTemp[MAX_INFO_LENGTH];
	memset(strTemp, 0, sizeof(strTemp));
	wchar_t *pTemp = strTemp;
	//合成信息
	va_list args; 
	va_start(args,format); 
	vswprintf(pTemp,MAX_INFO_LENGTH,format,args); 
	va_end(args); 

	wstring stemp(strTemp);

	return stemp;
}