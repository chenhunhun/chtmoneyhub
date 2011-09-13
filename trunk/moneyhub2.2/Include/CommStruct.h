#pragma once
#include <string>


#define ICON_DEFAULT_PAGE			(HICON)0
#define ICON_HOME					(HICON)-1


//////////////////////////////////////////////////////////////////////////

// 这个不用多说了。。。到处都在用
// 创建CreateNewWebPageData是变长的，有如下可能：
// 1. sizeof(CreateNewWebPageData) + sizeof(URL)
// 2. sizeof(CreateNewWebPageData) + sizeof(Search Text)
class CTabItem;

struct CreateNewWebPageData
{
	DWORD dwSize;			// 若dwSize=0，说明lpszURL指向的是一个真实的URL字符串地址，否则lpszURL是一个标记，指向结构体的末尾数据段
	int iCategory;

	LPCTSTR lpszURL;
	BOOL bShowImmediately;

	// 用户临时替换父窗口
	HWND hAxControlWnd;
	CTabItem *lpTabItemData;
	HWND hFrame;			// 将hAxControlWnd移到这个frame里面，如果为空表示新创建一个frame
	HWND hChildFrame;		// 将hAxControlWnd移到这个ChildFrame的右边，如果为空表示移到最左边

	HWND hCreateFromChildFrame;		// 通过点击链接创建的新页面，这个变量为点击链接的那个CChildFrame
	HWND hCreateFromMainFrame;		// 若是新窗口，则记录原先是从那个创建的

	int iNewPosition;		// 新建标签时放置的位置，0表示默认（即根据配置文件来确定），-1表示放到末尾
	int iWeight;			// 权重，用来评估这个页面的访问重要性

	bool bNoClose;
};
