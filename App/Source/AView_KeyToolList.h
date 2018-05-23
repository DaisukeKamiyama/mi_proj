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

AView_KeyToolList
R0073
*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_KeyToolList

class AView_KeyToolList : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_KeyToolList( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_KeyToolList();
  private:
	void					NVIDO_Init();//#92
	void					NVIDO_DoDeleted();//#92
	
	//<関連オブジェクト取得>
  public:
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu );
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, 
							const ALocalPoint inLocalPoint );//win 080706
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );
	
	//<インターフェイス>
  public:
	void					SPI_SetInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
										const ATextArray& inInfoTextArray, const ABoolArray& inIsUserToolArray );
	void					SPI_UpdateDrawProperty();
	void					SPI_DeleteAllData();
  private:
	void					CalcDrawData();
	void					AdjustScroll();
	void					ClearHoverCursor();
	
	ATextArray							mInfoTextArray;
	ABoolArray							mIsUserToolArray;
	
	AIndex								mCurrentHoverCursorIndex;
	
	AWindowID							mCurrentTextWindowID;
	AControlID							mCurrentTextViewControlID;
	ANumber								mLineHeight;
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_KeyToolList;}
};

//#199 AKeyToolListViewFactoryは削除。AViewDefaultFactory<>へ統合。

