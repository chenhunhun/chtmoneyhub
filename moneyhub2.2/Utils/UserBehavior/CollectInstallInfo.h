/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  CollectInstallInfo.h
 *      说明：  收集安装相关所有反馈信息的类的声明文件。
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.19	范振兴	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#include "../HardwareID/genhwid.h"
#include "../HardwareID/cpuid.h"
#include "../HardwareID/SMBiosStructs.h"
#include <string>
using namespace std;

/*
数据（data）：
软件环境信息：
	硬件ID				terminal_id
操作系统名称		system_name
操作系统版本		system_version
操作系统语言		system_lang
IE浏览器版本		ie_version
杀毒软件名称		antivirus_name
防火墙软件名称		firewall_name(可以先没有)
安防软件名称		
硬件环境信息
CPU Name，			cpu_name
System Manufacturer		product_dmi
Product Name			product_name 
物理内存大小			memory_size

*/
class CCollectInstallInfo
{
public:
	CCollectInstallInfo(void);
	~CCollectInstallInfo(void);
public:
	// 获得安装时的所有信息
	string GetInstallInfo();
private:
	// 获得硬件ID的函数（硬件id为公司内自定义，参考需求文档）
	void GetTerminal_id();
	// 获得系统名称及版本的函数
	void GetSystem_nameAversion();
	// 获得系统语言
	void GetSystem_lang();
	// 获得IE版本
	void GetIe_version();
	// 获得杀毒软件名称
	void GetAntivirus_name();

	// 获得CPU名称
	void GetCpu_name();
	// 获得产品生产厂商和型号名称
	void GetProduct_dmiAname();
	// 获得系统内存大小
	void GetMemory_size();
	// 将wstring类型变量转换为string类型
	string ws2s(const wstring& ws);

	// 记录xml信息的成员变量
	string m_sXmlInfo;
};
