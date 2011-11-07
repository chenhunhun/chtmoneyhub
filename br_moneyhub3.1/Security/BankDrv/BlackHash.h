#pragma once



#define		HASH_SIZEB	16

extern bool g_isReceiveB;
/**
* 填加黑名单Hash
* @param 
* @param length 文件路径长度。
* return 返回是否填加成功。
*/
bool AddBlackHash(const unsigned char* hash);

/**
* 检查是否是黑名单哈希值。
* @param hash 文件hash值。
*/
bool IsHashBlack(const unsigned char* hash);

/**
* 清除hash    ==1 清除g_blackHash内容， ==2 清除全部
* return 返回是否清除成功。
*/
bool ClearBlackHash(ULONG index = 1);

/**
* 判断是否注销
*/
/*bool isRestartB();*/

/**
* 判断全局表归属
*/
void  getBlackTable();
/**
*创建互斥体
*/
void initialMutexB();
