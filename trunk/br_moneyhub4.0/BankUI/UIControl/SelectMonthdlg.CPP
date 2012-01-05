#include "StdAfx.h"
#include "SelectMonthDlg.h"

LRESULT CSelectMonthDlg::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	//m_datatimebegin.SetMonthCalStyle(MCS_MULTISELECT);
	m_datatimebegin.GetSystemTime(&m_beginsel);
	m_datatimeend.GetSystemTime(&m_endsel);
	EndDialog(IDOK);
	return 0;
}
bool CSelectMonthDlg::GetSelectTime(string& btime, string& etime)
{
	char cbtime[256] = {0}, cetime[256] = {0};

	sprintf_s(cbtime, 256, "%d%02d%02d", m_beginsel.wYear, m_beginsel.wMonth, m_beginsel.wDay);
	sprintf_s(cetime, 256, "%d%02d%02d", m_endsel.wYear, m_endsel.wMonth, m_endsel.wDay);

	btime = cbtime;
	etime = cetime;
	return true;
}
LRESULT CSelectMonthDlg::OnNMClickSyslink1(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME btime[2];
	
	COleDateTime tm;		
	tm.ParseDateTime(m_beginTime.c_str());
	tm.GetAsSystemTime(btime[0]);

	GetLocalTime(&btime[1]);//获取当前的时间
	m_datatimebegin.SetRange(GDTR_MIN | GDTR_MAX, btime);
	m_datatimeend.SetRange(GDTR_MIN | GDTR_MAX, btime);

	m_datatimebegin.SetSystemTime(GDT_VALID, &btime[1]);
	m_datatimeend.SetSystemTime(GDT_VALID, &btime[1]);

	return 0;
}

LRESULT CSelectMonthDlg::OnDtnDatetimechangeDatetimepicker1(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	//DATETIMEPICKERINFO taginfo;
	//m_datatimebegin.GetDateTimePickerInfo(&taginfo);
	SYSTEMTIME btime[2];
	m_datatimebegin.GetSystemTime(&btime[0]);
	GetLocalTime(&btime[1]);//获取当前的时间
	m_datatimeend.SetRange(GDTR_MIN | GDTR_MAX, btime);

	return 0;
}

LRESULT CSelectMonthDlg::OnDtnDatetimechangeDatetimepicker2(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	//DATETIMEPICKERINFO taginfo;
	//m_datatimebegin.GetDateTimePickerInfo(&taginfo);
	SYSTEMTIME btime[2];
	m_datatimeend.GetSystemTime(&btime[1]);

	COleDateTime tm;		
	tm.ParseDateTime(m_beginTime.c_str());
	tm.GetAsSystemTime(btime[0]);

	m_datatimebegin.SetRange(GDTR_MIN | GDTR_MAX, btime);

	return 0;
}
LRESULT CSelectMonthDlg::OnNMClickSyslink2(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME btime[2];
	SYSTEMTIME atime;

	COleDateTimeSpan dt;
	dt.SetDateTimeSpan(7, 0, 0, 0); //(日，时，分，秒)，默认7天

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
	return 0;
}

LRESULT CSelectMonthDlg::OnNMClickSyslink3(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME btime[2];
	SYSTEMTIME atime;

	GetLocalTime(&btime[1]);//获取当前的时间
	GetLocalTime(&atime);

	int lyear, lmonth;
	GetLastMonth(atime.wYear, atime.wMonth, lyear, lmonth);

	int days = DateNum(lyear, lmonth);

	COleDateTimeSpan dt;
	dt.SetDateTimeSpan(days, 0, 0, 0); //(日，时，分，秒)

	COleDateTime tm;		
	tm.ParseDateTime(m_beginTime.c_str());
	tm.GetAsSystemTime(btime[0]);

	COleDateTime coletime(atime);
	coletime = coletime - dt;
	coletime.GetAsSystemTime(atime);

	m_datatimebegin.SetRange(GDTR_MIN | GDTR_MAX, btime);
	m_datatimeend.SetRange(GDTR_MIN | GDTR_MAX, btime);

	m_datatimebegin.SetSystemTime(GDT_VALID, &atime);
	m_datatimeend.SetSystemTime(GDT_VALID, &btime[1]);
	return 0;
}
void CSelectMonthDlg::GetLastMonth(int year,int  month, int& lyear,int& lmonth)
{
	if( month > 1)
	{
		lyear = year;
		lmonth = month - 1;
	}
	else
	{
		lyear = year - 1;
		lmonth = 12;
	}

}
int CSelectMonthDlg::DateNum(int year,int month) 
{ 
	if(month == 2) 
	{ 
		if((year%4 == 0 && year%100 != 0)|| year%400 == 0)
			return 29; 
		else
			return 28; 
	} 
	switch( month ) 
	{ 
		case   1:return   31; 
		case   3:return   31; 
		case   4:return   30; 
		case   5:return   31;   
		case   6:return   30; 
		case   7:return   31; 
		case   8:return   31; 
		case   9:return   30; 
		case   10:return   31;   
		case   11:return   30; 
		case   12:return   31; 
	} 
	return   0; 
} 
LRESULT CSelectMonthDlg::OnNMClickSyslink4(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME btime[2];
	SYSTEMTIME atime;

	GetLocalTime(&btime[1]);//获取当前的时间
	GetLocalTime(&atime);

	int lyear1, lmonth1;
	GetLastMonth(atime.wYear, atime.wMonth, lyear1, lmonth1);

	int days = DateNum(lyear1, lmonth1);

	int lyear2, lmonth2;
	GetLastMonth(lyear1, lmonth1, lyear2, lmonth2);

	days += DateNum(lyear2, lmonth2);

	int lyear3, lmonth3;
	GetLastMonth(lyear2, lmonth2, lyear3, lmonth3);

	days += DateNum(lyear3, lmonth3);

	COleDateTimeSpan dt;
	dt.SetDateTimeSpan(days, 0, 0, 0); //(日，时，分，秒)

	COleDateTime tm;		
	tm.ParseDateTime(m_beginTime.c_str());//L"1999-1-1 0:00:00"
	tm.GetAsSystemTime(btime[0]);

	COleDateTime coletime(atime);
	coletime = coletime - dt;
	coletime.GetAsSystemTime(atime);

	m_datatimebegin.SetRange(GDTR_MIN | GDTR_MAX, btime);
	m_datatimeend.SetRange(GDTR_MIN | GDTR_MAX, btime);

	m_datatimebegin.SetSystemTime(GDT_VALID, &atime);
	m_datatimeend.SetSystemTime(GDT_VALID, &btime[1]);
	return 0;

}

LRESULT CSelectMonthDlg::OnNMClickSyslink5(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	SYSTEMTIME btime[2];

	COleDateTime tm;		
	tm.ParseDateTime(m_beginTime.c_str());
	tm.GetAsSystemTime(btime[0]);

	GetLocalTime(&btime[1]);//获取当前的时间

	m_datatimebegin.SetRange(GDTR_MIN | GDTR_MAX, btime);
	m_datatimeend.SetRange(GDTR_MIN | GDTR_MAX, btime);

	m_datatimebegin.SetSystemTime(GDT_VALID, &btime[0]);
	m_datatimeend.SetSystemTime(GDT_VALID, &btime[1]);

	return 0;
}

LRESULT CSelectMonthDlg::OnBnClickedOk2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(::MessageBox(NULL, L"确认取消导入账单！", L"导入账单", MB_OKCANCEL) != IDOK)
		return 0;
	// TODO: Add your control notification handler code here
	EndDialog(IDCANCEL);
	return 0;
}
