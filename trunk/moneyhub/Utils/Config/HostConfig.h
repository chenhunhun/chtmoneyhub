#pragma once
#include <string>
#include <map>
#include <vector>
#include <list>
#include "../../ThirdParty/tinyxml/tinyxml.h"
using namespace std;

enum webconfig
{
	kWeb = 0,//站点
	kDownload = 1,//下载
	kFeedback = 2,//反馈
	kAdv = 3,//广告
	kBenefit = 4,//优惠券
	kDownloadMode = 5 //测试下载模式
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