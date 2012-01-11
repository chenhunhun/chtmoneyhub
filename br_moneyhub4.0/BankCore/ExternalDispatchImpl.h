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
#include <atltime.h>


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
	int ExcuteLocalLoad(const char* pMail, const unsigned char* pKek, int nLen, string& strUserID);
	void Rand20ByteUCharData(unsigned char* pStore, int nLen); // 随机生成一个20字节长的无符数组
	bool NeedExitCurrentLoad(const char* pServerRead); // 校验是否要退出当前登录，重返访客身份（用于修改密码和邮箱）
	void ChangeCurUserSynchroStatus(void); // 更改当前用户同步相关的变量

private:
	CAxControl* m_pAxControl;
	void RefreshPage(int nPage);	
	std::string m_strBkID;
	static std::map<std::string, std::string> m_mapParam;
	static CTime m_sLastRegTime; // 上一次注册时间
//	static CTime m_sLastFindMailTime; // 上一次发送找回密码邮件的时间

public:
	static DWORD WINAPI DownloadBkCtrlThreadProc(LPVOID lpParam);
	static HWND m_hFrame[3];
	static HWND m_hAxui;
	static HANDLE m_logHandle;
	static CRect	s_rectClient;
	static bool m_sbUpdateSynchroBtn;

	bool IsVista();

	//static CRITICAL_SECTION m_cs;

	static std::list<std::string> m_sstrVerctor;
	// 较验是否已经添加到列表中
	static bool IsInUserFavBankList(const std::string& strBkID);
	// 记录用户收藏的银行
	static bool AddUserFavBank(const std::string& strBkID);
	// 移除用户收藏的银行
	static bool RemoveUserFavBank(const std::string& strBkID);
	// 设置JS参数
	static void AddJSParam(const char* pPName, const char* pPVal);
	// 与服务器进行通讯
	static int CommunicateWithServer(int nEmSite, list<string> listParam, string& strBack);
};
