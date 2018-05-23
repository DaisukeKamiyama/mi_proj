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

#pragma once

#include "../AbsBase/ABase.h"
#include "AbsFrame.h"

#pragma mark ===========================
#pragma mark [クラス]ATextArrayMenuManager
/**
TextArrayメニュー管理クラス
１つのTextArrayデータに同期して複数のメニュー表示を更新するためのクラス
*/
class ATextArrayMenuManager : public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	ATextArrayMenuManager();
	~ATextArrayMenuManager();
	
	//TextArrayMenu管理
  public:
	void						RegisterTextArrayMenuID( const ATextArrayMenuID inTextArrayMenuID );
	void						UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, const ATextArray& inTextArray );
	void						UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, 
													const ATextArray& inDisplayTextArray, const ATextArray& inActionTextArray );//#914
	
	//対象Control/Menu登録
  public:
	void						RegisterControl( const AWindowID inWindowID, const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID );
	void						UnregisterControl( const AWindowID inWindowID, const AControlID inControlID );
	void						RegisterMenu( const AMenuRef inMenuRef, const ATextArrayMenuID inTextArrayMenuID );
	AIndex						GetItemIndexFromActionText( const AWindowID inWindowID, const AControlID inControlID, 
														   const AText& inActionText ) const;//#914
	void						GetActionTextFromItemIndex( const AWindowID inWindowID, const AControlID inControlID, 
														   const AIndex inItemIndex,
														   AText& outActionText ) const;//#914
	void						GetMenuTextFromItemIndex( const AWindowID inWindowID, const AControlID inControlID, 
														 const AIndex inItemIndex,
														 AText& outMenuText ) const;//#914
	
  private:
	ADataBase								mTextArrayMenuDB;
	ADataBase								mActionTextDB;//#914
	
	AArray<AWindowID>						mControlArray_WindowID;
	AArray<AControlID>						mControlArray_ControlID;
	AArray<ATextArrayMenuID>				mControlArray_TextArrayMenuID;
	
	AArray<AMenuRef>						mMenuArray_MenuRef;
	AArray<ATextArrayMenuID>				mMenuArray_TextArrayMenuID;
	
	//Object情報取得 
  public:
	virtual AConstCharPtr	GetClassName() const { return "ATextArrayMenuManager"; }
	
};



