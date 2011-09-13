
#pragma once

#include "AltSkinClasses.h"
#include "..\..\Utils\VersionManager\versionManager.h"

class CAboutDlg : public CDialogImpl<CAboutDlg>, public CDialogSkinMixer<CAboutDlg>
{
public:
	enum { IDD = IDD_DIALOG_ABOUT };

	BEGIN_MSG_MAP(CAboutDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CAboutDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

public:

	template <class T>
	class CInsertToList
	{
	public:
		CInsertToList(HWND hListBox, T* pDlg) : m_pListBox(hListBox), m_pDlg(pDlg)
		{
		}

		~CInsertToList()
		{
			if (!m_pDlg->m_bShowGlobal && m_pListBox)
			{
				m_pDlg->m_bShowGlobal = true;
				for (int i = 2; i >= 0; --i)
				{
					if (!m_verGlobal[i].first.empty())
					{
						wchar_t szBuf[256];
						swprintf_s(szBuf, L"%s  %s", m_verGlobal[i].first.c_str(), m_verGlobal[i].second.c_str());
						m_pListBox.InsertString(0, szBuf);
					}
				}
			}
		}

		void operator()(std::pair<std::wstring, std::wstring> ver) 		
		{
			if (ver.first == _T("主程序") || ver.first == _T("Main"))
				m_verGlobal[0] = ver;
			else if (ver.first == _T("页面内容") || ver.first == _T("Html"))
				m_verGlobal[1] = ver;
			else if (ver.first == _T("配置信息") || ver.first == _T("Config"))
				m_verGlobal[2] = ver;
			else
			{
				wchar_t szBuf[256];
				swprintf_s(szBuf, L"%s  %s", ver.first.c_str(), ver.second.c_str());
				m_pListBox.InsertString(m_pListBox.GetCount(), szBuf);
			}
		}

	private:
		T* m_pDlg;
		CListBox m_pListBox;
		std::pair<std::wstring, std::wstring> m_verGlobal[3];
	};

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		SetWindowText(_T("关于财金汇"));

		ApplyButtonSkin(IDOK);

		if (NULL == m_imgLargeLogo)
			m_imgLargeLogo.LoadFromFile(_T("about.png"), true);

		m_bShowGlobal = false;

		VMMAPDEF vmap;
		if (versionManager().getAllVersion(vmap, ALLVERSION, false))
			for_each(vmap.begin(), vmap.end(), CInsertToList<CAboutDlg>(::GetDlgItem(m_hWnd, IDC_LIST_VERSION), this));

		return TRUE;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);
		if (NULL != m_imgLargeLogo)
			m_imgLargeLogo.Draw(dc, 11, 45, m_imgLargeLogo.GetWidth(), m_imgLargeLogo.GetHeight(), 0, 0);
		
		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDOK);
		return 0;
	}

protected:
	CTuoImage m_imgLargeLogo;
	bool m_bShowGlobal;
};
