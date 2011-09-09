#pragma once

class CWebBrowserCore
{
	friend class CCustomSite;
	friend class CWebBrowserCore;
	friend class CAxControl;

public:
	CWebBrowserCore();
	~CWebBrowserCore();

public:
	HRESULT CreateIEServer(HWND hAxWnd, IOleClientSite *pClientSite);
	void ChangeSize(int left, int top, int width, int height);
	void Release();

private:
	IOleObject *m_pOleObject;
	IOleInPlaceObjectWindowless *m_pOleInPlaceWindowLess;

public:
	IWebBrowser2 *m_pWebBrowser2;
};
