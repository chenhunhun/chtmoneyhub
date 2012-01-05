// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#include "..\Utils\SQLite\CppSQLite3.h"
#include <string>
#include <atlstr.h>
#include <vector>
#include <time.h>
#pragma once
using namespace std;


class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	typedef struct tagTbCpyNode
	{
		std::string strSour; // 源数据
		std::string strDes;// 目标数据
	}TBCPYNODE, *PTBCPYNODE;
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /**/bHandled)
	{

		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);


		OnOK(0,IDOK, m_hWnd, bHandled);
		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	bool CreateDatFile(LPCSTR lpStr)
	{
		ATLASSERT(NULL != lpStr);
		HANDLE hFile = CreateFileA(lpStr, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			::MessageBoxA(NULL, "创建数据库文件失败！", "创建数据库", MB_OK);
			return false;
		}
		CloseHandle(hFile);

		return true;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 

		TCHAR szPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szPath, _countof(szPath));
		TCHAR *p = _tcsrchr(szPath, '\\');
		if (p) *p = 0;
		std::string strName  = CW2A(szPath);

		std::string strSys = strName + "\\Config\\SysDB.dat";
		// 创建系统数据库
		if (!CreateDatFile(strSys.c_str()))
			return 0;

		CppSQLite3DB db;
		db.open(strSys.c_str());
		// 创建系统数据库表
		CreateSystemTables(db);

		// 初始化系统数据库表
		InitSystemTables(db);

		std::string strGuest = strName + "\\Config\\Guest.dat";
		// 创建系统数据库
		if (!CreateDatFile(strGuest.c_str()))
			return 0;

		CppSQLite3DB dbGuest;
		dbGuest.open(strGuest.c_str());
		// 创建用户数据库
		CreateUserTables(dbGuest);

		// 初始化用户数据库
		InitUserDataBySql(dbGuest);

		// 初始化用户数据库
		InitUserDataBySysDB(dbGuest, strSys.c_str(), strGuest.c_str());

		CloseDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}

	// 在原有的版本上升级记账所要的一些数据
	void CreateAccountTables(CppSQLite3DB& db)
	{
		db.execDML("CREATE TABLE tbAccount( \
				   `id` BIGINT primary key NOT NULL DEFAULT 0 , \
				   `UT` BIGINT NOT NULL DEFAULT 0 , \
				   `mark` INT(2) NOT NULL DEFAULT 0 , \
				   `Name` VARCHAR(256) NOT NULL , \
				   `tbBank_id` BIGINT  NULL , \
				   `tbAccountType_id` INTEGER  NOT NULL , \
				   `AccountNumber` VARCHAR(256) NULL , \
				   `Comment` VARCHAR(256) NULL , \
				   `seq` INT(4) NOT NULL DEFAULT 0 , \
				   `EndDate` CHAR(4) NULL DEFAULT 0, \
				   `keyInfo` varchar(255) \
				   );");


		db.execDML("CREATE TABLE tbAccountGetBillMonth( \
				   `id` BIGINT primary key NOT NULL DEFAULT 0 , \
				   `UT` BIGINT NULL DEFAULT 0 , \
				   `mark` INT(2) NULL DEFAULT 0 , \
				   `tbmonth` VARCHAR(6) NULL DEFAULT NULL , \
				   `tbKeyInfo` VARCHAR(100) NULL DEFAULT NULL , \
				   `tbAccount_id` BIGINT NOT NULL  \
				   );");

		db.execDML("CREATE TABLE tbAccountType( \
				   `id` integer primary key NOT NULL, \
				   `Name` VARCHAR(256) NULL  \
				   );");

		db.execDML("CREATE TABLE tbBank( \
				   `id` BIGINT primary key NOT NULL DEFAULT 0 , \
				   `UT` BIGINT NOT NULL DEFAULT 0 , \
				   `mark` INT(2) NOT NULL DEFAULT 0 , \
				   `Name` VARCHAR(256) NOT NULL , \
				   `BankID` VARCHAR(4) NULL , \
				   `Phone` VARCHAR(256) NULL , \
				   `Website` VARCHAR(256) NULL , \
				   `classId` integer NOT NULL, \
				   `seq` INT(4) NOT NULL DEFAULT 0  \
				   );");

		db.execDML("CREATE TABLE tbCategory1( \
				   `id` BIGINT primary key NOT NULL DEFAULT 0 , \
				   `UT` BIGINT NOT NULL DEFAULT 0 , \
				   `mark` INT(2) NOT NULL DEFAULT 0 , \
				   `Name` VARCHAR(256) NULL , \
				   `Type` TINYINT NOT NULL , \
				   `seq` INT(4) NOT NULL DEFAULT 0 \
				   );");

		db.execDML("CREATE TABLE tbCategory2( \
				   `id` BIGINT primary key NOT NULL DEFAULT 0 , \
				   `UT` BIGINT NOT NULL DEFAULT 0 , \
				   `mark` INT(2) NOT NULL DEFAULT 0 , \
				   `Name` VARCHAR(256) NULL , \
				   `tbCategory1_id` BIGINT NOT NULL , \
				   `seq` INT(4) NOT NULL DEFAULT 0  \
				   );");

		db.execDML("CREATE TABLE tbCurrency( \
				   `id` integer  primary key NOT NULL DEFAULT 0 , \
				   `Name` VARCHAR(256) NOT NULL , \
				   `seq` INT(4) NOT NULL DEFAULT 0 \
				   );");

		db.execDML("CREATE TABLE tbDBInfo(schema_version INT);");

		db.execDML("CREATE TABLE tbEvent( \
				   `id` BIGINT PRIMARY KEY NOT NULL DEFAULT 0 , \
				   `UT` BIGINT NULL DEFAULT 0 , \
				   `mark` INT(2) NULL DEFAULT 0 , \
				   `event_date` INTEGER  NULL , \
				   `description` CHAR(256) NULL , \
				   `repeat` INTEGER  NULL , \
				   `alarm` INTEGER  NULL , \
				   `status` INTEGER  NULL , \
				   `datestring` CHAR(12) NULL , \
				   `tbAccount_id` BIGINT NULL , \
				   `tbSubAccount_id` BIGINT NULL , \
				   `type` INT(2) NULL  \
				   )");

		db.execDML("CREATE TABLE tbPayee(  \
				   `id` bigint  PRIMARY KEY NOT NULL DEFAULT 0 ,  \
				   `UT` BIGINT NOT NULL DEFAULT 0 ,  \
				   `mark` INT(2) NOT NULL DEFAULT 0 ,  \
				   `Name` VARCHAR(256) NOT NULL ,  \
				   `email` VARCHAR(256),  \
				   `tel` VARCHAR(256),  \
				   `seq` INT(4) NOT NULL DEFAULT 0   \
				   );");

		db.execDML("CREATE TABLE tbSubAccount( \
				   `id` BIGINT primary key NOT NULL DEFAULT 0 , \
				   `UT` BIGINT NOT NULL DEFAULT 0 , \
				   `mark` INT(2) NOT NULL DEFAULT 0 , \
				   `tbAccount_id` BIGINT NOT NULL , \
				   `tbCurrency_id` BIGINT NOT NULL , \
				   `name` VARCHAR(256) NOT NULL, \
				   `OpenBalance` FLOAT NOT NULL , \
				   `Balance` FLOAT NOT NULL , \
				   `Days` INTEGER  NULL , \
				   `EndDate` DATE NULL DEFAULT 0 , \
				   `Comment` VARCHAR(256) NULL , \
				   `tbAccountType_id` INTEGER NULL , \
				   `seq` INT(4) NOT NULL DEFAULT 0 \
				   );");

		db.execDML("CREATE TABLE tbTransaction( \
				   `id` BIGINT primary key NOT NULL DEFAULT 0, \
				   `UT` BIGINT NOT NULL DEFAULT 0, \
				   `mark` INT(2) NOT NULL DEFAULT 0 , \
				   `TransDate` DATE NOT NULL , \
				   `tbPayee_id` BIGINT NULL , \
				   `tbCategory2_id` BIGINT NULL , \
				   `Amount` FLOAT NOT NULL DEFAULT 0 , \
				   `direction` BIGINT NULL DEFAULT 0 , \
				   `tbSubAccount_id` BIGINT NOT NULL , \
				   `tbSubAccount_id1` BIGINT NULL , \
				   `ExchangeRate` FLOAT NULL , \
				   `Comment` VARCHAR(256) NULL , \
				   `sign` VARCHAR(256) NULL, \
				   `transactionClasses` int(2), \
				   `seq` INT(4) NOT NULL DEFAULT 0 \
				   );");
		db.execDML("CREATE INDEX `fk_tbAccount_tbBank` ON `tbAccount` (`tbBank_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbAccount_tbAccountType1` ON `tbAccount` (`tbAccountType_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbCategory2_tbCategory11` ON `tbCategory2` (`tbCategory1_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbSubAccount_tbAccount1` ON `tbSubAccount` (`tbAccount_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbSubAccount_tbCurrency1` ON `tbSubAccount` (`tbCurrency_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbSubAccount_tbAccountType1` ON `tbSubAccount` (`tbAccountType_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbTransaction_tbPayee1` ON `tbTransaction` (`tbPayee_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbTransaction_tbCategory21` ON `tbTransaction` (`tbCategory2_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbTransaction_tbSubAccount1` ON `tbTransaction` (`tbSubAccount_id` ASC);");
		db.execDML("CREATE INDEX `fk_tbTransaction_tbSubAccount2` ON `tbTransaction` (`tbSubAccount_id1` ASC);");
		db.execDML("CREATE TABLE tbProductChoice( \
				   `id` BIGINT PRIMARY KEY  NOT NULL DEFAULT 0 , \
				   `UT` BIGINT NULL DEFAULT 0 , \
				   `mark` INT(2) NULL DEFAULT 0 , \
				   `interestchoice` VARCHAR(256) NULL DEFAULT NULL , \
				   `durationchoice` VARCHAR(256) NULL DEFAULT NULL , \
				   `currencychoice` VARCHAR(256) NULL DEFAULT NULL , \
				   `bankchoice` VARCHAR(256) NULL DEFAULT NULL , \
				   `userId` BIGINT NULL DEFAULT 0  \
				   );");
		db.execDML("insert into tbProductChoice (id, interestchoice,durationchoice,currencychoice,bankchoice,userId,ut) values(0,'1|2|3|4','1|2|3|4|5|6','1|2','1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19',0,0);");

		return;
	}

	void CreateUserTables(CppSQLite3DB& db)
	{
		// 创建用户表
		CreateAccountTables(db);

	}

	void InitUserDataBySql(CppSQLite3DB& db)
	{
		// 在从系统表中拷贝数据时会将相应表中原有的数据清除，所以新插入的数据应该在拷完之后
		db.execDML("INSERT INTO tbAccountType (id, Name) VALUES (100, \"活期存款\");");
		db.execDML("INSERT INTO tbAccountType (id, Name) VALUES (101, \"定期存款\");");
		db.execDML("INSERT INTO tbAccountType (id, Name) VALUES (102, \"理财产品\");");

		//db.execDML("INSERT INTO tbAccount (id, Name, tbAccountType_id) VALUES (1, \"我的现金\", 1);");
		//db.execDML("INSERT INTO tbAccount (id, Name, tbAccountType_id) VALUES (2, \"银行卡\", 3);");
		//db.execDML("INSERT INTO tbAccount (id, Name, tbAccountType_id) VALUES (3, \"信用卡\", 2);");

		// db.execDML("INSERT INTO tbSubAccount (id, Name, tbAccount_id, tbCurrency_id, OpenBalance, Balance, tbAccountType_id) VALUES (1, \"活期存款\", 2, 1, 0, 0, 100);");
		////db.execDML("INSERT INTO tbSubAccount (id, Name, tbAccount_id, tbCurrency_id, OpenBalance, Balance) VALUES (2, \"定期存款\", 2, 1, 0, 0);");
		// db.execDML("INSERT INTO tbSubAccount (id, Name, tbAccount_id, tbCurrency_id, OpenBalance, Balance) VALUES (2, \"人民币\", 3, 1, 0, 0);");
		// db.execDML("INSERT INTO tbSubAccount (id, Name, tbAccount_id, tbCurrency_id, OpenBalance, Balance) VALUES (3, \"美元\", 3, 2, 0, 0);");
		// sdb.execDML("INSERT INTO tbSubAccount (id, Name, tbAccount_id, tbCurrency_id, OpenBalance, Balance) VALUES (4, \"人民币\", 1, 1, 0, 0);");

		db.execDML("INSERT INTO tbDBInfo (schema_version) VALUES(8);"); //4.0进当前版本为8
	}

	// 将数据库的表拷贝到另一个数据库中
	bool CpyTbBankTable(PTBCPYNODE pPath, PTBCPYNODE pTabName, std::vector<PTBCPYNODE>* pColName,  LPSTR lpCondition = NULL)
	{
		ATLASSERT (NULL != pPath && NULL != pTabName && NULL != pColName);
		if (NULL == pPath || NULL == pTabName || NULL == pColName)
			return false;

		// 检验文件是否存在
//		if (!IsFileExist((LPSTR)pPath->strSour.c_str()) || !IsFileExist((LPSTR)pPath->strDes.c_str())) 
//			return false;

		CppSQLite3DB dbSour;
		dbSour.open(pPath->strSour.c_str());

		CppSQLite3DB dbDes;
		dbDes.open(pPath->strDes.c_str());

		std::string strSQL ;//= "delete from ";
		//strSQL += lpDesTbName;
		// 删除掉数据库原有的数据
		//dbDes.execDML(strSQL.c_str());

		int nSize = 0;
		std::string strColsSour, strColsDes;
		std::vector<PTBCPYNODE>::const_iterator iter;
		for (iter = pColName->begin(); iter != pColName->end(); iter ++)
		{
			PTBCPYNODE pTemp = *(iter);
			if (pTemp == NULL)
				continue;

			strColsSour += pTemp->strSour;
			strColsDes += pTemp->strDes;

			nSize ++;

			if (nSize != pColName->size())
			{
				strColsSour += ", ";
				strColsDes += ", ";
			}
		}


		// 得到要拷贝的语句
		strSQL = "select ";
		strSQL += strColsSour;
		strSQL += " from ";
		strSQL += pTabName->strSour;
		if (NULL != lpCondition)
		{
			strSQL += lpCondition;
		}

		// 得到要拷贝的记录
		CppSQLite3Table  tempTb = dbSour.getTable(strSQL.c_str());

		// 得到插入的SQL语句
		std::string strSqlFront = "Insert into ";
		strSqlFront += pTabName->strDes;
		strSqlFront += '(';
		strSqlFront += "id,";
		strSqlFront += strColsDes;
		strSqlFront += ") Values ";

		std::string strSQLInsert;
		for (int i = 0; i < tempTb.numRows(); i ++)
		{
			tempTb.setRow (i);

			std::string strColVals;
			strColVals += '(';

			CString strIndex;
			strIndex.Format(L"%d,", i + 1);
			strColVals += CW2A(strIndex);

			for (iter = pColName->begin(); iter != pColName->end(); iter ++)
			{
				PTBCPYNODE pTemp = *(iter);
				if (pTemp == NULL)
					continue;

				string strColName = pTemp->strSour;
				strColVals += '\'';
				strColVals += tempTb.fieldValue(strColName.c_str());
				strColVals += '\'';
				
				if (iter != pColName->end() - 1)
				{
					strColVals += ',';
				}
			}


			strColVals += ')';

			std::string strExSQL = strSqlFront + strColVals;

			// 执行插入
			dbDes.execDML(strExSQL.c_str());


		}

		return true;

	}


	bool CpySysTbToUserTb(PTBCPYNODE pPath, PTBCPYNODE pTabName, std::vector<PTBCPYNODE>* pColName,  LPSTR lpCondition = NULL)
	{
		ATLASSERT (NULL != pPath && NULL != pTabName && NULL != pColName);
		if (NULL == pPath || NULL == pTabName || NULL == pColName)
			return false;

		// 检验文件是否存在
//		if (!IsFileExist((LPSTR)pPath->strSour.c_str()) || !IsFileExist((LPSTR)pPath->strDes.c_str())) 
//			return false;

		CppSQLite3DB dbSour;
		dbSour.open(pPath->strSour.c_str());

		CppSQLite3DB dbDes;
		dbDes.open(pPath->strDes.c_str());

		std::string strSQL ;//= "delete from ";
		//strSQL += lpDesTbName;
		// 删除掉数据库原有的数据
		//dbDes.execDML(strSQL.c_str());

		int nSize = 0;
		std::string strColsSour, strColsDes;
		std::vector<PTBCPYNODE>::const_iterator iter;
		for (iter = pColName->begin(); iter != pColName->end(); iter ++)
		{
			PTBCPYNODE pTemp = *(iter);
			if (pTemp == NULL)
				continue;

			strColsSour += pTemp->strSour;
			strColsDes += pTemp->strDes;

			nSize ++;

			if (nSize != pColName->size())
			{
				strColsSour += ", ";
				strColsDes += ", ";
			}
		}


		// 得到要拷贝的语句
		strSQL = "select ";
		strSQL += strColsSour;
		strSQL += " from ";
		strSQL += pTabName->strSour;
		if (NULL != lpCondition)
		{
			strSQL += lpCondition;
		}

		// 得到要拷贝的记录
		CppSQLite3Table  tempTb = dbSour.getTable(strSQL.c_str());

		// 得到插入的SQL语句
		std::string strSqlFront = "Insert into ";
		strSqlFront += pTabName->strDes;
		strSqlFront += '(';
		//strSqlFront += "id,";
		strSqlFront += strColsDes;
		strSqlFront += ") Values ";

		std::string strSQLInsert;
		for (int i = 0; i < tempTb.numRows(); i ++)
		{
			tempTb.setRow (i);

			std::string strColVals;
			strColVals += '(';

			//CString strIndex;
			//strIndex.Format(L"%d,", i + 1);
			//strColVals += CW2A(strIndex);

			for (iter = pColName->begin(); iter != pColName->end(); iter ++)
			{
				PTBCPYNODE pTemp = *(iter);
				if (pTemp == NULL)
					continue;

				string strColName = pTemp->strSour;
				strColVals += '\'';
				strColVals += tempTb.fieldValue(strColName.c_str());
				strColVals += '\'';
				
				if (iter != pColName->end() - 1)
				{
					strColVals += ',';
				}
			}


			strColVals += ')';

			std::string strExSQL = strSqlFront + strColVals;

			// 执行插入
			dbDes.execDML(strExSQL.c_str());


		}

		return true;
	}

	void InitUserDataBySysDB(CppSQLite3DB& db, LPCSTR lpSour, LPCSTR lpDesc)
	{
		ATLASSERT(NULL != lpSour && NULL != lpDesc);
		if (NULL == lpSour || NULL == lpDesc)
			return;


		// 不同的数据库中相同的表结构之间的数据拷贝，拷贝时不会删除目标表中的原有数据
		TBCPYNODE path;
		path.strSour = lpSour;
		path.strDes = lpDesc;
		TBCPYNODE tabNode;
		tabNode.strSour = "sysAccountType";
		tabNode.strDes = "tbAccountType";
		std::vector<PTBCPYNODE> colVect;
		TBCPYNODE colNode1, colNode2, colNode3, colNode4;
		colNode1.strSour = "id";
		colNode1.strDes = "id";
		colNode2.strSour = "Name";
		colNode2.strDes = "Name";
		colVect.push_back(&colNode1);
		colVect.push_back(&colNode2);
		CpySysTbToUserTb(&path, &tabNode, &colVect, NULL);

		//CpyFromSameTable(lpSour, "sysCurrency", lpDesc, "tbCurrency");
		tabNode.strSour = "sysCurrency";
		tabNode.strDes = "tbCurrency";
		CpySysTbToUserTb(&path, &tabNode, &colVect, NULL);

		//CpyFromSameTable(lpSour, "sysCategory1", lpDesc, "tbCategory1");
		tabNode.strSour = "sysCategory1";
		tabNode.strDes = "tbCategory1";
		colNode3.strSour = "Type";
		colNode3.strDes = "Type";
		colVect.push_back(&colNode3);
		colNode4.strSour = "seq";
		colNode4.strDes = "seq";
		colVect.push_back(&colNode4);
		CpySysTbToUserTb(&path, &tabNode, &colVect, NULL);

		//CpyFromSameTable(lpSour, "sysCategory2", lpDesc, "tbCategory2");
		tabNode.strSour = "sysCategory2";
		tabNode.strDes = "tbCategory2";
		colVect.clear();
		colNode1.strSour = "id";
		colNode1.strDes = "id";
		colNode2.strSour = "Name";
		colNode2.strDes = "Name";
		colNode3.strSour = "sysCategory1_id";
		colNode3.strDes = "tbCategory1_id";
		colVect.push_back(&colNode1);
		colVect.push_back(&colNode2);
		colVect.push_back(&colNode3);
		CpySysTbToUserTb(&path, &tabNode, &colVect, NULL);


		// 不同数据库之间不同的表结构之间的数据拷贝，拷贝时不会删除目标表中原有的数据
		tabNode.strSour = "sysBank";
		tabNode.strDes = "tbBank";
		colVect.clear();
		colNode1.strSour = "id";
		colNode1.strDes = "BankID";
		colNode2.strSour = "Name";
		colNode2.strDes = "Name";
		colNode3.strSour = "classId";
		colNode3.strDes = "classId";
		colVect.push_back(&colNode1);
		colVect.push_back(&colNode2);
		colVect.push_back(&colNode3);
		CpyTbBankTable(&path, &tabNode, &colVect, " where classId in(0, 1, 2)");//, " where sysAppType_id = '2'"
	}


	void CreateSystemTables(CppSQLite3DB& db)
	{
		db.execDML("CREATE  TABLE IF NOT EXISTS `sysAppType` (\
				   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
				   `Name` VARCHAR(256) NOT NULL);");

		db.execDML("CREATE  TABLE IF NOT EXISTS `sysBank` (\
				   `id` VARCHAR(4) NOT NULL PRIMARY KEY,\
				   `Name` VARCHAR(256) UNIQUE NOT NULL,\
				   `classId` INTEGER NOT NULL DEFAULT(99),\
				   `ShortName` VARCHAR(256) UNIQUE NOT NULL,\
				   `Position` INTEGER NOT NULL,\
				   `sysAppType_id` INTEGER NOT NULL);");

		db.execDML("CREATE  TABLE IF NOT EXISTS `sysSubLink` (\
				   `sysBank_id` VARCHAR(4) NOT NULL  ,\
				   `URL` VARCHAR(256) UNIQUE NOT NULL,\
				   `URLText` VARCHAR(256) NOT NULL,\
				   `LinkOrder` INTEGER NOT NULL,\
				   PRIMARY KEY(sysBank_id, URL));");

		db.execDML("CREATE  TABLE IF NOT EXISTS `sysCategory1` (\
				   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
				   `Name` VARCHAR(256) NOT NULL,\
				   `Type` TINYINTNOT NOT NULL, \
				   `seq` int(4) NOT NULL default 0, \
				   `mark` int(2) NOT NULL default 0, \
				   `UT` bigint NOT NULL default 0);");

		db.execDML("CREATE  TABLE IF NOT EXISTS `sysCategory2` (\
				   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
				   `Name` VARCHAR(256) NOT NULL,\
				   `sysCategory1_id` TINYINTNOT NULL ,\
				   `seq` int(4) NOT NULL default 0, \
				   `mark` int(2) NOT NULL default 0, \
				   `UT` bigint NOT NULL default 0);");

		db.execDML("CREATE  TABLE IF NOT EXISTS `sysAccountType` (\
				   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
				   `Name` VARCHAR(256) NOT NULL);");

		db.execDML("CREATE  TABLE IF NOT EXISTS `sysCurrency` (\
				   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
				   `Name` VARCHAR(256) NOT NULL, \
				   `seq` integer(4) NOT NULL default 0);");

		db.execDML("CREATE  TABLE IF NOT EXISTS `sysDBInfo` (\
				   `schema_version` INTEGER NOT NULL);");

		db.execDML("CREATE  TABLE IF NOT EXISTS `sysURLWhiteList` (\
				   `sysBank_id` VARCHAR(4) NOT NULL PRIMARY KEY ,\
				   `URLPattern` VARCHAR(256) NOT NULL);");
	}
		
	void InitSystemTables(CppSQLite3DB& db)
	{
		db.execDML("INSERT INTO sysAppType (id, Name) VALUES (1, \"all\");");
		db.execDML("INSERT INTO sysAppType (id, Name) VALUES (2, \"banks\");");
		db.execDML("INSERT INTO sysAppType (id, Name) VALUES (3, \"securities\");");
		db.execDML("INSERT INTO sysAppType (id, Name) VALUES (4, \"insurances\");");
		db.execDML("INSERT INTO sysAppType (id, Name) VALUES (5, \"funds\");");
		db.execDML("INSERT INTO sysAppType (id, Name) VALUES (6, \"payments\");");
		db.execDML("INSERT INTO sysAppType (id, Name) VALUES (7, \"shopping\");");

		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a001\", \"中国银行\", 0, \"boc\", 0, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a002\", \"中国农业银行\", 0, \"abchina\", 1, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a003\", \"中国工商银行\", 0, \"icbc\", 2, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a004\", \"中国建设银行\", 0, \"ccb\", 3, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a005\", \"交通银行\", 0, \"bankcomm\", 4, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a006\", \"招商银行\", 0, \"cmbchina\", 5, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a007\", \"中信银行\", 0, \"ecitic\", 6, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a008\", \"光大银行\", 0, \"cebbank\", 7, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a009\", \"浦发银行\", 0, \"spdb\", 8, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a010\", \"深圳发展银行\", 0, \"sdb\", 9, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a011\", \"广发银行\", 0, \"gdb\", 10, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a012\", \"华夏银行\", 0, \"hxb\", 11, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a013\", \"民生银行\", 0, \"cmbc\", 12, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a014\", \"兴业银行\", 0, \"cib\", 13, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a015\", \"北京银行\", 0, \"beijing\", 14, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a016\", \"北京农商银行\", 0, \"bjrcb\", 15, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a017\", \"中国邮政储蓄银行\", 0, \"psbc\", 16, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a018\", \"平安银行\", 0, \"pinganbank\", 17, 2);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a019\", \"渤海银行\", 0, \"cbhb\", 18, 2);");

		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"b001\", \"中信证券\", 1, \"cs_ecitic\", 0, 3);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"c001\", \"中国人寿保险\", 1, \"chinalife\", 0, 4);");

		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d001\", \"华夏基金\", 1, \"chinaamc\", 0, 5);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d002\", \"招商基金\", 1, \"cmfchina\", 1, 5);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d003\", \"国泰基金\", 1, \"gtfund\", 2, 5);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d004\", \"南方基金\", 1, \"southernfund\", 3, 5);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d005\", \"嘉实基金\", 1, \"jsfund\", 4, 5);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d006\", \"富国基金\", 1, \"fullgoal\", 5, 5);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d007\", \"易方达基金\", 1, \"efunds\", 6, 5);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d008\", \"大成基金\", 1, \"dcfund\", 7, 5);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d009\", \"博时基金\", 1, \"bosera\", 8, 5);");

		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e001\", \"支付宝\", 2, \"alipay\", 0, 6);");
		//db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e002\", \"首信易支付\", 2, \"payease\", 1, 6);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e003\", \"开联\", 2, \"openunion\", 2, 6);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e004\", \"易宝\", 2, \"yeepay\", 3, 6);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e005\", \"财付通\", 2, \"tenpay\", 4, 6);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e006\", \"快钱\", 2, \"99bill\", 5, 6);");
		//db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e007\", \"盛付通\", 2, \"shengfutong\", 6, 6);");
		db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e008\", \"银联在线\", 2, \"chinapay\", 7, 6);");
		db.execDML("INSERT INTO sysBank (id, Name, ShortName, Position, sysAppType_id) VALUES (\"f001\", \"乐淘商城\", \"letao\", 0, 7);");
		db.execDML("INSERT INTO sysBank (id, Name, ShortName, Position, sysAppType_id) VALUES (\"f002\", \"淘宝网\", \"taobao\", 1, 7);");

		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a001\", \"http://www.boc.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a001\", \"https://ebs.boc.cn/BocnetClient/LoginFrame.do?_locale=zh_CN\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a002\", \"http://www.abchina.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a002\", \"http://www.abchina.com/cn/EBanking/Ebanklogin/PCustomerLogin/default.htm\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a003\", \"http://www.icbc.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a003\", \"https://mybank.icbc.com.cn/icbc/perbank/index.jsp\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a003\", \"https://vip.icbc.com.cn/\", \"贵宾版\", 2);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a004\", \"http://www.ccb.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a004\", \"https://ibsbjstar.ccb.com.cn/app/V5/CN/STY1/login.jsp\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a005\", \"http://www.bankcomm.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a005\", \"https://pbank.95559.com.cn/personbank/index.jsp\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a006\", \"http://www.cmbchina.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a006\", \"https://pbsz.ebank.cmbchina.com/CmbBank_GenShell/UI/GenShellPC/Login/Login.aspx\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a007\", \"http://bank.ecitic.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a007\", \"https://e.bank.ecitic.com/perbank5/signIn.do\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a008\", \"http://www.cebbank.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a008\", \"https://www.cebbank.com/per/prePerlogin1.do?_locale=zh_CN\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a008\", \"http://ebank.cebbank.com/preLogin.html\", \"专业版\", 2);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a009\", \"http://www.spdb.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a009\", \"http://ebank.spdb.com.cn/login/perlogin.html\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a010\", \"http://www.sdb.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a010\", \"https://ebank.sdb.com.cn/perbank/logon_pro.jsp\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a011\", \"http://www.gdb.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a011\", \"https://ebanks.gdb.com.cn/sperbank/perbankLogin.jsp\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a012\", \"http://www.hxb.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a012\", \"https://ebank.hxb.com.cn/HxPer/basPreLogin.do\", \"个人网银普通版\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a012\", \"https://ebank.hxb.com.cn/HxPer/regLogin01.do\", \"个人网银签约版\", 2);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a012\", \"https://dbank.hxb.com.cn/easybanking/jsp/indexCert.jsp\", \"个人网银证书版\", 3);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a013\", \"http://www.cmbc.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a013\", \"https://ebank.cmbc.com.cn/index_NonPrivate.html\", \"个人大众版\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a013\", \"https://business.cmbc.com.cn/index_Private.html\", \"贵宾版\", 2);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a013\", \"https://ebank.cmbc.com.cn/indexCCLogin.html\", \"信用卡\", 3);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a014\", \"http://www.cib.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a014\", \"https://www.cib.com.cn/index.jsp\", \"个人普通用户\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a014\", \"https://www.cib.com.cn/indexCert.jsp\", \"个人证书用户\", 2);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a015\", \"http://www.bankofbeijing.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a015\", \"https://ebank.bankofbeijing.com.cn/bccbpb/accountLogon.jsp\", \"个人普通用户\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a015\", \"https://ebank.bankofbeijing.com.cn/bccbpb/fortuneLogon.jsp\", \"个人财富用户\", 2);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a016\", \"http://www.bjrcb.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a016\", \"https://ibs.bjrcb.com/per/prelogin.do\", \"个人网银登陆\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a016\", \"https://ebank.bjrcb.com/ent/preloginCheque.do\", \"支付密码系统\", 3);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a017\", \"http://www.psbc.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a017\", \"https://pbank.psbc.com/\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a018\", \"http://bank.pingan.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a018\", \"https://www.pingan.com.cn/pinganone/pa/ebanklogin.screen\", \"平安一账通\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a019\", \"http://www.cbhb.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a019\", \"https://ebank.cbhb.com.cn/per/prelogin.do\", \"个人网银\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"b001\", \"http://www.cs.ecitic.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"c001\", \"http://www.chinalife.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d001\", \"http://www.chinaamc.com/\", \"首页\", 0);");	
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d002\", \"http://www.cmfchina.com/index.html\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d002\", \"https://direct.cmfchina.com/index.jsp\", \"基金网上交易\", 1);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d003\", \"http://www.gtfund.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d004\", \"http://www.nffund.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d005\", \"http://www.jsfund.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d006\", \"http://www.fullgoal.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d007\", \"http://www.efunds.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d008\", \"http://www.dcfund.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d009\", \"http://www.bosera.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e001\", \"http://www.alipay.com/\", \"首页\", 0);");	
		//db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e002\", \"http://www.beijing.com.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e003\", \"http://www.openunion.cn/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e003\", \"http://card.openunion.cn/\", \"连心卡\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e004\", \"http://www.yeepay.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e005\", \"http://www.tenpay.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e006\", \"http://www.99bill.com/\", \"首页\", 0);");
	//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e007\", \"http://www.yeepay.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e008\", \"http://www.chinapay.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"f001\", \"http://www.letao.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"f002\", \"http://www.taobao.com/\", \"首页\", 0);");
		db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"f002\", \"http://www.tmall.com/\", \"淘宝商城\", 1);");


		// 往系统中添加的用户模板数据
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (1, \"现金\");");
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (2, \"信用卡\");");
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (3, \"储蓄卡/存折\");");
		//db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (4, \"定期存单\");");
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (4, \"支付\");");
		//db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (6, \"贷款\");");
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (5, \"投资\");");
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (6, \"借出的钱\");");
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (7, \"借入的钱\");");
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (8, \"固定资产\");");
		db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (9, \"其它\");");

		db.execDML("INSERT INTO sysAccountType(id, Name) VALUES(301, \"现金\");");
		db.execDML("INSERT INTO sysAccountType(id, Name) VALUES(201, \"信用卡\");");
		db.execDML("INSERT INTO sysAccountType(id, Name) VALUES(304, \"支付\");");
		db.execDML("INSERT INTO sysAccountType(id, Name) VALUES(305, \"投资\");");
		db.execDML("INSERT INTO sysAccountType(id, Name) VALUES(306, \"借出的钱\");");
		db.execDML("INSERT INTO sysAccountType(id, Name) VALUES(307, \"借入的钱\");");
		db.execDML("INSERT INTO sysAccountType(id, Name) VALUES(308, \"固定资产\");");

		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (1, \"人民币\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (2, \"美元\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (3, \"欧元\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (4, \"日元\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (5, \"英镑\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (6, \"港币\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (7, \"加拿大元\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (8, \"澳元\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (9, \"瑞士法郎\");");
		db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (10, \"新加坡元\");");

		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (1, \"食品餐饮\", 0, 10);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (2, \"公共交通\", 0, 30);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (3, \"私家车费用\", 0, 40);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (4, \"居家物业\", 0, 50);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (5, \"娱乐休闲\", 0, 60);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (6, \"通讯物流\", 0, 70);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (7, \"礼金慈善\", 0, 80);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (8, \"医疗保健\", 0, 90);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (9, \"耐用消费品\", 0, 100);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (10, \"学习培训\", 0, 110);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (11, \"金融税费\", 0, 120);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (12, \"职业收入\", 1, 130);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (13, \"财产收入\", 1, 140);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (14, \"其它收入\", 1, 150);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (15, \"服饰美容\", 0, 20);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (10018, \"转账支出\", 0, 1000);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (10019, \"转账收入\", 1, 1000);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (10020, \"信用卡收入\", 1, 1000);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (10021, \"信用卡支出\", 0, 1000);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (10023, \"未定义支出\", 0, 1000);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (10024, \"未定义收入\", 1, 1000);");
		db.execDML("INSERT INTO sysCategory1 (id, Name, Type, seq) VALUES (10025, \"余额调整\", 0, 1000);");// 张京要求9-19


		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (1, \"食品\", 1);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (2, \"水果零食\", 1);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (3, \"烟酒饮料\", 1);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (4, \"外出就餐\", 1);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (5, \"其它\", 1);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (6, \"公交地铁\", 2);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (7, \"打车租车\", 2);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (8, \"长途交通\", 2);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (9, \"其它\", 2);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10, \"油费\", 3);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (11, \"维修保养\", 3);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (12, \"停车费\", 3);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (13, \"路桥费\", 3);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (14, \"税费保险\", 3);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (15, \"其它\", 3);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (16, \"日常用品\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (17, \"水电煤气\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (18, \"房租\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (19, \"取暖费\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (20, \"物业管理费\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (21, \"维修保养\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (22, \"有线电视\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (23, \"卫生保洁\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (24, \"母婴用品\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (25, \"其它\", 4);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (26, \"运动健身\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (27, \"聚会就餐\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (28, \"影音娱乐\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (29, \"宠物支出\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (30, \"旅游度假\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (31, \"玩具\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (32, \"书报杂志\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (33, \"博彩支出\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (34, \"其它\", 5);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (35, \"固话费\", 6);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (36, \"手机费\", 6);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (37, \"上网费\", 6);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (38, \"邮寄快递\", 6);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (39, \"其它\", 6);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (40, \"礼品礼金\", 7);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (41, \"孝敬长辈\", 7);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (42, \"慈善捐款\", 7);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (43, \"其它\", 7);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (44, \"门诊费\", 8);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (45, \"住院费\", 8);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (46, \"药品费\", 8);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (47, \"保健费\", 8);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (48, \"其它\", 8);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (49, \"家具\", 9);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (50, \"电器\", 9);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (51, \"家居装饰\", 9);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (52, \"家用工具\", 9);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (53, \"数码产品\", 9);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (54, \"交通工具\", 9);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (55, \"其它\", 9);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (56, \"培训费\", 10);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (57, \"学费\", 10);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (58, \"资料费\", 10);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (59, \"学习用品\", 10);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (60, \"其它\", 10);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (61, \"利息支出\", 11);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (62, \"税费支出\", 11);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (63, \"投资损失\", 11);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (64, \"保险支出\", 11);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (65, \"赔偿罚款\", 11);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (66, \"手续费\", 11);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (67, \"其它\", 11);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (68, \"工资收入\", 12);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (69, \"加班收入\", 12);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (70, \"奖金收入\", 12);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (71, \"兼职收入\", 12);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (72, \"经营所得\", 12);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (73, \"其它\", 12);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (74, \"租金收入\", 13);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (75, \"利息收入\", 13);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (76, \"投资回报\", 13);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (77, \"其它\", 13);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (78, \"礼金收入\", 14);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (79, \"中奖收入\", 14);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (80, \"继承\", 14);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (81, \"获得赠与\", 14);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (82, \"意外来钱\", 14);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (83, \"其它\", 14);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (84, \"服装\", 15);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (85, \"鞋帽箱包\", 15);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (86, \"饰品\", 15);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (87, \"化妆品\", 15);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (88, \"美容美发\", 15);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (89, \"其它\", 15);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10059, \"CATA420\", 10018);");//bug 2063
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10060, \"CATA420\", 10019);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10061, \"信用卡收入\", 10020);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10062, \"信用卡支出\", 10021);");
	//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10063, \"其它\", 17);");
	//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10064, \"其它\", 22);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10065, \"CATA420\", 10023);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10066, \"CATA420\", 10024);");
		db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10067, \"CATA420\", 10025)"); // 张京要求9-19

		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20067, \"CATA420\", 1)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20068,\"CATA420\",2)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20069,\"CATA420\",3)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20070,\"CATA420\",4)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20071,\"CATA420\",5)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20072,\"CATA420\",6)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20073,\"CATA420\",7)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20074,\"CATA420\",8)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20075,\"CATA420\",9)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20076,\"CATA420\",10)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20077,\"CATA420\",11)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20078,\"CATA420\",12)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20079,\"CATA420\",13)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20080,\"CATA420\",14)");
		db.execDML("insert into sysCategory2 (id,name,sysCategory1_id) values( 20081,\"CATA420\",15)");

	}

};
