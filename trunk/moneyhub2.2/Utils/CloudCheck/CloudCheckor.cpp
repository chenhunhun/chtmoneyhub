#include "stdafx.h"
#include "CloudCheckor.h"
#include "../RunLog/ILog.h"
#include "../RunLog/LogConst.h"
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
	return SerialScan();
}

void CCloudCheckor::SysModuleVerify(set<wstring>* files,set<wstring>* waitfiles)
{
	set<wstring>::iterator ite;
	int i = -1;
	for(ite = files->begin();ite != files->end();ite ++)
	{
		i++;
		if(m_pshow)
			m_pshow->Update((i*100)/files->size());

		if(ModuleVerifier::IsSysModuleVerified((*ite).c_str()))
		{
			m_passfiles.insert((*ite));
		}
		else
			waitfiles->insert((*ite));
	}
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
	for(ite = m_files->begin();ite != m_files->end();ite ++)
	{
		i++;
		if(m_pshow)
			m_pshow->Update((i*100)/m_files->size());

		DWORD dwPre = GetTickCount();

		Result = KSAFE_S_R_SAFE;
		hr = ksafeScanFileW((*ite).c_str(), 0, 0, NULL, &Result);

		DWORD dwNext = GetTickCount();
		DWORD dwSpan=dwNext-dwPre;

		if(dwSpan > 20000)
			exceedtime ++;

		if(exceedtime >= 3)
		{
			return false;
		}

		if(m_log)
			m_log->WriteSysLog(LOG_TYPE_INFO,L"CloudCheck-0x00000001-%s:%d",(*ite).c_str(),Result);

		if(FAILED(hr) || Result != KSAFE_S_R_SAFE)
		{
			m_nopassfiles.insert((*ite));
		}
		if(Result == KSAFE_S_R_SAFE)
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

bool CCloudCheckor::ParallelScan()
{
	return true;
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