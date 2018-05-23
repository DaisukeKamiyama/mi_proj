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

AMenuManager

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "ADynamicMenu.h"

#pragma mark ===========================
#pragma mark [クラス]AMenuManager
/**
メニュー管理クラス
AApplicationオブジェクトによって保持される。（アプリに一つ存在する） 
*/
class AMenuManager : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	AMenuManager();
	~AMenuManager();
	
	//メニューには３種類ある。
	//1. メニューバーのメニュー
	//2. コンテクストメニュー
	//3. コントロールのメニュー
	
	//「1. メニューバーのメニュー」の管理について
	//メニューバーのメニューの項目は固定メニュー項目と動的メニュー項目に分けられる
	//固定メニュー項目：普通のメニュー項目。１つのMenuItemIDを持つ。（MenuItemIDはInterfaceBuilderではCommandIDとして設定する）
	//動的メニュー項目：「最近開いたファイル」等、動的に変化するメニュー。ひとまとまりの動的メニュー全体で１つのMenuItemIDを持つ。InterfaceBuilder上で１つのダミーメニュー項目を設定する
	//動的メニュー項目は、それぞれ動的なサブメニューを持ちうる。
	
	//メニューバーのメニューの管理データ初期化 
  public:
	void				InitMenuBarMenu();
  private:
	void				RegisterMenuBarMenuItem( const AMenuRef inMenuRef );
	AHashArray<AMenuItemID>			mMenuBarMenuItemArray_MenuItemID;
	AArray<AMenuRef>				mMenuBarMenuItemArray_MenuRef;
	AArray<AIndex>					mMenuBarMenuItemArray_MenuItemStaticIndex;
	AArray<ABool>					mMenuBarMenuItemArray_OnlyStatic;
	AArray<AObjectID>				mMenuBarMenuItemArray_DynamicMenuID;
	AArray<ABool>					mMenuBarMenuItemArray_IsDynamicMenu;
	
	AHashArray<AMenuRef>			mMenuBarMenuRefArray;//#654 高速化
	AObjectArray< AArray<AIndex> >	mMenuBarMenuRefArray_ItemIndexArray;
	
	//固定メニュー項目制御 
  public:
	void				SetMenuItemText( const AMenuItemID inItemID, const AText& inText );
	void				GetMenuItemText( const AMenuItemID inItemID, AText& outText ) const;
	void				SetEnableMenuItem( const AMenuItemID inItemID, const ABool inEnable );
	void				SetMenuItemShortcut( const AMenuItemID inItemID, const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys );
	void				CheckSameNumberMenuItem( const AArray<AMenuItemID>& inItemIDArray, const AFloatNumber inNumber );//win
	void				SetCheckMark( const AMenuItemID inItemID, const ABool inCheck );
	void				DisableAllMenuBarMenuItems();
	void				StartEnableMenuItemBufferredMode() { mEnableMenuItemBufferredMode = true; }
	void				EndEnableMenuItemBufferredMode();
	ABool				GetBufferredEnableMenuItem( const AMenuItemID inMenuItemID ) const;
	ABool				GetEnableMenuItemBufferredMode() const { return mEnableMenuItemBufferredMode; }//#688
	void				GetMenuItemShortcut( const AMenuItemID inItemID, ANumber& outShortcutKey, AMenuShortcutModifierKeys& outShortcutModifierKeys ) const;//#798
	void				GetMenuItemShortcutDisplayText( AMenuItemID inItemID, AText& outText ) const;//#798
  private:
	ABool							mEnableMenuItemBufferredMode;
	AHashArray<AMenuItemID>			mEnableMenuItemBuffer_ItemID;
	AArray<ABool>					mEnableMenuItemBuffer_BufferEnabled;
	AArray<ABool>					mEnableMenuItemBuffer_CurrentEnabled;
	
	//RealMenuItemIndex取得 
  public:
	AIndex				GetRealMenuItemIndex( const AMenuItemID inItemID ) const;
  private:
	AIndex				GetRealMenuItemIndexByMenuItemArrayIndex( const AIndex inMenuItemArrayIndex ) const;
	
	//動的メニューの生成／削除／取得 
  public:
	AObjectID				RegisterDynamicMenuItem( const AMenuItemID inItemID );
	void					ReserveDynamicMenuItem( const AMenuItemID inItemID );
	AObjectID				CreateDynamicMenu( const AMenuItemID inItemID, const ABool inIsRoot );
	AItemCount				GetDynamicMenuItemCountByObjectID( const AObjectID inDynamicMenuObjectID ) const;
	AItemCount				GetDynamicMenuItemCountByMenuItemID( const AMenuItemID inMenuItemID ) const;
	void					DeleteDynamicMenuByObjectID( const AObjectID inDynamicMenuID );
	ADynamicMenu&			GetDynamicMenuByObjectID( const AObjectID inDynamicMenuObjectID );
	ADynamicMenu&			GetDynamicMenuConstByObjectID( const AObjectID inDynamicMenuObjectID ) const;
	ADynamicMenu&			GetDynamicMenuByMenuItemID( const AMenuItemID inMenuItemID );
	ADynamicMenu&			GetDynamicMenuConstByMenuItemID( const AMenuItemID inMenuItemID ) const;
	void					RealizeDynamicMenu( const AObjectID inDynamicMenuObjectID );
	void					UnrealizeDynamicMenu( const AObjectID inDynamicMenuObjectID );
	void					UpdateDynamicMenuEnableMenuItem( const AMenuItemID inMenuItemID );//#129
	void					SetDynamicMenuAsFontMenu( const AMenuItemID inItemID, const ABool inEnableDefaultFontItem = false );//#375
	AObjectID				GetDynamicMenuObjectIDByMenuItemID( const AMenuItemID inMenuItemID );
	ABool					GetDynamicMenuRealizedByObjectID( const AObjectID inDynamicMenuObjectID ) const;
	ABool					GetDynamicMenuEnableMenuItem( const AMenuItemID inMenuItemID, const AIndex inMenuItemIndex, ABool& isEnabled ) const;//#688
  private:
	AObjectArrayIDIndexed<ADynamicMenu>			mDynamicMenuArray;//#693
	mutable AThreadMutex				mDynamicMenuArrayMutex;
	
	//動的メニューの項目追加／削除／取得 
  public:
	void					AddDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, 
							const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
							const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
							const ABool inEnabled = true );//#129
	void					AddDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, 
							const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
							const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
							const ABool inEnabled = true );//#129
	void					InsertDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, 
							const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
							const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
							const ABool inEnabled = true );//#129
	void					InsertDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
							const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
							const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
							const ABool inEnabled = true );//#129
	void					DeleteDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex );
	void					DeleteDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex );
	void					DeleteAllDynamicMenuItemsByMenuItemID( const AMenuItemID inItemID );
	void					DeleteAllDynamicMenuItemsByObjectID( const AObjectID inDynamicMenuObjectID );
	void					MoveDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inOldIndex, const AIndex inNewIndex );//R0173
	void					SetDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const ATextArray& inMenuTextArray, const ATextArray& inActionTextArray );
	void					GetMenuActionText( const AMenuItemID inItemID, const AMenuRef inMenuRef, const AIndex inRealMenuItemIndex, AText& outText ) const;
	void					SetMenuItemIcon( const AMenuItemID inItemID, const AIconID inIconID, const AIndex inOffset = 0 );//win
	AObjectID				GetDynamicMenuItemSubMenuObjectIDByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex ) const;
	void					SetDynamicMenuSubMenu( const AMenuItemID inItemID, const AIndex inItemIndex, const AObjectID inSubMenuObjectID );
	void					GetDynamicMenuItemActionTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, AText& outText ) const;
	void					GetDynamicMenuItemActionTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, AText& outText ) const;
	void					SetDynamicMenuItemActionTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, const AText& inText );
	void					SetDynamicMenuItemActionTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, const AText& inText );
	void					GetDynamicMenuItemMenuTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, AText& outText ) const;
	void					SetDynamicMenuItemMenuTextByMenuItemID(  const AMenuItemID inItemID, const AIndex inItemIndex, const AText& inText );//#862
	void					GetDynamicMenuItemMenuTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, AText& outText ) const;
	void					SetDynamicMenuItemMenuTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, const AText& inText );
	void					GetDynamicMenuItemShortcutByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
							ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const;
	void					SetDynamicMenuItemShortcutByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
																 const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers ) const;
	void					SetDynamicMenuItemShortcutByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, 
																   const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers ) const;
	void					SetCheckMarkToDynamicMenu( const AMenuItemID inItemID, const AIndex inIndex );//B0308
	void					SetCheckMarkToDynamicMenuWithActionText( const AMenuItemID inItemID, const AText& inActionText );
	AIndex					FindDynamicMenuItemIndexFromActionText( const AObjectID inDynamicMenuObjectID, const AText& inActionText ) const;//#695

#if 0
win対応
CWindowImpへ移動
	//TextArrayメニュー 
  public:
	void						RegisterTextArrayMenu( const AMenuRef inMenuRef, const ATextArrayMenuID inTextArrayMenuID, //win const AControlRef inControlRef );
								const AWindowID inWindowID = kObjectID_Invalid, const AControlID inControlID = kObjectID_Invalid );
	void						UnregisterTextArrayMenu( const AMenuRef inMenuRef );
	void						UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, const ATextArray& inTextArray );
	void						RegisterTextArrayMenuID( const ATextArrayMenuID inTextArrayMenuID );
  private:
	AArray<AMenuRef>						mTextArrayMenuArray_MenuRef;
	AArray<ATextArrayMenuID>				mTextArrayMenuArray_TextArrayMenuID;
	//win AArray<AControlRef>						mTextArrayMenuArray_ControlRef;
	AArray<AObjectID>						mTextArrayMenuArray_WindowID;
	AArray<AControlID>						mTextArrayMenuArray_ControlID;
	ADataBase								mTextArrayMenuDB;
#endif
	
	//Fontメニュー 
  public:
	void						RegisterFontMenu( const AMenuRef inMenuRef /*win , const AControlRef inControlRef*/ );
	
	//コンテクストメニュー 
  public:
	void						RegisterContextMenu( AConstCharPtr inMenuName, const AContextMenuID inContextMenuID );
	void						RegisterContextMenu( const AContextMenuID inContextMenuID );//#0
	void						SetContextMenuItemText( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const AText& inText );
	AItemCount					GetContextMenuItemCount( const AContextMenuID inContextMenuID ) const;//#232
	void						SetContextMenuEnableMenuItem( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const ABool inEnable );//#232
	void						GetContextMenuItemText( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, AText& outText ) const;//#232
	//#688 void						ShowContextMenu( const AIndex inContextMenuIndex, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef );
	AMenuRef					GetContextMenu( const AContextMenuID inContextMenuID ) const;//#688
	void						SetContextMenuItemSubMenuFromDynamicMenu( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, 
								const AObjectID inSubDynamicMenuObjectID,
								AArray<AMenuRef>& outSubMenuArray, //#688
								AArray<AMenuRef>& outOldParentMenuArray, AArray<AIndex>& outOldParentItemIndexArray);//#688
	void						SetContextMenuTextArray( const AContextMenuID inContextMenuID, const ATextArray& inTextArray, const ATextArray& inActionTextArray );//#232
	void						SetContextMenuTextArrayMenu( const AContextMenuID inContextMenuID, const ATextArrayMenuID inTextArrayMenuID );//#232
	void						SetContextMenuMenuItemID( const AContextMenuID inContextMenuID, const AMenuItemID inMenuItemID );//#232
	void						SetContextMenuCheckMark( const AContextMenuID inContextMenuID, const AText& inText );//#232
	void						SetContextMenuItemIcon( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const AIconID inIconID );//win
  private:
	AArray<AContextMenuID>					mContextMenuArray_ID;
	AArray<AMenuRef>						mContextMenuArray_MenuRef;
	AObjectArray< ATextArray >				mContextMenuArray_ActionText;
	
	//Object情報取得 
  public:
	virtual AConstCharPtr	GetClassName() const { return "AMenuManager"; }
	
};



