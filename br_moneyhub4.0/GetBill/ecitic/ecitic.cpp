// eciticGetBill.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <map>
#include <list>
#include <vector>
#include "../../BankData/BankData.h"
#include "../../ThirdParty/RunLog/RunLog.h"
#include "ConvertBase.h"
using namespace std;

static wstring wstrEMPSID = L"";	// EMP_SID
static wstring wstrCardNo = L"";	// 完整信用卡号
HWND g_notifyWnd = NULL;
int g_bState = 0;

#define CHECHSTATE if(g_bState != 0){state = g_bState;break;}
#define CHECKGETURLPAGEINFOSTATUS 	if(iRes != 0){state = BLII_NEED_RESTART;RecordInfo(L"ecitic", 1800, L"GetUrlPageInfo失败");CHECHSTATE;break;}

void WINAPI SetBillState(int State)
{
	g_bState = State;
}
void WINAPI SetNotifyWnd(HWND notifyWnd)
{
	g_notifyWnd = notifyWnd;
}

void ChangeNotifyWord(WCHAR* info)
{
	if(g_notifyWnd != 0)
		::PostMessage(g_notifyWnd, WM_BILL_CHANGE_NOFIFY, 0, (LPARAM)info);
}

void ShowNotifyWnd(bool bShow)
{
	if(g_notifyWnd != NULL)
	{
		if(!bShow)
			::SendMessage(g_notifyWnd, WM_BILL_HIDE_NOFIFY, 0, 0);
		else
			::SendMessage(g_notifyWnd, WM_BILL_SHOW_NOFIFY, 0, 0);
	}
}

bool RecordInfo(wstring program, DWORD common, wchar_t *format, ...)
{
	wchar_t strTemp[MAX_INFO_LENGTH];
	memset(strTemp, 0, sizeof(strTemp));
	wchar_t *pTemp = strTemp;
	//合成信息
	va_list args; 
	va_start(args,format); 
	vswprintf(pTemp,MAX_INFO_LENGTH,format,args); 
	va_end(args); 

	wstring stemp(strTemp);


	wchar_t cinfo[20]= { 0 };
	swprintf(cinfo, 20, L"0x%08x", common);
	wstring wscTmp(cinfo);
	wscTmp  = program + L"-" + wscTmp + L"-" + stemp;

	CRunLog::GetInstance ()->GetLog ()->WriteSysLog (LOG_TYPE_INFO, L"%ws", wscTmp.c_str());
	return true;
}

void WINAPI FreeMemory(LPBILLRECORDS plRecords)
{
	list<LPBILLRECORD>::iterator ite = plRecords->BillRecordlist.begin();
	for(;ite != plRecords->BillRecordlist.end(); ite ++)
	{
		if((*ite) != NULL)
		{
			list<LPMONTHBILLRECORD>::iterator mite = (*ite)->bills.begin();
			for(;mite != (*ite)->bills.end(); mite ++)
			{
				list<LPTRANRECORD>::iterator lite = (*mite)->TranList.begin();
				for(; lite != (*mite)->TranList.end();lite ++)
				{
					if((*lite) != NULL)
						delete (*lite);
				}
				(*mite)->TranList.clear();
				delete (*mite);
			}
			(*ite)->bills.clear();

			delete (*ite);
		}
	}

	plRecords->BillRecordlist.clear();
	memset(plRecords->aid, 0, 256);
	plRecords->m_mapBack.clear();
	memset(plRecords->tag, 0, 256);
	plRecords->isFinish = false;
}

// 删除string中的所有某字符
void StringDeleteChar(string& str, char c)
{
	while (str.find(c) != string::npos)
	{
		string::size_type pos = str.find(c);
		str.erase(pos, 1);
	}
}

// 从页面中获取月份信息，格式化为201106
void GetMonthsInfo(string strPageInfo, LPBILLRECORDS plRecords, int &state)	
{
	////////////数据格式begin/////////////
	//////displayYMON("201110");</
	//////获取201110就可以直接存用，不用格式化，非常方便。
	////////////数据格式end///////////

	string::size_type tPos = strPageInfo.find("displayYMON(\"");
	if (tPos == string::npos)
	{
		state = BILL_COM_ERROR;
		RecordInfo(L"ecitic", 1800, L"获取月份信息失败");
		return;
	}

	while (tPos != string::npos)
	{
		tPos += 13;
		string strMonthInfo = strPageInfo.substr(tPos, 6);
		SELECTINFONODE mNode;
		strncpy_s(mNode.szNodeInfo, 256, strMonthInfo.c_str(), strMonthInfo.length());
		mNode.dwVal = CHECKBOX_SHOW_UNCHECKED;
		plRecords->m_mapBack.push_back(mNode);
		tPos = strPageInfo.find("displayYMON(\"", tPos);
	}
	RecordInfo(L"ecitic", 1800, L"获取月份信息成功");
}

/*
功能：寻找strPageInfo中从指定startPos开始的第n个匹配的子串des的下标
参数：strPageInfo	-->源串
des				-->查找的子串
desLength	-->查找子串的长度(C风格字串不包含\0长度)
startPos		-->起始
n					-->第n个匹配的子串
返回值：若查找成功，则返回对应下标，若失败则返回std::string::npos
*/
string::size_type StringAppointFind(string strPageInfo, char* des, int desLength, string::size_type startPos, int n)
{
	string::size_type resPos = startPos;

	int i = 0;

	while (i != n)
	{
		string::size_type tmpPos;
		if (resPos + desLength >= strPageInfo.size())
		{
			return std::string::npos;
		}
		tmpPos = strPageInfo.find(des, resPos + desLength);
		if (string::npos == tmpPos)
		{
			return std::string::npos;
		}
		resPos = tmpPos;
		i++;
	}

	return resPos;
}

// 获取RMB，USD余额并写入到plRecords
void GetAccountBalance(string strPageInfo, LPBILLRECORDS lpRecords)
{
	string::size_type startPos;
	string::size_type endPos;
	string strBalance;

	// 获取RMB余额
	// 在此即获取页面源码第4个form_table_td2标签中的内容
	// 格式：<TD class="form_table_td2">&nbsp;0.00&nbsp;&nbsp;*&nbsp;负数表示超存金额</TD>
	LPBILLRECORD pRecord = new BILLRECORD;
	pRecord->balance = "F";
	pRecord->type = RMB;
	startPos = StringAppointFind(strPageInfo, "form_table_td2", 14, 0, 4);
	if (startPos == std::string::npos)
	{
		pRecord->balance = "F";
	}
	else
	{
		startPos += 22;	// form_table_td2">&nbsp;
		endPos = strPageInfo.find("&nbsp;", startPos);
		if (endPos == std::string::npos)
		{
			pRecord->balance = "F";
		}
		else
		{
			strBalance = strPageInfo.substr(startPos, endPos - startPos);
			// 中信信用卡里余额为'-'则代表还有钱，无'-'则欠钱，与我们的逻辑相反，故需全部取反，并去除金额中的','号
			if (strBalance.find("-") != string::npos)
			{
				StringDeleteChar(strBalance, '-');
				StringDeleteChar(strBalance, ',');
			}
			else
			{
				strBalance = '-' + strBalance;
				StringDeleteChar(strBalance, ',');
			}
			pRecord->balance = strBalance.c_str();
		}
	}
	lpRecords->BillRecordlist.push_back(pRecord);
	if (pRecord->balance == "F")
	{
		RecordInfo(L"ecitic", 1800, L"获取人民币余额失败");
	}

	// 获取USD金额
	// 在此即获取页面源码第8个form_table_td2标签中的内容
	// 格式：<TD class="form_table_td2">&nbsp;0.00&nbsp;&nbsp;*&nbsp;负数表示超存金额</TD>
	LPBILLRECORD puRecord = new BILLRECORD;
	puRecord->balance = "F";
	puRecord->type = USD;
	startPos = StringAppointFind(strPageInfo, "form_table_td2", 14, 0, 8);
	if (startPos == std::string::npos)
	{
		puRecord->balance = "F";
	}
	else
	{
		startPos += 22; // form_table_td2">&nbsp;
		endPos = strPageInfo.find("&nbsp;", startPos);
		if (endPos == std::string::npos)
		{
			puRecord->balance = "F";
		}
		else
		{
			strBalance = strPageInfo.substr(startPos, endPos - startPos);
			// 中信信用卡里余额为'-'则代表还有钱，无'-'则欠钱，与我们的逻辑相反，故需全部取反，并去除金额中的','号
			if (strBalance.find("-") != string::npos)
			{
				StringDeleteChar(strBalance, '-');
				StringDeleteChar(strBalance, ',');
			}
			else
			{
				strBalance = '-' + strBalance;
				StringDeleteChar(strBalance, ',');
			}
			puRecord->balance = strBalance.c_str();
		}
	}
	lpRecords->BillRecordlist.push_back(puRecord);
	if (puRecord->balance == "F")
	{
		RecordInfo(L"ecitic", 1800, L"获取美元余额失败");
	}
}

// 将日期中的中文去除
string FilterMonthStringNumber(const string& scr)
{
	string result;
	char *p = (char*)scr.c_str();
	char temp[2] = { 0 };
	for(unsigned char i = 0;i < scr.size();i ++)
	{		
		if((((*p) >= '0')&&((*p) <= '9'))) 
		{
			memcpy(&temp[0], p, 1);
			result += temp; 
		}
		p ++;
	}
	return result;
}

/*
功能：从网页内容中分析获取并将RMB或USD账单记录写入到plRecords
参数：strPageInfo		-->账单信息来源
strTmpMonth	-->信息所在月份
billtype			-->交易币种，RMB or USD
plRecords			-->老朋友了，无处不在
*/
void GetRMBOrUSDBill(BillType billtype, string strPageInfo, string strTmpMonth,  LPBILLRECORDS plRecords)
{
	// 找到已生成的人民币或者美元账单
	LPBILLRECORD pRecord = NULL;
	list<LPBILLRECORD>::iterator iter = plRecords->BillRecordlist.begin();
	for (; iter !=plRecords->BillRecordlist.end(); iter++)
	{
		if((*iter)->type == billtype)
		{
			pRecord = (*iter);
			break;
		}
	}

	LPMONTHBILLRECORD pMonthRecord = new MONTHBILLRECORD;
	pMonthRecord->month = strTmpMonth;	

	// 判断是否获取的是正确的页面信息
	// 三种情况：有记录与无记录，源码中都有“页面号”，而登录超时则无。
	string::size_type nTmpPos = strPageInfo.find("页面号");
	if (string::npos == nTmpPos)
	{
		pMonthRecord->m_isSuccess = false;
		return;
	}
	//	<tr align="center" class="bg1"
	//	onmouseover="this.className ='result_over';"
	//	onmouseout="this.className ='bg1';">
	//	<td align = "center">2011年10月25日</td>
	//	<td align = "center">2011年10月27日</td>
	//	<td align = "left">PAYPAL *DIGITALRIVE WW  4029357733   SGP</td>
	//	<td align = "center">85326961299142117335514</td>
	//	<td align = "center">6897</td>
	//	<td align = "center">人民币</td>
	//	<td align = "center">19,733.99</td>
	//	<td align = "center">19,733.99</td>
	//	</tr>

	nTmpPos = 0;
	int i = 0;
	LPTRANRECORD preRecord = NULL;
	while (string::npos != strPageInfo.find("<td align = \"", nTmpPos))
	{
		if (i%8 == 0)
		{
			preRecord = new TRANRECORD;
		}

		string::size_type beginPos = strPageInfo.find("<td align = \"", nTmpPos);
		string::size_type endPos = strPageInfo.find("</td>", beginPos);
		if (beginPos == string::npos || endPos == string::npos)
		{
			break;
		}
		nTmpPos = endPos;
		string strPer;
		if (i%8 == 2)
		{
			strPer = strPageInfo.substr(beginPos + 19, endPos - beginPos - 19);
		}
		else
		{
			strPer = strPageInfo.substr(beginPos + 21, endPos - beginPos - 21);
		}
		switch (i%8)
		{
		case 0:	// 交易日期
			strPer = FilterMonthStringNumber(strPer);
			sprintf_s(preRecord->TransDate, 256, "%s", strPer.c_str());
			break;
		case 1:	// 入账日期
			strPer = FilterMonthStringNumber(strPer);
			sprintf_s(preRecord->PostDate, 256, "%s", strPer.c_str());
			break;
		case 2:	// 交易描述
			sprintf_s(preRecord->Description, 256, "%s", strPer.c_str());
			break;
		case 3:	// 参考编号
			break;
		case 4:	// 卡号末四位
			break;
		case 5:	// 交易币种
			break;
		case 6:	// 交易金额
			StringDeleteChar(strPer, ',');
			sprintf_s(preRecord->Amount, 256, "%s", strPer.c_str());
			break;
		case 7:	//	结算金额
			break;
		default:
			break;
		}

		i++;
		if (i % 8 ==0 && i != 0)
		{
			pMonthRecord->TranList.push_back(preRecord);
		}
	}

	pMonthRecord->m_isSuccess = true;

	if (i%8 != 0)	// 获取数据有不完整现象
	{
		pMonthRecord->TranList.clear();
		pMonthRecord->m_isSuccess = false;
	}

	pRecord->bills.push_back(pMonthRecord);
}

int TransferDataPost(wstring& page, wstring& cookie, wstring postdata, wstring referer, string& strPageInfo)
{
	USES_CONVERSION;
	string data = W2A(postdata.c_str());
	HINTERNET		m_hInetSession; // 会话句柄
	HINTERNET		m_hInetConnection; // 连接句柄
	HINTERNET		m_hInetFile; //


	m_hInetSession = ::InternetOpen(L"Moneyhub3.1", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInetSession == NULL)
	{
		return 3000;
	}

	DWORD dwTimeOut = 5000;
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_SEND_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);
	InternetSetOptionEx(m_hInetSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(DWORD), 0);


	m_hInetConnection = ::InternetConnect(m_hInetSession, L"e.bank.ecitic.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (m_hInetConnection == NULL)
	{
		InternetCloseHandle(m_hInetSession);

		return 3001;
	}

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;

	m_hInetFile = HttpOpenRequestW(m_hInetConnection, _T("POST"), page.c_str(), NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_SECURE /*| INTERNET_FLAG_DONT_CACHE*/ | INTERNET_FLAG_KEEP_CONNECTION, 0);
	if (m_hInetFile == NULL)
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetSession);
		return 3002;
	}	

	TCHAR szHeaders[1024];	
	_stprintf_s(szHeaders, _countof(szHeaders), _T("Cookie: %s;\r\nContent-Length: %d\r\n"), cookie.c_str(), postdata.length());


	BOOL ret = HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	wstring heads = L"Cache-Control: no-cache\r\nAccept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/x-silverlight, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-cn\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; InfoPath.2; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: " + referer + L"\r\nConnection: Keep-Alive\r\nHost: e.bank.ecitic.com\r\n";

	ret = HttpAddRequestHeadersW(m_hInetFile, heads.c_str(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	char sData[256] = {0};
	sprintf_s(sData , 256, "%s", data.c_str());
	BOOL bSend = ::HttpSendRequestW(m_hInetFile, NULL, 0, (void*)sData, strlen(sData));
	if (!bSend)
	{
		int err = GetLastError();
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);

		return err;
	}

	TCHAR szStatusCode[32];
	DWORD dwInfoSize = sizeof(szStatusCode);
	if (!HttpQueryInfo(m_hInetFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);

		return 3004;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);
		if (nStatusCode != HTTP_STATUS_PARTIAL_CONTENT && nStatusCode != HTTP_STATUS_OK)
		{
			InternetCloseHandle(m_hInetConnection);
			InternetCloseHandle(m_hInetFile);
			InternetCloseHandle(m_hInetSession);
			return 3005;
		}
	}

	const DWORD dwBytesToRead = 1024;
	char szReadBuf[dwBytesToRead];
	DWORD dwBytesRead;
	strPageInfo = "";

	do
	{
		BOOL bReadStatus = ::InternetReadFile(m_hInetFile, (LPVOID)szReadBuf, dwBytesToRead, &dwBytesRead);

		if (false == bReadStatus)
		{
			InternetCloseHandle(m_hInetConnection);
			InternetCloseHandle(m_hInetFile);
			InternetCloseHandle(m_hInetSession);
			return 3006;
		}

		string::size_type nTmpPos = strPageInfo.size();
		strPageInfo += szReadBuf;
		strPageInfo = strPageInfo.substr(0, nTmpPos +dwBytesRead);
	}while (std::string::npos == strPageInfo.find("</html>") && 0 != dwBytesRead);

	InternetCloseHandle(m_hInetConnection);
	InternetCloseHandle(m_hInetFile);
	InternetCloseHandle(m_hInetSession);

	return 0;
}

int WINAPI FetchBillFunc(IWebBrowser2* pFatherWebBrowser, IWebBrowser2* pChildWebBrowser, BillData* pData, int &step, LPBILLRECORDS plRecords)
{
	USES_CONVERSION;
	int state = 0;
	if(pData->aid == "a007")
	{
		RecordInfo(L"eciticDll", 1800, L"进入导入账单处理%d", step);

		wstring wstrUrl;

		HRESULT hr = S_OK;
		IDispatch *docDisp			= NULL;
		IHTMLDocument2 *doc			= NULL;
		IHTMLDocument3 *doc3		= NULL;
		IHTMLElement *elem			= NULL;

		hr = pFatherWebBrowser->get_Document(&docDisp);
		if (SUCCEEDED(hr) && docDisp != NULL)
		{					
			hr = docDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&doc));	
			if (SUCCEEDED(hr) && doc != NULL) 
			{
				if (docDisp) { docDisp->Release(); docDisp = NULL; }

				switch(step)
				{	
				case 1:// case 1中信在https://e.bank.ecitic.com/perbank5/signInCredit.do入口不用做什么，直接是在信用卡界面。
					{
					}
					break;

				case 2:
					{
						BSTR pbUrl = NULL;
						pChildWebBrowser->get_LocationURL(&pbUrl);
						if(NULL != pbUrl)
						{
							wstring url = pbUrl;
							if(url.find(L"/perbank5/gotoCreditMain.do") != wstring::npos)	//　主页面加载完成，可以去完成下一步的事情了，在此是获取页面余额和月份信息
							{
								// 获取账户余额
								//Sleep(3000);
								BSTR pbCookie = NULL;
								doc->get_cookie(&pbCookie);
								wstring cookie = pbCookie;
								wstring postdata = L"cardNo=";
								postdata += wstrCardNo;
								wstring page = L"/perbank5/pb5640_maincardSettingreq.do?EMP_SID=";
								page += wstrEMPSID;
								wstring referer = L"https://e.bank.ecitic.com" + page;
								string strPageInfo;
								if(TransferDataPost(page, cookie, postdata, referer, strPageInfo) == 0)
								{
									GetAccountBalance(strPageInfo, plRecords);
									RecordInfo(L"ecitic", 1800, L"GetAccountBalance成功%d", step);
								}
								CHECHSTATE;
								

								// 获取月份信息
								postdata = L"accountNo=" + wstrCardNo + L"&currencyType1=840";
								page = L"/perbank5/pb5130_returnOtherMonth.do?EMP_SID=" + wstrEMPSID;
								referer = L"https://e.bank.ecitic.com/perbank5/pb5130_historyBillQry.do?EMP_SID=" + wstrEMPSID;
								if(TransferDataPost(page, cookie, postdata, referer, strPageInfo) != 0)
									state = BILL_COM_ERROR;;
								GetMonthsInfo(strPageInfo, plRecords, state);
								CHECHSTATE;
								RecordInfo(L"ecitic", 1800, L"获取月份信息成功%d", step);

								if (state != BILL_COM_ERROR)
									state = BILL_SELECT_MONTH;
							}
							else if (url.find(L"/perbank5/welcomeCredit.do") != wstring::npos)	// 子页面，可以获取EMP_SID和账号末四位
							{
								CHECHSTATE;
								IDispatch *docDisp_c			= NULL;
								IHTMLDocument2 *doc_c			= NULL;

								hr = pChildWebBrowser->get_Document(&docDisp_c);
								if (SUCCEEDED(hr) && docDisp_c != NULL)
								{					
									hr = docDisp_c->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&doc_c));
									if (SUCCEEDED(hr) && doc_c != NULL) 
									{
										if (docDisp_c) { docDisp_c->Release(); docDisp_c = NULL; }
									}
								}

								hr = doc_c->get_body( &elem);

								wstring info;
								if(elem != NULL)   
								{      
									BSTR pbBody = NULL;
									elem->get_innerHTML(&pbBody); 
									if(pbBody != NULL)
										info = pbBody;
									elem->Release();   
									elem = NULL;
								}
								if (doc_c) { doc_c->Release(); doc_c = NULL; }

								wstring::size_type pos;
								wstring::size_type endpos;
								// 获取EMP_SID
								pos = info.find(L"EMP_SID");
								if (pos != wstring::npos)
								{
									wstrEMPSID = info.substr(pos + 8, 40);
								}
								if (wstrEMPSID == L"")
								{
									state = BILL_COM_ERROR;
									RecordInfo(L"ecitic", 1800, L"获取EMP_SID失败%d", step);
								}
								CHECHSTATE;
								RecordInfo(L"ecitic", 1800, L"获取EMP_SID成功%d", step);

								// 获取完整账号并设置存储结构中的账号末四位
								pos = info.find(L"accountNo=");
								if (pos != wstring::npos)
								{
									endpos = info.find(L"&amp", pos);
									if (endpos != wstring::npos)
									{
										wstrCardNo = info.substr(pos + 10, endpos - pos - 10);
									}
								}
								if (wstrCardNo == L"")
								{
									state = BILL_GET_ACCOUNT_ERROR;
									RecordInfo(L"ecitic", 1800, L"获取CARDNO失败%d", step);
								}
								else
								{
									wstring card = wstrCardNo.substr(wstrCardNo.length()-4, 4);
									sprintf_s(plRecords->tag, 256, W2A(card.c_str()));
								}
								CHECHSTATE;
								RecordInfo(L"ecitic", 1800, L"获取CARDNO成功%d", step);

								/////////////////已绑定账户与当前登陆账户是否匹配begin//////////////////
								sprintf_s(plRecords->aid, 256,pData->aid.c_str());
								plRecords->type = pData->type;
								plRecords->accountid = pData->accountid;
								if(plRecords->tag != "" && pData->tag != "")
								{
									if(pData->tag != string(plRecords->tag))
									{
										ShowNotifyWnd(false);//隐藏正在登陆框

										WCHAR sInfo[256] = { 0 };
										swprintf(sInfo, 256, L"当前账户已绑定中信卡号末4位%s，实际导入信用卡号与原账户不一致，是否继续导入",A2W(pData->tag.c_str()));

										HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
										if(MessageBoxW(hMainFrame, sInfo, L"财金汇", MB_OKCANCEL) == IDCANCEL)
										{
											state = BLII_NEED_RESTART;
											break;
										}

										ShowNotifyWnd(true);//显示正在登陆
									}								
								}
								/////////////////已绑定账户与当前登陆账户是否匹配end//////////////////////
								CHECHSTATE;
							}
						}
					}
					break;

				case 3:	// 获取余额与月份信息
					{
						CHECHSTATE;

						step = 9; // 修改step，防止多次调用
						// 根据用户选择的月份，将账单数据导出，人民币与美元都导。

						//////////读取用户选择的月份begin////////////////
						list<string> selMonth;
						for (list<SELECTINFONODE>::iterator ite = plRecords->m_mapBack.begin();
							ite != plRecords->m_mapBack.end(); ite ++)
						{
							if (CHECKBOX_SHOW_CHECKED == ite->dwVal)
							{
								selMonth.push_back(ite->szNodeInfo);
							}
						}
						//////////读取用户选择的月份end//////////////////

						// 清理传过来的记录里的月份信息，若无选中月份，直接退出
						plRecords->m_mapBack.clear();
						if (0 == selMonth.size())
						{
							plRecords->isFinish = true;
							state = BILL_FINISH_STATE;
							break;
						}

						list<string>::iterator iter;
						BSTR pbCookie = NULL;
						doc->get_cookie(&pbCookie);
						wstring cookie = pbCookie;
						wstring postdata;
						wstring page;
						wstring referer;
						string strPageInfo;

						///////////////获取人民币账单begin///////////////////
						for (iter = selMonth.begin(); iter != selMonth.end(); iter++)
						{
							/*
							修改提示信息，
							比如正在获取人民币账单：XXXX-XX-XX 至 XXXX-XX-XX 
							*/
							wstring wstrTmp = A2W((*iter).c_str());
							wstring nInfo = L"正在导入RMB账单：" + wstrTmp;
							ChangeNotifyWord((WCHAR*)nInfo.c_str());
							CHECHSTATE;

							postdata = L"accountNo=" + wstrCardNo + L"&accoMonth=" + A2W((*iter).c_str()) + L"&currencyType=156&startNo=1&crdRecordItem=500&opFlag=1&sFrnCyno=840&EMP_SID=" + wstrEMPSID;
							page = L"/perbank5/pb5130_invokeHisDetailBiz.do";
							referer = L"https://e.bank.ecitic.com/perbank5/pb5130_returnOtherMonth.do?EMP_SID=" + wstrEMPSID;
							strPageInfo.clear();
							if(TransferDataPost(page, cookie, postdata, referer, strPageInfo) == 0)								
								GetRMBOrUSDBill(RMB, strPageInfo, *iter, plRecords);
							CHECHSTATE;
						}
						///////////////获取人民币账单end/////////////////////
						if (state != 0)
						{
							state = BILL_CANCEL_GET_BILL;
							break;
						}
						RecordInfo(L"ecitic", 1800, L"获取人民币账单信息完成%d", step);

						///////////////获取美元账单begin///////////////////
						for (iter = selMonth.begin(); iter != selMonth.end(); iter++)
						{
							/*
							修改提示信息，
							比如正在获取美元账单：XXXX-XX-XX 至 XXXX-XX-XX 
							*/
							wstring wstrTmp = A2W((*iter).c_str());
							wstring nInfo = L"正在导入USD账单：" + wstrTmp;
							ChangeNotifyWord((WCHAR*)nInfo.c_str());
							CHECHSTATE;

							postdata = L"accountNo=" + wstrCardNo + L"&accoMonth=" + A2W((*iter).c_str()) + L"&currencyType=840&startNo=1&crdRecordItem=500&opFlag=1&sFrnCyno=156&EMP_SID=" + wstrEMPSID;
							page = L"/perbank5/pb5130_invokeHisDetailBiz.do";
							referer = L"https://e.bank.ecitic.com/perbank5/pb5130_returnOtherMonth.do?EMP_SID=" + wstrEMPSID;
							strPageInfo.clear();
							if(TransferDataPost(page, cookie, postdata, referer, strPageInfo) == 0)								
								GetRMBOrUSDBill(USD, strPageInfo, *iter, plRecords);
							CHECHSTATE;
						}
						///////////////获取美元账单end/////////////////////
						if (state != 0)
						{
							state = BILL_CANCEL_GET_BILL;
							break;
						}
						RecordInfo(L"ecitic", 1800, L"获取人民币账单信息完成%d", step);

						if (0 == g_bState)
						{
							plRecords->isFinish = true;
							state = BILL_FINISH_STATE;
						}
					}
					break;

				default:
					break;	
				}
			}
		}

		if (elem) { elem->Release(); elem = NULL; }
		if (doc3) { doc3->Release(); doc3 = NULL; }
		if (doc) { doc->Release(); doc = NULL; }
		if (docDisp) { docDisp->Release(); docDisp = NULL; }
	}
	return state;
}