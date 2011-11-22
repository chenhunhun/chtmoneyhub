#pragma once
#include "../Util/ThreadStorageManager.h"
#include "BaseClass/TuoToolbar.h"
#include "Menu/Menu.h"
#include "../resource/resource.h"
#include "FrameBorder/SizingBorder.h"

#include "../Skin/SkinManager.h"
#include "AltSkinClasses.h"
#include <gdiplus.h>
#include <GdiPlusImaging.h>
#include <string>
#include "CoolMessageBox.h"
#define WM_BILL_CHANGE_NOFIFY2			WM_USER + 0x100F
#define WM_BILL_HIDE_NOFIFY2			WM_USER + 0x100E
#define WM_BILL_SHOW_NOFIFY2			WM_USER + 0x100D


using namespace std;
using namespace Gdiplus;
enum ShowInfoState
{
	eShowInfo,
	eHide,
	eHideOthers
};

class CShowInfoBar : public CTuoToolBarCtrl<CShowInfoBar>, public CSizingBorder<CShowInfoBar>, public CFSMUtil
{
public:
	CShowInfoBar(FrameStorageStruct *pFS);
	~CShowInfoBar();

	void CreateShowInfoBar(HWND hParent);
	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void SetInfo(wstring info){ m_strText = info.c_str(); }
	void SetFinanceName(wstring name){ m_strText.Format(L"请登录%s网站，登录之后，将开始自动获取账单", name.c_str());}
	void SetState(ShowInfoState eState) { m_state = eState; }
	ShowInfoState GetShowState() { return m_state; }

private:
	CRect m_rcMsgBoxText;
	CString m_strText;
	CString m_strText2;

	DWORD m_crTextColor;
	DWORD m_crBackColor;

	CFont m_fontText;
	CFont m_fontText2;

	ShowInfoState m_state;

private:
	// message
	BEGIN_MSG_MAP_EX(CShowInfoBar)
		CHAIN_MSG_MAP(CSizingBorder<CShowInfoBar>)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CShowInfoBar>)		// 这个必须放在最底下，因为鼠标消息要被上面的截掉
		END_MSG_MAP()
		// message handler

	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
public:
	DECLARE_WND_CLASS_NODRAW(_T("MH_ShowInfobar"))
};

#define MH_STARTEVENT 0xf1
#define MH_GIF_REFRESH 0xf2
#define MH_DELAY_SHOW	0xf3//延迟显示
class CShowInfoDlg : public CDialogImpl<CShowInfoDlg>, public CDialogSkinMixer<CShowInfoDlg>
{
public:
	CShowInfoDlg(INotifyInterface* pSetInterface, LPCTSTR lpText = NULL)
	{
		m_hFrame = 0;
		m_type = 0;
		m_pSetInterface = pSetInterface;
		m_ShowTime = 0;
		m_rcMsgBoxSpinner.SetRect(20, 50, 20 + 45, 50 + 45);
		m_strCaption = L"财金汇账单管理";
		m_SpinnerGif = NULL;
		m_pGifItem = NULL;
		m_GifCount = 0;
		m_GifFrameCount = 0;
		m_GifGuid = FrameDimensionTime;
		if (NULL != lpText)
			m_strText = lpText;
	}
	~CShowInfoDlg()
	{
		if(m_pGifItem)
			free(m_pGifItem);
		if(m_SpinnerGif)
			delete m_SpinnerGif;
	}
public:
	enum { IDD = IDD_DIALOG_MSGBOX };

	BEGIN_MSG_MAP(CShowInfoDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CShowInfoDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_FINISH_GET_BILL, OnCancel)//正常关闭
		MESSAGE_HANDLER(WM_BILL_CHANGE_NOFIFY2, OnChangeMessage)
		COMMAND_ID_HANDLER(IDC_BUTTON_1, OnButtonClicked)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_BILL_HIDE_NOFIFY2, OnHide)
		MESSAGE_HANDLER(WM_BILL_SHOW_NOFIFY2, OnShow);
		MESSAGE_HANDLER(WM_CLOSE, OnCancel);
		//COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	LRESULT OnHide(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		KillTimer(MH_STARTEVENT);
		ShowWindow(SW_HIDE);
		return 0;
	}
	LRESULT OnShow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		SetWindowPos(HWND_TOP, 0, 0, 0, 0,SWP_NOSIZE);
		CenterWindow(GetDesktopWindow());
		ShowWindow(SW_SHOW);
		SetTimer(MH_STARTEVENT, 3 * 60 * 1000 ,NULL);//3min 还没有完成抓取账单的功能，那么关闭
		return 0;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if(wParam == MH_GIF_REFRESH)
		{
			m_ShowTime ++;
			if(m_ShowTime > 600)
			{
				if(m_pSetInterface)
				{
					SetTimer(MH_STARTEVENT, 3 * 60 * 1000 ,NULL);//3min 还没有完成抓取账单的功能，那么关闭
				}				
			}
			CDC dc(m_bmpSpinner.GetDC());
			Graphics gh(dc); //hDC是外部传入的画图DC
			gh.DrawImage(m_SpinnerGif, 0, 0,m_SpinnerGif->GetWidth(),m_SpinnerGif->GetHeight());
			//重新设置当前的活动数据帧
			m_SpinnerGif->SelectActiveFrame(&m_GifGuid, m_GifCount ++);
			if(m_GifCount == m_GifFrameCount) //frameCount是上面GetFrameCount返回值
				m_GifCount = 0;     //如果到了最后一帧数据又重新开始
			//计算此帧要延迟的时间
			//long lPause = ((long*)pItem->value)[m_GifCount]*10;
		}
		if(wParam == MH_STARTEVENT)//超时导入账单
		{			
			if(m_pSetInterface)
			{
				m_pSetInterface->GetBillExceedTime();	//设置超时			
			}
			KillTimer(MH_GIF_REFRESH);
			KillTimer(MH_STARTEVENT);
			if(m_type == 1)
			{
				if(m_hFrame != NULL)
					::EnableWindow(m_hFrame, true);
			}
			EndDialog(IDCANCEL);

		}
		if(wParam == MH_DELAY_SHOW)
		{			
			ShowWindow(SW_SHOW);
			KillTimer(MH_DELAY_SHOW);
		}
		return 0;
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_type == 1)
		{
			m_hFrame = FindWindow(_T("MONEYHUB_MAINFRAME"), NULL);
			if(m_hFrame != NULL)
			{
				::EnableWindow(m_hFrame, false);
				//::SetParent(m_hWnd, m_hFrame);//还是无响应，暂时不能这么改，需要改为非模式对话框才解决问题。
			}
		}
		::SetWindowLongW(m_hWnd, GWL_EXSTYLE, ::GetWindowLongW(m_hWnd, GWL_EXSTYLE)| WS_EX_TOOLWINDOW); 

		SetTimer(MH_GIF_REFRESH, 100 ,NULL);
		m_bmpSpinner.Create(m_hWnd, m_rcMsgBoxSpinner, NULL, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS, 0);

		::SetActiveWindow(m_hWnd);
		SetWindowPos(HWND_TOP, 0, 0, 0, 0,SWP_NOSIZE);		
		CenterWindow(GetDesktopWindow());
		SetWindowText(m_strCaption);
		m_btnSysClose.ShowWindow(SW_HIDE);
		if(m_pSetInterface != NULL)
			m_pSetInterface->SetNotifyWnd(m_hWnd);
		else
		{
			SetTimer(MH_DELAY_SHOW , 500, NULL);//延迟0.5s进行显示
			m_btnCancelButton.ShowWindow(SW_HIDE);//隐藏取消按钮
		}

		return 0;
	}

	LRESULT OnNcPaint(WORD /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		static int time = 2;

		if(m_pSetInterface == NULL && time > 1)
		{
			ShowWindow(SW_HIDE);
			time --;
		}
		return 0;
	}

	void PreCreate()
	{
		LPCTSTR lpszFontName[] = { _T("Tahoma"), _T("微软雅黑") };

		int iIndex = IsVista() ? 1 : 0;
		if (NULL == m_tFontText)
			m_tFontText.CreateFont(18, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
		m_tTextColor = RGB(16, 93, 145);

		if(m_strText.IsEmpty())
			m_strText = L"正在导入账单，请稍候......";
		m_btnCancelButton = GetDlgItem(IDC_BUTTON_1);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 1), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 2), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 3), SW_HIDE);
		m_btnCancelButton.ApplySkin(&m_imgDlgButton, 4);
		m_btnCancelButton.SetWindowText(_T("取消"));
		LoadMsgBoxIcons();
		CalcMsgBoxSize();
		CalcButtonsPos();
	}

	void CalcMsgBoxSize()
	{
		CDC dc(GetDC());
		dc.SelectFont(m_tFontText);

		m_rcMsgBoxText.SetRect(0, 0, 0, 0);
		dc.DrawText(m_strText, -1, &m_rcMsgBoxText, DT_CALCRECT);

		int nWidth = m_rcMsgBoxText.Width() + 220;
		int nHeight = m_rcMsgBoxText.Height() + 120;

		CRect rcButton;
		m_btnCancelButton.GetWindowRect(&rcButton);
		int nMinWidth = (rcButton.Width() + 10) + 10;
		if (nWidth < nMinWidth)
			nWidth = nMinWidth;

		SetWindowPos(NULL, 0, 0, nWidth, nHeight, 0);
	}

	void CalcButtonsPos()
	{
		CRect rcButton;
		m_btnCancelButton.GetWindowRect(&rcButton);
		int nButtonsWidth = (rcButton.Width() + 10) - 10;

		CRect rc;
		GetClientRect(&rc);

		int x = (rc.Width() - nButtonsWidth) / 2;
		int y = rc.bottom - rcButton.Height() - 12;

		m_btnCancelButton.SetWindowPos(NULL, x, y, rcButton.Width(), rcButton.Height(), 0);

	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		CRect rect;
		GetClientRect(&rect);

		// 文字
		CRect rcDraw(m_rcMsgBoxSpinner.right + 10, m_rcMsgBoxSpinner.top, 
			m_rcMsgBoxSpinner.right + m_rcMsgBoxText.Width() + 100,
			m_rcMsgBoxSpinner.top + m_rcMsgBoxText.Height());

		rcDraw.OffsetRect(4, 0);
		if (rcDraw.Height() < m_rcMsgBoxSpinner.Height())
			rcDraw.OffsetRect(0, (m_rcMsgBoxSpinner.Height() - rcDraw.Height()) / 2);

		dc.SetBkMode(TRANSPARENT);
		dc.SelectFont(m_tFontText);
		dc.SetTextColor(m_tTextColor);
		::DrawText(dc, m_strText, -1, &rcDraw, DT_VCENTER);


		return 0;
	}

	LRESULT OnButtonClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		KillTimer(MH_GIF_REFRESH);
		if(m_pSetInterface != NULL)
		{
			//有问题，先去掉
			//int ret = mhMessageBox(GetRootWindow(m_hWnd), L"将取消账单导入，请确认？", L"财金汇", MB_OKCANCEL);
			//if(ret == IDCANCEL)
			//	return 0;

			m_pSetInterface->CancelGetBill();
		}
		if(m_type == 1)
		{
			if(m_hFrame != NULL)
				::EnableWindow(m_hFrame, true);
		}
		EndDialog(IDCANCEL);
		return 0;
	}

	LRESULT OnChangeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		LPCWSTR info = (LPCWSTR)lParam;

		if(info == NULL)
			return 0;
		m_strText = info;
		Invalidate();

		return 0;
	}
	LRESULT OnCancel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if(m_type == 1)
		{
			if(m_hFrame != NULL)
			{
				::EnableWindow(m_hFrame, true);
				::SetActiveWindow(m_hFrame);
			}
		}
		KillTimer(MH_GIF_REFRESH);
		EndDialog(IDCANCEL);
		return 0;
	}


public:

	bool LoadMsgBoxIcons()
	{
		bool bRet = true;

		TCHAR szPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szPath, _countof(szPath));
		::PathRemoveFileSpecW(szPath);

		CString strSkinDir(szPath);
		strSkinDir += _T("\\Skin\\");	
		m_SpinnerGif = new Gdiplus::Image(strSkinDir + _T("spinner.gif"));

		UINT count = 0;
		count = m_SpinnerGif->GetFrameDimensionsCount();
		GUID *pDimensionIDs = (GUID*)new GUID[count];
		m_SpinnerGif->GetFrameDimensionsList(pDimensionIDs, count);
		WCHAR strGuid[39];
		StringFromGUID2(pDimensionIDs[0], strGuid, 39);
		m_GifFrameCount = (int)m_SpinnerGif->GetFrameCount(&pDimensionIDs[0]);

		delete []pDimensionIDs;

		int size = m_SpinnerGif->GetPropertyItemSize(PropertyTagFrameDelay);;
		m_pGifItem = (PropertyItem*)malloc(size);
		m_SpinnerGif->GetPropertyItem(PropertyTagFrameDelay, size, m_pGifItem);

		return bRet;
	}
	bool IsVista()
	{
		OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
		BOOL bRet = ::GetVersionEx(&ovi);
		return ((bRet != FALSE) && (ovi.dwMajorVersion >= 6));
	}
	void SetType(int type)
	{
		m_type = type;
	}

protected:
	int m_ShowTime;//记录时间次数用的，因为只有2个定时器

	INotifyInterface* m_pSetInterface;

	DWORD m_tTextColor;
	CFont m_tFontText;

	CRect m_rcMsgBoxSpinner;
	CRect m_rcMsgBoxText;
	CSkinButton m_btnCancelButton;

	Gdiplus::Image* m_SpinnerGif;
	PropertyItem* m_pGifItem;
	int m_GifCount;
	int m_GifFrameCount;
	GUID    m_GifGuid;

	CString m_strText;
	CString m_strCaption;
	UINT m_uType;

	UINT m_uCancelRet;

	CStatic m_bmpSpinner;
	int m_type;
	HWND m_hFrame;
};

class CWaitPicDlg : public CDialogImpl<CWaitPicDlg>
{
public:
	CWaitPicDlg()
	{
		m_ShowTime = 0;
		m_rcMsgBoxSpinner.SetRect(0, 0, 85, 85);
		m_strCaption = L"财金汇";
		m_SpinnerGif = NULL;
		m_pGifItem = NULL;
		m_GifCount = 0;
		m_GifFrameCount = 0;
		m_GifGuid = FrameDimensionTime;
	}
	~CWaitPicDlg()
	{
		if(m_pGifItem)
			free(m_pGifItem);
		if(m_SpinnerGif)
			delete m_SpinnerGif;
	}
public:
	enum { IDD = IDD_DIALOG_MSGBOX };

	BEGIN_MSG_MAP(CWaitPicDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnCancel)//正常关闭
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	LRESULT OnCancel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		KillTimer(MH_GIF_REFRESH);
		EndDialog(IDCANCEL);
		return 0;
	}
	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		HBRUSH hbr;
		HDC hdc = (HDC) wParam;
		HWND hwnd = (HWND) lParam;
		//获得当前控件的画刷，因为改变字体颜色后，需要返回自身的画刷才能生效
		hbr = (HBRUSH) GetCurrentObject(hdc, OBJ_BRUSH); 

		if(m_bmpSpinner.m_hWnd == hwnd) //判断是否需要修改的控件
		{//如果是，修改字体颜色
			CDCHandle dc((HDC)wParam);
			dc.SetBkColor(RGB(255, 255, 255));
			//dc.SetBkMode(TRANSPARENT);

			return LRESULT(m_bkBrush); 
		}
		return LRESULT(hbr); 

	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		CDC fdc(GetDC());
		if(wParam == MH_GIF_REFRESH)
		{
			CDC dc(m_bmpSpinner.GetDC());
			Graphics gh(dc); //hDC是外部传入的画图DC
			gh.DrawImage(m_SpinnerGif, 0, 0,m_SpinnerGif->GetWidth(),m_SpinnerGif->GetHeight());
			//重新设置当前的活动数据帧
			m_SpinnerGif->SelectActiveFrame(&m_GifGuid, m_GifCount ++);
			if(m_GifCount == m_GifFrameCount) //frameCount是上面GetFrameCount返回值
				m_GifCount = 0;     //如果到了最后一帧数据又重新开始
			//计算此帧要延迟的时间
			//long lPause = ((long*)pItem->value)[m_GifCount]*10;
		}

		if(wParam == MH_DELAY_SHOW)
		{
			ShowWindow(SW_SHOW);
			SetTimer(MH_GIF_REFRESH, 100 ,NULL);
			KillTimer(MH_DELAY_SHOW);
		}
		return 0;
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PreCreate();
		::SetWindowLongW(m_hWnd, GWL_EXSTYLE, ::GetWindowLongW(m_hWnd, GWL_EXSTYLE)^0x80000 | WS_EX_TOOLWINDOW); 
		::SetLayeredWindowAttributes(m_hWnd, RGB(255, 255, 255), 0, LWA_COLORKEY);
		m_bkBrush = ::CreateSolidBrush(RGB(255,255,255));

		m_bmpSpinner.Create(m_hWnd, m_rcMsgBoxSpinner, NULL, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS, 0);
		SetWindowPos(HWND_TOP, 0, 0, 0, 0,SWP_NOSIZE);		
		CenterWindow(GetDesktopWindow());
		SetTimer(MH_DELAY_SHOW , 500, NULL);//延迟0.5s进行显示
		//SetTimer(MH_GIF_REFRESH, 100 ,NULL);

		return 0;
	}

	LRESULT OnNcPaint(WORD /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		static int time = 2;

		if(time > 1)
		{
			ShowWindow(SW_HIDE);
			time --;
		}
		return 0;
	}

	void PreCreate()
	{
		::ShowWindow(GetDlgItem(IDC_BUTTON_1), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 1), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 2), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 3), SW_HIDE);
		LoadMsgBoxIcons();
		CalcMsgBoxSize();
	}

	void CalcMsgBoxSize()
	{
		SetWindowPos(NULL, 0, 0, 85, 85, 0);
	}



public:

	bool LoadMsgBoxIcons()
	{
		bool bRet = true;

		TCHAR szPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szPath, _countof(szPath));
		::PathRemoveFileSpecW(szPath);

		CString strSkinDir(szPath);
		strSkinDir += _T("\\Skin\\");	
		m_SpinnerGif = new Gdiplus::Image(strSkinDir + _T("loading.gif"));

		UINT count = 0;
		count = m_SpinnerGif->GetFrameDimensionsCount();
		GUID *pDimensionIDs = (GUID*)new GUID[count];
		m_SpinnerGif->GetFrameDimensionsList(pDimensionIDs, count);
		WCHAR strGuid[39];
		StringFromGUID2(pDimensionIDs[0], strGuid, 39);
		m_GifFrameCount = (int)m_SpinnerGif->GetFrameCount(&pDimensionIDs[0]);

		delete []pDimensionIDs;

		int size = m_SpinnerGif->GetPropertyItemSize(PropertyTagFrameDelay);;
		m_pGifItem = (PropertyItem*)malloc(size);
		m_SpinnerGif->GetPropertyItem(PropertyTagFrameDelay, size, m_pGifItem);

		return bRet;
	}



protected:
	int m_ShowTime;//记录时间次数用的，因为只有2个定时器

	CRect m_rcMsgBoxSpinner;
	Gdiplus::Image* m_SpinnerGif;
	PropertyItem* m_pGifItem;
	int m_GifCount;
	int m_GifFrameCount;
	GUID    m_GifGuid;

	CString m_strCaption;
	UINT m_uType;

	CStatic m_bmpSpinner;
	HBRUSH	m_bkBrush;
};

