#include<stdafx.h>
//#include"MainDlg.h"

//
//LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	{
//		// center the dialog on the screen
//		CenterWindow();
//
//		// set icons
//		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
//			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
//		SetIcon(hIcon, TRUE);
//		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
//			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
//		SetIcon(hIconSmall, FALSE);
//
//		// register object for message filtering and idle updates
//		CMessageLoop* pLoop = _Module.GetMessageLoop();
//		ATLASSERT(pLoop != NULL);
//		pLoop->AddMessageFilter(this);
//		pLoop->AddIdleHandler(this);
//
//		UIAddChildWindowContainer(m_hWnd);
//
//		return TRUE;
//	}
//
//	LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	{
//		// unregister message filtering and idle updates
//		CMessageLoop* pLoop = _Module.GetMessageLoop();
//		ATLASSERT(pLoop != NULL);
//		pLoop->RemoveMessageFilter(this);
//		pLoop->RemoveIdleHandler(this);
//
//		return 0;
//	}
//
//	LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	{
//		CAboutDlg dlg;
//		dlg.DoModal();
//		return 0;
//	}
//
//	LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	{
//		// TODO: Add validation code 
//		CloseDialog(wID);
//		return 0;
//	}
//
//	LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	{
//		CloseDialog(wID);
//		return 0;
//	}
//
//	void CMainDlg::CloseDialog(int nVal)
//	{
//		DestroyWindow();
//		::PostQuitMessage(nVal);
//	}
//	bool CMainDlg::CreateSystemTables(LPSTR lpStrPath)
//{
//	ATLASSERT (NULL != lpStrPath);
//	if (NULL == lpStrPath)
//		return false;
//
//	HANDLE hFile = CreateFileA(lpStrPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
//	if (INVALID_HANDLE_VALUE == hFile)
//		return false;
//
//	CloseHandle(hFile);
//
//	CppSQLite3DB db;
//	db.open(lpStrPath);
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysAppType` (\
//			   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
//			   `Name` VARCHAR(256) NOT NULL);");
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysBank` (\
//			   `id` VARCHAR(4) NOT NULL PRIMARY KEY,\
//			   `Name` VARCHAR(256) UNIQUE NOT NULL,\
//			   `classId` INTEGER NOT NULL DEFAULT(99),\
//			   `ShortName` VARCHAR(256) UNIQUE NOT NULL,\
//			   `Position` INTEGER NOT NULL,\
//			   `sysAppType_id` INTEGER NOT NULL);");
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysSubLink` (\
//			   `sysBank_id` VARCHAR(4) NOT NULL  ,\
//			   `URL` VARCHAR(256) UNIQUE NOT NULL,\
//			   `URLText` VARCHAR(256) NOT NULL,\
//			   `LinkOrder` INTEGER NOT NULL,\
//			   PRIMARY KEY(sysBank_id, URL));");
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysCategory1` (\
//			   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
//			   `Name` VARCHAR(256) NOT NULL,\
//			   `Type` TINYINTNOT NULL);");
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysCategory2` (\
//			   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
//			   `Name` VARCHAR(256) NOT NULL,\
//			   `sysCategory1_id` TINYINTNOT NULL);");
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysAccountType` (\
//			   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
//			   `Name` VARCHAR(256) NOT NULL);");
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysCurrency` (\
//			   `id` INTEGER NOT NULL PRIMARY KEY  AUTOINCREMENT ,\
//			   `Name` VARCHAR(256) NOT NULL);");
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysDBInfo` (\
//			   `schema_version` INTEGER NOT NULL);");
//
//	db.execDML("CREATE  TABLE IF NOT EXISTS `sysURLWhiteList` (\
//			   `sysBank_id` VARCHAR(4) NOT NULL PRIMARY KEY ,\
//			   `URLPattern` VARCHAR(256) NOT NULL);");
//
//	
//	db.execDML("INSERT INTO sysAppType (id, Name) VALUES (1, \"all\");");
//	db.execDML("INSERT INTO sysAppType (id, Name) VALUES (2, \"banks\");");
//	db.execDML("INSERT INTO sysAppType (id, Name) VALUES (3, \"securities\");");
//	db.execDML("INSERT INTO sysAppType (id, Name) VALUES (4, \"insurances\");");
//	db.execDML("INSERT INTO sysAppType (id, Name) VALUES (5, \"funds\");");
//	db.execDML("INSERT INTO sysAppType (id, Name) VALUES (6, \"payments\");");
//	db.execDML("INSERT INTO sysAppType (id, Name) VALUES (7, \"shopping\");");
//
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a001\", \"中国银行\", 0, \"boc\", 0, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a002\", \"中国农业银行\", 0, \"abchina\", 1, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a003\", \"中国工商银行\", 0, \"icbc\", 2, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a004\", \"中国建设银行\", 0, \"ccb\", 3, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a005\", \"交通银行\", 0, \"bankcomm\", 4, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a006\", \"招商银行\", 0, \"cmbchina\", 5, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a007\", \"中信银行\", 0, \"ecitic\", 6, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a008\", \"光大银行\", 0, \"cebbank\", 7, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a009\", \"浦发银行\", 0, \"spdb\", 8, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a010\", \"深圳发展银行\", 0, \"sdb\", 9, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a011\", \"广东发展银行\", 0, \"gdb\", 10, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a012\", \"华夏银行\", 0, \"hxb\", 11, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a013\", \"民生银行\", 0, \"cmbc\", 12, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a014\", \"兴业银行\", 0, \"cib\", 13, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a015\", \"北京银行\", 0, \"beijing\", 14, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a016\", \"北京农商银行\", 0, \"bjrcb\", 15, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a017\", \"中国邮政储蓄银行\", 0, \"psbc\", 16, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"a018\", \"平安银行\", 0, \"pinganbank\", 17, 2);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"b001\", \"中信证券\", 1, \"cs_ecitic\", 0, 3);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"c001\", \"中国人寿保险\", 1, \"chinalife\", 0, 4);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d001\", \"华夏基金\", 1, \"chinaamc\", 0, 5);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"d002\", \"招商基金\", 1, \"cmfchina\", 1, 5);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e001\", \"支付宝\", 2, \"alipay\", 0, 6);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e002\", \"首信易支付\", 2, \"payease\", 1, 6);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e003\", \"开联\", 2, \"lianxin\", 2, 6);");
//	db.execDML("INSERT INTO sysBank (id, Name, classId, ShortName, Position, sysAppType_id) VALUES (\"e004\", \"易宝支付\", 2, \"yeepay\", 3, 6);");
//	db.execDML("INSERT INTO sysBank (id, Name, ShortName, Position, sysAppType_id) VALUES (\"f001\", \"乐淘商城\", \"letao\", 0, 7);");
//	db.execDML("INSERT INTO sysBank (id, Name, ShortName, Position, sysAppType_id) VALUES (\"f002\", \"淘宝网\", \"taobao\", 1, 7);");
//
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a001\", \"http://www.boc.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a001\", \"https://ebs.boc.cn/BocnetClient/LoginFrame.do?_locale=zh_CN\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a002\", \"http://www.abchina.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a002\", \"http://www.abchina.com/cn/EBanking/Ebanklogin/PCustomerLogin/default.htm\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a003\", \"http://www.icbc.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a003\", \"https://mybank.icbc.com.cn/icbc/perbank/index.jsp\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a003\", \"https://vip.icbc.com.cn/\", \"贵宾版\", 2);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a004\", \"http://www.ccb.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a004\", \"https://ibsbjstar.ccb.com.cn/app/V5/CN/STY1/login.jsp\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a005\", \"http://www.bankcomm.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a005\", \"https://pbank.95559.com.cn/personbank/index.jsp\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a006\", \"http://www.cmbchina.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a006\", \"https://pbsz.ebank.cmbchina.com/CmbBank_GenShell/UI/GenShellPC/Login/Login.aspx\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a007\", \"http://bank.ecitic.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a007\", \"https://e.bank.ecitic.com/perbank5/signIn.do\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a008\", \"http://www.cebbank.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a008\", \"https://www.cebbank.com/per/prePerlogin1.do?_locale=zh_CN\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a008\", \"http://ebank.cebbank.com/preLogin.html\", \"专业版\", 2);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a009\", \"http://www.spdb.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a009\", \"http://ebank.spdb.com.cn/login/perlogin.html\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a010\", \"http://www.sdb.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a010\", \"https://ebank.sdb.com.cn/perbank/logon_pro.jsp\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a011\", \"http://www.gdb.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a011\", \"https://ebanks.gdb.com.cn/sperbank/perbankLogin.jsp\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a012\", \"http://www.hxb.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a012\", \"https://ebank.hxb.com.cn/HxPer/basPreLogin.do\", \"个人网银普通版\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a012\", \"https://ebank.hxb.com.cn/HxPer/regLogin01.do\", \"个人网银签约版\", 2);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a012\", \"https://dbank.hxb.com.cn/easybanking/jsp/indexCert.jsp\", \"个人网银证书版\", 3);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a013\", \"http://www.cmbc.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a013\", \"https://ebank.cmbc.com.cn/index_NonPrivate.html\", \"个人大众版\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a013\", \"https://business.cmbc.com.cn/index_Private.html\", \"贵宾版\", 2);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a013\", \"https://ebank.cmbc.com.cn/indexCCLogin.html\", \"信用卡\", 3);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a014\", \"http://www.cib.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a014\", \"https://www.cib.com.cn/index.jsp\", \"个人普通用户\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a014\", \"https://www.cib.com.cn/indexCert.jsp\", \"个人证书用户\", 2);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a015\", \"http://www.bankofbeijing.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a015\", \"https://ebank.bankofbeijing.com.cn/bccbpb/accountLogon.jsp\", \"个人普通用户\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a015\", \"https://ebank.bankofbeijing.com.cn/bccbpb/fortuneLogon.jsp\", \"个人财富用户\", 2);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a016\", \"http://www.bjrcb.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a016\", \"https://ibs.bjrcb.com/per/prelogin.do\", \"个人网银登陆\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a016\", \"https://ebank.bjrcb.com/ent/preloginCheque.do\", \"支付密码系统\", 3);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a017\", \"http://www.psbc.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a017\", \"https://pbank.psbc.com/\", \"个人网银\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a018\", \"http://bank.pingan.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"a018\", \"https://www.pingan.com.cn/pinganone/pa/ebanklogin.screen\", \"平安一账通\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"b001\", \"http://www.cs.ecitic.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"c001\", \"http://www.chinalife.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d001\", \"http://www.chinaamc.com/\", \"首页\", 0);");	
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d002\", \"http://www.cmfchina.com/index.html\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"d002\", \"https://direct.cmfchina.com/index.jsp\", \"基金网上交易\", 1);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e001\", \"http://www.alipay.com/\", \"首页\", 0);");	
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e002\", \"http://www.beijing.com.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e003\", \"http://www.openunion.cn/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"e004\", \"http://www.yeepay.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"f001\", \"http://www.letao.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"f002\", \"http://www.taobao.com/\", \"首页\", 0);");
//	db.execDML("INSERT INTO sysSubLink (sysBank_id, URL, URLText, LinkOrder) VALUES (\"f002\", \"http://www.tmall.com/\", \"淘宝商城\", 1);");
//	
//	
//	// 往系统中添加的用户模板数据
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (1, \"现金\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (2, \"信用卡\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (3, \"储蓄卡/存折\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (4, \"定期存单\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (5, \"支付\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (6, \"贷款\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (7, \"投资\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (8, \"借给别人的钱\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (9, \"欠别人的钱\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (10, \"固定资产\");");
//	db.execDML("INSERT INTO sysAccountType (id, Name) VALUES (11, \"其它\");");
//
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (1, \"人民币\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (2, \"美元\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (3, \"欧元\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (4, \"日元\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (5, \"英镑\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (6, \"港币\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (7, \"加拿大元\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (8, \"澳大利亚元\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (9, \"瑞士法郎\");");
//	db.execDML("INSERT INTO sysCurrency (id, Name) VALUES (10, \"新加坡元\");");
//
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (1, \"保险费\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (2, \"待报销\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (3, \"服饰\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (4, \"家具家电\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (5, \"健康\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (6, \"交通\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (7, \"缴税\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (8, \"教育\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (9, \"劳务支出\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (10, \"人情\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (11, \"日用品\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (12, \"食物\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (13, \"通讯\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (14, \"休闲娱乐\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (15, \"账单\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (16, \"工资\", 1);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (17, \"其它收入\", 1);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (18, \"转账支出\", 0);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (19, \"转账收入\", 1);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (20, \"信用卡收入\", 1);");
//	db.execDML("INSERT INTO sysCategory1 (id, Name, Type) VALUES (21, \"信用卡支出\", 0);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (1, \"健康\", 1);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (2, \"其它\", 1);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (3, \"养老\", 1);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (4, \"待报销\", 2);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (5, \"裤子\", 3);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (6, \"内衣\", 3);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (7, \"上衣\", 3);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (8, \"手表\", 3);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (9, \"鞋袜\", 3);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (10, \"电器\", 4);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (11, \"家具\", 4);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (12, \"健身\", 5);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (13, \"体检\", 5);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (14, \"医药\", 5);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (15, \"出租车\", 6);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (16, \"飞机票\", 6);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (17, \"公共交通\", 6);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (18, \"火车票\", 6);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (19, \"其它\", 6);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (20, \"自驾车费用\", 6);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (21, \"缴税\", 7);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (22, \"教材\", 8);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (23, \"学费\", 8);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (24, \"保姆\", 9);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (25, \"小时工\", 9);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (26, \"请客\", 10);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (27, \"送礼\", 10);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (28, \"家居用品\", 11);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (29, \"清洁用品\", 11);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (30, \"粮油\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (31, \"零食\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (32, \"其它\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (33, \"食堂\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (34, \"蔬菜\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (35, \"熟食\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (36, \"水果\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (37, \"外出饮食\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (38, \"烟酒\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (39, \"饮料\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (40, \"鱼肉蛋奶\", 12);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (41, \"固话费\", 13);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (42, \"手机费\", 13);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (43, \"旅游\", 14);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (44, \"其它\", 14);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (45, \"书报杂志光盘\", 14);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (46, \"影剧\", 14);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (47, \"游戏\", 14);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (48, \"房租\", 15);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (49, \"取暖费\", 15);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (50, \"水电气费\", 15);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (51, \"物业费\", 15);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (52, \"工资\", 16);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (53, \"奖金\", 16);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (54, \"报销\", 17);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (55, \"出租房屋\", 17);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (56, \"股票收入\", 17);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (57, \"礼物\", 17);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (58, \"利息\", 17);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (59, \"转账支出\", 18);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (60, \"转账收入\", 19);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (61, \"信用卡收入\", 20);");
//	db.execDML("INSERT INTO sysCategory2 (id, Name, sysCategory1_id) VALUES (62, \"信用卡支出\", 21);");
//
//	return false;
//
//}