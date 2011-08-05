#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"

// 一团乱麻
// 有时间整理一下


const int g_iButtonFrame[] = { 4, 4, 3 };


CToolbarSkin::CToolbarSkin(CSkinManager *pSkin)
{
	LPCTSTR lpFileNames[] = {
		_T("toolbar_btn_back"),
		_T("toolbar_btn_forward"),
		_T("toolbar_btn_refresh"),
	};
	for (int i = 0; i < _countof(g_iButtonFrame); i++)
		pSkin->RegisterFileName(lpFileNames[i], m_bitmap[i]);

	pSkin->RegisterFileName(_T("rebar_bg"), m_bmpRebarBack);
	pSkin->RegisterFileName(_T("rebar_bg_secure"), m_bmpRebarBack_Secure);

	pSkin->RegisterFileName(_T("checkdlg"), m_bmpCheckDlgBack);
	
	pSkin->RegisterFileName(_T("authendlg"), m_bmpAuthenDlgBack);
	pSkin->RegisterFileName(_T("authendlg_closebtn"), m_bmpDlgCloseBtn);
	pSkin->RegisterFileName(_T("button_back"), m_bmpDlgButtonBack);

	pSkin->RegisterFileName(_T("list_tabctrl_back"), m_bmpListTabCtrlBack);
	pSkin->RegisterFileName(_T("list_tabctrl_items"), m_bmpListTabCtrlItems);

	pSkin->RegisterFileName(_T("setting_dlg_back"), m_bmpSettingDlgBack);
	pSkin->RegisterFileName(_T("msgbox_back"), m_bmpMessageBoxBack);
	pSkin->RegisterFileName(_T("msgbox_icons"), m_bmpMessageBoxIcons);
}


void CToolbarSkin::Refresh()
{
	for (int i = 0; i < _countof(g_iButtonFrame); i++)
	{
		m_bitmap[i].LoadFromFile();
	}
	m_bmpRebarBack.LoadFromFile();
	m_bmpRebarBack_Secure.LoadFromFile();

	m_bmpCheckDlgBack.LoadFromFile();
	
	m_bmpAuthenDlgBack.LoadFromFile();
	m_bmpDlgCloseBtn.LoadFromFile();
	m_bmpDlgButtonBack.LoadFromFile();

	m_bmpListTabCtrlBack.LoadFromFile();
	m_bmpListTabCtrlItems.LoadFromFile();

	m_bmpSettingDlgBack.LoadFromFile();
	m_bmpMessageBoxBack.LoadFromFile();
	m_bmpMessageBoxIcons.LoadFromFile();
}

//////////////////////////////////////////////////////////////////////////

void CToolbarSkin::DrawButton(CDCHandle dc, const RECT &rect, UINT uID, UINT uButtonStatus) const
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	ATLASSERT(btnStatus >= 0 && btnStatus < g_iButtonFrame[uID]);
	UINT uYPos = (rect.bottom + rect.top - m_bitmap[uID].GetHeight()) / 2;
	RECT rcDest = { rect.left, uYPos, rect.right, uYPos + GetButtonHeight(uID) };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap[uID], btnStatus, g_iButtonFrame[uID]);
}

void CToolbarSkin::DrawToolBarSep(CDCHandle dc, const RECT &rect) const
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, CSkinProperty::GetDWORD(Skin_Property_Toolbar_Seperator_Color));
	dc.SelectPen(pen);
	int iX = (rect.right + rect.left) / 2;
	dc.MoveTo(iX, rect.top + 3);
	dc.LineTo(iX, rect.bottom - 3);
}

void CToolbarSkin::DrawRebarBack(CDCHandle dc, const RECT &rect, bool bIsMax, bool bSecure)
{
	if (rect.bottom - rect.top != m_rebarData.iLastHeight || rect.right - rect.left != m_rebarData.iLastWidth || m_rebarData.bLastIsMax != bIsMax 
		|| m_rebarData.bLastIsSecure != bSecure)
	{
		m_rebarData.iLastWidth = rect.right - rect.left;
		m_rebarData.iLastHeight = rect.bottom - rect.top;
		m_rebarData.bLastIsMax = bIsMax;
		m_rebarData.bLastIsSecure = bSecure;
		if (m_rebarData.bufferBitmap != NULL)
			m_rebarData.bufferBitmap.DeleteObject();

		BITMAPINFOHEADER bih = { sizeof(BITMAPINFOHEADER), m_rebarData.iLastWidth, m_rebarData.iLastHeight, 1, 32 };
		m_rebarData.bufferBitmap.CreateDIBSection(dc, (BITMAPINFO*)&bih, DIB_RGB_COLORS, NULL, NULL, 0x0);
		m_rebarData.bufferDC.SelectBitmap(m_rebarData.bufferBitmap);

		// iDrawType = 0表示只有一组图片，iDrawType=1/2表示有两组图片，分别是tab在rebar下面时，有tab和无tab时rebar的样子
		
		CTuoImage* pImg = bSecure ? &m_bmpRebarBack_Secure : &m_bmpRebarBack;

		int iLeft = 0;
		int iSize = pImg->GetWidth(); 
		int iTopBorderHeight = bIsMax ? CSkinProperty::GetDWORD(Skin_Property_Rebar_Skin_Top_Border_Height) : 0;
		int iBottomBorderHeight = 1;
 		if (m_rebarData.bLastIsMax)
 		{
			/*
 			RECT rcSrc = { iLeft + iSize / 2, iTopBorderHeight, iLeft + iSize / 2 + 1, m_bmpRebarBack.GetHeight() - iBottomBorderHeight };
 			RECT rcDest = { 0, 0, m_rebarData.iLastWidth, m_bmpRebarBack.GetHeight() - iBottomBorderHeight - iTopBorderHeight };
 			m_bmpRebarBack.Draw(m_rebarData.bufferDC, rcDest, rcSrc);
			*/

			int i = 0;
			while (true)
			{
				RECT rcSrc = { iLeft + iSize / 2 - 10, iTopBorderHeight, iLeft + iSize / 2 + 10, pImg->GetHeight() - iBottomBorderHeight };
				//RECT rcDest = { 0, 0, m_rebarData.iLastWidth, m_bmpRebarBack.GetHeight() - iBottomBorderHeight - iTopBorderHeight };
				RECT rcDest = { 0, 0, 20, pImg->GetHeight() - iBottomBorderHeight - iTopBorderHeight };
				OffsetRect(&rcDest, 20 * i++, 0);
				pImg->Draw(m_rebarData.bufferDC, rcDest, rcSrc);

				if (rcDest.right > m_rebarData.iLastWidth)
					break;
			}

 		}
 		else
		{
			RECT rcDest = { 0, 0, m_rebarData.iLastWidth, pImg->GetHeight() - iBottomBorderHeight };
			//CSkinManager::DrawExtends(m_rebarData.bufferDC, rcDest, m_bmpRebarBack, iLeft, iLeft + iSize, 0, m_bmpRebarBack.GetHeight() - iBottomBorderHeight, iSize / 2);
			CSkinManager::DrawTileExtends(m_rebarData.bufferDC, rcDest, *pImg, iLeft, iLeft + iSize, 0, pImg->GetHeight() - iBottomBorderHeight, 20);
		}

		if (m_rebarData.iLastHeight > pImg->GetHeight() - iTopBorderHeight)
			m_rebarData.bufferDC.StretchBlt(0, pImg->GetHeight() - iBottomBorderHeight - iTopBorderHeight, m_rebarData.iLastWidth, m_rebarData.iLastHeight - pImg->GetHeight() + iTopBorderHeight, m_rebarData.bufferDC, 0, pImg->GetHeight() - iBottomBorderHeight - iTopBorderHeight - 1, m_rebarData.iLastWidth, 1, SRCCOPY);

		// 绘制最下面的border
		if (iBottomBorderHeight > 0)
		{
			if (m_rebarData.bLastIsMax)
			{
				RECT rcSrc = { iLeft + iSize / 2, pImg->GetHeight() - iBottomBorderHeight, iLeft + iSize / 2 + 1, pImg->GetHeight() };
				RECT rcDest = { 0, m_rebarData.iLastHeight - iBottomBorderHeight, m_rebarData.iLastWidth, m_rebarData.iLastHeight };
				pImg->Draw(m_rebarData.bufferDC, rcDest, rcSrc);
			}
			else
			{
				RECT rcDest = { 0, m_rebarData.iLastHeight - iBottomBorderHeight, m_rebarData.iLastWidth, m_rebarData.iLastHeight };
				//CSkinManager::DrawExtends(m_rebarData.bufferDC, rcDest, m_bmpRebarBack, iLeft, iLeft + iSize, m_bmpRebarBack.GetHeight() - iBottomBorderHeight, m_bmpRebarBack.GetHeight(), iSize / 2);
				CSkinManager::DrawTileExtends(m_rebarData.bufferDC, rcDest, *pImg, iLeft, iLeft + iSize, pImg->GetHeight() - iBottomBorderHeight, pImg->GetHeight(), 20);
			}
		}
	}

	dc.BitBlt(0, rect.top, m_rebarData.iLastWidth, m_rebarData.iLastHeight, m_rebarData.bufferDC, 0, 0, SRCCOPY);
}

void CToolbarSkin::DrawRebarBackPart(CDCHandle dc, const RECT &rect, HWND hWnd)
{
	RECT rcWnd;
	::GetWindowRect(hWnd, &rcWnd);
	CWindow hRoot = ::GetRootWindow(hWnd);
	hRoot.ScreenToClient(&rcWnd);
	dc.BitBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, m_rebarData.bufferDC, rect.left + rcWnd.left, rect.top + rcWnd.top, SRCCOPY);
}

void CToolbarSkin::DrawCheckDlgBack(CDCHandle dc, const RECT &rect)
{
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc, rect.right - rect.left, rect.bottom - rect.top);
	memDC.SelectBitmap(bmp);

	m_bmpCheckDlgBack.Draw(memDC, 0, 0, m_bmpCheckDlgBack.GetWidth(), m_bmpCheckDlgBack.GetHeight(), 0, 0, m_bmpCheckDlgBack.GetWidth(), m_bmpCheckDlgBack.GetHeight());
	dc.BitBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);
}

void CToolbarSkin::DrawAuthenDlgBack(CDCHandle dc, const RECT &rect)
{
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc, rect.right - rect.left, rect.bottom - rect.top);
	memDC.SelectBitmap(bmp);

	m_bmpAuthenDlgBack.Draw(memDC, 0, 0, m_bmpAuthenDlgBack.GetWidth(), m_bmpAuthenDlgBack.GetHeight(), 0, 0, m_bmpAuthenDlgBack.GetWidth(), m_bmpAuthenDlgBack.GetHeight());
	dc.BitBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);
}

void CToolbarSkin::DrawAuthenDlgCloseBtn(CDCHandle dc, const RECT &rect, UINT iCurrentFrame)
{
	ATLASSERT(iCurrentFrame >= 0 && iCurrentFrame < 3);

	RECT rcDraw = { rect.left, rect.top, 
		rect.left + m_bmpDlgCloseBtn.GetWidth() / 3, 
		rect.top + m_bmpDlgCloseBtn.GetHeight() };

	CSkinManager::DrawImagePart(dc, rcDraw, m_bmpDlgCloseBtn, iCurrentFrame, 3);
}

void CToolbarSkin::DrawAuthenDlgButton(CDCHandle dc, const RECT &rect, UINT iCurrentFrame, LPCTSTR lpszName)
{
	ATLASSERT(iCurrentFrame >= 0 && iCurrentFrame < 4);

	RECT rcDraw = { rect.left, rect.top, 
		rect.left + m_bmpDlgButtonBack.GetWidth() / 4, 
		rect.top + m_bmpDlgButtonBack.GetHeight() };

	CSkinManager::DrawImagePart(dc, rcDraw, m_bmpDlgButtonBack, iCurrentFrame, 4);

	dc.SetBkMode(TRANSPARENT);
	dc.SelectFont(s()->Common()->GetDefaultFont());
	dc.SetTextColor(RGB(61, 98, 123));
	::DrawText(dc, lpszName, -1, &rcDraw, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void CToolbarSkin::DrawSettingDlgBack(CDCHandle dc, const RECT &rect)
{
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc, rect.right - rect.left, rect.bottom - rect.top);
	memDC.SelectBitmap(bmp);

	m_bmpSettingDlgBack.Draw(memDC, 0, 0, m_bmpSettingDlgBack.GetWidth(), m_bmpSettingDlgBack.GetHeight(), 0, 0, m_bmpSettingDlgBack.GetWidth(), m_bmpSettingDlgBack.GetHeight());
	dc.BitBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);
}

void CToolbarSkin::DrawMessageBoxBack(CDCHandle dc, const RECT &rect)
{
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc, rect.right - rect.left, rect.bottom - rect.top);
	memDC.SelectBitmap(bmp);

	m_bmpMessageBoxBack.Draw(memDC, 0, 0, m_bmpMessageBoxBack.GetWidth(), m_bmpMessageBoxBack.GetHeight(), 0, 0, m_bmpMessageBoxBack.GetWidth(), m_bmpMessageBoxBack.GetHeight());
	dc.BitBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);
}

void CToolbarSkin::DrawMessageBoxIcon(CDCHandle dc, const RECT &rect, UINT iCurr)
{
	ATLASSERT(iCurr >= 0 && iCurr < 4);

	RECT rcDraw = { rect.left, rect.top, 
		rect.left + m_bmpMessageBoxIcons.GetWidth() / 4, 
		rect.top + m_bmpMessageBoxIcons.GetHeight() };

	CSkinManager::DrawImagePart(dc, rcDraw, m_bmpMessageBoxIcons, iCurr, 4);
}