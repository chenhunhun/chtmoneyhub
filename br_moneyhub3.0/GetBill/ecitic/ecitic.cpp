// eciticGetBill.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <map>
#include <list>
#include <vector>
#include "../../BankData/BankData.h"
#include "../../ThirdParty/RunLog/RunLog.h"

bool   isRMB = true;
int    mruloop =0;
int    mruloop5 =3;

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

	//CRunLog::GetInstance ()->GetLog ()->WriteSysLog (LOG_TYPE_INFO, L"%ws", wscTmp.c_str());
	return true;
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

void WINAPI FreeMemory(std::list<BILLRECORD*>** plRecords)
{
	std::list<BILLRECORD*>::iterator ite = (*plRecords)->begin();
	for(;ite != (*plRecords)->end(); ite ++)
	{
		if((*ite) != NULL)
		{
			list<LPTRANRECORD>::iterator lite = (*ite)->TranList.begin();
			for(;lite != (*ite)->TranList.end(); lite ++)
			{
				delete (*lite);
			}
			(*ite)->TranList.clear();
			delete (*ite);
		}
	}

	(*plRecords)->clear();
	delete (*plRecords);
	(*plRecords) = NULL;
}


int WINAPI FetchBillFunc(IWebBrowser2* pWebBrowser, BillData* pData, int step, std::list<BILLRECORD*>** plRecords )
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

		hr = pWebBrowser->get_Document(&docDisp);
		if (SUCCEEDED(hr) && docDisp != NULL)
		{					
			hr = docDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&doc));	
			
			if (SUCCEEDED(hr) || doc != NULL) 
			{

            	hr = doc->get_parentWindow(&m_windowHeader) ;

				switch(step)
				{					
				case 1:
					break;
				case 2:
					break;
				case 3:
					{
						const int arraySize = 1;
						//Putting parameters
						DISPPARAMS dispparams;
						memset(&dispparams, 0, sizeof dispparams);
						dispparams.cArgs      = arraySize;
						dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
						dispparams.cNamedArgs = 0;

								IHTMLElementCollection *pCollection			= NULL;
								IHTMLElement* pElement= NULL;

						CString refString;
						hr = pWebBrowser->get_Document(&docDisp);
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
											IDispatch *pDispInputTd = NULL;   
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
													refString = l_tp;
													break;
     											 }
									 }
							}
						}
	        			int nLen = refString.GetLength();
							LPCSTR lpszBuf = W2A(refString.GetBuffer(nLen));
						//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 
						dispparams.rgvarg[0].bstrVal = ::SysAllocString((LPOLESTR)A2COLE(lpszBuf));
						dispparams.rgvarg[0].vt = VT_BSTR;
						//RecordInfo(L"eciticDll", 1800, L"执行CallFuncEx2");  
						CallJScript2(doc, "submitInner", dispparams);

						// 释放所有申请的空间
						for(int i = 0; i< 1;i ++)
						{
							if(dispparams.rgvarg[i].vt == VT_BSTR)
								::SysFreeString(dispparams.rgvarg[i].bstrVal);
							dispparams.rgvarg[i].bstrVal = NULL;
						}
						delete[] dispparams.rgvarg;


      				}
					break;
				case 4:
					{
						//mruloop4--;
						//if(mruloop4 > 0 && mruloop5 > 0)
						{
								const int arraySize = 0;
								//Putting parameters
								DISPPARAMS dispparams;
								memset(&dispparams, 0, sizeof dispparams);
								dispparams.cArgs      = arraySize;
								dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
								dispparams.cNamedArgs = 0;

								//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
								//RecordInfo(L"eciticDll", 1800, L"执行CallFuncEx2");  
								CallJScript2(doc, "submitFun", dispparams);

								// 释放所有申请的空间
								for(int i = 0; i< 0;i ++)
								{
									if(dispparams.rgvarg[i].vt == VT_BSTR)
										::SysFreeString(dispparams.rgvarg[i].bstrVal);
									dispparams.rgvarg[i].bstrVal = NULL;
								}
								delete[] dispparams.rgvarg;
						}
					}
					break;
				case 5:
					{
						//mruloop5--;
						//if(mruloop4 > 0 && mruloop5 > 0)
						{
								const int arraySize = 4;
								//Putting parameters
								DISPPARAMS dispparams;
								memset(&dispparams, 0, sizeof dispparams);
								dispparams.cArgs      = arraySize;
								dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
								dispparams.cNamedArgs = 0;

								//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
								dispparams.rgvarg[0].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("1"));
								dispparams.rgvarg[0].vt = VT_BSTR;
		        				if(isRMB)
				     				dispparams.rgvarg[1].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("840"));
								else 
				     				dispparams.rgvarg[1].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("156"));
								dispparams.rgvarg[1].vt = VT_BSTR;
								if(isRMB)
								{
									isRMB = false;
									dispparams.rgvarg[2].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("156"));//NULL;
								}
								else
								{
									isRMB = true;
									dispparams.rgvarg[2].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("840"));//NULL;
								}
								dispparams.rgvarg[2].vt = VT_BSTR;
								dispparams.rgvarg[3].bstrVal = ::SysAllocString((LPOLESTR)A2COLE(pData->month.c_str()));//FORMMODAL_DIALOG
								dispparams.rgvarg[3].vt = VT_BSTR;
								//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
								//RecordInfo(L"eciticDll", 1800, L"执行CallFuncEx2");  
								CallJScript2(doc, "setValue", dispparams);

								// 释放所有申请的空间
								for(int i = 0; i< 4;i ++)
								{
									if(dispparams.rgvarg[i].vt == VT_BSTR)
										::SysFreeString(dispparams.rgvarg[i].bstrVal);
									dispparams.rgvarg[i].bstrVal = NULL;
								}
								delete[] dispparams.rgvarg;
						}
					}
					break;
				case 6:
					{
								IHTMLElementCollection *pCollection			= NULL;
								IHTMLElement* pElement= NULL;

								int tempkkk=0;
						(*plRecords) = new std::list<BILLRECORD*>;
						hr = pWebBrowser->get_Document(&docDisp);
						if (SUCCEEDED(hr) && docDisp != NULL)
						{
							hr = docDisp->QueryInterface(IID_IHTMLDocument3, reinterpret_cast<void**>(&doc3));
							if(SUCCEEDED(hr) || doc3 != NULL)
							{
										doc3->getElementsByTagName(_T("tr"),&pCollection);
										BILLRECORD* pRecord = new BILLRECORD;
										if(isRMB)
		     								pRecord->type = USD;
										else
    										pRecord->type = RMB;

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

														strcpy_s(pRecord->month, 256, pData->month.c_str());
														pRecord->accountid = pData->accountid;
														strcpy_s(pRecord->aid, 256,pData->aid.c_str());

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
																		strcpy_s(ptrRecord->Description, 256, OLE2A(lData));
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
																		ptrRecord->Amount = ( 0 - atof(FilterStringNumber(scr).c_str()));
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
														 pRecord->TranList.push_back(ptrRecord);
												 }//if(l_tp  =="bg1")
									 }//for(long i=0

									 if(tempkkk == 10)
				    					(*plRecords)->push_back(pRecord);
							}
						}
							 if(tempkkk == 10)
							 {
								 mruloop++;
										if(isRMB)//USD
										{
													const int arraySize = 0;
														//Putting parameters
														DISPPARAMS dispparams;
														memset(&dispparams, 0, sizeof dispparams);
														dispparams.cArgs      = arraySize;
														dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
														dispparams.cNamedArgs = 0;

														//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
														//RecordInfo(L"eciticDll", 1800, L"执行CallFuncEx2");  
														CallJScript2(doc, "otherMonthMethod", dispparams);

														// 释放所有申请的空间
														for(int i = 0; i< 0;i ++)
														{
															if(dispparams.rgvarg[i].vt == VT_BSTR)
																::SysFreeString(dispparams.rgvarg[i].bstrVal);
															dispparams.rgvarg[i].bstrVal = NULL;
														}
														delete[] dispparams.rgvarg;
										}
										else  //RMB
										{
		             						 state = 98;
											 isRMB = true;
													const int arraySize = 1;
														//Putting parameters
														DISPPARAMS dispparams;
														memset(&dispparams, 0, sizeof dispparams);
														dispparams.cArgs      = arraySize;
														dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
														dispparams.cNamedArgs = 0;

														//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
														//RecordInfo(L"eciticDll", 1800, L"执行CallFuncEx2");  
														dispparams.rgvarg[0].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("14"));
														dispparams.rgvarg[0].vt = VT_BSTR;
														CallJScript2(doc, "otherCurrency", dispparams);

														// 释放所有申请的空间
														for(int i = 0; i< 1;i ++)
														{
															if(dispparams.rgvarg[i].vt == VT_BSTR)
																::SysFreeString(dispparams.rgvarg[i].bstrVal);
															dispparams.rgvarg[i].bstrVal = NULL;
														}
														delete[] dispparams.rgvarg;
										}
							 }
							 else if(tempkkk == 0)
							 {
								 pWebBrowser->GoBack();
								 pWebBrowser->GoBack();
								 mruloop++;
								//state = 301;  //winGoBack
							 }
							 if(mruloop == 2)
								 state = 99;

								pElement-> Release();
								pCollection-> Release(); 
					}
					break;
				case 7:
					{
					}
					break;
				default:
					break;	
				}

				if (docDisp) { docDisp->Release(); docDisp = NULL; }
			}
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
	strtmp.Replace(_T("年"),_T("-"));
	strtmp.Replace(_T("月"),_T("-"));
	strtmp.Replace(_T("日"),_T(""));
	bstro = SysAllocString(CT2OLE(strtmp));
	brstr   =   bstro;
	return brstr;
}
