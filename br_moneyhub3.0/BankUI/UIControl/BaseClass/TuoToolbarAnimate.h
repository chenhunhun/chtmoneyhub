#pragma once
#include "../../Skin/TuoImage.h"

#define WM_ANI_TIMER		(0xffee)


struct AniData
{
	const CTuoImage *pImage;
	int iTotalFrames;
	int iCurrentFrame;
	DWORD dwLastTick;
	DWORD dwInterval;
};


template <class T>
class CTuoToolBarCtrlAnimate
{

public:

	CTuoToolBarCtrlAnimate() : m_dwMinTimerInterval(0xffffffff) {}


	void SetAni(int iButtonIndex, const CTuoImage *pImage, int iTotalFrames, DWORD dwInterval)
	{
		AniData aniData = { pImage, iTotalFrames, 0, 0, dwInterval };
		m_AniDataMap.insert(std::make_pair(iButtonIndex, aniData));
		if (dwInterval < m_dwMinTimerInterval)
		{
			m_dwMinTimerInterval = dwInterval;
			T *pT = static_cast<T*>(this);
			pT->KillTimer(WM_ANI_TIMER);
			pT->SetTimer(WM_ANI_TIMER, m_dwMinTimerInterval);
		}
	}

	void UpdateAni(int iButtonIndex, const CTuoImage *pImage, int iTotalFrames)
	{
		AniDataMap::iterator it = m_AniDataMap.find(iButtonIndex);
		if (it != m_AniDataMap.end())
		{
			it->second.pImage = pImage;
			it->second.iTotalFrames = iTotalFrames;
			it->second.iCurrentFrame = 0;
		}
	}

	void RemoveAni(int iButtonIndex)
	{
		m_AniDataMap.erase(iButtonIndex);
		if (m_AniDataMap.empty())
		{
			T *pT = static_cast<T*>(this);
			pT->KillTimer(WM_ANI_TIMER);
			m_dwMinTimerInterval = 0xffffffff;
		}
	}

	bool DrawAni(HDC hDC, int iIndex, const RECT &rc)
	{
		AniDataMap::iterator it = m_AniDataMap.find(iIndex);
		if (it == m_AniDataMap.end())
			return false;
		AniData &aniData = it->second;
		DWORD dwCurrentTick = ::GetTickCount();
		if (aniData.dwLastTick - dwCurrentTick >= aniData.dwInterval)
		{
			aniData.iCurrentFrame = (aniData.iCurrentFrame + 1) % aniData.iTotalFrames;
			aniData.dwLastTick = dwCurrentTick;
		}
		aniData.pImage->Draw(hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, aniData.pImage->GetWidth() / aniData.iTotalFrames * aniData.iCurrentFrame, 0);
		return true;
	}

	BEGIN_MSG_MAP(T)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		if (wParam == WM_ANI_TIMER)
		{
			if (m_AniDataMap.size() > 0)
			{
				T *pT = static_cast<T*>(this);
				pT->Invalidate();
			}
		}
		else
			bHandled = FALSE;
		return 0;
	}


	DWORD m_dwMinTimerInterval;

	typedef std::map<int, AniData> AniDataMap;
	AniDataMap m_AniDataMap;
};
