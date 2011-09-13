#pragma once
#include <tuodwmapi.h>
#include "../../../Utils/ListManager/ListManager.h"
#include "../../Skin/SkinManager.h"
#include "CategoryCtrl.h"

class CTuotuoCategoryCtrl;
class CChildFrame;

class CCategoryItem
{
	friend class CTuotuoCategoryCtrl;

public:
	CCategoryItem(CTuotuoCategoryCtrl *pCateCtrl);

	// 画tab，iDeltaHeight表示在chrome皮肤下，标签窗口上部空出来的高度
	void DoPaint(CDCHandle dc, ButtonStatus eTabStatus, CategoryPosition btnPos, ButtonStatus eBtn, bool bDrawCloseBtn, int iDeltaHeight);

	const CWebsiteData* GetWebData() const		{ return m_pWebData; }

	// 标题
	LPCTSTR GetText() const;
	void SetText(LPCTSTR lpszText);

	// 宽度
	void SetNewWidthImmediately();

	// 位置
	void SetPosImmediately(int iPos);

	// 索引
	size_t GetIndex() const				{ return m_nIndex; }
	void SetIndex(size_t nIndex)		{ m_nIndex = nIndex; }

	// 是否显示锁与安全色
	bool ShowSecureLock();
	bool ShowSecureColor();

	// 重绘
	void Redraw()						{ m_pCateCtrl->Invalidate(); }

public:
	CTabItem *m_pSelectedItem;
	std::vector<CTabItem*> m_TabItems;

private:
	CTuotuoCategoryCtrl *m_pCateCtrl;

	int m_nIndex;
	std::tstring m_sText;

	int m_iXPos;
	int m_iWidth;

	const CWebsiteData *m_pWebData;
};
