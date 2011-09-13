#include "stdafx.h"
#include "ProcessManager.h"

#define PROTECT_PROCESS

#ifdef PROTECT_PROCESS
#include "../Security/BankLoader/export.h"
#endif

CProcessManager* CProcessManager::m_pProcessManager = NULL;

CProcessManager::CProcessManager()
{
}

CProcessManager::~CProcessManager()
{
}


void CProcessManager::CreateNewWebPage(HWND hChildFrame)
{
	if (::IsWindow(m_ProcessData.hAxUI))
	{
		::PostMessage(m_ProcessData.hAxUI, WM_MULTI_PROCESS_CREATE_NEW_PAGE, 0, (LPARAM)hChildFrame);
		return;
	}

	MSG msg = { NULL, WM_MULTI_PROCESS_CREATE_NEW_PAGE, 0, (LPARAM)hChildFrame };
	m_ProcessData.msgTempQueue.push(msg);
}


#ifdef SINGLE_PROCESS
DWORD WINAPI SingleProcessProc(LPVOID lParam)
{
	TCHAR szCmdLine[2048];
	_stprintf_s(szCmdLine, _T("%d"), lParam);
	extern void RunIECore(LPCTSTR szCmdLine);
	RunIECore(szCmdLine);
	return 0;
}
#endif

void CProcessManager::CreateProcess(HWND hMainFrame)
{
	if (m_ProcessData.hProcess)
	{
		DWORD dwExitCode = 0;
		::GetExitCodeProcess(m_ProcessData.hProcess, &dwExitCode);
		if (dwExitCode == STILL_ACTIVE)
			return;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	TCHAR szCmdLine[2048], szFile[MAX_PATH];
	::GetModuleFileName(NULL, szFile, _countof(szFile));
	_stprintf_s(szCmdLine, _T("\"%s\" -! %d"), szFile, hMainFrame);

#ifdef SINGLE_PROCESS
	::CreateThread(NULL, 0, SingleProcessProc, hMainFrame, 0, NULL);
#else
#ifdef PROTECT_PROCESS
	HANDLE hProcess;
	DWORD PID;
	if(BankLoader::LoadProcess(szCmdLine, hProcess, PID))
		m_ProcessData.hProcess = hProcess;
#else
	if (::CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		m_ProcessData.hProcess = pi.hProcess;
#endif
#endif
}


void CProcessManager::OnAxUICreated(HWND hAxUI)
{
	if (::IsWindow(hAxUI))
	{
		m_ProcessData.hAxUI = hAxUI;
		while (!m_ProcessData.msgTempQueue.empty())
		{
			MSG &msg = m_ProcessData.msgTempQueue.front();
			::PostMessage(m_ProcessData.hAxUI, msg.message, msg.wParam, msg.lParam);
			m_ProcessData.msgTempQueue.pop();
		}

	}
}

//////////////////////////////////////////////////////////////////////////

CProcessManager* CProcessManager::_()
{
	if (NULL == m_pProcessManager)
		m_pProcessManager = new CProcessManager();

	return m_pProcessManager;
}


void  CProcessManager::SetFilterId(UINT32 id)
{
	BankLoader::SendProtectId(id);
}