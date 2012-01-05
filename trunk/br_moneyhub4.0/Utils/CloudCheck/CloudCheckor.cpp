#include "stdafx.h"
#include "CloudCheckor.h"
#include "../../ThirdParty/RunLog/ILog.h"
#include "../../ThirdParty/RunLog/LogConst.h"
#include "../../Security/Authentication/ModuleVerifier/export.h"

CCloudCheckor* CCloudCheckor::m_checkor = NULL;

CCloudCheckor::CCloudCheckor(void)
{
	m_files = NULL;
	m_log = NULL;
}

CCloudCheckor::~CCloudCheckor(void)
{
	m_pshow = NULL;
	m_files = NULL;
	m_passfiles.clear();
	m_nopassfiles.clear();
}
CCloudCheckor* CCloudCheckor::GetCloudCheckor()
{
	if(m_checkor == NULL)
	{
		m_checkor = new CCloudCheckor;
	}
	return m_checkor;
}

void CCloudCheckor::SetFiles(set<wstring>* pfiles)
{
	m_files = pfiles;
}

KSAFE_CALLBACK_RET __cdecl CCloudCheckor::KSafeCallBack(
    KSAFE_EVENT     Event,
    unsigned long   dwParam1,
    unsigned long   dwParam2,
    const wchar_t*  pObjectName,
    const wchar_t*  pVirusName,
    void*           pUserContext)
{
    switch (Event)
    {
    case KSAFE_EVT_OK:
    case KSAFE_EVT_INPROGRESS:
        break;

    case KSAFE_EVT_RESULT:
        {
            if (!pVirusName)    pVirusName  = L"";
            if (!pObjectName)   pObjectName = L"";

            LPCWSTR lpszMessage = L"";
            switch (dwParam1)
            {
            case KSAFE_S_R_SAFE:                lpszMessage = L"[SAFE]";                break;
            case KSAFE_S_R_PENDING:             lpszMessage = L"[PENDING]";             break;
            case KSAFE_S_R_UNKNOWN:             lpszMessage = L"[UNKNOWN]";             break;
            case KSAFE_S_R_DETECTED:            lpszMessage = L"[DETECTED]";            break;
            case KSAFE_S_R_DISINFECTED:         lpszMessage = L"[DISINFECTED]";         break;
            case KSAFE_S_R_DELETED:             lpszMessage = L"[DELETED]";             break;
            case KSAFE_S_R_FAILED_TO_CLEAN:     lpszMessage = L"[FAILED]";              break;
            default:                            lpszMessage = L"[...]";                 break;
            }
        }
        break;

    case KSAFE_EVT_ERROR:
        
        break;

    default:
        break;
    }

    return KSAFE_CLBK_OK;
}   

bool CCloudCheckor::Initialize()
{
	// 增加判断网络状态的处理
	if(m_log)
		m_log->WriteSysLog(LOG_TYPE_INFO,L"CloudCheck-0x00000001-%s",L"测试网络状况");

	if(!(InternetCheckConnection(L"http://www.sohu.com", FLAG_ICC_FORCE_CONNECTION, 0) ||
		InternetCheckConnection(L"http://www.baidu.com", FLAG_ICC_FORCE_CONNECTION, 0) ||
		InternetCheckConnection(L"http://www.sina.com", FLAG_ICC_FORCE_CONNECTION, 0)))
	{
		return false;
	}
	if(m_log)
		m_log->WriteSysLog(LOG_TYPE_INFO,L"CloudCheck-0x00000001-%s",L"加载云模块");

	// 加载引擎模块
	HRESULT hr = ksafeLoadW(NULL);
    if (FAILED(hr))
    {
        return false;
    }

    // 初始化引擎模块
    hr = ksafeInitialize(NULL);
    if (FAILED(hr))
    {
		ksafeUnload();
        return false;
    }
	if(m_log)
		m_log->WriteSysLog(LOG_TYPE_INFO,L"CloudCheck-0x00000001-%s",L"初始化云成功");
	return true;
}

bool CCloudCheckor::Uninitialize()
{
	HRESULT hr = ksafeUninitialize();
	if (FAILED(hr))
    {
        return false;
    }

	hr = ksafeUnload();
	if (FAILED(hr))
    {
        return false;
    }

	//先手工删除云查杀dll自动创建的文件夹
	if(::PathFileExistsW(L"Data"))
	{
		SHFILEOPSTRUCT fos;
		ZeroMemory(&fos, sizeof(fos));
		fos.hwnd = HWND_DESKTOP;
		fos.wFunc = FO_DELETE;
		fos.fFlags = FOF_SILENT|FOF_NOCONFIRMATION|FOF_NOERRORUI;
		fos.pFrom = L"Data";
		SHFileOperation(&fos);
	}
	return true;
}

bool CCloudCheckor::BeginScanFiles()
{
	m_numCVFiles = m_files->size();

	if(m_numCVFiles > 10)
	{
		return ParallelScan();
	}
	else
	  return SerialScan();
}

void CCloudCheckor::SysModuleVerify(set<wstring>* files,set<wstring>* waitfiles)
{
	m_numSVFiles = files->size();
	m_numCurSVFiles = 0;
	m_sysVerifyExceedTime = 0;
	if(m_numSVFiles <= 50 )
	{
		set<wstring>::iterator ite;
		int i = -1;
		for(ite = files->begin();ite != files->end();ite ++)
		{
			i++;
			if(m_pshow)
				m_pshow->Update((i*100)/files->size());

			if(ModuleVerifier::IsSysModuleVerified((*ite).c_str(), true))
			{
				m_passfiles.insert((*ite));
			}
			else
				waitfiles->insert((*ite));
		}
	}
	else
	{
		InitializeCriticalSection(&m_syscs1);
		InitializeCriticalSection(&m_syscs2);
		SVINFO svInfo[4];
		set<wstring> tdfile[4];

		HANDLE chkschd[4];
		set<wstring>::iterator ite;
		int j = 0;
		for(ite = files->begin();ite != files->end();ite ++)
		{
			if(j < (int)(m_numSVFiles/4))
				tdfile[0].insert((*ite));
			if((j >= (int)(m_numSVFiles/4)) && (j < (int)(m_numSVFiles/2)))
				tdfile[1].insert((*ite));
			if(j >= (int)(m_numSVFiles/2) && (j < (int)(m_numSVFiles*3/4)))
				tdfile[2].insert((*ite));	
			if(j >= (int)(m_numSVFiles*3/4))
				tdfile[3].insert((*ite));	
			j ++;
		}

		for(int i = 0; i < 4 ; i ++)
		{
			svInfo[i].pfiles = &tdfile[i];
			svInfo[i].pwaitfiles = waitfiles;
			
			DWORD dw;
			chkschd[i] = CreateThread(NULL, 0, _threadSysVerify, (void*)&svInfo[i], 0, &dw);
		}
		DWORD result = ::WaitForMultipleObjects(4, chkschd, TRUE, 600000);

		DeleteCriticalSection(&m_syscs2);
		DeleteCriticalSection(&m_syscs1);
	}
}
DWORD WINAPI CCloudCheckor::_threadSysVerify(LPVOID lp)
{
	LPSVINFO lpinfo = (LPSVINFO)lp;

	if(lpinfo == NULL)
		return 0;


	CCloudCheckor* pCheckor = CCloudCheckor::GetCloudCheckor();

	set<wstring>::iterator ite;
	int i = -1;

	DWORD dwPre,dwNext,dwSpan;
	bool bRevokeCheck = true;
	for(ite = lpinfo->pfiles->begin();ite != lpinfo->pfiles->end();ite ++)
	{
		pCheckor->m_numCurSVFiles ++;
		if(pCheckor->m_pshow)
			pCheckor->m_pshow->Update((pCheckor->m_numCurSVFiles * 100)/pCheckor->m_numSVFiles);

		if(pCheckor->m_sysVerifyExceedTime > 3)//如果出现超过3次超过5s的查询时间，那么以后的插入将不再进行网络检查
			bRevokeCheck = false;
		dwPre = GetTickCount();
		if(ModuleVerifier::IsSysModuleVerified((*ite).c_str(), bRevokeCheck))
		{
			EnterCriticalSection(&pCheckor->m_syscs1);
			pCheckor->m_passfiles.insert((*ite));
			LeaveCriticalSection(&pCheckor->m_syscs1);
		}
		else
		{
			EnterCriticalSection(&pCheckor->m_syscs2);			
			lpinfo->pwaitfiles->insert((*ite));
			LeaveCriticalSection(&pCheckor->m_syscs2);
		}
		dwNext = GetTickCount();
		dwSpan = dwNext-dwPre;

		if(dwSpan > 5000)
			pCheckor->m_sysVerifyExceedTime ++ ;
	}
	return 0;

}

bool CCloudCheckor::SerialScan()
{
	if(m_files == NULL)
		return true;
	if(m_pshow)
		m_pshow->Update(0);
	HRESULT hr;
	KSAFE_RESULT Result;
	set<wstring>::iterator ite;
	int i = -1;
	int exceedtime = 0;
	DWORD dwPre, dwNext, dwSpan;
	for(ite = m_files->begin();ite != m_files->end();ite ++)
	{
		i++;
		if(m_pshow)
			m_pshow->Update((i*100)/m_files->size());

		dwPre = GetTickCount();

		Result = KSAFE_S_R_SAFE;
		hr = ksafeScanFileW((*ite).c_str(), 0, 0, NULL, &Result);

		dwNext = GetTickCount();
		dwSpan = dwNext - dwPre;

		if(dwSpan > 5000)
			exceedtime ++;

		if(exceedtime >= 3)
		{
			return false;
		}

		if(m_log)
			m_log->WriteSysLog(LOG_TYPE_INFO,L"CloudCheck-0x00000001-%s:%d",(*ite).c_str(),Result);

		if(SUCCEEDED(hr) && Result == KSAFE_S_R_DETECTED)
		{
			m_nopassfiles.insert((*ite));
		}
		else if(SUCCEEDED(hr) && Result == KSAFE_S_R_SAFE)
			m_passfiles.insert((*ite));

	}
	if(m_pshow)
		m_pshow->Update(100);

	return true;
}

set<wstring>* CCloudCheckor::GetPassFiles()
{
	return &m_passfiles;
}
set<wstring>* CCloudCheckor::GetUnPassFiles()
{
	return &m_nopassfiles;
}

DWORD WINAPI CCloudCheckor::_threadCloudVerify(LPVOID lp)
{
	LPSVINFO lpinfo = (LPSVINFO)lp;

	if(lpinfo == NULL)
		return 0;


	CCloudCheckor* pCheckor = CCloudCheckor::GetCloudCheckor();

	DWORD dwPre,dwNext,dwSpan;

	if(pCheckor->m_pshow)
		pCheckor->m_pshow->Update(0);
	HRESULT hr;
	KSAFE_RESULT Result;
	set<wstring>::iterator ite;

	for(ite = lpinfo->pfiles->begin();ite != lpinfo->pfiles->end();ite ++)
	{
		pCheckor->m_numCurCVFiles ++;
		if(pCheckor->m_pshow)
			pCheckor->m_pshow->Update((pCheckor->m_numCurCVFiles*100)/pCheckor->m_numCVFiles);

		dwPre = GetTickCount();

		Result = KSAFE_S_R_SAFE;
		hr = ksafeScanFileW((*ite).c_str(), 0, 0, NULL, &Result);

		dwNext = GetTickCount();
		dwSpan = dwNext - dwPre;

		if(dwSpan > 10000)
			pCheckor->m_cloudVerifyExceedTime ++;

		if(pCheckor->m_cloudVerifyExceedTime >= 3)
		{
			pCheckor->m_resCV = false;
			return 1;
		}

		if(pCheckor->m_log)
			pCheckor->m_log->WriteSysLog(LOG_TYPE_INFO,L"CloudCheck-0x00000001-%s:%d", (*ite).c_str(), Result);

		if(SUCCEEDED(hr) && Result == KSAFE_S_R_DETECTED)
		{
			EnterCriticalSection(&pCheckor->m_syscs3);
			pCheckor->m_nopassfiles.insert((*ite));
			LeaveCriticalSection(&pCheckor->m_syscs3);
		}
		else if(SUCCEEDED(hr) && Result == KSAFE_S_R_SAFE)
		{
			EnterCriticalSection(&pCheckor->m_syscs4);
			pCheckor->m_passfiles.insert((*ite));
			LeaveCriticalSection(&pCheckor->m_syscs4);
		}

	}

	return 0;

}
bool CCloudCheckor::ParallelScan()
{
	m_resCV = true;
	m_numCurCVFiles = 0;
	if(m_sysVerifyExceedTime >= 3)
		m_cloudVerifyExceedTime = 2;
	else
		m_cloudVerifyExceedTime = 3 - m_sysVerifyExceedTime;

	InitializeCriticalSection(&m_syscs3);
	InitializeCriticalSection(&m_syscs4);

	SVINFO svInfo[4];
	set<wstring> tdfile[4];

	HANDLE chkschd[4];
	set<wstring>::iterator ite;
	int j = 0;
	for(ite = m_files->begin();ite != m_files->end();ite ++)
	{
		if(j < (int)(m_numCVFiles/4))
			tdfile[0].insert((*ite));
		if((j >= (int)(m_numCVFiles/4)) && (j < (int)(m_numCVFiles/2)))
			tdfile[1].insert((*ite));
		if(j >= (int)(m_numCVFiles/2) && (j < (int)(m_numCVFiles*3/4)))
			tdfile[2].insert((*ite));	
		if(j >= (int)(m_numCVFiles*3/4))
			tdfile[3].insert((*ite));	
		j++;
	}

	for(int i = 0; i < 4 ; i ++)
	{
		svInfo[i].pfiles = &tdfile[i];
		svInfo[i].pwaitfiles = NULL;

		DWORD dw;
		chkschd[i] = CreateThread(NULL, 0, _threadCloudVerify, (void*)&svInfo[i], 0, &dw);
	}
	DWORD result = ::WaitForMultipleObjects(4, chkschd, TRUE, 600000);

	if(m_pshow)
		m_pshow->Update(100);
	DeleteCriticalSection(&m_syscs4);
	DeleteCriticalSection(&m_syscs3);

	return m_resCV;
}

void CCloudCheckor::Clear()
{
	m_pshow = NULL;
	m_files = NULL;
	m_passfiles.clear();
	m_nopassfiles.clear();
}
void CCloudCheckor::SetLog(CMoneyhubLog* log)
{
	m_log = log;
}


KSAFE_RESULT CCloudCheckor::FileScan(std::wstring filePath)
{
	KSAFE_RESULT Result = KSAFE_S_R_SAFE;//未联网状态下认为该文件安全
	HRESULT hr = ksafeScanFileW(filePath.c_str(), 0, 0, NULL, &Result);

	// 	KSAFE_S_R_SAFE              = 0,        ///< 安全
	// 		KSAFE_S_R_PENDING           = 1,        ///< 用于异步模式(暂未开放)
	// 
	// 		KSAFE_S_R_UNKNOWN           = 100,      ///< 未知文件(无法判断为安全的文件)
	// 
	// 		KSAFE_S_R_DETECTED          = 1000,     ///< 发现是病毒

	if( !SUCCEEDED(hr))
		OutputDebugStringW(L"scan is error!");

	return Result;
}


bool CCloudCheckor::FileVerify(std::wstring filePath, bool bRevokeCheck)
{
	bool bReturn = false;

	if( ModuleVerifier::IsSysModuleVerified( filePath.c_str(), bRevokeCheck) )
		bReturn = true;

	return bReturn;	
}