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

ADynamicMenu

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]ADynamicMenu
#pragma mark ===========================
//動的メニュー項目を管理するクラス

/*

動的メニューには下記の２種類がある。
1. ルート（コンストラクタのinMenuRefにメニューのMenuRefを設定）
　メニューバーに既に存在するメニューに、項目を追加するもの。例：ウインドウメニュー
　メンバ変数の使われ方：
　　mMenuRef: メニューバーのメニューのMenuRef
　　mIsRealized: メニューバーのメニューに実体化されているかどうか
2. サブメニュー（コンストラクタのinMenuRefにNULLを設定）
　上記のルートメニューのサブメニュー。
　メンバ変数の使われ方：
　　mMenuRef: コンストラクタで生成した独自のMenuRef
　　mIsRealized: 常にtrue（ルートメニューのmIsRealizedとは連動しない）
　　※上位階層のメニュー項目を削除しても、そのサブメニューの実体化は解除されず、オブジェクトも削除されない。

*/

#pragma mark ---コンストラクタ／デストラクタ

/**
コンストラクタ 
*/
ADynamicMenu::ADynamicMenu( AObjectArrayItem* inParent, AMenuManager& inMenuManager, const AMenuItemID inItemID, const AMenuRef inMenuRef ) 
: AObjectArrayItem(inParent), mMenuManager(inMenuManager), mMenuItemID(inItemID), mIsRoot(inMenuRef!=NULL), mMenuRef(inMenuRef)//#857, mIsRealized(false)
{
	//サブメニューの場合、MenuRefを生成
	if( mIsRoot == false )
	{
		mMenuRef = AMenuWrapper::CreateMenu();
		//#857 mIsRealized = true;
	}
}

/**
デストラクタ
*/
ADynamicMenu::~ADynamicMenu()
{
	DeleteAllMenuItems();
	//サブメニューの場合、MenuRefを削除
	if( mIsRoot == false )
	{
		AMenuWrapper::DeleteMenu(mMenuRef);
	}
}

#pragma mark ===========================

#pragma mark ---情報取得／設定

/**
項目数取得
*/
AItemCount	ADynamicMenu::GetMenuItemCount() const
{
	return mMenuItemArray_MenuText.GetItemCount();
}

/**
RealMenuItemIndexからActionTextを取得
*/
void	ADynamicMenu::GetActionTextByRealMenuItemIndex( const AIndex inRealMenuItemIndex, AText& outText ) const
{
	AIndex	index = inRealMenuItemIndex;
	if( mIsRoot == true )
	{
		index = inRealMenuItemIndex - GetMenuManagerConst().GetRealMenuItemIndex(mMenuItemID);
	}
	mMenuItemArray_SelectActionText.Get(index,outText);
}

/**
ActionTextの取得
*/
void	ADynamicMenu::GetActionTextByMenuItemArrayIndex( const AIndex inIndex, AText& outText ) const
{
	mMenuItemArray_SelectActionText.Get(inIndex,outText);
}
/**
ActionTextの設定
*/
void	ADynamicMenu::SetActionTextByMenuItemArrayIndex( const AIndex inIndex, const AText& inText ) 
{
	mMenuItemArray_SelectActionText.Set(inIndex,inText);
}

/**
MenuTextの取得
*/
void	ADynamicMenu::GetMenuTextByMenuItemArrayIndex( const AIndex inIndex, AText& outText ) const
{
	mMenuItemArray_MenuText.Get(inIndex,outText);
}
/**
MenuTextの設定
*/
void	ADynamicMenu::SetMenuTextByMenuItemArrayIndex( const AIndex inIndex, const AText& inText ) 
{
	mMenuItemArray_MenuText.Set(inIndex,inText);
	//#0 高速化 ReRealize();
	if( IsRealized() == true )
	{
		AIndex	realMenuItemIndex = GetRealMenuItemIndex(inIndex);
		AMenuWrapper::SetMenuItemText(mMenuRef,realMenuItemIndex,inText);
	}
}

/**
サブメニューのObjectIDの取得
*/
AObjectID	ADynamicMenu::GetSubMenuObjectIDByMenuItemArrayIndex( const AIndex inIndex ) const
{
	return mMenuItemArray_SubMenuObjectID.Get(inIndex);
}

/**
ショートカット取得
*/
void	ADynamicMenu::GetShortcutByMenuItemArrayIndex( const AIndex inIndex, ANumber& outShortcut, AMenuShortcutModifierKeys& outModifiers ) const
{
	outShortcut = mMenuItemArray_ShortcutKey.Get(inIndex);
	outModifiers = mMenuItemArray_ShortcutModifierKeys.Get(inIndex);
}
/**
ショートカット設定
*/
void	ADynamicMenu::SetShortcutByMenuItemArrayIndex( const AIndex inIndex, const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers )
{
	//ショートカット検索テーブルから削除 win
	RemoveFromShortcutArray(mMenuItemID,mMenuItemArray_SelectActionText.GetTextConst(inIndex));
	//
	mMenuItemArray_ShortcutKey.Set(inIndex,inShortcut);
	mMenuItemArray_ShortcutModifierKeys.Set(inIndex,inModifiers);
	//ショートカット検索テーブルに追加 win
	if( inShortcut != 0 )
	{
		AddToShortcutArray(mMenuItemID,mMenuItemArray_SelectActionText.GetTextConst(inIndex),
				inShortcut,inModifiers);
	}
	//
	ReRealize();
}

//ショートカット検索テーブル（Windowsはメニューに動的にショートカット割り当てできないので） win
AHashNumberArray			ADynamicMenu::sMenuShortcutArray_Key;
AArray<AMenuShortcutModifierKeys>	ADynamicMenu::sMenuShortcutArray_ModifierKeys;
AArray<AMenuItemID>			ADynamicMenu::sMenuShortcutArray_MenuItemID;
ATextArray					ADynamicMenu::sMenuShortcutArray_ActionText;

/**
ショートカット検索（Windows用）
*/
ABool	ADynamicMenu::FindShortcut( const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers,
		AMenuItemID& outMenuItemID, AText& outText )
{
	AArray<AIndex>	indexArray;
	sMenuShortcutArray_Key.FindAll(inShortcut,indexArray);
	for( AIndex i = 0; i < indexArray.GetItemCount(); i++ )
	{
		if( sMenuShortcutArray_ModifierKeys.Get(indexArray.Get(i)) == inModifiers )
		{
			outMenuItemID = sMenuShortcutArray_MenuItemID.Get(indexArray.Get(i));
			outText.SetText(sMenuShortcutArray_ActionText.GetTextConst(indexArray.Get(i)));
			return true;
		}
	}
	return false;
}

/**
ショートカット検索テーブルに追加
*/
void	ADynamicMenu::AddToShortcutArray( const AMenuItemID inMenuItemID, const AText& inActionText,
		const ANumber inShortcut, const AMenuShortcutModifierKeys inModifiers )
{
	sMenuShortcutArray_Key.Add(inShortcut);
	sMenuShortcutArray_ModifierKeys.Add(inModifiers);
	sMenuShortcutArray_MenuItemID.Add(inMenuItemID);
	sMenuShortcutArray_ActionText.Add(inActionText);
}

/**
ショートカット検索テーブルから削除
*/
void	ADynamicMenu::RemoveFromShortcutArray( const AMenuItemID inMenuItemID, const AText& inActionText )
{
	for( AIndex i = 0; i < sMenuShortcutArray_MenuItemID.GetItemCount(); i++ )
	{
		if( sMenuShortcutArray_MenuItemID.Get(i) == inMenuItemID &&
					sMenuShortcutArray_ActionText.GetTextConst(i).Compare(inActionText) == true )
		{
			sMenuShortcutArray_Key.Delete1(i);
			sMenuShortcutArray_ModifierKeys.Delete1(i);
			sMenuShortcutArray_MenuItemID.Delete1(i);
			sMenuShortcutArray_ActionText.Delete1(i);
			return;
		}
	}
}

#pragma mark ===========================

#pragma mark ---メニュー項目追加／削除

/**
メニュー項目追加
*/
void	ADynamicMenu::InsertMenuItem( const AIndex inItemIndex, const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuObjectID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator, const ABool inEnabled )//#129
{
	//内部データへの追加
	mMenuItemArray_MenuText.Insert1(inItemIndex,inMenuText);
	mMenuItemArray_SelectActionText.Insert1(inItemIndex,inActionText);
	mMenuItemArray_SubMenuObjectID.Insert1(inItemIndex,inSubMenuObjectID);
	mMenuItemArray_Enabled.Insert1(inItemIndex,inEnabled);//#129 true->inEnabled
	mMenuItemArray_ShortcutKey.Insert1(inItemIndex,inShortcutKey);
	mMenuItemArray_ShortcutModifierKeys.Insert1(inItemIndex,inShortcutModifierKeys);
	mMenuItemArray_Separator.Insert1(inItemIndex,inSeparator);
	//ショートカット検索テーブルに追加 win
	if( inShortcutKey != 0 )
	{
		AddToShortcutArray(mMenuItemID,mMenuItemArray_SelectActionText.GetTextConst(inItemIndex),
				inShortcutKey,inShortcutModifierKeys);
	}
	
	//実際のメニューに実体化されている場合は、実メニューの更新も行う。
	if( /*#857 mIsRealized*/IsRealized() == true )
	{
		//実際にメニューに挿入する
		RealizeMenuItem(inItemIndex);
	}
}

/**
メニュー項目追加
*/
void	ADynamicMenu::AddMenuItem( const AText& inMenuText, const AText& inActionText, const AObjectID inSubMenuID,
		const ANumber inShortcutKey, const AMenuShortcutModifierKeys inShortcutModifierKeys, const ABool inSeparator, const ABool inEnabled )//#129
{
	InsertMenuItem(GetMenuItemCount(),inMenuText,inActionText,inSubMenuID,inShortcutKey,inShortcutModifierKeys,inSeparator,inEnabled);//#129
}

/**
メニュー項目設定（Arrayから設定）
*/
void	ADynamicMenu::SetMenuItem( const ATextArray& inMenuTextArray, const ATextArray& inActionTextArray )
{
	DeleteAllMenuItems();
	for( AIndex i = 0; i < inMenuTextArray.GetItemCount(); i++ )
	{
		AText	menuText, actionText;
		inMenuTextArray.Get(i,menuText);
		inActionTextArray.Get(i,actionText);
		AddMenuItem(menuText,actionText,kObjectID_Invalid,NULL,NULL,false,true);
	}
}

/**
メニュー項目削除
*/
void	ADynamicMenu::DeleteMenuItem( const AIndex inItemIndex )
{
	//ショートカット検索テーブルから削除 win
	RemoveFromShortcutArray(mMenuItemID,mMenuItemArray_SelectActionText.GetTextConst(inItemIndex));
	//内部データの削除
	mMenuItemArray_MenuText.Delete1(inItemIndex);
	mMenuItemArray_SelectActionText.Delete1(inItemIndex);
	mMenuItemArray_SubMenuObjectID.Delete1(inItemIndex);
	mMenuItemArray_Enabled.Delete1(inItemIndex);
	mMenuItemArray_ShortcutKey.Delete1(inItemIndex);
	mMenuItemArray_ShortcutModifierKeys.Delete1(inItemIndex);
	mMenuItemArray_Separator.Delete1(inItemIndex);
	
	//実際のメニューに実体化されている場合は、実メニューの更新も行う。
	if( /*#857 mIsRealized*/IsRealized() == true )
	{
		//削除
		AMenuWrapper::DeleteMenuItem(mMenuRef,GetRealMenuItemIndex(inItemIndex));
	}
}

//R0173
/**
メニュー項目移動
*/
void	ADynamicMenu::MoveMenuItem( const AIndex inOldIndex, const AIndex inNewIndex )
{
	mMenuItemArray_MenuText.MoveObject(inOldIndex,inNewIndex);
	mMenuItemArray_SelectActionText.MoveObject(inOldIndex,inNewIndex);
	mMenuItemArray_SubMenuObjectID.Move(inOldIndex,inNewIndex);
	mMenuItemArray_Enabled.Move(inOldIndex,inNewIndex);
	mMenuItemArray_ShortcutKey.Move(inOldIndex,inNewIndex);
	mMenuItemArray_ShortcutModifierKeys.Move(inOldIndex,inNewIndex);
	mMenuItemArray_Separator.Move(inOldIndex,inNewIndex);
	//再実体化
	ReRealize();
}

/**
メニュー項目全削除
*/
void	ADynamicMenu::DeleteAllMenuItems()
{
	while( GetMenuItemCount() > 0 )
	{
		DeleteMenuItem(0);
	}
}

/**
メニュー項目全Enable/Disable
*/
void	ADynamicMenu::SetEnableMenu( const ABool inEnable )
{
	/*#695 高速化。GetRealMenuItemIndex()をループ内で実行する必要ない。
	for( AIndex i = 0; i < mMenuItemArray_MenuText.GetItemCount(); i++ )
	{
		AMenuWrapper::SetEnableMenuItem(mMenuRef,GetRealMenuItemIndex(i),inEnable);
	}
	*/
	//項目数取得
	AItemCount	itemCount = mMenuItemArray_Enabled.GetItemCount();
	//項目数0ならリターン
	if( itemCount == 0 )   return;
	//最初の項目についてGetRealMenuItemIndex()を実行（ループ内で各項目にオフセット値として足す）
	AIndex	realMenuItemIndexOffset = GetRealMenuItemIndex(0);
	//各項目についてSetEnableMenuItem()実行
	for( AIndex index = 0; index < itemCount; index++ )
	{
		//★コメントアウト DisableAllMenuBarMenuItems()でmMenuItemArray_Enabledを更新していないので。
		//if( mMenuItemArray_Enabled.Get(index) != inEnable )
		{
			AMenuWrapper::SetEnableMenuItem(mMenuRef,index+realMenuItemIndexOffset,inEnable);
			mMenuItemArray_Enabled.Set(index,inEnable);
		}
	}
}

/**
ADynamicMenuオブジェクト内の項目IndexからRealMenuItemIndexへの変換
*/
AIndex	ADynamicMenu::GetRealMenuItemIndex( const AIndex inMenuItemArrayIndex )
{
	AIndex	realMenuItemIndex = inMenuItemArrayIndex;
	//動的メニューのルートの場合は、該当メニューの場所を探す必要がある。（上に固定メニューや他の動的メニューが存在しうる。）
	if( mIsRoot == true )
	{
		realMenuItemIndex = GetMenuManager().GetRealMenuItemIndex(mMenuItemID) + inMenuItemArrayIndex;
	}
	return realMenuItemIndex;
}

/**
フォントメニューとして設定
*/
void	ADynamicMenu::SetAsFontMenu( const ABool inEnableDefaultFontItem )//#375
{
	if( /*#857 mIsRealized*/IsRealized() == false )   Realize();
	AMenuWrapper::InsertFontMenu(mMenuRef,0,mMenuItemID);
	
	//実メニューから内部配列を逆構成
	for( AIndex index = 0; index < AMenuWrapper::GetMenuItemCount(mMenuRef); index++ )
	{
		AText	text;
		AMenuWrapper::GetMenuItemText(mMenuRef,index,text);
		mMenuItemArray_MenuText.Insert1(index,text);
		/*win FMFontFamily	fontFamily;
		FMFontStyle	style;
		::GetFontFamilyFromMenuSelection(mMenuRef,1+index,&fontFamily,&style);
		text.SetFormattedCstring("%d",fontFamily);*/
		mMenuItemArray_SelectActionText.Insert1(index,text);
		mMenuItemArray_SubMenuObjectID.Insert1(index,kObjectID_Invalid);
		mMenuItemArray_Enabled.Insert1(index,true);
		mMenuItemArray_ShortcutKey.Insert1(index,0);
		mMenuItemArray_ShortcutModifierKeys.Insert1(index,0);
		mMenuItemArray_Separator.Insert1(index,0);
	}
	
	//#375
	if( inEnableDefaultFontItem == true )
	{
		AText	defaultFontTitle;
		defaultFontTitle.SetLocalizedText("DefaultFontTitle");
		AText	defaultFontValue("default");
		InsertMenuItem(0,defaultFontTitle,defaultFontValue,kObjectID_Invalid,0,0,false,true);
	}
}

/**
ActionTextに対応する項目にチェックマークをつける
*/
void	ADynamicMenu::SetCheckMarkWithActionText( const AText& inActionText )
{
	/*#695 高速化。GetRealMenuItemIndex()をループ内で実行する必要ない。
	for( AIndex index = 0; index < mMenuItemArray_SelectActionText.GetItemCount(); index++ )
	{
		if( mMenuItemArray_SelectActionText.GetTextConst(index).Compare(inActionText) == true )
		{
			AMenuWrapper::SetCheckMark(mMenuRef,GetRealMenuItemIndex(index),true);
		}
		else
		{
			AMenuWrapper::SetCheckMark(mMenuRef,GetRealMenuItemIndex(index),false);
		}
	}
	*/
	//項目数取得
	AItemCount	itemCount = mMenuItemArray_SelectActionText.GetItemCount();
	//項目数0ならリターン
	if( itemCount == 0 )   return;
	//最初の項目についてGetRealMenuItemIndex()を実行（ループ内で各項目にオフセット値として足す）
	AIndex	realMenuItemIndexOffset = GetRealMenuItemIndex(0);
	//各項目についてチェックoffにする
	for( AIndex index = 0; index < itemCount; index++ )
	{
		AMenuWrapper::SetCheckMark(mMenuRef,index+realMenuItemIndexOffset,false);
	}
	//inActionTextに対応する項目のチェックonにする
	AIndex	checkItemIndex = mMenuItemArray_SelectActionText.Find(inActionText);
	if( checkItemIndex != kIndex_Invalid )
	{
		AMenuWrapper::SetCheckMark(mMenuRef,checkItemIndex+realMenuItemIndexOffset,true);
	}
}

//B0308
/**
指定Indexのメニュー項目にチェックマークをつける
*/
void	ADynamicMenu::SetCheckMark( const AIndex inIndex )
{
	/*#695 高速化。GetRealMenuItemIndex()をループ内で実行する必要ない。
	AItemCount	itemCount = mMenuItemArray_SelectActionText.GetItemCount();
	for( AIndex index = 0; index < itemCount; index++ )
	{
		if( index == inIndex )
		{
			AMenuWrapper::SetCheckMark(mMenuRef,GetRealMenuItemIndex(index),true);
		}
		else
		{
			AMenuWrapper::SetCheckMark(mMenuRef,GetRealMenuItemIndex(index),false);
		}
	}
	*/
	//項目数取得
	AItemCount	itemCount = mMenuItemArray_SelectActionText.GetItemCount();
	//項目数0ならリターン
	if( itemCount == 0 )   return;
	//最初の項目についてGetRealMenuItemIndex()を実行（ループ内で各項目にオフセット値として足す）
	AIndex	realMenuItemIndexOffset = GetRealMenuItemIndex(0);
	//各項目のチェックoff
	for( AIndex index = 0; index < itemCount; index++ )
	{
		AMenuWrapper::SetCheckMark(mMenuRef,index+realMenuItemIndexOffset,false);
	}
	//指定indexのみチェックon
	if( inIndex != kIndex_Invalid )
	{
		AMenuWrapper::SetCheckMark(mMenuRef,inIndex+realMenuItemIndexOffset,true);
	}
}

//#695
/**
ActionTextから、対応するメニュー項目のインデックスを検索する
*/
AIndex	ADynamicMenu::FindMenuItemIndexFromActionText( const AText& inActionText ) const
{
	return mMenuItemArray_SelectActionText.Find(inActionText);
}

#pragma mark ===========================

#pragma mark ---実体化／非実体化

/**
メニュー実体化
サブメニューを含めて、動的メニューを実際のメニュー上に実体化する。
*/
void	ADynamicMenu::Realize()
{
	if( mIsRoot == true )
	{
		//実体メニュー上に存在する同一MenuItemIDのメニュー項目を削除する
		//（他のDynamicMenuオブジェクトにより追加された項目は、ここで削除される。）
		AIndex	startIndex = GetRealMenuItemIndex(0);
		while( startIndex < AMenuWrapper::GetMenuItemCount(mMenuRef) )//#688 実体メニュー上にstartIndex番目の項目があるかどうかのチェック追加
		{
			//#688 startIndex番目の項目のMenuItemIDが一致しなくなったら終了
			if( AMenuWrapper::GetMenuItemID(mMenuRef,startIndex) != mMenuItemID )
			{
				break;
			}
			//startIndex番目の項目を削除
			AMenuWrapper::DeleteMenuItem(mMenuRef,startIndex);
		}
	}
	else
	{
		//サブメニューはコンストラクタで実体化済み
		_ACError("",this);
		return;
	}
	//メニュー項目の追加
	AItemCount	itemCount = GetMenuItemCount();
	for( AIndex index = 0; index < itemCount; index++ )
	{
		RealizeMenuItem(index);
	}
	//#857 mIsRealized = true;
	SetRealized(true);//#857
}

/**
メニュー実体化解除
*/
void	ADynamicMenu::Unrealize()
{
	if( mIsRoot == true )
	{
		//実体メニュー上に存在する同一MenuItemIDのメニュー項目を削除する
		//（他のDynamicMenuオブジェクトにより追加された項目は、ここで削除される。）
		AIndex	startIndex = GetRealMenuItemIndex(0);
		while( startIndex < AMenuWrapper::GetMenuItemCount(mMenuRef) )//#688 実体メニュー上にstartIndex番目の項目があるかどうかのチェック追加
		{
			//#688 startIndex番目の項目のMenuItemIDが一致しなくなったら終了
			if( AMenuWrapper::GetMenuItemID(mMenuRef,startIndex) != mMenuItemID )
			{
				break;
			}
			//startIndex番目の項目を削除
			AMenuWrapper::DeleteMenuItem(mMenuRef,startIndex);
		}
	}
	else
	{
		//サブメニューはコンストラクタで実体化済み
		_ACError("",this);
		return;
	}
	//#857 mIsRealized = false;
	SetRealized(false);//#857
}

/**
メニュー項目実体化
*/
void	ADynamicMenu::RealizeMenuItem( const AIndex inItemIndex )
{
	AIndex	realMenuItemIndex = GetRealMenuItemIndex(inItemIndex);
	//メニュー項目追加
	if( mMenuItemArray_Separator.Get(inItemIndex) == true )
	{
		AMenuWrapper::InsertMenuItem_Separator(mMenuRef,realMenuItemIndex,mMenuItemID);//#427
	}
	else
	{
		AText	text;//, actiontext;
		mMenuItemArray_MenuText.Get(inItemIndex,text);
		//mMenuItemArray_SelectActionText.Get(inItemIndex,actiontext);//win 080714
		AMenuWrapper::InsertMenuItem(mMenuRef,realMenuItemIndex,text,mMenuItemID,
				mMenuItemArray_ShortcutKey.Get(inItemIndex),mMenuItemArray_ShortcutModifierKeys.Get(inItemIndex));
		//#129
		AMenuWrapper::SetEnableMenuItem(mMenuRef,realMenuItemIndex,mMenuItemArray_Enabled.Get(inItemIndex));
		//サブメニュー有りならサブメニューを設定
		AObjectID	subMenuObjectID = mMenuItemArray_SubMenuObjectID.Get(inItemIndex);
		if( subMenuObjectID != kObjectID_Invalid )
		{
			AMenuWrapper::SetSubMenu(mMenuRef,realMenuItemIndex,GetMenuManager().GetDynamicMenuByObjectID(subMenuObjectID).GetMenuRef());
		}
	}
}

/**
サブメニューを設定する
*/
void	ADynamicMenu::SetSubMenu( const AIndex inItemIndex, const AObjectID inSubMenuObjectID )
{
	mMenuItemArray_SubMenuObjectID.Set(inItemIndex,inSubMenuObjectID);
	AMenuWrapper::SetSubMenu(mMenuRef,GetRealMenuItemIndex(inItemIndex),GetMenuManager().GetDynamicMenuByObjectID(inSubMenuObjectID).GetMenuRef());
}

/**
実体メニュー再構成
*/
void	ADynamicMenu::ReRealize()
{
	//実体メニュー再構成
	if( /*#857 mIsRealized*/IsRealized() == true )
	{
		//実体メニュー上に存在する同一MenuItemIDのメニュー項目を削除する
		AIndex	startIndex = GetRealMenuItemIndex(0);
		while( startIndex < AMenuWrapper::GetMenuItemCount(mMenuRef) )//#688 実体メニュー上にstartIndex番目の項目があるかどうかのチェック追加
		{
			//#688 startIndex番目の項目のMenuItemIDが一致しなくなったら終了
			if( AMenuWrapper::GetMenuItemID(mMenuRef,startIndex) != mMenuItemID )
			{
				break;
			}
			//startIndex番目の項目を削除
			AMenuWrapper::DeleteMenuItem(mMenuRef,startIndex);
		}
		//
		AItemCount	itemCount = GetMenuItemCount();
		for( AIndex index = 0; index < itemCount; index++ )
		{
			RealizeMenuItem(index);
		}
	}
}

//#857
//Realized判定用array
//（1つのMenuItemIDに対し、複数のADynamicMenuオブジェクトが対応し、realizeするときに必ずしも他をunrealizedしないので、
//常に最後にrealizeされた1つがrealizedと判定できるように、staticな変数を用いる）
//各メニューID毎にrealizeされているADynamicMenuオブジェクトのObjectIDを記憶
AArray<AMenuItemID>		ADynamicMenu::sCurrentRealizedMenuArray_MenuItemID;
AArray<AObjectID>		ADynamicMenu::sCurrentRealizedMenuArray_MenuObjectID;

//#857
/**
現在Realizeされているかどうかの判定
*/
ABool	ADynamicMenu::IsRealized() const
{
	//ルートメニュー以外は常にrealized
	if( mIsRoot == false )
	{
		return true;
	}
	//現在realizeされているADynamicMenuオブジェクトを検索し、本オブジェクトと一致していたらtrueを返す
	AIndex	index = GetCurrentRealizedMenuArrayIndex();
	return (sCurrentRealizedMenuArray_MenuObjectID.Get(index)==GetObjectID());
}

//#857
/**
Realizeされたことを設定する
*/
void	ADynamicMenu::SetRealized( const ABool inRealized )
{
	//ルートメニュー以外の場合は何もしない
	if( mIsRoot == false )
	{
		return;
	}
	//realizeされたなら本オブジェクトのObjectID, unrealilzeされたならkObjectID_Invalidを設定する
	AObjectID	objectID = kObjectID_Invalid;
	if( inRealized == true )
	{
		objectID = GetObjectID();
	}
	//現在realizeされているADynamicMenuオブジェクトを検索し、ObjectIDを設定
	AIndex	index = GetCurrentRealizedMenuArrayIndex();
	sCurrentRealizedMenuArray_MenuObjectID.Set(index,objectID);
}

//#857
/**
sCurrentRealizedMenuArray_MenuItemIDからmMenuItemIDに対応する項目を検索
*/
AIndex	ADynamicMenu::GetCurrentRealizedMenuArrayIndex() const
{
	//配列から、mMenuItemIDに対応する項目を検索
	AIndex	index = sCurrentRealizedMenuArray_MenuItemID.Find(mMenuItemID);
	if( index == kIndex_Invalid )
	{
		//mMenuItemIDに対応する項目が無ければ生成する
		index = sCurrentRealizedMenuArray_MenuItemID.Add(mMenuItemID);
		sCurrentRealizedMenuArray_MenuObjectID.Add(kObjectID_Invalid);
	}
	return index;
}

#pragma mark ===========================

#pragma mark ---Enable/Disable更新
//#129

/**
Enable/Disable更新
*/
void	ADynamicMenu::UpdateEnableItems()
{
	if( /*#857 mIsRealized*/IsRealized() == false )   return;
	AItemCount	itemCount = GetMenuItemCount();
	for( AIndex index = 0; index < itemCount; index++ )
	{
		UpdateEnableItem(index);
	}
}

/**
Enable/Disable更新
*/
void	ADynamicMenu::UpdateEnableItem( const AIndex inItemIndex )
{
	//#688
	//バッファモード中はEnable/Disable設定しない（AppDelegateのvalidateMenuItemでEnable/Disable設定される）
	if( AApplication::GetApplication().NVI_GetMenuManager().GetEnableMenuItemBufferredMode() == true )
	{
		return;
	}
	//
	AIndex	realMenuItemIndex = GetRealMenuItemIndex(inItemIndex);
	if( mMenuItemArray_Separator.Get(inItemIndex) == true )
	{
		AMenuWrapper::SetEnableMenuItem(mMenuRef,realMenuItemIndex,false);
	}
	else
	{
		AMenuWrapper::SetEnableMenuItem(mMenuRef,realMenuItemIndex,mMenuItemArray_Enabled.Get(inItemIndex));
	}
}

//#688
/**
Enable/Disableの値取得（AppDelegateのvalidateMenuItemからコールされる。menuはauto validateで動作しているので、その機構に合わせるため）
*/
void	ADynamicMenu::GetDynamicMenuEnableMenuItem( const AIndex inRealMenuItemIndex, ABool& isEnabled ) const
{
	AIndex	index = inRealMenuItemIndex - GetMenuManagerConst().GetRealMenuItemIndex(mMenuItemID);
	if( mMenuItemArray_Separator.Get(index) == true )
	{
		isEnabled = false;
	}
	else
	{
		isEnabled = mMenuItemArray_Enabled.Get(index);
	}
}

