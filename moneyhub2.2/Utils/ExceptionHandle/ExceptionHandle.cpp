#include "StdAfx.h"
#include "ExceptionHandle.h"
#include <eh.h>
#include"../RunLog/RunLog.h"
#include "../UserBehavior/UserBehavior.h"

bool CExceptionHandle::ms_bIsSetTanslator = false;
void CExceptionHandle::MapSEtoCE()
{
	if(!ms_bIsSetTanslator)
	{
		_set_se_translator(TranslateSEtoCE);
		ms_bIsSetTanslator = true;    
	}

}

CExceptionHandle::operator DWORD()
{
	return (m_er.ExceptionCode);
}


CExceptionHandle::CExceptionHandle(PEXCEPTION_POINTERS pep)
{
	memset (m_pThreadName, 0, nThread_Name_Len);
//	m_dwThreadID = GetCurrentThreadId();
	m_er = *pep->ExceptionRecord;
	m_context = *pep->ContextRecord;
}

void CExceptionHandle::TranslateSEtoCE(UINT dwEC,PEXCEPTION_POINTERS pep)
{
	throw CExceptionHandle(pep);
}

void CExceptionHandle::SetThreadName(const char* pName) // 设置线程的名称
{
	memset (m_pThreadName, 0, nThread_Name_Len);
	int nLen = strlen (pName);
	if (nLen > nThread_Name_Len)
		nLen = nThread_Name_Len;
	memcpy (m_pThreadName, pName, nLen);
}

bool CExceptionHandle::RecordException()
{
	/*std::string strErr;
	TranslateExceptionAndSend (strErr);*/

	// 转换成宽字节
	std::wstring strTN;
	strTN = CA2W (m_pThreadName);

	CString strTemp;
	strTemp.Format (L"ExceptionThreadName = %s, ExceptionCode = %d, ExceptionFlags = %d", strTN.c_str (), m_er.ExceptionCode, m_er.ExceptionFlags);
	
	// 错误代码写入到本地日志中
	CRunLog::GetInstance ()->GetLog ()->WriteSysLog (LOG_TYPE_ERROR, (wchar_t*)(LPCTSTR)strTemp);

	// 错误代码写入到服务器端
	
	CString strErrcode;
	strErrcode.Format(L"%d", m_er.ExceptionCode);

	std::string strErrCode = CW2A(strErrcode);
	std::string strErr = CW2A(strTemp);
	CUserBehavior::GetInstance()->Action_SendErrorInfo(strErrCode, strErr);

	return true;
}

bool CExceptionHandle::TranslateExceptionAndSend(std::string& strErr)
{
	switch (m_er.ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		{
			break;
		}
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		{
			break;
		}
	case EXCEPTION_BREAKPOINT:
		{
			break;
		}
	case EXCEPTION_SINGLE_STEP:
		{
			break;
		}
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		{
			break;
		}
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		{
			break;
		}
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		{
			break;
		}
	case EXCEPTION_FLT_INEXACT_RESULT:
		{
			break;
		}
	case EXCEPTION_FLT_INVALID_OPERATION:
		{
			break;
		}
	case EXCEPTION_FLT_OVERFLOW:
		{
			break;
		}
	case EXCEPTION_FLT_STACK_CHECK:
		{
			break;
		}
	case EXCEPTION_FLT_UNDERFLOW:
		{
			break;
		}
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		{
			break;
		}
	case EXCEPTION_INT_OVERFLOW:
		{
			break;
		}
	case EXCEPTION_PRIV_INSTRUCTION:
		{
			break;
		}
	case EXCEPTION_IN_PAGE_ERROR:
		{
			break;
		}
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		{
			break;
		}
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		{
			break;
		}
	case EXCEPTION_STACK_OVERFLOW:
		{
			break;
		}
	case EXCEPTION_INVALID_DISPOSITION:
		{
			break;
		}
	case EXCEPTION_GUARD_PAGE:
		{
			break;
		}
	case EXCEPTION_INVALID_HANDLE:
		{
			break;
		}
	//case EXCEPTION_POSSIBLE_DEADLOCK:
		/*{
			break;
		}*/
	default:
		break;
	}
	return true;
}