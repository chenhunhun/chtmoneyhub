#pragma once

#include <fdi.h>

#define MAX_CAB_PATH (256)

typedef HFDI (DIAMONDAPI* FDICREATEFUNC)(PFNALLOC, PFNFREE, PFNOPEN, PFNREAD, PFNWRITE, PFNCLOSE, PFNSEEK, int, PERF);
typedef BOOL (DIAMONDAPI* FDICOPYFUNC)(HFDI hfdi, char*, char*, int, PFNFDINOTIFY, PFNFDIDECRYPT, void*);
typedef BOOL (DIAMONDAPI* FDIISCABINETFUNC)(HFDI, int, PFDICABINETINFO);
typedef BOOL (DIAMONDAPI* FDIDESTROYFUNC)(HFDI);

class CCabExtract
{
public:
	CCabExtract(BOOL bCreate = TRUE);
	~CCabExtract(void);

public:
	BOOL FdiCreate();
	BOOL FdiDestroy();
	BOOL FdiIsCabinet(LPCTSTR szCabFile);
	BOOL ExtractFile(LPCTSTR szCabFile, LPCTSTR szTargetDir);

protected:
	static int FDICallback(FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin);
	static void CreateFolderTree(char* szFolder);

protected:	
	HINSTANCE m_hDll;
	HFDI m_hFDIContext;
	ERF m_tError;

	char m_szCabFile[MAX_CAB_PATH];
	char m_szTargetDir[MAX_CAB_PATH];
	char m_szPath[MAX_CAB_PATH];
	char m_szFile[MAX_CAB_PATH];

	FDICREATEFUNC m_fFdiCreate;
	FDICOPYFUNC m_fFdiCopy;
	FDIISCABINETFUNC m_fFdiIsCabinet;
	FDIDESTROYFUNC m_fFdiDestroy;
};
