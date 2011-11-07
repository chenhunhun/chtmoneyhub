// eciticGetBill.cpp : Defines the exported functions for the DLL application.
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

bool   isRMB = true;
list<LPMONTHBILLRECORD> billsrmb;
list<LPMONTHBILLRECORD> billsusd;
std::map<std::string,int> strmonthsnum;		//	
std::string strbalances[2];
bool   isFIRST = true;
CString  cstrp[16];
std::string cstrselect;
//map<std::string, bool> m_mapBack;
//bool   isselmon = false;

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

void ChangeNotifyWord(WCHAR* info)
{
	if(g_notifyWnd != 0)
		::SendMessage(g_notifyWnd, WM_BILL_CHANGE_NOFIFY, 0, (LPARAM)info);
}

bool RecordeciticInfo(wstring program, DWORD common, wchar_t *format, ...)
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

void  InitDLL()
{
	isFIRST = true;
	isRMB = true;
	strmonthsnum.clear();
	billsrmb.clear();
	billsusd.clear();
	//m_mapBack.clear();
	//isselmon = false;
}

CComVariant CallJScript(IHTMLDocument2 *doc, std::string strFunc,std::vector<std::string>& paramVec);
CComVariant CallJScript2(IHTMLDocument2 *doc, std::string strFunc, DISPPARAMS& dispparams);
BSTR  ReplaceCHINESE( BSTR bstro);
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

	InitDLL();

}

void CallJSF(IHTMLDocument2 *doc,int m_cstrnum,std::string strFunc)
{
	USES_CONVERSION;
	const int arraySize = m_cstrnum;
	//Putting parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs      = arraySize;
	dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
	dispparams.cNamedArgs = 0;

	for(int k = 0; k< m_cstrnum;k ++)
	{
			int nLen = cstrp[k].GetLength();
			LPCSTR lpszBuf = W2A(cstrp[k].GetBuffer(nLen));
			//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 
			dispparams.rgvarg[k].bstrVal = ::SysAllocString((LPOLESTR)A2COLE(lpszBuf));
			dispparams.rgvarg[k].vt = VT_BSTR;
			cstrp[k].ReleaseBuffer();
	}
	//RecordInfo(L"eciticDll", 1800, L"执行CallFuncEx2");  
	CallJScript2(doc, strFunc, dispparams);


	// 释放所有申请的空间
	for(int i = 0; i< m_cstrnum;i ++)
	{
		if(dispparams.rgvarg[i].vt == VT_BSTR)
			::SysFreeString(dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].bstrVal = NULL;
	}
	delete[] dispparams.rgvarg;

}


int WINAPI FetchBillFunc(IWebBrowser2* pFatherWebBrowser, IWebBrowser2* pChildWebBrowser, BillData* pData, int &step, LPBILLRECORDS plRecords )
{
	USES_CONVERSION;
	int state = 0;
	if(pData->aid == "a007")
	{
		//RecordInfo(L"eciticDll", 1800, L"进入导入账单处理%d", step);
		HRESULT hr = S_OK;
		IDispatch *docDisp			= NULL;
		IHTMLDocument2 *doc			= NULL;
		IHTMLDocument3 *doc3		= NULL;
		IHTMLElement *elem			= NULL;
		IHTMLWindow2* m_windowHeader= NULL;
		IDispatch *docDispf			= NULL;
		IHTMLDocument2 *docf			= NULL;
		IHTMLWindow2* m_windowHeaderf= NULL;
		IHTMLDocument3 *doc3f		= NULL;

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
						if(isFIRST)
						{
							InitDLL();
							IHTMLElementCollection *pCollection			= NULL;
							IHTMLElement* pElement= NULL;
							IDispatch *pDispInputTd = NULL;   

							CString refString;
							//hr = pWebBrowser->get_Document(&docDisp);
							if (SUCCEEDED(hr) && docDisp != NULL)
							{
								hr = docDisp->QueryInterface(IID_IHTMLDocument3, reinterpret_cast<void**>(&doc3));
								if(SUCCEEDED(hr) || doc3 != NULL)
								{
										 doc3->getElementsByTagName(_T("script"),&pCollection);//a tr
										 long trlength ;
										 pCollection->get_length(&trlength);
										 for(long i=0; i<trlength; i++)
				        				 {
											 CComVariant vIndex   =  i; 
											 _variant_t   name; 
											 hr = pCollection->item(vIndex,vIndex,&pDispInputTd);
											 if(!SUCCEEDED(hr)) break;
											 hr = pDispInputTd->QueryInterface(IID_IHTMLElement,(void**)&pElement);
											 if(!SUCCEEDED(hr)) break;
											 BSTR vName = NULL;
											 pElement->get_outerHTML(  &vName ); 
											 CString  l_tp(vName);
											 int lows = 0;
											 lows = l_tp.Find(_T("temp_menu2_url30"));
											 if(lows > 10)
											 {
												l_tp.Delete(0,lows+1);
												lows = l_tp.Find(_T("\'"));
												l_tp.Delete(0,lows+1);
												lows = l_tp.Find(_T("\'"));
												l_tp.Delete(lows,l_tp.GetLength()-lows);
												cstrp[13] = l_tp;
												//break;
											 }
											 lows = l_tp.Find(_T("pb5640_mainCardSetting"));
											 if(lows > 10)
											 {
				     							lows = l_tp.Find(_T("temp_menu2_url1"));
												l_tp.Delete(0,lows+1);
												lows = l_tp.Find(_T("\'"));
												l_tp.Delete(0,lows+1);
												lows = l_tp.Find(_T("\'"));
												l_tp.Delete(lows,l_tp.GetLength()-lows);
												cstrp[14] = l_tp;
												//break;
											 }
										 }
									if (pElement) { pElement->Release(); pElement = NULL; }
									if (pCollection) { pCollection->Release(); pCollection = NULL; }
									if (pDispInputTd) { pDispInputTd->Release(); pDispInputTd = NULL; }
								}
							}
						}
						cstrp[0] = cstrp[13];
						CallJSF(docf ,1,"submitInner");
      				}
					break;
				case 4:
				case 9:
					{
						CallJSF(doc ,0,"submitFun");
					}
					break;
				case 5:
					{
						int m_isdata = 0;
						if(isFIRST)
						{
							isFIRST =false;
							IHTMLElementCollection *pCollection			= NULL;
							IHTMLElement* pElement= NULL;
							IDispatch *pDispInputTd = NULL;   

							hr = docDisp->QueryInterface(IID_IHTMLDocument3, reinterpret_cast<void**>(&doc3));
							if(SUCCEEDED(hr) || doc3 != NULL)
							{
								 int m_begin = 0;
								 long trlength ;
								 doc3->getElementsByTagName(_T("TD"),&pCollection);  //displayYMON
								 BSTR vName = NULL;
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
										pElement->get_innerHTML(  &vName );
										 CString  l_ish(vName);
										int m_ish = 0;
										m_ish = l_ish.Find(_T("displayYMON"));
										if(m_ish >0)
										{
											m_begin++;
											pElement->get_innerText(  &vName );
											CString  l_tp(vName);
											std::string  stemp =  CT2A(l_tp);
													SELECTINFONODE selectNode;
													strncpy(selectNode.szNodeInfo, stemp.c_str(), stemp.length());
													selectNode.dwVal = CHECKBOX_SHOW_UNCHECKED;
													plRecords->m_mapBack.push_back(selectNode);
											//plRecords->m_mapBack.insert(std::make_pair(stemp, false));
											//m_mapBack.insert(std::make_pair(stemp, false));
											strmonthsnum.insert(std::make_pair(stemp, m_begin));
							       		}
								 }//for(long i=0

							     doc3->getElementsByTagName(_T("input"),&pCollection);
								 //long trlength ;
								 pCollection->get_length(&trlength);
								 for(long i=0; i<trlength; i++)
			        			 {
										CComVariant vIndex   =  i; 
										_variant_t   name; 
										hr = pCollection->item(vIndex,vIndex,&pDispInputTd);
										if(!SUCCEEDED(hr)) break;
										hr = pDispInputTd->QueryInterface(IID_IHTMLElement,(void**)&pElement);
										if(!SUCCEEDED(hr)) break;
										CComVariant vVal;	
										vVal ="";
										CString   str; 
										pElement->getAttribute(_T("name"),2,&vVal);
										str = vVal.bstrVal;
										if(str == "accountNo")
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
											 break;
										}

								 }//for(long i=0
								 if(state == BILL_COM_ERROR)
									 break;

								 if(pData->tag != "" && strlen(plRecords->tag) >= 4)
								 {
									 string scr = plRecords->tag;
									if(pData->tag != scr)
									{
										ShowNotifyWnd(false);//隐藏正在登陆框
										WCHAR sInfo[256] = { 0 };
										swprintf(sInfo, 256, L"当前账户已绑定建行卡号末4位%s，实际导入建行卡号与原账户不一致，是否继续导入", scr.c_str());

										HWND hMainFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
										if(MessageBoxW(hMainFrame, sInfo, L"财金汇", MB_OKCANCEL) == IDCANCEL)
										{
											state = BLII_NEED_RESTART;
											break;
										}
									}
								 }

							}
							if (pElement) { pElement->Release(); pElement = NULL; }
							if (pCollection) { pCollection->Release(); pCollection = NULL; }
							if (pDispInputTd) { pDispInputTd->Release(); pDispInputTd = NULL; }

					       	state =BILL_SELECT_MONTH;
					    	step =5;
							//isselmon = true;
						}
						else
						{
							//if(isselmon)
							//{
							//	isselmon =false;
							//	map<string, bool>::iterator ites = plRecords->m_mapBack.begin();
							//	for(; ites != plRecords->m_mapBack.end() ; ites ++)
							//	{
							//		if((*ites).second)
							//		{
							//			std::map<string, bool>::iterator ite = m_mapBack.begin();
							//			for(; ite != m_mapBack.end() ; ite ++)
							//			{
							//				if((*ite).first == (*ites).first)
							//				{
							//					(*ite).second = true;
							//					break;
							//				}
							//			}
							//		}
							//	}
							//}

							if(isRMB)
							{
								cstrselect = "";
								//m_month = 0;
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
												tmpstr = tmpstr.replace(4,2,s1p);
												tmpstr = tmpstr.substr(0,6);
												cstrselect = tmpstr;
												m_isdata = 94;
												//m_month = (*ite).second;
												break;
											}
										}
										ites->dwVal =CHECKBOX_SHOW_UNCHECKED;
									}
									if(m_isdata > 0)
										break;
								}
							}
							cstrp[0] = _T("1");
	        				if(isRMB)
     							cstrp[1] = _T("840");
							else 
     							cstrp[1] = _T("156");
							if(isRMB)
							{
								isRMB = false;
     							cstrp[2] = _T("156");
							}
							else
							{
								isRMB = true;
     							cstrp[2] = _T("840");
							}
							cstrp[3] = cstrselect.c_str();

							//dispparams.rgvarg[3].bstrVal = ::SysAllocString((LPOLESTR)A2COLE(pData->select.c_str()));//FORMMODAL_DIALOG
							//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
							//RecordInfo(L"eciticDll", 1800, L"执行CallFuncEx2"); 

							if( cstrp[3] !=  _T("") )
	             				CallJSF(doc ,4,"setValue");
						     	//CallJScript2(doc, "setValue", dispparams);
							else
							{
	       						cstrp[0] = cstrp[14];
	             				CallJSF(docf ,1,"submitInner");
							}
						}//else
					}
					break;
				case 6:
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
									 pMRecord->month = cstrselect.c_str();
		                 			 pMRecord->m_isSuccess = true;

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

											 BSTR vName = NULL;
											 pElement->get_className(  &vName );
											 CString  l_tp(vName);
						     				 if(l_tp  =="bg1")
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

														switch(k)
														{
														case 0:
															{
																lData = ReplaceCHINESE(lData);
																if( lData!= NULL)
																	strcpy_s(ptrRecord->TransDate, 256, OLE2A(lData));
																break;
															}
														case 1:
															{
																lData = ReplaceCHINESE(lData);
																if( lData!= NULL)
																	strcpy_s(ptrRecord->PostDate, 256,OLE2A(lData));
																break;
															}
														case 2:
															{
																if( lData!= NULL)
																{
																	strcpy_s(ptrRecord->Description, 256, OLE2A(lData));
																	//strcpy_s(ptrRecord->Payee, 256, OLE2A(lData));
																}
																break;
															}
														case 3:
															{
																//if( lData!= NULL)
																//{
																//	string scr = OLE2A(lData);																
																//	//ptrRecord->Amount = ( 0 - atof(FilterStringNumber(scr).c_str()));
																//}
																break;
															}
														case 4:
															{
																if( lData!= NULL)
																	ptrRecord->CardNumber = atoi(OLE2A(lData));
																break;
															}
														case 5:
															{
																//if( lData!= NULL)
																//	strcpy_s(ptrRecord->Country, 256, OLE2A(lData));
																break;
															}
														case 6:
															{
																if( lData!= NULL)
																{
																	string scr = OLE2A(lData);																
																	strcpy_s(ptrRecord->Amount, 256, FilterStringNumber(scr).c_str());
//																		ptrRecord->Amount = ( 0 - atof(FilterStringNumber(scr).c_str()));
																}
																break;
															}
														case 7:
															{
																//if( lData!= NULL)
																//{
																//	string scr = OLE2A(lData);																
																//	ptrRecord->Amount = ( 0 - atof(FilterStringNumber(scr).c_str()));
																//}
																break;
															}

														default:
															break;
														}//switch
													}//for(int k = 
	    											 tempkkk = 10;
									    			 pMRecord->TranList.push_back(ptrRecord);
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
											 }//if(l_tp  =="bg1")
									 }//for(long i=0
									 if(tempkkk == 0)
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

									 //if(tempkkk == 10)
									 {
					     				if(isRMB)
					    					 billsusd.push_back(pMRecord);
										else
					    					 billsrmb.push_back(pMRecord);
									  }
								}
							}
							if(tempkkk == 10)
							{
								if(isRMB)//USD
								{
             							CallJSF(doc ,0,"otherMonthMethod");
								}
								else  //RMB
								{
									isRMB = true;
   									cstrp[0] = _T("14");
         							CallJSF(doc ,1,"otherCurrency");
								}
							}
							else if(tempkkk == 0)
							{
								 step = 3;
								 state =BILL_BROWSER_LOOP;
							}
							pElement-> Release();
							pCollection-> Release(); 
					}
					break;
				case 7:
					{
					}
					break;
				case 10:
					{
							IHTMLElementCollection *pCollection			= NULL;
							IHTMLElement* pElement= NULL;
							IDispatch *pDispInputTd = NULL;   

							hr = docDisp->QueryInterface(IID_IHTMLDocument3, reinterpret_cast<void**>(&doc3));
							if(SUCCEEDED(hr) || doc3 != NULL)
							{
								  //   doc3->getElementsByTagName(_T("input"),&pCollection);
									 long trlength ;
									 //pCollection->get_length(&trlength);
									 //for(long i=0; i<trlength; i++)
				      //  			 {
										//	CComVariant vIndex   =  i; 
										//	_variant_t   name; 
										//	hr = pCollection->item(vIndex,vIndex,&pDispInputTd);
										//	if(!SUCCEEDED(hr)) break;
										//	hr = pDispInputTd->QueryInterface(IID_IHTMLElement,(void**)&pElement);
										//	if(!SUCCEEDED(hr)) break;
										//	CComVariant vVal;	
										//	vVal ="";
										//	CString   str; 
										//	pElement->getAttribute(_T("name"),2,&vVal);
										//	str = vVal.bstrVal;
										//	if(str == "cardNo")
										//	{
										//		 pElement->getAttribute(_T("value"),2,&vVal);
										//		 str = vVal.bstrVal;
										//		 string ntag = CT2A(str);
										//		 strcpy_s(plRecords->tag, 256, ntag.c_str());	
										//		 break;
										//	}

									 //}//for(long i=0

									 doc3->getElementsByTagName(_T("TD"),&pCollection);
									 BSTR vName = NULL;
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
											pElement->get_innerText(  &vName );
											CString  l_tp(vName);
											l_tp.TrimLeft();   l_tp.TrimRight();
											int lows = 0;
											if(i ==9)
											{
												lows = l_tp.Find(_T(" "));
												l_tp.Delete(lows,l_tp.GetLength()-lows);
												string s(CW2A((LPCTSTR)l_tp));
												s = FilterStringNumber(s);
												if(l_tp == "")
													strbalances[0] = "F";
												else
													strbalances[0] = s;
											}
											if(i == 17)
											{
												lows = l_tp.Find(_T(" "));
												l_tp.Delete(lows,l_tp.GetLength()-lows);
												string s(CW2A((LPCTSTR)l_tp));
												s = FilterStringNumber(s);
												if(l_tp == "")
													strbalances[1] = "F";
												else
													strbalances[1] = s;
											}
									 }//for(long i=0

							}
							else
							{
													strbalances[0] = "F";
													strbalances[1] = "F";
							}
							if (pElement) { pElement->Release(); pElement = NULL; }
							if (pCollection) { pCollection->Release(); pCollection = NULL; }
							if (pDispInputTd) { pDispInputTd->Release(); pDispInputTd = NULL; }


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

BSTR  ReplaceCHINESE( BSTR bstro)
{
	USES_CONVERSION;
	BSTR  brstr = NULL;
	CStringW   strtmp   =   (   CString   )bstro;
	strtmp.Replace(_T("年"),_T(""));
	strtmp.Replace(_T("月"),_T(""));
	strtmp.Replace(_T("日"),_T(""));
	bstro = SysAllocString(CT2OLE(strtmp));
	brstr   =   bstro;
	return brstr;
}
