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

AWindow_Text

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATypes.h"
#include "AWindow_JumpList.h"
//#include "AWindow_Info.h"
#include "AWindow_TextSheet.h"
#include "AWindow_AddToolButton.h"
#include "ATextFinder.h"
#include "AWindow_ChangeToolButton.h"//#1344

class ADocument_Text;
class ADocPrefDB;
class AModePrefDB;
class AView_Text;
class AView_LineNumber;
//#725 class AView_InfoHeader;
class AView_TabSelector;
class AView_Ruler;
class AView_Toolbar;
class AWindow_IdInfo;
class AWindow_CandidateList;
class AWindow_KeyToolList;

//#725
/**
レイアウトモード
*/
enum ALayoutMode
{
	kLayoutMode_4Pane_Header11110110 = 0,
	kLayoutMode_4Pane_Header11110111,
	kLayoutMode_4Pane_Header01110111,
	kLayoutMode_4Pane_Header11111111,
	kLayoutMode_3Pane_Header11110110,
	kLayoutMode_3Pane_Header11110111,
	kLayoutMode_3Pane_Header01110111,
	kLayoutMode_3Pane_Header11111111,
};

/**
binding設定タイプ
*/
enum ATextWindowViewBindings
{
	kTextWindowViewBindings_Normal,
	kTextWindowViewBindings_ShowHideLeftSidebar,
	kTextWindowViewBindings_ShowRightSidebar,
	kTextWindowViewBindings_HideRightSidebar,
	kTextWindowViewBindings_ShowSubTextColumn,
	kTextWindowViewBindings_HideSubTextColumn,
	
};

/**
レイアウトデータ
*/
struct ATextWindowLayoutData
{
	//マクロバー
	ANumber				w_MacroBar;
	ANumber				h_MacroBar;
	AWindowPoint		pt_MacroBar;
	//タブ
	ANumber				w_TabSelector;
	ANumber				h_TabSelector;
	AWindowPoint		pt_TabSelector;
	//ルーラー
	ANumber				w_Ruler;
	ANumber				h_Ruler;
	AWindowPoint		pt_Ruler;
	//サブテキストペインカラム
	ANumber				w_SubTextPaneColumn;
	ANumber				h_SubTextPaneColumn;
	AWindowPoint		pt_SubTextPaneColumn;
	ANumber				h_subHScrollbar;
	//メインビュー領域（サブテキストペイン以外のテキストビュー）
	ANumber				w_MainColumn;
	ANumber				h_MainColumn;
	AWindowPoint		pt_MainColumn;
	//メインビュー用VScrollbar領域
	AWindowPoint		pt_VScrollbarArea;
	//メインTextView幅
	ANumber				w_MainTextView;
	//メインHScrollbar高さ
	ANumber				h_MainHScrollbar;
	//メインテキスト表示幅・余白幅
	ANumber				w_MainTextDisplayWidth;
	ANumber				w_MainTextMargin;
	//行番号エリア幅
	ANumber				w_LineNumber;
	//左サイドバー
	ANumber				w_LeftSideBar;
	ANumber				h_LeftSideBar;
	AWindowPoint		pt_LeftSideBar;
	ANumber				h_LeftSideBarWithMargin;
	AWindowPoint		pt_VSeparator_LeftSideBar;
	//右サイドバー
	ANumber				w_RightSideBar;
	ANumber				h_RightSideBar;
	ANumber				h_RightSideBar_WithBottomMargin;
	AWindowPoint		pt_RightSideBar;
	AWindowPoint		pt_VSeparator_RightSideBar;
	//フッター
	ANumber				w_Footer;
	ANumber				h_Footer;
	AWindowPoint		pt_Footer;
};

/**
ポップアップレイアウトパターン
*/
enum ATextWindowPopupLayoutPattern
{
	kTextWindowPopupLayoutPatternA = 1,
	kTextWindowPopupLayoutPatternB = 2,
	kTextWindowPopupLayoutPatternC = 3,
};

/**
ポップアップレイアウトデータ
*/
struct ATextWindowPopupLayoutData
{
	//候補ウインドウ
	ARect				candidateWindowRect;
	//キーワード情報ウインドウ
	ARect				idInfoWindowRect;
};

#pragma mark ===========================
#pragma mark [クラス]ATextWindowTabData
//テキストウインドウのタブごとのデータを保持するクラス
class ATextWindowTabData : public AObjectArrayItem
{
  public:
	ATextWindowTabData( AObjectArrayItem* inParent = NULL ) 
	: AObjectArrayItem(inParent)//#291, mJumpListScrollPosition(kPoint_00)
	//#725 ,mSearchBoxControlID(kControlID_Invalid)//#137
	//#725 ,mSearchButtonControlID(kControlID_Invalid)//#137
    //,mLastFocusedTextViewControlID(kControlID_Invalid)//#137
	,mToolbarControlID(kControlID_Invalid)//#246
	,mRulerControlID(kControlID_Invalid)//#246
	,mSubTextControlID(kControlID_Invalid), mSubTextLineNumberControlID(kControlID_Invalid)//#212
	,mSubTextHScrollBarControlID(kControlID_Invalid)//#725
	,mHScrollBarControlID(kControlID_Invalid)//#725
	//#725 ,mJumpListVisible(false)//#291
	,mAmazonButtonControlID(kControlID_Invalid)//amazon
	//,mRulerWindowID(kObjectID_Invalid)//#725
	,mLatentMainTextViewControlID(kControlID_Invalid)
	,mLatentTextViewControlID(kControlID_Invalid)
	{}
	~ATextWindowTabData() {}
	
	void					InsertViewData( const AIndex inViewIndex, 
							const AControlID inTextViewControlID, const AControlID inLineNumberViewControlID, const AControlID inVScrollBarControlID,
							const AControlID inSplitButtonControlID, const AControlID inSeparatorControlID, const AControlID inConcatButtonControlID,
							const AWindowID inViewCloseButtonWindowID,//win
							const ANumber inViewHeight )
	{
		mTextViewControlIDArray.Insert1(inViewIndex,inTextViewControlID);
		mLineNumberViewControlIDArray.Insert1(inViewIndex,inLineNumberViewControlID);
		mVScrollBarControlIDArray.Insert1(inViewIndex,inVScrollBarControlID);
		mSplitButtonControlIDArray.Insert1(inViewIndex,inSplitButtonControlID);
		mSeparatorControlIDArray.Insert1(inViewIndex,inSeparatorControlID);
		mConcatButtonControlIDArray.Insert1(inViewIndex,inConcatButtonControlID);
		mCloseViewButtonWindowIDArray.Insert1(inViewIndex,inViewCloseButtonWindowID);//win
		mViewHeightArray.Insert1(inViewIndex,inViewHeight);
	}
	/*
	void					AddViewData( const AControlID inTextViewControlID, 
							const AControlID inLineNumberViewControlID, const AControlID inVScrollBarControlID,
							const AControlID inSplitButtonID, const AControlID inSeparatorControlID, const ANumber inViewHeight )
	{
		InsertViewData(mTextViewControlIDArray.GetItemCount(),inTextViewControlID,inLineNumberViewControlID,inVScrollBarControlID,
				inSplitButtonID,inSeparatorControlID,inSeparatorControlID,inViewHeight);
	}
	*/
	void					DeleteViewData( const AIndex inViewIndex )
	{
		//#135 mLastFocusedTextViewControlIDに設定したTextViewを削除するときは、mLastFocusedTextViewControlIDをクリアする
		if( mTextViewControlIDArray.Get(inViewIndex) == mLatentMainTextViewControlID )
		{
			mLatentMainTextViewControlID = kControlID_Invalid;
		}
		if( mTextViewControlIDArray.Get(inViewIndex) == mLatentTextViewControlID )
		{
			mLatentTextViewControlID = kControlID_Invalid;
		}
		//データ削除
		mTextViewControlIDArray.Delete1(inViewIndex);
		mLineNumberViewControlIDArray.Delete1(inViewIndex);
		mVScrollBarControlIDArray.Delete1(inViewIndex);
		mSplitButtonControlIDArray.Delete1(inViewIndex);
		mSeparatorControlIDArray.Delete1(inViewIndex);
		mConcatButtonControlIDArray.Delete1(inViewIndex);
		mCloseViewButtonWindowIDArray.Delete1(inViewIndex);//win
		mViewHeightArray.Delete1(inViewIndex);
	}
	
	AItemCount				GetViewCount() const { return mTextViewControlIDArray.GetItemCount(); }
	AControlID				GetTextViewControlID( const AIndex inViewIndex ) const { return mTextViewControlIDArray.Get(inViewIndex); }
	AControlID				GetLineNumberViewControlID( const AIndex inViewIndex ) const { return mLineNumberViewControlIDArray.Get(inViewIndex); }
	AControlID				GetVScrollBarControlID( const AIndex inViewIndex ) const { return mVScrollBarControlIDArray.Get(inViewIndex); }
	AControlID				GetSplitButtonControlID( const AIndex inViewIndex ) const { return mSplitButtonControlIDArray.Get(inViewIndex); }
	AControlID				GetSeparatorControlID( const AIndex inViewIndex ) const { return mSeparatorControlIDArray.Get(inViewIndex); }
	AControlID				GetConcatButtonControlID( const AIndex inViewIndex ) const { return mConcatButtonControlIDArray.Get(inViewIndex); }
	AWindowID				GetCloseViewButtonWindowID( const AIndex inViewIndex ) const { return mCloseViewButtonWindowIDArray.Get(inViewIndex); }//win
	ANumber					GetViewHeight( const AIndex inViewIndex ) const { return mViewHeightArray.Get(inViewIndex); }
	void					SetViewHeight( const AIndex inViewIndex, const ANumber inViewHeight ) { mViewHeightArray.Set(inViewIndex,inViewHeight); }
	//#291 void					SetJumpListScrollPosition( const APoint& inScrollPosition ) { mJumpListScrollPosition = inScrollPosition;}
	//#291 void					GetJumpListScrollPosition( APoint& outScrollPosition ) { outScrollPosition = mJumpListScrollPosition; }
	
	AIndex					FindFromTextViewControlID( const AControlID inControlID ) const { return mTextViewControlIDArray.Find(inControlID); }
	AIndex					FindFromLineNumberViewControlID( const AControlID inControlID ) const { return mLineNumberViewControlIDArray.Find(inControlID); }
	AIndex					FindFromVScrollBarControlID( const AControlID inControlID ) const { return mVScrollBarControlIDArray.Find(inControlID); }
	AIndex					FindFromSplitButtonControlID( const AControlID inControlID ) const { return mSplitButtonControlIDArray.Find(inControlID); }
	AIndex					FindFromConcatButtonControlID( const AControlID inControlID ) const { return mConcatButtonControlIDArray.Find(inControlID); }
	AIndex					FindFromSeparatorControlID( const AControlID inControlID ) const { return mSeparatorControlIDArray.Find(inControlID); }
  private:
	AArray<AControlID>					mTextViewControlIDArray;
	AArray<AControlID>					mLineNumberViewControlIDArray;
	AArray<AControlID>					mVScrollBarControlIDArray;
	AArray<AControlID>					mSplitButtonControlIDArray;
	AArray<AControlID>					mSeparatorControlIDArray;
	AArray<AControlID>					mConcatButtonControlIDArray;
	AArray<AWindowID>					mCloseViewButtonWindowIDArray;//win
	ANumberArray						mViewHeightArray;
	//#291 APoint								mJumpListScrollPosition;
	
	//最後にフォーカスされたメインカラムのtext view
  public:
	void					SetLatentMainTextViewControlID( const AControlID inControlID ) { mLatentMainTextViewControlID = inControlID; }
	AControlID				GetLatentMainTextViewControlID() const { return mLatentMainTextViewControlID; }
	AControlID							mLatentMainTextViewControlID;
	
	//最後にフォーカスされたtext view
  public:
	void					SetLatentTextViewControlID( const AControlID inControlID ) { mLatentTextViewControlID = inControlID; }
	AControlID				GetLatentTextViewControlID() const { return mLatentTextViewControlID; }
	AControlID							mLatentTextViewControlID;
	
	//SubPane #212
  public:
	void					SetSubTextControlID( const AControlID inTextControlID, const AControlID inLineNumberControlID,
							const AControlID inHScrollbarControlID ) 
	{
		mSubTextControlID = inTextControlID;
		mSubTextLineNumberControlID = inLineNumberControlID;
		mSubTextHScrollBarControlID = inHScrollbarControlID;//#725
	}
	AControlID				GetSubTextControlID() const { return mSubTextControlID; }
	AControlID				GetSubTextLineNumberControlID() const { return mSubTextLineNumberControlID; }
	AControlID				GetSubTextHScrollbarControlID() const { return mSubTextHScrollBarControlID; }//#725
  private:
	AControlID							mSubTextControlID;
	AControlID							mSubTextLineNumberControlID;
	AControlID							mSubTextHScrollBarControlID;//#725
	
    //ツールバー #246
  public:
	void					SetToolbarControlID( const AControlID inControlID ) { mToolbarControlID = inControlID; }
	AControlID				GetToolbarControlID() const { return mToolbarControlID; }
  private:
	AControlID							mToolbarControlID;
	
	//ルーラー #246
	///*#725
  public:
	void					SetRulerControlID( const AControlID inControlID ) { mRulerControlID = inControlID; }
	AControlID				GetRulerControlID() const { return mRulerControlID; }
  private:
	AControlID							mRulerControlID;
	//*/
	/*
  public:
	void					SetRulerWindowID( const AWindowID inWindowID ) { mRulerWindowID = inWindowID; }
	AWindowID				GetRulerWindowID() const { return mRulerWindowID; }
  private:
	AWindowID							mRulerWindowID;
	*/
	
	//HScrollbar #725
  public:
	void					SetHScrollbarControlID( const AControlID inControlID ) { mHScrollBarControlID = inControlID; }
	AControlID				GetHScrollbarControlID() const { return mHScrollBarControlID; }
  private:
	AControlID							mHScrollBarControlID;
	
	//ツールボタン
  public:
	void					AddToolButton( const AControlID inID, const AToolbarItemType inType, const AFileAcc& inFile, const AIndex inDBIndex ) 
	{
		mToolButtonArray_ControlID.Add(inID);
		mToolButtonArray_Type.Add(inType);
		mToolButtonArray_File.GetObject(mToolButtonArray_File.AddNewObject()).CopyFrom(inFile);
		mToolButtonArray_DBIndex.Add(inDBIndex);
	}
	void					DeleteAllToolButtonData() 
	{
		mToolButtonArray_ControlID.DeleteAll();
		mToolButtonArray_Type.DeleteAll();
		mToolButtonArray_File.DeleteAll();
		mToolButtonArray_DBIndex.DeleteAll();
	}
	AControlID				GetToolButtonControlID( const AIndex inIndex ) const { return mToolButtonArray_ControlID.Get(inIndex); }
	AToolbarItemType		GetToolButtonType( const AIndex inIndex ) const { return mToolButtonArray_Type.Get(inIndex); }
	const AFileAcc&			GetToolButtonFile( const AIndex inIndex ) const { return mToolButtonArray_File.GetObjectConst(inIndex); }
	AItemCount				GetToolButtonCount() const { return mToolButtonArray_ControlID.GetItemCount(); }
	AIndex					FindToolButtonIndexByID( const AControlID inID ) const { return mToolButtonArray_ControlID.Find(inID); }
	AIndex					GetToolButtonDBIndex( const AIndex inIndex ) const { return mToolButtonArray_DBIndex.Get(inIndex); }
	void					SetToolButtonModeIndex( const AModeIndex inModeIndex ) { mToolButtonModeIndex = inModeIndex; }
	AModeIndex				GetToolButtonModeIndex() const { return mToolButtonModeIndex; }
	AArray<ALocalPoint>&	GetToolButtonPositionArray() { return mToolButtonPositionArray; }
  private:
	AArray<AControlID>					mToolButtonArray_ControlID;
	AArray<AToolbarItemType>			mToolButtonArray_Type;
	AObjectArray<AFileAcc>				mToolButtonArray_File;
	AArray<AIndex>						mToolButtonArray_DBIndex;
	AArray<ALocalPoint>					mToolButtonPositionArray;
	AModeIndex							mToolButtonModeIndex;
	
	/*#725
	//検索ボックス #137
  public:
	void					SetSearchBoxControlID( const AControlID inControlID ) { mSearchBoxControlID = inControlID; }
	AControlID				GetSearchBoxControlID() const { return mSearchBoxControlID; }
	void					SetSearchButtonControlID( const AControlID inControlID ) { mSearchButtonControlID = inControlID; }
	AControlID				GetSearchButtonControlID() const { return mSearchButtonControlID; }
  private:
	AControlID							mSearchBoxControlID;
	AControlID							mSearchButtonControlID;
	*/
	//amazon
  public:
	void					SetAmazonButtonControlID( const AControlID inControlID ) { mAmazonButtonControlID = inControlID; }
	AControlID				GetAmazonButtonControlID() const { return mAmazonButtonControlID; }
  private:
	AControlID							mAmazonButtonControlID;
	
	/*#725
	//ジャンプリスト
  public:
	ABool					GetJumpListVisible() const { return mJumpListVisible; }
	void					SetJumpListVisible( const ABool inVisible ) { mJumpListVisible = inVisible; }
  private:
	ABool								mJumpListVisible;
	*/
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ATextWindowTabData"; }
	
};

#pragma mark ===========================
#pragma mark [クラス]AWindow_Text
//テキストウインドウのクラス
class AWindow_Text : public AWindow
{
	//コンストラクタ／デストラクタ
  public:
	AWindow_Text(/*#175  AObjectArrayItem* inParent */);
	~AWindow_Text();
  private:
	void	NVIDO_DoDeleted();//#92
	ABool					mLuaConsoleMode;//#567
	
	//#725
  public:
	void					SPI_SetInitialSideBarDisplay( const ABool inLeftSideBar, const ABool inRightSideBar, const ABool inSubText )
	{ mLeftSideBarDisplayed = inLeftSideBar; mRightSideBarDisplayed = inRightSideBar; mSubTextColumnDisplayed = inSubText; }
	
	//<関連オブジェクト取得>
  public:
	ADocument_Text&			SPI_GetCurrentTabTextDocument();
	const ADocument_Text&	SPI_GetCurrentTabTextDocumentConst() const;
	ADocument_Text&			SPI_GetTextDocument( const AControlID inTextViewControlID );//#212
	const ADocument_Text&	SPI_GetTextDocumentConst( const AControlID inTextViewControlID ) const;//#737
	ADocument_Text&			SPI_GetCurrentFocusTextDocument();//#212
	AView_LineNumber&		SPI_GetCurrentLineNumberView( const AIndex inViewIndex );//#379
	const AView_LineNumber&	SPI_GetLineNumberViewConst( const AIndex inTabIndex, const AIndex inViewIndex ) const;//#450
	AItemCount				SPI_GetCurrentViewCount() const;//#379
	AView_Text&				SPI_GetCurrentFocusTextView();//#853
	AView_Text& 			SPI_GetMainTextViewForDocumentID( const ADocumentID inDocumentID );
  private:
	AModePrefDB&			GetCurrentTabModePrefDB();
	AView_Text&				GetTextViewByControlID( const AIndex inViewIndex );
	const AView_Text&		GetTextViewConstByControlID( const AIndex inViewIndex ) const;
	AView_LineNumber&		GetLineNumberViewByControlID( const AControlID inControlID );
	const AView_LineNumber&	GetLineNumberViewConstByControlID( const AControlID inControlID ) const;
	//#725 AView_InfoHeader&		GetInfoHeader();
	//#725 const AView_InfoHeader&	GetInfoHeaderConst() const;//#199
	AView_TabSelector&		GetTabSelector();
	const AView_TabSelector&	GetTabSelectorConst() const;
	
	//<イベント処理>
  public:
	ABool					SPI_GetAskingSaveChanges() const { return mAskingSaveChanges; }
	void					SPI_UpdateWindowAlpha();//B0333 #1255
	//#725 void					SPI_HideFloatingWindow();//B0442
	void					NVIDO_TryClose( const AIndex inTabIndex ) { TryClose(inTabIndex); }//#164
	void					SPI_ShowMoveLineWindow();//#594
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	ABool					EVTDO_ValueChanged( const AControlID inID );
	void					EVTDO_WindowActivated();
	void					EVTDO_WindowDeactivated();
	void					EVTDO_WindowFocusActivated();//#688
	void					EVTDO_WindowFocusDeactivated();//#688
	void					EVTDO_WindowCollapsed();
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	ABool					EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	void					EVTDO_UpdateMenu();
	void					EVTDO_DoCloseButton();
	void					EVTDO_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChangesResult inAskSaveChangesReply );
	void					EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText );
	void					EVTDO_WindowResizeCompleted( const ABool inResized );
	void					EVTDO_DoWindowBeforeSelected();//#240
	void					TryClose( const AIndex inTabIndex );//win 080709
	void					ExecuteClose( const AObjectID inClosingDocumentID );
	void					SaveFTPAlias();
	ABool					IsCrossCaretMode() const;
	ABool								mAskingSaveChanges;
#if IMPLEMENTATION_FOR_MACOSX
	ABool					EVTDO_DoServiceGetTypes( AArray<AScrapType>& outCopyTypes, AArray<AScrapType>& outPasteTypes );//B0370
	ABool					EVTDO_DoServiceCopy( const AScrapRef inScrapRef );//B0370
	ABool					EVTDO_DoServicePaste( const AScrapRef inScrapRef );//B0370
#endif
	ABool					NVIDO_ViewReturnKeyPressed( const AControlID inControlID );//#137 #135
	ABool					NVIDO_ViewTabKeyPressed( const AControlID inControlID );//#137 #135
	ABool					NVIDO_ViewEscapeKeyPressed( const AControlID inControlID );//#137 #135
	void					FindNextWithSearchBox();//#137
	void					FindPreviousWithSearchBox();//#268
	ANumber					NVIDO_SeparatorMoved( const AWindowID inSeparatorWindowID, const AControlID inID, 
							const ANumber inDelta, const ABool inCompleted );//#212 #409
	void					NVIDO_SeparatorDoubleClicked( const AWindowID inTargetWindowID, const AControlID inID );//#291
	ABool					EVTDO_DoGetHelpTag( const AControlID inID, const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) ;//#602
  public:
	void					SPI_TabToWindow( const AIndex inTabIndex, 
											const ABool inWindowPositionDirected = false, const APoint inPoint = kPoint_00 );//#389 #859
	void					SPI_WindowToTab();//#389
	void					SPI_DetachTab_Left( const AIndex tabIndex );//#389
	void					SPI_DetachTab_Right( const AIndex tabIndex );//#389
	void					SPI_DetachTab( const AIndex tabIndex );//#389
	//#524 void					SPI_Print();//win
	//#725 void					SPI_IndexWindowNext();//#438
	//#725 void					SPI_IndexWindowPrev();//#438
	void					EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile );//#112
	
	//<インターフェイス>
	
	//ウインドウ制御
  public:
	void					SPI_DoMacroButton( const AIndex inToolButtonIndex );//#724
	void					SPI_UpdateViewBounds( const ADocumentID inDocumentID );//#379
	void					SPI_UpdateViewBounds();
	//B0402 void					SPI_UpdateViewBoundsAll( const AModeIndex inModeIndex );
	ABool					SPI_IsDocumentVisible( const ADocumentID inDocumentID ) const;
	//#476 void					SPI_SwitchFullScreenMode();//#404
	//#675
	void					SPI_IncrementSuppressUpdateViewBoundsCounter()
	{ mSuppressUpdateViewBoundsCounter++; }
	void					SPI_DecrementSuppressUpdateViewBoundsCounter()
	{ mSuppressUpdateViewBoundsCounter--; }
	void					SPI_RefreshWindowAll();//#675
	void					SPI_SetWindowBoundsByMainColumnPosition( const ARect& inRect );//#850
	void					SPI_GetWindowMainColumnBounds( ARect& outRect ) const;//#850
	ABool					SPI_IsStatusBarDisplayed() const;
  private:
	void					UpdateViewBounds( const AIndex inTabIndex, const AUpdateViewBoundsTriggerType inTriggerType = kUpdateViewBoundsTriggerType_Other );//#645
	void					UpdateViewBounds( const AIndex inTabIndex, const ARect& windowBounds, 
							const AUpdateViewBoundsTriggerType inTriggerType = kUpdateViewBoundsTriggerType_Other );//#341 #645
	ABool					Is4PaneMode() const;//#725
	void					CalculateLayout( const AIndex inTabIndex, ATextWindowLayoutData& outLayout ) ;//#725
	void					UpdateLayout_TextView( const AIndex inTabIndex, 
												   const AWindowPoint pt_MainView, const ANumber w_MainView, const ANumber h_MainView,
												   const ANumber w_MainTextMargin, //#1186
												   const ANumber w_TextView, const ANumber w_LineNumber, AWindowPoint pt_VScrollbarArea );//#725
	ANumber					GetLineNumberWidth( const AIndex inTabIndex ) const;//#450
	ABool					UpdateLayout_SubTextColumn( const AIndex inTabIndex, const ATextWindowLayoutData& layout );
	void					UpdateLayout_SubTextPane( const AIndex inTabIndex, const AWindowPoint pt_SubTextPane, 
													 const ANumber w_SubTextPane, const ANumber h_SubTextPane, const ANumber h_SubHScrollbar );//#725
	void					UpdateLayout_AdjustLeftSideBarHeight( const ANumber inLeftSideBarHeight,
							const AIndex inAdjustStartIndex = 0 );//#725
	void					UpdateLayout_AdjustRightSideBarHeight( const ANumber inRightSideBarHeight,
							const AIndex inAdjustStartIndex = 0 );//#725
	void					UpdateLayout_RightSideBar( const AIndex inTabIndex, const ATextWindowLayoutData& inLayout,
							const ABool inAnimate = false );//#725
	ABool					UpdateLayout_LeftSideBar( const AIndex inTabIndex, const ATextWindowLayoutData& inLayout );//#725
	void					UpdateLayout_LineNumberAndPositionButton( const AIndex inTabIndex, const ATextWindowLayoutData& layout );//#725
	void					UpdateVisibleSubWindows();//#725
	void					UpdateViewBindings( const ATextWindowViewBindings inBindings );//#725
	void					NVIDO_Create( const ADocumentID inDocumentID );
	void					NVIDO_Show();
	void					NVIDO_Hide();
	void					NVIDO_UpdateProperty();
	void					UpdateVSeparatorHelpTag();//#602
	AItemCount							mSuppressUpdateViewBoundsCounter;//#675
	AHashArray<AControlID>				mToolbarExistArray;
	
	//タブ制御
  public:
	void					SPNVI_CreateTextDocumentTab( const ADocumentID inTextDocumentID, 
							const ABool inSelect = true, const ABool inSelectableTab = true );//#212 #379
	void					SPI_CloseTab( const AIndex inTabIndex );//win 080709
	void					SPI_TryCloseDocument( const ADocumentID inDocumentID );
	void					SPI_ExecuteCloseDocument( const ADocumentID inDocumentID );
	void					SPI_SwitchTabNext();//#357
	void					SPI_SwitchTabPrev();//#357
	AIndex					SPI_GetDisplayTabIndex( const AIndex inTabIndex ) const;//#513
	ABool					NVIDO_GetNextDisplayTab( AIndex& ioIndex, const ABool inLoop ) const;//#559
	ABool					NVIDO_GetPrevDisplayTab( AIndex& ioIndex, const ABool inLoop ) const;//#559
	AIndex					SPI_GetDisplayTabIndexByTabIndex( const AIndex inTabIndex ) const;//#850
	void					SPI_SetDisplayTabIndexByDocumentID( const ADocumentID inDocumentID, const AIndex inTabDisplayIndex );//#850
	void					SPI_SaveTabSet();//#1050
	void					SortTabs();//#1062
  private:
	void					NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID );
	void					NVIDO_DeleteTab( const AIndex inTabIndex );
	void					NVIDO_TabDeleted();//#663
	void					NVIDO_SelectTab( const AIndex inTabIndex );//#725
	void					NVMDO_DoTabActivating( const AIndex inTabIndex );//win
	void					NVMDO_DoTabActivated( const AIndex inTabIndex, const ABool inTabSwitched );//win 080712
	void					NVMDO_DoTabDeactivated( const AIndex inTabIndex );
	AObjectArray<ATextWindowTabData>	mTabDataArray;
	
	//タブ切り離し・結合
  public:
	void					SPI_CopyTabToWindow( const AIndex inTabIndex, const AWindowID inNewWindowID );
	void					SPI_CopyViewDataTo( const AIndex inSrcTabIndex, const AWindowID inDstWindowID, const AIndex inDstTabIndex );
	void					SPNVI_CopyCreateTab( const AWindowID inSrcWindowID, const AIndex inSrcTabIndex );
	//#850 void					SPI_SetTabModeMainWindow( const ABool inTabModeMainWindow );//#212 { mTabModeMainWindow = inTabModeMainWindow; }
	//#850 ABool					SPI_IsTabModeMainWindow() const { return mTabModeMainWindow; }
	private:
	void					CopyViewData( const AIndex inTabIndex, const AArray<ANumber>& inViewHeightArray, 
		const AArray<ATextPoint>& inViewCaretTextPointArray, const AArray<ATextPoint>& inViewSelectTextPointArray,
		const AArray<AImagePoint>& inViewOriginOfLocalFrameArray );
	//#850 ABool								mTabModeMainWindow;
	
	//ビュー制御
  public:
	//#864 AIndex					SPI_GetViewIndexByControlID( const AControlID inControlID, AIndex& outTabIndex ) const;//#864
	ANumber					SPI_GetViewHeight( const AIndex inViewIndex ) const;
	void					SPI_SetViewHeight( const AIndex inViewIndex, const ANumber inHeight );
	ANumber					SPI_GetTextViewAreaHeight() const;//#92
	/*#199void					SPI_UpdateText( const AObjectID inTextDocumentID, const AIndex inStartLineIndex, const AIndex inEndLineIndex, 
							const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged );*/
	void					SPI_UpdateTextDrawProperty( const AObjectID inTextDocumentID );
	void					SPI_UpdateTextDrawPropertyAll( const AModeIndex inModeIndex );
	void					SPI_UpdateViewImageSize( const AObjectID inTextDocumentID );
	void					SPI_UpdateLineImageInfo( const AModeIndex inModeIndex );//#450
	void					SPI_ConcatSplitView();
	void					SPI_ConcatSplitView( const AIndex inViewIndex );
	void					SPI_DeleteSplitView( const AIndex inViewIndex );
	void					SPI_AdjustScroll_Center( const ADocumentID inDocumentID );
	//#199 void					SPI_RefreshTextViews( const AObjectID inTextDocumentID ) const;//win 080727
	void					SPI_RevealTextViewByDocumentID( const ADocumentID inDocumentID, const AIndex inSpos, const AIndex inEpos,
							const ABool inInSubTextPane, const AAdjustScrollType inAdjustScrollType );
	void					SPI_RefreshCaret();//#844
	ABool					SPI_GetIsBottomTextView( const AControlID inControlID ) const;
  private:
	void					CreateSplitView( const AIndex inTabIndex, const AIndex inViewIndex, const ANumber inViewHeight,
							//#699 const ABool inCaretMode, 
											const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, 
											const AImagePoint& inOriginOfLocalFrame, const ANumberArray& inCollapsedLinesArray );//#450
	void					UpdateTextDrawProperty( const AIndex inTabIndex );
	AViewID					GetTextViewID( const AIndex inTabIndex, const AIndex inViewIndex );//#199
	ANumber					ChangeTextViewHeight( const AIndex inTabIndex, const AIndex inViewIndex, const ANumber inDelta,
												 const ABool inCompleted );//#866
	
	//サブウインドウ #725
  public:
	void					SPI_MoveOverlaySubWindow( const ASubWindowLocationType inSrcType, const AIndex inSrcIndex,
							const ASubWindowLocationType inDstType, const AIndex inDstIndex );
	void					SPI_AddOverlaySubWindow( const ASubWindowLocationType inLocationType, const AIndex inIndex,
							const ASubWindowType inSubWindowType );
	ABool					SPI_DragSubWindow( const AWindowID inSubWindowID, const AGlobalPoint inMouseGlobalPoint );
	void					SPI_EndDragSubWindow( const AWindowID inSubWindowID );
	void					SPI_UpdateOverlayWindowLocationProperty();
	void					SPI_ExpandCollapseSubWindow( const AWindowID inSubWindowID );
	ABool					SPI_IsCollapsedSubWindow( const AWindowID inSubWindowID );
	void					SPI_DoSubWindowClosed( const AWindowID inSubWindowID );
	void					SPI_SearchInHeaderList();
	void					SPI_SaveSideBarData() { SetDataSideBar(); }
  private:
	void					RefreshSideBar();
	void					SetDataSideBar();
	
	//サブテキスト #725
  private:
	void					ShowHideSubTextColumn( const ABool inShow );
	void					ShowHideSubTextColumnCore( const ABool inShow );
	ANumber					ChangeSubTextPaneColumnWidth( const ANumber inDelta, const ABool inCompleted );
	
	//サブテキスト #725
  public:
	ABool					SPI_IsSubTextView( const AControlID inTextViewControlID ) const;
	AControlID				SPI_GetCurrentSubTextControlID() const;
	ADocumentID				SPI_GetCurrentSubTextPaneDocumentID() const
	{ return mSubTextCurrentDocumentID; }
	ABool					SPI_IsSubTextColumnDisplayed() const { return mSubTextColumnDisplayed; }
	
	//左サイドバー #212
  public:
	ABool					SPI_IsLeftSideBarDisplayed() const { return mLeftSideBarDisplayed; }
	void					SPI_ShowHideLeftSideBarColumn() { ShowHideLeftSideBar(!mLeftSideBarDisplayed); }
	void					SPI_ShowHideLeftSideBarColumn( const ABool inShowHide ) { ShowHideLeftSideBar(inShowHide); }//#379
	void					SPI_DisplayInSubText( const AIndex inTabIndex, 
							const ABool inScroll, const AIndex inScrollToParagraphIndex, //#450 const AImagePoint& inOriginPoint = kImagePoint_00,
							const ABool inSwitchFocus,
							const ABool inDiffDisplay );//#379
	void					SPI_DisplayTabInSubText( const ABool inAdjustScroll );
	void					SPI_DisplayTabInSubText( const AIndex inTabIndex, const ABool inAdjustScroll );//#333
	//#725 void					SPI_UpdateSubPaneArray();
	//#725 void					SPI_GetWindowBoundsWithoutSubPane( ARect& outRect ) const;//#668
	AItemCount				SPI_GetLeftSideBarItemCount() const { return mLeftSideBarArray_Type.GetItemCount(); }//#725
  private:
	void					CreateSubTextInEachTab( const AIndex inTabIndex, 
							/*#699 const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint,*/
							const AImagePoint& inOriginOfLocalFrame );
	void					DeleteSubTextInEachTab( const AIndex inTabIndex );
	void					ShowHideLeftSideBar( const ABool inShow );
	//#341 void					AdjustWindowWidthForSubPane( const ABool inOldSubTextPaneDisplayed );
	void					GetChangedWindowBoundsForSubPane( const ABool inOldSubTextPaneDisplayed, ARect& outBounds );//#341
	//#899 void					SetSubPaneMode( const ASubPaneMode inSubPaneMode );
	//#899 void					UpdateSubPaneAutomatic( const ASubPaneMode inSubPaneMode, const ABool inAdjustScroll );
	ANumber					ChangeLeftSideBarWidth( const ANumber inDelta, const ABool inCompleted );//#409
	ANumber					ChangeLeftSideBarHeight( const AIndex inSubPaneIndex, const ANumber inDelta );
	//#725 void					DeleteSubPaneArray();
	void					UpdateSubTextFileNameButton();
	void					UpdateSubTextFileNameButtonText();
	void					CreateLeftSideBar();//#725
	void					CreateLeftSideBarItem( const AIndex inIndex );//#725
	void					DeleteLeftSideBar();//#725
	void					AddNewSubWindowInLeftSideBar( const ASubWindowType inSubWindowType, const ANumber inParameter,
							const ANumber inHeight );//#725
	void					SetDefaultSubWindowInLeftSideBar();//#725
	ABool								mLeftSideBarDisplayed;
	//#899 ASubPaneMode						mSubPaneMode;
	ANumber								mLeftSideBarWidth;
	ANumber								mPrevLeftSideBarWidth;
	ABool								mSubTextColumnDisplayed;//#725
	ANumber								mSubTextColumnWidth;//#725
	ADocumentID							mSubTextCurrentDocumentID;
	ADocumentID							mLastNormalSubTextDocumentID;//#725
	AArray<ASubWindowType>				mLeftSideBarArray_Type;
	AArray<AFloatNumber>				mLeftSideBarArray_Height;
	AArray<AWindowID>					mLeftSideBarArray_OverlayWindowID;//#725
	AArray<ABool>						mLeftSideBarArray_Dragging;//#725
	AArray<ABool>						mLeftSideBarArray_Collapsed;//#725
	//#725 AArray<AWindowID>					mSubPaneArray_HSeparatorWindowID;
	ABool								mLeftSideBarCreated;//#725
	//#725
	ANumber								mFooterProgressRightPosition;
	
	//右サイドバー #291
  public:
	ABool					SPI_IsRightSideBarDisplayed() const { return mRightSideBarDisplayed; }
	void					SPI_ShowHideRightSideBarColumn() { ShowHideRightSideBar(!mRightSideBarDisplayed); }
	//#725 void					SPI_UpdateInfoPaneArray();
	//#725 void					SPI_UpdateOverlayWindowAlpha();
	AItemCount				SPI_GetRightSideBarItemCount() const { return mRightSideBarArray_Type.GetItemCount(); }//#725
  private:
	void					ShowHideRightSideBar( const ABool inShow );
	ANumber					ChangeRightSideBarWidth( const ANumber inDelta, const ABool inCompleted );//#409
	ANumber					ChangeRightSideBarHeight( const AIndex inInfoPaneIndex, const ANumber inDelta );
	void					DeleteRightSideBar();//#725
	//#725 void					DeleteInfoPaneArray();
	//#725 void					RevertFromOverlay_IdInfoWindow();
	//#725 void					RevertFromOverlay_FileListWindow();
	void					CreateRightSideBar();//#725
	void					CreateRightSideBarItem( const AIndex inIndex );//#725
	void					AddNewSubWindowInRightSideBar( const ASubWindowType inSubWindowType, const ANumber inParameter,
							const ANumber inHeight );//#725
	void					SetDefaultSubWindowInRightSideBar();//#725
	ABool								mRightSideBarDisplayed;
	ANumber								mRightSideBarWidth;
	ANumber								mPrevRightSideBarWidth;
	AArray<ASubWindowType>				mRightSideBarArray_Type;
	AArray<AFloatNumber>				mRightSideBarArray_Height;
	AArray<AWindowID>					mRightSideBarArray_OverlayWindowID;//#725
	AArray<ABool>						mRightSideBarArray_Dragging;//#725
	AArray<ABool>						mRightSideBarArray_Collapsed;//#725
	//#725 AArray<AWindowID>					mInfoPaneArray_HSeparatorWindowID;
	ABool								mRightSideBarCreated;//#725
	
	//サブペインVSeparator #291
  private:
	AWindowID							mLeftSideBarVSeparatorWindowID;
	
	//情報ペインVSeparator #291
  private:
	AWindowID							mRightSideBarVSeparatorWindowID;
	
	//#725
	//サブテキストカラムVSeparator
  private:
	AWindowID							mSubTextColumnVSeparatorWindowID;
	
	/*#725
	//ペイン表示モード #212 #291
  private:
	ABool								mPaneDisplayMode_LeftWideHeader;
	ABool								mPaneDisplayMode_RightWideHeader;
	//#634 ABool								mPaneDisplayMode_OverlayInfoPane;
	*/
	//ポップアップウインドウ
  public:
	void					SPI_ShowIdInfoPopupWindow( const ABool inShow );
	void					SPI_ShowCandidatePopupWindow( const ABool inShow, const ABool inAbbrevInputMode );
	AWindow_CandidateList&	SPI_GetPopupCandidateWindow();
	void					SPI_AdjustPopupWindowsPosition();
	AWindow_KeyToolList&	SPI_GetPopupKeyToolListWindow();
	void					SPI_HideAllPopupWindows();
	ABool					SPI_IsPopupCandidateWindowVisible();
  private:
	ATextWindowPopupLayoutPattern	CalcTextWindowPopupLayout( ATextWindowPopupLayoutData& outLayout ) ;
	AWindow_IdInfo&			GetPopupIdInfoWindow();
	AWindowID							mPopupIdInfoWindowID;
	AWindowID							mPopupCandidateWindowID;
	AWindowID							mPopupKeyToolListWindowID;
	ATextWindowPopupLayoutPattern		mCurrentPopupLayoutPattern;
	
	//Diff表示 #379
  public:
	void					SPI_UpdateDiffDisplay( const ADocumentID inDocumentID );
	void					SPI_AdjustDiffDisplayScroll( const ADocumentID inWorkingDocID, 
							const AControlID inWorkingTextViewControlID, const AIndex inCurrentDiffIndex );
	void					SPI_GetDiffViewWindowPointByParagraph( const ADocumentID inDocID, const AIndex inDiffParagraphStart,
							AWindowPoint& outWindowPoint ) const;
	ABool					SPI_GetDiffDisplayMode() const { return mDiffDisplayMode; }
	void					SPI_SetDiffDisplayMode( const ABool inDiffDisplayMode );
	void					SPI_UpdateDiffButtonText();//#379
  private:
	AIndex					GetDiffDocumentTabIndexFromWorkingTabIndex( const AIndex inTabIndex );
	ABool								mDiffDisplayMode;
	
	//フォーカス制御 #137
  private:
	AControlID				GetTopMostFocusTextViewControlID() const;//#137
	AControlID				GetTopMostFocusTextViewControlID( const AIndex inTabIndex ) const;//#137
	ABool					IsTextViewControlID( const AControlID inControlID ) const;//#137
	void					EVTDO_DoViewFocusActivated( const AControlID inFocusControlID );
	void					UpdateLatentTextView( const AControlID inFocusControlID );
	AControlID							mLatentTextViewContolID;
	
	//コントロール制御
  public:
	void					SPI_UpdateLineNumberButton( const AControlID inTextViewControlID );
  private:
	void					UpdatePositionButton( const AControlID inTextViewControlID );
	
	//ウインドウ情報設定／取得
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_Text; }
	ABool					NVIDO_IsChildWindowVisible() const;//B0411
	
	//ウインドウタイトル設定／取得
  public:
	void					SPI_UpdateWindowTitleBarText();
	void					SPI_GetTitleText( const ADocumentID inDocumentID, AText& outText ) ;
  private:
	void					NVIDO_UpdateTitleBar();
	
	//ジャンプリスト
  public:
	void					SPI_ShowHideJumpListWindow( const ABool inShow );
	//#291 void					SPI_UpdateJumpList( const AObjectID inTextDocumentID );
	//#291 void					SPI_UpdateJumpListWithColor();//R0006
	//#291 void					SPI_UpdateJumpListWithColor( const AObjectID inTextDocumentID );//R0006
	//#291 void					SPI_UpdateJumpListSelection( const AIndex inLineIndex );
	void					SPI_DoJumpListSelected( const ADocumentID inDocumentID,
												   const AIndex inIndex, const AModifierKeys inModifierKeys );
	/*#725
	void					SPI_UpdatePropertyJumpList();
	AWindow_JumpList&		SPI_GetJumpListWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_JumpList,kWindowType_JumpList,mJumpListWindowID);
	const AWindow_JumpList&	SPI_GetJumpListWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_JumpList,kWindowType_JumpList,mJumpListWindowID);
  private:
	AWindowID							mJumpListWindowID;//#199
	//#291 ABool								mJumpListVisible;
	*/
  private:
	void					UpdateFloatingJumpListDisplay();//#725
	AWindowID							mFloatingJumpListWindowID;//#725
	
	//検索ボックス #137
  public:
	void					SPI_SetSearchBoxText( const AText& inText );
	
	//CrossCaret
  public:
	void					SPI_ShowHideCrossCaret();
	
	//
  public:
	/*#199
	void					SPI_EditPreProcess( const AObjectID inDocumentID, const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	void					SPI_EditPostProcess( const AObjectID inDocumentID, const ATextIndex inTextIndex, const AItemCount inInsertedCount );
	*/
	void					SPI_SwitchFocus_SplitView( const ABool inNext );
	void					SPI_SplitView( const ABool inFocusToUpperView );
	void					SPI_SplitView( const AIndex inTabIndex, const AIndex inViewIndex, const ABool inFocusToUpperView );
	ABool					SPI_IsCaretMode() const;
	void					SPI_UpdateToolbarItemValue( const AObjectID inDocumentID );
	void					SPI_MoveToLine( const AIndex inLineIndex, const ABool inParagraphMode );
	void					SPI_ShowSaveWindow();
	void					SPI_SavePreProcess( const AObjectID inDocumentID );
	//#199 void					SPI_SelectText( const AObjectID inDocumentID, const AIndex inStartIndex, const AItemCount inLength );
	void					SPI_GetSelectedText( const AObjectID inDocumentID, AText& outText ) const;
	void					SPI_GetSelect( const AObjectID inDocumentID, AArray<ATextIndex>& outStart, AArray<ATextIndex>& outEnd ) const;
	void					SPI_SetSelect( const AObjectID inDocumentID, const AIndex inStart, const AIndex inEnd );
	void					SPI_GetSelect( const AObjectID inDocumentID, AIndex& outStartIndex, AIndex& outEndIndex ) const;//B0000
	//#725 InfoHeader廃止 void					SPI_SetInfoHeaderText( const AText& inText );
	//#725 InfoHeader廃止 void					SPI_GetInfoHeaderText( AText& outText ) const;
	ANumber					SPI_GetTextViewWidth( const ADocumentID inDocumentID ) ;
	//#199 void					SPI_DoLineInfoChangeStart( const ADocumentID inDocumentID );
	//#199 void					SPI_DoLineInfoChangeEnd( const ADocumentID inDocumentID );
	void					SPI_ShowWrapLetterCountWindow( const AWrapMode inWrapMode );//#1113
	void					SPI_ShowChildErrorWindow( const ADocumentID inDocumentID, const AText& inMessage1, const AText& inMessage2,
													 const ABool inLongDialog = false );
	//B0000 080810 void					SPI_ClearCursorRow();
	void					SPI_DoTool( const AFileAcc& inFile, const AModifierKeys inModifierKeys, const ABool inDontShowDropWarning );
	void					SPI_DoTool_Text( const ADocumentID inDocumentID, const AText& inText, const ABool inDontShowDropWarning );
	void					SPI_SetCandidateIndex( const AControlID inTextViewControlID, const AIndex inIndex );//RC2
	void					SPI_KeyTool( const AControlID inTextViewControlID, const AUChar inKeyChar );//R0073
	void					SPI_ShowAskApplyEditByOtherApp( const ADocumentID inDocumentID );//R0232
	//#138 void					SPI_ShowExternalCommentWindow( const AText& inModuleName, const AIndex inOffset );//RC3
  private:
	void					UpdateToolbarItemValue( const AIndex inTabIndex );
	//#725 InfoHeader廃止 void					SetInfoHeaderTextDefault();
	void					EVTDO_ToolbarItemAdded( const AControlID inID );//#688
	void					UpdateToolbarExistArray();
	
	/*#725
	//InfoHeader #379
  public:
	void					SPI_UpdateInfoHeader( const ADocumentID inDocumentID );
	*/
	//検索
  public:
	ABool					SPI_FindNext( const AFindParameter& inParam );
	ABool					SPI_FindPrevious( const AFindParameter& inParam );
	ABool					SPI_FindFromFirst( const AFindParameter& inParam );
	void					SPI_FindList( const AFindParameter& inParam, const ABool inExtractMatchedText );
	ABool					SPI_ReplaceNext( const AFindParameter& inParam );
	AItemCount				SPI_ReplaceAfterCaret( const AFindParameter& inParam );
	AItemCount				SPI_ReplaceInSelection( const AFindParameter& inParam );
	AItemCount				SPI_ReplaceAll( const AFindParameter& inParam );
	ABool					SPI_ReplaceAndFind( const AFindParameter& inParam );
	ABool					SPI_ReplaceSelectedTextOnly( const AFindParameter& inParam );
	void					SPI_BatchReplace( const AFindParameter& inParam, const AText& inBatchText );
	
	//
  private:
	ABool					NVIDO_AskSaveForAllTabs();
	void					NVIDO_ExecuteClose();
	
	//ツールボタン
  public:
	void					SPI_RemakeToolButtonsAll( const AModeIndex inModeIndex );
	void					SPI_MacroBarItemDrop( const AViewID inButtonViewID, const AWindowPoint& inDropPoint );//#232 #724
	void					SPI_MacroBarItemStopDragging();//#724
	void					SPI_MacroBarItemDragging( const AViewID inButtonViewID, const AWindowPoint& inDropPoint );//#724
	void					SPI_UpdateMacroBarItemContent();
  private:
	void					UpdateMacroBarItemContent( const AIndex inTabIndex );
	void					CreateToolButtons( const AIndex inTabIndex );
	ANumber					CalculateMacroBarHeight( const AIndex inTabIndex,
							const AWindowPoint pt_MacroBar, const ANumber w_MacroBar );
	void					UpdateLayout_MacroBar( const AIndex inTabIndex, 
							const AWindowPoint pt_MacroBar, const ANumber w_MacroBar, const ANumber h_MacroBar );//#725
	void					UpdateMacroBarItemPositions( const AIndex inTabIndex, const ANumber w_MacroBar );//#724
	AView_Toolbar&			GetToolbarView( const AIndex inTabIndex );//#232
	AIndex					FindMacroBarIndex( const AWindowPoint& inPoint ) const;//#724
	AIndex								mDraggingMacroBarCurrentItemDisplayIndex;//#724
	AArray<AIndex>						mDraggingMacroBarDisplayIndexArray;//#724
	
	/*#725
	//検索結果表示ペイン #92
  public:
	ADocumentID				SPI_GetOrCreateFindResultDocument();
	ANumber					SPI_GetFindResultHeight() const;
	void					SPI_SetFindResultHeight( const ANumber inHeight );
  private:
	void					CreateFindResultDocument();
	void					DeleteFindResultDocument();
	ADocumentID							mFindResultDocumentID;
	AViewID								mFindResultViewID;
	AViewID								mFindResultSeparatorViewID;
	AControlID							mFindResultVScrollBarControlID;
	//win AControlID							mFindResultCloseButtonControlID;
	ADisplayPosition					mFindResultDisplayPosition;
	ANumber								mFindResultHeight;
	AWindowID							mFindResultCloseButtonWindowID;//win
	*/
	
	//<所有クラス(AView)生成／削除／取得>
  public:
	
	//<所有クラス(View)インターフェイス>
	
	//処理通知コールバック
  public:
	void					SPI_ViewScrolled( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY,
											  const ABool inRedraw, const ABool inConstrainToImageSize );//#1031
  private:
	void					NVIDO_SelectionChanged( const AControlID inControlID );
	void					NVIDO_ButtonViewDragCanceled( const AControlID inControlID );
	
	//
  public:
	void					SPI_UpdateJumpListSelection( const ABool inAdjustScroll );
	void					SPI_DoListViewHoverUpdated( const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex );
	
	//参照ビュー
  public:
	//void					SPI_Test( const AIndex inLineIndex );
  private:
	AControlID							mRefViewControlID;
	ANumber								mRefViewHeight;
	
	//<所有クラス(AWindow_TextSheet_MoveLine)生成／削除／取得>
  public:
	AWindow_TextSheet_MoveLine&	SPI_GetMoveLieWindow()//#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_MoveLine,kWindowType_TextSheet_MoveLine,mMoveLineWindowID);
	const AWindow_TextSheet_MoveLine&	SPI_GetMoveLieWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_MoveLine,kWindowType_TextSheet_MoveLine,mMoveLineWindowID);
  private:
	AWindowID							mMoveLineWindowID;
	
	/*#844
	//<所有クラス(AWindow_TextSheet_FontSize)生成／削除／取得>
  public:
	AWindow_TextSheet_FontSize&	SPI_GetFontSizeWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_FontSize,kWindowType_TextSheet_FontSize,mFontSizeWindowID);
	const AWindow_TextSheet_FontSize&	SPI_GetFontSizeWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_FontSize,kWindowType_TextSheet_FontSize,mFontSizeWindowID);
  private:
	AWindowID							mFontSizeWindowID;
	*/
	//<所有クラス(AWindow_TextSheet_WrapLetterCount)生成／削除／取得>
  public:
	AWindow_TextSheet_WrapLetterCount&	SPI_GetWrapLetterCountWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_WrapLetterCount,kWindowType_TextSheet_WrapLetterCount,mWrapLetterCountWindowID);
	const AWindow_TextSheet_WrapLetterCount&	SPI_GetWrapLetterCountWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_WrapLetterCount,kWindowType_TextSheet_WrapLetterCount,mWrapLetterCountWindowID);
  private:
	AWindowID							mWrapLetterCountWindowID;
	
	//<所有クラス(AWindow_TextSheet_CorrectEncoding)生成／削除／取得>
  public:
	void					SPI_ShowTextEncodingCorrectWindow( const ABool inModify,
							const ATextEncodingFallbackType inFallbackType );//#473
	AWindow_TextSheet_CorrectEncoding&	SPI_GetCorrectEncodingWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_CorrectEncoding,kWindowType_TextSheet_CorrectEncoding,mTextEncodingCorrectWindowID);
	const AWindow_TextSheet_CorrectEncoding&	SPI_GetCorrectEncodingWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_CorrectEncoding,kWindowType_TextSheet_CorrectEncoding,mTextEncodingCorrectWindowID);
  private:
	AWindowID							mTextEncodingCorrectWindowID;
	
	/*#844
	//<所有クラス(AWindow_TextSheet_PrintOption)生成／削除／取得>
  public:
	void					SPI_ShowPrintOptionWindow();
	AWindow_TextSheet_PrintOption&	SPI_GetPrintOptionWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_PrintOption,kWindowType_TextSheet_PrintOption,mPrintOptionWindowID);
	const AWindow_TextSheet_PrintOption&	SPI_GetPrintOptionWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_PrintOption,kWindowType_TextSheet_PrintOption,mPrintOptionWindowID);
  private:
	AWindowID							mPrintOptionWindowID;
	
	//<所有クラス(AWindow_TextSheet_Property)生成／削除／取得>
  public:
	void					SPI_ShowDocPropertyWindow();
	AWindow_TextSheet_Property&	SPI_GetPropertyWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_Property,kWindowType_TextSheet_Property,mPropertyWindowID);
	const AWindow_TextSheet_Property&	SPI_GetPropertyWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_Property,kWindowType_TextSheet_Property,mPropertyWindowID);
  private:
	AWindowID							mPropertyWindowID;
	*/
	
	//<所有クラス(AWindow_TextSheet_DropWarning)生成／削除／取得>
  public:
	void					SPI_ShowDropWarningWindow( const AText& inText );
	AWindow_TextSheet_DropWarning&	SPI_GetDropWarningWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_DropWarning,kWindowType_TextSheet_DropWarning,mDropWarningWindowID);
	const AWindow_TextSheet_DropWarning&	SPI_GetDropWarningWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_DropWarning,kWindowType_TextSheet_DropWarning,mDropWarningWindowID);
  private:
	AWindowID							mDropWarningWindowID;
	
	//<所有クラス(AWindow_AddToolButton)生成／削除／取得>
  public:
	void					SPI_ShowAddToolButtonWindow();
	void					SPI_ShowAddToolButtonWindow( const AText& inTitle, const AText& inToolText, const ABool inEditWithTextWindow );
	void					SPI_ShowAddToolButtonWindowFromKeyRecord();
	AWindow_AddToolButton&	SPI_GetAddToolButtonWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_AddToolButton,kWindowType_AddToolButton,mAddToolButtonWindowID);
	const AWindow_AddToolButton&	SPI_GetAddToolButtonWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_AddToolButton,kWindowType_AddToolButton,mAddToolButtonWindowID);
  private:
	AWindowID							mAddToolButtonWindowID;
	
	//<所有クラス(AWindow_ChangeToolButton)生成／削除／取得> #1344
  public:
	void					SPI_ShowChangeToolButtonWindow( const AFileAcc& inFile );
	AWindow_ChangeToolButton&	SPI_GetChangeToolButtonWindow()
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_ChangeToolButton,kWindowType_ChangeToolButton,mChangeToolButtonWindowID);
	const AWindow_ChangeToolButton&	SPI_GetChangeToolButtonWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_ChangeToolButton,kWindowType_ChangeToolButton,mChangeToolButtonWindowID);
  private:
	AWindowID							mChangeToolButtonWindowID;
	
	//<所有クラス(AWindow_TextSheet_EditByOtherApp)生成／削除／取得> R0232
  public:
	AWindow_TextSheet_EditByOtherApp&	SPI_GetEditByOtherAppWindow() //#199
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_EditByOtherApp,kWindowType_EditByOtherApp,mEditByOtherAppWindowID);
	const AWindow_TextSheet_EditByOtherApp&	SPI_GetEditByOtherAppWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_EditByOtherApp,kWindowType_EditByOtherApp,mEditByOtherAppWindowID);
  private:
	AWindowID							mEditByOtherAppWindowID;
	
	//<所有クラス(AWindow_TextSheet_SCMCommit)生成／削除／取得> #455
  public:
	AWindow_TextSheet_SCMCommit&	SPI_GetSCMCommitWindow()
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_TextSheet_SCMCommit,kWindowType_TextSheet_SCMCommit,mSCMCommitWindowID);
	const AWindow_TextSheet_SCMCommit&	SPI_GetSCMCommitWindowConst() const
	MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(AWindow_TextSheet_SCMCommit,kWindowType_TextSheet_SCMCommit,mSCMCommitWindowID);
  private:
	AWindowID							mSCMCommitWindowID;
	
	//DiffInfoWindow #379
  public:
	void					SPI_RefreshDiffInfoWindow() const;
	void					SPI_ShowDiffWindow();
	void					SPI_HideDiffWindow();
  private:
	AWindowID							mDiffInfoWindowID;
	
	//#725
  private:
	AWindowID							mSubTextShadowWindowID;
	
	/*#diff
	//#737
	//左右比較
  public:
	void					SPI_CompareWithSubText( const ABool inCompareWithSelectionInSubText );
	
	ABool					SPI_IsMainSubDiffMode( const AControlID inTextControlID ) const;
	AIndex					SPI_MainSubDiff_GetDiffIndexByIncludingLineIndex(  const AControlID inTextControlID , 
							const AIndex inLineIndex, ADiffType& outDiffType ) const;
	ADiffType				SPI_MainSubDiff_GetDiffTypeByDiffIndex( const AIndex inDiffIndex ) const;
	AIndex					SPI_MainSubDiff_GetDiffIndexByStartLineIndex( const AIndex inLineIndex ) const;
	AIndex					SPI_MainSubDiff_GetDiffIndexByEndLineIndex( const AIndex inLineIndex ) const;
	AIndex					SPI_MainSubDiff_GetDiffStartLineIndexByDiffIndex( const AIndex inDiffIndex ) const;
	AIndex					SPI_MainSubDiff_GetDiffEndLineIndexByDiffIndex( const AIndex inDiffIndex ) const;
	AIndex					SPI_MainSubDiff_GetDiffTargetParagraphIndexFromMainTextParagraphIndex( const AIndex inParagraphIndex ) const;
	AIndex					SPI_MainSubDiff_GetDiffIndexByStartParagraphIndex( const AIndex inParagraphIndex ) const;
	AIndex					SPI_MainSubDiff_GetDiffIndexByEndParagraphIndex( const AIndex inParagraphIndex ) const;
	void					SPI_MainSubDiff_GetSubTextWindowPointByParagraph( const AIndex inDiffParagraphStart,
							AWindowPoint& outWindowPoint ) const;
	
  private:
	ABool								mIsMainSubDiffMode;
	AControlID							mMainSubDiff_MainTextControlID;
	AControlID							mMainSubDiff_SubTextControlID;
	AArray<ADiffType>					mMainSubDiff_DiffTypeArray;
	AHashArray<AIndex>					mMainSubDiff_StartParagraphIndexArray;
	AHashArray<AIndex>					mMainSubDiff_EndParagraphIndexArray;
	AArray<AIndex>						mMainSubDiff_OldStartParagraphIndexArray;
	AArray<AIndex>						mMainSubDiff_OldEndParagraphIndexArray;
	ATextArray							mMainSubDiff_DeletedTextArray;
	AArray<AIndex>						mMainSubDiff_ParagraphIndexArray;
	AIndex								mMainSubDiff_DiffRangeMainStartParagraphIndex;
	AIndex								mMainSubDiff_DiffRangeMainEndParagraphIndex;
	AIndex								mMainSubDiff_DiffRangeSubStartParagraphIndex;
	AIndex								mMainSubDiff_DiffRangeSubEndParagraphIndex;
	*/
	//SCM #455
  public:
	void					SPI_ShowCommitWindow( const ADocumentID inDocID );
	void					SPI_NextDiff();
	void					SPI_PrevDiff();
	
	//#725
	//Progress表示
  public:
	void					SPI_ShowImportFileProgress( const ANumber inCount );
	void					SPI_HideImportFileProgress();
  private:
	void					UpdateFooterProgress();
	ABool								mFooterProgress_ShowImportFileProgress;
	ANumber								mFotterProgress_ImportFileProgressRemainCount;
	
	//
  private:
	AView_Ruler&			GetRulerView( const AIndex inTabIndex );//#246
	void					UpdateRulerProperty( const AIndex inTabIndex );
	
	/*
  private:
	ABool								mQuitting;
	ABool								mAllClosing;
	*/
	
  private:
	AText								mCacheLineNumberButtonText;
	AText								mCachePositionButtonText;
	//#725
	AText								mCacheLineNumberButtonText_SubText;
	AText								mCachePositionButtonText_SubText;
	
	//B0000 高速化
  private:
	AIndex								mJumpMenuItemIndex;
	
  private:
	static const AItemCount	kViewCountMax = 8;
	
	//#137 検索ボックス
	static const ANumber	kSearchBoxWidth 					= 100;
	static const ANumber	kSearchBoxHeight 					= 20;
	static const ANumber	kSearchBoxAndButtonMargin			= 2;//win
	static const ANumber	kSearchButtonWidth 					= 20;
	static const ANumber	kSearchButtonHeight 				= 20;
	static const ANumber	kSearchBoxUpMargin					= 6;
	static const ANumber	kSearchButtonRightMargin			= 4;
	
	
	static const ANumber	kNewWindowPositionOffset = 5;
	static const ANumber	kWindowTitleBarHeight = 32;
	
	//#725
  public:
	ABool					SPI_IsDoingSetWindowBoundsAnimate() const { return mDoingSetWindowBoundsAnimate; }
  private:
	ABool								mDoingSetWindowBoundsAnimate;
	
  public:
	static const ANumber	kHeight_MacroBarItem = 18;//#724
	
	//#725 レイアウトモード
  private:
	ALayoutMode				mLayoutMode;
	
	/*#1091-test
  public:
	void					SPI_SetDefferedFocus( const AControlID inControlID ) 
	{
		mDefferedFocusTick = 60;
		mDefferedFocus = inControlID;
	}
  private:
	void					EVTDO_DoTickTimerAction();
	ANumber								mDefferedFocusTick;
	AControlID							mDefferedFocus;
	*/
	
	//
  public:
	static const AControlID	kVirtualControlID_CompareFileChoosen	= 210;
	
	//Object情報取得
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_Text"; }
};

//#199 ATextWindowFactoryは削除。AWindowDefaultFactory<>へ統合。

/**
view bounds更新抑制クラス
*/
class AStSuppressTextWindowUpdateViewBounds
{
  public:
	AStSuppressTextWindowUpdateViewBounds( const AWindowID inTextWindowID );
	~AStSuppressTextWindowUpdateViewBounds();
  private:
	AWindowID	mTextWindowID;
};




