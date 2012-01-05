#pragma once
#ifndef _SOGOUTOOLTIP_H
#define _SOGOUTOOLTIP_H
#include "../../Skin/SkinManager.h"
#include "Locks.h"
#include "Structs.h"
#include "TooltipElem.h"
#include "FadeInOut.h"


// 开发测试使用
#define _SOGOU_TOOLTIP_LAYERED
#define _SOGOU_TOOLTIP_NEW

// 隐藏汽泡
//		wparam:	BOOL 是否通知用户
//		lparam:	NULL
#define MSG_SOGOUTT_HIDE (WM_USER + 1)
#define MSG_SOGOUTT_CHECK_HIDE (WM_USER+2)
// 准备隐藏汽泡
//		wparam:	BOOL 是否是用户点了X关闭的
//		lparam:	NULL
#define MSG_SOGOUTT_PREPARE_HIDE (WM_USER+3)

#define TTN_GETINDEX	(WM_USER + 1)
#define TTN_NEEDSHOWTIP (WM_USER + 2)

#define ID_SOGOU_TOOLTIP 1000
#define RELAYEVENTEX_NOTIP  ((DWORD_PTR)NULL)


// 气泡提示所使用的图标
#define BALLOON_ICO_PASSPORT			(HICON)-1
#define BALLOON_ICO_WEBACC				(HICON)-2
#define BALLOON_ICO_SOUND_BLOCK			(HICON)-3
#define BALLOON_ICO_PRIVACY				(HICON)-4
#define BALLOON_ICO_ADBLOCK_WINDOW		(HICON)-5
#define BALLOON_ICO_ADBLOCK_FLOAT		(HICON)-6

#define BALLOON_SPECIAL_ICON_COUNT		6		// 特殊图标个数

#define BALLOON_ICO_HUNG				(HICON)-11
#define BALLOON_ICO_CRASH				(HICON)-12


// 气泡位置为-1时表示要消失
#define INVALID_ITEM		-1



class CSogouToolRect : public tag_ToolRect
{
public:
	CSogouToolRect() 
	{
		hWnd = NULL ; 
		uFlags = 0 ; 
		hIcon = NULL ; 
		pdwIndex = NULL ; 
		ZeroMemory(&rc,sizeof(rc));
		ZeroMemory(&IconSize , sizeof(SIZE)); 
	}
	CSogouToolRect(const TOOLRECT& tr )
	{
		hWnd = tr.hWnd ;
		uFlags = tr.uFlags ;
		hIcon = tr.hIcon ;
		IconSize = tr.IconSize ; 
		pdwIndex = tr.pdwIndex ;
		wstrTitle = tr.wstrTitle ;
		wstrText = tr.wstrText ;
		rc = tr.rc ;

	}
	bool operator == (const POINT&pt) { return ::PtInRect(&rc , pt) != FALSE; }
	bool operator == (const CSogouToolRect& STRect) { return memcmp(&rc , &STRect.rc , sizeof(RECT))==0 ; }
	bool operator == (const TOOLRECT& STRect) { return memcmp(&rc , &STRect.rc , sizeof(RECT))==0 ; }
	bool operator == (const DWORD_PTR pdwi ){return pdwi==pdwIndex;}

};

class CSogouToolTipInfo : public tag_SogouToolTipInfo
{
public:
	CSogouToolTipInfo()
	{
		uFlags = 0;
		hIcon = NULL ; 
		hBmp = NULL ;
		hWnd = NULL ; 
		hFollowWnd = NULL ;
		uDelay = 1000 ; 
		uShowTimeOut = 5000;
		uCallBackMsg = 0 ;
		dwCheckBoxState = 0 ;
		IconSize = CSize(0,0) ; 
		BalloonSize = CSize(0,0) ;
		pdwIndex = NULL ; 
	}
	CSogouToolTipInfo(const SOGOUTOOLINFO& sttf)
	{
		uFlags = sttf.uFlags ;
		hIcon = sttf.hIcon ;
		hBmp = sttf.hBmp ;
		hWnd = sttf.hWnd ;
		hFollowWnd = sttf.hFollowWnd ;
		uDelay = sttf.uDelay ;
		uShowTimeOut = sttf.uShowTimeOut ;
		uCallBackMsg = sttf.uCallBackMsg ;
		dwCheckBoxState = sttf.dwCheckBoxState ;
		wstrText = sttf.wstrText ;
		wstrTitle = sttf.wstrTitle ;
		wstrCheckBox = sttf.wstrCheckBox ;
		IconSize = sttf.IconSize ;
		BalloonSize = sttf.BalloonSize ;
		pdwIndex = sttf.pdwIndex;
	}

	CSogouToolTipInfo& operator = (const SOGOUTOOLINFO& sttf)
	{
		uFlags = sttf.uFlags ;
		hIcon = sttf.hIcon ;
		hBmp = sttf.hBmp ;
		hWnd = sttf.hWnd ;
		hFollowWnd = sttf.hFollowWnd ;
		uDelay = sttf.uDelay ;
		uShowTimeOut = sttf.uShowTimeOut ;
		uCallBackMsg = sttf.uCallBackMsg ;
		dwCheckBoxState = sttf.dwCheckBoxState ;
		wstrText = sttf.wstrText ;
		wstrTitle = sttf.wstrTitle ;
		wstrCheckBox = sttf.wstrCheckBox ;
		IconSize = sttf.IconSize ;
		BalloonSize = sttf.BalloonSize ;
		pdwIndex = sttf.pdwIndex ;
		return *this ;
	}

	bool operator == (CSogouToolTipInfo& sttf) { return hWnd == sttf.hWnd ; }
	bool operator == (const SOGOUTOOLINFO& sttf) { return hWnd == sttf.hWnd ; }
	bool operator == (const HWND chWnd) { return hWnd == chWnd ; }
	bool operator < (const CSogouToolTipInfo& sttf) const { return hWnd<sttf.hWnd ; }
};


//	 -----------------------------------------------
//	|    TipWindow (Transparent except triangle)    |
//	|      ----------------------------------       |
//	|     |   Title                       X  |      |
//	|    /           TipClientWindow         |      |
//	|   /triangle                            |<-19->|
//	|   --                                   | (px) |
//	|     |                                  |      |
//	|      ----------------------------------       |
//	|                  BorderThick 20px             |
//	 -----------------------------------------------

//
//              3                            4  
//              |\                          /|
//	            | \                        / |
//	            |  \                      /  |
//           ---     --------------------     ---	
//	        |                                    |
//     5 ---                                      --- 6
//       \                                          /
//   	  \                                        /
//	       \                                      /
//	        |         forward of triangle        |
//	        |                                    |
//   	   /                                      \
//	      /                                        \
//	     /                                          \
//     7 ---                                      --- 8
//   	    |                                    |
//	      	 ---     --------------------     ---
//	            |  /                      \  |
//	            | /                        \ |
//	            |/                          \|
//              1                            2
//                 
//   |->Offset<-|
// 

class CSogouToolTip : public CWindowImpl<CSogouToolTip>
//	,public CTransparenceHelper<CSogouToolTip>
{
public:
	CSogouToolTip() ;
	~CSogouToolTip() ;

public:
	////////////////////////////////////
	// methods
	void RelayEvent(LPMSG lpMsg) ;
	void RelayEventEx(HWND hWnd , DWORD_PTR pdwIndex) ;
	void RelayEventSingleton(HWND hWnd, DWORD dwIndex);			// [TuotuoXP]
	BOOL SetOwnerWindow(HWND hWnd) ;
	bool SetText(std::wstring wsText) ;
	void Show(const SOGOUTOOLINFO& TTInfo , LPPOINT lpPt = NULL) ;
	BOOL AddTool(const SOGOUTOOLINFO& sttf) ;
	BOOL AddToolRect(const TOOLRECT& TRect) ;
	HWND GetBottomHWnd() ;

protected:

	BOOL IsChangeTip(LPMSG lpMsg , LPSOGOUTOOLINFO psttf , LPTOOLRECT& ptrc ) ;
	BOOL IsNeedShowTip(LPMSG lpMsg , LPSOGOUTOOLINFO psttf, LPTOOLRECT& ptrc ) ;
	void PrepareShowTip(LPSOGOUTOOLINFO lpsttf , LPTOOLRECT lptrc) ;
	LPSOGOUTOOLINFO GetWndToolInfo(HWND hWnd) ;
	LPTOOLRECT GetToolRectInfo(HWND hWnd , POINT pt) ;
	void Show( HWND hWnd , LPPOINT lpPt = NULL) ;
	void ShowEx(const SOGOUTOOLINFO& TTInfo , LPPOINT lpPt = NULL , const LPTOOLRECT lpToolRect = NULL ) ;
	void PrepareHide(bool bUserClose = false) ;
	void Hide(bool bNotifyUser = false) ;
	void ProcessFadeOut(int iStep , UINT uEla) ;
	bool GetDetail(const SOGOUTOOLINFO& TTInfo, const LPTOOLRECT lpToolRect) ;
	void CalcItemRect(CSize size) ;
	void CalcItemRect(CRect rcClient) ;
	void UpdateWindow(HDC hdc) ;
	HWND GetInsertAfterWnd(CRect& rcWindow) ;

	BEGIN_MSG_MAP(CSogouToolTip)
#ifndef _SOGOU_TOOLTIP_LAYERED
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
#endif
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST , WM_MOUSELAST , OnMouseMessage)
//		CHAIN_MSG_MAP(CTransparenceHelper<CSogouToolTip>)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSEACTIVATE(OnMouseActivate)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SETCURSOR(OnSetCursor)
		MESSAGE_HANDLER_EX(MSG_SOGOUTT_HIDE , OnHide)
		MESSAGE_HANDLER_EX(MSG_SOGOUTT_PREPARE_HIDE , OnPrepareHide)
		MESSAGE_HANDLER_EX(MSG_SOGOUTT_CHECK_HIDE , OnCheckHide)
	END_MSG_MAP()

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHide(UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	LRESULT OnPrepareHide(UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	LRESULT OnCheckHide(UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	void OnDestroy() ;
	void OnTimer(UINT_PTR nIDEvent) ;
	int OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message);
	void OnMouseMove(UINT nFlags, CPoint point) ;
	void OnLButtonDown(UINT nFlags, CPoint point) ;
	void OnLButtonUp(UINT nFlags, CPoint point) ;
	void OnLButtonDblClk(UINT nFlags , CPoint point) ;
	BOOL OnSetCursor(CWindow wnd, UINT nHitTest, UINT message) ;
	void OnMouseLeave();
#ifndef _SOGOU_TOOLTIP_LAYERED
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnPaint(CDCHandle dc, RECT rect) ;
#endif

private:

	std::wstring TranslateReturnInText(std::wstring wsText) ;
	BOOL ParseDispinfo(CSogouToolTipInfo& stti , NMSTTDISPINFO dsp );

	BOOL CreateToolTipWindow() ;
	RECT GetTipClientWindowRect(const SOGOUTOOLINFO& sttf); 	// 取得需要的有效窗口大小
	RECT GetTipWindowRect(LPRECT lpClientRect , CPoint pt ) ;
	// 计算小三角的方向
	int GetTriangleForward(LPRECT lpRect , CPoint pt ) ;
	void UpdateLayeredWindow(CDCHandle dc , int iAlph = 255) ;
	void EnsureVisiable(CLayeredMemDC* pMemDC, std::list<RECT> rcList) ;
	void EnsureRectVisiable(CDCHandle dc, RECT rc) ;
	int LIndt() ;
	int TIndt() ;
	int RIndt() ;
	int BIndt() ;
	int RowSpac();

private:

	static HHOOK s_hHookMouse ;
	static HHOOK s_hHookWndProc ;
	static LRESULT CALLBACK HookMouseProc(int nCode , WPARAM wp , LPARAM lp) ;
	static LRESULT CALLBACK HookWndProc(int nCode , WPARAM wp , LPARAM lp) ;
	static void RemoveWndFromTipList(HWND hWnd) ;
	static void AddWndToTipList( HWND hWnd) ;
	static std::vector<HWND>& TipWndVec(){static std::vector<HWND> vecTipWnd;return vecTipWnd;}
	static CCriticalSection& ListLock(){static CCriticalSection TipListLock;return TipListLock;}
	BOOL Hook();
	BOOL UnHook() ;

public:
	static bool HasTipWindow() ;
	static void HideAllBalloon() ;

private:
	int m_iTrigOffset ;	// 小三角相对于窗口的位移
	CRect rcDefaultTip ;

private:

	typedef std::vector<CSogouToolRect> TTIRectVec ;
	typedef std::vector<CSogouToolTipInfo> TTInfoVec ;
	typedef std::pair<HWND , TTIRectVec> TTIRectMapPair;
	typedef std::map<HWND , TTIRectVec> TTIRectMap ;

	// tooltip界面布局
	DWORD	m_dwStyles,m_dwStylesEx ;	// 气泡窗口样式
	int		m_iTitleHeight;	// 标题的高度
	CRect	m_rcClient  ;	// 客户区的大小及区域
	CRect	m_rcCloseBtn  ;	// 关闭按钮的区域
	CRect	m_rcTitle  ;	// 标题的区域
	CRect	m_rcContent ;	// 内容区域
	CRect	m_rcCheckBox ;	// 复选框区域
	CRect	m_rcCheckBoxText ;	// 复先框文字区域
	CPoint	m_ptIcon ;		// 图标的位置
	int		m_iForwardOfTriangle ;	// 气泡箭头方向
	DWORD	m_dwCloseBtnState ;	// 关闭按钮当前的状态
	DWORD	m_dwCheckState ;	// 复选框当前状态
	CPoint	m_ptTipPos ;	// 气泡指向的位置
	CTooltipText m_tooltipText ;	// 气泡文字
	list<CItemData>	m_UrlList ;	// 气泡文字解析后的树

	BOOL	m_bLBtnDown ;	// 鼠标左键是否按下

	HWND	m_hOwnerWnd ;

	CLayeredMemDC* m_pLayerMemDC ; // 内存DC

	bool m_bFadeOutCanStop;	// 渐入渐出的过程能不能被打断，如气泡是在渐出时如果有鼠标消息
							// 就会中断，但Tip不会。
	CTooltipFadeInOut m_TTFade ;	// 渐入渐出管理

	CSogouToolTipInfo m_sttfCurr ;	// 当前气泡信息
	CSogouToolRect m_TRectCurr ;	// 当前的区域信息
	TTIRectMap m_ToolRectInfo ;	// 这个实便的所有区域
	TTInfoVec m_ToolInfo ;		// 这个实例所承载的所有气泡

	SOGOUTOOLINFO* m_pDelayShowToolInfo ;	// 延迟显示的气泡信息
	LPTOOLRECT m_pDelayShowToolRect ;		// 延迟显示的区域信息


	DWORD m_dwLastItemIndex;	// 上一个Tip的索引
	HWND m_hLastTipWnd;	// 最后显示Tip的窗口
	CSogouToolTipInfo m_tipInfo ;	

	CCursor m_hCursorHand ;
	LINKBLOCK* m_pCurLink ;
	eCursor m_CursorState ;


public:

	static CSogouToolTip *sm_pPop;
	static CSogouToolTip* Get();
	static CSogouToolTip* GetTooltip()
	{
		static CSogouToolTip *pTooltip = NULL;
		if (pTooltip == NULL)
		{
			pTooltip = new CSogouToolTip();
			pTooltip->CreateToolTipWindow();
		}
		return pTooltip;
	}
};

#endif


