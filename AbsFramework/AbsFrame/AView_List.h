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

AView_List

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "AView.h"

//#871
enum ADisplayRowOrderMode
{
	kDisplayRowOrderMode_Normal = 0,
	kDisplayRowOrderMode_Sorted,
	kDisplayRowOrderMode_Filtered,
};

//AbsFrame.hへ移動const AScrapType	kScrapType_ListViewItem = 'LVIt';

class AView_ListFrame;

#pragma mark ===========================
#pragma mark [クラス]AView_List
/**
List表示Viewクラス
*/
class AView_List : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_List( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID );
	virtual ~AView_List();
  private:
	void					NVIDO_Init();//#92
	void					NVIDO_DoDeleted();//#92

  public:	
	void					SPI_SetEnableCursorRow();
	void					SPI_SetEnableFileIcon( const ABool inEnable );
	//#1012 void					SPI_SetEnableIcon( const ABool inEnable );
	void					SPI_SetEnableImageIcon( const ABool inEnable );//#725
	void					SPI_SetEnableDragDrop( const ABool inEnable );//B0319
	ABool					SPI_GetEnableDragDrop() const { return mEnableDragDrop; }
	void					SPI_SetRowMargin( const ANumber inRowMargin );
	void					SPI_SetEnableContextMenu( const ABool inEnable, const AMenuItemID inMenuItemID );//R0186
	//#205 void					SPI_SetFrameControlID( const AControlID inFrameControlID );//win 080618
	void					SPI_SetFrameViewID( const AViewID inViewID );//#205
	void					SPI_CreateFrame(  const ABool inCreateHeader = false, const ABool inCreateVScrollBar = false, 
							const ABool inCreateHScrollBar = false,
							const ANumber inLeftMargin = 1, const ANumber inTopMargin = 1, 
							const ANumber inRightMargin = 1, const ANumber inBottomMargin = 1 );
	AView_ListFrame&		SPI_GetFrameView() {return GetFrameView(); }
	void					SPI_SetControlBindings( const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
							const ABool inWidthFixed, const ABool inHeightFixed );
  private:
	virtual void			NVIDO_ListInit() {}//#138
	//#205 AControlID							mFrameControlID;
	AViewID								mFrameViewID;//#205
	AControlID							mVScrollBarControlID;
	AControlID							mHScrollBarControlID;
	ABool								mEnableDragDrop;
	
	//#871
	//表示  
  public:
	AIndex					SPI_GetDisplayRowIndexFromDBIndex( const AIndex inDBIndex ) const
	{ return mRowOrderDisplayToDBArray.Find(inDBIndex); }
  private:
	AArray<AIndex>						mRowOrderDisplayToDBArray;
	ADisplayRowOrderMode				mDisplayRowOrderMode;
	
	//<関連オブジェクト取得>
	AView_ListFrame&		GetFrameView();//#205
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu );//win
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	ABool					EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );//R0186
	void					EVTDO_DoDraw();
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );//win 080706
	ABool					EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoControlBoundsChanged();
	void					EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
							ABool& outIsCopyOperation );
	void					EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint );//win 080712
	void					XorDragCaret();
	void 					MoveRow( const AIndex inDeleteRowDBIndex, const AIndex inNewRowDBIndex );
	AIndex					CalcDragCaretDisplayRowIndex( const ALocalPoint& inLocalPoint );
	ABool					mDragDrawn;
	AIndex					mDragCaretRowDisplayIndex;
	void					StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );//win
//#688 #if IMPLEMENTATION_FOR_WINDOWS
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
//#688 #endif
	virtual void			EVTDO_ListClicked( const AIndex inClickedRowIndex, const AIndex inClickedRowDBIndex, const ANumber inClickCount ) {}//#199
	ABool					EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;//#328 #644
	ABool					GetOutlineFoldingAndOutlineMarginRect( const AIndex inDisplayRowIndex, ALocalRect& outRect ) const;//#130
	void					UpdateHover( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
										const ABool inUpdateAlways );
	
	//フォーカス win
  private:
	void					EVTDO_DoViewFocusActivated();
	void					EVTDO_DoViewFocusDeactivated();
	
	//<インターフェイス>
	//選択
  public:
	void					SPI_SetSelect( const AIndex inSelect );
	void					SPI_SetSelectByDBIndex( const AIndex inSelectDBIndex, const ABool inAdjustScroll = true );//#698
	
	//テーブル全体
  public:
	AItemCount				SPI_GetRowCount() const { return GetRowCount(); }//win 080618
	AItemCount				SPI_GetDisplayRowCount() const;//#798
	void					NVIDO_SetNumber( const ANumber inNumber );//win
	
	void					SPI_BeginSetTable();
	void					SPI_EndSetTable();
	
	//列データの登録・設定・取得
  public:
	void					SPI_RegisterColumn_Text( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable );//win 080618
	void					SPI_RegisterColumn_Bool( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable );//win 080618
	void					SPI_RegisterColumn_Number( const APrefID inColumnID, const ANumber inWidth, const AConstCharPtr inTitle, const ABool inEditable );//win 080618
	void					SPI_SetColumn_Text( const APrefID inColumnID, const ATextArray& inTextArray );
	void					SPI_SetColumn_Bool( const APrefID inColumnID, const ABoolArray& inBoolArray );//win 080618
	void					SPI_SetColumn_Number( const APrefID inColumnID, const ANumberArray& inNumberArray );//win 080618
	void					SPI_SetColumn_Color( const APrefID inColumnID, const AColorArray& inColorArray );//win 080618
	const ATextArray&		SPI_GetColumn_Text( const APrefID inColumnID ) const;//R0108
	const ANumberArray&	SPI_GetColumn_Number( const APrefID inColumnID ) const;//win 080618
	const ABoolArray&		SPI_GetColumn_Bool( const APrefID inColumnID ) const;//win 080618
	const AColorArray&		SPI_GetColumn_Color( const APrefID inColumnID ) const;//win 080618
	
  public:
	void					SPI_SetTable_File( const ATextArray& inTextArray );
	void					SPI_SetTable_Icon( const AArray<AIconID>& inIconArray );
	void					SPI_SetTable_ImageIconID( const AArray<AImageID>& inImageIconArray );//#725
	void					SPI_SetTable_Enabled( const ABoolArray& inEnabledArray );//#232
	void					SPI_SetTable_TextStyle( const AArray<ATextStyle>& inTextStyleArray );
	void					SPI_SetTable_Color( const AArray<AColor>& inColorArray );//R0006
	void					SPI_SetTable_BackgroundColor( const AArray<AColor>& inColorArray );//#427
	void					SPI_SetTable_RowEditable( const ABoolArray& inEditableArray );//#427
	void					SPI_SetTable_OutlineFoldingLevel( const AArray<AIndex>& inOutlineFoldingLevelArray );//#130
	//#1012 void					SPI_SetColumnIcon( const ADataID inColumnID, const ANumberArray& inIconIDArray );//win 080618
	void					SPI_SetColumnWidth( const ADataID inColumnID, const ANumber inWidth );//win 080618
	ANumber					SPI_GetColumnWidth( const ADataID inColumnID ) const;//win 080618
	
	//win void					SPI_SetTextDrawProperty( const AFont inFont, const AFloatNumber inFontSize, const AColor& inColor );
	void					SPI_SetTextDrawProperty( const AText& inFontName, const AFloatNumber inFontSize, const AColor& inColor );//win
	void					SPI_SetFontSize( const AFloatNumber inFontSize );//win
	AIndex					SPI_GetContextMenuSelectedRowDBIndex() const;//R0186
	void					SPI_SetWheelScrollPercent( const ANumber inNormal, const ANumber inHighSpeed );//R0000
	
	//win 080618 void					SPI_GetTable_Text( const APrefID inColumnID, ATextArray& outTextArray ) const;
	void					SPI_GetTable_File( ATextArray& outTextArray ) const;
	
	//#695 未使用のため削除const AArray<ATextStyle>&	SPI_GetTable_TextStyle() const;//R0108
	
	//行毎Set
  public:
	void					SPI_SetTableRow_Text( const APrefID inColumnID, const AIndex inDBIndex, const AText& inText, const ATextStyle inTextStyle );//R0108
	void					SPI_SetTableRow_OutlineFoldingLevel( const AIndex inDBIndex, const AIndex inOutlineFoldingLevel );//#130
	
	//行毎Insert/Delete
  public:
	void					SPI_InsertTableRows( const AIndex inDBIndex, const AItemCount inCount,
							const ATextStyle inTextStyle, const AColor inColor, 
							const ABool inEnableBackgroundColor, const AColor inBackgroundColor, 
							const ABool inRowEditable );
	void					SPI_DeleteTableRow( const AIndex inDBIndex );
	
	void					SPI_RefreshRow( const AIndex inDisplayRowIndex );
	void					SPI_RefreshRows( const AIndex inDisplayRowStartIndex, const AIndex inDisplayRowEndIndex );
	
	//選択
  public:
	void					SPI_SetSelectByDisplayRowIndex( const AIndex inDisplayRowIndex )
	{ SPI_SetSelect(inDisplayRowIndex); }//#798
	AIndex					SPI_GetSelectedDisplayRowIndex() const
	{ return mSelectedRowDisplayIndex; }//#798
	void					SPI_SetSelectNextDisplayRow( const AModifierKeys inModifers );
	void					SPI_SetSelectPreviousDisplayRow( const AModifierKeys inModifers );
	AIndex					SPI_GetSelectedDBIndex() const 
	{
		if( mSelectedRowDisplayIndex == kIndex_Invalid )   return kIndex_Invalid;
		return mRowOrderDisplayToDBArray.Get(mSelectedRowDisplayIndex); 
	}
	void					SPI_AdjustScroll();
	void					SPI_AdjustScroll( const AIndex inRowDisplayIndex );
	void					SPI_SetMultiSelectEnabled( const ABool inEnable ) { mMultiSelectEnalbed = inEnable; }//#237
	void					SPI_GetSelectedDBIndexArray( AArray<AIndex>& outDBIndexArray ) const;//#237
  private:
	void					ClearCursorRow();//B0000 080810 (SPI_ClearCursorRow()->ClearCursorRow() (private化)
	
	AIndex								mSelectedRowDisplayIndex;
	AIndex								mCursorRowStartDisplayIndex;
	AIndex								mCursorRowEndDisplayIndex;
	AIndex								mCursorRowLeftMargin;
	ABool								mCursorRowHoverFolding;//#130
	ABool								mMultiSelectEnalbed;//#237
	AArray<AIndex>						mSelectedRowDisplayIndexArray;//#237
	
	//
  public:
	ADataID					SPI_GetLastClickedColumnID() const { return mLastClickedColumnID; }//win 080618
	ADataID								mLastClickedColumnID;//win 080618
	
  private:
	
  private:
	void					SetImageSize();
	ANumber					GetRowHeight() const;
	AItemCount				GetRowCount() const;
	void					UpdateScrollBarUnit();
	void					GetRowImageRect( const AIndex inStartRowIndex, const AIndex inEndRowIndex, AImageRect& outRowImageRect ) const;
	void					GetCellImageRect( const APrefID inColumnID, const AIndex inRowIndex, AImageRect& outRowImageRect ) const;//win 080618
	void					NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
													 const ABool inRedraw, const ABool inConstrainToImageSize,
													 const AScrollTrigger inScrollTrigger );//win 080618 #138 #1031
	
  public:
	void					SPI_SetInhibit0Length( const ABool inInhibit0Length );//#205
	
	//表示行制御
  private:
	void					UpdateRowOrderDisplay( const ABool inRedraw = true );
	
	//フィルタ #798
  public:
	void					SPI_SetFilterText( const ADataID inColumnID, const AText& inFilterText );
  private:
	AText								mFilterText;
	ADataID								mFilterColumnID;
	
	//フォールディング #130
  public:
	void					SPI_ExpandCollapse( const AIndex inDisplayRowIndex, const ABool inSelectRow );
	ABool					SPI_IsCollapsed( const AIndex inDisplayRowIndex ) const;
	void					SPI_ExpandAll();
	void					SPI_ExpandCollapseAllForSameLevel( const AIndex inDisplayRowIndex );
	void					SPI_CollapseAllForLevel( const ANumber inLevel );
	void					SPI_ExpandAllForLevel( const ANumber inLevel );
	ANumber					SPI_GetOutlineFoldingLevel( const AIndex inDBRowIndex ) const;
	void					SPI_GetCollapseRowsTextArray( const ADataID inColumnID, ATextArray& outTextArray ) const;
	void					SPI_ApplyCollapseRowsByTextArray( const ADataID inColumnID, const ATextArray& inTextArray );
  private:
	ABool					HasFoldingChild( const AIndex inDBRowIndex ) const;
	
	//ソート
  public:
	void					SPI_Sort( const APrefID inColumnID, const ABool inAscendant );
	void					SPI_SortOff();
  private:
	void					QuickSort( const ADataID inColumnID, AIndex first, AIndex last, const ABool inAscendant );
	ABool								mSortMode;
	APrefID								mSortColumnID;
	ABool								mSortAscendant;
	
	//Edit
  public:
	void					SPI_StartEditMode( const APrefID inColumnID, const AIndex inRowIndex = kIndex_Invalid );
	void					SPI_EndEditMode();
	ABool					SPI_EditBoxInput( const AUChar inChar );
	void					SPI_DoEditBoxDeleted( const AText& inText );
  private:
	ABool								mEditMode;
	APrefID								mEditModeColumnID;
	AIndex								mEditModeRowIndex;
	
	//色設定 #291
  public:
	void					SPI_SetAlwaysActiveColors( const ABool inBool ) { mAlwaysActiveColors = inBool; }
  private:
	ABool								mAlwaysActiveColors;
	
	//背景色 #634
  public:
	void					SPI_SetBackgroundColor( const AColor inColor, const AColor inColorDeactive ) 
	{ mBackgroundColor = inColor; mBackgroundColorDeactivate = inColorDeactive; }
  private:
	AColor								mBackgroundColor;
	AColor								mBackgroundColorDeactivate;
	
	//透明度 #291
  public:
	void					SPI_SetTransparency( const AFloatNumber inTransparency ) { mTransparency = inTransparency; }
  private:
	AFloatNumber						mTransparency;
	
	//ellipsis表示
  public:
	void					SPI_SetEllipsisFirstCharacters( const ABool inFirstCharacters ) { mEllipsisFirstCharacters = inFirstCharacters; }
  private:
	ABool								mEllipsisFirstCharacters;
	
	/*#725
	//背景画像設定 #291
  public:
	void					SPI_SetBackground( const AIndex inBackGroundImageIndex, const AFloatNumber inBackGroundImageAlpha )
	{
		mBackGroundImageIndex = inBackGroundImageIndex;
		mBackGroundImageAlpha = inBackGroundImageAlpha;
	}
  private:
	AIndex								mBackGroundImageIndex;
	AFloatNumber						mBackGroundImageAlpha;
	*/
	//
  private:
	ANumber								mRowHeight;
	ANumber								mAscent;
	
	ADataBase							mDataBase;
	//#1012 ADataBase							mIconDataBase;//win 080618
	ABool								mOutlineLevelDataExist;//#130
	
	AArray<APrefID>						mColumnIDArray;
	AArray<ANumber>						mColumnWidthArray;
	AArray<ABool>						mColumnEditable;
	//win フォント番号→フォント名AFont								mFont;
	AText								mFontName;
	AFloatNumber						mFontSize;
	AColor								mColor;
	ABool								mEnableCursorRow;
	ABool								mEnableFileIcon;
	//#1012 ABool								mEnableIcon;
	ABool								mEnableContextMenu;//R0186
	AMenuItemID							mContextMenuItemID;//R0186
	AIndex								mContextMenuSelectedRowDisplayIndex;//R0186 #892
	ABool								mEnableImageIcon;//#725
	
	//#688 ABool								mMouseTrackByLoop;//win 080728
	AMouseTrackingMode					mMouseTrackingMode;//win 080728
	ABool								mMouseTrackResultIsDrag;//win 080728
	AImagePoint							mMouseTrackStartImagePoint;//win 080728
	
	ANumber								mRowMargin;
	
	/*#695
	AArray<ATextStyle>					mTextStyleArray;
	AArray<AColor>						mColorArray;//R0006
	AArray<AColor>						mBackgroundColorArray;//#427
	ABoolArray							mRowEditableArray;//#427
	*/
	
	
	//R0000
	ANumber								mWheelScrollPercent;
	ANumber								mWheelScrollPercentHighSpeed;
	
	//R0108高速化→やっぱやめ。AWindow_JumpList::SPI_UpdateTableのほうで対応することにする。
	//AImagePoint							mSavedOrigin;
	
	//#205
	ABool								mInhibit0Length;
	
	static const ANumber				kDefaultRowMargin = 2;
	static const ANumber				kRowLeftMargin = 6;
	static const ANumber				kCellLeftMargin = 4;
	static const ANumber				kCellRightMargin = 4;
	
	static const APrefID				kFilePathText = 10000;
	static const APrefID				kIconID = 10001;
	static const APrefID				kEnabled = 10002;//#232
	static const APrefID				kTextStyle = 10003;//#695
	static const APrefID				kColor = 10004;//#695
	static const APrefID				kBackgroundColor = 10005;//#695
	static const APrefID				kBackgroundColorEnabled = 10006;//#695
	static const APrefID				kRowEditable = 10007;//#695
	static const APrefID				kOutlineFoldingLevel = 10008;//#130
	static const APrefID				kOutlineFoldingCollapsed = 10009;//#130
	static const APrefID				kImageIconID = 10010;//#725
	
	static const ANumber				kVScrollBarWidth = 15;//#427 以前は16だったが、MacOSXの場合、端の1pixelにゴミ表示される問題が発生したので15にする
	static const ANumber				kHScrollBarHeight = 15;//#427
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_List; }
};


#pragma mark ===========================
#pragma mark [クラス]AListViewFactory
class AListViewFactory : public AViewFactory
{
  public:
	AListViewFactory( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID )
			: /*#199 mParent(inParent), mWindow(inWindow)*/mWindowID(inWindowID), mControlID(inID)
	{
	}
	AView_List*	Create() 
	{
		return new AView_List(/*#199 mParent,mWindow*/mWindowID,mControlID);
	}
  private:
	//#199 AObjectArrayItem* mParent;
	//#199 AWindow&	mWindow;
	AWindowID							mWindowID;
	AControlID							mControlID;
};

#pragma mark ===========================
#pragma mark [クラス]AView_ListFrame

class AView_ListFrame : public AView
{
	//コンストラクタ、デストラクタ
  public:
	AView_ListFrame( const AWindowID inWindowID, const AControlID inID, const AControlID inListViewControlID,
			const ABool inHeader, const AControlID inVScrollBarControlID, const AControlID inHScrollBarControlID );
	virtual ~AView_ListFrame();
  private:
	void					NVIDO_Init();//win 080707
	void					NVIDO_DoDeleted();//#92
	
	//<イベント処理>
  private:
	ABool					EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) {return false;}
	ABool					EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );
	void					EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) {}//win 080706
	void					EVTDO_DoDraw();
	ACursorType				EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	void					EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys );
	
	//<インターフェイス>
  public:
	void					SPI_RegisterColumn( const ADataID inColumnID, const ANumber inWidth, const AText& inTitle );
	void					SPI_SetColumnWidth( const AIndex inIndex, const ANumber inWidth );
	void					SPI_SetHeaderOffset( const ANumber inOffset );
	void					SPI_SetFocused( const ABool inFocused );
	
	//
  private:
	ABool								mHeaderEnable;
	AControlID							mVScrollBarControlID;
	AControlID							mHScrollBarControlID;
	AControlID							mListViewControlID;
	AArray<ADataID>						mColumnID;
	AArray<ANumber>						mColumnWidth;
	ATextArray							mColumnTitle;
	ANumber								mHeaderOffset;
	ABool								mFocused;
	ALocalPoint							mMouseTrackingPreviousLocalPoint;
	AIndex								mMouseTrackingColumnIndex;
	
	//Object Type
  private:
	AViewType				NVIDO_GetViewType() const { return kViewType_ListFrame; }
};

#pragma mark ===========================
#pragma mark [クラス]AListFrameViewFactory
class AListFrameViewFactory : public AViewFactory
{
  public:
	AListFrameViewFactory( const AWindowID inWindowID, const AControlID inID, const AControlID inListViewControlID,
			const ABool inHeader, const AControlID inVScrollBarControlID, const AControlID inHScrollBarControlID )
	: mWindowID(inWindowID), mControlID(inID), mListViewControlID(inListViewControlID),
		mHeader(inHeader),mVScrollBarControlID(inVScrollBarControlID), mHScrollBarControlID(inHScrollBarControlID)
	{
	}
	AView_ListFrame*	Create() 
	{
		return new AView_ListFrame(mWindowID,mControlID,mListViewControlID,mHeader,mVScrollBarControlID,mHScrollBarControlID);
	}
  private:
	//#199 AObjectArrayItem* mParent;
	//#199 AWindow&	mWindow;
	AWindowID							mWindowID;
	AControlID							mControlID;
	ABool								mHeader;
	AControlID							mListViewControlID;
	AControlID							mVScrollBarControlID;
	AControlID							mHScrollBarControlID;
};

