// CMBChinaGetBill.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <map>
#include <list>
#include <vector>
#include "../../BankData/BankData.h"
#include "../../ThirdParty/RunLog/RunLog.h"
#include "CSVFile.h"
#include <time.h>


HWND g_notifyWnd = NULL;
int g_bState = 0;

#define CHECHSTATE if(g_bState != 0){state = g_bState;break;}

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

string FilterStringNumber(string& scr)
{
	string result;
	char *p = (char*)scr.c_str();
	char temp[2] = { 0 };
	for(unsigned char i = 0;i < scr.size();i ++)
	{		
		if((((*p) >= '0')&&((*p) <= '9'))|| (*p) == '-' || (*p) =='.') 
		{
			memcpy(&temp[0], p, 1);
			result += temp; 
		}
		p ++;
	}
	return result;
}

string FilterTimeString(string& scr)
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
		if(result.size() >= 8)
			break;
		p ++;
	}
	return result;
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
	plRecords->m_mapBack.clear();
	memset(plRecords->aid, 0, 256);
	memset(plRecords->tag, 0, 256);
	plRecords->isFinish = false;
}


int TransferDataGet(wstring& cookie, string btime, string etime)
{
	HINTERNET		m_hInetSession; // 会话句柄
	HINTERNET		m_hInetConnection; // 连接句柄
	HINTERNET		m_hInetFile; //
	HANDLE			m_hSaveFile;

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
	

	m_hInetConnection = ::InternetConnect(m_hInetSession, L"lab.alipay.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (m_hInetConnection == NULL)
	{
		InternetCloseHandle(m_hInetSession);

		return 3001;
	}

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;
	
	USES_CONVERSION;
	srand((unsigned)time(NULL));
	int seed = rand();
	WCHAR oName[256] = {0};
	swprintf_s(oName, 256 , L"/consume/record/items.resource?beginTime=%s&endTime=%s&month=&time=chooseDate&cashflow=&moneyFlowsType=&page=1&keywordType=bankName&keywordValue=&history=false&beginAmount=&endAmount=&filter=false&maxItems=57&orderBy=&randseed=%d", A2W(btime.c_str()),A2W(etime.c_str()),seed);

	m_hInetFile = HttpOpenRequestW(m_hInetConnection, _T("GET"), oName, NULL, NULL, ppszAcceptTypes, INTERNET_FLAG_SECURE /*| INTERNET_FLAG_DONT_CACHE*/ | INTERNET_FLAG_KEEP_CONNECTION, 0);
	if (m_hInetFile == NULL)
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetSession);
		return 3002;
	}	

	TCHAR szHeaders[1024];	
	_stprintf_s(szHeaders, _countof(szHeaders), _T("Cookie: %s;%s\r\n"), cookie.c_str(), L"LoginForm=alipayLogin;ac_stat=success;");

	BOOL ret = HttpAddRequestHeaders(m_hInetFile, szHeaders, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	wstring heads = L"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;\
		q=0.8Accept-Charset:GB2312,utf-8;q=0.7,*;q=0.7Accept-Encoding:gzip, deflateAccept-Language:zh-cn,zh;\
		q=0.5Connection:keep-aliveHost:lab.alipay.comKeep-Alive:115Referer:https://lab.alipay.com/consume/record/index.htm\
		User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n";

	ret = HttpAddRequestHeadersW(m_hInetFile, heads.c_str(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 
	BOOL bSend = ::HttpSendRequestW(m_hInetFile, NULL, 0, NULL, 0);
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


	TCHAR szContentLength[32];
	dwInfoSize = sizeof(szContentLength);
	if (!::HttpQueryInfo(m_hInetFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);

		return 3006;
	}

	DWORD dwBytesRead = 0;
	char szReadBuf[1024];
	DWORD dwBytesToRead = sizeof(szReadBuf);
	bool sucess = true;

	WCHAR szAppDataFileName[MAX_PATH + 1];
	ExpandEnvironmentStringsW(L"%APPDATA%\\MoneyHub\\mAliPay.csv", szAppDataFileName, MAX_PATH);

	m_hSaveFile = CreateFile(szAppDataFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hSaveFile == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		return err;
	}
	do
	{
		if (!::InternetReadFile(m_hInetFile, szReadBuf, dwBytesToRead, &dwBytesRead))
		{
			sucess = false;
			break;
		}
		else if (dwBytesRead)
		{

			DWORD dwBytesWritten = 0;
			if (!WriteFile(m_hSaveFile, szReadBuf, dwBytesRead, &dwBytesWritten, NULL))
			{
				sucess = false;
				break;
			}
		}
	}while (dwBytesRead);
	CloseHandle(m_hSaveFile);

	InternetCloseHandle(m_hInetConnection);
	InternetCloseHandle(m_hInetFile);
	InternetCloseHandle(m_hInetSession);
	if(sucess == false)
	{
		return 1020;
	}

	return 0;
}

int WINAPI FetchBillFunc(IWebBrowser2* pFatherWebBrowser, IWebBrowser2* pChildWebBrowser, BillData* pData, int &step, LPBILLRECORDS plRecords)
{
	USES_CONVERSION;
	int state = 0;
	if(pData->aid == "e001")
	{
		RecordInfo(L"Alipay", 1800, L"进入导入账单处理%d", step);
		HRESULT hr = S_OK;
		IDispatch *docDisp			= NULL;
		IHTMLDocument2 *doc			= NULL;
		IHTMLDocument3 *doc3		= NULL;
		IHTMLElement *elem			= NULL;

		hr = pFatherWebBrowser->get_Document(&docDisp);
		if (SUCCEEDED(hr) && docDisp != NULL)
		{					
			hr = docDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&doc));	
			if (SUCCEEDED(hr) || doc != NULL) 
			{
				if (docDisp) { docDisp->Release(); docDisp = NULL; }
				switch(step)
				{					
				case 1:
					{
						g_notifyWnd = NULL;
						g_bState = 0;
					}
					break;
				case 2://支付宝存在两种进入的页面，所以需要不同的处理规则
				case 5:
					{
						BSTR pbUrl = NULL;
						pChildWebBrowser->get_LocationURL(&pbUrl);
						if(NULL != pbUrl)
						{
							wstring url = pbUrl;
							RecordInfo(L"Alipay访问", 1800, (WCHAR*)url.c_str());
							if(url.find(L"lab.alipay.com/user/i.htm") != wstring::npos)
							{
								

								RecordInfo(L"Alipay进入正确", 1800, (WCHAR*)url.c_str());
								wstring info;
								hr=doc->get_body( &elem);  
								if(elem!=NULL)   
								{      
									BSTR pbBody = NULL;//::SysAllocString(OLESTR("Written by IHTMLDocument2:招商银行首页需要进行显示."));
									hr=elem->get_innerHTML(&pbBody);   //类似的还有put_innerTEXT
									if(pbBody != NULL)
										info = pbBody;
									elem->Release();   
									elem = NULL;
								}

								if(info.find(L"立即体验新版支付宝") != wstring::npos)
								{
									CComVariant var;									
									pFatherWebBrowser->Navigate((BSTR)L"https://lab.alipay.com/user/afterShowWelcome.htm", &var, &var, &var, &var);
									break;
								}	

								wstring money;
								size_t nPos = info.find(L"balance");
								if(nPos != wstring::npos)
								{
									nPos = info.find(L"number", nPos);
									wstring moneytemp = info.substr(nPos + 7, nPos + 50);
									int pos = moneytemp.find(L"</");

									money = moneytemp.substr(0, pos);
								}
								if(money == L"")
								{
									RecordInfo(L"Alipay", 1800, L"页面余额没找到");
								}
								wstring zhanghu;
								int strsize = wcslen(L"email");
								nPos = info.find(L"email");//
								if(nPos != wstring::npos)
								{
									int nnpos = info.find(L"title=", nPos);
									if (nnpos != wstring::npos)
									{
										wstring zhanghutemp = info.substr(nnpos + 6, wstring::npos);
										int spos = zhanghutemp.find_first_of('>');
										zhanghu = zhanghutemp.substr(0, spos);
									}
								}		
								if(zhanghu == L"")
								{
									RecordInfo(L"Alipay", 1800, L"页面账户没找到");
									state = BILL_GET_ACCOUNT_ERROR;
									break;
								}

								USES_CONVERSION;
								string ntag = W2A(zhanghu.c_str());
								if(pData->tag != "" && ntag!= "")
									if(pData->tag != ntag)
									{
										ShowNotifyWnd(false);//隐藏正在登陆框
										//当前账户B已绑定卡号
										WCHAR sInfo[256] = { 0 };
										swprintf(sInfo, 256, L"当前账户已绑定支付宝账号%s，实际导入支付宝账号与原账户不一致，是否继续导入",A2W(pData->tag.c_str()));

										HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
										if(MessageBoxW(hMainFrame, sInfo, L"财金汇", MB_OKCANCEL) == IDCANCEL)
										{
											state = BLII_NEED_RESTART;
											break;
										}
										ShowNotifyWnd(true);//显示正在登陆
									}

								strcpy_s(plRecords->tag, 256, ntag.c_str());	
								//以后查库的检测放在这个位置
								BILLRECORD* pRecord = new BILLRECORD;
								pRecord->type = RMB;
								plRecords->accountid = pData->accountid;
								strcpy_s(plRecords->aid, 256, pData->aid.c_str());

								if(money != L"")
									pRecord->balance = W2A(money.c_str());//余额
								else
									pRecord->balance = "F";//没有获取到余额	

								plRecords->BillRecordlist.push_back(pRecord);

								strcpy_s(pData->select ,256,"1999-1-1 0:00:00");
								state = BILL_SELECT_MONTH2;
							}
						}
					}
					break;
				case 3:
				case 6:
					  {
						step = 10;
						BSTR pbCookie = NULL;
						doc->get_cookie(&pbCookie);
						wstring cookie = pbCookie;
						string sel = pData->select;
						string btime = sel.substr(0, 8);
						string etime = sel.substr(8, string::npos);	

						LPMONTHBILLRECORD pMRecord = new MONTHBILLRECORD;
						pMRecord->m_isSuccess = false;
						pMRecord->month = pData->select;

						if (TransferDataGet(cookie, btime, etime) == 0)
						{
							//处理不好有内存泄露
							if(g_bState!= 0) 
								delete pMRecord;

							CHECHSTATE;
							//得到数据文件后
							RecordInfo(L"Alipay", 1800, L"得到了传输的数据");
							std::locale::global(std::locale(""));

							char szAppDataPath[MAX_PATH + 1];
							ExpandEnvironmentStringsA("%APPDATA%\\MoneyHub\\", szAppDataPath, MAX_PATH);
							string fname(szAppDataPath);
							fname += "mAliPay.csv";
							CSVFile CSVRead; 
							int res = CSVRead.Open(true, szAppDataPath, "mAliPay.csv");
							if (res != 0)  
							{ 
								delete pMRecord;
								RecordInfo(L"Alipay", 1800, L"mAliPay.csv文件读取失败:%d",res );
								state = BILL_COM_ERROR;
								break;  
							} 
							pMRecord->m_isSuccess = true;

							while (CSVRead.CSVReadNextRow())  
							{  
								std::string strID = CSVRead.m_CSVCurRow[0];//流水号  
								std::string strTime = CSVRead.m_CSVCurRow[1];//时间
								std::string strName = CSVRead.m_CSVCurRow[2];//名称
								std::string strComment = CSVRead.m_CSVCurRow[3];//备注
								std::string strIncome = CSVRead.m_CSVCurRow[4];//收入
								string strPayment = CSVRead.m_CSVCurRow[5];//支出
								string strBalance = CSVRead.m_CSVCurRow[6];//账户余额
								string strfunds = CSVRead.m_CSVCurRow[7];//资金渠道

								//strID = CSVRead.m_CSVCurRow[0];//("流水号", );  
								//CSVRead.CSVRead("时间", strTime);  
								//CSVRead.CSVRead("名称", strName); 
								//CSVRead.CSVRead("备注", strComment); 
								//CSVRead.CSVRead("收入", strIncome); 
								//CSVRead.CSVRead("支出", strPayment); 
								//CSVRead.CSVRead("账户余额（元）", strBalance); 
								//CSVRead.CSVRead("资金渠道", strfunds); 

								LPTRANRECORD ptrRecord = new TRANRECORD;
								strTime = FilterTimeString(strTime);//只留下8位长度的时间
								sprintf_s(ptrRecord->PostDate, 256, "%s", strTime.c_str());
								sprintf_s(ptrRecord->TransDate, 256, "%s", strTime.c_str());
								if(strComment != "")
									sprintf_s(ptrRecord->Description, 256, "%s-%s,支付方式:%s", strName.c_str(), strComment.c_str(),strfunds.c_str());
								else
									sprintf_s(ptrRecord->Description, 256, "%s,支付方式:%s", strName.c_str(),strfunds.c_str());
								if(strIncome != "")
									sprintf_s(ptrRecord->Amount, 256, "%s", FilterStringNumber(strIncome).c_str());
								if(strPayment != "")
									sprintf_s(ptrRecord->Amount, 256, "%s", FilterStringNumber(strPayment).c_str());

								pMRecord->TranList.push_back(ptrRecord);
							}

							BILLRECORD* pRecord = (*plRecords->BillRecordlist.begin());
							pRecord->bills.push_back(pMRecord);
							plRecords->isFinish = true;
							plRecords->type = pData->type;
							state = BILL_FINISH_STATE;
							CSVRead.close();
							::DeleteFileA(fname.c_str());
						}
						else
						{
							RecordInfo(L"Alipay", 1800, L"Post失败");
							delete pMRecord;
							state = BILL_COM_ERROR;//出错

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
