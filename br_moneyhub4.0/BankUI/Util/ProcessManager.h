#pragma once


struct ProcessData
{
	HWND hAxUI;
	std::queue<MSG> msgTempQueue;
	HANDLE hProcess;

	ProcessData() : hAxUI(NULL), hProcess(NULL) {}
	~ProcessData()
	{
		if (hProcess)
			::CloseHandle(hProcess);
	}
};



class CProcessManager
{

public:

	CProcessManager();
	~CProcessManager();

	void CreateNewWebPage(HWND hChildFrame);
	void CreateProcess(HWND hMainFrame);

	void OnAxUICreated(HWND hAxUI);

	ProcessData m_ProcessData;

	void SetFilterId(UINT32 id);
	static CProcessManager* _();
private:
	static CProcessManager* m_pProcessManager; // gao save
};
