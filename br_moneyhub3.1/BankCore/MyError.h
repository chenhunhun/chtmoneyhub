#pragma  once

#define MY_PRO_NAME						L"MoyHubIE"

//关键线程
#define MY_THREAD_ID_INIT					2003 //相关数据初始化
#define MY_THREAD_IE_STATE					2004 //IE相关
#define MY_THREAD_BANK_DOWNLOAD				2005 //下载安装银行控件过程
#define MY_THREAD_USBKEY_CHECK			    2006 //usbkey下载安装过程
#define MY_THREAD_IE_EXTERNEL				2007 //ie扩展接口过程
#define MY_THREAD_GET_BILL					2008 //获取账单相关

// gao
#define MY_ERROR_PRO_CORE                      L"moneyhubCore"
#define MY_ERROR_PRO_UI                      L"moneyhubUI" // listmanager中要区分是内核还是UI
enum {
	// URL解析失败
	ERR_INTOPEN = 3000,  // internet open error
	ERR_INTCONNECT, // internet connect error
	ERR_INTOPENREQ, // internet open request error
	ERR_INTSENDREQ, // internet send request error
	ERR_INTADDREQHEAD, // internet add requerst header error
	ERR_INTQUREYINFO,// internet query info error
	ERR_INTFILENOTFOUND, // 网络文件不可用
	ERR_INTQUERYDATAAVAILABLE, // internte query data available error
	ERR_INTREADFILE, // internet read file error
	ERR_INTWRITEFILE, // internet write file error
	ERR_OUTOFTIME, // out of time error
	ERR_CATCH,
	ERR_SETOPTION,

	ERR_RENAMEFILE,
	ERR_CREATEPROCESS,
	ERR_CREATEFILE, // create file error
	ERR_WRITEFILE, // write file error
	ERR_SETFILEPOINTER, // set file pointer error

	ERR_SETUP_CAB_OUTOFTIME, // 安装CAB包安装超时
	ERR_SETUP_EXE_OUTOFTIME, // 安装exe文件超时
	ERR_READ_XML_CTRL, // 读取银行控件的XML文件出错
	ERR_READ_XML_USBKEY, // 读取USBKEY的XML文件出错
	ERR_DOWNLOAD_XML_CTRL, // 下载银行控件的XML文件出错
	ERR_DOWNLOAD_XML_USBKEY, // 下载USBKEY的XML文件出错
	ERR_DOWNLOAD_URL_NOFOUND_CTRL, // 没有读取到银行控件的下载链接
	ERR_DOWNLOAD_URL_NOFOUND_USBKEY, // 没有读取到USBKEY的下载链接
	ERR_CHECK_DOWNLOAD_FILE,
	ERR_SETUP_DOWNLOAD_FILE,

	ERR_XML_FILE,
	ERR_UNHANDLE_EXCEPT, // 未处理的异常

};