
#include <string>
#include <vector>
#include <list>
#include <string>
#include <windows.h>

#include "..\Utils\SQLite\CppSQLite3.h"
#include "..\Utils\jsoncpp\include\json\json.h"
#pragma once
#define MY_BANK_NAME				L"MoneyhubBankData"
#define MY_ERROR_SQL_ERROR		3000
#define MY_ERROR_RUNTIME_ERROR	3001
typedef struct tagEventRecord
{
	int id;
	__time32_t event_date;
	__time32_t start_date;
	char description[256];
	int repeat;
	int alarm;
	int status;

} EVENTRECORD, *LPEVENTRECORD;

typedef struct tagCouponRecord
{
	int id;
	__time32_t expire;

} COUPONRECORD, *LPCOUPONRECORD;

typedef struct tagAlarmRecord
{
	int type;	// 0 event, 1 coupon
	int id;
	__time32_t event_date;
	char description[256];
	int status;

} ALARMRECORD, *LPALARMRECORD;

typedef struct tagFavRecord
{
	char fav[256];	
	int status; // =0表示没安装银行控件，=100表示成功安装控件
	int favorder;
	bool bdelete;
} FAVRECORD, *LPFAVRECORD;

typedef struct tagUSBKeyRecord
{
	USHORT  vid;		// 添加vid
	USHORT  pid;		// 添加pid
	DWORD	mid;		// 如果没有mid填0
	char	fav[5];		// 金融机构id
	char	xml[512];	// 查询版本及获得下载地址的xml
	char	bkurl[256]; // 备用下载地址
	char	ver[16];	// 已经安装的版本，如果没安装过填1.0.0.0，否则取2.0.0.0
	int		status;		// 表示下载状态，0-99表示下载过程，100表示下载，200表示下载安装完
} USBRECORD, *LPUSBRECORD;

class CBankData
{
friend class ObjectLock;

public:
	CBankData();
	~CBankData();

public:
	static CBankData* GetInstance();
	static const char* GetDbPath();
	static const char* GetCouponPath();

	void RemoveDatabase();

public:
	bool DeleteUSB(int vid, int pid, DWORD mid);
	int AddUSB(int vid, int pid, DWORD mid, std::string fav, std::string xml,std::string bkurl, std::string ver,int status);
	int UpdateUSB(int vid, int pid, DWORD mid, std::string fav, std::string xml);
	int UpdateUSB(int vid, int pid, DWORD mid, int status);
	int UpdateUSB(int vid, int pid, DWORD mid, const std::string& strVer);
	bool IsUsbExist(int vid, int pid, DWORD mid = 0);
	bool GetAllUsb(std::vector<LPUSBRECORD>& vec);
	//bool GetNotSetupUsbKey(std::vector<LPUSBRECORD>& vec);
	bool GetAUSBRecord(int vid, int pid, DWORD mid, USBRECORD& record);
	bool ChangeOrder(const char* pBankID, int nTo, int nFrom); // 拖动收藏顺序

	// 这里是其他c++程序调用的
public:
	bool IsFavExist(std::string& strFav);		//判断是否存在
	
	//下面是JS脚本调用的处理
public:
	// 收藏夹数据操作
	// 注意调用该函数后请清空lFav
	//void GetFav(std::list<LPFAVRECORD>& lFav);			//要给所有的Fav发送状态
	int DeleteFav(std::string strFav);				//删除收藏夹
	int SaveFav(std::string strFav, int status);	//修改收藏夹
	int GetFav(std::string& strFav);				//这个暂时不动
	int GetFav(std::list<std::string>& strFav, bool bNoInstall = true); // 得到没有安装但已经收藏的银行控件ID

	// 用户密码操作
	void SavePwd(std::string strPwd);
	std::string GetPwd();

	int AddEvent(std::string event);
	int DeleteEvent(int id);
	int GetEvents(int year, int month, std::string& events);
	int GetEventsOneDay(int year, int month, int day, std::string& events);

	int SetAlarm(std::string alarm);
	std::string GetTodayAlarms();
    int GetTodayAlarmsNumber();
	
	int SaveCoupon(int style, std::string id, std::string expire, std::string couponname);
	int DeleteCoupon(int id);
	std::string GetCoupons();

protected:

	bool InternalGetFav(std::vector<LPFAVRECORD>& vec);
	bool InternalDeleteFav(std::string strFav);
	bool InternalSaveFav(std::string strFav, int status);


	int InternalAddEvent(LPEVENTRECORD lpEventRec);
	bool InternalDeleteEvent(int id);
	bool InternalGetEvents(int year, int month, std::vector<LPEVENTRECORD>& vecEventRec);
	bool InternalGetEventsOneDay(int year, int month, int day, std::vector<LPEVENTRECORD>& vecEventRec);

	bool InternalSetAlarm(int id, int status, std::string alarmtype);
	bool InternalGetTodayAlarms(std::vector<LPALARMRECORD>& vec);
	
	bool InternalSaveCoupon(int style, std::string id, __time32_t expire, std::string couponname);
	bool InternalDeleteCoupon(int id);
	bool InternalGetCoupons(std::vector<LPCOUPONRECORD>& vecCouponRec);

	void TryResetAlarmStatus(__time32_t tToday);

	std::string ToSQLSafeString(std::string strQuery);
	std::string ToSQLUnsafeString(std::string strQuery);
	std::string ToStrTime(__time32_t t);
	__time32_t  ToIntTime(std::string t);
	
	bool IsAlarmEvent(__time32_t today, __time32_t event_date, int repeat1, int alarm, __time32_t* pAlarmDate = NULL);
	bool NotLeapYear(int year);
	void AdjustTimeValue(struct tm* _tm);

	bool IsMonthShowEvent(__time32_t tHead, __time32_t tTail, __time32_t event_date, int repeat1, std::vector<__time32_t>* pvecShowDates = NULL);

	bool IsValidEventJson(Json::Value& root);
	bool IsValidAlarmJson(Json::Value& root);

	bool DownloadCouponImages(int style, std::string sn, int id);
	bool DeleteCouponImages(int id);
	void split(std::string& s, std::string& delim,std::vector< std::string >* ret);

private:
	static CRITICAL_SECTION m_cs;
};

class ObjectLock
{
public:
	ObjectLock(CBankData* pData)
	{
		m_pBankData = pData;
		EnterCriticalSection(&m_pBankData->m_cs);
	}

	~ObjectLock()
	{
		LeaveCriticalSection(&m_pBankData->m_cs);
	}

private:
	CBankData* m_pBankData;
};