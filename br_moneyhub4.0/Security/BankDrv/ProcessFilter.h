#pragma once

#include "windef.h"

extern KMUTEX   g_addPIDMutex;
/**
* 填加保护进程的ID
* @param PID 需要保护的进程ID。
* return 返回是否成功。
*/
BOOL AddProtectPID(ULONG PID);

/**
* 删除保护进程的ID
* @param PID 需要保护的进程ID。
* return 返回是否成功。
*/
BOOL RemoveProtectPID(ULONG PID);

/**
* 检查进程是否被保护
* @param PID 进程ID。
* return 返回是否成功。
*/
BOOL IsProcessProtected(ULONG PID);

/**
* 获取被保护的进程的数量
* return 返回被保护的进程的数量
*/
UINT32 GetPIDNumber();

/**
* 获取被保护的进程
* @param pPIDs 获取被保护的进程的缓冲区，需要在调用时分配好。
* @param count 缓冲区大小，一共可以保存count个PID。
* return 返回被保护的进程的数量。
*/
UINT32 GetKernelPIDs(UINT32* pPIDs, UINT32 count);
UINT32 GetPidOther(UINT32 *pPids, UINT32 count);
bool getPPidNum(UINT32 *uiPPid, UINT32 count, UINT32 iNum);
/**
* 进程退出回调整
* @param ParentId 父进程ID
* @param ProcessId 当前进程ID
* @param Create 是否是创建进程，TRUE则是，FALSE为进程退出。
*/
VOID OnProcessQuit(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create);

VOID initialMutexAddPID();


bool IsInstall(bool bCehck = false);