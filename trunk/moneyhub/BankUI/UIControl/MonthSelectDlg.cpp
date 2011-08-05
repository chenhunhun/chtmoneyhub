#include "stdafx.h"
#include "MonthSelectDlg.h"


CMyCheckBox::CMyCheckBox():m_pStatic(NULL), m_rectNoStatic(0, 0, 0, 0), m_rectWhole(0, 0, 0, 0), m_nID(0)
{
	//m_bChecked(true), m_dwStatue(eCBS_Checked), 
}

bool CMyCheckBox::Create(HWND hParent, CRect& rect, int nID, const char* pStr, bool bCheck)
{
	// 由于复选框的图片是16*16，所以高度默认为16
	ATLASSERT((rect.right - rect.left >= 16) && (rect.bottom - rect.top >= 16));
	if ((rect.right - rect.left < 16) || (rect.bottom - rect.top < 16))
		return false;

	m_nID = nID;
	m_bChecked = bCheck ? m_dwStatue = eCBS_Checked : m_dwStatue = eCBS_UnChecked;

	m_rectNoStatic = CRect(rect.left, rect.top, rect.left + 16, rect.top + 16);
	m_rectWhole = rect;

	m_pStatic= new CStatic();
	m_pStatic->Create(hParent, CRect(rect.left + 16, rect.top, rect.right, rect.bottom), CA2W(pStr), WS_CHILD|WS_VISIBLE|SS_CENTER);
	m_pStatic->ShowWindow(true);

	return true;
}


void CMyCheckBox::GetCtrlRectWithoutStatic(CRect& rect)
{
	rect = m_rectNoStatic;
}

void CMyCheckBox::GetCtrlRect(CRect& rect)
{
	rect = m_rectWhole;
}

DWORD CMyCheckBox::GetCheckStatues(void)
{
	return m_dwStatue;
}
void CMyCheckBox::ButtonClick()
{
	m_bChecked = !m_bChecked;
}

void CMyCheckBox::SetCheckStatues(DWORD bStatues)
{
	m_dwStatue = bStatues;
}

bool CMyCheckBox::IsChecked()
{
	return m_bChecked;
	/*switch(m_dwStatue)
	{
	case eCBS_Checked:
	case eCBS_CheckedHover:
	case eCBS_CheckedActive:
	case eCBS_CheckedDisable:
		return true;
	default:
		break;
	}

	return false;*/
}

int CMyCheckBox::GetCtrlID()
{
	return m_nID;
}



CMonthSelectDlg::CMonthSelectDlg(SelectMonthNode* pMonthNode, char* pDlgText, char* pTitleText):m_pMonthNode(pMonthNode), m_pStaticeTitle(NULL),
m_strDlgText(pDlgText), m_strTitleText(pTitleText)
{
}

CMonthSelectDlg::~CMonthSelectDlg()
{
	if (NULL == m_pStaticeTitle)
		delete m_pStaticeTitle;

	m_pStaticeTitle = NULL;

	MyCheckBoxList::const_iterator it;
	for(it = m_CheckBoxArr.begin(); it != m_CheckBoxArr.end(); it ++)
	{
		CMyCheckBox* pNode = (*it);
		ATLASSERT(NULL != pNode);
		if (NULL == pNode)
			continue;
		delete pNode;
	}

	m_CheckBoxArr.clear();
}

LRESULT CMonthSelectDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// 设置对话框文本
	if (m_strDlgText.empty())
		SetWindowText(_T("导出账单"));
	else
		SetWindowTextA(m_hWnd, m_strDlgText.c_str());
	
	// 设置界面上显示的标题文本
	if (m_strTitleText.empty())
		m_strTitleText = "选择要下载的账单月份";

	m_imgCheckBox.m_strFileName = L"tips_checkbox";
	m_imgCheckBox.LoadFromFile();

	CRect rectDlg;
	GetClientRect(&rectDlg);

	const int nHigh = 10;
	const int nSperate = 50;
	const int nBtnHight = 16;

	const int nXBorderLeft = 40;
	const int nYBorderLeft = 80;
	int nDHight = nYBorderLeft;
	//int nDWeight = nXBorderLeft;
	int nTpWeight = nXBorderLeft;
	//int nLine = 0;
	int nID = 7000;

	SelectMonthNode::const_iterator it = m_pMonthNode->begin();
	while (it != m_pMonthNode->end())
	{
		std::string str = (*it).first;
		int nLen = str.length();
		double dCheckBtnWidth = 16.0 + (59.0 / 9.0) * (nLen + 1);
		//int nCheckBtnWidth = 16 + (59 / 9) * (nLen + 1);
		CMyCheckBox* pNode = new CMyCheckBox();

		bool bShowOneLine = (*it).second & CHECKBOX_SHOW_ONE_LINE;
		if ((nTpWeight + (int)dCheckBtnWidth > rectDlg.right - nXBorderLeft) || bShowOneLine)
		{
			if (nTpWeight > nXBorderLeft)
			{
				nDHight += nBtnHight;
				nDHight += nHigh;
				nTpWeight = nXBorderLeft;
			}
		}


		int xBegin = nTpWeight; 
		int yBegin = nDHight;

		nTpWeight += (int)dCheckBtnWidth;
		nTpWeight += nSperate;
		

		bool bChecked = (*it).second & CHECKBOX_SHOW_CHECKED;
		pNode->Create(m_hWnd, CRect(xBegin, yBegin, xBegin + (int)dCheckBtnWidth, nDHight + nBtnHight), nID ++, str.c_str(), bChecked);

		m_CheckBoxArr.push_back(pNode);

		it ++;

		if(bShowOneLine && it != m_pMonthNode->end())
		{
			nDHight += nBtnHight;
			nDHight += nHigh;
			nTpWeight = nXBorderLeft;
		}

		//CButton* pButton = new CButton();
		//pButton->Create(m_hWnd, rect, _T(""), WS_CHILD | BS_AUTOCHECKBOX, 0, 7000);//
		//pButton->ShowWindow(true);
		/*CStatic* pStatic= new CStatic();
		pStatic->Create(m_hWnd, CRect(140,124,199,150), CA2W(str.c_str()), WS_CHILD|WS_VISIBLE|SS_CENTER);
		pStatic->ShowWindow(true);
		m_CheckBoxArr.insert(std::make_pair(pNode, pStatic));*/
	}

	ApplyButtonSkin(IDOK);

	// 确定按钮定位
	CRect rectBtn;
	::GetClientRect(GetDlgItem(IDOK), &rectBtn);
	::MoveWindow(GetDlgItem(IDOK), rectDlg.Width() / 2 - rectBtn.Width() / 2, nDHight + (nYBorderLeft / 2) - (rectBtn.Height() / 4), rectBtn.Width(), rectBtn.Height(), true);


	double dTitleWidth = 16.0 + (59.0 / 9.0) * (m_strTitleText.length() + 1);
	m_pStaticeTitle = new CStatic();
	CRect rectTitle(rectDlg.Width() / 2 - (int)dTitleWidth / 2, 3 * nYBorderLeft / 4 - 8, rectDlg.Width() / 2 - (int)dTitleWidth / 2 + dTitleWidth, 3 * nYBorderLeft / 4 + 8);
	m_pStaticeTitle->Create(m_hWnd, rectTitle, CA2W(m_strTitleText.c_str()), WS_CHILD|WS_VISIBLE|SS_CENTER);
	m_pStaticeTitle->ShowWindow(true);/**/


	// 设备对话框大小
	MoveWindow(0, 0, rectDlg.Width(), nDHight + nYBorderLeft);

	CenterWindow(GetParent());
	return 0;
}

LRESULT CMonthSelectDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	//CRect rect;
	////::GetWindowRect(
	//::GetWindowRect(GetDlgItem(7000), &rect);
	//ScreenToClient(&rect);

	MyCheckBoxList::const_iterator it;
	for(it = m_CheckBoxArr.begin(); it != m_CheckBoxArr.end(); it ++)
	{
		CMyCheckBox* pNode = (*it);
		CRect rectBtn;
		pNode->GetCtrlRectWithoutStatic(rectBtn);
		CSkinManager::DrawImagePart(dc, rectBtn , m_imgCheckBox , pNode->GetCheckStatues() , 8) ;
	}

	return 0;
}

void CMonthSelectDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	MyCheckBoxList::const_iterator it;
	for(it = m_CheckBoxArr.begin(); it != m_CheckBoxArr.end(); it ++)
	{
		CMyCheckBox* pNode = (*it);
		CRect rectBtn;
		pNode->GetCtrlRect(rectBtn);
		CRect rectCheck;
		pNode->GetCtrlRectWithoutStatic(rectCheck);
		DWORD dwStatues = pNode->GetCheckStatues();

		if ( rectBtn.PtInRect(point) )
		{
			CDCHandle hdc = GetDC();
			switch(dwStatues)
			{
			case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:
				dwStatues =  eCBS_UnCheckedHover;
				break;
			case eCBS_UnChecked:case eCBS_UnCheckedHover:case eCBS_UnCheckedActive:
				dwStatues =  eCBS_CheckedHover;
				break;
			default:
				break;
			}
			pNode->ButtonClick();
			pNode->SetCheckStatues(dwStatues);

			CSkinManager::DrawImagePart(hdc , rectCheck , m_imgCheckBox , dwStatues , 8) ;
			//此处添加对下次显示的配置

			ReleaseDC(hdc);
			hdc = NULL; 
		}
	}
}

void CMonthSelectDlg::OnLButtonUp(UINT nFlags, CPoint point)
{}

void CMonthSelectDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CDCHandle hdc = GetDC();

	MyCheckBoxList::const_iterator it;
	for(it = m_CheckBoxArr.begin(); it != m_CheckBoxArr.end(); it ++)
	{
		CMyCheckBox* pNode = (*it);
		CRect rectBtn;
		pNode->GetCtrlRect(rectBtn);
		CRect rectCheck;
		pNode->GetCtrlRectWithoutStatic(rectCheck);
		bool bCheck = pNode->IsChecked();
		DWORD dwStatues = pNode->GetCheckStatues();

	if ( rectBtn.PtInRect(point) )
		{
			switch(dwStatues)
			{
			case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:
				if ( bCheck )
					dwStatues = eCBS_CheckedActive ;
				else 
					dwStatues = eCBS_CheckedHover ;
				break;
			case eCBS_UnChecked:case eCBS_UnCheckedActive:case eCBS_UnCheckedHover:
				if ( bCheck )
					dwStatues = eCBS_UnCheckedActive ;
				else
					dwStatues = eCBS_UnCheckedHover ;
				break;
			default:
				break;
			}
		}
		else
		{
			switch(dwStatues)
			{
			case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:
				dwStatues = eCBS_Checked ;
				break;
			case eCBS_UnChecked:case eCBS_UnCheckedActive:case eCBS_UnCheckedHover:
				dwStatues = eCBS_UnChecked ;
				break;
			default:
				break;
			}
		}

		pNode->SetCheckStatues(dwStatues);
		CSkinManager::DrawImagePart(hdc , rectCheck , m_imgCheckBox , dwStatues , 8) ;

		//break;
	}

		ReleaseDC(hdc);
		hdc = NULL; 
}

LRESULT CMonthSelectDlg::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	std::list<CMyCheckBox*>::const_iterator cstCBox = m_CheckBoxArr.begin();
	std::map<char*, DWORD>::iterator cstIt = m_pMonthNode->begin();
	for(; cstCBox != m_CheckBoxArr.end() && cstIt != m_pMonthNode->end(); cstCBox ++ , cstIt ++)
	{
		CMyCheckBox* pTpBox = (*cstCBox);
		DWORD dwChecked = CHECKBOX_SHOW_UNCHECKED;
		if (pTpBox->IsChecked())
			dwChecked = CHECKBOX_SHOW_CHECKED;

		(*cstIt).second = dwChecked;
	}

	EndDialog(0);
	return 0;
}