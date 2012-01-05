#pragma once
#include <Windows.h>
using namespace std;

const int nThread_Name_Len = 100;
class CExceptionHandle
{
public :
	static void MapSEtoCE();

	operator DWORD();

	bool RecordException(); // 记录异常

	void SetThreadName(const char* pName); // 设置线程的名称

private:

	bool TranslateExceptionAndSend(std::string& strErr); // 将异常转换成内容
	CExceptionHandle(PEXCEPTION_POINTERS pep);

	static void __cdecl TranslateSEtoCE(UINT dwEC,PEXCEPTION_POINTERS pep);

private:
	static bool ms_bIsSetTanslator;
	EXCEPTION_RECORD m_er;
	CONTEXT m_context;
	char m_pThreadName[nThread_Name_Len];
};