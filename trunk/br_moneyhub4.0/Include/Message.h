#pragma once
#define WM_MULTI_PROCESS_CREATE_NEW_PAGE			(WM_USER + 0x4000)

#define WM_GLOBAL_CREATE_NEW_WEB_PAGE				(WM_USER + 0x4001)
#define WM_MULTI_PROCESS_NOTIFY_AXUI_CREATED		(WM_USER + 0x4002)
#define WM_FAV_BANK_CHANGE							(WM_USER + 0x400A) // gao 当用户收藏银行时
#define WM_FAV_BANK_DOWNLOAD						(WM_USER + 0x400B) // gao 用户对收藏银行控件进行下载
#define WM_MY_MENU_CLICKED							(WM_USER + 0x400C) // gao 用户点击了自定义菜单
#define WM_USER_INFO_MENU_CLICKED					(WM_USER + 0x400D) // 登录后点击用户名的点击事件
#define WM_SHOW_USER_CLICKED_MENU					(WM_USER + 0x400E)
#define WM_GLOBAL_GET_EXIST_WEB_PAGE				(WM_USER + 0x4014)

#define WM_MAINFRAME_CLOSE							(WM_USER + 0x4015)

#define WM_ITEM_NOTIFY_CREATED						(WM_USER + 0x4003)
#define WM_ITEM_SET_MAIN_TOOLBAR					(WM_USER + 0x4004)
#define WM_ITEM_SET_STATUS							(WM_USER + 0x4005)
#define WM_ITEM_SET_TAB_TEXT						(WM_USER + 0x4006)
#define WM_ITEM_SET_TAB_URL							(WM_USER + 0x4007)
#define WM_ITEM_GET_AX_CONTROL_WND					(WM_USER + 0x4008)
#define WM_ITEM_CHECK_URL_REQUIRE					(WM_USER + 0x4009)
#define WM_ITEM_SET_SSL_STATE						(WM_USER + 0x4010)
#define WM_ITEM_AUTOCLOSE							(WM_USER + 0x4011)
#define WM_ITEM_SET_PAGE_PROGRESS					(WM_USER + 0x4012)
#define WM_ITEM_TOGGLE_CATECTRL						(WM_USER + 0x4013)


#define WM_TAB_AUTOCLOSE							(WM_USER + 0x4020)
//	取消获取账单
#define WM_CANCEL_GET_BILL							(WM_USER + 0x4021)
#define WM_GETTING_BILL								(WM_USER + 0x4022)
// 结束获取账单
#define WM_FINISH_GET_BILL							(WM_USER + 0x4023)
#define WM_SET_DISPLAYHWND							(WM_USER + 0x4024)
#define WM_AX_ACCOUNT_SELECT						(WM_USER + 0x4025) // 显示选择账单界面
#define WM_AX_GET_ALL_BILL							(WM_USER + 0x4026) // 将获得的账单交给js
#define WM_AX_SHOW_INFO_DLG							(WM_USER + 0x4027) // 导出账单的中间提示对话框
#define WM_AX_END_INFO_DLG							(WM_USER + 0x4028)
#define WM_AX_CANCEL_GETBILL						(WM_USER + 0x4029) // 取消用这个
#define WM_AX_EXCEED_GETBILL_TIME					(WM_USER + 0x4030) // 超时用这个
#define WM_RE_GETBILL								(WM_USER + 0x4031) // 导入账单重新打开首页
#define WM_AUTO_USER_DLG							(WM_USER + 0x4032) // 打开或关闭（用户注册、登陆）对话框
#define WM_UPDATE_USER_STATUS						(WM_USER + 0x4033) // 更新用户状态
#define WM_USER_INFO_CLICKED						(WM_USER + 0x4034) // 红色叹号点击事件
#define	WM_SHOW_USER_DLG							(WM_USER + 0x4035) // 显示用户注册登陆等等对话框
#define WM_CHANGE_USER_DLG_NAME						(WM_USER + 0x4036) // 更改框架名称（JS调用）
#define WM_AX_LOAD_USER_QUIT						(WM_USER + 0x4037) // 用户退出登录
#define WM_CHANGE_FIRST_PAGE_SHOW					(WM_USER + 0x4038) // 根据用户是否登录，在首页显示不同内容
#define WM_AX_CHANGE_SETTINT_STATUS					(WM_USER + 0x4039) // 通知内核用户进入设置时显示的界面
#define WM_RESEND_VERIFY_MAIL						(WM_USER + 0x403A) // 重发邮件
#define WM_NOTIFYUI_CLOSE_CUR_USER					(WM_USER + 0x403B) // 通知UI关闭当前用户库
#define WM_NOTIFYUI_UPDATE_USER_DB					(WM_USER + 0x403C) // 通知UI更新当前用户库
#define WM_NOTIFYUI_SYNCHRO_BTN_CHANGE				(WM_USER + 0x403D) // 通知UI同步按钮更改颜色
#define WM_AX_CALL_JS_SHOW_TEXT						(WM_USER + 0x403E) // 调用JS的显示文字
#define WM_ITEM_ASK_SAVE_FAV						(WM_USER + 0x4040)
#define WM_ITEM_TEST_SAVE_FAV						(WM_USER + 0x4041)
#define WM_AX_INIT_SYNCHRO_BTN						(WM_USER + 0x4042) // 初始化同步按钮状态
#define WM_NOTIFYUI_CUR_USER_STOKEN					(WM_USER + 0x4043) // 通知UI更新当前用户stoken
#define WM_SET_JSPARAM       						(WM_USER + 0x4044) // 更新界面


#define WM_AX_NAVIGATE								(WM_USER + 0x5000)
#define WM_AX_GET_WEBBROWSER2_CROSS_THREAD			(WM_USER + 0x5001)

#define WM_AX_GOBACK								(WM_USER + 0x5002)
#define WM_AX_GOFORWARD								(WM_USER + 0x5003)
#define WM_AX_REFRESH								(WM_USER + 0x5004)
#define WM_AX_SSLSTATUS								(WM_USER + 0x5005)


#define WM_AX_FRAME_SETALARM						(WM_USER + 0x5006)
#define WM_AX_FRAME_ADDFAV							(WM_USER + 0x5007) // 添加收藏
#define WM_AX_FRAME_CHANGE_PROGRESS					(WM_USER + 0x5008) // 更新进度
#define WM_AX_TOOLS_CHANGE							(WM_USER + 0x5009) // 更新事件
#define WM_AX_MONTH_SELECT_DLG						(WM_USER + 0x500A) // 弹出先择对话框
#define WM_AX_CALL_JS_TABACTIVE						(WM_USER + 0x500B) // 调用JS TabActivated函数
#define WM_AX_FRAME_DELETEFAV						(WM_USER + 0x5011) // 取消收藏
#define WM_AX_USER_AUTO_LOAD						(WM_USER + 0x5012) // 用户自动登录
//#define WM_FINIHS_AUTO_LOAD							(WM_USER + 0x5013) // 自动登录检测完成

#define WM_CLOUDALARM								(WM_USER + 0x2100)
#define WM_CLOUDCLEAR								(WM_USER + 0x2101)
#define WM_CLOUDCHECK								(WM_USER + 0x2102)
#define WM_CLOUDNCHECK								(WM_USER + 0x2103)
#define WM_CLOUDNDESTORY							(WM_USER + 0x2104)
#define WM_RESTARTMONHUB							(WM_USER + 0x2105)

#define WM_CANCEL_ADDFAV							(WM_USER + 0x2140)// 取消收藏
#define WM_AX_GET_BILL								(WM_USER + 0x2141)
#define WM_AX_CLOSE_GET_BILL						(WM_USER + 0x2142)

#define WM_USB_ADD_FAV								(WM_USER + 0x2200)
#define WM_USB_CHANGE								(WM_USER + 0x2201)
#define WM_MONEYHUB_UAC								(WM_USER + 0x2204)
#define WM_MONEYHUB_FEEDBACK						(WM_USER + 0x2210)//财金汇反馈

#define WM_MYDANGEROUS                              (WM_USER + 0x2202)//检测到危险，显示提示


// 跨进程间的一些常量定义
#define MY_PARAM_END_TAG								"#" // 多个参数合并用的分隔符（包括和PHP通讯，内核和UI）
#define MY_TAG_LOAD_DLG									1 // 表示登录对话框
#define MY_TAG_REGISTER_DLG								2 // 表示注册对话框
#define MY_TAG_SETTING_DLG								3 // 表示设置对话框
#define MY_TAG_REGISTER_GUIDE							4 // 表示注册向导对话框
//#define MY_TAG_SEND_OPT								5 // 通知服务器发送OPT到指定的邮箱中
//#define MY_TAG_CHECK_OPT								6 // 校验OPT
#define MY_TAG_INIT_PWD									7 // 重置密码

//#define MY_STATUE_USER_LAODED							1 // 用户处于登录状态
//#define MY_STATUE_USER_NOTLOAD							2 // 用户处于未登录状态

#define MY_STATUE_SHOW_DLG								1 // 打开窗口
#define MY_STATUE_CLOSE_DLG								2 // 关闭窗口
#define MY_STATUS_HIDE_DLG								3 // 隐藏窗口

#define MY_USER_INFO_MENU_CLICK_MAILCHANGE				1 // 修改邮箱
#define MY_USER_INFO_MENU_CLICK_PWDCHANGE				2 // 修改密码
#define MY_USER_INFO_MENU_CLICK_QUIT					3 // 退出

#define MY_MAIL_VERIFY_SUCC								"46" // 服务器发送回的值，表示邮件认证成功
#define MY_MAIL_VERIFY_ERROR							"47" // 服务器发送回的值，表示邮件未认证或论证失败

#define REGEDIT_MONHUB_PATH								"Software\\Bank\\Setting"
#define MONHUB_GUIDEINFO_KEY							"GInfo"
#define MONHUB_GUEST_USERID								"Guest"