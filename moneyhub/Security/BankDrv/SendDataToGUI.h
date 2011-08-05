#pragma once

#define DATAPATHLEN  ( 260*sizeof(wchar_t) )
#define DATAHASHLEN  16

extern PKEVENT pEvent;
extern PKEVENT g_pEventFilterGo;

//数据格式为灰名单个数(DWORD) + 文件1名(MAX_PATH(260)*sizeof(WCHAR)) + md5值( 16 ) + 文件2名 + ...
/**
*
*/
bool setSigned();

/**
*
*/
bool setData(void * pPath, unsigned int pCLen, void* pHash, unsigned int pSLen);

/**
*
*/
void getData(void * pPath, void* pHash);
/**
*
*/
bool InitializeFilterGoEvent();