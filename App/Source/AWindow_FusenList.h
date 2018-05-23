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

AWindow_FusenList
#138 AWindow_JumpListをベースとして付箋紙リスト用に新規生成

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

class AView_List;
class AView_List_FusenList;

#pragma mark ===========================
#pragma mark [クラス]AWindow_FusenList
//付箋紙リストウインドウのクラス

class AWindow_FusenList : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_FusenList();
	~AWindow_FusenList();
	
	//<関連ドキュメント取得>
  public:
	AView_List_FusenList&	SPI_GetListView();
	const AView_List_FusenList&	SPI_GetListViewConst() const;
	
	//<イベント処理>
  private:
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	
	//<インターフェイス>
	
	//ウインドウ制御
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
  public:
	void					SPI_SetScrollPosition( const APoint& inPoint );
	void					SPI_GetScrollPosition( APoint& outPoint ) const;
	
	//コントロールの位置・サイズ更新
  private:
	void					UpdateViewBounds( const AIndex inTabIndex );
	
  private:
	AControlID							mListViewControlID;
	AControlID							mVScrollBarControlID;
	
	static const ANumber						kVScrollBarWidth = 12;
	static const ANumber						kDragBoxHeight = 8;
	static const ANumber						kButtonHeight = 0;
	
	//Object Type
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_FusenList; }
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_FusenList"; }
};




