// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "Moneyhub_Agent.h"
#include "Shlwapi.h"
#include "atltime.h"
#include "../Utils/Config/HostConfig.h"
#include "../Utils/HardwareID/genhwid.h"
#include "../Utils/sn/SNManager.h"
#include "../Utils/PostData/UrlCrack.h"
//#define OWN_DEBUG 
#include <Wininet.h>
#include "Zip/zip.h"
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Shlwapi.lib")
#ifdef OWN_DEBUG 
#define DISPALY_DELAY_FULL_TIME	4
#define DISPALY_DELAY_TIME		10*1000// 应该是3小时
#else
#define DISPALY_DELAY_FULL_TIME	10800
#define DISPALY_DELAY_TIME		10*1000// 10s中查一次系统时间
#endif
#define CHECK_TIME 1000
#define MH_DELAYEVENT (0xff00)
#define MH_CHECKEVENT (0xff01)
#define MH_STARTDELAYEVENT (0xff02)

typedef BOOL (WINAPI * pChangeWindowMessageFilter)(UINT message, DWORD dwFlag);

CMainDlg::CMainDlg():
bTrackLeave(false),m_bShouldShow(false),m_bTodayShow(true),m_timer(0),m_delayflag(false),m_iCurrentPageType(eNormalPage)
{
	m_totalTipNum = 0;
	m_iPageNum = 1;
	m_iCurrentPage = 0;
	m_iTipsNum = 0;
	m_iCurrentTipsNum = 0;
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}
LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_iCurrentPageType = eNormalPage;// 默认为非设置页
	FetchData();
	ReCalculateParam();
	return 0;
}


__int64  CMainDlg::TimeDiff(SYSTEMTIME  left,SYSTEMTIME  right)  
{  
	CTime  tmLeft(left.wYear,left.wMonth,left.wDay,0,0,0);  
	CTime  tmRight(left.wYear,left.wMonth,left.wDay,0,0,0);  
	CTimeSpan  sp;  
	sp  =  tmLeft  -  tmRight;
	long  lLMinllis  =  (left.wHour*3600  +  left.wMinute*60  +  left.wSecond);  
	long  lRMinllis  =  (right.wHour*3600  +  right.wMinute*60  +  right.wSecond);  

	return  (__int64)sp.GetDays() * 86400  +  (lLMinllis  -  lRMinllis);  //此处返回秒，可用根据自己的格式需要进行转换，如时分秒
} 

int CMainDlg::PostData2Server(string hid, string sn, wstring file)
{
	wstring totalurl = CHostContainer::GetInstance()->GetHostName(kPGetFile);
	CUrlCrack url;
	if (!url.Crack(totalurl.c_str()))
		return 1000;

	HINTERNET		m_hInetSession; // 会话句柄
	HINTERNET		m_hInetConnection; // 连接句柄
	HINTERNET		m_hInetFile; //
	HANDLE			m_hSendFile;

	m_hInetSession = ::InternetOpen(L"Moneyhub3.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
	{
		return 3000;
	}
	
	DWORD dwTimeOut = 60000;
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	

	m_hInetConnection = ::InternetConnect(m_hInetSession, url.GetHostName(), url.GetPort(), NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (m_hInetConnection == NULL)
	{
		InternetCloseHandle(m_hInetSession);

		return 3001;
	}

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;
	
	USES_CONVERSION;
	m_hInetFile = HttpOpenRequestW(m_hInetConnection, _T("POST"), url.GetPath(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
	if (m_hInetFile == NULL)
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetSession);
		return 3002;
	}	

	char *pBoundary = "---------------------170081691720202";//比实际的boundary多两个--！！！

	char cfileinfo[10240] = {0};//--\nformadata和实际数据之间要有2个\r\n，否则有问题
	sprintf_s(cfileinfo, 10240, "%s\r\nContent-Disposition: form-data; name=\"hid\"\r\n\r\n%s\r\n%s\r\nContent-Disposition: form-data; name=\"sn\"\r\n\r\n%s\r\n%s\r\n\
Content-Disposition: form-data; name=\"file\"; filename=\"Feedback.zip\"\r\n\r\n",pBoundary, hid.c_str(), pBoundary, sn.c_str(), pBoundary);

	WCHAR szAppDataFileName[MAX_PATH + 1];
	ExpandEnvironmentStringsW(L"%APPDATA%\\MoneyHub\\Feedback.zip", szAppDataFileName, MAX_PATH);

	m_hSendFile = CreateFile(szAppDataFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hSendFile == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		return err;
	}

	DWORD dwLength = GetFileSize(m_hSendFile, NULL);
	unsigned char* lpBuffer = new unsigned char[dwLength + 1];

	if (lpBuffer == NULL)
	{
		return false;
	}

	DWORD dwRead = 0;
	if (!ReadFile(m_hSendFile, lpBuffer, dwLength, &dwRead, NULL))
	{
		delete[] lpBuffer;
		return false;
	}
	CloseHandle(m_hSendFile);

	DWORD dDataSize = dwRead + strlen(cfileinfo) + strlen(pBoundary) + 6;
	unsigned char* lpData = new unsigned char[dDataSize + 1];
	if(lpData == NULL)
	{
		delete[] lpBuffer;
		return false;
	}
	memset(lpData, 0, dDataSize + 1);
	unsigned char* lpCur = lpData;

	memcpy(lpData, cfileinfo, strlen(cfileinfo));
	lpCur = lpData + strlen(cfileinfo);
	memcpy(lpCur, lpBuffer, dwRead);

	delete[] lpBuffer;
	lpCur += dwRead;
	memcpy(lpCur, "\r\n", 2);	
	lpCur += 2; 
	memcpy(lpCur, pBoundary, strlen(pBoundary));	
	lpCur += strlen(pBoundary);
	memcpy(lpCur, "--\r\n", 4);
	
	TCHAR szHeaders[1024];	
	_stprintf_s(szHeaders, _countof(szHeaders), _T("Content-Type: multipart/form-data;boundary=-------------------170081691720202\r\nContent-Length:%d"), dDataSize);
	BOOL ret = HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

	HttpAddRequestHeaders(m_hInetFile, _T("User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n"), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	_stprintf_s(szHeaders, _countof(szHeaders), _T("Moneyhubuid: %s\r\n"), A2W(hid.c_str()));
	ret = HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	wstring heads = L"Connection:keep-alive\r\nUser-Agent:Moneyhub3.1\r\n";
	ret = HttpAddRequestHeadersW(m_hInetFile, heads.c_str(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	BOOL bSend = ::HttpSendRequestW(m_hInetFile, NULL, 0, (LPVOID)lpData, dDataSize);
	delete[] lpData;
	if (!bSend)
	{
		int err = GetLastError();
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);
		return err;
	}
	InternetCloseHandle(m_hInetConnection);
	InternetCloseHandle(m_hInetFile);
	InternetCloseHandle(m_hInetSession);

	return 0;

}
LRESULT CMainDlg::OnFeedBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	//获得HID和SN
	string sn = CSNManager::GetInstance()->GetSN();
	string hid = GenHWID2();
	//组装feedback的文件包
	WCHAR szAppDataPath[MAX_PATH + 1];
	ExpandEnvironmentStringsW(L"%APPDATA%\\MoneyHub\\", szAppDataPath, MAX_PATH);
	wstring dir = szAppDataPath;

	wstring tempzip = dir + L"Feedback.zip";

	HZIP hzip = CreateZip(tempzip.c_str(), NULL);

	std::wstring dirdata = dir;
	dirdata += L"Data\\*.dat";
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFileW(dirdata.c_str(), &FindFileData);
	if(INVALID_HANDLE_VALUE != hFind)
	{
		do{
			std::wstring filename = dir + L"Data\\" + FindFileData.cFileName;
			ZipAdd(hzip, FindFileData.cFileName, filename.c_str());
		}while (FindNextFileW(hFind, &FindFileData) != 0);	
		FindClose(hFind);
	}

	dirdata = dir + L"*.dmp";
	hFind = FindFirstFileW(dirdata.c_str(), &FindFileData);
	if(INVALID_HANDLE_VALUE != hFind)
	{
		do{
			std::wstring filename = dir + FindFileData.cFileName;
			ZipAdd(hzip, FindFileData.cFileName, filename.c_str());
		}while (FindNextFileW(hFind, &FindFileData) != 0);	
		FindClose(hFind);
	}

	TCHAR szPath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	wstring exepath = szPath;
	size_t lpos = exepath.find_last_of(L"\\");
	exepath = exepath.substr(0, lpos + 1);

	exepath += L"syslog.txt";
	if(PathFileExistsW(exepath.c_str()))
		ZipAdd(hzip, L"syslog.txt", exepath.c_str());

	wstring file1 = dir + L"Run.log";
	if(PathFileExistsW(file1.c_str()))
		ZipAdd(hzip, L"Run.log", file1.c_str());

	wstring file2 = dir + L"JSLog.txt";
	if(PathFileExistsW(file2.c_str()))
		ZipAdd(hzip, L"JSLog.txt", file2.c_str());

	CloseZip(hzip);
	PostData2Server(hid, sn, L"");

	if(PathFileExistsW(tempzip.c_str()))//删除因反馈而生成的临时文件
		DeleteFile (tempzip.c_str());
	return 0;
}
LRESULT CMainDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if(wParam == MH_STARTDELAYEVENT)
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"推迟显示");

		GetLocalTime(&m_lastTime);
		//延迟2分钟显示
		KillTimer(MH_STARTDELAYEVENT);
		m_iCurrentPageType = eNormalPage;// 默认为非设置页
		FetchData();
		CBankData::GetInstance()->GetSysDbObject()->close();//关闭该sysdb
		ReCalculateParam();

		CheckShow();
		ShowPop();
#ifndef OWN_DEBUG 
		AutoUpdate();
#endif

	}
	if(wParam == MH_DELAYEVENT)
	{
		bool run = false;
		if(m_Netstat == false)
		{
			m_Netstat = CheckNetState();
			if(m_Netstat == true)
			{
				run = true;
				AutoUpdate();
			}
		}
		SYSTEMTIME tmCur;
		GetLocalTime(&tmCur);

		if(TimeDiff(tmCur, m_lastTime) > DISPALY_DELAY_FULL_TIME)
		{
			GetLocalTime(&m_lastTime);//记录当前时间

			m_Netstat = CheckNetState();
			//3小时一次弹出窗口
#ifndef OWN_DEBUG
			if(!run)
			{
				AutoUpdate();
			}
#endif
			m_iCurrentPageType = eNormalPage;// 默认为非设置页
			FetchData();
			ReCalculateParam();

			CheckShow();
			ShowPop();
		}
	}
	else if(wParam == MH_CHECKEVENT)
	{
		if(!bTrackLeave)
			m_timer ++;
		if(m_timer >= 10)
		{
			KillTimer(MH_CHECKEVENT);
			m_timer = 0;
			AnimateWindow(m_hWnd, 1000, AW_BLEND | AW_HIDE); //淡出1秒
			ShowWindow(SW_HIDE);
		}
	}
	return 0;
}


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_Netstat = true;
	m_iteCurEvent[0] = NULLDATA;
	m_iteCurEvent[1] = NULLDATA;
	m_iteCurEvent[2] = NULLDATA;
#ifndef OWN_DEBUG 
	//AutoUpdate();// 开机后启动自动更新
#endif
	ModifyStyleEx(0, WS_EX_TOOLWINDOW);
	SetWindowText(L"MoneyHub_Svr_Mainframe");

	//设置所用到的字体
	int sSize = 12;
	m_TitleFont = CreateFont(sSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));
	m_CommonFont = CreateFont(sSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));
	m_CommonEngFont = CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));

	m_colorEdge.CreatePen(PS_SOLID, 1, RGB(187, 200, 209));				// 整个泡泡边框的底色画笔	
	m_colorBack = CreateSolidBrush(RGB(246, 253, 255));					// 背景色
	m_colorTipBk = CreateSolidBrush(RGB(255, 255, 255));				// 中间提示区域的背景色
	m_icoMainIcon.LoadIcon(IDI_MONEYHUB_AGENT, ICON_WIDTH, ICON_WIDTH); // 图标
	m_colorInnerEgde.CreatePen(PS_SOLID, 1, RGB(193, 211, 224)); //

	m_colorMouseMoveBk = CreateSolidBrush(RGB(248, 241, 211));	// 中间鼠标滑过的时候的颜色
	m_colorMouseMoveLine = CreatePen(PS_SOLID, 1, RGB(246, 232, 172));;  // 中间鼠标滑过时的中间区域的线的底色

	// 这里要计算总页数，目前用1代替
	m_iPageNum = 2;

	SetWindowPos(NULL,0,0, 252, 190,SWP_NOZORDER);

	CRect rc;
	GetWindowRect(&rc);

	ScreenToClient(&rc);
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, rc.Width(), rc.Height(), 4, 4);
	SetWindowRgn(hRgn, TRUE);
	DeleteObject(hRgn);

	m_imgPreButton.LoadFromFile(_T("Pop\\prev.png"));			// 前一页图片
	m_imgNextButton.LoadFromFile(_T("Pop\\next.png"));;			// 下一页
	m_imgComButton.LoadFromFile(_T("Pop\\yes_btn.png"));;		// 通用按钮，取消和确定
	m_imgCloseButton.LoadFromFile(_T("Pop\\pop_close.png"));
	m_imgCheckBox.LoadFromFile(_T("tips_checkbox.png"));

	m_btNext = GetDlgItem(IDC_NEXT);			// 下一页按钮
	m_btPre = GetDlgItem(IDC_PRE);				// 前一页按钮
	m_btOK = GetDlgItem(IDC_OK);				// 确定按钮
	m_btCancel = GetDlgItem(IDC_CANCEL);		// 取消按钮
	m_close = GetDlgItem(ID_CLOSE);

	m_ckBox[0] = GetDlgItem(IDC_CHECK1);
	m_ckBox[1] = GetDlgItem(IDC_CHECK2);
	m_ckBox[2] = GetDlgItem(IDC_CHECK3);

	m_ckBox[0].ApplySkin(&m_imgCheckBox, 8);
	m_ckBox[1].ApplySkin(&m_imgCheckBox, 8);
	m_ckBox[2].ApplySkin(&m_imgCheckBox, 8);

	m_btNext.ApplySkin(&m_imgNextButton, 4);		// 下一页按钮
	m_btPre.ApplySkin(&m_imgPreButton, 4);			// 前一页按钮
	m_btOK.ApplySkin(&m_imgComButton, 4);			// 确定按钮
	m_btCancel.ApplySkin(&m_imgComButton, 4);		// 取消按钮
	m_close.ApplySkin(&m_imgCloseButton, 3);


	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	//设置图片位置
	GetClientRect(&rc); 
	RECT wkrc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &wkrc, 0);
	int x = (wkrc.right - wkrc.left) - rc.Width() - 20;
	int y = (wkrc.bottom - wkrc.top) - rc.Height();
	SetWindowPos(HWND_TOP, x, y, 0, 0 ,SWP_NOSIZE);

	CRect rcClose;
	m_close.GetWindowRect(&rcClose);
	rcClose.SetRect(rc.Width() - rcClose.Width() - 6, 6, rc.Width() - 6, rcClose.Height() + 6);
	m_close.MoveWindow(&rcClose);

	CRect rcPre;
	m_btPre.GetWindowRect(&rcPre);
	rcPre.SetRect(6, rc.Height() - rcPre.Height() - 6, rcPre.Width() + 6, rc.Height() - 6);
	m_btPre.MoveWindow(&rcPre);

	CRect rcNext;
	m_btNext.GetWindowRect(&rcNext);
	rcNext.SetRect(75, rc.Height() - rcNext.Height() - 6, rcNext.Width() + 75, rc.Height() - 6);
	m_btNext.MoveWindow(&rcNext);

	CRect rcOk;
	m_btOK.GetWindowRect(&rcOk);
	rcOk.SetRect(rc.Width() - rcOk.Width() - 80, rc.Height() - rcOk.Height() - 6, rc.Width() - 80, rc.Height() - 6);
	m_btOK.MoveWindow(&rcOk);

	CRect rcCancel;
	m_btCancel.GetWindowRect(&rcCancel);
	rcCancel.SetRect(rc.Width() - rcCancel.Width() - 6, rc.Height() - rcCancel.Height() - 6, rc.Width() - 6, rc.Height() - 6);
	m_btCancel.MoveWindow(&rcCancel);

	m_rcSetting.top = rc.Height() - 25 - 6;
	m_rcSetting.bottom = rc.Height() - 6;
	m_rcSetting.left = 120;
	m_rcSetting.right = 120 + 30;

	m_rcNoTips.top = rc.Height() - 25 - 6;
	m_rcNoTips.bottom = rc.Height() - 6;
	m_rcNoTips.left = rc.Width() - 85;
	m_rcNoTips.right = rc.Width() - 12;
	
	m_rcNumShow.top = rc.Height() - 25 - 6;
	m_rcNumShow.bottom = rc.Height() - 6;
	m_rcNumShow.left = 31;
	m_rcNumShow.right = 75;

	m_rcTotalInfo.top = 32;
	m_rcTotalInfo.bottom = m_rcTotalInfo.top + 3 * 40 + 2;
	m_rcTotalInfo.left = 6;
	m_rcTotalInfo.right = rc.Width() - 6;

	CRect rcCheckBox[3];
	m_ckBox[0].GetWindowRect(&rcCheckBox[0]);
	rcCheckBox[0].SetRect(rc.Width() - rcCheckBox[0].Width() - 16, 32 + 12, rc.Width() - 16, 32 + rcCheckBox[0].Height() + 12);
	rcCheckBox[1].SetRect(rc.Width() - rcCheckBox[0].Width() - 16, 32 + 40 + 12, rc.Width() - 16, 32 + rcCheckBox[0].Height() + 12 + 40);
	rcCheckBox[2].SetRect(rc.Width() - rcCheckBox[0].Width() - 16, 32 + 80 + 12, rc.Width() - 16, 32 + rcCheckBox[0].Height() + 12 + 80);
	m_ckBox[0].MoveWindow(&rcCheckBox[0]);
	m_ckBox[1].MoveWindow(&rcCheckBox[1]);
	m_ckBox[2].MoveWindow(&rcCheckBox[2]);

	m_iCurrentPageType = eNormalPage;// 默认为非设置页
	//FetchData();
	//ReCalculateParam();

	//设置时间5s
	SetTimer(MH_DELAYEVENT,DISPALY_DELAY_TIME,NULL);//3小时重新检查和弹出，10s查一次系统时间

	// 第一次隐藏
	SetTimer(MH_STARTDELAYEVENT, 5 ,NULL);// 修改为立即之后开始显示
#ifndef OWN_DEBUG 
	ShowWindow(SW_HIDE);
#else

	CheckShow();
	ShowPop();
#endif
	CGlobalData::GetInstance()->SetFramHwnd(m_hWnd);

	OSVERSIONINFOEX OSVerInfo; 
	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(!GetVersionEx((OSVERSIONINFO *)&OSVerInfo)) 
	{ 
		OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
		GetVersionEx((OSVERSIONINFO *)&OSVerInfo); 
	} 

	if(OSVerInfo.dwMajorVersion >= 6) // Vista 以上 
	{ 
		HMODULE hDLL = ::LoadLibraryW(L"user32.dll");	// 动态装载组件

		if(hDLL)
		{
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"ChangeWindowMessageFilter");
			pChangeWindowMessageFilter lpChangeWindowMessageFilter = (pChangeWindowMessageFilter)::GetProcAddress( hDLL, "ChangeWindowMessageFilter");	
			// "ChangeWindowMessageFilter"函数指针
			if(lpChangeWindowMessageFilter)	
			{
				lpChangeWindowMessageFilter(WM_MONEYHUB_FEEDBACK, MSGFLT_ADD);
			}
			FreeLibrary(hDLL);
		}
	}
	m_Netstat = CheckNetState();


	return TRUE;
}

bool CMainDlg::CheckNetState()
{
	if(!(InternetCheckConnection(L"http://www.sohu.com", FLAG_ICC_FORCE_CONNECTION, 0) ||
		InternetCheckConnection(L"http://www.baidu.com", FLAG_ICC_FORCE_CONNECTION, 0) ||
		InternetCheckConnection(L"http://www.sina.com", FLAG_ICC_FORCE_CONNECTION, 0)))
	{
		return false;
	}
	return true;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// 将更改的设置结果写入到数据库中
	if(m_iCurrentPageType == eSettingPage)
	{
		if(m_iteCurEvent[0] != NULLDATA)
			m_eventcopy[m_iteCurEvent[0]].bRemind = m_ckBox[0].GetSelectedState();
		if(m_iteCurEvent[1] != NULLDATA)
			m_eventcopy[m_iteCurEvent[1]].bRemind = m_ckBox[1].GetSelectedState();
		if(m_iteCurEvent[2] != NULLDATA)
			m_eventcopy[m_iteCurEvent[2]].bRemind = m_ckBox[2].GetSelectedState();
	}

	CBankData::GetInstance()->SetUserAlarmsConfig(m_eventcopy);
	m_iCurrentPageType = eNormalPage;// 默认为非设置页
	FetchData();
	ReCalculateParam();
	return 0;
}

//处理第一次不能隐藏窗口的问题
LRESULT CMainDlg::OnNcPaint(WORD /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	static int time = 2;

	if(time > 1)
	{
		ShowWindow(SW_HIDE);
		time --;
	}
	return 0;
}
void CMainDlg::ShowPop()
{
	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0 ,SWP_NOMOVE | SWP_NOSIZE);
	if(m_bShouldShow)//需要提醒
		if(m_bTodayShow)//今天需要提醒
		{
			CRect rc,rc1;
			GetClientRect(&rc); 
			HWND   hTop = ::FindWindowExW(::GetDesktopWindow(),NULL,L"Progman",NULL);//
			int x = 0, y = 0;
			if(hTop)
			{
				HWND hTopc = ::FindWindowEx(hTop, NULL, L"SHELLDLL_DefView",NULL);
				if(hTopc)
				{
					HWND hTopb = ::FindWindowEx(hTopc, NULL, L"SysListView32",NULL);
					if(hTopb)
					{
						::GetWindowRect(hTopb,&rc1);
						if(IsVista())//Vista、Win7的位置和XP下的不一样
						{
							x = rc1.right - rc.Width() - 20;
							y = rc1.bottom - rc.Height() - 20;
						}
						else
						{
							x = rc1.right - rc.Width() - 20;
							y = rc1.bottom - rc.Height();
						}
					}
				}				
			}
			if(x == 0 || y == 0)
			{
				RECT wkrc;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &wkrc, 0);
				x = (wkrc.right - wkrc.left) - rc.Width() - 20;
				y = (wkrc.bottom - wkrc.top) - rc.Height();
			}
			::SetWindowPos(m_hWnd,NULL, x, y, 0, 0 ,SWP_NOSIZE);

			AnimateWindow(m_hWnd, 2000, AW_BLEND);// 淡入

		//	SetTimer(MH_CHECKEVENT,CHECK_TIME,NULL);
			ShowWindow(SW_SHOW);
			return;
		}
	ShowWindow(SW_HIDE);
}
void CMainDlg::CheckShow()
{
	DWORD dwType;
	DWORD dwValue;
	DWORD dwReturnBytes = sizeof(DWORD);


	m_bShouldShow = true;
	m_bTodayShow = true;
	if(m_totalTipNum <= 0)
		m_bShouldShow = false;//设置显示状态

	if (ERROR_SUCCESS == ::SHGetValueW(HKEY_CURRENT_USER, L"Software\\Bank\\Setting",L"TodayReminderEnabled", &dwType, &dwValue, &dwReturnBytes))
	{
		SYSTEMTIME sys;
		GetLocalTime( &sys );
		if((sys.wMonth * 100 + sys.wDay) == dwValue)
			m_bTodayShow = false;
	}

	LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B7}");
	HANDLE _hProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

	DWORD err = GetLastError();
	if(_hProcessMutex != NULL)
		CloseHandle(_hProcessMutex);

}


LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DeleteObject(m_TitleFont);
	DestroyWindow();
	::PostQuitMessage(nVal);
}


BOOL CMainDlg::OnEraseBkgnd(CDCHandle dc)
{
	CRect rect;
	GetClientRect(&rect);
	DrawBack(dc, rect);
	return 0;
}
void CMainDlg::ReCalculateParam()
{
	m_bOnSettingRect = false;		// 
	m_bOnNoTipsRect = false;

	m_eventcopy.clear();

	m_iteCurEvent[0] = NULLDATA;
	m_iteCurEvent[1] = NULLDATA;
	m_iteCurEvent[2] = NULLDATA;

	if(m_iCurrentPageType == eNormalPage)
	{	
		m_btOK.ShowWindow(FALSE);
		m_btCancel.ShowWindow(FALSE);
		std::vector<DATUSEREVENT>::iterator	ite = m_event.begin();//m_event;
		m_iTipsNum = 0;
		m_totalTipNum = 0;

		for(;ite != m_event.end(); ite ++)
		{
			int currentnum = 0;
			if(ite->bRemind == true)
			{				
				currentnum += ite->iNotify;
				currentnum += ite->iRemind;
				m_totalTipNum += ite->iNotify;
				m_totalTipNum += ite->iRemind;
				if(currentnum > 0)
				{
					m_iTipsNum ++;
					m_eventcopy.push_back(*ite);
				}
			}
		}
		//等于0个显示的情况没处理
		m_iPageNum = (int)((m_iTipsNum + (ONEPAGENUM - m_iTipsNum % ONEPAGENUM) % ONEPAGENUM) / ONEPAGENUM);//计算总页数
		if(m_iPageNum > 1)
		{
			m_btNext.ShowWindow(TRUE);
			m_btNext.EnableWindow(TRUE);
			m_btPre.ShowWindow(TRUE);
			m_btPre.EnableWindow(FALSE);
		}
		else
		{
			m_btNext.ShowWindow(FALSE);
			m_btPre.ShowWindow(FALSE);
		}
		m_iCurrentPage = 1;//初始化当前页为1
		RefreshData();
	}
	else if(m_iCurrentPageType == eSettingPage)
	{
		m_btOK.ShowWindow(TRUE);
		m_btCancel.ShowWindow(TRUE);
		std::vector<DATUSEREVENT>::iterator	ite = m_event.begin();//m_event;
		m_iTipsNum = 0;
		m_totalTipNum = 0;

		for(;ite != m_event.end(); ite ++)
		{
			if(ite->bRemind == true)
			{
				m_totalTipNum += ite->iNotify;
				m_totalTipNum += ite->iRemind;
			}
			m_iTipsNum ++;
			m_eventcopy.push_back(*ite);
		}
		//等于0个显示的情况没处理

		m_iPageNum = (int)((m_iTipsNum + (ONEPAGENUM - m_iTipsNum % ONEPAGENUM) % ONEPAGENUM) / ONEPAGENUM);//计算总页数
		if(m_iPageNum > 1)
		{
			m_btNext.ShowWindow(TRUE);
			m_btNext.EnableWindow(TRUE);
			m_btPre.ShowWindow(TRUE);
			m_btPre.EnableWindow(FALSE);
		}
		else
		{
			m_btNext.ShowWindow(FALSE);
			m_btPre.ShowWindow(FALSE);
		}

		m_iCurrentPage = 1;//初始化当前页为1
		RefreshData();
		
	}
	Invalidate();
}
bool Compare(DATUSEREVENT a, DATUSEREVENT b)
{
	return a.rTime < b.rTime;
}
//从数据库取数据
void CMainDlg::FetchData()
{
	m_event.clear();
	CBankData::GetInstance()->GetAllUserTodayAlarmsNumber(m_event);
	std::sort(m_event.begin(), m_event.end(), Compare);
}

void CMainDlg::DrawBack(CDCHandle dc, const RECT &rect1)
{
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc, rect1.right - rect1.left, rect1.bottom - rect1.top);
	memDC.SelectBitmap(bmp);

	CRect rect(rect1);
	rect.top -= 1;
	rect.left -= 1;

	// 画背景
	memDC.SelectBrush((HBRUSH)m_colorBack);	
	memDC.RoundRect(&rect1, CPoint(3, 3));

	// 画边框
	rect.DeflateRect(1,1);
	memDC.SelectPen((HPEN)m_colorEdge);
	memDC.RoundRect(rect, CPoint(6, 6));

	// 绘制图标
	m_icoMainIcon.DrawIconEx(memDC, 4, 4, 24, 24);

	// 绘制标题
	RECT rcTitle;
	rcTitle.top = 7;
	rcTitle.bottom = 24;
	rcTitle.left = 4 + 24 + 4;
	rcTitle.right = rcTitle.left + 150;

	memDC.SetBkMode(TRANSPARENT);
	memDC.SelectFont(m_TitleFont);
	memDC.SetTextColor(RGB(47, 132, 189));
	WCHAR wCTitle[256] = { 0 };
	swprintf_s(wCTitle, 256, L"财金汇消息(共%d条)", m_totalTipNum);
	memDC.DrawText(wCTitle, -1, &rcTitle, DT_SINGLELINE | DT_VCENTER);
	// 画中间的提示区域
	memDC.SelectPen((HPEN)m_colorInnerEgde);
	memDC.SelectBrush((HBRUSH)m_colorTipBk);
	memDC.Rectangle(m_rcTotalInfo);	

	memDC.SelectFont(m_CommonFont);
	memDC.SetTextColor(RGB(62, 99, 124));
	if(m_iPageNum > 1)
	{
		//先用测试数据显示
		WCHAR winfo[256] = {0};
		swprintf(winfo, 256, L"(%d/%d)", m_iCurrentPage, m_iPageNum);
		
		memDC.DrawText(winfo, -1, &m_rcNumShow, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}
	if(m_iCurrentPageType == eNormalPage)//是默认的设置页
	{
		//这里需要再绘按钮数据
		if(!m_bOnSettingRect)
			memDC.SetTextColor(RGB(47, 132, 189));
		else
			memDC.SetTextColor(RGB(0, 0, 255));
		memDC.DrawText(L"设置", -1, &m_rcSetting, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		if(!m_bOnNoTipsRect)
			memDC.SetTextColor(RGB(47, 132, 189));
		else
			memDC.SetTextColor(RGB(0, 0, 255));
		memDC.DrawText(L"今日不再提醒", -1, &m_rcNoTips, DT_SINGLELINE | DT_VCENTER | DT_CENTER);


		int i = 0;
		for(; i< 3; i ++)
		{
			CRect rectData(m_rcTotalInfo), rectDataTip(m_rcTotalInfo);
			if(m_iteCurEvent[i] != NULLDATA && m_iteCurEvent[i] <= m_eventcopy.size())
			{
				if(m_bMouseOn [i] == true)
				{
					CRect rectMouse(m_rcTotalInfo);

					rectMouse.top += i * 40 + 1;
					rectMouse.bottom = rectMouse.top + 40;
					rectMouse.left += 1;
					rectMouse.right -= 1;
					memDC.SelectPen((HPEN)m_colorMouseMoveLine);
					memDC.SelectBrush((HBRUSH)m_colorMouseMoveBk);
					memDC.Rectangle(rectMouse);				

				}
				
				wstring info = m_eventcopy[m_iteCurEvent[i]].account;
				memDC.SelectFont(m_CommonEngFont);
				memDC.SetTextColor(RGB(120, 133, 142));
				rectData.top += i * 40 + 1;
				rectData.bottom = rectData.top + 20;
				rectData.left += 18;
				rectData.right -= 18;
				memDC.DrawText(info.c_str(), -1, &rectData, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_WORD_ELLIPSIS);

				memDC.SelectFont(m_TitleFont);
				WCHAR winfo1[256] = {0};
				WCHAR winfo2[256] = {0};
				if(m_eventcopy[m_iteCurEvent[i]].iNotify > 0 && m_eventcopy[m_iteCurEvent[i]].iRemind > 0)
				{
					swprintf(winfo1, 256, L"%d个通知，%d个提醒", m_eventcopy[m_iteCurEvent[i]].iNotify,m_eventcopy[m_iteCurEvent[i]].iRemind);
					swprintf(winfo2, 256, L"%d　　　　%d　　　", m_eventcopy[m_iteCurEvent[i]].iNotify,m_eventcopy[m_iteCurEvent[i]].iRemind);
				}
				else if(m_eventcopy[m_iteCurEvent[i]].iNotify <= 0)
				{
					swprintf(winfo1, 256, L"%d个提醒", m_eventcopy[m_iteCurEvent[i]].iRemind);
					swprintf(winfo2, 256, L"%d　　　", m_eventcopy[m_iteCurEvent[i]].iRemind);
				}
				else if(m_eventcopy[m_iteCurEvent[i]].iRemind <= 0)
				{
					swprintf(winfo1, 256, L"%d个通知", m_eventcopy[m_iteCurEvent[i]].iNotify);
					swprintf(winfo2, 256, L"%d　　　", m_eventcopy[m_iteCurEvent[i]].iNotify);
				}

				memDC.SetTextColor(RGB(62, 99, 124));
				rectDataTip.top += i * 40 + 20 + 1;
				rectDataTip.bottom = rectDataTip.top + 20;
				rectDataTip.right = m_rcTotalInfo.right - 22;
				rectDataTip.left = m_rcTotalInfo.left + 22;
				memDC.DrawText(winfo1, -1, &rectDataTip, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
				memDC.SetTextColor(RGB(239, 105, 47));
				memDC.DrawText(winfo2, -1, &rectDataTip, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
			}

		}

	}
	else
	{
		int i = 0;
		for(; i< 3; i ++)
		{
			CRect rectData(m_rcTotalInfo), rectDataTip(m_rcTotalInfo);
			if(m_iteCurEvent[i] != NULLDATA && m_iteCurEvent[i] <= m_eventcopy.size())
			{
				wstring info = m_eventcopy[m_iteCurEvent[i]].account;
				memDC.SelectFont(m_CommonEngFont);
				memDC.SetTextColor(RGB(62, 99, 124));
				rectData.top += i * 40 + 1;
				rectData.bottom = rectData.top + 40;
				rectData.left += 18;
				rectData.right = rectData.left + 120;
				memDC.DrawText(info.c_str(), -1, &rectData, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_WORD_ELLIPSIS);

				memDC.SelectFont(m_CommonFont);
				rectDataTip.top += i * 40 + 1;
				rectDataTip.bottom = rectDataTip.top + 40;
				rectDataTip.right = m_rcTotalInfo.right - 35;
				rectDataTip.left = m_rcTotalInfo.right - 95;
				memDC.DrawText(L"允许提示", -1, &rectDataTip, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}

		}
	}

	

	dc.BitBlt(0, 0, rect1.right - rect1.left, rect1.bottom - rect1.top, memDC, 0, 0, SRCCOPY);

	::DeleteObject(&bmp);
	::DeleteObject(&memDC);
}

void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_iCurrentPageType == eNormalPage)
	{			
		m_bMouseOn[0] = false;
		m_bMouseOn[1] = false;
		m_bMouseOn[2] = false;
		if(m_rcTotalInfo.PtInRect(point))
		{
			if(point.y >= m_rcTotalInfo.top + 80 && point.y < m_rcTotalInfo.top + 120)
			{
				m_bMouseOn[2] = true;
			}
			if(point.y < m_rcTotalInfo.top + 80 && point.y >= m_rcTotalInfo.top + 40)
			{
				m_bMouseOn[1] = true;
			}
			if(point.y < m_rcTotalInfo.top + 40 && point.y >= m_rcTotalInfo.top)
			{
				m_bMouseOn[0] = true;
			}
		}
		InvalidateRect(&m_rcTotalInfo);

		if(m_rcSetting.PtInRect(point))
		{
			m_bOnSettingRect = true;
		}
		else
			m_bOnSettingRect = false;

		InvalidateRect(&m_rcSetting);

		if(m_rcNoTips.PtInRect(point))
		{
			m_bOnNoTipsRect = true;
		}
		else
			m_bOnNoTipsRect = false;

		InvalidateRect(&m_rcNoTips);			
	}
}

LRESULT CMainDlg::OnHide(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ShowWindow(SW_HIDE);
	return 0;
}
void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));

	if(m_iCurrentPageType == eNormalPage)
	{
		if(m_rcSetting.PtInRect(point))
		{
			m_iCurrentPageType = eSettingPage;
			ReCalculateParam();
		}
		else if(m_rcNoTips.PtInRect(point))
		{
			NoTips();//今日不再提醒
		}
		//还没做点击了用户
		if(m_rcTotalInfo.PtInRect(point))
		{
			wstring mail;
			bool bOpen = false;
			if((point.y >= m_rcTotalInfo.top + 80 && point.y < m_rcTotalInfo.top + 120) && m_iteCurEvent[2] != NULLDATA)
			{
				mail = m_eventcopy[m_iteCurEvent[2]].account;
				bOpen = true;
			}
			if((point.y < m_rcTotalInfo.top + 80 && point.y >= m_rcTotalInfo.top + 40) && m_iteCurEvent[1] != NULLDATA)
			{
				mail = m_eventcopy[m_iteCurEvent[1]].account;
				bOpen = true;
			}
			if((point.y < m_rcTotalInfo.top + 40 && point.y >= m_rcTotalInfo.top) && m_iteCurEvent[0] != NULLDATA)
			{
				mail = m_eventcopy[m_iteCurEvent[0]].account;
				bOpen= true;
			}

			if(bOpen)
			{
				LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B6}");//如果财金汇在运行中，那么什么也不做，只是切换页面
				HANDLE _hProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

				DWORD err = GetLastError();
				// 当内核对象已经存在或者禁止进入时，说明已经开启该进程
				if (err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED)
				{
				}
				else
				{					
					CBankData::GetInstance()->SetUserAutoLogin(mail);//如果财金汇
				}

				if(_hProcessMutex)
					::CloseHandle(_hProcessMutex);

				OnLookUp(mail);
			}
		}
	}
}
void CMainDlg::OnLookUp(wstring mail)
{
	OSVERSIONINFOEX OSVerInfo; 
	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(!GetVersionEx((OSVERSIONINFO *)&OSVerInfo)) 
	{ 
		OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
		GetVersionEx((OSVERSIONINFO *)&OSVerInfo); 
	} 
	//ShowWindow(false);
	WCHAR szPath[MAX_PATH] ={0};
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	::PathRemoveFileSpecW(szPath);

	wstring path(szPath);
	path += L"\\MoneyHub.exe";

	if(OSVerInfo.dwMajorVersion >= 6) // Vista 以上 
	{
		wstring params = L"-agent" + mail;
		SHELLEXECUTEINFOW shExecInfo ;    
		memset(&shExecInfo,0,sizeof(SHELLEXECUTEINFOW));    
		shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);    
		shExecInfo.lpVerb = L"runas";    
		shExecInfo.lpFile = path.c_str();    
		shExecInfo.lpParameters = params.c_str();    
		shExecInfo.nShow = SW_SHOW ;    
		shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;   
		shExecInfo.hInstApp = NULL;
		//::MessageBoxW(NULL, L"ShellExecuteEx", L"财金汇UAC", MB_OK);
		if(ShellExecuteEx(&shExecInfo)) 
		{ 
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"Svr启动moneyhub成功");
			if(shExecInfo.hProcess != 0)
			{
				CloseHandle(shExecInfo.hProcess);
			}	 
		} 
		else 
		{ 
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_BANK_UAC, L"Svr启动moneyhub失败");
		}

	}
	else
	{
		path +=  L" -agent" + mail;
		STARTUPINFO si;	
		PROCESS_INFORMATION pi;	
		ZeroMemory( &pi, sizeof(pi) );	
		ZeroMemory( &si, sizeof(si) );	
		si.cb = sizeof(si);	
		//带参数打开
		if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{	
			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"启动财金汇成功");
			CloseHandle( pi.hProcess );		
			CloseHandle( pi.hThread );		
		}
		else
		{
			int error = GetLastError();
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DLG_FUC, CRecordProgram::GetInstance()->GetRecordInfo(L"启动财金汇失败:%d", error));
		}
	}

	ShowWindow(SW_HIDE);

	return;
}
void CMainDlg::NoTips()
{
	DWORD data; 
	SYSTEMTIME sys;
	GetLocalTime( &sys );
	data = sys.wMonth*100+sys.wDay;

	::SHSetValueW(HKEY_CURRENT_USER, L"Software\\Bank\\Setting",L"TodayReminderEnabled",REG_DWORD, &data, sizeof(DWORD));
	m_bTodayShow = false;
	ShowWindow(SW_HIDE);

	return;
}

void CMainDlg::RefreshData()
{
	if(m_iCurrentPageType == eSettingPage)
	{
		if(m_iteCurEvent[0] != NULLDATA)
			m_eventcopy[m_iteCurEvent[0]].bRemind = m_ckBox[0].GetSelectedState();
		if(m_iteCurEvent[1] != NULLDATA)
			m_eventcopy[m_iteCurEvent[1]].bRemind = m_ckBox[1].GetSelectedState();
		if(m_iteCurEvent[2] != NULLDATA)
			m_eventcopy[m_iteCurEvent[2]].bRemind = m_ckBox[2].GetSelectedState();
	}
	m_ckBox[0].ShowWindow(FALSE);
	m_ckBox[1].ShowWindow(FALSE);
	m_ckBox[2].ShowWindow(FALSE);
	m_iteCurEvent[0] = NULLDATA;
	m_iteCurEvent[1] = NULLDATA;
	m_iteCurEvent[2] = NULLDATA;

	int left =  m_iTipsNum - (m_iCurrentPage - 1) * ONEPAGENUM; 
	if(left > ONEPAGENUM)
		m_iCurrentTipsNum = ONEPAGENUM;
	else
		m_iCurrentTipsNum = left;
	int ktemp = (m_iCurrentPage - 1) * ONEPAGENUM;
	int jtemp = 0;	

	if(m_iCurrentPageType == eNormalPage)
	{
		for(;ktemp < m_iTipsNum; ktemp ++, jtemp ++)
		{
			m_bMouseOn[ jtemp ] =  false;
			if(jtemp >= 3)
				break;
			m_iteCurEvent[jtemp] = ktemp;
		}
	}
	else
	{
		for(;ktemp < m_iTipsNum; ktemp ++, jtemp ++)
		{
			if(jtemp >= 3)
				break;
			m_iteCurEvent[jtemp] = ktemp;
			m_ckBox[jtemp].ShowWindow(TRUE);
			if(m_eventcopy[ktemp].bRemind == true)
				m_ckBox[jtemp].SetSelectedState(true);
			else
				m_ckBox[jtemp].SetSelectedState(false);
		}
	}
}
LRESULT CMainDlg::OnPre(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_iCurrentPage >= 2 && m_iCurrentPage <= m_iPageNum)
	{
		m_iCurrentPage --;
		if(m_iCurrentPage == 1)
			m_btPre.EnableWindow(FALSE);
		m_btNext.EnableWindow(TRUE);

		RefreshData();
		Invalidate();

	}
	return 0;

}
LRESULT CMainDlg::OnNext(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_iCurrentPage >= 1 && m_iCurrentPage < m_iPageNum)
	{
		m_iCurrentPage ++;
		if(m_iCurrentPage == m_iPageNum)
			m_btNext.EnableWindow(FALSE);
		m_btPre.EnableWindow(TRUE);

		RefreshData();
		Invalidate();
	}

	return 0;
}

bool CMainDlg::IsVista()
{
	OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
	BOOL bRet = ::GetVersionEx(&ovi);
	return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6));
}

void CMainDlg::AutoUpdate()
{
	BOOL isAuto = IsAutoRunUpdate();

	if(isAuto == FALSE)
	{
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"用户关闭了定时升级");
		return;
	}

	WCHAR szPath[MAX_PATH] ={0};
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	::PathRemoveFileSpecW(szPath);
	
	wstring path(szPath);
	path += L"\\Moneyhub_Updater.exe /h";

	STARTUPINFO si;	
	PROCESS_INFORMATION pi;	
	ZeroMemory( &pi, sizeof(pi) );	
	ZeroMemory( &si, sizeof(si) );	
	si.cb = sizeof(si);	
	//带参数打开	
	if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{	
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DLG_FUC, L"定时升级");
		CloseHandle( pi.hProcess );		
		CloseHandle( pi.hThread );		
	}
	else
	{
		int error = GetLastError();
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DLG_FUC, CRecordProgram::GetInstance()->GetRecordInfo(L"定时升级失败:%d", error));
	}

}	

BOOL CMainDlg::IsAutoRunUpdate()
{
	BOOL bAutoRun = FALSE;

	DWORD dwType;
	DWORD dwValue;
	DWORD dwReturnBytes = sizeof(DWORD);

	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("AutoRun"), &dwType, &dwValue, &dwReturnBytes))
	{
		bAutoRun = TRUE;
	}
	else if (dwValue == 1)
	{
		bAutoRun = TRUE;
	}

	return bAutoRun;
}
