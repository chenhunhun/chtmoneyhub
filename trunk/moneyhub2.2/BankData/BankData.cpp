// BankData.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BankData.h"
#include <shlobj.h>
#include <tchar.h>
#include <atlconv.h>
#include <time.h>
#include <algorithm>
#include <assert.h>
#include <atlstr.h>
#include "DownloadFile.h"
#include "../Utils/RecordProgram/RecordProgram.h"
#include "../include/ConvertBase.h"

CRITICAL_SECTION CBankData::m_cs;

CBankData::CBankData()
{
	InitializeCriticalSection(&m_cs);
}

CBankData::~CBankData()
{
	DeleteCriticalSection(&m_cs);
}

int CBankData::GetTodayAlarmsNumber()
{
	ObjectLock lock(this);

	std::vector<LPALARMRECORD> vec;
	int size = 0;
	if (InternalGetTodayAlarms(vec))
	{
		for (size_t i = 0; i < vec.size(); i++)
		{
			LPALARMRECORD pAlarmRec = vec[i];
			if ((pAlarmRec->type == 0 && pAlarmRec->status == 1) || (pAlarmRec->type == 1 && pAlarmRec->status == 1))
				size ++;
			delete pAlarmRec;
		}
		vec.clear();
	}

	return size;
}

CBankData* CBankData::GetInstance()
{
	static CBankData bank_data;
	return &bank_data;
}

void CBankData::split(std::string& s, std::string& delim,std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		ret->push_back(s.substr(last,index-last));
		last=index+1;
		index=s.find_first_of(delim,last);
	}
	if (index-last>0)
	{
		ret->push_back(s.substr(last,index-last));
	}
} 

bool CBankData::ChangeOrder(const char* pBankID, int nTo, int nFrom)
{
	ATLASSERT (nTo > 0 && nFrom > 0 && pBankID != NULL);
	if (nTo <= 0 || nFrom <= 0 || pBankID == NULL)
		return false;

	if (nTo == nFrom)
		return false;

	int nTempTo = -1;
	int nTempFrom = -1;

	// 读取数据库中所有的记录集
	ObjectLock lock(this);
	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Binary blob;
		CppSQLite3Query q = db.execQuery("SELECT * FROM tbFav order by favorder desc;");
		
		for (int i = 1; !q.eof (); i ++, q.nextRow ())
		{
			if (nTo == i)
			{
				nTempTo = q.getIntField ("favorder");
			}
			if (nFrom == i)
			{
				nTempFrom = q.getIntField ("favorder");
			}
		}

		q.finalize();
			
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"ChangeOrder1 error:%d", ex.errorCode()));
		return false;
	}
	if (-1 == nTempTo || -1 == nTempFrom)
		return false;
	nTo = nTempTo; // 得到nTo索引对应的favorder的值
	nFrom = nTempFrom;


	CppSQLite3Buffer bufSQL1, bufSQL2;

	if (nTo <  nFrom)
	{
		bufSQL1.format("update tbFav set favorder = favorder + 1 where favorder between %d and %d;", nTo, nFrom);
		bufSQL2.format ("update tbFav set favorder = %d where favinfo = '%s';", nTo, pBankID); 
	}
	else
	{
		bufSQL1.format("update tbFav set favorder = favorder - 1 where favorder between '%d' and '%d';", nFrom, nTo);
		bufSQL2.format ("update tbFav set favorder = %d where favinfo = '%s';", nTo, pBankID); 
	}

	//bufSQL.format("SELECT favinfo FROM tbFav WHERE favinfo='%d';",strFav.c_str());
//	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		db.execDML(bufSQL1);
		db.execDML(bufSQL2);
		
		return true;
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"ChangeOrder2 error:%d", ex.errorCode()));
		return false;
	}
}

const char* CBankData::GetDbPath()
{
	static char szDbPath[1024] = { 0 };

	if (szDbPath[0] == 0)
	{
		WCHAR tempPath[MAX_PATH] = { 0 };
		SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, tempPath);
		wcscat_s(tempPath, L"\\MoneyHub");
		::CreateDirectoryW(tempPath, NULL);
		wcscat_s(tempPath, L"\\Data");
		::CreateDirectoryW(tempPath, NULL);
		wcscat_s(tempPath, L"\\moneyhub.dat");
		// CppSQLite3DB 存在中文创建失败的问题，需要utf-8编码的文件名
		
		int srcCount = wcslen(tempPath);
		srcCount = (srcCount > MAX_PATH) ? MAX_PATH : srcCount;
		::WideCharToMultiByte(CP_UTF8, 0, tempPath, srcCount, szDbPath, 1024, NULL,FALSE);

		CppSQLite3DB db;
		db.open(szDbPath);

		if (!db.tableExists("tbFav")) {
			db.execDML("CREATE TABLE tbFav(favinfo CHAR(4), status INT, favorder INT, deleted BOOL);");
			//GetInstance()->SaveFav("a001");
			db.execDML("CREATE TABLE tbAuthen(pwd BLOB);");
			db.execDML("CREATE TABLE tbEvent(id INT, event_date INT, description CHAR(256), repeat INT, alarm INT, status INT, datestring CHAR(12));");
			db.execDML("CREATE TABLE tbCoupon(id INT, expire INT, status INT, name CHAR(256), sn CHAR(256), typeid INT);");
			db.execDML("CREATE TABLE tbToday(current INT);");
			db.execDML("CREATE TABLE tbDBInfo(schema_version INT);");
			db.execDML("CREATE TABLE tbUSBInfo(vid INT, pid INT, mid INTEGER, fav CHAR(4), xml CHAR(512), bkurl CHAR(256), ver CHAR(16), status INT);"); // 将usbinfo进行存储的表
			CppSQLite3Buffer bufSQL;
			bufSQL.format("INSERT INTO tbDBInfo VALUES(%d);", 5);
			db.execDML(bufSQL);		

			//插入今天的日期作为第一条记录
			SYSTEMTIME systime;
			GetSystemTime(&systime);

			struct tm tmVal;
			memset(&tmVal, 0, sizeof(struct tm));
			tmVal.tm_year = systime.wYear - 1900;
			tmVal.tm_mon = systime.wMonth - 1;
			tmVal.tm_mday = systime.wDay;

			__time32_t tToday = _mktime32(&tmVal);
			bufSQL.format("INSERT INTO tbToday VALUES(%d);", tToday);
			db.execDML(bufSQL);
		}
		if(!db.tableExists("tbDBInfo")) {
			//管理数据库版本
			db.execDML("CREATE TABLE tbDBInfo(schema_version INT);");
			CppSQLite3Buffer bufSQL;
			bufSQL.format("INSERT INTO tbDBInfo VALUES(%d);", 4);
			db.execDML(bufSQL);		
			db.execDML("ALTER TABLE tbEvent ADD COLUMN datestring CHAR(12);");
			db.execDML("DROP TABLE IF EXISTS tbCoupon;");
			db.execDML("CREATE TABLE tbCoupon(id INT, expire INT, status INT, name CHAR(256), sn CHAR(256), typeid INT);");
			db.execDML("CREATE TABLE tbUSBInfo(vid INT, pid INT, mid INTEGER, fav CHAR(4), xml CHAR(512), bkurl CHAR(256), ver CHAR(16), status INT);"); // 将usbinfo进行存储的表
		} else {
			CppSQLite3Query q = db.execQuery("SELECT schema_version FROM tbDBInfo;");
			int s_version = q.getIntField("schema_version");
			q.finalize();
			if (s_version < 4) {
				//升级数据库表结构
				db.execDML("UPDATE tbDBInfo SET schema_version=4;");
				db.execDML("DROP TABLE IF EXISTS tbCoupon;");
				db.execDML("CREATE TABLE tbCoupon(id INT, expire INT, status INT, name CHAR(256), sn CHAR(256), typeid INT);");
			}
			if (s_version < 5) {
				db.execDML("UPDATE tbDBInfo SET schema_version=5;");
				//取出旧收藏，删除旧表，建新表，放入新表
				std::string strFav; //= "a001&b001";
				try
				{
					CppSQLite3Binary blob;
					CppSQLite3Query q1 = db.execQuery("SELECT favinfo FROM tbFav;");
					if (!q1.eof()) {
						blob.setEncoded((unsigned char*)q1.fieldValue("favinfo"));
						strFav = (const char *)blob.getBinary();
					} else {
						strFav = "";
					}
					q1.finalize();
				}
				catch (CppSQLite3Exception&)
				{
					strFav = "";
				}
				std::vector<std::string> vFav;
				if (strFav != "") {
					std::string splitter = "&";
					GetInstance()->split(strFav, splitter, &vFav);
				}
				db.execDML("DROP TABLE IF EXISTS tbFav;");
				db.execDML("CREATE TABLE tbFav(favinfo CHAR(4), status INT, favorder INT, deleted BOOL);");
				for (size_t i = 0; i < vFav.size(); i++)
				{
					CppSQLite3Buffer bufSQL;
					bufSQL.format("INSERT INTO tbFav VALUES('%s',%d, %d, %d);", vFav[i].c_str(), 200, (i + 1), 0);//将老版本的升级为状态200，表示安装完
					db.execDML(bufSQL);		
				}
				vFav.clear();

				db.execDML("CREATE TABLE tbUSBInfo(vid INT, pid INT, mid INTEGER, fav CHAR(4), xml CHAR(512), bkurl CHAR(256), ver CHAR(16), status INT);"); // 将usbinfo进行存储的表
			}
		}
	}

	return szDbPath;
}

const char* CBankData::GetCouponPath()
{
	static char szCouponPath[1024] = { 0 };

	if (szCouponPath[0] == 0)
	{
		SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szCouponPath);
		strcat_s(szCouponPath, "\\MoneyHub");
		::CreateDirectoryA(szCouponPath, NULL);
		strcat_s(szCouponPath, "\\Coupons");
		::CreateDirectoryA(szCouponPath, NULL);
	}

	return szCouponPath;
}

void CBankData::RemoveDatabase()
{
	ObjectLock lock(this);

	try
	{
		DeleteFileA(GetDbPath());
	}
	catch (...)
	{
	}
}
//判断是否已经存在
bool CBankData::IsFavExist(std::string& strFav)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		bool isExist = false;
		CppSQLite3Buffer bufSQL;
		bufSQL.format("SELECT favinfo FROM tbFav WHERE favinfo='%s';",strFav.c_str());
		CppSQLite3Query q = db.execQuery(bufSQL);
		if(!q.eof())
			isExist = true;

		q.finalize();
		return isExist;
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"IsFavExist error:%d", ex.errorCode()));
		return false;
	}
}

// 删除收藏夹
int CBankData::DeleteFav(std::string strFav)
{
	ObjectLock lock(this);
	return InternalDeleteFav(strFav) ? 1 : 0;
	
}
// 添加收藏夹
int CBankData::SaveFav(std::string strFav, int status)
{
	ObjectLock lock(this);
	return InternalSaveFav(strFav, status) ? 1 : 0;
}

//bool CBankData::GetNotSetupUsbKey(std::vector<LPUSBRECORD>& vec)
//{
//	vec.clear ();
//
//	ObjectLock lock(this);
//
//	try
//	{
//		CppSQLite3DB db;
//		db.open(GetDbPath());
//
//		CppSQLite3Binary blob;
//		CppSQLite3Query q = db.execQuery("SELECT * FROM tbUSBInfo where status < 200;");
//		while (!q.eof())
//		{
//
//			LPUSBRECORD pNode = new USBRECORD;
//			ATLASSERT (NULL != pNode);
//			if (NULL == pNode)
//				continue;
//
//			// 这里只要vid pid mid就可以了
//			pNode->pid = q.getIntField ("pid");
//			pNode->vid = q.getIntField ("vid");
//			pNode->mid = q.getIntField ('mid');
//			vec.push_back (pNode);
//
//			q.nextRow();
//		}
//
//		q.finalize();
//		return true;		
//	}
//
//	catch (CppSQLite3Exception&)
//	{
//		return false;
//	}
//		
//}

int CBankData::GetFav(std::list<std::string>& strFav,bool bNoInstall) // 得到没有安装但已经收藏的银行控件ID
{
	strFav.clear ();
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Binary blob;
		CppSQLite3Query q;
		if(bNoInstall)
			q = db.execQuery("SELECT * FROM tbFav;");
		else
			q = db.execQuery("SELECT * FROM tbFav WHERE status<200;");
		while (!q.eof()) {
			strFav.push_back(ToSQLUnsafeString(q.getStringField("favinfo")));
			q.nextRow();
		}

		q.finalize();
		return true;		
	}

	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"GetFav error:%d", ex.errorCode()));
		return false;
	}
}

int CBankData::GetFav(std::string& strFav)
{

	ObjectLock lock(this);

	strFav = "[";

	std::vector<LPFAVRECORD> vec;

	bool bOK = InternalGetFav(vec);
	if (bOK)
	{
		char szVal[1024];
		for (size_t i = 0; i < vec.size(); i++)
		{
			LPFAVRECORD pFav = vec[i];

			sprintf_s(szVal, sizeof(szVal), "{\"id\":\"%s\",\"status\":\"%d\"}", pFav->fav, pFav->status);

			strFav += szVal;

			if (i != (vec.size() - 1))
				strFav += ",";

			delete pFav;
		}
		vec.clear();
	}

	strFav += "]";

	return bOK;
}


int CBankData::AddUSB(int vid, int pid, DWORD mid, std::string fav, std::string xml,std::string bkurl, std::string ver,int status)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Buffer bufSQL;
		bufSQL.format("INSERT INTO tbUSBInfo (vid, pid, mid, fav, xml, bkurl, ver, status) VALUES(%d, %d, %d, '%s', '%s', '%s', '%s', %d);", vid, pid, mid, fav.c_str(), xml.c_str(), bkurl.c_str(),ver.c_str(), status);

		db.execDML(bufSQL);
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"AddUSB error:%d", ex.errorCode()));
		return 0;
	}
	return 1;
}
bool CBankData::DeleteUSB(int vid, int pid, DWORD mid)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Buffer bufSQL;
		bufSQL.format("DELETE FROM tbUSBInfo WHERE vid=%d AND pid=%d AND mid = %d", vid, pid, mid);

		db.execDML(bufSQL);
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"DeleteUSB error:%d", ex.errorCode()));
		return false;
	}
	return true;
}
int CBankData::UpdateUSB(int vid, int pid, DWORD mid, std::string fav, std::string xml)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Buffer bufSQL;
		bufSQL.format("UPDATE tbUSBInfo SET fav='%s', xml='%s' WHERE vid=%d AND pid=%d AND mid=%d;", fav.c_str(), xml.c_str(), vid, pid, mid);

		db.execDML(bufSQL);
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"UpdateUSB0 error:%d", ex.errorCode()));
		return 0;
	}
	return 1;

}
int CBankData::UpdateUSB(int vid, int pid, DWORD mid, int status)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Buffer bufSQL;
		bufSQL.format("UPDATE tbUSBInfo SET status=%d WHERE vid=%d AND pid=%d AND mid=%d;", status, vid, pid, mid);

		db.execDML(bufSQL);
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"UpdateUSB1 error:%d", ex.errorCode()));
		return 0;
	}
	return 1;

}

int CBankData::UpdateUSB(int vid, int pid, DWORD mid, const std::string& strVer)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Buffer bufSQL;
		bufSQL.format("UPDATE tbUSBInfo SET ver='%s' WHERE vid=%d AND pid=%d AND mid=%d;", strVer.c_str(), vid, pid, mid);

		db.execDML(bufSQL);
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"UpdateUSB2 error:%d", ex.errorCode()));
		return 0;
	}
	return 1;
}

bool CBankData::IsUsbExist(int vid, int pid, DWORD mid)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Buffer bufSQL;
		bufSQL.format("SELECT * FROM tbUSBInfo WHERE vid=%d AND pid=%d AND mid=%d;", vid, pid, mid);

		CppSQLite3Binary blob;

		bool bExist = false;
		CppSQLite3Query q = db.execQuery(bufSQL);
		if (!q.eof()) {
			bExist = true;
		}
		q.finalize();
		return bExist;
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"IsUsbExist error:%d", ex.errorCode()));
		return false;
	}

}
bool CBankData::GetAllUsb(std::vector<LPUSBRECORD>& vec)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Binary blob;

		CppSQLite3Query q = db.execQuery("SELECT * FROM tbUSBInfo");
		while (!q.eof()) {
			LPUSBRECORD pusb = new USBRECORD;
			pusb->vid = q.getIntField("vid");
			pusb->pid = q.getIntField("pid");
			pusb->mid = q.getIntField("mid");
			strcpy_s(pusb->fav, sizeof(pusb->fav), ToSQLUnsafeString(q.getStringField("fav")).c_str());
			strcpy_s(pusb->xml, sizeof(pusb->xml), ToSQLUnsafeString(q.getStringField("xml")).c_str());
			strcpy_s(pusb->bkurl, sizeof(pusb->bkurl), ToSQLUnsafeString(q.getStringField("bkurl")).c_str());
			strcpy_s(pusb->ver, sizeof(pusb->ver), ToSQLUnsafeString(q.getStringField("ver")).c_str());
			pusb->status = q.getIntField("status");

			vec.push_back(pusb);
			q.nextRow();
		}
		q.finalize();
		return true;
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"GetAllUsb error:%d", ex.errorCode()));
		return false;
	}

}

// 通过给定的vid, pid, mid读取相应的记录,没有找到相应的记录就返回 false
bool CBankData::GetAUSBRecord(int vid, int pid, DWORD mid, USBRECORD& record)
{
	memset (&record, sizeof (USBRECORD), 0);
	ObjectLock lock(this);
	bool bRet = false;

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Binary blob;

		CppSQLite3Buffer bufSQL;
		bufSQL.format("SELECT * FROM tbUSBInfo WHERE vid=%d AND pid=%d AND mid=%d;", vid, pid, mid);

		CppSQLite3Query q = db.execQuery(bufSQL);
		if (!q.eof())
		{
			record.vid = vid;
			record.pid = pid;
			record.mid = mid;
			strcpy_s(record.fav, sizeof(record.fav), ToSQLUnsafeString(q.getStringField("fav")).c_str());
			strcpy_s(record.xml, sizeof(record.xml), ToSQLUnsafeString(q.getStringField("xml")).c_str());
			strcpy_s(record.bkurl, sizeof(record.bkurl), ToSQLUnsafeString(q.getStringField("bkurl")).c_str());
			strcpy_s(record.ver, sizeof(record.ver), ToSQLUnsafeString(q.getStringField("ver")).c_str());
			record.status = q.getIntField("status");
			bRet = true;
		}
		q.finalize();
		return bRet;
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"GetAUSBRecord error:%d", ex.errorCode()));
		return bRet;
	}
}

void CBankData::SavePwd(std::string strPwd)
{
	ObjectLock lock(this);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		db.execDML("DELETE FROM tbAuthen;");

		CppSQLite3Binary blob;
		blob.setBinary((const unsigned char *)strPwd.c_str(), strPwd.length() + 1);

		CppSQLite3Buffer bufSQL;
		bufSQL.format("INSERT INTO tbAuthen VALUES(%Q);", blob.getEncoded());

		db.execDML(bufSQL);
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"SavePwd error:%d", ex.errorCode()));
	}
}

std::string CBankData::GetPwd()
{
	ObjectLock lock(this);

	std::string strPwd = "";

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Binary blob;

		CppSQLite3Query q = db.execQuery("SELECT pwd FROM tbAuthen;");
		if (!q.eof()) {
			blob.setEncoded((unsigned char*)q.fieldValue("pwd"));
			strPwd = (const char *)blob.getBinary();
		} else {
			strPwd = "";
		}
		q.finalize();
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"GetPwd error:%d", ex.errorCode()));
	}

	return strPwd;
}

//
// event 格式
// {"id":0,"event_date":"2010-09-01","description":"事件详情（不超过30个中文字）","repeat":2,"alarm":3,"status":1}
//
int CBankData::AddEvent(std::string event)
{
	ObjectLock lock(this);

	try
	{
		Json::Value root;
		Json::Reader reader;

		bool parsingSuccessful = reader.parse(event, root);
		if (!parsingSuccessful)
			return 0;

		if (!IsValidEventJson(root))
			return 0;

		EVENTRECORD rec;
		rec.id = root.get("id", 0).asInt();
		rec.event_date = ToIntTime(root.get("event_date", "").asString());
		strncpy_s(rec.description, sizeof(rec.description), ToSQLSafeString(root.get("description", "").asString()).c_str(), 255);
		rec.repeat = root.get("repeat", 0).asInt();
		rec.alarm = root.get("alarm", 0).asInt();
		rec.status = root.get("status", 0).asInt();

		return InternalAddEvent(&rec);
	}
	catch (std::runtime_error& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_RUNTIME_ERROR, AToW(ex.what()));
		return 0;		
	}
}

int CBankData::DeleteEvent(int id)
{
	ObjectLock lock(this);
	
	return InternalDeleteEvent(id) ? 1 : 0;
}

int CBankData::GetEvents(int year, int month, std::string& events)
{
	ObjectLock lock(this);

	events = "[";

	std::vector<LPEVENTRECORD> vec;
	bool bOK = InternalGetEvents(year, month, vec);
	if (bOK)
	{
		char szVal[1024];
		for (size_t i = 0; i < vec.size(); i++)
		{
			LPEVENTRECORD pEventRec = vec[i];
			
			sprintf_s(szVal, sizeof(szVal), "{\"id\":%d,\"event_date\":\"%s\",\"start_date\":\"%s\",\"description\":\"%s\",\"repeat\":%d,\"alarm\":%d,\"status\":%d}",
				pEventRec->id,
				ToStrTime(pEventRec->event_date).c_str(),
				ToStrTime(pEventRec->start_date).c_str(),
				pEventRec->description,
				pEventRec->repeat,
				pEventRec->alarm,
				pEventRec->status);

			events += szVal;

			if (i != (vec.size() - 1))
				events += ",";

			delete pEventRec;
		}

		vec.clear();
	}
	
	events += "]";

	return bOK ? 1 : 0;
}

int CBankData::GetEventsOneDay(int year, int month, int day, std::string& events)
{
	ObjectLock lock(this);

	events = "[";

	std::vector<LPEVENTRECORD> vec;
	bool bOK = InternalGetEventsOneDay(year, month, day, vec);
	if (bOK)
	{
		char szVal[1024];
		for (size_t i = 0; i < vec.size(); i++)
		{
			LPEVENTRECORD pEventRec = vec[i];

			sprintf_s(szVal, sizeof(szVal), "{\"id\":%d,\"event_date\":\"%s\",\"start_date\":\"%s\",\"description\":\"%s\",\"repeat\":%d,\"alarm\":%d,\"status\":%d}",
				pEventRec->id,
				ToStrTime(pEventRec->event_date).c_str(),
				ToStrTime(pEventRec->start_date).c_str(),
				pEventRec->description,
				pEventRec->repeat,
				pEventRec->alarm,
				pEventRec->status);

			events += szVal;

			if (i != (vec.size() - 1))
				events += ",";

			delete pEventRec;
		}

		vec.clear();
	}

	events += "]";

	return bOK ? 1 : 0;
}

int CBankData::SetAlarm(std::string alarm)
{
	ObjectLock lock(this);

	try
	{
		Json::Value root;
		Json::Reader reader;

		bool parsingSuccessful = reader.parse(alarm, root);
		if (!parsingSuccessful)
			return 0;

		if (!IsValidAlarmJson(root))
			return 0;

		int id = root.get("id", 0).asInt();
		int status = root.get("status", 0).asInt();
		std::string alarmtype = root.get("type", 0).asString();

		return InternalSetAlarm(id, status, alarmtype) ? 1 : 0;
	}
	catch (std::runtime_error& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_RUNTIME_ERROR, AToW(ex.what()));
		return 0;
	}
}

bool DateSortCallback(const LPALARMRECORD& m1, const LPALARMRECORD& m2)
{
	return m1->event_date < m2->event_date;
}

std::string CBankData::GetTodayAlarms()
{
	ObjectLock lock(this);

	std::string result = "[";
	
	std::vector<LPALARMRECORD> vec;
	if (InternalGetTodayAlarms(vec))
	{

		sort(vec.begin(), vec.end(), DateSortCallback);

		char szVal[1024];
		for (size_t i = 0; i < vec.size(); i++)
		{
			LPALARMRECORD pAlarmRec = vec[i];

			if (pAlarmRec->type == 0)
			{
				sprintf_s(szVal, sizeof(szVal), "{\"id\":%d,\"event_date\":\"%s\",\"description\":\"%s\",\"status\":%d,\"type\":\"%s\"}",
					pAlarmRec->id,
					ToStrTime(pAlarmRec->event_date).c_str(),
					pAlarmRec->description,
					pAlarmRec->status,
					"event");

			}
			else
			{
				sprintf_s(szVal, sizeof(szVal), "{\"id\":%d,\"event_date\":\"%s\",\"description\":\"%s%s\",\"status\":%d,\"type\":\"%s\"}",
					pAlarmRec->id,
					ToStrTime(pAlarmRec->event_date).c_str(),
					pAlarmRec->description,
					"优惠券到期",
					pAlarmRec->status,
					"coupon");
			}

			result += szVal;

			if (i != (vec.size() - 1))
				result += ",";

			delete pAlarmRec;
		}

		vec.clear();
	}

	result += "]";

	return result;
}

int CBankData::SaveCoupon(int style, std::string id, std::string expire, std::string couponname)
{
	ObjectLock lock(this);

	return InternalSaveCoupon(style, id, ToIntTime(expire), couponname) ? 1 : 0;
}

int CBankData::DeleteCoupon(int id)
{
	ObjectLock lock(this);

	return InternalDeleteCoupon(id) ? 1 : 0;
}

std::string CBankData::GetCoupons()
{
	ObjectLock lock(this);

	std::string result = "[";

	std::vector<LPCOUPONRECORD> vec;

	if (InternalGetCoupons(vec))
	{
		char szVal[1024];
		for (size_t i = 0; i < vec.size(); i++)
		{
			LPCOUPONRECORD pCouponRec = vec[i];

			CStringA strLargeImage, strMiddleImage, strSmallImage;
			strLargeImage.Format("file:///%s\\%dL.jpg", GetCouponPath(), pCouponRec->id);
			strMiddleImage.Format("file:///%s\\%dM.jpg", GetCouponPath(), pCouponRec->id);
			strSmallImage.Format("file:///%s\\%dS.jpg", GetCouponPath(), pCouponRec->id);

			strLargeImage.Replace('\\', '/');
			strMiddleImage.Replace('\\', '/');
			strSmallImage.Replace('\\', '/');

			sprintf_s(szVal, sizeof(szVal), "{\"id\":%d,\"expire\":\"%s\",\"large-image\":\"%s\",\"middle-image\":\"%s\",\"small-image\":\"%s\"}",
				pCouponRec->id, ToStrTime(pCouponRec->expire).c_str(), strLargeImage, strMiddleImage, strSmallImage);
			result += szVal;

			if (i != (vec.size() - 1))
				result += ",";

			delete pCouponRec;
		}
		
	}

	result += "]";

	return result;
}

//////////////////////////////////////////////////////////////////////////
// 内部函数


bool CBankData::InternalGetFav(std::vector<LPFAVRECORD>& vec)
{
	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Binary blob;
		CppSQLite3Query q = db.execQuery("SELECT * FROM tbFav order by favorder desc;");
		while (!q.eof()) {
			LPFAVRECORD pFav = new FAVRECORD;
			pFav->status = q.getIntField("status");
			strcpy_s(pFav->fav, sizeof(pFav->fav), ToSQLUnsafeString(q.getStringField("favinfo")).c_str());

			vec.push_back(pFav);
			q.nextRow();
		}

		q.finalize();
		return true;		
	}

	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalGetFav error:%d", ex.errorCode()));
		return false;
	}
}
bool CBankData::InternalDeleteFav(std::string strFav)
{
	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());
		CppSQLite3Buffer bufSQL;
		bufSQL.format("DELETE FROM tbFav WHERE favinfo='%s';", strFav.c_str());
		db.execDML(bufSQL);		
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalDeleteFav error:%d", ex.errorCode()));
		return false;
	}
	return true;

}
bool CBankData::InternalSaveFav(std::string strFav, int status)
{
	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		bool isExist = false;
		CppSQLite3Buffer bufSQL;
		bufSQL.format("SELECT favinfo FROM tbFav WHERE favinfo='%s';",strFav.c_str());
		CppSQLite3Query q = db.execQuery(bufSQL);

		// 找出favorder的最大值
		bufSQL.format ("select max(favorder) from tbFav;");
		CppSQLite3Query p = db.execQuery(bufSQL);
		int nMaxValue = p.getIntField ("max(favorder)");
		if (nMaxValue <= 0)
			nMaxValue = 1;
		else
			nMaxValue ++;

		if(!q.eof())
			isExist = true;

		q.finalize();
		p.finalize ();

		if(!isExist)// 如果该参数不存在，那就插入
		{
			CppSQLite3Buffer bufSQL;
			bufSQL.format("INSERT INTO tbFav VALUES('%s', %d, %d, 0);", strFav.c_str(), status, nMaxValue);
			db.execDML(bufSQL);
		}
		else
		{	
			CppSQLite3Buffer bufSQL;
			bufSQL.format("UPDATE tbFav SET status=%d WHERE favinfo='%s';", status, strFav.c_str());
			db.execDML(bufSQL);
		}
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalSaveFav error:%d", ex.errorCode()));
		return false;
	}
	return true;
}


int CBankData::InternalAddEvent(LPEVENTRECORD lpEventRec)
{
	int nId = 1;

	//转换时间格式为带星期
	struct tm newtime;
	_localtime32_s(&newtime, &(lpEventRec->event_date));
	char datestring[128];
	strftime(datestring, 128, "%Y-%m-%d %w", &newtime);

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		CppSQLite3Buffer bufSQL;
		if (lpEventRec->id == 0)
		{
			bufSQL.format("SELECT MAX(id) FROM tbEvent");
			CppSQLite3Query q = db.execQuery(bufSQL);

			if (!q.fieldIsNull(0))
				nId = atoi(q.fieldValue(0)) + 1;

			bufSQL.format("INSERT INTO tbEvent VALUES(%d, %d, '%s', %d, %d, %d, '%s');", 
				nId,
				(int)lpEventRec->event_date,
				lpEventRec->description,
				lpEventRec->repeat,
				lpEventRec->alarm,
				lpEventRec->status,
				datestring);

			db.execDML(bufSQL);		
		}
		else
		{
			nId = lpEventRec->id;

			bufSQL.format("UPDATE tbEvent SET event_date=%d, description='%s', repeat=%d, alarm=%d, status=%d, datestring='%s' WHERE id=%d;",
				(int)lpEventRec->event_date,
				lpEventRec->description,
				lpEventRec->repeat,
				lpEventRec->alarm,
				lpEventRec->status,
				datestring,
				lpEventRec->id);

			db.execDML(bufSQL);
		}
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalAddEvent error:%d", ex.errorCode()));
		return 0;
	}

	return nId;
}

bool CBankData::InternalDeleteEvent(int id)
{
	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		if (!db.tableExists("tbEvent"))
			return true;

		CppSQLite3Buffer bufSQL;
		bufSQL.format("DELETE FROM tbEvent WHERE id=%d;", id);

		db.execDML(bufSQL);
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalDeleteEvent error:%d", ex.errorCode()));
		return false;
	}

	return true;
}

bool CBankData::InternalGetEvents(int year, int month, std::vector<LPEVENTRECORD>& vecEventRec)
{
	assert(vecEventRec.empty());

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		if (!db.tableExists("tbEvent"))
			return true;

		// 开始时间
		struct tm tmVal;
		memset(&tmVal, 0, sizeof(struct tm));
		tmVal.tm_mday = 1;
		tmVal.tm_year = year - 1900;
		tmVal.tm_mon = month - 1;
		
		__time32_t tHead = _mktime32(&tmVal);

		// 结束时间
		memset(&tmVal, 0, sizeof(struct tm));
		tmVal.tm_mday = 1;
		if (month < 12)
		{
			tmVal.tm_year = year - 1900;
			tmVal.tm_mon = month;
		}
		else
		{
			tmVal.tm_year = year - 1900 + 1;
			tmVal.tm_mon = 0;
		}

		__time32_t tTail = _mktime32(&tmVal);


/*
		// Query
		CppSQLite3Buffer bufSQL;
		bufSQL.format("SELECT * FROM tbEvent WHERE event_date>=%d AND event_date<%d;", tHead, tTail);

		CppSQLite3Query q = db.execQuery(bufSQL);
		while (!q.eof())
		{
			LPEVENTRECORD pEventRec = new EVENTRECORD;
			
			pEventRec->id = q.getIntField("id");
			pEventRec->event_date = q.getIntField("event_date");
			strcpy_s(pEventRec->description, sizeof(pEventRec->description), ToSQLUnsafeString(q.getStringField("description")).c_str());
			pEventRec->repeat = q.getIntField("repeat");
			pEventRec->alarm = q.getIntField("alarm");
			pEventRec->status = q.getIntField("status");

			vecEventRec.push_back(pEventRec);

			q.nextRow();
		}
		q.finalize();
*/

		// Query
		CppSQLite3Buffer bufSQL;
		bufSQL.format("SELECT * FROM tbEvent WHERE event_date<%d;", tTail);

		CppSQLite3Query q = db.execQuery(bufSQL);
		while (!q.eof())
		{
			__time32_t event_date = q.getIntField("event_date");
			int repeat1 = q.getIntField("repeat");

			std::vector<__time32_t> vecShowDates;
			if (IsMonthShowEvent(tHead, tTail, event_date, repeat1, &vecShowDates))
			{
				for (size_t i = 0; i < vecShowDates.size(); i++)
				{
					LPEVENTRECORD pEventRec = new EVENTRECORD;

					pEventRec->id = q.getIntField("id");
					pEventRec->event_date = vecShowDates[i];
					pEventRec->start_date = event_date;
					strcpy_s(pEventRec->description, sizeof(pEventRec->description), ToSQLUnsafeString(q.getStringField("description")).c_str());
					pEventRec->repeat = q.getIntField("repeat");
					pEventRec->alarm = q.getIntField("alarm");
					pEventRec->status = q.getIntField("status");

					vecEventRec.push_back(pEventRec);
				}
			}
			
			q.nextRow();
		}
		q.finalize();
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalGetEvents error:%d", ex.errorCode()));
		return false;
	}

	return true;
}

bool CBankData::InternalGetEventsOneDay(int year, int month, int day, std::vector<LPEVENTRECORD>& vecEventRec)
{
	assert(vecEventRec.empty());

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		if (!db.tableExists("tbEvent"))
			return true;

		// 开始时间
		struct tm tmVal;
		memset(&tmVal, 0, sizeof(struct tm));
		tmVal.tm_year = year - 1900;
		tmVal.tm_mon = month - 1;
		tmVal.tm_mday = day;

		__time32_t tHead = _mktime32(&tmVal);

		//计算星期几
		struct tm newtime;
		_localtime32_s(&newtime, &tHead);
		int wday = newtime.tm_wday;

		// 结束时间
		__time32_t tTail = tHead + 24 * 60 * 60;

		// Query
		CppSQLite3Buffer bufSQL;
		bufSQL.format("SELECT * FROM tbEvent WHERE event_date<%d AND ((repeat=0 AND event_date>=%d) OR (repeat=1 AND SUBSTR(datestring,12)='%d') OR (repeat=2 AND SUBSTR(datestring,9,2)='%02d') OR (repeat=3 AND SUBSTR(datestring,6,2)%%3=%d AND SUBSTR(datestring,9,2)='%02d') OR (repeat=4 AND SUBSTR(datestring,6,2)='%02d' AND SUBSTR(datestring,9,2)='%02d'));", tTail, tHead, wday, day, month%3, day, month, day);

		CppSQLite3Query q = db.execQuery(bufSQL);
		while (!q.eof())
		{
			LPEVENTRECORD pEventRec = new EVENTRECORD;

			pEventRec->id = q.getIntField("id");
			pEventRec->event_date = q.getIntField("event_date");
			pEventRec->start_date = q.getIntField("event_date");
			strcpy_s(pEventRec->description, sizeof(pEventRec->description), ToSQLUnsafeString(q.getStringField("description")).c_str());
			pEventRec->repeat = q.getIntField("repeat");
			pEventRec->alarm = q.getIntField("alarm");
			pEventRec->status = q.getIntField("status");

			vecEventRec.push_back(pEventRec);

			q.nextRow();
		}
		q.finalize();
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalGetEvents error:%d", ex.errorCode()));
		return false;
	}

	return true;
}

bool CBankData::InternalSetAlarm(int id, int status, std::string alarmtype)
{
	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		if (!db.tableExists("tbEvent"))
			return true;

		CppSQLite3Buffer bufSQL;
		if (alarmtype == "event") {
			bufSQL.format("UPDATE tbEvent SET status=%d WHERE id=%d;", status, id);
		} else {
			bufSQL.format("UPDATE tbCoupon SET status=%d WHERE id=%d;", status, id);
		}

		db.execDML(bufSQL);
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalSetAlarm error:%d", ex.errorCode()));
		return false;
	}

	return true;
}

//
// 返回 status = 1 & status = 2 的提醒事件
//
bool CBankData::InternalGetTodayAlarms(std::vector<LPALARMRECORD>& vec)
{
	try
	{
		//////////////////////////////////////////////////////////////////////////
		// 获得今天的时间

		SYSTEMTIME systime;
		GetSystemTime(&systime);

		struct tm tmVal;
		memset(&tmVal, 0, sizeof(struct tm));
		tmVal.tm_year = systime.wYear - 1900;
		tmVal.tm_mon = systime.wMonth - 1;
		tmVal.tm_mday = systime.wDay;

		__time32_t tToday = _mktime32(&tmVal);

		TryResetAlarmStatus(tToday);

		//////////////////////////////////////////////////////////////////////////
		// Open database

		CppSQLite3DB db;
		db.open(GetDbPath());

		//////////////////////////////////////////////////////////////////////////
		// From tbEvent

		if (db.tableExists("tbEvent"))
		{
			CppSQLite3Buffer bufSQL;
			bufSQL.format("SELECT * FROM tbEvent ORDER BY event_date;");
			
			CppSQLite3Query q = db.execQuery(bufSQL);
			while (!q.eof())
			{
				int id = q.getIntField("id");
				__time32_t event_date = q.getIntField("event_date");
				int repeat1 = q.getIntField("repeat");
				int alarm = q.getIntField("alarm");

				__time32_t alarm_date;

				if (IsAlarmEvent(tToday, event_date, repeat1, alarm, &alarm_date))
				{
					LPALARMRECORD pAlarmRec = new ALARMRECORD;
					
					pAlarmRec->type = 0;
					pAlarmRec->id = q.getIntField("id");
					
					// 返回的是警告时间，不是创建时间
					//pAlarmRec->event_date = q.getIntField("event_date");
					pAlarmRec->event_date = alarm_date;

					strcpy_s(pAlarmRec->description, sizeof(pAlarmRec->description), ToSQLUnsafeString(q.getStringField("description")).c_str());
					pAlarmRec->status = q.getIntField("status");
					
					vec.push_back(pAlarmRec);
				}

				q.nextRow();
			}
			q.finalize();
		}

		//////////////////////////////////////////////////////////////////////////
		// From tbCoupon

		if (db.tableExists("tbCoupon"))
		{
			//__time32_t tAlarm = tToday + 11 * 24 * 60 * 60;
			__time32_t tAlarm = tToday + 4 * 24 * 60 * 60;

			CppSQLite3Buffer bufSQL;
			bufSQL.format("SELECT * FROM tbCoupon WHERE expire>=%d AND expire<%d;", tToday, tAlarm);

			CppSQLite3Query q = db.execQuery(bufSQL);
			while (!q.eof())
			{
				LPALARMRECORD pAlarmRec = new ALARMRECORD;

				pAlarmRec->type = 1;
				pAlarmRec->id = q.getIntField("id");
				pAlarmRec->event_date = q.getIntField("expire");
				try {
					strcpy_s(pAlarmRec->description, sizeof(pAlarmRec->description), ToSQLUnsafeString(q.getStringField("name")).c_str());
				} catch(CppSQLite3Exception&) {
					strcpy_s(pAlarmRec->description, sizeof(pAlarmRec->description), ToSQLUnsafeString("").c_str());
				}
				try {
					pAlarmRec->status = q.getIntField("status");
				} catch(CppSQLite3Exception&) {
					pAlarmRec->status = 1;
				}

				vec.push_back(pAlarmRec);

				q.nextRow();
			}

			q.finalize();
		}
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalGetTodayAlarms error:%d", ex.errorCode()));
		return false;
	}

	return true;
}

bool CBankData::InternalSaveCoupon(int style, std::string id, __time32_t expire, std::string couponname)
{
	int nId = 1;

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());
		CppSQLite3Buffer bufSQL;

		bufSQL.format("SELECT MAX(id) FROM tbCoupon");
		CppSQLite3Query q = db.execQuery(bufSQL);

		if (!q.fieldIsNull(0))
			nId = atoi(q.fieldValue(0)) + 1;

		if (id == "-1") {
			//对于可重复优惠券，我们只保留一张
			bufSQL.format("DELETE FROM tbCoupon WHERE typeid=%d;", style);
			db.execDML(bufSQL);	
		}

		bufSQL.format("INSERT INTO tbCoupon(id, expire, status, name, sn, typeid) VALUES(%d, %d, %d, '%s', '%s', %d);", nId, (int)expire, 1, couponname.c_str(), id.c_str(), style);
		db.execDML(bufSQL);	
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalSaveCoupon error:%d", ex.errorCode()));
		return false;
	}

	if(!DownloadCouponImages(style, id, nId))// 如果下载失败，那么删除已经获取到的优惠券
	{
		InternalDeleteCoupon(nId);
		return false;
	}
	return true;
}

bool CBankData::InternalDeleteCoupon(int id)
{
	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		if (!db.tableExists("tbCoupon"))
			return true;

		CppSQLite3Buffer bufSQL;
		bufSQL.format("DELETE FROM tbCoupon WHERE id=%d;", id);

		db.execDML(bufSQL);	
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalDeleteCoupon error:%d", ex.errorCode()));
		return false;
	}

	DeleteCouponImages(id);

	return true;
}

bool CBankData::InternalGetCoupons(std::vector<LPCOUPONRECORD>& vecCouponRec)
{
	assert(vecCouponRec.empty());

	try
	{
		CppSQLite3DB db;
		db.open(GetDbPath());

		if (!db.tableExists("tbCoupon"))
			return true;

		CppSQLite3Buffer bufSQL;
		bufSQL.format("SELECT * FROM tbCoupon ORDER BY 1;");

		CppSQLite3Query q = db.execQuery(bufSQL);

		while (!q.eof())
		{
			LPCOUPONRECORD pCouponRec = new COUPONRECORD;
			pCouponRec->id = q.getIntField(0);
			pCouponRec->expire = q.getIntField(1);

			vecCouponRec.push_back(pCouponRec);

			q.nextRow();
		}

		q.finalize();
	}
	catch (CppSQLite3Exception& ex)
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, CRecordProgram::GetInstance()->GetRecordInfo(L"InternalGetCoupons error:%d", ex.errorCode()));
		return false;
	}

	return true;
}

void CBankData::TryResetAlarmStatus(__time32_t tToday)
{
	CppSQLite3DB db;
	db.open(GetDbPath());

	CppSQLite3Query q = db.execQuery("SELECT * FROM tbToday;");
	if (!q.eof())
	{
		int tOldTime = q.getIntField("current");
		if (tOldTime != tToday)
		{
			db.execDML("UPDATE tbEvent SET alarm=1 WHERE alarm=0;");
			
			CppSQLite3Buffer bufSQL;
			bufSQL.format("UPDATE tbToday SET current=%d;", tToday);
			db.execDML(bufSQL);
		}
	}
	else
	{
		CppSQLite3Buffer bufSQL;
		bufSQL.format("INSERT INTO tbToday VALUES(%d);", tToday);
		db.execDML(bufSQL);		
	}

	q.finalize();
}

std::string CBankData::ToSQLSafeString(std::string strQuery)
{
	std::string strOut;
	for (size_t i = 0; i < strQuery.size(); i++)
	{
		if (strQuery[i] == '"')
			strOut += "\\\"";
		else
			strOut += strQuery[i];
	}

	return strOut;
}

std::string CBankData::ToSQLUnsafeString(std::string strQuery)
{
	std::string strOut;
	for (size_t i = 0; i < strQuery.size(); i++)
	{
		if (i < (strQuery.size() - 1) && strQuery[i] == '\\' && strQuery[i + 1] == '"')
		{
			strOut += "\"";
			i++;
		}
		else
			strOut += strQuery[i];
	}

	return strOut;
}

std::string CBankData::ToStrTime(__time32_t t)
{
	std::string result;
	struct tm tmVal;

	if (_localtime32_s(&tmVal, &t) == 0)
	{
		char szTime[32];
		sprintf_s(szTime, "%04d-%02d-%02d", tmVal.tm_year + 1900, tmVal.tm_mon + 1, tmVal.tm_mday);
		result = szTime;
	}

	return result;
}

__time32_t CBankData::ToIntTime(std::string t)
{
	int year, month, day;
	if (3 == sscanf_s(t.c_str(), "%d-%d-%d", &year, &month, &day))
	{
		struct tm tmVal;
		memset(&tmVal, 0, sizeof(struct tm));
		tmVal.tm_year = year - 1900;
		tmVal.tm_mon = month - 1;
		tmVal.tm_mday = day;

		return _mktime32(&tmVal);
	}

	return -1;
}

bool CBankData::IsAlarmEvent(__time32_t today, __time32_t event_date, int repeat1, int alarm, __time32_t* pAlarmDate)
{
	const int one_day = 24 * 60 * 60;

	struct tm tmToday;
	struct tm tmEvent;

	if ((today + alarm * one_day) < event_date)
		return false;

	if (repeat1 == 0)	// 不重复
	{
		if (event_date >= today && event_date < (today + (alarm + 1) * one_day))
		{
			if (pAlarmDate)
				*pAlarmDate = event_date;

			return true;
		}
	}
	else if (repeat1 == 1) // 周重复
	{
		if (_localtime32_s(&tmToday, &today) == 0 && _localtime32_s(&tmEvent, &event_date) == 0)
		{
			int diff_day = tmEvent.tm_wday - tmToday.tm_wday;
			if (diff_day < 0) diff_day += 7;

			event_date = today + diff_day * one_day;
			if ((today + alarm * one_day) >= event_date)
			{
				if (pAlarmDate)
					*pAlarmDate = event_date;

				return true;
			}
		}
	}
	else if (repeat1 == 2) // 月重复
	{
		if (_localtime32_s(&tmToday, &today) == 0 && _localtime32_s(&tmEvent, &event_date) == 0)
		{
			tmEvent.tm_year = tmToday.tm_year;
			tmEvent.tm_mon = tmToday.tm_mon;

			int diff_day = tmEvent.tm_mday - tmToday.tm_mday;
			if (diff_day < 0)
			{
				tmEvent.tm_mon++;
				if (tmEvent.tm_mon == 12)
				{
					tmEvent.tm_year++;
					tmEvent.tm_mon = 0;
				}
			}

			AdjustTimeValue(&tmEvent);
			event_date = _mktime32(&tmEvent);

			if ((today + alarm * one_day) >= event_date)
			{
				if (pAlarmDate)
					*pAlarmDate = event_date;

				return true;
			}
		}
	}
	else if (repeat1 == 3) // 季度重复
	{
		if (_localtime32_s(&tmToday, &today) == 0 && _localtime32_s(&tmEvent, &event_date) == 0)
		{
			tmEvent.tm_year = tmToday.tm_year;
						
			if (tmEvent.tm_mon < tmToday.tm_mon ||
				((tmEvent.tm_mon == tmToday.tm_mon) && (tmEvent.tm_mday < tmToday.tm_mday)))
			{
				int nEventQuarter = (tmEvent.tm_mon / 3 + 1);
				int nTodayQuarter = (tmToday.tm_mon / 3 + 1);			
				
				tmEvent.tm_mon += (nTodayQuarter - nEventQuarter) * 3;
				if (tmEvent.tm_mon < tmToday.tm_mon || (tmEvent.tm_mon == tmToday.tm_mon && tmEvent.tm_mday < tmToday.tm_mday))
					tmEvent.tm_mon++;

				if (tmEvent.tm_mon >= 12)
				{
					tmEvent.tm_mon %= 12;
					tmEvent.tm_year++;
				}
			}

			AdjustTimeValue(&tmEvent);
			event_date = _mktime32(&tmEvent);

			if ((today + alarm * one_day) >= event_date)
			{
				if (pAlarmDate)
					*pAlarmDate = event_date;

				return true;
			}
		}		
	}
	else if (repeat1 == 4) // 年重复
	{
		if (_localtime32_s(&tmToday, &today) == 0 && _localtime32_s(&tmEvent, &event_date) == 0)
		{
			tmEvent.tm_year = tmToday.tm_year;

			if (tmEvent.tm_mon < tmToday.tm_mon || 
				((tmEvent.tm_mon == tmToday.tm_mon) && (tmEvent.tm_mday < tmToday.tm_mday)))
			{
				tmEvent.tm_year++;
			}

			AdjustTimeValue(&tmEvent);
			event_date = _mktime32(&tmEvent);

			if ((today + alarm * one_day) >= event_date)
			{
				if (pAlarmDate)
					*pAlarmDate = event_date;

				return true;
			}
		}	
	}

	return false;
}

bool CBankData::NotLeapYear(int year)
{
	return !(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

void CBankData::AdjustTimeValue(struct tm* _tm)
{
	if (_tm->tm_mon == 3 ||			// 4月
		_tm->tm_mon == 5 ||			// 6月
		_tm->tm_mon == 8 ||			// 9月
		_tm->tm_mon == 10)			// 11月
	{
		if (_tm->tm_mday == 31)
			_tm->tm_mday = 30;
	}

	if (_tm->tm_mon == 1)			// 2月
	{
		if (_tm->tm_mday > 28)
		{
			if (NotLeapYear(_tm->tm_year))
				_tm->tm_mday = 28;
			else
				_tm->tm_mday = 29;
		}
	}
}

bool CBankData::IsMonthShowEvent(__time32_t tHead, __time32_t tTail, __time32_t event_date, int repeat1, std::vector<__time32_t>* pvecShowDates)
{
	const int one_day = 24 * 60 * 60;

	struct tm tmHead;
	struct tm tmTail;
	_localtime32_s(&tmHead, &tHead);
	_localtime32_s(&tmTail, &tTail);


	int nCount = 0;
	if (repeat1 == 0)	// 不重复
	{
		if (event_date >= tHead && event_date < tTail)
		{
			nCount++;
			if (pvecShowDates)
				pvecShowDates->push_back(event_date);
		}
	}
	else if (repeat1 == 1)	// 周重复
	{
		struct tm tmEvent;
		if (_localtime32_s(&tmEvent, &event_date) == 0)
		{
			int nDiff = tmEvent.tm_wday - tmHead.tm_wday;
			if (nDiff < 0) nDiff += 7;

			__time32_t tNow = tHead + nDiff * one_day;
			while (true)
			{
				if (tNow >= event_date && tNow < tTail)
				{
					nCount++;
					if (pvecShowDates)
						pvecShowDates->push_back(tNow);
				}

				tNow += 7 * one_day;

				if (tNow >= tTail)
					break;
			}
		}
	}
	else if (repeat1 == 2)	// 月重复
	{
		struct tm tmEvent;
		if (_localtime32_s(&tmEvent, &event_date) == 0)
		{
			tmEvent.tm_mon = tmHead.tm_mon;
			AdjustTimeValue(&tmEvent);
			
			__time32_t tNow = _mktime32(&tmEvent);

			nCount++;
			if (pvecShowDates)
				pvecShowDates->push_back(tNow);
		}

	}
	else if (repeat1 == 3)	// 季重复
	{
		struct tm tmEvent;
		if (_localtime32_s(&tmEvent, &event_date) == 0)
		{
			if ((tmEvent.tm_mon - tmHead.tm_mon) % 3 == 0)
			{
				tmEvent.tm_mon = tmHead.tm_mon;
				AdjustTimeValue(&tmEvent);

				__time32_t tNow = _mktime32(&tmEvent);

				nCount++;
				if (pvecShowDates)
					pvecShowDates->push_back(tNow);
			}
		}		
	}
	else if (repeat1 == 4)	// 年重复
	{
		struct tm tmEvent;
		if (_localtime32_s(&tmEvent, &event_date) == 0)
		{
			if (tmEvent.tm_mon == tmHead.tm_mon)
			{
				tmEvent.tm_year = tmHead.tm_year;
				AdjustTimeValue(&tmEvent);

				__time32_t tNow = _mktime32(&tmEvent);

				nCount++;
				if (pvecShowDates)
					pvecShowDates->push_back(tNow);
			}
		}		
	}

	return nCount > 0;
}

bool CBankData::IsValidEventJson(Json::Value& root)
{
	return root.isMember("id") && 
		root.isMember("event_date") && 
		root.isMember("description") &&
		root.isMember("repeat") && 
		root.isMember("alarm") && 
		root.isMember("status");
}

bool CBankData::IsValidAlarmJson(Json::Value& root)
{
	return root.isMember("id") &&
		root.isMember("status");

	return true;
}

bool CBankData::DownloadCouponImages(int style, std::string sn, int id)
{
	CDownloadFile file(style, sn, id);
	return file.Start();
}

bool CBankData::DeleteCouponImages(int id)
{
	CDownloadFile file(0, "", id);
	return file.Delete();
}