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
	CAxControl* m_pAxControl;
	BSTR m_SysAllocResource;//每个线程有一个该资源用于每个线程资源的管理，防止出现内存泄露

	void RefreshPage(int nPage);	
	std::string m_strBkID;



public:
	static DWORD WINAPI DownloadBkCtrlThreadProc(LPVOID lpParam);
	static HWND m_hFrame[3];
	static std::list<std::string> m_sstrVerctor;
	// 较验是否已经添加到列表中
	static bool IsInUserFavBankList(const std::string& strBkID);
	// 记录用户收藏的银行
	static bool AddUserFavBank(const std::string& strBkID);
	// 移除用户收藏的银行
	static bool RemoveUserFavBank(const std::string& strBkID);
};
