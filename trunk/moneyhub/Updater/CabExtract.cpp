
#include "stdafx.h"
#include "CabExtract.h"

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#pragma warning(disable: 4996)

CCabExtract::CCabExtract(BOOL bCreate)
{
	m_hFDIContext = NULL;
	m_hDll = NULL;

	memset(&m_tError, 0, sizeof(ERF));

	if (bCreate)
		FdiCreate();
}

CCabExtract::~CCabExtract(void)
{
	FdiDestroy(); 

	if (m_hDll)
		FreeLibrary(m_hDll);
}

BOOL CCabExtract::FdiCreate()
{
	memset(&m_tError, 0, sizeof(ERF));

	if (m_hFDIContext)
		return FALSE;

	if (!m_hDll)
	{
		m_hDll = LoadLibrary(_T("cabinet.dll"));
		m_fFdiCreate = (FDICREATEFUNC)GetProcAddress(m_hDll, "FDICreate");
		m_fFdiCopy = (FDICOPYFUNC)GetProcAddress(m_hDll, "FDICopy");
		m_fFdiIsCabinet = (FDIISCABINETFUNC)GetProcAddress(m_hDll, "FDIIsCabinet");
		m_fFdiDestroy = (FDIDESTROYFUNC)GetProcAddress(m_hDll, "FDIDestroy");

		if (!m_fFdiCreate || !m_fFdiCopy || !m_fFdiIsCabinet || !m_fFdiDestroy)
		{
			m_hDll = NULL;
			return FALSE;
		}
	}

	m_hFDIContext = m_fFdiCreate((PFNALLOC)(malloc),
								(PFNFREE)(free),
								(PFNOPEN)(_open),
								(PFNREAD)(_read),
								(PFNWRITE)(_write),
								(PFNCLOSE)(_close),
								(PFNSEEK)(_lseek),
								cpu80386,
								&m_tError);

	return (m_hFDIContext != NULL);
}

BOOL CCabExtract::FdiDestroy()
{
	if (!m_hFDIContext)
		return TRUE;

	BOOL bRet = m_fFdiDestroy(m_hFDIContext);
	m_hFDIContext = NULL;
	
	return bRet;
}

BOOL CCabExtract::FdiIsCabinet(LPCTSTR szCabFile)
{
	if (!m_hFDIContext)
		return FALSE;

	memset(&m_tError, 0, sizeof(ERF));

	USES_CONVERSION;

	int handle = _open(CT2A(szCabFile), _O_BINARY | _O_RDONLY | _O_SEQUENTIAL, 0);
	if (!handle)
		return FALSE;

	FDICABINETINFO fdici;
	BOOL bRet = m_fFdiIsCabinet(m_hFDIContext, handle, &fdici);

	_close(handle);

	return bRet;
}

BOOL CCabExtract::ExtractFile(LPCTSTR szCabFile, LPCTSTR szTargetDir)
{
	if (!m_hFDIContext)
		return FALSE;

	int nLen = _tcslen(szCabFile);
	if (nLen >= MAX_CAB_PATH) return FALSE;

	USES_CONVERSION;

	strcpy(m_szCabFile, CT2A(szCabFile));
	strcpy(m_szTargetDir, CT2A(szTargetDir));

	char* pSlash = strrchr(m_szCabFile, '\\');
	strcpy(m_szFile, pSlash + 1);
	strcpy(m_szPath, m_szCabFile);
	m_szPath[pSlash - m_szCabFile + 1] = 0;

	if (!m_fFdiCopy(m_hFDIContext, m_szFile, m_szPath, 0, (PFNFDINOTIFY)(FDICallback), NULL, this))
		return FALSE;

	return TRUE;
}

int CCabExtract::FDICallback(FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin)
{
	int nRet = 0;

	CCabExtract* pThis = (CCabExtract*)pfdin->pv;

	switch (fdint)
	{
	case fdintCABINET_INFO:
	case fdintNEXT_CABINET:
		break;

	case fdintCOPY_FILE:
		{
			char szFile[MAX_CAB_PATH];
			char szSubFolder[MAX_CAB_PATH];
			char szPath[MAX_CAB_PATH];
			char szFullPath[MAX_CAB_PATH];

			strcpy(szFile, pfdin->psz1);
			strcpy(szSubFolder, pfdin->psz1);

			char* pSlash = strrchr(szSubFolder, '\\');
			if (pSlash)
			{
				strcpy(szFile, pSlash + 1);
				*(pSlash + 1) = 0;
			}
			else
				szSubFolder[0] = 0;

			strcpy(szPath, pThis->m_szTargetDir);
			strcat(szPath, szSubFolder);
			strcpy(szFullPath, szPath);
			strcat(szFullPath, szFile);

			pThis->CreateFolderTree(szPath);
			
			nRet = _open(szFullPath,  _O_BINARY | _O_CREAT |  _O_TRUNC | _O_WRONLY | _O_SEQUENTIAL, _S_IREAD | _S_IWRITE);
		}
		break;

	case fdintCLOSE_FILE_INFO:
		{
			_close((int)pfdin->hf);

			nRet = 1;
		}
		break;
	}

	return nRet;
}

void CCabExtract::CreateFolderTree(char* szFolder)
{
	int nLen = strlen(szFolder);
	if (nLen >= MAX_CAB_PATH) return;
	
	char szParPath[MAX_CAB_PATH];
	int nStartPos = 0;

	char* ch = szFolder;
	while (ch = strchr(ch, '\\'))
	{
		strcpy(szParPath, szFolder);
		szParPath[ch - szFolder + 1] = 0;

		if (szParPath[0] != 0)
			CreateDirectoryA(szParPath, NULL);

		ch++;
	}
}

#pragma warning(default: 4996)