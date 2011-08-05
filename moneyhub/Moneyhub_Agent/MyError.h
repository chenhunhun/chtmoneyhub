#pragma  once

#define MY_PRO_NAME						L"MoyHubSvr"

//关键线程
#define MY_THREAD_ID_INIT					2001 //相关数据初始化
#define MY_THREAD_DRIVER_COM				2002 //和驱动通信
#define MY_THREAD_BANK_UAC					2003 //uac
#define MY_THREAD_USBKEY_CHECK			    2004 //usbkey检测
#define MY_THREAD_DLG_FUC					2005 //定时的功能实现
#define MY_THREAD_UI_COM					2006 //和ui通信
#define MY_THREAD_CATHE						2007

#define MY_ERROR_SQL_ERROR					   3000 //sqlite错误
#define MY_ERROR_RUNTIME_ERROR				   3001 //sqlite接口中的runtime错误