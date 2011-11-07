/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  UserBehavior.h
 *      说明：  反馈信息类的声明文件。
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
#include <vector>
#include <string>
// Moneyhub启动类型，桌面、泡泡、浏览器启动三种
// 发给服务器端的字段格式
/*
<?xml version="1.0" encoding="UTF-8"?>
<data>
<terminalid>yxTKCfGPO2OxBQ89hEc%2F%2BE5mqqRS6MWT</terminalid>
<SN>1234567890123456</SN>
<action>start</action>
<systemname>Windows Xp</systemname>
<systemversion>sp3 1.1.1.1</systemversion>
<systemlang>CHN</systemlang>
<ieversion>ie8</ieversion>
<antivirusname>360safe</antivirusname>
<cpuname>Intel Core 2 Duo</cpuname>
<productdmi>Dell inc</productdmi>
<productname>optiplex 780</productname>
<memorysize>4GB</memorysize>
<moneyhubversion>2.1.1.1</moneyhubversion>
<beforeversion>2.0.0.0</beforeversion>
<afterversion>2.1.1.1</afterversion>
<starttype>desk_start</starttype>
<visitedurl> http://www.moneyhub.com/boc.php</visitedurl>
<errorinfo>bug......</errorinfo>
<moduleinfo>safe.dell_windowssystem32..</moduleinfo>
<modulemd5>bug......</modulemd5>
<valiresult>permit</valiresult>
<valichannel>cloud safe</valichannel>
</data>*/
enum UBStartupStyle
{
	kDesktop = 0,
	kPopupWin = 1,
	kBrowser = 2,
};
enum VerifyType
{
	kSysModify = 0,
	kCloudModify = 1
};
enum VerifyResult
{
	kAllow = 0,
	kDeny = 1
};

const std::string strIEError = "100001";
const std::string strChkBkDatErr = "100002";
const std::string strChkMonyHubErr = "100003";
const std::string strChkAuthenErr = "100004";
const std::string strChkCfDatErr = "100005";
const std::string strChkUrlDatErr = "100006";
const std::string strChkDriverErr = "100007";


const std::string strCoreRunErr = "200008";
const std::string strFindDangerous = "200009";
const std::string strSelfDataErr = "200010";
const std::string strKeyFileErr = "200011";
const std::string strDriverErr = "200012";

class CUserBehavior
{
public:
	CUserBehavior();
	// 添加析构函数，清空打包发送的反馈信息 郑鹏 2011.2.12 15:23 Begin
	~CUserBehavior();
	// 添加析构函数，清空打包发送的反馈信息 郑鹏 2011.2.12 15:23 End
	
public:
	// 安装反馈的函数
	void Action_Install(int i = 0);
	// 程序启动时的反馈函数
	// 输入参数：UBStartupStyle ubss ：启动类型
	void Action_ProgramStartup(UBStartupStyle ubss);
	// 程序退出时的反馈函数
	void Action_ProgramExit();
	// 程序浏览时的反馈函数
	// 输入参数：std::string& strUrl：浏览的页面
	void Action_ProgramNavigate(const std::string& strUrl);
	// 程序卸载时的反馈函数
	void Action_Uninstall(int i = 0);
	// 程序升级时的反馈函数
	// 输入参数：std::string before：升级前的版本信息；std::string end：升级后的版本信息
	void Action_Upgrade(std::string before,std::string end);
	
	// 反馈驱动程序过滤的模块名单
	void Action_Study(std::string& strfile, const char* hash, VerifyType vtype, VerifyResult vre);

	// 反馈错误码和错误
	void Action_SendErrorInfo(const std::string& strErrCode, const std::string& strErr);

	void Action_SendDataToServerWhenExit(void); // 退出时发送数据到服务器

	static DWORD WINAPI _threadFeedBackToServer(LPVOID lp);// 独立进行反馈动作的线程

	void BeginFeedBack();
	void CloseFeedBack();

protected:
	CRITICAL_SECTION m_cs;
	HANDLE m_hThread;
	// 存储当前moneyhub版本信息
	std::string m_strMoneyVersion;
	
	std::wstring m_url;
public:
	static CUserBehavior* GetInstance();

	// 添加m_strFeedBackInfo成员变量，记录打包发送的反馈信息 郑鹏 2011.2.12 15:23 Begin
private:
	std::vector<std::string> m_strFeedBackInfo; 
	// 添加m_strFeedBackInfo成员变量，记录打包发送的反馈信息 郑鹏 2011.2.12 15:23 End

};