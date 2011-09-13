#pragma once
#include <windows.h>

class CLockBase
{
	long iLocked;

public:

	CLockBase():iLocked(0){}
	virtual ~CLockBase(){}

protected:

	virtual void Lock()
	{
		_IncrementCount();
	}
	virtual void UnLock()
	{
		_DecrementCount();
	}
	virtual BOOL IsLocked()
	{
		return ::InterlockedExchangeAdd(&iLocked,0);
	}
	void _IncrementCount()
	{
		::InterlockedIncrement(&iLocked);
	}
	void _DecrementCount()
	{
		::InterlockedDecrement(&iLocked);
	}
};

class CCriticalSection :public CLockBase
{

	CRITICAL_SECTION m_cs;

public:

	CCriticalSection(bool bLock = false)
	{
		::InitializeCriticalSection(&m_cs);
		if(bLock)
			Lock();
	}

	~CCriticalSection()
	{
		::DeleteCriticalSection(&m_cs);
	}

	BOOL TryLock()
	{
		if(::TryEnterCriticalSection(&m_cs))
		{
			__super::_IncrementCount();
			return TRUE;
		}
		return FALSE;
	}

	void Lock()
	{
		::EnterCriticalSection(&m_cs);
		__super::Lock();
	}

	void UnLock()
	{
		::LeaveCriticalSection(&m_cs);
		__super::_DecrementCount();
	}
};

//////////////////////////////////////////////////////////////////////////

class CAutoLock
{
public:

	CAutoLock(CRITICAL_SECTION *cs) : m_cs(cs)
	{
		if (m_cs)
			::EnterCriticalSection(m_cs);
	}
	~CAutoLock()
	{
		if (m_cs)
			::LeaveCriticalSection(m_cs);
	}

private:

	CRITICAL_SECTION *m_cs;
};
