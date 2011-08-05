#pragma once

#include "ntddk.h"

/**
* 取消写保护
* @param pOldAttr 分配好的空间用于保存前状态。
*/
VOID DisableWriteProtect(OUT PULONG pOldAttr);

/**
* 开启写保护
* @param uOldAttr 调用DisableWriteProtect时从参数返回的状态。
*/
VOID EnableWriteProtect(ULONG uOldAttr);