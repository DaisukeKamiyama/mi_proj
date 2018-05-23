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

AView_IdInfo

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"

#pragma mark ===========================
#pragma mark [クラス]AView_IdInfo

class AView_IdInfo : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_IdInfo( /*#199 AObjectArrayItem* inParent, AWindow& inWindow,*/const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_IdInfo();
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
	void					ClearHoverCursor();
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );//#442
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;//#507
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//#725
	
	//<インターフェイス>
  public:
	void					SPI_OpenItem( const AIndex inIndex, const AModifierKeys inModifierKeys );
	//#853 ABool					SPI_SetIdInfo( const AText& inIdText, const ADocumentID inTextDocumentID );//#238
							/*#238
							const AModeIndex inModeIndex, 
							const ATextArray& inInfoTextArray, const AObjectArray<AFileAcc>& inFileArray, const ATextArray& inCommentTextArray,
							const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray,
							const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
							const ATextArray& inParentKeywordArray );
							*/
	void					SPI_SetIdInfo( const AText& inIdText, const ATextArray& inParentKeyword,
							const ADocumentID inTextDocumentID, 
							const ATextArray& inKeywordTextArray, const ATextArray& inParentKeywordTextArray,
							const ATextArray& inTypeTextArray, 
							const ATextArray& inInfoTextArray, const ATextArray& inCommentTextArray, 
							const AArray<AIndex>& inCategoryIndexArray, const AArray<AIndex>& inColorSlotIndexArray,
							const AArray<AScopeType>& inScopeArray,
							const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
							const ATextArray& inFilePathArray, const AIndex inArgIndex, const AText& inCallerFuncIdText );//#853
	void					SPI_DeleteAllIdInfo();//#725
	ANumber					SPI_GetMaxItemHeight() const;//#853
	void					SPI_NotifyToCallGrafByIdInfoJump( const AText& inCallerFuncIdText, 
							const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
							const AIndex inStartIndex, const AIndex inEndIndex );
	ABool					SPI_HighlightIdInfoArgIndex( const AText& inKeyword, const AIndex inArgIndex );//#853
	void					SPI_UpdateDisplayComment();
	void					SPI_UpdateDrawProperty();
	//#853 const AText&			SPI_GetCurrentIdText() const { return mIdText; }
	void					SPI_SetContextMenuItemID( const AMenuItemID inMenuItemID ) 
	{ mContextMenuItemID = inMenuItemID; }
	void					SPI_SetSelect( const AIndex inIndex );
	AIndex					SPI_GetSelectedIndex() const { return mCurrentSelectionIndex; }
	void					SPI_SelectNext();
	void					SPI_SelectPrev();
	void					SPI_SetDisableAutoHide();
	AItemCount				SPI_GetIdInfoItemCount() const { return mKeywordTextArray.GetItemCount(); }
	void					SPI_DoTickTimer();
  private:
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					OpenItem( const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys );
	AIndex					FindItem( const ALocalPoint inLocalPoint );
	void					AdjustScroll();
	void					CalcDrawData();
	void					GetItemBoxImageRect( const AIndex inIndex, AImageRect& outImageRect ) const;//#643
	ATextArray							mKeywordTextArray;//#853
	ATextArray							mTypeTextArray;
	ATextArray							mInfoTextArray;
	ATextArray							mCommentTextArray;
	ATextArray							mRawCommentTextArray;
	ATextArray							mFilePathArray;
	AArray<AIndex>						mCategoryIndexArray;
	AArray<AIndex>						mColorSlotIndexArray;
	AArray<AScopeType>					mScopeTypeArray;
	AArray<AIndex>						mStartIndexArray;
	AArray<AIndex>						mEndIndexArray;
	ATextArray							mParentKeywordArray;
	//
	AText								mCallerFuncIdText;
	
	AIndex								mArgIndex;
	AIndex								mCurrentModeIndex;
	AText								mIdText;
	AHashTextArray						mRequestedParentKeyword;
	ANumber								mCurrentHoverCursorIndex;
	
	AIndex								mCurrentSelectionIndex;//#725
	
	ALocalPoint							mMouseTrackStartLocalPoint;
	ABool								mMouseDownResultIsDrag;
	
	//#238
  public:
	void								SPI_SetFixDisplay( const ABool inFixDisplay ) { mFixDisplay = inFixDisplay; }
	ABool								SPI_GetFixDisplay() { return mFixDisplay; }
  private:
	ABool								mFixDisplay;
	
	//#238
  public:
	ATextArray							mIdTextHistory;
	ATextArray							mPathHistory;
	
	//自動hide
  private:
	void					ResetHideCounter( const ANumber inCounter );
	ABool								mDisableAutoHide;
	ANumber								mHideCounter;
	AFloatNumber						mAlpha;
	
	/*#725
	//透明度 #291
  public:
	void					SPI_SetTransparency( const AFloatNumber inTransparency ) { mTransparency = inTransparency; }
  private:
	AFloatNumber						mTransparency;
	*/
	//
  private:
	AArray<ANumber>						mDrawData_Y;
	AArray<ANumber>						mDrawData_HeaderHeight;
	AArray<ANumber>						mDrawData_Height;
	AArray<ANumber>						mDrawData_InfoTextHeight;
	AObjectArray< AArray<AIndex> >		mDrawData_InfoTextLineStart;
	AObjectArray< AArray<AItemCount> >	mDrawData_InfoTextLineLength;
	AArray<ANumber>						mDrawData_CommentTextHeight;
	AObjectArray< AArray<AIndex> >		mDrawData_CommentTextLineStart;
	AObjectArray< AArray<AItemCount> >	mDrawData_CommentTextLineLength;
	
	//コンテキストメニュー
  private:
	AMenuItemID							mContextMenuItemID;//#442
	
	//情報取得
  private:
	AViewType				NVIDO_GetViewType() const {return kViewType_IdInfo;}
};

//#199 AIdInfoViewFactoryは削除。AViewDefaultFactory<>へ統合。
