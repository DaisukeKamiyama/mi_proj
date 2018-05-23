/*

Copyright (C) 2018  Daisuke Kamiyama (https://www.mimikaki.net)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
/*

AMenuWrapper

*/

#pragma once

#include "../AbsBase/ABase.h"

/**
メニュー Wrapper
*/
class AMenuWrapper
{
  public:
	//全体
	static void				InitMenu();
	static AMenuRef			GetRootMenu();
	
	//メニュー
	static AMenuRef			CreateMenu();
	static void				DeleteMenu( const AMenuRef inMenuRef );
	static AItemCount		GetMenuItemCount( const AMenuRef inMenuRef );
	static void				EnableAutoEnableMenu( const AMenuRef inMenuRef );
	
	//メニュー項目　追加・削除
	//メニュー項目　情報設定・取得
	static AMenuRef			GetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex );
	static void				SetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuRef inSubMenuRef );
	static void				DetachSubMenu( const AMenuRef inSubMenuRef, AMenuRef& outOldParentMenuRef, AIndex& outOldParentMenuItemIndex );//#688
	static AMenuItemID		GetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex );
	static void				SetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuItemID inMenuItemID );
	static void				SetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText );
	static void				GetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, AText& outText );
	static void				GetMenuTitleText( const AMenuRef inMenuRef, AText& outText );
	static void				SetEnableMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, const ABool inEnable );
	//static void				SetEnableMenuItemAll( const AMenuRef inMenuRef, const ABool inEnable );//#688
	static void				InsertMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText, 
							const AMenuItemID inItemID,
							const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys );
	static void				AddMenuItem( const AMenuRef inMenuRef, const AText& inText, 
							const AMenuItemID inItemID,
							const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys );//#688
	static void				InsertMenuItem_Separator( const AMenuRef inMenuRef, const AIndex inItemIndex,
							const AMenuItemID inItemID );//#427
	static void				AddMenuItem_Separator( const AMenuRef inMenuRef, const AMenuItemID inItemID );//#427
	static void				DeleteMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex );
	static void				DeleteAllMenuItems( const AMenuRef inMenuRef );
	static void				AddMenuItemsFromTextArray( const AMenuRef inMenuRef, const ATextArray& inTextArray );
	static void				SetMenuItemsFromTextArray( const AMenuRef inMenuRef, const ATextArray& inTextArray );
	static void				SetCheckMark( const AMenuRef inMenuRef, const AIndex inItemIndex, const ABool inCheck );
	static void				SetShortcut( const AMenuRef inMenuRef, const AIndex inItemIndex, const ANumber inShortcutKey, 
							const AMenuShortcutModifierKeys inShortcutModifierKeys );
	static void				GetShortcut( const AMenuRef inMenuRef, const AIndex inItemIndex, 
							ANumber& outShortcutKey, AMenuShortcutModifierKeys& outShortcutModifierKeys );//#798
	static void				GetShortcutDisplayText( const AMenuRef inMenuRef, const AIndex inItemIndex, AText& outText );//#798
	static void				ClearAllCheckMark( const AMenuRef inMenuRef );
	static AIndex			GetMenuItemIndexByText( const AMenuRef inMenuRef, const AText& inText );
	static void				InsertFontMenu( const AMenuRef inMenuRef, const AIndex inMenuItemIndex, const AMenuItemID inItemID );
	static AMenuRef			RegisterContextMenu( AConstCharPtr inMenuName );
	
	//コンテキストメニュー
	//#688 static void				ShowContextMenu( const AMenuRef inMenuRef, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef );//win 080713
	//winstatic void				SetMenuItemIcon( const AControlID inID, const AIndex inIndex, const AIconID inIconID );//#232
	static void				SetMenuItemIcon( const AMenuRef inMenuRef, const AIndex inItemIndex, const AIconID inIconID );//win
	
};

/**
メニュー Wrapper
*/
/*#1034
class ACarbonMenuWrapper
{
  public:
	//全体
	static void				InitMenu();
	static ACarbonMenuRef			GetRootMenu();
	
	//メニュー
	static ACarbonMenuRef			CreateMenu();
	static void				DeleteMenu( const ACarbonMenuRef inMenuRef );
	static AItemCount		GetMenuItemCount( const ACarbonMenuRef inMenuRef );
	
	//メニュー項目　追加・削除
	//メニュー項目　情報設定・取得
	static ACarbonMenuRef			GetSubMenu( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex );
	static void				SetSubMenu( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ACarbonMenuRef inSubMenuRef );
	static AMenuItemID		GetMenuItemID( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex );
	static void				SetMenuItemID( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AMenuItemID inMenuItemID );
	static void				SetMenuItemText( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText );
	static void				GetMenuItemText( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, AText& outText );
	static void				GetMenuTitleText( const ACarbonMenuRef inMenuRef, AText& outText );
	static void				SetEnableMenuItem( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ABool inEnable );
	static void				InsertMenuItem( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText, 
							const AMenuItemID inItemID,
							const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys );
	static void				InsertMenuItem_Separator( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex,
							const AMenuItemID inItemID );//#427
	static void				AddMenuItem_Separator( const ACarbonMenuRef inMenuRef, const AMenuItemID inItemID );//#427
	static void				DeleteMenuItem( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex );
	static void				DeleteAllMenuItems( const ACarbonMenuRef inMenuRef );
	static void				AddMenuItemsFromTextArray( const ACarbonMenuRef inMenuRef, const ATextArray& inTextArray );
	static void				SetMenuItemsFromTextArray( const ACarbonMenuRef inMenuRef, const ATextArray& inTextArray );
	static void				SetCheckMark( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ABool inCheck );
	static void				SetShortcut( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const ANumber inShortcutKey, 
							const AMenuShortcutModifierKeys inShortcutModifierKeys );
	static void				ClearAllCheckMark( const ACarbonMenuRef inMenuRef );
	static AIndex			GetMenuItemIndexByText( const ACarbonMenuRef inMenuRef, const AText& inText );
	static void				InsertFontMenu( const ACarbonMenuRef inMenuRef, const AIndex inMenuItemIndex, const AMenuItemID inItemID );
	static ACarbonMenuRef			RegisterContextMenu( AConstCharPtr inMenuName );
	
	//コンテキストメニュー
	static void				ShowContextMenu( const ACarbonMenuRef inMenuRef, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef );//win 080713
	//winstatic void				SetMenuItemIcon( const AControlID inID, const AIndex inIndex, const AIconID inIconID );//#232
	static void				SetMenuItemIcon( const ACarbonMenuRef inMenuRef, const AIndex inItemIndex, const AIconID inIconID );//win
	
};
*/
