#include <atlctrlx.h>
#include <string>
#include "../BankData/BankData.h"
#include "Security/Security.h"
#include "Skin/AltSkinClasses.h"
#include <Dbt.h>
#pragma once
using namespace std;
#define WM_SHNOTIFY WM_USER + 0x101f//通知内核进程有设备usb插入

#define WM_FEEDBACK WM_USER + 0x102f //被通知构造反馈包反馈数据
#define ICON_WIDTH					48
#define ONEPAGENUM			3
#define NULLDATA			-1
enum ePageType
{
	eNormalPage,
	eSettingPage
};
class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
	public CMessageFilter, public CIdleHandler
{
public:
	CMainDlg();

public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)

		MSG_WM_LBUTTONDOWN(OnLButtonDown)

		COMMAND_ID_HANDLER(IDC_OK, OnOK)
		COMMAND_ID_HANDLER(IDC_CANCEL, OnCancel)// 点击了取消
		COMMAND_ID_HANDLER(IDC_PRE, OnPre)
		COMMAND_ID_HANDLER(IDC_NEXT, OnNext)
		COMMAND_ID_HANDLER(ID_CLOSE, OnHide)

		MESSAGE_HANDLER(WM_NCPAINT,OnNcPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)

		MESSAGE_HANDLER(WM_MONEYHUB_FEEDBACK, OnFeedBack)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTrayIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	LRESULT OnFeedBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnPre(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//关闭按钮的
	LRESULT OnHide(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnNext(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void OnLookUp(wstring mail);
	void NoTips();//今日不再提醒

	//LRESULT OnDeviceChange(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnNcPaint(WORD /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	void OnMouseMove(UINT nFlags, CPoint point) ;

	BOOL OnEraseBkgnd(CDCHandle dc);

	bool CheckNetState();
	

protected:
	void CloseDialog(int nVal);
private:
	int m_totalTipNum;			// 显示条数
	int m_iPageNum;				// 应该显示的总页数
	std::vector<DATUSEREVENT>		m_event; // 获得当天所有账号所有的事件，原始数据
	std::vector<DATUSEREVENT>		m_eventcopy; // 获得当天所有账号所有的事件，临时文件，用于更改设置
	int								m_iteCurEvent[3];//指向当前的多个数据，如果指向NULLDATA则为没有数据
	void FetchData();
	void RefreshData();

	ePageType m_iCurrentPageType;		// 记录当前显示界面的种类，目前分为设置页和正常显示页
	int m_iCurrentPage;			// 当前显示的页码
	int m_iTipsNum;				// 要显示的数据条数
	int m_iCurrentTipsNum;		// 当前页显示的数据条数

	void ReCalculateParam();

	CRect m_rcTotalInfo;		// 中间的提示区域
	CRect m_rcSetting;			// 设置
	CRect m_rcNoTips;			// 今日不再提醒
	CRect m_rcNumShow;			// 显示页码切换
	bool  m_bMouseOn[3];		// 记录鼠标是否在这3个图标上
	bool  m_bOnSettingRect;		// 
	bool  m_bOnNoTipsRect;

	CSkinCheckBox m_ckBox[3];		// 3个checkbox按钮
	CSkinButton m_btNext;		// 下一页按钮
	CSkinButton m_btPre;		// 前一页按钮
	CSkinButton m_btOK;			// 确定按钮
	CSkinButton m_btCancel;		// 取消按钮
	CSkinButton m_close;		// 关闭按钮

	CPen m_colorEdge;			// 整个泡泡边框的底色
	CBrush m_colorBack;			// 背景色
	CBrush m_colorTipBk;		// 中间提示区域的底色
	CBrush m_colorMouseMoveBk;	// 中间鼠标滑过的时候的颜色
	CPen m_colorMouseMoveLine;  // 中间鼠标滑过时的中间区域的线的底色
	CPen m_colorInnerEgde;		// 里面边框的颜色

	CIcon m_icoMainIcon;			// ico
	bool bTrackLeave;				// (也可设为成员变量)
	bool m_bShouldShow;				// 是否应该显示
	bool m_bTodayShow;
	int m_timer;					// 计数器，计算界面离开的时间

	CTuoImage m_imgCloseButton;		// 关闭按钮的背景
	CTuoImage m_imgPreButton;		// 前一页图片
	CTuoImage m_imgNextButton;		// 下一页
	CTuoImage m_imgComButton;		// 通用按钮，取消和确定
	CTuoImage m_imgCheckBox;		// checkbox的图片

	HFONT m_TitleFont;				// 标题字体
	HFONT m_CommonEngFont;			// 普通英文字体
	HFONT m_CommonFont;				// 普通字体
	bool m_Netstat;//记录网络状态


	void DrawBack(CDCHandle dc, const RECT &rect);
	void ShowPop();
	bool IsVista();
	void CheckShow();
	void AutoUpdate();
	BOOL IsAutoRunUpdate();
	bool m_delayflag;
	int PostData2Server(string hid, string sn, wstring file);

	SYSTEMTIME m_lastTime;
	__int64  TimeDiff(SYSTEMTIME  left,SYSTEMTIME  right);
};
