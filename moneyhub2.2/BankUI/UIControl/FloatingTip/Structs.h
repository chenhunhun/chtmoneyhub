#pragma once
#ifndef _STRUCTORS_H
#define _STRUCTORS_H

#include <string>

enum eTimers
{
	eTimer_ToolTipDelayShow = 1 ,
	eTimer_BalloonHide ,
	eTimer_MouseLeave ,
	eTimer_FadeInOut ,
};

enum eCloseBtnState
{	
	eMouseOut = 0,
	eMouseOver ,
	eMouseDown
};

enum eCursor
{
	eCURSOR_DEFAULT = 0,
	eCURSOR_HAND
};

enum eToolTipFlag
{
	eTTF_CLOSE = 0x1 ,
	eTTF_BALLOON = 0x2 ,	// 汽球，有三角
	eTTF_ICON = 0x4 ,
	eTTF_TITLE = 0x8 ,
	eTTF_TEXTCALLBACK = 0x10 , 
	eTTF_CUSTOMDRAW = 0x20 ,	// not supported
	eTTF_INDEXCALLBACK = 0x40 ,
	eTTF_CHECKBOX = 0x80,	// 
	eTTF_ADVANCE = 0x100,	// 高级汽泡
	eTTF_ROBADVANCE = 0x200,	// 如果当前正在显示的是eTTF_ADVANCE,则把它强行转化为!eTTF_ADVANCE，并显示出来。
	eTTF_SHOWTIMEOUT = 0x400,
	eTTF_USERCLOSENOTIFY = 0x800, // 必须设置eTTF_BALLOON，如果用户点X，则通知hWnd窗口
	eTTF_HIDDENNOTIFY = 0x1000,  // 气泡消失
	eTTF_FADEIN = 0x2000,	// 是否淡入
	eTTF_FIXEDWIDTH = 0x4000,	// 固定汽泡宽度
	eTTF_FIXEDHEIGHT = 0x8000,   // 固定汽泡高度
	eTTF_BALLOONPOPDOWN = 0x10000, // 气泡默认是向上弹的，设置这个标志让它向下弹，专门为子进程崩溃做的。 
	eTTF_CHECKBOXTAKEEFFECTNOW = 0x20000	// 复选框更改后立即生效，即立马通知调用者
};

enum eCheckBoxState
{
	eCBS_UnChecked = 0 ,
	eCBS_UnCheckedHover ,
	eCBS_UnCheckedActive ,
	eCBS_UnCheckedDisable,
	eCBS_Checked,
	eCBS_CheckedHover,
	eCBS_CheckedActive,
	eCBS_CheckedDisable,

};

enum eForward
{
	eLeft = 1 ,
	eTop , 
	eRight ,
	eBottom
};



typedef struct  tag_QuadPixel
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char alpha;
} QUADPIXEL , *PQUADPIXEL ;

//////////////////////////////////////////////////////////////////////////
// NMSTTDISPINFO

#define STTDSPF_ICON		0x1
#define STTDSPF_TITLE		0x2
#define STTDSPF_TEXT		0x4
#define STTDSPF_INDEX		0x8

typedef struct tag_NMSTTDISPINFO
{
	NMHDR hdr ;
	DWORD dwFlags;
	HICON hIcon ;
	SIZE IconSize ;
	DWORD_PTR pdwIndex ;
	std::wstring szTitle;
	std::wstring szText;
} NMSTTDISPINFO , *LPNMSTTDISPINFO;

typedef struct tag_NMSTTGETINDEX
{
	NMHDR hdr ; 
	DWORD_PTR pdwIndex ;
} NMSTTINDEX , *LPNMSTTINDEX;

typedef struct tag_NMSTTNEEDSHOWTIP
{
	NMHDR hdr ;
	BOOL bShowTip ;
} NMSTTNEEDSHOWTIP , *LPNMSTTNEEDSHOWTIP;

typedef struct tag_NMSTTLINKCLICK
{
	NMHDR hdr ;
	LPCTSTR lpUrl ;
} NMSTTLINKCLICK;


typedef struct  tag_SogouToolTipInfo
{
	UINT uFlags ; 
	HICON hIcon ;
	HBITMAP hBmp ;
	SIZE IconSize ;
	SIZE BalloonSize ;		// 气泡大小是指定的，而不是自适应
	HWND hWnd ;	
	HWND hFollowWnd ;		// 气泡跟随哪个窗口走
	UINT uCallBackMsg ;
	DWORD dwCheckBoxState;
	UINT uDelay ;	// 延迟显示
	UINT uShowTimeOut ; // 等多久消失
	DWORD_PTR pdwIndex ;
	std::wstring wstrTitle ;
	std::wstring wstrText ;
	std::wstring wstrCheckBox ;
} SOGOUTOOLINFO ,*LPSOGOUTOOLINFO ;


// uCallBackMsg 消息
//		wParam:	通知的类型 TTCBMT_*
//		lParam:	DWORD_PTR	消息的值
#define TTCBMT_USERCLOSE		1	// 用户点了X，且设置了eTTF_USERCLOSENOTIFY
#define TTCBMT_CHECKBOXSTATE	2	// 有CheckBox，需要回调通知CheckBox的选中状态
#define TTCBMT_LINK				3	// 点了链接
#define TTCBMT_HIDDEN			4	// 气泡消失

typedef struct tag_ToolRect
{
	HWND hWnd ;
	UINT uFlags ;	// eTTF_TEXTCALLBACK , eTTF_ICON
	HICON hIcon ;
	SIZE IconSize ;
	DWORD_PTR pdwIndex ;
	RECT rc ;
	std::wstring wstrTitle ;
	std::wstring wstrText ;
} TOOLRECT , *LPTOOLRECT;



typedef struct tag_FadeInOut
{
	BOOL bOut ;
	BOOL bVaild ;
	int iPrecent ;
	int iStep ;
	SIZE bmpSize ;
	int iBufSize ;
	QUADPIXEL* pPixels ;
} FADEINOUT , *LPFADEINOUT;

#endif