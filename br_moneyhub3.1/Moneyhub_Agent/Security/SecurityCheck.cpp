
#include "stdafx.h"
#include "../stdafx.h"
#include "SecurityCheck.h"
#include "../Include/Util.h"
#include "../../Encryption/SHA1/sha.h"
#include "../../Encryption/CHKFile/CHK.h"
#include "ConvertBase.h"
#include "../../Security/BankLoader/BankLoader.h"
#include "../../Security/BankLoader/VerifyCache.h"
#include "../../Security/Authentication/encryption/md5.h"
#include "../../Security/Authentication/ModuleVerifier/export.h"
#include "SysListReader.h"
#include "Security.h"
#include <string>
#include <set>
#include <map>
#include "../../Utils/CloudCheck/CloudFileSelector.h"
#include "../../Utils/CloudCheck/CloudFileSelector.h"
#include "../../Utils/CloudCheck/CloudCheckor.h"
#include "../../Utils/UserBehavior/UserBehavior.h"
#include "../../ThirdParty/RunLog/RunLog.h"
#include "../../ThirdParty/RunLog/LogConst.h"
#include "../Skin/CoolMessageBox.h"
#include "DriverCommunicator.h"
#include "../Skin/ProgressDlg.h"

using namespace std;

#define SECU_DLG_TITLE L"财金汇检测"

CSecurityCheck _SecuCheckPop;
HWND           g_showDLG = NULL;

std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen<= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen -1] = 0;
	std::string strTemp(pszDst);
	delete [] pszDst;
	return strTemp;
}
string ws2s(wstring& inputws){ return WChar2Ansi(inputws.c_str()); }

CSecurityCheck::CSecurityCheck()
: m_lpData(NULL) 
{
	InitializeCriticalSection(&m_cs);
}
CSecurityCheck::~CSecurityCheck()
{
	DeleteCriticalSection(&m_cs);
}
void CSecurityCheck::Start(int bCheckType)
{
	CGlobalData::GetInstance()->Init();//初始化白名单黑名单

	DWORD dw;

	CloseHandle(CreateThread(NULL, 0, _threadCheckAuto, this, 0, &dw)); //发送给驱动的缓存
	CloseHandle(CreateThread(NULL, 0, _threadSelfCommunicate, this, 0, &dw)); //后台扫描
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, _threadShowWaitDLG, NULL, NULL, 0));
}


bool CSecurityCheck::CheckSelfSysList()
{
	std::string strModulePath = CT2A(::GetModulePath());
	std::string strCHK = strModulePath + "\\Config\\syslist.mchk";
	wchar_t message[MSG_BUF_LEN];

	int ret = VerifySysList(strCHK.c_str(), message);
	if (ret < 0)
		return false;
	return true;
}

int CSecurityCheck::VerifyCloudList(const char* lpCHKFileName, wchar_t *message,CCloudFileSelector& cselector)
{
	USES_CONVERSION;

	// 读文件
	HANDLE hFile = CreateFileA(lpCHKFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// 读取文件长度
		DWORD dwLength = GetFileSize(hFile, NULL);
		unsigned char* lpBuffer = new unsigned char[dwLength + 1];

		if (lpBuffer == NULL)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"读CloudCheck内存空间满");
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"ReadFile CloudCheck error");
			delete []lpBuffer;
			return -3002;
		}
		CloseHandle(hFile);

		unsigned char* content = new unsigned char[dwRead];

		if (content == NULL)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"ReadFile CloudCheck 内存空间满");
			delete []lpBuffer;
			return -3001;
		}
		int contentLength = unPackCHK(lpBuffer, dwRead, content);

		delete []lpBuffer;

		if (contentLength < 0)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"ReadFile CloudCheck unPackCHK error");
			return -3003;
		}

		content[contentLength] = '\0';

		CStringA strContent = (char *)content;
		SplitCloudListContent(strContent,cselector);

		delete []content;
		return 0;

	}
	else
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"CreateFile CloudCheck error");
		return -3000;
	}
}

int CSecurityCheck::VerifySysList(const char* lpCHKFileName, wchar_t *message)
{
	USES_CONVERSION;

	// 读文件
	HANDLE hFile = CreateFileA(lpCHKFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// 读取文件长度
		DWORD dwLength = GetFileSize(hFile, NULL);
		unsigned char* lpBuffer = new unsigned char[dwLength + 1];

		if (lpBuffer == NULL)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"ReadFile syslist 内存满");
			return -3001;
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"ReadFile syslist error");
			delete []lpBuffer;
			return -3002;
		}
		CloseHandle(hFile);

		unsigned char* content = new unsigned char[dwRead];

		if (content == NULL)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"syslist 内存满");
			delete []lpBuffer;
			return -3001;
		}

		int contentLength = unPackCHK(lpBuffer, dwRead, content);

		delete []lpBuffer;

		if (contentLength < 0)
		{
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"syslist unPackCHK error");
			return -3003;
		}

		content[contentLength] = '\0';

		CStringA strContent = (char *)content;
		ReadSysList(strContent);

		delete []content;
		return 0;

	}
	else
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_CATHE, L"syslist CreateFile error");
		return -3000;
	}

	return 0;
}

void MakeMD5Str(unsigned char* des, int length, unsigned char* src)
{
	if(length > 100)
		return;

	int z = 0;
	for(;z < length;z ++)
	{
		unsigned char data = src[z];
		des[2*z] = data >> 4;
		des[2*z + 1] = data & 0x0F;
	}
	for(z = 0; z < 2*length; z ++)
	{
		if(des[z] > 9)
			des[z] += 0x37;
		else
			des[z] += 0x30;
	}
}
// 
// DWORD WINAPI CSecurityCheck::_threadCheckAuto(LPVOID lp)
// {
// 	DWORD pid = ::GetCurrentThreadId();
// 	CSecurityCheck* pThis = (CSecurityCheck *)lp;
// 	CDriverCommunicator cd;
// 
// 
// 	// 在安装时的检测将包含该部分	
// 	bool isCatheExist = false;
// 	if (!pThis->CheckSercurityCache(isCatheExist))//获得白名单与黑名单，如果不存在就根据白名单与黑名单生成安全缓存
// 	{
// 		return 0;
// 	}
// 
// 	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"获取黑白名单");
// 
// 	if(cd.CheckDriver() == false)//2分钟内检测驱动状态
// 	{
// 		mhMessageBox(NULL,L"财经汇驱动异常,请稍候启动财金汇!",L"财金汇检测",MB_OK);
// 
// 		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"CheckDriver异常");
// 		return 0;
// 	}
// 
// 
// 	CSecurityCache *pWhiteCache = CGlobalData::GetInstance()->GetWhiteCache();
// 	CSecurityCache *pBlackCache = CGlobalData::GetInstance()->GetBlackCache();
// 
// 	cd.sendData();
// 	cd.SendBlackList();
// 
// 	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"发送黑白名单");
// 
// 	bool CloudStateFlag = true;//记录是否进行了云查杀初始化状态
// 	int reConnectTime = 0;
// 	while( 1 )
// 	{
// 		bool IsSendWhite = false;//判断是否应该重发白名单
// 
// 		Sleep(15 * 1000);//每隔15s中检查一次灰名单和云查杀名单
// 		// 先处理灰名单
// 		if(CGlobalData::GetInstance()->GetGrayList()->size() > 0)
// 		{
// 			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"处理灰名单");
// 
// 			EnterCriticalSection(&pThis->m_cs);
// 			set<wstring> tList,sList;
// 			swap((*CGlobalData::GetInstance()->GetGrayList()),tList);
// 			LeaveCriticalSection(&pThis->m_cs);
// 			//  先判断该部分文件在缓存中存在与否，如果已经在缓存中存在就不更新
// 			set<wstring>::iterator grayite;
// 			for(grayite = tList.begin();grayite != tList.end();grayite ++)
// 			{
// 				SecCachStruct cufile;
// 				memset(cufile.filename, 0, sizeof(cufile.filename));
// 				wcscpy_s(cufile.filename, MAX_PATH, grayite->c_str());
// 				bool re = pWhiteCache->IsInSecurityCache(cufile);//得到是否在Cache中存在
// 				
// 				if(!re)
// 					sList.insert((*grayite));//将不再缓存中的进行检查
// 			}
// 
// 			CCloudCheckor::GetCloudCheckor()->SetShow(NULL);
// 			CCloudCheckor::GetCloudCheckor()->SysModuleVerify(&sList,CGlobalData::GetInstance()->GetWaitList());
// 
// 			// 将肯定已经安全的文件放入安全缓存中。没有经过云查杀的放入待查杀列表中
// 			set<wstring>* ppsfiles = CCloudCheckor::GetCloudCheckor()->GetPassFiles();
// 			if(ppsfiles->size() > 0)
// 			{
// 				IsSendWhite = true;
// 				// 将经过微软认证的文件加到白名单
// 				set<wstring>::iterator ite;
// 				for(ite = ppsfiles->begin(); ite != ppsfiles->end(); ite++)
// 				{
// 					SecCachStruct cufile;
// 					wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
// 					if(!pWhiteCache->CalculEigenvalue(cufile))//计算特征md5值
// 						continue;
// 					pWhiteCache->Add(cufile);
// 
// 					string strfile = ws2s((*ite));
// 					unsigned char md[2*SECURE_SIZE + 1] = {0};
// 					MakeMD5Str(md, SECURE_SIZE, cufile.chkdata);
// 					CUserBehavior::GetInstance()->Action_Study(strfile, (char*)md,kSysModify,kAllow);
// 				}
// 
// 
// 			}
// 
// 			CCloudCheckor::GetCloudCheckor()->Clear();
// 		}
// 
// 
// 		// 处理待查杀列表
// 		if(CGlobalData::GetInstance()->GetWaitList()->size() > 0)//此时如果所有文件都通过了微软认证，不需要调用云查杀了
// 		{
// 			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"云查杀处理灰名单");
// 
// 			if(((CloudStateFlag == false) && (reConnectTime >= 60)) || CloudStateFlag == true)
// 			{
// 				reConnectTime = 0;
// 				pThis->CloudCheck(CGlobalData::GetInstance()->GetWaitList(),CloudStateFlag);
// 
// 
// 				if(CloudStateFlag == false)//
// 				{
// 					CGlobalData::GetInstance()->ShowCloudMessage();
// 					continue;
// 				}
// 
// 				else
// 				{
// 					CGlobalData::GetInstance()->ClearCloudMessage();
// 					CGlobalData::GetInstance()->GetWaitList()->clear();//清空云查杀待查杀列表
// 					set<wstring>* pnpsfiles = CCloudCheckor::GetCloudCheckor()->GetUnPassFiles();
// 					if(pnpsfiles ->size() > 0)
// 					{
// 						// 找到主程序,关闭进程
// 						CGlobalData::GetInstance()->CloseMoneyHub();
// 
// 						set<wstring>::iterator ite;
// 						for(ite = pnpsfiles->begin(); ite != pnpsfiles->end(); ite++)
// 						{
// 							SecCachStruct cufile;
// 							wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
// 							if(!pBlackCache->CalculEigenvalue(cufile))//计算特征md5值
// 								continue;
// 
// 							pBlackCache->Add(cufile,1);
// 
// 							string strfile = ws2s((*ite));
// 
// 							unsigned char md[2*SECURE_SIZE + 1] = {0};
// 							MakeMD5Str(md, SECURE_SIZE, cufile.chkdata);
// 							CUserBehavior::GetInstance()->Action_Study(strfile, (char*)md,kCloudModify,kDeny);
// 						}
// 						pBlackCache->GetEigenvalue(g_blackHashList);
// 						pBlackCache->Flush();//保存黑名单
// 						cd.SendBlackList();//将黑名单发送给驱动
// 						CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"学习后发送黑名单");
// 					}
// 
// 					// 将肯定已经安全的文件放入安全缓存中。没有经过云查杀的放入待查杀列表中
// 					set<wstring>* ppsfiles = CCloudCheckor::GetCloudCheckor()->GetPassFiles();
// 					if(ppsfiles->size() > 0)
// 					{
// 						IsSendWhite = true;
// 						// 将经过微软认证和云查杀的文件加到白名单
// 						set<wstring>::iterator ite;
// 						for(ite = ppsfiles->begin(); ite != ppsfiles->end(); ite++)
// 						{
// 							SecCachStruct cufile;
// 							wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
// 							if(!pWhiteCache->CalculEigenvalue(cufile))//计算特征md5值
// 								continue;
// 							pWhiteCache->Add(cufile);
// 
// 							string strfile = ws2s((*ite));
// 							unsigned char md[2*SECURE_SIZE + 1] = {0};
// 							MakeMD5Str(md, SECURE_SIZE, cufile.chkdata);
// 							CUserBehavior::GetInstance()->Action_Study(strfile, (char*)md,kCloudModify,kAllow);
// 						}
// 					}
// 				}
// 				
// 			}
// 			else
// 				reConnectTime ++;
// 		}
// 		
// 		
// 		if(	IsSendWhite)
// 		{			
// 			pWhiteCache->GetEigenvalue(g_moduleHashList);
// 			pWhiteCache->Flush();
// 
// 			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"学习后发送白名单");
// 
// 			cd.sendData();//发送白名单
// 		}
// 
// 
// 		CCloudCheckor::GetCloudCheckor()->Clear();
// 	}
// 
// 
// 	return 0;
// }
// 
// DWORD WINAPI CSecurityCheck::_threadSelfCommunicate(LPVOID lp)
// {
// 	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"开启通信进程");
// 
// 	DWORD pid = ::GetCurrentThreadId();
// 	CSecurityCheck* pThis = (CSecurityCheck *)lp;
// 	CDriverCommunicator cd;
// 	if(cd.CheckDriver() == false)//2分钟内检测驱动状态
// 	{
// 		mhMessageBox(NULL,L"财经汇驱动异常,请稍候启动财金汇!",L"财金汇检测",MB_OK);
// 
// 		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"CheckDriver异常");
// 		return 0;
// 	}
// 
// 	HANDLE hCommEvent = CreateEvent(NULL, false, false, L"GRAYHANDLE");
// 	if(hCommEvent == NULL)
// 		return 0;
// 	//download event object to device driver, m_hCommDevice is the device object
// 
// 	cd.SendReferenceEvent(hCommEvent);
// 
// 	map<wstring,wstring>* logic = CGlobalData::GetInstance()->GetLogicDosDeviceMap();
// 	map<wstring,wstring>::iterator mite;
// 	while(true)
// 	{
// 		WaitForSingleObject(hCommEvent, INFINITE);
// 		if(hCommEvent == NULL)
// 			return 0;
// 
//    
// 	// 获得灰名单
// 		set<wstring> files;
// 		if(cd.GetGrayFile(files))
// 		{
// 			set<wstring>::iterator ite = files.begin();
// 			for(;ite != files.end();ite ++)
// 			{
// 				size_t pos = 0; 
// 				wstring file = (*ite);
// 				wstring::size_type oldStrLen = file.length(); 
// 				// 从驱动获取的路径需要转换
// 				for(mite = CGlobalData::GetInstance()->GetLogicDosDeviceMap()->begin();mite != CGlobalData::GetInstance()->GetLogicDosDeviceMap()->end();mite ++)
// 				{
// 					size_t pos = 0;
// 					pos = file.find((*mite).second, pos); 
// 					oldStrLen = (*mite).second.length();
// 					if (pos == wstring::npos) continue; 
// 					else
// 					{
// 						file.replace(pos,oldStrLen,(*mite).first);
// 						break;
// 					}
// 
// 				}
// 
// 				transform(file.begin(), file.end(), file.begin(), towupper);
// 
// 				EnterCriticalSection(&pThis->m_cs);
// 				CGlobalData::GetInstance()->GetGrayList()->insert(file);
// 				LeaveCriticalSection(&pThis->m_cs);
// 			}
// 
// 		}
// 
// 	}     
// 
// 
// 	return 0;
// }

//////////////////////new fun
DWORD WINAPI CSecurityCheck::_threadCheckAuto(LPVOID lp)
{
	DWORD pid = ::GetCurrentThreadId();
	CSecurityCheck* pThis = (CSecurityCheck *)lp;
	CDriverCommunicator cd;


	// 在安装时的检测将包含该部分	
	bool isCatheExist = false;
	if (!pThis->CheckSercurityCache(isCatheExist))//获得白名单与黑名单，如果不存在就根据白名单与黑名单生成安全缓存
	{
		return 0;
	}

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"获取黑白名单");

	if(cd.CheckDriver() == false)//2分钟内检测驱动状态
	{
		mhMessageBox(NULL,L"财经汇驱动异常,请稍候启动财金汇!",L"财金汇检测",MB_OK);

		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"CheckDriver异常");
		return 0;
	}

	CSecurityCache *pWhiteCache = CGlobalData::GetInstance()->GetWhiteCache();
	CSecurityCache *pBlackCache = CGlobalData::GetInstance()->GetBlackCache();

	cd.sendData();
	cd.SendBlackList();

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"发送黑白名单");

	while( 1 )
	{
		set<wstring> tList,sList;
		Sleep(15 * 1000);//每隔15s中检查一次灰名单和云查杀名单
		
		// 先处理白名单                                             
		EnterCriticalSection(&pThis->m_cs);
		swap((*CGlobalData::GetInstance()->GetWhiteFileList()),tList);
		CGlobalData::GetInstance()->GetWhiteFileList()->clear();
		LeaveCriticalSection(&pThis->m_cs);

		// 先判断该部分文件在缓存中存在与否，如果已经在缓存中存在就不更新
		set<wstring>::iterator grayite;
		for(grayite = tList.begin();grayite != tList.end();grayite ++)
		{
			SecCachStruct cufile;
			memset(cufile.filename, 0, sizeof(cufile.filename));
			wcscpy_s(cufile.filename, MAX_PATH, grayite->c_str());
			bool re = pWhiteCache->IsInSecurityCache(cufile);//得到是否在Cache中存在

			if(!re)
			{
				sList.insert((*grayite));//将不在缓存中的进行更新
			}
		}

		/**
		*  云查杀返回Unknown的文件也放入白名单中
		*/
		EnterCriticalSection(&pThis->m_cs);
		swap((*CGlobalData::GetInstance()->GetUnKnowFileList()), tList);
		CGlobalData::GetInstance()->GetUnKnowFileList()->clear();
		LeaveCriticalSection(&pThis->m_cs);

		for(grayite = tList.begin();grayite != tList.end();grayite ++)
		{
			SecCachStruct cufile;
			memset(cufile.filename, 0, sizeof(cufile.filename));
			wcscpy_s(cufile.filename, MAX_PATH, grayite->c_str());
			bool re = pWhiteCache->IsInSecurityCache(cufile);//得到是否在Cache中存在

			if(!re)
			{
				sList.insert((*grayite));//将不在缓存中的进行更新
			}
		}

		set<wstring>* ppsfiles = &sList;
		if(ppsfiles->size() > 0)
		{
			set<wstring>::iterator ite;
			for(ite = ppsfiles->begin(); ite != ppsfiles->end(); ite++)
			{
				SecCachStruct cufile;
				wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
				if(!pWhiteCache->CalculEigenvalue(cufile))//计算特征md5值
					continue;
				pWhiteCache->Add(cufile);

				string strfile = ws2s((*ite));
				unsigned char md[2*SECURE_SIZE + 1] = {0};
				MakeMD5Str(md, SECURE_SIZE, cufile.chkdata);
				CUserBehavior::GetInstance()->Action_Study(strfile, (char*)md,kSysModify,kAllow);
			}
			pWhiteCache->GetEigenvalue(g_moduleHashList);
			pWhiteCache->Flush();

			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"学习后发送白名单");
			cd.sendData();//发送白名单
		}

		// 处理黑名单
		if(CGlobalData::GetInstance()->GetBlackFileList()->size() > 0)
		{
			EnterCriticalSection(&pThis->m_cs);
			swap((*CGlobalData::GetInstance()->GetBlackFileList()), tList);
			CGlobalData::GetInstance()->GetBlackFileList()->clear();
			LeaveCriticalSection(&pThis->m_cs);

			set<wstring>* pnpsfiles = &tList;
			if(pnpsfiles ->size() > 0)
			{
				// 找到主程序,关闭进程
				//CGlobalData::GetInstance()->CloseMoneyHub();//发现危险，提示重启财金汇

				set<wstring>::iterator ite;
				for(ite = pnpsfiles->begin(); ite != pnpsfiles->end(); ite++)
				{
					SecCachStruct cufile;
					wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
					
					if(!pBlackCache->CalculEigenvalue(cufile))//计算特征md5值
						continue;

					pBlackCache->Add(cufile,1);

					string strfile = ws2s((*ite));

					unsigned char md[2*SECURE_SIZE + 1] = {0};
					MakeMD5Str(md, SECURE_SIZE, cufile.chkdata);
					CUserBehavior::GetInstance()->Action_Study(strfile, (char*)md,kCloudModify,kDeny);
				}
				pBlackCache->GetEigenvalue(g_blackHashList);
				pBlackCache->Flush();//保存黑名单
				cd.SendBlackList();//将黑名单发送给驱动
				CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"学习后发送黑名单");
			}
		}
	}

	return 0;
}


DWORD WINAPI CSecurityCheck::_threadSelfCommunicate(LPVOID lp)
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"开启通信进程");

	DWORD pid = ::GetCurrentThreadId();
	CSecurityCheck* pThis = (CSecurityCheck *)lp;
	CDriverCommunicator cd;
	if(cd.CheckDriver() == false)//2分钟内检测驱动状态
	{
		mhMessageBox(NULL,L"财经汇驱动异常,请稍候启动财金汇!",L"财金汇检测",MB_OK);

		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_THREAD_DRIVER_COM, L"CheckDriver异常");
		return 0;
	}

	HANDLE hCommEvent = CreateEvent(NULL, false, false, L"GRAYHANDLE");
	if(hCommEvent == NULL)
		return 0;
	//download event object to device driver, m_hCommDevice is the device object

	cd.SendReferenceEvent(hCommEvent);

	if( !CCloudCheckor::GetCloudCheckor()->Initialize() )
		OutputDebugStringW(L"clound initialize is error!");

	map<wstring,wstring>* logic = CGlobalData::GetInstance()->GetLogicDosDeviceMap();
	map<wstring,wstring>::iterator mite;
	while(true)
	{
		WaitForSingleObject(hCommEvent, INFINITE);

		if(hCommEvent == NULL)
			return 0;

		// 获得灰名单
		set<wstring> files;
		if(cd.GetGrayFile(files))
		{
			set<wstring>::iterator ite = files.begin();

			for(;ite != files.end();ite ++)
			{
				std::wstring filePath = (*ite);
				std::wstring::size_type oldStrLen = filePath.length();
				
				for(mite = CGlobalData::GetInstance()->GetLogicDosDeviceMap()->begin();mite != CGlobalData::GetInstance()->GetLogicDosDeviceMap()->end();mite ++)
				{
					size_t pos = 0;
					pos = filePath.find((*mite).second, pos); 
					oldStrLen = (*mite).second.length();
					if (pos == wstring::npos) continue; 
					else
					{
						filePath.replace(pos,oldStrLen,(*mite).first);
						break;
					}
				}

				transform(filePath.begin(), filePath.end(), filePath.begin(), towupper);
				
				//设置超时
				HANDLE hMutex = OpenMutexW(NULL, FALSE, L"UI_SECURITY_MUTEX");
				DWORD er = GetLastError();
				bool bIs = hMutex || ERROR_ACCESS_DENIED == er;

				if( g_showDLG && !bIs)
					PostMessageW(g_showDLG, WM_BEGIN_WAIT_DLG, NULL, NULL);

				bool bScan = CCloudCheckor::GetCloudCheckor()->FileVerify(filePath);
// 				OutputDebugStringW(filePath.c_str());
// 				OutputDebugStringW( bScan ? L"签名验证通过" : L"签名未通过，进行云查杀" );
				KSAFE_RESULT kResult;
				if( !bScan )
					kResult = CCloudCheckor::GetCloudCheckor()->FileScan(filePath);
				
				if( g_showDLG && !bIs )
					PostMessageW(g_showDLG, WM_HIDE_WAIT_DLG, NULL, NULL);
				
				EnterCriticalSection(&pThis->m_cs);
				if( (bScan) || (KSAFE_S_R_SAFE == kResult) )
				{
					bScan = true;
					CGlobalData::GetInstance()->GetWhiteFileList()->insert(filePath);
				}
				else if(   KSAFE_S_R_UNKNOWN == kResult )
				{
					bScan = true;
					CGlobalData::GetInstance()->GetUnKnowFileList()->insert(filePath);
				}
				else
				{
					CGlobalData::GetInstance()->GetBlackFileList()->insert(filePath);
				}
				LeaveCriticalSection(&pThis->m_cs);

				USES_CONVERSION;
				CDriverCommunicator cd;
				cd.SetKernelContinue(bScan);

			}
		}

	}     

	return 0;
}

///////////////////


DWORD WINAPI CSecurityCheck:: _threadCacheCheck(LPVOID lp)
{
	DWORD pid = ::GetCurrentThreadId();
	set<wstring>* pThis = (set<wstring>*)lp;
	if(NULL == lp)
		return 0;

	set<wstring>::iterator ite;
	for(ite = pThis->begin(); ite != pThis->end(); ite++)
	{
		SecCachStruct cufile;
		memset(cufile.filename,0,sizeof(cufile.filename));
		wcscpy_s(cufile.filename,MAX_PATH,ite->c_str());

		bool re = CGlobalData::GetInstance()->GetWhiteCache()->IsInSecurityCache(cufile);//得到是否在Cache中存在
		//无重复写操作，不需要上锁
		if(false == re)
		{
			//如果在缓存中,删除,否则不变,里面顺便过滤掉了不存在和非MZ的文件
			EnterCriticalSection(&_SecuCheckPop.m_cs);
			_SecuCheckPop.m_files.insert(cufile.filename);
			LeaveCriticalSection(&_SecuCheckPop.m_cs);
		}
	}

	return 0;
}
void CSecurityCheck::ReadSysList(const CStringA& strContent)
{	
	ReadSysList_Plus(strContent);
}


void CSecurityCheck::SplitCloudListContent(const CStringA& strContent,CCloudFileSelector& cselector)
{
	USES_CONVERSION;

	int curPos = 0;
	CStringA resToken = strContent.Tokenize("\r\n", curPos);
	while (resToken != "")
	{
		resToken.Trim();

		if (!resToken.IsEmpty() && resToken.GetAt(0) != ';')
		{
			int nPoundKey = resToken.Find(';');
			if (nPoundKey != -1)
				resToken = resToken.Mid(0, nPoundKey);

			resToken.Replace('/', '\\');

			// (1) Java
			if (resToken.GetAt(0) == '@')
			{
				wstring file = CA2W(resToken.Mid(1));
				cselector.AddWhiteList(1,file);
			}
			// (2) Win7/Vista
			else if (resToken.GetAt(0) == '#')
			{
				wstring file = CA2W(resToken.Mid(1));
				cselector.AddWhiteList(2,file);
			}

			// (3) 包含IE其他
			else if (resToken.GetAt(0) == '$')
			{
				wstring file = CA2W(resToken.Mid(1));
				cselector.AddWhiteList(3,file);
			}

			else if (resToken.GetAt(0) == '*')
			{
				CStringA restr = resToken.Mid(1);
				int cPos = 0;
				//获得当前的reg
				wstring reg =  CA2W(restr.Tokenize("+", cPos));
				wstring key = CA2W(restr.Tokenize("+", cPos));
				wstring type = CA2W(restr.Tokenize("+", cPos));
				if(type == L"1")
					cselector.AddRegFolder(reg,key,1);
				else if(type == L"2")
					cselector.AddRegFolder(reg,key,2);
			}

			else if (resToken.GetAt(0) == '&')
			{
				CStringA restr = resToken.Mid(1);
				int cPos = 0;
				//获得当前的reg
				wstring reg =  CA2W(restr.Tokenize("+", cPos));
				wstring key = CA2W(restr.Tokenize("+", cPos));
				wstring type = CA2W(restr.Tokenize("+", cPos));
				wstring file = CA2W(restr.Tokenize("+",cPos));
				if(type == L"1")
					cselector.AddRegFile(reg,key,file,1);
				else if(type == L"2")
					cselector.AddRegFile(reg,key,file,2);
				else if(type == L"3")
					cselector.AddRegFile(reg,key,file,3);
			}

			else if ( resToken.GetAt(0) == '!')
			{
				wstring file = CA2W(resToken.Mid(1));

				WCHAR expName[MAX_PATH] ={0};
				ExpandEnvironmentStringsW(file.c_str(), expName, MAX_PATH);
				wstring wtp(expName);
				cselector.AddFolder(expName);
			}

			else if (resToken.GetAt(0) == '^')
			{
				CStringA restr = resToken.Mid(1);
				int cPos = 0;
				//获得当前的reg
				wstring folder =  CA2W(restr.Tokenize("+", cPos));
				wstring externsion = CA2W(restr.Tokenize("+", cPos));
				cselector.AddExtensionsFile(folder,externsion);					
			}
		}

		resToken = strContent.Tokenize("\r\n", curPos);
	}
}

bool CSecurityCheck::ReBuildSercurityCache()
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_CATHE, L"检验安全缓存");

	CSecurityCache *pWhiteCache = CGlobalData::GetInstance()->GetWhiteCache();
	CSecurityCache *pBlackCache = CGlobalData::GetInstance()->GetBlackCache();

	// 生成白名单
	if(!CheckCache())//收集所有的文件，并且查缓存
		return false;

	// 通过的列表存储在cloudcheckor里面的passfiles里
	CCloudCheckor* pCloudCheckor = CCloudCheckor::GetCloudCheckor();
	pCloudCheckor->SetShow(NULL);
	pCloudCheckor->SysModuleVerify(&m_files,CGlobalData::GetInstance()->GetWaitList());
	m_files.clear();

	bool cFlag = false;//记录是否进行了云查杀
	if(CGlobalData::GetInstance()->GetWaitList()->size() > 0)//此时如果所有文件都通过了微软认证，不需要调用云查杀了
	{
		//进行云查杀
		CloudCheck(CGlobalData::GetInstance()->GetWaitList(),cFlag);
	}


	// 将肯定已经安全的文件放入安全缓存中。没有经过云查杀的放入待查杀列表中
	set<wstring>* ppsfiles = pCloudCheckor->GetPassFiles();

	set<wstring>* pnpsfiles = NULL;
	if(cFlag == true)
	{
		CGlobalData::GetInstance()->GetWaitList()->clear();
		pnpsfiles = pCloudCheckor->GetUnPassFiles();
	}

	if((pnpsfiles != NULL) && (pnpsfiles ->size() >0) )
	{
		set<wstring>::iterator ite;
		for(ite = pnpsfiles->begin(); ite != pnpsfiles->end(); ite++)
		{
			SecCachStruct cufile;
			wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
			if(!pBlackCache->CalculEigenvalue(cufile))//计算特征md5值
				continue;

			pBlackCache->Add(cufile,1);
		}

		pBlackCache->Flush();
	}

	// 将经过微软认证和云查杀的文件加到白名单
	set<wstring>::iterator ite;
	for(ite = ppsfiles->begin(); ite != ppsfiles->end(); ite++)
	{
		SecCachStruct cufile;
		wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
		if(!pWhiteCache->CalculEigenvalue(cufile))//计算特征md5值
			continue;
		pWhiteCache->Add(cufile);
	}

	if(pWhiteCache->IsChanged())
		pWhiteCache->SetSend(true);
	pWhiteCache->GetEigenvalue(g_moduleHashList);
	pWhiteCache->Flush();

	//生成黑名单

	CheckBlackListCache();

	pCloudCheckor->Clear();
	return true;
}

bool CSecurityCheck::CheckSercurityCache(bool& isCacheExist)
{
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_CATHE, L"检验安全缓存");

	CSecurityCache *pWhiteCache = CGlobalData::GetInstance()->GetWhiteCache();
	CSecurityCache *pBlackCache = CGlobalData::GetInstance()->GetBlackCache();
	const int MIN_LIST_NUMBER = 300;
	if(pWhiteCache->GetFileNumber() > MIN_LIST_NUMBER)//如果cache文件被删除，那么重建
	{
		isCacheExist = true;
		pWhiteCache->SetSend(true);
		pWhiteCache->GetEigenvalue(g_moduleHashList);//获得所有的白名单特征值

		pBlackCache->SetSend(true);
		pBlackCache->GetEigenvalue(g_blackHashList);//获得所有的黑名单特征值
		
		return true;
	}
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_CATHE, L"读取安全缓存");

	isCacheExist = false;//缓存不存在，那么开始重新生成缓存

	// 生成白名单
	if(!CheckCache())//收集所有的文件，并且查缓存
		return false;
	
	// 通过的列表存储在cloudcheckor里面的passfiles里
	CCloudCheckor* pCloudCheckor = CCloudCheckor::GetCloudCheckor();
	pCloudCheckor->SetShow(NULL);
	pCloudCheckor->SysModuleVerify(&m_files,CGlobalData::GetInstance()->GetWaitList());
	m_files.clear();

	bool cFlag = false;//记录是否进行了云查杀
	if(CGlobalData::GetInstance()->GetWaitList()->size() > 0)//此时如果所有文件都通过了微软认证，不需要调用云查杀了
	{
		//进行云查杀
		CloudCheck(CGlobalData::GetInstance()->GetWaitList(),cFlag);
	}


	
	// 将肯定已经安全的文件放入安全缓存中。没有经过云查杀的放入待查杀列表中
	set<wstring>* ppsfiles = pCloudCheckor->GetPassFiles();

	set<wstring>* pnpsfiles = NULL;
	if(cFlag == true)
	{
		CGlobalData::GetInstance()->GetWaitList()->clear();
		pnpsfiles = pCloudCheckor->GetUnPassFiles();
	}

	if((pnpsfiles != NULL) && (pnpsfiles ->size() >0) )
	{
		set<wstring>::iterator ite;
		for(ite = pnpsfiles->begin(); ite != pnpsfiles->end(); ite++)
		{
			SecCachStruct cufile;
			wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
			if(!pBlackCache->CalculEigenvalue(cufile))//计算特征md5值
				continue;

			pBlackCache->Add(cufile,1);
		}
	}

	// 将经过微软认证和云查杀的文件加到白名单
	set<wstring>::iterator ite;
	for(ite = ppsfiles->begin(); ite != ppsfiles->end(); ite++)
	{
		SecCachStruct cufile;
		wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
		if(!pWhiteCache->CalculEigenvalue(cufile))//计算特征md5值
			continue;
		pWhiteCache->Add(cufile);
	}

	isCacheExist = true;
	if(pWhiteCache->IsChanged())
		pWhiteCache->SetSend(true);
	pWhiteCache->GetEigenvalue(g_moduleHashList);
	pWhiteCache->Flush();

	//生成黑名单

	CheckBlackListCache();
	pBlackCache->SetSend(true);

	pCloudCheckor->Clear();
	return true;
}

bool CSecurityCheck:: CheckBlackListCache()// 生成黑名单
{
	CSecurityCache *pBlackCache = CGlobalData::GetInstance()->GetBlackCache();
	CCloudFileSelector cfselector;
	std::string strModulePath = CT2A(::GetModulePath());
	std::string strCHK = strModulePath + "\\Config\\BlackList.mchk";
	wchar_t message[MSG_BUF_LEN];

	int ret = VerifyCloudList(strCHK.c_str(), message,cfselector);
	if (ret < 0)
		return false;
	// 检测原有缓存
	// 黑名单中的文件直接生成md5值
	set<wstring>* pcfiles = cfselector.GetFiles();

	set<wstring> blacklist;
	set<wstring>::iterator ite = pcfiles->begin();
	for(; ite != pcfiles->end(); ite++)
	{
		SecCachStruct cufile;
		memset(cufile.filename,0,sizeof(cufile.filename));
		wcscpy_s(cufile.filename,MAX_PATH,ite->c_str());

		bool re = pBlackCache->IsInSecurityCache(cufile);//得到是否在Cache中存在
		if(false == re)
		{
			//如果在缓存中,删除,否则不变,里面顺便过滤掉了不存在和非MZ的文件
			blacklist.insert(cufile.filename);
		}
	}

	if(blacklist.size() > 0)
	{
		set<wstring>::iterator ite = blacklist.begin();
		for(;ite != blacklist.end();ite ++)
		{
			SecCachStruct cufile;
			wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
			if(!pBlackCache->CalculEigenvalue(cufile))//计算特征md5值
				continue;

			pBlackCache->Add(cufile);
		}
	}

	pBlackCache->GetEigenvalue(g_blackHashList);
	if(pBlackCache->IsChanged())
		pBlackCache->Flush();

	return true;
}

bool CSecurityCheck::CheckCache()
{
	if (!CheckSelfSysList())//获得系统文件
	{
		return false;
	}

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_CATHE, L"获得了系统文件");

	//获得所有文件
	CCloudFileSelector cfselector;
	std::string strModulePath = CT2A(::GetModulePath());
	std::string strCHK = strModulePath + "\\Config\\CloudCheck.mchk";
	wchar_t message[MSG_BUF_LEN];

	int ret = VerifyCloudList(strCHK.c_str(), message,cfselector);
	if (ret < 0)
	{
		return false;
	}

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_CATHE, L"获得了云查杀文件");

	// 查找白名单文件
	cfselector.GetAllFiles();
	set<wstring>* pcfiles = cfselector.GetFiles();

	//合并所有文件，节省检测时间
	for(int i = 0; i < (int)g_sysModuleNameList.size(); i++)
	{
		wstring wtp(g_sysModuleNameList[i].GetString());
		transform(wtp.begin(), wtp.end(), wtp.begin(), towupper); //转换大写
		pcfiles->insert(wtp);
	}
	g_sysModuleNameList.clear();//清空list节省内存

	int j = 0;
	m_listnumber =  pcfiles->size();
	if(m_listnumber <= 100)
	{
		mhMessageBox(NULL, _T("检测文件异常！"), SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		return false;
	}
	int nCount = pcfiles->size();
	//多线程查缓存并清除掉非MZ文件
	set<wstring> threadcheck[3];
	HANDLE chkschd[3];
	set<wstring>::iterator ite;
	for(ite = pcfiles->begin();ite != pcfiles->end();ite ++)
	{
		if(j < (int)(m_listnumber/3))
			threadcheck[0].insert((*ite));
		if((j >= (int)(m_listnumber/3)) && (j < (int)(m_listnumber*2/3)))
			threadcheck[1].insert((*ite));
		if(j >= (int)(m_listnumber*2/3))
			threadcheck[2].insert((*ite));	
		j++;
	}


	for(int i = 0; i < 3 ; i ++)
	{
		DWORD dw;
		chkschd[i] = CreateThread(NULL, 0, _threadCacheCheck, (void*)&threadcheck[i], 0, &dw);
	}
	DWORD result = ::WaitForMultipleObjects(3,chkschd,TRUE,600000);


	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_THREAD_CATHE, L"已经检查完所有的白名单");

	if(result == WAIT_TIMEOUT)
	{
		return false;
	}
	cfselector.ClearFiles();
	return true;
}

bool CSecurityCheck::CloudCheck(set<wstring>* files,bool& flag)
{
	bool re;
	flag = false;
	re = CCloudCheckor::GetCloudCheckor()->Initialize();

	CCloudCheckor::GetCloudCheckor()->SetLog(CRunLog::GetInstance()->GetLog());

	if(re == true)
	{
		CCloudCheckor::GetCloudCheckor()->SetFiles(files);
		// 开始云查杀
		CGlobalData::GetInstance()->ShowCloudStatus();
		re = CCloudCheckor::GetCloudCheckor()->BeginScanFiles();
		CGlobalData::GetInstance()->NoShowCloudStatus();
		if(re != true)
		{
		}
		else
			flag = true;//记录进行了云查杀
		// 卸载
		re = CCloudCheckor::GetCloudCheckor()->Uninitialize();
	}
	return re;
}


unsigned int  CSecurityCheck::_threadShowWaitDLG(PVOID lp)
{
	ThreadCacheDC::CreateThreadCacheDC();

	CProgressDlg *dlg = new CProgressDlg();
	dlg->Create(NULL, 0);
	dlg->ShowWindow(SW_HIDE);		
	g_showDLG = dlg->m_hWnd;

	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	ThreadCacheDC::DestroyThreadCacheDC();
	return 1;
}