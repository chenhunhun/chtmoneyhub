
#include "BaseClass/TuoToolbar.h"
#include "../Util/ThreadStorageManager.h"

using namespace std;

#define TEXT_BTN_USER_LOADINT	1 // 正在登录
#define TEXT_BTN_USER_NAME		2 // 登录后的用户名显示

class CTextButton : public CTuoToolBarCtrl<CTextButton>, public CFSMUtil
{
public:
	CTextButton(FrameStorageStruct *pFS);
	~CTextButton();

	DECLARE_WND_CLASS_NODRAW(_T("MH_TextButton"))

public:
	void CreateButton(HWND hParent, LPCTSTR lpShow, int nType);
	int GetTextButtonHeight();
	int GetTextButtonWidth();

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

protected:
	BEGIN_MSG_MAP_EX(CLoginButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CTextButton>)
		MSG_WM_CREATE(OnCreate);
	TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnClick(int nIndex, POINT pt);
private:
	std::wstring	m_wstrShow;
	int				m_nWidth;
	int				m_nHeight;
	CFont			m_TextFont;
	HWND			m_hParnetWnd;
	int				m_nType;
};