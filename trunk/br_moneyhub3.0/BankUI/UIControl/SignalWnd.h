
#pragma once

typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, WS_EX_APPWINDOW> CSignalTraits;
class CSignalWnd : public CWindowImpl<CSignalWnd, CWindow, CSignalTraits>
{
public:
	DECLARE_WND_CLASS(_T("MONEYHUB_SIGNAL_WND"))

	BEGIN_MSG_MAP(CSignalWnd)
	END_MSG_MAP()
};