#pragma once

//////////////////////////////////////////////////////////////////////////

#define		HASH_SIZE	16

extern bool g_isReceive;
/**
* 填加安全文件Hash
* @param 
* @param length 文件路径长度。
* return 返回是否填加成功。
*/
bool AddSecureHash(const unsigned char* hash);

/**
* 检查文件hash是否安全。
* @param hash 文件hash值。
* return 文件hash安全则返回true，否则返回false。
*/
bool IsHashSecure(const unsigned char* hash);

/**
* 清除hash    ==1 清除g_secuHash内容， ==2 清除全部
* return 返回是否清除成功。
*/
bool ClearHash(ULONG index = 1);

/**
* 判断是否注销
*/
bool isRestart();

/**
* 判断全局表归属
*/
void  getSecuTable();
/**
*创建互斥体
*/
void initialMutex();
