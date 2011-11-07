
#pragma once

class CAxControl;
class CExternalDispatchImpl;
class CInternetSecurityManagerImpl;
class CDownloadManagerImpl;

class CCustomSite : public IOleClientSite
	              , public IOleCommandTarget
				  , public IDocHostUIHandler2
				  , public IDocHostShowUI
				  , public IServiceProvider
				  , public IOleInPlaceSite
				  , public IOleInPlaceFrame

{
public :
	CCustomSite(CAxControl *pAxControl);
	~CCustomSite();

public :
	//IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//IOleClientSite
	HRESULT STDMETHODCALLTYPE SaveObject();
	HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
	HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer);
	HRESULT STDMETHODCALLTYPE ShowObject();
	HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();

	//IOleCommandTarget
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);

	//IDocHostUIHandler
	HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO *pInfo);
	HRESULT STDMETHODCALLTYPE ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
	HRESULT STDMETHODCALLTYPE HideUI();
	HRESULT STDMETHODCALLTYPE UpdateUI();
	HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
	HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate);
	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate);
	HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
	HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw);
	HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	HRESULT STDMETHODCALLTYPE GetExternal(IDispatch **ppDispatch);
	HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
	HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject *pDO, IDataObject **ppDORet);

	// IDocHostUIHandler2
	HRESULT STDMETHODCALLTYPE GetOverrideKeyPath(LPOLESTR *pchKey, DWORD dw);

	// IDocHostShowUI
	HRESULT STDMETHODCALLTYPE ShowMessage(HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT *plResult);
	HRESULT STDMETHODCALLTYPE ShowHelp(HWND hwnd, LPOLESTR pszHelpFile, UINT uCommand, DWORD dwData, POINT ptMouse, IDispatch *pDispatchObjectHit);

	//IServiceProvider
	HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, REFIID riid, void **ppvObject);

	// IOleWindow
	HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd);
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);

	// IOleInPlaceSite : IOleWindow
	HRESULT STDMETHODCALLTYPE CanInPlaceActivate();
	HRESULT STDMETHODCALLTYPE OnInPlaceActivate();
	HRESULT STDMETHODCALLTYPE OnUIActivate();
	HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant);
	HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate();
	HRESULT STDMETHODCALLTYPE DiscardUndoState();
	HRESULT STDMETHODCALLTYPE DeactivateAndUndo();
	HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect);

	// IOleInPlaceUIWindow
	HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder);
	HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
	HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
	HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName);

	// IOleInPlaceFrame
	HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
	HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
	HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared);
	HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText);
	HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID);

public:
	CAxControl *m_pAxControl;
	CExternalDispatchImpl *m_pExternalDispatchImpl;
	CInternetSecurityManagerImpl *m_pInternetSecurityManager;
};
