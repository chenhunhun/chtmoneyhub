
#pragma once

#include "../Resource/resource.h"
#include "CoolMessageBox.h"
#include "AltSkinClasses.h"
#include "../../BankData/BankData.h"
using namespace std;
#define MH_PROGRESS_REFRESH 0xf3
class CSaveFavProgressDlg : public CDialogImpl<CSaveFavProgressDlg>, public CDialogSkinMixer<CSaveFavProgressDlg>
{
public:
	CSaveFavProgressDlg(string aid, wstring text):m_aid(aid), m_text(text)
	{
	}
	~CSaveFavProgressDlg(){}

public:
	enum { IDD = IDD_DIALOG_SAVEFAV_PROGRESS };

	BEGIN_MSG_MAP(CSaveFavProgressDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CSaveFavProgressDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
public:

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);
		s()->StatusBar()->DrawCheckProgress(dc, m_proRC, m_percent);

		return 0;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		SetWindowText(_T("财金汇"));
		//m_btnSysClose.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_PROGRESS1).GetWindowRect(&m_proRC);
		ScreenToClient(&m_proRC);
		GetDlgItem(IDC_PROGRESS1).ShowWindow(SW_HIDE);

		ApplyButtonSkin(IDCANCEL);
		ApplyStaticSkin(IDC_STATIC);

		SetDlgItemText(IDC_STATIC, m_text.c_str());
		m_percent = 0;

		SetTimer(MH_PROGRESS_REFRESH, 500, 0);

		return TRUE;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if(wParam == MH_PROGRESS_REFRESH)
		{
			if(m_aid != "")
			{
				int state = CBankData::GetInstance()->GetFavState(m_aid);
				if(state > 100)
					EndDialog(IDOK);
				if(state >= 0)
				{
					m_percent = state;
					InvalidateRect(&m_proRC);
					//m_pbPercent.SetPos(state);
				}
				if(state < -1)
					EndDialog(IDCANCEL);
			}
			else
				EndDialog(IDCANCEL);
		}

		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

protected:
	wstring m_text;// 显示的文字
	string m_aid;// 机构的编号

protected:
	RECT m_proRC;
	int m_percent;//百分比
};