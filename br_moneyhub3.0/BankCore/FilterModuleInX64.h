/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  CFilterModuleInX64
 *      说明：  一个内核挂钩类。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2011.02.12	毕海	初始版本
 *	
 *-----------------------------------------------------------*
 */

#pragma once

enum  RETURNTYPE
{
	ISBLACK = 0,
	ISWHITE,
	ISGRAY,
};

#define HASH64  16
struct checkModuleX64
{
	unsigned char md5[HASH64];

	bool operator <(const checkModuleX64& vi) const
	{
		return memcmp(md5, vi.md5, HASH64) < 0;
	}
};
typedef std::set<checkModuleX64> CheckModuleX64;;

class CFilterModuleInX64
{
public:
	CFilterModuleInX64(void);
	~CFilterModuleInX64(void);
public:
	/**
	*
	*/
	bool InitializeHook();

	/**
	*
	*/

private:
	static bool m_bRunOnce ;
	
};



extern CFilterModuleInX64 g_filterModuleInX64App;