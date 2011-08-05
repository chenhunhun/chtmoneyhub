#pragma once
#include <string>
#include <map>
#include <vector>
using namespace std;

enum webconfig
{
	kWeb = 0,//站点
	kDownload = 1,//下载
	kFeedback = 2,//反馈
	kAdv = 3,//广告
	kBenefit = 4 //优惠券
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

private:
	map<webconfig,wstring> m_host;

	std::string GetFileContent(wstring strPath,bool bCHK);
	bool GetAllHostName(VECTORNPBNAME *pVvecNPB = NULL);
};