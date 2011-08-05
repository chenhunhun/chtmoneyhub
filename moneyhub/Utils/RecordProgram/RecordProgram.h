#pragma once
#include "windows.h"
#include <string>
using namespace std;

class CRecordProgram
{
public:
	CRecordProgram(void);
	~CRecordProgram(void);


public:
	static CRecordProgram * GetInstance();
	bool FeedbackError(wstring program, DWORD error, std::wstring wcsDes);

	bool RecordDebugInfo(wstring program, DWORD debug, wstring wcsDes);
	bool RecordCommonInfo(wstring program, DWORD common, wstring wcsDes);
	bool RecordWarnInfo(wstring program, DWORD warn, wstring wcsDes);

	wstring GetRecordInfo(wchar_t *format, ...);

private:
	static CRecordProgram * m_instance;
};
