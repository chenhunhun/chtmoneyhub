#include "../UIControl/BaseClass/TuoToolbar.h"
#include "../Skin/TuoImage.h"
#include <string>
using namespace std;

// 以barctrl为基类的按钮类，将点击自身的消息发送给父窗口
// 接口调用顺序为
class CTipsButton : public CTuoToolBarCtrl<CTipsButton>
{
public:
	CTipsButton();

	DECLARE_WND_CLASS_NODRAW(_T("MH_TipsButton"))

public:
	// 加载按钮图像的
	void Refresh();
	// 作为子类需要重载的
	int GetWidth() const { return m_bitmap.GetWidth() / m_statenumber; }
	int GetHeight() const { return m_bitmap.GetHeight(); }
	
	void DrawButton(CDCHandle dc, const RECT &rc, UINT uButtonStatus) const;


private:
	CTuoImage m_bitmap;
	UINT m_msg;
	int m_statenumber;


public:
	// 设置按钮加载的图片和该图片所表示的状态数量
	void SetBitmapFileName(wstring fname,int statenumber);
	// 创建按钮的函数，需要获得父窗口句柄和消息
	void CreateButton(HWND hParent,UINT msg);

	// override
	void DrawBackground(HDC hDC, const RECT &rc);
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus);

	void SetButtonState(bool bEnabled);
protected:
	BEGIN_MSG_MAP_EX(CTipsButton)
		CHAIN_MSG_MAP(CTuoToolBarCtrl<CTipsButton>)
		MSG_WM_CREATE(OnCreate);
		TUO_COMMAND_CODE_HANDLER_EX(NM_CLICK, OnClick)
	END_MSG_MAP()


	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnClick(int nIndex, POINT pt);
};