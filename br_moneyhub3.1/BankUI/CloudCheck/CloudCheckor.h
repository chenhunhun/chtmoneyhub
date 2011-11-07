#pragma once
#include ".\ksafesdk\ksafesdk_def.h"
#include ".\ksafesdk\ksafesdk_loader.h"
#include <set>
#include <string>
#include "Windows.h"
#include <Wininet.h>
#pragma comment(lib,"Wininet.lib")
#include "../../Tools/Logger(Unicode)/ILog.h"

using namespace std;
class CProcessShow
{
public:
	CProcessShow(){};
	~CProcessShow(){};
public:
	virtual void Update(int i) = 0;

};

class CCloudCheckor
{
public:
	~CCloudCheckor(void);
private:
	set<wstring>* m_files;
	set<wstring> m_passfiles;
	set<wstring> m_nopassfiles;
	CProcessShow* m_pshow;
	CMoneyhubLog *m_log;

	CCloudCheckor(void);
	static CCloudCheckor* m_checkor;

	//回调函数
	static KSAFE_CALLBACK_RET __cdecl KSafeCallBack (
    KSAFE_EVENT     Event,
    unsigned long   dwParam1,
    unsigned long   dwParam2,
    const wchar_t*  pObjectName,
    const wchar_t*  pVirusName,
    void*           pUserContext);
public:
	static CCloudCheckor* GetCloudCheckor();
	
public:
	//设置要查杀的文件列表
	void SetFiles(set<wstring>* pfiles);
	void SetLog(CMoneyhubLog* log);
	void SetShow(CProcessShow *pshow){m_pshow = pshow;};

	bool Initialize(); 
	bool Uninitialize();

	void SysModuleVerify(set<wstring>* files,set<wstring>* waitfiles);

	bool BeginScanFiles();

	set<wstring>* GetPassFiles();
	set<wstring>* GetUnPassFiles();
	void Clear();
private:
	bool SerialScan();
	bool ParallelScan();

};
