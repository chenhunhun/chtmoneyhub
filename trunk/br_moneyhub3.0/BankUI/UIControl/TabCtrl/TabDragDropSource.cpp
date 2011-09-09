#include "stdafx.h"
#include "TabCtrl.h"
#include "TabDragDropSource.h"
#include "../../Skin/SkinManager.h"


//////////////////////////////////////////////////////////////////////
// CIDataObject Class

CIDataObject::CIDataObject(CIDropSource* pDropSource) : m_cRefCount(0), m_pDropSource(pDropSource)
{}

CIDataObject::~CIDataObject()
{
	for (int i = 0; i < m_StgMedium.GetSize(); i++)
	{
		ReleaseStgMedium(m_StgMedium[i]);
		delete m_StgMedium[i];
	}
	for (int j = 0; j < m_ArrFormatEtc.GetSize(); j++)
		delete m_ArrFormatEtc[j];
}

STDMETHODIMP CIDataObject::QueryInterface(/* [in] */ REFIID riid,
										  /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = NULL;
	if (IID_IUnknown==riid || IID_IDataObject==riid)
		*ppvObject=this;
	/*if(riid == IID_IAsyncOperation)
	*ppvObject=(IAsyncOperation*)this;*/
	if (NULL!=*ppvObject)
	{
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CIDataObject::AddRef( void)
{
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CIDataObject::Release( void)
{
	long nTemp;
	nTemp = --m_cRefCount;
	if(nTemp==0)
		delete this;
	return nTemp;
}

STDMETHODIMP CIDataObject::GetData( 
								   /* [unique][in] */ FORMATETC __RPC_FAR *pformatetcIn,
								   /* [out] */ STGMEDIUM __RPC_FAR *pmedium)
{
	if(pformatetcIn == NULL || pmedium == NULL)
		return E_INVALIDARG;
	pmedium->hGlobal = NULL;

	ATLASSERT(m_StgMedium.GetSize() == m_ArrFormatEtc.GetSize());
	for(int i=0; i < m_ArrFormatEtc.GetSize(); ++i)
	{
		if(pformatetcIn->tymed & m_ArrFormatEtc[i]->tymed &&
			pformatetcIn->dwAspect == m_ArrFormatEtc[i]->dwAspect &&
			pformatetcIn->cfFormat == m_ArrFormatEtc[i]->cfFormat)
		{
			CopyMedium(pmedium, m_StgMedium[i], m_ArrFormatEtc[i]);
			return S_OK;
		}
	}
	return DV_E_FORMATETC;
}

STDMETHODIMP CIDataObject::GetDataHere( 
									   /* [unique][in] */ FORMATETC __RPC_FAR *pformatetc,
									   /* [out][in] */ STGMEDIUM __RPC_FAR *pmedium)
{ 
	return E_NOTIMPL;
}

STDMETHODIMP CIDataObject::QueryGetData( 
										/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc)
{
	if(pformatetc == NULL)
		return E_INVALIDARG;

	//support others if needed DVASPECT_THUMBNAIL  //DVASPECT_ICON   //DVASPECT_DOCPRINT
	if (!(DVASPECT_CONTENT & pformatetc->dwAspect))
		return (DV_E_DVASPECT);
	HRESULT  hr = DV_E_TYMED;
	for(int i = 0; i < m_ArrFormatEtc.GetSize(); ++i)
	{
		if(pformatetc->tymed & m_ArrFormatEtc[i]->tymed)
		{
			if(pformatetc->cfFormat == m_ArrFormatEtc[i]->cfFormat)
				return S_OK;
			else
				hr = DV_E_CLIPFORMAT;
		}
		else
			hr = DV_E_TYMED;
	}
	return hr;
}

STDMETHODIMP CIDataObject::GetCanonicalFormatEtc( 
	/* [unique][in] */ FORMATETC __RPC_FAR *pformatectIn,
	/* [out] */ FORMATETC __RPC_FAR *pformatetcOut)
{ 
	if (pformatetcOut == NULL)
		return E_INVALIDARG;
	return DATA_S_SAMEFORMATETC;
}

STDMETHODIMP CIDataObject::SetData( 
								   /* [unique][in] */ FORMATETC __RPC_FAR *pformatetc,
								   /* [unique][in] */ STGMEDIUM __RPC_FAR *pmedium,
								   /* [in] */ BOOL fRelease)
{ 
	if(pformatetc == NULL || pmedium == NULL)
		return E_INVALIDARG;

	ATLASSERT(pformatetc->tymed == pmedium->tymed);
	FORMATETC* fetc=new FORMATETC;
	STGMEDIUM* pStgMed = new STGMEDIUM;

	if(fetc == NULL || pStgMed == NULL)
		return E_OUTOFMEMORY;

	ZeroMemory(fetc,sizeof(FORMATETC));
	ZeroMemory(pStgMed,sizeof(STGMEDIUM));

	*fetc = *pformatetc;
	m_ArrFormatEtc.Add(fetc);

	if(fRelease)
		*pStgMed = *pmedium;
	else
	{
		CopyMedium(pStgMed, pmedium, pformatetc);
	}
	m_StgMedium.Add(pStgMed);

	return S_OK;
}
void CIDataObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
{
	switch(pMedSrc->tymed)
	{
	case TYMED_HGLOBAL:
		pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_GDI:
		pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_MFPICT:
		pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_ENHMF:
		pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_FILE:
		pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName,pFmtSrc->cfFormat, NULL);
		break;
	case TYMED_ISTREAM:
		pMedDest->pstm = pMedSrc->pstm;
		pMedSrc->pstm->AddRef();
		break;
	case TYMED_ISTORAGE:
		pMedDest->pstg = pMedSrc->pstg;
		pMedSrc->pstg->AddRef();
		break;
	case TYMED_NULL:
	default:
		break;
	}
	pMedDest->tymed = pMedSrc->tymed;
	pMedDest->pUnkForRelease = NULL;
	if(pMedSrc->pUnkForRelease != NULL)
	{
		pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
		pMedSrc->pUnkForRelease->AddRef();
	}
}
STDMETHODIMP CIDataObject::EnumFormatEtc(
	/* [in] */ DWORD dwDirection,
	/* [out] */ IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenumFormatEtc)
{
	if(ppenumFormatEtc == NULL)
		return E_POINTER;

	*ppenumFormatEtc=NULL;
	switch (dwDirection)
	{
	case DATADIR_GET:
		*ppenumFormatEtc= new CEnumFormatEtc(m_ArrFormatEtc);
		if(*ppenumFormatEtc == NULL)
			return E_OUTOFMEMORY;
		(*ppenumFormatEtc)->AddRef(); 
		break;

	case DATADIR_SET:
	default:
		return E_NOTIMPL;
		break;
	}

	return S_OK;
}

STDMETHODIMP CIDataObject::DAdvise( 
								   /* [in] */ FORMATETC __RPC_FAR *pformatetc,
								   /* [in] */ DWORD advf,
								   /* [unique][in] */ IAdviseSink __RPC_FAR *pAdvSink,
								   /* [out] */ DWORD __RPC_FAR *pdwConnection)
{ 
	return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP CIDataObject::DUnadvise( 
									 /* [in] */ DWORD dwConnection)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CIDataObject::EnumDAdvise( 
	/* [out] */ IEnumSTATDATA __RPC_FAR *__RPC_FAR *ppenumAdvise)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

//////////////////////////////////////////////////////////////////////
// CEnumFormatEtc Class
//////////////////////////////////////////////////////////////////////

CEnumFormatEtc::CEnumFormatEtc(const CSimpleArray<FORMATETC>& ArrFE):
m_cRefCount(0),m_iCur(0)
{
	for(int i = 0; i < ArrFE.GetSize(); ++i)
		m_pFmtEtc.Add(ArrFE[i]);
}

CEnumFormatEtc::CEnumFormatEtc(const CSimpleArray<FORMATETC*>& ArrFE):
m_cRefCount(0),m_iCur(0)
{
	for(int i = 0; i < ArrFE.GetSize(); ++i)
		m_pFmtEtc.Add(*ArrFE[i]);
}

STDMETHODIMP  CEnumFormatEtc::QueryInterface(REFIID refiid, void FAR* FAR* ppv)
{
	*ppv = NULL;
	if (IID_IUnknown==refiid || IID_IEnumFORMATETC==refiid)
		*ppv=this;

	if (*ppv != NULL)
	{
		((LPUNKNOWN)*ppv)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CEnumFormatEtc::AddRef(void)
{
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CEnumFormatEtc::Release(void)
{
	long nTemp = --m_cRefCount;
	ATLASSERT(nTemp >= 0);
	if(nTemp == 0)
		delete this;

	return nTemp; 
}

STDMETHODIMP CEnumFormatEtc::Next( ULONG celt,LPFORMATETC lpFormatEtc, ULONG FAR *pceltFetched)
{
	if(pceltFetched != NULL)
		*pceltFetched=0;

	ULONG cReturn = celt;

	if(celt <= 0 || lpFormatEtc == NULL || m_iCur >= m_pFmtEtc.GetSize())
		return S_FALSE;

	if(pceltFetched == NULL && celt != 1) // pceltFetched can be NULL only for 1 item request
		return S_FALSE;

	while (m_iCur < m_pFmtEtc.GetSize() && cReturn > 0)
	{
		*lpFormatEtc++ = m_pFmtEtc[m_iCur++];
		--cReturn;
	}
	if (pceltFetched != NULL)
		*pceltFetched = celt - cReturn;

	return (cReturn == 0) ? S_OK : S_FALSE;
}

STDMETHODIMP CEnumFormatEtc::Skip(ULONG celt)
{
	if((m_iCur + int(celt)) >= m_pFmtEtc.GetSize())
		return S_FALSE;
	m_iCur += celt;
	return S_OK;
}

STDMETHODIMP CEnumFormatEtc::Reset(void)
{
	m_iCur = 0;
	return S_OK;
}

STDMETHODIMP CEnumFormatEtc::Clone(IEnumFORMATETC FAR * FAR*ppCloneEnumFormatEtc)
{
	if(ppCloneEnumFormatEtc == NULL)
		return E_POINTER;

	CEnumFormatEtc *newEnum = new CEnumFormatEtc(m_pFmtEtc);
	if(newEnum ==NULL)
		return E_OUTOFMEMORY;  	
	newEnum->AddRef();
	newEnum->m_iCur = m_iCur;
	*ppCloneEnumFormatEtc = newEnum;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// CIDropSource Class
//////////////////////////////////////////////////////////////////////

STDMETHODIMP CIDropSource::QueryInterface(/* [in] */ REFIID riid,
										  /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = NULL;
	if (IID_IUnknown==riid || IID_IDropSource==riid)
		*ppvObject=this;

	if (*ppvObject != NULL)
	{
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CIDropSource::AddRef( void)
{
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CIDropSource::Release( void)
{
	long nTemp;
	nTemp = --m_cRefCount;
	ATLASSERT(nTemp >= 0);
	if(nTemp==0)
		delete this;
	return nTemp;
}

STDMETHODIMP CIDropSource::QueryContinueDrag( 
	/* [in] */ BOOL fEscapePressed,
	/* [in] */ DWORD grfKeyState)
{
	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;
	if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON)))
	{
		m_bDropped = true;
		return DRAGDROP_S_DROP;
	}

//	CTabDraggingWindow::_()->ShowOrHideTemporary(CTabDraggingWindow::_()->GetParam() == 1);
	return S_OK;

}

STDMETHODIMP CIDropSource::GiveFeedback(DWORD dwEffect)
{
//	CTabDraggingWindow::_()->ShowOrHideTemporary(CTabDraggingWindow::_()->GetParam() == 1);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// CTabDropTarget Class
//////////////////////////////////////////////////////////////////////

CTabDropTarget::CTabDropTarget(HWND hTargetWnd) : m_hTargetWnd(hTargetWnd), m_bAllowDrop(false)
{
	//HRESULT hr = ::CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (LPVOID*)&m_pDropTargetHelper);
	//ATLASSERT(SUCCEEDED(hr));

	m_SupportedFrmt.ptd = NULL;
	m_SupportedFrmt.cfFormat = CF_TEXT;
	m_SupportedFrmt.dwAspect = DVASPECT_CONTENT;
	m_SupportedFrmt.lindex = -1;
	m_SupportedFrmt.tymed = TYMED_HGLOBAL;
}


bool CTabDropTarget::QueryDrop(DWORD grfKeyState, LPDWORD pdwEffect)
{
	DWORD dwOKEffects = *pdwEffect;
	if (!m_bAllowDrop)
	{
		*pdwEffect = DROPEFFECT_NONE;
		return false;
	}

	*pdwEffect = DROPEFFECT_MOVE;

	return (DROPEFFECT_NONE == *pdwEffect)?false:true;
}   

HRESULT STDMETHODCALLTYPE CTabDropTarget::DragEnter(
	/* [unique][in] */ IDataObject __RPC_FAR *pDataObj,
	/* [in] */ DWORD grfKeyState,
	/* [in] */ POINTL pt,
	/* [out][in] */ DWORD __RPC_FAR *pdwEffect)
{
	if(pDataObj == NULL)
		return E_INVALIDARG;

	if(m_pDropTargetHelper)
		m_pDropTargetHelper->DragEnter(m_hTargetWnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
	m_bAllowDrop = pDataObj->QueryGetData(&m_SupportedFrmt) == S_OK;

	STGMEDIUM medium;
	if (pDataObj->GetData(&m_SupportedFrmt, &medium) == S_OK)
	{
		if (m_SupportedFrmt.cfFormat == CF_TEXT && medium.tymed == TYMED_HGLOBAL)
		{
			char *pMem = (char*)::GlobalLock(medium.hGlobal);
			int iLen = ::GlobalSize(medium.hGlobal);
			size_t iTextLen = strlen(pMem);
			if (iTextLen + sizeof(DragGlobalData) + 1 == iLen)
			{
//				CTabDraggingWindow::_()->SetParam(2);
				DragGlobalData *pData = (DragGlobalData*)(pMem + iLen - sizeof(DragGlobalData));
				if (pData->dwProcessID != ::GetCurrentProcessId())
					m_bAllowDrop = false;
				else
					SendMessage(m_hTargetWnd, WM_TAB_OLE_SET_DRAGGING_ITEM, (WPARAM)pData, 0);
			}
			else
				m_bAllowDrop = false;
			::GlobalUnlock(medium.hGlobal);
		}
	}
	QueryDrop(grfKeyState, pdwEffect);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTabDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
	QueryDrop(grfKeyState, pdwEffect);
	if (m_bAllowDrop)
		SendMessage(m_hTargetWnd, WM_TAB_OLE_DRAG_OVER, 0, MAKELPARAM(pt.x, pt.y));
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTabDropTarget::DragLeave()
{
	m_bAllowDrop = false;
	DragGlobalData data = { NULL, 0 };
	SendMessage(m_hTargetWnd, WM_TAB_OLE_SET_DRAGGING_ITEM, (WPARAM)&data, 0);
//	CTabDraggingWindow::_()->SetParam(1);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTabDropTarget::Drop(
	/* [unique][in] */ IDataObject __RPC_FAR *pDataObj,
	/* [in] */ DWORD grfKeyState, /* [in] */ POINTL pt, 
	/* [out][in] */ DWORD __RPC_FAR *pdwEffect)
{
	if (pDataObj == NULL)
		return E_INVALIDARG;	

	if(m_pDropTargetHelper)
		m_pDropTargetHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);

	if(QueryDrop(grfKeyState, pdwEffect))
	{
		if(m_bAllowDrop)
		{
			STGMEDIUM medium;
			if(pDataObj->GetData(&m_SupportedFrmt, &medium) == S_OK)
			{
				if(m_SupportedFrmt.cfFormat == CF_TEXT && medium.tymed == TYMED_HGLOBAL)
				{
					char *pMem = (char*)::GlobalLock(medium.hGlobal);
					int iLen = ::GlobalSize(medium.hGlobal);
					DragGlobalData *pData = (DragGlobalData*)(pMem + iLen - sizeof(DragGlobalData));
					*pData->hDragToFrame = ::GetAncestor(m_hTargetWnd, GA_ROOTOWNER);
					::GlobalUnlock(medium.hGlobal);
					::SendMessage(m_hTargetWnd, WM_TAB_OLE_SET_DRAG_TO, (WPARAM)pData, 0);
				}
			}
		}
	}
	m_bAllowDrop=false;
	*pdwEffect = DROPEFFECT_NONE;
	return S_OK;
}


HRESULT CTabDropTarget::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	*ppvObject = NULL;
	if (IID_IUnknown == riid || IID_IDropTarget == riid)
		*ppvObject = this;
	if (*ppvObject != NULL)
		return S_OK;
	return E_NOINTERFACE;
}

ULONG CTabDropTarget::AddRef()
{
	return 1;
}

ULONG CTabDropTarget::Release()
{
	return 1;
}
