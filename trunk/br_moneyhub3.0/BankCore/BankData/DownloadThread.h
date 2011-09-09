#pragma once


enum {
	ERR_STOPPED = -1,
	ERR_SUCCESS = 0,
	ERR_URLCRACKERROR = 1,
	ERR_NETWORKERROR = 2,  // 网络错误
	ERR_FILENOTFOUND = 3, 
	ERR_DISKERROR = 4, 
	ERR_FATALERROR = 5,
//	ERR_OUTOFTIME = 6, 
	ERR_UNKNOW = 7,
};


enum MY_DLTHRED_STATE
	{
	emInit = -1,
	emBegin = 0,
	emFinished = 1,
	emCancled = 2,
	};

enum MY_DOWNLOAD_KIND
{
	emOther = 0x00000000,
	emBkActiveX = 0x00000001,
	emUsbKey = 0x00000010,
	emUSBBkActiveX = 0x00000011, // 由USBKEY插入时，只下载银行控件，不下载USBKEY程序
};

enum MY_DOWNLOAD_ERR
{
	emUsbKeyDlErr,
	emUsbKeySetupErr,
	emBankCtrlDlErr,
	emBankCtrlSetupErr,
};

typedef struct _DWON_LOAD_USBKEY
{
	_DWON_LOAD_USBKEY ()
	{
	};

	int nVid; // 
	int nPid;
	int nMid;
	std::string strVersion; // 正在下载的版本

}DWON_LOAD_USBKEY, *PDWON_LOAD_USBKEY;

// 下载参数结构体
typedef struct _DOWN_LOAD_PARAM_NODE
{
	_DOWN_LOAD_PARAM_NODE ()
	{
		bSetupDlFile = false;
		emKind = emOther;
	};

	std::wstring strHWID; // 下载主机的硬件ID
	std::wstring strUrl; // 文件下载的URL链接
	std::wstring strSaveFile; // 文件下载时保存的路径
	std::wstring strSetupPath; // 文件下载后安装的路径
	std::string  strSendData;
	//LPVOID lpPostData; // 发送到服务器端的数据
	//DWORD dwPostDataLength; // 发送到数据库端的长度
	bool bSetupDlFile; // 是否下载好了的文件进行安装
	MY_DOWNLOAD_KIND emKind; // 下载的类型
	bool bCreateThread; // 是否创建一个新的线程

	DWON_LOAD_USBKEY dwUsbKeyParam;

}DOWN_LOAD_PARAM_NODE, *PDOWN_LOAD_PARAM_NODE;

// 文件下载类
class CDownloadThread
{
public:
	CDownloadThread();
	~CDownloadThread();

	void CancleDownload(void); // 用户取消下载
	//void PauseDownload(void); // 用户暂停下载
	
	void ReadDownloadPercent(UINT64& i64FileSize, UINT64& i64ReadSize); // 读取到文件下载的进度

	int GetDownloadPercent(void);

	LPCTSTR GetDownloadURL(void);

	LPCTSTR GetDownloadStorePath(void);

	LPCTSTR GetBreakFilePath(void);


	// 将正常的路径转换成断点文件路径
	static bool TranslanteToBreakDownloadName(const std::wstring& strPath, std::wstring& strDesc);
	// 将断点文件路径转换成正常的路径
	//static bool TranslanteToOriginalFileName(const std::wstring& strPath, std::wstring& strDesc);

	void DownLoadInit(LPCTSTR lpszHWID, LPCTSTR lpszUrl, LPCTSTR lpszSaveFile, LPSTR lpSendData);

	//void BeginDownloadByThread(bool bCreateThread = true); // 是否启动线程式开始下载
	int DownLoadData();

protected:
	bool IsCancled(void);

	int GetThreadState(void);
	int TransferDataGet();
	int TransferDataPost();
	//LPCTSTR GetSavePath(void);

private:
	void CloseHandles();
	void SetupDownloadFile(); // 安装文件
	UINT64  IsBreakPointFile(std::wstring wcsFile);
	int DownLoadBreakpointFile();

	//static DWORD WINAPI ThreadProc(LPVOID lpParam); // 下载线程式实体

private:
	
	std::wstring	m_strSaveFile; // 文件保存的路径
	std::wstring	m_strDownURl; // 下载文件的URL
	std::wstring	m_strHWID; // 本地计算机的硬件ID
	std::string		m_strSendData; // 要发送到服务器的数据

	bool			m_bCancle; // 是否被用户取消
	int				m_bDLThreadState; // 保存下载的状态
	
	DWORD			m_dwPostDataLength; // 要发送到服务器数据的长度

	HINTERNET		m_hInetSession; // 会话句柄
	HINTERNET		m_hInetConnection; // 连接句柄
	HINTERNET		m_hInetFile; //
	HANDLE			m_hSaveFile;

	std::wstring	m_wcsBreakFileName; // 转换后的临时文件全路径
	UINT64			m_ui64FileSize; // 要下载的文件大小
	UINT64			m_ui64TotalRead; // 总共下载的大小
	UINT			m_repeatNum; // 连接服务器失败的总次数

	//CRITICAL_SECTION m_cs; // 用来取消，暂停时和下载同步时用
	bool			m_bRetryWait; //  重试状态
	bool			m_bCreateThread;

};

class CDownloadAndSetupThread : public CDownloadThread
{
public:
	CDownloadAndSetupThread();
	~CDownloadAndSetupThread();

	void DownLoadAndSetupDlFile(PDOWN_LOAD_PARAM_NODE pDownloadParam);

	MY_DOWNLOAD_KIND GetDownloadKind (void); // 得到下载控件类型

	bool IsFinished(void);

	bool IsCancled(void);

	void CancleDownloadAndSetup(void);

public:
	DWON_LOAD_USBKEY m_dlUSBParam;

private:

	static DWORD WINAPI DLAndSetupThreadProc(LPVOID lpParam); // 下载和安装线程式实体
	bool SetupDownloadFile(MY_DOWNLOAD_KIND emKind,LPCTSTR lpPath, LPCTSTR lpSetUp); // 进行安装
	bool SetupBankControl(LPCTSTR lpPath, LPCTSTR lpSetUp); // 银行控件安装
	bool SetupBankUsbKey(LPCTSTR lpPath, LPCTSTR lpSetUp); // 银行USBKEY安装
	bool CheckDownLoadFile(LPCTSTR lpPath); // 校验下载的文件
	bool CheckDlFileAndShowErrMeg(LPCTSTR lpPath, MY_DOWNLOAD_KIND emKind);
	void SetupDlFileOrNotNeed(MY_DOWNLOAD_KIND emKind, bool bSetup);// 如果要安装则执行安装，如果不用安装则跳过

private:
	bool				m_bSetupDlFile; // 是否下载好了的文件进行安装
	bool				m_bSetupFinish; // 用来标记是否安装完成
	bool				m_bCancled;
	bool				m_bThreadExcute;
	MY_DOWNLOAD_KIND	m_emDlKind;
	std::wstring		m_strDlPath;
	std::wstring		m_strSetupPath;
	//CRITICAL_SECTION	m_cs; // 用来取消，暂停时和下载同步时用
};
