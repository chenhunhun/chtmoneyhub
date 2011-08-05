/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  AxHookManager.h
 *      说明：  控件钩子管理
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */

#pragma once

class CAxHookManager
{

public:

	CAxHookManager();

	static void Initialize();
	static CAxHookManager* Get();
	void Hook();
	void Unhook();

private:

	static LRESULT CALLBACK AxWndProc(int iCode, WPARAM wParam, LPARAM lParam);
	static DWORD sm_dwTLSIndex;
	HHOOK m_hAxHook;
};
