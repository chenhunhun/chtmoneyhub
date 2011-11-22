#include "stdafx.h"
#include "ConvertBase.h"
#include "AxControl.h"
#include "ExternalDispatchImpl.h"
#include "..\BankData\BankData.h"
#include "..\Utils\ListManager\ListManager.h"
#include "..\Utils\UserBehavior\UserBehavior.h"
#include "..\Utils\Config\HostConfig.h"
#include "..\BankUI\Util\ProcessManager.h"
#include "..\BankUI\UIControl\SelectMonthDlg.h"
#include "..\BankUI\UIControl\CoolMessageBox.h"
#include "BankData\BkInfoDownload.h"
#include "GetBill\BillUrlManager.h"
#include "../Utils/SecurityCache/comm.h"
#include "../Utils/HardwareID/genhwid.h" // 读取硬件ID
#include "../Utils/SN/SNManager.h" // 读取SN
#include "../Utils/CryptHash/base64.h"

#define CMD_DELETE_FAVORITE		L"DeleteFav"
#define CMD_SAVE_FAVORITE		L"SaveFav"
#define CMD_GET_FAVORITE		L"GetFav" //初始化获取Fav的接口
#define CMD_CHANGEORDER_FAV		L"ChangeOrder" // 对收藏的银行进行拖动

#define CMD_ADD_EVENT			L"AddEvent"
#define CMD_DELETE_EVENT		L"DeleteEvent"
#define CMD_GET_EVENTS			L"GetEvents"
#define CMD_GET_EVENTS_ONEDAY	L"GetEventsOneDay"

#define CMD_SET_ALARM			L"SetAlarm"
#define CMD_GET_TODAYALARMS		L"GetTodayAlarms"

#define CMD_SAVE_COUPON			L"SaveCoupon"
#define CMD_DELETE_COUPON		L"DeleteCoupon"
#define CMD_GET_COUPONS			L"GetCoupons"

#define CMD_GET_APPDATA			L"GetAppData"
#define CMD_PRINT_HTML			L"PrintHtml"

#define CMD_SEND_VISITRECORD	L"SendVisitRecord"
#define CMD_GET_ENVIRONMENT		L"GetEnvironment"
#define CMD_REFRESH_PAGE		L"RefreshPage"
#define CMD_HOST_NAME			L"GetHostName"


#define CMD_QUERY_SQL			L"QuerySQL"
#define CMD_EXECUTE_SQL			L"ExecuteSQL"
#define CMD_GET_XML_DATA		L"GetXMLData"
#define CMD_GET_PHP_DATA		L"GetPhpData"

#define CMD_GET_BILL_LIST		L"sendGetBillInterface"
#define CMD_WRITE_LOG			L"WriteLog"

#define CMD_SET_JS_PARAM		L"SetParameter" // 张京新增接口2011-09-09
#define CMD_GET_JS_PARAM		L"GetParameter"
#define CMD_GET_SCREEN_SIZE		L"GetScreenSize"
#define CMD_USER_LOAD			L"UserLoad" // 用户登录接口
#define CMD_USER_REGEDIT		L"UserRegedit" // 用户注册接口
#define CMD_USER_CHECK			L"UserMailCheck" // 邮箱校验接口
#define CMD_USER_AUTO_DLG		L"AutoDialog" // 打开或关闭指定的对话框接口
#define CMD_USER_AUTO_LOAD		L"AutoLoad" // 自动登录
#define CMD_SHELL_EXPLORER		L"ShellExplorer" // 跳转到网页
#define CMD_CHANGE_WINDOW_NAME	L"ChangeWindowName"
#define CMD_CHANGE_MAIL_PWD		L"ChangeMailOrPwd"
#define CMD_GET_CUR_USERID		L"GetCurrentUserID"

#define CMD_SHOWWAITWINDOW		L"ShowWaitWindow"

#define MAIL_CHANGE_SUCC		"72" // 邮箱修改成功
#define PWD_CHANGE_SUCC			"71" // 密码修改成功
#define USER_REGE_SUCC			"41" // 用户注册成功
#define MANU_LOAD_SUCC			"51" // 手动登录成功
#define USER_DB_PWD_LEN			32 // 用户数据库密码长度


#define DISPID_SAVE_FAVORITE	12346
#define DISPID_DELETE_FAVORITE	12347
#define DISPID_GET_FAVORITE		12350
#define DISPID_CHANGEORDER_FAV	12351

#define DISPID_ADD_EVENT		12360
#define DISPID_DELETE_EVENT		12361
#define DISPID_GET_EVENTS		12362
#define DISPID_GET_EVENTSONEDAY	12363

#define DISPID_SET_ALARM		12371
#define DISPID_GET_TODAYALARMS	12372

#define DISPID_SAVE_COUPON		12381
#define DISPID_DELETE_COUPON	12382
#define DISPID_GET_COUPONS		12383

#define DISPID_GET_APPDATA		12400
#define DISPID_PRINT_HTML		12401

#define DISPID_SEND_VISITRECORD 12410
#define DISPID_GET_ENVIRONMENT	12411
#define DISPID_REFRESH_PAGE		12412
#define DISPID_HOST_NAME		12413

#define DISPID_QUERY_SQL		12420
#define DISPID_EXECUTE_SQL		12421
#define DISPID_GET_XML_DATA		12422
#define DISPID_GET_PHP_DATA		12423
#define DISPID_SET_JS_PARAM		12424 // 张京新增接口2011-09-09
#define DISPID_GET_JS_PARAM		12425
#define DISPID_GET_SIZE			12426
//#define DISPID_GET_HEIGHT		12427
#define DISPID_USER_LOAD		12428
#define DISPID_USER_REGEDIT		12429
#define DISPID_USER_CHECK		12430
#define DISPID_USER_AUTO_DLG	12431
#define DISPID_USER_AUTO_LOAD	12432
#define DISPID_SHELL_EXPLORER	12433
#define DISPID_CHANGE_WIN_NAME	12434
#define DISPID_CHANGE_MIAL_PWD	12435
#define DISPID_GET_CUR_USERID	12436

#define DISPID_GET_BILL_LIST	12500

#define DISPID_WRITE_LOG		12600
#define DISPID_SHOWWAITWINDOW	12610


extern HWND g_hMainFrame;

HWND CExternalDispatchImpl::m_hFrame[3] = {NULL, NULL, NULL};
HWND CExternalDispatchImpl::m_hAxui = 0;
HANDLE CExternalDispatchImpl::m_logHandle = NULL;
CRect CExternalDispatchImpl::s_rectClient = CRect(0, 0, 0, 0);
std::list<std::string> CExternalDispatchImpl::m_sstrVerctor;
std::map<std::string, std::string> CExternalDispatchImpl::m_mapParam;
CExternalDispatchImpl::CExternalDispatchImpl(CAxControl *pAxControl) : m_pAxControl(pAxControl)
{
}

CExternalDispatchImpl::~CExternalDispatchImpl()
{
}

STDMETHODIMP_(ULONG) CExternalDispatchImpl::AddRef()
{
	return 1;
}

STDMETHODIMP_(ULONG) CExternalDispatchImpl::Release()
{
	return 1;
}

STDMETHODIMP CExternalDispatchImpl::QueryInterface(const IID &riid, void **ppvObject)
{
	if (riid == IID_IUnknown) 
	{ 
		AddRef() ;
		*ppvObject = (void*)(this); 
		return S_OK; 
	} 
	else if (riid == IID_IDispatch)
	{
		AddRef() ;
		*ppvObject = (IDispatch*)(this); 
		return S_OK; 
	}

	return E_NOINTERFACE ;
}

// IDispatch
STDMETHODIMP CExternalDispatchImpl::GetTypeInfoCount(UINT *pctinfo)
{
	return E_NOTIMPL ;
}

STDMETHODIMP CExternalDispatchImpl::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL ;
}

STDMETHODIMP CExternalDispatchImpl::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	HRESULT hr = NOERROR;

	for (UINT i = 0; i < cNames; i++)
	{
		if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_SAVE_FAVORITE, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_SAVE_FAVORITE;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_DELETE_FAVORITE, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_DELETE_FAVORITE;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_FAVORITE, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_FAVORITE;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_CHANGEORDER_FAV, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_CHANGEORDER_FAV;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_ADD_EVENT, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_ADD_EVENT;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_DELETE_EVENT, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_DELETE_EVENT;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_EVENTS, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_EVENTS;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_EVENTS_ONEDAY, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_EVENTSONEDAY;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_SET_ALARM, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_SET_ALARM;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_TODAYALARMS, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_TODAYALARMS;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_SAVE_COUPON, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_SAVE_COUPON;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_DELETE_COUPON, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_DELETE_COUPON;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_COUPONS, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_COUPONS;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_APPDATA, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_APPDATA;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_PRINT_HTML, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_PRINT_HTML;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_SEND_VISITRECORD, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_SEND_VISITRECORD;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_ENVIRONMENT, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_ENVIRONMENT;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_REFRESH_PAGE, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_REFRESH_PAGE;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_HOST_NAME, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_HOST_NAME;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_QUERY_SQL, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_QUERY_SQL;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_EXECUTE_SQL, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_EXECUTE_SQL;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_XML_DATA, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_XML_DATA;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_PHP_DATA, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_PHP_DATA;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_SET_JS_PARAM, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_SET_JS_PARAM;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_JS_PARAM, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_JS_PARAM;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_SCREEN_SIZE, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_SIZE;
		}
		/*else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_SCREEN_HEIGHT, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_HEIGHT;
		}*/
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_USER_LOAD, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_USER_LOAD;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_USER_REGEDIT, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_USER_REGEDIT;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_USER_CHECK, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_USER_CHECK;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_USER_AUTO_DLG, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_USER_AUTO_DLG;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_USER_AUTO_LOAD, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_USER_AUTO_LOAD;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_SHELL_EXPLORER, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_SHELL_EXPLORER;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_CHANGE_WINDOW_NAME, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_CHANGE_WIN_NAME;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_CHANGE_MAIL_PWD, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_CHANGE_MIAL_PWD;
		}
		else if(CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_CUR_USERID, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_CUR_USERID;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_GET_BILL_LIST, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_GET_BILL_LIST;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_WRITE_LOG, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_WRITE_LOG;
		}
		else if (CSTR_EQUAL == CompareString(lcid, NORM_IGNORECASE | NORM_IGNOREWIDTH, CMD_SHOWWAITWINDOW, -1, (TCHAR *)rgszNames[i], -1))
		{
			rgDispId[i] = DISPID_SHOWWAITWINDOW;
		}
		else
		{
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}

	return hr;
}

DWORD WINAPI CExternalDispatchImpl::DownloadBkCtrlThreadProc(LPVOID lpParam)
{
	if (NULL == lpParam)
		return 0;

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"创建了DownloadBkCtrlThreadProc");
	CBkInfoDownloadManager *pTemp = CBkInfoDownloadManager::GetInstance ();
	std::string strTempID = (char*)lpParam;
	AddUserFavBank(strTempID); // 记录用户收藏银行的ID
	pTemp->MyBankCtrlDownload ((char*)lpParam);
	RemoveUserFavBank(strTempID); // 移除用户收藏的ID
	char *lp = (char*)lpParam;
	delete[] lp;
	return 0;
}

STDMETHODIMP CExternalDispatchImpl::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	USES_CONVERSION;

	if (dispIdMember == DISPID_SAVE_FAVORITE && (wFlags & DISPATCH_METHOD))
	{
		CBankData* pBankData = CBankData::GetInstance();

		//std::string strFav;
		wstring wInfo = pDispParams->rgvarg[0].bstrVal;
		m_strBkID = WToA(wInfo, 936);
		
		//m_strBkID = OLE2A(pDispParams->rgvarg[0].bstrVal);
		if (m_strBkID.find ("&") > 0)
			m_strBkID = m_strBkID.substr (0, m_strBkID.find ("&"));

		pBankData->SaveFav(m_strBkID, 0);// 保存收藏, 0表示没有下载

		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"添加一个收藏");
		CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, A2W(m_strBkID.c_str()));

		// 执行银行控件下载
	/*
		CBkInfoDownloadManager *pTemp = CBkInfoDownloadManager::GetInstance ();
		pTemp->MyBankCtrlDownload ((char*)strFav.c_str ());*/
		
		//
		DWORD dwThreadID = 0;
		char* appid = new char[20];
		memset(appid, 0, 20);
		strcpy_s(appid, 20, m_strBkID.c_str ());

		CloseHandle (CreateThread (NULL, 0, DownloadBkCtrlThreadProc,(LPVOID)appid, 0, &dwThreadID)); 
		
		return S_OK ;
	}
	else if (dispIdMember == DISPID_CHANGEORDER_FAV &&  (wFlags & DISPATCH_METHOD))
	{
		CBankData* pBankData = CBankData::GetInstance ();
		std::string strBkID = OLE2A(pDispParams->rgvarg[2].bstrVal);
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"更改了顺序");
		pBankData->ChangeOrder ((char*)strBkID.c_str (), pDispParams->rgvarg[1].intVal, pDispParams->rgvarg[0].intVal);
	}
	else if (dispIdMember == DISPID_DELETE_FAVORITE && (wFlags & DISPATCH_METHOD))
	{
		//删除收藏
		CBankData* pBankData = CBankData::GetInstance();
		if (pDispParams->rgvarg[0].vt != VT_BSTR)
			return S_OK;
		std::string strBkID = OLE2A(pDispParams->rgvarg[0].bstrVal);
		RemoveUserFavBank(strBkID); // 移除

		CBkInfoDownloadManager::GetInstance()->CancleDownload((char*)strBkID.c_str());
		pBankData->DeleteFav((char*)strBkID.c_str());

		CListManager::_()->DeleteAFavBank(strBkID);//
		CListManager::_()->UpdateHMac ();
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"删除一个收藏");

		return S_OK ;
	}
	else if (dispIdMember == DISPID_GET_FAVORITE && (wFlags & DISPATCH_METHOD))
	{
		//获取收藏
		std::string strFav;

		CBankData* pBankData = CBankData::GetInstance();
		if (pBankData->GetFav(strFav))
		{
			wstring wstrFav = AToW(strFav, 936);
			if (pVarResult != NULL)
			{
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = ::SysAllocString((LPOLESTR)(wstrFav.c_str()));
			}

			return S_OK;
		}
		else
		{
			return E_NOTIMPL;
		}
		// 这里需要在函数调用完之后更新所有收藏的状态
	}
	else if (dispIdMember == DISPID_ADD_EVENT && (wFlags & DISPATCH_METHOD))
	{
		//添加事件
		wstring wInfo = pDispParams->rgvarg[0].bstrVal;
		std::string ev = WToA(wInfo, 936);

		//std::string ev = OLE2A(pDispParams->rgvarg[0].bstrVal);

		CBankData* pBankData = CBankData::GetInstance();
		int ret = pBankData->AddEvent(ev);

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_I4;
			pVarResult->lVal = ret;
		}

		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"添加事件");
		CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, A2W(ev.c_str()));
		return S_OK;
	}
	else if (dispIdMember == DISPID_DELETE_EVENT && (wFlags & DISPATCH_METHOD))
	{
		//删除事件
		int id = pDispParams->rgvarg[0].intVal;

		CBankData* pBankData = CBankData::GetInstance();
		int ret = pBankData->DeleteEvent(id);

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_I4;
			pVarResult->lVal = ret;
		}

		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"删除事件");

		return S_OK;
	}
	else if (dispIdMember == DISPID_GET_EVENTS && (wFlags & DISPATCH_METHOD))
	{
		//获取某一月的所有事件
		int year = pDispParams->rgvarg[1].intVal;
		int month = pDispParams->rgvarg[0].intVal;

		CBankData* pBankData = CBankData::GetInstance();
		std::string ret;
		pBankData->GetEvents(year, month, ret);
		wstring wret = AToW(ret, 936);

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)wret.c_str());
		}

		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"获取事件");
		CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, A2W (ret.c_str ()));

		return S_OK;
	}
	else if (dispIdMember == DISPID_GET_EVENTSONEDAY && (wFlags & DISPATCH_METHOD))
	{
		//获取某一日的所有事件
		int year = pDispParams->rgvarg[2].intVal;
		int month = pDispParams->rgvarg[1].intVal;
		int day = pDispParams->rgvarg[0].intVal;

		CBankData* pBankData = CBankData::GetInstance();
		std::string ret;
		pBankData->GetEventsOneDay(year, month, day, ret);
		wstring wret = AToW(ret, 936);

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)wret.c_str());
		}

		return S_OK;
	}
	else if (dispIdMember == DISPID_SET_ALARM && (wFlags & DISPATCH_METHOD))
	{
		//设定提前提醒日期
		wstring wInfo = pDispParams->rgvarg[0].bstrVal;
		std::string alarm = WToA(wInfo, 936);
		
		//std::string alarm = OLE2A(pDispParams->rgvarg[0].bstrVal);
		
		CBankData* pBankData = CBankData::GetInstance();
		int ret = pBankData->SetAlarm(alarm);
		
		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_I4;
			pVarResult->intVal = ret;
		}

		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"设置提醒");
		return S_OK;
	}
	else if (dispIdMember == DISPID_GET_TODAYALARMS && (wFlags & DISPATCH_METHOD))
	{
		//获取今日提醒
		CBankData* pBankData = CBankData::GetInstance();
		std::string ret = pBankData->GetTodayAlarms();
		wstring wret = AToW(ret, 936);

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)wret.c_str());
		}

		return S_OK;
	}
	else if (dispIdMember == DISPID_SAVE_COUPON && (wFlags & DISPATCH_METHOD))
	{
		//保存优惠券
		int style = pDispParams->rgvarg[3].intVal;
		std::string id = OLE2A(pDispParams->rgvarg[2].bstrVal);
		std::string strDate = OLE2A(pDispParams->rgvarg[1].bstrVal);
		std::string strName = OLE2A(pDispParams->rgvarg[0].bstrVal);
		
		CBankData* pBankData = CBankData::GetInstance();
		int ret = pBankData->SaveCoupon(style, id, strDate, strName);
		
		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_I4;
			pVarResult->lVal = ret;
		}

		return S_OK;
	}
	else if (dispIdMember == DISPID_DELETE_COUPON && (wFlags & DISPATCH_METHOD))
	{
		//删除优惠券
		int id = pDispParams->rgvarg[0].intVal;

		CBankData* pBankData = CBankData::GetInstance();
		int ret = pBankData->DeleteCoupon(id);

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_I4;
			pVarResult->lVal = ret;
		}

	
		return S_OK;
	}
	else if (dispIdMember == DISPID_GET_COUPONS && (wFlags & DISPATCH_METHOD))
	{
		//获取所有优惠券
		CBankData* pBankData = CBankData::GetInstance();
		std::string ret = pBankData->GetCoupons();

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)A2COLE(ret.c_str()));
		}

		return S_OK;
	}
	else if (dispIdMember == DISPID_GET_APPDATA && (wFlags & DISPATCH_METHOD))
	{
		//获取APPDATA所在路径
		TCHAR szDataPath[MAX_PATH + 1];
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szDataPath);
		_tcscat_s(szDataPath, _T("\\MoneyHub\\"));
		WCHAR szAppDataPath[MAX_PATH + 1];
		ExpandEnvironmentStringsW(szDataPath, szAppDataPath, MAX_PATH);
		
		CStringW strPath = _T("file:///");
		strPath += szDataPath;
		strPath.Replace('\\', '/');

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)(LPCTSTR)strPath);	
		}
		else
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"获取APPDATA失败");
		}

		return S_OK;
	}
	else if (dispIdMember == DISPID_PRINT_HTML && (wFlags & DISPATCH_METHOD))
	{
		//跳出到IE中进行打印
		/*
		TCHAR szDataPath[MAX_PATH + 1];
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szDataPath);
		_tcscat_s(szDataPath, _T("\\MoneyHub\\Coupons\\"));
		WCHAR szAppDataPath[MAX_PATH + 1];
		ExpandEnvironmentStringsW(szDataPath, szAppDataPath, MAX_PATH);

		CStringW strUrl = _T("file:///");
		strUrl += szDataPath;
		strUrl.Replace('\\', '/');

		CString strId;
		strId.Format(_T("%d"), pDispParams->rgvarg[0].intVal);

		strUrl += strId + _T(".htm");
		*/

		CString strUrl = pDispParams->rgvarg[0].bstrVal;
		CListManager::PrintWebPage(strUrl);

		return S_OK;
	}
	else if (dispIdMember == DISPID_SEND_VISITRECORD && (wFlags & DISPATCH_METHOD))
	{
		// 反馈访问记录
		std::string strURL = OLE2A(pDispParams->rgvarg[0].bstrVal);
		
		CUserBehavior::GetInstance()->Action_ProgramNavigate(strURL);
		// 在本地记录
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, A2W(strURL.c_str()));

		pVarResult = NULL;

		return S_OK;
	}
	else if (dispIdMember == DISPID_GET_ENVIRONMENT && (wFlags & DISPATCH_METHOD))
	{
		//获取环境变量
		WCHAR ret[1024] = { 0 };
		ExpandEnvironmentStringsW(pDispParams->rgvarg[0].bstrVal, ret, sizeof(ret));
		
		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)ret);
		}

		return S_OK;
	}

	else if (dispIdMember == DISPID_REFRESH_PAGE && (wFlags & DISPATCH_METHOD))
	{
		// 刷新指定页面
		int nPage = pDispParams->rgvarg[0].intVal;

		RefreshPage(nPage);	

		pVarResult = NULL;

		return S_OK;
	}
	// 获得host地址
	else if (dispIdMember == DISPID_HOST_NAME && (wFlags & DISPATCH_METHOD))
	{
		// 刷新指定页面
		std::string strURL = OLE2A(pDispParams->rgvarg[0].bstrVal);

		wstring url;
		if( strURL == "web" )
			url = CHostContainer::GetInstance()->GetHostName(kWeb);
		else if( strURL == "download" )
			url = CHostContainer::GetInstance()->GetHostName(kDownload);
		else if( strURL == "cloud" )
			url = CHostContainer::GetInstance()->GetHostName(kFeedback);
		else if( strURL == "adv" )
			url = CHostContainer::GetInstance()->GetHostName(kAdv);
		else if( strURL == "benefit" )
			url = CHostContainer::GetInstance()->GetHostName(kBenefit);

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)(LPCTSTR)url.c_str());			
		}

		return S_OK;
	}
	else if (dispIdMember == DISPID_QUERY_SQL && (wFlags & DISPATCH_METHOD))
	{
		// 执行查询SQL语句
		std::string strSQL;
		std::string strDbName;
		if (pDispParams->cArgs == 2)
		{
			strSQL = OLE2A(pDispParams->rgvarg[1].bstrVal);
			strDbName = OLE2A(pDispParams->rgvarg[0].bstrVal);
		}
		else
			strSQL = OLE2A(pDispParams->rgvarg[0].bstrVal);

		// 查询SQL语句

		CBankData* pBankData = CBankData::GetInstance();
		std::string ret = pBankData->QuerySQL(strSQL, strDbName);
		wstring wresult = AToW(ret, 936);//区域问题转换 936为中国

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)wresult.c_str());
		}

		return S_OK;
	}
	else if (dispIdMember == DISPID_EXECUTE_SQL && (wFlags & DISPATCH_METHOD))
	{
		if (pDispParams->rgvarg[0].vt == VT_BSTR)
		{
			// 执行SQL语句
			wstring wInfo = pDispParams->rgvarg[0].bstrVal;
			std::string strSQL = WToA(wInfo, 936);

			CBankData* pBankData = CBankData::GetInstance();
			int ret = pBankData->ExecuteSQL(strSQL);

			if (pVarResult != NULL)
			{
				pVarResult->vt = VT_I4;
				pVarResult->lVal = ret;
			}
		}

		return S_OK;
	}
	else if (dispIdMember == DISPID_SHOWWAITWINDOW && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR)
		{
			string isShow;
			isShow = OLE2A(pDispParams->rgvarg[0].bstrVal);

			if(isShow == "true")
			{
				DWORD dw = 0;
				HANDLE hThread = CreateThread(NULL, 0, _threadShowJSInfoDLG, 0, 0, &dw);
				int i = 0;
				while(i < 20)
				{
					if(g_hJSWndInfoDlg != NULL)
						break;
					Sleep(200);
					i ++;
				}
			}
			else if(isShow == "tshow")
			{
				DWORD dw = 0;
				HANDLE hThread = CreateThread(NULL, 0, _threadShowJSInfoDLG, (LPVOID)1, 0, &dw);
				int i = 0;
				while(i < 20)
				{
					if(g_hJSWndInfoDlg != NULL)
						break;
					Sleep(200);
					i ++;
				}
			}
			else if(isShow == "false")
				_endShowJSInfoDLG();
			else if(isShow == "thide")
				_endShowJSInfoDLG();

			
		}
		return S_OK;
	}
	else if (dispIdMember == DISPID_WRITE_LOG && (wFlags & DISPATCH_METHOD))
	{
#ifdef _DEBUG
		//EnterCriticalSection(&CExternalDispatchImpl::m_cs);
		if(pDispParams->rgvarg[0].vt != VT_BSTR && pDispParams->rgvarg[0].vt != VT_I4)
		{
			MessageBox(NULL, L"日志写入类型错误", L"财金汇", MB_OK);
			return S_FALSE;
		}
		string info;
		if(pDispParams->rgvarg[0].vt == VT_I4)
		{
			CHAR tp[256] = {0};
			sprintf_s(tp , 256, "%d", pDispParams->rgvarg[0].intVal);
			info = tp;
		}
		else
		{
			wstring wInfo = pDispParams->rgvarg[0].bstrVal;
			info = WToA(wInfo, 936);
		}
		if(m_logHandle == NULL)
		{
			WCHAR szAppDataPath[MAX_PATH + 1];
			ExpandEnvironmentStringsW(L"%APPDATA%\\MoneyHub\\JSLog.txt", szAppDataPath, MAX_PATH);

			m_logHandle = CreateFileW(szAppDataPath,GENERIC_WRITE | GENERIC_READ, 
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);//以追加方式打开文件
			if(m_logHandle == INVALID_HANDLE_VALUE)
			{
				m_logHandle = NULL;
			}
		}
		if(m_logHandle != NULL)
		{
			SetFilePointer(m_logHandle, 0, NULL, FILE_END);
			DWORD wl;
			if(!WriteFile(m_logHandle ,(LPVOID)info.c_str(),(info.length()), &wl, NULL))
			{
				WriteFile(m_logHandle ,L"\r\n",4, &wl, NULL);
				//LeaveCriticalSection(&CExternalDispatchImpl::m_cs);
				return S_FALSE;
			}
		}
		//LeaveCriticalSection(&CExternalDispatchImpl::m_cs);
#endif
		return S_OK;
	}
	else if (dispIdMember == DISPID_GET_XML_DATA && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[3].vt == VT_BSTR && pDispParams->rgvarg[2].vt == VT_BSTR && pDispParams->rgvarg[1].vt == VT_BSTR && pDispParams->rgvarg[0].vt == VT_BSTR)
		{

			// 取得数据统计结果
			std::string strStartDate = OLE2A(pDispParams->rgvarg[3].bstrVal);
			std::string strEndDate = OLE2A(pDispParams->rgvarg[2].bstrVal);
			std::string strDataSeries = OLE2A(pDispParams->rgvarg[1].bstrVal);
			std::string strChartType= OLE2A(pDispParams->rgvarg[0].bstrVal);

			CBankData* pBankData = CBankData::GetInstance();
			std::wstring strTemp = CA2W(strChartType.c_str());
			CRecordProgram::GetInstance()->RecordDebugInfo(strTemp, 11, L"开始获取图型数据(第一个参数是图型的类型)");

			std::string ret = pBankData->GetXMLData(strStartDate, strEndDate, strDataSeries, strChartType);

			CRecordProgram::GetInstance()->RecordDebugInfo(strTemp, 11, L"获取图型数据结束(第一个参数是图型的类型)");
			wstring wresult = AToW(ret, 936);//区域问题转换 936为中国

			if (pVarResult != NULL)
			{
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = ::SysAllocString((LPOLESTR)wresult.c_str());
			}
		}
		return S_OK;
	}
	// 为JS开发的和PHP通讯的接口
	else if (dispIdMember == DISPID_GET_PHP_DATA && (wFlags & DISPATCH_METHOD))
	{
		// 取得数据统计结果
		std::string strChartType= OLE2A(pDispParams->rgvarg[0].bstrVal);
		CString strURL = CA2W(strChartType.c_str());
		CString strSavePath;

		WCHAR tempPath[MAX_PATH] = { 0 };

		SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, tempPath);
		wcscat_s(tempPath, L"\\MoneyHub");
		::CreateDirectoryW(tempPath, NULL);
		wcscat_s(tempPath, L"\\Data");
		::CreateDirectoryW(tempPath, NULL);
		wcscat_s(tempPath, L"\\TempData");
		::CreateDirectoryW(tempPath, NULL);
		wcscat_s(tempPath, L"\\ad.dat");
		DeleteFile(tempPath);

		char szDbPath[1024] = { 0 };
		int srcCount = wcslen(tempPath);
		srcCount = (srcCount > MAX_PATH) ? MAX_PATH : srcCount;
		::WideCharToMultiByte(CP_UTF8, 0, tempPath, srcCount, szDbPath, 1024, NULL,FALSE);
		std::string strTp = szDbPath;

		strSavePath = CA2W(strTp.c_str());
		CDownloadAndSetupThread cDlThread;
		cDlThread.DownLoadInit(L"fffff", strURL, "fffff");
		cDlThread.DownLoadDataWithFile(strSavePath); // 执行阻塞下载

		if (pVarResult != NULL)
		{
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)(LPCTSTR)strSavePath);			
		}


		return S_OK;
	}

	// 为JS设置参数
	else if (dispIdMember == DISPID_SET_JS_PARAM && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[1].vt == VT_BSTR && pDispParams->rgvarg[0].vt == VT_BSTR)
		{
			std::string strParamName = OLE2A(pDispParams->rgvarg[1].bstrVal);
			std::string strParamValue = OLE2A(pDispParams->rgvarg[0].bstrVal);

			if (strParamName.length() <= 0)
				return S_OK;

			std::map<std::string, std::string>::const_iterator cstIt;
			cstIt = m_mapParam.find(strParamName); // 检验是否已经插入
			if (cstIt != m_mapParam.end())
			{
				// 已经存在
				m_mapParam.erase(cstIt);
			}

			m_mapParam.insert(std::make_pair(strParamName, strParamValue));
			if (pVarResult != NULL)
			{
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = ::SysAllocString((LPOLESTR)L"1");// 插入成功
			}
		}
		return S_OK;
	}

	// 为JS获取参数
	else if (dispIdMember == DISPID_GET_JS_PARAM && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR)
		{
			std::string strParamName = OLE2A(pDispParams->rgvarg[0].bstrVal);

			std::map<std::string, std::string>::const_iterator cstIt;
			cstIt = m_mapParam.find(strParamName);
			if (cstIt != m_mapParam.end())
			{
				if (pVarResult != NULL)
				{
					// 返回参数值
					pVarResult->vt = VT_BSTR;
					pVarResult->bstrVal = ::SysAllocString((LPOLESTR)A2COLE(cstIt->second.c_str()));			
				}
			}
		}
		return S_OK;
	}

	// 用户登录接口
	else if (dispIdMember == DISPID_USER_LOAD && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[1].vt == VT_BSTR && pDispParams->rgvarg[2].vt == VT_BSTR)
		{
			string strAutoLoad = OLE2A(pDispParams->rgvarg[0].bstrVal); // 为true或者false
			string strPassword = OLE2A(pDispParams->rgvarg[1].bstrVal);
			string strMail = OLE2A(pDispParams->rgvarg[2].bstrVal);

			//string hid = "xml=gaohuitai2006@163.com#kekValue#33351731445934782129#0L5lgKrJ8NdSpe5qgEpKp6GyKskHJ%2FvK#";


			char pKek[33] = {0};
			//memset(pOut, 0, strPassword.length() + 1];
			UserKekPack((unsigned char*)strPassword.c_str(), strPassword.length(), (unsigned char*)pKek); // 加密生成KEK
			string strTp;
			FormatHEXString(pKek, 32, strTp);

			string strParam = "xml=" + strMail + MY_PARAM_END_TAG;
			strParam += strTp;//strTp
			strParam += MY_PARAM_END_TAG;


			string strHWID = GenHWID2();
			wstring wstrHWID = CA2W(strHWID.c_str());
			string strSN = CSNManager::GetInstance()->GetSN();

			strParam += strSN;
			strParam += MY_PARAM_END_TAG;

			strParam += strHWID;
			strParam += MY_PARAM_END_TAG;

			CDownloadThread downloadThread;
			downloadThread.DownLoadInit(wstrHWID.c_str(), L"http://moneyhub.ft.com/server/manu_log_on.php", (LPSTR)strParam.c_str());
			char chTemp[1024] = {0};
			DWORD dwRead = 0;
			string strSub;
			int nBackVal = downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead);
			if (ERR_SUCCESS != nBackVal)
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, CRecordProgram::GetInstance()->GetRecordInfo(L"手动通讯时与服务器通信失败:%d",nBackVal));
				// 用本地登录进行登录
			}

			//if () 如果为41，成功，如果43，已经存在
			string strRead = chTemp;
			if (strRead.find(MY_PARAM_END_TAG) != string::npos)
				strSub = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));
			if (strSub.find(MANU_LOAD_SUCC) != string::npos) // 如果为51，成功
			{
				// 读取stoken
				string strTag = MY_PARAM_END_TAG;
				strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
				string strStoken = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

				strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
				string strEDEK = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

				strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
				string strUserID = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

				// 查找UserID在DataDB中否存在
				string strSQL = "select userid from datUserInfo where userid = '";
				strSQL += strUserID;
				strSQL += "'";
				string strBack = CBankData::GetInstance()->QuerySQL(strSQL, "DataDB");
				if (strBack.find(strUserID) == string::npos) // 不存在
				{
					string strSQL = "insert into datUserInfo(userid, mail, edek) values ('";
					strSQL += strUserID;
					strSQL += "','";

					strSQL += strMail;

					strSQL += "','";
					strSQL += strEDEK;

					strSQL += "')";

					// 新加一条记录到DataDB中
					CBankData::GetInstance()->ExecuteSQL(strSQL, "DataDB");
				}

				strSQL = "update  datUserInfo set edek = '";
				strSQL += strEDEK;

				strSQL += "', stoken = '";
				strSQL += strStoken;
	
				strSQL += "', mail = '";
				strSQL += strMail;
				strSQL += "'";

				if (strAutoLoad == "true")
				{
					strSQL += ",autoload = 1";
				}

				strSQL += " where userid = '";
				strSQL += strUserID;

				strSQL += "'";

				// 记录mail,EDEK到DataDB.dat中
				CBankData::GetInstance()->ExecuteSQL(strSQL, "DataDB");

				//strSub.clear(); // 成功时不返回任何数据给JS


				char edekVal[33] = {0};
				char keyVal[USER_DB_PWD_LEN + 1] = {0};
				int nBack = 0;

				// 将十六进制符串变成十进制数据
				FormatDecVal(strEDEK.c_str(), edekVal, nBack);

				// 将EDEK进行解密
				UserEdekUnPack((unsigned char*)edekVal, 32, (unsigned char*)pKek, (unsigned char*)keyVal);

				string strUserDb = strUserID + ".dat";
				// 将该用户的库设置成当前数据库,并用密码打开
				CBankData::GetInstance()->SetCurrentUserDB((LPSTR)strUserDb.c_str(), keyVal, USER_DB_PWD_LEN);


				// 更改当前用户信息
				CBankData::GetInstance()->m_CurUserInfo.strstoken = strStoken;
				CBankData::GetInstance()->m_CurUserInfo.strmail = strMail;
				CBankData::GetInstance()->m_CurUserInfo.struserid = strUserID;

				// 让主框架程序显示修登录的用户名
				wstring wstr = CA2W(strMail.c_str());
				::SendMessage(g_hMainFrame, WM_SETTEXT, WM_UPDATE_USER_STATUS, (LPARAM)wstr.c_str());

			}

			if (pVarResult != NULL)
			{
				// 返回参数值
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = ::SysAllocString((LPOLESTR)A2COLE(strSub.c_str()));

			}


		}
		return S_OK;
	}
	// 用户注册接口
	else if (dispIdMember == DISPID_USER_REGEDIT && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[1].vt == VT_BSTR)
		{
			string strPassword = OLE2A(pDispParams->rgvarg[0].bstrVal);
			string strMail = OLE2A(pDispParams->rgvarg[1].bstrVal);
			string strParam = strMail + MY_PARAM_END_TAG;

			char pKek[33] = {0};
			// 生成KEY
			UserKekPack((unsigned char*)strPassword.c_str(), strPassword.length(), (unsigned char*)pKek);

			string strTp;
			FormatHEXString(pKek, 32, strTp);

			// 添加KEK
			strParam += strTp;
			strParam += MY_PARAM_END_TAG;


			// 随机生成DEK
			srand((unsigned)time(NULL));
			int scope = 255;
			int i = 0;
			unsigned char chValue[USER_DB_PWD_LEN + 1] = {0};
			while( i < USER_DB_PWD_LEN)
			{
				chValue[i] = rand() % scope + 1;// 产生随机数
				i ++;
			}

			char pEDEK[33] = {0};
			UserEdekPack(chValue, USER_DB_PWD_LEN, (unsigned char *)pKek, (unsigned char*)pEDEK); // 将KEK加密生成EDEK

			FormatHEXString(pEDEK, 32, strTp);
			// 添加参数EDEK
			strParam += strTp;
			strParam += MY_PARAM_END_TAG;

			// 添加参数SN
			strParam += CSNManager::GetInstance()->GetSN();
			strParam += MY_PARAM_END_TAG;

			// 添加参数HID
			string strHWID = GenHWID2();
			strParam += strHWID;
			strParam += MY_PARAM_END_TAG;

			//Base64Encode(pEDEK, 32, strParam);
			//Base64Encode(strParam); // 做Base64编码

			string strSend = "xml=" + strParam;

			wstring wstrHWID = CA2W(strHWID.c_str());
			
			CDownloadThread downloadThread;
			downloadThread.DownLoadInit(wstrHWID.c_str(), L"http://moneyhub.ft.com/server/registration.php", (LPSTR)strSend.c_str());
			char chTemp[1024] = {0};
			DWORD dwRead = 0;
			string strSub;
			int nBackVal = downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead);
			if (ERR_SUCCESS != nBackVal)
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, CRecordProgram::GetInstance()->GetRecordInfo(L"用户注册时与服务器通信失败:%d",nBackVal));
			}
			else
			{
				//if () 如果为41，成功，如果43，已经存在
				string strRead = chTemp;
				if (strRead.find(MY_PARAM_END_TAG) != string::npos)
					strSub = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

				if (strSub.find(USER_REGE_SUCC) != string::npos) // 如果为41，成功
				{
					// 读取userid
					string strTag = MY_PARAM_END_TAG;
					strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
					string strUserID = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));



					string strSQL = "insert into datUserInfo(userid, mail, edek) values ('";
					strSQL += strUserID;
					strSQL += "','";

					strSQL += strMail;

					strSQL += "','";
					strSQL += strTp;

					strSQL += "')";

					// 记录mail,EDEK到DataDB.dat中
					CBankData::GetInstance()->ExecuteSQL(strSQL, "DataDB");

					strMail = strMail.substr(0, strMail.find('.'));
					strMail += ".dat";

					// 创建一个新用户库,用DEK进行加密
					CBankData::GetInstance()->CreateNewUserDB((LPSTR)strMail.c_str(), (LPSTR)chValue, USER_DB_PWD_LEN);	
					CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, L"用户注册成功！");

				}
			}

			if (pVarResult != NULL)
			{
				// 返回参数值
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = ::SysAllocString((LPOLESTR)A2COLE(strSub.c_str()));
				
			}

		}

		return S_OK;
	}
	// 邮箱检验接口
	else if (dispIdMember == DISPID_USER_CHECK && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR)
		{
			std::string strParam = OLE2A(pDispParams->rgvarg[0].bstrVal);

			if (strParam.length() <= 0)
				return S_OK;

			strParam += MY_PARAM_END_TAG;


			string strTp;
			// 格式化十六进制的字符串
			FormatHEXString((char*)strParam.c_str(), strParam.length(), strTp);

			string strMail = "xml=" + strTp;

			wstring wstrHWID = CA2W(GenHWID2().c_str());
			

			CDownloadThread downloadThread;
			downloadThread.DownLoadInit(wstrHWID.c_str(), L"http://moneyhub.ft.com/server/before_registration.php ", (LPSTR)strMail.c_str());
			char chTemp[256] = {0};
			DWORD dwRead = 0;
			string strSub;
			int nBackVal = downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead);
			if (ERR_SUCCESS != nBackVal)
			{
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, CRecordProgram::GetInstance()->GetRecordInfo(L"邮箱校验时与服务器通信失败:%d",nBackVal));
			}

			string strRead = chTemp;
			if (strRead.find(MY_PARAM_END_TAG) != string::npos)
				strSub = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));
			
			if (pVarResult != NULL)
			{
				// 返回参数值
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = ::SysAllocString((LPOLESTR)A2COLE(strSub.c_str()));
			}

		}
		return S_OK;
	}

	// 打开或关闭用户对话框接口
	else if (dispIdMember == DISPID_USER_AUTO_DLG && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[1].vt == VT_BSTR)
		{
			std::string strParamType = OLE2A(pDispParams->rgvarg[0].bstrVal); // 操作类型 打开还是关闭
			std::string strParamName = OLE2A(pDispParams->rgvarg[1].bstrVal); // 页面名称 注册还是登录
			int nName = -1, nType = -1;

			if ("load" == strParamName)
				nName = MY_TAG_LOAD_DLG;
			else if ("register" == strParamName)
				nName = MY_TAG_REGISTER_DLG;
			else if ("setting" == strParamName)
				nName = MY_TAG_SETTING_DLG;
			else if ("registerguide" == strParamName)
				nName = MY_TAG_REGISTER_GUIDE;

			if ("true" == strParamType)
				nType = WY_STATUE_SHOW_DLG;
			else if ("false" == strParamType)
				nType = MY_STATUE_CLOSE_DLG;

			::PostMessage(g_hMainFrame, WM_AUTO_USER_DLG, nName, nType);
		}
		return S_OK;
	}

	// 自动登录
	else if (dispIdMember == DISPID_USER_AUTO_LOAD && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR)
		{
			std::string strMail = OLE2A(pDispParams->rgvarg[0].bstrVal);

		}
		return S_OK;
	}

	// 打开网页
	else if (dispIdMember == DISPID_SHELL_EXPLORER && (wFlags & DISPATCH_METHOD))
	{
		return S_OK;
	}

	else if (dispIdMember == DISPID_CHANGE_WIN_NAME && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR)
		{
			std::string strParam = OLE2A(pDispParams->rgvarg[0].bstrVal);
			::SendMessage(g_hMainFrame, WM_SETTEXT, WM_CHANGE_USER_DLG_NAME, (LPARAM)strParam.c_str());

		}
		return S_OK;
	}

	// 更改邮箱或密码
	else if (dispIdMember == DISPID_CHANGE_MIAL_PWD && (wFlags & DISPATCH_METHOD))
	{
		if(pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[1].vt == VT_BSTR && pDispParams->rgvarg[2].vt == VT_BSTR &&
			pDispParams->rgvarg[3].vt == VT_BSTR)
		{
			std::string strType = OLE2A(pDispParams->rgvarg[3].bstrVal); // mial或password
			std::string strUserID = OLE2A(pDispParams->rgvarg[2].bstrVal); // 用户ID
			std::string strValue = OLE2A(pDispParams->rgvarg[1].bstrVal); // 值
			std::string strOldPwd = OLE2A(pDispParams->rgvarg[0].bstrVal); // 原始密码，校验时使用

			char pOldKek[33] = {0};
			// 生成KEY
			UserKekPack((unsigned char*)strOldPwd.c_str(), strOldPwd.length(), (unsigned char*)pOldKek);


			string strHWID = GenHWID2();
			wstring wstrHWID = CA2W(strHWID.c_str());
			string strSN = CSNManager::GetInstance()->GetSN();

			string strSub,strTp;
			std::string strParam = strUserID;
			if (strType == "password")
			{
				strParam += MY_PARAM_END_TAG;
				
				// 生成KEY
				char pNewKek[33] = {0};
				UserKekPack((unsigned char*)strValue.c_str(), strValue.length(), (unsigned char*)pNewKek);

				// 添加NEWKEK
				FormatHEXString(pNewKek, 32, strTp);
				strParam += strTp;
				strParam += MY_PARAM_END_TAG;

				// 随机生成DEK
				srand((unsigned)time(NULL));
				int scope = 255;
				int i = 0;
				unsigned char chValue[USER_DB_PWD_LEN + 1] = {0};
				while( i < USER_DB_PWD_LEN)
				{
					chValue[i] = rand() % scope + 1;// 产生随机数
					i ++;
				}

				char pNewEDEK[33] = {0};
				UserEdekPack(chValue, USER_DB_PWD_LEN, (unsigned char *)pNewKek, (unsigned char*)pNewEDEK); // 将KEK加密生成EDEK

				// 添加EDEK
				FormatHEXString(pNewEDEK, 32, strTp);
				strParam += strTp;
				strParam += MY_PARAM_END_TAG;

				// 添加OldKek
				FormatHEXString(pOldKek, 32, strTp);
				strParam += strTp;
				strParam += MY_PARAM_END_TAG;

				// 添加SN
				strParam += strSN;
				strParam += MY_PARAM_END_TAG;

				// 添加HID
				strParam += strHWID;
				strParam += MY_PARAM_END_TAG;

				CDownloadThread downloadThread;
				downloadThread.DownLoadInit(wstrHWID.c_str(), L"http://moneyhub.ft.com/server/change_password.php", (LPSTR)strParam.c_str());
				char chTemp[1024] = {0};
				DWORD dwRead = 0;
				int nBackVal = downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead);
				if (ERR_SUCCESS != nBackVal)
				{
					CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, CRecordProgram::GetInstance()->GetRecordInfo(L"修改密码时与服务器通信失败:%d",nBackVal));
				}
				else
				{
					string strRead = chTemp;
					if (strRead.find(MY_PARAM_END_TAG) != string::npos)
						strSub = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

					if (strSub.find(PWD_CHANGE_SUCC) != string::npos) // 如果为71，成功
					{
						// 读取stoken
						string strTag = MY_PARAM_END_TAG;
						strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
						string strStoken = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

						strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
						string strEDEK = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));
						// 更新EDEK， stoken
						string strSQL = "update  datUserInfo set edek = '";
						strSQL += strEDEK;

						strSQL += "', stoken = '";
						strSQL += strStoken;
						strSQL += "'";

						strSQL += " where userid = '";
						strSQL += strUserID;
						strSQL += "'";

						// 记录stoken,EDEK到DataDB.dat中
						CBankData::GetInstance()->ExecuteSQL(strSQL, "DataDB");

						// 读取OldEDEK,解密出数据库密码
						strSQL = "select edek from datUserInfo where userid = '";
						strSQL += strUserID;
						strSQL += "'";
						string strQueryVal = CBankData::GetInstance()->QuerySQL(strSQL, "DataDB");
						string strFind = "userid\":\"";
						if (strQueryVal.find(strFind) != string::npos)
						{
							strQueryVal = strQueryVal.substr(strQueryVal.find(strFind) + strFind.length(), strQueryVal.length());
							strQueryVal = strQueryVal.substr(0, strQueryVal.find("\""));

							char chOldEDEK[33] = {0};
							char chOldPwd[USER_DB_PWD_LEN + 1] = {0};

							// 将十六进制符串变成十进制数据
							int nBack = 0;
							FormatDecVal(strQueryVal.c_str(), chOldEDEK, nBack);

							// 将EDEK进行解密
							UserEdekUnPack((unsigned char*)chOldEDEK, 32, (unsigned char*)pOldKek, (unsigned char*)chOldPwd);

							string strUserDb = strUserID + ".dat";
							// 将该用户的库设置成当前数据库,并用密码打开
							//CBankData::GetInstance()->SetCurrentUserDB((LPSTR)strUserDb.c_str(), keyVal, USER_DB_PWD_LEN);
						}

					}
					else // 退出当前登录，重返访客身份
					{}
				}

			}
			else if (strType == "mail")// 修改邮箱
			{
				// 发送uID, mail
				strParam += MY_PARAM_END_TAG;

				// 添加mail
				strParam += strValue;
				strParam += MY_PARAM_END_TAG;

				
				// 添加OldKek
				FormatHEXString(pOldKek, 32, strTp);
				strParam += strTp;
				strParam += MY_PARAM_END_TAG;

				
				// 添加SN
				strParam += strSN;
				strParam += MY_PARAM_END_TAG;

				// 添加HID
				strParam += strHWID;
				strParam += MY_PARAM_END_TAG;

				CDownloadThread downloadThread;
				downloadThread.DownLoadInit(wstrHWID.c_str(), L"http://moneyhub.ft.com/server/change_mail.php", (LPSTR)strParam.c_str());
				char chTemp[1024] = {0};
				DWORD dwRead = 0;
				string strSub;
				int nBackVal = downloadThread.ReadDataFromSever(chTemp, 1024, &dwRead);
				if (ERR_SUCCESS != nBackVal)
				{
					CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_IE_EXTERNEL, CRecordProgram::GetInstance()->GetRecordInfo(L"修改邮箱时与服务器通信失败:%d",nBackVal));
				}
				else
				{
					//if () 如果为41，成功，如果43，已经存在
					string strRead = chTemp;
					if (strRead.find(MY_PARAM_END_TAG) != string::npos)
						strSub = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

					if (strSub.find(MAIL_CHANGE_SUCC) != string::npos) // 如果为72，成功
					{
						// 读取mail
						string strTag = MY_PARAM_END_TAG;
						strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
						string strNewMail = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

						// 更新mail， stoken
						string strSQL = "update  datUserInfo set mail = '";
						strSQL += strNewMail;
						strSQL += "'";

						strSQL += " where userid = '";
						strSQL += strUserID;

						strSQL += "'";

						// 记录mail,EDEK到DataDB.dat中
						CBankData::GetInstance()->ExecuteSQL(strSQL, "DataDB");
					}
				}


				if (pVarResult != NULL)
				{
					// 返回参数值
					pVarResult->vt = VT_BSTR;
					pVarResult->bstrVal = ::SysAllocString((LPOLESTR)A2COLE(strSub.c_str()));
				}
				return S_OK;
			}
		}
	}
	else if (dispIdMember == DISPID_GET_CUR_USERID && (wFlags & DISPATCH_METHOD))
	{
		if (pVarResult != NULL)
		{
			string strUserID = CBankData::GetInstance()->m_CurUserInfo.struserid;
			// 返回参数值
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)A2COLE(strUserID.c_str()));
		}
		return S_OK;
	}

	else if (dispIdMember == DISPID_GET_SIZE && (wFlags & DISPATCH_METHOD))
	{
		if (s_rectClient.Width() == 0 || s_rectClient.Height() == 0)
			::GetClientRect(m_hFrame[0], &s_rectClient); 
		//HWND   hTop = ::FindWindowExW(::GetDesktopWindow(),NULL,L"Progman",NULL);//
		//int x = 0, y = 0;
		//if(hTop)
		//{
		//	HWND hTopc = ::FindWindowEx(hTop, NULL, L"SHELLDLL_DefView",NULL);
		//	if(hTopc)
		//	{
		//		HWND hTopb = ::FindWindowEx(hTopc, NULL, L"SysListView32",NULL);
		//		if(hTopb)
		//		{
		//			::GetWindowRect(hTopb,&rc1);
		//			if(IsVista())//Vista、Win7的位置和XP下的不一样
		//			{
		//				x = rc1.right - 20;// - rc.Width()
		//				y = rc1.bottom - 20;// - rc.Height()
		//			}
		//			else
		//			{
		//				x = rc1.right - 20;// - rc.Width()
		//				y = rc1.bottom;// - rc.Height()
		//			}
		//		}
		//	}				
		//}
		//if(x == 0 || y == 0)
		//{
		//	RECT wkrc;
		//	SystemParametersInfo(SPI_GETWORKAREA, 0, &wkrc, 0);
		//	x = (wkrc.right - wkrc.left) - 20;// - rc.Width()
		//	y = (wkrc.bottom - wkrc.top);// - rc.Height()
		//}

		//int nX = ::GetSystemMetrics(SM_CXSCREEN);
		CString strTemp;
		strTemp.Format(L"%dx%d", s_rectClient.right, s_rectClient.bottom);
		if (pVarResult != NULL)
		{
			// 返回参数值
			pVarResult->vt = VT_BSTR;
			pVarResult->bstrVal = ::SysAllocString((LPOLESTR)(LPCTSTR)strTemp);			
		}
		return S_OK;
	}

	else if (dispIdMember == DISPID_GET_BILL_LIST && (wFlags & DISPATCH_METHOD))
	{
		// 获取账单	
		int id = 0;
		if(pDispParams->rgvarg[3].vt == VT_BSTR)
		{
			if(pDispParams->rgvarg[3].bstrVal != NULL)
				id = atoi(OLE2A(pDispParams->rgvarg[3].bstrVal));// 账户id
		}
		else if(pDispParams->rgvarg[3].vt == VT_I4)
			id = pDispParams->rgvarg[3].intVal;

		std::string aid;
		if(pDispParams->rgvarg[2].bstrVal != NULL)
			aid = OLE2A(pDispParams->rgvarg[2].bstrVal);//机构的id

		int type = 0;
		if(pDispParams->rgvarg[1].vt == VT_BSTR)
		{
			if(pDispParams->rgvarg[1].bstrVal != NULL)
				type = atoi(OLE2A(pDispParams->rgvarg[1].bstrVal));// 账户种类
		}
		else if(pDispParams->rgvarg[1].vt == VT_I4)
			type = pDispParams->rgvarg[1].intVal;// 账户种类

		string keyInfo;
		if(pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != NULL)
		{
			keyInfo = OLE2A(pDispParams->rgvarg[0].bstrVal);
		}

		CString strURL;
		bool isBeginStep;
		wstring url = CBillUrlManager::GetInstance()->GetBillUrl(aid, type, 1, isBeginStep);
		/*if (aid == "a003")
			url.replace(url.find(L"//vip."), 6, L"//mybank.");*/
		if(url.size() < 5)
		{
			MessageBoxW(NULL, L"网址错误，无法导入", L"导入账单", MB_OK);
			return S_OK;
		}
		HWND hChildFrame = ::GetExistWindow(g_hMainFrame, (LPCTSTR)url.c_str(), true);
		if(hChildFrame != NULL)
		{
			MessageBoxW(NULL, L"已经存在导入账单，请关闭或完成导入账单页后再完成下一个账单导入", L"导入账单", MB_OK);
			return S_OK;
		}

		hChildFrame = ::CreateBillPage(g_hMainFrame, url.c_str(), true);
		if(hChildFrame != NULL){

			HWND hAxControl = NULL ;
			for (int i = 0; i < 400; ++i)
			{
				hAxControl = (HWND)::SendMessage(hChildFrame, WM_ITEM_GET_AX_CONTROL_WND, 0, 0);
				if (hAxControl)
					break ;
				Sleep(10) ;
			}
			
			if(hAxControl)
			{
				BillData* pBData = new BillData;
				if(pBData)
				{
					pBData->aid = aid;
					pBData->accountid = id; 
					pBData->type = type;
					pBData->tag = keyInfo;
				}

				PostMessageW(hAxControl, WM_AX_GET_BILL, 0, (LPARAM)pBData);
			}
			else
				MessageBox(NULL, L"抓取账单失败", L"账单导入", MB_OK);

		}
		return S_OK;
	}

	return E_NOTIMPL ; 
}

void CExternalDispatchImpl::RefreshPage(int nPage)
{
	if(m_hFrame[nPage -1] != NULL)
	{
		switch(nPage)
		{
			case 1:
				PostMessageW(m_hFrame[nPage -1], WM_AX_FRAME_SETALARM, 0, 0);
				break;
			case 2:
				PostMessageW(m_hFrame[nPage -1], WM_AX_TOOLS_CHANGE, 0, 0);
				break;
			//case 3:
				//PostMessageW(m_hFrame[nPage -1], WM_AX_COUPON_CHANGE, 0, 0);
				break;
		}
	}
}

// 记录用户收藏的银行
bool CExternalDispatchImpl::AddUserFavBank(const std::string& strBkID)
{
	ATLASSERT(!strBkID.empty());
	if (strBkID.empty())
		return false;

	if (IsInUserFavBankList(strBkID))
		return false;

	m_sstrVerctor.push_back(strBkID);
	return true;
}

// 移除用户收藏的银行
bool CExternalDispatchImpl::RemoveUserFavBank(const std::string& strBkID)
{
	ATLASSERT(!strBkID.empty());
	if (strBkID.empty())
		return false;

	if (!IsInUserFavBankList(strBkID))
		return false;

	// 移除
	m_sstrVerctor.remove(strBkID);
	return true;
}

bool CExternalDispatchImpl::IsInUserFavBankList(const std::string& strBkID)
{
	std::list<std::string>::const_iterator cstIt;
	for(cstIt = m_sstrVerctor.begin(); cstIt != m_sstrVerctor.end(); cstIt ++)
	{
		std::string strTp = *cstIt;
		if (strTp == strBkID)
			break;
	}

	if (cstIt != m_sstrVerctor.end())
		return true;

	return false;
}

bool CExternalDispatchImpl::IsVista()
{
	OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
	BOOL bRet = ::GetVersionEx(&ovi);
	return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6));
}

// Base64Encode
//bool CExternalDispatchImpl::Base64Encode(char *pData, int nLen, string& strEncode)
//{
//	ATLASSERT (NULL != pData && nLen > 0);
//	if (NULL == pData || nLen <= 0)
//		return false;
//	strEncode.clear();
//
//	int dwSize = nLen * 2 + 1;
//	unsigned char* pszOut = new unsigned char[dwSize];
//	base64_encode((LPBYTE)pData, nLen, pszOut, &dwSize);
//	pszOut[dwSize] = 0;
//
//	
//	strEncode = (char*)pszOut;
//
//	delete []pszOut;
//
//	return true;
//}

// 格式化成十六进制字符串
//bool FormatHEXString(char *pData, int nLen, string& strEncode)
//{
//	ATLASSERT (NULL != pData && nLen > 0);
//	if (NULL == pData || nLen <= 0)
//		return false;
//	strEncode.clear();
//
//
//	//int dwSize = nLen * 2 + 1;
//	//unsigned char* pszOut = new unsigned char[dwSize];
//	//base64_encode((LPBYTE)pData, nLen, pszOut, &dwSize);
//
//	for(int i = 0; i < nLen; i ++)
//	{
//		CString strTp;
//		int nTemp = *(pData + i);
//		if (nTemp < 0)
//			nTemp += 256; // 取该字节的反码
//
//		strTp.Format(L"%02x", nTemp);
//		strEncode += CW2A(strTp);
//
//
//		// 根据PHP代码转换成C代码
//		/*int nOrd = *(pData + i);
//		strEncode += SingleDecToHex((nOrd - nOrd % 16) / 16);
//    	strEncode += SingleDecToHex(nOrd % 16);*/
//	}
//
//	return true;
//}