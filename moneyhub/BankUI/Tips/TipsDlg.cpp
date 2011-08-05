/**
 *-----------------------------------------------------------*
 *  版权所有：  (c), 2010 - 2999, 北京融信恒通科技有限公司
 *    文件名：  main.cpp
 *      说明：  Tips对话框类的实现文件
 *    版本号：  1.0.0
 * 
 *  版本历史：
 *	版本号		日期	作者	说明
 *	1.0.0	2010.10.22	融信恒通	初始版本

 *  开发环境：
 *  Visual Studio 2008
 *-----------------------------------------------------------*
 */
#include "stdafx.h"
#include "TipsDlg.h"
#include <string>
#include "Util.h"
#include "ConvertBase.h"
using namespace std;

CTipsDlg::CTipsDlg(int page):TOTAL_PAGE(3)
{
	m_page = page;//设置属性
	if(m_page < 1 || m_page > TOTAL_PAGE)
		m_page = 1;
}

CTipsDlg::~CTipsDlg()
{
}

LRESULT CTipsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	SetWindowText(_T("使用向导"));

 	GetConfig();
	
	m_tipsString = GetDlgItem(IDC_MESSAGE_SHOW);
	m_titleString.Attach(GetDlgItem(IDC_STATIC_TITLE));
	m_pic.Attach(GetDlgItem(IDB_BITMAP_SHOW));
	m_guideString.Attach(GetDlgItem(IDC_GUIDE));

	m_titleString.SetFont(m_fontTitle);
	m_tipsString.SetFont(m_fontText);
	m_guideString.SetFont(m_fontText);

	ApplyButtonSkin(IDC_PREPAGE);
	ApplyButtonSkin(IDC_NEXTPAGE);
	ApplyButtonSkin(IDCANCEL);

	DWORD dwType;
	DWORD dwValue;
	DWORD dwReturnBytes = sizeof(DWORD);

	m_bChecked = false;
	if (ERROR_SUCCESS == ::SHGetValueW(HKEY_CURRENT_USER, L"Software\\Bank\\Setting",L"IsGuideShow", &dwType, &dwValue, &dwReturnBytes))
	{
		m_bChecked = !dwValue;//设置选中状态,0表示未选中
	}

	m_imgCheckBox.m_strFileName = L"tips_checkbox";
	m_imgCheckBox.LoadFromFile();

	CRect mt;
	::GetWindowRect(GetDlgItem(IDC_CHECK1),&mt);
	ScreenToClient(&mt);
	m_dwCheckState = m_bChecked ? eCBS_Checked:eCBS_UnChecked ;
	m_rcCheckBox = CRect(mt.left,mt.top+4,mt.left+16,mt.top+20);
	m_rcCheckBoxRgn = CRect ( &mt);

	Refresh();

	return TRUE;
}

LRESULT CTipsDlg::OnPrePage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_page--;
	Refresh();

	return 0;
}

LRESULT CTipsDlg::OnNextPage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_page++;
	Refresh();

	return 0;
}

LRESULT CTipsDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);

	return 0;
}

void CTipsDlg::Refresh()
{
	::ShowWindow(GetDlgItem(IDC_PREPAGE), SW_SHOW);
	::ShowWindow(GetDlgItem(IDC_NEXTPAGE), SW_SHOW);
	if(m_page <= 1)
	{
		::ShowWindow(GetDlgItem(IDC_PREPAGE), SW_HIDE);
	}
	else if(TOTAL_PAGE <= m_page)
	{
		::ShowWindow(GetDlgItem(IDC_NEXTPAGE), SW_HIDE);
	}
	
	m_tipsString.SetWindowText(m_showstring[m_page-1].c_str());
	m_titleString.SetWindowText(m_showtitle[m_page-1].c_str());

	Invalidate();
}

LRESULT CTipsDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	CRect rc;
	
	m_pic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_pic.SetWindowPos(m_hWnd,rc.left,rc.top,m_showList[m_page-1].GetWidth(),m_showList[m_page-1].GetHeight(),SWP_NOZORDER);

	m_pic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	CSkinManager::DrawImagePart(dc, m_rcCheckBox , m_imgCheckBox , m_dwCheckState , 8) ;
	CSkinManager::DrawImagePart(dc, rc , m_showList[m_page-1] , 0 , 1) ;

	return 0;
}

void CTipsDlg::OnMouseMove(UINT nFlags, CPoint point)
{

	CDCHandle hdc = GetDC();

	if ( m_rcCheckBoxRgn.PtInRect(point) )
		{
			switch(m_dwCheckState)
			{
			case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:
				if ( m_bChecked )
					m_dwCheckState = eCBS_CheckedActive ;
				else 
					m_dwCheckState = eCBS_CheckedHover ;
				break;
			case eCBS_UnChecked:case eCBS_UnCheckedActive:case eCBS_UnCheckedHover:
				if ( m_bChecked )
					m_dwCheckState = eCBS_UnCheckedActive ;
				else
					m_dwCheckState = eCBS_UnCheckedHover ;
				break;
			default:
				m_dwCheckState;
				break;
			}
		}
		else
		{
			switch(m_dwCheckState)
			{
			case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:
				m_dwCheckState = eCBS_Checked ;
				break;
			case eCBS_UnChecked:case eCBS_UnCheckedActive:case eCBS_UnCheckedHover:
				m_dwCheckState = eCBS_UnChecked ;
				break;
			default:
				m_dwCheckState;
				break;
			}
		}
		CSkinManager::DrawImagePart(hdc , m_rcCheckBox , m_imgCheckBox , m_dwCheckState , 8) ;

		ReleaseDC(hdc);
		hdc = NULL; 
}

void CTipsDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_rcCheckBoxRgn.PtInRect(point) )
	{
		CDCHandle hdc = GetDC();
		switch(m_dwCheckState)
		{
		case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:
			m_dwCheckState =  eCBS_UnCheckedHover;
			break;
		case eCBS_UnChecked:case eCBS_UnCheckedHover:case eCBS_UnCheckedActive:
			m_dwCheckState =  eCBS_CheckedHover;
			break;
		default:
			m_dwCheckState;
			break;
		}
		m_bChecked = !m_bChecked;//取反
		CSkinManager::DrawImagePart(hdc , m_rcCheckBox , m_imgCheckBox , m_dwCheckState , 8) ;
		//此处添加对下次显示的配置

		DWORD i = (m_bChecked == true) ? 0:1;
		::SHSetValueW(HKEY_CURRENT_USER,L"Software\\Bank\\Setting",L"IsGuideShow",REG_DWORD,&i,4);
			ReleaseDC(hdc);
		hdc = NULL; 
	}
}

bool CTipsDlg::IsVista()
{
	OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
	BOOL bRet = ::GetVersionEx(&ovi);
	return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6));
}

void CTipsDlg::GetConfig()
{
	WCHAR swPath[MAX_PATH] = {0};
	char sPath[MAX_PATH] = {0};
	::GetModuleFileNameA(NULL, sPath, MAX_PATH);
	::GetModuleFileNameW(NULL, swPath, MAX_PATH);

	::PathRemoveFileSpecA(sPath);
	::PathRemoveFileSpecW(swPath);
	string str(sPath);
	str += "\\Html\\Tips\\config.xml";
	TiXmlDocument tipsConfig(str.c_str());
	tipsConfig.LoadFile(TIXML_ENCODING_UTF8);

	TiXmlNode *pRoot = tipsConfig.FirstChild("Guide");
	if (!pRoot)
		return;
	
	TOTAL_PAGE = 0; 

	for (TiXmlNode *pPage = pRoot->FirstChild("Page"); pPage != NULL; pPage = pRoot->IterateChildren("Page", pPage))
	{
		TiXmlNode *pMessage = pPage->FirstChild("Message");
		if(pMessage)
			m_showstring[TOTAL_PAGE] = AToW(pMessage->FirstChild()->Value());

		string strType = pPage->ToElement()->Attribute("Attr");
		m_showtitle[TOTAL_PAGE] = AToW(pPage->ToElement()->Attribute("Title"));
		wstring strFileName(swPath);
		strFileName += AToW(pPage->ToElement()->Attribute("PagePic"));
		m_showList[TOTAL_PAGE].LoadFromFile(strFileName.c_str(),false);
		TOTAL_PAGE ++;		
	}

}