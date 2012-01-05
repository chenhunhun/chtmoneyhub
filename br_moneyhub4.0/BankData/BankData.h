
#include <string>
#include <vector>
#include <list>
#include <string>
#include <windows.h>

#include "..\Utils\SQLite\CppSQLite3.h"
#include "..\ThirdParty\json\include\json\json.h"
#include "../ThirdParty/tinyxml/tinyxml.h"
#include "../Include/Message.h"
#pragma once
#define MY_BANK_NAME					L"MoneyhubBankData"
#define MY_ERROR_SQL_ERROR				3000
#define MY_ERROR_RUNTIME_ERROR			3001

#define BILL_CANCEL_GET_BILL			2000		// 取消导入账单
#define BILL_EXCEED_MAX_TIME			2001		// 超时
#define BILL_NORMAL_STATE				0			// login success
#define BILL_COM_ERROR					1000		// 其他失败
#define BILL_GET_ACCOUNT_ERROR			1001		// 获取账户失败
#define BILL_INNER_CANCEL				3000		// 内部取消收藏，要关闭显示正在登陆，同时关闭页面
#define BILL_LOGIN_SUCC		         	3			// login success
#define BILL_LOAD_DLG					10			// 显示正在登录账户界面
#define BILL_SELECT_ACCOUNT				20			// 选择账户
#define BILL_SELECT_ACCOUNT_MONTH2		21			// 弹出账户选择后又弹出月份选择
#define BILL_SELECT_MONTH				30			// 选择月份
#define BILL_SELECT_MONTH2				31			// 第二种选择月份的方法
#define BLII_NEED_RESTART				33			// 选错了账号需要重新选取
#define BILL_DOWNLOAD_DLG				40			// 显示正在下载界面
#define BILL_RECORD_NUMBER				50			// 显示已下载记录条数界面
#define BILL_BROWSER_GO		    		81			// continue getbill 
#define BILL_BROWSER_LOOP		    	83			// loop getbill
#define BILL_FINISH_STATE				99			// 一个账单抓取完成
#define BILL_ALL_FINISH					100			// 将所有的账单抓取完成

#define WM_BILL_CHANGE_NOFIFY			WM_USER + 0x100F //不是跨进程的消息！！
#define WM_BILL_HIDE_NOFIFY				WM_USER + 0x100E //隐藏对话框的问题
#define WM_BILL_SHOW_NOFIFY				WM_USER + 0x100D //显示对话框
using namespace std;

#define CHECKBOX_SHOW_CHECKED			0x00000010 // 显示时以选中的状态显示
#define CHECKBOX_SHOW_UNCHECKED			0x00000100 // 显示时以没有选中的状态显示
#define CHECKBOX_SHOW_CHECKED_BEFORE	0x00001000 // 显示时显示为已经导入

typedef struct SELECTINFONODE // 选择账户和选择月份的时候用
{
	SELECTINFONODE()
	{
		memset(szNodeInfo, 0, 256);
		dwVal = 0;
	}
	char	szNodeInfo[256];
	DWORD	dwVal;
}SELECTINFONODE, *PSELECTINFONODE;

typedef struct TransactionRecord
{
	TransactionRecord(){
		memset(TransDate,0,256);
		memset(PostDate,0,256);
		memset(Description,0,256);
		memset(Amount,0,256);
		memset(Country,0,256);
		memset(OriginalTransAmount,0,256);
		memset(Payee,0,256);
	}
	char	TransDate[256];				//交易日
	char    Payee[256];					//支付对象
	char	PostDate[256];				//记账日
	char	Description[256];			//摘要
	char	Amount[256];				//金额
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
	BillData()
	{
		memset(select, 0, 256);
		type = 0;
		accountid = 0;
	}
	string				aid;					// 金融机构的aid
	char				select[256];			// 选择的时间或者账户
	string				tag;					// 金融账户的标记，账号末四位
	int					type;					// 表明账户类型的
	INT64				accountid;				// 要导入的账户id
	bool				isSel;					// 初始化表明是否已经选择了对象	
};
typedef struct MonthBillRecord
{
	string				month;					// 导入的月份
	bool				m_isSuccess;			// 判断账单是否导入成功
	list<LPTRANRECORD>	TranList;				// 账单
}MONTHBILLRECORD,*LPMONTHBILLRECORD;

typedef struct BillRecord
{
	string				balance;				// 余额，失败的话为“F”
	BillType			type;					// 账户类型		中文;美元
	list<LPMONTHBILLRECORD> bills;				// 分月或者时间记载的账单
}BILLRECORD, *LPBILLRECORD;

typedef struct BillRecords
{
	BillRecords()
	{
		memset(aid, 0, 256);
		memset(tag, 0, 256);
	}
	char				aid[256];					// 机构id
	char				tag[256];					// 实际得到的账号标记tag
	int					type;						// 账户的类型，信用卡or借记卡
	INT64				accountid;					// 要导入的账户，如果为后选择，那么该值不管
	bool				isFinish;					// 账户是否导入完成
	list<LPBILLRECORD>	BillRecordlist;				// 账单
	list<SELECTINFONODE> m_mapBack;				// 导出账单中间获取的数据(多个账号或者是月份,bool值表示是否选中)
	//dll申请的内存统一由该接口进行处理！！！，否则有内存问题
}BILLRECORDS, *LPBILLRECORDS;

typedef struct tagEventRecord
{
	INT64 id;
	INT64 UT;
	__time32_t event_date;
	__time32_t start_date;
	char description[256];
	int repeat;
	int alarm;
	int status;
	string tbAccountid;
	string tbSubaccountid;
	int type;
} EVENTRECORD, *LPEVENTRECORD;


typedef struct tagAlarmRecord
{
	int type;	// 0 event, 1 coupon
	INT64 id;
	INT64 UT;
	__time32_t event_date;
	char description[256];
	int status;
	string tbAccountid;
	string tbSubaccountid;
	int ntype;
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

//////////////////////////////////////////////////////////////////////////
// Login Reg Syn Data

struct UDData
{
	string       iTableNum;
	string       lUT;
	string       idbver;
	string       imark;
	int       iupdown;      //have updata (1 up) ( 2 down)( 3 same)   no insert( 0 null down )(4 up)  (待处理6 insert 7 update)
	string	  strdata;
};
typedef std::map<std::string, UDData> UserIDMap;

struct tfield
{
	int       ifieldNum;
	std::map<int , std::string> m_field; // 
};
typedef std::map<std::string, tfield> TableField; // 

// 用户登录的状态
typedef enum USER_LOAD_STATE
{
	emNotLoad, // 未登录
//	emLoading, // 正在登录
	emUserNet, // 网络登录
	emUserLocal, // 本地登录
}USERSTATUS;

// 设置页面的状态
typedef enum USER_CHANGE_STATUS
{
	emUserNoChange, // 用户未登录
	emUserChangePwd, // 用户修改密码
	emUserChangeMail, // 用修改邮箱
	emUserLoad, // 用户已经登录
}CHANGESTATUS;

typedef struct USERINFO
{
	USERINFO()
	{
		struserid = MONHUB_GUEST_USERID;
		emChangeStatus = emUserNoChange;
		emUserStatus = emNotLoad;
	}
	string       strmail;
	string       strstoken;
	string       struserid;
	CHANGESTATUS emChangeStatus;
	USERSTATUS	 emUserStatus; // 用户状态（网络登录，未登录，本地登录）

}USERINFO;
//svr所用的查询的账户信息记录

typedef struct datUserEvent
{
	wstring account; //账户名称
	int iNotify; //通知数量
	int iRemind; //提醒数量
	bool bRemind; //是否提示
	long rTime; //最近的时间
}DATUSEREVENT,*LPDATUSEREVENT;
//
enum eGetIdMode
{
	egSingle,
	egBatch
};
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
	USERINFO m_CurUserInfo; // 当前用户信息
	static CBankData* GetInstance();
	const char* GetDbPath(std::string strUsID = "");

	void RemoveDatabase();
	string FilterStringNumber(string& scr);

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
	bool IsCurrentDbNeedSynchro();				// 检验同步标记
	
	//下面是JS脚本调用的处理
public:
	
	bool IsMonthImport(const char* pKeyInfo, const char* pMonth, INT64 nAccountID); 
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

	INT64 AddEvent(std::string event);
	int DeleteEvent(INT64 id);
	int DeleteAllEvent(); //新增删除所有时间的接口
	int GetEvents(int year, int month, std::string& events);
	int GetEventsOneDay(int year, int month, int day, std::string& events);

	int SetAlarm(std::string alarm);
	std::string GetTodayAlarms();
	//下面是Svc调用的接口
    int GetAllUserTodayAlarmsNumber(vector<DATUSEREVENT>& tEvent);
	void SetUserAlarmsConfig(vector<DATUSEREVENT>& tEvent);
	void SetCurrency(string info);//设置
	void SetUserAutoLogin(wstring mail);


	void SetGetIDMode(eGetIdMode eMode);//设置获得id的模式，单一和批量的
	int m_initID;//
	eGetIdMode m_eGetIDMode;
	INT64 GetNewID();
	INT64 GetNewUT();
	
	std::string QuerySQL(std::string strSQL, std::string strDBName); // 账单接口
	int ExecuteSQL(std::string strSQL, std::string strDBName = "");
	std::string GetXMLData(const std::string& strStartDate, const std::string& strEndDate, const std::string& dataseries, const std::string& charttype);
	
	// 如果lpConditon = NULL，将数据库的整个表拷贝到另一个数据库中，否则只拷贝满足条件的记录(两个表结构一样)
	bool CpyDbTb2OtherDbTb(CppSQLite3DB& dbSour, CppSQLite3DB& dbDesc, LPSTR lpTbName, LPSTR lpDesTbName, LPSTR lpConditon = NULL);

	bool InstallUpdateDB();
	bool UpdateDB7to8(wstring wFilePath, string sUtfFilePath); // 3.1库到4.0版本的升级函数

	// 从模板库创建一个新的用户库
	bool CreateNewUserDB(LPSTR lpstrUserDBName, LPSTR lpPassword, int nLen);

	// 从访客库创建一个新的用户库
	void CreateUserDbFromGuestDb(LPSTR lpstrUserDBName, LPSTR lpPassword, int nLen);

	// 将用户设置为当前用户
	bool SetCurrentUserDB(LPSTR lpstrUserDBName, LPSTR lpPassword, int nLen);

	// 通过UserID删除用户数据库
	bool DeleteUserDbByID(const char* pUserID, bool bCurUser = false);

	// 修改用户数据库密码
	bool ChangeUserDBPwd(const LPSTR lpstrUserDBName, const char* pOldPwd, const int nOldLen, const char* pNewPwd, const int nNewLen);
	
	// 
	bool ReadNeedAutoLoadUser(string& strUserID, string& strMail, string& strStoken);

	// 检验用户库是否存在
	bool IsUserDBExist(LPSTR lpstrUserDBName);

	bool IsTableEmptyInGuestDB(const char* pTbName);


	// （可以满足表的结构不同，指定的列名进行拷贝）
	//bool CpyDbTb2OtherDbTb(const CppSQLite3DB& dbSour, const CppSQLite3DB& dbDesc, PTBCPYNODE pTabName, std::vector<PTBCPYNODE>* pColName,  LPSTR lpCondition = NULL);

	std::string strDBVer();
	UserIDMap*  GetUserIDMap() {return &m_UserIDMap;}
	UserIDMap   m_UserIDMap;
	TableField*  GetTablefield() {return &m_TableField;}
	TableField   m_TableField;
	int         GetList(INT64 i64Big30 ,int &m_total);
	void        GfieldName ();
	int         TableStr(std::string &strtable);
	string      itostr(int num);
	string      Filterchar(string scr);

	CppSQLite3DB* GetDataDbObject(); // 得到DataDB数据库对象的指针
	CppSQLite3DB* GetSysDbObject(); // 得到SysDB数据库对象的指针(应该将3个获取数据库的方法写成一个方法)

protected:
	bool InternalGetFav(std::vector<LPFAVRECORD>& vec);
	bool InternalDeleteFav(std::string strFav);
	bool InternalSaveFav(std::string strFav, int status);
	bool CreateDataDBFile(LPSTR lpPath);
	int GetDBVersion(CppSQLite3DB& db); // 读取数据库中软件的版本
	bool CopyDataFromOldVersoion(CppSQLite3DB& dbSour, CppSQLite3DB& dbDesc);

	INT64 InternalAddEvent(LPEVENTRECORD lpEventRec);
	bool InternalDeleteEvent(INT64 id);
	bool InternalGetEvents(int year, int month, std::vector<LPEVENTRECORD>& vecEventRec);
	bool InternalGetEventsOneDay(int year, int month, int day, std::vector<LPEVENTRECORD>& vecEventRec);

	bool InternalSetAlarm(INT64 id, int status, std::string alarmtype);
	bool InternalGetTodayAlarms(std::vector<LPALARMRECORD>& vec);

	std::string InternalQuerySQL(std::string strSQL, std::string strDBName); // 账单接口
	int InternalExecuteSQL(std::string strSQL, std::string strDBName);
	std::string InternalGetPieXMLData(std::string strStartDate, std::string strEndDate);
	std::string InternalGetNewPieXMLData(std::string strStartDate, std::string strEndDate);
	std::string InternalGetBarXMLData(const std::string& strStartDate, const std::string& strEndDate, const std::string& strKind);

	void ReplaceCharInString(std::string& strBig, const std::string & strsrc, const std::string &strdst); 

	bool GetExchangePartSql(LPCSTR lpStrConditon, std::string& strExchange);

	void SetIndexDoubleAttribute(TiXmlElement * pSet, const char * name, double val );


	// 判断数据是否存在
	bool IsFileExist(LPWSTR lpPath);


	// 从源文件转换到目标文件
	//bool TranslateSourceFile2DestinationFile(LPSTR lpSource, LPSTR lpDestination, bool bEncrypt = true);

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

	void split(std::string& s, std::string& delim,std::vector< std::string >* ret);
	void InitSysDbTempFile(void); // 初始化系统数据库临时文件

private:
	static CRITICAL_SECTION m_cs;
	std::string m_strUtfUserDbPath; // 存储的是当前用户数据库的全路径 utf8!!
	std::wstring m_strUserDbPath;//
	std::string m_strUtfDataPath; // 存放用户数据库的路径 utf8
	std::wstring m_strDataPath; // 存放用户数据库的路径
	//std::string m_strUserSourDbPath; // 存储当前用户的数据库源文件路径
	std::string m_strUtfSysDbPath; // 存储系统数据的全路径 uft8!!!
	std::wstring m_strSysDbPath; // 存储系统数据的全路径

	std::string m_strUtfDataDbPath; // 存储DataDB的全路径  ！！utf8
	std::wstring m_strDataDbPath;//
	std::wstring m_strGuestTemplete; // 存储用户模板数据的路径//这个不用utf存储

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