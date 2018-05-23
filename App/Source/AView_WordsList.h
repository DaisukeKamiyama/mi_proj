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

AView_WordsList
#723

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AThread_WordsListFinder.h"

#pragma mark ===========================
#pragma mark [クラス]AView_WordsList

/**
ワードリストビュー
*/
class AView_WordsList : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_WordsList( const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_WordsList();
  private:
	void					NVIDO_Init();
	void					NVIDO_DoDeleted();
	AThread_WordsListFinder&	GetWordsListFinderThread();
	
	//<関連オブジェクト取得>
  public:
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu );
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );
	void					ClearHoverCursor();
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					CalcDrawData( const ABool inCalcAll );
	void					GetItemBoxImageRect( const AIndex inIndex, AImageRect& outImageRect ) const;
	AIndex					FindItem( const ALocalPoint inLocalPoint ) const;
	AIndex								mCurrentHoverCursorIndex;
	AIndex								mCurrentSelectionIndex;
	ALocalPoint							mMouseTrackStartLocalPoint;
	AItemCount							mMouseClickCount;
	
	//<インターフェイス>
  public:
	void					SPI_OpenItem( const AIndex inIndex, const AModifierKeys inModifierKeys );
	void					SPI_RequestWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex, const AText& inWord );
	void					SPI_AbortCurrentWordsListFinderRequest();
	void					SPI_DoWordsListFinderPaused();
	void					SPI_DeleteAllData();
	void					SPI_NotifyToWordsListByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	void					SPI_UpdateDrawProperty();
	void					SPI_SelectNextItem();
	void					SPI_SelectPreviousItem();
	void					SPI_AdjustScroll();
	void					SPI_CancelSelect();
	ABool					SPI_GetSearchInProjectFolder() const { return mSearchInProjectFolder; }
  private:
	AText								mWord;
	AFileAcc							mCurrentProjectFolder;
	AIndex								mModeIndex;
	ABool								mSearchInProjectFolder;
	
	//
  public:
	void					SPI_SetLockedMode( const ABool inLocked );
  private:
	ABool								mLockedMode;
	
	//検索実行中フラグ（検索プログレスを表示するために使用する）
  public:
	ABool					SPI_IsFinding() const { return mIsFinding; }
  private:
	ABool								mIsFinding;
	
	//データ
  private:
	ATextArray							mFilePathArray;
	ATextArray							mFunctionNameArray;
	ATextArray							mClassNameArray;
	AArray<AIndex>						mStartArray;
	AArray<AIndex>						mEndArray;
	AArray<AColor>						mHeaderColorArray;
	ATextArray							mContentTextArray;
	AArray<AIndex>						mContentTextStartIndexArray;
	ABoolArray							mClickedArray;
	ABool								mAllFilesSearched;
	
	//最後の要求データを記憶
  private:
	AFileAcc							mLastRequest_ProjectFolder;
	AIndex								mLastRequest_ModeIndex;
	AText								mLastRequest_Word;
	
	//描画データ
  private:
	AArray<ANumber>						mDrawData_Y;
	AArray<ANumber>						mDrawData_HeaderHeight;
	AArray<ANumber>						mDrawData_Height;
	AArray<ANumber>						mDrawData_InfoTextHeight;
	AObjectArray< AArray<AIndex> >		mDrawData_InfoTextLineStart;
	AObjectArray< AArray<AItemCount> >	mDrawData_InfoTextLineLength;
	ABool								mDrawDataAllCalculated;
	
	//スレッド
  private:
	AObjectID							mWordsListFinderThreadID;
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_WordsList;}
};

