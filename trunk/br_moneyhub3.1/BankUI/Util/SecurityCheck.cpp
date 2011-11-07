#include "stdafx.h"
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

#include <string>
#include <set>

#include "../ThirdParty/RunLog/Runlog.h"
#include "../ThirdParty/RunLog/LogConst.h"

#include "../Util/DriverCommunicator.h"
#include "../UIControl/SecuCheckDlg.h"
#include "../UIControl/CoolMessageBox.h"

#include "../Utils/ListManager/ListManager.h"
#include "../Utils/FavBankOperator/FavBankOperator.h"
#include "../Utils/UserBehavior/UserBehavior.h"


#include <map>
using namespace std;

CSecurityCheck _SecuCheck;
int CSecurityCheck::ProcessPos = 0;
int CSecurityCheck::ProcessHelp = 0;
bool IsInstall = false;

CSecurityCache g_WhiteCache;
CSecurityCache g_BlackCache;

CSecurityCheck::CSecurityCheck()
: m_fnCheckEventFunc(NULL), m_lpData(NULL)
{
}

void CSecurityCheck::Start(int bCheckType)
{
	DWORD dw;
	if (bCheckType == 0)
	{
		IsInstall = true;
		CloseHandle(CreateThread(NULL, 0, _threadInstallCheck, this, 0, &dw));//安装时的检查
	}
	else if(bCheckType == 1)
	{
		IsInstall = false;
		CloseHandle(CreateThread(NULL, 0, _threadCheck, this, 0, &dw));// 2秒钟检查
	}
}
//////////////////////////////////////////////////////////////////////////
//

bool CSecurityCheck::CheckSelfModules()
{
	USES_CONVERSION;

	std::string strModulePath = CT2A(::GetModulePath());
	std::string strCHK = strModulePath + "\\Authen.mchk";
	wchar_t message[MSG_BUF_LEN];

	//Event(CHK_SECURITYCHECK, 5, NULL);

	int ret = VerifySelfModules(strCHK.c_str(), message);
	if (ret < 0)
	{

		//authen，反馈
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_ERROR_ID_CHECK4, MY_ERROR_DESCRIPT_CHECK4);

		Event(CHK_SECURITYCHECK, (DWORD)ret, message, strChkAuthenErr.c_str());
		return false;
	}

	//Event(CHK_SECURITYCHECK, 10, NULL);

	return true;
}

int CSecurityCheck::VerifySelfModules(const char* lpCHKFileName, wchar_t *message)
{
	// 读文件
	HANDLE hFile = CreateFileA(lpCHKFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// 读取文件长度
		DWORD dwLength = GetFileSize(hFile, NULL);
		unsigned char* lpBuffer = new unsigned char[dwLength + 1];

		if (lpBuffer == NULL)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"读程序模块白名单失败");
			return -3002;
		}
		CloseHandle(hFile);

		unsigned char* content = new unsigned char[dwRead];

		if (content == NULL)
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		int contentLength = unPackCHK(lpBuffer, dwRead, content);

		delete []lpBuffer;

		if (contentLength < 0)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
			return -3003;
		}

		delete []content;
		return 0;

	}
	else
	{
		wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
		return -3000;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// by adam
int CSecurityCheck::CheckMoneyHubList(unsigned char *buffer, int length, const wchar_t *path, wchar_t *message)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	char fileName[1024];
	unsigned char md[20];
	SHA_CTX	c;
	int bufferLength;
	unsigned char *ptr;
	int pos;

	USES_CONVERSION;

	ptr=buffer;
	bufferLength=ptr[0]|(ptr[1]<<8)|(ptr[2]<<16)|(ptr[3]<<24);

	if (bufferLength+4!=length)
	{
		wcscpy_s(message, MSG_BUF_LEN, L"检验信息格式错误");
		return -3010;
	}
	ptr+=4;
	pos=4;
	while (pos<length)
	{
		std::wstring strFile = path;

		bufferLength=ptr[0]|(ptr[1]<<8);
		if (pos+2+bufferLength>length)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"检验信息格式错误");
			return -3011;
		}
		ptr=ptr+2;
		pos=pos+2;

		memcpy(fileName,ptr,bufferLength);
		fileName[bufferLength]=0;
		ptr+=bufferLength;
		pos+=bufferLength;

		strFile =strFile + A2W(fileName);

		hFile = CreateFile(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			// 读取文件长度
			DWORD dwLength = GetFileSize(hFile, NULL);
			char* lpBuffer = new char[dwLength + 1];

			if(lpBuffer==NULL)
			{
				wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
				return -3001;
			}

			DWORD dwRead = 0;
			if(!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
			{
				delete []lpBuffer;
				swprintf(message, MSG_BUF_LEN, L"读文件[%s]失败", strFile.c_str());
				return -3002;
			}

			CloseHandle(hFile);

			SHA1_Init(&c);
			SHA1_Update(&c,(const void *)lpBuffer,dwRead);
			SHA1_Final(md, &c);

			delete []lpBuffer;

			if (memcmp(md,ptr,20)!=0)
			{
				swprintf(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
				return -3012;
			}

			ptr+=20;
			pos+=20;

			//Event(CHK_SECURITYCHECK, 5 * pos / length, strFile.c_str());//占用5个百分点
			//Sleep(50);
		} 
		else 
		{
			swprintf_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
			return -3013;
		}
	}
	if (pos<length)
	{
		wcscpy_s(message, MSG_BUF_LEN, L"检验信息格式错误");
		return -3014;
	}

	wcscpy_s(message, MSG_BUF_LEN, L"成功");
	return 0;
}

// by adam
int CSecurityCheck::VerifyMoneyHubList(const char *path, const char* CHKFileName, wchar_t *message)
{
	USES_CONVERSION;

	// 读文件
	HANDLE hFile = CreateFileA(CHKFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// 读取文件长度
		DWORD dwLength = GetFileSize(hFile, NULL);
		unsigned char* lpBuffer = new unsigned char[dwLength + 1];

		if (lpBuffer == NULL)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"读财金汇软件的完整性信息失败");
			return -3002;
		}
		CloseHandle(hFile);

		unsigned char* content = new unsigned char[dwRead];

		if (content == NULL)
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		int contentLength = unPackCHK(lpBuffer, dwRead, content);

		delete []lpBuffer;

		if (contentLength < 0)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
			return -3003;
		}

		int ret = CheckMoneyHubList(content,contentLength, A2W(path),message);

		delete []content;
		return ret;

	}
	else
	{
		wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
		return -3000;
	}
}

//////////////////////////////////////////////////////////////////////////
//
bool CSecurityCheck::CheckBankDataFiles(LPSTR path,wchar_t * message)
{
	std::string dir = path;
	dir += "\\*.*";

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFileA(dir.c_str(), &FindFileData);

	if(INVALID_HANDLE_VALUE == hFind)
		return false;
	do{
		std::string fn = FindFileData.cFileName;

		if ((strcmp(fn.c_str(), ".") != 0) && (strcmp(fn.c_str(), "..") != 0) && (strcmp(fn.c_str(), ".svn") != 0) )
			//&& (_tcsicmp(fn.c_str(), "info.xml") != 0) && (_tcsicmp(fn.c_str(), "syslist.txt") != 0) && (_tcsicmp(fn.c_str(), "CloudCheck.ini") != 0))
		{
			// 子目录
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string subDir = path;
				subDir = subDir + "\\" + fn;
				std::string filePath = path;
				filePath = filePath + "\\" + fn + "\\" + "bank.mchk" ;

				// 校验该银行是否收藏，没有收藏时跳过
				std::string strBkID = CFavBankOperator::GetBankIDOrBankName(fn, false);
				CFavBankOperator* pOperPointer = CListManager::_()->GetFavBankOper ();
				ATLASSERT (NULL != pOperPointer);
				if (!pOperPointer->IsInFavBankSet (strBkID))
					continue;

				int ret = VerifyMoneyHubList(subDir.c_str(), filePath.c_str(), message);

				if(ret < 0)
				{
					CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, L"校验如下银行数据出错");
					USES_CONVERSION;
					CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_COMMON_ERROR, A2W (filePath.c_str ()));

					FindClose(hFind);
					return false;
				}
			}
			else 
			{
			
			}
		}
	}while (FindNextFileA(hFind, &FindFileData) != 0);

	FindClose(hFind);

	return true;
}

bool CSecurityCheck::CheckSelfDataFiles( bool bCheckBank, bool bThreadCheck)
{
	USES_CONVERSION;

	ProcessPos += 5;
	if(IsInstall)
		Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);
	std::string strModulePath = CT2A(::GetModulePath());
	std::string strCHK = strModulePath + "\\MoneyHub.mchk";
	wchar_t message[MSG_BUF_LEN];

	int ret = VerifyMoneyHubList(strModulePath.c_str(), strCHK.c_str(), message);

	ProcessPos += 5;
	if(IsInstall)
		Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);

	if ( ret < 0 )
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_ERROR_ID_CHECK, MY_ERROR_DESCRIPT_CHECK);

		if(!bThreadCheck)
			Event(CHK_SECURITYCHECK, (DWORD)ret, message, strChkMonyHubErr.c_str());
		return false;
	}

	//验证Html和Config完整性
	std::vector <std::string> vecDirectoryName;
	vecDirectoryName.push_back("Html");
	vecDirectoryName.push_back("Config");
	int size = vecDirectoryName.size();


	for(int i = 0; i < size; i++)
	{
		std::string strDPath = strModulePath;
		strDPath += "\\" ;
		strDPath += vecDirectoryName[i];

		std::string strDChk = strDPath;
		strDChk += "\\";
		_strlwr_s((LPSTR)vecDirectoryName[i].c_str(), vecDirectoryName[i].size ()+1);
		strDChk += vecDirectoryName[i].c_str();
		strDChk += ".mchk";
		//MessageBoxA(NULL,strDPath.c_str(),strDChk.c_str(),MB_OK);
		
		ret = VerifyMoneyHubList(strDPath.c_str(), strDChk.c_str(), message);
		
		if( ret < 0 )
		{

			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_ERROR_ID_CHECK2, MY_ERROR_DESCRIPT_CHECK2);

			if(!bThreadCheck)
				Event(CHK_SECURITYCHECK, (DWORD)ret, message, strChkCfDatErr.c_str());
			return false;
		}
	}

	////验证银行控件完整性
	if( bCheckBank )
	{
		std::string strBankPath =strModulePath ;//getAppdataPath();//; 
		strBankPath += "\\BankInfo\\banks";
		if( false == CheckBankDataFiles((LPSTR)strBankPath.c_str() ,message) ) // 银行控件不返回失败，如果损坏，直接重新下载
		{

			//moneyhubchk，反馈
			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_ERROR_ID_CHECK3, MY_ERROR_DESCRIPT_CHECK3);

			 ret = -3002;
			 if(!bThreadCheck)
				Event(CHK_SECURITYCHECK, (DWORD)ret, message, strChkBkDatErr.c_str());
			return false;
		}
		ProcessPos += 10;//20
		 if(IsInstall && !bThreadCheck)
			Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);
	}
	////

	return true;
}

//////////////////////////////////////////////////////////////////////////
//

bool CSecurityCheck::CheckSelfUrlList()
{
	std::string strModulePath = CT2A(::GetModulePath());//; getAppdataPath()
	std::string strCHK = strModulePath + "\\BankInfo\\banks\\main\\info.mchk";
	wchar_t message[MSG_BUF_LEN];

	ProcessPos ++;//21
	if(IsInstall)
		Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);
	//Event(CHK_SELFMODULES, 11, NULL);
	int ret = VerifyUrlList(strCHK.c_str(), message);
	if (ret < 0)
	{

		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_ERROR_ID_CHECK5, MY_ERROR_DESCRIPT_CHECK5);

		Event(CHK_SELFURLLIST, (DWORD)ret, message, strChkUrlDatErr.c_str());
		return false;
	}

	return true;
}

int CSecurityCheck::VerifyUrlList(const char* lpCHKFileName, wchar_t *message)
{
	// 读文件
	HANDLE hFile = CreateFileA(lpCHKFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// 读取文件长度
		DWORD dwLength = GetFileSize(hFile, NULL);
		unsigned char* lpBuffer = new unsigned char[dwLength + 1];

		if (lpBuffer == NULL)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"读URL白名单失败");
			return -3002;
		}
		CloseHandle(hFile);

		unsigned char* content = new unsigned char[dwRead];

		if (content == NULL)
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		int contentLength = unPackCHK(lpBuffer, dwRead, content);
		delete []lpBuffer;

		if (contentLength < 0)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
			return -3003;
		}

		delete []content;
		return 0;

	}
	else
	{
		wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
		return -3000;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//

bool CSecurityCheck::CheckSelfSysList()
{
	std::string strModulePath = CT2A(::GetModulePath());
	std::string strCHK = strModulePath + "\\Config\\syslist.mchk";
	wchar_t message[MSG_BUF_LEN];

	//Event(CHK_SELFSYSLIST, 0, NULL);

	int ret = VerifySysList(strCHK.c_str(), message);
	if (ret < 0)
	{
		Event(CHK_SELFSYSLIST, (DWORD)ret, message, strChkCfDatErr.c_str());
		return false;
	}

	//Event(CHK_SELFSYSLIST, 100, NULL);

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
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"读名单失败");
			return -3002;
		}
		CloseHandle(hFile);

		unsigned char* content = new unsigned char[dwRead];

		if (content == NULL)
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		int contentLength = unPackCHK(lpBuffer, dwRead, content);

		delete []lpBuffer;

		if (contentLength < 0)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
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
		wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
		return -3000;
	}

	return 0;
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
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		DWORD dwRead = 0;
		if (!ReadFile(hFile, lpBuffer, dwLength, &dwRead, NULL))
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"读系统模块白名单失败");
			return -3002;
		}
		CloseHandle(hFile);

		unsigned char* content = new unsigned char[dwRead];

		if (content == NULL)
		{
			delete []lpBuffer;
			wcscpy_s(message, MSG_BUF_LEN, L"内存空间满");
			return -3001;
		}

		int contentLength = unPackCHK(lpBuffer, dwRead, content);

		delete []lpBuffer;

		if (contentLength < 0)
		{
			wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
			return -3003;
		}

		//////////////////////////////////////////////////////////////////////////
		// g_sysModuleNameList

		content[contentLength] = '\0';

		CStringA strContent = (char *)content;
		ReadSysList(strContent);

		//////////////////////////////////////////////////////////////////////////

		delete []content;
		return 0;

	}
	else
	{
		wcscpy_s(message, MSG_BUF_LEN, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描");
		return -3000;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//

bool CSecurityCheck::CheckDriverStatus()
{
	//Event(CHK_DRVIMAGEPATH, 0, NULL);

	if (!CDriverLoader::CheckDriverImagePath(false))
	{
		//Driver，反馈
		CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_ERROR_ID_DRIVER, MY_ERROR_DESCRIPT_DRIVER);

		const wchar_t message[] = L"安全驱动被篡改，程序中止运行";
		Event(CHK_DRVIMAGEPATH, (DWORD)-1000, message, strChkDriverErr.c_str());
		return false;
	}

	//Event(CHK_DRVIMAGEPATH, 100, NULL);
	//ProcessPos = 99;//99
	//Event(CHK_SECURITYCHECK, ProcessPos, NULL);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//

bool CSecurityCheck::ExtraCheckGdiplus()
{
	HMODULE hModule = LoadLibraryA("gdiplus.dll");
	if (hModule != NULL)
	{
		TCHAR szModulePath[MAX_PATH + 1];
		GetModuleFileName(hModule, szModulePath, MAX_PATH);
		if (!ModuleVerifier::IsSysModuleVerified(szModulePath))
		{
			//gdiplus，反馈
			CRecordProgram::GetInstance()->RecordWarnInfo(MY_PRO_NAME, MY_ERROR_ID_GDIPLUS, MY_ERROR_DESCRIPT_GDIPLUS);

			//mhMessageBox(NULL, _T("发现Windows补丁版本较低，建议运行Windows更新程序升级。"), SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		}

		FreeLibrary(hModule);
	}

	ProcessPos = 100;//22
	if(IsInstall)
		Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);
	return true;
}

//////////////////////////////////////////////////////////////////////////


void CSecurityCheck::SetEventsFunc(CHECKEVENTFUNC pfn, LPVOID lpData)
{
	m_fnCheckEventFunc = pfn;
	m_lpData = lpData;
}

void CSecurityCheck::Event(CheckStateEvent cse, DWORD prog_or_error, const wchar_t* lpszInfo, const char* lpszErrNum)
{
	if (m_fnCheckEventFunc)
		m_fnCheckEventFunc(cse, prog_or_error, lpszInfo, m_lpData);
	else
	{
		// 如果有出错信息，弹出提示框
		if (((int)prog_or_error) < 0)
		{
			if(lpszInfo != NULL)
				mhMessageBox(NULL, lpszInfo, SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);

			// 发送出错信息到服务器端
			if (NULL != lpszInfo && NULL != lpszErrNum)
			{
				wstring strErrNum = CA2W(lpszErrNum);				
				CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_COMMON_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"检测错误%s-%s", strErrNum.c_str (), lpszInfo));
			}
		}
	}

}
#ifdef SINGLE_PROCESS
#define MY_DEBUG
#endif
DWORD WINAPI CSecurityCheck::_threadCheck(LPVOID lp)
{

	CSecurityCheck* pThis = (CSecurityCheck *)lp;
	CDriverCommunicator cd;

	pThis->Event(CHK_START, 0, NULL, NULL);

#ifndef MY_DEBUG
	if (!pThis->CheckSelfDataFiles(false)) // 不进行银行控件检查
		return 0;

	if (!pThis->CheckSelfModules())//
		return 0;

	if (!pThis->CheckSelfUrlList())//
		return 0;


	if (!pThis->CheckDriverStatus())
		return 0;

 	if (!pThis->ExtraCheckGdiplus())
 		return 0;
#endif
	pThis->Event(CHK_END, 100, NULL, NULL);


	CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"退出检测");

	return 0;
}

DWORD WINAPI CSecurityCheck::_threadInstallCheck(LPVOID lp)
{
	CSecurityCheck* pThis = (CSecurityCheck *)lp;

	pThis->Event(CHK_START, 0, NULL, NULL);

	if (!pThis->CheckSelfDataFiles(false)) // 不进行银行控件检查
		return 0;

	if (!pThis->CheckSelfModules())//
		return 0;

	if (!pThis->CheckSelfUrlList())//
		return 0;

	g_WhiteCache.SetCacheFileName(L"%Appdata%\\Moneyhub\\WhiteCache.dat");
	g_WhiteCache.Init();
	g_BlackCache.SetCacheFileName(L"%Appdata%\\Moneyhub\\BlackCache.dat");
	g_BlackCache.Init();

	if (!pThis->CheckWhiteListCache())//生成白名单缓存
	{
		pThis->Event(CHK_ERROR, 0, NULL, NULL);
		return 0;
	}

	if (!pThis->CheckBlackListCache())//生成黑名单缓存
	{
		pThis->Event(CHK_ERROR, 0, NULL, NULL);
		return 0;
	}

	g_WhiteCache.Clear();
	g_BlackCache.Clear();

	if (!pThis->CheckDriverStatus())
		return 0;

	if (!pThis->ExtraCheckGdiplus())
		return 0;

	pThis->Event(CHK_END, 0, NULL, NULL);


	CRecordProgram::GetInstance()->RecordDebugInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"退出安装检测");

	return 0;
}

bool CSecurityCheck:: CheckBlackListCache()// 生成黑名单
{
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

		bool re = g_BlackCache.IsInSecurityCache(cufile);//得到是否在Cache中存在
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
			if(!g_BlackCache.CalculEigenvalue(cufile))//计算特征md5值
				continue;

			g_BlackCache.Add(cufile);
		}
	}

	g_BlackCache.GetEigenvalue(g_blackHashList);
	if(g_BlackCache.IsChanged())
		g_BlackCache.Flush();

	return true;
}
DWORD WINAPI CSecurityCheck:: _threadCacheCheck(LPVOID lp)
{
	set<wstring>* pThis = (set<wstring>*)lp;
	if(NULL == lp)
		return 0;

	set<wstring>::iterator ite;
	for(ite = pThis->begin(); ite != pThis->end(); ite++)
	{
		SecCachStruct cufile;
		memset(cufile.filename,0,sizeof(cufile.filename));
		wcscpy_s(cufile.filename,MAX_PATH,ite->c_str());

		bool re = g_WhiteCache.IsInSecurityCache(cufile);//得到是否在Cache中存在
		//无重复写操作，不需要上锁
		if(false == re)
		{
			//如果在缓存中,删除,否则不变,里面顺便过滤掉了不存在和非MZ的文件
			EnterCriticalSection(&_SecuCheck.m_cs);
			_SecuCheck.m_files.insert(cufile.filename);
			LeaveCriticalSection(&_SecuCheck.m_cs);
		}
		//上不上锁无所谓了，因为本来就是显示总数
		_SecuCheck.m_nowcheck ++;
		if(((int)(_SecuCheck.m_nowcheck * 30 / _SecuCheck.m_listnumber)) > ProcessHelp)
		{
			ProcessHelp ++;
			ProcessPos += 1;//98
			_SecuCheck.Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);
		}
	}

	return 0;
}
void CSecurityCheck::ReadSysList(const CStringA& strContent)
{	
	ReadSysList_Plus(strContent);
}

//add by fanzhenxing

void CSecurityCheck::Update(int i)
{
	if((i*3/10) > ProcessHelp)
	{
		ProcessHelp ++;
		ProcessPos += 1;//42
		Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);
	}
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

bool CSecurityCheck::CheckWhiteListCache()
{	
	if(!CheckCache())//收集所有的文件，并且查缓存
		return false;

	set<wstring> waitlist;//待查杀列表
	//waitlist为未通过微软签名的列表	
	//通过的列表存储在cloudcheckor里面的passfiles里
	CCloudCheckor::GetCloudCheckor()->SetShow((CProcessShow*)this);
	ProcessHelp = 1;
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"Begin SysModuleVerify");
	CCloudCheckor::GetCloudCheckor()->SysModuleVerify(&m_files,&waitlist);

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"SysModuleVerify Finish");

	m_files.clear();

	bool cFlag = false;//记录是否进行了云查杀
	if(waitlist.size() > 0)//此时如果所有文件都通过了微软认证，不需要调用云查杀了
	{
		//进行云查杀
		CloudCheck(&waitlist,cFlag);
	}
	else
	{
		ProcessPos += 30;//92
		Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);
	}

	//将肯定已经安全的文件放入安全缓存中。没有经过云查杀的放入待查杀列表中
	set<wstring>* ppsfiles = CCloudCheckor::GetCloudCheckor()->GetPassFiles();
	set<wstring>* pnpsfiles = NULL;
	if(cFlag == true)
		pnpsfiles = CCloudCheckor::GetCloudCheckor()->GetUnPassFiles();

	if((pnpsfiles != NULL) && (pnpsfiles ->size() >0) )
	{
		mhMessageBox(NULL, _T("经过云查杀，系统内可能存在安全风险，建议您进行全盘杀毒扫描！"), SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"系统存在安全风险");

		//将没有通过的放入黑名单
		set<wstring>::iterator ite;
		for(ite = pnpsfiles->begin(); ite != pnpsfiles->end(); ite++)
		{
			SecCachStruct cufile;
			wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
			if(!g_BlackCache.CalculEigenvalue(cufile))//计算特征md5值
				continue;

			g_BlackCache.Add(cufile,1);
		}
	}

	// 生成hash表
	int k = 0;
	std::set<wstring>::size_type cal = 1; // gao

	// 将经过微软认证和云查杀的文件加到白名单
	set<wstring>::iterator ite;
	for(ite = ppsfiles->begin(); ite != ppsfiles->end(); ite++)
	{
		k++;
		SecCachStruct cufile;
		wcscpy_s(cufile.filename,MAX_PATH,(*ite).c_str());
		if(!g_WhiteCache.CalculEigenvalue(cufile))//计算特征md5值
			continue;

		g_WhiteCache.Add(cufile);

		if((k * 6 / ppsfiles->size()) > cal)
		{
			cal ++;
			ProcessPos += 1;//98
			Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);
		}	
	}

	//在安装的时候没有待查杀

	ProcessPos = 100;//22
	Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);

	if(g_WhiteCache.IsChanged())
		g_WhiteCache.SetSend(true);
	g_WhiteCache.GetEigenvalue(g_moduleHashList);
	g_WhiteCache.Flush();

	CCloudCheckor::GetCloudCheckor()->Clear();

	return true;
}

bool CSecurityCheck::CheckCache()
{
	if (!CheckSelfSysList())//获得系统文件
		return false;
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

	// 查找云查杀文件
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
		mhMessageBox(NULL, _T("检测文件异常，请重试！"), SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		return false;
	}
	m_nowcheck = 0;
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

	InitializeCriticalSection(&m_cs);
	ProcessHelp = 1;
	for(int i = 0; i < 3 ; i ++)
	{
		DWORD dw;
		chkschd[i] = CreateThread(NULL, 0, _threadCacheCheck, (void*)&threadcheck[i], 0, &dw);
	}
	DWORD result = ::WaitForMultipleObjects(3,chkschd,TRUE,600000);

	DeleteCriticalSection(&m_cs);


	if(result == WAIT_TIMEOUT)
	{
		mhMessageBox(NULL, _T("检测时间异常，请重试！"), SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		return false;
	}
	cfselector.ClearFiles();
	ProcessPos += 1;//41
	Event(CHK_SECURITYCHECK, ProcessPos, NULL, NULL);

	return true;

}
bool CSecurityCheck::CloudCheck(set<wstring>* files,bool& flag)
{
	bool re;
	flag = false;
	//Vista权限问题
	CCloudCheckor::GetCloudCheckor()->SetLog(CRunLog::GetInstance()->GetLog());
	re = CCloudCheckor::GetCloudCheckor()->Initialize();
	if(re != true)
	{
		//mhMessageBox(NULL, _T("初始化失败，请检查网络，可能会影响财金汇运行"), SECU_DLG_TITLE, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
	}	
	//云查杀初始化成功
	else
	{
		CCloudCheckor::GetCloudCheckor()->SetFiles(files);
		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"开始云查杀");
		
		// 开始云查杀
		ProcessHelp = 1;
		re = CCloudCheckor::GetCloudCheckor()->BeginScanFiles();		

		CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"结束云查杀");

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
