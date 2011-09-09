/**
* @file    ksafesdk_loader.h
* @brief   ...
* @author  bbcallen
* @date    2010-08-06 11:34
*/

#ifndef KSAFESDK_LOADER_H
#define KSAFESDK_LOADER_H

#include <windows.h>
#include <objidl.h>
#include "ksafesdk_def.h"

//////////////////////////////////////////////////////////////////////////////

#ifndef	KSAFESDK_API
#define	KSAFESDK_API __declspec(dllimport)
#endif

typedef KSAFESDK_API HRESULT (__stdcall* PFN_ksafeInitialize)(
    /*[in]*/  PFNCALLBACKW      pCallback);
typedef KSAFESDK_API HRESULT (__stdcall* PFN_ksafeUninitialize)();
typedef KSAFESDK_API HRESULT (__stdcall* PFN_ksafeScanFileW)(
    /*[in]*/  const wchar_t*    pScanFileName,
    /*[in]*/  unsigned long     dwScanningMode,
    /*[in]*/  unsigned long     dwCleaningMode,
    /*[in]*/  void*             pUserContext,
    /*[out]*/ KSAFE_RESULT*     pResult);

//////////////////////////////////////////////////////////////////////////

extern PFN_ksafeInitialize      ksafeInitialize;
extern PFN_ksafeUninitialize    ksafeUninitialize;
extern PFN_ksafeScanFileW       ksafeScanFileW;

//////////////////////////////////////////////////////////////////////////////

HRESULT ksafeLoadW(const wchar_t* pLoadFolder);
HRESULT ksafeUnload();

//////////////////////////////////////////////////////////////////////////////

#endif//KSAFESDK_LOADER_H