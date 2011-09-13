#pragma once

/**
* 更新系统服务
* @param ServiceID 服务ID号
* @param ServiceProc 系统服务例程。
* return 返回原系统服务例程
*/
PVOID UpdateService(ULONG ServiceID, PVOID ServiceProc);

/**
* 检查服务函数
* @param id 服务ID号
* @param pFun 服务例程。
* return 返回服务是否正常工作
*/
bool _checkHookSafeMapV(PVOID pFun,ULONG id);