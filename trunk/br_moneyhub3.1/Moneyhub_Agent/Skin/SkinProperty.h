#pragma once


enum SkinProperty
{
	Skin_Property_Use_Aero_Effect,
	Skin_Property_Aero_Type,

	Skin_Property_Rebar_Aero_Type,
	Skin_Property_Rebar_Aero_Sep,
	Skin_Property_Rebar_Skin_Top_Border_Height,
	Skin_Property_Rebar_Skin_Bottom_Border_Height,
	Skin_Property_Rebar_Overlay,
	Skin_Property_Rebar_Overlay_Stretch_Left,
	Skin_Property_Rebar_Overlay_Stretch_Right,
	Skin_Property_Rebar_Overlay_Vertical_Stretch,
	Skin_Property_Rebar_Overlay_Margin_Top,
	Skin_Property_Rebar_Overlay_Margin_Left,
	Skin_Property_Rebar_Overlay_Margin_Right,
	Skin_Property_Rebar_Overlay_Corner_Size,

	Skin_Property_Tabbar_Aero,
	Skin_Property_Statusbar_Aero,

	Skin_Property_Frame_Border_Width,
	Skin_Property_Frame_Border_Title_Activate_Color,
	Skin_Property_Frame_Border_Title_Deactivate_Color,
	Skin_Property_Frame_Border_Corner_Size,
	Skin_Property_Frame_Border_Corner_At_Bottom,
	Skin_Property_Frame_System_Button_Top_Margin,
	Skin_Property_Frame_System_Button_Right_Margin,

	Skin_Property_Toolbar_UndoButton_DropdownArrow_Width,
	Skin_Property_Toolbar_Seperator_Color,
	Skin_Property_Toolbar_Text_Color,
	Skin_Property_Toolbar_Text_Aero_Color,
	Skin_Property_Toolbar_Text_Aero_Glow,

	Skin_Property_AddressBar_EditCtrl_Use_Big_Font,
	Skin_Property_AddressBar_Sync_State,

	Skin_Property_AddressBar_DropdownArrow_Width,
	Skin_Property_AddressBar_Dropdown_Border_Color,
	Skin_Property_AddressBar_Dropdown_URL_Color,
	Skin_Property_AddressBar_Dropdown_Title_Color,
	Skin_Property_AddressBar_Dropdown_Local_Background_Color,
	Skin_Property_AddressBar_Dropdown_Suggestion_Background_Color,
	Skin_Property_AddressBar_Dropdown_AutoComplete_Background_Color,
	Skin_Property_AddressBar_Dropdown_Seperator_Color,

	Skin_Property_SearchBar_EditCtrl_Use_Big_Font,
	Skin_Property_SearchBar_Sync_State,

	Skin_Property_Tabbar_Margin,
	Skin_Property_Tabbar_Overlap_Left,
	Skin_Property_Tabbar_Overlap_Right,
	Skin_Property_Tabbar_Text_Color,
	Skin_Property_Tabbar_Text_Current_Color,
	Skin_Property_Tabbar_Text_Top_Margin,

	Skin_Property_Sidebar_Subbar_Background_Color,
	Skin_Property_Sidebar_Subbar_Text_Color,
	Skin_Property_Sidebar_Subbar_Text_Hover_Color,
	Skin_Property_Sidebar_Subbar_Text_Selected_Color,

	Skin_Property_Statusbar_Bottom_Border_Height,
	Skin_Property_Statusbar_Text_Color,
	Skin_Property_Statusbar_Progress_Min_Width,

	Skin_Property_Float_Text_Color,

	Skin_Property_Menu_Background_Color,
	Skin_Property_Menu_Border_Color,
	Skin_Property_Menu_Vertical_Sep_Color,
	Skin_Property_Menu_Text_Color,
	Skin_Property_Menu_Disabled_Text_Color,
	Skin_Property_Menu_Shortcut_Text_Color,

	Skin_Property_End
};

#define SKIN_VERSION			0x10000003


enum DataType
{
	DataType_Int,
	DataType_String
};


class CSkinProperty
{

public:

	bool ReadINIData();

	static COLORREF GetColor(SkinProperty eSkinProp) { return (COLORREF)sm_dwSkinData[eSkinProp]; }
	static DWORD GetDWORD(SkinProperty eSkinProp) { return (DWORD)sm_dwSkinData[eSkinProp]; }
	static LPCTSTR GetString(SkinProperty eSkinProp) { return (LPCTSTR)sm_dwSkinData[eSkinProp]; }

private:

	static DWORD sm_dwSkinData[Skin_Property_End];
};
