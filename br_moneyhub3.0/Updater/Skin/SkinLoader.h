#pragma once
#include "SkinProperty.h"
#include "TuoImage.h"


enum BorderEffect
{
	Border_OwnerDrawn,		// 自绘边框
	Border_SystemBorder,	// 使用系统自带边框
	Border_AdvanceAero		// 使用类似ie8那种全透明边框
};


// 注：这个类整个主框架进程中就只有一份

class CSkinLoader
{

public:

	static void Initialize();

	void LoadSkinProperty();

private:

	CSkinLoader();


	CSkinProperty m_propertyReader;

public:

	BorderEffect m_eDisplayBorder;		// 窗口边框模式
	int m_iNeedDuplicateFrame;			// 是否需要复制frame，0=暂不需要，1=需要，2=已经复制过，不再需要了
	int m_iRebarAeroHeight;				// rebar部分aero效果显示到的行数（>10表示显示整个rebar）
	bool m_bStatusbarAero;				// statusbar部分aero效果显示
	bool m_bTabAero;					// tabbar部分的aero效果显示


	static bool sm_bIsSupportAreoEffect;
	static bool sm_bIsVista;
	static bool IsSupportAeroEffect() { return sm_bIsSupportAreoEffect; }
	static bool IsVista() { return sm_bIsVista; }
	static void RefreshAeroStatus();
};


extern CSkinLoader *g_pSkinLoader;

inline CSkinLoader* sl()
{
	return g_pSkinLoader;
}
