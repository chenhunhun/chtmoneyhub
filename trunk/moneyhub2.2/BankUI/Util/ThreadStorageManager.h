#pragma once


class CPopupFrameSkin;
class CHookManager;
class CAddrDropDownList;
class CTuotuoMenuBarCtrl;

class CAsyncUI;
class CMainFrame;
class CChildFrame;
class CTuotuoTabCtrl;
class CMainMenuCmd;
class CTabItem;
class CAddressBarCtrl;
class CAddressComboBoxCtrl;
class CBrowserToolBarCtrl;
class CSearchBarCtrl;
class CSearchBarEditContainer;
class CMDIClient;
class CFavoriteBarCtrl;
class CRecentBarCtrl;
class CBrowserStatusBarCtrl;
class CStatusBarToolCtrl;
class CSidebarContainer;
class CFindBarCtrl;
class CTitleBarCtrl;
class CBigButton;
class CTopLevelControl;
class CSSLLockButton;
class CHelpButton;
class CSettingButton; // gao
class CMenuButton;
class CLogoButton;
class CBackButton;
class CForwardButton;
class CRefreshButton;
class CTuotuoCategoryCtrl;

struct FrameStorageStruct
{
	// message router

	CMainFrame *pMainFrame;
	CTuotuoCategoryCtrl *pCate;
	CTuotuoTabCtrl *pTab;
	CBigButton *pBigButton;
	CSSLLockButton *pSSLLockButton;
	CMenuButton* pMenuButton;
	CLogoButton* pLogoButton;
	CHelpButton* pHelpButton;
	CSettingButton* pSettingButton; // gao
	CBackButton* pBackButton;
	CForwardButton* pForwardButton;
	CRefreshButton* pRefreshButton;

	CTabItem *pCurrentItem;
	CTuotuoMenuBarCtrl *pCmdBar;
	CMainMenuCmd *pMenuCmd;
	CAddressBarCtrl *pAddressBar;
	CAddressComboBoxCtrl *pAddrCombo;
	CMDIClient *pMDIClient;
	CTitleBarCtrl *pTitleBar;
	CBrowserToolBarCtrl *pToolbar;
	CSearchBarCtrl *pSearchBar;
	CSearchBarEditContainer *pSearchCombo;
	CFavoriteBarCtrl *pFavorBar;
	CRecentBarCtrl* pRecentBar;
	CBrowserStatusBarCtrl *pStatus;
	CStatusBarToolCtrl *pStatusTool;
	CSidebarContainer *pSideBar;
	CFindBarCtrl *pFindBar;
	CTopLevelControl *pTopLevel;

	HWND hMainFrame;
	int iMenuCount;

	CMainFrame* MainFrame() const			{ return pMainFrame; }
	CTuotuoTabCtrl* Tab() const				{ return pTab; }
	CTabItem* TabItem() const				{ return pCurrentItem; }
	CMainMenuCmd* MenuCmd() const			{ return pMenuCmd; }
	CTuotuoMenuBarCtrl* MainMenu() const	{ return pCmdBar; }
	CTitleBarCtrl* TitleBar() const			{ return pTitleBar; }
	CAddressBarCtrl* AddrBar() const		{ return pAddressBar; }
	CAddressComboBoxCtrl* AddrCombo() const	{ return pAddrCombo; }
	CMDIClient* MDI() const					{ return pMDIClient; }
	CBrowserToolBarCtrl* ToolBar() const	{ return pToolbar; }
	CSearchBarCtrl* SearchBar() const		{ return pSearchBar; }
	CSearchBarEditContainer* SearchCombo() const { return pSearchCombo; }
	CFavoriteBarCtrl* FavorBar() const		{ return pFavorBar; }
	CRecentBarCtrl* RecentBar() const		{ return pRecentBar; }
	CSidebarContainer* SideBar() const		{ return pSideBar; }
	CBrowserStatusBarCtrl* Status() const	{ return pStatus; }
	CStatusBarToolCtrl* StatusTool() const	{ return pStatusTool; }
	CFindBarCtrl* FindBar() const			{ return pFindBar; }
	CTopLevelControl *TopLevel() const		{ return pTopLevel; }
	CBigButton* BigButton() const			{ return pBigButton; }
	CSSLLockButton *SSLLockButton() const	{ return pSSLLockButton; };
	CMenuButton* MenuButton() const			{ return pMenuButton; }
	CLogoButton* LogoButton() const			{ return pLogoButton; }
	CHelpButton* HelpButton() const			{ return pHelpButton; }
	CSettingButton * SettingButton() const  { return pSettingButton;}
	CBackButton* BackButton() const			{ return pBackButton; }
	CForwardButton* ForwardButton() const	{ return pForwardButton; }
	CRefreshButton* RefreshButton() const	{ return pRefreshButton; }

	FrameStorageStruct()
	{
		memset(this, 0, sizeof(FrameStorageStruct));
	}
};

//////////////////////////////////////////////////////////////////////////

class CFSMUtil
{
public:

	CFSMUtil(FrameStorageStruct *pFS) : m_pFS(pFS) {}

	inline FrameStorageStruct* FS() const { return m_pFS; }

	inline static FrameStorageStruct* FS(HWND hWnd)
	{
		HWND hMainFrame = ::GetAncestor(hWnd, GA_ROOTOWNER);
		return (FrameStorageStruct*)::GetWindowLongPtr(hMainFrame, GWLP_USERDATA);
	}

private:

	FrameStorageStruct *m_pFS;
};

//////////////////////////////////////////////////////////////////////////


struct ThreadStorageStruct
{
	CPopupFrameSkin *pSkin;
	CHookManager *pMenuHook;


	int iMenuPopupCount;		// 记录当前菜单弹出的数量，=0表示没有菜单
	bool PopupMenuDisplaying() const { return iMenuPopupCount != 0; }


	ThreadStorageStruct() : pSkin(NULL), pMenuHook(NULL), iMenuPopupCount(0) {}
};


__declspec(selectany) DWORD g_dwTlsIndex = 0;
__declspec(selectany) DWORD g_dwTlsIndexMagicNumber = 0;

namespace TSM
{
	inline void InitializeTls()
	{
		g_dwTlsIndex = ::TlsAlloc();
		g_dwTlsIndexMagicNumber = ::TlsAlloc();
		ATLASSERT(g_dwTlsIndex != TLS_OUT_OF_INDEXES);
		ATLASSERT(g_dwTlsIndexMagicNumber != TLS_OUT_OF_INDEXES);
	}

	inline ThreadStorageStruct* GetTS()
	{
		if (::TlsGetValue(g_dwTlsIndexMagicNumber) != (LPVOID)0x19840403)
			return NULL;
		return (ThreadStorageStruct*)::TlsGetValue(g_dwTlsIndex);
	}

	inline void SetTS(ThreadStorageStruct *pTs)
	{
		::TlsSetValue(g_dwTlsIndex, pTs);
		::TlsSetValue(g_dwTlsIndexMagicNumber, (LPVOID)0x19840403);
	}
}
