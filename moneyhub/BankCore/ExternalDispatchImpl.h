/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  ExternalDispatchImpl.h
 *      说明：  JS外部调用接口类声明文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.27	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#pragma once
class CAxControl;
#include "windows.h"


class CExternalDispatchImpl : public IDispatch
{

public :
	CExternalDispatchImpl(CAxControl *pAxControl);
	~CExternalDispatchImpl();

public :
	STDMETHOD(QueryInterface)(REFIID, void **);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	//IDispatch
	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo);
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS  *pDispParams, VARIANT  *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

private:
	// 进行本地登录
	int ExcuteLocalLoad(const char* pMail, const unsigned char* pKek, int nLen);

private:
	CAxControl* m_pAxControl;
	void RefreshPage(int nPage);	
	std::string m_strBkID;
	static std::map<std::string, std::string> m_mapParam;

public:
	static DWORD WINAPI DownloadBkCtrlThreadProc(LPVOID lpParam);
	static HWND m_hFrame[3];
	static HWND m_hAxui;
	static HANDLE m_logHandle;
	static CRect	s_rectClient;

	bool IsVista();

	//static CRITICAL_SECTION m_cs;

	static std::list<std::string> m_sstrVerctor;
	// 较验是否已经添加到列表中
	static bool IsInUserFavBankList(const std::string& strBkID);
	// 记录用户收藏的银行
	static bool AddUserFavBank(const std::string& strBkID);
	// 移除用户收藏的银行
	static bool RemoveUserFavBank(const std::string& strBkID);
};
