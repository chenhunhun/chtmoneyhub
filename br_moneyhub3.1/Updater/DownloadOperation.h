#pragma once

#include <string>
#include <vector>

#define  BANKINFOPATH        L"BankInfo\\banks\\*.*"
#define  BANKINFOPATHNOFIND  L"BankInfo\\banks\\"
#define  XMLMODULEINFO       "<module><name>%s</name><version>%s</version></module>"

typedef enum 
{
	BLACKUPDATE =0,
	WHITEUPDATE,
	ALL,
	NONE

}WBRETURN;

class CDownloadOperation
{
public:
	CDownloadOperation(void);
	~CDownloadOperation(void);
private:
	/**
	*   all path 
	*/
	std::wstring getModulePath();
	std::wstring getAppDataPath();
	/*
	bChoose = true;		%temp%
	bChoose = false;    %appdata%
	*/
	std::wstring  getBankCachePath(bool bChoose = false);


	void  deleteDirectoryW(LPWSTR path);
	void  uncompressTraverse(LPWSTR path , BYTE bIndex,LPWSTR parentDirectory = NULL);

	/**
	*验证升级数据文件是否包含黑白名单缓存，并效验缓存文件是否有变化，减少用户等待时间
	*/
	WBRETURN IsNewTBCacheFile(wchar_t * pFileName);
	bool     UpdateWBFile(WBRETURN wb);
public:
	/**
	*解压缩下载模块文件到安装目录
	*/
	void   uncompressFile();
	/**
	*删除下载的升级文件
	*/
	void deleteAllUpdataFile(wchar_t* wcsPath = NULL);

	bool   getXMLInfo(LPSTR content,DWORD dwlen,LPSTR version,LPSTR moduleName = NULL);

	/**
	*   检查下载后的文件的合法性，如被破坏掉，删除  . return : 0 == 全部不合格,1 ==部分合格 ,2 ==全部合格
	*/
	int isValid();

	bool isSpecialCab(wchar_t * wcsP);

	void ShowMessage();

	/**
	*  数据升级前删除安装目录中升级部分文件，保存到临时文件夹内，以便升级失败后还原
	*/
	void moveFileTraverse(IN LPWSTR path, IN  LPWSTR wcsExsitingFile, IN  LPWSTR wcsNewFile);
	void moveFiles(LPWSTR wcsFileName, bool bDirection);
private:
	std::vector <std::wstring> m_vecSpecialDName;
	std::vector <std::wstring> m_vecWBFileName;
};
