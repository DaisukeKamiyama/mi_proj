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

AView_CandidateList

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AThread_CandidateFinder.h"

#pragma mark ===========================
#pragma mark [クラス]AView_CandidateList

class AView_CandidateList : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_CandidateList( /* #199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_CandidateList();
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
	void					SPI_SetInfo( const AModeIndex inModeIndex,
							const AWindowID inTextWindowID, const AControlID inTextViewControlID,
							const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
							const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray,
							//#717 const AObjectArray<AFileAcc>& inFileArray )
							const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
							const ATextArray& inParentKeywordArray,
							const AArray<AItemCount>& inMatchedCountArray );
	void					SPI_DeleteAllData();
	void					SPI_SetIndex( const AIndex inIndex );
	AIndex					SPI_GetIndex() const { return mIndex; }
	void					SPI_UpdateDrawProperty();
	void					SPI_InvalidateClick() { mCurrentTextWindowID = kObjectID_Invalid; mCurrentTextViewControlID = kControlID_Invalid; }
	AItemCount				SPI_GetCandidateCount() const { return mInfoTextArray.GetItemCount(); }
	AModeIndex				SPI_GetCurrentModeIndex() const { return mCurrentModeIndex; }
  private:
	void					CalcDrawData();
	void					AdjustScroll();
	void					ClearHoverCursor();
	void					GetItemImageRect( const AIndex inIndex, AImageRect& outImageRect ) const;//#717
	
	ATextArray							mInfoTextArray;
	ATextArray							mKeywordTextArray;
	AArray<AIndex>						mCategoryIndexArray;
	AArray<AScopeType>					mScopeTypeArray;
	//#717 AObjectArray<AFileAcc>				mFileArray;
	ATextArray							mFilePathArray;//#717
	AArray<ACandidatePriority>			mPriorityArray;//#717
	AArray<AItemCount>					mMatchedCountArray;
	ATextArray							mParentKeywordArray;//#717
	
	AIndex								mIndex;
	AModeIndex							mCurrentModeIndex;
	AWindowID							mCurrentTextWindowID;
	AControlID							mCurrentTextViewControlID;
	ABool								mInfoTextIsEmpty;
	
	AIndex								mCurrentHoverCursorIndex;
	
	ANumber								mLineHeight;
	
	//キーワード開始X位置
  public:
	ANumber					SPI_GetKeywordStartX() const { return mKeywordStartX; }
  private:
	ANumber								mKeywordStartX;
	
	//透明度 #291
  public:
	void					SPI_SetTransparency( const AFloatNumber inTransparency ) { mTransparency = inTransparency; }
  private:
	AFloatNumber						mTransparency;
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_CandidateList;}
};

//#199 ACandidateListViewFactoryは削除。AViewDefaultFactory<>へ統合。

