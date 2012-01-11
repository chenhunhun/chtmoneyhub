#pragma once
#include <string>
#include <map>
#include <vector>
#include <list>
#include "../../ThirdParty/tinyxml/tinyxml.h"
using namespace std;

enum webconfig
{
	kBeginTag, // 开始标志

	kJsWeb = -1, //js用的域名
	kWeb = 0,//主页地址
	kHelp = 1,//帮助页地址
	kFeedback = 2,//用户反馈页地址
	kUninstall = 3,//卸载反馈页地址	
	kDownloadMode = 4, //测试下载模式
	kPDownloadInstall = 5,//下载控件
	kPUpgrade = 6,//升级
	kPUkey = 7, // usbkey相关文件
	kPGetFile = 8, // 上传文件供分析
	kPDataFeedback = 9, // 数据反馈
	kPAutoLogon = 10, // 自动登陆
	kPManuLogon = 11, // 手动登陆
	kPBeforeRegistration = 12, // 注册前校验
	kPSendMailVerify = 13, // 重发验证邮件
	kPRegistration = 14, // 注册
	kPGetList = 15, // 获取完整列表
	kPGetDownloadData = 16, // 下载数据
	kPGetUploadData = 17, // 上传数据
	kPChangeMail = 18, // 修改邮箱
	kPChangePassword = 19, // 修改密码
	kPMailVerify = 20, // 验证邮箱
	kPUserUnlock = 21, //解除用户锁定状态
	kPUserServerTime = 22, // 获取系统时间
	kPSendFindMail = 23, // 发送OPT到指定的邮箱
	kPCheckOPT = 24, // 校验OPT
	kPInitPassword = 25, // 重置密码
	kExchangeRate, //更新汇率

	kEndTag // 结束标志
};

typedef   std::vector<std::wstring>  VECTORNPBNAME;

class CHostContainer
{
private:
	CHostContainer();
	~CHostContainer();

	static CHostContainer* m_Instance;
public:
	static CHostContainer* GetInstance();

public:
	// 初始化读取config中的web文件
	void Init(VECTORNPBNAME *pVvecNPB = NULL);
	// 根据参数获得网址的函数
	wstring GetHostName(webconfig host);

	bool IsUrlInUrlError(wstring url);

private:
	map<webconfig,wstring> m_host;
	std::list<std::wstring> m_urlError;

	bool ReadUrlData(const TiXmlNode *pErrorHtml);

	std::string GetFileContent(wstring strPath,bool bCHK);
	bool GetAllHostName(VECTORNPBNAME *pVvecNPB = NULL);
};