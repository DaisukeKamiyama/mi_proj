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

ATextArrayMenuManager

*/
//#232

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]ATextArrayMenuManager
#pragma mark ===========================
//

/*
TextArrayMenu管理クラス

動的に変更されるTextArrayについて、TextArrayの変更を関連する全てのControl/Menuに反映させるためのクラス。
TextArrayはRegisterTextArrayMenuID()で登録し、mTextArrayMenuDBに保存される。
*/

#pragma mark ---コンストラクタ／デストラクタ

/**
コンストラクタ
*/
ATextArrayMenuManager::ATextArrayMenuManager() : AObjectArrayItem(NULL)
{
}
/**
デストラクタ
*/
ATextArrayMenuManager::~ATextArrayMenuManager()
{
}

#pragma mark ===========================

#pragma mark ---TextArrayMenu管理

/**
TextArrayMenuIDを登録
*/
void	ATextArrayMenuManager::RegisterTextArrayMenuID( const ATextArrayMenuID inTextArrayMenuID )
{
	mTextArrayMenuDB.CreateData_TextArray(inTextArrayMenuID,"","");
	mActionTextDB.CreateData_TextArray(inTextArrayMenuID,"","");//#914
}

/**
TextArrayMenuIDに対応する全てのメニュー表示を更新
*/
void	ATextArrayMenuManager::UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, const ATextArray& inTextArray )
{
	UpdateTextArrayMenu(inTextArrayMenuID,inTextArray,inTextArray);
}
void	ATextArrayMenuManager::UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, 
												   const ATextArray& inDisplayTextArray, const ATextArray& inActionTextArray )
{
	//DBに反映
	mTextArrayMenuDB.SetData_TextArray(inTextArrayMenuID,inDisplayTextArray);
	mActionTextDB.SetData_TextArray(inTextArrayMenuID,inActionTextArray);//#914
	
	//登録Controlに反映
	for( AIndex i = 0; i < mControlArray_WindowID.GetItemCount(); i++ )
	{
		if( mControlArray_TextArrayMenuID.Get(i) == inTextArrayMenuID )
		{
			AApplication::GetApplication().NVI_GetWindowByID(mControlArray_WindowID.Get(i)).
					NVI_SetMenuItemsFromTextArray(mControlArray_ControlID.Get(i),inDisplayTextArray);
		}
	}
	//登録Menuに反映
	for( AIndex i = 0; i < mMenuArray_MenuRef.GetItemCount(); i++ )
	{
		if( mMenuArray_TextArrayMenuID.Get(i) == inTextArrayMenuID )
		{
			AMenuWrapper::SetMenuItemsFromTextArray(mMenuArray_MenuRef.Get(i),inDisplayTextArray);
		}
	}
}

#pragma mark ===========================

#pragma mark ---対象Control/Menu登録

/**
Controlを登録
*/
void	ATextArrayMenuManager::RegisterControl( const AWindowID inWindowID, const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID )
{
	mControlArray_WindowID.Add(inWindowID);
	mControlArray_ControlID.Add(inControlID);
	mControlArray_TextArrayMenuID.Add(inTextArrayMenuID);
	//Controlに現在のTextArrayの内容を反映
	AApplication::GetApplication().NVI_GetWindowByID(inWindowID).
			NVI_SetMenuItemsFromTextArray(inControlID,mTextArrayMenuDB.GetData_ConstTextArrayRef(inTextArrayMenuID));
}

/**
Controlの登録を解除
*/
void	ATextArrayMenuManager::UnregisterControl( const AWindowID inWindowID, const AControlID inControlID )
{
	for( AIndex index = 0; index < mControlArray_WindowID.GetItemCount(); index++ )
	{
		if( mControlArray_WindowID.Get(index) == inWindowID && mControlArray_ControlID.Get(index) == inControlID )
		{
			mControlArray_WindowID.Delete1(index);
			mControlArray_ControlID.Delete1(index);
			mControlArray_TextArrayMenuID.Delete1(index);
			break;
		}
	}
}

//#914
/**
ActionTextから項目indexを検索
*/
AIndex	ATextArrayMenuManager::GetItemIndexFromActionText( const AWindowID inWindowID, const AControlID inControlID, 
														  const AText& inActionText ) const
{
	for( AIndex index = 0; index < mControlArray_WindowID.GetItemCount(); index++ )
	{
		if( mControlArray_WindowID.Get(index) == inWindowID && mControlArray_ControlID.Get(index) == inControlID )
		{
			return mActionTextDB.Find_TextArray(mControlArray_TextArrayMenuID.Get(index),inActionText);
		}
	}
	return kIndex_Invalid;
}

/**
項目indexのActionTextを取得
*/
void	ATextArrayMenuManager::GetActionTextFromItemIndex( const AWindowID inWindowID, const AControlID inControlID, 
														  const AIndex inItemIndex,
														  AText& outActionText ) const
{
	outActionText.DeleteAll();
	for( AIndex index = 0; index < mControlArray_WindowID.GetItemCount(); index++ )
	{
		if( mControlArray_WindowID.Get(index) == inWindowID && mControlArray_ControlID.Get(index) == inControlID )
		{
			mActionTextDB.GetData_TextArray(mControlArray_TextArrayMenuID.Get(index),inItemIndex,outActionText);
			return;
		}
	}
}

/**
項目indexのMenuTextを取得
*/
void	ATextArrayMenuManager::GetMenuTextFromItemIndex( const AWindowID inWindowID, const AControlID inControlID, 
														  const AIndex inItemIndex,
														  AText& outMenuText ) const
{
	outMenuText.DeleteAll();
	for( AIndex index = 0; index < mControlArray_WindowID.GetItemCount(); index++ )
	{
		if( mControlArray_WindowID.Get(index) == inWindowID && mControlArray_ControlID.Get(index) == inControlID )
		{
			mTextArrayMenuDB.GetData_TextArray(mControlArray_TextArrayMenuID.Get(index),inItemIndex,outMenuText);
			return;
		}
	}
}

/**
Menuを登録
*/
void	ATextArrayMenuManager::RegisterMenu( const AMenuRef inMenuRef, const ATextArrayMenuID inTextArrayMenuID )
{
	mMenuArray_MenuRef.Add(inMenuRef);
	mMenuArray_TextArrayMenuID.Add(inTextArrayMenuID);
	//Menuに現在のTextArrayの内容を反映
	AMenuWrapper::SetMenuItemsFromTextArray(inMenuRef,mTextArrayMenuDB.GetData_ConstTextArrayRef(inTextArrayMenuID));
}


