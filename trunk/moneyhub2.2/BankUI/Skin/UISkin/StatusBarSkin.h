#pragma once
#include "TuoImage.h"

#define ANI_RES_PASSPORT			0
#define ANI_RES_DOWNLOAD			1

class CSkinManager;

class CStatusBarSkin
{

public:

	CStatusBarSkin(CSkinManager *pSkin);

	void Refresh();

	RECT AdjustToolRect(const RECT &rect, int iIconIndex);

	int GetStatusBarHeight(bool bIsWindowMax);
	int GetPageStatusIconWidth();
	int GetPageStatusIconHeight();
	int GetStatusToolWidth();

	const CTuoImage* GetAni(int iIndex) const { return &m_bmpStatusToolAni[iIndex]; }
	const int GetAniFrame(int iIndex) const	{ return m_bmpStatusToolAni[iIndex].GetWidth() / m_bmpStatusToolAni[iIndex].GetHeight(); }

	void DrawStatusPageIcon(CDCHandle dc, const RECT &rect, int iIndex);
	void DrawStatusSplit(CDCHandle dc, const RECT &rect);
	void DrawStatusProgress(CDCHandle dc, const RECT& rect, DWORD dwProgress);

	// Security Check Dialog
	void DrawCheckProgress(CDCHandle dc, const RECT& rect, DWORD dwProgress);

	// iDrawType = 0表示标签不在下面，状态栏的skin只有一种情况，=1表示tab在下面，且tab显示，=2表示tab在下面，但是tab隐藏
	void DrawStatusBarBack(CDCHandle dc, const RECT &rect, bool bIsMax, int iDrawType, bool bSecure);
	void DrawStatusBarBackPart(CDCHandle dc, const RECT &rcBtn, HWND hCurrentWnd, CWindow wndStatusBar, int iDrawType);
	void DrawStatusBarTool(CDCHandle dc, const RECT &rcBtn, int iIconIndex, UINT uStatus);
	void DrawStatusBarToolWithText(CDCHandle dc, const RECT &rcBtn, int iIconIndex, UINT uStatus, LPCTSTR lpszText);

private:

	CTuoImage m_bmpStatusBarBack;
	CTuoImage m_bmpStatusBarBack_Secure;
	CTuoImage m_bmpStatusToolIcon;
	CTuoImage m_bmpPageStatusIcon;
	CTuoImage m_bmpStatusToolAni[2];

	CTuoImage m_bmpStatusSplit;
	CTuoImage m_bmpStatusProgress;
	CTuoImage m_bmpStatusProgress2;
};
