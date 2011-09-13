
#pragma once

#include "AltSkinClasses.h"
#include "..\BankData\BankData.h"
#include "CoolMessageBox.h"
#define RADIO_SIZE 13

class CSettingDlg : public CDialogImpl<CSettingDlg>, public CDialogSkinMixer<CSettingDlg>
{
private:
	enum TAB_PAGE_ENUM
	{
		emTPEPassword = -1,
		emTPERemider,
		emTPEUpDate,
	};
public:
	enum { IDD = IDD_DIALOG_SETTING };

	BEGIN_MSG_MAP(CSettingDlg)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
	//	COMMAND_HANDLER(IDC_EDIT_OLDPWD, EN_CHANGE, //OnEnChangeEditOldpwd)
		CHAIN_MSG_MAP(CDialogSkinMixer<CSettingDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtlColorStatic)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)

		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_TABCTRL, OnTabCtrl)
		COMMAND_RANGE_HANDLER(IDC_RADIO_REMINDER_YES, IDC_RADIO_PASSWORD_NO, OnRadioChange)
		//MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnChangeBackColor)
		//MESSAGE_HANDLER(WM_CTLCOLORBTN, OnChangeBackColor);
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		UINT nID = ::GetDlgCtrlID((HWND)lParam);
		if (nID == IDC_STATIC
			|| nID == IDC_STATIC_CURPWD
			|| nID == IDC_STATIC_NEWPWD
			|| nID == IDC_STATIC_NEWPWD2
			|| nID == IDC_STATIC_DELPWD
			|| nID == IDC_TEXT_REMINDER_YES
			|| nID == IDC_TEXT_REMINDER_NO
			|| nID == IDC_TEXT_UPDATE_YES
			|| nID == IDC_TEXT_UPDATE_NO
			|| nID == IDC_TEXT_PASSWORD_YES
			|| nID == IDC_TEXT_PASSWORD_NO
			|| nID == IDC_RADIO_REMINDER_YES
			|| nID == IDC_RADIO_REMINDER_NO
			|| nID == IDC_RADIO_UPDATE_YES
			|| nID == IDC_RADIO_UPDATE_NO
			|| nID == IDC_RADIO_PASSWORD_YES
			|| nID == IDC_RADIO_PASSWORD_NO)
		{
			CDCHandle dc((HDC)wParam);
			dc.SetTextColor(RGB(61, 98, 123));
			return LRESULT(m_bkBrush); 
		}

		return NULL;
	}

	//处理第一次不能隐藏窗口的问题
	LRESULT OnNcPaint(WORD /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		static int time = 2;

		if(time > 0)
		{
			if (m_bIsCreatePwd)
				::SetFocus (GetDlgItem (IDC_EDIT_NEWPWD));
			else
				::SetFocus (GetDlgItem (IDC_EDIT_OLDPWD));
			time --;
		}
		return 0;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		SetWindowText(_T("设置"));

		m_TabCtrl = GetDlgItem(IDC_TABCTRL);
		m_TabCtrl.ApplySkin(NULL, &s()->Toolbar()->m_bmpListTabCtrlItems, 3);

//		m_TabCtrl.AddTab(_T("密码"));
		m_TabCtrl.AddTab(_T("提醒"));
		m_TabCtrl.AddTab(_T("更新"));

		ApplyButtonSkin(IDOK);
		ApplyButtonSkin(IDCANCEL);

		m_bkBrush = ::CreateSolidBrush(RGB(247,252,255));//暂时用取色的方式设置按钮的背景
		m_bRemiderYes = IsReminderEnabled() ? true : false;
		m_bUpdateYes = IsAutoRunUpdate() ? true : false;

		CBankData* pBankData = CBankData::GetInstance();
		m_bIsCreatePwd = pBankData->GetPwd().empty();
		m_bCreateOrModifyPwd = true;
		CRect rec;
		::GetClientRect(GetDlgItem(IDC_RADIO_REMINDER_YES),&rec);
		::GetClientRect(GetDlgItem(IDC_RADIO_REMINDER_NO),&rec);
		::GetClientRect(GetDlgItem(IDC_RADIO_PASSWORD_YES),&rec);
		::GetClientRect(GetDlgItem(IDC_RADIO_PASSWORD_NO),&rec);
		::GetClientRect(GetDlgItem(IDC_RADIO_UPDATE_YES),&rec);
		::GetClientRect(GetDlgItem(IDC_RADIO_UPDATE_NO),&rec);

		int nChars = 20;
		::SendMessage(GetDlgItem(IDC_EDIT_OLDPWD), EM_LIMITTEXT, nChars, 0L);
		::SendMessage(GetDlgItem(IDC_EDIT_NEWPWD), EM_LIMITTEXT, nChars, 0L);
		::SendMessage(GetDlgItem(IDC_EDIT_NEWPWD2), EM_LIMITTEXT, nChars, 0L);
		::SendMessage(GetDlgItem(IDC_EDIT_DELPWD), EM_LIMITTEXT, nChars, 0L);

//		ShowPagePassword();
		ShowPageRemider ();

		return TRUE;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_TabCtrl.GetCurTab() == emTPERemider)
		{
			BOOL bEnabled = m_bRemiderYes ? TRUE : FALSE;
			::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Setting"),
				_T("ReminderEnabled"), REG_DWORD, &bEnabled, sizeof(DWORD));

		}
		else if (m_TabCtrl.GetCurTab() == emTPEUpDate)
		{
			BOOL bEnabled = m_bUpdateYes ? TRUE : FALSE;
			::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"),
			_T("AutoRun"), REG_DWORD, &bEnabled, sizeof(DWORD));
		}
		else if (m_TabCtrl.GetCurTab() == emTPEPassword)
		{
			if (m_bIsCreatePwd)
			{
				CString strNewPwd;
				CString strConfirm;
				CString strTitle = _T("创建密码");

				GetDlgItemText(IDC_EDIT_NEWPWD, strNewPwd);
				GetDlgItemText(IDC_EDIT_NEWPWD2, strConfirm);

				if (strNewPwd.IsEmpty())
				{
					mhMessageBox(GetActiveWindow(), _T("新密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
					::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD));
					return 0;
				}

				if (strNewPwd.GetLength() < 6)
				{
					mhMessageBox(GetActiveWindow(), _T("新密码字符数不能小于6，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
					::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD));
					SetDlgItemText (IDC_EDIT_NEWPWD, L"");
					return 0;
				}

				if (strConfirm.IsEmpty())
				{
					mhMessageBox(GetActiveWindow(), _T("确认新密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
					::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD2));
					return 0;
				}

				if (strConfirm != strNewPwd)
				{
					mhMessageBox(GetActiveWindow(), _T("确认新密码不正确，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
					::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD2));
					SetDlgItemText (IDC_EDIT_NEWPWD2, L"");
					return 0;
				}

				USES_CONVERSION;
				std::string strPwd = CT2A(strNewPwd);
				CBankData* pBankData = CBankData::GetInstance();
				pBankData->SavePwd(strPwd);
			}
			else
			{
				if (m_bCreateOrModifyPwd)
				{
					CBankData* pBankData = CBankData::GetInstance();
					std::string strPwd = pBankData->GetPwd();

					CString strOldPwd;
					CString strNewPwd;
					CString strConfirm;
					CString strTitle = _T("修改密码");

					GetDlgItemText(IDC_EDIT_OLDPWD, strOldPwd);
					GetDlgItemText(IDC_EDIT_NEWPWD, strNewPwd);
					GetDlgItemText(IDC_EDIT_NEWPWD2, strConfirm);

					USES_CONVERSION;
					if (strOldPwd.IsEmpty() || strOldPwd.Compare(A2CT(strPwd.c_str())) != 0)
					{
						mhMessageBox(GetActiveWindow(), _T("当前密码不正确，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
						::SetFocus(GetDlgItem(IDC_EDIT_OLDPWD));
						SetDlgItemText (IDC_EDIT_OLDPWD, L"");
						return 0;
					}

					if (strNewPwd.IsEmpty())
					{
						mhMessageBox(GetActiveWindow(), _T("新密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
						::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD));
						return 0;
					}

					if (strNewPwd.GetLength() < 6)
					{
						mhMessageBox(GetActiveWindow(), _T("新密码字符数不能小于6，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
						::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD));
						SetDlgItemText (IDC_EDIT_NEWPWD, L"");
						return 0;
					}

					if (strConfirm.IsEmpty())
					{
						mhMessageBox(GetActiveWindow(), _T("确认新密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
						::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD2));
						return 0;
					}

					if (strConfirm != strNewPwd)
					{
						mhMessageBox(GetActiveWindow(), _T("确认新密码不正确，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
						::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD2));
						SetDlgItemText (IDC_EDIT_NEWPWD2, L"");
						return 0;
					}

					strPwd = CT2A(strNewPwd);
					pBankData->SavePwd(strPwd);
				}
				else
				{
					CBankData* pBankData = CBankData::GetInstance();
					std::string strPwd = pBankData->GetPwd();

					CString strOldPwd;
					CString strTitle = _T("删除密码");

					GetDlgItemText(IDC_EDIT_DELPWD, strOldPwd);

					if (strOldPwd.IsEmpty())
					{
						mhMessageBox(GetActiveWindow(), _T("当前密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
						::SetFocus(GetDlgItem(IDC_EDIT_DELPWD));
						return 0;
					}

					USES_CONVERSION;
					if (strOldPwd.Compare(A2CT(strPwd.c_str())) != 0)
					{
						mhMessageBox(GetActiveWindow(), _T("当前密码不正确."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
						::SetFocus(GetDlgItem(IDC_EDIT_DELPWD));
						SetDlgItemText (IDC_EDIT_DELPWD, L"");
						return 0;
					}

					pBankData->SavePwd("");
				}
			}
		}

		EndDialog(IDOK);

		return 0;
	}


	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	LRESULT OnTabCtrl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int nCurTab = m_TabCtrl.GetCurTab();

		if (nCurTab == emTPERemider)
			ShowPageRemider();
		else if (nCurTab == emTPEUpDate)
			ShowPageUpdate();
		else if (nCurTab == emTPEPassword)
			ShowPagePassword();

		return 0;
	}

	LRESULT OnRadioChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (wID == IDC_RADIO_REMINDER_YES)
		{
			m_bRemiderYes = true;
			
			CheckDlgButton(IDC_RADIO_REMINDER_NO, BST_UNCHECKED);
		}
		else if (wID == IDC_RADIO_REMINDER_NO)
		{
			m_bRemiderYes = false;
			
			CheckDlgButton(IDC_RADIO_REMINDER_YES, BST_UNCHECKED);
		}
		else if (wID == IDC_RADIO_UPDATE_YES)
		{
			m_bUpdateYes = true;
			
			CheckDlgButton(IDC_RADIO_UPDATE_NO, BST_UNCHECKED);
		}
		else if (wID == IDC_RADIO_UPDATE_NO)
		{
			m_bUpdateYes = false;
			
			CheckDlgButton(IDC_RADIO_UPDATE_YES, BST_UNCHECKED);
		}
		else if (wID == IDC_RADIO_PASSWORD_YES)
		{
			m_bCreateOrModifyPwd = true;

			CheckDlgButton(IDC_RADIO_PASSWORD_NO, BST_UNCHECKED);
			
			::EnableWindow(GetDlgItem(IDC_EDIT_OLDPWD), TRUE);
			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD), TRUE);
			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD2), TRUE);
			::EnableWindow(GetDlgItem(IDC_EDIT_DELPWD), FALSE);
			::SetFocus (GetDlgItem (IDC_EDIT_OLDPWD));
		}
		else if (wID == IDC_RADIO_PASSWORD_NO)
		{
			m_bCreateOrModifyPwd = false;

			CheckDlgButton(IDC_RADIO_PASSWORD_YES, BST_UNCHECKED);

			::EnableWindow(GetDlgItem(IDC_EDIT_OLDPWD), FALSE);
			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD), FALSE);
			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD2), FALSE);
			::EnableWindow(GetDlgItem(IDC_EDIT_DELPWD), TRUE);
			::SetFocus (GetDlgItem (IDC_EDIT_DELPWD));
		}

		return 0;
	}

protected:
	CSkinListTabCtrl m_TabCtrl;

	bool m_bRemiderYes;
	bool m_bUpdateYes;
	bool m_bCreateOrModifyPwd;
	bool m_bIsCreatePwd;

	CRect m_rcTitleBar;
	CSkinButton m_btnOK;
	CSkinButton m_btnCancel;
	HBRUSH m_bkBrush;

public:

#define CTRL_LINE_HEIGHT (30)
#define CTRL_LINE_SPACE (90)

	void ShowPageRemider()
	{
		// Position
		::SetWindowPos(GetDlgItem(IDC_RADIO_REMINDER_YES), NULL, 200, 70, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
		::SetWindowPos(GetDlgItem(IDC_TEXT_REMINDER_YES), NULL, 220, 70, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
		::SetWindowPos(GetDlgItem(IDC_RADIO_REMINDER_NO), NULL, 200, 70 + CTRL_LINE_HEIGHT, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
		::SetWindowPos(GetDlgItem(IDC_TEXT_REMINDER_NO), NULL, 220, 70 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);

		if (m_bRemiderYes)
		{
			CheckDlgButton(IDC_RADIO_REMINDER_YES, BST_CHECKED);
			CheckDlgButton(IDC_RADIO_REMINDER_NO, BST_UNCHECKED);
		}
		else
		{
			CheckDlgButton(IDC_RADIO_REMINDER_YES, BST_UNCHECKED);
			CheckDlgButton(IDC_RADIO_REMINDER_NO, BST_CHECKED);
		}

		// Show or hide
		::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_YES), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_NO), SW_SHOW);

		::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_YES), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_NO), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_YES), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_YES), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_NO), SW_SHOW);

		::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_YES), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_NO), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_YES), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_STATIC_CURPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EDIT_OLDPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_STATIC_NEWPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EDIT_NEWPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_STATIC_NEWPWD2), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EDIT_NEWPWD2), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_NO), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_NO), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_STATIC_DELPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EDIT_DELPWD), SW_HIDE);
	}
	
	void ShowPageUpdate()
	{
		// Position
		::SetWindowPos(GetDlgItem(IDC_RADIO_UPDATE_YES), NULL, 200, 70, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
		::SetWindowPos(GetDlgItem(IDC_TEXT_UPDATE_YES), NULL, 220, 70, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
		::SetWindowPos(GetDlgItem(IDC_RADIO_UPDATE_NO), NULL, 200, 70 + CTRL_LINE_HEIGHT, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
		::SetWindowPos(GetDlgItem(IDC_TEXT_UPDATE_NO), NULL, 220, 70 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);

		if (m_bUpdateYes)
		{
			CheckDlgButton(IDC_RADIO_UPDATE_YES, BST_CHECKED);
			CheckDlgButton(IDC_RADIO_UPDATE_NO, BST_UNCHECKED);
		}
		else
		{
			CheckDlgButton(IDC_RADIO_UPDATE_YES, BST_UNCHECKED);
			CheckDlgButton(IDC_RADIO_UPDATE_NO, BST_CHECKED);
		}

		// Show or hide
		::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_YES), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_NO), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_YES), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_NO), SW_SHOW);

		::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_YES), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_YES), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_NO), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_YES), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_NO), SW_SHOW);

		::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_YES), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_STATIC_CURPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EDIT_OLDPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_STATIC_NEWPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EDIT_NEWPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_STATIC_NEWPWD2), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EDIT_NEWPWD2), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_NO), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_NO), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_STATIC_DELPWD), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_EDIT_DELPWD), SW_HIDE);
	}

	void ShowPagePassword()
	{
		// Position
		::SetWindowPos(GetDlgItem(IDC_RADIO_PASSWORD_YES), NULL, 200, 70, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
		::SetWindowPos(GetDlgItem(IDC_TEXT_PASSWORD_YES), NULL, 220, 70, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);

		if (m_bIsCreatePwd)
		{
			::SetWindowPos(GetDlgItem(IDC_STATIC_NEWPWD), NULL, 220, 74 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_EDIT_NEWPWD), NULL, 200 + CTRL_LINE_SPACE, 70 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_STATIC_NEWPWD2), NULL, 220, 74 + CTRL_LINE_HEIGHT * 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_EDIT_NEWPWD2), GetDlgItem(IDC_EDIT_NEWPWD), 200 + CTRL_LINE_SPACE, 70 + CTRL_LINE_HEIGHT * 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
		}
		else
		{
			::SetWindowPos(GetDlgItem(IDC_STATIC_CURPWD), NULL, 220, 74 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_EDIT_OLDPWD), NULL, 200 + CTRL_LINE_SPACE, 70 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_STATIC_NEWPWD), NULL, 220, 74 + CTRL_LINE_HEIGHT * 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_EDIT_NEWPWD), GetDlgItem(IDC_EDIT_OLDPWD), 200 + CTRL_LINE_SPACE, 70 + CTRL_LINE_HEIGHT * 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_STATIC_NEWPWD2), NULL, 220, 74 + CTRL_LINE_HEIGHT * 3, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_EDIT_NEWPWD2), GetDlgItem(IDC_EDIT_NEWPWD), 200 + CTRL_LINE_SPACE, 70 + CTRL_LINE_HEIGHT * 3, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_RADIO_PASSWORD_NO), NULL, 200, 90 + CTRL_LINE_HEIGHT * 4, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_TEXT_PASSWORD_NO), NULL, 220, 90 + CTRL_LINE_HEIGHT * 4, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_STATIC_DELPWD), NULL, 220, 94 + CTRL_LINE_HEIGHT * 5, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
			::SetWindowPos(GetDlgItem(IDC_EDIT_DELPWD), NULL, 200 + CTRL_LINE_SPACE, 90 + CTRL_LINE_HEIGHT * 5, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);

			SetDlgItemText(IDC_TEXT_PASSWORD_YES, _T("修改密码"));
		}

		if (m_bCreateOrModifyPwd)
		{
			CheckDlgButton(IDC_RADIO_PASSWORD_YES, BST_CHECKED);
			CheckDlgButton(IDC_RADIO_PASSWORD_NO, BST_UNCHECKED);

			::EnableWindow(GetDlgItem(IDC_EDIT_OLDPWD), TRUE);
			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD), TRUE);
			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD2), TRUE);
			::EnableWindow(GetDlgItem(IDC_EDIT_DELPWD), FALSE);
			::SetFocus (GetDlgItem (IDC_EDIT_OLDPWD));
		}
		else
		{
			CheckDlgButton(IDC_RADIO_PASSWORD_YES, BST_UNCHECKED);
			CheckDlgButton(IDC_RADIO_PASSWORD_NO, BST_CHECKED);

			::EnableWindow(GetDlgItem(IDC_EDIT_OLDPWD), FALSE);
			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD), FALSE);
			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD2), FALSE);
			::EnableWindow(GetDlgItem(IDC_EDIT_DELPWD), TRUE);
			::SetFocus (GetDlgItem (IDC_EDIT_DELPWD));
		}

		::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_YES), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_NO), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_YES), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_NO), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_YES), SW_SHOW);

		::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_YES), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_NO), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_YES), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_NO), SW_HIDE);

		::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_YES), SW_SHOW);

		if (m_bIsCreatePwd)
		{
			::ShowWindow(GetDlgItem(IDC_STATIC_CURPWD), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_EDIT_OLDPWD), SW_HIDE);
			::SetFocus (GetDlgItem (IDC_EDIT_NEWPWD));
		}
		else
		{
			::ShowWindow(GetDlgItem(IDC_STATIC_CURPWD), SW_SHOW);
			::ShowWindow(GetDlgItem(IDC_EDIT_OLDPWD), SW_SHOW);
		}

		::ShowWindow(GetDlgItem(IDC_STATIC_NEWPWD), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_EDIT_NEWPWD), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_STATIC_NEWPWD2), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_EDIT_NEWPWD2), SW_SHOW);

		if (m_bIsCreatePwd)
		{
			::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_YES), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_NO), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_NO), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_STATIC_DELPWD), SW_HIDE);
			::ShowWindow(GetDlgItem(IDC_EDIT_DELPWD), SW_HIDE);
		}
		else
		{
			::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_NO), SW_SHOW);
			::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_NO), SW_SHOW);
			::ShowWindow(GetDlgItem(IDC_STATIC_DELPWD), SW_SHOW);
			::ShowWindow(GetDlgItem(IDC_EDIT_DELPWD), SW_SHOW);
		}
	}


public:
	static BOOL IsReminderEnabled()
	{
		BOOL bEnabled = FALSE;

		DWORD dwType;
		DWORD dwValue;
		DWORD dwReturnBytes = sizeof(DWORD);

		if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Setting"),
			_T("ReminderEnabled"), &dwType, &dwValue, &dwReturnBytes))
		{
			bEnabled = TRUE;
		}
		else if (dwValue == 1)
		{
			bEnabled = TRUE;
		}

		return bEnabled;
	}

	static BOOL IsAutoRunUpdate()
	{
		BOOL bAutoRun = FALSE;

		DWORD dwType;
		DWORD dwValue;
		DWORD dwReturnBytes = sizeof(DWORD);

		if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("AutoRun"), &dwType, &dwValue, &dwReturnBytes))
		{
			bAutoRun = TRUE;
		}
		else if (dwValue == 1)
		{
			bAutoRun = TRUE;
		}

		return bAutoRun;
	}

	//LRESULT OnEnChangeEditOldpwd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
