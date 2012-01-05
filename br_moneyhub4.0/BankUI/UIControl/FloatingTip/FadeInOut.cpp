#include "stdafx.h"
#include "FadeInOut.h"

CTooltipFadeInOut::CTooltipFadeInOut()
: m_bValid(false)
, m_bFadeOut(false)
, m_iPercent(0)
, m_iStep(0)
{
}

CTooltipFadeInOut::~CTooltipFadeInOut()
{
}

void CTooltipFadeInOut::SetVerb(bool bOut)
{
	m_bFadeOut = bOut ;
}

void CTooltipFadeInOut::SetCurrPercent(int iPrecent)
{
	m_iPercent = iPrecent ;
}

void CTooltipFadeInOut::SetSetp(int iStep)
{
	m_iStep = iStep ;
	if(m_iStep<=0) m_iStep = 1 ;

}

const int CTooltipFadeInOut::FirstStep()
{
	if ( !m_bValid )
		return NULL ;
	ATLASSERT ( m_iStep>0  ) ;

	if(m_iStep<=0)m_iStep = 1 ;

	if ( m_bFadeOut )
		m_iPercent = m_iPercent<0?0:m_iPercent ;
	else
		m_iPercent = m_iPercent>=100?100:m_iPercent ;

	return m_iPercent ;

}
const int CTooltipFadeInOut::NextStep()
{
	if ( !m_bValid )
		return NULL ;

	ATLASSERT ( m_iStep>0  ) ;

	if(m_iStep<=0)m_iStep = 1 ;

	if ( m_bFadeOut )
	{
		m_iPercent -= m_iStep ;
		m_iPercent = m_iPercent<0?0:m_iPercent ;
	}
	else
	{
		m_iPercent += m_iStep ;
		m_iPercent = m_iPercent>=100?100:m_iPercent ;
	}

	return m_iPercent ;
}

bool CTooltipFadeInOut::IsCompleted()
{
	return m_bValid&&( (m_bFadeOut&&0==m_iPercent) || (!m_bFadeOut&&100==m_iPercent) ) ;
}
