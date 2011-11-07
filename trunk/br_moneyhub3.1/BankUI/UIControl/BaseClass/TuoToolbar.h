#pragma once


enum ToolType
{
	ToolType_Normal,			// 普通按钮
	ToolType_Dropdown,			// Dropdown按钮
	ToolType_WithDropdown,		// 普通按钮+DropDown
	ToolType_Toggle,			// 带状态的按钮
	ToolType_Null,				// 没按钮，就是个占位符
	ToolType_Sep				// 分隔符
};



struct ToolData
{
	ToolType eType;
	bool bEnabled;
	DWORD_PTR dwData;
	int iButtonWidth;
	int iDropDownPartWidth;		// 一般情况下无用，只有在Type为ToolType_WithDropdown时有用

	// 以下成员为内部使用
	struct
	{
		int iXPos;
		bool bChecked;
	} inner;
};

typedef std::vector<ToolData> ToolVector;

#define WM_TUO_COMMAND				(WM_APP + 0x0111)
#define WM_TUO_CHEVRON_COMMAND		(WM_APP + 0x0112)

#define BTN_CHEVRON			-2

#define BTN_SEP_WIDTH		5


class CTuoToolBarCtrlBase
{

public:

	CTuoToolBarCtrlBase() : m_iLineNumberInRebar(0) {}

	void AddButton(ToolData *pToolData)
	{
		ToolData *pLastTool = m_toolVec.size() == 0 ? NULL : &m_toolVec[m_toolVec.size() - 1];
		m_toolVec.push_back(*pToolData);
	}

	void AddButtons(ToolData *pToolData, int iCount)
	{
		ToolData *pLastTool = m_toolVec.size() == 0 ? NULL : &m_toolVec[m_toolVec.size() - 1];
		int iCurrentPos = pLastTool ? pLastTool->inner.iXPos + pLastTool->iButtonWidth : 0;
		for (int i = 0; i < iCount; i++)
		{
			pToolData[i].inner.iXPos = iCurrentPos;
			if (pToolData[i].eType == ToolType_Sep)
				pToolData[i].iButtonWidth = BTN_SEP_WIDTH;
			iCurrentPos += pToolData[i].iButtonWidth;
			m_toolVec.push_back(pToolData[i]);
		}
	}

	void ClearAllButtons()
	{
		m_toolVec.clear();
	}

	int GetButtonCount() const { return m_toolVec.size(); }

	bool GetEnable(int iIndex) const { return m_toolVec[iIndex].bEnabled; }
	bool GetChecked(int iIndex) const { return m_toolVec[iIndex].inner.bChecked; }

	DWORD_PTR GetData(int iIndex) const { return m_toolVec[iIndex].dwData; }
	void SetData(int iIndex, DWORD_PTR dwData) { m_toolVec[iIndex].dwData = dwData; }

	void GetButtonRect(int iIndex, RECT *prect) const
	{
		::GetClientRect(m_hToolWnd, prect);
		prect->left = m_toolVec[iIndex].inner.iXPos;
		prect->right = prect->left + m_toolVec[iIndex].iButtonWidth;
	}

	int GetFullWidth() const
	{
		if (m_toolVec.size() == 0)
			return 0;
		else
		{
			const ToolData *pTool = &m_toolVec[m_toolVec.size() - 1];
			return pTool->inner.iXPos + pTool->iButtonWidth;
		}
	}
	int GetHeight() const { return m_iDefaultHeight; }
	void SetHeight(int iHeight) { m_iDefaultHeight = iHeight; }

	int GetButtonWidth(int iIndex) const { return m_toolVec[iIndex].iButtonWidth; }

	int GetLineNumberInRebar() const { return m_iLineNumberInRebar; }

	HWND m_hToolWnd;
	int m_iDefaultHeight;
	int m_iLineNumberInRebar;		// 工具栏在rebar里面的行数

protected:

	ToolVector m_toolVec;

};


template <class T>
class ATL_NO_VTABLE CTuoToolBarCtrl : public CWindowImpl<T>, public CTuoToolBarCtrlBase
{

public:

	CTuoToolBarCtrl() : m_bIsMouseInWindow(false), m_iCurrentIndex(INVALID_ITEM), m_iMouseDownIndex(INVALID_ITEM), m_bAnchorHighlight(false),
		m_bUseDropDownButton(false), m_bStartCapturing(false), m_iButtonDisplayIndex(0) {}


	void SetUseChevron(bool bUseChevron)
	{
		m_bUseChevron = bUseChevron;
	}


	void SetEnable(int iIndex, bool bEnable, bool bRedraw = true)
	{
		if (m_toolVec[iIndex].bEnabled == bEnable)			// 若状态未变，则不刷新
			return;
		m_toolVec[iIndex].bEnabled = bEnable;
		if (bRedraw)
			RedrawButton(iIndex, true);
	}

	void SetChecked(int iIndex, bool bChecked, bool bRedraw = true)
	{
		ATLASSERT(m_toolVec[iIndex].eType == ToolType_Toggle);		// 只有toggle按钮支持此状态
		if (m_toolVec[iIndex].inner.bChecked == bChecked)			// 若状态未变，则不刷新
			return;
		m_toolVec[iIndex].inner.bChecked = bChecked;
		if (bRedraw)
			RedrawButton(iIndex, true);
	}

	void SetButtonDropDownPartWidth(int iIndex, int iWidth)
	{
		m_toolVec[iIndex].iDropDownPartWidth = iWidth;
	}
	void SetButtonWidth(int iIndex, int iWidth, bool bRedraw = false)
	{
		if (m_toolVec[iIndex].iButtonWidth == iWidth)			// 若宽度未变，则不刷新
			return;
		m_toolVec[iIndex].iButtonWidth = iWidth;

		// 当前按钮宽度变了，需要刷新此按钮后面的按钮
		RECT rcClient;
		GetClientRect(&rcClient);
		rcClient.left = m_toolVec[iIndex].inner.iXPos;
		T *pT = static_cast<T*>(this);
		if (bRedraw && rcClient.right > rcClient.left)
		{
			CClientDC dc(m_hWnd);
			CMemoryDC memDC(dc, rcClient);
			pT->DrawBackground(memDC, rcClient);
			for (size_t i = iIndex; i < m_toolVec.size(); i++)
			{
				m_toolVec[i].inner.iXPos = rcClient.left;
				rcClient.right = rcClient.left + m_toolVec[i].iButtonWidth;
				if (m_toolVec[i].eType != ToolType_Null)			// 对于占位符按钮不用绘制
				{
					if (i < (size_t)m_iButtonDisplayIndex)			// 只有按钮在可显示范围内才绘制
					{
						if (m_toolVec[i].eType == ToolType_Sep)
							DrawButtonSep(memDC, rcClient);
						else
							pT->DrawButton(memDC, i, rcClient, GetStatusOfButton(i));
					}
				}
				rcClient.left = rcClient.right;
			}
		}
		else
		{
			T *pT = static_cast<T*>(this);
			for (size_t i = iIndex; i < m_toolVec.size(); i++)
			{
				m_toolVec[i].inner.iXPos = rcClient.left;
				rcClient.right = rcClient.left + m_toolVec[i].iButtonWidth;
				rcClient.left = rcClient.right;
			}
		}

		// 工具栏大小发生变化，需要重新计算显示的工具栏的边界
		RECT rc;
		GetClientRect(&rc);
		if (m_toolVec.size() > 0 && m_toolVec[m_toolVec.size() - 1].inner.iXPos + m_toolVec[m_toolVec.size() - 1].iButtonWidth > rc.right)
		{
			// 工具栏的按钮显示不下，所以要截断
			int iCurrentWidth = 0;
			for (size_t i = 0; i < m_toolVec.size(); i++)
			{
				iCurrentWidth += m_toolVec[i].iButtonWidth;
				if (iCurrentWidth > rc.right)
				{
					m_iButtonDisplayIndex = i;
					break;
				}
			}
		}
		else
			m_iButtonDisplayIndex = m_toolVec.size();
	}

	// 获取超出边界范围的那个按钮的下标
	int GetDisplayBoundButton() const		{ return m_iButtonDisplayIndex; }

	// 若为true，则表示即使鼠标离开了工具栏，按钮显示在“鼠标在上面”的状态
	void SetAnchorHighlight(bool bAnchorHighlight)
	{
		m_bAnchorHighlight = bAnchorHighlight;
	}

	int HitTest(const POINT &pt) { return GetIndexByMousePosition(pt); }
	int GetHotItem() const { return m_iCurrentIndex; }
	void SetHotItem(int iIndex)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_CANCEL, m_hWnd };
		::TrackMouseEvent(&tme);
		m_bIsMouseInWindow = false;
		ChangeCurrentIndex(iIndex, true);
	}
	void PressButton(int iIndex)
	{
		m_iMouseDownIndex = iIndex;
	}


	void RefreshAfterAddButtonOrSizeChange(bool bForceRedraw = false)
	{
		int iOldButtonDisplayIndex = m_iButtonDisplayIndex;
		// 当工具栏大小发生变化时，需要重新计算显示的工具栏的边界
		RECT rc;
		GetClientRect(&rc);
		if (m_toolVec.size() > 0 && m_toolVec[m_toolVec.size() - 1].inner.iXPos + m_toolVec[m_toolVec.size() - 1].iButtonWidth > rc.right)
		{
			// 工具栏的按钮显示不下，所以要截断
			int iCurrentWidth = 0;
			for (size_t i = 0; i < m_toolVec.size(); i++)
			{
				iCurrentWidth += m_toolVec[i].iButtonWidth;
				if (iCurrentWidth > rc.right)
				{
					m_iButtonDisplayIndex = i;
					break;
				}
			}
		}
		else
			m_iButtonDisplayIndex = m_toolVec.size();
return;
		T *pT = static_cast<T*>(this);
		CClientDC dc(m_hWnd);
		GetClientRect(&rc);
		if (!bForceRedraw && iOldButtonDisplayIndex == m_iButtonDisplayIndex)
		{
			rc.left = m_iButtonDisplayIndex == 0 ? 0 : m_toolVec[m_iButtonDisplayIndex - 1].inner.iXPos + m_toolVec[m_iButtonDisplayIndex - 1].iButtonWidth;
			CMemoryDC memDC(dc, rc);
			pT->DrawBackground(memDC, rc);
			return;
		}

		if (iOldButtonDisplayIndex > m_iButtonDisplayIndex)
		{
			// 这个说明toolbar变短了，需要将显示了一半的按钮用背景色盖上
			rc.left = m_iButtonDisplayIndex == 0 ? 0 : m_toolVec[m_iButtonDisplayIndex - 1].inner.iXPos + m_toolVec[m_iButtonDisplayIndex - 1].iButtonWidth;
			CMemoryDC memDC(dc, rc);
			pT->DrawBackground(memDC, rc);
		}
		else
		{
			// 这个说明toolbar变长了，需要补绘制新显示出来的按钮
			rc.left = iOldButtonDisplayIndex == 0 ? 0 : m_toolVec[iOldButtonDisplayIndex - 1].inner.iXPos + m_toolVec[iOldButtonDisplayIndex - 1].iButtonWidth;
			CMemoryDC memDC(dc, rc);
			pT->DrawBackground(memDC, rc);
			for (int i = iOldButtonDisplayIndex; i < m_iButtonDisplayIndex; i++)
			{
				if (m_toolVec[i].eType == ToolType_Null)
					continue;
				rc.left = m_toolVec[i].inner.iXPos;
				rc.right = rc.left + m_toolVec[i].iButtonWidth;
				if (m_toolVec[i].eType == ToolType_Sep)
					DrawButtonSep(memDC, rc);
				else
					pT->DrawButton(memDC, i, rc, GetStatusOfButton(i));
			}
		}
	}


	BEGIN_MSG_MAP_EX(T)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MSG_WM_CAPTURECHANGED(OnCaptureChanged)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		m_hToolWnd = m_hWnd;
		CREATESTRUCT *pCS = (CREATESTRUCT*)lParam;
		m_iDefaultHeight = pCS->cy;
		return 0;
	}

	LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		// 这里还有个地方比较晦涩，就是这个WM_WINDOWPOSCHANGED消息会在调用ShowWindow时立即产生一个
		bHandled = FALSE;
		WINDOWPOS *pPos = (WINDOWPOS*)lParam;
		if ((pPos->flags & SWP_NOSIZE) == 0)
		{
			RefreshAfterAddButtonOrSizeChange();
			Invalidate();
		}
		return 0;
	}


	void OnPaint(CDCHandle dc, RECT rcClient)		// 一次性绘制所有按钮
	{
		GetClientRect(&rcClient);
		CMemoryDC memDC(dc, rcClient);
		T *pT = static_cast<T*>(this);
		pT->DrawBackground(memDC, rcClient);
		for (int i = 0; i < m_iButtonDisplayIndex; i++)		// 只绘制那些显示得下的按钮
		{
			if (m_toolVec[i].eType == ToolType_Null)
				continue;
			rcClient.left = m_toolVec[i].inner.iXPos;
			rcClient.right = rcClient.left + m_toolVec[i].iButtonWidth;
			if (m_toolVec[i].eType == ToolType_Sep)
				DrawButtonSep(memDC, rcClient);
			else
				pT->DrawButton(memDC, i, rcClient, GetStatusOfButton(i));
		}
	}

	void OnMouseMove(UINT nFlags, CPoint point)
	{
		if (!m_bIsMouseInWindow)		// 产生 WM_MOUSELEAVE 的消息
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd };
			if (::TrackMouseEvent(&tme))
				m_bIsMouseInWindow = true;
		}

		if (m_iMouseDownIndex >= 0)		// 如果鼠标是按下的，则只有按下的那个按钮状态发生改变
		{
			int iCurrentIndex = GetIndexByMousePosition(point);
			if (iCurrentIndex != m_iMouseDownIndex)		// 用户按着鼠标不放，却离开了按钮，于是就把按钮恢复成原始状态
			{
				if (m_iCurrentIndex != INVALID_ITEM)
				{
					m_iCurrentIndex = INVALID_ITEM;
					RedrawButton(m_iMouseDownIndex, true);
				}
			}
			else									// 如果用户鼠标又放回了按钮，则把按钮状态设为按下
			{
				if (m_iCurrentIndex != m_iMouseDownIndex)
				{
					m_iCurrentIndex = m_iMouseDownIndex;
					RedrawButton(m_iMouseDownIndex, true);
				}
			}
			return;
		}

		int iMousePos = GetIndexByMousePosition(point);
		if (iMousePos != INVALID_ITEM || !m_bAnchorHighlight)			// 如果m_bAnchorHighlight=true，表示鼠标移出后也要保持最后一个高亮过的按钮的高亮状态
			ChangeCurrentIndex(iMousePos);
//		if (iMousePos >= 0 && m_toolVec[iMousePos].eType != ToolType_Null && m_toolVec[iMousePos].eType != ToolType_Sep)
//			CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, iMousePos);
//		else
//			CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}

	void OnMouseLeave()
	{
		m_bIsMouseInWindow = false;
		if (!m_bAnchorHighlight)			// 如果m_bAnchorHighlight=true，表示鼠标移出后也要保持最后一个高亮过的按钮的高亮状态
			ChangeCurrentIndex(INVALID_ITEM);
//		CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);
	}

	void OnLButtonDown(UINT nFlags, CPoint point)
	{
//		CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);

		int iNewIndex = GetIndexByMousePosition(point);
		if (iNewIndex == INVALID_ITEM)			// 鼠标按下的时候按在了空白处
		{
			ChangeCurrentIndex(INVALID_ITEM);
			return;
		}

		ToolData *pTool = &m_toolVec[iNewIndex];
		if (IsButtonActivate(pTool))
		{
			bool bUseDropDown = false;
			if (pTool->eType == ToolType_WithDropdown)
			{
				if (point.x >= pTool->inner.iXPos + pTool->iDropDownPartWidth)
				{
					bUseDropDown = true;
					m_bUseDropDownButton = true;
				}
			}
			else if (pTool->eType == ToolType_Dropdown)
				bUseDropDown = true;

			if (bUseDropDown)
			{
				m_iMouseDownIndex = iNewIndex;
				ChangeCurrentIndex(iNewIndex, true);

				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_CANCEL, m_hWnd };
				::TrackMouseEvent(&tme);

				SendMessage(WM_TUO_COMMAND, MAKELONG(iNewIndex, NM_DROPDOWN), MAKELPARAM(point.x, point.y));

				tme.dwFlags = TME_LEAVE;
				::TrackMouseEvent(&tme);

				m_bUseDropDownButton = false;
				m_iMouseDownIndex = INVALID_ITEM;
				::GetCursorPos(&point);		// 重新获取鼠标坐标
				ScreenToClient(&point);
				ChangeCurrentIndex(GetIndexByMousePosition(point), true);

				// 当用鼠标在当前展开菜单的按钮上再点一下时，菜单不会消失（会闪一下之后立即再弹出来）
				// 为了避免这种现象，加了下面的语句
				MSG msg;
				RECT rect;
				GetButtonRect(iNewIndex, &rect);
				ClientToScreen(&rect);
				if (::PeekMessage(&msg, m_hWnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, PM_NOREMOVE) && ::PtInRect(&rect, msg.pt))
					::PeekMessage(&msg, m_hWnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, PM_REMOVE);
			}
			else
			{
				m_iMouseDownIndex = iNewIndex;
				ChangeCurrentIndex(iNewIndex, true);
				m_bStartCapturing = true;
				SetCapture();
			}
		}
		else			// 鼠标按下时，按在了disabled的按钮上
			ChangeCurrentIndex(INVALID_ITEM);
	}

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (m_iMouseDownIndex < 0)		// 此处不用“== INVALID_ITEM”进行判断的原因是：chevron走dropdown，不走click
			return;
		int iCurrentIndex = GetIndexByMousePosition(point);
		ToolData *pTool = iCurrentIndex == m_iMouseDownIndex ? &m_toolVec[iCurrentIndex] : NULL;
		m_iMouseDownIndex = INVALID_ITEM;
		if (pTool && pTool->eType == ToolType_Toggle)		// 如果是toggle按钮，则要改变状态
			pTool->inner.bChecked = !pTool->inner.bChecked;
		ChangeCurrentIndex(iCurrentIndex, true);
		m_bStartCapturing = false;
		::ReleaseCapture();

//		if (iCurrentIndex >= 0 && m_toolVec[iCurrentIndex].eType != ToolType_Null && m_toolVec[iCurrentIndex].eType != ToolType_Sep)
//			CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, iCurrentIndex);
//		else
//			CSogouToolTip::GetTooltip()->RelayEventSingleton(m_hWnd, INVALID_ITEM);

		if (pTool && IsButtonActivate(pTool) && (pTool->eType == ToolType_Normal || pTool->eType == ToolType_Toggle
			|| (pTool->eType == ToolType_WithDropdown && point.x < pTool->inner.iXPos + pTool->iDropDownPartWidth)))
		{
			if (SendMessage(WM_TUO_COMMAND, MAKELONG(iCurrentIndex, NM_CLICK), MAKELPARAM(point.x, point.y)))
				RedrawButton(iCurrentIndex, true);
		}
	}

	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		int iCurrentIndex = GetIndexByMousePosition(pt);
		if (iCurrentIndex >= 0)			// 此处不用“!= INVALID_ITEM”进行判断的原因是：chevron不支持右键
		{
			ToolData *pTool = &m_toolVec[iCurrentIndex];
			if (IsButtonActivate(pTool) && pTool->eType != ToolType_Null && pTool->eType != ToolType_Sep)
			{
				if (SendMessage(WM_TUO_COMMAND, MAKELONG(iCurrentIndex, NM_RCLICK), lParam) != 0)
					return 0;
			}
		}
		bHandled = FALSE;
		return 0;
	}


	void OnCaptureChanged(CWindow wnd)
	{
		if (m_bStartCapturing)
		{
			// 如果调用到这里了，说明没有执行到LButtonUp，则取消所有按钮的状态
			m_bStartCapturing = false;
			m_iMouseDownIndex = INVALID_ITEM;
			ChangeCurrentIndex(INVALID_ITEM);
		}
	}


private:

	// 重绘单个按钮
	void RedrawButton(int iIndex, bool bDrawBackground)
	{
		if (iIndex >= m_iButtonDisplayIndex)		// 如果要绘制的按钮超出边界就不画了
			return;
		T *pT = static_cast<T*>(this);
		if (!pT->IsWindowVisible())					// 如果窗口是隐藏的，可以不用绘制
			return;

		RECT rc;
		GetClientRect(&rc);

		ToolData *pToolData = &m_toolVec[iIndex];
		if (pToolData->eType == ToolType_Null)			// 这个类型表示按钮只是一个占位符，不进行绘制
			return;
		rc.left = pToolData->inner.iXPos;
		rc.right = rc.left + pToolData->iButtonWidth;
		CClientDC dc(m_hWnd);
		if (bDrawBackground)
		{
			CMemoryDC memDC(dc, rc);
			pT->DrawBackground(memDC, rc);
			if (pToolData->eType == ToolType_Sep)
				DrawButtonSep(memDC, rc);
			else
				pT->DrawButton(memDC, iIndex, rc, GetStatusOfButton(iIndex));
		}
		else
		{
			if (pToolData->eType == ToolType_Sep)
				DrawButtonSep(dc, rc);
			else
				pT->DrawButton(dc, iIndex, rc, GetStatusOfButton(iIndex));
		}
	}

	// 绘制分割线
	void DrawButtonSep(HDC hDC, const RECT &rc) const
	{
		s()->Toolbar()->DrawToolBarSep(hDC, rc);
	}

	// 根据鼠标坐标获取所在按钮的下标
	int GetIndexByMousePosition(POINT pt) const
	{
		if (m_toolVec.empty())
			return INVALID_ITEM;
		RECT rcClient;
		GetClientRect(&rcClient);
		if (pt.x < 0 || pt.x > rcClient.right || pt.y < 0 || pt.y > rcClient.bottom)
			return INVALID_ITEM;
		ATLASSERT(m_iButtonDisplayIndex > 0 && m_iButtonDisplayIndex <= (int)m_toolVec.size());

		// 判断是否鼠标在最后一个按钮上（为了做折半查找，需要先把最后一个点的后半部分去掉）
		const ToolData *pLastToolData = &m_toolVec[m_iButtonDisplayIndex - 1];
		if (pt.x >= pLastToolData->inner.iXPos)
		{
			if (pt.x > pLastToolData->inner.iXPos + pLastToolData->iButtonWidth)
			{
				// 鼠标位于当前显示的最后一个按钮的右边，在这里检查一下是否位于chevron按钮上
				return INVALID_ITEM;
			}
			else
				return m_iButtonDisplayIndex - 1;
		}

		int iBeginLeft = 0, iBeginRight = m_iButtonDisplayIndex - 1;		// 这里的右边界设定为能够显示得下的按钮（因为鼠标是不会移到那些显示不下的按钮上的）
		while (iBeginLeft < iBeginRight - 1)
		{
			int iMid = (iBeginLeft + iBeginRight) / 2;
			if (pt.x > m_toolVec[iMid].inner.iXPos)		// 鼠标还要靠右
				iBeginLeft = iMid;
			else if (pt.x < m_toolVec[iMid].inner.iXPos)	// 鼠标还要靠左
				iBeginRight = iMid;
			else
				return iMid;
		}
		return iBeginLeft;
	}

	// 获取指定的按钮当前的状态
	UINT GetStatusOfButton(int iButtonIndex) const
	{
		const ToolData *pTool = &m_toolVec[iButtonIndex];
		if (pTool->bEnabled)
		{
			UINT uCheckBtn = pTool->eType == ToolType_Toggle && pTool->inner.bChecked ? BTN_STATUS_CHECKED : 0;
			if (iButtonIndex == m_iCurrentIndex)
			{
				if (m_iMouseDownIndex == m_iCurrentIndex)
					return uCheckBtn | (m_bUseDropDownButton ? BTN_STATUS_DROPDOWN : 0) | BTN_STATUS_MOUSEDOWN;
				else
					return uCheckBtn | BTN_STATUS_MOUSEOVER;
			}
			else
				return uCheckBtn | BTN_STATUS_MOUSEOUT;
		}
		else
			return BTN_STATUS_DISABLED;
	}

	// 看一个按钮是否可以响应鼠标事件
	bool IsButtonActivate(ToolData *pTool) const
	{
		if (!pTool->bEnabled)
			return false;
		if (pTool->eType == ToolType_Null || pTool->eType == ToolType_Sep)
			return false;
		return true;
	}

	// 改变并绘制当前按钮（需要将原先激活的按钮恢复，在绘制新的激活按钮）
	void ChangeCurrentIndex(int iNewIndex, bool bForceRefresh = false)
	{
		if (m_iCurrentIndex == iNewIndex)
		{
			if (bForceRefresh && iNewIndex != INVALID_ITEM)
				RedrawButton(iNewIndex, true);
			return;
		}
		int iOldIndex = m_iCurrentIndex;
		m_iCurrentIndex = iNewIndex;
		if (iOldIndex != INVALID_ITEM)
			RedrawButton(iOldIndex, true);
		if (m_iCurrentIndex != INVALID_ITEM)
			RedrawButton(m_iCurrentIndex, true);
	}


	bool m_bIsMouseInWindow;
	int m_iCurrentIndex;
	int m_iMouseDownIndex;
	bool m_bUseDropDownButton;		// 当鼠标按下时，按在了按钮的dropdown部分上

	bool m_bAnchorHighlight;		// 如果为true，则意味着即使鼠标离开了工具栏，按钮也显示为“鼠标在上面”的状态
	bool m_bStartCapturing;			// 执行过SetCapture，但是又没有执行ReleaseCapture

	int m_iButtonDisplayIndex;		// 在有限的窗口内显示的最后一个按钮的下标（因为超出窗口宽度的按钮不需要再绘制了）

protected:

	// overridable methods

	void DrawBackground(HDC hDC, const RECT &rc) { ATLASSERT(0 && "Please override me!"); }
	void DrawButton(HDC hDC, int iIndex, const RECT &rc, UINT uCurrentStatus) { }
};

//////////////////////////////////////////////////////////////////////////

#define TUO_COMMAND_HANDLER_EX(id, code, func) \
	if (uMsg == WM_TUO_COMMAND && (WORD)code == HIWORD(wParam) && (WORD)id == LOWORD(wParam)) \
	{ \
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) }; \
		lResult = func(pt); \
		return TRUE; \
	}

#define TUO_COMMAND_CODE_HANDLER_EX(code, func) \
	if (uMsg == WM_TUO_COMMAND && (WORD)code == HIWORD(wParam)) \
	{ \
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) }; \
		lResult = func((int)LOWORD(wParam), pt); \
		return TRUE; \
	}

#define TUO_CHEVRON_COMMAND(func) \
	if (uMsg == WM_TUO_CHEVRON_COMMAND) \
	{ \
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) }; \
		lResult = func(pt); \
		return TRUE; \
	}
