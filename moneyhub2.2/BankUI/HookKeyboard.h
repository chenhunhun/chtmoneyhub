#pragma once
/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  HookKeyboard
 *      说明：  一个拷屏防键盘操作类。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	 说明
 *	1.0.0	2010.11.11	融信恒通	初始版本
 *	
 *-----------------------------------------------------------*
 */

 namespace HOOKKEY
 {
	void addNPB(/*std::wstring wcsName*/);	
	bool installHook();
	bool uninstallHook(bool b = false);
	//禁用打印按键
	bool    disablePrintKey();
	bool    EnablePrintKey();
};

extern wchar_t  g_noHookfilterUrl[3000];