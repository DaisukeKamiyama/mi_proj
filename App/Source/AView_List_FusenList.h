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

AView_List_FusenList

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_List_FusenList

class AView_List_FusenList : public AView_List
{
	//コンストラクタ、デストラクタ
  public:
	AView_List_FusenList( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_List_FusenList();
  private:
	void					NVIDO_ListInit();
	
	//<関連オブジェクト取得>
  public:
  private:
	
	//<イベント処理>
  private:
	void					EVTDO_ListClicked( const AIndex inClickedRowIndex, const AIndex inClickedRowDBIndex, const ANumber inClickCount );
	
	//<インターフェイス>
  public:
	void					SPI_ChangeActiveDocument( const AObjectID inProjectObjectID, const AText& inRelativePath );
	void					SPI_UpdateTable( const AObjectID inProjectObjectID, const AText& inRelativePath );
  private:
	void					UpdateTable();
	
  private:
	AObjectID							mProjectObjectID;
	AText								mRelativePath;
	ATextArray							mModuleNameArray;
	ANumberArray						mOffsetArray;
	
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_List_FusenList;}
};

