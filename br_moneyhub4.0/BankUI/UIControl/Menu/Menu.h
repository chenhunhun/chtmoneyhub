#pragma once


#define MENU_TEXT_MAX_WIDTH			300


#define MSG_WM_TUOTUO_MENU() \
	if (uMsg == WM_DRAWITEM && ((LPDRAWITEMSTRUCT)lParam)->CtlType == ODT_MENU) \
	{ \
		CTuotuoMenu::DrawItem((LPDRAWITEMSTRUCT)lParam); \
		return TRUE; \
	} \
	else if (uMsg == WM_MEASUREITEM && ((LPMEASUREITEMSTRUCT)lParam)->CtlType == ODT_MENU) \
	{ \
		CTuotuoMenu::MeasureItem((LPMEASUREITEMSTRUCT)lParam); \
		return TRUE; \
	} \
	else if (uMsg == WM_MENUSELECT) \
	{ \
		::OnMenuSelect(m_hWnd, wParam, lParam); \
	} \
	else if (uMsg == WM_ENTERMENULOOP) \
	{ \
		::OnMenuLoop(m_hWnd, true); \
	} \
	else if (uMsg == WM_EXITMENULOOP) \
	{ \
		::OnMenuLoop(m_hWnd, false); \
	}


extern int g_iCurrentMenuMaxAllowedHeight;

BOOL OnMenuSelect(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnMenuLoop(HWND hWnd, bool bIsEnterMenuLoop);

class CTuotuoMenu : public CWindowImpl<CTuotuoMenu>, public CMenuHandle
{

public:

	CTuotuoMenu(HMENU hMenu, HWND hWnd);

private:

	// message

	BEGIN_MSG_MAP_EX(CTuotuoMenu)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)
		MSG_WM_PRINT(OnPrint)
		MSG_WM_NCPAINT(OnNcPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER_EX(WM_USER, OnGetMenuObject)
	END_MSG_MAP()

	// message handler

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	void OnPrint(CDCHandle dc, UINT uFlags);
	void OnNcPaint(CRgn rgn);
	BOOL OnEraseBkgnd(CDCHandle dc);
	LRESULT OnGetMenuObject(UINT uMsg, WPARAM wParam, LPARAM lParam) { return (LRESULT)this; }

	// override

	virtual void OnFinalMessage(HWND /* hWnd */);

	// user function

	static void MeasureItem(LPMEASUREITEMSTRUCT lpMeasure);
	static void DrawItem(LPDRAWITEMSTRUCT lpDraw);

	void CheckSize();

	static CTuotuoMenu* GetCurrentMenu() { return sm_PopupMenuVector.empty() ? NULL : sm_PopupMenuVector[sm_PopupMenuVector.size() - 1]; }

	struct TrackPopupMenuParams
	{
		DWORD dwFlags;
		POINT pt;
		RECT rcExclude;
		UINT uMenuRes;
	};


	CDC m_backDC;
	CBitmap m_backBMP;

	bool m_bUseTrackParams;

	TrackPopupMenuParams m_TrackParams;
	static CTuotuoMenu *sm_pTopMenuWnd;

	typedef std::map<HMENU, CTuotuoMenu*> PopupMenuMap;
	static PopupMenuMap sm_PopupMenuMap;

	typedef std::vector<TrackPopupMenuParams> TrackPopupMenuFlagVector;
	static TrackPopupMenuFlagVector sm_TrackFlags;

	typedef std::vector<HMENU> TrackPopupMenuHMenuVector;
	static TrackPopupMenuHMenuVector sm_TrackPopupMenuHMenuVector;

	typedef std::vector<CTuotuoMenu*> PopupMenuVec;
	static PopupMenuVec sm_PopupMenuVector;
};


//////////////////////////////////////////////////////////////////////////

BOOL TuoTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm = NULL, UINT uMenuRes = 0);

void CalcMenuMaxAllowedHeight(HWND hWnd, LPTPMPARAMS lptpm);
int GetMenuItemWidth(CDCHandle dc, LPCTSTR lpszCaption, WORD wID, DWORD dwMenuResID, bool bContainSubMenu);
int GetMenuHeight(CMenuHandle menu);
