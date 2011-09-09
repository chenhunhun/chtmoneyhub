// CMBChinaGetBill.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <map>
#include <list>
#include <vector>
#include "../../BankData/BankData.h"
#include "../../ThirdParty/RunLog/RunLog.h"

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


int WINAPI FetchBillFunc(IWebBrowser2* pWebBrowser, BillData* pData, int step, std::list<BILLRECORD*>** plRecords)
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

		hr = pWebBrowser->get_Document(&docDisp);
		if (SUCCEEDED(hr) && docDisp != NULL)
		{					
			hr = docDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void**>(&doc));	
			if (SUCCEEDED(hr) || doc != NULL) 
			{
				if (docDisp) { docDisp->Release(); docDisp = NULL; }

				switch(step)
				{					
				case 1:
					DISPPARAMS dispparams;
					memset(&dispparams, 0, sizeof dispparams);
					dispparams.cArgs      = 1;
					dispparams.rgvarg     = new VARIANT;
					dispparams.cNamedArgs = 0;

					//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数
					dispparams.rgvarg[0].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("C"));
					dispparams.rgvarg[0].vt = VT_BSTR;
					RecordInfo(L"CMBChinaDll", 1800, L"执行changeLoginType");
					CallJScript2(doc, "changeLoginType", dispparams);

					// 释放所有申请的空间

					if(dispparams.rgvarg[0].vt == VT_BSTR)
						::SysFreeString(dispparams.rgvarg[0].bstrVal);
					dispparams.rgvarg[0].bstrVal = NULL;

					RecordInfo(L"CMBChinaDll", 1800, L"释放执行完changeLoginType申请空间");
					delete dispparams.rgvarg;

					break;
				case 2:
					{
						const int arraySize = 7;
						//Putting parameters
						DISPPARAMS dispparams;
						memset(&dispparams, 0, sizeof dispparams);
						dispparams.cArgs      = arraySize;
						dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
						dispparams.cNamedArgs = 0;

						//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数
						dispparams.rgvarg[0].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("SIDType=A"));
						dispparams.rgvarg[0].vt = VT_BSTR;
						dispparams.rgvarg[1].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("MainPanel=none"));
						dispparams.rgvarg[1].vt = VT_BSTR;
						dispparams.rgvarg[2].bstrVal = ::SysAllocString((LPOLESTR)A2COLE(""));//NULL;
						dispparams.rgvarg[2].vt = VT_BSTR;
						dispparams.rgvarg[3].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("FORM"));//FORMMODAL_DIALOG
						dispparams.rgvarg[3].vt = VT_BSTR;
						dispparams.rgvarg[4].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("CreditCard/am_QueryReckoningSurvey.aspx"));
						dispparams.rgvarg[4].vt = VT_BSTR;

						dispparams.rgvarg[5].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("CBANK_CREDITCARD"));
						dispparams.rgvarg[5].vt = VT_BSTR;
						dispparams.rgvarg[6].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("C_A"));
						dispparams.rgvarg[6].vt = VT_BSTR;
						RecordInfo(L"CMBChinaDll", 1800, L"执行CallFuncEx2");
						CallJScript2(doc, "CallFuncEx2", dispparams);

						RecordInfo(L"CMBChinaDll", 1800, L"执行完CallFuncEx2");
						// 释放所有申请的空间
						for(int i = 0; i< 7;i ++)
						{
							if(dispparams.rgvarg[i].vt == VT_BSTR)
								::SysFreeString(dispparams.rgvarg[i].bstrVal);
							dispparams.rgvarg[i].bstrVal = NULL;
						}
						RecordInfo(L"CMBChinaDll", 1800, L"释放执行完CallFuncEx2申请空间");
						delete[] dispparams.rgvarg;
						RecordInfo(L"CMBChinaDll", 1800, L"释放执行完CallFuncEx2");

					}
					break;
				case 3:
				case 4:
				case 5:
					{
						hr=doc->get_body( &elem);  
						if(elem!=NULL)   
						{      
							BSTR pbBody = NULL;//::SysAllocString(OLESTR("Written by IHTMLDocument2:招商银行首页需要进行显示."));
							hr=elem->get_innerHTML(&pbBody);   //类似的还有put_innerTEXT
							elem->Release();   
							elem = NULL;
						} 
						//CallFuncEx2('C_A','CBANK_CREDITCARD','CreditCard/am_QueryReckoningSurvey.aspx','FORM',null,'MainPanel=none','SIDType=A');
						std::vector<std::string> paramVec;
						paramVec.push_back("SIDType=A");
						paramVec.push_back("MainPanel=none");
						string  paramMonth = "IN_YYYYMM=" + pData->month;
						paramVec.push_back(paramMonth);
						paramVec.push_back(pData->month);
						paramVec.push_back("FORM");
						//https://pbsz.ebank.cmbchina.com/CmbBank_CreditCard/UI/CreditCardPC/CreditCard/am_QueryReckoningSurvey.aspx
						paramVec.push_back("../CreditCard/am_QueryReckoningList.aspx");
						RecordInfo(L"CMBChinaDll", 1800, L"执行triggerFunc");
						
						CallJScript(doc, "triggerFunc", paramVec);
					}
					break;
				case 99:
				case 100:
				case 101:
					{
						
						bool isAutoBuy = false;
						RecordInfo(L"CMBChinaDll", 1800, L"进入最后导入步骤");
						(*plRecords) = new std::list<BILLRECORD*>;
						hr = pWebBrowser->get_Document(&docDisp);
						if (SUCCEEDED(hr) && docDisp != NULL)
						{
							hr = docDisp->QueryInterface(IID_IHTMLDocument3, reinterpret_cast<void**>(&doc3));
							if(SUCCEEDED(hr) || doc3 != NULL)
							{
								CComPtr<IHTMLElement> rmbelemtest;
								BSTR testid = ::SysAllocString((LPOLESTR)A2COLE("trBonusPoint"));
								hr = doc3->getElementById(testid, &rmbelemtest);
								::SysFreeString(testid);
								if(FAILED(hr) || rmbelemtest == NULL)
								{
									break;
								}
								
								// 先获取人民币账号
								char dglist[2][10] = { "dgRMBList", "dgUSDList"};
								for(int sjtype = 0;sjtype < 2; sjtype ++)
								{
									CComPtr<IHTMLElement> rmbelem;
									BSTR id = ::SysAllocString((LPOLESTR)A2COLE(dglist[sjtype]));
									hr = doc3->getElementById(id, &rmbelem); 
									if(SUCCEEDED(hr) && rmbelem != NULL)
									{	
										BILLRECORD* pRecord = new BILLRECORD;
										pRecord->type = (sjtype==0) ? RMB:USD;
										CComPtr<IHTMLElementCollection> elementCollection;
										CComPtr<IDispatch> pDispatch;
										long length;

										hr = rmbelem->get_children(&pDispatch);
										if(!SUCCEEDED(hr)) break;
										hr = pDispatch->QueryInterface(IID_IHTMLElementCollection, (VOID**)&elementCollection);
										if(!SUCCEEDED(hr)) break;
										hr = elementCollection->get_length(&length);
										if(!SUCCEEDED(hr)) break;

										for (INT i = 0; i < length; i++)
										{
											IDispatch *pDispInput = NULL;   
											CComVariant vIndex=i;   
											hr = elementCollection->item(vIndex,vIndex,&pDispInput);
											if(!SUCCEEDED(hr)) break;

											CComPtr<IHTMLElement> pElemInput;   
											hr = pDispInput->QueryInterface(IID_IHTMLElement,(void**)&pElemInput);
											if(!SUCCEEDED(hr)) break;

											CComPtr<IDispatch> pDispatchTr;
											hr = pElemInput->get_children(&pDispatchTr);
											if(!SUCCEEDED(hr)) break;

											CComPtr<IHTMLElementCollection> trElementCollection;

											hr = pDispatchTr->QueryInterface(IID_IHTMLElementCollection, (VOID**)&trElementCollection);
											if(!SUCCEEDED(hr)) break;

											long trlength;
											hr = trElementCollection->get_length(&trlength);//到了10个总数据的部分
											if(!SUCCEEDED(hr)) break;

											LPTRANRECORD preRecord = NULL;
											for(int j = 1; j < trlength; j ++)
											{
												IDispatch *pDispInputTd = NULL;   
												CComVariant vIndexj = j;   
												hr = trElementCollection->item(vIndexj,vIndexj,&pDispInputTd);
												if(!SUCCEEDED(hr)) break;

												CComPtr<IHTMLElement> pElemIn;   
												hr = pDispInputTd->QueryInterface(IID_IHTMLElement,(void**)&pElemIn);
												if(!SUCCEEDED(hr)) break;

												CComPtr<IDispatch> pDispatchLast;
												hr = pElemIn->get_children(&pDispatchLast);
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
												
												LPTRANRECORD ptrRecord = preRecord;
												if(false == isAutoBuy)
													ptrRecord = new TRANRECORD;
											
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
															if( lData!= NULL)
															{
																string preMonth(pData->month);
																preMonth = preMonth.substr(0,4) + "-";
																sprintf_s(ptrRecord->TransDate, 256, "%s%s", preMonth.c_str(),OLE2A(lData));
															}
															break;
														}
													case 1:
														{
															if( lData!= NULL)
															{
																string preMonth(pData->month);
																preMonth = preMonth.substr(0,4) + "-";
																sprintf_s(ptrRecord->PostDate, 256, "%s%s", preMonth.c_str(),OLE2A(lData));
																if(strlen(ptrRecord->TransDate) == 0)//招行里面没有交易日期的话，用记账日期补齐
																	strcpy_s(ptrRecord->TransDate, 256, ptrRecord->PostDate);
															}
															break;
														}
													case 2:
														{

															if( lData!= NULL)
															{
																if(isAutoBuy == true)
																{
																	sprintf_s(ptrRecord->Description, 256, "%s(%s", ptrRecord->Description, OLE2A(lData));
																}
																else
																	strcpy_s(ptrRecord->Description, 256, OLE2A(lData));
															}


															break;
														}
													case 3:
														{
															if( lData!= NULL)
															{
																string scr = OLE2A(lData);																
																ptrRecord->Amount = ( 0 - atof(FilterStringNumber(scr).c_str()));
															}
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
															if( lData!= NULL)
																strcpy_s(ptrRecord->Country, 256, OLE2A(lData));
															break;
														}
													case 7:
														{
															if( lData!= NULL)
																if(isAutoBuy == true)
																{
																	if(ptrRecord != NULL)
																		sprintf_s(ptrRecord->Description, 256, "%s,人民币 %s)", ptrRecord->Description, OLE2A(lData));

																}
														}
														break;

													default:
														break;
													}
												}
												if((strlen(ptrRecord->Description)) > 0 && (strcmp(ptrRecord->Description, "自动购汇") == 0))
													isAutoBuy = true;
												else
													isAutoBuy = false;
												if(isAutoBuy == false)
												{												
													preRecord = ptrRecord;
													pRecord->TranList.push_back(ptrRecord);
												}

											}
										}
										(*plRecords)->push_back(pRecord);

									}

									if(id)
										::SysFreeString(id);
								}
								
								state = 99;

							}

						}


						break;
					}
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
