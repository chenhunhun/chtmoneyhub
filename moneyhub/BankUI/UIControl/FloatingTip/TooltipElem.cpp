#include "stdafx.h"
#include "TooltipElem.h"

//////////////////////////////////////////////////////////////////////////
// class CItemData

CItemData::CItemData()
: m_pData(NULL)
{

}

CItemData::CItemData(DWORD dwType)
: m_pData(NULL)
{
	ATLASSERT( dwType>=TIT_BEGIN && dwType<=TIT_END ) ;
	x_AllocBuffer(dwType) ;
}

CItemData::CItemData(const CItemData& cti)
: m_pData(NULL)
{
	if ( NULL!=cti.m_pData )
	{
		m_pData = cti.m_pData ;
		::InterlockedIncrement((LONG*)&m_pData->iRef) ;
	}
}

CItemData& CItemData::operator=(const CItemData& cti)
{
	if ( this == &cti )
		return *this ;

	x_ReleaseBuffer() ;

	if ( NULL != cti.m_pData )
	{
		m_pData = cti.m_pData ;
		::InterlockedIncrement((long*)m_pData->iRef) ;
	}

	return *this ;
}

CItemData::~CItemData()
{
	x_ReleaseBuffer() ;
}

RECT CItemData::GetRect(CDCHandle& dc)
{
	if ( NULL == m_pData )
		return RECT() ;

	if ( TIT_LINK != m_pData->dwType && TIT_TEXT != m_pData->dwType )
		return RECT() ;

	RECT rcItem = {0} ;
	wstring wsText ;
	switch(m_pData->dwType)
	{
	case TIT_LINK:
		{
			CFont font ;
			font.CreateFont(13,0,0,0,GetBold()?FW_BOLD:FW_NORMAL,0,TRUE,0,0,0,0,0,0,_T("Tahoma")) ;
			HFONT hOldFont = dc.SelectFont(font) ;
			wsText = m_pData->pData->pilLink->wsText ;
			dc.DrawText(wsText.c_str() , wsText.size() , &rcItem , DT_CALCRECT|DT_NOPREFIX|DT_NOCLIP|DT_SINGLELINE) ;
			dc.SelectFont(hOldFont) ;
		}
		break;
	case TIT_TEXT :
		wsText = m_pData->pData->pitText->wsText ;
		dc.DrawText(wsText.c_str() , wsText.size() , &rcItem , DT_CALCRECT|DT_NOPREFIX|DT_NOCLIP|DT_SINGLELINE) ;
		break ;
	}
	return rcItem ;
}

DWORD CItemData::GetType()
{
	if ( NULL == m_pData )
		return TIT_UNKNOWN ;
	return m_pData->dwType ;
}

wstring CItemData::GetText()
{
	if ( NULL == m_pData )
		return wstring() ;
	if ( m_pData->dwType == TIT_RETURN || TIT_UNKNOWN == m_pData->dwType)
		return wstring() ;

	switch(m_pData->dwType)
	{
	case TIT_TEXT:
		return m_pData->pData->pitText->wsText ;
	case TIT_LINK:
		return m_pData->pData->pilLink->wsText ;
	}
	return wstring() ;
}

int CItemData::GetId()
{
	if ( NULL == m_pData )
		return 0 ;
	if ( TIT_LINK!=m_pData->dwType )
		return 0 ;

	return m_pData->pData->pilLink->iId ;
}

COLORREF CItemData::GetColor()
{
	if ( NULL == m_pData )
		return 0 ;
	if ( TIT_UNKNOWN == m_pData->dwType || TIT_RETURN == m_pData->dwType)
		return 0;
	switch(m_pData->dwType)
	{
	case TIT_TEXT:
		return m_pData->pData->pitText->clr ;
	case TIT_LINK:
		return m_pData->pData->pilLink->clrNormal ;
	}
	return 0;
}

COLORREF CItemData::GetColorActive()
{
	if ( NULL == m_pData )
		return 0 ;
	if ( TIT_LINK!=m_pData->dwType )
		return 0 ;
	return m_pData->pData->pilLink->clrActive ;
}

COLORREF CItemData::GetColorHover()
{
	if ( NULL == m_pData )
		return 0 ;
	if ( TIT_LINK != m_pData->dwType)
		return 0 ;
	return m_pData->pData->pilLink->clrHover ;
}

BOOL CItemData::GetBold()
{
	if ( NULL == m_pData )
		return FALSE ;
	if ( TIT_LINK != m_pData->dwType )
		return FALSE ;

	return m_pData->pData->pilLink->bBold ;
}

bool CItemData::SetType(DWORD dwType)
{
	return x_AllocBuffer(dwType) ;
}

bool CItemData::SetColor(COLORREF clr)
{
	if ( NULL == m_pData )
		return false ;

	if ( TIT_TEXT!=m_pData->dwType && TIT_LINK!=m_pData->dwType )
		return false ;

	x_CopyOnWrite() ;

	switch(m_pData->dwType)
	{
	case TIT_TEXT:
		m_pData->pData->pitText->clr = clr ;
		break;
	case TIT_LINK:
		m_pData->pData->pilLink->clrNormal = clr ;
		break ;
	}

	return true ;
}

bool CItemData::SetText(const wstring& wsText)
{
	if ( NULL == m_pData )
		return false ;

	if ( TIT_TEXT!=m_pData->dwType && TIT_LINK!=m_pData->dwType )
		return false ;

	x_CopyOnWrite() ;

	switch(m_pData->dwType)
	{
	case TIT_TEXT:
		m_pData->pData->pitText->wsText = wsText ;
		break;
	case TIT_LINK:
		m_pData->pData->pilLink->wsText = wsText ;
		break ;
	}

	return true ;
}

bool CItemData::SetId(int id)
{
	if ( NULL == m_pData )
		return false ;

	if ( TIT_LINK!=m_pData->dwType )
		return false ;

	x_CopyOnWrite() ;

	m_pData->pData->pilLink->iId = id ;

	return true ;
}

bool CItemData::SetColorActive(COLORREF clr)
{
	if ( NULL == m_pData )
		return false ;

	if ( TIT_LINK!=m_pData->dwType )
		return false ;

	x_CopyOnWrite() ;

	m_pData->pData->pilLink->clrActive = clr ;

	return true ;

}

bool CItemData::SetColorHover(COLORREF clr)
{
	if ( NULL == m_pData )
		return false ;

	if ( TIT_LINK!=m_pData->dwType )
		return false ;

	x_CopyOnWrite() ;

	m_pData->pData->pilLink->clrHover = clr ;

	return true ;

}

bool CItemData::SetBold(BOOL bBold)
{
	if ( NULL == m_pData )
		return false ;

	if ( TIT_LINK!=m_pData->dwType )
		return false ;

	x_CopyOnWrite() ;

	m_pData->pData->pilLink->bBold = bBold ;

	return true ;
}

bool CItemData::x_CopyOnWrite()
{
	if ( m_pData->iRef > 1 )
	{
		_Data* pData = x_Alloc(m_pData->dwType) ;

		switch(pData->dwType)
		{
		case TIT_TEXT:
			*pData->pData->pitText = *m_pData->pData->pitText;
			break;
		case TIT_LINK:
			*pData->pData->pilLink = *m_pData->pData->pilLink;
			break;
		case TIT_RETURN:
			pData->pData->bWrap = m_pData->pData->bWrap ;
			break;
		default:
			pData->dwType = TIT_UNKNOWN ;
		}

		x_ReleaseBuffer() ;

		m_pData = pData ;
	}

	return true ;
}

CItemData::_Data* CItemData::x_Alloc(DWORD dwType)
{
	_Data* pData = new _Data ;

	pData->dwType = dwType ;
	pData->iRef = 1 ;

	pData->pData = new TEXTITEMDATA; 
	pData->pData->bWrap = false ;

	switch(dwType)
	{
	case TIT_TEXT:
		pData->pData->pitText = new unItemData::ItemText ;
		pData->pData->pitText->clr = RGB(0,0,0) ;
		break;
	case TIT_LINK:
		pData->pData->pilLink = new unItemData::ItemLink ;
		pData->pData->pilLink->clrActive = RGB(0xFF,0,0) ;
		pData->pData->pilLink->clrHover = RGB(0,0,0xFF) ;
		pData->pData->pilLink->clrNormal = RGB(0,0,0xFF) ;
		pData->pData->pilLink->bBold = FALSE ;
		pData->pData->pilLink->iId = 0 ;
		break;
	case TIT_RETURN:
		pData->pData->bWrap = true ;
		break;
	default:
		pData->dwType = TIT_UNKNOWN ;
	}

	return pData ;
}

bool CItemData::x_AllocBuffer(DWORD dwType)
{
	if ( NULL!=m_pData )
		x_ReleaseBuffer() ;		
	m_pData = x_Alloc(dwType) ;
	return true ;
}

bool CItemData::x_ReleaseBuffer()
{
	if ( NULL!=m_pData )
	{
		::InterlockedDecrement((long*)&m_pData->iRef) ;
		if ( m_pData->iRef<=0 )
		{
			// 已经不再使用
			switch(m_pData->dwType)
			{
			case TIT_TEXT: case TIT_LINK:
				delete m_pData->pData->pVoid ;
				m_pData->pData->pVoid = NULL ;
				break;
			}

			delete m_pData->pData ;
			m_pData->pData = NULL ;
			
			delete m_pData ;
		}

		m_pData = NULL ;
	}

	return true ;
}

//////////////////////////////////////////////////////////////////////////
// class CItem

CItem::CItem()
{
	m_bItemEnd = false ;
	m_bGood = false ;
}

CItem::CItem(LPCTSTR& pItem)
{
	m_bItemEnd = false ;
	m_bGood = x_Parse(pItem) ;
}

bool CItem::IsGood()
{
	return m_bGood ;
}

bool CItem::IsItemEnd()
{
	return m_bItemEnd ;
}

bool CItem::SetItem(LPCTSTR& pItem)
{
	m_Attributes.clear() ;
	m_bItemEnd = false ;
	m_bGood = x_Parse(pItem) ;
	return m_bGood ;
}

wstring CItem::GetAttribString(const wstring& wsAttr)
{
	return m_Attributes[wsAttr] ;
}

COLORREF CItem::GetAttribColor(const wstring& wsAttr)
{
	wstring wsValue = GetAttribString(wsAttr) ;
	DWORD dwColor = 0 ;
	TCHAR* p = NULL;
	dwColor = _tcstol(wsValue.c_str() , &p , 0) ;
	BYTE* pColor = (BYTE*)&dwColor ;
	return RGB(pColor[2],pColor[1],pColor[0]) ;
}

int CItem::GetAttribInt(const wstring& wsAttr)
{
	wstring wsValue = GetAttribString(wsAttr);
	if ( wsValue.empty() )
		return 0;
	else
		return _ttoi(wsValue.c_str()) ;
}

DWORD CItem::GetType()
{
	wstring wsType = m_Attributes[_T("type")] ;
	if ( _T("text") == wsType )
		return TIT_TEXT;
	else if ( _T("link") == wsType )
		return TIT_LINK ;
	else if ( _T("wrap") == wsType )
		return TIT_RETURN ;
	else 
		return TIT_UNKNOWN ;
}

bool CItem::x_Parse(LPCTSTR& pItem)
{
	if ( NULL == pItem )
		return false ;

	if( _tcsnicmp( _T("</item>") , pItem , 7 ) ==0 )
	{
		m_bItemEnd = true ;
		pItem += 7 ;
		return true ;
	}

	if ( _tcsnicmp(_T("<item ") , pItem , 6) !=0 )
		return false ;

	for ( pItem+=6 ; NULL!=pItem && '\0'!=*pItem&& '>'!=*pItem ; pItem++)
	{
		if ( ' ' == *pItem) continue;

		wstring wsValue ;
		
		if ( _tcsnicmp(_T("type=") , pItem , 5 ) == 0)
		{
			// 是类型属性
			pItem+=5;
			if ( !x_GetAttribValue(pItem , wsValue) )
				return false ;
			m_Attributes[_T("type")] = wsValue ;
		}
		else if ( _tcsnicmp(_T("id=") , pItem , 3 )==0 )
		{
			pItem+=3;
			if ( !x_GetAttribValue(pItem , wsValue) )
				return false ;
			m_Attributes[_T("id")] = wsValue ;
		}
		else if ( _tcsnicmp(_T("color=") , pItem , 6 )==0 )
		{
			pItem+=6;
			if ( !x_GetAttribValue(pItem , wsValue) )
				return false ;
			m_Attributes[_T("color")] = wsValue ;
		}
		else if ( _tcsnicmp(_T("hover=") , pItem , 6 )==0 )
		{
			pItem+=6;
			if ( !x_GetAttribValue(pItem , wsValue) )
				return false ;
			m_Attributes[_T("hover")] = wsValue ;
		}
		else if ( _tcsnicmp(_T("active=") , pItem , 7 )==0 )
		{
			pItem+=7;
			if ( !x_GetAttribValue(pItem , wsValue) )
				return false ;
			m_Attributes[_T("active")] = wsValue ;
		}
		else if ( _tcsnicmp(_T("bold=") , pItem , 5 )==0 )
		{
			pItem+=5;
			if ( !x_GetAttribValue(pItem , wsValue) )
				return false ;
			m_Attributes[_T("bold")] = wsValue ;
		}
		else
		{
			return false ;
		}
	}
	return '>'==*pItem++ ;
}

bool CItem::x_GetAttribValue(LPCTSTR& pItem , wstring& wsValue)
{
	// pItem 为 "'values' more..."
	while ( ' ' == *pItem ) pItem++;
	if ( '\'' != *pItem++ )
		return false ;

	while ( '\0' != *pItem && '\'' != *pItem ) wsValue.push_back(*pItem++) ;

	return '\'' == *pItem ;
}

//////////////////////////////////////////////////////////////////////////
// class CTooltipText

CTooltipText::CTooltipText()
{
}

bool CTooltipText::SetText(const wstring& wsText)
{
	wstring text(wsText) ;
	text = _T("<item type='text' color='0x000000' >") + text ;
	text += _T("</item>") ;

	m_ItemList.clear() ;

	LPCTSTR pText = text.c_str() ;
	while ( ' '==*pText ) pText++ ;
	return x_Parse(pText) ;
}

RECT CTooltipText::GetRect(CDCHandle& dc , int iRowSpace)
{
	int iHeight = x_GetLineHeight(dc) ;
	CRect rc (0,0,0,iHeight) ;
	CRect rcLine (0,0,0,iHeight) ;
	list<CItemData>::iterator it = m_ItemList.begin() ;

	for ( ;it!=m_ItemList.end() ; it++ )
	{
		if(TIT_RETURN == it->GetType() )
		{
			rc.bottom += iHeight ;
			rcLine.OffsetRect(CPoint(0,iHeight+iRowSpace)) ;
			rcLine.right = 0 ;
		}
		else
		{
			CRect rcItem = it->GetRect(dc) ;
			rcLine.right += rcItem.right ;
			RECT rcTmp = rc ;
			rc.UnionRect(&rcTmp ,&rcLine) ;
		}
	}

	return rc ;
}

list<CItemData>& CTooltipText::GetItemList()
{
	return m_ItemList ;
}

list<LINKBLOCK>& CTooltipText::GetLinkList()
{
	return m_LinkBlockList ;
}

bool CTooltipText::RanderText(CDCHandle& dc , RECT rcRect , int iRowSpace)
{
	int iHeight = x_GetLineHeight(dc) ;
	CRect rc (rcRect.left,rcRect.top,rcRect.left,rcRect.top+iHeight) ;
	CRect rcLine (rcRect.left,rcRect.top,rcRect.left,rcRect.top+iHeight) ;
	list<CItemData>::iterator it = m_ItemList.begin() ;
	m_LinkBlockList.clear() ;

	for ( ;it!=m_ItemList.end() ; it++ )
	{
		if(TIT_RETURN == it->GetType() )
		{
			rc.bottom += iHeight ;
			rcLine.OffsetRect(CPoint(0,iHeight+iRowSpace)) ;
			rcLine.right = rcRect.left ;
		}
		else if ( TIT_LINK == it->GetType() || TIT_TEXT == it->GetType() )
		{
			CRect rcItem = it->GetRect(dc) ;
			rcLine.left = rcLine.right ;
			rcLine.right += rcItem.right ;
			rcLine.right = rcLine.right>rcRect.right ? rcRect.right : rcLine.right;
			COLORREF clrOld = dc.GetTextColor() ;
			dc.SetTextColor(it->GetColor()) ;
			if ( it->GetType() == TIT_LINK )
			{
				CFont font ;
				font.CreateFont(13,0,0,0,it->GetBold()?FW_BOLD:FW_NORMAL,0,TRUE,0,0,0,0,0,0,_T("Tahoma")) ;
				HFONT hOldFont = dc.SelectFont(font) ;

				LINKBLOCK lb = {it->GetColor(),it->GetColorHover(),it->GetColorActive(),it->GetBold(),rcLine,it->GetId(),it->GetText()};
				m_LinkBlockList.push_back(lb);

				dc.DrawText(it->GetText().c_str() , it->GetText().size() , &rcLine , DT_NOCLIP|DT_NOPREFIX |DT_SINGLELINE|DT_WORD_ELLIPSIS) ;

				dc.SelectFont(hOldFont) ;
			}
			else
				dc.DrawText(it->GetText().c_str() , it->GetText().size() , &rcLine , DT_NOCLIP|DT_NOPREFIX |DT_SINGLELINE|DT_WORD_ELLIPSIS) ;
			dc.SetTextColor(clrOld) ;
		}
	}

	return true ;

}

int CTooltipText::x_GetLineHeight(CDCHandle& dc)
{
	CRect rc (0,0,0,0) ;
	dc.DrawText ( _T("H") , 1 , &rc , DT_CALCRECT|DT_NOPREFIX|DT_NOCLIP ) ;
	return rc.Height() ;
}

bool CTooltipText::x_Parse(LPCTSTR& pText)
{
	CItemData itemData ;

	// 如果第一个字符不是'<'，则是格式错误，返回false
	if ( '<' != *pText)
		return false ;

	CItem item;

	// 把根节点传给Item，往下parse一个节点
	if(!item.SetItem(pText) )
		return false ;

	if ( item.IsItemEnd() )
		return false ;

	itemData.SetType(item.GetType()) ;
	
	switch(itemData.GetType())
	{
	case TIT_TEXT:
		itemData.SetColor(item.GetAttribColor(_T("color"))) ;
		break;
	case TIT_LINK:
		itemData.SetColor(item.GetAttribColor(_T("color"))) ;
		itemData.SetColorActive(item.GetAttribColor(_T("active")));
		itemData.SetColorHover(item.GetAttribColor(_T("hover"))) ;
		itemData.SetId(item.GetAttribInt(_T("id"))) ;
		itemData.SetBold(item.GetAttribInt(_T("bold"))) ;
		break;
	}

	do 
	{
		wstring wsText ;
		LPCTSTR pstr = _tcschr(pText , '<') ;
		if ( NULL == pstr )
			return false ;		
		wsText.insert(wsText.begin() , pText , pstr) ;
		pText = pstr ;
		if ( !wsText.empty() || itemData.GetType()==TIT_RETURN )
		{
			itemData.SetText(wsText) ;
			m_ItemList.push_back(itemData) ;
		}

		LPCTSTR pTemp = pText ;
		if(!item.SetItem(pTemp))
			return false ;

		if ( !item.IsItemEnd() )
		{
			if(!x_Parse(pText) )
				return false ;
		}
		else
		{
			pText = pTemp ;
			break;
		}
	} while (true);
	
	return true ;
}