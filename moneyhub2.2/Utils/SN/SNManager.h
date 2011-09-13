#pragma once
#include <string>
using namespace std;


class CSNManager
{
private:
	CSNManager();
	~CSNManager();

	static CSNManager* m_Instance;
public:
	static CSNManager* GetInstance();

public:
	// 向注册表写入SN，生成SN的过程
	void MakeSN();
	// 向注册表获得SN的过程，如果不存在，返回空字符串
	string GetSN();

private:
	std::string m_sn;
	
	void MakeCheckCode(char *sn);//生成校验码

	unsigned short GenCRC16(const char *pdata, int size); 
};