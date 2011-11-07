#include "stdafx.h"
#include "CustomSite.h"
#include "WebBrowserCore.h"


CWebBrowserCore::CWebBrowserCore()
: m_pWebBrowser2(NULL), m_pOleObject(NULL), m_pOleInPlaceWindowLess(NULL)
{
}

CWebBrowserCore::~CWebBrowserCore()
{
}

HRESULT CWebBrowserCore::CreateIEServer(HWND hAxWnd, IOleClientSite *pClientSite)
{
	::OleInitialize(NULL);

	CComPtr<IClassFactory> spClassFactory;

	// 创建类对象
	HRESULT hRes = CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, NULL, IID_IClassFactory, (void**)&spClassFactory);
	if (FAILED(hRes))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"CreateIEServer:CoGetClassObject error:0x%08x",hRes));
		return hRes;
	}
	
	if (!spClassFactory)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE,L"CreateIEServer:spClassFactory null");
		return E_FAIL;
	}


	// 创建COM对象
	hRes = spClassFactory->CreateInstance(NULL, IID_IOleObject, (void**)&m_pOleObject);
	if (FAILED(hRes))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"CreateIEServer:spClassFactory->CreateInstance error:0x%08x",hRes));
		return hRes;
	}


	// 设置Client Site
	hRes = m_pOleObject->SetClientSite(pClientSite);
	if (FAILED(hRes))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"CreateIEServer:m_pOleObject->SetClientSite error:0x%08x",hRes));
		return hRes;
	}


	// 获取容器窗口大小
	CRect rect;
	::GetClientRect(hAxWnd, &rect);


	// 通知控件被包含
	hRes = ::OleSetContainedObject(m_pOleObject, TRUE);
	if (FAILED(hRes))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"CreateIEServer:OleSetContainedObject error:0x%08x",hRes));
		m_pOleObject->Release();
		m_pOleObject = NULL;

		return hRes;
	}

	// 设置显示区域大小
	hRes = m_pOleObject->DoVerb(OLEIVERB_SHOW, NULL, pClientSite, -1, hAxWnd, &rect);
	if (FAILED(hRes))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"CreateIEServer:DoVerb error:0x%08x",hRes));
		m_pOleObject->Release();
		m_pOleObject = NULL;

		return hRes;
	}

	// Web Browser Object
	hRes = m_pOleObject->QueryInterface(IID_IWebBrowser2, (void**)&m_pWebBrowser2);
	if (FAILED(hRes))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"CreateIEServer:QueryInterface1 error:0x%08x",hRes));
		m_pOleObject->Release();
		m_pOleObject = NULL;

		return hRes;
	}

	// IOleInPlaceWindowLess
	hRes = m_pOleObject->QueryInterface(IID_IOleInPlaceObject, (void**)&m_pOleInPlaceWindowLess);
	if (FAILED(hRes))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_STATE, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"CreateIEServer:QueryInterface2 error:0x%08x",hRes));
		m_pOleObject->Release();
		m_pOleObject = NULL;

		m_pWebBrowser2->Release();
		m_pWebBrowser2 = NULL;

		return hRes;
	}

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_STATE, L"CreateIEServer 成功");
	return S_OK;
}

void CWebBrowserCore::ChangeSize(int left, int top, int width, int height)
{
	if (m_pOleInPlaceWindowLess)
	{
		SIZE sz = { width, height };
		SIZE sz2;
		AtlPixelToHiMetric(&sz, &sz2);
		m_pOleObject->SetExtent(DVASPECT_CONTENT, &sz2);

		RECT rc = { left, top, left + width, top + height };
		m_pOleInPlaceWindowLess->SetObjectRects(&rc, &rc);
	}
}

void CWebBrowserCore::Release()
{
	if (m_pOleObject)
	{
		m_pOleObject->SetClientSite(NULL);
		m_pOleObject->Close(OLECLOSE_NOSAVE);
		m_pOleObject->Release();
		m_pOleObject = NULL;
	}

	if (m_pOleInPlaceWindowLess)
	{
		m_pOleInPlaceWindowLess->Release();
		m_pOleInPlaceWindowLess = NULL;
	}

	if (m_pWebBrowser2)
	{
		m_pWebBrowser2->Release();
		m_pWebBrowser2 = NULL;
	}
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_STATE, L"CWebBrowserCore释放");
}