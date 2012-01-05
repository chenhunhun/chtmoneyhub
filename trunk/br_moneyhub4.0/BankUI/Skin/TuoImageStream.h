#pragma once



class CTuoImageStream : public IStream, public Gdiplus::IImageBytes
{

public:

	CTuoImageStream(void *pData, DWORD dwSize) : m_pMappingData((BYTE*)pData), m_iCurrentPos(0), m_iSize(dwSize) {}


	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		if (riid == __uuidof(IUnknown) || riid == __uuidof(IStream) || riid == __uuidof(ISequentialStream))
		{
			*ppvObject = static_cast<IStream*>(this);
			return S_OK;
		}
		else if (riid == __uuidof(IImageBytes))
		{
			*ppvObject = static_cast<IImageBytes*>(this);
			return S_OK;
		}
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE; 
		}
	}
	ULONG STDMETHODCALLTYPE AddRef() { return 1; }
	ULONG STDMETHODCALLTYPE Release() { return 1; }

	HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead)
	{
		if (m_iCurrentPos + cb > m_iSize)
			cb = (ULONG)(m_iSize - m_iCurrentPos);
		memcpy(pv, m_pMappingData + m_iCurrentPos, cb);
		m_iCurrentPos += cb;
		if (pcbRead)
			*pcbRead = cb;
		return 0;
	}

	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
		switch (dwOrigin)
		{
		case STREAM_SEEK_SET:
			m_iCurrentPos = 0;
			break;
		case STREAM_SEEK_END:
			m_iCurrentPos = m_iSize;
			break;
		case STREAM_SEEK_CUR:
			break;
		default:
			ATLASSERT(0);
		}
		m_iCurrentPos += dlibMove.QuadPart;
		if (plibNewPosition)
			plibNewPosition->QuadPart = m_iCurrentPos;
		if (m_iCurrentPos < 0)
			m_iCurrentPos = 0;
		else if (m_iCurrentPos > m_iSize)
			m_iCurrentPos = m_iSize;
		return 0;
	}

	HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		pstatstg->cbSize.QuadPart = m_iSize;
		pstatstg->grfMode = STGM_SIMPLE;
		return 0;
	}

	HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten) { ATLASSERT(0); return 0; }
	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) { ATLASSERT(0); return 0; }
	HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) { ATLASSERT(0); return 0; }
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) { ATLASSERT(0); return 0; }
	HRESULT STDMETHODCALLTYPE Revert() { ATLASSERT(0); return 0; }
	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) { ATLASSERT(0); return 0; }
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) { ATLASSERT(0); return 0; }
	HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm) { ATLASSERT(0); return 0; }


	HRESULT STDMETHODCALLTYPE CountBytes(UINT *pcb)
	{
		*pcb = (UINT)m_iSize;
		return 0;
	}

	HRESULT STDMETHODCALLTYPE LockBytes(UINT cb, ULONG ulOffset, const VOID **ppvBytes)
	{
		*ppvBytes = m_pMappingData + ulOffset;
		return 0;
	}
	HRESULT STDMETHODCALLTYPE UnlockBytes(const VOID *pvBytes, UINT cb, ULONG ulOffset) { return 0; }

private:

	BYTE *m_pMappingData;
	LONGLONG m_iCurrentPos, m_iSize;
};
