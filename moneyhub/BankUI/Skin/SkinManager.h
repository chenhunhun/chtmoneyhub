#pragma once
#include "TuoImage.h"
#include "UISkin/MainFrameBorderSkin.h"
#include "UISkin/ComboSkin.h"
#include "UISkin/FavoriteBarSkin.h"
#include "UISkin/TabSkin.h"
#include "UISkin/CategorySkin.h"
#include "UISkin/MenuBarSkin.h"
#include "UISkin/StatusBarSkin.h"
#include "UISkin/ToolbarSkin.h"
#include "UISkin/BigButtonSkin.h"
#include "UISkin/SSLLockButtonSkin.h"
#include "UISkin/HelpButtonSkin.h"
#include "UISkin/MenuButtonSkin.h"
#include "UISkin/LogoButtonSkin.h"
#include "UISkin/BackButtonSkin.h"
#include "UISkin/ForwardButtonSkin.h"
#include "UISkin/RefreshButtonSkin.h"
#include "UISkin/SettingButtonSkin.h"
#include "UISkin/LoadButtonSkin.h"
#include "UISkin/LoginButtonSkin.h"
#include "UISkin/SepButtonSkin.h"
#include "UISkin/AlarmButtonSkin.h"
#include "UISkin/SynchroButtonSkin.h"

#include "ResourceSkin/MenuSkin.h"
#include "ResourceSkin/Common.h"
#include "ResourceSkin/ToolTipSkin.h"
#include "UISkin/CoolMenuSkin.h"

class CSkinManager
{

public:

	CSkinManager();

	void RefreshAllSkins(bool bReload);
	void RefreshProgressSkin();
	void CreateTheme(HWND hWnd, bool bDestroyOld);

	void RegisterFileName(LPCTSTR lpszFileName, CTuoImage &image);
	void RegisterFileName(LPCTSTR lpszFileName, CTuoIcon &icon);

	bool RefreshSkinFile(LPCTSTR lpszFileName);


	inline HTHEME GetTheme() const { return m_hTheme; }

	inline CComboSkin* Combo() { return &m_ComboSkin; }
	inline CMainFrameBorderSkin* MainFrame() { return &m_MainFrameBorderSkin; }
	inline CFavoriteBarSkin* FavorBar() { return &m_FavoriteSkin; }
	inline CMenuBarSkin* MenuBar() { return &m_MenuBarSkin; }
	inline CTabSkin* Tab() { return &m_TabSkin; }
	inline CCategorySkin* Category() { return &m_CategorySkin; }
	inline CStatusBarSkin* StatusBar() { return &m_StatusBarSkin; }
	inline CToolbarSkin* Toolbar() { return &m_ToolBarSkin; }
	inline CBigButtonSkin* BigButton() { return &m_BigButtonSkin; }
	inline CSSLLockButtonSkin* SSLLockButton() { return &m_SSLLockButtonSkin; }
	inline CMenuButtonSkin* MenuButton() { return &m_MenuButtonSkin; }
	inline CLogoButtonSkin* LogoButton() { return &m_LogoButtonSkin; }
	inline CHelpButtonSkin* HelpButton() { return &m_HelpButtonSkin; }
	inline CLoadButtonSkin* LoadButton() { return &m_LoadButtonSkin; }
	inline CAlarmButtonSkin* AlarmButton() { return &m_AlarmButtonSkin; }
	inline CSynchroButtonSkin* SynchroButton() { return &m_SynchroButtonSkin; }
	inline CLoginButtonSkin* LoginButton() { return &m_LoginButtonSkin; }
	inline CSepButtonSkin* SepButton() { return &m_SepButtonSkin; }
	inline CSettingButtonSkin* SettingButton() {return &m_SetButtonSkin;}
	inline CCommonSkin* Common() { return &m_CommonSkin; }
	inline CToolTipSkin* Tooltip() { return &m_TooltipSkin; }
	inline CBackButtonSkin* BackButton() { return &m_BackButtonSkin; }
	inline CForwardButtonSkin* ForwardButton() { return &m_ForwardButtonSkin; }
	inline CRefreshButtonSkin* RefreshButton() { return &m_RefreshButtonSkin; }
	inline CCoolMenuSkin* CoolMenuSkin() { return &m_CoolMenuSkin; }

	static ButtonStatus ConvertBtnStateToButtonStatus(UINT uBtnState);
	static ButtonStatus FromTuoToolStatus(UINT uTuoToolStatus);
	static void DrawExtends(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcLeft, int iSrcRight);
	static void DrawExtends(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcLeft, int iSrcRight, int iTop, int iBottom, int iSepPos);
	static void DrawTileExtends(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcLeft, int iSrcRight, int iTop, int iBottom, int iTile);
	static void DrawOverlayExtends(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSepLeft, int iSepRight);
	static void DrawOverlayExtendsPart(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSepLeft, int iSepRight, int iLeft, int iRight);
	static void DrawExtendsVertical(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcTop, int iSrcBottom);
	static void DrawExtendsVertical(CDCHandle dc, const RECT &rcDest, CTuoImage &img, int iSrcTop, int iSrcBottom, int iLeft, int iRight, int iSepPos);
	static void DrawImagePart(CDCHandle dc, const RECT &rcDest, const CTuoImage &img, int iCurrentFrame, int iTotalFrame);
	static int DrawGlowText(CDCHandle hdc, LPCTSTR lpchText, int cchText, LPRECT lprc, UINT format, bool bGlow);

private:

	typedef std::map<std::wstring, CTuoImage*> FileName2ImageMap;
	FileName2ImageMap m_File2Image;
	typedef std::map<std::wstring, CTuoIcon*> FileName2IconMap;
	FileName2IconMap m_File2Icon;

	CComboSkin m_ComboSkin;
	CMainFrameBorderSkin m_MainFrameBorderSkin;
	CFavoriteBarSkin m_FavoriteSkin;
	CMenuBarSkin m_MenuBarSkin;
	CTabSkin m_TabSkin;
	CCategorySkin m_CategorySkin;
	CStatusBarSkin m_StatusBarSkin;
	CToolbarSkin m_ToolBarSkin;
	CBigButtonSkin m_BigButtonSkin;
	CSSLLockButtonSkin m_SSLLockButtonSkin;
	CMenuButtonSkin m_MenuButtonSkin;
	CLogoButtonSkin m_LogoButtonSkin;
	CLoadButtonSkin m_LoadButtonSkin;
	CAlarmButtonSkin m_AlarmButtonSkin;
	CSynchroButtonSkin m_SynchroButtonSkin;
	CLoginButtonSkin m_LoginButtonSkin;
	CSepButtonSkin m_SepButtonSkin;
	CHelpButtonSkin m_HelpButtonSkin;
	CSettingButtonSkin m_SetButtonSkin;
	CCommonSkin m_CommonSkin;
	CMenuSkin m_MenuSkin;
	CToolTipSkin m_TooltipSkin;

	CBackButtonSkin m_BackButtonSkin;
	CForwardButtonSkin m_ForwardButtonSkin;
	CRefreshButtonSkin m_RefreshButtonSkin;

	CCoolMenuSkin m_CoolMenuSkin;

	HTHEME m_hTheme;

	static int m_iDrawFixWithSomeDisplayCard;			// 某些显卡用stretchblt时会出现问题，这里做补偿
};


extern CSkinManager *g_pSkin;

inline CSkinManager* s()
{
	return g_pSkin;
}
