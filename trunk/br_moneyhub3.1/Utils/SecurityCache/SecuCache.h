#pragma once
#define _MD5

#include <list>
#include <string>
using namespace std;
//////////////////////////////////////////////////////////////////////////
#ifdef _MD5
#define		SECURE_SIZE	16
#endif
#include "windows.h"
#include "../../Security/BankLoader/BankLoader.h"


#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define		SECURE_BUCKET_SIZE		256
struct SecCachStruct
{
	wchar_t filename[MAX_PATH];
	unsigned char chkdata[SECURE_SIZE];
	short int tag;//0表明从Cache文件中读出，1表明检查过，2表明有更新，3表明新增,1\3为有效数据，0,2为无效数据,4表明为学习的数据
};

class CSecurityCache
{
public:
	CSecurityCache();
	virtual ~CSecurityCache();
private:
	list<SecCachStruct*>  m_secuBucket[SECURE_BUCKET_SIZE];

	wstring m_cathfile;
	unsigned char *evalueCache;
	int m_length;

	bool m_isChange;
	bool m_isShouldSend;
	int m_number;
public:
	bool Init();
	int	 GetFileNumber();
	bool Add(SecCachStruct& sec,int style = 0);//如果是学习的，style设置为1，否则用默认的
	bool IsInSecurityCache(SecCachStruct& sec);
	bool SetCacheFileName(wchar_t* secname);
	bool Clear();
	bool Flush();
	bool SetAllDataInvalid();//设置原缓存中的数据都是有效的，在单独增加的时候有用。
	

	bool CalculEigenvalue(SecCachStruct& sec);
private:
	void CheckUpdate(int& effectnum);
	unsigned char HashFunc(wchar_t *filename);

public:
	bool IsMZFile(const wchar_t* pfile);
	bool IsChanged();
	bool IsShouldSend();
	bool SetSend(bool isSend);
	bool GetEigenvalue(unsigned char *data,DWORD& length);
	bool GetEigenvalue(ModuleList& mlist);
};
