// CMBChinaGetBill.cpp : Defines the exported functions for the DLL application.
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
CComVariant CallJScript2(IHTMLDocument2 *doc, std::string strFunc, DISPPARAMS& dispparams);
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

wstring FilterStringW(const wstring& scr)
{
	wstring result;
	wchar_t *p = (wchar_t*)scr.c_str();
	wchar_t temp[2] = { 0 };
	for(unsigned char i = 0;i < scr.size();i ++)
	{		
		if(((*p) != L' '))
		{
			memcpy(&temp[0], p, 2);
			result += temp; 
		}
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
	memset(plRecords->aid, 0, 256);
	plRecords->m_mapBack.clear();
	memset(plRecords->tag, 0, 256);
	plRecords->isFinish = false;
}

float GetBalance(wstring bill)
{
	if(bill == L"")
		return 0;
	//'￥365.75','￥1,048.00'

	float balance = 0;
	USES_CONVERSION;
	for(size_t npos = 0;npos < bill.length();)
	{
		wstring substring;
		int begin = bill.find_first_of(_T("'"), npos);
		if(begin == wstring::npos)
		{
			npos = bill.length();
		}
		else
		{
			int end = bill.find_first_of(_T("'"), begin + 1);
			if(end == wstring::npos)
				substring = bill.substr(begin + 1, end);
			else
				substring = bill.substr(begin + 1, end - begin);
			npos = end + 1;
			string res = W2A(substring.c_str());
			balance += atof(FilterStringNumber(res).c_str());
		}		

	}

	return balance;

}

int TransferDataPost(wstring& page, wstring& cookie, wstring postdata, wstring& revinfo)
{
	USES_CONVERSION;
	string data = W2A(postdata.c_str());
	HINTERNET		m_hInetSession; // 会话句柄
	HINTERNET		m_hInetConnection; // 连接句柄
	HINTERNET		m_hInetFile; //
	

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
	

	m_hInetConnection = ::InternetConnect(m_hInetSession, L"pbsz.ebank.cmbchina.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (m_hInetConnection == NULL)
	{
		InternetCloseHandle(m_hInetSession);

		return 3001;
	}

	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*"); 
	ppszAcceptTypes[1] = NULL;
	
	//ClientNo=161544C5565DC6059DA8BDBAFE364832526275626242479700072435&CreditAccNo=0114700657001001&IN_YYYYMM=201108

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
	wstring heads = L"Cache-Control: no-cache\r\n\
Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, application/msword, application/vnd.ms-powerpoint, application/vnd.ms-excel, */*\r\n\
Accept-Encoding: gzip, deflate\r\nAccept-Language: zh-cn\r\nContent-Type: application/x-www-form-urlencoded\r\n\
Referer: https://pbsz.ebank.cmbchina.com/CmbBank_CreditCardV2/UI/CreditCardPC/CreditCardV2/am_QueryReckoningSurvey.aspx\r\nConnection: Keep-Alive\r\nHost: pbsz.ebank.cmbchina.com\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; .NET4.0C; .NET4.0E)\r\n";

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


	TCHAR szContentLength[32] = {0};
	dwInfoSize = sizeof(szContentLength);
	if (!::HttpQueryInfo(m_hInetFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);

		return 3006;
	}
	DWORD revSize = 0;
	if(wcslen(szContentLength) != 0)
		revSize = _wtol(szContentLength);

	DWORD dwBytesRead = 0;
	char szReadBuf[1024];
	char* pBuf = new char[revSize + 1];//大小的内存
	if(pBuf == NULL)
	{
		InternetCloseHandle(m_hInetConnection);
		InternetCloseHandle(m_hInetFile);
		InternetCloseHandle(m_hInetSession);
		return 1021;
	}
	memset(pBuf, 0, revSize + 1);
	char* pCur = pBuf;
	DWORD dwBytesToRead = sizeof(szReadBuf);
	bool sucess = true;//这里需要将读到的数据进行拼装

	do
	{
		if (!::InternetReadFile(m_hInetFile, szReadBuf, dwBytesToRead, &dwBytesRead))
		{
			sucess = false;
			break;
		}
		else if (dwBytesRead)
		{
			memcpy(pCur , szReadBuf, dwBytesRead);
			pCur += dwBytesRead;
		}
	}while (dwBytesRead);

	int textlen;
	wchar_t * pResult;
	textlen = MultiByteToWideChar( CP_UTF8, 0, pBuf, -1, NULL, 0 ); 
	pResult = (wchar_t *)malloc((textlen + 1)*sizeof(wchar_t)); 
	memset(pResult, 0, (textlen + 1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_UTF8, 0,pBuf,-1,(LPWSTR)pResult,textlen ); 


	//招行返回值为utf-8格式的数据
	revinfo = pResult;
	delete[] pResult;

	delete[] pBuf;

	InternetCloseHandle(m_hInetConnection);
	InternetCloseHandle(m_hInetFile);
	InternetCloseHandle(m_hInetSession);

	if(sucess == false)
	{
		revinfo = L"";
		return 1020;
	}
	return 0;
}
void AnalyticData(wstring& info, LPMONTHBILLRECORD pmcRecord, LPMONTHBILLRECORD pmuRecord, string currentmonth)
{
	USES_CONVERSION;
	SYSTEMTIME time;
	GetLocalTime(&time);
	char cbtime[256] = {0};
	sprintf_s(cbtime, 256, "%d%02d%02d", time.wYear, time.wMonth, time.wDay);
	string localmonth = cbtime; 

	//首先验证数据是否是正确的招行对账单
	size_t ntitle = info.find(L"招商银行信用卡对账单");
	if(ntitle == wstring::npos)
	{
		pmcRecord->m_isSuccess = false;
		pmuRecord->m_isSuccess = false;
		return;
	}

	//开始解析数据
	int sjtype = 0;
	for(;sjtype < 2;sjtype ++)
	{
		LPMONTHBILLRECORD pnRecord = (sjtype==0) ? pmcRecord:pmuRecord;
		pnRecord->m_isSuccess = true;

		size_t nctitle;
		if(sjtype == 0)
			nctitle = info.find(L"dgRMBList");
		else
			nctitle = info.find(L"dgUSDList");
		bool isAutoBuy = false;//对自动购汇的标示

		LPTRANRECORD preRecord = NULL;
		if(nctitle != wstring::npos)
		{
			isAutoBuy = false;
			size_t cbegin = info.find(L"Original Trans Amount", nctitle);
			if(cbegin != wstring::npos)
			{
				size_t cend = info.find(L"</table>", nctitle);
				wstring cinfo = info.substr(cbegin, cend - cbegin);//账单数据

				size_t nCur = 0;
				for(;nCur != wstring::npos;)
				{
					size_t nbtr = cinfo.find(L"<tr", nCur);//解析出一条一条的数据
					if(nbtr != wstring::npos)
					{
						size_t netr = cinfo.find(L"</tr>", nbtr);
						nCur = netr;
						wstring ctr = cinfo.substr(nbtr + 4, netr - nbtr - 4);

						LPTRANRECORD ptrRecord = preRecord;
						if(false == isAutoBuy)
							ptrRecord = new TRANRECORD;

						if(ptrRecord == NULL)
							ptrRecord = new TRANRECORD;
						
						size_t nBcur = 0;
						int i = 0;
						for(;i < 7; i ++)
						{
							size_t nbegintr = ctr.find(L"<td", nBcur);

							if(nbegintr == wstring::npos)
								break;

							size_t nbegintr2 = ctr.find(L">", nbegintr);

							if(nbegintr2 == wstring::npos)
								break;

							size_t nendtr = ctr.find(L"<", nbegintr2);

							if(nendtr == wstring::npos)
								break;
							nBcur = nendtr + 1;

							wstring laData = ctr.substr(nbegintr2 + 1, nendtr - nbegintr2 - 1);//得到其中的中间记录
							
							wstring lData = FilterStringW(laData);//过滤掉其中的空格
							if(lData == L"&nbsp;")
								lData = L"";
							
							switch(i)
							{
							case 0:
								{
									if( lData != L"")
									{
										string preMonth;
										preMonth = currentmonth.substr(0,4);
										string nmonth = W2A(lData.c_str());																	
										nmonth = FilterMonthStringNumber(nmonth);
										string amonth;
										amonth = preMonth + nmonth;

										if (amonth.substr(0,6) > currentmonth)
										{
											int iYear = atoi(preMonth.c_str());
											iYear--;
											char ayear[128] = {0};
											sprintf_s(ayear, 128, "%d", iYear);
											amonth = ayear + nmonth;
										}

										sprintf_s(ptrRecord->TransDate, 256, "%s", amonth.c_str());

									}
									break;
								}
							case 1:
								{
									if( lData != L"")
									{
										string preMonth;
										preMonth = currentmonth.substr(0,4);
										string nmonth = W2A(lData.c_str());																	
										nmonth = FilterMonthStringNumber(nmonth);
										string amonth;
										amonth = preMonth + nmonth;

										if (amonth.substr(0,6) > currentmonth)
										{
											int iYear = atoi(preMonth.c_str());
											iYear--;
											char ayear[128] = {0};
											sprintf_s(ayear, 128, "%d", iYear);
											amonth = ayear + nmonth;
										}

										sprintf_s(ptrRecord->PostDate, 256, "%s", amonth.c_str());
										if(strlen(ptrRecord->TransDate) < 5 )//招行里面没有交易日期的话，用记账日期补齐
											strcpy_s(ptrRecord->TransDate, 256, ptrRecord->PostDate);
									}
									break;
								}
							case 2:
								{

									if( lData != L"")
									{
										if(isAutoBuy == true)
										{										
											sprintf_s(ptrRecord->Description, 256, "%s(%s", ptrRecord->Description, W2A(lData.c_str()));
										}
										else
										{											
											strcpy_s(ptrRecord->Description, 256, W2A(lData.c_str()));
										}
									}


									break;
								}
							case 3:
								{
									if( lData != L"")
									{
										string scr = W2A(lData.c_str());
										scr = FilterStringNumber(scr);
										if(scr != "")
											sprintf_s(ptrRecord->Amount, 256, "%s", scr.c_str());
									}
									break;
								}
							case 4:
								{
									if( lData != L"")
									{
										string scr = W2A(lData.c_str());
										scr = FilterStringNumber(scr);
										if(scr != "")
											ptrRecord->CardNumber = atoi(scr.c_str());
										else
											ptrRecord->CardNumber = 0;
									}

									break;
								}
							case 5:
								{
									if( lData != L"")
										strcpy_s(ptrRecord->Country, 256, W2A(lData.c_str()));
									break;
								}
							case 6:
								{
									if( lData != L"")
										if(isAutoBuy == true)
										{
											if(ptrRecord != NULL)
												sprintf_s(ptrRecord->Description, 256, "%s,人民币 %s)", ptrRecord->Description, W2A(lData.c_str()));

										}
								}
								break;

							default:
								break;
							}

							
						}


						if((strlen(ptrRecord->Description)) > 0 && (strcmp(ptrRecord->Description, "自动购汇") == 0))
						{
							isAutoBuy = true;
							preRecord = ptrRecord;
						}
						else
							isAutoBuy = false;
						if(isAutoBuy == false)
						{												
							preRecord = ptrRecord;
							pnRecord->TranList.push_back(ptrRecord);
						}
						
					}
					else
						break;

				}
			}
			

		}
	}

}

float dllbalance[2] = {0, 0};


int WINAPI FetchBillFunc(IWebBrowser2* pFatherWebBrowser, IWebBrowser2* pChildWebBrowser, BillData* pData, int &step, LPBILLRECORDS plRecords)
{
	USES_CONVERSION;
	int state = 0;
	if(pData->aid == "a006")
	{
		RecordInfo(L"CMBChinaDll", 1800, L"进入导入账单处理%d", step);
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
				case 1://切换信用卡页面
					{
						dllbalance[0] = 0;
						dllbalance[1] = 0;
						DISPPARAMS dispparams;
						memset(&dispparams, 0, sizeof dispparams);
						dispparams.cArgs      = 1;
						dispparams.rgvarg     = new VARIANT;
						dispparams.cNamedArgs = 0;

						//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数
						dispparams.rgvarg[0].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("C"));
						dispparams.rgvarg[0].vt = VT_BSTR;
						CallJScript2(doc, "changeLoginType", dispparams);

						// 释放所有申请的空间

						if(dispparams.rgvarg[0].vt == VT_BSTR)
							::SysFreeString(dispparams.rgvarg[0].bstrVal);
						dispparams.rgvarg[0].bstrVal = NULL;
						delete dispparams.rgvarg;
					}
					break;
				case 2:
					{
						BSTR pbUrl = NULL;
						pChildWebBrowser->get_LocationURL(&pbUrl);
						if(NULL != pbUrl)
						{
							wstring url = pbUrl;
							if(url.find(L"/Login/HomePage.aspx") != wstring::npos)
							{
								//跳转到信息查询页
								CHECHSTATE;
								const int arraySize = 5;
								//Putting parameters
								DISPPARAMS dispparams;
								memset(&dispparams, 0, sizeof dispparams);
								dispparams.cArgs      = arraySize;
								dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
								dispparams.cNamedArgs = 0;

								dispparams.rgvarg[0].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("mainWorkArea"));//NULL;
								dispparams.rgvarg[0].vt = VT_BSTR;
								dispparams.rgvarg[1].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("FORM"));
								dispparams.rgvarg[1].vt = VT_BSTR;
								dispparams.rgvarg[2].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("CreditCardV2/cm_QueryCreditLimit.aspx"));
								dispparams.rgvarg[2].vt = VT_BSTR;
								dispparams.rgvarg[3].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("CBANK_CREDITCARDV2"));
								dispparams.rgvarg[3].vt = VT_BSTR;
								dispparams.rgvarg[4].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("C"));
								dispparams.rgvarg[4].vt = VT_BSTR;
								RecordInfo(L"CMBChinaDll", 1800, L"执行CallFuncEx2");
								CallJScript2(doc, "CallFuncEx2", dispparams);

								RecordInfo(L"CMBChinaDll", 1800, L"执行完CallFuncEx2");
								// 释放所有申请的空间
								for(int i = 0; i< 5;i ++)
								{
									if(dispparams.rgvarg[i].vt == VT_BSTR)
										::SysFreeString(dispparams.rgvarg[i].bstrVal);
									dispparams.rgvarg[i].bstrVal = NULL;
								}
								delete[] dispparams.rgvarg;
								step = 3;
							}
						}
					}
						break;
				case 3:
					{
						BSTR pbUrl = NULL;
						pChildWebBrowser->get_LocationURL(&pbUrl);
						if(NULL != pbUrl)
						{
							wstring url = pbUrl;
							if(url.find(L"/CreditCardV2/cm_QueryCreditLimit.aspx") != wstring::npos)
							{
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
								//跳转到了查询标签页,获得账号末4位
								hr = doc_c->get_body( &elem);

								wstring info;
								if(elem!=NULL)   
								{      
									BSTR pbBody = NULL;
									elem->get_innerHTML(&pbBody);   //类似的还有put_innerTEXT
									if(pbBody != NULL)
										info = pbBody;
									elem->Release();   
									elem = NULL;
								}
								if (doc_c) { doc_c->Release(); doc_c = NULL; }
								wstring card;
								size_t nPos = info.find(L"DspCreditCardNo=");
								if(nPos != wstring::npos)
								{
									wstring moneytemp = info.substr(nPos + 28, nPos + 60);
									int pos = moneytemp.find(L"','");
									card = moneytemp.substr(0, pos);
								}
								if(card == L"")
								{
									RecordInfo(L"Alipay", 1800, L"页面账户没找到");
									state = BILL_GET_ACCOUNT_ERROR;
									break;
								}
								
								USES_CONVERSION;								
								string tag = W2A(card.c_str());
								strcpy_s(plRecords->tag, 256, tag.c_str());
								plRecords->type = pData->type;
								plRecords->accountid = pData->accountid;

								if(card != L"" && pData->tag != "")
								{
									if(pData->tag != tag)
									{
										ShowNotifyWnd(false);//隐藏正在登陆框

										WCHAR sInfo[256] = { 0 };
										swprintf(sInfo, 256, L"当前账户已绑定招行卡号末4位%s，实际导入招行卡号与原账户不一致，是否继续导入",A2W(pData->tag.c_str()));

										HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
										if(MessageBoxW(hMainFrame, sInfo, L"财金汇", MB_OKCANCEL) == IDCANCEL)
										{
											state = BLII_NEED_RESTART;
											break;
										}

										ShowNotifyWnd(true);//显示正在登陆
									}								
								}
								CHECHSTATE;

								//跳转到已出账单查询页
								//../CreditCardV2/am_QueryReckoningSurvey.aspx
								// 下面这步还可以优化，用post方式实现，改动较多，暂时不动
								Sleep(1500);
								const int arraySize = 5;
								//Putting parameters
								DISPPARAMS dispparams;
								memset(&dispparams, 0, sizeof dispparams);
								dispparams.cArgs      = arraySize;
								dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
								dispparams.cNamedArgs = 0;

								dispparams.rgvarg[0].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("mainWorkArea"));//NULL;
								dispparams.rgvarg[0].vt = VT_BSTR;
								dispparams.rgvarg[1].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("FORM"));
								dispparams.rgvarg[1].vt = VT_BSTR;
								dispparams.rgvarg[2].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("CreditCardV2/am_QueryReckoningSurvey.aspx"));
								dispparams.rgvarg[2].vt = VT_BSTR;
								dispparams.rgvarg[3].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("CBANK_CREDITCARDV2"));
								dispparams.rgvarg[3].vt = VT_BSTR;
								dispparams.rgvarg[4].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("C"));
								dispparams.rgvarg[4].vt = VT_BSTR;
								//RecordInfo(L"CMBChinaDll", 1800, L"执行CallFuncEx2");
								CallJScript2(doc, "CallFuncEx2", dispparams);

								//RecordInfo(L"CMBChinaDll", 1800, L"执行完CallFuncEx2");
								// 释放所有申请的空间
								for(int i = 0; i< 5;i ++)
								{
									if(dispparams.rgvarg[i].vt == VT_BSTR)
										::SysFreeString(dispparams.rgvarg[i].bstrVal);
									dispparams.rgvarg[i].bstrVal = NULL;
								}
								delete[] dispparams.rgvarg;
								step = 4;
							}
						}


					}


					break;

				case 4:
					{
						BSTR pbUrl = NULL;
						pChildWebBrowser->get_LocationURL(&pbUrl);
						if(NULL != pbUrl)
						{
							wstring url = pbUrl;
							if(url.find(L"/CreditCardV2/am_QueryReckoningSurvey.aspx") != wstring::npos)
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
								//获取rmb最近一个月的账单金额，放入余额中
								hr = doc_c->get_body( &elem);

								wstring info;
								if(elem != NULL)   
								{      
									BSTR pbBody = NULL;//::SysAllocString(OLESTR("Written by IHTMLDocument2:招商银行首页需要进行显示."));
									elem->get_innerHTML(&pbBody);   //类似的还有put_innerTEXT
									if(pbBody != NULL)
										info = pbBody;
									elem->Release();   
									elem = NULL;
								}
								if (doc_c) { doc_c->Release(); doc_c = NULL; }
	
								bool bGetUSD = false;
								bool bGetRMB = false;
								wstring bills;
								size_t nPos = info.find(L"账单分期");
								if(nPos != wstring::npos)
								{
									int end = info.find(L"fm_AdjustExplainFrameSize", nPos);
									if(end != wstring::npos)
									{
										bills = info.substr(nPos, end - nPos);
									}
								}

								if(bills == L"")
								{
									RecordInfo(L"CMBChinaDll", 1800, L"账单分期失败%d", step);
								}
								else
								{
									int rasize = bills.find(L"￥");
									if(rasize != wstring::npos)
									{
										int rbsize = bills.find(L"</", rasize);
										if(rbsize != wstring::npos)
										{
											//获得最近一期的人民币账单金额

											// 获得rmb账单余额											
											wstring rmbtra = bills.substr(rasize + 1, rbsize - rasize - 1);
											USES_CONVERSION;
											string srmbtra = W2A(rmbtra.c_str());
											dllbalance[0] = atof(FilterStringNumber(srmbtra).c_str());//放入rmb最近一期的金额
											bGetRMB = true;
											
											int uasize = bills.find(L"＄", rbsize);
											if(uasize != wstring::npos)
											{
												int ubsize = bills.find(L"</", uasize);
												if(ubsize != wstring::npos)
												{											
													wstring usdtra = bills.substr(uasize + 1, ubsize - uasize - 1);
													string sustra = W2A(usdtra.c_str());
													dllbalance[1] = atof(FilterStringNumber(sustra).c_str());
													bGetUSD = true;
												}												
											}
										}
									}									
								}
								wstring ClientNo;
								size_t bnpos = info.find(L"id=ClientNo");
								if(bnpos != wstring::npos)
								{
									nPos = info.find(L"value=",bnpos);
									if(nPos != wstring::npos)
									{
										int cnpos = info.find(L" ", nPos);
										if(cnpos != wstring::npos)
											ClientNo = info.substr(nPos + 6, cnpos - nPos - 6);

									}
								}
								if(ClientNo != L"" && (bGetRMB == true && bGetUSD == true))
								{
									bGetRMB = false;bGetUSD = false;
									BSTR pbCookie = NULL;
									doc->get_cookie(&pbCookie);
									wstring cookie = pbCookie;
									wstring postdata = L"ClientNo=" + ClientNo + L"&index=2";
									wstring page = L"/CmbBank_CreditCardV2/UI/CreditCardPC/CreditCardV2/am_QueryReckoningNotRMB.aspx";
									
									//发送结果，获得rmb未出账单和美元未出账单
									wstring revinfo;
									USES_CONVERSION;
									CHECHSTATE;
									if (TransferDataPost(page, cookie, postdata, revinfo) == 0)
									{
										CHECHSTATE;
										//[['人民币金额','right'],'￥365.75','￥1,048.00'],
										//获得了人民币未出账单部分的余额
										wstring tbills;
										float rbalance = 0;
										size_t nPos = revinfo.find(L"['人民币金额','right']");
										if(nPos != string::npos)
										{
											wstring moneytemp = revinfo.substr(nPos + 16, string::npos);
											int pos = moneytemp.find(L"]");

											if(pos != wstring::npos)
												bGetRMB = true;

											if(pos > 1)
												tbills = moneytemp.substr(1, pos - 1);

											if(tbills != L"")
											{
												rbalance = GetBalance(tbills);
												bGetRMB = true;
												dllbalance[0] += rbalance;
											}
										}
										else
										{
											RecordInfo(L"CMBChinaDll", 1800, L"人民币金额获取失败nPos");											
										}
									}
									if(bGetRMB == false)
										RecordInfo(L"CMBChinaDll", 1800, L"人民币金额获取失败TransferDataPost");

									postdata = L"ClientNo=" + ClientNo;
									page = L"/CmbBank_CreditCardV2/UI/CreditCardPC/CreditCardV2/am_QueryReckoningNotUSD.aspx";

									//发送结果，获得rmb未出账单和美元未出账单
									revinfo = L"";
									CHECHSTATE;
									if (TransferDataPost(page, cookie, postdata, revinfo) == 0)
									{
										CHECHSTATE;
										//[['人民币金额','right'],'￥365.75','￥1,048.00'],
										//获得了人民币未出账单部分的余额
										wstring wrevinfo = revinfo;
										wstring tbills;

										float ubalance = 0;
										size_t nPos = wrevinfo.find(L"['美元金额','right']");
										if(nPos != string::npos)
										{
											wstring moneytemp = wrevinfo.substr(nPos + 16, string::npos);
											int pos = moneytemp.find(L"]");

											if(pos != wstring::npos)
												bGetUSD = true;

											if(pos > 1)
												tbills = moneytemp.substr(1, pos - 1);

											if(tbills != L"")
											{
												ubalance = GetBalance(tbills);
												dllbalance[1] += ubalance;
											}
										}
										else
										{
											RecordInfo(L"CMBChinaDll", 1800, L"GetUSD金额获取失败nPos");											
										}
									}
									if(bGetUSD == false)
										RecordInfo(L"CMBChinaDll", 1800, L"GetUSD金额获取失败TransferDataPost");

								}
								else
								{
									bGetRMB = false;
									bGetUSD = false;
								}
				
								strcpy_s(plRecords->aid, 256, pData->aid.c_str());
								
								LPBILLRECORD pRecord = new BILLRECORD;
								pRecord->type = RMB;
								if(bGetRMB == false)
									pRecord->balance = "F";
								else
								{
									char cbalance[256] = {0};
									if(dllbalance[0] > 0.009 || dllbalance[0] < -0.009)
										sprintf_s(cbalance, 256, "%.2f", -dllbalance[0]);
									else
										sprintf_s(cbalance, 256, "0.00");
									pRecord->balance = cbalance;
								}

								plRecords->BillRecordlist.push_back(pRecord);

								LPBILLRECORD puRecord = new BILLRECORD;
								puRecord->type = USD;
								if(bGetUSD == false)
									puRecord->balance = "F";
								else
								{
									char cbalance[256] = {0};
									if(dllbalance[1] > 0.009 || dllbalance[1] < -0.009)
										sprintf_s(cbalance, 256, "%.2f", -dllbalance[1]);
									else
										sprintf_s(cbalance, 256, "0.00");
									puRecord->balance = cbalance;
								}
								plRecords->BillRecordlist.push_back(puRecord);

								//获得所有的月份信息
								size_t npos = 0;
								int itest = 0;
								for(;npos < bills.size();)
								{
									wstring curmonth;
									int apos = bills.find(L"am_ReSendReckoning.aspx", npos);
									if(apos == wstring::npos)
									{
										if(itest == 0)
										{
											RecordInfo(L"CMBChinaDll", 1800, L"获取所有的月份数据TD align=right获取失败%d", step);
											state = BILL_COM_ERROR;
										}
										break;
									}
									itest ++;
									int bpos = bills.find(L"IN_YYYYMM=", apos);
									if(bpos == wstring::npos)
									{
										RecordInfo(L"CMBChinaDll", 1800, L"获取所有的月份数据TD align=right获取失败%d", step);
										state = BILL_COM_ERROR;
										break;
									}

									int cpos = bills.find(L"','", bpos);
									if(cpos == wstring ::npos)
									{
										RecordInfo(L"CMBChinaDll", 1800, L"获取所有的月份数据TD align=right获取失败%d", step);
										state = BILL_COM_ERROR;
										break;
									}


									curmonth = bills.substr(bpos + 10, cpos - bpos - 10);


									string month = W2A(curmonth.c_str());

									SELECTINFONODE mNode;
									strncpy_s(mNode.szNodeInfo, 256, month.c_str(), month.length());
									mNode.dwVal = CHECKBOX_SHOW_UNCHECKED;
									plRecords->m_mapBack.push_back(mNode);

									npos = bills.find(L"</",cpos);
									if(npos == wstring::npos)
										break;
								}	
								if(state != BILL_COM_ERROR)
									state = BILL_SELECT_MONTH;
							}
						}
					}

					break;
				case 5:
					{
						step = 9;//修改step，防止多次调用
						//减少崩溃的概率
						list<string> m_selectmonth;
						list<SELECTINFONODE>::iterator ite = plRecords->m_mapBack.begin();
						for(; ite != plRecords->m_mapBack.end();ite ++)
						{
							if(ite->dwVal == CHECKBOX_SHOW_CHECKED)
							{
								string month = ite->szNodeInfo;
								m_selectmonth.push_back(month);
							}
						}
						plRecords->m_mapBack.clear();
						if(m_selectmonth.size() == 0)
						{
							plRecords->isFinish = true;
							state = BILL_FINISH_STATE;
							break;//对未选择月份进行处理，直接退出
						}
						//选完月份马上清除
						
						USES_CONVERSION;
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
						wstring info;
						hr = doc_c->get_body( &elem);
						if(elem != NULL)   
						{      
							BSTR pbBody = NULL;//::SysAllocString(OLESTR("Written by IHTMLDocument2:招商银行首页需要进行显示."));
							elem->get_innerHTML(&pbBody);   //类似的还有put_innerTEXT
							if(pbBody != NULL)
								info = pbBody;
							elem->Release();   
							elem = NULL;
						}
						if (doc_c) { doc_c->Release(); doc_c = NULL; }	

						wstring CreditAccNo;

						int apos = info.find(L"CreditAccNo=");
						if(apos == wstring::npos)
							break;
						int bpos = info.find(L"',", apos);
						CreditAccNo = info.substr(apos, bpos - apos);

						wstring ClientNo;						
						size_t bnpos = info.find(L"id=ClientNo");
						if(bnpos != wstring::npos)
						{
							size_t nPos = info.find(L"value=",bnpos);
							if(nPos != wstring::npos)
							{
								int cnpos = info.find(L" ", nPos);
								if(cnpos != wstring::npos)
									ClientNo = info.substr(nPos + 6, cnpos - nPos - 6);

							}
						}

						list<string>::iterator lite = m_selectmonth.begin();
						LPBILLRECORD pcRecord = NULL;
						LPBILLRECORD puRecord = NULL;
						list<LPBILLRECORD> ::iterator pite = plRecords->BillRecordlist.begin();
						//找到已经生成的人民币账单和美元账单地址
						for(;pite != plRecords->BillRecordlist.end(); pite ++)
						{
							if((*pite)->type == RMB)
								pcRecord = (*pite);
							else
								puRecord = (*pite);

						}

						//triggerFunc('../CreditCardV2/am_QueryReckoningList.aspx','FORM','201109','CreditAccNo=0114700657001001','IN_YYYYMM=201109');
						int ntime = 0;
						for(;lite != m_selectmonth.end(); lite ++)
						{
							string currentmonth = FilterMonthStringNumber(*lite);
							wstring month = A2W(currentmonth.c_str());
							wstring ninfo = L"正在导入" + month.substr(0, 4) + L"年" + month.substr(4, 2) + L"月账单......";
							//ClientNo=161544C5565DC6059DA8BDBAFE364832166854439912247400036413&CreditAccNo=0114700657001001&IN_YYYYMM=201110

							BSTR pbCookie = NULL;
							doc->get_cookie(&pbCookie);
							wstring cookie = pbCookie;
							wstring postdata = L"ClientNo=" + ClientNo + L"&" + CreditAccNo + L"&IN_YYYYMM=" + month;
							wstring page = L"/CmbBank_CreditCardV2/UI/CreditCardPC/CreditCardV2/am_QueryReckoningList.aspx";

							//发送结果，获得rmb未出账单和美元未出账单
							wstring revinfo;
							LPMONTHBILLRECORD pmcRecord = new MONTHBILLRECORD;
							LPMONTHBILLRECORD pmuRecord = new MONTHBILLRECORD;
							pmcRecord->month = currentmonth;
							pmuRecord->month = currentmonth;

							ChangeNotifyWord((WCHAR*)ninfo.c_str());
							if (TransferDataPost(page, cookie, postdata, revinfo) == 0)
							{
							
								AnalyticData(revinfo, pmcRecord, pmuRecord, currentmonth);//这里面会自动放入是否成功
														

							}
							else
							{
								pmcRecord->m_isSuccess = false;
								pmuRecord->m_isSuccess = false;
							}
							//无论如何都要插入，否则存在内存泄露
							pcRecord->bills.push_back(pmcRecord);
							puRecord->bills.push_back(pmuRecord);
							//插入数据后才做检查，否则可能有内存泄露
							CHECHSTATE;
						}
						if(g_bState == 0)
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

CComVariant CallJScript2(IHTMLDocument2 *doc, std::string strFunc, DISPPARAMS& dispparams)
{
	//Getting IDispatch for Java Script objects
	if(!doc)
		return false;

	// 获得JS脚本
	CComPtr<IDispatch> spScript;
	if(!SUCCEEDED(doc->get_Script(&spScript)))
		return false;
	//
	USES_CONVERSION;
	BSTR bstrMember = ::SysAllocString((LPOLESTR)A2COLE(strFunc.c_str()));
	DISPID dispid = NULL;
	HRESULT hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
		LOCALE_SYSTEM_DEFAULT,&dispid);
	if(FAILED(hr))
	{
		::SysFreeString(bstrMember);
		return false;
	}


	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT) -1;      
	// initialize to invalid arg
	// Call JavaScript function
	hr = spScript->Invoke(dispid,IID_NULL,0,DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	::SysFreeString(bstrMember);
	bstrMember = NULL;

	if(FAILED(hr))
	{
		return false;
	}

	return vaResult;
}
