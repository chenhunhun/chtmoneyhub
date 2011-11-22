#pragma once

typedef std::map<std::wstring, std::wstring>  VMMAPDEF;

enum MONEYHUBVERSION
{
	ALLVERSION =0,
	MAINVERSION =1,
	BANKVERSION,
	OTHERVERSION,
};

class versionManager
{
public:
	versionManager(void);
	~versionManager(void);

public:
	static versionManager * m_instance;
	static versionManager * getHinstance();
	/**
	*    获取程序版本号，index标示想要获取的何种模块的版本信息。
	*/
	 bool getAllVersion(VMMAPDEF& mapVersion, MONEYHUBVERSION index, bool bEnName = true);

private:
	 void getMainModuleVersion(VMMAPDEF& mapVersion);
	 void getBankModuleVersion(VMMAPDEF& mapVersion);
	 void getOtherModuleVersion(VMMAPDEF& mapVersion);
private:
	 std::wstring getModulePath();

	 bool  traverseFile(LPWSTR path, LPWSTR wName, VMMAPDEF& mapVersion );
	 bool getVersionFromChk(std::wstring& vs , LPCSTR lpCHKFileName, std::wstring& szBankName);
	 bool chkToXml(LPCSTR lpChkFileName , LPSTR  lpContentXml, UINT *pLen);

private:
	 bool m_bEnName;
	 VMMAPDEF m_mapEnChName;
};
