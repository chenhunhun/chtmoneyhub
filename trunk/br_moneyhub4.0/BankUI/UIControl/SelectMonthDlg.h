#pragma once

#include "../Resource/resource.h"
#include "CoolMessageBox.h"
#include "AltSkinClasses.h"
#include "../../BankData/BankData.h"
#include "ATLComTime.h"
#include <atlctrls.h>
using namespace std;

class CSelectMonthDlg : public CDialogImpl<CSelectMonthDlg>, public CDialogSkinMixer<CSelectMonthDlg>
{
public:
	CSelectMonthDlg()
	{
	}
	~CSelectMonthDlg(){}

public:
	enum { IDD = IDD_SELECT_MONTH };

	BEGIN_MSG_MAP(CSelectMonthDlg)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnBnClickedOk)
		NOTIFY_HANDLER(IDC_SYSLINK1, NM_CLICK, OnNMClickSyslink1)
		NOTIFY_HANDLER(IDC_DATETIMEPICKER1, DTN_DATETIMECHANGE, OnDtnDatetimechangeDatetimepicker1)
		NOTIFY_HANDLER(IDC_DATETIMEPICKER2, DTN_DATETIMECHANGE, OnDtnDatetimechangeDatetimepicker2)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		NOTIFY_HANDLER(IDC_SYSLINK2, NM_CLICK, OnNMClickSyslink2)
		NOTIFY_HANDLER(IDC_SYSLINK3, NM_CLICK, OnNMClickSyslink3)
		NOTIFY_HANDLER(IDC_SYSLINK4, NM_CLICK, OnNMClickSyslink4)
		NOTIFY_HANDLER(IDC_SYSLINK5, NM_CLICK, OnNMClickSyslink5)
		COMMAND_HANDLER(IDOK2, BN_CLICKED, OnBnClickedOk2)
		CHAIN_MSG_MAP(CDialogSkinMixer<CSelectMonthDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:

	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		TCHAR szClassName[256];
		if (GetClassName((HWND)lParam, szClassName, _countof(szClassName)))
		{
			if ((_tcsicmp(szClassName, CStatic::GetWndClassName()) == 0) || (_tcsicmp(szClassName, CLinkCtrl::GetWndClassName()) == 0))
			{
				CDCHandle dc((HDC)wParam);
				dc.SetTextColor(m_crTextColor);
				dc.SetBkColor(RGB(247, 252, 255));

				return LRESULT(m_bkBrush); 
			}
		}
		

		return NULL;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		return 0;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		INITCOMMONCONTROLSEX   cls={ sizeof(INITCOMMONCONTROLSEX) ,ICC_DATE_CLASSES }; 
		InitCommonControlsEx(&cls); 

		m_bkBrush = ::CreateSolidBrush(RGB(247,252,255));//暂时用取色的方式设置按钮的背景

		m_datatimebegin.Attach(GetDlgItem(IDC_DATETIMEPICKER1));
		m_datatimeend.Attach(GetDlgItem(IDC_DATETIMEPICKER2));
		m_link[0].Attach(GetDlgItem(IDC_SYSLINK1));//显示部分还有阴影，有时间删掉
		m_link[1].Attach(GetDlgItem(IDC_SYSLINK2));
		m_link[2].Attach(GetDlgItem(IDC_SYSLINK3));
		m_link[3].Attach(GetDlgItem(IDC_SYSLINK4));
		m_datatimebegin.SetFormat(_T("yyyy-MM-dd"));
		m_datatimeend.SetFormat(_T("yyyy-MM-dd"));


		CenterWindow(GetParent());
		SetWindowText(_T("请选择导入交易记录的起止时间"));
		m_btnSysClose.ShowWindow(SW_HIDE);

		ApplyButtonSkin(IDOK);
		ApplyButtonSkin(IDOK2);
		//ApplyButtonSkin(IDCANCEL);
		ApplyStaticSkin(IDC_STATIC1);
		ApplyStaticSkin(IDC_STATIC2);	

		//SetDlgItemText(IDC_STATIC1, L"请选择起止日期:");

		SYSTEMTIME btime[2];
		SYSTEMTIME atime;

		COleDateTimeSpan dt;
		dt.SetDateTimeSpan(30, 0, 0, 0); //(日，时，分，秒)，默认30天

		COleDateTime tm;		
		tm.ParseDateTime(m_beginTime.c_str());
		tm.GetAsSystemTime(btime[0]);

		GetLocalTime(&btime[1]);//获取当前的时间
		GetLocalTime(&atime);

		COleDateTime coletime(atime);
		coletime = coletime - dt;
		coletime.GetAsSystemTime(atime);		
		
		m_datatimebegin.SetRange(GDTR_MIN | GDTR_MAX, btime);
		m_datatimeend.SetRange(GDTR_MIN | GDTR_MAX, btime);

		m_datatimebegin.SetSystemTime(GDT_VALID, &atime);
		m_datatimeend.SetSystemTime(GDT_VALID, &btime[1]);
		
		return TRUE;
	}


	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

protected:

	CDateTimePickerCtrl m_datatimebegin;	//设置开始时间
	CDateTimePickerCtrl m_datatimeend;	//设置结束时间
	HBRUSH	m_bkBrush;

	wstring m_beginTime;
	
	CLinkCtrl m_link[4];
	int DateNum(int year,int  month);
	void GetLastMonth(int year,int  month, int& lyear,int& lmonth);
public:
	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNMClickSyslink1(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnDtnDatetimechangeDatetimepicker1(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnDtnDatetimechangeDatetimepicker2(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnNMClickSyslink2(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnNMClickSyslink3(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnNMClickSyslink4(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

private:
	SYSTEMTIME m_beginsel, m_endsel;
public:
	bool GetSelectTime(string& btime, string& etime); 
	void SetBeginTime(wstring begintime)
	{
		m_beginTime = begintime;
	}
	LRESULT OnNMClickSyslink5(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnBnClickedOk2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};