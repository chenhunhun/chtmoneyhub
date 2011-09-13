#include "stdafx.h"
#include <windns.h>
#include <wincrypt.h>
#include "RegMonitor.h"
#include "ConvertBase.h"
#include "StringHelper.h"
#include "../../Utils/APIHook.h"
#include "../../Utils/ListManager/RegData.h"
#include "../../Utils/ListManager/ListManager.h"
#include "../../Utils/HardwareID/genhwid.h"
#include "../../BankUI/UIControl/CoolMessageBox.h"
#include "../../Utils/sn/SNManager.h"
#include "../AxHookManager.h"
#include "../../Utils/RecordProgram/RecordProgram.h"
#include "../../Utils/getOSInfo/GetOSInfo.h"

#define IS_HANDLE_OURS(handle)			(((UINT)(handle) & 0xffff0000) == 0xffff0000)

CRegKeyManager* g_pRegKeyManager = NULL;

CRegKeyManager::CRegKeyManager()
{
	::InitializeCriticalSection(&m_cs);
}
CRegKeyManager::~CRegKeyManager()
{
	::DeleteCriticalSection(&m_cs);
}

//////////////////////////////////////////////////////////////////////////

// 改为所有的进程都要进行注册表核对
LRESULT CRegKeyManager::OnOpenKey(LRESULT lRes, HKEY hRootKey, LPCTSTR lpszSubKey, PHKEY phReturnedKey)
{
	DWORD pid = ::GetCurrentThreadId();
	if (lRes == ERROR_SUCCESS)
	{
		if (lpszSubKey)
		{
			std::wstring strRegPath = lpszSubKey;
			HKEY hPredefinedKey = g_pRegKeyManager->GetRegFullPath(hRootKey, strRegPath);
			if (hPredefinedKey)
			{
				// 遍历所有进程内的注册表
				WebDataMap::iterator webite = CListManager::_()->GetWebDataMap()->begin();
				for(;webite != CListManager::_()->GetWebDataMap()->end();webite ++)
				{
					CWebsiteData* pWebsiteData = webite->second;

					ReqMap::iterator reqite = pWebsiteData->GetReqMap()->begin();
					for(;reqite != pWebsiteData->GetReqMap()->end();reqite ++)
					{
						CRequirement *pRequirement = reqite->second;

						if (pRequirement->GetType() != Require_ActiveX)
							continue;
						const CRegDir *pRegDir = pRequirement->GetRegData()->GetRegDirs(strRegPath.c_str(), hPredefinedKey);
						// 根据已经查找到的事件
						if (pRegDir && pRegDir->ForceReplaceSystemValue())
						{
							// 一旦找到一个合适的requirement，我们就创建一个假的key
							HKEY hVirtualKey = g_pRegKeyManager->CreateNewKey(hPredefinedKey, strRegPath.c_str(), NULL);

							::EnterCriticalSection(&m_cs);
							CListManager::_()->RegQueryLock ();
							// m_VirtualKeyMap表示实际查找过的在实际注册表和每个线程的虚拟注册表里存在的注册表信息
							m_VirtualKeyMap.insert(std::make_pair(hVirtualKey, pRegDir));
							CListManager::_()->RegQueryUnlock (); 
							::LeaveCriticalSection(&m_cs);
							pRegDir->WriteAllItemsToReg(hVirtualKey);
							// 用虚拟替代系统的
							*phReturnedKey = hVirtualKey;

							return ERROR_SUCCESS;
						}
					}

				}
			}
		}


		::EnterCriticalSection(&m_cs);
		CListManager::_()->RegQueryLock ();
		VirtualKeyMap::iterator itv = m_VirtualKeyMap.find(hRootKey);
		if (itv != m_VirtualKeyMap.end())
		{
			// hRootKey已经是假的了，说明我们是在假的目录里面往深了查找
			const CRegDir *pNextReg = itv->second->GetRegDirs(lpszSubKey);
			//ATLASSERT(pNextReg);
			if (pNextReg)
			{
				pNextReg->WriteAllItemsToReg(*phReturnedKey);
				m_VirtualKeyMap.insert(std::make_pair(*phReturnedKey, pNextReg));
				CListManager::_()->RegQueryUnlock ();
				::LeaveCriticalSection(&m_cs);
				return ERROR_SUCCESS;
			}
		}
		CListManager::_()->RegQueryUnlock ();
		::LeaveCriticalSection(&m_cs);

		g_pRegKeyManager->CreateNewKey(hRootKey, lpszSubKey, phReturnedKey ? *phReturnedKey : NULL);
	}
	else if (lRes == ERROR_FILE_NOT_FOUND && phReturnedKey)
	{
		::EnterCriticalSection(&m_cs);
		CListManager::_()->RegQueryLock ();
		VirtualKeyMap::iterator itv = m_VirtualKeyMap.find(hRootKey);
		if (itv != m_VirtualKeyMap.end())
		{
			// hRootKey已经是假的了，说明我们是在假的目录里面往深了查找
			const CRegDir *pNextReg = itv->second->GetRegDirs(lpszSubKey);
			if (pNextReg)
			{
				HKEY hVirtualKey = NULL;
				// 迭代创建虚拟的注册表
				::RegCreateKey(hRootKey, lpszSubKey, &hVirtualKey);
				pNextReg->WriteAllItemsToReg(hVirtualKey);
				m_VirtualKeyMap.insert(std::make_pair(hVirtualKey, pNextReg));
				*phReturnedKey = hVirtualKey;
				CListManager::_()->RegQueryUnlock ();
				::LeaveCriticalSection(&m_cs);
				return ERROR_SUCCESS;
			}
		}

		CListManager::_()->RegQueryUnlock ();
		::LeaveCriticalSection(&m_cs);

		std::wstring strRegPath = lpszSubKey;
		HKEY hPredefinedKey = g_pRegKeyManager->GetRegFullPath(hRootKey, strRegPath);
		if (hPredefinedKey)
		{
			// 遍历所有requirements
			WebDataMap::iterator webite = CListManager::_()->GetWebDataMap()->begin();
			for(;webite != CListManager::_()->GetWebDataMap()->end();webite ++)
			{
				CWebsiteData* pWebsiteData = webite->second;

				ReqMap::iterator reqite = pWebsiteData->GetReqMap()->begin();
				for(;reqite != pWebsiteData->GetReqMap()->end();reqite ++)
				{
					CRequirement *pRequirement = reqite->second;

					if (pRequirement->GetType() != Require_ActiveX)
						continue;
					const CRegDir *pRegDir = pRequirement->GetRegData()->GetRegDirs(strRegPath.c_str(), hPredefinedKey);
					if (pRegDir)
					{
						// 一旦找到一个合适的requirement，我们就创建一个假的key
						HKEY hVirtualKey = g_pRegKeyManager->CreateNewKey(hPredefinedKey, strRegPath.c_str(), NULL);
						::EnterCriticalSection(&m_cs);
						CListManager::_()->RegQueryLock (); // gao 2010-12-16 添加和删除收藏银行，也会操作GetWebDataMap的数据，因此进行互斥
						m_VirtualKeyMap.insert(std::make_pair(hVirtualKey, pRegDir));
						CListManager::_()->RegQueryUnlock (); // gao 2010-12-16 添加和删除收藏银行，也会操作GetWebDataMap的数据，因此进行互斥

						::LeaveCriticalSection(&m_cs);
						pRegDir->WriteAllItemsToReg(hVirtualKey);
						*phReturnedKey = hVirtualKey;

						return ERROR_SUCCESS;
					}
				}


			}
		}

	}

	return lRes;
}

void CRegKeyManager::CloseKey(HKEY hKey)
{
	::EnterCriticalSection(&m_cs);
	CListManager::_()->RegQueryLock ();
	m_HKeyMap.erase(hKey);
	CListManager::_()->RegQueryUnlock ();
	m_VirtualKeyMap.erase(hKey);
	::LeaveCriticalSection(&m_cs);
}

//////////////////////////////////////////////////////////////////////////

// 创建新的键值的函数
HKEY CRegKeyManager::CreateNewKey(HKEY hRootKey, LPCTSTR lpszSubKey, HKEY hSubKey)
{
	std::wstring strSubKey;
	if (((UINT)hRootKey & 0x80000000) == 0)
	{
		// rootkey不是predefined的，需要再次获取一下
		hRootKey = GetRegFullPath(hRootKey, strSubKey);
		if (hRootKey == NULL)
			return NULL;
		if (lpszSubKey)
		{
			strSubKey += _T("\\");
			strSubKey += lpszSubKey;
		}
	}
	else if (lpszSubKey)
		strSubKey = lpszSubKey;
	TrimString(strSubKey, BLANKS_SLASH);

	// 这个注册表键值不存在，需要搞一个虚拟的
	// 在实际中建一个是为了得到HKEY值，既不跟实际的注册表一样，又达到虚拟自己的目的
	if (hSubKey == NULL)
	{
		std::wstringstream strVirtualKey;
		strVirtualKey << _T("Software\\Bank\\VirtualReg\\") << (UINT)hRootKey << _T("\\") << strSubKey;
		::RegCreateKey(HKEY_CURRENT_USER, strVirtualKey.str().c_str(), &hSubKey);
	}
	::EnterCriticalSection(&m_cs);
	m_HKeyMap.insert(std::make_pair(hSubKey, std::make_pair(hRootKey, strSubKey)));
	::LeaveCriticalSection(&m_cs);
	return hSubKey;
}

// 这个函数是根据虚拟的访问的key获得全路径的函数，就是凑成整个一个注册表标准字符串的函数
HKEY CRegKeyManager::GetRegFullPath(HKEY hSubKey, std::wstring &strPath) const
{
	HKEY hRootKey = NULL;
	if ((UINT)hSubKey & 0x80000000)
		return hSubKey;
	::EnterCriticalSection(&m_cs);
	HKeyMap::const_iterator it = m_HKeyMap.find(hSubKey);
	if (it != m_HKeyMap.end())
	{
		hRootKey = it->second.first;
		if (strPath.empty())
			strPath = it->second.second;
		else if (!it->second.second.empty())
			strPath = it->second.second + _T("\\") + strPath;
	}
	::LeaveCriticalSection(&m_cs);
	return hRootKey;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef LONG (WINAPI * RegCreateKeyExAFunc)(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
RegCreateKeyExAFunc OldRegCreateKeyExA = NULL;
LONG WINAPI DetourRegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
{
	// 在这些钩子的地方都是先打开实际的注册表，然后用自己的open去创建虚拟的，也就是说利用递归的方法每次都是先查实际注册表里有的项。
	LONG lRes = OldRegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	return g_pRegKeyManager->OnOpenKey(lRes, hKey, AToW(lpSubKey).c_str(), phkResult);
}
typedef LONG (WINAPI * RegCreateKeyExWFunc)(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
RegCreateKeyExWFunc OldRegCreateKeyExW = NULL;
LONG WINAPI DetourRegCreateKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
{
	LONG lRes = OldRegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	return g_pRegKeyManager->OnOpenKey(lRes, hKey, lpSubKey, phkResult);
}

typedef LONG (WINAPI * RegOpenKeyExAFunc)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
RegOpenKeyExAFunc OldRegOpenKeyExA = NULL;
LONG WINAPI DetourRegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	LONG lRes = OldRegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
	return g_pRegKeyManager->OnOpenKey(lRes, hKey, AToW(lpSubKey).c_str(), phkResult);
}
typedef LONG (WINAPI * RegOpenKeyExWFunc)(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
RegOpenKeyExWFunc OldRegOpenKeyExW = NULL;
LONG WINAPI DetourRegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	LONG lRes = OldRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
	return g_pRegKeyManager->OnOpenKey(lRes, hKey, lpSubKey, phkResult);
}

typedef LONG (WINAPI * RegCloseKeyFunc)(HKEY hKey);
RegCloseKeyFunc OldRegCloseKey = NULL;
LONG WINAPI DetourRegCloseKey(HKEY hKey)
{
	g_pRegKeyManager->CloseKey(hKey);
	return IS_HANDLE_OURS(hKey) ? ERROR_SUCCESS : OldRegCloseKey(hKey);
}

typedef LONG (WINAPI * RegOpenUserClassesRootFunc)(HANDLE hToken, DWORD dwOptions, REGSAM samDesired, PHKEY phkResult);
RegOpenUserClassesRootFunc OldRegOpenUserClassesRoot = NULL;
LONG WINAPI DetourRegOpenUserClassesRoot(HANDLE hToken, DWORD dwOptions, REGSAM samDesired, PHKEY phkResult)
{
	LONG lRes = OldRegOpenUserClassesRoot(hToken, dwOptions, samDesired, phkResult);
	return g_pRegKeyManager->OnOpenKey(lRes, HKEY_CLASSES_ROOT, NULL, phkResult);
}

typedef BOOL (WINAPI * HttpSendRequestAFunc)(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength);
HttpSendRequestAFunc OldHttpSendRequestA = NULL;
BOOL WINAPI DetourHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength)
{
	char szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "MoneyhubUID: %s\r\nSN:%s\r\n", GenHWID2().c_str(),CSNManager::GetInstance()->GetSN().c_str());
	HttpAddRequestHeadersA(hRequest, szBuf, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	return OldHttpSendRequestA(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

typedef BOOL (WINAPI * HttpSendRequestWFunc)(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength);
HttpSendRequestWFunc OldHttpSendRequestW = NULL;
wstring hwid,sn;

BOOL WINAPI DetourHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength)
{	
	USES_CONVERSION;
	// 减少多次转换
	if(hwid == L"")
	{
		wstring thwid(A2W(GenHWID2().c_str()));
		hwid = thwid;
	}
	if(sn == L"")
	{
		wstring tsn(A2W(CSNManager::GetInstance()->GetSN().c_str()));
		sn = tsn;
	}

 	TCHAR szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));
	swprintf_s(szBuf, L"MoneyhubUID: %s\r\nSN:%s\r\n",hwid.c_str(),sn.c_str());
	HttpAddRequestHeadersW(hRequest, szBuf, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE); 

	return OldHttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}


typedef int (WINAPI * MessageBoxAFunc)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
MessageBoxAFunc OldMessageBoxA = NULL;
int WINAPI DetourMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	USES_CONVERSION;
	wstring msg(A2CT(lpText));
	if(msg.find(L"当前安全设置禁止") != wstring::npos)
		return IDOK;
	if(msg.find(L"功能失败") != wstring::npos)
		return IDOK;
	return mhMessageBox(hWnd, A2CT(lpText), A2CT(lpCaption), uType);
}

typedef int (WINAPI * MessageBoxWFunc)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
MessageBoxWFunc OldMessageBoxW = NULL;
int WINAPI DetourMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	USES_CONVERSION;
	wstring msg(W2CT(lpText));
	if(msg.find(L"当前安全设置禁止") != wstring::npos)
		return IDOK;
	if(msg.find(L"功能失败") != wstring::npos)
		return IDOK;
	return mhMessageBox(hWnd, W2CT(lpText), W2CT(lpCaption), uType);
}

typedef int (WINAPI * GetSystemMetricsFunc)(int nIndex);
GetSystemMetricsFunc OldGetSystemMetrics = NULL;
int WINAPI DetourGetSystemMetrics(int nIndex)
{
	if (nIndex == SM_CYCAPTION)
		return DLG_CAPTION_HEIGHT;

	return OldGetSystemMetrics(nIndex);
}

typedef BOOL (WINAPI * SystemParametersInfoFunc)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
SystemParametersInfoFunc OldSystemParametersInfo = NULL;
BOOL WINAPI DetourSystemParametersInfo(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
	BOOL bRet = OldSystemParametersInfo(uiAction, uiParam, pvParam, fWinIni);

	if (uiAction == SPI_GETNONCLIENTMETRICS)
	{
		LPNONCLIENTMETRICS pNCMS = (LPNONCLIENTMETRICS)pvParam;
		pNCMS->iBorderWidth = 0;
		pNCMS->iCaptionHeight = DLG_CAPTION_HEIGHT;
	}

	return bRet;
}

typedef LONG (WINAPI * DnsQuery_AFunc)(PCSTR pszName, WORD wType, DWORD Options, PVOID pExtra, PDNS_RECORD *ppQueryResults, PVOID *pReserved);
DnsQuery_AFunc OldDnsQuery_A = NULL;
LONG WINAPI DetourDnsQuery_A(PCSTR pszName, WORD wType, DWORD Options, PVOID pExtra, PDNS_RECORD *ppQueryResults, PVOID *pReserved)
{
	//Options |= DNS_QUERY_BYPASS_CACHE | DNS_QUERY_NO_HOSTS_FILE;
	Options |= DNS_QUERY_NO_HOSTS_FILE;
	return OldDnsQuery_A(pszName, wType, Options, pExtra, ppQueryResults, pReserved);
}

typedef LONG (WINAPI * DnsQuery_UTF8Func)(PCSTR pszName, WORD wType, DWORD Options, PVOID pExtra, PDNS_RECORD *ppQueryResults, PVOID *pReserved);
DnsQuery_UTF8Func OldDnsQuery_UTF8 = NULL;
LONG WINAPI DetourDnsQuery_UTF8(PCSTR pszName, WORD wType, DWORD Options, PVOID pExtra, PDNS_RECORD *ppQueryResults, PVOID *pReserved)
{
	//Options |= DNS_QUERY_BYPASS_CACHE | DNS_QUERY_NO_HOSTS_FILE;
	Options |= DNS_QUERY_NO_HOSTS_FILE;
	return OldDnsQuery_UTF8(pszName, wType, Options, pExtra, ppQueryResults, pReserved);
}

typedef LONG (WINAPI * DnsQuery_WFunc)(PCWSTR pszName, WORD wType, DWORD Options, PVOID pExtra, PDNS_RECORD *ppQueryResults, PVOID *pReserved);
DnsQuery_WFunc OldDnsQuery_W = NULL;
LONG WINAPI DetourDnsQuery_W(PCWSTR pszName, WORD wType, DWORD Options, PVOID pExtra, PDNS_RECORD *ppQueryResults, PVOID *pReserved)
{
	//Options |= DNS_QUERY_BYPASS_CACHE | DNS_QUERY_NO_HOSTS_FILE;
	Options |= DNS_QUERY_NO_HOSTS_FILE;
	return OldDnsQuery_W(pszName, wType, Options, pExtra, ppQueryResults, pReserved);
}

typedef BOOL (WINAPI * CertVerifyCRLRevocationFunc)(DWORD dwCertEncodingType, PCERT_INFO pCertId, DWORD cCrlInfo, PCRL_INFO rgpCrlInfo[]);
CertVerifyCRLRevocationFunc OldCertVerifyCRLRevocation = NULL;
BOOL WINAPI DetourCertVerifyCRLRevocation(DWORD dwCertEncodingType, PCERT_INFO pCertId, DWORD cCrlInfo, PCRL_INFO rgpCrlInfo[])
{
	return TRUE;
}

typedef BOOL (WINAPI * CertGetCertificateChainFunc)(HCERTCHAINENGINE hChainEngine, PCCERT_CONTEXT pCertContext, LPFILETIME pTime, HCERTSTORE hAdditionalStore, PCERT_CHAIN_PARA pChainPara, DWORD dwFlags, LPVOID pvReserved, PCCERT_CHAIN_CONTEXT* ppChainContext);
CertGetCertificateChainFunc OldCertGetCertificateChain = NULL;
BOOL WINAPI DetourCertGetCertificateChain(HCERTCHAINENGINE hChainEngine, PCCERT_CONTEXT pCertContext, LPFILETIME pTime, HCERTSTORE hAdditionalStore, PCERT_CHAIN_PARA pChainPara, DWORD dwFlags, LPVOID pvReserved, PCCERT_CHAIN_CONTEXT* ppChainContext)
{
	//dwFlags |= CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY;
	//dwFlags &= ~0x04000000;
	dwFlags &= 0x0FFFFFFF;
	return OldCertGetCertificateChain(hChainEngine, pCertContext, pTime, hAdditionalStore, pChainPara, dwFlags, pvReserved, ppChainContext);
}

typedef int (WINAPI * MessageBoxIndirectAFunc)(const LPMSGBOXPARAMSA lpMsgBoxParams);
MessageBoxIndirectAFunc OldMessageBoxIndirectA = NULL;
int WINAPI DetourMessageBoxIndirectA(const LPMSGBOXPARAMSA lpMsgBoxParams)
{
	USES_CONVERSION;
	wstring lpText = A2W(lpMsgBoxParams->lpszText);
	wstring lpCaption = A2W(lpMsgBoxParams->lpszCaption);
	
	return mhMessageBox(lpMsgBoxParams->hwndOwner, lpText.c_str(), lpCaption.c_str(), lpMsgBoxParams->dwStyle);
}

typedef int (WINAPI * MessageBoxIndirectWFunc)(const LPMSGBOXPARAMSW lpMsgBoxParams);
MessageBoxIndirectWFunc OldMessageBoxIndirectW = NULL;
int WINAPI DetourMessageBoxIndirectW(const LPMSGBOXPARAMSW lpMsgBoxParams)
{
	return mhMessageBox(lpMsgBoxParams->hwndOwner, lpMsgBoxParams->lpszText, lpMsgBoxParams->lpszCaption, lpMsgBoxParams->dwStyle);
}

/*typedef HANDLE (WINAPI* CreateThreadFunc)(LPSECURITY_ATTRIBUTES lpThreadAttributes,SIZE_T dwStackSize,LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter,DWORD dwCreationFlags,LPDWORD lpThreadId);
CreateThreadFunc OldCreateThread = NULL;
HANDLE WINAPI DetourCreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,SIZE_T dwStackSize,LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter,DWORD dwCreationFlags,LPDWORD lpThreadId)
{
	CAxHookManager hook;
	HANDLE tHandle = NULL;
	tHandle = OldCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
	if(tHandle != NULL)
	{
		Sleep(200);
		DWORD tid = (DWORD)(*lpThreadId);
		// 在这里马上hook会因为线程还没有创建好导致失败，要等待一小会再进行hook
		hook.Hook(tid);
		
	}

	return tHandle;
}*/

typedef LONG(WINAPI * UnhandledExceptionFilterFunc)(struct _EXCEPTION_POINTERS* ExceptionInfo);
UnhandledExceptionFilterFunc OldUnhandledExceptionFilter = NULL;
LONG WINAPI DetourUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	// 记录用户出错信息
	if (NULL != ExceptionInfo)
	{
		/*LPVOID lpAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;
		DWORD dwEcode = ExceptionInfo->ExceptionRecord->ExceptionCode;
		CString strECode;
		strECode.Format(L"%d", dwEcode);
		std::string strErrCode = CW2A(strECode);*/
		//std::string strAddr = (char*)ExceptionInfo->ExceptionRecord->ExceptionAddress;
		
		DWORD dwAddr = (DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress;
		
		CRecordProgram::GetInstance()->FeedbackError(MY_ERROR_PRO_CORE, ERR_UNHANDLE_EXCEPT, 
			CRecordProgram::GetInstance()->GetRecordInfo(L"UnhandledException Errorcode = %d, ErrAddress = %08x", ExceptionInfo->ExceptionRecord->ExceptionCode, dwAddr));
		exit(0);
		return 0;
		//CUserBehavior::GetInstance()->Action_SendErrorInfo(strErrCode, strAddr);
		//ExceptionInfo->ExceptionRecord->ExceptionInformation();
	}

	return OldUnhandledExceptionFilter(ExceptionInfo);
}

typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI * SetUnhandledExceptionFilterFunc)(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
SetUnhandledExceptionFilterFunc OldSetUnhandledExceptionFilter = NULL;
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI DetourSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return OldSetUnhandledExceptionFilter(DetourUnhandledExceptionFilter);
}

#include "../FilterModuleInX64.h"
void CRegKeyManager::Initialize()
{
	g_pRegKeyManager = new CRegKeyManager();

	HMODULE hKernel32 = ::LoadLibrary(_T("kernel32.dll"));
	if (hKernel32)
	{
		OldSetUnhandledExceptionFilter = (SetUnhandledExceptionFilterFunc)::GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
		OldUnhandledExceptionFilter = (UnhandledExceptionFilterFunc)::GetProcAddress(hKernel32, "UnhandledExceptionFilter");

	}

	if (NULL == ::GetProcAddress(hKernel32, "RegCreateKeyExW"))
		hKernel32 = ::LoadLibrary(_T("advapi32.dll"));
	if (hKernel32)
	{
		OldRegCreateKeyExA = (RegCreateKeyExAFunc)::GetProcAddress(hKernel32, "RegCreateKeyExA");
		OldRegCreateKeyExW = (RegCreateKeyExWFunc)::GetProcAddress(hKernel32, "RegCreateKeyExW");
		OldRegOpenKeyExA = (RegOpenKeyExAFunc)::GetProcAddress(hKernel32, "RegOpenKeyExA");
		OldRegOpenKeyExW = (RegOpenKeyExWFunc)::GetProcAddress(hKernel32, "RegOpenKeyExW");
		OldRegCloseKey = (RegCloseKeyFunc)::GetProcAddress(hKernel32, "RegCloseKey");
		OldRegOpenUserClassesRoot = (RegOpenUserClassesRootFunc)::GetProcAddress(hKernel32, "RegOpenUserClassesRoot");
	}

	HMODULE hWininet = ::LoadLibrary(_T("wininet.dll"));
	if (hWininet)
	{
		OldHttpSendRequestA = (HttpSendRequestAFunc)::GetProcAddress(hWininet, "HttpSendRequestA");
		OldHttpSendRequestW = (HttpSendRequestWFunc)::GetProcAddress(hWininet, "HttpSendRequestW");
	}

	HMODULE hUser32 = ::LoadLibrary(_T("user32.dll"));
	if (hUser32)
	{
		OldMessageBoxIndirectA = (MessageBoxIndirectAFunc)::GetProcAddress(hUser32, "MessageBoxIndirectA");
		OldMessageBoxIndirectW = (MessageBoxIndirectWFunc)::GetProcAddress(hUser32, "MessageBoxIndirectW");
		OldMessageBoxA = (MessageBoxAFunc)::GetProcAddress(hUser32, "MessageBoxA");
		OldMessageBoxW = (MessageBoxWFunc)::GetProcAddress(hUser32, "MessageBoxW");
		OldGetSystemMetrics = (GetSystemMetricsFunc)::GetProcAddress(hUser32, "GetSystemMetrics");
		OldSystemParametersInfo = (SystemParametersInfoFunc)::GetProcAddress(hUser32, "SystemParametersInfo");
	}

	HMODULE hDnsapi = ::LoadLibrary(_T("dnsapi.dll"));
	if (hDnsapi)
	{
		OldDnsQuery_A = (DnsQuery_AFunc)::GetProcAddress(hDnsapi, "DnsQuery_A");
		OldDnsQuery_UTF8 = (DnsQuery_UTF8Func)::GetProcAddress(hDnsapi, "DnsQuery_UTF8");
		OldDnsQuery_W = (DnsQuery_WFunc)::GetProcAddress(hDnsapi, "DnsQuery_W");
	}

	HMODULE hCrypt32 = ::LoadLibrary(_T("crypt32.dll"));
	if (hCrypt32)
	{
		OldCertVerifyCRLRevocation = (CertVerifyCRLRevocationFunc)::GetProcAddress(hCrypt32, "CertVerifyCRLRevocation");
		OldCertGetCertificateChain = (CertGetCertificateChainFunc)::GetProcAddress(hCrypt32, "CertGetCertificateChain");
	}

	LONG lRes = Detours::DetourTransactionBegin();
	ATLASSERT(lRes == NO_ERROR);
	
	lRes = Detours::DetourUpdateThread(::GetCurrentThread());
	ATLASSERT(lRes == NO_ERROR);

	lRes = Detours::DetourAttach((PVOID*)&OldUnhandledExceptionFilter, DetourUnhandledExceptionFilter);
	lRes = Detours::DetourAttach((PVOID*)&OldSetUnhandledExceptionFilter, DetourSetUnhandledExceptionFilter);
	lRes = Detours::DetourAttach((PVOID*)&OldRegCreateKeyExA, DetourRegCreateKeyExA);
	lRes = Detours::DetourAttach((PVOID*)&OldRegCreateKeyExW, DetourRegCreateKeyExW);
	lRes = Detours::DetourAttach((PVOID*)&OldRegOpenKeyExA, DetourRegOpenKeyExA);
	lRes = Detours::DetourAttach((PVOID*)&OldRegOpenKeyExW, DetourRegOpenKeyExW);
	lRes = Detours::DetourAttach((PVOID*)&OldRegCloseKey, DetourRegCloseKey);
	lRes = Detours::DetourAttach((PVOID*)&OldRegOpenUserClassesRoot, DetourRegOpenUserClassesRoot);

	//lRes = Detours::DetourAttach((PVOID*)&OldCreateThread, DetourCreateThread);
	lRes = Detours::DetourAttach((PVOID*)&OldMessageBoxIndirectA, DetourMessageBoxIndirectA);
	lRes = Detours::DetourAttach((PVOID*)&OldMessageBoxIndirectW, DetourMessageBoxIndirectW);
	
	lRes = Detours::DetourAttach((PVOID*)&OldHttpSendRequestA, DetourHttpSendRequestA);
	lRes = Detours::DetourAttach((PVOID*)&OldHttpSendRequestW, DetourHttpSendRequestW);

 	lRes = Detours::DetourAttach((PVOID*)&OldMessageBoxA, DetourMessageBoxA);
 	lRes = Detours::DetourAttach((PVOID*)&OldMessageBoxW, DetourMessageBoxW);
	lRes = Detours::DetourAttach((PVOID*)&OldGetSystemMetrics, DetourGetSystemMetrics);

	lRes = Detours::DetourAttach((PVOID*)&OldDnsQuery_A, DetourDnsQuery_A);
	lRes = Detours::DetourAttach((PVOID*)&OldDnsQuery_UTF8, DetourDnsQuery_UTF8);
	lRes = Detours::DetourAttach((PVOID*)&OldDnsQuery_W, DetourDnsQuery_W);

	lRes = Detours::DetourAttach((PVOID*)&OldCertVerifyCRLRevocation, DetourCertVerifyCRLRevocation);
	lRes = Detours::DetourAttach((PVOID*)&OldCertGetCertificateChain, DetourCertGetCertificateChain);
	lRes = Detours::DetourTransactionCommit();
	
	if(  CGetOSInfo::getInstance()->isX64() )
		g_filterModuleInX64App.InitializeHook();

	ATLASSERT(lRes == NO_ERROR);
}




