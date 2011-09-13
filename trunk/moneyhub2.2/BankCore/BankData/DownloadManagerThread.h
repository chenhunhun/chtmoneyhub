#pragma once

#include <list>
#include <map>
#include "DownloadThread.h"

class CDownloadManagerThread
{
	
public:
	CDownloadManagerThread();
	~CDownloadManagerThread();

	// 添加一个下载任务
	//bool CreateDownLoadTask(LPCTSTR lpszHWID, LPCTSTR lpszUrl, LPCTSTR lpszSaveFile, LPVOID lpPostData = NULL, DWORD dwPostDataLength = 0, bool bCreateThread = true);
	bool CreateDownLoadTask(PDOWN_LOAD_PARAM_NODE pNode);
	void CancleAllDownload(); // 用户取消下载
	void PauseAllDownload(); // 用户暂停下载
//	void ContinueDownload(LPCTSTR lpszUrl); // 继续下载
	int GetAverageDownLoadProcess(); // 得到下载进度(-1连接失败，0-100之间表示进度，200正安装，300表示完成)
	
	int GetDownLoadThreadSize(); // 得到下载线程的个数

//	bool AllDownloadFinished(void); // 检验是否有已经完成的线程

	bool AllDownloadThreadExit(void); // 检验所有的线程是否已经退出

	bool HasDownloadTask(void); // 是否存在下载的子任务

	CDownloadAndSetupThread* ReadAFinishDownloadThread();

	void ReadAllDownLoadPath(std::list<std::wstring>& listStor);

	void DeleteAllDownLoadThread(void); // 删除所有线程

	//bool IsReadable(void); // 是否能读

	bool IsBankCtrlSetup(void); // 该银行的银行控件是否已经安装

	bool IsBankCtrlCancled(void); // 银行控件取消收藏

	//void SetReadState(bool bRead = true);

	bool AddBankCtrlAlready(void);

	// void TagBankCtrlSetup(void); // 将该银行标记成已经安装
private:
	CDownloadAndSetupThread* GetDLThread(LPCTSTR lpszUrl);
	
private:
	LPCRITICAL_SECTION	m_lpCS;
	std::map<CDownloadAndSetupThread*, bool> m_pDLThreadList; // bool类型用来表示对应的下载线程完成后是否已经被外界线程读取过
};