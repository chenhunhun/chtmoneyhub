#pragma once

#include "stdafx.h"
#include "SynchroDlg.h"

#include "../../Utils/Config/HostConfig.h"
//#include "../Util/Util.h"
#include "../../ThirdParty/RunLog/RunLog.h"
#include "../../Utils/PostData/postData.h"
#include "../../Utils/CryptHash/base64.h"
#include "../../Utils/sn/SNManager.h"
#include "../../Utils/HardwareID/genhwid.h"

//#pragma comment(lib, "CryptUI.lib")
//#pragma comment(lib, "Crypt32.lib")

#define POSTHOST _T("http://ss1.ft.com/server/")
// 要发送的数据ID列表地址信息  
#define POSTLIST_URL _T("get_list.php")
// 要发送的下载地址信息    moneyhub.ft.com/server/get_download_data.php
#define POSTDOWN_URL _T("get_download_data.php")
// 要发送的上传地址信息
#define POSTUP_URL _T("get_upload_data.php")
// 服务器时间
#define POSTSERVER_TIME _T("verify_time.php")
// 解锁
#define POSTUSER_CLEAR _T("user_unlock.php")

BOOL   Bthread = FALSE;
DWORD WINAPI/* CSynchroDlg::*/_threadCal(LPVOID lp)
{
	Bthread = TRUE;
	ThreadCacheDC::CreateThreadCacheDC();
	CSynchroDlg* dlg = (CSynchroDlg *)lp;
	//if(dlg->BCancel)
	//	return 0;
	USES_CONVERSION;

#ifdef _DEBUG
	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (dlg->stransi.c_str ()));
	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (dlg->strresidue.c_str ()));
#else
#endif
	//BStop = FALSE;
	if(dlg->isBase64)
	{
		int dwSize = dlg->strresidue.size() * 2 + 1;
		//int dwSize = strlen(pData) * 2 + 1;
		unsigned char* pszOut = new unsigned char[dwSize];
		base64_encode((LPBYTE)dlg->strresidue.c_str(), dlg->strresidue.size(), pszOut, &dwSize);
		//base64_encode((LPBYTE)pData.GetBuffer(pData.GetLength()), pData.GetLength(), pszOut, &dwSize);
		pszOut[dwSize] = 0;
		dlg->stransi += CPostData::getInstance()->UrlEncode((char *)pszOut);
		delete []pszOut;
	}
	else
		dlg->stransi += dlg->strresidue;
	bool b_suc = true;
	if(CPostData::getInstance()->PostIniBigData(dlg->m_sHostName ) != 0)
		b_suc = false;

#ifdef _DEBUG
		int nLen = dlg->m_sHostName.GetLength();
		std::wstring strt =dlg->m_sHostName.GetBuffer(nLen);
		dlg->m_sHostName.ReleaseBuffer();
	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR,  strt );
	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (dlg->stransi.c_str ()));
#else
#endif

	if(b_suc)
	{
		if(CPostData::getInstance()->UseHttpSendReqEx(dlg->stransi))
     		dlg->ManageData(dlg->stransi);
		else
			b_suc = false;
	}
	if(!b_suc)
		dlg->dwIndex = 93;      
	dlg->BStop = TRUE;    

	ThreadCacheDC::DestroyThreadCacheDC();

	Bthread = FALSE;
	return 0;
}


void remove_space(string& str)
{ 
	string buff(str); 
	char space = ' '; 
	str.assign(buff.begin() + buff.find_first_not_of(space), 
	buff.begin() + buff.find_last_not_of(space) + 1); 
} 

CSynchroDlg::CSynchroDlg(/*ThreadDataStruct* pTds*/int m_status )
: m_bFinished(false), m_bFailed(false), m_bCanceled(false), m_nPercent(0), m_rectProgress(80, 110, 420, 400)
{//m_rectProgress(76, 95, 346, 108
	strresidue = "";
	//stransi = "";
	struserinfo = "";
	m_bSP2D  = BCancel = BCdel =  FALSE;
	isBase64 = BPop = TRUE;
	BSend = BStop = TRUE;
	//strBig30 = "";
	dwIndex = i64Big30 = dwprogress = 0;
	itotalsyn = isynnum =0;
	//m_pInputTds = pTds;
	//m_sHostName =  CHostContainer::GetInstance()->GetHostName(kUserConnect).c_str();
	//m_sHostName.Delete(0,7);    m_sHostName.Delete(m_sHostName.GetLength()-1,1);
	m_sHostName =  CHostContainer::GetInstance()->GetHostName(kPUserServerTime).c_str();
	//m_sHostName = _T("127.0.0.1");

	//m_tds.hClient = pTds->hClient;
	//m_tds.hCreateEvent = pTds->hCreateEvent;
	//m_tds.strFileName = pTds->strFileName;
	//m_tds.strUrl = pTds->strUrl;
	//m_tds.bHasContentLength = pTds->bHasContentLength;

	// 进度条的矩行区域
	dwPostSize =400;
	dwPostSize *= 1024;  // Convert KB to bytes
	dotPointer ="";
}

CSynchroDlg::~CSynchroDlg()
{
	//KillTimer(200);
	//CBankData::GetInstance()->CloseDB();
}

LRESULT CSynchroDlg::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	CRect rect(80, 110, 420, 400);
	
	if(dwprogress != 0)
		s()->StatusBar()->DrawDownloadProgress(dc, m_rectProgress, m_nPercent);
	//s()->StatusBar()->DrawDownloadProgress(dc, rect, m_nPercent);

		SetDlgItemText(IDC_STATIC_FILENAMES, m_sUserName);
   		SetDlgItemText(IDC_STATIC_UNKNOWNLENGTHS, m_sPassword);
     	if( BPop )
		{
			::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
			SetDlgItemText(IDCANCEL, m_sRequest);
			::ShowWindow(GetDlgItem(IDCANCEL), SW_SHOW);
		}
		else
		{
			::ShowWindow(GetDlgItem(IDCANCEL), SW_HIDE);
			::ShowWindow(GetDlgItem(IDOK), SW_SHOW);
			::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_SHOW);
		}

	return 0;
}

LRESULT CSynchroDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetDesktopWindow());
	//CenterWindow(GetParent());
	SetWindowText(_T("同步数据"));

	if (NULL == g_pSkin)
		new CSkinManager();

	g_pSkin->RefreshProgressSkin();

	m_titleString.Attach(GetDlgItem(IDC_STATIC_FILENAMES));

		LPCTSTR lpszFontName[] = { _T("Tahoma"), _T("宋体") };    //微软雅黑

		int iIndex = IsVista() ? 1 : 0;
		if (NULL == m_tFontText)
			m_tFontText.CreateFont(15, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
		m_titleString.SetFont(m_tFontText);
		//m_tTextColor = RGB(16, 93, 145);


//	m_pbPercent = GetDlgItem(IDC_PROGRESS_SYNCHRO);
//	m_pbPercent.SetRange(0, 100);

	ApplyButtonSkin(IDCANCEL);
	//ApplyStaticSkin(IDC_STATIC_UNKNOWNLENGTHS);
	ApplyStaticSkin(IDC_STATIC_STATUSS);
	//ApplyStaticSkin(IDC_STATIC_FILENAMES);
	ApplyButtonSkin(IDOK);
	ApplyButtonSkin(IDC_BUTTONsyn);
	
	//if (m_pBSCB->IsContentLength())
	if (1)
//	if (m_tds.bHasContentLength)
	{
		m_bIsContentLength = true;
		//::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_HIDE);
		//m_pbPercent.ShowWindow(SW_SHOW);
	}
	else
	{
		m_bIsContentLength = false;
		SetDlgItemText(IDC_STATIC_UNKNOWNLENGTHS, _T("未知文件大小"));
		::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_SHOW);
		//m_pbPercent.ShowWindow(SW_HIDE);
	}
	
	//CProgressBarCtrl* pTempCtrl = (CProgressBarCtrl*)GetDlgItem(IDC_PROGRESS_SYNCHRO);
	//pTempCtrl->ShowWindow(SW_HIDE);

	//SetDlgItemText(IDC_STATIC_FILENAMES, m_tds.strFileName);

	//m_pInputTds->hProgDialog = m_hWnd;  您当前使用的财金汇版本较旧,无法执行同步,建议升级至最新版本
	SetEvent(m_tds.hCreateEvent);

	//m_sAgent = _T("Mozilla/4.0 (compatible; MSIE 6.0;Windows NT 5.1)");  当前网络故障,请连接互联网后同步

	//WCHAR sInfo[256] = { 0 }; 
	BOOL b_ret = TRUE;
//	std::string  strd1 = "UPDATE tbBank SET id='1293811200000', name='CATA420',classId='',Phone='',Website='' WHERE BankID='66';";

	CBankData* pBankData = CBankData::GetInstance();
					//pBankData->GfieldName ();
					//pBankData->GetList (0);
     //int ret = pBankData->ExecuteSQL(strd1);   
	//std::string  strd1 = "100671234567890102";    int dpos;
	//dpos = strd1.size() - 3; //
	//std::string strdata = "2011-11-ff:100671000:gggggg";
	//	strd1.replace(dpos ,3, "");

		//									dpos = strdata.find(strd1); //
	//	DataRe( strdata);
	//									strd1 = "yyyy";
	//strdata.replace(dpos, 4,strd1);
					//struserinfo = "4iWCmoB9rM5WnXWkHJ5ToktTNOZWc1LK#";//CBankData::GetInstance()->m_CurUserInfo.struserid;  //"119@110.com#";  //email
					struserinfo = CBankData::GetInstance()->m_CurUserInfo.struserid + "#";  //"119@110.com#";  //email
					//struserinfo += CSNManager::GetInstance()->GetSN() + "#";
					//struserinfo += GenHWID2() + "#";
					//struserinfo = "XVBDW3V7t2QyUrwzleVG00OC7cTr9saj#32318847380630968096#rXCDTzJpim2AlV%2F9aLQRRxegylqc3PaC#nyUmjbLH1112270951595b003870587f280f0b5bf5af36eacc197a206f6bc9e21c37e0c3f3a4a7ac38b9#";//CBankData::GetInstance()->m_CurUserInfo.strstoken;  //STOKEN
					struserinfo += CBankData::GetInstance()->m_CurUserInfo.strstoken + "#";  //STOKEN

	m_sUserName = "正在进行用户身份校验...";	m_sPassword = "";	m_sRequest = "取消";  //ShowButCANCEL();
	dwIndex = 119; 
	SetTimer(200, 1000);

	return b_ret;
}

LRESULT CSynchroDlg::OnDownloadStatus(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	ProgStruct* pps = (ProgStruct*)wParam;
	VARIANT_BOOL* pCancel = (VARIANT_BOOL*)lParam;

	LONG lProgress = pps->lProgress;
	LONG lProgressMax = pps->lProgressMax;

	if (m_bIsContentLength)
	{
		if (lProgressMax > 0)
		{
			
			int nPercent = lProgress * 100 / lProgressMax;
			if (m_nPercent < nPercent)
			{
				m_nPercent = nPercent;
	//			m_pbPercent.SetPos(m_nPercent);
				CString strTemp;
				strTemp.Format(L"progress = %d", m_nPercent);
				OutputDebugString(strTemp);
				//Invalidate();
				//CRect rect(76, 95, 340, 120);
				::InvalidateRect(this->m_hWnd, m_rectProgress, TRUE);
			}
		}
	}
	else
	{
		if (lProgress != 0)
		{
			CString strText;
			strText.Format(_T("未知文件大小(已经下载%d字节)"), lProgress);
			SetDlgItemText(IDC_STATIC_UNKNOWNLENGTHS, strText);
			::InvalidateRect(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), NULL, TRUE);
		}		
	}

	if (m_bCanceled)
	{
		*pCancel = VARIANT_TRUE;
		EndDialog(IDCANCEL);
	}

	return 0;
}

LRESULT CSynchroDlg::OnDownloadFinish(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (m_bCanceled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	//m_bFinished = true;
	SetDlgItemText(IDC_STATIC_STATUSS, _T("文件下载完成！"));
	::InvalidateRect(GetDlgItem(IDC_STATIC_STATUSS), NULL, TRUE);
	EndDialog(IDOK);

	return 0;
}

LRESULT CSynchroDlg::OnDownloadFailed(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (m_bCanceled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	//m_bFailed = true;
	SetDlgItemText(IDC_STATIC_STATUSS, _T("文件下载失败！"));
	::InvalidateRect(GetDlgItem(IDC_STATIC_STATUSS), NULL, TRUE);
	SetDlgItemText(IDCANCEL, _T("确定"));

	return 0;
}

void CSynchroDlg::UpdateFileName(CString strFileName)
{
	SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
	::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
}

bool CSynchroDlg::DoSysClose()
{
	/*CPostData::getInstance()->CloseHandles();*/			BCdel = TRUE;     
   	PopMess ();
	return 3;  //3
}

LRESULT CSynchroDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	/*CPostData::getInstance()->CloseHandles();*/			BCdel = TRUE;
	PopMess ();
	return 3;  //3
}

LRESULT CSynchroDlg::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PopMess ();
	return 3; //0
}

LRESULT CSynchroDlg::Onsyn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CBankData* pBankData = CBankData::GetInstance();
	if(pBankData->GetTablefield()->size() != 0)
		pBankData->GetTablefield()->clear();
	if(pBankData->GetUserIDMap()->size() != 0)
		pBankData->GetUserIDMap()->clear();
	m_sHostName =  CHostContainer::GetInstance()->GetHostName(kPUserServerTime).c_str();
	m_sUserName = "正在进行用户身份校验...";	m_sPassword = "";	m_sRequest = "取消";		//ShowButCANCEL();
	dwIndex = 119;                      BStop = TRUE;			BSend  = TRUE;			BPop = TRUE;
	itotalsyn = isynnum =0;
	//PopMess ();
	return 3;     //S_OK;
}

void   CSynchroDlg::PopMess ()
{
	BCancel = TRUE;
	//if(BPop)
	//{
	//	WCHAR sInfo[256] = { 0 }; 
	//	swprintf(sInfo, 256, L"是否确认退出同步?");
	//	if(MessageBoxW(sInfo, L"财金汇",  MB_OKCANCEL) == IDOK)
 //   		BSend  = FALSE;
	//}
	//else
    	//BSend  = FALSE;
}
void CSynchroDlg::SendPData (std::string strxml ,std::string strpdata)
{
	stransi = strxml;			strresidue = strpdata;
	DWORD dw;
	CloseHandle(CreateThread(NULL, 0, _threadCal, this, 0, &dw));
	return;


	USES_CONVERSION;
				int nLen = m_sHostName.GetLength();
			LPCSTR lpszBuf = W2A(m_sHostName.GetBuffer(nLen));

	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (lpszBuf));
	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (strxml.c_str ()));
	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (strpdata.c_str ()));
	//BStop = FALSE;
	if(isBase64)
	{
		int dwSize = strpdata.size() * 2 + 1;
		//int dwSize = strlen(pData) * 2 + 1;
		unsigned char* pszOut = new unsigned char[dwSize];
		base64_encode((LPBYTE)strpdata.c_str(), strpdata.size(), pszOut, &dwSize);
		//base64_encode((LPBYTE)pData.GetBuffer(pData.GetLength()), pData.GetLength(), pszOut, &dwSize);
		pszOut[dwSize] = 0;
		strxml += CPostData::getInstance()->UrlEncode((char *)pszOut);
		delete []pszOut;
	}
	else
	{
		strxml += strpdata;
	}
	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (strxml.c_str ()));
	bool b_suc = true;
	if(CPostData::getInstance()->PostIniBigData(m_sHostName ) != 0)
	{
		b_suc = false;
		//if(m_bSP2D){
		//	m_bSP2D = FALSE;			ErrorSP2D();}
	}
	if(b_suc)
	{
		if(CPostData::getInstance()->UseHttpSendReqEx(strxml))
     		ManageData(strxml);
		else
		{
			b_suc = false;
			//if(m_bSP2D){
			//	m_bSP2D = FALSE;			ErrorSP2D();}
		}
	}
	if(!b_suc)
		dwIndex = 93;      
	BStop = TRUE;    
}

void CSynchroDlg::ManageData (std::string  strre)
{
	USES_CONVERSION;
#ifdef _DEBUG
	CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (strre.c_str ()));
#else
#endif
	string tem = "";	string dddt = "";	size_t dpos = 0;
	CBankData* pBankData = CBankData::GetInstance();

	//if(m_bFirst)
	{
     	//remove_space(strre);
		//m_bFirst =FALSE;
		dpos = strre.find("#");
		if(dpos != std::string::npos)
		{
	    	dddt = strre.substr(0 ,dpos);
			dddt = pBankData->FilterStringNumber(dddt);
		    strre.replace(0,dpos+1,tem);
		}
		if(dddt != "")
    		dwIndex = atoi(dddt.c_str());
		else
        	dwIndex = 0;
	}
	
	switch(dwIndex)
	{	
		case 11:
			{
				dddt = pBankData->GetDbPath();
				dpos = dddt.find("Guest.dat");
				if(dpos == std::string::npos)
				{
					ForData (strre ,5 );
					pBankData->GfieldName ();			pBankData->GetList (i64Big30,itotalsyn);			isynnum = itotalsyn; //strBig30
					m_bFinished = true;
				}
			}
			break;
		case 15:
			{
				if(m_bFailed)
				{
					std::string strtable = "datUserInfo";
					std::string strSQL = "UPDATE ";          strSQL += strtable;              strSQL += " SET needsynchro = '0'";
					strSQL += " WHERE userid='";
					strSQL += pBankData->m_CurUserInfo.struserid ;                      strSQL += "';" ;
    				if(pBankData->ExecuteSQL(strSQL, "DataDB") > 0)
					{
						strtable = "tbTotalEvent";
					}
				}
				//ForAddData (strre ,6 );
			}
			break;
		case 21:
		case 22:
			{
				ForData (strre ,6 );				RefreshDB();					
			}
			break;
		case 35:
			{
				UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
				for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
				{
					int itemp = Uidmap->second.iupdown;
					if( Uidmap->second.iTableNum != "9" && (itemp == 4  || itemp == 1 ))
					{
		        		isynnum--;	ShowP ();
						Uidmap->second.iupdown = 3;						Uidmap->second.strdata = "";
					}
				}
			}
			break;
		case 36:
			{
				m_bSP2D = FALSE;
				UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
				for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
				{
					int itemp = Uidmap->second.iupdown;
					if( Uidmap->second.iTableNum == "9" && (itemp == 8  || itemp == 9 ))
					{
		        		isynnum--;	ShowP ();
						Uidmap->second.iupdown = 3;						Uidmap->second.strdata = "";
					}
				}
			}
			break;
		case 91:
			{
				_int64  le99 = pBankData->GetNewUT();
					//SYSTEMTIME systm;
					//GetSystemTime(&systm);
					//time_t rawtime; 					struct tm * timeinfo; 					time ( &rawtime );
					//_int64 imsecond = rawtime*1000;
					//timeinfo = gmtime ( &rawtime );      1320825106      1293831360000
					dpos = strre.find("#");
					if(dpos == string::npos){
						dwIndex =92;						break;
					}
					//imsecond -= 1293831360000;      1293811200000
					dddt = strre.substr(0 ,dpos);
					i64Big30 = _atoi64( dddt.c_str() );         			i64Big30 -= 1293840000000;
					//char   sID[64] ={0}; 					_i64toa(le,sID ,10);
					//if(imsecond - i64t > 1800000 )
						//strBig30 = sID;
			}
			break;
		default:
			break;
	}

	//NextPData (dwIndex);
//	ddd.replace(ifor - strresidue.size() ,strresidue.size() ,tem);
//	char* tmp;
//	while(tmp = strchr(dotPointer, ','))
//		dotPointer = tmp + 1;
//	strresidue = dotPointer;
}

void   CSynchroDlg::ForData (std::string strfor ,int icase )
{
	//table1   #ID1    #UT1#     version1   #mark#     DATA#
	CBankData* pBankData = CBankData::GetInstance();
	int ifor = strfor.size();
	size_t dpos = 0;    string tem = "";
	for(int i=0 ; i< 30; i++)
		str11[i] = "";
	int iii = 0;
	for(int i=0 ; i< ifor; i++)
	{
		for(int i=0 ; i< icase; i++)  
		{
			dpos = strfor.find("#"); //
			if(dpos == string::npos)
			{
				iii =1000;
				break;
			}
			str11[i] = strfor.substr(0 ,dpos);
			//itable = atoi(dddt.c_str());
			strfor.replace(0,dpos+1,tem);
		}
		if(iii == 1000)
			break;
		if(icase == 5 )  //11
		{
			UDData uddata ={ str11[0],str11[2],str11[3],str11[4] ,iii ,str11[5]};
			pBankData->GetUserIDMap()->insert(std::make_pair(str11[1], uddata));
			itotalsyn++;
		}
		else if(icase == 6 )  //21 22
		{
			UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->find(str11[1]);
			if (Uidmap != pBankData->GetUserIDMap()->end())
			{
				Uidmap->second.strdata = str11[5];
				if(Uidmap->second.iupdown == 0)
					Uidmap->second.iupdown = 6;
				else if(Uidmap->second.iupdown == 2)
					Uidmap->second.iupdown = 7;
			}
		}
	}

	if(dwIndex == 21) 			MergetbCategory1();
	else if(dwIndex == 22)		MergetbTransaction();

}

void   CSynchroDlg::ForAddData (std::string strfor ,int icase )
{
	//0 table#  1 ID    2 #UT1#     3version1# 4mark#   5DATA#
	//int ifor =  strfor.size();
	//size_t dpos = 0;    string tem = "";
	//for(int i=0 ; i< 30; i++)
	//	str11[i] = "";
	//int iii = 0;
	//for(int i=0 ; i< ifor; i++)
	//{
	//	for(int i=0 ; i< icase; i++)  
	//	{
	//		dpos = strfor.find("#"); //
	//		if(dpos == string::npos)
	//		{
	//			iii =1000;
	//			break;
	//		}
	//		str11[i] = strfor.substr(0 ,dpos);
	//		//itable = atoi(dddt.c_str());
	//		strfor.replace(0,dpos+1,tem);
	//	}
	//	if(iii == 1000)
	//		break;
	//	CBankData* pBankData = CBankData::GetInstance();
	//	UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->find(str11[1]);
	//	if (Uidmap != pBankData->GetUserIDMap()->end())
	//	{
	//		_int64 icut = _atoi64(Uidmap->second.lUT.c_str());
	//		_int64 isut = _atoi64(str11[2].c_str());
	//		if(icut > isut)
	//		{
	//			//Uidmap->second.iupdown = 1;
	//			//Uidmap->second.iTableNum = strtb;
	//			//if(strBig30 != "")
	//			{
	//				//Uidmap->second.lUT = strBig30;	                std::string  strd1 = "UPDATE ";
	//				//std::string  strtable = Uidmap->second.iTableNum;					pBankData->TableStr (strtable );
	//				//strd1 += strtable;                                                  strd1 += " SET UT='";
	//				//strd1 += strBig30;                                                  strd1 += "' WHERE id='";
	//				//strd1 += str11[1];                                                  strd1 += "';";
 //    //               int ret = pBankData->ExecuteSQL(strd1);
	//			}
	//		}
	//		//else if(icut < isut)
	//		//{
	//		//	Uidmap->second.iupdown = 7;
	//		//	Uidmap->second.lUT = str11[2];
	//		//	Uidmap->second.idbver = str11[3];
	//		//	Uidmap->second.imark = str11[4] ;
	//		//	Uidmap->second.strdata = str11[5] ;
	//		//}
	//		//else
	//		//	Uidmap->second.iupdown = 3;
	//	}
	//}
	//RefreshDB();
}

void CSynchroDlg::OnTimer(UINT_PTR nIDEvent)
{
	USES_CONVERSION;
		CBankData* pBankData = CBankData::GetInstance();					std::string strtable = "";
		std::string strdisperror ="共";				strdisperror += pBankData->itostr(itotalsyn);		strdisperror += "条数据需同步,成功同步";
		strdisperror += pBankData->itostr(itotalsyn-isynnum);				strdisperror += "条";

	if(BCancel && !BSend )
	{
		//if(m_bSP2D){
		//	m_bSP2D = FALSE;			ErrorSP2D();}
		if(!Bthread)
		{
			if(pBankData->GetTablefield()->size() != 0)
				pBankData->GetTablefield()->clear();
			if(pBankData->GetUserIDMap()->size() != 0)
				pBankData->GetUserIDMap()->clear();
			if(m_bFailed)
     			EndDialog(IDOK);
			else
     			EndDialog(IDCANCEL);
			return;
		}
	}
	if(BCancel && (BStop || BCdel) && !Bthread)
	{
   		BSend  = BStop =BCdel = FALSE;			Deblocking();					
		//Bthread = FALSE;
	}
	if(BStop && !BCancel)
	{
		BStop = FALSE;
		switch(dwIndex)
		{	
			case 11:
				{
					if(m_bFinished){	
										P1DOWN(); }
					else{
						ShowMess ("数据库忙,请稍候几秒再试!"); 						/*ShowButOk();*/}
				}
				break;
			case 12:
				{
					BSend  = FALSE;			m_sUserName = "当前用户正在其他机器同步,请稍候再试!";	m_sPassword = "";
					m_sRequest = "确定";	//		ShowButOk();
				}
				break;
			case 15:
				{
					std::string strtemp ="共同步了";		strtemp += pBankData->itostr(itotalsyn-isynnum);					strtemp += "条数据";
					BSend  = FALSE;			dwprogress =0;		m_sUserName = "同步已完成";	m_sPassword = strtemp.c_str();	
					m_sRequest = "确定";			//ShowButOk();
				}
				break;
			case 21:
			case 22:
				{
					if(dwIndex == 21){	m_sUserName = "正在检查比对数据...";	/*m_sPassword = strdispok.c_str();*/						}
					else{	m_sUserName = "正在同步数据,请稍候...";	/*m_sPassword = strdispok.c_str();*/						}					
					P2DOWN();				
				}
				break;
			case 35:
			case 36:
				{
					strPost2D();				m_sUserName = "正在上传数据...";	/*m_sPassword = strdispok.c_str();*/
					//ShowMess ("正在上传数据...",strdispok);	
				}
				break;
			case 62:
			case 63:
					//ShowMess ("登录验证失败,您需要重新登录!");  						ShowButOk();
			case 64:
					//ShowMess ("用户名已修改,您需要重新登录!");  						ShowButOk();
			case 65:
				{
					//if(m_bSP2D){
					//	m_bSP2D = FALSE;			ErrorSP2D();}
					//ShowMess ("密码已修改,您需要重新登录!");  						ShowButOk();
					//pBankData->m_CurUserInfo.strstoken = "";				
					pBankData->m_CurUserInfo.emUserStatus = emNotLoad;
					BCancel= TRUE;				BSend  = FALSE;
				}
				break;
			case 81:
			case 82:
				{
					//if(m_bSP2D){
					//	m_bSP2D = FALSE;			ErrorSP2D();}
					BSend  = FALSE;		dwprogress =0;				m_sUserName = "同步失败,请检查网络,尝试重新同步!";	m_sPassword = strdisperror.c_str();
					BPop = FALSE;     //ShowButRE();
				}
				break;
			case 83:
				{
					BSend  = FALSE;			m_sUserName = "您财金汇版本较旧,无法同步,请升级至最新版本!";	m_sPassword = "";
					m_sRequest = "确定";		//ShowButOk();
				}
				break;
			case 91:
				{
					m_sUserName = "正在下载云端数据...";	m_sPassword = "";
			    	ShowProgress();							PGList();
				}
				break;
			case 92:
			case 93:
				{
	             	BSend  = FALSE;		dwprogress =0;			m_sUserName = "无法连接服务器,请检查网络连接状况！";	m_sPassword = strdisperror.c_str();
					BPop = FALSE;     //ShowButRE();
				}
				break;
			case 119:
				{
					std::string struserddd = CSNManager::GetInstance()->GetSN() + "#";
					struserddd += GenHWID2() + "#";
#ifdef _DEBUG
	                CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (struserddd.c_str ()));
#else
#endif
					if(!InternetCheckConnection(L"http://www.baidu.com", FLAG_ICC_FORCE_CONNECTION, 0) )
					{
						BSend  = FALSE;			m_sUserName = "当前网络故障,请连接互联网后同步!";	m_sPassword = "";  						
						BPop = FALSE;     /*ShowButRE();*/    					break;
					}
					std::string strtemp = struserinfo + CBankData::GetInstance()->strDBVer();                   strtemp += "#";	
					std::string  strd = "xml=" + strtemp;                        strtemp = "";
					SendPData(strd ,strtemp);
				}
				break;
			case 120:
				{
	             	BSend  = FALSE;		dwprogress =0;			m_sUserName = "传输数据错误!";	m_sPassword = "";
					BPop = FALSE;     //ShowButRE();
				}
				break;
			default:
				{
					BSend  = FALSE;		dwprogress =0;			m_sUserName = "同步失败,您可以尝试重新同步!";	m_sPassword = strdisperror.c_str();
					BPop = FALSE;     //ShowButRE();
				}
				break;
		}
    	Invalidate();
		//::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
		//::InvalidateRect(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), NULL, TRUE);
	}
    	//Invalidate();
}

void  CSynchroDlg::strPost1D()   //post data 1
{//31#table#ID1#UT1#version1#mark#DATA1
	    std::string strtemp = struserinfo  ;       std::string  strtemp1 = "1#";
		CBankData* pBankData = CBankData::GetInstance();
    	UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum != "9" && (itemp == 4  || itemp == 1 ))
			{
				if(itemp == 4)
			    	strtemp1 += "31#";
				else if(itemp == 1)
			    	strtemp1 += "32#";
				strtemp1 += Uidmap->second.iTableNum;     strtemp1 += "#";
				//std::string strSQL ="'";
				strtemp1 += Uidmap->first;			      strtemp1 += "#";
				strtemp1 += Uidmap->second.lUT;      strtemp1 += "#";
				strtemp1 += pBankData->strDBVer();     strtemp1 += "#";
				strtemp1 += Uidmap->second.imark;      strtemp1 += "#";
				strtemp1 += Uidmap->second.strdata;      strtemp1 += "#";
			}
		}
		if(strtemp1 == "1#")
		{
					strPost2D();
		}
		else
		{
			m_sHostName =  CHostContainer::GetInstance()->GetHostName(kPGetUploadData).c_str();//POSTUP_URL;
			/*strtemp += strtemp1 ;		*/	    std::string strxml = "xml=" + struserinfo;
    		SendPData(strxml,strtemp1);
		}
}

void  CSynchroDlg::ErrorSP2D()
{
		CBankData* pBankData = CBankData::GetInstance();
		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum == "9" && (itemp == 8  || itemp == 9 ))
			{
				if(itemp == 8)
					Uidmap->second.iupdown = 4;  //insert
				else if(itemp == 9)
					Uidmap->second.iupdown = 1;  //update
			}
		}
}
					
void  CSynchroDlg::strPost2D()   //post data 2
{
	    std::string strtemp = struserinfo  ;        std::string strtemp1 = "2#";
		CBankData* pBankData = CBankData::GetInstance();
    	UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		int ipnum = 0;
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum == "9" && (itemp == 4  || itemp == 1  ))
			{
				if(itemp == 4){
					strtemp1 += "31#";       Uidmap->second.iupdown = 8; } //insert
				else if(itemp == 1){
					strtemp1 += "32#";       Uidmap->second.iupdown = 9; } //update
				strtemp1 += Uidmap->second.iTableNum;     strtemp1 += "#";
				//std::string strSQL ="'";
				strtemp1 += Uidmap->first;		     strtemp1 += "#";
				strtemp1 += Uidmap->second.lUT;      strtemp1 += "#";
				strtemp1 += pBankData->strDBVer();     strtemp1 += "#";
				strtemp1 += Uidmap->second.imark;      strtemp1 += "#";
				strtemp1 += Uidmap->second.strdata;      strtemp1 += "#";
		    	ipnum++;       if(ipnum == 100)  break;
			}
		}
		if(strtemp1 == "2#")
		{
			m_bFailed = true;			Deblocking();
		}
		else
		{
			m_bSP2D = TRUE;
			m_sHostName =  CHostContainer::GetInstance()->GetHostName(kPGetUploadData).c_str();//POSTUP_URL;
			/*strtemp += strtemp1 ;*/			    std::string strxml = "xml=" + struserinfo;
    		SendPData(strxml,strtemp1);
		}
}

void  CSynchroDlg::RefreshDB()   //refresh db
{
		std::string strtable = "";
		CBankData* pBankData = CBankData::GetInstance();
    	UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			if(( Uidmap->second.iupdown == 6 || Uidmap->second.iupdown == 10 ) && (Uidmap->second.imark == "0") )
			{
				std::string strfirst = Uidmap->first;		int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");

				strtable = Uidmap->second.iTableNum; 
				pBankData->TableStr (strtable );
				std::string strdata ="'";
				strdata += strfirst;/*Uidmap->first;*/      strdata += "','";   strdata += Uidmap->second.lUT;      strdata += "','";
				strdata += Uidmap->second.imark;      strdata += "','";
				strdata += DataRe( Uidmap->second.strdata);
				std::string strSQL = "INSERT INTO ";
				strSQL += strtable;							strSQL += " VALUES (";
				strSQL += strdata;           strSQL += ");";
    			if(pBankData->ExecuteSQL(strSQL) > 0)
				{
					isynnum--;		ShowP ();
					if(Uidmap->second.iupdown == 10)
		    			Uidmap->second.iupdown = 1;
					else
		    			Uidmap->second.iupdown = 3;
				}
				if(strtable == "tbEvent")
				{
					std::string itable = Uidmap->second.iTableNum;
	            	TableField::iterator Tfmap = pBankData->GetTablefield()->find(itable);
		    		int itnum = Tfmap->second.ifieldNum;
			    	PDataStr(Uidmap->second.strdata);       str11[29] = "";
					strtable = "tbTotalEvent";
					strSQL = "INSERT INTO ";
					strdata = strfirst;/*Uidmap->first;*/      strdata += "','";   strdata += Uidmap->second.imark;      strdata += "',";
					for(int i=3 ; i< itnum; i++) 
					{
						if(i!= 4 && i!= 9  && i!= 10)
						{
							str11[29] += str11[i];								str11[29] += ",";  
						}
					}
					str11[29] += "'";		str11[29] += pBankData->m_CurUserInfo.struserid;		str11[29] += "'";
					strdata += str11[29];
					strSQL += strtable;							strSQL += " VALUES ('";
					strSQL += strdata;           strSQL += ");";
    				if(pBankData->ExecuteSQL(strSQL, "DataDB") > 0)
					{
						strtable = "tbTotalEvent";
					}
				}
			}
			//else if(Uidmap->second.imark == "1")
			//{
			//	itotalsyn--;
			//}
		}
    	Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			if( Uidmap->second.iupdown == 7 )
			{//PDATE tbBank SET name='CATA420',classId='34',BankID='66',Phone='',Website='' WHERE id=40;"
				std::string strfirst = Uidmap->first;		int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");

				PDataStr(Uidmap->second.strdata);
				std::string itable = Uidmap->second.iTableNum;  strtable = itable;
	        	TableField::iterator Tfmap = pBankData->GetTablefield()->find(itable);
				int itnum = Tfmap->second.ifieldNum;

				pBankData->TableStr (strtable );
				str11[0] = "'";				str11[0] += strfirst;/*Uidmap->first;*/      str11[0] += "'";
				str11[1] = "'";	str11[1] += Uidmap->second.lUT;      str11[1] += "'";
				str11[2] = "'";	str11[2] += Uidmap->second.imark;    str11[2] += "'";

				std::string strSQL = "UPDATE ";          strSQL += strtable;              strSQL += " SET ";
				for(int k = 1 ;k < itnum ; k++)
				{
					std::map<int , std::string>::const_iterator fmap = Tfmap->second.m_field.find(k);
					strSQL += fmap->second ;             strSQL += "=" ;          strSQL += str11[k] ;     
					if( k == itnum -1)			       				    strSQL += " WHERE id=";
					else					    			strSQL += "," ;
				}
				strSQL += str11[0] ;                      strSQL += ";" ;
    			if(pBankData->ExecuteSQL(strSQL) > 0)
				{
					isynnum--;			ShowP ();			Uidmap->second.iupdown = 3;
				}
				if(strtable == "tbEvent")
				{
					strtable = "tbTotalEvent";
					std::string strSQL = "UPDATE ";          strSQL += strtable;              strSQL += " SET ";
					for(int k = 2 ;k < itnum ; k++)
					{
						if(k!= 4 && k!= 9  && k!= 10)
						{
							std::map<int , std::string>::const_iterator fmap = Tfmap->second.m_field.find(k);
							strSQL += fmap->second ;       strSQL += "=" ;          strSQL += str11[k] ;   			strSQL += "," ;
						}
					}
					strSQL += "datUserInfo_userid" ;       strSQL += "='" ;		strSQL += pBankData->m_CurUserInfo.struserid ;
					strSQL += "' WHERE id=";
					strSQL += str11[0] ;                      strSQL += ";" ;
    				if(pBankData->ExecuteSQL(strSQL, "DataDB") > 0)
					{
						strtable = "tbTotalEvent";
					}
				}
			}
		}
}

void  CSynchroDlg::PGList()
{
		CBankData* pBankData = CBankData::GetInstance();
		//if(pBankData->GetUserIDMap()->size() == 0)//获取服务器端数据ID列表
		{
			m_sHostName =  CHostContainer::GetInstance()->GetHostName(kPGetList).c_str();//POSTLIST_URL;
			std::string strd = "xml=" + struserinfo;
			SendPData(strd,"");
		}
		//else //增量数据ID列表
		{
   //  		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
			//std::string strtemp = "3#";
			//for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
			//{
			//	if( Uidmap->second.iupdown != 3 )
			//	{
			//		strtemp += Uidmap->second.iTableNum +"#";
			//		strtemp += Uidmap->first +"#";
			//	}
			//}
			//if(strtemp == "3#")
			//{
				//P1DOWN();
			//}
			//else
			//{
			//	m_sHostName = POSTHOST ;			m_sHostName +=  POSTDOWN_URL;
			//	strtemp = struserinfo + strtemp ;			std::string strd = ("xml=");
			//	SendPData(strd,strtemp);
			//}
		}
}

void  CSynchroDlg::P1DOWN()   //一级数据down
{
		std::string strtable = "";
 	    CBankData* pBankData = CBankData::GetInstance();
     	UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		strtable = "1#";
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			if( Uidmap->second.iTableNum != "9" && (Uidmap->second.iupdown == 0 || Uidmap->second.iupdown == 2))
			{
				strtable += Uidmap->second.iTableNum +"#";
				strtable += Uidmap->first +"#";
			}
		}
		//strtable = "1#";
		if(strtable == "1#")
		{
			P2DOWN();
		}
		else
		{
			m_sHostName =  CHostContainer::GetInstance()->GetHostName(kPGetDownloadData).c_str();//POSTDOWN_URL;
			//strtable = struserinfo + strtable;
			std::string strxml = "xml=" + struserinfo;
	    	SendPData(strxml,strtable);
		}
}

void  CSynchroDlg::P2DOWN()  //二级数据down
{
		CBankData* pBankData = CBankData::GetInstance();
		std::string strtable = "2#";        int ipnum =0;
     	UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			if( Uidmap->second.iTableNum == "9" && (Uidmap->second.iupdown == 0 || Uidmap->second.iupdown == 2))
			{
				strtable += Uidmap->second.iTableNum +"#";
				strtable += Uidmap->first +"#";
		    	ipnum++;       if(ipnum == 100)  break;
			}
		}
		if(strtable == "2#")
		{
			strPost1D();
		}
		else
		{
			m_sHostName =  CHostContainer::GetInstance()->GetHostName(kPGetDownloadData).c_str();//POSTDOWN_URL;
			/*strtable = struserinfo + strtable;	*/		std::string strxml = "xml=" + struserinfo;
    		SendPData(strxml,strtable);
		}
}

string  CSynchroDlg::DataRe(std::string  strfor )
{
	std::string strtemp = strfor;	size_t dpos = 0;  int k =0;
	for(int i=0 ; i< 30; i++)  
	{
		dpos = strtemp.find(":"); //
		if(dpos == string::npos)
			break;
		strtemp.replace(dpos ,1, "','");
	}
	dpos = strtemp.size() - 2; //
	strtemp.replace(dpos ,2, "");
	return strtemp;
}

int  CSynchroDlg::PDataStr(std::string  strfor )//syn datainfo
{
	size_t dpos = 0;   string tem = "";
	for(int i=0 ; i< 30; i++)
		str11[i] = "";
	for(int i=3 ; i< 30; i++)  
	{
		dpos = strfor.find(":"); //
		if(dpos == string::npos)
			break;
		str11[i] = strfor.substr(0 ,dpos);		str11[i] = "'" + str11[i] ;      str11[i] += "'";
		//itable = atoi(dddt.c_str());
		strfor.replace(0,dpos+1,tem);
	}
	return 1;
}

void  CSynchroDlg::Deblocking()
{
	m_sHostName =  CHostContainer::GetInstance()->GetHostName(kPUserUnlock).c_str();//POSTUSER_CLEAR;
	std::string strtemp ;
	strtemp = struserinfo + "10#";			std::string strd = "xml=" + strtemp;
	SendPData(strd,"");
	if(BCancel)
    	BStop = FALSE;
}

void   CSynchroDlg::ShowProgress ()
{
	dwprogress = 10;
	//::InvalidateRect(this->m_hWnd, m_rectProgress, TRUE);
		//Invalidate();
}

void  CSynchroDlg::ShowMess (std::string strdis)
{
		dwprogress =0;   
		::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_HIDE);
		CString strFileName = strdis.c_str();		SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
	::InvalidateRect(this->m_hWnd, m_rectProgress, TRUE);
		Invalidate();
		//::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
}

void  CSynchroDlg::ShowMess (std::string strdis ,std::string strdis1)
{
		CString strFileName = strdis.c_str();		SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
		::ShowWindow(GetDlgItem(IDC_STATIC_FILENAMES), SW_SHOW);
		strFileName = strdis1.c_str();		SetDlgItemText(IDC_STATIC_UNKNOWNLENGTHS, strFileName);
		::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_SHOW);
		Invalidate();
		//::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
		//::InvalidateRect(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), NULL, TRUE);
}

void  CSynchroDlg::ShowP ()
{
	USES_CONVERSION;
	dwprogress = 10;
		CBankData* pBankData = CBankData::GetInstance();
		std::string strdispok ="已完成";					//int ddee=0;
		if(itotalsyn < 1 || isynnum < 0)		dwprogress = 0;
		else		{m_nPercent = (itotalsyn - isynnum)*100/itotalsyn;		/*m_nPercent = ddee;*/}		strdispok += pBankData->itostr(m_nPercent);		strdispok += "%";
		m_sPassword = strdispok.c_str();
		//Invalidate();

	//CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (strdispok.c_str ()));
}

void  CSynchroDlg::ShowButCANCEL ()
{
		::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
		::ShowWindow(GetDlgItem(IDCANCEL), SW_SHOW);
		CString strFileName = "取消";		SetDlgItemText(IDCANCEL, strFileName);
		::InvalidateRect(GetDlgItem(IDCANCEL), NULL, TRUE);
}

void  CSynchroDlg::ShowButRE ()
{
     	BPop = FALSE;
		::ShowWindow(GetDlgItem(IDOK), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_SHOW);
		::ShowWindow(GetDlgItem(IDCANCEL), SW_HIDE);
		CString strFileName = "确定";		SetDlgItemText(IDOK, strFileName);
		strFileName = "重试";		SetDlgItemText(IDC_BUTTONsyn, strFileName);
		::InvalidateRect(GetDlgItem(IDC_BUTTONsyn), NULL, TRUE);
		::InvalidateRect(GetDlgItem(IDOK), NULL, TRUE);
}

void  CSynchroDlg::MergetbBank()
{
		size_t dpos = 0;    string tem = "";  string strfor = "";   string strd1 = ""; int iii = 0;
		CBankData* pBankData = CBankData::GetInstance();

			for(int i=0 ; i< 30; i++)
				str11[i] = "";
		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum == "1" && itemp == 6)  //server
			{
				strfor = Uidmap->second.strdata;
				for(int k=0 ; k< 5; k++)  
				{
					dpos = strfor.find(":"); //
					if(dpos == string::npos)
						break;
					str11[k] = strfor.substr(0 ,dpos);
					strfor.replace(0,dpos+1,tem);
				}
				UserIDMap::iterator Uidtemp = pBankData->GetUserIDMap()->begin();
				for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)// client
				{
					itemp = Uidtemp->second.iupdown;
					if( Uidtemp->second.iTableNum == "1" && itemp == 4)
					{
		        		strfor = Uidtemp->second.strdata;
						for(int k=5 ; k< 10; k++)  
						{
							dpos = strfor.find(":"); //
							if(dpos == string::npos)
								break;
							str11[k] = strfor.substr(0 ,dpos);
							strfor.replace(0,dpos+1,tem);
						}
						if(str11[0] == str11[5] && str11[4] == str11[9])  //类别（银行/支付/投资），名称
						{
							itotalsyn--;   isynnum--;
							std::string strfirst = Uidtemp->first;			int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");
								INT64 isut = _atoi64(Uidmap->second.lUT.c_str());				INT64 icut = _atoi64(Uidtemp->second.lUT.c_str());
								if(isut < icut)
								{
									Uidmap->second.iupdown = 10;
									Uidmap->second.lUT = Uidtemp->second.lUT;   Uidmap->second.strdata = Uidtemp->second.strdata;
								}
							Uidtemp->second.iupdown = 3;			//iii =1000;								break;
							strd1 = "UPDATE "; 	std::string  strtable = "1";	pBankData->TableStr (strtable );
							strd1 += strtable;                                                  strd1 += " SET mark='1'";
							strd1 += " WHERE id='";			strd1 += /*Uidtemp->first*/strfirst;					strd1 += "';";
							int ret = pBankData->ExecuteSQL(strd1);

							std::string  strd1,strd2;
							UserIDMap::iterator Uidup = pBankData->GetUserIDMap()->begin();
							for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
							{
								strd1 = Uidtemp->first;			  dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
								strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
								//strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
								if( Uidup->second.iTableNum == "2" )
								{
									strfor = Uidup->second.strdata;
									dpos = strfor.find(strd1); //
									if(dpos != string::npos)
									{
										strfirst = Uidup->first;		int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
										strfor.replace(dpos,strd1.size(),strd2);
										Uidup->second.strdata = strfor;
										strd1 = "UPDATE "; 	std::string  strtable = "2";	pBankData->TableStr (strtable );
										strd1 += strtable;        strd1 += " SET tbBank_id='";    strd1 +=  strd2;        strd1 +=  "'";      
										strd1 += " WHERE id='";			strd1 += strfirst/*Uidup->first*/;					strd1 += "';";
										int ret = pBankData->ExecuteSQL(strd1);
									}
								}
							}//for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
						}
					}
				}//for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)
			}//if( Uidmap->second.iTableNum == "4" && itemp == 6)
		}
		MergetbAccount();

}

void  CSynchroDlg::MergetbAccount()
{
		size_t dpos = 0;    string tem = "";  string strfor = "";      string strd1 = ""; int iii = 0;
		CBankData* pBankData = CBankData::GetInstance();

			for(int i=0 ; i< 30; i++)
				str11[i] = "";
		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum == "2" && itemp == 6)  //server
			{
				strfor = Uidmap->second.strdata;
				for(int k=0 ; k< 8; k++)  
				{
					dpos = strfor.find(":"); //
					if(dpos == string::npos)
						break;
					str11[k] = strfor.substr(0 ,dpos);
					strfor.replace(0,dpos+1,tem);
				}
				UserIDMap::iterator Uidtemp = pBankData->GetUserIDMap()->begin();
				for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)// client
				{
					itemp = Uidtemp->second.iupdown;
					if( Uidtemp->second.iTableNum == "2" && itemp == 4)
					{
		        		strfor = Uidtemp->second.strdata;
						for(int k=8 ; k< 16; k++)  
						{
							dpos = strfor.find(":"); //
							if(dpos == string::npos)
								break;
							str11[k] = strfor.substr(0 ,dpos);
							strfor.replace(0,dpos+1,tem);
						}
						if(str11[1] == str11[9] && str11[7] == str11[15] && str11[7] != "" && str11[15] != "")  //相同卡号或支付宝账号（不能都是空），且金融机构相同
						{
							itotalsyn--;   isynnum--;
							std::string strfirst = Uidtemp->first;			int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");
								INT64 isut = _atoi64(Uidmap->second.lUT.c_str());				INT64 icut = _atoi64(Uidtemp->second.lUT.c_str());
								if(isut < icut)
								{
									Uidmap->second.iupdown = 10;
									Uidmap->second.lUT = Uidtemp->second.lUT;   Uidmap->second.strdata = Uidtemp->second.strdata;
								}
							Uidtemp->second.iupdown = 3;			//iii =1000;								break;
							strd1 = "UPDATE "; 	std::string  strtable = "2";	pBankData->TableStr (strtable );
							strd1 += strtable;                                                  strd1 += " SET mark='1'";
							strd1 += " WHERE id='";			strd1 += /*Uidtemp->first*/strfirst;					strd1 += "';";
							int ret = pBankData->ExecuteSQL(strd1);

							std::string  strd1,strd2;
							UserIDMap::iterator Uidup = pBankData->GetUserIDMap()->begin();
							for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
							{
								std::string strd1 = Uidtemp->first;			int dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
								std::string strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
								//strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
								if( Uidup->second.iTableNum == "11" )
								{
									strfor = Uidup->second.strdata;
									dpos = strfor.find(strd1); //
									if(dpos != string::npos)
									{
										strfirst = Uidup->first;		int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
										strfor.replace(dpos,strd1.size(),strd2);
										Uidup->second.strdata = strfor;
										strd1 = "UPDATE "; 	std::string  strtable = "11";	pBankData->TableStr (strtable );
										strd1 += strtable;        strd1 += " SET tbAccount_id='";    strd1 +=  strd2;        strd1 +=  "'";      
										strd1 += " WHERE id='";			strd1 += /*Uidup->first*/strfirst;					strd1 += "';";
										int ret = pBankData->ExecuteSQL(strd1);
									}
								}
								strd1 = Uidtemp->first;			    dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
								strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
//								strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
								if( Uidup->second.iTableNum == "7" )
								{
									strfor = Uidup->second.strdata;
									dpos = strfor.find(strd1); //
									if(dpos != string::npos)
									{
										strfirst = Uidup->first;		 int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
										strfor.replace(dpos,strd1.size(),strd2);
										Uidup->second.strdata = strfor;
										strd1 = "UPDATE "; 	std::string  strtable = "7";	pBankData->TableStr (strtable );
										strd1 += strtable;        strd1 += " SET tbAccount_id='";    strd1 +=  strd2;        strd1 +=  "'";      
										strd1 += " WHERE id='";			strd1 += /*Uidup->first*/strfirst;					strd1 += "';";
										int ret = pBankData->ExecuteSQL(strd1);
									}
								}
								strd1 = Uidtemp->first;			    dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
								strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
//								strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
								if( Uidup->second.iTableNum == "3" )
								{
									strfor = Uidup->second.strdata;
									dpos = strfor.find(strd1); //
									if(dpos != string::npos)
									{
										strfirst = Uidup->first;		 int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
										strfor.replace(dpos,strd1.size(),strd2);
										Uidup->second.strdata = strfor;
										strd1 = "UPDATE "; 	std::string  strtable = "3";	pBankData->TableStr (strtable );
										strd1 += strtable;        strd1 += " SET tbAccount_id='";    strd1 +=  strd2;        strd1 +=  "'";      
										strd1 += " WHERE id='";			strd1 += /*Uidup->first*/strfirst;					strd1 += "';";
										int ret = pBankData->ExecuteSQL(strd1);
									}
								}


							}//for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
						}
					}
				}//for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)
			}//if( Uidmap->second.iTableNum == "4" && itemp == 6)
		}
		MergetbsubAccount();

}

void  CSynchroDlg::MergetbsubAccount()
{
		size_t dpos = 0;    string tem = "";  string strfor = "";      string strd1 = ""; int iii = 0;
		CBankData* pBankData = CBankData::GetInstance();

			for(int i=0 ; i< 30; i++)
				str11[i] = "";
		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum == "3" && itemp == 6)  //server
			{
				strfor = Uidmap->second.strdata;
				for(int k=0 ; k< 10; k++)  
				{
					dpos = strfor.find(":"); //
					if(dpos == string::npos)
						break;
					str11[k] = strfor.substr(0 ,dpos);
					strfor.replace(0,dpos+1,tem);
				}
				UserIDMap::iterator Uidtemp = pBankData->GetUserIDMap()->begin();
				for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)// client
				{
					itemp = Uidtemp->second.iupdown;
					if( Uidtemp->second.iTableNum == "3" && itemp == 4)
					{
		        		strfor = Uidtemp->second.strdata;
						for(int k=10 ; k< 20; k++)  
						{
							dpos = strfor.find(":"); //
							if(dpos == string::npos)
								break;
							str11[k] = strfor.substr(0 ,dpos);
							strfor.replace(0,dpos+1,tem);
						}
						if(str11[0] == str11[10] && str11[1] == str11[11] && (str11[8] == "201" || str11[8] == "304"))  //主账户ID 、子账户名称（即币种名）	信用卡账户
						{
							itotalsyn--;   isynnum--;
							std::string strfirst = Uidtemp->first;			int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");
								INT64 isut = _atoi64(Uidmap->second.lUT.c_str());				INT64 icut = _atoi64(Uidtemp->second.lUT.c_str());
								if(isut < icut)
								{
									Uidmap->second.iupdown = 10;
									Uidmap->second.lUT = Uidtemp->second.lUT;   Uidmap->second.strdata = Uidtemp->second.strdata;
								}
							Uidtemp->second.iupdown = 3;			//iii =1000;								break;
							strd1 = "UPDATE "; 	std::string  strtable = "3";	pBankData->TableStr (strtable );
							strd1 += strtable;                                                  strd1 += " SET mark='1'";
							strd1 += " WHERE id='";			strd1 += /*Uidtemp->first*/strfirst;					strd1 += "';";
							int ret = pBankData->ExecuteSQL(strd1);

							std::string  strd1,strd2;
							UserIDMap::iterator Uidup = pBankData->GetUserIDMap()->begin();
							for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
							{
								strd1 = Uidtemp->first;			    dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
								strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
//								strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
								if( Uidup->second.iTableNum == "7" )
								{
									strfor = Uidup->second.strdata;
									dpos = strfor.find(strd1); //
									if(dpos != string::npos)
									{
										strfirst = Uidup->first;		 int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
										strfor.replace(dpos,strd1.size(),strd2);
										Uidup->second.strdata = strfor;
										strd1 = "UPDATE "; 	std::string  strtable = "7";	pBankData->TableStr (strtable );
										strd1 += strtable;        strd1 += " SET tbSubAccount_id='";    strd1 +=  strd2;        strd1 +=  "'";      
										strd1 += " WHERE id='";			strd1 += /*Uidup->first*/strfirst;					strd1 += "';";
										int ret = pBankData->ExecuteSQL(strd1);
									}
								}
								strd1 = Uidtemp->first;			    dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
								strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
//								strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
								if( Uidup->second.iTableNum == "9" )
								{
										strfirst = Uidup->first;		 int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
									strfor = Uidup->second.strdata;
									dpos = strfor.find(strd1); //
									if(dpos != string::npos)
									{
										for(int i=0 ; i< 30; i++)
											str11[i] = "";
										strfor.replace(dpos,strd1.size(),strd2);
										Uidup->second.strdata = strfor;
										for(int k=0 ; k< 10; k++)  
										{
											dpos = strfor.find(":"); //
											if(dpos == string::npos)
												break;
											str11[k] = strfor.substr(0 ,dpos);
											strfor.replace(0,dpos+1,tem);
										}

										strd1 = "UPDATE "; 	std::string  strtable = "9";	pBankData->TableStr (strtable );
										strd1 += strtable;    
										if(str11[5] == strd2)
								    		strd1 += " SET tbSubAccount_id='"; 
										else if(str11[6] == strd2)
								    		strd1 += " SET tbSubAccount_id1='"; 
										strd1 +=  strd2;        strd1 +=  "'";      
										strd1 += " WHERE id='";			strd1 += /*Uidup->first*/strfirst;					strd1 += "';";
										int ret = pBankData->ExecuteSQL(strd1);
									}
								}


							}//for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
						}
					}
				}//for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)
			}//if( Uidmap->second.iTableNum == "4" && itemp == 6)
		}
		//MergetbsubAccount();

}

void  CSynchroDlg::MergetbCategory1()
{
		size_t dpos = 0;    string tem = "";  string strfor = "";      string strd1 = ""; int iii = 0;
		CBankData* pBankData = CBankData::GetInstance();
		//int ifor = pBankData->GetUserIDMap()->size();
	 //   for(int i=0 ; i< ifor; i++)
		//{
			for(int i=0 ; i< 30; i++)
				str11[i] = "";
    		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
			for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
			{
				int itemp = Uidmap->second.iupdown;
				if( Uidmap->second.iTableNum == "4" && itemp == 6)  //server
				{
					strfor = Uidmap->second.strdata;
					for(int k=0 ; k< 3; k++)  
					{
						dpos = strfor.find(":"); //
						if(dpos == string::npos)
							break;
						str11[k] = strfor.substr(0 ,dpos);
						strfor.replace(0,dpos+1,tem);
					}
    				UserIDMap::iterator Uidtemp = pBankData->GetUserIDMap()->begin();
					for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)// client
					{
						itemp = Uidtemp->second.iupdown;
						if( Uidtemp->second.iTableNum == "4" && itemp == 4)
						{
			        		strfor = Uidtemp->second.strdata;
							for(int k=3 ; k< 6; k++)  
							{
								dpos = strfor.find(":"); //
								if(dpos == string::npos)
									break;
								str11[k] = strfor.substr(0 ,dpos);
								strfor.replace(0,dpos+1,tem);
							}
							if(str11[0] == str11[3] && str11[1] == str11[4])  //4 类别（收/支），名称
							{
								itotalsyn--;   isynnum--;
			    				std::string strfirst = Uidtemp->first;			int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");
								INT64 isut = _atoi64(Uidmap->second.lUT.c_str());				INT64 icut = _atoi64(Uidtemp->second.lUT.c_str());
								if(isut < icut)
								{
									Uidmap->second.iupdown = 10;
									Uidmap->second.lUT = Uidtemp->second.lUT;   Uidmap->second.strdata = Uidtemp->second.strdata;
								}
								Uidtemp->second.iupdown = 3;			//iii =1000;								break;
								strd1 = "UPDATE "; 	std::string  strtable = "4";	pBankData->TableStr (strtable );
								strd1 += strtable;                                                  strd1 += " SET mark='1'";
								strd1 += " WHERE id='";			strd1 += /*Uidtemp->first*/strfirst;					strd1 += "';";
								int ret = pBankData->ExecuteSQL(strd1);

								std::string  strd1,strd2;
								UserIDMap::iterator Uidup = pBankData->GetUserIDMap()->begin();
								for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
								{
									strd1 = Uidtemp->first;			int dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
									strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
//									strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
									if( Uidup->second.iTableNum == "5" )
									{
										for(int i=10 ; i< 30; i++)
											str11[i] = "";
			        					strfor = Uidup->second.strdata;
										for(int k=10 ; k< 16; k++)  
										{
											dpos = strfor.find(":"); //
											if(dpos == string::npos)
												break;
											str11[k] = strfor.substr(0 ,dpos);
											strfor.replace(0,dpos+1,tem);
										}
										std::string  strd3 = str11[11];
										if(strd3 == strd1)
										//strfor = Uidup->second.strdata;
										//dpos = strfor.find(strd1); //
										//if(dpos != string::npos)
										{
							    			strfirst = Uidup->first;		 int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
											//strfor.replace(dpos,strd1.size(),strd2);
											strfor = str11[10];    strfor += ":";	strfor += strd2;    strfor += ":";	strfor += str11[12];    strfor += ":";
											Uidup->second.strdata = strfor;
											strd1 = "UPDATE "; 	std::string  strtable = "5";	pBankData->TableStr (strtable );
											strd1 += strtable;        strd1 += " SET tbCategory1_id='";    strd1 +=  strd2;        strd1 +=  "'";      
											strd1 += " WHERE id='";			strd1 += /*Uidup->first*/strfirst;					strd1 += "';";
											int ret = pBankData->ExecuteSQL(strd1);
										}
									}
								}//for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
							}
						}
					}//for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)
				}//if( Uidmap->second.iTableNum == "4" && itemp == 6)
			}
		//}//for(int i=0 ; i< ifor; i++)
		MergetbCategory2();
}

void  CSynchroDlg::MergetbCategory2()
{
		size_t dpos = 0;    string tem = "";  string strfor = "";      string strd1 = ""; int iii = 0;
		CBankData* pBankData = CBankData::GetInstance();

			for(int i=0 ; i< 30; i++)
				str11[i] = "";
		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum == "5" && itemp == 6)  //server
			{
				strfor = Uidmap->second.strdata;
				for(int k=0 ; k< 3; k++)  
				{
					dpos = strfor.find(":"); //
					if(dpos == string::npos)
						break;
					str11[k] = strfor.substr(0 ,dpos);
					strfor.replace(0,dpos+1,tem);
				}
				UserIDMap::iterator Uidtemp = pBankData->GetUserIDMap()->begin();
				for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)// client
				{
					itemp = Uidtemp->second.iupdown;
					if( Uidtemp->second.iTableNum == "5" && itemp == 4)
					{
		        		strfor = Uidtemp->second.strdata;
						for(int k=3 ; k< 6; k++)  
						{
							dpos = strfor.find(":"); //
							if(dpos == string::npos)
								break;
							str11[k] = strfor.substr(0 ,dpos);
							strfor.replace(0,dpos+1,tem);
						}
						if(str11[0] == str11[3] && str11[1] == str11[4])    //所属主分类（ID），名称
						{
							itotalsyn--;   isynnum--;
			    				std::string strfirst = Uidtemp->first;			int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");
								INT64 isut = _atoi64(Uidmap->second.lUT.c_str());				INT64 icut = _atoi64(Uidtemp->second.lUT.c_str());
								if(isut < icut)
								{
									Uidmap->second.iupdown = 10;
									Uidmap->second.lUT = Uidtemp->second.lUT;   Uidmap->second.strdata = Uidtemp->second.strdata;
								}
							Uidtemp->second.iupdown = 3;			//iii =1000;								break;
							strd1 = "UPDATE "; 	std::string  strtable = "5";	pBankData->TableStr (strtable );
							strd1 += strtable;                                                  strd1 += " SET mark='1'";
							strd1 += " WHERE id='";			strd1 += strfirst/*Uidtemp->first*/;					strd1 += "';";
							int ret = pBankData->ExecuteSQL(strd1);

							std::string  strd1,strd2;
							UserIDMap::iterator Uidup = pBankData->GetUserIDMap()->begin();
							for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
							{
									strd1 = Uidtemp->first;			int dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
									strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
//								strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
								if( Uidup->second.iTableNum == "9" )
								{
										for(int i=10 ; i< 30; i++)
											str11[i] = "";
			        					strfor = Uidup->second.strdata;
										for(int k=10 ; k< 26; k++)  
										{
											dpos = strfor.find(":"); //
											if(dpos == string::npos)
												break;
											str11[k] = strfor.substr(0 ,dpos);
											strfor.replace(0,dpos+1,tem);
										}
										if(str11[12] == strd1)
									//strfor = Uidup->second.strdata;
									//dpos = strfor.find(strd1); //
									//if(dpos != string::npos)
									{
							    			strfirst = Uidup->first;		 int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
										//strfor.replace(dpos,strd1.size(),strd2);
										strfor = "";
										for(int k=10 ; k< 22; k++)  
										{
											if(k == 12){
												strfor += strd2;		strfor += ":";}
											else{
												strfor += str11[k];		strfor += ":";}
										}
										Uidup->second.strdata = strfor;
										strd1 = "UPDATE "; 	std::string  strtable = "9";	pBankData->TableStr (strtable );
										strd1 += strtable;        strd1 += " SET tbCategory2_id='";    strd1 +=  strd2;        strd1 +=  "'";      
										strd1 += " WHERE id='";			strd1 += /*Uidup->first*/strfirst;					strd1 += "';";
										int ret = pBankData->ExecuteSQL(strd1);
									}
								}
							}//for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
						}
					}
				}//for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)
			}//if( Uidmap->second.iTableNum == "4" && itemp == 6)
		}
		MergetbPayee();
}

void  CSynchroDlg::MergetbPayee()
{
		size_t dpos = 0;    string tem = "";  string strfor = "";      string strd1 = ""; int iii = 0;
		CBankData* pBankData = CBankData::GetInstance();

			for(int i=0 ; i< 30; i++)
				str11[i] = "";
		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum == "6" && itemp == 6)  //server
			{
				strfor = Uidmap->second.strdata;
				for(int k=0 ; k< 3; k++)  
				{
					dpos = strfor.find(":"); //
					if(dpos == string::npos)
						break;
					str11[k] = strfor.substr(0 ,dpos);
					strfor.replace(0,dpos+1,tem);
				}
				UserIDMap::iterator Uidtemp = pBankData->GetUserIDMap()->begin();
				for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)// client
				{
					itemp = Uidtemp->second.iupdown;
					if( Uidtemp->second.iTableNum == "6" && itemp == 4)
					{
		        		strfor = Uidtemp->second.strdata;
						for(int k=3 ; k< 6; k++)  
						{
							dpos = strfor.find(":"); //
							if(dpos == string::npos)
								break;
							str11[k] = strfor.substr(0 ,dpos);
							strfor.replace(0,dpos+1,tem);
						}
						if(str11[0] == str11[3] )  //name
						{
							itotalsyn--;   isynnum--;
			    				std::string strfirst = Uidtemp->first;			int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");
								INT64 isut = _atoi64(Uidmap->second.lUT.c_str());				INT64 icut = _atoi64(Uidtemp->second.lUT.c_str());
								if(isut < icut)
								{
									Uidmap->second.iupdown = 10;
									Uidmap->second.lUT = Uidtemp->second.lUT;   Uidmap->second.strdata = Uidtemp->second.strdata;
								}
							Uidtemp->second.iupdown = 3;			//iii =1000;								break;
							strd1 = "UPDATE "; 	std::string  strtable = "6";	pBankData->TableStr (strtable );
							strd1 += strtable;                                                  strd1 += " SET mark='1'";
							strd1 += " WHERE id='";			strd1 += /*Uidtemp->first*/strfirst;					strd1 += "';";
							int ret = pBankData->ExecuteSQL(strd1);

							std::string  strd1,strd2;
							UserIDMap::iterator Uidup = pBankData->GetUserIDMap()->begin();
							for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
							{
									strd1 = Uidtemp->first;			int dpos = strd1.size() - 3; 		strd1.replace(dpos ,3, "");
									strd2 = Uidmap->first;			    dpos = strd2.size() - 3; 		strd2.replace(dpos ,3, "");
//								strd1 = Uidtemp->first;   strd2 = Uidmap->first; //4
								if( Uidup->second.iTableNum == "9" )
								{
									strfor = Uidup->second.strdata;
									dpos = strfor.find(strd1); //
									if(dpos != string::npos)
									{
							    			strfirst = Uidup->first;		 int dpos1 = strfirst.size() - 3; 	strfirst.replace(dpos1 ,3, "");
										strfor.replace(dpos,strd1.size(),strd2);
										Uidup->second.strdata = strfor;
										strd1 = "UPDATE "; 	std::string  strtable = "9";	pBankData->TableStr (strtable );
										strd1 += strtable;        strd1 += " SET tbPayee_id='";    strd1 +=  strd2;        strd1 +=  "'";      
										strd1 += " WHERE id='";			strd1 += strfirst/*Uidup->first*/;					strd1 += "';";
										int ret = pBankData->ExecuteSQL(strd1);
									}
								}
							}//for(;Uidup != pBankData->GetUserIDMap()->end();Uidup ++)//down
						}
					}
				}//for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)
			}//if( Uidmap->second.iTableNum == "4" && itemp == 6)
		}
		MergetbBank();
}

void  CSynchroDlg::MergetbEvent()
{
}

void  CSynchroDlg::MergetbTransaction()
{
		size_t dpos = 0;    string tem = "";  string strfor = "";   string strd1 = ""; int iii = 0;
		CBankData* pBankData = CBankData::GetInstance();

			for(int i=0 ; i< 30; i++)
				str11[i] = "";
		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			int itemp = Uidmap->second.iupdown;
			if( Uidmap->second.iTableNum == "9" && itemp == 6)  //server
			{
				strfor = Uidmap->second.strdata;
				for(int k=0 ; k< 12; k++)  
				{
					dpos = strfor.find(":"); //
					if(dpos == string::npos)
						break;
					str11[k] = strfor.substr(0 ,dpos);
					strfor.replace(0,dpos+1,tem);
				}
				UserIDMap::iterator Uidtemp = pBankData->GetUserIDMap()->begin();
				for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)// client
				{
					itemp = Uidtemp->second.iupdown;
					if( Uidtemp->second.iTableNum == "9" && itemp == 4)
					{
		        		strfor = Uidtemp->second.strdata;
						for(int k=12 ; k< 24; k++)  
						{
							dpos = strfor.find(":"); //
							if(dpos == string::npos)
								break;
							str11[k] = strfor.substr(0 ,dpos);
							strfor.replace(0,dpos+1,tem);
						}
						if(str11[10] == "1" && str11[22] == "1")//自动导入的账目/*&& Uidmap->second.imark == Uidtemp->second.imark*/
						{
							if(str11[0] == str11[12] && str11[3] == str11[15] && str11[5] == str11[17] && str11[2] == str11[14])  //同一天的同一条交易
							{
			    				std::string strfirst = Uidtemp->first;			int dpos = strfirst.size() - 3; 		strfirst.replace(dpos ,3, "");
								itotalsyn--;   isynnum--;
								INT64 isut = _atoi64(Uidmap->second.lUT.c_str());				INT64 icut = _atoi64(Uidtemp->second.lUT.c_str());
								if(isut < icut)
								{
									Uidmap->second.iupdown = 10;
									Uidmap->second.lUT = Uidtemp->second.lUT;   Uidmap->second.strdata = Uidtemp->second.strdata;
								}
								Uidtemp->second.iupdown = 3;			//iii =1000;								break;
								strd1 = "UPDATE "; 	std::string  strtable = "9";	pBankData->TableStr (strtable );
								strd1 += strtable;                                                  strd1 += " SET mark='1'";
								strd1 += " WHERE id='";			strd1 += strfirst/*Uidtemp->first*/;					strd1 += "';";
								int ret = pBankData->ExecuteSQL(strd1);
							}
						}
					}
				}//for(;Uidtemp != pBankData->GetUserIDMap()->end();Uidtemp ++)
			}//if( Uidmap->second.iTableNum == "4" && itemp == 6)
		}
}

void  CSynchroDlg::MergetbProductChoice()
{
}

