#pragma once

#include "windows.h"
#include "atlstr.h"
#include <vector>
#include <algorithm>
#include <string>

class CHistoryManager
{
protected:
	CHistoryManager(void);
	~CHistoryManager(void);

	static CHistoryManager* m_Instance;
	CStringW m_strRedirectPath;
	CStringW m_strWinTemp;

public:
	static CHistoryManager* GetInstance();

	BOOL Init();
	BOOL Finalize();

	CStringA Redirect(CStringA lpFileName);
	CStringW Redirect(CStringW lpFileName);

	BOOL CleanHistory();
	BOOL DeleteDir(CStringW strPath);

	BOOL CleanUpdate();

	BOOL PrepareTempDirs();
	void SplitPath(const std::wstring& strPath, std::vector<std::wstring>& vec);
};
