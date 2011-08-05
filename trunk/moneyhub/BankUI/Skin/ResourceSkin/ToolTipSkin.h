#pragma once
#include "../TuoImage.h"

class CSkinManager;

#define CHAR_HEIGHT	13

#define IsInnerIcon(hicon) (((int)hicon)<0&&((int)hicon)>=-20)
#define IsInnerIconSmall(hicon) (((int)hicon)<0&&((int)hicon)>=-10)
#define IsInnerIconLarge(hicon) (((int)hicon)<-10&&((int)hicon)>=-20)

class CToolTipSkin
{

public:

	CToolTipSkin(CSkinManager *pSkin);

	void Refresh();

	int TriangleOffset(int iFoward = 0 , bool bBalloon=true);// { return m_iTriangleOffset ; };
	int BorderThick() { return m_iBorderThick ; }
	int TriangleWidth() { return m_iTriangleWidth ; }
	int MaxWidth() { return m_iMaxWidth ; }
	SIZE GetIconSizeByIconType(HICON hIcon);

	RECT DefaultTipRect() { return m_rcDefault ; }
	HFONT GetTitleFont() {return m_fontTitle;}
	HFONT GetTextFont(){return m_fontText;}
	RECT CalcTextRect(HDC hdc , LPCTSTR lpText   , int iRawSpacing = -1) ;
	void DrawBalloonBack(CDCHandle dc , CRect rcClient , DWORD dwIndex ) ;
	void DrawCloseBtn(HDC hdc , RECT rc , DWORD dwIndex) ;
	void DrawCheckBox(HDC hdc , RECT rc , DWORD dwIndex) ;
	void DrawIcon(CDCHandle dc , HICON hIcon , const CRect& rc) ;

private:

	int m_iTriangleOffset ;
	int m_iBorderThick ;
	int m_iTriangleWidth ;
	RECT m_rcDefault ;
	int m_iMaxWidth ;
	int m_iRowSpacing ;

	CFont m_fontText ;
	CFont m_fontTitle ;


public:

	CTuoImage m_imgCloseBtn;
	CTuoImage m_imgBallonBk;
	CTuoImage m_imgCheckBox;
	CTuoImage m_imgBalloonIcon;
};
