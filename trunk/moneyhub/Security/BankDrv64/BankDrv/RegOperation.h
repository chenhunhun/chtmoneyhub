#pragma once
#include "ntddk.h"

#define  AUTORUN        L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define  REGNAME        L"MoneyhubAgent"
#define  PROGRAMNAME    L"moneyhub_pop.exe" 
 
ULONG CreateReg( PCWSTR pKey) ;
//delete subItem ,if delete subKey ,using rtldeleteregistryKey
bool DeleteItemReg(const PCWSTR pKey );
bool DeleteItemKey(const PCWSTR pKey, const PCWSTR pSubKey);
bool SetReg(const PCWSTR pKey , const PCWSTR pSetKey);
bool QueryReg(const PCWSTR pKey, const PCWSTR pQueryKey, PCWSTR pBuf, ULONG size);