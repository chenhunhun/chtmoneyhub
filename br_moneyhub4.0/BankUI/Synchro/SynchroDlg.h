
#pragma once
#include "../../BankUI/Resource/resource.h"
#include "../BankCore/DownloadManagerImpl.h"
#include "../../BankUI/UIControl/MainFrame.h"
#include "../../BankUI/UIControl/CoolMessageBox.h"
#include "../../BankData\BankData.h"
#include <wininet.h>
//#include <PrSht.h>
//#include <CryptuiAPI.h>
#include "../../Utils/ListManager/ListManager.h"

#include "../UIControl/BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

class CSynchroDlg : public CDialogImpl<CSynchroDlg>, public CDialogSkinMixer<CSynchroDlg>
{
public:
	CSynchroDlg(/*ThreadDataStruct* pTds*/int m_status = 0);
	~CSynchroDlg();

public:
	enum { IDD = IDD_DIALOG_SYNCHRO };
	enum { CREATEHCONNECT, CREATEHREQUEST };

	BEGIN_MSG_MAP(CSynchroDlg)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnBnClickedOk)
		CHAIN_MSG_MAP(CDialogSkinMixer<CSynchroDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_URLDOWNLOADFROMURL_STATUS, OnDownloadStatus)
		MESSAGE_HANDLER(WM_URLDOWNLOADFROMURL_FINISH, OnDownloadFinish)
		MESSAGE_HANDLER(WM_URLDOWNLOADFROMURL_FAILED, OnDownloadFailed)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_ID_HANDLER(IDC_SYSCLOSE, OnSysClose)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_BUTTONsyn, Onsyn)
		REFLECT_NOTIFICATIONS()
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):  
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDownloadStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDownloadFinish(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDownloadFailed(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT Onsyn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

public:
	void UpdateFileName(CString strFileName);
	bool DoSysClose();

	DWORD dwPostSize;

	char* dotPointer;
	BOOL    isBase64;
	_int64   i64Big30;
	struct tData
	{
		string       iTableNum;
		string       lUT;
		string       idbver;
		string       imark;
		int       iupdown;      //have (updata 1 up  2 down)( 3 same)   no insert( 0 null down )(4 up)
		string	  strdata;
	};
	typedef std::map<std::string, tData> TempUID;

	TempUID*  GetTempUID() {return &m_TempUID;}
	TempUID   m_TempUID;

	//static DWORD WINAPI _threadCal(LPVOID lp);

	CString		m_sAgent;			// Agent string reported by WinInet
	CString		m_szMemo;			// String to store status memo
	int			m_nCallContext;		// Context of handle being created
	DWORD		m_dwInternetStatus;	// Last received internet status
	DWORD		m_dwError;			// Error returned in method CallBack
	//root HINTERNET handle

	BOOL		BSend ,BStop,BCancel,BPop,BCdel ;
	BOOL		m_bSP2D;
	string      strresidue;
	string      stransi;
	string      struserinfo;
	int         dwIndex,dwprogress;
	int         isynnum,itotalsyn;
    string      str11[30];
	void   ManageData (std::string  strre);
	void   SendPData (std::string  strxml ,std::string  strpdata);
	void   ShowMess (std::string strdis);
	void   ShowMess (std::string strdis,std::string strdis1);
	void   ShowP ();
	void   ShowButCANCEL ();
	void   ShowButRE ();
	void   PopMess ();
	void   ShowProgress ();

	void   ForData (std::string strfor ,int icase );
	void   ForAddData (std::string strfor ,int icase );
	void   OnTimer(UINT_PTR nIDEvent);
	int      PDataStr(std::string  strfor );
	void  P1DOWN();
	void      P2DOWN();
	void      PGList();
	void  RefreshDB();
	void  strPost1D();
	void  strPost2D();
	void  ErrorSP2D();
	string      DataRe(std::string  strfor );
	void  Deblocking();
	void  MergetbBank();
	void  MergetbAccount();
	void  MergetbsubAccount();
	void  MergetbCategory1();
	void  MergetbCategory2();
	void  MergetbPayee();
	void  MergetbEvent();
	//void  MergeD4();
	void  MergetbTransaction();
	void  MergetbProductChoice();

	CString		m_sHostName;
protected:
	//CBindStatusCallback* m_pBSCB;
	ThreadDataStruct m_tds;
	ThreadDataStruct* m_pInputTds;

	bool m_bFinished;
	bool m_bFailed;
	bool m_bIsContentLength;
	bool m_bCanceled;

	// Protected member vars, used for DDX
	CString		m_sRequest;
	CString		m_sUserName;
	CString		m_sPassword;
	CComboBox	m_ComboAction;

	HCERTSTORE		m_hMyStore;
	PCCERT_CONTEXT  m_pHSCertContext;

	CFont m_tFontText;
	DWORD m_tTextColor;
	CRect m_rcMsgBoxSpinner;
	CString m_strText;
	CStatic m_titleString;
	bool IsVista()
	{
		OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
		BOOL bRet = ::GetVersionEx(&ovi);
		return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6));
	}


protected:
//	CProgressBarCtrl	m_pbPercent;
	int					m_nPercent;
	CRect				m_rectProgress;
public:
	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};