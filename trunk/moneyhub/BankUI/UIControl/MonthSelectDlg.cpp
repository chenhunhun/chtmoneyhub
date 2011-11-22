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
	m_pStatic->Create(hParent, CRect(rect.left + 22, rect.top, rect.right, rect.bottom), CA2W(pStr), WS_CHILD|WS_VISIBLE|SS_LEFT);
	m_pStatic->ShowWindow(true);

	m_font.CreatePointFont(90, _T("宋体"), NULL, false, false);
	m_pStatic->SetFont((HFONT)m_font);

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

	if (eCBS_Checked == bStatues)
		m_bChecked = true;
	if (eCBS_UnChecked == bStatues)
		m_bChecked = false;
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

///////////////////////////////////////////////////
/*				CMonthSelectDlg					*/
//////////////////////////////////////////////////

CMonthSelectDlg::CMonthSelectDlg(list<SELECTINFONODE>* pMonthNode, char* pDlgText, char* pTitleText):m_pMonthNode(pMonthNode)//, m_pStaticeTitle(NULL)
{
	if (pDlgText != NULL)
		m_strDlgText = pDlgText;
	if (pTitleText != NULL)
		m_strTitleText = pTitleText;

	
	m_pStaticSelAll = NULL;
	m_pStaticSelNon = NULL;
}

CMonthSelectDlg::~CMonthSelectDlg()
{
	/*if (NULL != m_pStaticeTitle)
		delete m_pStaticeTitle;
	m_pStaticeTitle = NULL;*/

	if (NULL != m_pStaticSelAll)
		delete m_pStaticSelAll;
	m_pStaticSelAll = NULL;

	if (NULL != m_pStaticSelNon)
		delete m_pStaticSelNon;
	m_pStaticSelNon = NULL;


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

//bool MyCmp(string x,string y)
//{
//	int i;
//	for(i = 0; i < x.length(); i++)
//		if(x[i] >= 'A' && x[i] <= 'Z')
//			x[i] = 'a' + (x[i] - 'A');
//	for(i = 0; i < y.length(); i++)
//		if(y[i] >= 'A' && y[i] <= 'Z')
//			y[i] = 'a' + (y[i] - 'A');
//	return x < y; 
//}


LRESULT CMonthSelectDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_sFont.CreatePointFont(90, _T("Tahoma"));
	// 设置对话框文本
	if (m_strDlgText.empty())
		SetWindowText(_T("选择要下载的账单月份"));
	else
		SetWindowTextA(m_hWnd, m_strDlgText.c_str());
	
	// 设置界面上显示的标题文本
	if (m_strTitleText.empty())
		m_strTitleText = "选择要下载的账单月份";

	int nTotalColumn = 2;//计算要显示的列数，最大为2列

	m_imgCheckBox.m_strFileName = L"tips_checkbox";
	m_imgCheckBox.LoadFromFile();

	CRect rectDlg;
	GetClientRect(&rectDlg);

	const int nDlgWeight = 474;//最大可用宽度和高度
	const int nDlgHeight = 452;

	//下面这两个是可变的
	int nHighSperate = 30;//行间高度
	int nSperate = 20;//中间的列间距，最小为20,最大为60

	const int nBtnHight = 16;

	int nXBorderLeft = 40;//左右2侧最小
	const int nYBorderLeft = 80;//上面的预留长度
	int nID = 7000;

	string strConst = "（已导入）";
	// 找出字符串最长的长度
	list<SELECTINFONODE>::const_iterator it = m_pMonthNode->begin();

	double dBigBtnWidth;
	int nBigLen = 0;
	while (it != m_pMonthNode->end())
	{
		std::string str = (*it).szNodeInfo;
		int nLen = str.length();

		if ((*it).dwVal & CHECKBOX_SHOW_CHECKED_BEFORE) // 如果是已导入的项
			nLen += strConst.length();

		if (nLen > nBigLen)
			nBigLen = nLen;
		it ++;		
	}

	// 计算一最宽的一列
	dBigBtnWidth = 22.0 + 6 * (nBigLen + 1);

	int lLength = 2 * dBigBtnWidth + nSperate + 80;//两端的最大列间距
	
	if(lLength > nDlgWeight)
	{
		nTotalColumn = 1;//2列显示不下，显示1列
	}
	else if((lLength + 10 > nDlgWeight)&&(lLength < nDlgWeight))//列宽为30
	{
		nSperate = 30;
	}
	else if((lLength + 20 > nDlgWeight)&&(lLength < nDlgWeight))//列宽为30
	{
		nSperate = 40;
	}
	else if((lLength + 30 > nDlgWeight)&&(lLength < nDlgWeight))//列宽为30
	{
		nSperate = 50;
	}
	else
		nSperate = 60;

	//计算两边的间隙长度
	if(nTotalColumn == 1)
		nXBorderLeft = (int)((nDlgWeight - dBigBtnWidth) / 2);

	else
		nXBorderLeft = (int)((nDlgWeight - 2 * dBigBtnWidth - nSperate) / 2);

	//上面计算出了列数和中间的间隙长度，下面计算行数和行宽

	int nLine = m_pMonthNode->size();
	if( m_pMonthNode->size() % 2 != 0)
		nLine += 1;

	nLine = nLine / nTotalColumn;//计算出行数
	
	while((nLine * (16 + nHighSperate)) > nDlgHeight)
	{
		nHighSperate -= 5;//算出最大高度
	}

	if(nHighSperate < 0)
		MessageBox(L"列数过多",L"财金汇",MB_OK);

	//下面开始进行创建字体
	
	it = m_pMonthNode->begin();
	int nNum = 0;
	while (it != m_pMonthNode->end())
	{
		std::string str = (*it).szNodeInfo;

		if ((*it).dwVal & CHECKBOX_SHOW_CHECKED_BEFORE) // 已导入项
		{
			str += strConst;
		}
		int nLen = str.length();

		CMyCheckBox* pNode = new CMyCheckBox();

		int xBegin = 0; 
		int yBegin = 0;

		if (nTotalColumn == 1)//1列
		{
			xBegin = nXBorderLeft;
			yBegin = nNum * (nHighSperate + 16) + nYBorderLeft;
		}
		else//2列
		{
			if(nNum % 2 == 0)
			{
				xBegin = nXBorderLeft;
				yBegin = nNum / 2 * (nHighSperate + 16) + nYBorderLeft;
			}
			else
			{
				xBegin = nXBorderLeft + dBigBtnWidth + nSperate;
				yBegin = (nNum - 1) / 2 * (nHighSperate + 16) + nYBorderLeft;
			}

		}

		bool bChecked = (*it).dwVal & CHECKBOX_SHOW_CHECKED;
		pNode->Create(m_hWnd, CRect(xBegin, yBegin, xBegin + (int)dBigBtnWidth, yBegin + nBtnHight), nID ++, str.c_str(), bChecked);

		m_CheckBoxArr.push_back(pNode);

		it ++;
		nNum ++;
	}

	int nDHight = nLine *  (nHighSperate + 16) + nYBorderLeft;//下面这个预留长度需要调试
	string strText = "全部选择";
	double dWidth = 7.5 * (strText.length() + 1);
	m_pStaticSelAll = new CStatic();
	m_cRectSelAll.left = nXBorderLeft;
	m_cRectSelAll.top = nDHight;
	m_cRectSelAll.right = nXBorderLeft + (int)dWidth;
	m_cRectSelAll.bottom = nDHight + nBtnHight;//CRect(nXBorderLeft, nDHight, nXBorderLeft + (int)dWidth,nDHight + nBtnHight), CA2W(strText.c_str())
	m_pStaticSelAll->Create(m_hWnd, m_cRectSelAll, CA2W(strText.c_str()), WS_CHILD|WS_VISIBLE|SS_LEFT, 0, STATIC_CTRL_ID_SEL_ALL);
	m_pStaticSelAll->ShowWindow(true);
	m_pStaticSelAll->SetFont((HFONT)m_sFont);

	int nStaticLeft = nXBorderLeft + (int)dWidth + 30;
	strText = "全部取消";
	dWidth = 7.5 * (strText.length() + 1);
	m_pStaticSelNon = new CStatic();
	m_cRectSelNon.left = nStaticLeft;
	m_cRectSelNon.top = nDHight;
	m_cRectSelNon.right = nStaticLeft + (int)dWidth;
	m_cRectSelNon.bottom =nDHight + nBtnHight;
	m_pStaticSelNon->Create(m_hWnd, m_cRectSelNon, CA2W(strText.c_str()), WS_CHILD|WS_VISIBLE|SS_LEFT, 0, STATIC_CTRL_ID_SEL_NON);
	m_pStaticSelNon->ShowWindow(true);
	m_pStaticSelNon->SetFont((HFONT)m_sFont);

	ApplyButtonSkin(IDOK);
	ApplyButtonSkin(IDCANCEL);

	// 确定按钮定位
	CRect rectBtn;
	::GetClientRect(GetDlgItem(IDOK), &rectBtn);
	::MoveWindow(GetDlgItem(IDOK), rectDlg.Width() / 2 - rectBtn.Width() / 2 - 80, nDHight + (nYBorderLeft / 2) - (rectBtn.Height() / 4), rectBtn.Width(), rectBtn.Height(), true);

	::GetClientRect(GetDlgItem(IDCANCEL), &rectBtn);
	::MoveWindow(GetDlgItem(IDCANCEL), rectDlg.Width() / 2 - rectBtn.Width() / 2 + 80, nDHight + (nYBorderLeft / 2) - (rectBtn.Height() / 4), rectBtn.Width(), rectBtn.Height(), true);

	/*double dTitleWidth = 16.0 + (59.0 / 9.0) * (m_strTitleText.length() + 1);
	m_pStaticeTitle = new CStatic();
	CRect rectTitle(rectDlg.Width() / 2 - (int)dTitleWidth / 2, 3 * nYBorderLeft / 4 - 8, rectDlg.Width() / 2 - (int)dTitleWidth / 2 + dTitleWidth, 3 * nYBorderLeft / 4 + 8);
	m_pStaticeTitle->Create(m_hWnd, rectTitle, CA2W(m_strTitleText.c_str()), WS_CHILD|WS_VISIBLE|SS_CENTER);
	m_pStaticeTitle->ShowWindow(true);*/

	// 隐藏关闭按钮
	m_btnSysClose.ShowWindow(SW_HIDE);

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

	// 看看选中的是不是全选
		CRect rectStatic;
	if (NULL != m_pStaticSelAll)
	{
		if (m_cRectSelAll.PtInRect(point))
		{
			OnSetAllCheckButtonStatus(eCBS_Checked);
		}
	}

	// 看看选中的是不是全不选

	if (NULL != m_pStaticSelNon)
	{
		if (m_cRectSelNon.PtInRect(point))
		{
			OnSetAllCheckButtonStatus(eCBS_UnChecked);
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
	list<SELECTINFONODE>::iterator cstIt = m_pMonthNode->begin();
	bool bSel = false;
	for(; cstCBox != m_CheckBoxArr.end() && cstIt != m_pMonthNode->end(); cstCBox ++ , cstIt ++)
	{
		CMyCheckBox* pTpBox = (*cstCBox);
		DWORD dwChecked = CHECKBOX_SHOW_UNCHECKED;
		if (pTpBox->IsChecked())
		{
			dwChecked = CHECKBOX_SHOW_CHECKED;
			bSel = true;
		}

		(*cstIt).dwVal = dwChecked;
	}

	if (!bSel)
		::MessageBox(NULL, L"您没有选择要导入的月份，请重新选择！", L"月份选择", MB_OK | MB_ICONINFORMATION);
	else
		EndDialog(IDOK);
	return 0;
}
LRESULT CMonthSelectDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(::MessageBox(NULL, L"确认取消导入账单！", L"导入账单", MB_OKCANCEL) != IDOK)
		return 0;
	EndDialog(IDCANCEL);
	return 0;
}

void CMonthSelectDlg::OnSetAllCheckButtonStatus(DWORD dwStatus)
{
	CDCHandle hdc = GetDC();

	MyCheckBoxList::const_iterator it;
	for(it = m_CheckBoxArr.begin(); it != m_CheckBoxArr.end(); it ++)
	{
		CMyCheckBox* pNode = (*it);
		CRect rectCheck;
		pNode->GetCtrlRectWithoutStatic(rectCheck);

		//DWORD dwStatues = eCBS_Checked;
		pNode->SetCheckStatues(dwStatus);
		
		CSkinManager::DrawImagePart(hdc , rectCheck , m_imgCheckBox , dwStatus , 8) ;

	}

	ReleaseDC(hdc);
	hdc = NULL;
}
