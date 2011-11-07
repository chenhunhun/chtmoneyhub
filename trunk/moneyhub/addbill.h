	<site id="a004" name="中国建设银行" dll="ccb.dll">
		<type type="2">
		<step seq="1">http://creditcard.ccb.com/</step>
		<step seq="4">http://creditcard.ccb.com/tran/WCCMainPlatV5?CCB_IBSVersion=V5&SERVLET_NAME=WCCMainPlatV5&TXCODE=E13010&CODE=REFURT0yMDExMDgxNSZUSU1FPTE3MzIxNyZTRVJJQUxOTz01NzIyNzM1JkFDQ19OTz01MzI0NTg1MDEwOTgzOTEyJkFDQ19UWVBFPTA0JkJSQU5DSF9DT0RFPSZVU0VSX0lEPTExMDEwNDE5NzIwMTI0MjUxNSZCQlJBTkNIX0NPREU9Jk5BTUU91cXQ8SZUX1RYQ09ERT1udWxsJlNVQ0NFU1M9WQ^^&SIGN=05bdf1f883ac8ea6f0d81db201fa7f19b7b70a579d4a29e9a0585e1eec752bc941277e37991a1c64fc7bbd37bcad7ebd51c4bfca86d94f69c27c8ff1c6d23d33e7f32a38d164afcf51bd59807ac7c035e0da63e2ce8cd4d07caaf7decc170b331babc9502e7bcaaaad4274969794bd3ca9e55a37fc5c797b34e610828595ec1a</step>
		<step seq="4">http://creditcard.ccb.com/tran/WCCMainPlatV5?CCB_IBSVersion=V5&SERVLET_NAME=WCCMainPlatV5&TXCODE=E13010&CODE=0130Z1108915&SIGN=ERRCODE</step>
		<step seq="5">http://creditcard.ccb.com/tran/WCCMainPlatV5?CCB_IBSVersion=V5&SERVLET_NAME=WCCMainPlatV5</step>
		<step seq="6">http://creditcard.ccb.com/tran/WCCMainB1L1?CCB_IBSVersion=V5&SERVLET_NAME=WCCMainB1L1</step>
		</type>
	</site>	


//document.frames[ 'myframe '].document.script.myElement.myFunction()
//可以动态在网页中插入脚本代码的


//	HRESULT hr = CoInitialize(NULL);
//
//
//	IScriptControlPtr pScriptControl;//(__uuidof(ScriptControl));
//
//	//m_spHtmlDoc->get_parentWindow(&m_pHtmlWindow);
//    pScriptControl->AddObject("window", m_windowHeader, VARIANT_TRUE);
//
//// Create a VARIANT array of VARIANTs which hold BSTRs
//	LPSAFEARRAY psa;
//	SAFEARRAYBOUND rgsabound[]  = { 3, 0 }; // 3 elements, 0-based
//	int i;
//
//	psa = SafeArrayCreate(VT_VARIANT, 1, rgsabound);
//	if (!psa)
//	{
//		return E_OUTOFMEMORY;
//	}
//
//	VARIANT vFlavors[3];
//	for (i = 0; i < 3; i++)
//	{
//		VariantInit(&vFlavors[i]);
//		V_VT(&vFlavors[i]) = VT_BSTR;
//	}
//
//	//V_BSTR(&vFlavors[0]) = SysAllocString(OLESTR("Vanilla"));
//	//V_BSTR(&vFlavors[1]) = SysAllocString(OLESTR("Chocolate"));
//	//V_BSTR(&vFlavors[2]) = SysAllocString(OLESTR("Espresso Chip"));
//
//	long lZero = 0;
//	long lOne = 1;
//	long lTwo = 2;
//
//	// Put Elements to the SafeArray:
//	//hr = SafeArrayPutElement(psa, &lZero,&vFlavors[0]);
//	//hr = SafeArrayPutElement(psa, &lOne,&vFlavors[1]);
//	//hr = SafeArrayPutElement(psa, &lTwo,&vFlavors[2]);
//
//	// Free Elements from the SafeArray:
//	for(i=0;i<3;i++)
//	{
//		SysFreeString(vFlavors[i].bstrVal);
//	}
//
//	// Set up Script control properties
//	//pScriptControl->Language = "JScript";
//	//pScriptControl->AllowUI = TRUE;
//	//pScriptControl->AddCode("function MyStringFunction(Argu1,Argu2,Argu3)\{  return \"hi there\" ;}" );
//
//	//  Call MyStringFunction with the two args:
//	_variant_t outpar = pScriptControl->Run("MyStringFunction", &psa);
//	
//
//	// Convert VARIANT to C string:
//	_bstr_t bstrReturn = (_bstr_t)outpar;
//	char *pResult = (char *)bstrReturn;
//
//
//	// Print the result out:
//	printf("func=%s\n",pResult);
//	
//	//  Clean up:
//	SafeArrayDestroy(psa);
//
//	CoUninitialize();


//						m_windowHeader->execScript("" ,new string("javascript"));
//						VARIANT   v;  return temp
//VariantInit(&v);  
//v.vt=VT_EMPTY; 
////						m_windowHeader->execScript( _T("submitInner"),_T("javascript"),&v);
//						m_windowHeader->execScript( _T("alert('hello')"),_T("JavaScript"),&v);
//VARIANT ret;
//ret.vt=VT_EMPTY;
//CComBSTR func="alert (temp_menu2_url30)";
////CComBSTR func="alert(document.cookie)";
//CComBSTR lang="JAVAScript";
////hr=m_windowHeader->execScript(func,lang,&ret);



//CString   strPos   =   _T("BeforeEnd ");
//CString   m_strScript("function get130(){  return temp_menu2_url30; } ");
//CString   strScript   =   _T("<br><script defer  ")   +   m_strScript   +   _T("</script>");
////CString   strScript   =   _T("&nbsp <script   Language=javascript> \n <!--   \n ")   +   m_strScript   +   _T("\n//     --> \n </script> ");
//CComBSTR bstrPos   =   strPos.AllocSysString();
//CComBSTR bstrContent   =   m_strScript.AllocSysString();
//elem->insertAdjacentHTML(   bstrPos,   bstrContent   );
//::SysFreeString(bstrPos);
//::SysFreeString(bstrContent);


//pElemLast->get_title(  &lData ); 


//CComPtr<IDispatch> pDisp;
//hr = m_pWB2->get_Document(&pDisp);
//if (SUCCEEDED(hr))   
//{
//CComQIPtr<IHTMLDocument2> pDoc2=pDisp;
//  CComPtr<IHTMLElementCollection> pAll;
//  hr = doc->get_all(&pAll);
//  if (SUCCEEDED(hr))
//  {
//      LPCOLESTR pszElementID;
//      pszElementID=CComBSTR(lpID);
//      //CComVariant varID = pszElementID;
//      CComVariant varID = _T("input");
//      CComPtr<IDispatch> pDispItem;
//      CComPtr<IHTMLElement> pElem;
//      hr = pAll->tags(varID, &pDispItem);
//      if (SUCCEEDED(hr))
//      {
//          CComQIPtr<IHTMLElementCollection> pInputAll=pDispItem;
//          CComVariant varSID = _T("SELECT_ID");
//          CComPtr<IDispatch> pSELECTItem;
//          hr = pInputAll->item(varSID, CComVariant(0), &pSELECTItem);
//          if (SUCCEEDED(hr) && NULL!=pSELECTItem)
//          {
//              CComPtr<IHTMLInputElement> pSELECTElem;
//              hr = pSELECTItem->QueryInterface(&pSELECTElem);
//              if (SUCCEEDED(hr))
//              {
//                  BSTR b;
//pSELECTElem->get_value(&b);
//              }
//          }
//      }
//  }

//}
//hr=doc->get_body( &elem); 
//if(elem!=NULL)   
//{      
//	BSTR pbBody = NULL;//::SysAllocString(OLESTR("Written by IHTMLDocument2:招商银行首页需要进行显示."));
//	hr=elem->get_innerHTML(&pbBody);   //类似的还有put_innerTEXT
//	elem->Release();   
//}   
//CComVariant vVal,vType;	// 域名称，域值，域类型  getLocationCurrentDisplay
//												vType = vVal ="111";
//												CString str;

//vVal = CallJScript2(doc, "getLocationParentDisplay", dispparams);

//dispparams.rgvarg[1].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("MainPanel=none"));
//dispparams.rgvarg[1].vt = VT_BSTR;
//dispparams.rgvarg[2].bstrVal = L"";//NULL;
//dispparams.rgvarg[2].vt = VT_BSTR;
//dispparams.rgvarg[3].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("FORM"));//FORMMODAL_DIALOG
//dispparams.rgvarg[3].vt = VT_BSTR;
//dispparams.rgvarg[4].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("CreditCard/am_QueryReckoningSurvey.aspx"));
//dispparams.rgvarg[4].vt = VT_BSTR;

//dispparams.rgvarg[5].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("CBANK_CREDITCARD"));
//dispparams.rgvarg[5].vt = VT_BSTR;
//dispparams.rgvarg[6].bstrVal = ::SysAllocString((LPOLESTR)A2COLE("C_A"));
//dispparams.rgvarg[6].vt = VT_BSTR;



//				CString refString;
//  BOOL bRetVal = FALSE;
//  //CComPtr<IDispatch> spDisp = GetHtmlDocument();

//   HGLOBAL hMemory;LPCTSTR pstr;
//      hMemory = GlobalAlloc(GMEM_MOVEABLE, 0);
//      if (hMemory != NULL)
//      {
//          CComQIPtr<IPersistStreamInit> spPersistStream = docDisp;
//          if (spPersistStream != NULL)
//          {
//              CComPtr<IStream> spStream;
//              if (SUCCEEDED(CreateStreamOnHGlobal(hMemory, TRUE, &spStream)))
//              {
//                  spPersistStream->Save(spStream, FALSE);

//                   pstr = (LPCTSTR) GlobalLock(hMemory);
//                  if (pstr != NULL)
//                  {
//                      // Stream is always ANSI, but CString
//                      // assignment operator will convert implicitly.

//                      bRetVal = TRUE;
//                      try
//                      {                        
//                  		refString = UTF8ToUnicode((char *)pstr);
//                         //refString = pstr;
//                      }
//                      catch(...)
//                      {
//                          bRetVal = FALSE;
//                      }

//                      if(bRetVal == FALSE)
//                          GlobalFree(hMemory);
//                      else
//                          GlobalUnlock(hMemory);
//                  }
//              }
//          }
//      }


//          		CString strDataToSend = _T("rrrrc中间关键china");
//  					  LRESULT copyDataResult;
//					HWND hWnd = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
////COPYDATASTRUCT cds = { WM_SET_BILLMONTH, pNewPage->dwSize, pNewPage };
//	//			  if (hWnd)
//				  {
//COPYDATASTRUCT cs;//char cmd[] = {"Shutdown"};//int cmdLen = ::strlen(cmd) + 1;//HGLOBAL h = ::GlobalAlloc(GMEM_FIXED, cmdLen);//::memset(h, 0, cmdLen);//::memcpy((char*)h,cmd, cmdLen);//cs.dwData = WM_SET_BILLMONTH;//cs.cbData = cmdLen + 1;//cs.lpData = h;//                    char buffer[1024];//                    strcpy(buffer,CT2A(strDataToSend));//                    COPYDATASTRUCT cpd;
//					cpd.dwData = WM_SET_BILLMONTH;
//					cpd.cbData = strlen(buffer);
//					cpd.lpData = buffer;
//                	 ::SendMessage(m_pAxControl->m_hChildFrame, WM_COPYDATA ,0 ,(LPARAM)&cpd  );
//					//copyDataResult = ::PostMessage(m_pAxControl->m_hChildFrame ,WM_COPYDATA,0,
//					//											(LPARAM)&cpd);
//					strDataToSend.ReleaseBuffer();
//::GlobalFree(h);
//					// copyDataResult has value returned by other app
//				  } 


//pElement->get_className(  &vName );
//pElement->get_id(  &vName );
//pElement->get_innerHTML(  &vName );
//pElement->get_innerText(  &vName );
//pElement->get_language(  &vName );
//pElement->get_onclick(  &vType );
//pElement->get_outerText(  &vName );
//pElement->get_recordNumber(  &vType );
//pElement->get_sourceIndex(  &ss );
//pElement->get_tagName(  &vName );
//pElement->get_title(  &vName ); 
//pElement->get_outerHTML(  &vName ); 
//pElement->get_outerText(  &vName ); 



//IHTMLScriptElementPtr  pElement(pDispInputTd);
//											hr = pDispInputTd->QueryInterface(IHTMLScriptElement,(void**)&script);
//if(!SUCCEEDED(hr)) break;


//pElement->get_src(  &vName );
//pElement->get_htmlFor(  &vName );
//pElement->get_event(  &vName );
//pElement->get_text(  &vName );
////pElement->get_defer(  &vName );
//pElement->get_readyState(  &vName );
////pElement->get_onerror(  &vName );
//pElement->get_type(  &vName );
//pElement->get_readyState(  &vName );



//BSTR   bstrsrc;
//pElement-> get_text(&bstrsrc);
//SysFreeString(bstrsrc); 

	// pElement->getAttribute(_T("var addQuickMenuWorkHtml"),2,&vVal);
	// str = vVal.bstrVal;
	// pElement->getAttribute(_T("target"),2,&vVal);
	// str = vVal.bstrVal;
	// pElement->getAttribute(_T("type"),2,&vVal);
	// str = vVal.bstrVal;
	// pElement->getAttribute(_T("src"),2,&vVal);
	// str = vVal.bstrVal;


	//CComPtr<IHTMLElement> pElemInputText;  
	//hr=pDispInputTd->QueryInterface(IID_IHTMLElement,(void**)&pElemInputText);
	//hr=pDispInputText->QueryInterface(IID_IHTMLElement,(void**)&pElemInputText);
	//	BSTR lData = NULL;
	//	hr = pElemInputText->get_innerText(&lData);

//CComPtr<IDispatch> pDispatch;
//long length;

//hr = pElement->get_children(&pDispatch);
//if(!SUCCEEDED(hr)) break;
//		CComPtr<IHTMLElementCollection> lElementCollection;
//hr = pDispatch->QueryInterface(IID_IHTMLElementCollection, (VOID**)&elementCollection);
//if(!SUCCEEDED(hr)) break;
//hr = elementCollection->get_length(&length);
//if(!SUCCEEDED(hr)) break;
// for(long i=0; i<length; i++)
//  				 {
// }
//BSTR   bstrhtml;
//pElement->get_outerHTML(&bstrhtml);
//CString   str(bstrhtml); 


//CComDispatchDriver spInputElement;	// IDispatch 的智能指针
//pCollection->item( vIndex,vIndex, &spInputElement );
//	spInputElement.GetPropertyByName( L"class", &vName );
//	spInputElement.GetPropertyByName( L"onmouseover",&vVal  );

//												//BSTR vVal = NULL;
//CComVariant vVal,vType;	// 域名称，域值，域类型
//												vType = vVal ="111";
//												long ss;

//												 str = vVal.bstrVal;
//CComDispatchDriver spInputElement;	// IDispatch 的智能指针
//pCollection->item( vIndex,vIndex, &spInputElement );

//CComPtr<IDispatch> pDispatch;
//long length;

//hr = pElement->get_children(&pDispatch);
//if(!SUCCEEDED(hr)) break;
//		CComPtr<IHTMLElementCollection> lElementCollection;
//hr = pDispatch->QueryInterface(IID_IHTMLElementCollection, (VOID**)&elementCollection);
//if(!SUCCEEDED(hr)) break;
//hr = elementCollection->get_length(&length);
//if(!SUCCEEDED(hr)) break;
// for(long i=0; i<length; i++)
//  				 {
// }
//	BSTR   bstrhtml;
//pElement-> get_outerHTML(&bstrhtml);
//CString   str(bstrhtml); 
//CComPtr<IHTMLElement> pElemInputText;  
//hr=pDispInputText->QueryInterface(IID_IHTMLElement,(void**)&pElemInputText);
//	BSTR lData = NULL;
//	hr = pElement->get_innerText(&lData);
//pElement->getAttribute(_T("class"),2,&lVal);
//char *tp=(LPSTR)(LPCTSTR)l_tp;

//   CComPtr<IHTMLElementCollection> pCollection;
//CComPtr<IHTMLElement> pElement;

//BSTR vName = NULL;
//											doc->get_cookie(  &vName );
//											doc->get_URL(  &vName );
//											//doc->get_body(  &vName );




				case 303:
					{
//cur=01&selectDate=1&dates=2011%2F07%2F08---2011%2F08%2F07&dates=2011%2F06%2F08---2011%2F07%2F07&dates=2011%2F05%2F08---2011%2F06%2F07&dates=2011%2F04%2F08---2011%2F05%2F07&dates=2011%2F03%2F08---2011%2F04%2F07&dates=2011%2F02%2F08---2011%2F03%2F07&TXCODE=E13013&ACCT_NO=5324585010983912&CUR_TYPE=01&CUR_TYPE1=01%7C14&KEY=&TYPE=0&CUR_LIMIT=01%3A100%2C000.00%7C14%3A12%2C500.00%7C&QUERY_DATE=1&QUERY_DATE1=7&BRANCHID=310000000
//cur=01&selectDate=2&dates=2011%2F07%2F08---2011%2F08%2F07&dates=2011%2F06%2F08---2011%2F07%2F07&dates=2011%2F05%2F08---2011%2F06%2F07&dates=2011%2F04%2F08---2011%2F05%2F07&dates=2011%2F03%2F08---2011%2F04%2F07&dates=2011%2F02%2F08---2011%2F03%2F07&TXCODE=E13013&ACCT_NO=5324585010983912&CUR_TYPE=01&CUR_TYPE1=01%7C14&KEY=&TYPE=0&CUR_LIMIT=01%3A100%2C000.00%7C14%3A12%2C500.00%7C&QUERY_DATE=2&QUERY_DATE1=7&BRANCHID=310000000
//cur=01&selectDate=3&dates=2011%2F07%2F08---2011%2F08%2F07&dates=2011%2F06%2F08---2011%2F07%2F07&dates=2011%2F05%2F08---2011%2F06%2F07&dates=2011%2F04%2F08---2011%2F05%2F07&dates=2011%2F03%2F08---2011%2F04%2F07&dates=2011%2F02%2F08---2011%2F03%2F07&TXCODE=E13013&ACCT_NO=5324585010983912&CUR_TYPE=01&CUR_TYPE1=01%7C14&KEY=&TYPE=0&CUR_LIMIT=01%3A100%2C000.00%7C14%3A12%2C500.00%7C&QUERY_DATE=3&QUERY_DATE1=7&BRANCHID=310000000
//cur=01&selectDate=4&dates=2011%2F07%2F08---2011%2F08%2F07&dates=2011%2F06%2F08---2011%2F07%2F07&dates=2011%2F05%2F08---2011%2F06%2F07&dates=2011%2F04%2F08---2011%2F05%2F07&dates=2011%2F03%2F08---2011%2F04%2F07&dates=2011%2F02%2F08---2011%2F03%2F07&TXCODE=E13013&ACCT_NO=5324585010983912&CUR_TYPE=01&CUR_TYPE1=01%7C14&KEY=&TYPE=0&CUR_LIMIT=01%3A100%2C000.00%7C14%3A12%2C500.00%7C&QUERY_DATE=4&QUERY_DATE1=7&BRANCHID=310000000
//cur=01&selectDate=5&dates=2011%2F07%2F08---2011%2F08%2F07&dates=2011%2F06%2F08---2011%2F07%2F07&dates=2011%2F05%2F08---2011%2F06%2F07&dates=2011%2F04%2F08---2011%2F05%2F07&dates=2011%2F03%2F08---2011%2F04%2F07&dates=2011%2F02%2F08---2011%2F03%2F07&TXCODE=E13013&ACCT_NO=5324585010983912&CUR_TYPE=01&CUR_TYPE1=01%7C14&KEY=&TYPE=0&CUR_LIMIT=01%3A100%2C000.00%7C14%3A12%2C500.00%7C&QUERY_DATE=5&QUERY_DATE1=7&BRANCHID=310000000
//cur=01&selectDate=6&dates=2011%2F07%2F08---2011%2F08%2F07&dates=2011%2F06%2F08---2011%2F07%2F07&dates=2011%2F05%2F08---2011%2F06%2F07&dates=2011%2F04%2F08---2011%2F05%2F07&dates=2011%2F03%2F08---2011%2F04%2F07&dates=2011%2F02%2F08---2011%2F03%2F07&TXCODE=E13013&ACCT_NO=5324585010983912&CUR_TYPE=01&CUR_TYPE1=01%7C14&KEY=&TYPE=0&CUR_LIMIT=01%3A100%2C000.00%7C14%3A12%2C500.00%7C&QUERY_DATE=6&QUERY_DATE1=7&BRANCHID=310000000
                    //dates=2011%2F07%2F08---2011%2F08%2F07&dates=2011%2F06%2F08---2011%2F07%2F07&dates=2011%2F05%2F08---2011%2F06%2F07&dates=2011%2F04%2F08---2011%2F05%2F07&dates=2011%2F03%2F08---2011%2F04%2F07&dates=2011%2F02%2F08---2011%2F03%2F07&TXCODE=E13013&ACCT_NO=5324585010983912&CUR_TYPE=  &CUR_TYPE1=01%7C14&KEY=&TYPE=0&CUR_LIMIT=01%3A100%2C000.00%7C14%3A12%2C500.00%7C&QUERY_DATE= &QUERY_DATE1=7&BRANCHID=310000000
//document.getElementById("accountFrm").contentDocument.getElementById("result").contentDocument.getElementById("cur").value = "01";
//document.getElementById("accountFrm").contentDocument.getElementById("result").contentDocument.getElementById("time_span").value = "1";
//document.getElementById("accountFrm").contentDocument.getElementById("result").contentDocument.jhForm.submit();


						const int arraySize = 0;
						//Putting parameters
						DISPPARAMS dispparams;
						memset(&dispparams, 0, sizeof dispparams);
						dispparams.cArgs      = arraySize;
						dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
						dispparams.cNamedArgs = 0;

						//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
						//RecordInfo(L"ccbDll", 1800, L"执行CallFuncEx2");  
							//BSTR   bs1(L"document.getElementById(\"accountFrm\").contentDocument.getElementById(\"result\").contentDocument.getElementById(\"cur\").value = \"14\"");//;
							//	//(L"document.topSearchForm.beginTime.value=\"20060203\";document.topSearchForm.endTime.value=\"20101231\";document.topSearchForm.submit();");
							//BSTR   bs4(L"document.getElementById(\"accountFrm\").contentDocument.getElementById(\"result\").contentDocument.getElementById(\"time_span\").value = \"2\"");
							BSTR   bs1(L"document.getElementById(\"cur\").value = \"14\"");//01
							BSTR   bs4(L"document.getElementById(\"time_span\").value = \"2\"");
							BSTR   bs2(L"javascript"); 
							VARIANT   v = {0};   
							VariantInit(&v); 
							HRESULT hr1;
							hr1 = m_windowHeader->execScript(bs1, bs2,&v);
							hr1 = m_windowHeader->execScript(bs4, bs2,&v);
						//CallJScript2(doc, "load", dispparams);
						//CallJScript2(doc, "load_curtype", dispparams);
						CallJScript2(doc, "sub", dispparams);

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


							//BSTR   bs1(L"document.getElementById(\"accountFrm\").contentDocument.getElementById(\"result\").contentDocument.getElementById(\"cur\").value = \"14\"");//;
							//	//(L"document.topSearchForm.beginTime.value=\"20060203\";document.topSearchForm.endTime.value=\"20101231\";document.topSearchForm.submit();");
							//BSTR   bs4(L"document.getElementById(\"accountFrm\").contentDocument.getElementById(\"result\").contentDocument.getElementById(\"time_span\").value = \"2\"");
						//	BSTR   bs1(L"document.getElementById(\"cur\").value = \"14\"");//;
						//	BSTR   bs4(L"document.getElementById(\"time_span\").value = \"2\"");
						//	BSTR   bs2(L"javascript"); 
						//	VARIANT   v = {0};   
						//	VariantInit(&v); 
						//	HRESULT hr1;
						//	hr1 = m_windowHeader->execScript(bs1, bs2,&v);
						//	hr1 = m_windowHeader->execScript(bs4, bs2,&v);
						////CallJScript2(doc, "load", dispparams);
						////CallJScript2(doc, "load_curtype", dispparams);
						//CallJScript2(doc, "sub", dispparams);

				case 4:
					{
						//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
						//RecordInfo(L"ccbDll", 1800, L"执行CallFuncEx2");  $(".close").click()
							BSTR   bs1(L"closeBox(\"#mLogin\")");  //;parent.closeBox('#mLogin');
							BSTR   bs2(L"javascript"); 
							VARIANT   v = {0};   
							VariantInit(&v); 
							HRESULT hr1;
							hr1 = m_windowHeader->execScript(bs1, bs2,&v);

						const int arraySize = 0;
						//Putting parameters
						DISPPARAMS dispparams;
						memset(&dispparams, 0, sizeof dispparams);
						dispparams.cArgs      = arraySize;
						dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
						dispparams.cNamedArgs = 0;

						//A2COLE是在栈中分配的空间，如果循环调用，有栈溢出，所以要限制适用调用的次数 temp_menu2_url30
						//RecordInfo(L"ccbDll", 1800, L"执行CallFuncEx2");  $(".close").click()
						CallJScript2(doc, "initAccountSearch", dispparams);

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


//void   OnInsertScript()  
//{
//CString   strPos   =   "BeforeEnd ";
//CString   m_strScript   =   "function get130(){  return temp_menu2_url30; } ";
//CString   strScript   =   "&nbsp <script   Language=JScript> \n <!--   \n "   +   m_strScript   +   "\n//     --> \n </script> "
//BSTR   bstrPos   =   strPos.AllocSysString();
//BSTR   bstrContent   =   strScript.AllocSysString();
//pElement-> insertAdjacentHTML(   bstrPos,   bstrContent   );
//::SysFreeString(bstrPos);
//::SysFreeString(bstrContent);
//pElement-> Release();
//
//
//IHTMLElement   *pElement=   NULL;
//if   (SUCCEEDED(HrGetElementFromSelection(&pElement)))
//{
//CString   strScript   =   (!pDlg-> m_bVB)   ?  
//( "&nbsp <script   Language=JScript> \n <!--   \n "   +   pDlg-> m_strScript   +   "\n//     --> \n </script> ")
//:   ( "&nbsp <script   Language=VBScript> \n <!--   \n "   +   pDlg-> m_strScript   +   "\n '     --> \n </script> ");
//BSTR   bstrPos   =   strPos.AllocSysString();
//BSTR   bstrContent   =   strScript.AllocSysString();
//pElement-> insertAdjacentHTML(   bstrPos,   bstrContent   );
//::SysFreeString(bstrPos);
//::SysFreeString(bstrContent);
//pElement-> Release();
//}
//
//} 


   string & Utf8ToAnsi(string & szUtf8)
    {
        TCHAR * pSrc = (TCHAR*)szUtf8.c_str();

        int i = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pSrc, -1, NULL, 0); 

        WCHAR * pszW = new WCHAR[i+1]; pszW[i] = 0;

        MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pSrc, -1, pszW, i);
        i = WideCharToMultiByte(CP_ACP, 0, pszW, -1, NULL, 0, NULL, NULL);  

        TCHAR * pszT = new TCHAR[i+1]; pszT[i] = 0;
        WideCharToMultiByte(CP_ACP, 0, pszW, -1, (LPSTR)pszT, i, NULL, NULL);
        
        szUtf8 = (LPCSTR)pszT;

        delete [] pszT;
        delete [] pszW;

        return szUtf8;
    }

   CString UTF8ToUnicode(char* UTF8)
{
	DWORD dwUnicodeLen;        //转换后Unicode的长度
	TCHAR *pwText;            //保存Unicode的指针
	CString strUnicode;        //返回值

	//获得转换后的长度，并分配内存
	dwUnicodeLen = MultiByteToWideChar(CP_UTF8,0,UTF8,-1,NULL,0);
	pwText = new TCHAR[dwUnicodeLen];
	if (!pwText)
	{
		return strUnicode;
	}

	//转为Unicode
	MultiByteToWideChar(CP_UTF8,0,UTF8,-1,pwText,dwUnicodeLen);

	//转为CString
	strUnicode.Format(_T("%s"),pwText);

	//清除内存
	delete []pwText;

	//返回转换好的Unicode字串
	return strUnicode;
}

#include <comdef.h>
//#import "D:\windows\system32\msscript.ocx"  // msscript.ocx 
//using namespace MSScriptControl;










	//vaResult.vt    = VT_EMPTY; 
	//vaResult ="111";
	// initialize to invalid arg
	// Call JavaScript function

	//CString str;

	//											 str = vaResult.bstrVal;
//COleVariant vtResult;
//static BYTE parms[] = VTS_BSTR;
//
//COleDispatchDriver dispDriver(spScript, FALSE);
//
//dispDriver.InvokeHelper(dispid, DISPATCH_METHOD, VT_VARIANT,
//(void*)&vtResult,parms );
//








			//	BSTR  brstr = pDispParams->rgvarg[5].pvarVal->bstrVal;
	 //                   	//hr = pBrowser->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void **>(&previousBrowser));
                        if(maxnum == 89 && m_step ==4)
     				    	state = CBillUrlManager::GetInstance()->GetBill(m_pWebBrowser2, m_pBillData, m_step,b_isGetMonth);
						else
/*=  SysStringLen(pDispParams->rgvarg[0].pvarVal->bstrVal)*/


			//OutputDebugString(strSour.c_str());


							  BSTR bsss;
							pRightDoc->get_body(&pElement);
							pElement->get_innerHTML(&bsss);
							pElement->get_innerText(&bsss);
							pElement->get_className(&bsss);


//							hr1 = m_windowHeader->execScript(L"document.getElementById(\"result\").contentWindow.document.forms.jhForm.cur.value = \"14\"", L"javascript",&v1);
							//hr1 = m_windowHeader->execScript(L"document.getElementById(\"accountFrm\").contentWindow.document.getElementById(\"result\").contentWindow.document.forms.jhForm.time_span.value = \"2\"", L"javascript",&v);
		     //   				str3 = _T("document.getElementById(\"CUR_TYPE\").value = \"14\"");    //14  01
							//BSTR   bs3(L"document.getElementById(\"time_span\").value = \"1\"");
					 //   	str3 = _T("document.getElementById(\"cur\").value = \"01\"") ;   //14  01
							//BSTR   bs4(L"document.getElementById(\"div_E13014\").innerHTML =\"Peoples Republic of China\"");//document.getElementById(\"accountFrm\").contentWindow.
							//BSTR   bs4(L"requestE13014=function (){ alert(\"ddd\"); };"); 
							//BSTR   bs4(L"alert('hello')"); 
//							hr1 = m_windowHeader->execScript(L"document.getElementById(\"result\").contentWindow.document.forms.jhForm.cur.value = \"14\"", L"javascript",&v1);
							//hr1 = m_windowHeader->execScript(L"document.getElementById(\"accountFrm\").contentWindow.document.getElementById(\"result\").contentWindow.document.forms.jhForm.time_span.value = \"2\"", L"javascript",&v);

						if(m_month < 1)
					    	str3 = _T("window.location=\"http://creditcard.ccb.com/#\"") ;   //14  01
						//SysFreeString(bsName);











////////////////////////////////////////////////////////
//Fill   input   field   in   IE
//If   it   works,   it   is   written   by   masterz,otherwise   I   don 't
//know   who   writes   it^_^
///////////////////////////////////////////////////////
#import   <mshtml.tlb>   //   Internet   Explorer   5
#import   <shdocvw.dll>
#include   "Shlwapi.h "
#pragma   comment(lib, "Shlwapi.lib ")
int   _tmain(int   argc,   _TCHAR*   argv[])
{
CoInitialize(NULL);
SHDocVw::IShellWindowsPtr   m_spSHWinds;
if(m_spSHWinds.CreateInstance(__uuidof(SHDocVw::ShellWindows))   ==   S_OK)
{
IDispatchPtr   spDisp;
long   nCount   =   m_spSHWinds-> GetCount();
for   (long   i   =   0;   i   <   nCount;   i++)
{
_variant_t   va(i,   VT_I4);
spDisp   =   m_spSHWinds-> Item(va);
SHDocVw::IWebBrowser2Ptr   spBrowser(spDisp);
if   (spBrowser   !=   NULL)
{
IDispatchPtr   spDisp;
if(spBrowser-> get_Document(&spDisp)   ==   S_OK   &&   spDisp!=   0   )
{
MSHTML::IHTMLDocument2Ptr   spHtmlDocument(spDisp);
MSHTML::IHTMLElementPtr   spHtmlElement;
if(spHtmlDocument==NULL)
continue;
spHtmlDocument-> get_body(&spHtmlElement);
if(spHtmlDocument==NULL)
continue;
HRESULT   hr;
MSHTML::IHTMLElementCollection*   pColl=NULL;
hr=spHtmlDocument-> get_all(&pColl);
if(pColl!=NULL&&SUCCEEDED(hr))
{
long   lcount   =   0;
pColl-> get_length(&lcount);
for(int   i=0;i <lcount;i++)
{
_variant_t   index;
index.vt=VT_I4;
index.intVal=i;
IDispatchPtr   disp;
disp=pColl-> item(index,index);
if(disp==NULL)
hr=E_FAIL;
else
{
MSHTML::IHTMLInputElementPtr   pInput(disp);
if(pInput)
{
BSTR   bstrtype;
pInput-> get_type(&bstrtype);
printf(_bstr_t(bstrtype));
if(StrCmpW(bstrtype,L "text ")==0)
{
pInput-> put_value(_bstr_t( "fill   it "));
printf( "fill   a   field\n ");
}
SysFreeString(bstrtype);
}
}

}
pColl-> Release();
}
}

}
}

}
else  
{
printf( "Shell   Windows   interface   is   not   avilable\n ");
}
CoUninitialize();
return   0;
}



针对域，这段代码，多少有点作用
IHTMLDocument2*   pDoc2;
CComBSTR   tagName;
pElement-> get_tagName(&tagName);
CString   str   =   tagName;
str.MakeUpper();
if   (str   ==   "FRAME "   ||   str   ==   "IFRAME ")
{
HRESULT   hr;  
IHTMLWindow2   *pHTMLWindow;
IHTMLFrameBase2*   pHTMLFrameBase2;
hr   =pElement-> QueryInterface(IID_IHTMLFrameBase2,   (void**)&pHTMLFrameBase2);
pElement-> Release();
hr   =   pHTMLFrameBase2-> get_contentWindow(&pHTMLWindow);
pHTMLFrameBase2-> Release();
hr   =   pHTMLWindow-> get_document(&pDoc2);
然后用IHTMLDocument2对域进行操作

试试这样,试试获取每个对象的文本
IHTMLElementCollection*   pCollection;
pHTMLDocument-> get_all(&pCollection);
long   len;
pCollection-> get_length(&len);
for   (long   l=0;   l <len;   l++)
{
        VARIANT   varIndex,   var2;
        VariantInit(&varIndex);
        VariantInit(&var2);
        varIndex.vt   =   VT_I4;
        varIndex.lVal   =   l;
        IDispatch   pDisp;
        pCollection-> item(   varIndex,   var2,   &pDisp   );
        IHTMLElement*   pElem;
        pDisp-> QueryInterface(   IID_IHTMLElement,   (LPVOID*)   &pElem   );
        BSTR   bstrHTMLText;
        pElem-> get_outerText((&bstrHTMLText);
CString   strText   =   bstrHTMLText;
SysFreeString(   bstrHTMLText);
pElem-> Release();
}
pCollection-> Release(); 





e:\moneyhub\BankUI\UIControl\ChildFrm.cpp

//		FS()->MDI()->CloseCategory(m_pItem->m_pCategory);
		FS()->MDI()->HideCategory(m_pItem->m_pCategory);

	// 结束提示对话框
	if(m_pItem->m_pCategory->m_relateHwnd)
		::SendMessage(m_pItem->m_pCategory->m_relateHwnd, WM_FINISH_GET_BILL, 0, 0);

	if (wParam == FINISH_GET_BILL_STEP)
	{
		mhMessageBox(GetRootWindow(m_hWnd), L"账单导入成功", L"财金汇账单功能", MB_OK);
		if(m_pItem != NULL)
			FS()->MDI()->CloseCategory(m_pItem->m_pCategory);
	}

	CString buf=(LPCTSTR)lParam;
	if(m_pItem)
	{
		m_pItem->m_pCategory->SetShowInfo( 2 );
		FS()->MainFrame()->RecalcClientSize(-1, -1);
	}

<<<<<<< .mine
	::PostMessage(FS()->hMainFrame, WM_GETTING_BILL, lParam, (LPARAM)m_hWnd);
=======
	::PostMessage(FS()->hMainFrame, WM_GETTING_BILL, wParam, (LPARAM)m_hWnd);
>>>>>>> .r6580

		MESSAGE_HANDLER_EX(WM_SET_BILLMONTH, OnSetBillMonth)
	LRESULT OnSetBillMonth(UINT  uMsg , WPARAM wParam, LPARAM lParam); 

LRESULT CChildFrame::OnSetBillMonth(UINT  uMsg , WPARAM wParam, LPARAM lParam)
{
	//COPYDATASTRUCT *pCopyDataStruct = (COPYDATASTRUCT*)lParam;
	//int ggg = pCopyDataStruct->dwData;
	int kkk = (DWORD)wParam;
 // CString strRecievedText = (LPCSTR) (pCopyDataStruct->lpData);
	LPCSTR ddd = (LPCSTR) lParam;
  CString strRecievedText1 = (char *) lParam;

	if(m_pItem)
	{
		m_pItem->m_pCategory->SetShowInfo( 2 );
		FS()->MainFrame()->RecalcClientSize(-1, -1);
	}
	//::PostMessage(FS()->hMainFrame, WM_GETTING_BILL, lParam, (LPARAM)m_hWnd);
	::PostMessage(FS()->hMainFrame, WM_GETTING_BILL, (LPARAM)m_hWnd, lParam);

  return 0;

}




	std::wstring wstrText;
	if(SHOW_LOAD_DLG == wParam) // 显示正登陆。。。
		wstrText = L"正在登陆...";

	CShowInfoDlg tip((HWND)lParam, wstrText.c_str()); // 默认时显示 "正在导入账单，请稍候......"
>>>>>>> .r6580
	tip.DoModal();
	///**/SelectMonthNode monthNode;
	////std::string str = "2007-06";
	//monthNode.insert(std::make_pair((char*)"2007-06-24", CHECKBOX_SHOW_ONE_LINE | CHECKBOX_SHOW_CHECKED));
	//monthNode.insert(std::make_pair(strRecievedText1, CHECKBOX_SHOW_CHECKED));
	//monthNode.insert(std::make_pair((char*)"2007-08", CHECKBOX_SHOW_UNCHECKED));
	//monthNode.insert(std::make_pair((char*)"2007-09", CHECKBOX_SHOW_CHECKED));
	//monthNode.insert(std::make_pair((char*)"2007-10", CHECKBOX_SHOW_UNCHECKED | CHECKBOX_SHOW_ONE_LINE));
	//monthNode.insert(std::make_pair((char*)"2007-11", CHECKBOX_SHOW_UNCHECKED | CHECKBOX_SHOW_ONE_LINE));
	//CMonthSelectDlg dlg(&monthNode);
	//dlg.DoModal();


int CWebBrowserEventsManager::ShowUserGetBillState(const int& state)
{
	int nBack = 0;
	switch(state)
	{
	case SHOW_LOAD_DLG:// 显示正在登录账户界面
		{
			// 该步骤已经由::PostMessage(FS()->hMainFrame, WM_GETTING_BILL, 0, (LPARAM)m_hWnd)替代
		break;
		}
	case SHOW_ACCOUNT_SELECT:// 显示选择账单界面
		{
			// 关闭登录提示界面
			::SendMessage(m_pAxControl->m_hChildFrame, WM_FINISH_GET_BILL, SHOW_ACCOUNT_SELECT, 0);

			// 弹出月份选择界面
			::SendMessage(m_pAxControl->m_hWnd, WM_AX_ACCOUNT_SELECT, 0, (LPARAM)&(m_pBillData->m_mapBack)); // 将list的指针通过lParam传入

			// 检验看用户是否选中月份
			map<std::string, bool>::const_iterator cstIt = m_pBillData->m_mapBack.begin();
			bool bSel = false;
			for (; cstIt != m_pBillData->m_mapBack.end(); cstIt ++)
			{
				if (cstIt->second == true)
					bSel = true;

			}

			if (!bSel)
				nBack = FINISH_GET_BILL_STEP; // 结束账单导出
			else
				nBack = CBillUrlManager::GetInstance()->GetBill(m_pWebBrowser2, m_pWebBrowser2,  m_pBillData, m_step);
			
		}
		break;
	case SHOW_MONTH_SELECT:// 显示月份选择界面
		{
			//弹出月份选择界面
			::SendMessage(m_pAxControl->m_hWnd, WM_AX_MONTH_SELECT_DLG, 0, (LPARAM)&(m_pBillData->m_mapBack));

			// 直接弹出正在下载界面，所以不用退出
			// break;
		}
	case SHOW_DOWNLOAD_DLG:// 显示正在下载界面
		break;
	case A_MONTH_RECORD_FINISH: // 完成一个账单导入
		{
			// 看看还有没要导出账单的月份
			map<std::string, bool>::const_iterator cstIt = m_pBillData->m_mapBack.begin();
			bool bSel = false;
			for (; cstIt != m_pBillData->m_mapBack.end(); cstIt ++)
			{
				if (cstIt->second == true)
					bSel = true;

			}
			if (!bSel)
				nBack = FINISH_GET_BILL_STEP; // 结束账单导出
			else
			{
				map<int, wstring>* purlmap =  CBillUrlManager::GetInstance()->GetUrlMap(m_pBillData->aid, m_pBillData->type);
				map<int, wstring>::iterator ite = purlmap->find(m_step);
				if (ite == purlmap->end())
				{
					nBack = FINISH_GET_BILL_STEP;
					break;
				}

				// 将步骤后退到正式导出账单的起始步
				if (ite != purlmap->end())
				{
					ite --;
					m_step = ite->first;
				}
			}
		}
		break;
	case SHOW_RECORD_NUMBER:// 显示已下载记录条数界面
		{
			break;
		}
	//case FINISH_GET_BILL_STEP: // 完成账单下载
	//	{
	//		// 检验是一月账单下载完成还是所有的都完成
	//		CBillUrlManager::GetInstance()->FreeDll();
	//		::SendMessage(m_pAxControl->m_hChildFrame, WM_FINISH_GET_BILL, FINISH_GET_BILL_STEP, 0);
	//		::PostMessage(CExternalDispatchImpl::m_hFrame[2], WM_AX_CLOSE_GET_BILL, FINISH_GET_BILL_STEP , 0);
	//	}
		break;
	default:
		break;
	}

	return nBack;
}

			map<int, wstring>::iterator ite= purlmap->find(1);
				wstring url = ite->second;
				if(url.size() > 0)
						if(wcsncmp(url.c_str() , pDispParams->rgvarg[5].pvarVal->bstrVal,url.size()) == 0)
							m_step++;

				return S_OK;




		   // 	if(strtmp.GetLength() > 500)
					//::PostMessage(m_pAxControl->m_hChildFrame,WM_GETTING_BILL, 0, 0);
        	//strtmp.Replace(_T("&"),_T(""));

								map<int, wstring>::iterator ite= purlmap->find(8);
				wstring url = ite->second;
						if(strtmp == url.c_str() && url.size() == strtmp.GetLength())
						{
	                    	//hr = pBrowser->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void **>(&previousBrowser));
								//m_pBillData->m_loop =0;
								m_step = 8;
		    					maxnum =BILL_BROWSER_GO;
						}
				ite= purlmap->find(9);				 url = ite->second;
						if(strtmp == url.c_str() && m_step>7 && url.size() > 0)
						{
         						m_step =9;
				    			maxnum =BILL_BROWSER_GO;
						}




CAtlHttpClient   company；
CAtlNavigateData   companydata；

companydata.SetMethod( "POST ");
companydata.SetExtraHeaders(cookiestr);   //发送cookie给服务器；
company.Navigate( "www.csdn.net ", "/indes.asp ",&companydata); 



			CString   strtmpqq;
       		CString   strtmp   =   (   CString   ) pDispParams->rgvarg[0].pvarVal->bstrVal;
		   // 	if(strtmp.GetLength() > 500)
					//::PostMessage(m_pAxControl->m_hChildFrame,WM_GETTING_BILL, 0, 0);
        	strtmp.Replace(_T("&"),_T(""));
				map<int, wstring>::iterator ite= purlmap->find(8);
				wstring url = ite->second;
						if(strtmp == url.c_str() && url.size() == strtmp.GetLength())
						{
	                    	//hr = pBrowser->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void **>(&previousBrowser));
								//m_pBillData->m_loop =0;
								m_step = 14;
		    					maxnum =BILL_BROWSER_GO;
	



CString strHeaders =
     _T("Content-Type: application/x-www-form-urlencoded");
     // URL-encoded form variables -
     // name = "John Doe", userid = "hithere", other = "P&Q"
     CString strFormData = _T("name=John+Doe&userid=hithere&other=P%26Q");
    
     CInternetSession session;
     CHttpConnection* pConnection =
     session.GetHttpConnection(_T("ServerNameHere"));
     CHttpFile* pFile =
     pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
     _T("FormActionHere"));
     BOOL result = pFile->SendRequest(strHeaders,
     (LPVOID)(LPCTSTR)strFormData, strFormData.GetLength());
    
    
    Without MFC, the same code translates to straight SDK calls as follows:
     static TCHAR hdrs[] =
     _T("Content-Type: application/x-www-form-urlencoded");
     static TCHAR frmdata[] =
     _T("name=John+Doe&userid=hithere&other=P%26Q");
     statuc TCHAR accept[] =
     _T("Accept: */*");
    
     // for clarity, error-checking has been removed
     HINTERNET hSession = InternetOpen("MyAgent",
     INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
     HINTERNET hConnect = InternetConnect(hSession, _T("ServerNameHere"),
     INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
     HINTERNET hRequest = HttpOpenRequest(hConnect, "POST",
     _T("FormActionHere"), NULL, NULL, accept, 0, 1);
     HttpSendRequest(hRequest, hdrs, strlen(hdrs), frmdata, strlen(frmdata));
     // close any valid internet-handles  

char* GetAnsiString(const CString &s)

{

int nSize = s.GetLength();

char *pAnsiString = new char[nSize+1];

wcstombs(pAnsiString, s, nSize+1);

return pAnsiString;

}

std::string WChar2Ansi(LPCWSTR pwszSrc)
{
         int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
 
         if (nLen<= 0) return std::string("");
 
         char* pszDst = new char[nLen];
         if (NULL == pszDst) return std::string("");
 
         WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
         pszDst[nLen -1] = 0;
 
         std::string strTemp(pszDst);
         delete [] pszDst;
 
         return strTemp;
}
	//param = new char[lgg+1];
	//param = W2A(strtemp.GetBuffer(lgg));
	//param = strtemp.GetBuffer(lgg);
	//param = (char*)(LPCSTR)strtemp;
	//strncpy(param,strtemp,sizeof(param));
//	long lgg = strtemp.GetLength();
	//m_sAgent = _T("Mozilla/4.0 (compatible; MSIE 6.0;Windows NT 5.1)");
		
	//m_bFirst = TRUE;
	// Get data from user input
	//DoDataExchange(TRUE);

	// Disable the OK button until all operations are complete
	//::EnableWindow( hWndCtl, 0 );

	//reset the edit boxes
	//SetDlgItemText(IDC_RESOURCE,_T(""));
//	SetDlgItemText(IDC_HEADER,_T(""));

	//::EnableWindow( hWndCtl, 1 );


						//dpos = strre.find("#"); //1
						//if(dpos == string::npos)
						//	break;
						//strtable = strre.substr(0 ,dpos);
    		//			//itable = atoi(dddt.c_str());
						//strre.replace(0,dpos+1,tem);

						//dpos = strre.find("#");//2
						//if(dpos == string::npos)
						//	break;
						//stru64id = strre.substr(0 ,dpos);
    		//			//u64id = _atoi64(dddt.c_str());
						//strre.replace(0,dpos+1,tem);

						//dpos = strre.find("#");//3
						//if(dpos == string::npos)
						//	break;
						//strlut = strre.substr(0 ,dpos);
    		//			//lut = atol(dddt.c_str());
						//strre.replace(0,dpos+1,tem);

						//dpos = strre.find("#");//4
						//if(dpos == string::npos)
						//	break;
						//strdbver = strre.substr(0 ,dpos);
    		//			//idbver = atoi(dddt.c_str());
						//strre.replace(0,dpos+1,tem);

						//dpos = strre.find("#");//5
						//if(dpos == string::npos)
						//	break;
						//strmark = strre.substr(0 ,dpos);
    		//			//imark = atoi(dddt.c_str());
						//strre.replace(0,dpos+1,tem);

						//UDData uddata ={ strtable,strlut,strdbver,strmark ,0 ,""};
						//CBankData::GetInstance()->GetUserIDMap()->insert(std::make_pair(stru64id, uddata));









