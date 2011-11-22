// ccbGetBill.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <map>
#include <list>
#include <vector>
#include "../../BankData/BankData.h"
#include "../../ThirdParty/RunLog/RunLog.h"

HWND g_notifyWnd = NULL;
int g_bState = 0;

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
		::SendMessage(g_notifyWnd, WM_BILL_CHANGE_NOFIFY, 0, (LPARAM)info);
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

list<LPMONTHBILLRECORD> billsrmb;
list<LPMONTHBILLRECORD> billsusd;
std::map<std::string,int> strmonthsnum;		//	
std::string strbalances[2];
bool   isFIRST = true;
bool   isRMB = true;
int    mRMB =0;
int m_month = 0;
IHTMLDocument2 *doc2			= NULL;
//map<std::string, bool> m_mapBack;
std::string cstrselect;

CComVariant CallJScript(IHTMLDocument2 *doc, std::string strFunc,std::vector<std::string>& paramVec);
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
string ReverseNumber(const string& scr)
{
	if(scr == "F")
		return "F";
	float res = atof(scr.c_str());

	char cbalance[256] = {0};
	if(res > 0.009 || res < -0.009)
		sprintf_s(cbalance, 256, "%.2f", -res);
	else
		sprintf_s(cbalance, 256, "0.00");

	string sres = cbalance;

	return sres;
}
void  InitDLL()
{
		isFIRST = true;
		isRMB = true;
		mRMB = 0;
		m_month = 0;
    	strmonthsnum.clear();
    	billsrmb.clear();
    	billsusd.clear();
		//m_mapBack.clear();
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
	memset(plRecords->tag, 0, 256);
	plRecords->isFinish = false;
	plRecords->m_mapBack.clear();

	if (doc2) { doc2->Release(); doc2 = NULL; }
	InitDLL();

}


// 得到document2指针
IHTMLDocument2* GetDocByFramesName(IWebBrowser2* pWebBrowser, wstring strframe)
{
	IDispatch *docDisp1			= NULL;
	HRESULT hr;

	hr = pWebBrowser->get_Document(&docDisp1);
	if (SUCCEEDED(hr) && docDisp1 != NULL)
		hr = docDisp1->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&doc2));	

	CComPtr<IHTMLFramesCollection2> pFrame = NULL;
	if(!SUCCEEDED(doc2->get_frames(&pFrame)))
		return NULL;

	VARIANT sPram;
	sPram.vt = VT_BSTR;
	sPram.bstrVal = SysAllocString(strframe.c_str());            //L"accountFrm";

	VARIANT frameOut;//找到iFrame对象
	frameOut.vt=VT_DISPATCH; 
	if(!SUCCEEDED(pFrame->item(&sPram, &frameOut)))
		return NULL;

	CComPtr<IHTMLWindow2> pRightFrameWindow = NULL;
	if (!SUCCEEDED(V_DISPATCH(&frameOut)->QueryInterface(IID_IHTMLWindow2,
		(void**)&pRightFrameWindow)))
		return NULL;

	//CComPtr<IHTMLDocument2> pRightDoc = NULL;
	if (!SUCCEEDED(pRightFrameWindow->get_document(&doc2)))
		return NULL;

	return doc2;

}


int WINAPI FetchBillFunc(IWebBrowser2* pFatherWebBrowser, IWebBrowser2* pChildWebBrowser, BillData* pData, int &step, LPBILLRECORDS plRecords )
{
	USES_CONVERSION;
	int state = 0;
	if(pData->aid == "a004")
	{
		//RecordInfo(L"ccbDll", 1800, L"进入导入账单处理%d", step);
		HRESULT hr = S_OK;
		IDispatch *docDisp			= NULL;
		IHTMLDocument2 *doc			= NULL;
		IHTMLDocument3 *doc3		= NULL;
		IHTMLElement *elem			= NULL;
		IHTMLWindow2* m_windowHeader= NULL;
		IDispatch *docDispf			= NULL;
		IHTMLDocument2 *docf			= NULL;
		IHTMLWindow2* m_windowHeaderf= NULL;

		hr = pFatherWebBrowser->get_Document(&docDispf);
		hr = pChildWebBrowser->get_Document(&docDisp);
		if (SUCCEEDED(hr) && docDisp != NULL)
		{					
			hr = docDispf->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&docf));	
			hr = docDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&doc));	
			
			if (SUCCEEDED(hr) || doc != NULL) 
			{

            	hr = docf->get_parentWindow(&m_windowHeaderf) ;
            	hr = doc->get_parentWindow(&m_windowHeader) ;

				switch(step)
				{					
				case 1:
					break;
				case 2:
					break;
				case 3:
					{
						InitDLL();
    					const int arraySize = 0;
						//Putting parameters
						DISPPARAMS dispparams;
						memset(&dispparams, 0, sizeof dispparams);
						dispparams.cArgs      = arraySize;
						dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
						dispparams.cNamedArgs = 0;

						//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
						//RecordInfo(L"ccbDll", 1800, L"执行CallFuncEx2");  $(".close").click()   informMerchant
						CallJScript2(docf, "initAccountSearch", dispparams);

						// 释放所有申请的空间
						for(int i = 0; i< 0;i ++)
						{
							if(dispparams.rgvarg[i].vt == VT_BSTR)
								::SysFreeString(dispparams.rgvarg[i].bstrVal);
							dispparams.rgvarg[i].bstrVal = NULL;
						}
						delete[] dispparams.rgvarg;

					}
					break;
				case 6:
					{
					}
					break;
				case 8:
					{
							if(!isFIRST)
								InitDLL();

							IHTMLElementCollection *pCollection			= NULL;
							IHTMLElement* pElement= NULL;

							//hr = pWebBrowser->get_Document(&docDisp);
							//if (SUCCEEDED(hr) && docDisp != NULL)
							{
								hr = docDisp->QueryInterface(IID_IHTMLDocument3, reinterpret_cast<void**>(&doc3));
 								if(SUCCEEDED(hr) || doc3 != NULL)
								{
									 doc3->getElementsByTagName(_T("input"),&pCollection);
									 int m_begin = 0;
									 long trlength ;
									 pCollection->get_length(&trlength);
									 for(long i=0; i<trlength; i++)
				        			 {
											IDispatch *pDispInputTd = NULL;   
											CComVariant vIndex   =  i; 
											_variant_t   name; 
											hr = pCollection->item(vIndex,vIndex,&pDispInputTd);
													if(!SUCCEEDED(hr)) break;
											hr = pDispInputTd->QueryInterface(IID_IHTMLElement,(void**)&pElement);
													if(!SUCCEEDED(hr)) break;
											CComVariant vVal;	
											//vVal ="";
											CString   str; 
											pElement->getAttribute(_T("name"),2,&vVal);
											str = vVal.bstrVal;
											if(str == "dates")
											{
												 m_begin++;
												 pElement->getAttribute(_T("value"),2,&vVal);
												 str = vVal.bstrVal;
												 std::string  stemp =  CT2A(str);
												 stemp = stemp.substr(0, 7);
													SELECTINFONODE selectNode;
													sprintf_s(selectNode.szNodeInfo, 256, "%s", stemp.c_str());
													selectNode.dwVal = CHECKBOX_SHOW_UNCHECKED;
													plRecords->m_mapBack.push_back(selectNode);
												 //plRecords->m_mapBack.insert(std::make_pair(stemp, false));
												 //m_mapBack.insert(std::make_pair(stemp, false));
												 strmonthsnum.insert(std::make_pair(stemp, m_begin));
											}
											else if(str == "ACCT_NO")
											{
												 pElement->getAttribute(_T("value"),2,&vVal);
												 str = vVal.bstrVal;
												 string ntag = CT2A(str);
												 if(ntag.size() >= 4)
												 {
													 ntag = ntag.substr(ntag.size() - 4, string::npos);
													 strcpy_s(plRecords->tag, 256, ntag.c_str());
												 }
												 else
												 {
													 state = BILL_COM_ERROR;
												 }
												 strcpy_s(plRecords->tag, 256, ntag.c_str());	
											}

									 }//for(long i=0

								}
							}
							//pElement-> Release();
                    	if (pCollection) { pCollection->Release(); pCollection = NULL; }

						if(state == BILL_COM_ERROR)
							break;
						IHTMLElementCollection *pAll,*pTag;
						IDispatch   *pDisp; 
						long lCount ;
						VARIANT vTemp  = {0}, vID  = {0};
						BSTR text;

						GetDocByFramesName(pFatherWebBrowser, _T("accountFrm"));
						doc2->get_all(&pAll);//获取页面所有元素
						//pRightDoc->Release();
						lCount = 0;
						//定义需要的标签
						BSTR bstrTagName;
						bstrTagName = SysAllocString(L"TD");
						VARIANT  varTag;
						varTag.vt=VT_BSTR;
						varTag.bstrVal=bstrTagName;
						SysFreeString(bstrTagName);
						hr = pAll->tags(varTag,&pDisp);//取得特定tag的接口
						if(pDisp && SUCCEEDED(hr))
						{
						   hr = pDisp->QueryInterface(IID_IHTMLElementCollection,(void **)&pTag);
						   if(pTag && SUCCEEDED(hr))
						   {
								pTag->get_length(&lCount);
								for(int i=0; i<lCount; i++)
								{
									 vID.vt = VT_I4;
									 vID.intVal = i;
									 hr = pTag->item(vID, vTemp, &pDisp);
									 if(pDisp && SUCCEEDED(hr))
									 {
										  BILLRECORD* pRecord = new BILLRECORD;
										  hr = pDisp->QueryInterface(IID_IHTMLElement, (void **)&pElement);
										  if(pElement && SUCCEEDED(hr))
										  {
				    							pElement->get_innerHTML(&text);
				    							pElement->get_id(&text);
												CString  l_tp(text);
												if(l_tp == _T("current_balance0"))
												{
													pElement->get_outerText(&text);
            										CString  l_one(text);
													l_one.TrimLeft();
													l_one.TrimRight();
													if(l_one == "")
			     										strbalances[0] = "F";
													else
														strbalances[0] = FilterStringNumber((string)OLE2A(text));
							 //        					pRecord->type = RMB;
														//pRecord->balance = OLE2A(text);//余额
												}
												else if(l_tp == _T("current_balance1"))
												{
													pElement->get_outerText(&text);
            										CString  l_one(text);
													l_one.TrimLeft();
													l_one.TrimRight();
													if(l_one == "")
			     										strbalances[1] = "F";
													else
														strbalances[1] = FilterStringNumber((string)OLE2A(text));
												}
											   //得到需要的内容
											   pElement->Release();
										  }
										  else
										  {
			     										strbalances[0] = "F";
			     										strbalances[1] = "F";
										  }
									 }//if
								}
								pTag->Release();
					       }
						   pDisp->Release();
						 }
						 pAll->Release();

						 if(pData->tag != "" && strlen(plRecords->tag) >= 4)
						 {
							string scr = plRecords->tag;
							
							if(pData->tag != scr)
							{
								ShowNotifyWnd(false);//隐藏正在登陆框
								WCHAR sInfo[256] = { 0 };
								swprintf(sInfo, 256, L"当前账户已绑定建行卡号末4位%s，实际导入建行卡号与原账户不一致，是否继续导入",A2W(pData->tag.c_str()));

								HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
								if(MessageBoxW(hMainFrame, sInfo, L"财金汇", MB_OKCANCEL) == IDCANCEL)
								{
									state = BLII_NEED_RESTART;
									break;
								}

								ShowNotifyWnd(true);//显示正在登陆
							}
						 }

						 state =BILL_SELECT_MONTH;
						 step =12;
					}
					break;
				case 9:
					{
						if(m_month < 7 )
						{
							IHTMLElementCollection *pCollection			= NULL;
							IHTMLElement* pElement= NULL;

							int tempkkk=0;
							hr = pChildWebBrowser->get_Document(&docDisp);
							if (SUCCEEDED(hr) && docDisp != NULL)
							{
								hr = docDisp->QueryInterface(IID_IHTMLDocument3, reinterpret_cast<void**>(&doc3));
								if(SUCCEEDED(hr) || doc3 != NULL)
								{
										doc3->getElementsByTagName(_T("tr"),&pCollection);
			                			LPMONTHBILLRECORD pMRecord = new MONTHBILLRECORD;
										pMRecord->month = cstrselect;
		                 			    pMRecord->m_isSuccess = true;

										int m_begin = 0;
										long trlength ;
										pCollection->get_length(&trlength);
										for(long i=0; i<trlength; i++)
					        			{
												IDispatch *pDispInputTd = NULL;   
												CComVariant vIndex   =  i; 
												_variant_t   name; 
												hr = pCollection->item(vIndex,vIndex,&pDispInputTd);
														if(!SUCCEEDED(hr)) break;
												hr = pDispInputTd->QueryInterface(IID_IHTMLElement,(void**)&pElement);
														if(!SUCCEEDED(hr)) break;

													 bool b_isdata = false;
								     				 //if(l_tp  =="bg1")
													 {
															CComPtr<IDispatch> pDispatchLast;
															hr = pElement->get_children(&pDispatchLast);
															if(!SUCCEEDED(hr)) break;

															CComPtr<IHTMLElementCollection> lElementCollection;
															hr = pDispatchLast->QueryInterface(IID_IHTMLElementCollection, (VOID**)&lElementCollection);
															if(!SUCCEEDED(hr)) break;

															long llength;
															hr = lElementCollection->get_length(&llength);//到了最里面数据的部分
															if(!SUCCEEDED(hr)) break;

															LPTRANRECORD ptrRecord = new TRANRECORD;

															for(int k = 0; k < llength; k ++)
															{
																IDispatch *pDispInputLast = NULL;   
																CComVariant vIndexk = k;   
																hr = lElementCollection->item(vIndexk, vIndexk, &pDispInputLast);
																if(!SUCCEEDED(hr)) break;

																CComPtr<IHTMLElement> pElemLast;   
																hr = pDispInputLast->QueryInterface(IID_IHTMLElement,(void**)&pElemLast);
																if(!SUCCEEDED(hr)) break;

																BSTR lData = NULL;
																hr = pElemLast->get_innerText(&lData);
            													CString  l_one(lData);
								                				if(l_one  =="交易日" && m_begin != 19999)
																{
																	m_begin = 19999;
																	break;
																}

																if(m_begin == 19999)
																{
																		switch(k)
																		{
																		case 0:
																			{
																				if( lData!= NULL)
																				{
			    		                    			     						 tempkkk = 10;
																					string scr = OLE2A(lData);
																					string s1p ( "" );
																					scr = scr.replace(4,1,s1p);
																					scr = scr.replace(6,1,s1p);
																					strcpy_s(ptrRecord->TransDate, 256, scr.c_str());
																				}
																				break;
																			}
																		case 1:
																			{
																				if( lData!= NULL)
																				{
																					string scr = OLE2A(lData);
																					string s1p ( "" );
																					scr = scr.replace(4,1,s1p);
																					scr = scr.replace(6,1,s1p);
																					strcpy_s(ptrRecord->PostDate, 256, scr.c_str());
																				}
																				break;
																			}
																		case 2:
																			{
																				if( lData!= NULL)
																				{
        			             													CString  l_one(lData);
																					int nnnn = l_one.Find(_T("还款"));
																					if(nnnn >0)
									     												b_isdata = true;
																					strcpy_s(ptrRecord->Description, 256, OLE2A(lData));
																					//strcpy_s(ptrRecord->Payee, 256, OLE2A(lData));
																				}
																				break;
																			}
																		case 3:
																			{
																				//if( lData!= NULL)
																				//	strcpy_s(ptrRecord->Country, 256, OLE2A(lData));
																				break;
																			}
																		case 4:
																			{
																				if( lData!= NULL)
																				{
																					string scr = OLE2A(lData);
																					string scr1 = "-";
																					if(b_isdata)
																					{
																						b_isdata = false;
																		    			scr = scr1 + scr;
																					}
																					strcpy_s(ptrRecord->Amount, 256, FilterStringNumber(scr).c_str());
																				}
																				break;
																			}

																		default:
																			break;
																		}//switch
																}//if(m_begin == 19999)
															}//for(int k = 
															 if(tempkkk == 10)
															 {
																 string ss1;
																 if(isRMB)
														     		 ss1 ="美元帐户正在导入";
																 else
														    		 ss1 ="人民币帐户正在导入";
																 ss1 +=cstrselect;
																 char* ch1 = (char*)ss1.c_str();
																 unsigned lenOfWideCharStr = ::MultiByteToWideChar(CP_ACP,0,ch1,-1,NULL,0); //include '\0' 
																 TCHAR* info = new wchar_t [lenOfWideCharStr]; 
																 ::MultiByteToWideChar(CP_ACP,0,ch1,-1,info,lenOfWideCharStr);
																 ChangeNotifyWord(info);
																 delete[] info;
											    				 pMRecord->TranList.push_back(ptrRecord);
															 }
															 else
															 {
																 string ss1;
																 if(isRMB)
														     		 ss1 ="美元帐户无数据";
																 else
														    		 ss1 ="人民币帐户无数据";
																 ss1 =cstrselect + ss1;
																 char* ch1 = (char*)ss1.c_str();
																 unsigned lenOfWideCharStr = ::MultiByteToWideChar(CP_ACP,0,ch1,-1,NULL,0); //include '\0' 
																 TCHAR* info = new wchar_t [lenOfWideCharStr]; 
																 ::MultiByteToWideChar(CP_ACP,0,ch1,-1,info,lenOfWideCharStr);
																 ChangeNotifyWord(info);
																 delete[] info;
															 }
													 }//if(l_tp  =="bg1")
										 }//for(long i=0
										 //if(tempkkk == 10)
										 {
            					     		if(isRMB)
								    			 billsusd.push_back(pMRecord);
											else
								    			 billsrmb.push_back(pMRecord);
									      }
								}
							}
	                    	if (pElement) { pElement->Release(); pElement = NULL; }
	                    	if (pCollection) { pCollection->Release(); pCollection = NULL; }

							state =BILL_BROWSER_LOOP;
							step =12;
						}//(pData->m_loop == 3)

					}
					break;
				case 12:
					{
    					const int arraySize = 0;
						//Putting parameters
						DISPPARAMS dispparams;
						memset(&dispparams, 0, sizeof dispparams);
						dispparams.cArgs      = arraySize;
						dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
						dispparams.cNamedArgs = 0;

						if(isFIRST)
						{
							isFIRST =false;
							CallJScript2(doc, "load", dispparams);
							CallJScript2(doc, "load_curtype", dispparams);
							CallJScript2(doc, "sub", dispparams);

							//list<SELECTINFONODE>::iterator ites = plRecords->m_mapBack.begin();
							//for(; ites != plRecords->m_mapBack.end() ; ites ++)
							//{
							//	if((*ites).second)
							//	{
							//		std::map<string, bool>::iterator ite = m_mapBack.begin();
							//		for(; ite != m_mapBack.end() ; ite ++)
							//		{
							//			if((*ite).first == (*ites).first)
							//			{
							//				(*ite).second = true;
							//				break;
							//			}
							//		}
							//	}
							//}
						}

						 //int m_month = 0;
						if(isRMB)
						{
							m_month = 0;
							list<SELECTINFONODE>::iterator ites = plRecords->m_mapBack.begin();
							for(; ites != plRecords->m_mapBack.end() ; ites ++)
							{
								if(ites->dwVal == CHECKBOX_SHOW_CHECKED)
								{
									std::map<string, int>::iterator ite = strmonthsnum.begin();
									for(; ite != strmonthsnum.end() ; ite ++)
									{
										if((*ite).first == ites->szNodeInfo)
										{
											std::string tmpstr = (*ite).first;
											string s1p ( "" );
											tmpstr = tmpstr.replace(4,1,s1p);
											tmpstr = tmpstr.substr(0,6);
											cstrselect = tmpstr;
											m_month = (*ite).second;
											break;
										}
									}
									ites->dwVal =CHECKBOX_SHOW_UNCHECKED;
								}
								if(m_month > 0)
									break;
							}
						}

						CString s111,str3;  

						if(isRMB)
							mRMB = m_month;
						else
							m_month = mRMB;

						s111.Format(_T("%d"),m_month);
						str3 = _T("document.getElementById(\"QUERY_DATE\").value = \"")+s111;
						str3 = str3 + _T("\"");
						BSTR bs3 = str3.AllocSysString();

						if(isRMB)
						{
							isRMB = false;
					    	str3 = _T("document.getElementById(\"CUR_TYPE\").value = \"01\"") ;   //14  01
						}
						else
						{
							isRMB = true;
					    	str3 = _T("document.getElementById(\"CUR_TYPE\").value = \"14\"") ;   //14  01
						}
						BSTR   bs1;
					    bs1 = str3.AllocSysString();
						BSTR   bs2(L"javascript"); 
						VARIANT   v = {0};   
						VariantInit(&v); 
						HRESULT hr1;
						if(m_month > 0)
						{
							hr1 = m_windowHeader->execScript(bs3, bs2,&v);
							hr1 = m_windowHeader->execScript(bs1, bs2,&v);
						}

						//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 
						//RecordInfo(L"ccbDll", 1800, L"执行CallFuncEx2");  
						if(m_month > 0)
			        		CallJScript2(doc, "submit_next", dispparams);

						if(m_month == 0)
						{
                    			plRecords->isFinish = true;
								plRecords->accountid = pData->accountid;
                 				strcpy_s(plRecords->aid, 256, pData->aid.c_str());
								plRecords->type = pData->type;
								//std::map<string, int>::iterator ite = strbalance.begin();
								for(int i =0; i<2 ; i ++)
								{
									BILLRECORD* pRecord = new BILLRECORD;
									pRecord->balance =ReverseNumber(strbalances[i]);//余额
									list<LPMONTHBILLRECORD>::iterator itr ;
									if(i == 0)
									{
				    					pRecord->type = RMB;
							    		for (itr = billsrmb.begin (); itr != billsrmb.end (); itr ++) //
							    			pRecord->bills.push_back((*itr));
									}
									else
									{
							     		pRecord->type = USD;
					    				for (itr = billsusd.begin (); itr != billsusd.end (); itr ++) //
						     				pRecord->bills.push_back((*itr));
									}
									plRecords->BillRecordlist.push_back(pRecord);
								}
							    state = BILL_FINISH_STATE;
						}
						// 释放所有申请的空间
						for(int i = 0; i< 0;i ++)
						{
							if(dispparams.rgvarg[i].vt == VT_BSTR)
								::SysFreeString(dispparams.rgvarg[i].bstrVal);
							dispparams.rgvarg[i].bstrVal = NULL;
						}
						delete[] dispparams.rgvarg;
						::SysFreeString(bs1);
						::SysFreeString(bs3);
		     			//m_month--;
					}
					break;
				case 14:
					{
					}
					break;
				case 100:
					{
					}
					break;
				default:
					break;	
				}

				if (docDisp) { docDisp->Release(); docDisp = NULL; }
			}
		}
		else
		{
			state = BILL_COM_ERROR;
		}

		if (elem) { elem->Release(); elem = NULL; }
		if (doc3) { doc3->Release(); doc3 = NULL; }
		if (doc) { doc->Release(); doc = NULL; }
		if (docDisp) { docDisp->Release(); docDisp = NULL; }
		if (docf) { docf->Release(); docf = NULL; }
		if (docDispf) { docDispf->Release(); docDispf = NULL; }  //m_windowHeader
		if (m_windowHeader) { m_windowHeader->Release(); m_windowHeader = NULL; }
		if (m_windowHeaderf) { m_windowHeaderf->Release(); m_windowHeaderf = NULL; }
	}
	return state;
}


CComVariant CallJScript(IHTMLDocument2 *doc, std::string strFunc,std::vector<std::string>& paramVec)
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

	const int arraySize = paramVec.size();
	//Putting parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs      = arraySize;
	dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
	dispparams.cNamedArgs = 0;


	std::vector<std::string>::iterator ite = paramVec.begin();
	int i = 0;
	for( ;ite != paramVec.end(); ite ++)
	{
		//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数
		dispparams.rgvarg[i].bstrVal = ::SysAllocString((LPOLESTR)A2COLE((*ite).c_str()));
		dispparams.rgvarg[i].vt = VT_BSTR;
		i ++;
	}
	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT) -1;      
	// initialize to invalid arg
	// Call JavaScript function
	hr = spScript->Invoke(dispid,IID_NULL,0,DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	// 释放所有申请的空间
	for(i = 0; i< arraySize;i ++)
	{
		::SysFreeString(dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].bstrVal = NULL;
	}
	delete[] dispparams.rgvarg;
	::SysFreeString(bstrMember);
	bstrMember = NULL;

	if(FAILED(hr))
	{
		return false;
	}

	return vaResult;
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
	CComVariant vaResult ;
	UINT nArgErr = (UINT) -1;      
	hr = spScript->Invoke(dispid,IID_NULL,0,DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	::SysFreeString(bstrMember);
	bstrMember = NULL;

	if(FAILED(hr))
	{
		return false;
	}
	return vaResult;
}
