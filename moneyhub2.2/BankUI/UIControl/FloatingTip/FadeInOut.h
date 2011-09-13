#pragma once
#ifndef _FADEINOUT_H
#define _FADEINOUT_H
#include "Structs.h"

class CTooltipFadeInOut
{
public:
	CTooltipFadeInOut() ;
	~CTooltipFadeInOut() ;
public:
	void SetVerb(bool bOut) ;
	bool IsFadeOut() {return m_bFadeOut;}
	void SetCurrPercent(int iPrecent) ;
	void SetSetp(int iStep) ;
	bool IsCompleted() ;
	bool IsVaild(){return m_bValid;}
	const int NextStep() ;
	const int FirstStep() ;
	const int GetStep(){return m_iPercent;}
	void SetValid(bool bValid = true){m_bValid = bValid ;}
private:
	bool m_bValid;
	bool m_bFadeOut ;
	int m_iPercent;
	int m_iStep ;
};

#endif