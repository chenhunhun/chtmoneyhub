// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "FileVersionInfo.h"
#include "BankProcessMgr.h"
#include "Skin/CoolMessageBox.h"
#include <algorithm>
//#include "Updater.h"
#include "..\Utils\CryptHash\base64.h"
#include "Updater.h"
#include "DownloadOperation.h"
#include "..\Utils\Config\HostConfig.h"



#pragma comment(lib, "crypt32.lib")



CMainDlg::CMainDlg()
: m_bDownloadOK(false)
{
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetWindowText(_T("财金汇在线升级"));
	SetDlgItemText(IDC_HIDE, _T("隐藏"));
	SetDlgItemText(IDCANCEL, _T("取消"));

	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	m_ctlMessage = GetDlgItem(IDC_MESSAGE);
	m_ctlProgress = GetDlgItem(IDC_PROGRESS);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);


//	UINT pnIcons[] = { IDR_MAINFRAME, };
//	_TrayIconMgr.Create(m_hWnd, pnIcons, sizeof(pnIcons) / sizeof(UINT), WM_TRAYICON);

	//////////////////////////////////////////////////////////////////////////
	// start update manager

	m_ctlProgress.SetRange(0, 100);

	if (!_UpdateMgr.IsRunning())
		Start();	

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
//	_TrayIconMgr.Remove();

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnTrayIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	switch (lParam)
	{
	case WM_MOUSEMOVE:
		break;

	case WM_LBUTTONDOWN:
		if (!m_bDownloadOK)
		{
			if (IsIconic())
				ShowWindow(SW_RESTORE);
			else
				ShowWindow(SW_SHOW);
		}

		SetForegroundWindow(m_hWnd);	

		break;

	case WM_RBUTTONUP:
		break;

	case NIN_BALLOONUSERCLICK:
	case NIN_BALLOONTIMEOUT:
	case NIN_BALLOONHIDE:
		break;
	}

	return 0;
}

LRESULT CMainDlg::OnTaskbarCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
//	_TrayIconMgr.TestIcon();
	bHandled = FALSE;

	return 0;
}

LRESULT CMainDlg::OnDownloadOK(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bDownloadOK = true;

	SetInstallFlag(true);

	BOOL bIsVisible = IsWindowVisible();
	ShowWindow(SW_HIDE);
	SetForegroundWindow(m_hWnd);

//	_TrayIconMgr.Remove();
	// WaitForSingleObject
	while (_UpdateMgr.IsRunning())
		Sleep(100);

	//验证升级数据的合法性，如无效，则进行删除
	CDownloadOperation dop;
	int index = dop.isValid();

	//部分或者全部升级数据通过验证
	if( index )
	{
		feedbackForUpdate();//升级数据反馈

		CBankProcessMgr BankMgr;
		std::vector<HANDLE> vecProcess;
		int nProcessNum = BankMgr.GetProcList(_T("MoneyHub.exe"), vecProcess);
		if (nProcessNum > 0)
		{
			if (_UpdateMgr.m_bCriticalPack)
			{
				tstring strMsgBoxTitle = _T("财金汇安装更新");
				tstring strMsgBoxContent = _T("目前有一个关键更新需要升级，是否立即升级？");
				if (IDYES == mhMessageBox(m_hWnd, strMsgBoxContent.c_str(), strMsgBoxTitle.c_str(), MB_YESNO | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST))
					InstallPack();
			}
		}
	}
 	else
	{
		dop.deleteAllUpdataFile();
		deleteAllReg();
	}

	CloseDialog(0);

	return 0;
}

LRESULT CMainDlg::OnDownloadStop(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	Stop();
	SetDlgItemText(IDCANCEL, _T("确认"));
	::EnableWindow(GetDlgItem(IDC_HIDE), FALSE);
	m_ctlProgress.ShowWindow(SW_HIDE);

	if (!IsWindowVisible())
		OnCancel();

	return 0;
}

LRESULT CMainDlg::OnHide(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	//CloseDialog(wID);
	ShowWindow(SW_HIDE);
	
	return 0;
}

LRESULT CMainDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	OnCancel();

	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OnCancel();

	return 0;
}



void CMainDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
	Stop();
	CloseDialog(0);
}

void CMainDlg::CloseDialog(int nVal)
{
//	_TrayIconMgr.Remove();

	DestroyWindow();
	::PostQuitMessage(nVal);
}

void CMainDlg::Start()
{
	std::string strLocalData;
	if (!GetLocalInfo(strLocalData))	
	{
		ShowMessage(_T("客户端错误"));
		//收集信息错误，反馈
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_NAME, MY_ERROR_ID_GETLOCALINFO, MY_ERROR_DESCRIPT_GETLOCALINFO);
		return;
	}

	_UpdateMgr.SetEventsFunc(_UpdateMgrCallback, this);
	std::wstring url = CHostContainer::GetInstance()->GetHostName(kDownload) + _UpdateUrl;
	_UpdateMgr.Start(url.c_str(), (LPVOID)strLocalData.c_str(), strLocalData.size());
}

void CMainDlg::Stop()
{
	_UpdateMgr.SetEventsFunc(NULL, 0);
	_UpdateMgr.Stop();
}



// 将用户本机上程序及用户收藏了的银行相关驱动程序的版本号
bool CMainDlg::GetLocalInfo(std::string& info)
{
	USES_CONVERSION;

	LPSTR szXmlInfoPattern = "<?xml version=\"1.0\" encoding=\"utf-8\"?><moneyhub><modules>";//<module><name>Main</name><version>%s</version></module> //</modules></moneyhub>;
	std::string strVersion = "";

	TCHAR szBankPath[1000];
	_tcscpy_s(szBankPath, _Module.GetModulePath());
	_tcscat_s(szBankPath, _T("\\MoneyHub.exe"));

	CFileVersionInfo vinfo;
	if (vinfo.Create(szBankPath))
	{
		tstring strFileVersion = vinfo.GetFileVersion();
		replace(strFileVersion.begin(), strFileVersion.end(), ',', '.');
		strFileVersion.erase(remove(strFileVersion.begin(), strFileVersion.end(), ' '), strFileVersion.end());
		strVersion = std::string(CT2A(strFileVersion.c_str()));
	}
	else 
		return false;
	
	std::string strName = "Main";
	g_oldVersion.insert(make_pair(strName,strVersion) );

	CDownloadOperation cdo;
	char buf[10000] = {0};

	 if( false == cdo.getXMLInfo(buf,sizeof(buf),(LPSTR)strVersion.c_str(), NULL) )//组装上传xml文件信息
		 return false;

	std::string  allXmlStr =szXmlInfoPattern;
	allXmlStr += buf;
	allXmlStr += "</modules></moneyhub>";
	
/////////////////////////////////////////////////////////////////////////
	DWORD dwBufLen = strlen(allXmlStr.c_str()) + 32;
	LPSTR lpszXmlInfo = new char[dwBufLen];
	//sprintf_s(lpszXmlInfo, dwBufLen, szXmlInfoPattern, strVersion.c_str());
	strcpy_s(lpszXmlInfo,dwBufLen,allXmlStr.c_str());

	int dwSize = strlen(lpszXmlInfo) * 2 + 1;
	unsigned char* pszOut = new unsigned char[dwSize];
	base64_encode((LPBYTE)lpszXmlInfo, strlen(lpszXmlInfo), pszOut, &dwSize);
	pszOut[dwSize] = 0;

	
	info = "xml=";
	info += UrlEncode((char *)pszOut);

	delete []pszOut;
	delete []lpszXmlInfo;

	return true;
}


void CMainDlg::_UpdateMgrCallback(UpdateMgrEvent ev, LPCTSTR info, LPVOID lp)
{
	CMainDlg* pThis = (CMainDlg *)lp;
	bool bNeedStop = false;
	bool bRetryNext = false;

	switch (ev)
	{
	case UE_EVERYTHING_OK:
		pThis->PostMessage(WM_DOWNLOADOK);
		break;

	case UE_PROGRESS_VALUE:
		pThis->SetProgressPos((DWORD)info);
		break;

	case UE_RETR_LIST:
		pThis->ShowMessage(_T("正在检查最新版本软件，请稍候..."));
		break;

	case UE_DONE_LIST:
		break;

	case UE_CHECK_LIST:
		break;

	case UE_CHECK_LIST_FAILED:
		pThis->ShowMessage(_T("升级服务器出错，请稍后再试"));// win7
		bNeedStop = true;
		break;

	case UE_AVAIL_YES:
		break;

	case UE_AVAIL_NO:
		if (info)
			pThis->ShowMessage(info);
		else
 			pThis->ShowMessage(_T("已经是最新的版本了"));
		bNeedStop = true;
		break;

	case UE_RETR_FILE:
		pThis->SetProgressPos(0);
		pThis->m_ctlProgress.ShowWindow(SW_SHOW);
		pThis->ShowMessage(_T("正在下载最新版本安装包..."));
		break;

	case UE_DONE_FILE:
		pThis->SetProgressPos(100);
		break;

	case UE_RETR_SIGN:
		pThis->SetProgressPos(0);
		pThis->m_ctlProgress.ShowWindow(SW_SHOW);
		pThis->ShowMessage(_T("正在下载安装包指纹信息..."));
		break;

	case UE_DONE_SIGN:
		pThis->SetProgressPos(100);
		break;

	case UE_CHECK_FILE:
		pThis->ShowMessage(_T("正在检查安装包的完整性..."));
		break;

	case UE_CHECK_FILE_FAILED:
		pThis->ShowMessage(_T("安装包出错，请稍后再试"));
		bNeedStop = true;
		bRetryNext = true;
		break;

	case UE_INET_ERROR:
		pThis->ShowMessage(_T("连接服务器失败，请检查网络连接"));
		bNeedStop = true;
		bRetryNext = true;
		break;

	case UE_FILE_ERROR:
		pThis->ShowMessage(_T("升级服务暂时不可用，请稍后再试"));
		bNeedStop = true;
		bRetryNext = true;
		break;

	case UE_FATAL_ERROR:
		pThis->ShowMessage(_T("内部错误，请稍后再试"));
		bNeedStop = true;
		bRetryNext = true;
		break;
	}

	if (bNeedStop)
		pThis->PostMessage(WM_DOWNLOADSTOP);

// 	if (bRetryNext && _Module.m_hOuterParent)
// 		::PostMessage(_Module.m_hOuterParent, WM_UPDATERETRY, 0, 0);
}

void CMainDlg::ShowMessage(tstring strMsg)
{
	ShowMessage(strMsg.c_str());
}

void CMainDlg::ShowMessage(LPCTSTR lpszMsg)
{
	m_ctlMessage.SetWindowText(lpszMsg);
}

void CMainDlg::SetProgressPos(int nPos)
{
	if (nPos < 0) 
		nPos = 0;
	else if (nPos > 100)
		nPos = 100;

	if (IsWindow() && m_ctlProgress.IsWindow())
	{
		::PostMessage(m_hWnd, WM_SETPROGRESSPOS, nPos, 0);

		//m_ctlProgress.SetPos(nPos);
	}
}

// true 安装成功
// false 安装中断
void CMainDlg::InstallPack()
{	
	int nMsgBoxRet = IDYES;

	CBankProcessMgr BankMgr;
	std::vector<HANDLE> vecProcess;
	int nProcessNum = BankMgr.GetProcList(_T("MoneyHub.exe"), vecProcess);

	if (nMsgBoxRet == IDYES)
	{
		if (nProcessNum > 0)
		{
			::PostMessage(HWND_BROADCAST, WM_BROADCAST_QUIT, 0, 0);
			Sleep(1000);
		}

		// 重新再扫描一遍
		if (BankMgr.GetProcList(_T("MoneyHub.exe"), vecProcess) > 0)
			BankMgr.TermProcList(vecProcess);


		DWORD dwType;
		TCHAR szCmdLine[1024];
		DWORD dwReturnBytes = sizeof(szCmdLine);

		if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), 
			_T("InstallPack"), &dwType, szCmdLine, &dwReturnBytes)
			|| dwType != REG_SZ)
			return;

		CDownloadOperation don;

		don.uncompressFile();
		Sleep(1000);

		if(  _UpdateMgr.isSafeFile(szCmdLine ) && _tcscmp(_T("modules"),szCmdLine) != 0  )
		{
			tstring strExeName = _T("\"");
			strExeName += _UpdateMgr.m_vecLocalFiles[0];
			strExeName += _T("\"");

			ShellExecute(GetDesktopWindow(), _T("open"), (LPTSTR)strExeName.c_str(), L"/grade", NULL, SW_SHOWNORMAL);
		}
		else
		{
			deleteAllReg();
			don.deleteAllUpdataFile();
		}
	}
}

// 下次安装
void CMainDlg::SetInstallFlag(bool bSet)
{	
	DWORD dwUpdate = bSet ? 1 : 0;
	::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("InstallFlag"), REG_DWORD, &dwUpdate, sizeof(DWORD));

	tstring strCommandLine = _T("");
	if (bSet && _UpdateMgr.m_isMain)
	{
		strCommandLine = _T("\"");
		strCommandLine += _UpdateMgr.m_vecLocalFiles[0];
		strCommandLine += _T("\"");
		//strCommandLine += _T(" /upgrade");
		OutputDebugStringW(strCommandLine.c_str());
	}

	DWORD dwLength = (strCommandLine.size() + 1) * sizeof(TCHAR);

	//主模块升级
	if(true == _UpdateMgr.m_isMain)
		::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("InstallPack"), REG_SZ, strCommandLine.c_str(), dwLength);
	//银行模块单独升级
	else
	{
	     strCommandLine =L"modules";
		 dwLength = (strCommandLine.size() + 1) * sizeof(TCHAR);
		::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("InstallPack"), REG_SZ, strCommandLine.c_str(), dwLength);
	}

}

std::string CMainDlg::UrlEncode(const std::string& src)   
{   
    static char hex[] = "0123456789ABCDEF";   
    std::string dst;   
  
    for (size_t i = 0; i < src.size(); i++)   
    {   
        unsigned char ch = src[i];   
        if (isalnum(ch))   
        {   
            dst += ch;   
        }   
        else  
            if (src[i] == ' ')   
            {   
                dst += '+';   
            }   
            else  
            {   
                unsigned char c = static_cast<unsigned char>(src[i]);   
                dst += '%';   
                dst += hex[c / 16];   
                dst += hex[c % 16];   
            }   
    }   
    return dst;   
}   

LRESULT CMainDlg::OnSetProgressPos(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_ctlProgress.SetPos(wParam);

	return 0;
}
