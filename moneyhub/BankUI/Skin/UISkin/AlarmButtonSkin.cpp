#include "stdafx.h"
#include "AlarmButtonSkin.h"
#include "../SkinManager.h"

CAlarmButtonSkin::CAlarmButtonSkin(CSkinManager *pSkin)
{
	pSkin->RegisterFileName(_T("Alarm_button"), m_bitmap);
}

void CAlarmButtonSkin::Refresh()
{
	m_bitmap.LoadFromFile();
	//m_bitmapMask.LoadFromFile();
}

void CAlarmButtonSkin::DrawButton(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const
{
	ButtonStatus btnStatus = CSkinManager::FromTuoToolStatus(uButtonStatus);
	UINT uYPos = 0;
	RECT rcDest = { rc.left, uYPos, rc.left + m_bitmap.GetWidth() /4, uYPos + m_bitmap.GetHeight() };
	CSkinManager::DrawImagePart(dc, rcDest, m_bitmap, btnStatus, 4);
}