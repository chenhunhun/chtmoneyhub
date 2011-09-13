#pragma once
#include "ntddk.h"

bool      _checkHookSafeOpenP();
NTSTATUS  sstHook_OpenProcess();
NTSTATUS  sstUnhook_OpenProcess();



