#include "stdafx.h"
#include "../../Skin/ResourceSkin/ToolTipSkin.h"
#include "ToolTip.h"


#pragma  comment(lib,"Winmm.lib")

#define ROWSPACING		5
#define TEXTPICSPACING	5
#define LINDENT 8
#define TINDENT 8
#define RINDENT 18
#define BINDENT 8
#define LFRAME	16
#define TFRAME	16
#define RFRAME	19
#define BFRAME	19
#define CHECKBOXHEIGHT 15

#define RECTINFO_NULL ((LPTOOLRECT)-1)

HHOOK CSogouToolTip::s_hHookMouse = NULL ;
HHOOK CSogouToolTip::s_hHookWndProc = NULL ;

void CSogouToolTip::AddWndToTipList(HWND hWnd)
{
	ATLASSERT(::IsWindow(hWnd)) ;
	ListLock().Lock();
	std::vector<HWND>::iterator iter = find(TipWndVec().begin() , TipWndVec().end() , hWnd) ;
	if ( iter==TipWndVec().end() )
		TipWndVec().push_back(hWnd) ;
	ListLock().UnLock();
}

void CSogouToolTip::RemoveWndFromTipList(HWND hWnd)
{
	ListLock().Lock();

	std::vector<HWND>::iterator iter = find( TipWndVec().begin() , TipWndVec().end() , hWnd) ;
	ATLASSERT( TipWndVec().end() != iter ) ;
	if ( TipWndVec().end()!=iter )
		TipWndVec().erase( iter ) ;

	ListLock().UnLock();
}

BOOL CSogouToolTip::Hook()
{
	if ( NULL==s_hHookMouse )
		s_hHookMouse = ::SetWindowsHookEx(WH_MOUSE , HookMouseProc , NULL , GetCurrentThreadId()) ;
	if ( NULL == s_hHookWndProc )
		s_hHookWndProc = ::SetWindowsHookEx(WH_CALLWNDPROC , HookWndProc , NULL , GetCurrentThreadId()) ;

	return NULL!=s_hHookMouse&&NULL!=s_hHookWndProc ;
}

BOOL CSogouToolTip::UnHook()
{
	BOOL bRst = TRUE ;
	if ( TipWndVec().size()==0 )
	{
		bRst = UnhookWindowsHookEx(s_hHookMouse) ;
		s_hHookMouse = NULL ;
		bRst &= UnhookWindowsHookEx(s_hHookWndProc) ;
		s_hHookWndProc = NULL ;
	}
	return bRst ;
}

LRESULT CSogouToolTip::HookMouseProc(int nCode , WPARAM wp , LPARAM lp)
{
	if ( WM_LBUTTONDOWN==wp || WM_MBUTTONDOWN==wp || WM_RBUTTONDOWN==wp )
	{
		static int iLastTickCount = 0 ;
		int iTickCount = GetTickCount() ;
		if ( iLastTickCount != iTickCount )
		{
			iLastTickCount = iTickCount ;
			MOUSEHOOKSTRUCT* pmhs = (MOUSEHOOKSTRUCT*)lp ;
			ListLock().Lock();
			std::vector<HWND>::iterator iter = find(TipWndVec().begin() , TipWndVec().end() , pmhs->hwnd) ;
			if ( iter == TipWndVec().end() )
			{	// 没有点到汽球上
				for ( iter=TipWndVec().begin() ; iter!=TipWndVec().end() ; iter++ )
					::PostMessage(*iter , MSG_SOGOUTT_CHECK_HIDE , NULL , NULL) ;
			}
			ListLock().UnLock() ;
		}
	}

	return ::CallNextHookEx(s_hHookMouse , nCode , wp , lp) ;
}

LRESULT CSogouToolTip::HookWndProc(int nCode , WPARAM wp , LPARAM lp)
{
	PCWPSTRUCT pcwps = (PCWPSTRUCT)lp ;
	if ( WM_ACTIVATEAPP == pcwps->message && !pcwps->wParam)
	{
		ListLock().Lock();
		std::vector<HWND>::iterator iter = find(TipWndVec().begin() , TipWndVec().end() , pcwps->hwnd) ;
		if ( iter == TipWndVec().end() )
		{	// 没有点到汽球上
			for ( iter=TipWndVec().begin() ; iter!=TipWndVec().end() ; iter++ )
				::PostMessage(*iter , MSG_SOGOUTT_CHECK_HIDE , NULL , NULL) ;
		}
		ListLock().UnLock() ;
	}
	else if ( WM_WINDOWPOSCHANGED==pcwps->message )
	{
		//
	}

	return ::CallNextHookEx(s_hHookWndProc , nCode , wp , lp) ;
}

bool CSogouToolTip::HasTipWindow()
{
	ListLock().Lock();
	bool bEmpty = TipWndVec().empty() ;
	ListLock().UnLock() ;
	return !bEmpty ;
}

CSogouToolTip::CSogouToolTip() : m_dwLastItemIndex(-1), m_hLastTipWnd(NULL)
{
	rcDefaultTip = CRect(20,20,150,80) ;
	m_dwStyles = WS_POPUP | WS_CLIPSIBLINGS;
#if defined(_SOGOU_TOOLTIP_LAYERED)
	m_dwStylesEx = WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE | WS_EX_LAYERED;
#else
	m_dwStylesEx = WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE;
#endif
	m_pLayerMemDC = NULL ;
	m_pDelayShowToolRect = NULL ;
	m_pDelayShowToolInfo = NULL ;
	m_iTitleHeight = 0 ;
	m_bFadeOutCanStop = true ;
	m_hOwnerWnd = NULL ;
}
CSogouToolTip::~CSogouToolTip() 
{
	if(IsWindow())
	{
		DestroyWindow() ;
	}

}

int CSogouToolTip::LIndt()
{
	return m_sttfCurr.uFlags&eTTF_BALLOON?LINDENT:4;
}

int CSogouToolTip::TIndt()
{
	return m_sttfCurr.uFlags&eTTF_BALLOON?TINDENT:3 ;
}

int CSogouToolTip::RIndt()
{
	return m_sttfCurr.uFlags&eTTF_BALLOON?RINDENT:4 ;
}

int CSogouToolTip::BIndt()
{
	return m_sttfCurr.uFlags&eTTF_BALLOON?BINDENT:4 ;
}

int CSogouToolTip::RowSpac()
{
	return m_sttfCurr.uFlags&eTTF_BALLOON?ROWSPACING:0;
}

std::wstring CSogouToolTip::TranslateReturnInText(std::wstring wsText)
{
	size_t nPos = 0 ;

	do 
	{
		nPos = wsText.find('\n',nPos) ;
		if ( nPos!=std::wstring::npos )
			wsText.replace(nPos , 1 , _T("<item type='wrap'></item>")) ;
	} while (nPos!=std::wstring::npos);

	return std::wstring(wsText) ;
}

#ifndef _SOGOU_TOOLTIP_NEW
#else

HWND CSogouToolTip::GetBottomHWnd()
{
	HWND hWnd = FindWindow(_T("MH_Tooltip") , NULL) ;
	return hWnd ;
}

void CSogouToolTip::HideAllBalloon()
{
	for ( std::vector<HWND>::iterator it = TipWndVec().begin(); it!=TipWndVec().end(); it++ )
	{
		if ( ::GetWindow(*it, GW_OWNER)==NULL )
			::PostMessage(*it, MSG_SOGOUTT_PREPARE_HIDE , FALSE, NULL);
	}
}

LPSOGOUTOOLINFO CSogouToolTip::GetWndToolInfo(HWND hWnd)
{
	TTInfoVec::const_iterator citer = find(m_ToolInfo.begin() , m_ToolInfo.end() , hWnd) ;
	ATLASSERT(m_ToolInfo.end()!=citer) ;
	return (SOGOUTOOLINFO*)&*citer ;
}

LPTOOLRECT CSogouToolTip::GetToolRectInfo(HWND hWnd , POINT pt)
{
	TTIRectVec& vecTTI = m_ToolRectInfo[hWnd] ;
	TTIRectVec::const_iterator cRIter = find(vecTTI.begin() , vecTTI.end() , pt) ;
	if ( cRIter==vecTTI.end() )
		return RECTINFO_NULL ;
	else
		return (LPTOOLRECT)&*cRIter ;
}

BOOL CSogouToolTip::IsChangeTip(LPMSG lpMsg , LPSOGOUTOOLINFO psttf , LPTOOLRECT& ptrc )
{	
	if ( lpMsg->hwnd!=m_sttfCurr.hWnd )
		return TRUE ;

	if( psttf->uFlags&eTTF_INDEXCALLBACK )
	{
		NMSTTINDEX ttidx = {{lpMsg->hwnd,ID_SOGOU_TOOLTIP,TTN_GETINDEX},NULL};
		::SendMessage(lpMsg->hwnd , WM_NOTIFY , NULL , (LPARAM)&ttidx) ;

		if ( m_sttfCurr.pdwIndex==ttidx.pdwIndex )
			return FALSE;
		else
			m_sttfCurr.pdwIndex = ttidx.pdwIndex ;
	}
	else
	{
		if ( NULL == ptrc )
			ptrc = GetToolRectInfo(lpMsg->hwnd , lpMsg->pt) ;

		if( ptrc != RECTINFO_NULL && m_TRectCurr==*ptrc )
			return FALSE;
	}

	return TRUE ;
}

BOOL CSogouToolTip::IsNeedShowTip(LPMSG lpMsg , LPSOGOUTOOLINFO psttf , LPTOOLRECT& ptrc )
{
	ATLASSERT(NULL!=psttf) ;
	if ( NULL == psttf )
		return FALSE ;

	if ( psttf->uFlags&eTTF_INDEXCALLBACK )
	{
		NMSTTNEEDSHOWTIP ttst = {{lpMsg->hwnd,ID_SOGOU_TOOLTIP,TTN_NEEDSHOWTIP},TRUE};
		::SendMessage(lpMsg->hwnd , WM_NOTIFY , NULL , (LPARAM)&ttst ) ;
		return ttst.bShowTip ;
	}
	else
	{
		if ( NULL == ptrc )
			ptrc = GetToolRectInfo(lpMsg->hwnd , lpMsg->pt) ;

		if ( ptrc == RECTINFO_NULL )
			return FALSE ;
		else
		{
			if ( m_pDelayShowToolRect==ptrc )
				return FALSE ;
			m_pDelayShowToolRect = ptrc ;
		}
	}

	return TRUE ;
}

void CSogouToolTip::RelayEvent(LPMSG lpMsg)
{
	if ( NULL == m_hWnd )
		CreateToolTipWindow() ;

	ATLASSERT(NULL!=lpMsg) ;
	MSG Msg = *lpMsg ;
	LPSOGOUTOOLINFO pCurToolInfo = NULL ;
	LPTOOLRECT pCurToolRect = NULL ;
	Msg.pt = CPoint(GET_X_LPARAM(lpMsg->lParam) , GET_Y_LPARAM(lpMsg->lParam)) ;
	if ( Msg.message>=WM_MOUSEFIRST && Msg.message <=WM_MOUSELAST)
	{
		if ( WM_MOUSEMOVE == Msg.message )
		{
			pCurToolInfo = GetWndToolInfo(Msg.hwnd) ;
			if( IsWindowVisible() )
			{
				if ( IsChangeTip(&Msg , pCurToolInfo , pCurToolRect) )
				{
					if( IsNeedShowTip(&Msg , pCurToolInfo , pCurToolRect) )
						PrepareShowTip(pCurToolInfo , pCurToolRect) ;
					else
						PrepareHide() ;
				}
			}
			else if ( IsNeedShowTip(&Msg , pCurToolInfo , pCurToolRect) )
			{
				PrepareShowTip(pCurToolInfo , pCurToolRect);
			}
		}
	}// end if

}

void CSogouToolTip::RelayEventEx(HWND hWnd , DWORD_PTR pdwIndex)
{
	if ( NULL == m_hWnd )
		CreateToolTipWindow() ;

	LPSOGOUTOOLINFO pCurToolInfo = NULL ;
	LPTOOLRECT pCurToolRect = NULL ;
	
	pCurToolInfo = GetWndToolInfo(hWnd) ;
	if ( RELAYEVENTEX_NOTIP==pdwIndex )
	{
		pCurToolInfo->pdwIndex = RELAYEVENTEX_NOTIP ;
		m_sttfCurr.hWnd = NULL ;
		PrepareHide() ;
	}
	else
	{
		if ( m_sttfCurr.pdwIndex != pdwIndex )
		{
			m_sttfCurr.pdwIndex = pdwIndex ;
			TTIRectMap::iterator cMapIter = m_ToolRectInfo.find(hWnd) ;
			if ( cMapIter==m_ToolRectInfo.end() )
				PrepareShowTip(pCurToolInfo , pCurToolRect);	// 没有加入区域
			else
			{
				TTIRectVec& vecTTI = cMapIter->second ;
				TTIRectVec::const_iterator cRIter = find(vecTTI.begin() , vecTTI.end() , pdwIndex) ;
				if ( cRIter!=vecTTI.end() )
				{
					PrepareShowTip(pCurToolInfo , (LPTOOLRECT)&*cRIter) ;
				}
				else
				{
					if(IsWindowVisible() && m_sttfCurr.hWnd==hWnd)
						PrepareHide();
				}
			}
		}
	}
}
#endif

void CSogouToolTip::PrepareShowTip(LPSOGOUTOOLINFO lpsttf , LPTOOLRECT lptrc) 
{
	m_pDelayShowToolRect=lptrc ;
	m_pDelayShowToolInfo = lpsttf ;

	SetTimer(eTimer_ToolTipDelayShow , m_sttfCurr.hWnd==m_pDelayShowToolInfo->hWnd?0:m_pDelayShowToolInfo->uDelay) ;
}

void CSogouToolTip::Show( HWND hWnd , LPPOINT lpPt) 
{
	ATLASSERT(NULL!=lpPt) ;
	ShowEx(*m_pDelayShowToolInfo , lpPt , m_pDelayShowToolRect) ;
	m_pDelayShowToolRect = NULL ;
}

void CSogouToolTip::Show(const SOGOUTOOLINFO& TTInfo , LPPOINT lpPt) 
{
	// 普通汽泡不能抢占高级汽泡
	if ( IsWindow()&&IsWindowVisible()
		&&(m_sttfCurr.uFlags&eTTF_BALLOON)&&(m_sttfCurr.uFlags&eTTF_ADVANCE)
		&&(TTInfo.uFlags&eTTF_BALLOON) &&( !(TTInfo.uFlags&eTTF_ADVANCE) && !(TTInfo.uFlags&eTTF_ROBADVANCE) )
	)
	{
		return ;
	}
	
	ShowEx(TTInfo , lpPt , NULL) ;
}

void CSogouToolTip::ShowEx(const SOGOUTOOLINFO& TTInfo , LPPOINT lpPt /* = NULL  */, const LPTOOLRECT lpToolRect /* = NULL */ )
{
	if ( NULL == m_hWnd )
		CreateToolTipWindow() ;

	std::wstring wszOldTitle = m_sttfCurr.wstrTitle ;
	m_pCurLink = NULL ;

	if ( m_TTFade.IsVaild() )
	{
		KillTimer(eTimer_FadeInOut) ;
		m_TTFade.SetValid(false) ;
	}

	if ( GetDetail(TTInfo, lpToolRect) )
	{
		if ( NULL != lpPt ) 
			m_ptTipPos = *lpPt ;
		else
			::GetCursorPos(&m_ptTipPos) ;

		m_rcClient = GetTipClientWindowRect(m_sttfCurr) ;
		m_iForwardOfTriangle = GetTriangleForward(&m_rcClient , m_ptTipPos) ;
		CalcItemRect(m_rcClient) ;
		CRect rcWindow = GetTipWindowRect(&m_rcClient , m_ptTipPos ) ;

// 		// 发出声音
// 		if ( m_sttfCurr.uFlags&eTTF_BALLOON )
// 		{
// 			// 如果当前气泡是不可见的，那么从不可见到可见肯定要发出声音
// 			// 但如果当前是可见的，只是切换内容，那么当title相同时我就认为是
// 			// 同一个气泡在刷新内容，就不发声，否则就发声
// 			if(!IsWindowVisible() || IsWindowVisible() && wszOldTitle!=m_sttfCurr.wstrTitle)
// 				PlaySound(L"SystemNotification",   NULL,   SND_ASYNC) ;
// 		}

#if defined(_SOGOU_TOOLTIP_LAYERED)
		CClientDC dc(m_hWnd);
		UpdateWindow(dc.m_hDC) ;
		SetWindowPos(HWND_NOTOPMOST, rcWindow.left,rcWindow.top,rcWindow.Width(),rcWindow.Height(), SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSENDCHANGING|SWP_NOSIZE ) ;
		SetWindowPos( GetInsertAfterWnd(rcWindow) , rcWindow.left,rcWindow.top,rcWindow.Width(),rcWindow.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW ) ;
#else
		SetWindowPos(m_hOwnerWnd?HWND_TOP:HWND_TOPMOST , rcWindow.left,rcWindow.top,rcWindow.Width(),rcWindow.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW ) ;
		CClientDC dc(m_hWnd);
		UpdateWindow(dc.m_hDC) ;
#endif
		
		if(m_sttfCurr.uFlags&eTTF_BALLOON)
		{
			if ( !(m_sttfCurr.uFlags&eTTF_FADEIN) )
			{
				DWORD dwTimeOut = 5000 ;
				if ( m_sttfCurr.uFlags&eTTF_SHOWTIMEOUT ) dwTimeOut = m_sttfCurr.uShowTimeOut ;
				SetTimer(eTimer_BalloonHide , dwTimeOut ) ;
			}
			AddWndToTipList(m_hWnd) ;
			Hook();
		}
	}
	else
		PrepareHide(false) ;

}

bool CSogouToolTip::GetDetail(const SOGOUTOOLINFO& TTInfo, const LPTOOLRECT lpToolRect)
{
	m_sttfCurr = TTInfo ;
	UINT uFlags = m_sttfCurr.uFlags ;
	if ( NULL != lpToolRect )
	{
		m_TRectCurr = *lpToolRect ;
		uFlags = ((m_sttfCurr.uFlags&~eTTF_TEXTCALLBACK)&~eTTF_ICON)|(lpToolRect->uFlags&(eTTF_TEXTCALLBACK|eTTF_ICON)) ;
	}

	if ( uFlags&eTTF_TEXTCALLBACK )
	{
		NMSTTDISPINFO nmdsp = {{m_hWnd,ID_SOGOU_TOOLTIP,TTN_GETDISPINFO},0,NULL, NULL==lpToolRect?m_sttfCurr.IconSize:lpToolRect->IconSize , NULL==lpToolRect?m_sttfCurr.pdwIndex:lpToolRect->pdwIndex , _T("") , _T("")};
		if( S_OK!=::SendMessage(TTInfo.hWnd , WM_NOTIFY , NULL , (LPARAM)&nmdsp) )
			return false;
		if(!ParseDispinfo(m_sttfCurr , nmdsp))
			return false;
	}
	else if ( NULL!=lpToolRect )
	{	// 在这里，区域不为空
		m_sttfCurr.hIcon = lpToolRect->hIcon ;
		m_sttfCurr.IconSize = lpToolRect->IconSize ;
		m_sttfCurr.pdwIndex = lpToolRect->pdwIndex ;
		m_sttfCurr.wstrText = lpToolRect->wstrText ;
		m_sttfCurr.wstrTitle = lpToolRect->wstrTitle ;
	}

	// 如果内容是空就不显示
	if ( m_sttfCurr.wstrText.empty() )
		return false ;

	m_sttfCurr.wstrTitle = m_sttfCurr.wstrTitle.size()==0?L"搜狗提示":m_sttfCurr.wstrTitle ;

	if ( (m_sttfCurr.uFlags&eTTF_ICON) && IsInnerIcon(m_sttfCurr.hIcon))
		m_sttfCurr.IconSize = s()->Tooltip()->GetIconSizeByIconType(m_sttfCurr.hIcon);

	return true ;
}

BOOL CSogouToolTip::ParseDispinfo(CSogouToolTipInfo& stti , NMSTTDISPINFO dsp )
{
	if ( dsp.dwFlags == 0 )
		return FALSE ;

	if ( (stti.uFlags&eTTF_BALLOON) || !dsp.szText.empty() )
	{

		if ( dsp.dwFlags&STTDSPF_INDEX )
		{
			stti.pdwIndex = dsp.pdwIndex ;
		}

		if ( dsp.dwFlags&STTDSPF_TEXT )
		{
			stti.wstrText = TranslateReturnInText(dsp.szText) ;
		}
		else
			stti.wstrText = _T(" ") ;

		if ( dsp.dwFlags&STTDSPF_TITLE )
		{
			stti.wstrTitle = dsp.szTitle ;
			stti.uFlags |= eTTF_TITLE ;
			if ( dsp.dwFlags&STTDSPF_ICON )
			{
				stti.hIcon = dsp.hIcon ;
				stti.IconSize = dsp.IconSize ;
				stti.uFlags |= eTTF_ICON ;
			}
			else
				stti.uFlags &= ~eTTF_ICON ;
		}
		else
		{
			stti.uFlags &= ~eTTF_TITLE ;
			stti.uFlags &= ~eTTF_ICON ;
		}

		return TRUE ;
	}
	
	return FALSE ;
}

void CSogouToolTip::PrepareHide(bool bUserClose/* = false*/) 
{
	if ( m_sttfCurr.uFlags&eTTF_CLOSE 
		&& m_sttfCurr.uFlags&eTTF_BALLOON 
		&& m_sttfCurr.uFlags&eTTF_USERCLOSENOTIFY 
		&& m_sttfCurr.uCallBackMsg >=WM_USER 
		&& bUserClose )
	{
		::PostMessage ( m_sttfCurr.hWnd , m_sttfCurr.uCallBackMsg , TTCBMT_USERCLOSE , NULL ) ;
	}

	PostMessage(MSG_SOGOUTT_PREPARE_HIDE , bUserClose?TRUE:FALSE);
}

void CSogouToolTip::Hide(bool bNotifyUser/* = false*/)
{
	if(IsWindow() && IsWindowVisible())
		SendMessage(MSG_SOGOUTT_HIDE , bNotifyUser?TRUE:FALSE , NULL ) ;
}

void CSogouToolTip::ProcessFadeOut(int iStep , UINT uEla)
{
	if(IsWindow() && IsWindowVisible()&& !m_TTFade.IsVaild() )
	{

		m_TTFade.SetVerb(true) ;
		m_TTFade.SetCurrPercent(100);
		m_TTFade.SetSetp(iStep) ;
		m_TTFade.SetValid() ;
		SetTimer(eTimer_FadeInOut , uEla) ;
	}

}


BOOL CSogouToolTip::AddTool(const SOGOUTOOLINFO& sttf) 
{
	ATLASSERT(NULL!=sttf.hWnd) ;
	TTInfoVec::iterator iter = find(m_ToolInfo.begin() , m_ToolInfo.end() , sttf.hWnd) ;
	ATLASSERT( m_ToolInfo.end() == iter ) ;
	if ( m_ToolInfo.end() != iter )
		return FALSE ;

	m_ToolInfo.push_back(sttf) ;

	return TRUE ;
}

BOOL CSogouToolTip::AddToolRect(const TOOLRECT& TRect)
{
	// 首先查看是否已加入这个窗口的ToolTip
	TTInfoVec::iterator iter = find(m_ToolInfo.begin() , m_ToolInfo.end() , TRect.hWnd) ;
	ATLASSERT( m_ToolInfo.end() != iter ) ;
	if ( m_ToolInfo.end() == iter )
		return FALSE ;

	m_ToolRectInfo[TRect.hWnd].push_back(CSogouToolRect(TRect)) ;

	return TRUE ;
}

void CSogouToolTip::CalcItemRect(CSize size) 
{
	CalcItemRect( CRect(LFRAME , TFRAME , size.cx+RFRAME , size.cy+BFRAME) ) ;
}

void CSogouToolTip::CalcItemRect(CRect rcClient) 
{

	if ( m_sttfCurr.uFlags&eTTF_BALLOON )
		m_rcCloseBtn = CRect(rcClient.right-RIndt()-RFRAME-4 , TFRAME+TIndt()-2 , rcClient.right-RIndt()-RFRAME+12 , TFRAME+TIndt()+14) ;

	m_dwCloseBtnState = eMouseOut ;
	if ( m_sttfCurr.uFlags&eTTF_TITLE )
	{
		int iIconOffset = m_sttfCurr.uFlags&eTTF_ICON?TEXTPICSPACING+m_sttfCurr.IconSize.cx:0 ;
		m_ptIcon = CPoint(rcClient.left+LIndt()+LFRAME , TFRAME+TIndt()) ;

		if( (m_sttfCurr.uFlags&eTTF_BALLOON) && (m_sttfCurr.uFlags&eTTF_CLOSE) )
			m_rcTitle = CRect( rcClient.left+LIndt()+LFRAME+iIconOffset , TFRAME+TIndt() , rcClient.right-RIndt()-RFRAME-TEXTPICSPACING-16 , m_iTitleHeight+TIndt()+TFRAME ) ;
		else
			m_rcTitle = CRect( rcClient.left+LIndt()+LFRAME+iIconOffset , TFRAME+TIndt() , rcClient.right-RIndt()-RFRAME , m_iTitleHeight+TIndt()+TFRAME ) ;

		m_rcContent = CRect( rcClient.left+LIndt()+LFRAME+iIconOffset , m_rcTitle.Height()+TFRAME+TIndt()+RowSpac() , rcClient.right-RIndt()-RFRAME , rcClient.bottom-BIndt()-BFRAME ) ;
	}
	else
		m_rcContent = CRect( rcClient.left+LIndt()+LFRAME , TFRAME+TIndt() , rcClient.right-RIndt()-RFRAME , /*m_iTextHeight+TIndt()+TFRAME*/rcClient.bottom-BIndt()-BFRAME ) ;

	if ( m_sttfCurr.uFlags&eTTF_BALLOON && m_sttfCurr.uFlags&eTTF_CHECKBOX )
	{
		m_rcCheckBox = CRect ( m_rcContent.right-m_rcCheckBoxText.Width()-30 , m_rcContent.bottom-11 , m_rcContent.right-m_rcCheckBoxText.Width()-14 , m_rcContent.bottom+5  ) ;
		m_rcCheckBoxText = CRect ( m_rcContent.right-m_rcCheckBoxText.Width()-10 , m_rcContent.bottom-11 , m_rcContent.right-10 , m_rcContent.bottom+5 ) ;
	}
	
}

bool CSogouToolTip::SetText(std::wstring wsText)
{
	if ( !IsWindow() || !IsWindowVisible() )
		return false ;
	m_sttfCurr.wstrText = wsText ;
	CPoint pt=m_ptTipPos ;
	Show(m_sttfCurr , &pt) ;
	return true ;
}

void CSogouToolTip::UpdateWindow(HDC hdc) 
{
#if defined(_SOGOU_TOOLTIP_LAYERED)
	delete m_pLayerMemDC ;
	m_pLayerMemDC = NULL ;
	m_pLayerMemDC = new CLayeredMemDC ;
	m_pLayerMemDC->CreateMemDC(m_rcClient.Width() , m_rcClient.Height() ) ;
	
	CDCHandle dc(m_pLayerMemDC->m_hDC) ;
#else
	CDCHandle dc(hdc) ;
#endif

	if ( m_sttfCurr.uFlags&eTTF_BALLOON )
	{
		m_bFadeOutCanStop = true ;
		dc.SetTextColor(RGB(00,67,167)) ;
	}
	else
	{
		dc.SetTextColor(RGB(0,0,0)) ;
	}

	const wchar_t* wchTitle = m_sttfCurr.wstrTitle.c_str() ;
	const wchar_t* wchText = m_sttfCurr.wstrText.c_str() ;
	dc.SetBkMode (TRANSPARENT) ;
	dc.SelectFont (s()->Common()->GetDefaultFont()) ;

	if(m_sttfCurr.uFlags&eTTF_BALLOON)
		s()->Tooltip()->DrawBalloonBack(dc , m_rcClient , m_iForwardOfTriangle) ;
	else
		s()->Tooltip()->DrawBalloonBack(dc , m_rcClient , 0) ;

	std::list<RECT> rcList ;
	
	if ( m_sttfCurr.uFlags&eTTF_TITLE )
	{
		if ( m_sttfCurr.uFlags&eTTF_ICON )
		{
			s()->Tooltip()->DrawIcon(dc , m_sttfCurr.hIcon , CRect(m_ptIcon.x , m_ptIcon.y , m_ptIcon.x+m_sttfCurr.IconSize.cx , m_ptIcon.y+m_sttfCurr.IconSize.cy)) ;
			rcList.push_back(CRect(m_ptIcon.x , m_ptIcon.y , m_ptIcon.x+m_sttfCurr.IconSize.cx , m_ptIcon.y+m_sttfCurr.IconSize.cy)) ;
		}
		if ( m_sttfCurr.uFlags&eTTF_BALLOON )
			dc.SelectFont (s()->Tooltip()->GetTitleFont()) ;
		else
			dc.SelectFont(s()->Common()->GetDefaultFont()) ;

		dc.DrawText ( wchTitle , wcslen(wchTitle) , &m_rcTitle , DT_VCENTER | DT_SINGLELINE|DT_NOPREFIX|DT_WORD_ELLIPSIS ) ;
		rcList.push_back(m_rcTitle) ;
		if ( (m_sttfCurr.uFlags&eTTF_BALLOON) && (m_sttfCurr.uFlags&eTTF_CLOSE) )
			s()->Tooltip()->DrawCloseBtn(dc , m_rcCloseBtn , m_dwCloseBtnState) ;
	}

	if ( m_sttfCurr.uFlags&eTTF_BALLOON )
	{
		dc.SelectFont (s()->Tooltip()->GetTextFont ()) ;
		m_tooltipText.RanderText(dc , m_rcContent) ;
		if ( m_sttfCurr.uFlags&eTTF_CHECKBOX )
		{
			m_dwCheckState = m_sttfCurr.dwCheckBoxState;
			s()->Tooltip()->DrawCheckBox(dc , m_rcCheckBox , m_dwCheckState) ;
			dc.DrawText(m_sttfCurr.wstrCheckBox.c_str() , m_sttfCurr.wstrCheckBox.size() , &m_rcCheckBoxText , DT_NOPREFIX|DT_WORD_ELLIPSIS) ;
			rcList.push_back(m_rcCheckBox) ;
			rcList.push_back(m_rcCheckBoxText) ;
		}
	}
	else
	{
		dc.SelectFont(s()->Common()->GetDefaultFont()) ;
		m_tooltipText.RanderText(dc , m_rcContent , 1) ;	
	}
	rcList.push_back(m_rcContent) ;

#if defined(_SOGOU_TOOLTIP_LAYERED)
	EnsureVisiable(m_pLayerMemDC ,rcList );
	int iAlph = m_TTFade.IsVaild()?m_TTFade.FirstStep()*255/100:255;
	UpdateLayeredWindow(dc,iAlph) ;
#endif

}

void CSogouToolTip::UpdateLayeredWindow(CDCHandle dc , int iAlph/* = 255*/)
{
	SIZE szWin = { m_rcClient.Width() , m_rcClient.Height() };
	POINT ptSrc = { 0, 0 };
	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, iAlph, AC_SRC_ALPHA };
	::UpdateLayeredWindow(m_hWnd, NULL, NULL, &szWin, dc, &ptSrc, 0, &stBlend, ULW_ALPHA);
}

void CSogouToolTip::EnsureVisiable(CLayeredMemDC* pMemDC , std::list<RECT> rcList)
{
	ATLASSERT(NULL!=pMemDC) ;
	CBitmapHandle hBmp = pMemDC->GetCurrentBitmap() ;
	SIZE size ;
	hBmp.GetSize(size) ;
	int iPixelCount = size.cx*size.cy ;
	int iBufSize = iPixelCount*sizeof(QUADPIXEL) ;
	QUADPIXEL* pPixels = new QUADPIXEL[iPixelCount] ;
	hBmp.GetBitmapBits(iBufSize , pPixels) ;

	std::list<RECT>::iterator iter = rcList.begin();

	for ( iter;iter!=rcList.end();iter++ )
	{
		for ( int iRow = size.cy-iter->top; iRow>size.cy-iter->bottom; iRow-- )
		{
			for ( int iCol = iter->left ; iCol<iter->right ; iCol++ )
			{
				pPixels[(size.cy-iRow)*size.cx + iCol].alpha=0xFF;
			}
		}
	}

	if ( m_sttfCurr.uFlags&eTTF_BALLOON && m_sttfCurr.uFlags&eTTF_FADEIN )
	{
		m_TTFade.SetVerb(false) ;
		m_TTFade.SetCurrPercent(0) ;
		m_TTFade.SetSetp(10) ;
		m_TTFade.SetValid() ;
		SetTimer ( eTimer_FadeInOut , 20 ) ;
	}
	else if ( !(m_sttfCurr.uFlags&eTTF_BALLOON) )
	{
		m_TTFade.SetVerb(false) ;
		m_TTFade.SetCurrPercent(0) ;
		m_TTFade.SetSetp(30) ;
		m_TTFade.SetValid() ;
		SetTimer ( eTimer_FadeInOut , 10 ) ;
	}

	hBmp.SetBitmapBits(iBufSize , pPixels) ;

	delete []pPixels ;
}


void CSogouToolTip::EnsureRectVisiable(CDCHandle dc , RECT rc)
{
	ATLASSERT(NULL!=dc.m_hDC) ;
	CBitmapHandle hBmp = dc.GetCurrentBitmap() ;
	SIZE size ;
	hBmp.GetSize(size) ;
	int iPixelCount = size.cx*size.cy ;
	int iBufSize = iPixelCount*sizeof(QUADPIXEL) ;
	QUADPIXEL* pPixels = new QUADPIXEL[iPixelCount] ;
	hBmp.GetBitmapBits(iBufSize , pPixels) ;

	for ( int iRow = size.cy-rc.top; iRow>size.cy-rc.bottom; iRow-- )
	{
		for ( int iCol = rc.left ; iCol<rc.right ; iCol++ )
			pPixels[(size.cy-iRow)*size.cx + iCol].alpha=0xFF;
	}

	hBmp.SetBitmapBits(iBufSize , pPixels) ;

	delete []pPixels ;
}

void CSogouToolTip::OnDestroy() 
{
	DestroyWindow() ;
	m_hWnd = NULL ;
}

#ifndef _SOGOU_TOOLTIP_LAYERED 

BOOL CSogouToolTip::OnEraseBkgnd(CDCHandle dc) 
{
	RECT rc;
	GetClientRect(&rc) ;
	dc.FillSolidRect(&rc,0xFFFFFF);
	return TRUE ;
}

void CSogouToolTip::OnPaint(CDCHandle dc, RECT rect)
{
	UpdateWindow(dc);
}

#endif

void CSogouToolTip::OnTimer(UINT_PTR nIDEvent) 
{
	switch (nIDEvent)
	{
	case eTimer_BalloonHide:
		KillTimer(eTimer_BalloonHide) ;
		PrepareHide() ;
		break;
	case eTimer_ToolTipDelayShow:
		{
			POINT ptCursor ;
			::GetCursorPos(&ptCursor) ;
			HWND hWndOnCursor = ::WindowFromPoint(ptCursor) ;
			if ( m_pDelayShowToolInfo->hWnd == hWndOnCursor) 
			{
				Show(hWndOnCursor , &ptCursor) ;
			}
			KillTimer(eTimer_ToolTipDelayShow) ;

			if ( !(m_pDelayShowToolInfo->uFlags&eTTF_BALLOON) )
				SetTimer(eTimer_MouseLeave , 50) ;

		}
		break ;
	case eTimer_MouseLeave:
		{
			POINT pt ;
			::GetCursorPos(&pt) ;
			HWND hWndOnCursor = WindowFromPoint(pt) ;
			if ( hWndOnCursor!=m_sttfCurr.hWnd )
			{
				if ( hWndOnCursor!=m_hWnd )
					m_sttfCurr.hWnd = NULL ;
				m_bFadeOutCanStop = true ;
				PrepareHide() ;
			}

			if ( !(GetStyle()&WS_VISIBLE) )
			{
				KillTimer(eTimer_MouseLeave) ;
			}
		}
		break ;
	case eTimer_FadeInOut:
		{
			if ( !m_TTFade.IsVaild() || NULL == m_pLayerMemDC )
				KillTimer(eTimer_FadeInOut) ;
			else
			{
				int iAlph = m_TTFade.NextStep()*255/100;
				UpdateLayeredWindow(*m_pLayerMemDC , iAlph);
				if ( m_TTFade.IsCompleted() )
				{
					if ( m_TTFade.IsFadeOut() )
					{
						Hide();
					}
					else if ( m_sttfCurr.uFlags&eTTF_BALLOON )
					{
						DWORD dwTimeOut = 5000 ;
						if ( m_sttfCurr.uFlags&eTTF_SHOWTIMEOUT ) dwTimeOut = m_sttfCurr.uShowTimeOut ;
						SetTimer(eTimer_BalloonHide , dwTimeOut ) ;
					}

					m_TTFade.SetValid(false) ;
					KillTimer(eTimer_FadeInOut) ;
				}
			}
		}
		break ;
	default:
		DefWindowProc() ;
	}
}

LRESULT CSogouToolTip::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	return S_OK;
}

int CSogouToolTip::OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message)
{
	return MA_NOACTIVATE;
}

void CSogouToolTip::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( !(m_sttfCurr.uFlags&eTTF_BALLOON)||!(m_sttfCurr.uFlags&eTTF_CLOSE) || !(m_sttfCurr.uFlags&eTTF_TITLE) )
		return ;

	m_CursorState = eCURSOR_DEFAULT ;
	BOOL bLBtnDown = GetAsyncKeyState(VK_LBUTTON) ;

	if ( m_TTFade.IsVaild() )
	{
		if (  m_bFadeOutCanStop  )
		{
			KillTimer(eTimer_FadeInOut) ;
			m_TTFade.SetValid(false) ;

		}else
		{
			return ;
		}
	}

	CRect rcClient ;
	GetClientRect(&rcClient) ;
	rcClient.DeflateRect(LFRAME , TFRAME , RFRAME , BFRAME) ;
	if ( rcClient.PtInRect(point) )
	{
		KillTimer(eTimer_BalloonHide) ;
	}

	if ( m_rcCloseBtn.PtInRect(point) )
	{
		if( bLBtnDown )
			m_dwCloseBtnState = eMouseDown ;
		else
			m_dwCloseBtnState = eMouseOver ;
	}
	else
		m_dwCloseBtnState = eMouseOut ;

#if defined(_SOGOU_TOOLTIP_LAYERED)
	CDCHandle dc(m_pLayerMemDC->m_hDC);
#else
	CWindowDC dc(m_hWnd) ;
#endif

	// 画checkbox
	if ( m_sttfCurr.uFlags&eTTF_CHECKBOX )
	{
		if ( m_rcCheckBox.PtInRect(point) || m_rcCheckBoxText.PtInRect(point) )
		{
			switch(m_dwCheckState)
			{
			case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:
				if ( m_bLBtnDown )
					m_dwCheckState = eCBS_CheckedActive ;
				else 
					m_dwCheckState = eCBS_CheckedHover ;
				break;
			case eCBS_UnChecked:case eCBS_UnCheckedActive:case eCBS_UnCheckedHover:
				if ( m_bLBtnDown )
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
		s()->Tooltip()->DrawCheckBox(dc , m_rcCheckBox , m_dwCheckState) ;
	}

	// 画link
	if ( !m_tooltipText.GetLinkList().empty() )
	{
		list<LINKBLOCK>::iterator it = m_tooltipText.GetLinkList().begin() ;
		CFont font;
		font.CreateFont(13,0,0,0,it->bBold?FW_BOLD:FW_NORMAL,0,TRUE,0,0,0,0,0,0,_T("Tahoma")) ;
		HFONT hOldFont = dc.SelectFont(font) ;
		COLORREF clrOld = dc.GetTextColor() ;

		for(;it!=m_tooltipText.GetLinkList().end();it++)
		{
			if ( ::PtInRect(&it->rcBlock , point) )
			{
				if ( bLBtnDown )
				{
					if ( m_pCurLink == &(*it) )
						dc.SetTextColor(it->clrActive) ;
					else
						dc.SetTextColor(it->clrHover) ;
				}
				else
					dc.SetTextColor(it->clrHover) ;
				SetCursor(m_hCursorHand) ;
				m_CursorState = eCURSOR_HAND;
			}
			else
			{
				dc.SetTextColor(it->clrNormal) ;
				if(!bLBtnDown )
					m_pCurLink = NULL ;
			}

			dc.DrawText(it->wsText.c_str() , it->wsText.size() , &it->rcBlock , DT_NOCLIP|DT_NOPREFIX |DT_SINGLELINE|DT_WORD_ELLIPSIS) ;
			EnsureRectVisiable(dc , it->rcBlock) ;	
		}
		dc.SetTextColor(clrOld) ;
		dc.SelectFont(hOldFont) ;
	}

	s()->Tooltip()->DrawCloseBtn(dc , m_rcCloseBtn , m_dwCloseBtnState) ;
#if defined(_SOGOU_TOOLTIP_LAYERED)
	int iAlph = m_TTFade.IsVaild()?m_TTFade.GetStep()*255/100:255;
	UpdateLayeredWindow(*m_pLayerMemDC,iAlph) ;
#endif

	TRACKMOUSEEVENT tkme = {sizeof(TRACKMOUSEEVENT) , TME_LEAVE , m_hWnd , 0};
	TrackMouseEvent(&tkme) ;
}

void CSogouToolTip::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( !(m_sttfCurr.uFlags&eTTF_BALLOON)||!(m_sttfCurr.uFlags&eTTF_CLOSE) || !(m_sttfCurr.uFlags&eTTF_TITLE) )
		return ;

#if defined(_SOGOU_TOOLTIP_LAYERED)
	CDCHandle dc(m_pLayerMemDC->m_hDC);
#else
	CWindowDC dc(m_hWnd) ;
#endif

	bool bUpdate = false ;
	m_CursorState = eCURSOR_DEFAULT ;

	if ( m_rcCloseBtn.PtInRect(point) )
	{
		m_dwCloseBtnState = eMouseDown ;
		bUpdate = true ;
		s()->Tooltip()->DrawCloseBtn(dc , m_rcCloseBtn , m_dwCloseBtnState) ;
	}

	if ( m_sttfCurr.uFlags&eTTF_CHECKBOX )
	{
		if ( m_rcCheckBox.PtInRect(point)|m_rcCheckBoxText.PtInRect(point) )
		{
			switch(m_dwCheckState)
			{
			case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:
				m_dwCheckState = eCBS_CheckedActive ;
				break;
			case eCBS_UnChecked:case eCBS_UnCheckedActive:case eCBS_UnCheckedHover:
				m_dwCheckState = eCBS_UnCheckedActive ;
				break;
			default:
				m_dwCheckState;
				break;
			}
			bUpdate = true ;
			s()->Tooltip()->DrawCheckBox(*m_pLayerMemDC , m_rcCheckBox , m_dwCheckState) ;

			if(m_sttfCurr.uCallBackMsg >=WM_USER && m_sttfCurr.uFlags&eTTF_CHECKBOXTAKEEFFECTNOW)
				::PostMessage(m_sttfCurr.hWnd , m_sttfCurr.uCallBackMsg , TTCBMT_CHECKBOXSTATE , (WPARAM)m_dwCheckState==eCBS_CheckedHover ) ;
		}
	}

	if ( !m_tooltipText.GetLinkList().empty() )
	{
		list<LINKBLOCK>::iterator it = m_tooltipText.GetLinkList().begin() ;
		CFont font;
		font.CreateFont(13,0,0,0,it->bBold?FW_BOLD:FW_NORMAL,0,TRUE,0,0,0,0,0,0,_T("Tahoma")) ;
		HFONT hOldFont = dc.SelectFont(font) ;
		COLORREF clrOld = dc.GetTextColor() ;

		for(;it!=m_tooltipText.GetLinkList().end();it++)
		{
			if ( ::PtInRect(&it->rcBlock , point) )
			{
				m_pCurLink = &(*it) ;
				dc.SetTextColor(it->clrActive) ;
				SetCursor(m_hCursorHand) ;
				m_CursorState = eCURSOR_HAND;
			}
			else
				dc.SetTextColor(it->clrNormal) ;

			dc.DrawText(it->wsText.c_str() , it->wsText.size() , &it->rcBlock , DT_NOCLIP|DT_NOPREFIX |DT_SINGLELINE|DT_WORD_ELLIPSIS) ;
			EnsureRectVisiable(dc , it->rcBlock) ;	
		}
		dc.SetTextColor(clrOld) ;
		dc.SelectFont(hOldFont) ;
		bUpdate = true ;
	}

#if defined(_SOGOU_TOOLTIP_LAYERED)
	if(bUpdate)
	{
		int iAlph = m_TTFade.IsVaild()?m_TTFade.GetStep()*255/100:255;
		UpdateLayeredWindow(*m_pLayerMemDC , iAlph) ;
	}
#endif

	m_bLBtnDown = TRUE ;
}

void CSogouToolTip::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( !(m_sttfCurr.uFlags&eTTF_BALLOON)||!(m_sttfCurr.uFlags&eTTF_CLOSE) || !(m_sttfCurr.uFlags&eTTF_TITLE) )
		return ;

	if ( !m_bLBtnDown )
		return ; 

#if defined(_SOGOU_TOOLTIP_LAYERED)
	CDCHandle dc(m_pLayerMemDC->m_hDC);
#else
	CWindowDC dc(m_hWnd) ;
#endif

	m_bLBtnDown = FALSE ;
	if ( m_rcCloseBtn.PtInRect(point) )
	{
		m_dwCloseBtnState = eMouseOver ;
		m_bFadeOutCanStop = false ;
		PrepareHide( true ) ;
	}
	else
		m_dwCloseBtnState = eMouseOut ;
	s()->Tooltip()->DrawCloseBtn(dc , m_rcCloseBtn , m_dwCloseBtnState) ;
	
	if ( m_sttfCurr.uFlags&eTTF_CHECKBOX )
	{
		if ( m_rcCheckBox.PtInRect(point) || m_rcCheckBoxText.PtInRect(point) )
		{
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
			s()->Tooltip()->DrawCheckBox(dc , m_rcCheckBox , m_dwCheckState) ;

			if(m_sttfCurr.uCallBackMsg >=WM_USER && m_sttfCurr.uFlags&eTTF_CHECKBOXTAKEEFFECTNOW)
				::PostMessage(m_sttfCurr.hWnd , m_sttfCurr.uCallBackMsg , TTCBMT_CHECKBOXSTATE , (WPARAM)m_dwCheckState==eCBS_CheckedHover ) ;

		}

	}

	if ( !m_tooltipText.GetLinkList().empty() )
	{
		list<LINKBLOCK>::iterator it = m_tooltipText.GetLinkList().begin() ;
		CFont font;
		font.CreateFont(13,0,0,0,it->bBold?FW_BOLD:FW_NORMAL,0,TRUE,0,0,0,0,0,0,_T("Tahoma")) ;
		HFONT hOldFont = dc.SelectFont(font) ;
		COLORREF clrOld = dc.GetTextColor() ;

		for(;it!=m_tooltipText.GetLinkList().end();it++)
		{
			if ( ::PtInRect(&it->rcBlock , point) )
			{
				m_pCurLink = &(*it) ;
				dc.SetTextColor(it->clrHover) ;
				SetCursor(m_hCursorHand) ;
				m_CursorState = eCURSOR_HAND;

				// notify caller
				if ( m_sttfCurr.uFlags&eTTF_BALLOON && m_sttfCurr.uCallBackMsg >=WM_USER )
				{
					::PostMessage ( m_sttfCurr.hWnd , m_sttfCurr.uCallBackMsg , TTCBMT_LINK , (WPARAM)it->iId ) ;
					Hide(false) ;
				}
			}
			else
				dc.SetTextColor(it->clrNormal) ;

			dc.DrawText(it->wsText.c_str() , it->wsText.size() , &it->rcBlock , DT_NOCLIP|DT_NOPREFIX |DT_SINGLELINE|DT_WORD_ELLIPSIS) ;
			EnsureRectVisiable(dc , it->rcBlock) ;	
		}
		dc.SetTextColor(clrOld) ;
		dc.SelectFont(hOldFont) ;
	}

#if defined(_SOGOU_TOOLTIP_LAYERED)
	int iAlph = m_TTFade.IsVaild()?m_TTFade.GetStep()*255/100:255;
	UpdateLayeredWindow(*m_pLayerMemDC , iAlph) ;
#endif

}

void CSogouToolTip::OnLButtonDblClk(UINT nFlags , CPoint point)
{
#if defined(_SOGOU_TOOLTIP_LAYERED)
	CDCHandle dc(*m_pLayerMemDC) ;
#else
	CWindowDC dc(m_hWnd) ;
#endif

	m_bLBtnDown = TRUE ;

	if ( m_sttfCurr.uFlags&eTTF_BALLOON&&m_sttfCurr.uFlags&eTTF_CHECKBOX )
	{

		if ( m_rcCheckBox.PtInRect(point) )
		{
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
			s()->Tooltip()->DrawCheckBox(dc , m_rcCheckBox , m_dwCheckState) ;
		}
	}

	if ( !m_tooltipText.GetLinkList().empty() )
	{
		list<LINKBLOCK>::iterator it = m_tooltipText.GetLinkList().begin() ;
		CFont font;
		font.CreateFont(13,0,0,0,it->bBold?FW_BOLD:FW_NORMAL,0,TRUE,0,0,0,0,0,0,_T("Tahoma")) ;
		HFONT hOldFont = dc.SelectFont(font) ;
		COLORREF clrOld = dc.GetTextColor() ;

		for(;it!=m_tooltipText.GetLinkList().end();it++)
		{
			if ( ::PtInRect(&it->rcBlock , point) )
			{
				m_pCurLink = &(*it) ;
				dc.SetTextColor(it->clrActive) ;
				SetCursor(m_hCursorHand) ;
				m_CursorState = eCURSOR_HAND;
			}
			else
				dc.SetTextColor(it->clrNormal) ;

			dc.DrawText(it->wsText.c_str() , it->wsText.size() , &it->rcBlock , DT_NOCLIP|DT_NOPREFIX |DT_SINGLELINE|DT_WORD_ELLIPSIS) ;
			EnsureRectVisiable(dc , it->rcBlock) ;	
		}
		dc.SetTextColor(clrOld) ;
		dc.SelectFont(hOldFont) ;
	}

#if defined(_SOGOU_TOOLTIP_LAYERED)
	int iAlph = m_TTFade.IsVaild()?m_TTFade.GetStep()*255/100:255;
	UpdateLayeredWindow(*m_pLayerMemDC , iAlph) ;
#endif

}

void CSogouToolTip::OnMouseLeave()
{
	if ( m_sttfCurr.uFlags&eTTF_BALLOON )
	{
		DWORD dwTimeOut = 3000 ;
		if ( m_sttfCurr.uFlags&eTTF_SHOWTIMEOUT )
			dwTimeOut = m_sttfCurr.uShowTimeOut ;
		SetTimer(eTimer_BalloonHide , dwTimeOut ) ;
	}
}

BOOL CSogouToolTip::OnSetCursor(CWindow wnd, UINT nHitTest, UINT message)
{
	switch(m_CursorState)
	{
	case eCURSOR_HAND:
		SetCursor(m_hCursorHand) ;
		break;
	default:
		DefWindowProc() ;
	}

	return TRUE ;
}

LRESULT CSogouToolTip::OnCheckHide(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( m_sttfCurr.uFlags&eTTF_BALLOON && !(m_sttfCurr.uFlags&eTTF_ADVANCE) )
	{
		PostMessage(MSG_SOGOUTT_HIDE , FALSE , NULL) ;
	}
	return 0;
}

LRESULT CSogouToolTip::OnHide(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	static const RECT rcNULL = {0,0,0,0} ;
//	m_rcVaildCursorPos = rcNULL ;

	if(IsWindowVisible())
	{
		if(m_sttfCurr.uFlags&eTTF_BALLOON)
			RemoveWndFromTipList(m_hWnd) ;
		m_sttfCurr.pdwIndex = NULL ;
		if ( m_sttfCurr.uFlags&eTTF_BALLOON && m_sttfCurr.uFlags&eTTF_CHECKBOX )
		{
			if ( m_sttfCurr.uCallBackMsg >=WM_USER )
			{
				BOOL bChecked = FALSE ;
				switch(m_dwCheckState)
				{
				case eCBS_Checked:case eCBS_CheckedHover:case eCBS_CheckedActive:case eCBS_CheckedDisable:
					bChecked = TRUE ;
					break;
				default:
					bChecked = FALSE ;
					break ;
				}
				::PostMessage(m_sttfCurr.hWnd , m_sttfCurr.uCallBackMsg , TTCBMT_CHECKBOXSTATE , (WPARAM)bChecked ) ;
			}
		}
		
		if ( m_sttfCurr.uFlags&eTTF_BALLOON && m_sttfCurr.uFlags&eTTF_HIDDENNOTIFY && m_sttfCurr.uCallBackMsg >=WM_USER )
		{
			::PostMessage ( m_sttfCurr.hWnd , m_sttfCurr.uCallBackMsg , TTCBMT_HIDDEN , NULL ) ;
		}

		BOOL bRst = ShowWindow(SW_HIDE) ;
		ATLTRACE(_T("tooltip:hide:%s\n"),bRst?_T("succ"):_T("failed")) ;
		UnHook() ;
	}
	return S_OK ;
}

LRESULT CSogouToolTip::OnPrepareHide(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bUserClose = wParam?true:false;
#if defined (_SOGOU_TOOLTIP_LAYERED)
	KillTimer(eTimer_ToolTipDelayShow) ;
	if ( m_sttfCurr.uFlags&eTTF_BALLOON  )
		ProcessFadeOut(bUserClose?10:3 , 20) ;
	else
		ProcessFadeOut(40 , 10) ;
#else
	Hide(bUserClose&&m_sttfCurr.uFlags&eTTF_USERCLOSENOTIFY) ;
#endif
	return S_OK;
}

BOOL CSogouToolTip::SetOwnerWindow(HWND hWnd)
{
	if ( NULL!=m_hWnd || NULL == hWnd )  // 如果窗口已创建，则owner不能被改变了
		return FALSE ;
	m_hOwnerWnd = hWnd ;
	return TRUE ;
}

BOOL CSogouToolTip::CreateToolTipWindow() 
{
	ATLASSERT(NULL==m_hWnd) ;
	Create ( m_hOwnerWnd , &rcDefaultTip , _T("MH_ToolTip") , m_dwStyles , m_dwStylesEx|(NULL==m_hOwnerWnd?WS_EX_TRANSPARENT | WS_EX_TOPMOST :0) , HMENU(NULL) , NULL ) ;
	SetFont(s()->Common()->GetDefaultFont()) ;
	m_hCursorHand.Attach(::LoadCursor(NULL , MAKEINTRESOURCE(IDC_HAND))) ;
	return TRUE ;
}

RECT CSogouToolTip::GetTipClientWindowRect(const SOGOUTOOLINFO& sttf) 	// 取得需要的有效窗口大小
{
	CRect rect(0,0,300,0) ;
	CWindowDC dc(m_hWnd) ;
	int iMaxWidth = s()->Tooltip()->MaxWidth() ;
	int iWidthTitle , iWidthText ;

	iWidthTitle = LIndt()+RIndt();

	RECT rc = {0,0,iMaxWidth,0};
	if ( m_sttfCurr.uFlags&eTTF_TITLE )
	{
		if ( m_sttfCurr.uFlags&eTTF_BALLOON )
			dc.SelectFont(s()->Tooltip()->GetTitleFont()) ;
		else
			dc.SelectFont(s()->Common()->GetDefaultFont()) ;
		DrawText(dc.m_hDC , sttf.wstrTitle.c_str() , sttf.wstrTitle.size() , &rc , DT_CALCRECT|DT_NOPREFIX|DT_NOCLIP) ;
		iWidthTitle+=rc.right ;
		if ( m_sttfCurr.uFlags&eTTF_ICON )
			iWidthTitle+=m_sttfCurr.IconSize.cx+TEXTPICSPACING;
		if ( m_sttfCurr.uFlags&eTTF_BALLOON && m_sttfCurr.uFlags&eTTF_CLOSE )
			iWidthTitle+=16+TEXTPICSPACING ;

		m_iTitleHeight = rc.bottom ;
		rect.bottom = TIndt()+m_iTitleHeight;
	}
	else
		rect.bottom = TIndt() ;

//	CTooltipText tipText;
	m_tooltipText.SetText(m_sttfCurr.wstrText) ;

	if ( m_sttfCurr.uFlags&eTTF_BALLOON )
	{
		dc.SelectFont(s()->Tooltip()->GetTextFont()) ;
		rc = m_tooltipText.GetRect(CDCHandle(dc.m_hDC));
		if ( rc.right > iMaxWidth ) rc.right = iMaxWidth ;

		if ( m_sttfCurr.uFlags&eTTF_CHECKBOX )
		{
			rc.bottom += CHECKBOXHEIGHT ;
			m_rcCheckBoxText = CRect(0,0,iMaxWidth,0) ;
			DrawText(dc.m_hDC , sttf.wstrCheckBox.c_str() , sttf.wstrCheckBox.size() , &m_rcCheckBoxText , DT_CALCRECT|DT_NOPREFIX|DT_NOCLIP) ;
			if ( m_rcCheckBoxText.right+20 > rc.right ) rc.right = m_rcCheckBoxText.right+20 ;
		}
	}
	else
	{
		dc.SelectFont(s()->Common()->GetDefaultFont()) ;
		rc = m_tooltipText.GetRect(CDCHandle(dc.m_hDC) , 1 ) ;
		if ( rc.right > iMaxWidth ) rc.right = iMaxWidth ;
	}

	iWidthText = rc.right + LIndt()+RIndt() ;
	if ( m_sttfCurr.uFlags&eTTF_TITLE && m_sttfCurr.uFlags&eTTF_ICON )
		iWidthText += m_sttfCurr.IconSize.cx+TEXTPICSPACING ;


	rect.bottom+= rc.bottom + BIndt();
	
	if(m_sttfCurr.uFlags&eTTF_TITLE)
		rect.bottom += RowSpac();

	rect.right = iWidthTitle>iWidthText?iWidthTitle:iWidthText ;
	rect.right = rect.right>iMaxWidth?iMaxWidth:rect.right ;

	if(m_sttfCurr.uFlags&eTTF_BALLOON)
		rect.right=rect.right<125?125:rect.right;

	if ( m_sttfCurr.uFlags&eTTF_BALLOON && m_sttfCurr.uFlags&eTTF_FIXEDWIDTH )
		rect.right = m_sttfCurr.BalloonSize.cx ;
	if ( m_sttfCurr.uFlags&eTTF_BALLOON && m_sttfCurr.uFlags&eTTF_FIXEDHEIGHT )
		rect.bottom = m_sttfCurr.BalloonSize.cy ;

	rect.OffsetRect(LFRAME,TFRAME) ;
	rect.InflateRect(LFRAME,TFRAME,RFRAME,BFRAME) ;

	return rect ;
}

RECT CSogouToolTip::GetTipWindowRect(LPRECT lpClientRect , CPoint pt ) 
{
	CRect rect(lpClientRect) ;

	m_iTrigOffset = s()->Tooltip()->TriangleOffset(m_iForwardOfTriangle , (m_sttfCurr.uFlags&eTTF_BALLOON) != 0);

	if ( m_sttfCurr.uFlags&eTTF_BALLOON )
	{
		switch(m_iForwardOfTriangle)
		{
		case 1:	rect.OffsetRect( pt.x-m_iTrigOffset-LFRAME , pt.y-rect.Height()+6 ) ;
			break;
		case 2: rect.OffsetRect(pt.x-rect.Width()+m_iTrigOffset+RFRAME , pt.y-rect.Height()+6) ;
			break;
		case 3: rect.OffsetRect(pt.x-m_iTrigOffset-LFRAME , pt.y-3) ;
			break;
		case 4: rect.OffsetRect(pt.x-rect.Width()+m_iTrigOffset+RFRAME , pt.y-3) ;
			break;
		case 5: //ATLASSERT(FALSE&&_T("还未支持这个方向"));rect.OffsetRect( pt.x-2 , pt.y - m_iTrigOffset ) ;
			break;
		case 6: //ATLASSERT(FALSE&&_T("还未支持这个方向"));rect.OffsetRect(pt.x-rect.Width() , pt.y-m_iTrigOffset);
			break;
		case 7: //ATLASSERT(FALSE&&_T("还未支持这个方向"));rect.OffsetRect(pt.x , pt.y-rect.Height()+m_iTrigOffset);
			break;
		case 8: //ATLASSERT(FALSE&&_T("还未支持这个方向"));rect.OffsetRect(pt.x-rect.Width() , pt.y-rect.Height()+m_iTrigOffset); 
			break;
		}
	}
	else
	{
// 		int iXscr = ::GetSystemMetrics(SM_CXSCREEN) ;
// 		int iYsrc = ::GetSystemMetrics(SM_CYSCREEN) ;

		HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST) ;
		MONITORINFO mi = { sizeof(MONITORINFO) };
		::GetMonitorInfo(hMon, &mi);
		RECT rcMon = mi.rcWork ;

		switch(m_iForwardOfTriangle)
		{
		case 1:	
			rect.OffsetRect( pt.x-m_iTrigOffset-LFRAME , pt.y - rect.Height()+BFRAME-5) ;
			if ( rect.right-19>rcMon.right )
				rect.OffsetRect(rcMon.right-rect.right+19,0);
			break;
		case 2: 
			ATLASSERT(FALSE);
			rect.OffsetRect(pt.x-rect.Width()+m_iTrigOffset+RFRAME , pt.y-rect.Height()+BFRAME) ;
			break;
		case 3: 
			rect.OffsetRect(pt.x-m_iTrigOffset-LFRAME , pt.y+6) ;
			if ( rect.right-19>rcMon.right )
				rect.OffsetRect(rcMon.right-rect.right+19,0);
			break;
		case 4:
			ATLASSERT(FALSE);
			rect.OffsetRect(pt.x-rect.Width()+m_iTrigOffset , pt.y) ;
			break;
		case 5: //ATLASSERT(FALSE&&_T("还未支持这个方向"));rect.OffsetRect( pt.x , pt.y - m_iTrigOffset ) ;
			break;
		case 6: //ATLASSERT(FALSE&&_T("还未支持这个方向"));rect.OffsetRect(pt.x-rect.Width() , pt.y-m_iTrigOffset);
			break;
		case 7: //ATLASSERT(FALSE&&_T("还未支持这个方向"));rect.OffsetRect(pt.x , pt.y-rect.Height()+m_iTrigOffset);
			break;
		case 8: //ATLASSERT(FALSE&&_T("还未支持这个方向"));rect.OffsetRect(pt.x-rect.Width() , pt.y-rect.Height()+m_iTrigOffset); 
			break;
		}
	}
	return rect;
}

HWND CSogouToolTip::GetInsertAfterWnd(CRect& rcWindow)
{
	if ( NULL == m_hOwnerWnd )
		return HWND_TOPMOST ;
	else
	{
		RECT rc ;
		GetWindowRect(&rc) ;

// 		POINT pt[] = { {rc.left,rc.top} , {(rc.left+rc.right)/2,rc.top} , {rc.right,rc.top},
// 					   {rc.left , (rc.top+rc.bottom)/2} , {rc.right , (rc.top+rc.bottom)/2},
// 					   {rc.left , rc.bottom},{(rc.left+rc.right)/2,rc.bottom},{rc.right ,  rc.bottom}
// 					} ;
// 


		return HWND_TOP ;
	}
}

//////////////////////////////////////////////////////////////////////////
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

int CSogouToolTip::GetTriangleForward(LPRECT lpRect , CPoint pt )
{
	CRect rc(lpRect) ;
 //	int iXscr = ::GetSystemMetrics(SM_CXFULLSCREEN) ;
// 	int iYsrc = ::GetSystemMetrics(SM_CYFULLSCREEN) ;

	HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST) ;
	MONITORINFO mi = { sizeof(MONITORINFO) };
	::GetMonitorInfo(hMon, &mi);
	RECT rcMon = mi.rcWork ;

	m_iTrigOffset = s()->Tooltip()->TriangleOffset(1,(m_sttfCurr.uFlags&eTTF_BALLOON)!=0) ;
	
	if ( m_sttfCurr.uFlags&eTTF_BALLOON )
	{
		if ( m_sttfCurr.uFlags&eTTF_BALLOONPOPDOWN )
		{
			if ( rc.Width()>=rc.Height() || rc.Height()<100 )// 宽度>=高度 或 高度小于100
			{
				if (pt.y + rc.Height()-BFRAME < rcMon.bottom ) 
					if ( pt.x + rc.Width()-m_iTrigOffset-LFRAME-RFRAME < rcMon.right  ) return 3 ;	// 向左的宽度够 , 气泡向左弹
					else return 4 ;// 向左的宽度不够 , 气泡向右弹
				else// 鼠标指针的位置到屏幕顶端的高度足够高 ，方向为1或2
					if ( pt.x + rc.Width()-m_iTrigOffset-LFRAME-RFRAME < rcMon.right  ) return 1 ;// 向左的宽度够 , 气泡向左弹
					else return 2;// 向左的宽度不够 , 气泡向右弹
			}
			else//高度大于宽度
			{
				if ( pt.y + rc.Height()-m_iTrigOffset-TFRAME-BFRAME < rcMon.bottom )// 向下的高度够 ，方向为5或6
					if ( pt.x + rc.Width()-RFRAME < rcMon.right ) return 5 ;// 宽度够，向右
					else return 6 ;// 宽度不够，向左
				else// 向下的高度不够，方向为7或8
					if ( pt.x + rc.Width()-RFRAME < rcMon.right ) return 7 ;// 宽度够，向右
					else return 8 ;// 宽度不够，向左
			}
		}
		else
		{
			if ( rc.Width()>=rc.Height() || rc.Height()<100 )// 宽度>=高度 或 高度小于100
			{	
				if ( pt.y-rc.Height()+TFRAME > rcMon.top )// 鼠标指针的位置到屏幕顶端的高度足够高 ，方向为1或2
					if ( pt.x-rc.Width()+m_iTrigOffset+LFRAME-RFRAME > rcMon.left  ) return 2 ;// 向左的宽度够 , 气泡向左弹
					else return 1;// 向左的宽度不够 , 气泡向右弹
				else // 鼠标指针的位置到屏幕顶端的高度不够，方向为3或4
					if ( pt.x-rc.Width()+m_iTrigOffset+LFRAME-RFRAME > rcMon.left ) return 4 ;	// 向左的宽度够 , 气泡向左弹
					else return 3 ;// 向左的宽度不够 , 气泡向右弹
			}
			else //高度大于宽度
			{	
				if ( pt.y + rc.Height() - m_iTrigOffset-TFRAME-BFRAME < rcMon.bottom )// 向下的高度够 ，方向为5或6
					if ( pt.x+rc.Width()-RFRAME < rcMon.right ) return 5 ;// 宽度够，向右
					else return 6 ;// 宽度不够，向左
				else// 向下的高度不够，方向为7或8
					if ( pt.x+rc.Width()-RFRAME < rcMon.right ) return 7 ;// 宽度够，向右
					else return 8 ;// 宽度不够，向左
			}
		}
	}
	else
	{
		if ( rc.Width()>=rc.Height() || rc.Height()<100 )// 宽度>=高度 或 高度小于100
		{
			if ( pt.y + rc.Height()-BFRAME-TFRAME+6 + 16 < rcMon.bottom ) // 这里+16是为了不被鼠标指针挡住
					return 3 ;
			else
					return 1 ;
		}
		else//高度>宽度
		{
			if ( rcMon.bottom > pt.y + rc.Height() - ( m_iTrigOffset ) )// 向下的高度够 ，方向为5或6
				if ( rcMon.right > pt.x + rc.Width() + LFRAME ) return 5 ;// 宽度够，向右
				else return 6 ;// 宽度不够，向左
			else// 向下的高度不够，方向为7或8
				if ( rcMon.right > pt.x + rc.Width() + LFRAME ) return 7 ;// 宽度够，向右
				else return 8 ;// 宽度不够，向左
		}
	}

	m_iTrigOffset = s()->Tooltip()->TriangleOffset(m_iTrigOffset , (m_sttfCurr.uFlags&eTTF_BALLOON) != 0);

	return 1 ;
}

//////////////////////////////////////////////////////////////////////////


void CSogouToolTip::RelayEventSingleton(HWND hWnd, DWORD dwIndex)
{

	LPSOGOUTOOLINFO pCurToolInfo = &m_tipInfo ;
	pCurToolInfo->uDelay = 500;
	pCurToolInfo->uFlags = eTTF_TEXTCALLBACK ;

	if (m_hLastTipWnd != hWnd || m_dwLastItemIndex != dwIndex)
	{
		m_hLastTipWnd = hWnd;
		m_dwLastItemIndex = dwIndex;
		if (m_dwLastItemIndex == -1 || hWnd == NULL)
		{
			pCurToolInfo->pdwIndex = RELAYEVENTEX_NOTIP;
			m_sttfCurr.hWnd = NULL;
			PrepareHide();
		}
		else
		{
			pCurToolInfo->pdwIndex = m_dwLastItemIndex;
			pCurToolInfo->hWnd = hWnd;
			PrepareShowTip(pCurToolInfo , NULL);	// 没有加入区域
		}
	}
}

//////////////////////////////////////////////////////////////////////////

CSogouToolTip* CSogouToolTip::sm_pPop = NULL;

CSogouToolTip* CSogouToolTip::Get()
{
	if (sm_pPop == NULL)
	{
		sm_pPop = new CSogouToolTip();
	}
	return sm_pPop;
}
