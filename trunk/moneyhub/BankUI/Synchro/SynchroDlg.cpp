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
//#pragma comment(lib, "wininet.lib")


#define POSTHOST _T("http://moneyhub.ft.com")
// 要发送的数据ID列表地址信息  
#define POSTLIST_URL _T("/server/get_list.php")
// 要发送的下载地址信息    moneyhub.ft.com/server/get_download_data.php
#define POSTDOWN_URL _T("/server/get_download_data.php")
// 要发送的上传地址信息
#define POSTUP_URL _T("/server/get_upload_data.php")
// 服务器时间
#define POSTSERVER_TIME _T("/server/verify_time.php")
// 解锁
#define POSTUSER_CLEAR _T("/server/UserClear.php")

void remove_space(string& str)
{ 
	string buff(str); 
	char space = ' '; 
	str.assign(buff.begin() + buff.find_first_not_of(space), 
	buff.begin() + buff.find_last_not_of(space) + 1); 
} 

CSynchroDlg::CSynchroDlg(/*ThreadDataStruct* pTds*/)
: m_bFinished(false), m_bFailed(false), m_bCanceled(false), m_nPercent(0), m_rectProgress(76, 95, 346, 108)
{
	strresidue = "";
	stransi = "";
	struserinfo = "";
	m_bSP2D = FALSE;
	isBase64 = TRUE;
	BSend  = FALSE;
	strBig30 = "";
	dwIndex = 0;
	isynnum =0;
	//m_pInputTds = pTds;
	m_sHostName =  CHostContainer::GetInstance()->GetHostName(kWeb).c_str();
	m_sHostName.Delete(0,7);    m_sHostName.Delete(m_sHostName.GetLength()-1,1);
	m_sRequest = POSTUP_URL;
	m_sHostName = POSTHOST ;
	m_sHostName +=  POSTSERVER_TIME;
	//m_sHostName = _T("127.0.0.1");
	//m_sRequest = _T("/file.php");

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
	CBankData::GetInstance()->CloseDB();
}

LRESULT CSynchroDlg::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	CRect rect(80, 110, 420, 400);
	
	if(dwIndex != 0)
		s()->StatusBar()->DrawDownloadProgress(dc, rect, m_nPercent);
	

	//s()->StatusBar()->DrawDownloadProgress(dc, rect, 30);

	return 0;
}

LRESULT CSynchroDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	SetWindowText(_T("数据同步"));

	if (NULL == g_pSkin)
		new CSkinManager();

	g_pSkin->RefreshProgressSkin();

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

	//m_pInputTds->hProgDialog = m_hWnd;
	SetEvent(m_tds.hCreateEvent);

	//m_sAgent = _T("Mozilla/4.0 (compatible; MSIE 6.0;Windows NT 5.1)");

	WCHAR sInfo[256] = { 0 }; BOOL b_ret = TRUE;
	if(!InternetCheckConnection(L"http://www.baidu.com", FLAG_ICC_FORCE_CONNECTION, 0) )
	{
		swprintf(sInfo, 256, L"当前网络故障,请连接互联网后同步!");
		b_ret = FALSE;
		MessageBoxW( sInfo, L"财金汇", MB_OK);
		DoSysClose();
    	return b_ret;
	}
	if(!InternetCheckConnection(POSTHOST, FLAG_ICC_FORCE_CONNECTION, 0) )
	{
		swprintf(sInfo, 256, L"服务器故障,请联系财金汇后同步!");
		b_ret = FALSE;
		MessageBoxW( sInfo, L"财金汇", MB_OK);
		DoSysClose();
    	return b_ret;
	}

	struserinfo = "119@110.com#";  //email
	struserinfo += CSNManager::GetInstance()->GetSN() + "#";
	struserinfo += GenHWID2() + "#";
	struserinfo += "xrzwW4FA20141110470aec4f3e968a50a7cc54d931ac5bcbc65a4d78f63ea49f4723f809cc389acd#";  //STOKEN
	std::string strtemp = struserinfo + CBankData::GetInstance()->strDBVer();                   strtemp += "#";	

	std::string  strd = "xml=";



	m_titleString.Attach(GetDlgItem(IDC_STATIC_FILENAMES));

		LPCTSTR lpszFontName[] = { _T("Tahoma"), _T("微软雅黑") };

		int iIndex = IsVista() ? 1 : 0;
		if (NULL == m_tFontText)
			m_tFontText.CreateFont(19, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
		//m_tTextColor = RGB(16, 93, 145);



					//::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
					//::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
					//CString strFileName = "正在同步数据，请稍候......!";
					//SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
					m_titleString.SetFont(m_tFontText);

					//::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
					////strFileName.Format(_T("已完成%d"), lProgress);
					//SetDlgItemText(IDC_STATIC_UNKNOWNLENGTHS, strFileName);
					//::InvalidateRect(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), NULL, TRUE);

	SendPData(strd ,strtemp);

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

	m_bFinished = true;
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

	m_bFailed = true;
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
	//if (!m_bFailed && !m_bFinished)
	{
		m_bCanceled = true;
		::EnableWindow(GetDlgItem(IDCANCEL), FALSE);
	}
	//else
	{
		EndDialog(IDCANCEL);
	}

	return true;
}

LRESULT CSynchroDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoSysClose();

	return 0;
}

LRESULT CSynchroDlg::Onsyn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
		CBankData* pBankData = CBankData::GetInstance();
		std::string strtemp = pBankData->strDBVer();
		strtemp = struserinfo + strtemp + "#";

	//strtemp = "630242631@qq.com;4;33;33;";
//	CStringA strtemp = "630242631@qq.com;4;33;33;";
	//CString strd = ",2,123456789005126|发送到服务器的数据|发送到服务器的数据";
	std::string strd = ",2,123456789005126|eeeee|ggggggggggggggggggg";
	//for(int i=0 ; i<10 ;i++)
	//{
	//	strd += strd;
	//}
	//strtemp += strd;
	//strtemp += ",|发送到服务器的数据";
	//strtemp = "630242631@qq.com;4;33;33;2,123456789005126";
	//strtemp = "";
	strd = ("xml=");

	SendPData(strd,strtemp);
	return S_OK;
}

void CSynchroDlg::SendPData (std::string strxml ,std::string strpdata)
{
	std::string  strd1 = "UPDATE tbBank SET id='1293811200000', name='CATA420',classId='',Phone='',Website='' WHERE BankID='66';";

	CBankData* pBankData = CBankData::GetInstance();
					//pBankData->GfieldName ();
					//pBankData->GetList (0);
     //int ret = pBankData->ExecuteSQL(strd1);


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
	if(CPostData::getInstance()->PostIniBigData(m_sHostName ) != 0)
	{
		if(m_bSP2D){
			m_bSP2D = FALSE;			ErrorSP2D();}
		 //   WCHAR sInfo[256] = { 0 }; 
			//swprintf(sInfo, 256, L"当前网络出现故障,请联系财金汇后同步!");
			//MessageBoxW( sInfo, L"财金汇", MB_OK);
			//DoSysClose();
	}
	if(CPostData::getInstance()->UseHttpSendReqEx(strxml))
     	ManageData(strxml);
	else
	{
		if(m_bSP2D){
			m_bSP2D = FALSE;			ErrorSP2D();}
		dwIndex =0;   BSend = FALSE;
		::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_HIDE);

		CString strFileName = "网络故障,请联系财金汇后同步!";
		SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
		::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
		strFileName = "确定";
		SetDlgItemText(IDCANCEL, strFileName);
		::InvalidateRect(GetDlgItem(IDCANCEL), NULL, TRUE);
		 //   WCHAR sInfo[256] = { 0 }; 
			//swprintf(sInfo, 256, L"网络故障,请联系财金汇后同步!");
			//MessageBoxW( sInfo, L"财金汇", MB_OK);
			//DoSysClose();

		Invalidate();
	}
}

void CSynchroDlg::ManageData (std::string  strre)
{
	USES_CONVERSION;
	string tem = "";	string dddt = "";	size_t dpos = 0;

	//strre = "  11#1#'34322333'#'6777666'#'7'#'0'#'1'#'4235553'#563455#7#0#   ";
	//strre = "  82#   ";

	//if(m_bFirst)
	{
     	remove_space(strre);
		//m_bFirst =FALSE;
		dpos = strre.find("#");
		if(dpos != std::string::npos)
		{
	    	CBankData* pBankData = CBankData::GetInstance();
	    	dddt = strre.substr(0 ,dpos);
			dddt = pBankData->FilterStringNumber(dddt);
		    strre.replace(0,dpos+1,tem);
		}
		if(dddt != "")
    		dwIndex = atoi(dddt.c_str());
		else
        	dwIndex = 0;
	}
	
	int ifor =  strre.size();
	//UINT64 u64id;
	switch(dwIndex)
	{	
		//case 0:
		//	{
		//		WCHAR sInfo[256] = { 0 }; 
		//			swprintf(sInfo, 256, L"当前网络故障,请连接互联网后同步!");
//					b_ret = FALSE;
//					MessageBoxW( sInfo, L"财金汇", MB_OK);
//					DoSysClose();
			//}
			//return;
		case 11:
			{
				ForData (strre ,5, ifor);
					//for(int i=0 ; i< ifor; i++)
					//{
			  //  	    int kkk = 5;
					//	ForData (strre ,kkk);
					//	if(kkk != 5)
					//		break;
					//}
				    //isynnum = CBankData::GetInstance()->GetList(strBig30);
			}
			break;
		case 13:
			{
				ForAddData (strre ,6, ifor);
			}
			break;
		case 21:
		case 22:
			{
				ForData (strre ,6, ifor);
			}
			break;
		case 91:
			{
					//SYSTEMTIME systm;
					//GetSystemTime(&systm);
					time_t rawtime; 					struct tm * timeinfo; 					time ( &rawtime );
					_int64 imsecond = rawtime*1000;
					//timeinfo = gmtime ( &rawtime );      1320825106      1293811200000
					dpos = strre.find("#");
					if(dpos == string::npos){
						dwIndex =92;						break;
					}
					dddt = strre.substr(0 ,dpos);
					_int64  le = _atoi64( dddt.c_str() );         _int64  i64t = le;					le -= 1293811200000;
					char   sID[64] ={0}; 					_i64toa(le,sID ,10);
					if(imsecond - i64t > 1800000 )
						strBig30 = sID;
			}
			break;
		default:
			break;
	}
	BSend = TRUE;

	//NextPData (dwIndex);
//	ddd.replace(ifor - strresidue.size() ,strresidue.size() ,tem);
//	char* tmp;
//	while(tmp = strchr(dotPointer, ','))
//		dotPointer = tmp + 1;
//	strresidue = dotPointer;
}

void   CSynchroDlg::ForData (std::string strfor ,int icase, int ifor)
{
	//table1#ID1 #UT1#     version1#mark#DATA#
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
			CBankData::GetInstance()->GetUserIDMap()->insert(std::make_pair(str11[1], uddata));
		}
		else if(icase == 6 )  //21 22
		{
			UserIDMap::iterator Uidmap = CBankData::GetInstance()->GetUserIDMap()->find(str11[1]);
			if (Uidmap != CBankData::GetInstance()->GetUserIDMap()->end())
			{
				Uidmap->second.strdata = str11[5];
				if(Uidmap->second.iupdown == 0)
					Uidmap->second.iupdown = 6;
				else if(Uidmap->second.iupdown == 2)
					Uidmap->second.iupdown = 7;
			}
		}
	}
}

void   CSynchroDlg::ForAddData (std::string strfor ,int icase, int ifor)
{
	//0 table#  1 ID    2 #UT1#     3version1# 4mark#   5DATA#
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
		CBankData* pBankData = CBankData::GetInstance();
		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->find(str11[1]);
		if (Uidmap != pBankData->GetUserIDMap()->end())
		{
			_int64 icut = _atoi64(Uidmap->second.lUT.c_str());
			_int64 isut = _atoi64(str11[2].c_str());
			if(icut > isut)
			{
				Uidmap->second.iupdown = 1;
				//Uidmap->second.iTableNum = strtb;
				if(strBig30 != "")
				{
					Uidmap->second.lUT = strBig30;	                std::string  strd1 = "UPDATE ";
					std::string  strtable = Uidmap->second.iTableNum;					pBankData->TableStr (strtable );
					strd1 += strtable;                                                  strd1 += " SET UT='";
					strd1 += strBig30;                                                  strd1 += "' WHERE id='";
					strd1 += str11[1];                                                  strd1 += "';";
                    int ret = pBankData->ExecuteSQL(strd1);
				}
			}
			else if(icut < isut)
			{
				Uidmap->second.iupdown = 7;
				Uidmap->second.lUT = str11[2];
				Uidmap->second.idbver = str11[3];
				Uidmap->second.imark = str11[4] ;
				Uidmap->second.strdata = str11[5] ;
			}
			else
				Uidmap->second.iupdown = 3;
		}
	}
	RefreshDB();
}

void CSynchroDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(BSend)
	{
		CBankData* pBankData = CBankData::GetInstance();
		std::string strtable = "";
		BSend = FALSE;
		switch(dwIndex)
		{	
			case 11:
				{
					pBankData->GfieldName ();					isynnum =  pBankData->GetList (strBig30);//strBig30
					P1DOWN();
				}
				break;
			case 12:
				{
					::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_HIDE);
					::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
					::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
					CString strFileName = "当前用户正在其他机器同步,请稍候再试!";
					SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
					::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
					strFileName = "确定";
					SetDlgItemText(IDCANCEL, strFileName);
					::InvalidateRect(GetDlgItem(IDCANCEL), NULL, TRUE);
				}
				break;
			case 13:
				{
					P1DOWN();
				}
				break;
			case 21:
				{
					RefreshDB();					P2DOWN();
				}
				break;
			case 22:
				{
					RefreshDB();					P2DOWN();
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
			        		isynnum--;
							Uidmap->second.iupdown = 3;
							Uidmap->second.strdata = "";
						}
					}
					strPost2D();
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
			        		isynnum--;
							Uidmap->second.iupdown = 3;
							Uidmap->second.strdata = "";
						}
					}
					strPost2D();
				}
				break;
			case 51:
				{
				}
				break;
			case 61:
				{
				}
				break;
			case 62:
			case 63:
				{
					if(m_bSP2D){
						m_bSP2D = FALSE;			ErrorSP2D();}
					::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_HIDE);
					::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
					::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
					CString strFileName = "登录验证失败，您需要重新登录!";
					SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
					::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
					strFileName = "确定";
					SetDlgItemText(IDCANCEL, strFileName);
					::InvalidateRect(GetDlgItem(IDCANCEL), NULL, TRUE);
				}
				break;
			case 81:
			case 82:
				{
					if(m_bSP2D){
						m_bSP2D = FALSE;			ErrorSP2D();}
					::ShowWindow(GetDlgItem(IDCANCEL), SW_HIDE);
					CString strFileName = "同步失败，可能是您的网络状况不佳，您可以尝试重新同步!";
					SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
					::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
					//strFileName.Format(_T("已完成%d"), lProgress);
					SetDlgItemText(IDC_STATIC_UNKNOWNLENGTHS, strFileName);
					::InvalidateRect(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), NULL, TRUE);
				}
				break;
			case 83:
				{
					::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_HIDE);
					::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
					::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
					CString strFileName = "您当前使用的财金汇版本较旧，无法执行同步，建议升级至最新版本!";
					SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
					::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
					strFileName = "确定";
					SetDlgItemText(IDCANCEL, strFileName);
					::InvalidateRect(GetDlgItem(IDCANCEL), NULL, TRUE);
				}
				break;
			case 91:
				{
					::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
					::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
					CString strFileName = "正在同步数据，请稍候......!";
					SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
					::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
					//strFileName.Format(_T("已完成%d"), lProgress);
					SetDlgItemText(IDC_STATIC_UNKNOWNLENGTHS, strFileName);
					::InvalidateRect(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), NULL, TRUE);
					PGList();
				}
				break;
			case 92:
				{
				}
				break;
			default:
				{
	            	::ShowWindow(GetDlgItem(IDC_STATIC_UNKNOWNLENGTHS), SW_HIDE);
					::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
					::ShowWindow(GetDlgItem(IDC_BUTTONsyn), SW_HIDE);
					CString strFileName = "同步失败，可能是您的网络状况不佳，您可以尝试重新同步!";
					SetDlgItemText(IDC_STATIC_FILENAMES, strFileName);
					::InvalidateRect(GetDlgItem(IDC_STATIC_FILENAMES), NULL, TRUE);
					strFileName = "确定";
					SetDlgItemText(IDCANCEL, strFileName);
					::InvalidateRect(GetDlgItem(IDCANCEL), NULL, TRUE);
				}
				break;
		}
	}

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
				strtemp1 += Uidmap->first;      strtemp1 += "#";
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
			m_sHostName = POSTHOST ;			m_sHostName +=  POSTUP_URL;
			strtemp += strtemp1 ;			    std::string strxml = "xml=";
    		SendPData(strxml,strtemp);
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
		    	ipnum++;       if(ipnum == 100)  break;
				if(itemp == 4){
					strtemp1 += "31#";       Uidmap->second.iupdown = 8; } //insert
				else if(itemp == 1){
					strtemp1 += "32#";       Uidmap->second.iupdown = 9; } //update
				strtemp1 += Uidmap->second.iTableNum;     strtemp1 += "#";
				//std::string strSQL ="'";
				strtemp1 += Uidmap->first;      strtemp1 += "#";
				strtemp1 += Uidmap->second.lUT;      strtemp1 += "#";
				strtemp1 += pBankData->strDBVer();     strtemp1 += "#";
				strtemp1 += Uidmap->second.imark;      strtemp1 += "#";
				strtemp1 += Uidmap->second.strdata;      strtemp1 += "#";
			}
		}
		if(strtemp1 == "2#")
		{
			Deblocking();
		}
		else
		{
			m_bSP2D = TRUE;
			m_sHostName = POSTHOST ;			m_sHostName +=  POSTUP_URL;
			strtemp += strtemp1 ;			    std::string strxml = "xml=";
    		SendPData(strxml,strtemp);
		}
}

void  CSynchroDlg::RefreshDB()   //refresh db
{
		std::string strtable = "";
		CBankData* pBankData = CBankData::GetInstance();
    	UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			if( Uidmap->second.iupdown == 6 )
			{
				strtable = Uidmap->second.iTableNum; 
				pBankData->TableStr (strtable );
				std::string strSQL ="'";
				strSQL += Uidmap->first;      strSQL += "','";
				std::string strdata = DataRe( Uidmap->second.strdata);
				strdata = strSQL + strdata;
				//strSQL ="'";
				strdata += Uidmap->second.imark;      strdata += "','";
				strdata += Uidmap->second.lUT;        strdata += "'";
				strSQL = "INSERT INTO ";
				strSQL += strtable;							strSQL += " VALUES (";
				strSQL += strdata;           strSQL += ");";
    			if(pBankData->ExecuteSQL(strSQL) > 0)
				{
					isynnum--;
					Uidmap->second.iupdown = 3;
				}
			}
		}
    	Uidmap = pBankData->GetUserIDMap()->begin();
		for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
		{
			if( Uidmap->second.iupdown == 7 )
			{//PDATE tbBank SET name='CATA420',classId='34',BankID='66',Phone='',Website='' WHERE id=40;"
				PDataStr(Uidmap->second.strdata);
				std::string itable = Uidmap->second.iTableNum;  strtable = itable;
	        	TableField::iterator Tfmap = pBankData->GetTablefield()->find(itable);
				int itnum = Tfmap->second.ifieldNum;

				pBankData->TableStr (strtable );
				str11[0] = "'";				str11[0] += Uidmap->first;      str11[0] += "'";
				str11[itnum-2] = "'";	str11[itnum-2] += Uidmap->second.imark;    str11[itnum-2] += "'";
				str11[itnum-1] = "'";	str11[itnum-1] += Uidmap->second.lUT;      str11[itnum-1] += "'";

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
					isynnum--;
					Uidmap->second.iupdown = 3;
				}
			}
		}
}

void  CSynchroDlg::PGList()
{
		CBankData* pBankData = CBankData::GetInstance();
		if(pBankData->GetUserIDMap()->size() == 0)//获取服务器端数据ID列表
		{
			m_sHostName = POSTHOST ;			m_sHostName +=  POSTLIST_URL;
			std::string strd = ("xml=");
			SendPData(strd,struserinfo);
		}
		else //增量数据ID列表
		{
     		UserIDMap::iterator Uidmap = pBankData->GetUserIDMap()->begin();
			std::string strtemp = "";
			for(;Uidmap != pBankData->GetUserIDMap()->end();Uidmap ++)//down
			{
				if( Uidmap->second.iupdown != 3 )
				{
					strtemp += Uidmap->second.iTableNum +"#";
					strtemp += Uidmap->first +"#";
				}
			}
			if(strtemp == "")
			{
				P1DOWN();
			}
			else
			{
				m_sHostName = POSTHOST ;			m_sHostName +=  POSTLIST_URL;
				strtemp = struserinfo + strtemp ;			std::string strd = ("xml=");
				SendPData(strd,strtemp);
			}
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
			m_sHostName = POSTHOST ;
			m_sHostName +=  POSTDOWN_URL;
			strtable = struserinfo + strtable;
			std::string strxml = "xml=";
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
			ipnum++;       if(ipnum == 100)  break;
			if( Uidmap->second.iTableNum == "9" && (Uidmap->second.iupdown == 0 || Uidmap->second.iupdown == 2))
			{
				strtable += Uidmap->second.iTableNum +"#";
				strtable += Uidmap->first +"#";
			}
		}
		if(strtable == "2#")
		{
			strPost1D();
		}
		else
		{
			m_sHostName = POSTHOST ;			m_sHostName +=  POSTDOWN_URL;
			strtable = struserinfo + strtable;			std::string strxml = "xml=";
    		SendPData(strxml,strtable);
		}
}

string  CSynchroDlg::DataRe(std::string  strfor )
{
	std::string strtemp = strfor;	size_t dpos = 0;
	for(int i=0 ; i< 30; i++)  
	{
		dpos = strfor.find(","); //
		if(dpos == string::npos)
			break;
		strtemp.replace(dpos ,1, "','");
	}
	return strtemp;
}

int  CSynchroDlg::PDataStr(std::string  strfor )//syn datainfo
{
	size_t dpos = 0;   string tem = "";
	for(int i=0 ; i< 30; i++)
		str11[i] = "";
	for(int i=1 ; i< 30; i++)  
	{
		dpos = strfor.find(","); //
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
			m_sHostName = POSTHOST ;			m_sHostName +=  POSTUSER_CLEAR;
			std::string strtemp ;
			strtemp = struserinfo + "10#";			std::string strd = ("xml=");
			SendPData(strd,strtemp);
}
				
void  CSynchroDlg::NextPData (int icase)
{

	switch(icase)
	{	// left
		case 0:
			return;
		case 11:
			{
				CBankData::GetInstance()->GetList(strBig30);
//					m_UserIDMap.insert(std::make_pair(u64id, uddata));


			}
			break;
		case 12:
			{
			}
			break;
		case 92:
			{
			}
			break;
		default:
			break;
	}


}

LRESULT CSynchroDlg::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}

