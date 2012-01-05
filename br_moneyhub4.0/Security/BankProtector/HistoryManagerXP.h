#pragma once

#include "windows.h"
#include "atlstr.h"
#include <algorithm>
#include <string>

class CHistoryManagerXP
{
public:
	CHistoryManagerXP(void);
	~CHistoryManagerXP(void);

	static CHistoryManagerXP* m_Instance;
	CStringW m_strRedirectPath;
	CStringW m_strWinTemp;

public:
	static CHistoryManagerXP* GetInstance();

	BOOL Init();
	BOOL Finalize();

	BOOL CleanHistory();
	BOOL DeleteDir(CStringW strPath);

	BOOL CleanUpdate();

	BOOL PrepareTempDirs();
	std::wstring GetRegKeyDirByHandle(HANDLE hHandle);
};
