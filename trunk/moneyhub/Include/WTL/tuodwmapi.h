#pragma once


#define WM_DWMSENDICONICTHUMBNAIL           0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP   0x0326
#define WM_DWMCOMPOSITIONCHANGED			0x031E



#define DWM_TNP_RECTDESTINATION       0x00000001
#define DWM_TNP_RECTSOURCE            0x00000002
#define DWM_TNP_OPACITY               0x00000004
#define DWM_TNP_VISIBLE               0x00000008
#define DWM_TNP_SOURCECLIENTAREAONLY  0x00000010


// Window attributes
enum _DWMWINDOWATTRIBUTE
{
	DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
	DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
	DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
	DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
	DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
	DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
	DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
	DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
	DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
	DWMWA_HAS_ICONIC_BITMAP,            // [set] Indicates an available bitmap when there is no better thumbnail representation.
	DWMWA_DISALLOW_PEEK,                // [set] Don't invoke Peek on the window.
	DWMWA_EXCLUDED_FROM_PEEK,           // [set] LivePreview exclusion information
	DWMWA_LAST
};


enum _THUMBBUTTONFLAGS
{
	_THBF_ENABLED		= 0,
	_THBF_DISABLED		= 0x1,
	_THBF_DISMISSONCLICK	= 0x2,
	_THBF_NOBACKGROUND	= 0x4,
	_THBF_HIDDEN			= 0x8,
	_THBF_NONINTERACTIVE	= 0x10
};


enum _THUMBBUTTONMASK
{
	_THB_BITMAP	= 0x1,
	_THB_ICON	= 0x2,
	_THB_TOOLTIP	= 0x4,
	_THB_FLAGS	= 0x8
};


enum _TBPFLAG
{
	_TBPF_NOPROGRESS	= 0,
	_TBPF_INDETERMINATE	= 0x1,
	_TBPF_NORMAL	= 0x2,
	_TBPF_ERROR	= 0x4,
	_TBPF_PAUSED	= 0x8
};


enum _STPFLAG
{
	_STPF_NONE	= 0,
	_STPF_USEAPPTHUMBNAILALWAYS	= 0x1,
	_STPF_USEAPPTHUMBNAILWHENACTIVE	= 0x2,
	_STPF_USEAPPPEEKALWAYS	= 0x4,
	_STPF_USEAPPPEEKWHENACTIVE	= 0x8
};

enum _DWMNCRENDERINGPOLICY
{
	_DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
	_DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
	_DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
	_DWMNCRP_LAST
};


typedef HANDLE HTHUMBNAIL;

struct _DWM_THUMBNAIL_PROPERTIES
{
	DWORD dwFlags;
	RECT rcDestination;
	RECT rcSource;
	BYTE opacity;
	BOOL fVisible;
	BOOL fSourceClientAreaOnly;
};


struct _TMARGINS
{
	int cxLeftWidth;      // width of left border that retains its size
	int cxRightWidth;     // width of right border that retains its size
	int cyTopHeight;      // height of top border that retains its size
	int cyBottomHeight;   // height of bottom border that retains its size
};



#include <pshpack8.h>
typedef struct _THUMBBUTTON
{
	_THUMBBUTTONMASK dwMask;
	UINT iId;
	UINT iBitmap;
	HICON hIcon;
	WCHAR szTip[ 260 ];
	_THUMBBUTTONFLAGS dwFlags;
} 	_THUMBBUTTON;

typedef struct _THUMBBUTTON *_LPTHUMBBUTTON;

#include <poppack.h>


MIDL_INTERFACE("ea1afb91-9e28-4b86-90e9-9e9f8a5eefaf")
_ITaskbarList3 : public ITaskbarList2
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetProgressValue(HWND hwnd, ULONGLONG ullCompleted, ULONGLONG ullTotal) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProgressState( HWND hwnd, _TBPFLAG tbpFlags) = 0;
	virtual HRESULT STDMETHODCALLTYPE RegisterTab(HWND hwndTab, HWND hwndMDI) = 0;
	virtual HRESULT STDMETHODCALLTYPE UnregisterTab(HWND hwndTab) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetTabOrder(HWND hwndTab, HWND hwndInsertBefore) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetTabActive(HWND hwndTab, HWND hwndMDI, DWORD dwReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons(HWND hwnd, UINT cButtons, _LPTHUMBBUTTON pButton) = 0;
	virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons(HWND hwnd, UINT cButtons, _LPTHUMBBUTTON pButton) = 0;
	virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList( HWND hwnd, HIMAGELIST himl) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon(HWND hwnd, HICON hIcon, LPCWSTR pszDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip(HWND hwnd, LPCWSTR pszTip) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip(HWND hwnd, RECT *prcClip) = 0;
};


MIDL_INTERFACE("c43dc798-95d1-4bea-9030-bb99e2983a1a")
_ITaskbarList4 : public _ITaskbarList3
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetTabProperties(HWND hwndTab, _STPFLAG stpFlags) = 0;
};


//////////////////////////////////////////////////////////////////////////

struct _PROPERTYKEY
{
	GUID fmtid;
	DWORD pid;
};


MIDL_INTERFACE("886d8eeb-8cf2-4446-8d02-cdba1dbdcf99")
_IPropertyStore : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetCount(DWORD *cProps) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAt(DWORD iProp, _PROPERTYKEY *pkey) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetValue(const _PROPERTYKEY &key, PROPVARIANT *pv) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetValue(const _PROPERTYKEY &key, const PROPVARIANT &propvar) = 0;
	virtual HRESULT STDMETHODCALLTYPE Commit() = 0;
};

//////////////////////////////////////////////////////////////////////////


#define DECLARE_FUNC_PTR(functype, ptr, name) \
	if (!CheckDWMAPI()) \
		return 0; \
	static functype ptr = NULL; \
	if (ptr == (functype)-1) \
		return 0; \
	if (ptr == NULL) \
	{ \
		ptr = (functype)::GetProcAddress(g_hDWM, name); \
		if (ptr == NULL) \
		{ \
			ptr = (functype)-1; \
			return 0; \
		} \
	}

__declspec(selectany) HMODULE g_hDWM = NULL;


inline bool CheckDWMAPI()
{
	if (g_hDWM == (HMODULE)-1)
		return false;
	if (g_hDWM == NULL)
	{
		TCHAR szDLLPath[MAX_PATH];
		DWORD dwPathLen = ::SearchPath(NULL, _T("dwmapi.dll"), NULL, _countof(szDLLPath), szDLLPath, NULL);
		if (dwPathLen == 0)
		{
			g_hDWM = (HMODULE)-1;
			return false;
		}
		WIN32_FIND_DATA FindData;
		HANDLE hFindFile = ::FindFirstFile(szDLLPath, &FindData);
		if (hFindFile == INVALID_HANDLE_VALUE)
		{
			g_hDWM = (HMODULE)-1;
			return false;
		}
		::FindClose(hFindFile);

		g_hDWM = ::LoadLibrary(_T("dwmapi.dll"));
		if (g_hDWM == NULL)
		{
			g_hDWM = (HMODULE)-1;
			return false;
		}
	}
	return true;
}

inline HRESULT TuoDwmRegisterThumbnail(HWND hwndDestination, HWND hwndSource, HTHUMBNAIL *phThumbnailId)
{
	typedef HRESULT (WINAPI * f)(HWND hwndDestination, HWND hwndSource, HTHUMBNAIL *phThumbnailId);
	DECLARE_FUNC_PTR(f, p, "DwmRegisterThumbnail")
	return p(hwndDestination, hwndSource, phThumbnailId);
}

inline HRESULT TuoDwmUpdateThumbnailProperties(HTHUMBNAIL hThumbnailId, const _DWM_THUMBNAIL_PROPERTIES* ptnProperties)
{
	typedef HRESULT (WINAPI * f)(HTHUMBNAIL hThumbnailId, const _DWM_THUMBNAIL_PROPERTIES* ptnProperties);
	DECLARE_FUNC_PTR(f, p, "DwmUpdateThumbnailProperties")
	return p(hThumbnailId, ptnProperties);
}

inline HRESULT TuoDwmUnregisterThumbnail(HTHUMBNAIL hThumbnailId)
{
	typedef HRESULT (WINAPI * f)(HTHUMBNAIL hThumbnailId);
	DECLARE_FUNC_PTR(f, p, "DwmUnregisterThumbnail")
	return p(hThumbnailId);
}


inline HRESULT TuoDwmSetIconicThumbnail(HWND hwnd, HBITMAP hbmp, DWORD dwSITFlags)
{
	typedef HRESULT (WINAPI * f)(HWND hwnd, HBITMAP hbmp, DWORD dwSITFlags);
	DECLARE_FUNC_PTR(f, p, "DwmSetIconicThumbnail")
	return p(hwnd, hbmp, dwSITFlags);
}

inline HRESULT TuoDwmSetIconicLivePreviewBitmap(HWND hwnd, HBITMAP hbmp, POINT *pptClient, DWORD dwSITFlags)
{
	typedef HRESULT (WINAPI * f)(HWND hwnd, HBITMAP hbmp, POINT *pptClient, DWORD dwSITFlags);
	DECLARE_FUNC_PTR(f, p, "DwmSetIconicLivePreviewBitmap")
	return p(hwnd, hbmp, pptClient, dwSITFlags);
}


inline HRESULT TuoDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
	typedef HRESULT (WINAPI * f)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
	DECLARE_FUNC_PTR(f, p, "DwmSetWindowAttribute")
	return p(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

inline HRESULT TuoDwmInvalidateIconicBitmaps(HWND hwnd)
{
	typedef HRESULT (WINAPI * f)(HWND hwnd);
	DECLARE_FUNC_PTR(f, p, "DwmInvalidateIconicBitmaps")
	return p(hwnd);
}


inline HRESULT TuoDwmExtendFrameIntoClientArea(HWND hWnd, const _TMARGINS *pMarInset)
{
	typedef HRESULT (WINAPI * f)(HWND hWnd, const _TMARGINS *pMarInset);
	DECLARE_FUNC_PTR(f, p, "DwmExtendFrameIntoClientArea")
	return p(hWnd, pMarInset);
}


inline BOOL TuoDwmDefWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
{
	typedef BOOL (WINAPI * f)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
	DECLARE_FUNC_PTR(f, p, "DwmDefWindowProc")
	return p(hwnd, msg, wParam, lParam, plResult);
}


inline bool IsAeroEffectEnabled()
{
	if (!CheckDWMAPI())
		return false;
	typedef HRESULT (WINAPI * f)(BOOL *pfEnabled);
	static f ptr = NULL;
	if (ptr == (f)-1)
		return false;
	if (ptr == NULL)
	{
		ptr = (f)::GetProcAddress(g_hDWM, "DwmIsCompositionEnabled");
		if (ptr == NULL)
		{
			ptr = (f)-1;
			return false;
		}
	}
	BOOL bEnabled = FALSE;
	ptr(&bEnabled);
	return bEnabled != FALSE;
}


#define MSGFLT_ADD		1


inline BOOL WINAPI TuoChangeWindowMessageFilter(UINT message, DWORD dwFlag)
{
	typedef BOOL (WINAPI * ChangeWindowMessageFilter)(UINT message, DWORD dwFlag);
	static ChangeWindowMessageFilter p = NULL;
	if (p == (ChangeWindowMessageFilter)-1)
		return FALSE;
	if (p == NULL)
	{
		p = (ChangeWindowMessageFilter)-1;
		HMODULE hUser32 = ::LoadLibrary(_T("user32.dll"));
		if (hUser32)
			p = (ChangeWindowMessageFilter)::GetProcAddress(hUser32, "ChangeWindowMessageFilter");
	}
	if (p)
		return p(message, dwFlag);
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

inline void SetWindowAppName(HWND hWnd, LPCTSTR lpszAppName)
{
	typedef HRESULT (WINAPI * SHGetPropertyStoreForWindow)(HWND hwnd, REFIID riid, void **ppv);
	static SHGetPropertyStoreForWindow f = NULL;
	if (f == (SHGetPropertyStoreForWindow)-1)
		return;

	if (f == NULL)
	{
		// win7下面把download窗口独立出来
		HMODULE hShell32 = ::LoadLibrary(_T("shell32.dll"));
		if (hShell32 == NULL)
		{
			f = (SHGetPropertyStoreForWindow)-1;
			return;
		}
		f = (SHGetPropertyStoreForWindow)::GetProcAddress(hShell32, "SHGetPropertyStoreForWindow");
		if (f == NULL)
		{
			f = (SHGetPropertyStoreForWindow)-1;
			return;
		}
	}

	static const _PROPERTYKEY PKEY_AppUserModel_ID = { { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 5 };
	PROPVARIANT pv;
	pv.vt = VT_BSTR;
	pv.bstrVal = (BSTR)lpszAppName;
	_IPropertyStore *pps;
	if (SUCCEEDED(f(hWnd, __uuidof(_IPropertyStore), (void**)&pps)))
	{
		pps->SetValue(PKEY_AppUserModel_ID, pv);
		pps->Release();
	}
}
