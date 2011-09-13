/**
*-----------------------------------------------------------*
*  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
*    文件名：  SecuHash.cpp
*      说明：  Hash白名单存储。
*    版本号：  1.0.0
* 
*  版本历史：
*	版本号		日期	作者	说明

*-----------------------------------------------------------*
*/
#include "stdafx.h"
#pragma once


#include "SecuCache.h"
#include "comm.h"
#include "../../Security/Authentication/encryption/md5.h"
#include "../RecordProgram/RecordProgram.h"

CSecurityCache::CSecurityCache()
{
	m_cathfile = L"%AppData%\\MoneyHub\\SecurityCache.dat";
	m_number = 0;
	evalueCache = 0;
}

CSecurityCache::~CSecurityCache()
{
	Clear();
}
bool CSecurityCache::SetCacheFileName(wchar_t* secname)
{
	if(secname)
		m_cathfile = secname;
	return true;
}
bool CSecurityCache::Add(SecCachStruct& sec,int style)
{
	unsigned char uid = HashFunc(sec.filename);

	SecCachStruct *data = new SecCachStruct;

	memcpy(data->filename,sec.filename,sizeof(sec.filename));
	memcpy(data->chkdata,sec.chkdata,sizeof(sec.chkdata));
	if(style == 0)
		data->tag = 3;//表明新增
	else
		data->tag = 4;//表明为学习的数据

	m_secuBucket[uid].push_back(data);
	m_isChange = true;//表明新增
	m_number ++;
	return true;
}
int CSecurityCache::GetFileNumber()
{
	return m_number;
}

bool CSecurityCache::Init()
{
	Clear();

	//Vista权限问题
	TCHAR szDataPath[MAX_PATH + 1];
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szDataPath);
	_tcscat_s(szDataPath, _T("\\MoneyHub"));

	WCHAR szAppDataPath[MAX_PATH + 1];
	ExpandEnvironmentStringsW(szDataPath, szAppDataPath, MAX_PATH);

	::CreateDirectoryW(szAppDataPath, NULL);

	WCHAR expName[MAX_PATH] ={0};
	ExpandEnvironmentStringsW(m_cathfile, expName, MAX_PATH);
	HANDLE hFile;
	/*if(true == isonce)
	{
		hFile = CreateFileW(expName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	}
	else
	{*/
	// 读取原来的cache文件
	hFile = CreateFileW(expName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//}

	if(hFile == INVALID_HANDLE_VALUE)
	{
		int error = ::GetLastError();
		CRecordProgram::GetInstance()->FeedbackError(L"SeCathe", 1000, CRecordProgram::GetInstance()->GetRecordInfo(L"读%s失败:%d", expName, error));
		return false;
	}

	DWORD dwLength = GetFileSize(hFile, NULL);
	if(dwLength <= 0)
	{
		CloseHandle(hFile);
		return true;
	}
	unsigned char* lpBuffer = new unsigned char[dwLength + 1];

	if (lpBuffer == NULL)
	{
		::MessageBoxW(NULL, L"内存空间满",L"安全缓存",MB_OK | MB_SETFOREGROUND);
		CloseHandle(hFile);
		return false;
	}

	DWORD dwRead = 0;
	if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
	{
		delete []lpBuffer;
		CloseHandle(hFile);
		::MessageBoxW(NULL, L"读安全缓存失败",L"安全缓存",MB_OK | MB_SETFOREGROUND);
		return false;
	}
	CloseHandle(hFile);
	
	unsigned char* unPackBuf = new unsigned char[dwRead];
	if(unPackBuf == NULL)
	{
		delete []lpBuffer;
		::MessageBoxW(NULL, L"内存空间满",L"安全缓存",MB_OK | MB_SETFOREGROUND);
		return false;
	}

	int ret = CacheUnPack(lpBuffer,dwRead,unPackBuf);

	delete []lpBuffer;

	if(ret < 0)
	{
		CRecordProgram::GetInstance()->FeedbackError(L"SeCathe", 1000, CRecordProgram::GetInstance()->GetRecordInfo(L"读%sUnPack异常", expName));

		delete[] unPackBuf;
		return false;
	}

	for(DWORD i = 0;i < (unsigned long)ret;)
	{
		SecCachStruct *data = new SecCachStruct;
		memcpy(data->filename,unPackBuf + i,sizeof(data->filename));
		i += sizeof(data->filename);
		memcpy(data->chkdata,unPackBuf + i,SECURE_SIZE);
		i += SECURE_SIZE;
		memcpy(&data->tag,unPackBuf + i,sizeof(short int));//tag文件从缓存中读出
		i += sizeof(short int);
		if(data->tag != 4)//表明是非学习数据
			data->tag = 1;//在缓存中读出的数据添加的话初始化为0,这里先设置为1，表明有效
		unsigned char uid = HashFunc(data->filename);
		m_secuBucket[uid].push_back(data);
		m_number ++;
	}
	delete[] unPackBuf;
	m_isChange = false;
	return true;
}

unsigned char CSecurityCache::HashFunc(wchar_t *filename)
{
	int len = wcslen(filename);
	char *p = (char*)filename;
	char uid = 0;
	for(int i = 0;i < (int)(len*sizeof(wchar_t));i ++)
	{
		uid = uid^(*(p+i));
	}
	return uid;
}

bool CSecurityCache::CalculEigenvalue(SecCachStruct& sec)
{
	const int bufsize = 8 * 1024 * 1024;
	byte* buf = new byte[bufsize];

	HANDLE hFile = CreateFileW(sec.filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		int error = ::GetLastError();
		CRecordProgram::GetInstance()->RecordWarnInfo(L"SeCathe", 1000, CRecordProgram::GetInstance()->GetRecordInfo(L"CalculEigenvalue %s异常", sec.filename));
		delete[] buf;
		memset(sec.chkdata,0,SECURE_SIZE);
		return false;
	}

	CMD5 md5;
	DWORD read;
	while (true)
	{
		ReadFile(hFile, buf, bufsize, &read, NULL);
		if(read == 0)
			break;

		md5.MD5Update(buf, read);
	}

	CloseHandle(hFile);
	md5.MD5Final(sec.chkdata);

	delete[] buf;
	return true;

}
bool CSecurityCache::IsMZFile(const wchar_t* pfile)
{
	//FILE *file;
	//file = _wfopen(pfile,L"rb");
	//if(file == NULL)
	//{  
	//	return false; 
	//}

	//try{
	//	USHORT ibuf;
	//	fread(&ibuf,sizeof(USHORT),1,file);//PE文件头地址
	//	fclose(file);
	//	if(ibuf == IMAGE_DOS_SIGNATURE)
	//	{
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}
	//catch(...)
	//{
	//	fclose(file);
	//	return false;
	//}

	// gao
	ATLASSERT (NULL != pfile && NULL != *pfile);
	if (NULL == pfile || NULL == *pfile)
		return false;

	// 打开文件
	// 64位用写操作有问题，只有用read权限就好了。
	HANDLE hFile = CreateFile (pfile, 
								GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	USHORT iBuf = 0;
	DWORD dwRead = 0;
	
	// 读取文件的第一个USHORT
	BOOL bRet = ReadFile (hFile, &iBuf, sizeof(USHORT), &dwRead, NULL);
	CloseHandle (hFile);
	if (FALSE == bRet || dwRead != sizeof (USHORT))
		return false;

	if (IMAGE_DOS_SIGNATURE == iBuf)
		return true;
	else
		return false;
}
bool CSecurityCache::IsInSecurityCache(SecCachStruct& sec)
{
	//检查文件存在与否，文件不存在，什么也不做，但要让上层认为已经检查过了
	if(PathFileExistsW(sec.filename) == false)
		return true;

	//不查非MZ文件
	bool ret = IsMZFile(sec.filename);
	if(ret == false)
		return true;

	if(m_number <= 0)
		return false;

	unsigned char uid = HashFunc(sec.filename);
	for(list<SecCachStruct*>::iterator ite = m_secuBucket[uid].begin();ite != m_secuBucket[uid].end();ite ++)
	{
		if(wcscmp((*ite)->filename,sec.filename) == 0)
		{
			CalculEigenvalue(sec);//计算特征值
			if(memcmp((*ite)->chkdata,sec.chkdata,SECURE_SIZE) != 0)//说明文件有更新，需要重新检查
			{
				(*ite)->tag = 2;//有更新，无效数据
				return false;
			}
			else 
			{	
				if((*ite)->tag != 4) // 学习数据不进行更新
					(*ite)->tag = 1;//检查过
			}
			return true;
		}
	}
	return false;
}
//更新到安全缓存文件中
bool CSecurityCache::Flush()
{
	int effectnum;
	CheckUpdate(effectnum);//得到有效数据个数
	if(false == m_isChange)
		return true;

	WCHAR expName[MAX_PATH] ={0};
	ExpandEnvironmentStringsW(m_cathfile, expName, MAX_PATH);
	HANDLE hFile;


	if(effectnum <= 0)
	{
		hFile = CreateFileW(expName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);	
		}
		return true;
	}

	SecCachStruct tp;
	DWORD len = effectnum*(sizeof(tp.filename)+SECURE_SIZE + sizeof(short int));
	unsigned char* pBuffer = new unsigned char[len];

	if(pBuffer == NULL)
		return false;

	unsigned char* cur = pBuffer;
	for(int i = 0; i < SECURE_BUCKET_SIZE; i++)
	{
		for(list<SecCachStruct*>::iterator ite = m_secuBucket[i].begin();ite != m_secuBucket[i].end();ite ++)
		{
			if(((*ite)->tag == 1) || (*ite)->tag == 3 || (*ite)->tag == 4)//有效数据
			{
				memcpy(cur,(void*)(*ite)->filename,sizeof((*ite)->filename));
				cur += sizeof((*ite)->filename);
				memcpy(cur,(void*)(*ite)->chkdata,SECURE_SIZE);
				cur += SECURE_SIZE;
				memcpy(cur,(void*)&((*ite)->tag),sizeof(short int));
				cur += sizeof(short int);
			}
		}
	}

	int packlen = len + 128;
	unsigned char* pPackBuf = new unsigned char[packlen];

	if(pPackBuf == NULL)
	{
		delete[] pBuffer;
		return false;
	}
	
	int ret = CachePack(pBuffer,len,pPackBuf);

	delete[] pBuffer;//先清缓存

	if(ret < 0)
	{
		delete[] pPackBuf;
		return false;
	}
	

	hFile = CreateFileW(expName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		::MessageBoxW(NULL, L"打开安全缓存失败",L"安全缓存",MB_OK | MB_SETFOREGROUND);
		delete[] pPackBuf;
		return false;	
	}

	DWORD dwLength;
	if(!WriteFile(hFile,pPackBuf,ret,&dwLength,NULL))
	{
		::MessageBoxW(NULL, L"写安全缓存失败",L"安全缓存",MB_OK | MB_SETFOREGROUND);
		CloseHandle(hFile);
		delete[] pPackBuf;
		return false;
	}
	CloseHandle(hFile);
	delete[] pPackBuf;
	return true;
}

void CSecurityCache::CheckUpdate(int& effectnum)
{
	effectnum = 0;
	for(int i = 0; i < SECURE_BUCKET_SIZE; i++)
	{
		for(list<SecCachStruct*>::iterator ite = m_secuBucket[i].begin();ite != m_secuBucket[i].end();ite ++)
		{
			if(((*ite)->tag == 0)||((*ite)->tag == 2))
				m_isChange = true;
			else if(((*ite)->tag == 1) || (*ite)->tag == 3 || (*ite)->tag == 4)
				effectnum ++;
		}
	}
}
bool CSecurityCache::GetEigenvalue(ModuleList& mlist)
{
	mlist.clear();
	for(int i = 0; i < SECURE_BUCKET_SIZE; i++)
	{
		for(list<SecCachStruct*>::iterator ite = m_secuBucket[i].begin();ite != m_secuBucket[i].end();ite ++)
		{
			if(((*ite)->tag == 1) || (*ite)->tag == 3 || (*ite)->tag == 4)//有效数据
			{
				ModuleItem item;
				memcpy(item.md,(void*)(*ite)->chkdata,SECURE_SIZE);
				mlist.push_back(item);
			}
		}
	}
	return true;
}

bool CSecurityCache::GetEigenvalue(unsigned char *data,DWORD& length)
{
	// 获得所有的数据
	int effectnum;
	CheckUpdate(effectnum);//得到有效数据个数
	if(false == m_isChange)
		return true;

	if(effectnum <= 0)
	{
		data = NULL;
		length = 0;
		return true;
	}

	length = effectnum * SECURE_SIZE;
	unsigned char* pBuffer = new unsigned char[length];

	if(pBuffer == NULL)
		return false;

	unsigned char* cur = pBuffer;
	for(int i = 0; i < SECURE_BUCKET_SIZE; i++)
	{
		for(list<SecCachStruct*>::iterator ite = m_secuBucket[i].begin();ite != m_secuBucket[i].end();ite ++)
		{
			if(((*ite)->tag == 1) || (*ite)->tag == 3 || (*ite)->tag == 4)//有效数据
			{
				memcpy(cur,(void*)(*ite)->chkdata,SECURE_SIZE);
				cur += SECURE_SIZE;
			}
		}
	}

	data = pBuffer;
	return true;
}

bool CSecurityCache::Clear()
{
	for(int i = 0; i < SECURE_BUCKET_SIZE; i++)
	{
		for(list<SecCachStruct*>::iterator ite = m_secuBucket[i].begin();ite != m_secuBucket[i].end();ite ++)
		{
			delete (*ite);
		}
		m_secuBucket[i].clear();
	}

	if(NULL != evalueCache)
	{
		delete[] evalueCache;
	}
	evalueCache = NULL;

	return true;
}

bool CSecurityCache::IsShouldSend()
{
	return m_isShouldSend;
}

bool CSecurityCache::SetAllDataInvalid()
{
	for(int i = 0; i < SECURE_BUCKET_SIZE; i++)
	{
		for(list<SecCachStruct*>::iterator ite = m_secuBucket[i].begin();ite != m_secuBucket[i].end();ite ++)
		{
			(*ite)->tag = 1;//有效数据
		}
	}
	return true;

}
bool CSecurityCache::SetSend(bool isSend)
{
	m_isShouldSend = isSend;
	return true;
}
bool CSecurityCache::IsChanged()
{
	int effectnum;
	CheckUpdate(effectnum);//得到有效数据个数
	return m_isChange;
}
