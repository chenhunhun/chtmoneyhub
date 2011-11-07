#include "stdafx.h"
#include "../SkinLoader.h"
#include "../SkinManager.h"
#include "../../UIControl/FloatingTip/ToolTip.h"
#include "ToolTipSkin.h"


CToolTipSkin::CToolTipSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("balloon_btn_close"), m_imgCloseBtn);
	pSkin->RegisterFileName(_T("balloon_bg"), m_imgBallonBk);
	pSkin->RegisterFileName(_T("balloon_checkbox"), m_imgCheckBox);
	pSkin->RegisterFileName(_T("balloon_icon_tip"), m_imgBalloonIcon);
}

void CToolTipSkin::Refresh()
{
	m_iBorderThick = 18 ;
	m_iTriangleOffset = 36 ;
	m_iTriangleWidth = 10 ;
	m_iRowSpacing = 5 ;
	m_rcDefault = CRect(20,20,150,60) ;
	m_iMaxWidth = ::GetSystemMetrics(SM_CXSCREEN)/2 ;

	if (m_fontText)
		m_fontText.DeleteObject();
	m_fontText.CreateFont(13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));
	if (m_fontTitle)
		m_fontTitle.DeleteObject();
	m_fontTitle.CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("宋体"));

	m_imgCloseBtn.LoadFromFile();
	m_imgBallonBk.LoadFromFile();
	m_imgCheckBox.LoadFromFile();
	m_imgBalloonIcon.LoadFromFile();
}

//////////////////////////////////////////////////////////////////////////

RECT CToolTipSkin::CalcTextRect(HDC hdc , LPCTSTR lpText , int iRawSpacing /* = -1 */)
{
	RECT rc = {0 , 0 , m_iMaxWidth , 10 };
	
	int iHeight = DrawText(hdc , lpText , _tcslen(lpText) ,&rc , DT_CALCRECT | DT_WORDBREAK | DT_NOCLIP | DT_NOFULLWIDTHCHARBREAK );
	
	return CRect() ;
}

void CToolTipSkin::DrawBalloonBack(CDCHandle dc , CRect rcClient , DWORD dwIndex )
{
	int iWidth = m_imgBallonBk.GetWidth()/9 ;
	int iHeight = m_imgBallonBk.GetHeight();
	int iOffset = dwIndex*iWidth ;
	CRect rcMid(28,28,61,61);

	if(dwIndex==0)
		rcMid = CRect(23,23,68,68) ;

	m_imgBallonBk.Draw(dc , 0 , 0 , rcMid.left , rcMid.top , iOffset,0 ) ;
	m_imgBallonBk.Draw(dc, 0,rcClient.bottom-(iHeight-rcMid.bottom) , rcMid.left , iHeight-rcMid.bottom , iOffset,rcMid.bottom );
	m_imgBallonBk.Draw(dc , rcClient.right-(iWidth-rcMid.right),0,iWidth-rcMid.right , rcMid.top , iOffset+rcMid.right , 0 );
	m_imgBallonBk.Draw(dc , rcClient.right-(iWidth-rcMid.right) , rcClient.bottom-(iHeight-rcMid.bottom) , iWidth-rcMid.right , iHeight-rcMid.bottom , iOffset+rcMid.right , rcMid.bottom );	
	m_imgBallonBk.Draw(dc ,rcMid.left,rcMid.top ,rcClient.Width()-(iWidth-rcMid.Width()) , rcClient.Height() - (iHeight-rcMid.Height()) , iOffset+rcMid.left , rcMid.top , rcMid.Width(),rcMid.Height()) ;

	// 中间一块平铺

	switch(dwIndex)
	{	// left
	case 5:
		break;
	case 7:
		break;
	default:
		m_imgBallonBk.Draw(dc , 0,rcMid.top , rcMid.left , rcClient.Height()-(iHeight-rcMid.Height()) , iOffset , rcMid.top , rcMid.left , rcMid.Height());
	}
	
	switch(dwIndex)
	{	// top
	case 3:
		m_imgBallonBk.Draw(dc , rcMid.left , 0 , rcMid.Width() , rcMid.top , iOffset+rcMid.left , 0) ;
		m_imgBallonBk.Draw(dc , rcMid.right , 0 , rcClient.Width()-iWidth , rcMid.top , iOffset+rcMid.right-1 , 0 , 1 , rcMid.top) ;
		break;
	case 4:
		m_imgBallonBk.Draw(dc , rcClient.Width()-(iWidth-rcMid.left) , 0 , rcMid.Width() , rcMid.top , iOffset+rcMid.left , 0) ;
		m_imgBallonBk.Draw(dc , rcMid.left , 0 , rcClient.Width()-iWidth , rcMid.top , iOffset+rcMid.left+1 , 0 , 1 , rcMid.top);
		break;
	default:
		m_imgBallonBk.Draw(dc , rcMid.left , 0 , rcClient.Width()-(iWidth-rcMid.Width()) , rcMid.top , iOffset + rcMid.left , 0 , 1 , rcMid.top);
	}

	
	switch(dwIndex)
	{	// right
	case 6:
		break;
	case 8:
		break;
	default:
		m_imgBallonBk.Draw(dc , rcClient.Width()-(iWidth-rcMid.right)-1 , rcMid.top , iWidth-rcMid.right , rcClient.Height()-(iHeight-rcMid.Height()) , iOffset + rcMid.right-1 , rcMid.top , iWidth-rcMid.right , rcMid.Height());
	}

	switch(dwIndex)
	{ //bottom
	case 1:
		m_imgBallonBk.Draw(dc , rcMid.left ,  rcClient.Height()-(iHeight-rcMid.bottom) , rcMid.Width() , (iHeight-rcMid.bottom) , iOffset+rcMid.left , rcMid.bottom );
		m_imgBallonBk.Draw(dc , rcMid.right , rcClient.Height()-(iHeight-rcMid.bottom) , rcClient.Width()-iWidth , iHeight-rcMid.bottom , iOffset+rcMid.right-1 , rcMid.bottom , 1,iHeight-rcMid.bottom);
		break;
	case 2:
		m_imgBallonBk.Draw(dc , rcClient.Width()-(iWidth-rcMid.left) ,  rcClient.Height()-(iHeight-rcMid.bottom) , rcMid.Width() , (iHeight-rcMid.bottom)  , iOffset+rcMid.left , rcMid.bottom ) ;
		m_imgBallonBk.Draw(dc , rcMid.left , rcClient.Height()-(iHeight-rcMid.bottom) , rcClient.Width()-iWidth , iHeight-rcMid.bottom  , iOffset+rcMid.left , rcMid.bottom , 1 , iHeight-rcMid.bottom ) ;
		break;
	default:
		m_imgBallonBk.Draw(dc , rcMid.left , rcClient.Height()-(iHeight-rcMid.bottom) , rcClient.Width()-(iWidth-rcMid.Width()) , (iHeight-rcMid.bottom)  ,iOffset+rcMid.left , rcMid.bottom , 1 , iHeight-rcMid.bottom ) ;
	}

}

void CToolTipSkin::DrawCloseBtn(HDC hdc , RECT rc , DWORD dwIndex)
{
	CSkinManager::DrawImagePart(hdc , rc , m_imgCloseBtn , dwIndex , 3) ;
}

void CToolTipSkin::DrawCheckBox(HDC hdc , RECT rc , DWORD dwIndex)
{
	CSkinManager::DrawImagePart(hdc , rc , m_imgCheckBox , dwIndex , 8) ;
}

void CToolTipSkin::DrawIcon(CDCHandle dc , HICON hIcon , const CRect& rc)
{
	if (IsInnerIcon(hIcon) )
	{
		if ( IsInnerIconSmall(hIcon) )
			CSkinManager::DrawImagePart(dc, rc, m_imgBalloonIcon, -(int)hIcon - 1, BALLOON_SPECIAL_ICON_COUNT);
	}
	else
		dc.DrawIconEx(rc.left , rc.top , hIcon , rc.Width() , rc.Height()) ;
}

SIZE CToolTipSkin::GetIconSizeByIconType(HICON hIcon)
{
	if (hIcon == NULL)
		return CSize(0, 0);
	else if (IsInnerIconSmall(hIcon))
		return CSize(m_imgBalloonIcon.GetWidth() / BALLOON_SPECIAL_ICON_COUNT, m_imgBalloonIcon.GetHeight());
	else
		ATLASSERT(0);
	return CSize(0, 0);
}

int CToolTipSkin::TriangleOffset(int iFoward /* = 0 */ , bool bBalloon /*= true*/)
{
	return bBalloon?16:0;

	switch(iFoward)
	{
	case 1:
		return bBalloon?31:16;
	case 2:
		return bBalloon?32:17;
	case 3:
		return bBalloon?31:16;
	case 4:
		return bBalloon?32:17;
	case 5:
		return bBalloon?30:14;
	case 6:
		return bBalloon?30:14;
	case 7:
		return bBalloon?35:20;
	case 8:
		return bBalloon?35:20;
	default:
		return 0;
		break ;
	}
	
}