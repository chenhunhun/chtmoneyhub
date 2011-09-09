#include "stdafx.h"
#include "SettingDlg.h"



//CSettingDlg::~CSettingDlg()
//{
//	std::list<PMYCTRLNODE>::const_iterator cstIt;
//	for(cstIt = m_listTab1Ctrl.end(); cstIt != m_listTab1Ctrl.end(); cstIt++)
//	{
//		PMYCTRLNODE pNode = *cstIt;
//		if (NULL == pNode)
//			continue;
//
//		delete pNode;
//		pNode = NULL;
//	}
//
//	m_listTab1Ctrl.clear();
//}
//
//
//LRESULT CSettingDlg::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
//{
//	UINT nID = ::GetDlgCtrlID((HWND)lParam);
//	if (nID == IDC_STATIC
//		|| nID == IDC_STATIC_DELPWD
//		|| nID == IDC_TEXT_REMINDER_YES
//		|| nID == IDC_TEXT_REMINDER_NO
//		|| nID == IDC_TEXT_UPDATE_YES
//		|| nID == IDC_TEXT_UPDATE_NO
//		|| nID == IDC_TEXT_PASSWORD_YES
//		|| nID == IDC_TEXT_PASSWORD_NO
//		|| nID == IDC_RADIO_REMINDER_YES
//		|| nID == IDC_RADIO_REMINDER_NO
//		|| nID == IDC_RADIO_UPDATE_YES
//		|| nID == IDC_RADIO_UPDATE_NO
//		|| nID == IDC_RADIO_PASSWORD_YES
//		|| nID == IDC_RADIO_PASSWORD_NO
//		|| nID == IDC_STATIC_PW_SHOW
//		|| nID == IDC_STATIC_MAIL_SHOW
//		|| nID == IDC_STATIC_LINE2
//		|| nID == IDC_STATIC_LINE1
//		|| nID == IDC_STATIC_PART_FRONT
//		|| nID == IDC_STATIC_PART_BEHIND
//		|| nID == IDC_STATIC_NEW_MAIL
//		|| nID == IDC_CHECK_AUTO
//		|| nID == IDC_CHECK_UN
//		|| nID == IDC_CHECK_PW
//		|| nID == IDC_STATIC_CLEAN_INFO
//		|| nID == IDC_STATIC_INFO
//		|| nID == IDC_STATIC_LINE3
//		|| nID == IDC_CHECK_SECCION
//		|| nID == IDC_EDIT_TIME
//		|| nID == IDC_STATIC_CLEANLOAD
//		|| nID == IDC_STATIC_QUITLOAD)
//	{
//		CDCHandle dc((HDC)wParam);
//		dc.SetTextColor(RGB(61, 98, 123));
//		return LRESULT(m_bkBrush); 
//	}
//
//	return NULL;
//}
//
//
////处理第一次不能隐藏窗口的问题
//LRESULT CSettingDlg::OnNcPaint(WORD /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//{
//	static int time = 2;
//
//	if(time > 0)
//	{
//		if (m_bIsCreatePwd)
//			::SetFocus (GetDlgItem (IDC_EDIT_LINE2));
//		else
//			::SetFocus (GetDlgItem (IDC_EDIT_LINE1));
//		time --;
//	}
//	return 0;
//}
//
//void CSettingDlg::RecordTab1Ctrls(int nID, LPCTSTR lpStr)
//{
//	if (NULL == lpStr)
//		return;
//
//	std::wstring strTp = lpStr;
//	std::list<PMYCTRLNODE>::const_iterator cstIt;
//	for (cstIt = m_listTab1Ctrl.begin(); cstIt != m_listTab1Ctrl.end(); cstIt++)
//	{
//		int nCtrlID = (*cstIt)->nCtrlID;
//		if (nCtrlID == nID)
//		{
//			if (strTp != (*cstIt)->strShow) //如果字符串发生了改变 
//				break;
//			else
//				return;
//		}
//	}
//
//
//	if (cstIt != m_listTab1Ctrl.end())
//	{
//		PMYCTRLNODE pNode = *cstIt;
//		pNode->strShow = lpStr;
//		return;
//	}
//	else
//	{
//
//		PMYCTRLNODE pNewNode = new MYCTRLNODE();
//		pNewNode->nCtrlID = nID;
//		pNewNode->strShow = lpStr;
//		m_listTab1Ctrl.push_back(pNewNode);
//	}
//}
//
//bool CSettingDlg::RemoveTab1Ctrl(int nID)
//{
//	std::list<PMYCTRLNODE>::const_iterator cstIt;
//	for (cstIt = m_listTab1Ctrl.begin(); cstIt != m_listTab1Ctrl.end(); cstIt++)
//	{
//		int nCtrlID = (*cstIt)->nCtrlID;
//		if (nCtrlID == nID)
//			break;
//	}
//
//	if (cstIt != m_listTab1Ctrl.end())
//	{
//		PMYCTRLNODE pNode = *cstIt;
//		m_listTab1Ctrl.erase(cstIt);
//		delete pNode;
//		return true;
//	}
//
//	return false;
//}
//
//
//LRESULT CSettingDlg::OnMailChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	m_strInfoPart1 = L"修改邮件地址";
//	SetDlgItemText(IDC_STATIC_PART_FRONT, L"修改邮件地址");
//	SetDlgItemText(IDC_STATIC_LINE1, L"当前邮件地址");
//	SetDlgItemText(IDC_STATIC_LINE2, L"新邮件地址：");
//	SetDlgItemText(IDC_STATIC_LINE3, L"      密码：");
//	//SetDlgItemText(IDC_EDIT_LINE1, L"");
//	RecordTab1Ctrls(IDC_STATIC_PART_FRONT, L"修改邮件地址");
//	RecordTab1Ctrls(IDC_STATIC_LINE1, L"当前邮件地址");
//	RecordTab1Ctrls(IDC_STATIC_LINE2, L"新邮件地址：");
//	RecordTab1Ctrls(IDC_STATIC_LINE3, L"      密码：");
//
//	::ShowWindow(GetDlgItem(IDC_BTN_MAIL_CHANGE), SW_HIDE);
//	RemoveTab1Ctrl(IDC_BTN_MAIL_CHANGE);
//	::ShowWindow(GetDlgItem(IDC_BTN_PW_CHANGE), SW_HIDE);
//	RemoveTab1Ctrl(IDC_BTN_PW_CHANGE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_PW_SHOW), SW_HIDE);
//	RemoveTab1Ctrl(IDC_STATIC_PW_SHOW);
//
//	// 确定按钮
//	RecordTab1Ctrls(IDC_EDIT_LINE3, L"");
//
//	::ShowWindow(GetDlgItem(IDC_STATIC_MAIL_SHOW), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_EDIT_LINE2), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_EDIT_LINE3), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_STATIC_LINE3), SW_SHOW);
//
//	CString strTemp;
//
//	GetDlgItemText(IDC_STATIC_MAIL_SHOW, strTemp);
//	RecordTab1Ctrls(IDC_STATIC_MAIL_SHOW, strTemp);
//
//	GetDlgItemText(IDC_EDIT_LINE2, strTemp);
//	RecordTab1Ctrls(IDC_EDIT_LINE2, strTemp);
//
//	GetDlgItemText(IDC_EDIT_LINE3, strTemp);
//	RecordTab1Ctrls(IDC_EDIT_LINE3, strTemp);
//
//	::ShowWindow(GetDlgItem(IDC_BTN_SURE_CHANGE), SW_SHOW);
//	RecordTab1Ctrls(IDC_BTN_SURE_CHANGE, L"");
//
//	return 0;
//}
//
//LRESULT CSettingDlg::OnPasswordChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	SetDlgItemText(IDC_STATIC_PART_FRONT, L"修改密码");
//	m_strInfoPart1 = L"修改密码";
//	SetDlgItemText(IDC_STATIC_LINE1, L"旧密码：");
//	SetDlgItemText(IDC_STATIC_LINE2, L"新密码：");
//	SetDlgItemText(IDC_STATIC_LINE3, L"确认密码：");
//
//	RecordTab1Ctrls(IDC_STATIC_PART_FRONT, L"修改密码");
//	RecordTab1Ctrls(IDC_STATIC_LINE1, L"旧密码：");
//	RecordTab1Ctrls(IDC_STATIC_LINE2, L"新密码：");
//	RecordTab1Ctrls(IDC_STATIC_LINE3, L"确认密码：");
//
//	CString strTemp;
//	GetDlgItemText(IDC_EDIT_LINE1, strTemp);
//	RecordTab1Ctrls(IDC_EDIT_LINE1, strTemp);
//
//	GetDlgItemText(IDC_EDIT_LINE2, strTemp);
//	RecordTab1Ctrls(IDC_EDIT_LINE2, strTemp);
//
//	GetDlgItemText(IDC_EDIT_LINE3, strTemp);
//	RecordTab1Ctrls(IDC_EDIT_LINE3, strTemp);
//
//	RecordTab1Ctrls(IDC_EDIT_LINE3, L"");
//
//	::ShowWindow(GetDlgItem(IDC_BTN_SURE_CHANGE), SW_SHOW);
//	RecordTab1Ctrls(IDC_BTN_SURE_CHANGE, L"");
//
//	::ShowWindow(GetDlgItem(IDC_STATIC_LINE3), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_EDIT_LINE1), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_EDIT_LINE2), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_EDIT_LINE3), SW_SHOW);
//
//
//
//	RemoveTab1Ctrl(IDC_BTN_MAIL_CHANGE);
//	::ShowWindow(GetDlgItem(IDC_BTN_MAIL_CHANGE), SW_HIDE);
//
//	RemoveTab1Ctrl(IDC_BTN_PW_CHANGE);
//	::ShowWindow(GetDlgItem(IDC_BTN_PW_CHANGE), SW_HIDE);
//
//	RemoveTab1Ctrl(IDC_STATIC_PW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_STATIC_PW_SHOW), SW_HIDE);
//
//	RemoveTab1Ctrl(IDC_STATIC_MAIL_SHOW);
//	::ShowWindow(GetDlgItem(IDC_STATIC_MAIL_SHOW), SW_HIDE);
//
//	return 0;
//}
//
//LRESULT CSettingDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	if (m_TabCtrl.GetCurTab() == emBaseSetting)
//	{
//		BOOL bEnabled = m_bRemiderYes ? TRUE : FALSE;
//		::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Setting"),
//			_T("ReminderEnabled"), REG_DWORD, &bEnabled, sizeof(DWORD));
//
//	}
//	else if (m_TabCtrl.GetCurTab() == emRemind)
//	{
//		BOOL bEnabled = m_bUpdateYes ? TRUE : FALSE;
//		::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"),
//			_T("AutoRun"), REG_DWORD, &bEnabled, sizeof(DWORD));
//	}
//	else if (m_TabCtrl.GetCurTab() == emSafe)
//	{
//		if (m_bIsCreatePwd)
//		{
//			CString strNewPwd;
//			CString strConfirm;
//			CString strTitle = _T("创建密码");
//
//			//				GetDlgItemText(IDC_EDIT_NEWPWD, strNewPwd);
//			//				GetDlgItemText(IDC_EDIT_NEWPWD2, strConfirm);
//
//			if (strNewPwd.IsEmpty())
//			{
//				mhMessageBox(GetActiveWindow(), _T("新密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//				//					::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD));
//				return 0;
//			}
//
//			if (strNewPwd.GetLength() < 6)
//			{
//				mhMessageBox(GetActiveWindow(), _T("新密码字符数不能小于6，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//				//					::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD));
//				//					SetDlgItemText (IDC_EDIT_NEWPWD, L"");
//				return 0;
//			}
//
//			if (strConfirm.IsEmpty())
//			{
//				mhMessageBox(GetActiveWindow(), _T("确认新密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//				//					::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD2));
//				return 0;
//			}
//
//			if (strConfirm != strNewPwd)
//			{
//				mhMessageBox(GetActiveWindow(), _T("确认新密码不正确，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//				//					::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD2));
//				//					SetDlgItemText (IDC_EDIT_NEWPWD2, L"");
//				return 0;
//			}
//
//			USES_CONVERSION;
//			std::string strPwd = CT2A(strNewPwd);
//			CBankData* pBankData = CBankData::GetInstance();
//			pBankData->SavePwd(strPwd);
//		}
//		else
//		{
//			if (m_bCreateOrModifyPwd)
//			{
//				CBankData* pBankData = CBankData::GetInstance();
//				std::string strPwd = pBankData->GetPwd();
//
//				CString strOldPwd;
//				CString strNewPwd;
//				CString strConfirm;
//				CString strTitle = _T("修改密码");
//
//				//					GetDlgItemText(IDC_EDIT_OLDPWD, strOldPwd);
//				//					GetDlgItemText(IDC_EDIT_NEWPWD, strNewPwd);
//				//					GetDlgItemText(IDC_EDIT_NEWPWD2, strConfirm);
//
//				USES_CONVERSION;
//				if (strOldPwd.IsEmpty() || strOldPwd.Compare(A2CT(strPwd.c_str())) != 0)
//				{
//					mhMessageBox(GetActiveWindow(), _T("当前密码不正确，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//					//						::SetFocus(GetDlgItem(IDC_EDIT_OLDPWD));
//					//						SetDlgItemText (IDC_EDIT_OLDPWD, L"");
//					return 0;
//				}
//
//				if (strNewPwd.IsEmpty())
//				{
//					mhMessageBox(GetActiveWindow(), _T("新密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//					//						::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD));
//					return 0;
//				}
//
//				if (strNewPwd.GetLength() < 6)
//				{
//					mhMessageBox(GetActiveWindow(), _T("新密码字符数不能小于6，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//					//						::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD));
//					//						SetDlgItemText (IDC_EDIT_NEWPWD, L"");
//					return 0;
//				}
//
//				if (strConfirm.IsEmpty())
//				{
//					mhMessageBox(GetActiveWindow(), _T("确认新密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//					//						::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD2));
//					return 0;
//				}
//
//				if (strConfirm != strNewPwd)
//				{
//					mhMessageBox(GetActiveWindow(), _T("确认新密码不正确，请重新输入."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//					//						::SetFocus(GetDlgItem(IDC_EDIT_NEWPWD2));
//					//						SetDlgItemText (IDC_EDIT_NEWPWD2, L"");
//					return 0;
//				}
//
//				strPwd = CT2A(strNewPwd);
//				pBankData->SavePwd(strPwd);
//			}
//			else
//			{
//				CBankData* pBankData = CBankData::GetInstance();
//				std::string strPwd = pBankData->GetPwd();
//
//				CString strOldPwd;
//				CString strTitle = _T("删除密码");
//
//				GetDlgItemText(IDC_EDIT_DELPWD, strOldPwd);
//
//				if (strOldPwd.IsEmpty())
//				{
//					mhMessageBox(GetActiveWindow(), _T("当前密码不能为空."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//					::SetFocus(GetDlgItem(IDC_EDIT_DELPWD));
//					return 0;
//				}
//
//				USES_CONVERSION;
//				if (strOldPwd.Compare(A2CT(strPwd.c_str())) != 0)
//				{
//					mhMessageBox(GetActiveWindow(), _T("当前密码不正确."), (LPCTSTR)strTitle, MB_OK | MB_ICONHAND);
//					::SetFocus(GetDlgItem(IDC_EDIT_DELPWD));
//					SetDlgItemText (IDC_EDIT_DELPWD, L"");
//					return 0;
//				}
//
//				pBankData->SavePwd("");
//			}
//		}
//	}
//
//	EndDialog(IDOK);
//
//	return 0;
//}
//
//// 隐藏界面上所有控件
//void CSettingDlg::HideAllControl()
//{
//	// Show or hide
//	::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_YES), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_NO), SW_HIDE);
//
//	::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_YES), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_NO), SW_HIDE);
//
//	::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_YES), SW_HIDE);
//
//	::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_YES), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_NO), SW_HIDE);
//
//	::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_YES), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_NO), SW_HIDE);
//
//	::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_YES), SW_HIDE);
//
//	::ShowWindow(GetDlgItem(IDC_STATIC_LINE1), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_EDIT_LINE1), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_LINE2), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_EDIT_LINE2), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_LINE3), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_EDIT_LINE3), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_NO), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_NO), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_DELPWD), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_EDIT_DELPWD), SW_HIDE);
//
//	// 上下两个提示不用隐藏，只须相应的更改显示文本即可
//	//::ShowWindow(GetDlgItem(IDC_STATIC_PART_BEHIND), SW_HIDE);
//	//::ShowWindow(GetDlgItem(IDC_STATIC_PART_FRONT), SW_HIDE);
//
//	//		::ShowWindow(GetDlgItem(IDC_STATIC_MAIL), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_MAIL_SHOW), SW_HIDE);
//	//		::ShowWindow(GetDlgItem(IDC_STATIC_PW), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_PW_SHOW), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_BTN_MAIL_CHANGE), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_BTN_PW_CHANGE), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_NEW_MAIL), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_CHECK_UN), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_CHECK_PW), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_CHECK_AUTO), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_BTN_SURE_CHANGE), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_CLEAN_INFO), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_INFO), SW_HIDE);
//	//		::ShowWindow(GetDlgItem(IDC_STATIC_MAIL_PW), SW_HIDE);
//
//	::ShowWindow(GetDlgItem(IDC_CHECK_SECCION), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_EDIT_TIME), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_CLEANLOAD), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_STATIC_QUITLOAD), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDC_BTN_CLEAN), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDOK), SW_HIDE);
//	::ShowWindow(GetDlgItem(IDCANCEL), SW_HIDE);
//
//}
//
//LRESULT CSettingDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	EndDialog(IDCANCEL);
//	return 0;
//}
//
//LRESULT CSettingDlg::OnTabCtrl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	int nCurTab = m_TabCtrl.GetCurTab();
//
//	if (nCurTab == emBaseSetting)
//		ShowPageBaseSetting();
//	else if (nCurTab == emRemind)
//		ShowPageStatusAndRemind();
//	else if (nCurTab == emSafe)
//		ShowPageSafeAndPrivacy();
//
//	return 0;
//}
//
//LRESULT CSettingDlg::OnRadioChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	if (wID == IDC_RADIO_REMINDER_YES)
//	{
//		m_bRemiderYes = true;
//
//		CheckDlgButton(IDC_RADIO_REMINDER_NO, BST_UNCHECKED);
//	}
//	else if (wID == IDC_RADIO_REMINDER_NO)
//	{
//		m_bRemiderYes = false;
//
//		CheckDlgButton(IDC_RADIO_REMINDER_YES, BST_UNCHECKED);
//	}
//	else if (wID == IDC_RADIO_UPDATE_YES)
//	{
//		m_bUpdateYes = true;
//
//		CheckDlgButton(IDC_RADIO_UPDATE_NO, BST_UNCHECKED);
//	}
//	else if (wID == IDC_RADIO_UPDATE_NO)
//	{
//		m_bUpdateYes = false;
//
//		CheckDlgButton(IDC_RADIO_UPDATE_YES, BST_UNCHECKED);
//	}
//	else if (wID == IDC_RADIO_PASSWORD_YES)
//	{
//		m_bCreateOrModifyPwd = true;
//
//		CheckDlgButton(IDC_RADIO_PASSWORD_NO, BST_UNCHECKED);
//
//		//			::EnableWindow(GetDlgItem(IDC_EDIT_OLDPWD), TRUE);
//		//			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD), TRUE);
//		//			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD2), TRUE);
//		::EnableWindow(GetDlgItem(IDC_EDIT_DELPWD), FALSE);
//		//			::SetFocus (GetDlgItem (IDC_EDIT_OLDPWD));
//	}
//	else if (wID == IDC_RADIO_PASSWORD_NO)
//	{
//		m_bCreateOrModifyPwd = false;
//
//		CheckDlgButton(IDC_RADIO_PASSWORD_YES, BST_UNCHECKED);
//
//		//			::EnableWindow(GetDlgItem(IDC_EDIT_OLDPWD), FALSE);
//		//			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD), FALSE);
//		//			::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD2), FALSE);
//		::EnableWindow(GetDlgItem(IDC_EDIT_DELPWD), TRUE);
//		::SetFocus (GetDlgItem (IDC_EDIT_DELPWD));
//	}
//
//	return 0;
//}
//
//void CSettingDlg::ShowPageBaseSetting()
//{
//	HideAllControl();
//
//	// 显示所有要显示的控件并更新文本
//	std::list<PMYCTRLNODE>::const_iterator cstIt;
//	for(cstIt = m_listTab1Ctrl.begin(); cstIt != m_listTab1Ctrl.end(); cstIt ++)
//	{
//		int nID = (*cstIt)->nCtrlID;
//		if (nID > 0)
//			::ShowWindow(GetDlgItem(nID), SW_SHOW);
//
//		if(!(*cstIt)->strShow.empty())
//		{
//			SetDlgItemText(nID, (*cstIt)->strShow.c_str());
//		}
//	}
//
//	SetDlgItemText(IDC_STATIC_PART_FRONT, m_strInfoPart1.c_str());
//
//	// 下半部分的提示、分隔线位置
//	::SetWindowPos(GetDlgItem(IDC_STATIC_PART_BEHIND), NULL, CTLR_BEGIN_POS_X, CTRL_BEHIND_BEGIN, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_FRAME_BEHIND), NULL, CTLR_BEGIN_POS_X, CTRL_BEHIND_BEGIN + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	if (m_bRemiderYes)
//	{
//		CheckDlgButton(IDC_RADIO_REMINDER_YES, BST_CHECKED);
//		CheckDlgButton(IDC_RADIO_REMINDER_NO, BST_UNCHECKED);
//	}
//	else
//	{
//		CheckDlgButton(IDC_RADIO_REMINDER_YES, BST_UNCHECKED);
//		CheckDlgButton(IDC_RADIO_REMINDER_NO, BST_CHECKED);
//	}	
//
//}
//
//
//void CSettingDlg::ShowPageStatusAndRemind()
//{
//	HideAllControl();
//
//	SetDlgItemText(IDC_STATIC_PART_FRONT, L"提醒");
//	SetDlgItemText(IDC_STATIC_PART_BEHIND, L"状态");
//
//	// 下半部分的提示、分隔线位置
//	::SetWindowPos(GetDlgItem(IDC_STATIC_PART_BEHIND), NULL, CTLR_BEGIN_POS_X, CTRL_2_BEHIND_BEGIN, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_FRAME_BEHIND), NULL, CTLR_BEGIN_POS_X, CTRL_2_BEHIND_BEGIN + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_YES), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_YES), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_NO), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_NO), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_CHECK_SECCION), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_EDIT_TIME), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_STATIC_CLEANLOAD), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_STATIC_QUITLOAD), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDOK), SW_SHOW);
//
//	if (m_bUpdateYes)
//	{
//		CheckDlgButton(IDC_RADIO_UPDATE_YES, BST_CHECKED);
//		CheckDlgButton(IDC_RADIO_UPDATE_NO, BST_UNCHECKED);
//	}
//	else
//	{
//		CheckDlgButton(IDC_RADIO_UPDATE_YES, BST_UNCHECKED);
//		CheckDlgButton(IDC_RADIO_UPDATE_NO, BST_CHECKED);
//	}
//
//
//}
//
//
//void CSettingDlg::ShowPageSafeAndPrivacy()
//{
//	HideAllControl();
//
//
//	// 下半部分的提示、分隔线位置
//	::SetWindowPos(GetDlgItem(IDC_STATIC_PART_BEHIND), NULL, CTLR_BEGIN_POS_X, CTRL_BEHIND_BEGIN, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_FRAME_BEHIND), NULL, CTLR_BEGIN_POS_X, CTRL_BEHIND_BEGIN + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	SetDlgItemText(IDC_STATIC_PART_FRONT, L"隐私设置");
//	SetDlgItemText(IDC_STATIC_PART_BEHIND, L"清除数据");
//
//
//	::ShowWindow(GetDlgItem(IDC_CHECK_UN), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_CHECK_PW), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_CHECK_AUTO), SW_SHOW);
//	// 显示确认按钮
//	::ShowWindow(GetDlgItem(IDC_BTN_SURE_CHANGE), SW_SHOW);
//
//	::ShowWindow(GetDlgItem(IDC_STATIC_INFO), SW_SHOW);
//	::ShowWindow(GetDlgItem(CTRL_BUTTON_POS_X), SW_SHOW);
//
//	::ShowWindow(GetDlgItem(IDC_STATIC_INFO), SW_SHOW);
//	::ShowWindow(GetDlgItem(IDC_STATIC_CLEAN_INFO), SW_SHOW);
//
//	::ShowWindow(GetDlgItem(IDC_BTN_CLEAN), SW_SHOW);
//
//	//::ShowWindow(GetDlgItem(CTRL_BUTTON_POS_X), SW_SHOW);
//	//// Position
//	//::SetWindowPos(GetDlgItem(IDC_RADIO_PASSWORD_YES), NULL, 200, 70, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//::SetWindowPos(GetDlgItem(IDC_TEXT_PASSWORD_YES), NULL, 220, 70, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	//if (m_bIsCreatePwd)
//	//{
//	//	::SetWindowPos(GetDlgItem(IDC_STATIC_NEWPWD), NULL, 220, 74 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_EDIT_NEWPWD), NULL, 200 + CTRL_FIRST_COL_WIGHT, 70 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_STATIC_NEWPWD2), NULL, 220, 74 + CTRL_LINE_HEIGHT * 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_EDIT_NEWPWD2), GetDlgItem(IDC_EDIT_NEWPWD), 200 + CTRL_FIRST_COL_WIGHT, 70 + CTRL_LINE_HEIGHT * 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//}
//	//else
//	//{
//	//	::SetWindowPos(GetDlgItem(IDC_STATIC_CURPWD), NULL, 220, 74 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_EDIT_OLDPWD), NULL, 200 + CTRL_FIRST_COL_WIGHT, 70 + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_STATIC_NEWPWD), NULL, 220, 74 + CTRL_LINE_HEIGHT * 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_EDIT_NEWPWD), GetDlgItem(IDC_EDIT_OLDPWD), 200 + CTRL_FIRST_COL_WIGHT, 70 + CTRL_LINE_HEIGHT * 2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_STATIC_NEWPWD2), NULL, 220, 74 + CTRL_LINE_HEIGHT * 3, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_EDIT_NEWPWD2), GetDlgItem(IDC_EDIT_NEWPWD), 200 + CTRL_FIRST_COL_WIGHT, 70 + CTRL_LINE_HEIGHT * 3, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_RADIO_PASSWORD_NO), NULL, 200, 90 + CTRL_LINE_HEIGHT * 4, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_TEXT_PASSWORD_NO), NULL, 220, 90 + CTRL_LINE_HEIGHT * 4, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_STATIC_DELPWD), NULL, 220, 94 + CTRL_LINE_HEIGHT * 5, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	//	::SetWindowPos(GetDlgItem(IDC_EDIT_DELPWD), NULL, 200 + CTRL_FIRST_COL_WIGHT, 90 + CTRL_LINE_HEIGHT * 5, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	//	SetDlgItemText(IDC_TEXT_PASSWORD_YES, _T("修改密码"));
//	//}
//
//	//if (m_bCreateOrModifyPwd)
//	//{
//	//	CheckDlgButton(IDC_RADIO_PASSWORD_YES, BST_CHECKED);
//	//	CheckDlgButton(IDC_RADIO_PASSWORD_NO, BST_UNCHECKED);
//
//	//	::EnableWindow(GetDlgItem(IDC_EDIT_OLDPWD), TRUE);
//	//	::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD), TRUE);
//	//	::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD2), TRUE);
//	//	::EnableWindow(GetDlgItem(IDC_EDIT_DELPWD), FALSE);
//	//	::SetFocus (GetDlgItem (IDC_EDIT_OLDPWD));
//	//}
//	//else
//	//{
//	//	CheckDlgButton(IDC_RADIO_PASSWORD_YES, BST_UNCHECKED);
//	//	CheckDlgButton(IDC_RADIO_PASSWORD_NO, BST_CHECKED);
//
//	//	::EnableWindow(GetDlgItem(IDC_EDIT_OLDPWD), FALSE);
//	//	::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD), FALSE);
//	//	::EnableWindow(GetDlgItem(IDC_EDIT_NEWPWD2), FALSE);
//	//	::EnableWindow(GetDlgItem(IDC_EDIT_DELPWD), TRUE);
//	//	::SetFocus (GetDlgItem (IDC_EDIT_DELPWD));
//	//}
//
//	//::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_YES), SW_HIDE);
//	//::ShowWindow(GetDlgItem(IDC_RADIO_REMINDER_NO), SW_HIDE);
//
//	//::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_YES), SW_HIDE);
//	//::ShowWindow(GetDlgItem(IDC_RADIO_UPDATE_NO), SW_HIDE);
//
//	//::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_YES), SW_SHOW);
//
//	//::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_YES), SW_HIDE);
//	//::ShowWindow(GetDlgItem(IDC_TEXT_REMINDER_NO), SW_HIDE);
//
//	//::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_YES), SW_HIDE);
//	//::ShowWindow(GetDlgItem(IDC_TEXT_UPDATE_NO), SW_HIDE);
//
//	//::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_YES), SW_SHOW);
//
//	//if (m_bIsCreatePwd)
//	//{
//	//	::ShowWindow(GetDlgItem(IDC_STATIC_CURPWD), SW_HIDE);
//	//	::ShowWindow(GetDlgItem(IDC_EDIT_OLDPWD), SW_HIDE);
//	//	::SetFocus (GetDlgItem (IDC_EDIT_NEWPWD));
//	//}
//	//else
//	//{
//	//	::ShowWindow(GetDlgItem(IDC_STATIC_CURPWD), SW_SHOW);
//	//	::ShowWindow(GetDlgItem(IDC_EDIT_OLDPWD), SW_SHOW);
//	//}
//
//	//::ShowWindow(GetDlgItem(IDC_STATIC_NEWPWD), SW_SHOW);
//	//::ShowWindow(GetDlgItem(IDC_EDIT_NEWPWD), SW_SHOW);
//	//::ShowWindow(GetDlgItem(IDC_STATIC_NEWPWD2), SW_SHOW);
//	//::ShowWindow(GetDlgItem(IDC_EDIT_NEWPWD2), SW_SHOW);
//
//	//if (m_bIsCreatePwd)
//	//{
//	//	::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_YES), SW_HIDE);
//	//	::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_NO), SW_HIDE);
//	//	::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_NO), SW_HIDE);
//	//	::ShowWindow(GetDlgItem(IDC_STATIC_DELPWD), SW_HIDE);
//	//	::ShowWindow(GetDlgItem(IDC_EDIT_DELPWD), SW_HIDE);
//	//}
//	//else
//	//{
//	//	::ShowWindow(GetDlgItem(IDC_RADIO_PASSWORD_NO), SW_SHOW);
//	//	::ShowWindow(GetDlgItem(IDC_TEXT_PASSWORD_NO), SW_SHOW);
//	//	::ShowWindow(GetDlgItem(IDC_STATIC_DELPWD), SW_SHOW);
//	//	::ShowWindow(GetDlgItem(IDC_EDIT_DELPWD), SW_SHOW);
//	//}
//}
//
//BOOL CSettingDlg::IsReminderEnabled()
//{
//	BOOL bEnabled = FALSE;
//
//	DWORD dwType;
//	DWORD dwValue;
//	DWORD dwReturnBytes = sizeof(DWORD);
//
//	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Setting"),
//		_T("ReminderEnabled"), &dwType, &dwValue, &dwReturnBytes))
//	{
//		bEnabled = TRUE;
//	}
//	else if (dwValue == 1)
//	{
//		bEnabled = TRUE;
//	}
//
//	return bEnabled;
//}
//
//BOOL CSettingDlg::IsAutoRunUpdate()
//{
//	BOOL bAutoRun = FALSE;
//
//	DWORD dwType;
//	DWORD dwValue;
//	DWORD dwReturnBytes = sizeof(DWORD);
//
//	if (ERROR_SUCCESS != ::SHGetValue(HKEY_CURRENT_USER, _T("Software\\Bank\\Update"), _T("AutoRun"), &dwType, &dwValue, &dwReturnBytes))
//	{
//		bAutoRun = TRUE;
//	}
//	else if (dwValue == 1)
//	{
//		bAutoRun = TRUE;
//	}
//
//	return bAutoRun;
//}
//
//LRESULT CSettingDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//{
//	CenterWindow(GetParent());
//	SetWindowText(_T("设置"));
//
//	m_TabCtrl = GetDlgItem(IDC_TABCTRL);
//	m_TabCtrl.ApplySkin(NULL, &s()->Toolbar()->m_bmpListTabCtrlItems, 3);
//
//	m_TabCtrl.AddTab(_T("基本设置"));
//	m_TabCtrl.AddTab(_T("状态与提醒"));
//	m_TabCtrl.AddTab(_T("安全和隐私"));
//
//	ApplyButtonSkin(IDOK);
//	ApplyButtonSkin(IDCANCEL);
//
//	m_bkBrush = ::CreateSolidBrush(RGB(247,252,255));//暂时用取色的方式设置按钮的背景
//	m_bRemiderYes = IsReminderEnabled() ? true : false;
//	m_bUpdateYes = IsAutoRunUpdate() ? true : false;
//
//	CBankData* pBankData = CBankData::GetInstance();
//	m_bIsCreatePwd = pBankData->GetPwd().empty();
//	m_bCreateOrModifyPwd = true;
//	CRect rec;
//	::GetClientRect(GetDlgItem(IDC_RADIO_REMINDER_YES),&rec);
//	::GetClientRect(GetDlgItem(IDC_RADIO_REMINDER_NO),&rec);
//	::GetClientRect(GetDlgItem(IDC_RADIO_PASSWORD_YES),&rec);
//	::GetClientRect(GetDlgItem(IDC_RADIO_PASSWORD_NO),&rec);
//	::GetClientRect(GetDlgItem(IDC_RADIO_UPDATE_YES),&rec);
//	::GetClientRect(GetDlgItem(IDC_RADIO_UPDATE_NO),&rec);
//
//	int nChars = 20;
//	//		::SendMessage(GetDlgItem(IDC_EDIT_OLDPWD), EM_LIMITTEXT, nChars, 0L);
//	//		::SendMessage(GetDlgItem(IDC_EDIT_NEWPWD), EM_LIMITTEXT, nChars, 0L);
//	////		::SendMessage(GetDlgItem(IDC_EDIT_NEWPWD2), EM_LIMITTEXT, nChars, 0L);
//	::SendMessage(GetDlgItem(IDC_EDIT_DELPWD), EM_LIMITTEXT, nChars, 0L);
//
//	m_strInfoPart1 = L"账户信息";
//	//SetDlgItemText(IDC_STATIC_PART_FRONT, L"账户信息");
//	SetDlgItemText(IDC_STATIC_PART_BEHIND, L"更新");
//
//	// 设置好位置不会变动的按钮
//	SetAllControlPos();
//
//	ApplyButtonSkin(IDC_BTN_MAIL_CHANGE);
//	ApplyButtonSkin(IDC_BTN_PW_CHANGE);
//	ApplyButtonSkin(IDC_BTN_CLEAN);
//	ApplyButtonSkin(IDC_BTN_SURE_CHANGE);
//
//	CString strText;
//	RecordTab1Ctrls(IDC_STATIC_LINE1, L"邮件地址：");
//	RecordTab1Ctrls(IDC_STATIC_LINE2, L"密    码：");
//
//	GetDlgItemText(IDC_STATIC_PW_SHOW, strText);
//	RecordTab1Ctrls(IDC_STATIC_PW_SHOW, strText);
//
//	GetDlgItemText(IDC_STATIC_MAIL_SHOW, strText);
//	RecordTab1Ctrls(IDC_STATIC_MAIL_SHOW, strText);
//
//	RecordTab1Ctrls(IDC_BTN_MAIL_CHANGE, L"");
//	RecordTab1Ctrls(IDC_BTN_PW_CHANGE, L"");
//
//	RecordTab1Ctrls(IDC_RADIO_UPDATE_YES, L"");
//	RecordTab1Ctrls(IDC_TEXT_UPDATE_YES, L"");
//	RecordTab1Ctrls(IDC_RADIO_UPDATE_NO, L"");
//	RecordTab1Ctrls(IDC_TEXT_UPDATE_NO, L"");
//	RecordTab1Ctrls(IDOK, L"");
//
//	ShowPageBaseSetting ();
//
//
//	return TRUE;
//}
//
//
//void CSettingDlg::SetAllControlPos(void)
//{
//	// 将标签页分成上下两部分
//
//	// 上半部分的提示、分隔线位置
//	::SetWindowPos(GetDlgItem(IDC_STATIC_PART_FRONT), NULL, CTLR_BEGIN_POS_X, CTRL_FRONT_BEGIN, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_FRAME_FRONT), NULL, CTLR_BEGIN_POS_X, CTRL_FRONT_BEGIN + CTRL_LINE_HEIGHT, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	// 上半部分分隔线下第一行（按高度定位）
//	int nHight = CTRL_FRONT_BEGIN + CTRL_LINE_HEIGHT + CTRL_SEPRATE_HIGHT + SEPRATE_HIGHT_Y;
//
//	// 标签1
//	::SetWindowPos(GetDlgItem(IDC_STATIC_LINE1), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_STATIC_MAIL_SHOW), NULL, CTLR_BEGIN_POS_X + CTRL_FIRST_COL_WIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_BTN_MAIL_CHANGE), NULL, CTRL_BUTTON_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_EDIT_LINE1), NULL, CTLR_BEGIN_POS_X + CTRL_FIRST_COL_WIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	// 标签2
//	::SetWindowPos(GetDlgItem(IDC_RADIO_REMINDER_YES), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_TEXT_REMINDER_YES), NULL, CTLR_BEGIN_POS_X + CTRL_RADIO_WEIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	// 标签3
//	::SetWindowPos(GetDlgItem(IDC_CHECK_UN), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	// 上半部分分隔线下第二行（按高度定位）
//	nHight += CTRL_LINE_HEIGHT;
//	nHight += SEPRATE_HIGHT_Y;
//	// 标签1
//	::SetWindowPos(GetDlgItem(IDC_STATIC_LINE2), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_STATIC_PW_SHOW), NULL, CTLR_BEGIN_POS_X + CTRL_FIRST_COL_WIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_BTN_PW_CHANGE), NULL, CTRL_BUTTON_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_EDIT_LINE2), NULL, CTLR_BEGIN_POS_X + CTRL_FIRST_COL_WIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	// 标签2
//	::SetWindowPos(GetDlgItem(IDC_RADIO_REMINDER_NO), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_TEXT_REMINDER_NO), NULL, CTLR_BEGIN_POS_X + CTRL_RADIO_WEIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	// 标签3
//	::SetWindowPos(GetDlgItem(IDC_CHECK_PW), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	// 上半部分分隔线下第三行（按高度定位）
//	nHight += CTRL_LINE_HEIGHT;
//	nHight += SEPRATE_HIGHT_Y;
//	// 标签1
//	::SetWindowPos(GetDlgItem(IDC_STATIC_LINE3), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_EDIT_LINE3), NULL, CTLR_BEGIN_POS_X + CTRL_FIRST_COL_WIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	// 标签2
//	// 标签3
//	::SetWindowPos(GetDlgItem(IDC_CHECK_AUTO), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	// 上半部分的确定按钮
//	::SetWindowPos(GetDlgItem(IDC_BTN_SURE_CHANGE), NULL, CTRL_BUTTON_POS_X, CTRL_SURE1_POS_Y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//
//	// 下半部分分隔线下第一行（按高度定位）
//	nHight = CTRL_BEHIND_BEGIN + CTRL_LINE_HEIGHT + CTRL_SEPRATE_HIGHT + SEPRATE_HIGHT_Y;
//	// 标签1
//	::SetWindowPos(GetDlgItem(IDC_RADIO_UPDATE_YES), NULL, CTLR_BEGIN_POS_X, nHight, RADIO_SIZE, RADIO_SIZE, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_TEXT_UPDATE_YES), NULL, CTLR_BEGIN_POS_X + CTRL_RADIO_WEIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	// 标签2
//	int nHight2 = CTRL_2_BEHIND_BEGIN + CTRL_LINE_HEIGHT + CTRL_SEPRATE_HIGHT + SEPRATE_HIGHT_Y;
//	CRect rect;
//	int nWidth = 0;
//	::GetClientRect(GetDlgItem(IDC_CHECK_SECCION), &rect);
//	::SetWindowPos(GetDlgItem(IDC_CHECK_SECCION), NULL, CTLR_BEGIN_POS_X, nHight2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	nWidth =  CTLR_BEGIN_POS_X + rect.Width();
//	::SetWindowPos(GetDlgItem(IDC_EDIT_TIME), NULL, nWidth, nHight2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	::GetClientRect(GetDlgItem(IDC_EDIT_TIME), &rect);
//	::SetWindowPos(GetDlgItem(IDC_STATIC_CLEANLOAD), NULL, nWidth + rect.Width() + 5 , nHight2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	// 标签3
//	::SetWindowPos(GetDlgItem(IDC_STATIC_CLEAN_INFO), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_BTN_CLEAN), NULL, CTRL_BUTTON_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	// 下半部分分隔线下第二行（按高度定位）
//	// 标签1
//	nHight += CTRL_LINE_HEIGHT;
//	nHight += SEPRATE_HIGHT_Y;
//	::SetWindowPos(GetDlgItem(IDC_RADIO_UPDATE_NO), NULL, CTLR_BEGIN_POS_X, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	::SetWindowPos(GetDlgItem(IDC_TEXT_UPDATE_NO), NULL, CTLR_BEGIN_POS_X + CTRL_RADIO_WEIGHT, nHight, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//	// 标签2
//	nHight2 += CTRL_LINE_HEIGHT;
//	nHight2 += SEPRATE_HIGHT_Y;
//	::SetWindowPos(GetDlgItem(IDC_STATIC_QUITLOAD), NULL, CTLR_BEGIN_POS_X, nHight2, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//	// 标签3上面提示信息的高度
//	::SetWindowPos(GetDlgItem(IDC_STATIC_INFO), NULL, CTLR_BEGIN_POS_X, CTRL_STAIC_INFO, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
//
//
//}