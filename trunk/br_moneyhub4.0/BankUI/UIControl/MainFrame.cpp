#include "stdafx.h"
#include "../Util/ProcessManager.h"
#include "../Util/Util.h"
#include "../Util/SelfUpdate.h"
#include "../Include/Util.h"
#include "TabCtrl/TabItem.h"
#include "CategoryCtrl/CategoryItem.h"
#include "MainFrame.h"
#include "SettingDlg.h"
#include "AboutDlg.h"
#include "ChildFrm.h"
#include "SelectMonthDlg.h"


#include "../../Utils/HardwareID/genhwid.h"
#include "../../Utils/sn/SNManager.h"
#include "Version.h"
#include "CoolMenuHook.h"
#include "../Tips/TipsDlg.h"
#include "../../Utils/Config/HostConfig.h"
#include "../../Include/ConvertBase.h"
#include "../../Utils/UserBehavior/UserBehavior.h"
#include "../../BankData/BankData.h"
#include "../../Utils/SecurityCache/comm.h"
#include "ShowInfoBar.h"
#include "ShowJSFrame.h"

#include "../Synchro/SynchroDlg.h"


#define TIMER_AUTOUPDATE (0xf0)
#define TIMER_BCHECKKERNEL (0xf1)
#define TIMER_CHECKKERNEL (0xf2)

const UINT WM_BROADCAST_QUIT = RegisterWindowMessage(_T("BankQuitBroadcastMsg"));
const UINT WM_UPDATERETRY = RegisterWindowMessage(_T("BankUpdateRetryMsg"));
const UINT WM_SWITCHTOPPAGE = RegisterWindowMessage(_T("BankSwitchTopPageMsg"));
const UINT WM_SVRCHECKUSER = RegisterWindowMessage(_T("BankSvrCheckUser"));


#define USER_LOAD_INFO_KEEP_TIME 10// 10 表示由该按钮弹出对话框保留10秒


CMainFrame::CMainFrame() : CFSMUtil(&m_tsm), m_ShowInfo(&m_tsm), m_BigButton(&m_tsm), m_TabCtrl(&m_tsm), m_CatetoryCtrl(&m_tsm), m_StatusBar(&m_tsm),
	m_SysBtnBar(&m_tsm), m_TitleBar(&m_tsm), m_Toolbar(&m_tsm), m_wndMDIClient(m_TabCtrl, m_CatetoryCtrl, &m_tsm), m_hDWP(NULL), m_MenuButton(&m_tsm),//m_LogoButton(&m_tsm), 
	m_BackButton(&m_tsm), m_ForwardButton(&m_tsm), m_RefreshButton(&m_tsm), m_HelpButton(&m_tsm),m_IsShowCloudMessage(false),
	m_IsShowCloudStatus(false), m_MenuDlg(NULL), m_LoadButton(&m_tsm), m_LoginButton(&m_tsm), m_SepButton(&m_tsm), m_pTextButton(NULL),// m_AlarmButton(&m_tsm),
	m_SynchroButton(&m_tsm), m_pUserInfoBtn(NULL), m_InfoDlg(NULL), m_pShowDlg(NULL), m_pLoadingButton(NULL), m_UserMenuDlg(NULL)//,m_SettingButton(&m_tsm)
{
	FS()->pMainFrame = this;
}


CMainFrame::~CMainFrame()
{
	if (NULL != m_MenuDlg)
		delete m_MenuDlg;

	if (NULL != m_pTextButton)
		delete m_pTextButton;
	m_pTextButton = NULL;

	if (NULL != m_pUserInfoBtn)
		delete m_pUserInfoBtn;
	m_pUserInfoBtn = NULL;

	if (NULL != m_InfoDlg)
		delete m_InfoDlg;
	m_InfoDlg = NULL;

	if (NULL != m_UserMenuDlg)
		delete m_UserMenuDlg;
	m_UserMenuDlg = NULL;
}

LRESULT CMainFrame::UpdateUserLoadStatus(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)// 更新用户登录状态
{
	
	// 把正在登录按钮去掉
	if (NULL != m_pLoadingButton)
	{
		delete m_pLoadingButton;
		m_pLoadingButton = NULL;
	}

	// 参数是按stoken#mail#userid#发送过来
	string strRead = (LPSTR)lParam;

	// 更新UI中的登录用户信息(供同步使用)
	string strTag = MY_PARAM_END_TAG;
	// 读取stoken
	CBankData::GetInstance()->m_CurUserInfo.strstoken = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

	// 读取Mail
	strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
	string strMail = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));
	CBankData::GetInstance()->m_CurUserInfo.strmail = strMail;

	// 读取userid
	strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
	CBankData::GetInstance()->m_CurUserInfo.struserid = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

	// 读取 邮箱认证信息
	strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
	string strMailVerif = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

	// 读取 用户登录状态
	strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
	string strUserStatus = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

	USERSTATUS emStatus = (USERSTATUS)atoi(strUserStatus.c_str());
	CBankData::GetInstance()->m_CurUserInfo.emUserStatus = emStatus;

	// 已经登录
	if (emStatus != emNotLoad)
	{
		// 登陆按钮
		m_LoadButton.ShowWindow(false);
		// 分隔线
		m_SepButton.ShowWindow(false);
		// 注册按钮
		m_LoginButton.ShowWindow(false);
		//m_LogoButton.ShowWindow(false);
	
		RECT rcClient;
		GetClientRect(&rcClient);
		//w = rcClient.right;
		//h = rcClient.bottom;

		POINT pt;
		SIZE sz;

		// 同步按钮
		pt.x = rcClient.right - m_SynchroButton.GetFullWidth() - 15;
		// 提醒按钮
	//	pt.x = pt.x - m_AlarmButton.GetFullWidth() - 10;

		if (m_pTextButton != NULL)
		{
			wstring strText;
			m_pTextButton->ReadButtonText(strText);
			string strTpText = CW2A(strText.c_str());
			if (strTpText != strMail)
			{
				delete m_pTextButton;
				m_pTextButton = NULL;
			}
		}
		if (m_pTextButton == NULL)
		{
			m_pTextButton = new CTextButton(&m_tsm);
			m_pTextButton->CreateButton(m_hWnd, CA2W(strMail.c_str()), TEXT_BTN_USER_NAME);
		}
		// 文本按钮
		pt.x = pt.x - m_pTextButton->GetTextButtonWidth() - 20;
		pt.y = 49;
		sz.cx = m_pTextButton->GetTextButtonWidth();
		sz.cy = m_pTextButton->GetTextButtonHeight();
		m_pTextButton->SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);

		if (strMailVerif == MY_MAIL_VERIFY_ERROR)
		{
			// 红色叹号
			if (NULL == m_pUserInfoBtn)
			{
				m_pUserInfoBtn = new CSepButton(&m_tsm);
				m_pUserInfoBtn->CreateButton(m_hWnd, _T("UserInfo.png"), WM_USER_INFO_CLICKED);

			}
			// 定位红色叹号
			pt.x = pt.x - m_pUserInfoBtn->GetButtonWidth() - 4;
			pt.y = 48;
			sz.cx = m_pUserInfoBtn->GetButtonWidth();
			sz.cy = m_pUserInfoBtn->GetButtonHeight();
			m_pUserInfoBtn->SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);

			m_pUserInfoBtn->ShowWindow(true);
			// 让红色叹号提示框显示并停留10秒
			PostMessage(WM_USER_INFO_CLICKED, USER_LOAD_INFO_KEEP_TIME, SW_SHOW);
		}
		
		// 跳转至首页
		//m_wndMDIClient.ActiveCategoryByIndex(0);

		// 文本按钮
	//	m_LoginButton.ShowWindow(true);
		// 同步按钮
		//m_SynchroButton.ShowWindow(true);
		// 提醒按钮
		//m_AlarmButton.ShowWindow(true);
	}
	else
	{
		// 同步按钮
		//m_SynchroButton.ShowWindow(false);
		// 提醒按钮
		//m_AlarmButton.ShowWindow(false);

		// 文本按钮
		if (NULL != m_pTextButton)
		{
			delete m_pTextButton;
			m_pTextButton = NULL;
		}

		// 红色叹号
		if (NULL != m_pUserInfoBtn)
		{
			m_pUserInfoBtn->ShowWindow(false);
		}

		// 红色叹号提示框
		if (NULL != m_InfoDlg)
		{
			m_InfoDlg->ShowWindow(FALSE);
			m_InfoDlg->InitSendMailTime();
		}

		// 登陆按钮
		m_LoadButton.ShowWindow(true);
		// 分隔线
		m_SepButton.ShowWindow(true);
		// 注册按钮
		m_LoginButton.ShowWindow(true);
		//m_LogoButton.ShowWindow(true);
	}

	// 调用JS去更新首页的显示内容 
	::PostMessage(FS()->TabItem()->GetChildFrame()->GetItem()->GetAxControl(), WM_CHANGE_FIRST_PAGE_SHOW, 0, emStatus);

	// 更新当前HTML页面
	FS()->TabItem()->GetChildFrame()->DoNavigateRefresh();

	// 检验是需要同步
	if (emStatus == emUserNet && CBankData::GetInstance() ->IsCurrentDbNeedSynchro())
	{
		// 曹哥要求，BUG2823
		//CSynchroDlg dlg(3);
		//dlg.DoModal();
	}
	return 0;
}

void CMainFrame::RecalcClientSize(int w, int h)
{
	int iTop = 0;
	if (w == -1)
	{
		RECT rcClient;
		GetClientRect(&rcClient);
		w = rcClient.right;
		h = rcClient.bottom;
	}

	POINT pt;
	SIZE sz;

	bool bIsWindowMax = (GetStyle() & WS_MAXIMIZE) != 0;

	RECT rcSysBtn;
	m_SysBtnBar.GetClientRect(&rcSysBtn);
	m_SysBtnBar.SetWindowPos(NULL, w - rcSysBtn.right - 2, 1, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_SysBtnBar.Invalidate();
	
	pt.x = w - m_SysBtnBar.GetFullWidth() - m_MenuButton.GetFullWidth() - 1;
	pt.y = 1;
	m_MenuButton.SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_MenuButton.Invalidate();

	m_TitleBar.SetWindowPos(NULL, 0, 0, pt.x, 30/*rcSysBtn.bottom*/, SWP_NOZORDER);

	// 图标按钮
	//pt.x = w - 105;
	//pt.y = 34;// m_BigButton.GetHeight() + 6;
	//sz.cx = m_LogoButton.GetFullWidth();
	//sz.cy = m_LogoButton.GetHeight();
	//m_LogoButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);

	// 同步按钮
	pt.x = w - m_SynchroButton.GetFullWidth() - 15;
	pt.y = 40;
	sz.cx = m_SynchroButton.GetFullWidth();
	sz.cy = m_SynchroButton.GetHeight();
	m_SynchroButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);

#define LOAD_BUTTON_HEIGHT 49
	// 添加登陆按钮
	pt.x = pt.x - m_LoadButton.GetFullWidth()- 15;
	pt.y = LOAD_BUTTON_HEIGHT;
	sz.cx = m_LoadButton.GetFullWidth();
	sz.cy = m_LoadButton.GetHeight();
	m_LoadButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);
	m_LoadButton.Invalidate();

	// 添加分隔图标
	pt.x = pt.x - m_SepButton.GetFullWidth() - 10;
	pt.y = LOAD_BUTTON_HEIGHT;
	sz.cx = m_SepButton.GetFullWidth();
	sz.cy = m_SepButton.GetHeight();
	m_SepButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);

	// 添加注册按钮
	pt.x = pt.x - m_LoginButton.GetFullWidth() - 10;
	pt.y = LOAD_BUTTON_HEIGHT;
	sz.cx = m_LoginButton.GetFullWidth();
	sz.cy = m_LoginButton.GetHeight();
	m_LoginButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);
	m_LoginButton.Invalidate();


	// 同步按钮
	pt.x = w - m_SynchroButton.GetFullWidth() - 15;
	// 提醒按钮
	/*pt.x = pt.x - m_AlarmButton.GetFullWidth() - 10;
	pt.y = 40;
	sz.cx = m_AlarmButton.GetFullWidth();
	sz.cy = m_AlarmButton.GetHeight();
	m_AlarmButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);*/

	if (NULL != m_pTextButton)
	{
		// 文本按钮
		pt.x = pt.x - m_pTextButton->GetTextButtonWidth() - 20;
		pt.y = 49;
		sz.cx = m_pTextButton->GetTextButtonWidth();
		sz.cy = m_pTextButton->GetTextButtonHeight();
		m_pTextButton->SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);
		m_pTextButton->Invalidate();
	}

	if (NULL != m_pUserInfoBtn)
	{
		// 红色叹号
		pt.x = pt.x - m_pUserInfoBtn->GetButtonWidth() - 4;
		pt.y = 48;
		sz.cx = m_pUserInfoBtn->GetButtonWidth();
		sz.cy = m_pUserInfoBtn->GetButtonHeight();
		m_pUserInfoBtn->SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);
		CRect rect(0, 0, sz.cx, sz.cy);
		::InvalidateRect(m_pUserInfoBtn->m_hWnd, &rect, false);
		//Invalidate();
	}

	if (NULL != m_InfoDlg)
	{
		if (m_InfoDlg->IsWindowShow())
		{
			// 移动红色叹号提示框
			TPMPARAMS tps;
			tps.cbSize = sizeof(TPMPARAMS);
			m_pUserInfoBtn->GetWindowRect(&tps.rcExclude);

			CPoint pt;
			pt.x = tps.rcExclude.left - 32;
			pt.y = tps.rcExclude.bottom;

			m_InfoDlg->ShowMenuWindow(pt);
		}
	}



	m_Toolbar.SetWindowPos(NULL, 0, 30/*rcSysBtn.bottom*/, w - s()->LogoButton()->GetWidth(), m_Toolbar.GetHeight(), SWP_NOZORDER);

	// (B-1)
	// Back Button
	pt.x = 9; //s()->MainFrame()->GetBorderWidth() + 2;
	pt.y = 30 + 3;// m_BigButton.GetHeight() + 3;
	sz.cx = m_BackButton.GetFullWidth();
	sz.cy = m_BackButton.GetHeight();
	m_BackButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);

	// (B-2)
	// Forward Button
	pt.x = 40;
	pt.y = 30 + 8;//m_BigButton.GetHeight() + 8;
	sz.cx = m_ForwardButton.GetFullWidth();
	sz.cy = m_ForwardButton.GetHeight();
	m_ForwardButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);

	// (B-3)
	// Reload Button
	pt.x = 40 + 6 + m_ForwardButton.GetFullWidth();
	pt.y = 30 + 8;//m_BigButton.GetHeight() + 8;
	sz.cx = m_RefreshButton.GetFullWidth();
	sz.cy = m_RefreshButton.GetHeight();
	m_RefreshButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);

	// (B-4)
	// setting button
	/*pt.x = 40 + 12 + m_ForwardButton.GetFullWidth() + m_RefreshButton.GetFullWidth();
	pt.y = 30 + 8;
	sz.cx = m_SettingButton.GetFullWidth ();
	sz.cy = m_SettingButton.GetHeight ();
	m_SettingButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);*/

	// (B-5)
	// Help Button
	pt.x = 40 + 12 + m_ForwardButton.GetFullWidth() + m_RefreshButton.GetFullWidth();//40 + 18 + m_ForwardButton.GetFullWidth() + m_RefreshButton.GetFullWidth() + m_SettingButton.GetFullWidth ();
	pt.y = 30 + 8;//m_BigButton.GetHeight() + 8;
	sz.cx = m_HelpButton.GetFullWidth();
	sz.cy = m_HelpButton.GetHeight();
	m_HelpButton.SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOZORDER);

	

	// 剩余高度
	int iTopHeight = 79;	
	int iLessHeight = h - iTopHeight - s()->StatusBar()->GetStatusBarHeight(false);
		
	if (bIsWindowMax)
		m_CatetoryCtrl.SetWindowPos(NULL, 0, iTopHeight, w, s()->Category()->GetCategoryHeight(), SWP_NOZORDER);
	else
	{
		if (iLessHeight > s()->Category()->GetCategoryHeight())
		{
			RECT rect = { s()->MainFrame()->GetBorderWidth(), iTopHeight, w - s()->MainFrame()->GetBorderWidth(), iTopHeight + s()->Category()->GetCategoryHeight() };
			m_CatetoryCtrl.SetWindowPos(NULL,s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), s()->Category()->GetCategoryHeight(), SWP_NOZORDER);
		}
		else
			m_CatetoryCtrl.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), iLessHeight, SWP_NOZORDER);
	}

	iTopHeight += s()->Category()->GetCategoryHeight();
	iLessHeight = h - iTopHeight - s()->StatusBar()->GetStatusBarHeight(false);

	// 控制显示信息栏是否显示，如果状态为0，那么根本不显示该信息栏
	if(FS()->TabItem() != NULL && FS()->TabItem()->m_pCategory->GetShowInfoState() != 0)//控制不显示信息栏
	{
		// 设置该显示的机构名称
		m_ShowInfo.SetFinanceName(FS()->TabItem()->m_pCategory->GetShowInfo());
		// 获取显示状态，是显示文字还是直接显示整个为灰色
		if(FS()->TabItem()->m_pCategory->GetShowInfoState() == 1)
			m_ShowInfo.SetState(eShowInfo);
		else if(FS()->TabItem()->m_pCategory->GetShowInfoState() == 2)
			m_ShowInfo.SetState(eHideOthers);
		if(m_ShowInfo.GetShowState() == eShowInfo)
		{
			// 当显示文字时， 设置宽度大小，根据当前大小设置不同长度
			if (bIsWindowMax)
				m_ShowInfo.SetWindowPos(NULL, 0, iTopHeight, w, SHOW_INFO_LENGTH, SWP_NOZORDER | SWP_SHOWWINDOW);
			else
			{
				if (iLessHeight > SHOW_INFO_LENGTH)
				{
					RECT rect = { s()->MainFrame()->GetBorderWidth(), iTopHeight, w - s()->MainFrame()->GetBorderWidth(), iTopHeight + SHOW_INFO_LENGTH };
					m_ShowInfo.SetWindowPos(NULL, &rect, SWP_NOZORDER | SWP_SHOWWINDOW);
				}
				else
					m_ShowInfo.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), iLessHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

			}
			iTopHeight += SHOW_INFO_LENGTH;
			iLessHeight = h - iTopHeight - s()->StatusBar()->GetStatusBarHeight(false);
		}
		// 显示最大灰色时，设置剩余显示界面部分为1,间接隐藏该部分窗口
		else
		{
			m_ShowInfo.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), h - s()->StatusBar()->GetStatusBarHeight(false) - iTopHeight - 1, SWP_NOZORDER | SWP_SHOWWINDOW);
			iTopHeight = h - s()->StatusBar()->GetStatusBarHeight(false) - 1;
			iLessHeight = 1;
		}
	}
	// 只有导入账单时才显示信息栏，否则进行隐藏
	else
		m_ShowInfo.ShowWindow(SW_HIDE);
		
	if (FS()->TabItem() == NULL || FS()->TabItem()->m_pCategory->GetWebData()->IsNoSubTab() == true)
		m_TabCtrl.ShowWindow(SW_HIDE);
	else
	{
		if (bIsWindowMax)
			m_TabCtrl.SetWindowPos(NULL, 0, iTopHeight, w, s()->Tab()->GetTabHeight(), SWP_NOZORDER | SWP_SHOWWINDOW);
		else
		{
			if (iLessHeight > s()->Tab()->GetTabHeight())
				m_TabCtrl.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), s()->Tab()->GetTabHeight(), SWP_NOZORDER | SWP_SHOWWINDOW);
			else
				m_TabCtrl.SetWindowPos(NULL, s()->MainFrame()->GetBorderWidth(), iTopHeight, w - 2 * s()->MainFrame()->GetBorderWidth(), iLessHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
		}

		iTopHeight += s()->Tab()->GetTabHeight() + 1;
	}
	// 显示ie childframe部分的处理
	int xMdi = bIsWindowMax ? 0 : s()->MainFrame()->GetBorderWidth();
	int yMdi = iTopHeight;
	int cxMdi = w - (bIsWindowMax ? 0 : s()->MainFrame()->GetBorderWidth() * 2);
	int cyMdi = h - iTopHeight - s()->StatusBar()->GetStatusBarHeight(bIsWindowMax);
	m_wndMDIClient.SetWindowPos(NULL, xMdi, yMdi, cxMdi, cyMdi, SWP_NOZORDER | SWP_NOSENDCHANGING);
	m_wndMDIClient.PostMessage(WM_SIZE, 0, MAKELPARAM(cxMdi, cyMdi));
	
	// 显示状态栏
	m_StatusBar.SetWindowPos(NULL, 0, h - s()->StatusBar()->GetStatusBarHeight(bIsWindowMax), w, s()->StatusBar()->GetStatusBarHeight(false), SWP_NOZORDER);

	CClientDC dc(m_hWnd);
	OnEraseBkgnd(dc);
}

void CMainFrame::OnFinalMessage(HWND hWnd)
{
	delete this;
}

void CMainFrame::UpdateTitle()
{
	std::wstring str = _T("财金汇");
	if (FS()->TabItem())
	{
		str += _T(" - ");
		if (FS()->TabItem()->m_pCategory->GetWebData())
			str += std::wstring(FS()->TabItem()->m_pCategory->GetWebData()->GetName()) + _T(" - ") + FS()->TabItem()->GetText();
	}

	SetWindowText(str.c_str());
	m_TitleBar.Invalidate();
}

void CMainFrame::UpdateSSLState()
{
	Invalidate();
}

//////////////////////////////////////////////////////////////////////////
// message handler

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	::SetActiveWindow(m_hWnd);
	//SetWindowPos(HWND_TOP, 0, 0, 0, 0,SWP_NOSIZE);
	SetClassLongPtr(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE) | CS_DROPSHADOW);
	ModifyStyle(WS_CAPTION, 0);

	// icons
	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(NULL);
	HICON hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	//
	m_TextFont.CreateFont(13, 0, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, _T("Tahoma"));

	// use data
	SetWindowLongPtr(GWLP_USERDATA, (LONG_PTR)&m_tsm);
	FS()->hMainFrame = m_hWnd;
	s()->CreateTheme(m_hWnd, false);

	RECT rc = { lpCreateStruct->x, lpCreateStruct->y, lpCreateStruct->x + lpCreateStruct->cx, lpCreateStruct->y + lpCreateStruct->cy };
	SetRestoreRectAndMaxStatus(rc, (lpCreateStruct->style & WS_MAXIMIZE) != 0);

	m_MenuButton.CreateButton(m_hWnd);

	m_Toolbar.CreateBrowserToolbar(m_hWnd);
	m_SysBtnBar.CreateSystemButtonBar(m_hWnd);
	m_TitleBar.CreateTitleBar(m_hWnd);
	//m_LogoButton.CreateLogoButton(m_hWnd);

	m_BackButton.CreateButton(m_hWnd);
	m_ForwardButton.CreateButton(m_hWnd);
	m_RefreshButton.CreateButton(m_hWnd);
	m_HelpButton.CreateButton(m_hWnd);

	m_LoadButton.CreateButton(m_hWnd);
	m_LoginButton.CreateButton(m_hWnd);
	m_SepButton.CreateButton(m_hWnd, _T("Sep_button.png"));

	m_SynchroButton.CreateButton(m_hWnd);
	//m_AlarmButton.CreateButton(m_hWnd);
	//m_SynchroButton.ShowWindow(false);
	//m_AlarmButton.ShowWindow(false);
//	m_SettingButton.CreateButton(m_hWnd);


	m_TabCtrl.Create(m_hWnd, NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE);
	m_CatetoryCtrl.Create(m_hWnd, NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE);
	m_ShowInfo.CreateShowInfoBar(m_hWnd);
	m_wndMDIClient.CreateTabMDIClient(m_hWnd);
	m_StatusBar.CreateBrowserStatusBar(m_hWnd);

	GetClientRect(&rc);
	SetRgnForWindow(lpCreateStruct->cx, lpCreateStruct->cy);
	//RecalcClientSize(rc.right, rc.bottom);

/*

	m_Rebar.CreateBrowserReBarCtrl(m_hWnd, lpCreateStruct->cx);
	m_CmdBar.CreateCommandBarCtrl(m_Rebar);
	m_AddressBar.CreateAddressBar(m_Rebar);
	m_SearchBar.CreateSearchBar(m_Rebar);
	m_FavorBar.CreateFavoriteBar(m_Rebar);
	m_RecentBar.CreateRecentBar(m_Rebar);

	m_Rebar.InitBand(&m_TitleBar, &m_SysBtnBar, &m_CmdBar, &m_Toolbar, &m_AddressBar, &m_SearchBar, &m_FavorBar, &m_RecentBar);

	m_FindBar.CreateFindTextBar(m_hWnd);

	// 如果是tab在顶上，则tab需要放在rebar里面（代替titlebar）
*/

	CCheckForUpdate::ClearInstallFlag();

	//InitCustomDrawMenu();
//	CCoolMenuHook::InstallHook(m_hWnd);
	//InitCoolMenuSettings();

	m_RefreshButton.EnableWindow(FALSE);
	m_ForwardButton.EnableWindow(FALSE);
	m_BackButton.EnableWindow(FALSE);
	return 0;
}


void CMainFrame::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
	lpMMI->ptMinTrackSize.x = 540;
	lpMMI->ptMinTrackSize.y = 110;
}

BOOL CMainFrame::OnEraseBkgnd(CDCHandle dc)
{
	bool bIsSecure = false;
	if (m_CatetoryCtrl.GetCurrentSelection() && m_CatetoryCtrl.GetCurrentSelection()->ShowSecureColor())
		bIsSecure = true;

	RECT rect;
	GetClientRect(&rect);
	rect.bottom = 139 + SHOW_INFO_LENGTH; //s()->Toolbar()->GetBigButtonHeight() + s()->Toolbar()->GetToolbarHeight() + s()->Tab()->GetTabHeight();
	s()->Toolbar()->DrawRebarBack(dc, rect, (GetStyle() & WS_MAXIMIZE) != 0, bIsSecure);
	FS()->pCate->Invalidate();

	// 画边框
	if ((GetStyle() & (WS_MAXIMIZE | WS_MINIMIZE)) == 0) 
	{
		GetClientRect(&rect);
		//rect.top += s()->Toolbar()->GetBigButtonHeight() + s()->Toolbar()->GetToolbarHeight() + s()->Tab()->GetTabHeight();
		rect.top += 108;
		s()->MainFrame()->DrawBorder(dc, rect, false, bIsSecure);
	}

	RECT rcSysBtn;
	m_SysBtnBar.GetClientRect(&rcSysBtn);
	GetClientRect(&rect);
	dc.SetPixel(rect.right - 2, rect.top + rcSysBtn.bottom - 1 , RGB(24, 82, 123));


	if(m_IsShowCloudMessage)
	{
		//这个位置要输出警告的字体
		HFONT oldFont = dc.SelectFont(m_TextFont);
		dc.SetTextColor(RGB(16, 93, 145));

		SetBkMode(dc,TRANSPARENT); 

		//获取显示文字的位置
		POINT pt;
		pt.x = 40 + 18 + m_ForwardButton.GetFullWidth() + m_RefreshButton.GetFullWidth() + m_HelpButton.GetFullWidth()+ 45;// + m_SettingButton.GetFullWidth ()
		pt.y = 30 + 8 + 8;
		LPTSTR mes = _T("无法连接云安全服务器，可能存在安全风险");
		dc.TextOut(pt.x,pt.y,mes,_tcslen(mes));	
		dc.SelectFont(oldFont);
	}
/*
	m_wndMDIClient.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.left -= s()->MainFrame()->GetBorderWidth();
	rect.right += s()->MainFrame()->GetBorderWidth();
	rect.top -= s()->MainFrame()->GetBorderHeight();
	rect.bottom += s()->MainFrame()->GetBorderHeight();
	dc.FillSolidRect(&rect, FS()->TabItem() == NULL || FS()->TabItem()->GetSecure() == false ? 0xcc0000 : 0x00cc00);

*/
	return TRUE;
}


void CMainFrame::OnSize(UINT nType, CSize size)
{
	RecalcClientSize(size.cx, size.cy);
}


LRESULT CMainFrame::OnClose(UINT /* uMsg */, WPARAM wParam, LPARAM lParam)
{

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"用户关闭");

	WINDOWPLACEMENT winp = { sizeof(WINDOWPLACEMENT) };
	::GetWindowPlacement(m_hWnd, &winp);

	DWORD dwPos = MAKELONG(winp.rcNormalPosition.left, winp.rcNormalPosition.top);
	DWORD dwSize = MAKELONG(winp.rcNormalPosition.right - winp.rcNormalPosition.left, winp.rcNormalPosition.bottom - winp.rcNormalPosition.top);
	DWORD dwMax = (GetStyle() & WS_MAXIMIZE) ? 1 : 0;
	::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndpos"), REG_DWORD, &dwPos, sizeof(DWORD));
	::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndsize"), REG_DWORD, &dwSize, sizeof(DWORD));
	::SHSetValue(HKEY_CURRENT_USER, _T("Software\\Bank"), _T("wndmax"), REG_DWORD, &dwMax, sizeof(DWORD));

	// 退出时删除Software\\Bank\\VirtualReg
	::SHDeleteKeyW(HKEY_CURRENT_USER, _T("Software\\Bank\\VirtualReg"));

	::SendMessageTimeoutW(m_IeWnd, WM_MAINFRAME_CLOSE,  0,  0, SMTO_NORMAL, 2000, 0);
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"通知内核退出");
	DestroyWindow();
	::PostQuitMessage(0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CMainFrame::OnNotifyAxUICreated(UINT /* uMsg */, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	SwitchToThisWindow(m_hWnd, TRUE);
	m_IeWnd = (HWND)lParam;
	CProcessManager::_()->OnAxUICreated((HWND)lParam);
	AccessDefaultPage(m_hWnd); // 创建默认的页面并显示用户指定的标签页

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"内核创建了管理页");

	m_RefreshButton.EnableWindow(TRUE);
	m_ForwardButton.EnableWindow(TRUE);
	m_BackButton.EnableWindow(TRUE);

	// 检验是否存在自动登陆用户
	UserAutoLoad();

#ifndef SINGLE_PROCESS
	SetTimer(TIMER_CHECKKERNEL, 5000, NULL);
#endif

	return 0;
}

LRESULT CMainFrame::OnMyMenuClicked(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & bHandled )
{
	int nVal = wParam;
	switch(nVal)
	{
	case 1:
		//OnHelpSetting(0, 0, 0, bHandled);
		PostMessage(WM_SHOW_USER_DLG, 0, MY_TAG_SETTING_DLG);
		break;
	case 2:
		OnHelpTips(0, 0, 0, bHandled);
		break;
	case 3:
		OnHelpFeedback(0, 0, 0, bHandled);
		break;
	case 4:
		OnHelpAbout(0, 0, 0, bHandled);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CMainFrame::OnShowUserMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/) // 弹出用户登录后的菜单
{
	TPMPARAMS tps;
	tps.cbSize = sizeof(TPMPARAMS);
	m_pTextButton->GetWindowRect(&tps.rcExclude);

	CPoint pt1;
	pt1.x = tps.rcExclude.right - 48;
	pt1.y = tps.rcExclude.bottom;

	if (NULL == m_UserMenuDlg)
	{
		m_UserMenuDlg = new CMenuDlg(m_hWnd, _T("Menu2.png"), WM_USER_INFO_MENU_CLICKED, 3, 33, 7); // 3表示按钮数，33按钮选中时的高度，7表示按钮尖尖部分的高度
		m_UserMenuDlg->Create(NULL,IDD_DLG_MENU);	
	}

	m_UserMenuDlg->ShowMenuWindow(pt1);

	return 0;
}

LRESULT CMainFrame::OnUserMenuClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/) // 用户登录后的菜单的响应事件
{
	int nVal = wParam;
	switch(nVal)
	{
	case MY_USER_INFO_MENU_CLICK_MAILCHANGE: // 更改邮箱
		{
			// 调用JS去更新首页的显示内容 
			::SendMessage(FS()->TabItem()->GetChildFrame()->GetItem()->GetAxControl(), WM_AX_CHANGE_SETTINT_STATUS, 0, emUserChangeMail);
			PostMessage(WM_SHOW_USER_DLG, 0, MY_TAG_SETTING_DLG);
			break;
		}
	case MY_USER_INFO_MENU_CLICK_PWDCHANGE: // 更改密码
		{
			// 调用JS去更新首页的显示内容 
			::SendMessage(FS()->TabItem()->GetChildFrame()->GetItem()->GetAxControl(), WM_AX_CHANGE_SETTINT_STATUS, 0, emUserChangePwd);
			PostMessage(WM_SHOW_USER_DLG, 0, MY_TAG_SETTING_DLG);
			break;
		}
	case MY_USER_INFO_MENU_CLICK_QUIT: // 退出
		{
			CBankData::GetInstance()->CloseDB();
			// 跳转至首页
			//m_wndMDIClient.ActiveCategoryByIndex(0);
			::PostMessage(m_IeWnd, WM_AX_LOAD_USER_QUIT, 0, 0);

			// 刷新当前页
			FS()->TabItem()->GetChildFrame()->DoNavigateRefresh();
			break;
		}
	default:
		break;
	}
	return 0;
}

LRESULT CMainFrame::OnShowHelpMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{	
	TPMPARAMS tps;
	tps.cbSize = sizeof(TPMPARAMS);
	m_MenuButton.GetWindowRect(&tps.rcExclude);

	CPoint pt;
	pt.x = tps.rcExclude.left;
	pt.y = tps.rcExclude.bottom;

	if (NULL == m_MenuDlg)
	{
		m_MenuDlg = new CMenuDlg(m_hWnd, _T("NoSel.png"), WM_MY_MENU_CLICKED, 4, 25, 14);
		m_MenuDlg->Create(NULL,IDD_DLG_MENU);	
	}

	m_MenuDlg->ShowMenuWindow(pt);
	return 0;
}

LRESULT CMainFrame::OnHelpSetting(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	/*CSettingDlg dlg;
	dlg.DoModal();*/

	return 0;
}

LRESULT CMainFrame::OnHelpTips(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTipsDlg tip(1);
	tip.DoModal();

	return 0;
}

LRESULT CMainFrame::OnHelpAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();

	return 0;
}

LRESULT CMainFrame::OnHelpUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	KillTimer(TIMER_AUTOUPDATE);

	CCheckForUpdate* pCheck = CCheckForUpdate::CreateInstance(m_hWnd);
	//pCheck->Check(false);
	pCheck->Release();

	return 0;
}

LRESULT CMainFrame::OnBroadcastQuit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	static bool bIsQuit = false;
	if (!bIsQuit)
	{
		bIsQuit = true;
		PostMessage(WM_CLOSE);
	}
	
	return 0;
}

LRESULT CMainFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
{
	if(TIMER_BCHECKKERNEL == wParam)
	{
		KillTimer(TIMER_BCHECKKERNEL);
	}
	else if(TIMER_CHECKKERNEL == wParam)
	{
		LPCTSTR lpszProcessMutex = _T("_Moneyhub_{878B413D-D8FF-49e7-808D-9A9E6DDCF2B7}");
		HANDLE _hIEcoreProcessMutex  = CreateMutex(NULL, TRUE, lpszProcessMutex);

		DWORD err = GetLastError();
		if(_hIEcoreProcessMutex != NULL)
			::CloseHandle(_hIEcoreProcessMutex);

		// 当内核对象已经存在或者禁止进入时，说明已经开启该进程
		if (err == ERROR_ALREADY_EXISTS || err == ERROR_ACCESS_DENIED)
		{
			return 0;
		}
		else
		{
			KillTimer(TIMER_CHECKKERNEL);//关闭定时器
			mhMessageBox(NULL, L"软件运行错误，为保护您的帐户安全，财金汇将重启", L"财金汇检测",MB_OK);

			CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_COMMON_ERROR, L"检测到内核无响应");

				// 重新启动财金汇
			WCHAR szPath[MAX_PATH] ={0};
			::GetModuleFileName(NULL, szPath, _countof(szPath));
			::PathRemoveFileSpecW(szPath);

			std::wstring path(szPath);
			path += L"\\Moneyhub.exe -wait";

			STARTUPINFO si;	
			PROCESS_INFORMATION pi;	
			ZeroMemory( &pi, sizeof(pi) );	
			ZeroMemory( &si, sizeof(si) );	
			si.cb = sizeof(si);	
			//带参数打开	
			if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{		
				CloseHandle( pi.hProcess );		
				CloseHandle( pi.hThread );		
			}

			CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"重启财金汇");

			exit(0);
		}

		return 0;
	}
	else
		bHandled = FALSE;

	return 0;
}

LRESULT CMainFrame::OnUpdateRetry(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	//SetTimer(TIMER_AUTOUPDATE, 10 * 60 * 1000);

	return 0;
}

LRESULT CMainFrame::OnSwitchTopPage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	enum TopPageStruct tps = (TopPageStruct)wParam;
	SwitchTopPage(m_hWnd, tps);

	return 0;
}

LRESULT CMainFrame::OnSvrCheckUser(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	wstring mail = (wchar_t *)lParam;

	USES_CONVERSION;
	string smail = W2A(mail.c_str());
	if((CBankData::GetInstance()->m_CurUserInfo.struserid == MONHUB_GUEST_USERID && smail == "访客") || (CBankData::GetInstance()->m_CurUserInfo.strmail == smail))
	{
		enum TopPageStruct tps = kToolsPage;
		SwitchTopPage(m_hWnd, tps);
	}
	else if(CBankData::GetInstance()->m_CurUserInfo.struserid == MONHUB_GUEST_USERID)
		mhMessageBox(m_hWnd, L"请您登陆用户账号进行查看", L"财金汇", MB_OK);
	else
		mhMessageBox(m_hWnd, L"要查看的账号与当前登录账号不一致，您需要先退出当前登录账号", L"财金汇", MB_OK);

	return 0;
}

LRESULT CMainFrame::OnHelpFeedback(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DWORD dwValueNameLength = 100;        // 值名字符串长度   
	WCHAR ptszValueName[100] = L"";    // 值名字符串
	wstring ieversion;

	//获得ie版本信息
	DWORD dwType, dwReturnBytes = sizeof(DWORD), dwPos = 0, dwSize = 0, dwMax = 0;
	if( ERROR_SUCCESS == ::SHGetValueW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Microsoft\\Internet Explorer", L"Version", &dwType, &ptszValueName, &dwValueNameLength))
	{
		ieversion = ptszValueName;
	}

	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, CRecordProgram::GetInstance()->GetRecordInfo(L"IE版本:%s",ieversion.c_str()));
	char szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));
	USES_CONVERSION;
	sprintf_s(szBuf, sizeof(szBuf), "%s?Moneyhubuid=%s&V=%s&Sn=%s",W2CA(CHostContainer::GetInstance()->GetHostName(kFeedback).c_str()), GenHWID2().c_str(), ProductVersion_All, CSNManager::GetInstance()->GetSN().c_str());

	HWND hPop = FindWindowW(NULL, L"MoneyHub_Svr_Mainframe");
	if(hPop != NULL)
		::PostMessage(hPop, WM_MONEYHUB_FEEDBACK, 0, 0);
	//mhMessageBox(NULL, L"谢谢您的支持！",L"财金汇", MB_OK);
	::CreateNewPage_0(m_hWnd, A2CT(szBuf),TRUE);

	return 0;
}

LRESULT CMainFrame::OnShowCloudMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	m_IsShowCloudMessage = true;
	Invalidate();
	return 0;
}
LRESULT CMainFrame::OnClearCloudMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	m_IsShowCloudMessage = false;
	Invalidate();
	return 0;
}
LRESULT CMainFrame::OnShowCloudStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	FS()->Status()->SetStatusText(L"正在运行云安全扫描……", true);
	Invalidate();
	return 0;
}

LRESULT CMainFrame::OnNoShowCloudStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	FS()->Status()->SetStatusText(L"", true);
	Invalidate();
	return 0;
}

LRESULT CMainFrame::OnCloudDestory(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	mhMessageBox(NULL,L"财金汇发现安全威胁并已阻断，为保证安全，请重新启动财金汇",L"财金汇",MB_OK);
	
	CRecordProgram::GetInstance()->FeedbackError(MY_PRO_NAME, MY_COMMON_ERROR, L"检测到安全威胁");

	::SendMessageTimeoutW(m_IeWnd, WM_MAINFRAME_CLOSE,  0,  0, SMTO_NORMAL, 2000, 0);
	DestroyWindow();

	// 重新启动财金汇
	WCHAR szPath[MAX_PATH] ={0};
	::GetModuleFileName(NULL, szPath, _countof(szPath));
	::PathRemoveFileSpecW(szPath);

	std::wstring path(szPath);
	path += L"\\Moneyhub.exe -wait";

	STARTUPINFO si;	
	PROCESS_INFORMATION pi;	
	ZeroMemory( &pi, sizeof(pi) );	
	ZeroMemory( &si, sizeof(si) );	
	si.cb = sizeof(si);	
	//带参数打开	
	if(CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{		
		CloseHandle( pi.hProcess );		
		CloseHandle( pi.hThread );		
	}
	CRecordProgram::GetInstance()->RecordCommonInfo(MY_PRO_NAME, MY_COMMON_PROCESS, L"重启财金汇");

	::PostQuitMessage(0);
	return 0;
}

LRESULT CMainFrame::OnShowCheckMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &/*bHandled*/)
{
	::MessageBoxW(NULL, L"您的财金汇客户端已被破坏，请重新下载安装，建议您对电脑进行全面的病毒扫描", L"财金汇安全提示", MB_OK);
	return 0;
}
// 取消收藏
LRESULT CMainFrame::OnCancelAddFav(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	if(m_IeWnd)
		::PostMessageW(m_IeWnd, WM_CANCEL_ADDFAV,  wParam,  lParam);
	return 0;
}

LRESULT CMainFrame::OnAutoUserDlg(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	if (MY_STATUE_CLOSE_DLG == lParam) // 表示关闭
	{
		if (NULL != m_pShowDlg && NULL != m_pShowDlg->m_hWnd) // 结束对话框
		{
			::SendMessage(m_pShowDlg->m_hWnd, END_SHOW_DIALOG, 0, 0);
		}
	}
	else if(MY_STATUE_SHOW_DLG == lParam) // 表示打开
	{
		BOOL bTag = FALSE;
		if (NULL != m_pShowDlg)
		{
			m_pShowDlg->ShowWindow(true);
		}
		else
			OnShowUserDlg(0, 0, wParam, bTag);
	}
	else if(MY_STATUS_HIDE_DLG == lParam)
	{
		if (NULL != m_pShowDlg)
			m_pShowDlg->ShowWindow(false);
	}
	return 0;
}

LRESULT CMainFrame::OnShowUserDlg(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	wstring strText, strPath;
	int nErrCode = 0;
	if(wParam !=3 )//caohaitao
	{
		if (MY_TAG_LOAD_DLG == lParam)
		{
			// 如果正在登录，则不弹登录框
			if (IsLoading())
				return 0;

			strText = L"登录财金汇";
			strPath = L"login.html";
			nErrCode = wParam; // 暂时只有登录界面才有错误码
		}
		else if (MY_TAG_REGISTER_DLG == lParam)
		{
			strText = L"注册财金汇账户";
			strPath = L"register.html";
		}
		else if (MY_TAG_SETTING_DLG == lParam)
		{
			strText = L"设置";
			strPath = L"setOption.html";
		}
		else if (MY_TAG_REGISTER_GUIDE == lParam)
		{
			strText = L"注册向导";
			strPath = L"registerGuide.html";
		}
		else if(MY_TAG_INIT_PWD == lParam)
		{
			strText = L"重置密码";
			strPath = L"findPwd.html";
		}

		CShowJSFrameDlg dlg(strText.c_str(), strPath.c_str(), nErrCode);

		m_pShowDlg = &dlg;


		dlg.DoModal(m_hWnd);

		m_pShowDlg = NULL;
	}
	else//caohaitao
		// 通知内核进行登录
		::PostMessage(m_IeWnd, WM_AX_USER_AUTO_LOAD, 3, 0);
	return 0;
}

LRESULT CMainFrame::UserInfoBtnClicked(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	if (SW_SHOW == lParam)
	{
		TPMPARAMS tps;
		tps.cbSize = sizeof(TPMPARAMS);
		m_pUserInfoBtn->GetWindowRect(&tps.rcExclude);

		CPoint pt;
		pt.x = tps.rcExclude.left - 32;
		pt.y = tps.rcExclude.bottom;

		if (NULL == m_InfoDlg)
		{
			m_InfoDlg = new CUserLoadInfoDlg(m_hWnd, _T("UserLoadInfo.png"));
			m_InfoDlg->Create(m_hWnd,IDD_DLG_MENU);	
		}

		// 设置对话框保留的时间，此时点击其它的地方该对话框不隐藏
		if (0 != wParam)
			m_InfoDlg->KeepWindowForSeconds(wParam);


		m_InfoDlg->ShowMenuWindow(pt);
	}
	else if (SW_HIDE == lParam)
	{
		if (NULL != m_InfoDlg)
			m_InfoDlg->ShowWindow(FALSE);
	}

	return 0;
}

LRESULT CMainFrame::ChangeUserDlgName(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &/*bHandled*/)
{
	string strTp = (char*)lParam;
	if (NULL != m_pShowDlg)
	{
		m_pShowDlg->SetWindowText(CA2W(strTp.c_str()));
		/*CRect rcClient;
		::GetClientRect(m_pShowDlg->m_hWnd, &rcClient);
		CRect rect(0, 0, rcClient.right, 20);
		::InvalidateRect(m_pShowDlg->m_hWnd, &rcClient, false);*/
	}

	return 0;
}

BOOL CMainFrame::UserAutoLoad()
{
	string strUserID, strMail, strStoken;
	bool bBack = CBankData::GetInstance()->ReadNeedAutoLoadUser(strUserID, strMail, strStoken);

	bool bAutoLoading = false;
	if (strUserID.length() > 0 && bBack)
		bAutoLoading = true;

	if (bAutoLoading)
	{
		if (NULL == m_pLoadingButton)
		{
			// 隐藏按钮
			// 登陆按钮
			m_LoadButton.ShowWindow(false);
			// 分隔线
			m_SepButton.ShowWindow(false);
			// 注册按钮
			m_LoginButton.ShowWindow(false);

			m_pLoadingButton = new CTextButton(&m_tsm);
			m_pLoadingButton->CreateButton(m_hWnd, L"正在登录...", TEXT_BTN_USER_LOADINT);


			RECT rcClient;
			GetClientRect(&rcClient);
			POINT pt;
			SIZE sz;

			// 同步按钮
			pt.x = rcClient.right - m_SynchroButton.GetFullWidth() - 15;

			// 添加登陆按钮
			pt.x = pt.x - m_pLoadingButton->GetFullWidth()- 15;
			pt.y = 49;
			sz.cx = m_pLoadingButton->GetFullWidth();
			sz.cy = m_pLoadingButton->GetHeight();
			m_pLoadingButton->SetWindowPos(NULL, pt.x, pt.y, sz.cx, sz.cy, SWP_NOSIZE);

			m_pLoadingButton->ShowWindow(true);

			// 通知内核进行登录
			::PostMessage(m_IeWnd, WM_AX_USER_AUTO_LOAD, 0, 0);
		}
	}
	return true;
}

// 是否有用户正在登录
bool CMainFrame::IsLoading()
{
	// 可以通过正在登录的按钮来判断(当然也可以添加一个变量进行标记)
	if (NULL != m_pLoadingButton)
		return true;
	return false;
}

LRESULT CMainFrame::OnCloseCurUserDB(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &/*bHandled*/)
{
	CBankData::GetInstance()->CloseDB();
	return 0;
}

LRESULT CMainFrame::OnUpdateCurUserDB(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &/*bHandled*/) // UI更新当前用户库
{
	string strRead = (LPSTR)lParam;
	string strTag = MY_PARAM_END_TAG;

	
	// 读取strPath
	string strPath = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));

	// 读取PWD
	strRead = strRead.substr(strRead.find(MY_PARAM_END_TAG) + strTag.length(), strRead.length());
	string strPwd = strRead.substr(0, strRead.find(MY_PARAM_END_TAG));
	char chPwd[33] = {0};
	int nBack = 0;
	// 转换成实际的密码
	FormatDecVal(strPwd.c_str(), chPwd, nBack);

#ifndef OFFICIAL_VERSION
	CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, L"BankUI SetCurrentUserDB" );
	wstring strw = CA2W(strPwd.c_str());
	CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, strw);
#endif

	if (!CBankData::GetInstance()->SetCurrentUserDB((LPSTR)strPath.c_str(), (LPSTR)chPwd, 32))
	{
		CRecordProgram::GetInstance()->FeedbackError(MY_BANK_NAME, MY_ERROR_SQL_ERROR, L"BankUI SetCurrentUserDB error" );
	}

	return 0;
}

LRESULT CMainFrame::OnResendVerifyMail(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL &/*bHandled*/)
{
	LRESULT lr = ::SendMessage(m_IeWnd, WM_RESEND_VERIFY_MAIL, wParam, lParam);

	if (S_OK == lr)
	{
		CString strMes;
		wstring str = CA2W(CBankData::GetInstance()->m_CurUserInfo.strmail.c_str());
		strMes.Format(L"已向%s发送验证邮件，请及时查收并按邮件说明完成验证！", str.c_str()); 
		::mhMessageBox(m_hWnd, strMes, L"邮件重发", MB_OK | MB_ICONINFORMATION);
	}
	else if (S_FALSE == lr)
		::mhMessageBox(m_hWnd, L"验证邮件发送失败，请检查您的网络状况！", L"邮件重发", MB_OK | MB_ICONINFORMATION);
	else
	{
		//::mhMessageBox(m_hWnd, L"已经认证！", L"邮件重发", MB_OK | MB_ICONINFORMATION);
		if (NULL != m_pUserInfoBtn)
			m_pUserInfoBtn->ShowWindow(false);
	}

	return 0;
}

LRESULT CMainFrame::OnSynchroBtnChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &/*bHandled*/)
{
	m_SynchroButton.KeepOnSelect(true);
	return 0;
}

LRESULT CMainFrame::OnInitSynchroBtnStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &/*bHandled*/)
{
	::PostMessage(FS()->TabItem()->GetChildFrame()->GetItem()->GetAxControl(), WM_AX_INIT_SYNCHRO_BTN, 0, 0);
	return 0;
}

LRESULT CMainFrame::OnChangeCurUserStoken(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL &/*bHandled*/)
{
	string strParam = (char*)lParam;

	if (0 != strParam.length())
		CBankData::GetInstance()->m_CurUserInfo.strstoken = strParam;
	return 0;
}