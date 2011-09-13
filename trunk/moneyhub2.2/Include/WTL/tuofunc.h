#pragma once


extern "C"
{
	struct CLIENT_ID
	{
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	};

	struct PEB;

	struct TEB
	{
		NT_TIB NtTib;
		PVOID EnvironmentPointer;
		CLIENT_ID ClientId;
		PVOID ActiveRpcHandle;
		PVOID ThreadLocalStoragePointer;
		PEB* ProcessEnvironmentBlock;
		// 后面的数据结构我不关心了
	};

	__inline TEB* TuoGetTeb()
	{
		__asm mov eax, fs:[0x18]
	}

	__inline void* TuoGetTLS()
	{
		return TuoGetTeb()->ThreadLocalStoragePointer;
	}
}


//////////////////////////////////////////////////////////////////////////



class CTuoPatchDraw
{

public:

	CTuoPatchDraw() : m_iWidth(0), m_iHeight(0) {}
	CTuoPatchDraw(int w, int h) : m_iWidth(w), m_iHeight(h)
	{
		m_iconCacheDC[0].CreateMemDC(w, h);
		m_iconCacheDC[1].CreateMemDC(w, h);
	}

	void CreatePatchDraw(int w, int h)
	{
		m_iWidth = w;
		m_iHeight = h;
		m_iconCacheDC[0].ChangeSize(w, h);
		m_iconCacheDC[1].ChangeSize(w, h);
	}

	void BeginDraw()
	{
		memset(m_iconCacheDC[0].m_pBits, 0x00, m_iWidth * m_iHeight * 4);
		memset(m_iconCacheDC[1].m_pBits, 0xff, m_iWidth * m_iHeight * 4);
	}

	void EndDraw()
	{
		BYTE *dst = m_iconCacheDC[0].m_pBits;
		BYTE *dstW = m_iconCacheDC[1].m_pBits;
		BYTE r, g, b, a, rw, gw, bw, aw, alpha_r, alpha_g, alpha_b, alpha;
		for (int y = 0; y < m_iHeight; y++)
		{
			for (int x = 0; x < m_iWidth; x++)
			{
				r = *dst++;
				g = *dst++;
				b = *dst++;
				a = *dst++;
				rw = *dstW++;
				gw = *dstW++;
				bw = *dstW++;
				aw = *dstW++;
				alpha_r = rw-r;
				alpha_g = gw-g;
				alpha_b = bw-b;
				alpha = (alpha_r + alpha_g + alpha_b) / 3;
				*(dst - 1) = 255 - alpha;
			}
		}
	}

	void AlphaBlend(HDC hDC, int iDestX, int iDestY)
	{
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0xff;
		bf.AlphaFormat = AC_SRC_ALPHA;
		::AlphaBlend(hDC, iDestX, iDestY, m_iWidth, m_iHeight, GetDC(0), 0, 0, m_iWidth, m_iHeight, bf);
	}

	HDC GetDC(int iIndex) const { return m_iconCacheDC[iIndex]; }

private:

	CLayeredMemDC m_iconCacheDC[2];
	int m_iWidth, m_iHeight;
};
