#pragma once
#include <tuodwmapi.h>
#include "../../../Utils/ListManager/ListManager.h"
#include "../../Skin/SkinManager.h"
#include "TabCtrl.h"


class CTuotuoTabCtrl;
class CCategoryItem;
class CChildFrame;

typedef std::vector<std::tstring> StrArray;


enum PageType
{
	PageType_Normal,
	PageType_Blank,
	PageType_SpecialPage
};

enum SpecialPageType
{
	SpecialPage_Null,
	SpecialPage_StartPageSelector,
	SpecialPage_StartPage,
	SpecialPage_TabStartPage,
	SpecialPage_StartPageNavi,
	SpecialPage_TabStartPageNavi,
	SpecialPage_StartPageSearch,
	SpecialPage_TabStartPageSearch,
	SpecialPage_StartPageFavor,
	SpecialPage_TabStartPageFavor,
	SpecialPage_TabStartPageBlank,
};

enum StartPageType
{
	StartPage_Null = -1,
	StartPage_Navigate = 0,
	StartPage_Favorite,
	StartPage_Search
};


class CTabItem
{

public:

	CTabItem(bool bNoClose);

	// export function

	void ChangeItemParent(CTuotuoTabCtrl *pTabCtr);

	// 画tab，iDeltaHeight表示在chrome皮肤下，标签窗口上部空出来的高度
	void DoPaint(CDCHandle dc, ButtonStatus eTabStatus, Position btnPos, ButtonStatus eBtn, bool bDrawCloseBtn, int iDeltaHeight, bool bAero);
	bool NeedRefreshLoadingIcon() const;

	void SetToDefault();
	void SetTabView(CChildFrame *pFrame)			{ m_pChildFrame = pFrame; }
	void SetAxControl(HWND hAxWnd);

	CChildFrame* GetChildFrame() const				{ return m_pChildFrame; }
	CWindow GetTabView() const;
	CWindow GetAxControl() const					{ return m_wndAxControl; }
	DWORD GetAxThreadID() const						{ return m_dwAxThreadID; }
	int GetCoreType() const							{ return m_iCoreType; }
	int GetCoreTypeSource() const					{ return m_iCoreTypeSource; }
	void SetCoreType(int iCore)						{ m_iCoreType = iCore; }
	void SetCoreTypeSource(int iSrc)				{ m_iCoreTypeSource = iSrc; }
	bool SwitchCoreUsed() const						{ return m_bSwitchCoreUsed; }
	void SetSwitchCoreUsed(bool bFlag)				{ m_bSwitchCoreUsed = bFlag; }

	PageType GetPageType() const		{ return m_ePageType; }
	void SetPageType(PageType eType)	{ m_ePageType = eType; }
	SpecialPageType GetSpecialPageType() const		{ return m_eSpecialPageType; }
	StartPageType GetStartPageType() const			{ return m_eStartPageType; }
	void SetStartPageType(StartPageType eStartPage)	{ m_eStartPageType = eStartPage; }

	bool IsCloseBorder() const;				// 检测是否为关闭的边界（即如果剩最后一个标签了，就不允许再关闭）
	bool CanBeOverwrite() const;			// 当前这个页面是否可以被覆盖（即点开收藏等链接时在当前这个页面打开）

	LPCTSTR GetText() const;
	LPCTSTR GetURLText() const			{ return m_sURL.c_str(); }
	LPCTSTR GetDatabaseURL() const		{ return m_sDatabaseURL.c_str(); }
	size_t GetURLTextLength() const		{ return m_sURL.length(); }
	size_t GetDatabaseURLLength() const		{ return m_sDatabaseURL.length(); }
	void SetText(LPCTSTR lpszText);
	void SetURLText(LPCTSTR lpszURL);
	void SetDatabaseURL(LPCTSTR lpszURL)		{ m_sDatabaseURL = lpszURL; }
	int GetPageWeight() const			{ return m_iPageWeight; }
	void SetPageWeight(int iWeight)		{ m_iPageWeight = iWeight; }
	bool IsPageViewSource() const;

	void SetIcon(HICON hIcon);
	bool LoadIconFromURL(LPCTSTR lpszURL, HWND hCallbackWnd);			// 返回值表示icon发生改变

	void SetProgress(int iProgress);
	int GetProgress() const				{ return m_iLoadingProgress; }

	void SetAdditinalData(LPCTSTR lpszAdditionalData, int iType);
	LPCTSTR GetAdditinalData() const			{ return m_sAdditionalData.c_str(); }
	int GetAdditionalType() const				{ return m_iAdditionalDataType; }

	void SetStatusText(LPCTSTR lpszText)	{ m_sStatus = lpszText; }
	LPCTSTR GetStatusText() const			{ return m_sStatus.c_str(); }
	void SetStatusIcon(int iIconIndex)		{ m_iStatusIcon = iIconIndex; }
	int GetStatusIcon() const				{ return m_iStatusIcon; }

	void SetMainToolbarStatus(int iType, bool bEnabled);
	bool GetMainToolbarBackStatus() const			{ return m_bIsBackEnabled; }
	bool GetMainToolbarForwardStatus() const		{ return m_bIsForwardEnabled; }

	void SetSearchBarStatus(LPCTSTR lpszSearchString, short iHiLight, short iSearchEngine, bool bForce = false);
	LPCTSTR GetSearchBarString() const	{ return m_strSearchString.c_str(); }
	bool GetSearchBarHiLight() const	{ return m_bSearchHilight; }
	short GetSearchBarEngine() const	{ return m_iSearchEngine; }

	LPCTSTR GetFindString() const		{ return m_strFindText.c_str(); }
	void SetFindString(LPCTSTR str)		{ m_strFindText = str; }
	bool GetFindHiLight() const			{ return m_bFindHiLight; }
	void SetFindHiLight(bool bFindHiLight)	{ m_bFindHiLight = bFindHiLight; }
	bool GetFindCaseSensitive() const		{ return m_bFindCaseSensitive; }
	void SetFindCaseSensitive(bool bCase)	{ m_bFindCaseSensitive = bCase; }
	int GetTotalFind() const			{ return m_iTotalFind; }
	void SetTotalFind(int iTotalFind)	{ m_iTotalFind = iTotalFind; }
	int GetCurrentFind() const			{ return m_iCurrentFind; }
	void SetCurrentFind(int iCurrentFind)	{ m_iCurrentFind = iCurrentFind; }
	bool IsFinding() const				{ return m_bIsFinding; }
	void SetIsFinding(bool bIsFinding)	{ m_bIsFinding = bIsFinding; }

	void SetFontSize(int iSize)			{ m_iFontSize = iSize; }
	int GetFontSize() const				{ return m_iFontSize; }
	void SetZoom(int iZoom)				{ m_iZoom = iZoom <= 20 ? 20 : (iZoom >= 400 ? 400 : iZoom); }
	int GetZoom() const					{ return m_iZoom; }
	DWORD GetInitZoomValue();

	int SetResourceStringList(LPCTSTR szList);			// 返回值为这一组字符串代表的类型
	int GetResourceCount(int iRes)		{ return m_strResourceTitle[iRes].size(); }
	StrArray& GetResourceStringList(int iRes)		{ return m_strResourceTitle[iRes]; }

	StrArray& GetBlockedPopups()			{ return m_strBlockedPopups; }
	StrArray& GetBlockedFloatingElements()  { return m_strBlockedFloatingElements; }
	int GetBlockedCount()					{ return m_strBlockedPopups.size() + m_strBlockedFloatingElements.size(); }

	void SetNewWidth();
	void SetNewWidthImmediately();
	int GetAniPos() { return m_iXPosAni; }
	void SetPos(int iPos);
	void SetPosImmediately(int iPos);
	void UpdateAnimateXPos();
	void UpdateAnimateWidth();
	//void FinishAnimation(CTuotuoTabCtrl *pOPTab);

	size_t GetIndex() const				{ return m_nIndex; }
	void SetIndex(size_t nIndex)		{ m_nIndex = nIndex; }


	void IncExceptionCount(DWORD dwCode)	{ m_iExceptionCount ++; m_dwLastExceptionCode = dwCode; }
	int GetExceptionCount() const			{ return m_iExceptionCount; }
	DWORD GetLastExceptionCode() const		{ return m_dwLastExceptionCode; }
	DWORD GetRequestNum() const             { return m_dwRequestNum; }

	void SetAutofillTip(bool bFlag)			{ m_bDisplayAutofillTip = bFlag; }
	bool IsAutofillTipShown() const			{ return m_bDisplayAutofillTip; }

	void SetSecure(bool bSecure)			{ m_bIsSecure = bSecure; }
	bool GetSecure() const					{ return m_bIsSecure; }

	bool IsNoClose() const					{ return m_bNoClose; }

public:

	CCategoryItem *m_pCategory;

private:

	bool m_bNoClose;

	CTuotuoTabCtrl *m_pTabCtr;
	CChildFrame *m_pChildFrame;
	CWindow m_wndAxControl;
	DWORD m_dwAxThreadID;
	int m_iCoreType;
	int m_iCoreTypeSource;			// 判断核的来源，0=系统名单，1=用户数据
	bool m_bSwitchCoreUsed;			// 用户是否手工切换过核

	int m_nIndex;

	PageType m_ePageType;
	SpecialPageType m_eSpecialPageType;
	StartPageType m_eStartPageType;

	std::tstring m_sURL;
	std::tstring m_sDatabaseURL;		// 记录在database中的url，在页面有跳转的情况下与m_sURL是不同的
	std::tstring m_sText;
	std::tstring m_sStatus;
	std::tstring m_sAdditionalData;		// 额外的数据，可能是referer或者是postdata
	int m_iAdditionalDataType;			// 额外的数据类型，-1表示无额外数据，0表示referer，1表示postdata
	HICON m_hIcon;			// 页面图标
	std::tstring m_strLastIconName;	// 记录icon对应的filename
	int m_iPageWeight;			// 页面创建的权重

	bool m_bIsBackEnabled, m_bIsForwardEnabled;			// 前进、后退按钮是否可以使用

	int m_iStatusIcon;			// 0=loading, 1=load complete, 2=complete with error
	int m_iLoadingProgress;		// the current loading progress 0 ~ 1000

	std::tstring m_strSearchString;		// 搜索栏的文字
	bool m_bSearchHilight;			// 搜索是否高亮
	short m_iSearchEngine;			// 搜索使用的搜索引擎

	std::tstring m_strFindText;			// 查找栏的文字
	bool m_bFindHiLight;			// 查找栏是否高亮
	bool m_bFindCaseSensitive;		// 是否区分大小写
	int m_iTotalFind;				// 总共找到的文字个数，-1为还没查找
	int m_iCurrentFind;				// 当前在哪个位置上
	bool m_bIsFinding;				// 是否正在查找（即childframe已经向axcontrol发送了查找请求，但是还未返回）

	int m_iFontSize;			// 浏览器字体大小 -2, -1, 0, 1, 2
	int m_iZoom;				// 浏览器zoom级别 100%

	StrArray m_strResourceTitle[RESOURCE_TYPE_COUNT];		// 提取资源使用, 0表示视频、1表示音频

	StrArray m_strBlockedPopups;			// 被拦截的弹出窗口的地址
	StrArray m_strBlockedFloatingElements;	// 被拦截的页面浮动元素

	// for animation
	int m_iXPos;			// 目标x坐标
	int m_iXPosAni;			// 当前动画中的x坐标
	int m_iXPosAniFrame;	// 当前处于第几帧
	int m_iXPosOld;			// 动画播放前的x坐标

	int m_iWidth;
	int m_iWidthAni;
	int m_iWidthAniFrame;
	int m_iWidthOld;

	int m_iLoadingProgressFrame;
	DWORD m_dwLastUpdateLoadingProgressTime;

	// for pingback
	int m_iExceptionCount;			// 发生exception的次数
	DWORD m_dwLastExceptionCode;	// 最后一次exception的code
	DWORD m_dwRequestNum ;          // Index
	bool m_bDisplayAutofillTip;		// 是否显示小兰条

	bool m_bIsSecure;

public:

	HWND m_hThumbnailProxy;				// 给win7使用的现实缩略图的proxy

	friend class CTuotuoTabCtrl;
};
