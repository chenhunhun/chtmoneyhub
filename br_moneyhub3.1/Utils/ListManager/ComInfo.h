#pragma once
#include <string>

using namespace std;

class CComInfo
{

public:

	CComInfo();

	void SetClassId(wstring classid);
	bool CheckCom();
private:
	
	GUID  m_classid;
	wstring m_strclassid;
};
