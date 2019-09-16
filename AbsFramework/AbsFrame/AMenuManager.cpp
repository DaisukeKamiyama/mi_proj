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

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AMenuManager
#pragma mark ===========================
//メニュー管理クラス

#pragma mark ---コンストラクタ／デストラクタ

//コンストラクタ
AMenuManager::AMenuManager() : AObjectArrayItem(NULL)
,mDynamicMenuArray(NULL)//#693,true)//#417koko
,mEnableMenuItemBufferredMode(false)
{
}
//デストラクタ
AMenuManager::~AMenuManager()
{
}

#pragma mark ===========================

#pragma mark ---メニューバーのメニューの管理データ初期化

//メニューバーのメニューの初期化
void	AMenuManager::InitMenuBarMenu()
{
	//メニューAPI初期設定
	AMenuWrapper::InitMenu();
	
	//メニュー項目の登録
	AMenuRef	rootMenuRef = AMenuWrapper::GetRootMenu();
	for( AIndex rootIndex = 0; rootIndex < AMenuWrapper::GetMenuItemCount(rootMenuRef); rootIndex++ )
	{
		AMenuRef	menuRef = AMenuWrapper::GetSubMenu(rootMenuRef,rootIndex);
		if( menuRef != NULL )
		{
			RegisterMenuBarMenuItem(menuRef);
		}
	}
	//
	for( AIndex i = 0; i < mMenuBarMenuRefArray.GetItemCount(); i++ )
	{
		AMenuRef	menuRef = mMenuBarMenuRefArray.Get(i);
		for( AIndex j = 0; j < mMenuBarMenuItemArray_MenuRef.GetItemCount(); j++ )
		{
			if( mMenuBarMenuItemArray_MenuRef.Get(j) == menuRef )
			{
				mMenuBarMenuRefArray_ItemIndexArray.GetObject(i).Add(j);
			}
		}
	}
}

//メニューバーのメニューを再帰的に検索して、メニュー項目をmMenuBarMenuItemArray_XXXへ登録していく。
void	AMenuManager::RegisterMenuBarMenuItem( const AMenuRef inMenuRef )
{
#if IMPLEMENTATION_FOR_MACOSX
	//#688 メニューに自動validate(setAutoenablesItems:YES)を設定
	AMenuWrapper::EnableAutoEnableMenu(inMenuRef);
#endif
	//
	for( AIndex index = 0; index < AMenuWrapper::GetMenuItemCount(inMenuRef); index++ )
	{
		AMenuRef	subMenuRef = AMenuWrapper::GetSubMenu(inMenuRef,index);
		AMenuItemID	itemID = AMenuWrapper::GetMenuItemID(inMenuRef,index);
		//メニュー項目の登録（RealMenuItemIndexの計算の必要上、セパレータ等MenuItemIDが0であるものも含めて、全部登録する。）
		mMenuBarMenuItemArray_MenuItemID.Add(itemID);
		mMenuBarMenuItemArray_MenuRef.Add(inMenuRef);
		mMenuBarMenuItemArray_MenuItemStaticIndex.Add(index);
		mMenuBarMenuItemArray_DynamicMenuID.Add(kObjectID_Invalid);
		mMenuBarMenuItemArray_IsDynamicMenu.Add(false);
		mMenuBarMenuItemArray_OnlyStatic.Add(true);
		//サブメニュー有りなら再帰コール
		if( subMenuRef != NULL )
		{
			RegisterMenuBarMenuItem(subMenuRef);
		}
		
		if( mMenuBarMenuRefArray.Find(inMenuRef) == kIndex_Invalid )
		{
			mMenuBarMenuRefArray.Add(inMenuRef);
			mMenuBarMenuRefArray_ItemIndexArray.AddNewObject();
		}
	}
}

#pragma mark ===========================

#pragma mark ---固定メニュー項目制御

//全てのメニューバーメニュー項目をDisableにする
void	AMenuManager::DisableAllMenuBarMenuItems()
{
	/* AItemCount	itemCount = mMenuBarMenuItemArray_MenuItemID.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AMenuItemID	menuItemID = mMenuBarMenuItemArray_MenuItemID.Get(i);
		AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(i);
		AObjectID	dynamicMenuID = mMenuBarMenuItemArray_DynamicMenuID.Get(i);
		if( dynamicMenuID == kObjectID_Invalid && menuItemID != 0 )
		{
			AMenuWrapper::SetEnableMenuItem(menuRef,GetRealMenuItemIndex(menuItemID),false);
		}
		else if( dynamicMenuID != kObjectID_Invalid )
		{
			GetDynamicMenuByObjectID(dynamicMenuID).SetEnableMenu(false);
		}
	}*/
	/*#688 バッファーモード対応のため、SetEnableMenuItem()を使うように変更
	AItemCount	count = mMenuBarMenuRefArray.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AMenuRef	menuRef = mMenuBarMenuRefArray.Get(i);
		AItemCount	count2 = AMenuWrapper::GetMenuItemCount(menuRef);
		for( AIndex j = 0; j < count2; j++ )
		{
			if( AMenuWrapper::GetMenuItemID(menuRef,j) != 0 )
			{
				AMenuWrapper::SetEnableMenuItem(menuRef,j,false);
			}
			AMenuItemID	menuItemID = AMenuWrapper::GetMenuItemID(menuRef,j);
		}
	}
	*/
	//InitMenuBarMenu()で取得した各MenuItemIDについて、項目disableにする
	AItemCount	count = mMenuBarMenuItemArray_MenuItemID.GetItemCount();
	for( AIndex i = 0; i < count; i++ )
	{
		AMenuItemID	menuItemID = mMenuBarMenuItemArray_MenuItemID.Get(i);
		if( menuItemID != 0 )
		{
			SetEnableMenuItem(menuItemID,false);
		}
	}
}

//メニューテキスト設定
void	AMenuManager::SetMenuItemText( const AMenuItemID inItemID, const AText& inText )
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::SetMenuItemText(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),inText);
}

//メニューテキスト取得
void	AMenuManager::GetMenuItemText( const AMenuItemID inItemID, AText& outText ) const
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::GetMenuItemText(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),outText);
}

//メニュー項目Enable/Disable
void	AMenuManager::SetEnableMenuItem( const AMenuItemID inItemID, const ABool inEnable )
{
	//#688 バッファーモード対応
	//バッファーモード中ならバッファへの設定のみしてリターン
	if( mEnableMenuItemBufferredMode == true )
	{
		AIndex	index = mEnableMenuItemBuffer_ItemID.Find(inItemID);
		if( index != kIndex_Invalid )
		{
			//バッファにinItemIDの項目があれば、そのバッファにinEnableを設定
			mEnableMenuItemBuffer_BufferEnabled.Set(index,inEnable);
		}
		else
		{
			//バッファにinItemIDの項目がなければ、バッファ追加して、inEnableを設定
			mEnableMenuItemBuffer_ItemID.Add(inItemID);
			mEnableMenuItemBuffer_BufferEnabled.Add(inEnable);
			mEnableMenuItemBuffer_CurrentEnabled.Add(!inEnable);//今回必ず反映させるため反転して設定
		}
		return;
	}
	
	//#688 バッファーモード対応
	//バッファーモード中でなければバッファ（と現在データ）に設定
	{
		AIndex	index = mEnableMenuItemBuffer_ItemID.Find(inItemID);
		if( index != kIndex_Invalid )
		{
			mEnableMenuItemBuffer_BufferEnabled.Set(index,inEnable);
			mEnableMenuItemBuffer_CurrentEnabled.Set(index,inEnable);
		}
	}
	
	//
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index != kIndex_Invalid )
	{
		if( mMenuBarMenuItemArray_DynamicMenuID.Get(index) != kObjectID_Invalid )
		{
			GetDynamicMenuByMenuItemID(inItemID).SetEnableMenu(inEnable);
			return;
		}
	}

	AArray<AIndex>	foundIndexArray;
	mMenuBarMenuItemArray_MenuItemID.FindAll(inItemID,foundIndexArray);
	for( AIndex i = 0; i < foundIndexArray.GetItemCount(); i++ )
	{
		AIndex	index = foundIndexArray.Get(i);
		AMenuWrapper::SetEnableMenuItem(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndexByMenuItemArrayIndex(index),inEnable);
	}
}

//#688
/**
バッファーモード終了して、メニュー項目にenable/disable設定
*/
void	AMenuManager::EndEnableMenuItemBufferredMode()
{
	//バッファーモード解除
	mEnableMenuItemBufferredMode = false;
	//バッファから読み出して、必要ならメニュー項目にenable/disable設定
	AItemCount	itemCount = mEnableMenuItemBuffer_ItemID.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		ABool	bufferEnabled = mEnableMenuItemBuffer_BufferEnabled.Get(i);
		ABool	currentEnabled = mEnableMenuItemBuffer_CurrentEnabled.Get(i);
		if( bufferEnabled != currentEnabled )
		{
			//現在値とバッファ値が異なっていたら、メニュー項目にenable/disable設定
			SetEnableMenuItem(mEnableMenuItemBuffer_ItemID.Get(i),bufferEnabled);
		}
	}
}

//#688
/**
バッファー値読み出し
*/
ABool	AMenuManager::GetBufferredEnableMenuItem( const AMenuItemID inMenuItemID ) const
{
	AIndex	index = mEnableMenuItemBuffer_ItemID.Find(inMenuItemID);
	if( index == kIndex_Invalid )   return false;//EVT_UpdateMenu()を実行後、enableにすべきものは全てmEnableMenuItemBuffer_ItemIDに登録されるはず
	return mEnableMenuItemBuffer_BufferEnabled.Get(index);
}

//メニュー項目にショートカット設定
void	AMenuManager::SetMenuItemShortcut( const AMenuItemID inItemID, const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys )
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::SetShortcut(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),inShortcutKey,inShortcutModifierKeys);
}

//#798
/**
メニュー項目のショートカットを取得
*/
void	AMenuManager::GetMenuItemShortcut( const AMenuItemID inItemID, ANumber& outShortcutKey, AMenuShortcutModifierKeys& outShortcutModifierKeys ) const
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::GetShortcut(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),outShortcutKey,outShortcutModifierKeys);
}

//#798
/**
メニュー項目の表示テキストを取得
*/
void	AMenuManager::GetMenuItemShortcutDisplayText( AMenuItemID inItemID, AText& outText ) const
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::GetShortcutDisplayText(mMenuBarMenuItemArray_MenuRef.Get(index),GetRealMenuItemIndex(inItemID),outText);
}

//メニューテキストから得られる数値がinNumberと同じものにチェックマークをつける（フォントサイズメニュー等用）
void	AMenuManager::CheckSameNumberMenuItem( const AArray<AMenuItemID>& inItemIDArray, const AFloatNumber inNumber )//win
{
	//複数のitemIDの中から検索 win
	AArray<AIndex>	foundIndexArray;
	for( AIndex i = 0; i < inItemIDArray.GetItemCount(); i++ )
	{
		AArray<AIndex>	indexArray;
		mMenuBarMenuItemArray_MenuItemID.FindAll(inItemIDArray.Get(i),indexArray);
		for( AIndex j = 0; j < indexArray.GetItemCount(); j++ )
		{
			foundIndexArray.Add(indexArray.Get(j));
		}
	}
	//
	for( AIndex i = 0; i < foundIndexArray.GetItemCount(); i++ )
	{
		AIndex	index = foundIndexArray.Get(i);
		AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
		AIndex	realMenuIetmIndex = GetRealMenuItemIndexByMenuItemArrayIndex(index);
		AText	text;
		AMenuWrapper::GetMenuItemText(menuRef,realMenuIetmIndex,text);
		if( text.GetFloatNumber() == inNumber )
		{
			AMenuWrapper::SetCheckMark(menuRef,realMenuIetmIndex,true);
		}
		else
		{
			AMenuWrapper::SetCheckMark(menuRef,realMenuIetmIndex,false);
		}
	}
}

//チェックマーク
void	AMenuManager::SetCheckMark( const AMenuItemID inItemID, const ABool inCheck )
{
	AArray<AIndex>	foundIndexArray;
	mMenuBarMenuItemArray_MenuItemID.FindAll(inItemID,foundIndexArray);
	for( AIndex i = 0; i < foundIndexArray.GetItemCount(); i++ )
	{
		AIndex	index = foundIndexArray.Get(i);
		AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
		AIndex	realMenuIetmIndex = GetRealMenuItemIndexByMenuItemArrayIndex(index);
		AMenuWrapper::SetCheckMark(menuRef,realMenuIetmIndex,inCheck);
	}
}

//
void	AMenuManager::GetMenuActionText( const AMenuItemID inItemID, const AMenuRef inMenuRef, const AIndex inRealMenuItemIndex, AText& outText ) const
{
	//ContextMenuから検索 #232
	AIndex	contextMenuIndex = mContextMenuArray_MenuRef.Find(inMenuRef);
	if( contextMenuIndex != kIndex_Invalid )
	{
		if( mContextMenuArray_ActionText.GetObjectConst(contextMenuIndex).GetItemCount() > 0 )
		{
			outText.SetText(mContextMenuArray_ActionText.GetObjectConst(contextMenuIndex).GetTextConst(inRealMenuItemIndex));
		}
		else
		{
			AMenuWrapper::GetMenuItemText(inMenuRef,inRealMenuItemIndex,outText);
		}
		return;
	}
	//
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( menuItemArrayIndex == kIndex_Invalid )
	{
		//R0199
		AMenuWrapper::GetMenuItemText(inMenuRef,inRealMenuItemIndex,outText);
		return;
	}
	if( mMenuBarMenuItemArray_DynamicMenuID.Get(menuItemArrayIndex) == kObjectID_Invalid )
	{
		AMenuWrapper::GetMenuItemText(inMenuRef,inRealMenuItemIndex,outText);
	}
	else
	{
		//AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
		for( AIndex dynamicMenuObjectIndex = 0; dynamicMenuObjectIndex < mDynamicMenuArray.GetItemCount(); dynamicMenuObjectIndex++ )
		{
			const ADynamicMenu&	dynamicMenu = mDynamicMenuArray.GetObjectConst(dynamicMenuObjectIndex);
			if(	dynamicMenu.IsRealized() == true && 
				dynamicMenu.GetMenuRef() == inMenuRef &&
				dynamicMenu.GetMenuItemID() == inItemID )
			{
				dynamicMenu.GetActionTextByRealMenuItemIndex(inRealMenuItemIndex,outText);
				return;
			}
		}
		//#232
		AMenuWrapper::GetMenuItemText(inMenuRef,inRealMenuItemIndex,outText);
	}
}

//win
/**
メニューアイコン設定
*/
void	AMenuManager::SetMenuItemIcon( const AMenuItemID inItemID, const AIconID inIconID, const AIndex inOffset )
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   return;
	AMenuWrapper::SetMenuItemIcon(mMenuBarMenuItemArray_MenuRef.Get(index),
				GetRealMenuItemIndex(inItemID)+inOffset,inIconID);
}

#pragma mark ===========================

#pragma mark ---RealMenuItemIndex取得

/*
OS APIと受け渡す際のメニュー項目のインデックスをRealMenuItemIndexと定義する。
途中に動的メニュー項目が入っていると、InterfaceBuilderで設定されたインデックス(StaticIndex)と、実際のメニューItemIndexは異なる。
そのため、GetRealMenuItemIndex()メソッドで変換を行う必要がある。
*/

//MenuItemIDからRealMenuItemIndexを取得
AIndex	AMenuManager::GetRealMenuItemIndex( const AMenuItemID inItemID ) const
{
	//引数で指定したメニュー項目のmMenuBarMenuItemArrayのインデックスを取得する
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( menuItemArrayIndex == kIndex_Invalid )   { _ACError("",this); return kIndex_Invalid; }
	return GetRealMenuItemIndexByMenuItemArrayIndex(menuItemArrayIndex);
}

//MenuBarMenuItemArrayのインデックスからRealMenuItemIndexを取得
AIndex	AMenuManager::GetRealMenuItemIndexByMenuItemArrayIndex( const AIndex inMenuItemArrayIndex ) const
{
	//"OnlyStatic"ならStaticIndexを返す
	if( mMenuBarMenuItemArray_OnlyStatic.Get(inMenuItemArrayIndex) == true )//#1477   return mMenuBarMenuItemArray_MenuItemStaticIndex.Get(inMenuItemArrayIndex);
	{
		//macOS側で自動的にメニュー項目が追加されることがあるので、静的メニューについては、普通に、NSMenuからAMenuItemID(=tag)を検索する #1477
		AIndex	index = AMenuWrapper::GetMenuItemIndexByID(mMenuBarMenuItemArray_MenuRef.Get(inMenuItemArrayIndex), mMenuBarMenuItemArray_MenuItemID.Get(inMenuItemArrayIndex));
		if( index != kIndex_Invalid )
		{
			return index;
		}
		else
		{
			//NSMenuから見つからなかった場合は、従来通り、内部データから取得する。
			return mMenuBarMenuItemArray_MenuItemStaticIndex.Get(inMenuItemArrayIndex);
		}
	}
	
	//MenuRefが同じで、staticIndexが対象MenuItemのIndexよりも小さいものを検索し、該当するものについて実際のメニュー項目数を足していく。
	AIndex	realMenuItemIndex = 0;
	AMenuRef	targetMenuRef = mMenuBarMenuItemArray_MenuRef.Get(inMenuItemArrayIndex);
	AIndex	targetStaticIndex = mMenuBarMenuItemArray_MenuItemStaticIndex.Get(inMenuItemArrayIndex);
	//AItemCount	itemCount = mMenuBarMenuItemArray_MenuItemID.GetItemCount();
	AIndex	menuRefArrayIndex = mMenuBarMenuRefArray.Find(targetMenuRef);
	if( menuRefArrayIndex == kIndex_Invalid )   return inMenuItemArrayIndex;
	const AArray<AIndex>&	itemIndexArray = mMenuBarMenuRefArray_ItemIndexArray.GetObjectConst(menuRefArrayIndex);
	AItemCount	itemCount = itemIndexArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	index = itemIndexArray.Get(i);
		if( /* mMenuBarMenuItemArray_MenuRef.Get(i) == targetMenuRef && 
				mMenuBarMenuItemArray_MenuItemStaticIndex.Get(i) < targetStaticIndex*/
			mMenuBarMenuItemArray_MenuItemStaticIndex.Get(index) < targetStaticIndex )
		{
			AObjectID	dynamicMenuObjectID = mMenuBarMenuItemArray_DynamicMenuID.Get(index);
			ABool	isDynamicMenu = mMenuBarMenuItemArray_IsDynamicMenu.Get(index);
			if( isDynamicMenu == false )
			{
				//固定メニューの場合は+1
				realMenuItemIndex++;
			}
			else if( dynamicMenuObjectID == kObjectID_Invalid )
			{
				//なにもしない
			}
			else
			{
				//動的メニューの場合は、現在存在する項目数分を足す
				realMenuItemIndex += GetDynamicMenuConstByObjectID(dynamicMenuObjectID).GetMenuItemCount();
			}
		}
	}
	return realMenuItemIndex;
}

#pragma mark ===========================

#pragma mark ---動的メニューの生成／削除／取得

/*
動的メニュー
動的メニューは、ルートと、サブメニューから構成される。それらのMenuItemIDは全て同一である。
動的メニューの生成方法には２種類ある
1. RegisterDynamicMenuItem()　（既にメニューバーに存在するメニュー項目を動的項目として登録）
起動時点では、メニューにはひとまとまりの動的メニュー部分に対して一つのメニュー項目が存在している。
その項目についてRegisterDynamicMenuItem()をコールすると、ADynamicMenuクラスを一つ生成し、ObjectIDを返す。
動的メニューに対する処理は全てこのObjectIDを介して行う。
当然、ルートのみ。
2. CreateDynamicMenu()　（新規に生成）
CreateDynamicMenu()により動的メニューを先に生成、構成した後、実際のメニューへ実体化する。
ルート、サブメニューどちらかの場合もあり得る。
ルートの場合は、RealizeDynamicMenu()/UnrealizeDynamicMenu()で実体化／非実体化する。

*/

//メニューバーに既に存在するメニュー項目を動的メニューとして登録する（ウインドウメニュー等）
AObjectID	AMenuManager::RegisterDynamicMenuItem( const AMenuItemID inItemID )
{
	//引数で指定したメニュー項目のmMenuBarMenuItemArrayのインデックスを取得する
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   { _ACError("",this); return kObjectID_Invalid; }
	
	//動的メニューオブジェクト生成
	AObjectID	dynamicMenuObjectID = CreateDynamicMenu(inItemID,true);
	//動的メニューのObjectIDをmMenuBarMenuItemArrayへ保存
	mMenuBarMenuItemArray_DynamicMenuID.Set(index,dynamicMenuObjectID);
	mMenuBarMenuItemArray_IsDynamicMenu.Set(index,true);
	
	//同じメニュー内のメニュー項目の"OnlyStatic"フラグをfalseにする
	AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
	for( AIndex i = 0; i < 	mMenuBarMenuItemArray_MenuItemID.GetItemCount(); i++ )
	{
		if( mMenuBarMenuItemArray_MenuRef.Get(i) == menuRef )
		{
			mMenuBarMenuItemArray_OnlyStatic.Set(i,false);
		}
	}
	
	//動的メニュー実体化
	GetDynamicMenuByObjectID(dynamicMenuObjectID).Realize();
	return dynamicMenuObjectID;
}

//メニューバーにまだ存在していないメニュー項目を動的メニューとして登録する
void	AMenuManager::ReserveDynamicMenuItem( const AMenuItemID inItemID )
{
	//引数で指定したメニュー項目のmMenuBarMenuItemArrayのインデックスを取得する
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( index == kIndex_Invalid )   { _ACError("",this); return; }
	
	//
	mMenuBarMenuItemArray_IsDynamicMenu.Set(index,true);
	
	//同じメニュー内のメニュー項目の"OnlyStatic"フラグをfalseにする
	AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
	for( AIndex i = 0; i < 	mMenuBarMenuItemArray_MenuItemID.GetItemCount(); i++ )
	{
		if( mMenuBarMenuItemArray_MenuRef.Get(i) == menuRef )
		{
			mMenuBarMenuItemArray_OnlyStatic.Set(i,false);
		}
	}
	
	//実体メニュー上に存在する同一MenuItemIDのメニュー項目を削除する
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(menuRef); )
	{
		if( AMenuWrapper::GetMenuItemID(menuRef,i) == inItemID )
		{
			AMenuWrapper::DeleteMenuItem(menuRef,i);
		}
		else
		{
			i++;
		}
	}
}

//動的メニューオブジェクト生成
AObjectID	AMenuManager::CreateDynamicMenu( const AMenuItemID inItemID, const ABool inIsRoot )
{
	AMenuRef	menuRef = NULL;
	if( inIsRoot == true )
	{
		AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
		if( index == kIndex_Invalid )   {_ACThrow("invalid MenuItemID",this);}
		menuRef = mMenuBarMenuItemArray_MenuRef.Get(index);
	}
	AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
	ADynamicMenuFactory	factory(this,*this,inItemID,menuRef);
	AIndex	index = mDynamicMenuArray.AddNewObject(factory);
	return mDynamicMenuArray.GetObject(index).GetObjectID();
}

//動的メニューオブジェクト削除
void	AMenuManager::DeleteDynamicMenuByObjectID( const AObjectID inDynamicMenuObjectID )
{
	AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
	mDynamicMenuArray.Delete1Object(mDynamicMenuArray.GetIndexByID(inDynamicMenuObjectID));
}

//動的メニューオブジェクト取得
ADynamicMenu&	AMenuManager::GetDynamicMenuByObjectID( const AObjectID inDynamicMenuObjectID )
{
	AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
	/*#272
	AIndex	dynamicMenuObjectIndex = mDynamicMenuArray.GetIndexByID(inDynamicMenuObjectID);
	if( dynamicMenuObjectIndex == kIndex_Invalid )   _ACThrow("dynamic menu not found",this);
	return mDynamicMenuArray.GetObject(dynamicMenuObjectIndex);
	*/
	return mDynamicMenuArray.GetObjectByID(inDynamicMenuObjectID);
}
ADynamicMenu&	AMenuManager::GetDynamicMenuConstByObjectID( const AObjectID inDynamicMenuObjectID ) const
{
	AStMutexLocker	mutexlocker(mDynamicMenuArrayMutex);
	/*#272
	AIndex	dynamicMenuObjectIndex = mDynamicMenuArray.GetIndexByID(inDynamicMenuObjectID);
	if( dynamicMenuObjectIndex == kIndex_Invalid )   _ACThrow("dynamic menu not found",this);
	return mDynamicMenuArray.GetObjectConst(dynamicMenuObjectIndex);
	*/
	return mDynamicMenuArray.GetObjectConstByID(inDynamicMenuObjectID);
}
ADynamicMenu&	AMenuManager::GetDynamicMenuByMenuItemID( const AMenuItemID inMenuItemID )
{
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inMenuItemID);
	if( menuItemArrayIndex == kIndex_Invalid )   _ACThrow("menu menu not found",this);
	return GetDynamicMenuByObjectID(mMenuBarMenuItemArray_DynamicMenuID.Get(menuItemArrayIndex));
}
ADynamicMenu&	AMenuManager::GetDynamicMenuConstByMenuItemID( const AMenuItemID inMenuItemID ) const
{
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inMenuItemID);
	if( menuItemArrayIndex == kIndex_Invalid )   _ACThrow("menu menu not found",this);
	return GetDynamicMenuConstByObjectID(mMenuBarMenuItemArray_DynamicMenuID.Get(menuItemArrayIndex));
}

//MenuItemIDから動的メニューObjectIDを取得
AObjectID	AMenuManager::GetDynamicMenuObjectIDByMenuItemID( const AMenuItemID inMenuItemID )
{
	return GetDynamicMenuConstByMenuItemID(inMenuItemID).GetObjectID();
}

//動的メニュー項目数取得
AItemCount	AMenuManager::GetDynamicMenuItemCountByObjectID( const AObjectID inDynamicMenuObjectID ) const
{
	return GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetMenuItemCount();
}
AItemCount	AMenuManager::GetDynamicMenuItemCountByMenuItemID( const AMenuItemID inMenuItemID ) const
{
	return GetDynamicMenuConstByMenuItemID(inMenuItemID).GetMenuItemCount();
}

//動的メニュー実体化済みかどうかを取得
ABool	AMenuManager::GetDynamicMenuRealizedByObjectID( const AObjectID inDynamicMenuObjectID ) const
{
	return GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).IsRealized();
}

//動的メニュー実体化
void	AMenuManager::RealizeDynamicMenu( const AObjectID inDynamicMenuObjectID )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).Realize();
	AMenuItemID	menuItemID = GetDynamicMenuByObjectID(inDynamicMenuObjectID).GetMenuItemID();
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(menuItemID);
	mMenuBarMenuItemArray_DynamicMenuID.Set(menuItemArrayIndex,inDynamicMenuObjectID);
	
	//同じメニュー内のメニュー項目の"OnlyStatic"フラグをfalseにする
	AMenuRef	menuRef = mMenuBarMenuItemArray_MenuRef.Get(menuItemArrayIndex);
	for( AIndex i = 0; i < 	mMenuBarMenuItemArray_MenuItemID.GetItemCount(); i++ )
	{
		if( mMenuBarMenuItemArray_MenuRef.Get(i) == menuRef )
		{
			mMenuBarMenuItemArray_OnlyStatic.Set(i,false);
		}
	}
	
}

//動的メニュー実体化解除
void	AMenuManager::UnrealizeDynamicMenu( const AObjectID inDynamicMenuObjectID )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).Unrealize();
	AMenuItemID	menuItemID = GetDynamicMenuByObjectID(inDynamicMenuObjectID).GetMenuItemID();
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(menuItemID);
	mMenuBarMenuItemArray_DynamicMenuID.Set(menuItemArrayIndex,kObjectID_Invalid);
}

//#129
/**
Enable/Disable更新
*/
void	AMenuManager::UpdateDynamicMenuEnableMenuItem( const AMenuItemID inMenuItemID )
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inMenuItemID);
	if( index != kIndex_Invalid )
	{
		if( mMenuBarMenuItemArray_DynamicMenuID.Get(index) != kObjectID_Invalid )
		{
			GetDynamicMenuByMenuItemID(inMenuItemID).UpdateEnableItems();
		}
	}
}

//#688
/**
DynamicMenuの各menu itemのEnable/Disable状態を取得
*/
ABool	AMenuManager::GetDynamicMenuEnableMenuItem( const AMenuItemID inMenuItemID, const AIndex inMenuItemIndex, ABool& isEnabled ) const
{
	AIndex	index = mMenuBarMenuItemArray_MenuItemID.Find(inMenuItemID);
	if( index != kIndex_Invalid )
	{
		if( mMenuBarMenuItemArray_DynamicMenuID.Get(index) != kObjectID_Invalid )
		{
			GetDynamicMenuConstByMenuItemID(inMenuItemID).GetDynamicMenuEnableMenuItem(inMenuItemIndex,isEnabled);
			return true;
		}
	}
	return false;
}

//動的メニューをフォントメニューとして設定する
void	AMenuManager::SetDynamicMenuAsFontMenu( const AMenuItemID inItemID, const ABool inEnableDefaultFontItem )//#375
{
	GetDynamicMenuByMenuItemID(inItemID).SetAsFontMenu(inEnableDefaultFontItem);//#375
}

#pragma mark ===========================

#pragma mark ---動的メニューの項目追加／削除／取得

//動的メニュー項目追加
void	AMenuManager::AddDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
		const ABool inEnabled )//#129
{
	GetDynamicMenuByMenuItemID(inItemID).AddMenuItem(inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);
}
void	AMenuManager::AddDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
		const ABool inEnabled )//#129
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).AddMenuItem(inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);
}

//動的メニュー項目追加
void	AMenuManager::InsertDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, 
		const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
		const ABool inEnabled )//#129
{
	GetDynamicMenuByMenuItemID(inItemID).InsertMenuItem(inItemIndex,inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);//#129
}
void	AMenuManager::InsertDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
		const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator,
		const ABool inEnabled )//#129
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).InsertMenuItem(inItemIndex,inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);//#129
}

//動的メニュー項目削除
void	AMenuManager::DeleteDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex )
{
	GetDynamicMenuByMenuItemID(inItemID).DeleteMenuItem(inItemIndex);
}
void	AMenuManager::DeleteDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).DeleteMenuItem(inItemIndex);
}

//動的メニュー項目削除（全削除）
void	AMenuManager::DeleteAllDynamicMenuItemsByMenuItemID( const AMenuItemID inItemID )
{
	GetDynamicMenuByMenuItemID(inItemID).DeleteAllMenuItems();
}
void	AMenuManager::DeleteAllDynamicMenuItemsByObjectID( const AObjectID inDynamicMenuObjectID )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).DeleteAllMenuItems();
}

//動的メニュー項目移動 R0173
void	AMenuManager::MoveDynamicMenuItemByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inOldIndex, const AIndex inNewIndex )
{
	GetDynamicMenuByObjectID(inDynamicMenuObjectID).MoveMenuItem(inOldIndex,inNewIndex);
}

//動的メニュー項目設定（TextArrayから）
void	AMenuManager::SetDynamicMenuItemByMenuItemID( const AMenuItemID inItemID, const ATextArray& inMenuTextArray, const ATextArray& inActionTextArray )
{
	GetDynamicMenuByMenuItemID(inItemID).SetMenuItem(inMenuTextArray,inActionTextArray);
}

//動的メニュー項目のActionText取得／設定（動的メニューObjectID, itemindexでの指定）
void	AMenuManager::GetDynamicMenuItemActionTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, AText& outText ) const
{
	GetDynamicMenuConstByMenuItemID(inItemID).GetActionTextByMenuItemArrayIndex(inItemIndex,outText);
}
void	AMenuManager::GetDynamicMenuItemActionTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, AText& outText ) const
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetActionTextByMenuItemArrayIndex(inItemIndex,outText);
}
void	AMenuManager::SetDynamicMenuItemActionTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, const AText& inText )
{
	GetDynamicMenuByMenuItemID(inItemID).SetActionTextByMenuItemArrayIndex(inItemIndex,inText);
}
void	AMenuManager::SetDynamicMenuItemActionTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, const AText& inText )
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).SetActionTextByMenuItemArrayIndex(inItemIndex,inText);
}

//動的メニュー項目のテキスト取得／設定（動的メニューObjectID, itemindexでの指定）
void	AMenuManager::GetDynamicMenuItemMenuTextByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, AText& outText ) const
{
	GetDynamicMenuConstByMenuItemID(inItemID).GetMenuTextByMenuItemArrayIndex(inItemIndex,outText);
}
void	AMenuManager::GetDynamicMenuItemMenuTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, AText& outText ) const
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetMenuTextByMenuItemArrayIndex(inItemIndex,outText);
}
void	AMenuManager::SetDynamicMenuItemMenuTextByMenuItemID(  const AMenuItemID inItemID, const AIndex inItemIndex, const AText& inText )
{
	GetDynamicMenuByMenuItemID(inItemID).SetMenuTextByMenuItemArrayIndex(inItemIndex,inText);
}
void	AMenuManager::SetDynamicMenuItemMenuTextByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, const AText& inText )
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).SetMenuTextByMenuItemArrayIndex(inItemIndex,inText);
}

//動的メニュー項目のショートカット取得／設定（動的メニューObjectID, itemindexでの指定）
void	AMenuManager::GetDynamicMenuItemShortcutByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
		ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetShortcutByMenuItemArrayIndex(inItemIndex,outShortcut,outModifiers);
}
void	AMenuManager::SetDynamicMenuItemShortcutByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex, 
			const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers ) const
{
	GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).SetShortcutByMenuItemArrayIndex(inItemIndex,inShortcut,inModifiers);
}
void	AMenuManager::SetDynamicMenuItemShortcutByMenuItemID( const AMenuItemID inItemID, const AIndex inItemIndex, 
															 const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers ) const
{
	GetDynamicMenuConstByMenuItemID(inItemID).SetShortcutByMenuItemArrayIndex(inItemIndex,inShortcut,inModifiers);
}

//動的メニュー項目のサブメニューObjectID取得／設定
AObjectID	AMenuManager::GetDynamicMenuItemSubMenuObjectIDByObjectID( const AObjectID inDynamicMenuObjectID, const AIndex inItemIndex ) const
{
	return GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).GetSubMenuObjectIDByMenuItemArrayIndex(inItemIndex);
}
void	AMenuManager::SetDynamicMenuSubMenu( const AMenuItemID inItemID, const AIndex inItemIndex, const AObjectID inSubMenuObjectID )
{
	GetDynamicMenuByMenuItemID(inItemID).SetSubMenu(inItemIndex,inSubMenuObjectID);
}

//B0308
//動的メニュー項目のチェックマーク設定
void	AMenuManager::SetCheckMarkToDynamicMenu( const AMenuItemID inItemID, const AIndex inIndex )
{
	AIndex	menuItemArrayIndex = mMenuBarMenuItemArray_MenuItemID.Find(inItemID);
	if( menuItemArrayIndex == kIndex_Invalid )   return;
	AObjectID	dynamicMenuObjectID = mMenuBarMenuItemArray_DynamicMenuID.Get(menuItemArrayIndex);
	if( dynamicMenuObjectID == kObjectID_Invalid )   return;
	GetDynamicMenuByObjectID(dynamicMenuObjectID).SetCheckMark(inIndex);
}
//（ActionTextから設定）
void	AMenuManager::SetCheckMarkToDynamicMenuWithActionText( const AMenuItemID inItemID, const AText& inActionText )
{
	GetDynamicMenuByMenuItemID(inItemID).SetCheckMarkWithActionText(inActionText);
}

//#695
/**
ActionTextから、対応するメニュー項目のインデックスを検索する
*/
AIndex	AMenuManager::FindDynamicMenuItemIndexFromActionText( const AObjectID inDynamicMenuObjectID, const AText& inActionText ) const
{
	return GetDynamicMenuConstByObjectID(inDynamicMenuObjectID).FindMenuItemIndexFromActionText(inActionText);
}

#pragma mark ===========================

#pragma mark ---Fontメニュー

//フォントメニューとして設定
void	AMenuManager::RegisterFontMenu( const AMenuRef inMenuRef /*win , const AControlRef inControlRef*/ )
{
//★	AMenuWrapper::DeleteAllMenuItems(inMenuRef);
//★	AMenuWrapper::InsertFontMenu(inMenuRef,0,0);
	//win （CWindowImpで実行）::SetControl32BitMaximum(inControlRef,AMenuWrapper::GetMenuItemCount(inMenuRef));
}

#pragma mark ===========================

#pragma mark ---コンテクストメニュー

/**
コンテクストメニューを登録する
*/
void	AMenuManager::RegisterContextMenu( AConstCharPtr inMenuName, const AContextMenuID inContextMenuID )
{
	//
	AMenuRef	menuRef = AMenuWrapper::RegisterContextMenu(inMenuName);
	//
	mContextMenuArray_MenuRef.Add(menuRef);
	mContextMenuArray_ID.Add(inContextMenuID);
	mContextMenuArray_ActionText.AddNewObject();
}

//#0
/**
コンテクストメニューを登録する（リソース無し）
*/
void	AMenuManager::RegisterContextMenu( const AContextMenuID inContextMenuID )
{
	//
	AMenuRef	menuRef = AMenuWrapper::CreateMenu();
	//
	mContextMenuArray_MenuRef.Add(menuRef);
	mContextMenuArray_ID.Add(inContextMenuID);
	mContextMenuArray_ActionText.AddNewObject();
}

/*#688 コンテキストメニューはCUserPaneで表示するように変更
//コンテクストメニュー表示
void	AMenuManager::ShowContextMenu( const AContextMenuID inContextMenuID, const AGlobalPoint& inMousePoint, const AWindowRef inWindowRef )//win 080712
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::ShowContextMenu(menuRef,inMousePoint,inWindowRef);//win 080712
}
*/

//#688
/**
コンテクストメニュー取得
*/
AMenuRef	AMenuManager::GetContextMenu( const AContextMenuID inContextMenuID ) const
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return NULL;}
	return mContextMenuArray_MenuRef.Get(index);
}

//#232
/**
*/
AItemCount	AMenuManager::GetContextMenuItemCount( const AContextMenuID inContextMenuID ) const
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return 0;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	return AMenuWrapper::GetMenuItemCount(menuRef);
}

//コンテクストメニューテキスト設定
void	AMenuManager::SetContextMenuItemText( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const AText& inText )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::SetMenuItemText(menuRef,inMenuItemIndex,inText);
}

//#232
/**
コンテクストメニューテキスト取得
*/
void	AMenuManager::GetContextMenuItemText( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, AText& outText ) const
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::GetMenuItemText(menuRef,inMenuItemIndex,outText);
}

//#232
/**
*/
void	AMenuManager::SetContextMenuEnableMenuItem( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const ABool inEnable )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::SetEnableMenuItem(menuRef,inMenuItemIndex,inEnable);
}

/**
動的メニューにつながっているサブメニューを、コンテキストメニュー項目のサブメニューにする
@param outSubMenuArray サブメニューのmenu ref格納array（この関数により１つ追加される）
@param outOldParentMenuArray 元の親menu ref格納array（この関数により１つ追加される）
@param outOldParentItemIndexArray 元の親menu item index（この関数により１つ追加される）
*/
void	AMenuManager::SetContextMenuItemSubMenuFromDynamicMenu( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, 
		const AObjectID inSubDynamicMenuObjectID,
		AArray<AMenuRef>& outSubMenuArray, //#688
		AArray<AMenuRef>& outOldParentMenuArray, AArray<AIndex>& outOldParentItemIndexArray )//#688
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	
	//コンテキストメニューのmenu ref取得
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	//サブメニューmenu ref取得
	AMenuRef	subMenuRef = GetDynamicMenuByObjectID(inSubDynamicMenuObjectID).GetMenuRef();
	//元の親メニューからサブメニューをdetachし、menuref, menu item indexを取得し、記憶する #688
	AMenuRef	oldParentMenu = NULL;
	AIndex	oldParentItemIndex = kIndex_Invalid;
	//元の親メニューから切り離し
	AMenuWrapper::DetachSubMenu(subMenuRef,oldParentMenu,oldParentItemIndex);
	//返り値に記憶
	outSubMenuArray.Add(subMenuRef);
	outOldParentMenuArray.Add(oldParentMenu);
	outOldParentItemIndexArray.Add(oldParentItemIndex);
	//コンテキストメニューにサブメニュー設定
	AMenuWrapper::SetSubMenu(menuRef,inMenuItemIndex,subMenuRef);
}

//#232
/**
ContextMenuにTextArrayを設定
*/
void	AMenuManager::SetContextMenuTextArray( const AContextMenuID inContextMenuID, const ATextArray& inTextArray, const ATextArray& inActionTextArray )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::SetMenuItemsFromTextArray(menuRef,inTextArray);
	mContextMenuArray_ActionText.GetObject(index).SetFromTextArray(inActionTextArray);
}

//#232
/**
ContextMenuにTextArrayMenuを設定
*/
void	AMenuManager::SetContextMenuTextArrayMenu( const AContextMenuID inContextMenuID, const ATextArrayMenuID inTextArrayMenuID )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	
	AApplication::GetApplication().NVI_GetTextArrayMenuManager().RegisterMenu(mContextMenuArray_MenuRef.Get(index),inTextArrayMenuID);
}

//#232
/**
ContextMenuにMenuItemIDを設定
*/
void	AMenuManager::SetContextMenuMenuItemID( const AContextMenuID inContextMenuID, const AMenuItemID inMenuItemID )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	
	//MenuItemIDを全てのメニュー項目に設定→メニュー選択時はEVTDO_DoMenuItemSelected()に対し、指定MenuItemIDとActionTextが伝えられる
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(menuRef); i++ )
	{
		AMenuWrapper::SetMenuItemID(menuRef,i,inMenuItemID);
	}
}

//#232
/**
*/
void	AMenuManager::SetContextMenuCheckMark( const AContextMenuID inContextMenuID, const AText& inText )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	for( AIndex i = 0; i < AMenuWrapper::GetMenuItemCount(menuRef); i++ )
	{
		AText	text;
		AMenuWrapper::GetMenuItemText(menuRef,i,text);
		if( text.Compare(inText) == true )
		{
			AMenuWrapper::SetCheckMark(menuRef,i,true);
		}
		else
		{
			AMenuWrapper::SetCheckMark(menuRef,i,false);
		}
	}
}

//win
/**
メニューアイコン設定
*/
void	AMenuManager::SetContextMenuItemIcon( const AContextMenuID inContextMenuID, const AIndex inMenuItemIndex, const AIconID inIconID )
{
	AIndex	index = mContextMenuArray_ID.Find(inContextMenuID);
	if( index == kIndex_Invalid )   {_ACError("",this); return;}
	AMenuRef	menuRef = mContextMenuArray_MenuRef.Get(index);
	AMenuWrapper::SetMenuItemIcon(menuRef,inMenuItemIndex,inIconID);
}

