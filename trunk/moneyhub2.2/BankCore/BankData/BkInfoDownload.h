#pragma once

#include <map>
#include <string>
#include "DownloadManagerThread.h"
#include "../js/JSParam.h"
#include "../../BankData/BankData.h"




extern HWND g_hMainFrame;
class IBankDownInterface
{
public:
	IBankDownInterface(){};
	// bUpdate根据版本可以判断是否再次提示, ur usbkeyr的相关信息， bAddBkCtrl表示是触发银行控件收藏
	virtual bool CheckServerXmlFile(bool bUpdate,  USBRECORD& ur) = 0;
	//调用该接口后，返回ture表示安装成功，否则需要重新安装
	virtual bool USBFinalTest(int vid, int pid, DWORD mid, bool& bSetup) = 0;
};
enum nsDownStates
{
	nsNULL,//显示字符为空
	nsDownloading,//显示正在现在
	nsInstalling//显示正在安装
};
// 负责显示进度的类
class CNotifyFavProgress
{
public:
	CNotifyFavProgress();
	HWND m_hwndNotify;// 这里只向我的首页发送进度，所以只保留一个句柄就够了

	void SetFavProgress(string appId, int progress, nsDownStates dstate, bool allowState);
	//根据控件进度，判断显示进度
	void SetFavProgress(string appId, int progress);

	void CancelFav(string appId);//取消收藏

	void AddFav(string appId);// 增加收藏

	static CNotifyFavProgress*	m_Instance;

public:
	static CNotifyFavProgress* GetInstance();

	void SetProgressNotifyHwnd(HWND hNotify); // 设置下载进度通知的句柄
};

class CBkInfoDownloadManager
{
public:
	~CBkInfoDownloadManager(void);

	// 读取下载进度
	int ReadDownLoadPercent(LPSTR lpBankID);

	// 检验是否已经安装了(其实就是检查主机上是否已经有了该控件了)
	bool IsSetupAlready(LPSTR lpBankID);

	void CancleDownload(LPSTR lpBankID); // 用户取消下载

	void PauseDownload(LPSTR lpBankID); // 用户暂停下载

	void FinishDLBreakFile(void);
	void CheckDownloadBreakFile(void); // 检验下载路径下的所有文件，是否存在下载未完的任务


	void MyBankCtrlDownload(LPSTR lpBankID); // 下载银行控件

	void MyBankUsbKeyDownload(LPUSBRECORD pUsbNode); // 下载USBKEY // LPSTR lpBankID, LPVOID lpVoid, int nSize

	void SetCheckFun(IBankDownInterface* pObject);

	static CBkInfoDownloadManager* GetInstance(void);

	void SetProgressNotifyHwnd(HWND hNotify); // 设置下载进度通知的句柄

	void NotifyCoreBankCtrlDLFinish(LPSTR lpBankID);

	static IBankDownInterface* GetInterfaceInstance(void); // 得到接口指针

protected:

private:
	// 下载银行控件(创建新线程进行下载)
	void DownloadBankCtrl(LPSTR lpBankID,LPCTSTR lpszUrl = NULL, LPCTSTR lpszSaveFile = NULL, LPVOID lpPostData = NULL, DWORD dwPostDataLength = 0);
	CBkInfoDownloadManager();

	static DWORD WINAPI CheckThreadProc(LPVOID lpParam);
	
	bool ParseBkCtrlListContent(const char* pContent, std::list<std::wstring>& UrlList, std::string& strCheckCode); // 分析银行控件xml文件内容

	bool ParseUSBListContent(const char* pContent, std::list<std::wstring>& UrlList, std::string& strVersion, std::string& strCheck); // 分析USBxml文件内容
	
	bool CheckBankCtrlXml(LPCTSTR lpPath, std::list<std::wstring>& UrlList, std::string& strCheckCode); // 解析服务器返回的xml文件

	bool CheckUsbKeyXml(LPCTSTR lpPath, std::list<std::wstring>& UrlList, std::string& strCheckCode, std::string& strVersion); // 解析下载USBKEY时服务器返回的xml文件

	// bool CheckUSBKeyXml(LPCTSTR lpPath, std::list<std::wstring>& UrlList, std::string& strCheckCode);

	bool ConstructBkCtrlCommunicateXml(LPSTR lpBankID, std::string& info); // 构建下载BankCtrl和服务器通讯的xml文件

	bool ReadUSBCommunicateXml(const std::string& strIn, std::string& info); // 读取下载USB和服务器进行通讯的xml文件

	bool CheckCheckCode(const std::string& strCheck); // 检验校验码

	std::string UrlEncode(const std::string& src); // 对数据进行加密

	std::wstring GetTempCachePath(); // 得到默认的下载路径

	bool SetUpDownLoadFile(const std::wstring& strXmlFilePath, const std::wstring& strFilePath); // 安装已经下载好的软件

	void ReadAcquiesceSetupPath(); // 获取默认的安装路径

	int MyTwoVersionCompare(std::string& strVer1, std::string& strVer2); // 进行两个版本比较

	static DWORD WINAPI ShowUSBFinishThreadProc(LPVOID lpVoid);
	

	bool IsXmlBreakFileExistInPair(LPCTSTR lpPath, LPCTSTR lpFileName);// 检验XML文件和断点文件是否一一对应
	
	CDownloadManagerThread* FindBankCtrlDLManager(LPSTR lpBankID);

	std::map<std::string, CDownloadManagerThread*>	m_BankManager;
	std::wstring									m_wstrSetupPath; // 安装路径
	std::wstring									m_wstrDLTempPath; // 下载的临时目录 
	std::wstring									m_strHWID;
	CRITICAL_SECTION								m_cs; // 临界区
	static IBankDownInterface*						m_staticpICheckFile;
	bool											m_bAddBkCtrl; // 一个标志位,用来标志银行控件时USBKEY触发下载的，还是用户点击收藏下载的
	static CBkInfoDownloadManager*					m_staticInstance;
public:
	static DWORD WINAPI DownloadBkUSBThreadProc(LPVOID lpParam);
};