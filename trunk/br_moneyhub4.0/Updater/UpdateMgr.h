#pragma once

#include <string>
#include <vector>
#include "../Security/Authentication/BankMdrVerifier/export.h"
#include "..//Utils/CryptHash/base64.h"

enum UpdateMgrEvent
{
	UE_EVERYTHING_OK = 0,

	UE_PROGRESS_VALUE,

	UE_RETR_LIST,	
	UE_RETR_FILE,
	UE_RETR_SIGN,
		
	UE_DONE_LIST,				
	UE_DONE_FILE,
	UE_DONE_SIGN,

	UE_CHECK_LIST,
	UE_CHECK_LIST_FAILED,

	UE_AVAIL_YES,		
	UE_AVAIL_NO,		

	UE_CHECK_FILE,
	UE_CHECK_FILE_FAILED,

	UE_INET_ERROR,
	UE_FILE_ERROR,
	UE_FATAL_ERROR,					
};

typedef void (*UPDATEMGREVENTFUNC)(UpdateMgrEvent ev, LPCTSTR info, LPVOID lp);

class CUpdateMgr
{
	friend class CDownloadOperation;
	friend class CHttpDownloader;
	friend class CMainDlg;
public:
	CUpdateMgr();
	~CUpdateMgr();

public:
	void Start(tstring strUrl, LPVOID lpPostData, DWORD dwPostDataLength);
	void Stop();
	bool IsRunning() const;
	int checkFileValid();//安装前验证文件合法性 . 0 == 全部不合格, 1 == 部分合格, 2==全部合格
	bool isSafeFile(wchar_t * pWcsFileName);//判断该文件是否是通过验证文件
public:
	void SetEventsFunc(UPDATEMGREVENTFUNC func, LPVOID lpVoid);
	void Event(UpdateMgrEvent ume, LPCTSTR info = NULL);

public:
	UINT64 GetBytesCount() const { return m_ui64FileSize; }
	UINT64 GetBytesRead() const { return m_ui64FileRead; }

protected:
	bool GetUpdateList();		// 下载LIST
	bool CheckListFile();		// 检查LIST文件
	bool GetUpdateFiles();		// 下载数据包
	bool GetSignatureFiles();	// 下载签名
	int  CheckUpdateFiles();	// 检查数据包  0 == 验证无效，1 == 部分文件验证合格 ，2== 文件全部验证合格 
protected:
	static DWORD WINAPI _threadUpdate(LPVOID lp);
	void ErrCode2Event(int nErrCode);
	void SetProgressVal(UINT64 uSize, UINT64 uRead);
	bool ParseListContent(LPBYTE pContent, DWORD dwSize);

	std::wstring getDownLoadFilePath();
	bool getLocalFilePathName();
	
protected:
	UINT64 m_ui64FileSize;
	UINT64 m_ui64FileRead;

	DWORD m_dwStatusId;
	tstring m_strStatusMsg;

	bool m_bCriticalPack;

	tstring m_strListUrl;
	std::vector<tstring> m_vecFileUrls;
	std::vector<tstring> m_vecSignUrls;

	tstring m_strLocalList;
	std::vector<tstring> m_vecLocalFiles;
	std::vector<tstring> m_vecLocalSigns;

	std::vector<std::string>  m_checkBase64;//base64 验证码
	std::vector<std::string>  m_vecUpgradeContent;//升级内容
	bool m_bRunning;
	bool m_bNeedStop;

	LPVOID m_lpPostData;
	DWORD m_dwPostDataLength;

	UPDATEMGREVENTFUNC m_funcEvent;
	LPVOID m_lpParam;

	static  bool m_isMain;

	//通过验证文件数组
	std::vector <std::wstring> m_vecPassCheck;
};

extern CUpdateMgr _UpdateMgr;
