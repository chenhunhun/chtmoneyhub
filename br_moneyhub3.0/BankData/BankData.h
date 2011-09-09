
#include <string>
#include <vector>
#include <list>
#include <string>
#include <windows.h>

#include "..\Utils\SQLite\CppSQLite3.h"
#include "..\ThirdParty\json\include\json\json.h"
#include "../ThirdParty/tinyxml/tinyxml.h"
#pragma once
#define MY_BANK_NAME				L"MoneyhubBankData"
#define MY_ERROR_SQL_ERROR		3000
#define MY_ERROR_RUNTIME_ERROR	3001
using namespace std;



typedef struct TransactionRecord
{
	TransactionRecord(){
		memset(TransDate,0,256);
		memset(PostDate,0,256);
		memset(Description,0,256);
		memset(Country,0,256);
		memset(OriginalTransAmount,0,256);
	}
	char	TransDate[256];				//交易日
	char	PostDate[256];				//记账日
	char	Description[256];			//摘要
	float	Amount;						//金额
	int		CardNumber;					//卡号末4位
	char	Country[256];				//交易地点
	char	OriginalTransAmount[256];	//交易地金额，一般没有	
}TRANRECORD, *LPTRANRECORD;

enum BillType
{
	RMB,
	USD
};

struct BillData
{
	string aid;
	string month;
	int type;
	int accountid;
};

typedef struct BillRecord
{
	BillRecord(){
		memset(aid,0,256);
		memset(month,0,256);
	}
	char		aid[256];
	char		month[256];
	int			accountid;
	BillType	type;					//账户类型		中文;美元
	list<LPTRANRECORD> TranList;			//账单
}BILLRECORD, *LPBILLRECORD;

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

	enum CACULATE_TYPE
	{
		emCACULATEWEEK = 1,
		emCACULATEMONTH,
		emCACULATESEASON,
		emCACULATEYEAR,
	};

public:
	static CBankData* GetInstance();
	const char* GetDbPath(std::string strUsID = "");
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
	bool ChangeOrder(const char* pBankID, int nTo, int nFrom); // 用户进行拖动更改收藏顺序

	// 这里是其他c++程序调用的
public:
	bool IsFavExist(std::string& strFav);		// 判断是否存在
	bool OpenDB(std::string strUsID = "");		// 打开用户数据库
	bool ReOpenDB(std::string strUsID = "");	// 重打开用户数据库
	void CloseDB(std::string strUsID = "");		// 关闭用户数据库
	bool CheckUserDB();							// 检验用户数据库
	
	//下面是JS脚本调用的处理
public:

	bool InsertGetBillData(BILLRECORD& TRecord);
	// 收藏夹数据操作
	// 注意调用该函数后请清空lFav
	//void GetFav(std::list<LPFAVRECORD>& lFav);			//要给所有的Fav发送状态
	int DeleteFav(std::string strFav);				//删除收藏夹
	int SaveFav(std::string strFav, int status);	//修改收藏夹
	int GetFav(std::string& strFav);				//这个暂时不动
	int GetFavState(std::string& strFav);
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

	std::string QuerySQL(std::string strSQL, std::string strDBName); // 账单接口
	int ExecuteSQL(std::string strSQL);
	std::string GetXMLData(const std::string& strStartDate, const std::string& strEndDate, const std::string& dataseries, const std::string& charttype);
	
	// 如果lpConditon = NULL，将数据库的整个表拷贝到另一个数据库中，否则只拷贝满足条件的记录(两个表结构一样)
	bool CpyDbTb2OtherDbTb(CppSQLite3DB& dbSour, CppSQLite3DB& dbDesc, LPSTR lpTbName, LPSTR lpDesTbName, LPSTR lpConditon = NULL);

	// （可以满足表的结构不同，指定的列名进行拷贝）
	//bool CpyDbTb2OtherDbTb(const CppSQLite3DB& dbSour, const CppSQLite3DB& dbDesc, PTBCPYNODE pTabName, std::vector<PTBCPYNODE>* pColName,  LPSTR lpCondition = NULL);

protected:

	bool InternalGetFav(std::vector<LPFAVRECORD>& vec);
	bool InternalDeleteFav(std::string strFav);
	bool InternalSaveFav(std::string strFav, int status);
	bool ReadCurrencyByID(int nID, int nID2, double& dValue); // 读取到两币种间的汇率
	bool CreateDataDBFile(LPSTR lpPath);
	int GetDBVersion(CppSQLite3DB& db); // 读取数据库中软件的版本
	bool CopyDataFromOldVersoion(CppSQLite3DB& dbSour, CppSQLite3DB& dbDesc);

	CppSQLite3DB* GetDataDbObject(); // 得到DataDB数据库对象的指针
	CppSQLite3DB* GetSysDbObject(); // 得到SysDB数据库对象的指针(应该将3个获取数据库的方法写成一个方法)

	int InternalAddEvent(LPEVENTRECORD lpEventRec);
	bool InternalDeleteEvent(int id);
	bool InternalGetEvents(int year, int month, std::vector<LPEVENTRECORD>& vecEventRec);
	bool InternalGetEventsOneDay(int year, int month, int day, std::vector<LPEVENTRECORD>& vecEventRec);

	bool InternalSetAlarm(int id, int status, std::string alarmtype);
	bool InternalGetTodayAlarms(std::vector<LPALARMRECORD>& vec);
	
	bool InternalSaveCoupon(int style, std::string id, __time32_t expire, std::string couponname);
	bool InternalDeleteCoupon(int id);
	bool InternalGetCoupons(std::vector<LPCOUPONRECORD>& vecCouponRec);

	std::string InternalQuerySQL(std::string strSQL, std::string strDBName); // 账单接口
	int InternalExecuteSQL(std::string strSQL);
	std::string InternalGetPieXMLData(std::string strStartDate, std::string strEndDate);
	std::string InternalGetNewPieXMLData(std::string strStartDate, std::string strEndDate);
	std::string InternalGetBarXMLData(const std::string& strStartDate, const std::string& strEndDate, const std::string& strKind);

	void ReplaceCharInString(std::string& strBig, const std::string & strsrc, const std::string &strdst); 

	bool GetExchangePartSql(LPCSTR lpStrConditon, std::string& strExchange);

	void SetIndexDoubleAttribute(TiXmlElement * pSet, const char * name, double val );

	// 判断数据是否存在
	bool IsFileExist(LPSTR lpPath);


	// 从源文件转换到目标文件
	//bool TranslateSourceFile2DestinationFile(LPSTR lpSource, LPSTR lpDestination, bool bEncrypt = true);

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
	void InitSysDbTempFile(void); // 初始化系统数据库临时文件

private:
	static CRITICAL_SECTION m_cs;
	void CreateAccountTables(CppSQLite3DB& db); // 账单接口
	std::string m_strUserDbPath; // 存储的是当前用户数据库的全路径
	//std::string m_strUserSourDbPath; // 存储当前用户的数据库源文件路径
	std::string m_strSysDbPath; // 存储系统数据的全路径
	std::string m_strDataDbPath; // 存储DataDB的全路径
	std::string m_strGuestTemplete; // 存储用户模板数据的路径

	CppSQLite3DB m_dbUser;
	CppSQLite3DB m_dbDataDB; // DataDB数据库对象
	CppSQLite3DB m_dbSysDB; // SysDB数据库对象
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