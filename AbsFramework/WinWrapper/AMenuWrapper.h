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

class AMenuWrapper
{
  public:
	static void				InitMenu();
	static AMenuRef			GetRootMenu();
	static AMenuRef			CreateMenu();
	static void				DeleteMenu( const AMenuRef inMenuRef );
	static AItemCount		GetMenuItemCount( const AMenuRef inMenuRef );
	static AMenuRef			GetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex );
	static void				SetSubMenu( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuRef inSubMenuRef );
	static AMenuItemID		GetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex );
	static void				SetMenuItemID( const AMenuRef inMenuRef, const AIndex inItemIndex, const AMenuItemID inMenuItemID );
	static void				SetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, const AText& inText );
	static void				GetMenuItemText( const AMenuRef inMenuRef, const AIndex inItemIndex, AText& outText );
	static void				SetEnableMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, const ABool inEnable );
	static void				InsertMenuItem( const AMenuRef inMenuRef, const AIndex inItemIndex, 
							const AText& inText, const AMenuItemID inItemID,
							const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys );
	//static void				AddMenuItem( const AMenuRef inMenuRef, const AText& inText, const AMenuItemID inItemID,
	//						const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys );
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
	static void				ClearAllCheckMark( const AMenuRef inMenuRef );
	static AIndex			GetMenuItemIndexByText( const AMenuRef inMenuRef, const AText& inText );
	//win static AIndex			GetMenuItemIndexByFont( const AMenuRef inMenuRef, const AFont inFont );
	//win static AFont			GetFontByMenuItemIndex( const AMenuRef inMenuRef, const AIndex inMenuItem );
	static void				InsertFontMenu( const AMenuRef inMenuRef, const AIndex inMenuItemIndex, const AMenuItemID inItemID );
	static AMenuRef			RegisterContextMenu( AConstCharPtr inMenuName );
	static void				ShowContextMenu( const AMenuRef inMenuRef, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef );//win 080712
	static void				SetMenuItemIcon( const AMenuRef inMenuRef, const AIndex inItemIndex, const AIconID inIconID );//win
	
  private:
	static void				SetID0ForSubMenuRoot( const AMenuRef inMenuRef );
	static HMENU						sMainMenu;
	
};

