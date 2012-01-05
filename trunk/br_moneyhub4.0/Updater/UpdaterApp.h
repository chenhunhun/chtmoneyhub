#pragma once

class CUpdaterApp : public CAppModule
{
public:
	CUpdaterApp() : m_bByUserClick(FALSE), m_hOuterParent(NULL)
	{
		m_szModulePath[0] = 0;
	}

	LPCTSTR GetModulePath()
	{
		static bool bInitPath = false;
		if (!bInitPath)
		{
			::GetModuleFileName(NULL, m_szModulePath, MAX_PATH);
			TCHAR* pSlash = _tcsrchr(m_szModulePath, '\\');
			*pSlash = '\0';

			bInitPath = true;
		}

		return m_szModulePath;
	}

protected:
	TCHAR m_szModulePath[MAX_PATH + 1];

public:
	BOOL m_bByUserClick;
	HWND m_hOuterParent;
};