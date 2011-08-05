#include <gdiplus.h>
#include <GdiPlusImaging.h>
#include "AltSkinClasses.h"
#include "../Resource.h"
using namespace std;
using namespace Gdiplus;
// 等待对话框开始计时
#define WM_BEGIN_WAIT_DLG							(WM_USER + 0x4025)
//隐藏等待对话框
#define WM_HIDE_WAIT_DLG                            (WM_USER + 0x4026)

#define MH_GIF_REFRESH 0xf2
#define MH_SHOWTIMER   0x0B
#define MAXTIMEELAPSE  600

class CProgressDlg : public CDialogImpl<CProgressDlg>, public CDialogSkinMixer<CProgressDlg>
{
public:
	CProgressDlg()
	{
		m_rcMsgBoxSpinner.SetRect(20, 50, 20 + 45, 50 + 45);
		m_strCaption = L"财金汇安全检测";
		m_SpinnerGif = NULL;
		m_pGifItem = NULL;
		m_GifCount = 0;
		m_GifFrameCount = 0;
		m_GifGuid = FrameDimensionTime;
	}
	~CProgressDlg()
	{
		if(m_pGifItem)
			free(m_pGifItem);
		if(m_SpinnerGif)
			delete m_SpinnerGif;
	}
public:
	enum { IDD = IDD_DIALOG_MSGBOX };

	BEGIN_MSG_MAP(CProgressDlg)
		CHAIN_MSG_MAP(CDialogSkinMixer<CProgressDlg>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_BEGIN_WAIT_DLG, OnBegin)
		MESSAGE_HANDLER(WM_HIDE_WAIT_DLG, OnHide)
		COMMAND_ID_HANDLER(IDC_BUTTON_1, OnButtonClicked)
		//COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if( wParam == MH_SHOWTIMER )
		{
			KillTimer(MH_SHOWTIMER);
			ShowWindow(SW_SHOW);
		}
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
		return 0;
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetTimer(MH_GIF_REFRESH, 100 ,NULL);
		m_bmpSpinner.Create(m_hWnd, m_rcMsgBoxSpinner, NULL, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS, 0);

		SetWindowPos(HWND_TOP, 0, 0, 0, 0,SWP_NOSIZE);
		CenterWindow(GetDesktopWindow());
		SetWindowText(m_strCaption);
		m_btnSysClose.ShowWindow(SW_HIDE);

		// 不用显示按钮
		::ShowWindow(GetDlgItem(IDC_BUTTON_1), false);
		::ShowWindow(GetDlgItem(IDC_BUTTON_2), false);
		::ShowWindow(GetDlgItem(IDC_BUTTON_3), false);
		::ShowWindow(GetDlgItem(IDC_BUTTON_4), false);

		return 0;
	}

	void PreCreate()
	{
		LPCTSTR lpszFontName[] = { _T("Tahoma"), _T("微软雅黑") };

		int iIndex = IsVista() ? 1 : 0;
		if (NULL == m_tFontText)
			m_tFontText.CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, lpszFontName[iIndex]);
		m_tTextColor = RGB(16, 93, 145);

		m_strText = L"安全检测中，请稍候......";
		m_btnCancelButton = GetDlgItem(IDC_BUTTON_1);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 1), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 2), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_BUTTON_1 + 3), SW_HIDE);
		m_btnCancelButton.ApplySkin(&m_imgDlgButton, 4);
		m_btnCancelButton.SetWindowText(_T("取消"));
		LoadMsgBoxIcons();
		CalcMsgBoxSize();
		//CalcButtonsPos();
	}

	void CalcMsgBoxSize()
	{
		CDC dc(GetDC());
		dc.SelectFont(m_tFontText);

		m_rcMsgBoxText.SetRect(0, 0, 0, 0);
		dc.DrawText(m_strText, -1, &m_rcMsgBoxText, DT_CALCRECT);

		int nWidth = m_rcMsgBoxText.Width() + 110;
		int nHeight = m_rcMsgBoxText.Height() + 100;

		/*CRect rcButton;
		m_btnCancelButton.GetWindowRect(&rcButton);
		int nMinWidth = (rcButton.Width() + 10) + 10;
		if (nWidth < nMinWidth)
			nWidth = nMinWidth;*/

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
			m_rcMsgBoxSpinner.right + m_rcMsgBoxText.Width() + 10,
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
		return 0;
	}

	LRESULT OnBegin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		SetTimer(MH_SHOWTIMER, MAXTIMEELAPSE, NULL);

		return 0;
	}


	LRESULT OnHide(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		KillTimer(MH_SHOWTIMER);
		ShowWindow(SW_HIDE);
		return 0;
	}

	void OnFinalMessage(HWND hWnd)
	{
		delete this;
		PostQuitMessage(0);
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

protected:

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
};