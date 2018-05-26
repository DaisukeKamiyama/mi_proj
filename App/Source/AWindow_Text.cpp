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
/*
AWindow_Text設計ポリシー

・ADocument_Textからの直接の関連はない。(110323)
・AAppから、DocumentIDを引数としてコールされることはある。その場合、該当するDocumentIDがある場合のみ処理を行う。(110323)




*/

#include "stdafx.h"

#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AView_Text.h"
#include "AView_LineNumber.h"
#include "AView_TabSelector.h"
//#include "AView_Separator.h"
#include "AView_Toolbar.h"
#include "AView_Ruler.h"
#include "AApp.h"
#include "ASCMFolderManager.h"
#include "ADocument_IndexWindow.h"
#include "AView_Index.h"
#include "AView_FindResultSeparator.h"
#include "AWindow_DiffInfo.h"
#include "AView_IdInfo.h"
#include "AView_CandidateList.h"
#include "AView_Footer.h"
#include "AWindow_SubTextShadow.h"
#include "AWindow_Notification.h"
#include "AView_Notification.h"
#include "AView_TextViewHSeparator.h"
#include "Cocoa.h"

//===================コントロール===================

//
const AControlID	kControlID_LineNumberButton 	= 101;
const AControlID	kControlID_PositionButton 		= 102;
//#725 const AControlID	kControlID_HScrollBar 			= 103;
const AControlID	kControlID_TabSelector			= 104;
//#246 const AControlID	kControlID_ToolbarPlacard		= 105;
//#725 const AControlID	kControlID_InfoHeader			= 106;
//#246 const AControlID	kControlID_Ruler				= 107;
const AControlID	kControlID_SizeBox				= 108;//win 080711
const AControlID	kSubPaneModeButtonControlID		= 110;//#212
const AControlID	kSubPaneFileNameButtonControlID	= 111;//#212
const AControlID	kSubPaneSyncButtonControlID		= 112;//#212
const AControlID	kSubPaneSwapButtonControlID		= 113;//#212
const AControlID	kSubPaneDiffButtonControlID		= 114;//#379
//#725 const AControlID	kControlID_FindResultInfoPaneBackground	= 119;//#634
const AControlID	kControlID_LineNumberButton_SubText 	= 122;//#725
const AControlID	kControlID_PositionButton_SubText	 	= 123;//#725

//右サイドバーの制御ボタン
const AControlID	kControlID_RightSideBar_SeparatorButton= 116;//#634
const AControlID	kControlID_RightSideBar_PrefButton		= 117;//#634
const AControlID	kControlID_RightSideBar_ShowHideButton	= 118;//#634
//左サイドバーの制御ボタン
const AControlID	kControlID_LeftSideBar_SeparatorButton	= 124;//#725
const AControlID	kControlID_LeftSideBar_PrefButton		= 125;//#725
const AControlID	kControlID_LeftSideBar_ShowHideButton	= 126;//#725

//サイドバー制御ボタンサイズ
//const ANumber	kWidth_LeftSideBarSeparatorButton = 32;//#725
const ANumber	kWidth_RightSideBar_PrefButton = 50;//#725
const ANumber	kWidth_RightSideBar_ShowHideButton = 32;//#725
const ANumber	kWidth_LeftSideBar_PrefButton = 50;//#725
const ANumber	kWidth_LeftSideBar_ShowHideButton = 32;//#725

//フォントパネルでの変更時のvirtual control ID
const AControlID	kFontPanelVirtualControlID = 127;//#688

//virtual contorl ID
const AControlID	kVirtualControlID_AskSaveChangesReply 	= 200;
const AControlID	kVirtualControlID_SaveWindowReply 		= 201;
const AControlID	kVirtualControlID_ToolRemoveOK 			= 202;
const AControlID	kVirtualControlID_FindResultCloseButon	= 203;//win
const AControlID	kVirtualControlID_DeleteMacrosAfterThis	= 204;

//
//ヘッダファイルで定義 const AControlID	kVirtualControlID_CompareFileChoosen 	= 210;

//左右サイドバー背景
const AControlID	kControlID_RightSideBarBackground		= 301;
const AControlID	kControlID_LeftSideBarBackground		= 302;
//ウインドウ全体背景
const AControlID	kControlID_WindowBackground				= 303;

//フッター
const AControlID	kControlID_Footer = 351;
const ANumber		kHeight_Footer = 20;
/*
const AControlID	kControlID_FooterSeparator_MainColumn = 352;
const AControlID	kControlID_FooterSeparator_MainColumn2 = 353;
const AControlID	kControlID_FooterSeparator_SubTextColumn = 354;
const AControlID	kControlID_FooterSeparator_SubTextColumn2 = 355;
const ANumber		kWidth_FooterSeparator = 5;
*/

//#725
//サイズボックス
const ANumber		kWidth_SizeBox = 24;

//#725
//プログレス表示
const AControlID	kControlID_MainColumnProgressIndicator = 402;
const ANumber	kHeight_ProgressIndicator = 16;
const ANumber	kWidth_ProgressIndicator = 16;
const ANumber	kTopMargin_ProgressIndicator = 2;
const ANumber	kRightMargin_ProgressText = 4;
const AControlID	kControlID_MainColumnProgressText = 403;
const ANumber	kImportProgressRightMargin = 20;

//#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kButtonFontSize = 10.0;
const AFloatNumber	kSearchBoxFontSize = 10.0;
/*
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kButtonFontSize = 8.0;
const AFloatNumber	kSearchBoxFontSize = 8.0;
#endif
*/

//レイアウトパラメータ
static const ANumber	kHScrollBarHeight = 15;
static const ANumber	kVScrollBarWidth = 15;
static const ANumber	kLineNumberButtonWidth = 120;
static const ANumber	kPositionButtonWidth = 250;
//#450 static const ANumber	kLeftMarginWidthWithNoEachLineNumber = 8;
//#450 static const ANumber	kLeftMarginWidthWidthEachLineNumber = 48;
static const ANumber	kToolbarHeight = 38;//32;
//#725 static const ANumber	kInfoHeaderHeight = 18;
static const ANumber	kRulerHeight = 20;//15;
static const ANumber	kSplitButtonHeight = 15;
static const ANumber	kSeparatorHeight = 7;
static const ANumber	kSeparatorWidth = 6;//#212 #688 
static const ANumber	kInfoPaneSeparatorHeight = 3;//#219
static const ANumber	kSubPaneSeparatorHeight = 3;//#212
/*#724
static const ANumber	kSmallToolButtonWidth = 16;
static const ANumber	kSmallToolButtonHeight = 16;
static const ANumber	kToolButtonWidth = 24;
static const ANumber	kToolButtonHeight = 24;
static const ANumber	kToolButtonMarginWidth = 2;
static const ANumber	kToolButtonUpMargin = 4;
*/
static const ANumber	kMacroBar_LeftMargin = 10;//#725
static const ANumber	kMacroBar_RightMargin = 6;//#725
static const ANumber	kMacroBar_TopMargin = 0;//#725
static const ANumber	kMacroBar_BottomMargin = 2;//#725
static const ANumber	kMarginHeight_MacroBarItem = 1;//#724

static const ANumber	kInfoPaneColumnControlButtonWidth = 32;//#725

static const ANumber	k_wLeftSideBarVisibleArea = 0;//#725
static const ANumber	k_wRightSideBarVisibleArea = 0;//#725

//右サイドバー高さ
const ANumber	kHeight_RightSidebarControlButtons = 20;

//#724
const AItemCount	kFixMacroItemCount = 2;

//#725
//サブテキスト用シャドウ表示
const ANumber	kWidth_SubTextShadow = 22;

//サブテキスト上部ボタンサイズ
static const ANumber				kSubPaneModeButtonHeight = 20;//18;
static const ANumber				kSubPaneSyncButtonWidth = 70;
static const ANumber				kSubPaneSwapButtonWidth = 70;
static const ANumber				kSubPaneDiffButtonWidth = 90;//#379

//===================幅・高さ最小値===================

//サイドバー最小幅
const ANumber	kMinWidth_SideBar = AApp::kSubWindowMinWidth;

//サブテキスト最小幅
const ANumber	kMinWidth_SubTextColumn = kSubPaneSyncButtonWidth*4;

//メインカラム最小幅・高さ
const ANumber	kMinWidth_MainColumn =120;
const ANumber	kMinHeight_MainColumn = 50;


//編集下余白ONのときの、イメージサイズの余白
const ANumber	kImageYBottomMarginForNormal = 32;
const ANumber	kImageYBottomMarginFor25PercentMargin = 1000;//★検討

#pragma mark ===========================
#pragma mark [クラス]AWindow_Text
#pragma mark ===========================
//テキストウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_Text::AWindow_Text(/*#175 AObjectArrayItem* inParent */)
: AWindow(/*#175inParent*/), 
		/*#199 mJumpListWindow(*this), */mAskingSaveChanges(false), /*#199 mMoveLineWindow(*this), mFontSizeWindow(*this),*///, mInfoWindow(this),
		/*#199 mWrapLetterCountWindow(*this), mTextEncodingCorrectWindow(*this), mPrintOptionWindow(*this), mDocPropertyWindow(*this),
		mAddToolButtonWindow(*this),*//* mQuitting(false), mAllClosing(false), *//* #199 mDropWarningWindow(*this), mEditByOtherAppWindow(*this),*/
		mJumpMenuItemIndex(kIndex_Invalid)
		//#138, mExternalCommentWindow(*this)//RC3
//#725		,mFindResultDocumentID(kObjectID_Invalid)//#92
//#725		,mFindResultViewID(kObjectID_Invalid)//#92
//#725		,mFindResultVScrollBarControlID(kControlID_Invalid)//#92
		//win ,mFindResultCloseButtonControlID(kControlID_Invalid)//#92
//#725		,mFindResultCloseButtonWindowID(kObjectID_Invalid)//win
//#725		,mFindResultDisplayPosition(kDisplayPosition_MiddleBottom), mFindResultHeight(100)//#92
//#899 ,mSubPaneMode(kSubPaneMode_PreviousDocument)//#212
		,mLeftSideBarDisplayed(false), mLeftSideBarWidth(500), mSubTextCurrentDocumentID(kObjectID_Invalid), mPrevLeftSideBarWidth(-1)//#212
//#725		,mPaneDisplayMode_LeftWideHeader(true)//#212
//#725		,mPaneDisplayMode_RightWideHeader(true)//#291
//#634,mPaneDisplayMode_OverlayInfoPane(true)//#291
		,mRightSideBarDisplayed(false), mRightSideBarWidth(300), mPrevRightSideBarWidth(-1)//#291
//#850		,mTabModeMainWindow(false)//#209
		,mRightSideBarVSeparatorWindowID(kObjectID_Invalid)//#291
		,mLeftSideBarVSeparatorWindowID(kObjectID_Invalid)//#291
		,mSubTextColumnVSeparatorWindowID(kObjectID_Invalid)//#725
		,mDiffDisplayMode(false)//#379
		,mDiffInfoWindowID(kObjectID_Invalid)//#379
,mLuaConsoleMode(false)//#567
,mSuppressUpdateViewBoundsCounter(0)//#675
,mFloatingJumpListWindowID(kObjectID_Invalid)//#725
,mLayoutMode(kLayoutMode_4Pane_Header11111111)//kLayoutMode_4Pane_Header11110110)//#725
,mSubTextColumnWidth(500),mSubTextColumnDisplayed(true)//#725
,mPopupIdInfoWindowID(kObjectID_Invalid)//#853
,mPopupCandidateWindowID(kObjectID_Invalid)//#717
,mPopupKeyToolListWindowID(kObjectID_Invalid)//#725
//drop,mIsMainSubDiffMode(false),mMainSubDiff_SubTextControlID(kControlID_Invalid),mMainSubDiff_MainTextControlID(kControlID_Invalid)//#737
//drop,mMainSubDiff_DiffRangeMainStartParagraphIndex(kIndex_Invalid),mMainSubDiff_DiffRangeMainEndParagraphIndex(kIndex_Invalid)//#737
//drop,mMainSubDiff_DiffRangeSubStartParagraphIndex(kIndex_Invalid),mMainSubDiff_DiffRangeSubEndParagraphIndex(kIndex_Invalid)//#737
,mFooterProgressRightPosition(0)//#725
,mDoingSetWindowBoundsAnimate(false)//#725
,mSubTextShadowWindowID(kObjectID_Invalid)//#725
,mDraggingMacroBarCurrentItemDisplayIndex(kIndex_Invalid)//#724
,mCurrentPopupLayoutPattern(kTextWindowPopupLayoutPatternA)//#725
,mFooterProgress_ShowImportFileProgress(false),mFotterProgress_ImportFileProgressRemainCount(0)//#725
,mRightSideBarCreated(false),mLeftSideBarCreated(false)//#725
,mLastNormalSubTextDocumentID(kObjectID_Invalid)//#725
,mLatentTextViewContolID(kControlID_Invalid)
//#1091-test ,mDefferedFocusTick(0),mDefferedFocus(kControlID_Invalid)
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AWindow_Text::AWindow_Text() started.",this);
	//#138 デフォルト範囲(10000～)を使う限り設定不要 NVM_SetDynamicControlIDRange(kDynamicControlIDStart,kDynamicControlIDEnd);
	//#175
	NVI_AddToRotateArray();
	//
	NVI_AddToTimerActionWindowArray();
	//#199
	NVI_AddToAllCloseTargetWindowArray();
	//#986
	NVI_SetCloseByEscapeKey(false);
	
	/*#725
	//JumpListWindow #199
	AChildWindowFactory<AWindow_JumpList>	jumpListWindowFactory(GetObjectID());
	mJumpListWindowID = GetApp().NVI_CreateWindow(jumpListWindowFactory);
	*/
	
	//MoveLineWindow #199
	AChildWindowFactory<AWindow_TextSheet_MoveLine>	moveLineWindowFactory(GetObjectID());
	mMoveLineWindowID = GetApp().NVI_CreateWindow(moveLineWindowFactory);
	NVI_RegisterChildWindow(mMoveLineWindowID);//#694
	
	/*#844
	//FontSizeWindow #199
	AChildWindowFactory<AWindow_TextSheet_FontSize>	fontSizeWindowFactory(GetObjectID());
	mFontSizeWindowID = GetApp().NVI_CreateWindow(fontSizeWindowFactory);
	NVI_RegisterChildWindow(mFontSizeWindowID);//#694
	*/
	//WrapLetterCountWindow #199
	AChildWindowFactory<AWindow_TextSheet_WrapLetterCount>	wrapLetterCountWindowFactory(GetObjectID());
	mWrapLetterCountWindowID = GetApp().NVI_CreateWindow(wrapLetterCountWindowFactory);
	NVI_RegisterChildWindow(mWrapLetterCountWindowID);//#694
	
	//TextEncodingCorrectWindow #199
	AChildWindowFactory<AWindow_TextSheet_CorrectEncoding>	textEncodingCorrectWindowFactory(GetObjectID());
	mTextEncodingCorrectWindowID = GetApp().NVI_CreateWindow(textEncodingCorrectWindowFactory);
	NVI_RegisterChildWindow(mTextEncodingCorrectWindowID);//#694
	
	/*#844
	//PrintOptionWindow #199
	AChildWindowFactory<AWindow_TextSheet_PrintOption>	printOptionWindowFactory(GetObjectID());
	mPrintOptionWindowID = GetApp().NVI_CreateWindow(printOptionWindowFactory);
	NVI_RegisterChildWindow(mPrintOptionWindowID);//#694
	
	//PropertyWindow #199
	AChildWindowFactory<AWindow_TextSheet_Property>	propertyWindowFactory(GetObjectID());
	mPropertyWindowID = GetApp().NVI_CreateWindow(propertyWindowFactory);
	NVI_RegisterChildWindow(mPropertyWindowID);//#694
	*/
	//DropWarningWindow #199
	AChildWindowFactory<AWindow_TextSheet_DropWarning>	dropWarningWindowFactory(GetObjectID());
	mDropWarningWindowID = GetApp().NVI_CreateWindow(dropWarningWindowFactory);
	NVI_RegisterChildWindow(mDropWarningWindowID);//#694
	
	//AddToolButtonWindow #199
	AChildWindowFactory<AWindow_AddToolButton>	addToolButtonFactory(GetObjectID());
	mAddToolButtonWindowID = GetApp().NVI_CreateWindow(addToolButtonFactory);
	NVI_RegisterChildWindow(mAddToolButtonWindowID);//#694
	
	//EditByOtherAppWindow #199
	AChildWindowFactory<AWindow_TextSheet_EditByOtherApp>	editByOtherAppWindowFactory(GetObjectID());
	mEditByOtherAppWindowID = GetApp().NVI_CreateWindow(editByOtherAppWindowFactory);
	NVI_RegisterChildWindow(mEditByOtherAppWindowID);//#694
	
	//SCMCommitWindow #455
	AChildWindowFactory<AWindow_TextSheet_SCMCommit>	scmCommitWindowFactory(GetObjectID());
	mSCMCommitWindowID = GetApp().NVI_CreateWindow(scmCommitWindowFactory);
	NVI_RegisterChildWindow(mSCMCommitWindowID);//#694
	
	if( GetApp().SPI_IsSupportPaneMode() == true )
	{
		//#291 右サイドバーVSeparator
		AWindowDefaultFactory<AWindow_VSeparatorOverlay>	infoPaneVSeparatorFactory;
		mRightSideBarVSeparatorWindowID = GetApp().NVI_CreateWindow(infoPaneVSeparatorFactory);
		
		//#291 サブペインVSeparator
		AWindowDefaultFactory<AWindow_VSeparatorOverlay>	subPaneVSeparatorFactory;
		mLeftSideBarVSeparatorWindowID = GetApp().NVI_CreateWindow(subPaneVSeparatorFactory);
		
		//#725 サブテキストカラムVSeparator
		AWindowDefaultFactory<AWindow_VSeparatorOverlay>	subTextPaneColumnVSeparatorFactory;
		mSubTextColumnVSeparatorWindowID = GetApp().NVI_CreateWindow(subTextPaneColumnVSeparatorFactory);
		
		//#212 サブペイン状態読み込み
		mLeftSideBarDisplayed = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kLeftSideBarDisplayed);
		mLeftSideBarWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kLeftSideBarWidth);
		//#899 mSubPaneMode = static_cast<ASubPaneMode>(GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kSubPaneMode));
		//#668 mLeftSideBarDisplayedはNVIDO_Create()で設定する
		
		//#291 右サイドバー状態読み込み
		mRightSideBarDisplayed = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kRightSideBarDisplayed);
		mRightSideBarWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kRightSideBarWidth);
	}
	//サブテキストカラム
	mSubTextColumnDisplayed = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSubTextColumnDisplayed);
	mSubTextColumnWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kSubTextColumnWidth);
}

//デストラクタ
AWindow_Text::~AWindow_Text()
{
	// #92 NVIDO_DoDeletedへ移動
}

//#92
/**
*/
void	AWindow_Text::NVIDO_DoDeleted()
{
	//ポップアップウインドウを閉じる #717
	if( mPopupIdInfoWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteSubWindow(mPopupIdInfoWindowID);
	}
	if( mPopupCandidateWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteSubWindow(mPopupCandidateWindowID);
	}
	if( mPopupKeyToolListWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteSubWindow(mPopupKeyToolListWindowID);
	}
	
	//#199mJumpListWindow.NVI_Close();
	//フローティングジャンプリスト削除
	//#725 SPI_GetJumpListWindow().NVI_Close();
	/*#725 ExecuteClose()で実行
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_DeleteSubWindow(mFloatingJumpListWindowID);
		mFloatingJumpListWindowID = kObjectID_Invalid;
	}
	*/
	//mInfoWindow.NVI_Close();
	//#199
	//#725 GetApp().NVI_DeleteWindow(mJumpListWindowID);
	GetApp().NVI_DeleteWindow(mMoveLineWindowID);
	NVI_UnregisterChildWindow(mMoveLineWindowID);//#694
	//#844 GetApp().NVI_DeleteWindow(mFontSizeWindowID);
	//#844 NVI_UnregisterChildWindow(mFontSizeWindowID);//#694
	GetApp().NVI_DeleteWindow(mWrapLetterCountWindowID);
	NVI_UnregisterChildWindow(mWrapLetterCountWindowID);//#694
	GetApp().NVI_DeleteWindow(mTextEncodingCorrectWindowID);
	NVI_UnregisterChildWindow(mTextEncodingCorrectWindowID);//#694
	//#844 GetApp().NVI_DeleteWindow(mPrintOptionWindowID);
	//#844 NVI_UnregisterChildWindow(mPrintOptionWindowID);//#694
	//#844 GetApp().NVI_DeleteWindow(mPropertyWindowID);
	//#844 NVI_UnregisterChildWindow(mPropertyWindowID);//#694
	GetApp().NVI_DeleteWindow(mDropWarningWindowID);
	NVI_UnregisterChildWindow(mDropWarningWindowID);//#694
	GetApp().NVI_DeleteWindow(mAddToolButtonWindowID);
	NVI_UnregisterChildWindow(mAddToolButtonWindowID);//#694
	GetApp().NVI_DeleteWindow(mEditByOtherAppWindowID);
	NVI_UnregisterChildWindow(mEditByOtherAppWindowID);//#694
	GetApp().NVI_DeleteWindow(mSCMCommitWindowID);
	NVI_UnregisterChildWindow(mSCMCommitWindowID);//#694
	GetApp().NVI_DeleteWindow(mDiffInfoWindowID);//#379
	GetApp().NVI_DeleteWindow(mSubTextShadowWindowID);//#725
	//#291
	//各分割線ウインドウ削除
	if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteWindow(mRightSideBarVSeparatorWindowID);
	}
	if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteWindow(mLeftSideBarVSeparatorWindowID);
	}
	//#725
	if( mSubTextColumnVSeparatorWindowID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteWindow(mSubTextColumnVSeparatorWindowID);
	}
	/*#725 ExecuteClose()からDeleteRightSideBar(),DeleteLeftSideBar()経由で削除される
	DeleteInfoPaneArray();//#291
	DeleteSubPaneArray();//#212
	*/
	
#if IMPLEMENTATION_FOR_WINDOWS
	//Windowsの場合、最後の１つのTextWindowを削除したときにアプリを終了するための内部イベントを送信
	//直接終了すると、Appオブジェクトの削除でWindow解放を二重にやってしまったりする問題がある
	if( GetApp().NVI_GetWindowCount(kWindowType_Text) == 1 )
	{
		ABase::PostToMainInternalEventQueue(kInternalEvent_Quit,GetObjectID(),0,GetEmptyText(),GetEmptyObjectIDArray());
	}
#endif
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//TextDocument取得（現在のTab）
ADocument_Text&	AWindow_Text::SPI_GetCurrentTabTextDocument()
{
	return GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID());
}
const ADocument_Text&	AWindow_Text::SPI_GetCurrentTabTextDocumentConst() const
{
	return GetApp().SPI_GetTextDocumentConstByID(NVI_GetCurrentDocumentID());
}

//TextDocument取得（指定TextViewのControlIDから）#212
ADocument_Text&	AWindow_Text::SPI_GetTextDocument( const AControlID inTextViewControlID )
{
	return GetTextViewByControlID(inTextViewControlID).SPI_GetTextDocument();
}
const ADocument_Text&	AWindow_Text::SPI_GetTextDocumentConst( const AControlID inTextViewControlID ) const
{
	return GetTextViewConstByControlID(inTextViewControlID).SPI_GetTextDocumentConst();
}

//TextDocument取得（現在のFocus）
ADocument_Text&	AWindow_Text::SPI_GetCurrentFocusTextDocument()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		return GetTextViewByControlID(focus).SPI_GetTextDocument();
	}
	else
	{
		return SPI_GetCurrentTabTextDocument();
	}
}

//ModePrefDB取得
AModePrefDB&	AWindow_Text::GetCurrentTabModePrefDB()
{
	return GetApp().SPI_GetModePrefDB(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex());
}

//TextView取得
AView_Text&	AWindow_Text::GetTextViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Text,kViewType_Text,inControlID);
	/*#199 
	if( NVI_GetViewByControlID(inControlID).NVI_GetViewType() != kViewType_Text )   _ACThrow("not text view",this);
	return dynamic_cast<AView_Text&>(NVI_GetViewByControlID(inControlID));
	*/
}
const AView_Text&	AWindow_Text::GetTextViewConstByControlID( const AControlID inControlID ) const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_Text,kViewType_Text,inControlID);
	/*#199 
	if( NVI_GetViewConstByControlID(inControlID).NVI_GetViewType() != kViewType_Text )   _ACThrow("not text view",this);
	return dynamic_cast<const AView_Text&>(NVI_GetViewConstByControlID(inControlID));
	*/
}

//LineNumberView取得
AView_LineNumber&	AWindow_Text::GetLineNumberViewByControlID( const AControlID inControlID )
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_LineNumber,kViewType_LineNumber,inControlID);
}
const AView_LineNumber&	AWindow_Text::GetLineNumberViewConstByControlID( const AControlID inControlID ) const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_LineNumber,kViewType_LineNumber,inControlID);
}

//TabSelector取得
AView_TabSelector&	AWindow_Text::GetTabSelector()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_TabSelector,kViewType_TabSelector,kControlID_TabSelector);
	/*#199
	return dynamic_cast<AView_TabSelector&>(NVI_GetViewByControlID(kControlID_TabSelector));
	*/
}
const AView_TabSelector&	AWindow_Text::GetTabSelectorConst() const
{
	MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(AView_TabSelector,kViewType_TabSelector,kControlID_TabSelector);
}

//#379
/**
現在のタブの縦分割ビューの数を取得
*/
AItemCount	AWindow_Text::SPI_GetCurrentViewCount() const
{
	return mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount();
}

//#379
/**
現在のタブのLineNumberビューを取得
*/
AView_LineNumber&	AWindow_Text::SPI_GetCurrentLineNumberView( const AIndex inViewIndex )
{
	AControlID	controlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLineNumberViewControlID(inViewIndex);
	return GetLineNumberViewByControlID(controlID);
}

/**
指定タブのLineNumberビューを取得
*/
const AView_LineNumber&	AWindow_Text::SPI_GetLineNumberViewConst( const AIndex inTabIndex, const AIndex inViewIndex ) const
{
	AControlID	controlID = mTabDataArray.GetObjectConst(inTabIndex).GetLineNumberViewControlID(inViewIndex);
	return GetLineNumberViewConstByControlID(controlID);
}

//#853
/**
現在のフォーカスのtext viewを取得
@note サブテキストペインを含む。
*/
AView_Text&	AWindow_Text::SPI_GetCurrentFocusTextView()
{
	//（サブテキストを含め）現在のfocusのtext viewを取得
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus == kControlID_Invalid )
	{
		focus = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0);
	}
	return GetTextViewByControlID(focus);
}

/**
指定document IDに対応する、メインカラムの最初の分割テキストビューを取得
*/
AView_Text& AWindow_Text::SPI_GetMainTextViewForDocumentID( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid ) {_ACThrow("",NULL);};
	
	return GetTextViewByControlID(mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(0));
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//
ABool	AWindow_Text::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
#if 0
	//#92
	if( inID == /*win mFindResultCloseButtonControlID*/kVirtualControlID_FindResultCloseButon )
	{
		//マルチファイル検索中ならClose不可
		if( GetApp().SPI_IsMultiFileFindWorking() == true )   return true;
		//
		DeleteFindResultDocument();
		return true;
	}
#endif
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	/*#724
	//検索ボックス検索ボタン #137
	if( inID == mTabDataArray.GetObject(tabIndex).GetSearchButtonControlID() )
	{
		if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == false )//#268 Commandを押しながらの場合は前検索にする
		{
			FindNextWithSearchBox();
		}
		else
		{
			FindPreviousWithSearchBox();
		}
	}
	*/
	//ビュー分割ボタン
	AIndex	splitButtonIndex = mTabDataArray.GetObject(tabIndex).FindFromSplitButtonControlID(inID);
	if( splitButtonIndex != kIndex_Invalid )
	{
		SPI_SplitView(tabIndex,splitButtonIndex,false);
		return true;
	}
	//ビュー結合ボタン
	AIndex	concatButtonIndex = mTabDataArray.GetObject(tabIndex).FindFromConcatButtonControlID(inID);
	if( concatButtonIndex != kIndex_Invalid )
	{
		SPI_ConcatSplitView(concatButtonIndex);
		return true;
	}
	//ツールボタン
	AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(inID);
	if( toolButtonIndex != kIndex_Invalid )
	{
		ABool	result = true;
		switch( mTabDataArray.GetObjectConst(tabIndex).GetToolButtonType(toolButtonIndex) )
		{
		  case kToolbarItemType_File:
			{
				AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
				if( focus != kControlID_Invalid )
				{
					GetTextViewByControlID(focus).SPI_DoTool(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonFile(toolButtonIndex),inModifierKeys,false);
				}
				break;
			}
			/*#232
		  case kToolbarItemType_Folder:
			{
				AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
				if( focus != kControlID_Invalid )
				{
					if( NVI_GetControlNumber(inID) != kIndex_Invalid )
					{
						AIndex	toolButtonDBIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex);
						AText	filename;
						GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetModeIndex()).
							GetToolbarItemFilenameArray(toolButtonDBIndex).Get(NVI_GetControlNumber(inID),filename);
						AFileAcc	file;
						file.SpecifyChild(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonFile(toolButtonIndex),filename);
						GetTextViewByControlID(focus).SPI_DoTool(file,inModifierKeys,false);
					}
				}
				break;
			}
			*/
		  case kToolbarItemType_ModePref:
			{
				GetApp().SPI_ShowModePrefWindow(GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetModeIndex());
				break;
			}
		  case kToolbarItemType_AddToolButton:
			{
				SPI_ShowAddToolButtonWindow();
				break;
			}
			/*#232 EVT_DoMenuItemSelected()で実行される
		  case kToolbarItemType_TextEncoding:
			{
				AText	tecname;
				NVI_GetControlText(inID,tecname);
				//B0343
				if( ATextEncodingWrapper::CheckTextEncodingAvailability(tecname) == false )
				{
					tecname.SetCstring("UTF-8");
					NVI_SetControlText(inID,tecname);
				}
				GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetTextEncoding(tecname);
				break;
			}
		  case kToolbarItemType_ReturnCode:
			{
				GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetReturnCode(static_cast<AReturnCode>(NVI_GetControlNumber(inID)));
				break;
			}
		  case kToolbarItemType_WrapMode:
			{
				AWrapMode	wrapMode = static_cast<AWrapMode>(NVI_GetControlNumber(inID));
				switch(wrapMode)
				{
				  case kWrapMode_NoWrap:
					{
						GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetWrapMode(wrapMode,0);
						break;
					}
				  case kWrapMode_WordWrap:
					{
						GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetWrapMode(wrapMode,0);
						break;
					}
				  case kWrapMode_WordWrapByLetterCount:
					{
						SPI_ShowWrapLetterCountWindow();
						break;
					}
				}
				break;
			}
		  case kToolbarItemType_Mode:
			{
				AText	modename;
				NVI_GetControlText(inID,modename);
				GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetMode(modename);
				break;
			}
			*/
		  default:
			{
				result = false;
				break;
			}
		}
		return result;
	}
	/*#164
	//R0228
	if( inID == GetTabSelector().SPI_GetTabCloseButtonControlID() )
	{
		AIndex	tabIndex = GetTabSelector().SPI_GetCurrentCloseButtonIndex();
		if( tabIndex != kIndex_Invalid )
		{
			TryClose(tabIndex);
		}
		return true;
	}
	*/
	//
	ABool	result = true;
	switch(inID)
	{
		//#724
		//ツールバー項目　テキストエンコーディング選択時処理
	  case kControlID_Toolbar_TextEncoding:
		{
			AText	tecname;
			NVI_GetControlText(kControlID_Toolbar_TextEncoding,tecname);
			GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetTextEncoding(tecname);
			break;
		}
		//ツールバー項目　改行コード選択時処理
	  case kControlID_Toolbar_ReturnCode:
		{
			GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).
					SPI_SetReturnCode((AReturnCode)NVI_GetControlNumber(kControlID_Toolbar_ReturnCode));
			break;
		}
		//ツールバー項目　折り返し選択時処理
	  case kControlID_Toolbar_WrapMode:
		{
			AWrapMode	wrapMode = (AWrapMode)NVI_GetControlNumber(kControlID_Toolbar_WrapMode);
			switch(wrapMode)
			{
			  case kWrapMode_NoWrap:
			  case kWrapMode_WordWrap:
			  case kWrapMode_LetterWrap://#1113
				{
					GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetWrapMode(wrapMode,0);
					break;
				}
			  case kWrapMode_WordWrapByLetterCount:
			  case kWrapMode_LetterWrapByLetterCount://#1113
				{
					SPI_ShowWrapLetterCountWindow(wrapMode);//#1113
					break;
				}
			}
			break;
		}
		//ツールバー項目　モード選択処理
	  case kControlID_Toolbar_Mode:
		{
			AText	modename;
			NVI_GetControlText(kControlID_Toolbar_Mode,modename);
			GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_SetModeByRawName(modename);
			break;
		}
		//ツールバー項目　新規ドキュメント選択時処理
	  case kControlID_Toolbar_NewDocument:
		{
			AText	modename;
			NVI_GetControlText(kControlID_Toolbar_NewDocument,modename);
			AModeIndex modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modename);
			if( modeIndex == kIndex_Invalid )   {_ACError("",this); break;}
			GetApp().SPNVI_CreateNewTextDocument(modeIndex);
			//チェックマーク解除
			NVI_SetControlNumber(kControlID_Toolbar_NewDocument,kIndex_Invalid);
			break;
		}
		//ツールバー項目　設定
	  case kControlID_Toolbar_Pref:
		{
			AText	modename;
			NVI_GetControlText(kControlID_Toolbar_Pref,modename);
			AModeIndex modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modename);
			if( modeIndex == kIndex_Invalid )
			{
				GetApp().SPI_ShowAppPrefWindow();
			}
			else
			{
				GetApp().SPI_ShowModePrefWindow(modeIndex);
			}
			break;
		}
		//ツールバー項目　テキストマーカー選択時処理
	  case kControlID_Toolbar_FindHighlight:
		{
			AIndex	textMarkerIndex = NVI_GetControlNumber(kControlID_Toolbar_FindHighlight);
			if( textMarkerIndex >= GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kTextMarkerArray_Title) )
			{
				//設定画面を開く
				GetApp().SPI_SelectOrCreateTextMarkerWindow();
			}
			else
			{
				//グループを選択
				GetApp().SPI_SetCurrentTextMarkerGroupIndex(textMarkerIndex);
			}
			break;
		}
		//ナビボタン左
	  case kControlID_Toolbar_GoPrevious:
		{
			GetApp().SPI_NavigatePrev();
			break;
		}
		//ナビボタン右
	  case kControlID_Toolbar_GoNext:
		{
			GetApp().SPI_NavigateNext();
			break;
		}
		//ツールバー項目　キー記録開始
	  case kControlID_Toolbar_KeyRecordStart:
		{
			//キー記録開始
			GetApp().SPI_StartRecord();
			break;
		}
		//ツールバー項目　キー記録停止
	  case kControlID_Toolbar_KeyRecordStop:
		{
			//キー記録停止
			GetApp().SPI_StopRecord();
			//キー記録停止ボタンのトグルONを解除
			NVI_SetControlBool(kControlID_Toolbar_KeyRecordStop,false);
			break;
		}
		//ツールバー項目　キー再生
	  case kControlID_Toolbar_KeyRecordPlay:
		{
			//キー再生
			SPI_GetCurrentFocusTextView().SPI_PlayKeyRecord();
			//キー再生ボタンのトグルONを解除
			NVI_SetControlBool(kControlID_Toolbar_KeyRecordPlay,false);
			break;
		}
		//ツールバー項目　キー記録をマクロに追加
	  case kControlID_Toolbar_AddMacroBar:
		{
			//キーをマクロに追加
			SPI_ShowAddToolButtonWindowFromKeyRecord();
			//マクロ追加ボタンのトグルONを解除
			NVI_SetControlBool(kControlID_Toolbar_AddMacroBar,false);
			break;
		}
		//ツールバー項目　ポップアップ禁止
	  case kControlID_Toolbar_ProhibitPopup:
		{
			//ポップアップ禁止状態取得、設定
			GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kProhibitPopup,
													 !GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup));
			//禁止にしたときは、現在表示中のポップアップを全て閉じる
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup) == true )
			{
				SPI_HideAllPopupWindows();
			}
			//全てのウインドウのツールバー値更新
			GetApp().SPI_UpdateToolbarItemValueAll();
			break;
		}
		/*
		//ツールバー項目　モード設定
	  case kControlID_Toolbar_ModePref:
		{
			GetApp().SPI_ShowModePrefWindow(GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetModeIndex());
			break;
		}
		//ツールバー項目　環境設定
	  case kControlID_Toolbar_AppPref:
		{
			GetApp().SPI_ShowAppPrefWindow();
			break;
		}
		*/
		//ツールバー項目　左サイドバー表示非表示
	  case kControlID_Toolbar_LeftSideBar:
		{
			/*test
			SPI_GetCurrentTabTextDocument().TestGuessTextEncoding();
			break;
			*/
			ShowHideLeftSideBar(!mLeftSideBarDisplayed);
			break;
		}
		//ツールバー項目　右サイドバー表示非表示
	  case kControlID_Toolbar_RightSideBar:
		{
			ShowHideRightSideBar(!mRightSideBarDisplayed);
			break;
		}
		//ツールバー項目　サブテキスト表示非表示
	  case kControlID_Toolbar_2Screens:
		{
			ShowHideSubTextColumn(!mSubTextColumnDisplayed);
			break;
		}
		//ツールバー項目　ビュー縦分割
	  case kControlID_Toolbar_SplitView:
		{
			SPI_SplitView(false);
			break;
		}
		//ツールバー項目　コミット
	  case kControlID_Toolbar_Commit:
		{
			SPI_ShowCommitWindow(NVI_GetCurrentDocumentID());
			break;
		}
		//ツールバー項目　次を検索
	  case kControlID_Toolbar_FindNext:
		{
			SPI_GetCurrentFocusTextView().SPI_FindSelectedNext();
			break;
		}
		//ツールバー項目　前を検索
	  case kControlID_Toolbar_FindPrevious:
		{
			SPI_GetCurrentFocusTextView().SPI_FindSelectedPrevious();
			break;
		}
		//ツールバー項目　マルチファイル検索
	  case kControlID_Toolbar_MultiFileFind:
		{
			AText	text;
			SPI_GetCurrentFocusTextView().SPI_GetSelectedText(text);
			GetApp().SPI_ShowMultiFileFindWindow(text);
			break;
		}
		//ツールバー項目　選択範囲をマルチファイル検索
	  case kControlID_Toolbar_MultiFileFindExecute:
		{
			AText	text;
			SPI_GetCurrentFocusTextView().SPI_GetSelectedText(text);
			//★未実装
			break;
		}
		//ツールバー項目　フローティングウインドウ表示非表示
	  case kControlID_Toolbar_ShowHideFloatingWindows:
		{
			GetApp().SPI_ShowHideFloatingSubWindowsTemporary();
			//全てのウインドウのツールバー値更新
			GetApp().SPI_UpdateToolbarItemValueAll();
			break;
		}
		//行番号ボタン
	  case kControlID_LineNumberButton:
		{
			SPI_ShowMoveLineWindow();
			break;
		}
		//行番号ボタン（サブテキスト）
	  case kControlID_LineNumberButton_SubText:
		{
			SPI_ShowMoveLineWindow();
			break;
		}
		//場所ボタン
	  case kControlID_PositionButton:
		{
			SPI_ShowHideJumpListWindow(true);
			break;
		}
		//場所ボタン（サブテキスト）
	  case kControlID_PositionButton_SubText:
		{
			SPI_ShowHideJumpListWindow(true);
			break;
		}
		//ツールバー項目　検索フィールド
	  case kControlID_Toolbar_SearchField:
		{
			FindNextWithSearchBox();
			break;
		}
		//#232
		//ツールボタン削除
	  case kVirtualControlID_ToolRemoveOK:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( toolButtonIndex != kIndex_Invalid )
			{
				AIndex	toolButtonDBIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex);
				GetApp().SPI_GetModePrefDB(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex()).DeleteToolbarItem(toolButtonDBIndex);
			}
			break;
		}
		//コンテキストメニュー選択したマクロバー項目以降を全て削除
	  case kVirtualControlID_DeleteMacrosAfterThis:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	clickedMacroDisplayIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( clickedMacroDisplayIndex != kIndex_Invalid )
			{
				//後ろから削除していく
				AItemCount	displayedMacroCount = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonCount();
				for( AIndex displayIndex = displayedMacroCount-1; displayIndex >= clickedMacroDisplayIndex; displayIndex-- )
				{
					AIndex	dbIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(displayIndex);
					GetCurrentTabModePrefDB().DeleteToolbarItem(dbIndex);
				}
			}
			break;
		}
		/*#899
		//#212 サブペインモード変更
	  case kSubPaneModeButtonControlID:
		{
			switch(mSubPaneMode)
			{
			  case kSubPaneMode_PreviousDocument:
				{
					SetSubPaneMode(kSubPaneMode_SameDocument);
					break;
				}
			  case kSubPaneMode_SameDocument:
				{
					SetSubPaneMode(kSubPaneMode_Manual);
					break;
				}
			  case kSubPaneMode_Manual:
				{
					SetSubPaneMode(kSubPaneMode_PreviousDocument);
					break;
				}
			}
			break;
		}
		*/
		//サブペイン表示をメインペインに同期させる
	  case kSubPaneSyncButtonControlID:
		{
			//Diff表示解除 #379
			SPI_SetDiffDisplayMode(false);
			//
			SPI_DisplayTabInSubText(true);
			//#899 SetSubPaneMode(kSubPaneMode_Manual);
			
			//
			if( false )
			{
				//現在のviewの折りたたみ状態を取得
				ANumberArray	collapsedLinesArray;
				GetTextViewByControlID(mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(0)).
						SPI_GetFoldingCollapsedLines(collapsedLinesArray);
				//
				GetTextViewByControlID(mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSubTextControlID()).
						SPI_CollapseLines(collapsedLinesArray,true,true);
			}
			break;
		}
		//サブペイン←→メインペイン入替
	  case kSubPaneSwapButtonControlID:
		{
			if( mSubTextCurrentDocumentID != kObjectID_Invalid )
			{
				AIndex	subPaneTabIndex = NVI_GetTabIndexByDocumentID(mSubTextCurrentDocumentID);
				AControlID	subPaneTextControlID = mTabDataArray.GetObjectConst(subPaneTabIndex).GetSubTextControlID();
				
				//現在のviewの折りたたみ状態を取得
				ANumberArray	mainCollapsedLinesArray;
				ANumberArray	subCollapsedLinesArray;
				if( false )
				{
					GetTextViewByControlID(mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(0)).
							SPI_GetFoldingCollapsedLines(mainCollapsedLinesArray);
					GetTextViewByControlID(subPaneTextControlID).SPI_GetFoldingCollapsedLines(subCollapsedLinesArray);
				}
				
				//サブテキストのスクロール開始位置パラグラフを取得 #450
				AIndex	subTextOriginParagraph = GetTextViewByControlID(subPaneTextControlID).SPI_GetOriginParagraph();
				//
				ABool	oldSubPaneFocused = GetTextViewByControlID(subPaneTextControlID).NVI_IsFocusActive();//#600
				AIndex	oldMainTabIndex = NVI_GetCurrentTabIndex();//#600
				
				//サブペインのタブ選択＋スクロール調整
				SPI_DisplayTabInSubText(true);
				//#899 SetSubPaneMode(kSubPaneMode_Manual);
				
				//メインペインのタブ選択
				NVI_SelectTab(subPaneTabIndex);
				//メインペインのフォーカスが当たっているビューを取得
				//タブ内のメインテキストの最終フォーカスTextViewを取得（もし最終フォーカスが未設定なら、メインペイン内の最初の分割ビューにする）
				AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).
							GetLatentMainTextViewControlID();
				if( IsTextViewControlID(mainTextViewControlID) == false )
				{
					mainTextViewControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0);
				}
				
				//
				if( false )
				{
					GetTextViewByControlID(mTabDataArray.GetObject(oldMainTabIndex).GetSubTextControlID()).
							SPI_CollapseLines(mainCollapsedLinesArray,true,true);
					GetTextViewByControlID(mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(0)).
							SPI_CollapseLines(subCollapsedLinesArray,true,true);
				}
				
				//新規にフォーカスを当てるべきControlID #600
				//元々のフォーカスがメインペインなら、今のサブペインにフォーカス
				AControlID	newFocusControlID = mTabDataArray.GetObjectConst(oldMainTabIndex).GetSubTextControlID();
				//メインペインのフォーカスが当たっているビューのスクロール調整
				if( IsTextViewControlID(mainTextViewControlID) == true )
				{
					//#450 GetTextViewByControlID(mainTextViewControlID).NVI_ScrollTo(subPaneOriginPoint);
					//上で取得したスクロール開始位置パラグラフへscroll #450
					GetTextViewByControlID(mainTextViewControlID).SPI_ScrollToParagraph(subTextOriginParagraph,0);
					//元々のフォーカスがサブペインなら、今のメインペインにフォーカス #600
					if( oldSubPaneFocused == true )
					{
						newFocusControlID = mainTextViewControlID;
					}
				}
				//フォーカス設定 #600
				NVI_SwitchFocusTo(newFocusControlID);
			}
			break;
		}
		//サブペイン比較モード切り替え #379
	  case kSubPaneDiffButtonControlID:
		{
			/*#81
			if( NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_GetToggleOn() == true &&
						GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )
			{
				//比較対象ドキュメント未設定ならDiff対象設定ウインドウ表示
				GetApp().SPI_GetDiffWindow().SPNVI_Show(NVI_GetCurrentDocumentID());
			}
			else
			*/
			{
				SPI_SetDiffDisplayMode(NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_GetToggleOn());
			}
			break;
		}
		//サイドバー環境設定 #634
	  case kControlID_RightSideBar_PrefButton:
		{
			/*
			GetApp().SPI_GetAppPrefWindow().NVI_CreateAndShow();
			GetApp().SPI_GetAppPrefWindow().NVI_SelectTabControl(2);
			GetApp().SPI_GetAppPrefWindow().NVI_RefreshWindow();
			GetApp().SPI_GetAppPrefWindow().NVI_SwitchFocusToFirst();
			*/
			//★未実装
			break;
		}
		//サイドバーshowhide #634
	  case kControlID_RightSideBar_ShowHideButton:
		{
			ShowHideRightSideBar(!mRightSideBarDisplayed);
			break;
		}
		//サブペインshowhide #634
	  case kControlID_LeftSideBar_ShowHideButton:
		{
			ShowHideLeftSideBar(!mLeftSideBarDisplayed);
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

/**
マクロバー項目を実行する
*/
void	AWindow_Text::SPI_DoMacroButton( const AIndex inToolButtonIndex )
{
	EVT_Clicked(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetToolButtonControlID(2+inToolButtonIndex),0);
}

//コントロール値変更通知時のコールバック（処理を行ったらtrueを返す）
ABool	AWindow_Text::EVTDO_ValueChanged( const AControlID inID )
{
	//#688
	//フォントパネル変更時処理
	if( inID == kFontPanelVirtualControlID )
	{
		//フォントパネルからフォント・サイズ取得
		AText	fontname;
		AFloatNumber	fontsize = 9.0;
		NVI_GetControlFont(kFontPanelVirtualControlID,fontname,fontsize);
		//ドキュメントにフォント・サイズ設定
		SPI_GetCurrentFocusTextDocument().SPI_SetFontNameAndSize(fontname,fontsize);
		return true;
	}
	//スクロールバーの値が変わった場合の処理
	AIndex	viewIndex = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).FindFromVScrollBarControlID(inID);
	if( viewIndex != kIndex_Invalid )
	{
		AControlID	textViewControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(viewIndex);
		AImagePoint	pt;
		GetTextViewByControlID(textViewControlID).NVI_GetOriginOfLocalFrame(pt);
		pt.y = NVI_GetControlNumber(inID);
		GetTextViewByControlID(textViewControlID).NVI_ScrollTo(pt);
		return true;
	}
	return false;
}

//WindowActivated時のコールバック
void	AWindow_Text::EVTDO_WindowActivated()
{
	//==================ジャンプリストを表示する==================
	//#725
	//ジャンプリストの表示、配置
	UpdateFloatingJumpListDisplay();
	
	//半透明ウインドウ#1255
	SPI_UpdateWindowAlpha();
	
	/*#724
	//#137 検索ボックスのバックスラッシュモードを設定
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )//#688 if追加
	{
		AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
		if( searchBoxControlID != kControlID_Invalid )
		{
			NVI_GetEditBoxView(searchBoxControlID).NVI_SetInputBackslashByYenKeyMode(
						GetApp().SPI_GetModePrefDB(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputBackslashByYenKey));
		}
	}
	*/
	
	//==================サイドバーの表示を更新==================
	//#725
	//サイドバー表示更新
	//#1275 RefreshSideBar();
}

//WindowDeactivated時のコールバック
void	AWindow_Text::EVTDO_WindowDeactivated()
{
	//==================ジャンプリストを非表示にする==================
	//win 080703 最前面のテキストウインドウではなくなるときのみジャンプリストを隠すように変更
	//Windowsではフローティングウインドウが無いので、ジャンプリストクリック時に、テキストウインドウのdeactivate→ジャンプリストのHideが動作してしまう
	if( GetApp().NVI_GetMostFrontWindowID(kWindowType_Text) != GetObjectID() )
	{
		if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
		{
			/*#725
			if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetJumpListVisible() == true )
			{
				SPI_GetJumpListWindow().NVI_HideIfNotOverlayMode();//#291
			}
			*/
			if( mFloatingJumpListWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Hide();
			}
		}
	}
	
	//半透明ウインドウ#1255
	SPI_UpdateWindowAlpha();
	
	//==================サイドバーの表示を更新==================
	//#725
	//サイドバー表示更新
	//#1275 RefreshSideBar();
}

//#688
/**
ウインドウフォーカスアクティベート時処理
*/
void	AWindow_Text::EVTDO_WindowFocusActivated()
{
	//★実行場所検討 キーワード情報ウインドウ等クリック時等は、keywindow移動しないため。
	//#725
	//フローティングウインドウのz-orderを調整
	GetApp().SPI_AdjustAllFlotingWindowsZOrder();
}

//#688
/**
ウインドウフォーカスディアクティベート時処理
*/
void	AWindow_Text::EVTDO_WindowFocusDeactivated()
{
}

//#725
#if 0
//B0442
//フローティングウインドウを隠す
void	AWindow_Text::SPI_HideFloatingWindow()
{
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		/*#725
		if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetJumpListVisible() == true )
		{
			SPI_GetJumpListWindow().NVI_HideIfNotOverlayMode();//#291
		}
		*/
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Hide();
		}
	}
}
#endif

//#1255
void	AWindow_Text::SPI_UpdateWindowAlpha()
{
	//B0333
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kAlphaWindowModeV3) == true )
	{
		//不透明率取得
		AFloatNumber	alpha = 1.0;
		//最前面テキストウインドウかどうかでそれぞれの不透明率取得
		if( GetApp().NVI_GetMostFrontWindowID(kWindowType_Text) == GetObjectID() )
		{
			alpha = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAlphaWindowPercent1V3);
			alpha /= 100.0;
		}
		else
		{
			alpha = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAlphaWindowPercent2V3);
			alpha /= 100.0;
		}
		//メインウインドウ
		NVI_SetWindowTransparency(alpha);
		//右サイドバー
		for( AIndex index = 0; index < mRightSideBarArray_OverlayWindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(index);
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowTransparency(alpha);
		}
		//左サイドバー
		for( AIndex index = 0; index < mLeftSideBarArray_OverlayWindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(index);
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowTransparency(alpha);
		}
	}
	else
	{
		NVI_SetWindowTransparency(1.0);
		//右サイドバー
		for( AIndex index = 0; index < mRightSideBarArray_OverlayWindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(index);
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowTransparency(1.0);
		}
		//左サイドバー
		for( AIndex index = 0; index < mLeftSideBarArray_OverlayWindowID.GetItemCount(); index++ )
		{
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(index);
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowTransparency(1.0);
		}
	}
}

//WindowCollapsed時のコールバック
void	AWindow_Text::EVTDO_WindowCollapsed()
{
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		/*#725
		if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetJumpListVisible() == true )
		{
			SPI_GetJumpListWindow().NVI_HideIfNotOverlayMode();//#291
		}
		*/
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Hide();
		}
	}
}

/**
ウインドウ位置変更通知時のコールバック
*/
void	AWindow_Text::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//ウインドウboundsに変化が無ければ何もしない
	if( inPrevBounds.left == inCurrentBounds.left && inPrevBounds.top == inCurrentBounds.top &&
				inPrevBounds.right == inCurrentBounds.right && inPrevBounds.bottom == inCurrentBounds.bottom )
	{
		return;
	}
	/*#725
	if( mRightSideBarDisplayed == false )//#291
	{
		SPI_GetJumpListWindow().SPI_UpdatePosition();
	}
	*/
	//B0407 動的リサイズ中、このメソッドがコールされる。リサイズに従って、コントロール部品の配置Updateを行う。（行折り返し等はUpdateしない）
	//if( ((inPrevBounds.right-inPrevBounds.left) != (inCurrentBounds.right-inCurrentBounds.left)) ||
	//			((inPrevBounds.bottom-inPrevBounds.top) != (inCurrentBounds.bottom-inCurrentBounds.top)) )
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex(),kUpdateViewBoundsTriggerType_WindowBoudnsChanged);
	}
	//フローティングジャンプリストの位置更新
	UpdateFloatingJumpListDisplay();
}

//
ABool	AWindow_Text::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	switch(inMenuItemID)
	{
		//行移動
	  case kMenuItemID_MoveLine:
		{
			SPI_ShowMoveLineWindow();
			break;
		}
		//フォントサイズ  #966
		//メニュー項目の文字列の最初の数字に従ってフォントサイズ設定
	  case kMenuItemID_FontSize:
	  case kMenuItemID_FontSize60:
	  case kMenuItemID_FontSize80:
	  case kMenuItemID_FontSize90:
	  case kMenuItemID_FontSize95:
	  case kMenuItemID_FontSize100:
	  case kMenuItemID_FontSize105:
	  case kMenuItemID_FontSize110:
	  case kMenuItemID_FontSize120:
	  case kMenuItemID_FontSize140:
	  case kMenuItemID_FontSize160:
	  case kMenuItemID_FontSize180:
	  case kMenuItemID_FontSize240:
		{
			AFloatNumber	num = inDynamicMenuActionText.GetFloatNumber();
			SPI_GetCurrentTabTextDocument().SPI_SetTemporaryFontSize(num);//#966
			break;
		}
	  case kMenuItemID_RevertFontSize:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetTemporaryFontSize(0);
			break;
		}
			/*#844
		//フォントサイズ（その他）
	  case kMenuItemID_FontSizeOther:
		{
			SPI_GetFontSizeWindow().NVI_Create(NVI_GetCurrentDocumentID());
			SPI_GetFontSizeWindow().NVI_Show();
			SPI_GetFontSizeWindow().NVI_SwitchFocusToFirst();
			break;
		}*/
		//フォント
	  case kMenuItemID_Font:
		{
			//win AFont	font = inDynamicMenuActionText.GetNumber();
			/*win AFont	font;
			AFontWrapper::GetFontByName(inDynamicMenuActionText,font);
			SPI_GetCurrentTextDocument().SPI_SetFont(font);*/
			SPI_GetCurrentTabTextDocument().SPI_SetFontName(inDynamicMenuActionText);
			break;
		}
		//TextEncoding
	  case kMenuItemID_SetTextEncoding:
		{
			//B0343
			if( ATextEncodingWrapper::CheckTextEncodingAvailability(inDynamicMenuActionText) == false )
			{
				break;
			}
			SPI_GetCurrentTabTextDocument().SPI_SetTextEncoding(inDynamicMenuActionText);
			break;
		}
		//改行コード
	  case kMenuItemID_ReturnCode_CR:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetReturnCode(returnCode_CR);
			break;
		}
	  case kMenuItemID_ReturnCode_CRLF:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetReturnCode(returnCode_CRLF);
			break;
		}
	  case kMenuItemID_ReturnCode_LF:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetReturnCode(returnCode_LF);
			break;
		}
		//行送り
	  case kMenuItemID_WrapMode_NoWrap:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetWrapMode(kWrapMode_NoWrap,0);
			break;
		}
	  case kMenuItemID_WrapMode_WordWrap:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetWrapMode(kWrapMode_WordWrap,0);
			break;
		}
	  case kMenuItemID_WrapMode_WordWrapByLetterCount:
		{
			SPI_ShowWrapLetterCountWindow(kWrapMode_WordWrapByLetterCount);//#1113
			break;
		}
		//#1113
	  case kMenuItemID_WrapMode_LetterWrap:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetWrapMode(kWrapMode_LetterWrap,0);
			break;
		}
	  case kMenuItemID_WrapMode_LetterWrapByLetterCount:
		{
			SPI_ShowWrapLetterCountWindow(kWrapMode_LetterWrapByLetterCount);
			break;
		}
		//フォント #868
		//モードのフォントを使う
	  case kMenuItemID_UseModeFont:
		{
			//ドキュメントに設定
			SPI_GetCurrentTabTextDocument().SPI_SetUseModeFont(true);
			//モード設定画面を開く（ただし、「標準モードと同じ」設定にしている場合は、標準モードを開く）
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kSameAsNormalMode_Font) == true )
			{
				modeIndex = kStandardModeIndex;
			}
			GetApp().SPI_ShowModePrefWindow(modeIndex);
			GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SelectTabControl(0);
			GetApp().SPI_GetModePrefWindow(modeIndex).NVI_ShowControlFontPanel(AWindow_ModePref::kFontControl_DefaultFont);
			break;
		}
		//ドキュメントのフォントを使う
	  case kMenuItemID_UseDocumentFont:
		{
			//ドキュメントに設定
			SPI_GetCurrentTabTextDocument().SPI_SetUseModeFont(false);
			//フォントパネルを表示
			AText	fontname;
			SPI_GetCurrentTabTextDocument().SPI_GetFontName(fontname);
			AFloatNumber	fontsize = SPI_GetCurrentTabTextDocument().SPI_GetFontSize();
			NVI_ShowFontPanel(kFontPanelVirtualControlID,fontname,fontsize);
			break;
		}
		//ジャンプリスト表示／非表示
	  case kMenuItemID_ShowJumpList:
		{
			SPI_ShowHideJumpListWindow(true);
			break;
		}
	  case kMenuItemID_Close:
		{
			//#513 win ウインドウを閉じた結果アプリ終了の可能性があるので、ここで保存
			GetApp().SPI_SaveReopenFile();
			//
			if( AKeyWrapper::IsOptionKeyPressed() == true )
			{
				GetApp().NVI_CloseAllWindow();
			}
			else
			{
				TryClose(NVI_GetCurrentTabIndex());
			}
			result = true;
			break;
		}
		//モード
	  case kMenuItemID_SetMode:
		{
			SPI_GetCurrentTabTextDocument().SPI_SetModeByRawName(inDynamicMenuActionText);
			break;
		}
		//ページ設定
	  case kMenuItemID_PageSetup:
		{
			SPI_GetCurrentTabTextDocument().NVI_PrintPageSetup(GetObjectID());
			//#902 SPI_GetCurrentTabTextDocument().NVI_SetDirty(true);
			break;
		}
		/*#524
		//印刷
	  case kMenuItemID_Print:
		{
			SPI_Print();
			break;
		}
		*/
		/*#844 プリントオプションは環境設定のみにする（ドキュメントごとの設定なし） 
		//印刷オプション
	  case kMenuItemID_ShowPrintOptionWindow:
		{
			SPI_ShowPrintOptionWindow();
			break;
		}
		*/
		/*#844
		//プロパティウインドウ
	  case kMenuItemID_ShowDocPropertyWindow:
		{
			SPI_ShowDocPropertyWindow();
			break;
		}
		*/
		//FTPエイリアス保存
	  case kMenuItemID_SaveFTPAlias:
		{
			SaveFTPAlias();
			break;
		}
		//
	  case kMenuItemID_CorrectEncoding:
		{
			if( SPI_GetCurrentTabTextDocument().NVI_IsDirty() == true )   break;
			ATextEncodingFallbackType	fallback = kTextEncodingFallbackType_None;//#473
			SPI_ShowTextEncodingCorrectWindow(true,fallback);
			break;
		}
		//プロパティウインドウ
	  case kMenuItemID_ShowHideCrossCaret:
		{
			/*#232
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
			{
				GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(viewIndex)).SPI_ShowHideCrossCaret();
			}
			*/
			SPI_ShowHideCrossCaret();
			break;
		}
		//
	  case kMenuItemID_FindEncodingProblem:
		{
			SPI_GetCurrentTabTextDocument().SPI_ReportTextEncodingErrors();
			break;
		}
		//R0199
	  case kMenuItemID_FindMisspell:
		{
			SPI_GetCurrentTabTextDocument().SPI_ReportSpellCheck();
			break;
		}
		//
	  case kMenuItemID_WindowToTab:
		{
			SPI_WindowToTab();//#389
			break;
		}
		//
	  case kMenuItemID_TabToWindow:
		{
			SPI_DetachTab(NVI_GetCurrentTabIndex());//#389
			break;
		}
		//
	  case kMenuItemID_JumpExportToText:
		{
			AText	text;
			const ATextArray&	textArray = GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetJumpMenuTextArray();
			for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
			{
				AText	t;
				textArray.Get(i,t);
				text.AddText(t);
				text.Add(kUChar_LineEnd);
			}
			ADocumentID docID = GetApp().SPNVI_CreateNewTextDocument(GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetModeIndex());
			if( docID == kObjectID_Invalid )   break;//win
			GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,text);
			break;
		}
		//
	  case kMenuItemID_SplitView:
		{
			SPI_SplitView(false);
			break;
		}
		//
	  case kMenuItemID_ConcatView:
		{
			SPI_ConcatSplitView();
			break;
		}
		//#212 サブペイン表示／非表示変更
	  case kMenuItemID_DisplaySubPane:
		{
			ShowHideLeftSideBar(!mLeftSideBarDisplayed);
			break;
		}
		//#291 右サイドバー表示／非表示変更
	  case kMenuItemID_DisplayInfoPane:
		{
			ShowHideRightSideBar(!mRightSideBarDisplayed);
			break;
		}
		//#725 サブテキストカラム表示・非表示変更
	  case kMenuItemID_DisplaySubTextPane:
		{
			ShowHideSubTextColumn(!mSubTextColumnDisplayed);
			break;
		}
		//#212
	  case kMenuItemID_SubTextPaneMenu:
		{
			AIndex	tabIndex;
			tabIndex = inDynamicMenuActionText.GetNumber();
			SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,false,false);
			break;
		}
		//#232
		//ツールボタン（フォルダ）の内容をクリックした
	  case kMenuItemID_ToolBar_Folder:
		{
			AControlID	focus = GetTopMostFocusTextViewControlID();
			if( focus != kControlID_Invalid )
			{
				AFileAcc	file;
				file.Specify(inDynamicMenuActionText);
				GetTextViewByControlID(focus).SPI_DoTool(file,inModifierKeys,false);
			}
			break;
		}
		//ツールボタンを編集する
	  case kMenuItemID_ToolBar_EditButton:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( toolButtonIndex != kIndex_Invalid )
			{
				switch( mTabDataArray.GetObjectConst(tabIndex).GetToolButtonType(toolButtonIndex) )
				{
				  case kToolbarItemType_File:
					{
						GetApp().GetAppPref().LaunchAppWithFile(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonFile(toolButtonIndex),0);
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
			}
			break;
		}
		//表示更新
	  case kMenuItemID_ToolBar_UpdateDisplay:
		{
			GetApp().SPI_RemakeToolButtonsAll(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex());
			break;
		}
		//ツールボタンを非表示にする
	  case kMenuItemID_ToolBar_InvalidButton:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( toolButtonIndex != kIndex_Invalid )
			{
				GetCurrentTabModePrefDB().SetEnableToolbarItem(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex),false);
			}
			break;
		}
		//ツールボタンを全て表示する
	  case kMenuItemID_ToolBar_DisplayAllButton:
		{
			GetCurrentTabModePrefDB().SetEnableToolbarItemAll();
			break;
		}
		//ツールボタンを削除
	  case kMenuItemID_ToolBar_DeleteButton:
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("TextWindow_DeleteToolButton1");
			mes2.SetLocalizedText("TextWindow_DeleteToolButton2");
			mes3.SetLocalizedText("TextWindow_DeleteToolButton3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kVirtualControlID_ToolRemoveOK);
			break;
		}
		
	  case kMenuItemID_CM_DetachTab_Left:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			SPI_DetachTab_Left(tabIndex);//#389
			break;
		}
	  case kMenuItemID_CM_DetachTab_Right:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			SPI_DetachTab_Right(tabIndex);//#389
			break;
		}
	  /* case kMenuItemID_CM_DetachTab_Bottom:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			ATextWindowFactory	factory(&(GetApp()));
			AObjectID	winID = GetApp().NVI_CreateWindow(factory);
			SPI_CopyTabToWindow(tabIndex,winID);
			SPI_CloseTab(tabIndex);
			//
			ARect	rect;
			GetApp().NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
			rect.top += (rect.bottom-rect.top)/2;
			GetApp().NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
			//
			NVI_GetWindowBounds(rect);
			rect.bottom -= (rect.bottom-rect.top)/2;
			NVI_SetWindowBounds(rect);
			break;
		}*/
	  case kMenuItemID_CM_CloseTab:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			SPI_CloseTab(tabIndex);
			break;
		}
	  case kMenuItemID_CM_DisplayInSubTextPane:
		{
			//#725 サブテキストはサイドバーに表示しないので、サイドバー表示処理はしない。 ShowHideLeftSideBar(true);
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			SPI_DisplayTabInSubText(tabIndex,true);
			break;
		}
#if IMPLEMENTATION_FOR_WINDOWS
		//開く
	  case kMenuItemID_Open:
		{
			AText	filter;
			GetApp().SPI_GetDefaultFileFilter(filter);
			NVI_ShowFileOpenWindow(filter,false);
			result = true;
			break;
		}
	  case kMenuItemID_OpenInvisible:
		{
			AText	filter;
			GetApp().SPI_GetDefaultFileFilter(filter);
			NVI_ShowFileOpenWindow(filter,true);
			result = true;
			break;
		}
#endif
		/*#725
		//インデックスウインドウ・次
	  case kMenuItemID_IndexNext:
		{
			SPI_IndexWindowNext();
			break;
		}
		//インデックスウインドウ・前
	  case kMenuItemID_IndexPrevious:
		{
			SPI_IndexWindowPrev();
			break;
		}*/
		//検索ボックスへのフォーカス移動 #137
	  case kMenuItemID_Find:
		{
			//環境設定で検索ボックスへのフォーカス移動がON、かつ、検索ボックス表示中、かつ、現在のフォーカスがtext viewの場合、検索ボックスへフォーカス移動
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindShortcutIsFocusSearchBox) == true &&
			   mToolbarExistArray.Find(kControlID_Toolbar_SearchField) != kIndex_Invalid && 
			   NVI_IsControlVisible(kControlID_Toolbar_SearchField) &&
			   IsTextViewControlID(NVI_GetCurrentFocus()) == true )
			{
				NVI_SwitchFocusTo(kControlID_Toolbar_SearchField);
				result = true;
				break;
			}
			/*#724
			AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
			if( NVI_GetCurrentFocus() != searchBoxControlID && searchBoxControlID != kControlID_Invalid &&
						GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplaySearchBox) == true &&
						GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFindShortcutIsFocusSearchBox) == true )
			{
				if( NVI_GetEditBoxView(searchBoxControlID).NVI_IsVisible() == true )//#383
				{
					NVI_GetEditBoxView(searchBoxControlID).NVI_SetSelectAll();
					NVI_SwitchFocusTo(searchBoxControlID);
					result = true;
				}
				else
				{
					result = false;
				}
			}
			else
			*/
			{
				//AAppで処理（検索ウインドウ表示）
				result = false;
			}
			break;
		}
		//次を検索 #137
	  case kMenuItemID_FindNext:
		{
			//最前面のtext viewで次を検索
			AControlID	focus = GetTopMostFocusTextViewControlID();
			if( focus != kControlID_Invalid )
			{
				GetTextViewByControlID(focus).SPI_FindNext();
				result = true;
			}
			/*#724
			AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
			//フォーカスが検索ボックスにあり、lastFocusedTextViewが有効な場合のみ実行
			if( NVI_GetCurrentFocus() == searchBoxControlID && searchBoxControlID != kControlID_Invalid &&
						lastFocusedTextView != kControlID_Invalid )
			{
				AFindParameter	param;
				GetApp().SPI_GetFindParam(param);
				GetTextViewByControlID(lastFocusedTextView).SPI_FindNext(param);
				SPI_GetTextDocument(lastFocusedTextView).SPI_SetFindHighlight(param);
				GetApp().SPI_GetFindWindow().SPI_AddRecentlyUsedFindText(param);
				result = true;
			}
			*/
			break;
		}
		//前を検索 #137
	  case kMenuItemID_FindPrevious:
		{
			//最前面のtext viewで前を検索
			AControlID	focus = GetTopMostFocusTextViewControlID();
			if( focus != kControlID_Invalid )
			{
				GetTextViewByControlID(focus).SPI_FindPrev();
				result = true;
			}
			/*#724
			AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
			AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
			//フォーカスが検索ボックスにあり、lastFocusedTextViewが有効な場合のみ実行
			if( NVI_GetCurrentFocus() == searchBoxControlID && searchBoxControlID != kControlID_Invalid &&
						lastFocusedTextView != kControlID_Invalid )
			{
				AFindParameter	param;
				GetApp().SPI_GetFindParam(param);
				GetTextViewByControlID(lastFocusedTextView).SPI_FindPrevious(param);
				SPI_GetTextDocument(lastFocusedTextView).SPI_SetFindHighlight(param);
				GetApp().SPI_GetFindWindow().SPI_AddRecentlyUsedFindText(param);
				result = true;
			}
			*/
			break;
		}
		//最初から検索 #137
	  case kMenuItemID_FindFromFirst:
		{
			//最前面のtext viewで最初から検索
			AControlID	focus = GetTopMostFocusTextViewControlID();
			if( focus != kControlID_Invalid )
			{
				GetTextViewByControlID(focus).SPI_FindFromFirst();
				result = true;
			}
			/*#724
			AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
			AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
			//フォーカスが検索ボックスにあり、lastFocusedTextViewが有効な場合のみ実行
			if( NVI_GetCurrentFocus() == searchBoxControlID && searchBoxControlID != kControlID_Invalid &&
						lastFocusedTextView != kControlID_Invalid )
			{
				AFindParameter	param;
				GetApp().SPI_GetFindParam(param);
				GetTextViewByControlID(lastFocusedTextView).SPI_FindFromFirst(param);
				SPI_GetTextDocument(lastFocusedTextView).SPI_SetFindHighlight(param);
				GetApp().SPI_GetFindWindow().SPI_AddRecentlyUsedFindText(param);
				result = true;
			}
			*/
			break;
		}
		//検索ハイライト消去 #137
	  case kMenuItemID_EraseFindHighlight:
		{
			SPI_GetCurrentFocusTextDocument().SPI_ClearFindHighlight();
			result = true;
			break;
		}
		//#357
	  case kMenuItemID_SwitchToNextTab:
		{
			SPI_SwitchTabNext();
			break;
		}
		//#357
	  case kMenuItemID_SwitchToPrevTab:
		{
			SPI_SwitchTabPrev();
			break;
		}
		//#390
	  case kMenuItemID_KeyRecordRecentlyPlay:
		{
			GetApp().SPI_SetKeyRecordedRawText(inDynamicMenuActionText);
			break;
		}
		/*#476
		//#404
	  case kMenuItemID_FullScreenMode:
		{
			SPI_SwitchFullScreenMode();
			break;
		}
		*/
		//#455
	  case kMenuItemID_SCMCommit:
		{
			SPI_ShowCommitWindow(NVI_GetCurrentDocumentID());
			break;
		}
		/*#725
		//#465
	  case kMenuItemID_CM_IndexView:
		{
			if( mFindResultViewID != kObjectID_Invalid )
			{
				AIndex	rowIndex = AView_Index::GetIndexViewByViewID(mFindResultViewID).SPI_GetContextMenuSelectedRowDBIndex();
				AView_Index::GetIndexViewByViewID(mFindResultViewID).SPI_ExportToNewDocument(rowIndex);
			}
			break;
		}
		*/
		//#454
	  case kMenuItemID_JumpListHistory:
		{
			//docpath+改行+idtextの形でactiontextに入っているので取り出す
			AText	docpath, headertext;
			AIndex	pos = 0;
			for( ; pos < inDynamicMenuActionText.GetItemCount(); pos++ )
			{
				if( inDynamicMenuActionText.Get(pos) == kUChar_LineEnd )   break;
			}
			inDynamicMenuActionText.GetText(0,pos,docpath);
			inDynamicMenuActionText.GetText(pos+1,inDynamicMenuActionText.GetItemCount()-(pos+1),headertext);
			//ドキュメントを見つける
			AFileAcc	file;
			file.Specify(docpath);
			ADocumentID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file);
			if( docID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_RevealFromJumpMenuText(headertext);
			}
			break;
		}
		/*#899
		//#379
	  case kMenuItemID_SubPaneMode_Manual:
		{
			SetSubPaneMode(kSubPaneMode_Manual);
			break;
		}
	  case kMenuItemID_SubPaneMode_PrevTab:
		{
			SetSubPaneMode(kSubPaneMode_PreviousDocument);
			break;
		}
	  case kMenuItemID_SubPaneMode_SameTab:
		{
			SetSubPaneMode(kSubPaneMode_SameDocument);
			break;
		}
		*/
		//#619
	  case kMenuItemID_AddToolbarButton:
		{
			SPI_ShowAddToolButtonWindow();
			break;
		}
		/*#844
	  case kMenuItemID_HideAllModeCreatorToolbarButton:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			GetApp().SPI_GetModePrefDB(modeIndex).SetEnableToolbarItemAll(false,true,false);
			break;
		}
		*/
	  case kMenuItemID_HideAllUserToolbarButton:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			GetApp().SPI_GetModePrefDB(modeIndex).SetEnableToolbarItemAll(false);//#844 ,false,true);
			break;
		}
		//マクロをFinder上に表示
	  case kMenuItemID_RevealMacroWithFinder:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			AFileAcc	folder;
			GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarFolder(folder);
			ALaunchWrapper::Reveal(folder);
			break;
		}
		//Finderでのマクロ構成変更を反映
	  case kMenuItemID_ReconfigMacro:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			GetApp().SPI_GetModePrefDB(modeIndex).LoadToolbar();
			break;
		}
		//マクロ　右クリックした項目以降を全て非表示にする
	  case kMenuItemID_HideMacrosAfterThis:
		{
			AIndex	tabIndex = NVI_GetCurrentTabIndex();
			AIndex	clickedMacroDisplayIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(NVI_GetLastClickedButtonControlID());
			if( clickedMacroDisplayIndex != kIndex_Invalid )
			{
				//後ろから非表示にしていく（SetEnableToolbarItem()からSPI_RemakeToolButtonsAll()がコールされるので、
				//SetEnableToolbarItem()をコールした時点でmTabDataArrayからデータが無くなるため。）
				AItemCount	displayedMacroCount = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonCount();
				for( AIndex displayIndex = displayedMacroCount-1; displayIndex >= clickedMacroDisplayIndex; displayIndex-- )
				{
					AIndex	dbIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(displayIndex);
					GetCurrentTabModePrefDB().SetEnableToolbarItem(dbIndex,false);
				}
			}
			break;
		}
		//マクロ　右クリックした項目以降を全て削除（確認ダイアログ表示）
	  case kMenuItemID_DeleteMacrosAfterThis:
		{
			AText	mes1, mes2, mes3;
			mes1.SetLocalizedText("TextWindow_DeleteToolButton1");
			mes2.SetLocalizedText("TextWindow_DeleteToolButton2");
			mes3.SetLocalizedText("TextWindow_DeleteToolButton3");
			NVI_ShowChildWindow_OKCancel(mes1,mes2,mes3,kVirtualControlID_DeleteMacrosAfterThis);
			break;
		}
		//タブ　右クリックした項目以降を閉じる
	  case kMenuItemID_CloseTabsAfterThis:
		{
			AArray<ADocumentID>	docIDArray;
			//クリックしたタブ取得
			AIndex	clickedTabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			AIndex	clickedTabDisplayIndex = GetTabSelectorConst().SPI_GetDisplayTabIndex(clickedTabIndex);
			//閉じるべきタブのdocument idを取得
			AItemCount	tabDisplayCount = GetTabSelectorConst().SPI_GetDisplayTabCount();
			for( AIndex tabDisplayIndex = tabDisplayCount-1 ; tabDisplayIndex >= clickedTabDisplayIndex; tabDisplayIndex-- )
			{
				AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(tabDisplayIndex);
				docIDArray.Add(NVI_GetDocumentIDByTabIndex(tabIndex));
			}
			//documentに対応するタブを閉じる（dirtyでないもののみ）
			for( AIndex i = 0; i < docIDArray.GetItemCount(); i++ )
			{
				ADocumentID	docID = docIDArray.Get(i);
				if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsDirty() == false )
				{
					TryClose(NVI_GetTabIndexByDocumentID(docID));
				}
			}
			break;
		}
		//現在のドキュメントのモード設定
	  case kMenuItemID_CurrentModePref:
		{
			AModeIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
			GetApp().SPI_ShowModePrefWindow(modeIndex);
			break;
		}
		//フラグのないタブを全て閉じる
	  case kMenuItemID_CloseAllTabWithoutFlag:
		{
			//全てのdocument idを取得（z-orderの後ろ側から順に）
			AArray<ADocumentID>	docIDArray;
			for( AIndex zorder = NVI_GetSelectableTabCount()-1; zorder >= 0; zorder-- )
			{
				//タブの表示index取得
				AIndex	tabIndex = NVI_GetTabIndexByZOrderIndex(zorder);
				//doc id取得
				docIDArray.Add(NVI_GetDocumentIDByTabIndex(tabIndex));
			}
			//上で取得した順にタブを閉じる（dirtyでないもの、かつ、フラグの無いもののみ）
			for( AIndex i = 0; i < docIDArray.GetItemCount(); i++ )
			{
				ADocumentID	docID = docIDArray.Get(i);
				if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsDirty() == false && 
							GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDocumentFlag() == false )
				{
					TryClose(NVI_GetTabIndexByDocumentID(docID));
				}
			}
			break;
		}
		//タブにフラグ設定／設定解除
	  case kMenuItemID_SetDocumentFlag:
		{
			AIndex	tabIndex = GetTabSelector().SPI_GetContextMenuSelectedTabIndex();
			ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
			GetApp().SPI_GetTextDocumentByID(docID).
					SPI_SetDocumentFlag(!GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDocumentFlag());
			break;
		}
		/*drop
		//#737
	  case kMenuItemID_CompareLeftRightSelected:
		{
			SPI_CompareWithSubText(true);
			break;
		}
		*/
		//フローティングウインドウを全て一時的に非表示
	  case kMenuItemID_ShowHideAllFloatingWindow:
		{
			GetApp().SPI_ShowHideFloatingSubWindowsTemporary();
			//全てのウインドウのツールバー値更新
			GetApp().SPI_UpdateToolbarItemValueAll();
			break;
		}
		//ポップアップ禁止
	  case kMenuItemID_InhibitPopup:
		{
			//ポップアップ禁止状態取得、設定
			GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kProhibitPopup,
													 !GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup));
			//禁止にしたときは、現在表示中のポップアップを全て閉じる
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup) == true )
			{
				SPI_HideAllPopupWindows();
			}
			//全てのウインドウのツールバー値更新
			GetApp().SPI_UpdateToolbarItemValueAll();
			break;
		}
		//#725
		//右サイドバー項目追加
	  case kMenuItemID_AddRecentlyOpenFileListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_FileList,0,200);
			break;
		}
	  case kMenuItemID_AddSameFolderFileListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_FileList,1,200);
			break;
		}
	  case kMenuItemID_AddSameProjectFileListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_FileList,2,200);
			break;
		}
	  case kMenuItemID_AddRemoteFileListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_FileList,3,200);
			break;
		}
	  case kMenuItemID_AddJumpListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_JumpList,0,300);
			break;
		}
	  case kMenuItemID_AddDocInfoInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_DocInfo,0,80);
			break;
		}
	  case kMenuItemID_AddTextMarkerInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_TextMarker,0,120);
			break;
		}
	  case kMenuItemID_AddCandidateListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_CandidateList,0,150);
			break;
		}
	  case kMenuItemID_AddToolListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_KeyToolList,0,150);
			break;
		}
	  case kMenuItemID_AddIdInfoListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_IdInfo,0,250);
			break;
		}
	  case kMenuItemID_AddCallGrefInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_CallGraf,0,300);
			break;
		}
	  case kMenuItemID_AddPreviewInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_Previewer,0,400);
			break;
		}
	  case kMenuItemID_AddWordsListInRightSideBar:
		{
			AddNewSubWindowInRightSideBar(kSubWindowType_WordsList,0,300);
			break;
		}
	  case kMenuItemID_AddKeyRecordInRightSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_KeyRecord,0,150);
			break;
		}
		//#725
		//左サイドバー項目追加
	  case kMenuItemID_AddRecentlyOpenFileListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_FileList,0,200);
			break;
		}
	  case kMenuItemID_AddSameFolderFileListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_FileList,1,200);
			break;
		}
	  case kMenuItemID_AddSameProjectFileListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_FileList,2,200);
			break;
		}
	  case kMenuItemID_AddRemoteFileListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_FileList,3,200);
			break;
		}
	  case kMenuItemID_AddJumpListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_JumpList,0,300);
			break;
		}
	  case kMenuItemID_AddDocInfoInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_DocInfo,0,80);
			break;
		}
	  case kMenuItemID_AddTextMarkerInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_TextMarker,0,120);
			break;
		}
	  case kMenuItemID_AddCandidateListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_CandidateList,0,150);
			break;
		}
	  case kMenuItemID_AddToolListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_KeyToolList,0,150);
			break;
		}
	  case kMenuItemID_AddIdInfoListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_IdInfo,0,250);
			break;
		}
	  case kMenuItemID_AddCallGrefInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_CallGraf,0,300);
			break;
		}
	  case kMenuItemID_AddPreviewInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_Previewer,0,400);
			break;
		}
	  case kMenuItemID_AddWordsListInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_WordsList,0,300);
			break;
		}
	  case kMenuItemID_AddKeyRecordInLeftSideBar:
		{
			AddNewSubWindowInLeftSideBar(kSubWindowType_KeyRecord,0,150);
			break;
		}
		//#688
		//ツールバー表示非表示
	  case kMenuItemID_ShowHideToolbar:
		{
			NVI_ShowHideToolbar();
			break;
		}
		//ツールバーカスタマイズ
	  case kMenuItemID_CustomizeToolbar:
		{
			NVI_ShowToolbarCustomize();
			break;
		}
		//#793
		//左のファイルと右のファイルを比較
	  case kMenuItemID_CompareLeftFileAndRightFile:
		{
			ADocumentID	mainDocID = NVI_GetCurrentDocumentID();
			ADocumentID	subDocID = SPI_GetCurrentSubTextPaneDocumentID();
			if( mainDocID != kObjectID_Invalid && subDocID != kObjectID_Invalid && mainDocID != subDocID )
			{
				AFileAcc	subFile;
				GetApp().SPI_GetTextDocumentByID(subDocID).NVI_GetFile(subFile);
				if( subFile.IsSpecified() == true )
				{
					//メインtext viewのドキュメントにて、サブテキストのファイルとの比較を実行
					GetApp().SPI_GetTextDocumentByID(mainDocID).SPI_SetDiffMode_File(subFile,kCompareMode_File);
					SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
		//#688
	  case kMenuItemID_Zoom:
		{
			NVI_Zoom();
			break;
		}
		//#1062
		//タブをアルファベット順にソートする
	  case kMenuItemID_SortTabs:
		{
			SortTabs();
			break;
		}
		//しまう #1194
	  case kMenuItemID_Minimize:
		{
			NVI_CollapseWindow(true);
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

//
void	AWindow_Text::EVTDO_UpdateMenu()
{
	if( NVI_GetTabCount() == 0 )   return;//#291
	/*B0411 B0410
	if( SPI_GetAskingSaveChanges() == true ) 
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,false);
		return;
	}
	if( NVI_IsPrintMode() == true )   return;
	*/
	//fprintf(stderr,"UpdateMenu() ");
	/*B0411
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,(NVI_ChildWindowVisible()==false));
	*/
	//★
	/*#725
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowJumpList,//#291 true);
				(SPI_GetJumpListWindow().NVI_GetOverlayMode()!=true));
		AText	text;
		//#291 if( SPI_GetJumpListWindow().NVI_IsWindowVisible() == true )
		if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetJumpListVisible() == true )//#291
		{
			text.SetLocalizedText("JumpList_Hide");
		}
		else
		{
			text.SetLocalizedText("JumpList_Show");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowJumpList,text);
	}
	*/
	//行移動
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MoveLine,true);
	//しまう #1194
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Minimize,true);
	//フォントサイズ
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSizeOther,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize60,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize80,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize90,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize95,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize100,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize105,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize110,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize120,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize140,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize160,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize180,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontSize240,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_RevertFontSize,true);
	AFloatNumber	fontsize = SPI_GetCurrentTabTextDocument().SPI_GetFontSize();
	//win GetApp().NVI_GetMenuManager().CheckSameNumberMenuItem(kMenuItemID_FontSize,fontsize);
	AArray<AMenuItemID>	itemIDArray;
	itemIDArray.Add(kMenuItemID_FontSize);
	itemIDArray.Add(kMenuItemID_FontSize60);
	itemIDArray.Add(kMenuItemID_FontSize80);
	itemIDArray.Add(kMenuItemID_FontSize90);
	itemIDArray.Add(kMenuItemID_FontSize95);
	itemIDArray.Add(kMenuItemID_FontSize100);
	itemIDArray.Add(kMenuItemID_FontSize105);
	itemIDArray.Add(kMenuItemID_FontSize110);
	itemIDArray.Add(kMenuItemID_FontSize120);
	itemIDArray.Add(kMenuItemID_FontSize140);
	itemIDArray.Add(kMenuItemID_FontSize160);
	itemIDArray.Add(kMenuItemID_FontSize180);
	itemIDArray.Add(kMenuItemID_FontSize240);
	GetApp().NVI_GetMenuManager().CheckSameNumberMenuItem(itemIDArray,fontsize);
	//フォント
	//#688 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Font,true);
	//#688 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FontRootMenu,true);
	//#688 AText	font;
	//win font.SetFormattedCstring("%d",SPI_GetCurrentTextDocument().SPI_GetFont());
	//#688 SPI_GetCurrentTabTextDocument().SPI_GetFontName(font);//win
	//#688 GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_Font,font);
	//TextEncoding
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetTextEncoding,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetTextEncodingRootMenu,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	AText	tecname;
	SPI_GetCurrentTabTextDocument().SPI_GetTextEncoding(tecname);
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_SetTextEncoding,tecname);
	//改行コード
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReturnCode_CR,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReturnCode_CRLF,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReturnCode_LF,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReturnCode_RootMenu,(SPI_GetCurrentTabTextDocument().NVI_IsReadOnly()==false));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_ReturnCode_CR,(SPI_GetCurrentTabTextDocument().SPI_GetReturnCode()==returnCode_CR));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_ReturnCode_CRLF,(SPI_GetCurrentTabTextDocument().SPI_GetReturnCode()==returnCode_CRLF));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_ReturnCode_LF,(SPI_GetCurrentTabTextDocument().SPI_GetReturnCode()==returnCode_LF));
	//行送り
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_NoWrap,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_WordWrap,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_WordWrapByLetterCount,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_LetterWrap,true);//#1113
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_LetterWrapByLetterCount,true);//#1113
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WrapMode_RootMenu,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_NoWrap,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_NoWrap));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_WordWrap,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_WordWrap));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_WordWrapByLetterCount,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_WordWrapByLetterCount));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_LetterWrap,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_LetterWrap));//#1113
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_WrapMode_LetterWrapByLetterCount,(SPI_GetCurrentTabTextDocument().SPI_GetWrapMode()==kWrapMode_LetterWrapByLetterCount));//#1113
	//フォント #868
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_UseModeFont,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_UseDocumentFont,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_UseModeFont,(SPI_GetCurrentTabTextDocument().SPI_GetUseModeFont()==true));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_UseDocumentFont,(SPI_GetCurrentTabTextDocument().SPI_GetUseModeFont()==false));
	//モード
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetMode,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetModeRootMenu,true);
	AText	modename;
	SPI_GetCurrentTabTextDocument().SPI_GetModeRawName(modename);
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_SetMode,modename);
	//ページ設定
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_PageSetup,true);
	//印刷
	//#524 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Print,true);
	//印刷オプション
	//#844 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowPrintOptionWindow,true);
	//プロパティウインドウ
	//#844 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowDocPropertyWindow,true);
	//FTPエイリアス保存
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SaveFTPAlias,(SPI_GetCurrentTabTextDocument().SPI_IsRemoteFileMode()==true));
	//テキストエンコーディング修正
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CorrectEncoding,(SPI_GetCurrentTabTextDocument().NVI_IsDirty()==false));
	//CrossCaret
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowHideCrossCaret,true);
		AText	text;
		if( IsCrossCaretMode() == true )
		{
			text.SetLocalizedText("CrossCaret_Hide");
		}
		else
		{
			text.SetLocalizedText("CrossCaret_Show");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowHideCrossCaret,text);
	}
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindEncodingProblem,true);
	//R0199
#if IMPLEMENTATION_FOR_WINDOWS
	//スペルチェック未対応
#else
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindMisspell,true);
#endif
	//
	//B0411 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WindowToTab,GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow));
	//B0411 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TabToWindow,GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow));
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_JumpExportToText,true);
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MoveToDefinitionRootMenu,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MoveToDefinition,true);
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SplitView,true);
	//#212
	AText	text;
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_DisplaySubPane,GetApp().SPI_IsSupportPaneMode());
		if( mLeftSideBarDisplayed == true )
		{
			text.SetLocalizedText("Menu_SubPane_Hide");
		}
		else
		{
			text.SetLocalizedText("Menu_SubPane_Show");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_DisplaySubPane,text);
	}
	//#291
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_DisplayInfoPane,GetApp().SPI_IsSupportPaneMode());
	{
		if( mRightSideBarDisplayed == true )
		{
			text.SetLocalizedText("Menu_InfoPane_Hide");
		}
		else
		{
			text.SetLocalizedText("Menu_InfoPane_Show");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_DisplayInfoPane,text);
	}
	//
	ABool	b = false;
	/*#137 すでにB0187の変更で意味無い処理なのでコメントアウト AControlID	focus = NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
	*B0187 AIndex	viewIndex =* mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus);*/
	if( /*B0187 viewIndex > 0*/ mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetViewCount() >= 2 )
	{
		b = true;
	}
	//#137}
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ConcatView,b);
	
#if IMPLEMENTATION_FOR_WINDOWS
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Open,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenInvisible,true);
#endif
	
	//ウインドウをタブ化（最もタブ数が多いウインドウへタブ化する）
	b = true;
	/*#850
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )   b = false;
	else
	*/
	{
		//#850 if( SPI_IsTabModeMainWindow() == true )   b = false;
		if( GetApp().SPI_GetMostNumberTabsTextWindowID() == GetObjectID() )
		{
			//自分が「最もタブ数が多いウインドウ」の場合はメニューdisable
			b = false;
		}
		else
		{
			if( NVI_IsChildWindowVisible() == true )   b = false;//B0411
			//B0411 タブ化する先のウインドウが子ウインドウ表示中はメニューdisable
			AWindowID	mainWindowID = GetApp().SPI_GetMostNumberTabsTextWindowID();//#850 SPI_GetTabModeMainTextWindowID();
			if( mainWindowID != kObjectID_Invalid )
			{
				if( GetApp().SPI_GetTextWindowByID(mainWindowID).NVI_IsChildWindowVisible() == true )   b = false;
			}
		}
	}
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_WindowToTab,b);
	//タブをウインドウ化
	b = true;
	/*#850 
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == false )   b = false;
	else
	*/
	{
		//#850 if( SPI_IsTabModeMainWindow() == false )   b = false;
		if( NVI_IsChildWindowVisible() == true )   b = false;//B0411
		if( NVI_GetSelectableTabCount() < 2 )   b = false;//#850
	}
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TabToWindow,b);
	/*
	//#92
	if( (mFindResultDocumentID!=kObjectID_Invalid) == true )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IndexNext,true);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_IndexPrevious,true);
	}
	*/
	/*#724
	//#137
	AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( NVI_GetCurrentFocus() == searchBoxControlID && searchBoxControlID != kControlID_Invalid )
	*/
	if( GetTopMostFocusTextViewControlID() != kControlID_Invalid )
	{
		AFindParameter	param;
		GetApp().SPI_GetFindParam(param);
		b = (param.findText.GetItemCount()>0);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindNext,b);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindPrevious,b);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindFromFirst,b);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_EraseFindHighlight,SPI_GetCurrentFocusTextDocument().SPI_IsFindHighlightSet());
	}
	//#357
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchToNextTab,(NVI_GetSelectableTabCount()>=2));//#379
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchToPrevTab,(NVI_GetSelectableTabCount()>=2));//#379
	//#404
	//#476 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FullScreenMode,SPI_IsTabModeMainWindow());
	//#455
	ABool	canCommit = GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_CanCommit();
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMCommit,canCommit);
	//#379
	/*#899
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SubPaneMode_Manual,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SubPaneMode_PrevTab,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SubPaneMode_SameTab,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SubPaneMode_Manual,(mSubPaneMode==kSubPaneMode_Manual));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SubPaneMode_PrevTab,(mSubPaneMode==kSubPaneMode_PreviousDocument));
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SubPaneMode_SameTab,(mSubPaneMode==kSubPaneMode_SameDocument));
	*/
	//#725
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_DisplaySubTextPane,true);
	//#688
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowHideToolbar,true);
	if( NVI_IsToolBarShown() == false )
	{
		text.SetLocalizedText("ShowToolBar");
	}
	else
	{
		text.SetLocalizedText("HideToolBar");
	}
	GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_ShowHideToolbar,text);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CustomizeToolbar,true);
	//#793
	//左右ファイル比較
	ADocumentID	mainDocID = NVI_GetCurrentDocumentID();
	ADocumentID	subDocID = SPI_GetCurrentSubTextPaneDocumentID();
	if( mainDocID != kObjectID_Invalid && subDocID != kObjectID_Invalid && mainDocID != subDocID )
	{
		//メイン・サブ両方行計算完了かどうかを取得
		ABool	wrapCalculateCompleted = ((GetApp().SPI_GetTextDocumentByID(mainDocID).SPI_IsWrapCalculating()==false)&&
										  (GetApp().SPI_GetTextDocumentByID(subDocID).SPI_IsWrapCalculating()==false));
		//両方行計算完了なら左右比較enable
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareLeftFileAndRightFile,wrapCalculateCompleted);
	}
	
	//#725 各サイドバー
	for( AMenuItemID i = kMenuItemID_AddRecentlyOpenFileListInRightSideBar; i <= kMenuItemID_AddKeyRecordInRightSideBar; i++ )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(i,true);
	}
	for( AMenuItemID i = kMenuItemID_AddRecentlyOpenFileListInLeftSideBar; i <= kMenuItemID_AddKeyRecordInLeftSideBar; i++ )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(i,true);
	}
	
	//拡大／縮小
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Zoom,true);
	//フローティングウインドウ一時的非表示、ポップアップ禁止
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShowHideAllFloatingWindow,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_InhibitPopup,true);
	
	//==================ツールバーenable/disable==================
	//キー記録・再生
	if( mToolbarExistArray.Find(kControlID_Toolbar_KeyRecordStart) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_KeyRecordStart,(GetApp().SPI_IsKeyRecording()==false));
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_KeyRecordStop) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_KeyRecordStop,(GetApp().SPI_IsKeyRecording()==true));
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_KeyRecordPlay) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_KeyRecordPlay,((GetApp().SPI_IsKeyRecording()==false)&&
						(GetApp().SPI_ExistRecordedText()==true)));
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_AddMacroBar) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_AddMacroBar,((GetApp().SPI_IsKeyRecording()==false)&&
						(GetApp().SPI_ExistRecordedText()==true)));
	}
	//テキストマーカー
	if( mToolbarExistArray.Find(kControlID_Toolbar_FindHighlight) != kIndex_Invalid )
	{
		NVI_SetControlNumber(kControlID_Toolbar_FindHighlight,GetApp().SPI_GetCurrentTextMarkerGroupIndex());
	}
	//Navボタン
	if( mToolbarExistArray.Find(kControlID_Toolbar_GoPrevious) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_GoPrevious,GetApp().SPI_GetCanNavigatePrev());
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_GoNext) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_GoNext,GetApp().SPI_GetCanNavigateNext());
	}
	//commit
	if( mToolbarExistArray.Find(kControlID_Toolbar_Commit) != kIndex_Invalid )
	{
		NVI_SetControlEnable(kControlID_Toolbar_Commit,
							 ((GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).NVI_IsDirty()==false)&&canCommit));
	}
	//#1062
	//タブをアルファベット順にソートする
	if( GetTabSelectorConst().SPI_GetDisplayTabCount() >= 2 )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SortTabs,true);
	}
}

//ウインドウの閉じるボタン
void	AWindow_Text::EVTDO_DoCloseButton()
{
	//#513 win  ウインドウを閉じた結果アプリ終了の可能性があるので、ここで保存
	GetApp().SPI_SaveReopenFile();
	//
	/*
	if( AKeyWrapper::IsOptionKeyPressed() == true 
#if IMPLEMENTATION_FOR_WINDOWS
				|| AKeyWrapper::IsControlKeyPressed() == true 
#endif
				)
	{
		GetApp().NVI_CloseAllWindow();
	}
	else
	*/
	{
		//#510
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCloseAllTabsWhenCloseWindow) == true ||
			(AKeyWrapper::IsOptionKeyPressed() == true || AKeyWrapper::IsControlKeyPressed() == true) )
		{
			//クローズボタンで全てのタブを閉じる設定
			//NVI_TryCloseAllTabs(false,false);//#667 closingAllWindows=falseとすることで、保存しないで閉じる場合に他のウインドウが閉じていた問題を対策する。
			if( NVI_AskSaveForAllTabs() == true )
			{
				//全てのタブで一度もキャンセルされなかった場合のみ、クローズ実行。（キャンセルされたら１つも閉じない）
				NVI_ExecuteClose();
			}
		}
		else
		{
			//クローズボタンでタブ１つ１つを閉じる設定
			TryClose(NVI_GetCurrentTabIndex());
		}
	}
}

/**
AskSaveChangesウインドウの応答受信処理
個別クローズの場合（＝モーダルではない場合）のみコールされる
*/
void	AWindow_Text::EVTDO_AskSaveChangesReply( const AObjectID inDocumentID, const AAskSaveChangesResult inAskSaveChangesReply )
{
	switch(inAskSaveChangesReply)
	{
		//キャンセル
	  case kAskSaveChangesResult_Cancel:
		{
			//処理無し
			break;
		}
		//保存しない
	  case kAskSaveChangesResult_DontSave:
		{
			ExecuteClose(inDocumentID);
			break;
		}
		//保存
	  case kAskSaveChangesResult_Save:
		{
			//B0325
			if( GetApp().SPI_GetTextDocumentByID(inDocumentID).NVI_IsFileSpecified() == false && 
						GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_IsRemoteFileMode() == false )
			{
				GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_Save(false);
			}
			else
			{
				GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_Save(false);
				ExecuteClose(inDocumentID);
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	mAskingSaveChanges = false;
}

//保存ダイアログ応答受信処理
void	AWindow_Text::EVTDO_SaveWindowReply( const AObjectID inDocumentID, const AFileAcc& inFile, const AText& inRefText )
{
	if( inRefText.Compare("FTPAlias") == true )
	{
		AText	text;
		GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_GetRemoteFileURL(text);
		inFile.WriteFile(text);
		AFileAttribute	attr;
		//win attr.creator = 'MMKE';
		//win attr.type = 'ftpa';
		//win AFileWrapper::SetFileAttribute(inFile,attr);
		GetApp().SPI_GetFTPAliasFileAttribute(attr);
		inFile.SetFileAttribute(attr);
	}
	//タブセット #1050
	else if( inRefText.Compare("tabset") == true )
	{
		//タブごとのファイルパスを改行区切りで取得
		AText	text;
		AItemCount	tabDisplayCount = GetTabSelectorConst().SPI_GetDisplayTabCount();
		for( AIndex tabDisplayIndex = 0; tabDisplayIndex < tabDisplayCount; tabDisplayIndex++ )
		{
			AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(tabDisplayIndex);
			ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
			AText	path;
			if( GetApp().SPI_GetTextDocumentByID(docID).SPI_IsRemoteFileMode() == true )
			{
				//リモートファイルならURL取得
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetRemoteFileURL(path);
			}
			else if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsFileSpecified() == true )
			{
				//ローカルファイル（specify済み）ならファイルパス取得
				GetApp().SPI_GetTextDocumentByID(docID).NVI_GetFilePath(path);
			}
			if( path.GetItemCount() > 0 )
			{
				text.AddText(path);
				text.Add(kUChar_LF);
			}
		}
		//ファイル保存
		inFile.WriteFile(text);
	}
	//通常ドキュメント
	else
	{
		GetApp().SPI_GetTextDocumentByID(inDocumentID).NVI_SpecifyFile(inFile);
		GetApp().SPI_AddToRecentlyOpenedFile(inFile);//#376
		GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_Save(false);
		//#376 GetApp().SPI_AddToRecentlyOpenedFile(inFile);
	}
}

/**
リサイズ完了時処理
*/
void	AWindow_Text::EVTDO_WindowResizeCompleted( const ABool inResized )
{
	//#688
	//実際にサイズに変更がなければ、何もせず終了（たとえばサイズボックスをクリックしただけの場合などは、inResizedはfalseになる）
	if( inResized == false )
	{
		return;
	}
	//#506
	NVI_SetFullScreenMode(false);
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFullScreenMode,false);
	//
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{      
		//各タブのdocumentの現在のメインtext view幅を取得
		ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
		ANumber	oldTextViewWidth = SPI_GetTextViewWidth(docID);
		//#724 UpdateViewBounds()内で、マクロバーの位置を再配置するために、位置データを削除する
		mTabDataArray.GetObject(tabIndex).GetToolButtonPositionArray().DeleteAll();
		//Viewの位置、サイズ更新
		UpdateViewBounds(tabIndex);
		//行折り返し再計算
		//if( oldTextViewWidth != SPI_GetTextViewWidth(docID) )
		{
			//行情報再計算（ウインドウ幅変わるので）
			//Diff対象ドキュメントは行情報再計算しない（ただ、サブペインの幅は決まっているが、サブペイン幅に従わない設定の時に、この判定は少し問題がある）
			if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_IsDiffTargetDocument() == false )//#379
			{
				//行折り返しありの場合に再計算
				if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetWrapMode() != kWrapMode_NoWrap )
				{
					GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
				}
			}
		}
		//
		UpdateTextDrawProperty(tabIndex);
		//#212 tab毎にする必要はないので、下へ移動NVI_RefreshWindow();
	}
	NVI_RefreshWindow();//#212
	//B0415にてNVI_SetWindowBounds()でEVT_WindowBoundsChanged()がコールされるように変更したため、
	//kMenuItemID_CM_DetachTab_Left実行時に、NVI_SetWindowBounds()がコールされたとき、ここに来るようになり、
	//ウインドウがactiveでないときに、NVI_SelectTabをコールするようになってしまった→その結果、ジャンプメニューが表示されてしまう。
	//そもそも、ここでNVI_SelectTabをコールする必要ないと思われるのでコメントアウト
	//B0415 NVI_SelectTab(NVI_GetCurrentTabIndex(),true);
}

//#240
/**
ウインドウSelect実行直前にコールされる
*/
void	AWindow_Text::EVTDO_DoWindowBeforeSelected()
{
	//選択されるWindowに対応するJumpList以外をHideにする
	//Acitiveになったときに他のWindowのJumpListが一瞬だけまだ表示された状態になってうっとうしいため
	//#725 GetApp().SPI_HideOtherJumpList(SPI_GetJumpListWindow().GetObjectID());
	/*
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_HideOtherFloatingJumpListWindows(mFloatingJumpListWindowID);
	}
	*/
	UpdateFloatingJumpListDisplay();
}

void	AWindow_Text::SPI_ShowMoveLineWindow()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		ATextPoint	spt, ept;
		GetTextViewByControlID(focus).SPI_GetSelect(spt,ept);
		ABool	paraMode = true;//常に段落モードにする。折り返し行の行番号指定で移動することはないため。(SPI_GetTextDocument(focus).SPI_GetWrapMode() != kWrapMode_NoWrap);
		AIndex	lineIndex = spt.y;
		AItemCount	lineCount = SPI_GetTextDocument(focus).SPI_GetLineCount();
		if( paraMode == true )
		{
			lineIndex = SPI_GetTextDocument(focus).SPI_GetParagraphIndexByLineIndex(spt.y);
			lineCount = SPI_GetTextDocument(focus).SPI_GetParagraphCount();
		}
		/*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().NVI_Create(kObjectID_Invalid);
		/*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().SPNVI_Show(lineIndex,lineCount,paraMode);
		/*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().NVI_SwitchFocusToFirst();
	}
}

ABool	AWindow_Text::IsCrossCaretMode() const
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		return GetTextViewConstByControlID(focus).SPI_IsCrossCaretMode();
	}
	return false;
}

#if IMPLEMENTATION_FOR_MACOSX
//B0370
ABool	AWindow_Text::EVTDO_DoServiceGetTypes( AArray<AScrapType>& outCopyTypes, AArray<AScrapType>& outPasteTypes )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		if( GetTextViewConstByControlID(focus).SPI_IsCaretMode() == false )
		{
			outCopyTypes.Add(kScrapType_UnicodeText);
			//#688 outCopyTypes.Add(kScrapType_Text);
		}
		if( SPI_GetTextDocument(focus).NVI_IsReadOnly() == false )
		{
			outPasteTypes.Add(kScrapType_UnicodeText);
			//#688 outPasteTypes.Add(kScrapType_Text);
		}
	}
	return true;
}

//B0370
ABool	AWindow_Text::EVTDO_DoServiceCopy( const AScrapRef inScrapRef )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		//テキストが大きすぎると、メニュー選択時等にスローオペレーションになるので、この行数以上の場合は、空のテキストを返す。#695
		ATextPoint	spt = {0,0}, ept = {0,0};//#695
		GetTextViewConstByControlID(focus).SPI_GetSelect(spt,ept);//#695
		if( ept.y - spt.y > kLimit_ServiceCopy_MaxLineCount )   return true;//#695 10万行より多い場合はメニュー選択でスローになるのを防ぐためservice copyできないようにする
		//現在選択中のテキストを返す
		AText	text;
		GetTextViewConstByControlID(focus).SPI_GetSelectedText(text);
		AScrapWrapper::SetTextScrap(inScrapRef,text,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp));//#688 ,SPI_GetTextDocument(focus).SPI_GetPreferLegacyTextEncoding());//win
	}
	return true;
}

//B0370
ABool	AWindow_Text::EVTDO_DoServicePaste( const AScrapRef inScrapRef )
{
	//#212 下へif( SPI_GetCurrentTextDocument().NVI_IsReadOnly() == true )   return false;
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		if( SPI_GetTextDocument(focus).NVI_IsReadOnly() == true )   return false;
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		GetTextViewByControlID(focus).SPI_ServicePaste(inScrapRef);
	}
	return true;
}
#endif

//#137
/**
EditBoxでReturnキーを押したときの処理
*/
ABool	AWindow_Text::NVIDO_ViewReturnKeyPressed( const AControlID inControlID )//#135
{
	//★
	/*#724
	//検索ボックス
	AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( inControlID == searchBoxControlID )
	{
		FindNextWithSearchBox();
	}
	*/
	return true;//#135
}

//#137
/**
EditBoxでTabキーを押したときの処理
*/
ABool	AWindow_Text::NVIDO_ViewTabKeyPressed( const AControlID inControlID )//#135
{
	//★
	/*#724
	//検索ボックス
	AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( inControlID == searchBoxControlID )
	{
		//TextViewへフォーカス移動
		AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
		if( lastFocusedTextView != kControlID_Invalid )
		{
			NVI_SwitchFocusTo(lastFocusedTextView);
		}
	}
	*/
	return true;//#135
}

//#137
/**
EditBoxでESCキーを押したときの処理
*/
ABool	AWindow_Text::NVIDO_ViewEscapeKeyPressed( const AControlID inControlID )//#135
{
	//★
	/*#724
	//検索ボックス
	AControlID	searchBoxControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( inControlID == searchBoxControlID )
	{
		//TextViewへフォーカス移動
		AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
		if( lastFocusedTextView != kControlID_Invalid )
		{
			NVI_SwitchFocusTo(lastFocusedTextView);
		}
	}
	*/
	return true;//#135
}

//#137
/**
検索ボックスによる検索
*/
void	AWindow_Text::FindNextWithSearchBox()
{
	//
	AControlID	searchBoxControlID = kControlID_Toolbar_SearchField;//#724 mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( searchBoxControlID == kControlID_Invalid )   return;
	//検索パラメータ取得
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	AText	text;
	NVI_GetControlText(searchBoxControlID,text);
	if( text.GetItemCount() == 0 )//#1114   return;//#724
	{
		//#1114
		//検索フィールドが空、かつ、イベントのテキストがESCの場合、テキストビューへフォーカスを戻す
		//（ESCキーを押した時、OSによって検索フィールドが空になっている。）
		AText	eventText;
		NSString*	str = [[NSApp currentEvent] charactersIgnoringModifiers];
		if( str != nil )
		{
			eventText.SetFromNSString(str);
		}
		if( eventText.GetItemCount() == 1 )
		{
			if( eventText.Get(0) == 0x1B )
			{
				if( mLatentTextViewContolID != kControlID_Invalid )
				{
					NVI_SwitchFocusTo(mLatentTextViewContolID);
				}
			}
		}
		return;
	}
	param.findText.SetText(text);
	//検索ウインドウにも検索文字列として登録
	GetApp().SPI_SetFindText(text);
	//最後のフォーカスTextViewで検索実行
	//AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
	if( mLatentTextViewContolID != kControlID_Invalid )
	{
		if( IsTextViewControlID(mLatentTextViewContolID) == true )
		{
			GetTextViewByControlID(mLatentTextViewContolID).SPI_FindNext(param);
		}
	}
}

//#268
/**
検索ボックスによる検索（前）
*/
void	AWindow_Text::FindPreviousWithSearchBox()
{
	AControlID	searchBoxControlID = kControlID_Toolbar_SearchField;//#724 mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( searchBoxControlID == kControlID_Invalid )   return;
	//検索パラメータ取得
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	AText	text;
	NVI_GetControlText(searchBoxControlID,text);
	if( text.GetItemCount() == 0 )   return;//#724
	param.findText.SetText(text);
	//検索ウインドウにも検索文字列として登録
	GetApp().SPI_SetFindText(text);
	//最後のフォーカスTextViewで検索実行
	//AControlID	lastFocusedTextView = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
	if( mLatentTextViewContolID != kControlID_Invalid )
	{
		if( IsTextViewControlID(mLatentTextViewContolID) == true )
		{
			GetTextViewByControlID(mLatentTextViewContolID).SPI_FindPrevious(param);
		}
	}
}

//#137
/**
検索ボックスに検索文字列を入れる
*/
void	AWindow_Text::SPI_SetSearchBoxText( const AText& inText )
{
	//#725
	AControlID	searchBoxControlID = kControlID_Toolbar_SearchField;//mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetSearchBoxControlID();
	if( searchBoxControlID == kControlID_Invalid )   return;
	NVI_SetControlText(searchBoxControlID,inText);
}

//#232
/**
CrossCaret表示・非表示切り替え
*/
void	AWindow_Text::SPI_ShowHideCrossCaret()
{
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
	{
		GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(viewIndex)).SPI_ShowHideCrossCaret();
	}
}

//#291
/**
セパレータ移動
@return 実際に移動したdelta
*/
ANumber	AWindow_Text::NVIDO_SeparatorMoved( const AWindowID inSeparatorWindowID, const AControlID inID, 
		const ANumber inDelta, const ABool inCompleted )//#409
{
	//
	if( inCompleted == false )
	{
		NVI_SetVirtualLiveResizing(true);
	}
	else
	{
		NVI_SetVirtualLiveResizing(false);
	}
	//
	if( inSeparatorWindowID == mRightSideBarVSeparatorWindowID )
	{
		return ChangeRightSideBarWidth(inDelta,inCompleted);//#409
	}
	else if( inSeparatorWindowID == mLeftSideBarVSeparatorWindowID )
	{
		return ChangeLeftSideBarWidth(inDelta,inCompleted);//#409
	}
	//#725 サブテキストカラム
	else if( inSeparatorWindowID == mSubTextColumnVSeparatorWindowID )
	{
		return ChangeSubTextPaneColumnWidth(inDelta,inCompleted);
	}
	//#634 サイドバーVSeparatorボタンによる移動
	else if( inSeparatorWindowID == GetObjectID() && inID == kControlID_RightSideBar_SeparatorButton )
	{
		return ChangeRightSideBarWidth(inDelta,inCompleted);
	}
	//#725
	else if( inSeparatorWindowID == GetObjectID() && inID == kControlID_LeftSideBar_SeparatorButton )
	{
		return ChangeLeftSideBarWidth(inDelta,inCompleted);
	}
	//
	else
	{
		for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			if( mRightSideBarArray_OverlayWindowID.Get(i) == inSeparatorWindowID )
			{
				return ChangeRightSideBarHeight(i,inDelta);
			}
		}
		for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			if( mLeftSideBarArray_OverlayWindowID.Get(i) == inSeparatorWindowID )
			{
				return ChangeLeftSideBarHeight(i,inDelta);
			}
		}
		//#866
		//テキストビューHSeparator線移動
		//該当HSeparator検索し、一致したらtext view高さ変更
		for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
		{
			AIndex	viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromSeparatorControlID(inID);
			if( viewIndex != kIndex_Invalid )
			{
				return ChangeTextViewHeight(tabIndex,viewIndex,inDelta,inCompleted);
			}
		}
	}
	return inDelta;
}

//#291
/**
セパレータダブルクリック
*/
void	AWindow_Text::NVIDO_SeparatorDoubleClicked( const AWindowID inSeparatorWindowID, const AControlID inID )
{
	if( inSeparatorWindowID == mRightSideBarVSeparatorWindowID )
	{
		ShowHideRightSideBar(!mRightSideBarDisplayed);
	}
	else if( inSeparatorWindowID == mLeftSideBarVSeparatorWindowID )
	{
		ShowHideLeftSideBar(!mLeftSideBarDisplayed);
	}
}

//#602
/**
ヘルプタグ情報取得コールバック
*/
ABool	AWindow_Text::EVTDO_DoGetHelpTag( const AControlID inID, const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	outTagSide = kHelpTagSide_Default;//#643
	ABool	result = false;
	switch(inID)
	{
	  case kSubPaneDiffButtonControlID:
		{
			AText	text;
			if( mDiffDisplayMode == false )
			{
				text.SetLocalizedText("HelpTag_ShowDiffSubPaneText");
			}
			else
			{
				text.SetLocalizedText("HelpTag_HideDiffSubPaneText");
			}
			outText1.SetText(text);
			result = true;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return result;
}


//#389 #859
/**
タブをウインドウ化
*/
void	AWindow_Text::SPI_TabToWindow( const AIndex inTabIndex, const ABool inWindowPositionDirected, const APoint inPoint )
{
	AWindowDefaultFactory<AWindow_Text>	factory;//#175 (&GetApp()); #199
	AObjectID	winID = GetApp().NVI_CreateWindow(factory);
	//#859 AIndex	tabIndex = NVI_GetCurrentTabIndex();
	SPI_CopyTabToWindow(inTabIndex,winID);
	if( inWindowPositionDirected == true )
	{
		GetApp().SPI_GetTextWindowByID(winID).NVI_SetWindowPosition(inPoint);
	}
	SPI_CloseTab(inTabIndex);
	GetApp().SPI_GetTextWindowByID(winID).SPI_UpdateViewBounds();//#291
	GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
}

//#389
/**
最もタブ数の多いウインドウへ、全ての選択可能タブを移動
*/
void	AWindow_Text::SPI_WindowToTab()
{
	//最もタブ数の多いウインドウを取得
	AWindowID	tabWindowID = GetApp().SPI_GetMostNumberTabsTextWindowID();
	if( tabWindowID == kObjectID_Invalid )   return;
	
	//ウインドウの全てのタブをメインウインドウへコピーして、タブを全て閉じる（＝ウインドウを閉じる）
	{
		//両ウインドウとも、ブロック内ではupdate view boundsを抑制
		AStSuppressTextWindowUpdateViewBounds	s1(tabWindowID);
		AStSuppressTextWindowUpdateViewBounds	s2(GetObjectID());
		//選択可能なタブを全て、最背面のタブから順に、コピー
		while( NVI_GetSelectableTabCount() > 0 )
		{
			AIndex	mostBackTabIndex = NVI_GetMostBackTabIndex();//z-order最後のタブ
			GetApp().SPI_GetTextWindowByID(tabWindowID).SPNVI_CopyCreateTab(GetObjectID(), mostBackTabIndex);//タブをコピー生成し、元のタブを削除
		}
	}
	//view bounds更新
	GetApp().SPI_GetTextWindowByID(tabWindowID).SPI_UpdateViewBounds();//#291
}

//#389
/**
タブをウインドウ化（左へ移動）
*/
void	AWindow_Text::SPI_DetachTab_Left( const AIndex tabIndex )
{
	ABool	active = false;
	if( tabIndex == NVI_GetCurrentTabIndex() )   active = true;
	AWindowDefaultFactory<AWindow_Text>	factory;//#175 (&(GetApp())); #199
	AObjectID	winID = GetApp().NVI_CreateWindow(factory);
	SPI_CopyTabToWindow(tabIndex,winID);
	SPI_CloseTab(tabIndex);
	ANumber	offset = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kTabToWindowPositionOffset);
	/*#668
	ARect	rect;
	GetApp().NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
	rect.left -= offset;
	rect.right -= offset;
	GetApp().NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
	*/
	//SPI_CopyTabToWindow()ではウインドウ位置・サイズはそれぞれのドキュメント設定に従うようにしたので、
	//位置だけをタブウインドウからのオフセット位置に設定する #668
	APoint	pt = {0,0};
	NVI_GetWindowPosition(pt);
	pt.x -= offset;
	GetApp().NVI_GetWindowByID(winID).NVI_SetWindowPosition(pt);
	//
	//#919 現在のウインドウの背景に隠れるとどれだかわからなくなるので、常に最前面にする。if( active == true )
	{
		GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
	}
	//#370 位置補正
	GetApp().SPI_GetTextWindowByID(winID).NVI_ConstrainWindowPosition(false);//部分的に隠れる場合も位置補正
}

//#389
/**
タブをウインドウ化（右へ移動）
*/
void	AWindow_Text::SPI_DetachTab_Right( const AIndex tabIndex )
{
	ABool	active = false;
	if( tabIndex == NVI_GetCurrentTabIndex() )   active = true;
	AWindowDefaultFactory<AWindow_Text>	factory;//#175 (&(GetApp())); #199
	AObjectID	winID = GetApp().NVI_CreateWindow(factory);
	SPI_CopyTabToWindow(tabIndex,winID);
	SPI_CloseTab(tabIndex);
	ANumber	offset = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kTabToWindowPositionOffset);
	/*#668
	ARect	rect;
	GetApp().NVI_GetWindowByID(winID).NVI_GetWindowBounds(rect);
	rect.left += offset;
	rect.right += offset;
	GetApp().NVI_GetWindowByID(winID).NVI_SetWindowBounds(rect);
	*/
	//SPI_CopyTabToWindow()ではウインドウ位置・サイズはそれぞれのドキュメント設定に従うようにしたので、
	//位置だけをタブウインドウからのオフセット位置に設定する #668
	APoint	pt = {0,0};
	NVI_GetWindowPosition(pt);
	pt.x += offset;
	GetApp().NVI_GetWindowByID(winID).NVI_SetWindowPosition(pt);
	//
	//#919 現在のウインドウの背景に隠れるとどれだかわからなくなるので、常に最前面にする。if( active == true )
	{
		GetApp().NVI_GetWindowByID(winID).NVI_SelectWindow();
	}
	//#370 位置補正
	GetApp().SPI_GetTextWindowByID(winID).NVI_ConstrainWindowPosition(false);//部分的に隠れる場合も位置補正
}

//#389
/**
タブをウインドウ化
*/
void	AWindow_Text::SPI_DetachTab( const AIndex tabIndex )
{
	ARect	bounds;
	NVI_GetWindowBounds(bounds);
	AGlobalRect	screenBounds;
	//#688 CWindowImp::GetAvailableWindowPositioningBounds(screenBounds);
	NVI_GetAvailableWindowBoundsInSameScreen(screenBounds);//#688
	if( (bounds.left+bounds.right)/2 < (screenBounds.left+screenBounds.right)/2 )
	{
		SPI_DetachTab_Right(tabIndex);
	}
	else
	{
		SPI_DetachTab_Left(tabIndex);
	}
}

//win
/**
印刷ダイアログ表示
*/
/*#524
void	AWindow_Text::SPI_Print()
{
	AText	title;
	NVI_GetCurrentTitle(title);
	SPI_GetCurrentTabTextDocument().NVI_Print(GetObjectID(),title);
}
*/

//#438
/**
インデックスウインドウ・次
*/
/*#725
void	AWindow_Text::SPI_IndexWindowNext()
{
	if( mFindResultViewID != kObjectID_Invalid )
	{
		AView_Index::GetIndexViewByViewID(mFindResultViewID).SPI_SelectNextRow();
	}
	else
	{
		GetApp().SPI_GoNextIndexWindowItem();
	}
}
*/

//#438
/**
インデックスウインドウ・前
*/
/*#725
void	AWindow_Text::SPI_IndexWindowPrev()
{
	if( mFindResultViewID != kObjectID_Invalid )
	{
		AView_Index::GetIndexViewByViewID(mFindResultViewID).SPI_SelectPreviousRow();
	}
	else
	{
		GetApp().SPI_GoPrevIndexWindowItem();
	}
}
*/

//#112
/**
ファイル選択時処理
*/
void	AWindow_Text::EVTDO_FileChoosen( const AControlID inControlID, const AFileAcc& inFile )
{
	switch(inControlID)
	{
	  case kVirtualControlID_CompareFileChoosen:
		{
			//比較ファイル選択時処理
			SPI_GetCurrentFocusTextDocument().SPI_SetDiffMode_File(inFile,kCompareMode_File);
			SPI_SetDiffDisplayMode(true);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

/*#1091-test
void	AWindow_Text::EVTDO_DoTickTimerAction()
{
	if( mDefferedFocusTick > 0 )
	{
		mDefferedFocusTick--;
		if( mDefferedFocusTick == 0 )
		{
			NVI_SwitchFocusTo(mDefferedFocus);
			fprintf(stderr,"mDefferedFocusTick(%d)  ",mDefferedFocus);
		}
	}
}
*/

#pragma mark ===========================

#pragma mark ---個別コマンド処理

//メニューから「閉じる」選択、もしくは、ウインドウの閉じるボタンクリック時、CloseをTryする。（DirtyならAskSaveChangesウインドウを開く）
void	AWindow_Text::TryClose( const AIndex inTabIndex )//win 080709 ABool→void（返り値未使用のため）
{
	//AskSaveChangesウインドウ表示中なら何もせずリターン
	if( mAskingSaveChanges == true )    return;//win 080709 false;
	
	//現在のドキュメントがDirtyかつウインドウ１つかどうか判断
	ADocumentID	closingDocID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	if( GetApp().SPI_GetTextDocumentByID(closingDocID).NVI_IsDirty() == true && 
				//#379 GetApp().SPI_GetTextDocumentByID(closingDocID).SPI_GetWindowCount() <= 1 )
				GetApp().SPI_GetTextDocumentByID(closingDocID).SPI_ExistAnotherWindow(GetObjectID()) == false )//#379
	{
		//AskSaveChangesウインドウを開いて、保存するかどうかを尋ねる
		AText	filename;
		GetApp().SPI_GetTextDocumentByID(closingDocID).NVI_GetTitle(filename);
		mAskingSaveChanges = true;//win 080709
		NVI_ShowAskSaveChangesWindow(filename,closingDocID);
		//win 080709上へ移動 mAskingSaveChanges = true;
		NVI_SelectWindow();
		//return false;
	}
	else
	{
		//Close実行
		ExecuteClose(closingDocID);
		//return true;
	}
}

//Close実行
void	AWindow_Text::ExecuteClose( const AObjectID inClosingDocumentID )
{
	//ジャンプメニュー実体化解除
	GetApp().NVI_GetMenuManager().UnrealizeDynamicMenu(GetApp().SPI_GetTextDocumentByID(inClosingDocumentID).SPI_GetJumpMenuObjectID());
	//ツールメニュー更新
	if( NVI_GetCurrentDocumentID() == inClosingDocumentID )//#988
	{
		GetApp().SPI_UpdateToolMenu(kIndex_Invalid);
	}
	
	//globals->mHookManager->CloseHook();
	
	//B0000 各種シートウインドウを強制クローズ（メニューからはクローズできないが、AppleScriptからのクローズは無視できない）
	/*OSX10.4でクラッシュするのでやめる。NavDialogDispose()がNG？
	if( mAskingSaveChanges == true )
	{
		NVM_GetImp().CloseAskSaveChanges();
		mAskingSaveChanges = false;
	}*/
	if( /*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mMoveLineWindow*/SPI_GetMoveLieWindow().NVI_Close();
	}
	//#844 if( /*#199 mFontSizeWindow*/SPI_GetFontSizeWindow().NVI_IsWindowVisible() == true )
	//#844 {
	//#844 	/*#199 mFontSizeWindow*/SPI_GetFontSizeWindow().NVI_Close();
	//#844 }
	if( /*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_Close();
	}
	if( /*#199 mTextEncodingCorrectWindow*/SPI_GetCorrectEncodingWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mTextEncodingCorrectWindow*/SPI_GetCorrectEncodingWindow().NVI_Close();
	}
	//#844 if( /*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_IsWindowVisible() == true )
	//#844 {
	//#844 	/*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_Close();
	//#844 }
	//#844 if( /*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_IsWindowVisible() == true )
	//#844 {
	//#844 	/*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_Close();
	//#844 }
	if( /*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().NVI_Close();
	}
	if( /*#199 mAddToolButtonWindow*/SPI_GetAddToolButtonWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mAddToolButtonWindow*/SPI_GetAddToolButtonWindow().NVI_Close();
	}
	if( /*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().NVI_IsWindowVisible() == true )
	{
		/*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().NVI_Close();
	}
	//#455
	if( SPI_GetSCMCommitWindow().NVI_IsWindowVisible() == true )
	{
		SPI_GetSCMCommitWindow().NVI_Close();
	}
	/* #138 if( mExternalCommentWindow.NVI_IsWindowVisible() == true )//RC3
	{
		mExternalCommentWindow.NVI_Close();
	}*/
	if( NVI_IsChildWindowVisible() == true )
	{
		NVI_CloseChildWindow();
	}
	
	//#379 閉じている最中のDiff描画防止のためDiffモードを解除しておく
	//Tab<->Window切り替え時に比較ボタンdisableになってしまうのでコメントアウトGetApp().SPI_GetTextDocumentByID(inClosingDocumentID).SPI_SetDiffMode(kDiffTargetMode_None);
	//#379 閉じようとしているドキュメントが所有するドキュメントのタブをクローズ
	AArray<ADocumentID>	docIDArray;
	GetApp().SPI_GetTextDocumentByID(inClosingDocumentID).SPI_GetOwnDocIDArray(docIDArray);
	for( AIndex i = 0; i < docIDArray.GetItemCount(); i++ )
	{
		//閉じようとしているドキュメントが所有するドキュメントが実際にこのウインドウ上に表示されているかどうかを判断。
		//（1ドキュメント2ウインドウ対応(#856)したことで、必ずしも所有ドキュメントがこのウインドウ上に表示されているとは限らなくなったため）
		ADocumentID	docID = docIDArray.Get(i);
		if( NVI_GetTabIndexByDocumentID(docID) != kIndex_Invalid )
		{
			//所有ドキュメントについてclose実行
			ExecuteClose(docID);
		}
	}
	
	//doc prefを保存
	GetApp().SPI_GetTextDocumentByID(inClosingDocumentID).SPI_SaveDocPrefIfLoaded();
	
	//#212
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inClosingDocumentID);
	DeleteSubTextInEachTab(tabIndex);
	
	//#212
	UpdateSubTextFileNameButton();
	
	//ジャンプリストの該当タブ削除 #291
	//#725 SPI_GetJumpListWindow().NVI_DeleteTabAndView(tabIndex);
	GetApp().SPI_CloseJumpListWindowsTab(GetObjectID(),inClosingDocumentID);//#725
	
	//#0 Deleteされるとメンバーデータへのアクセスが保証できなくなるので先に保存しておく
	AObjectID	winID = GetObjectID();
	
	/*#725 tabに属するoverlay windowは、NVI_DeleteTabAndView()で削除する。
	//ViewCloseOverlayウインドウを全て削除する win
	//SPI_DeleteSplitView()をコールすることも検討したが、現状、ほかのデータはNVI_DeleteTabAndView()で削除するつくりだし、
	//NVI_DeleteTabAndView()内でUpdateViewBounds()等をコールしているので、個別に削除する。
	for( AIndex v = 0; v < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); v++ )
	{
		GetApp().NVI_DeleteWindow(mTabDataArray.GetObjectConst(tabIndex).GetCloseViewButtonWindowID(v));
	}
	*/
	
	//タブ＋内包ビュー削除（内部でNVIDO_DeleteTab()が呼ばれる）
	NVI_DeleteTabAndView(NVI_GetTabIndexByDocumentID(inClosingDocumentID));
	
	//タブが0個になったらウインドウ削除
	if( NVI_GetTabCount() == 0 )
	{
		/*#725
		//FindViewResultCloseBUttonを削除 win
		if( mFindResultCloseButtonWindowID != kObjectID_Invalid )
		{
			GetApp().NVI_DeleteWindow(mFindResultCloseButtonWindowID);
			mFindResultCloseButtonWindowID = kObjectID_Invalid;
		}
		*/
		
		//フローティングのジャンプリストウインドウを閉じる（overlayのjumplistは下で右サイドバーカラム、サブペインカラム削除時に削除される。）
		//#725 SPI_GetJumpListWindow().NVI_Close();
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_DeleteSubWindow(mFloatingJumpListWindowID);
			mFloatingJumpListWindowID = kObjectID_Invalid;
		}
		
		//#291 各Infoウインドウのオーバーレイモードを解除
		//分割線ウインドウを隠す。削除されるのは、DoDeleted()のタイミング（イベントトランザクション終了時）。#291 #212
		if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			GetApp().NVI_GetWindowByID(mLeftSideBarVSeparatorWindowID).NVI_Hide();
		}
		if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			GetApp().NVI_GetWindowByID(mRightSideBarVSeparatorWindowID).NVI_Hide();
		}
		//#725 サブテキストカラム分割線非表示
		if( mSubTextColumnVSeparatorWindowID != kObjectID_Invalid )
		{
			GetApp().NVI_GetWindowByID(mSubTextColumnVSeparatorWindowID).NVI_Hide();
		}
		/*#725 各サブウインドウは下で削除する。
		if( SPI_IsTabModeMainWindow() == true )//#377
		{
			//オーバーレイウインドウは閉じることになるので、さきに隠す #338
			if( GetApp().SPI_GetIdInfoWindow().NVI_GetOverlayMode() == true )
			{
				GetApp().SPI_GetIdInfoWindow().NVI_Hide();
			}displa
			if( GetApp().SPI_GetFileListWindow().NVI_GetOverlayMode() == true )
			{
				GetApp().SPI_GetFileListWindow().NVI_Hide();
			}
		}
		*/
		
		//#725
		//右サイドバー、サブペイン削除
		DeleteRightSideBar();
		DeleteLeftSideBar();
		
		//#338 先にテキストウインドウを完全に削除してから、オーバーレイウインドウをフローティングに戻す。
		NVI_Close();
		GetApp().NVI_DeleteWindow(GetObjectID());
		//ウインドウメニュー更新
		//#922 GetApp().SPI_UpdateWindowMenu();
		/*#725 IdInfoWindow, FileListWindowをフローティングに戻すことはもうない。（上でInfoPane, SubPane削除）
		if( SPI_IsTabModeMainWindow() == true )//#377
		{
			//識別子情報ウインドウをフローティングに戻す
			RevertFromOverlay_IdInfoWindow();
			//ファイルリストウインドウをフローティングに戻す
			RevertFromOverlay_FileListWindow();
		}
		*/
		//#338 上に移動。
		//#338 下記処理の前にRevertFromOverlay_IdInfoWindow()を実行すると、ファイルリスト（ウインドウリスト）を再作成するとき、
		//#338 テキストウインドウが中途半端に存在する状態なので、ウインドウタイトル取得時にインデックスエラーが発生する。
		//#338 なお、この変更をすると、オーバーレイの親ウインドウを先に削除することになるため、CWindowImpにも対応を追加した。
		//NVI_Close();
		//GetApp().NVI_DeleteWindow(GetObjectID());
	}
	//タブがまだ残っている場合
	else//#212 フォーカスを新しい現在Tabの最初のTextViewに設定
	{
		//ここでswitch focusするとNSView<NSTextInputClient>をremoveFromSuperviewするときに時間がかかる。
		//（アクティビティモニタのサンプリングでは、TSMのactivateに時間がかかっているように見える。）
		//AWindow::NVI_DeleteTabAndView()にて、アクティブなタブを削除したときは、新たなタブを選択し直しており、
		//NVI_SelectTab()内でタブ内フォーカスが設定されるので、ここでのswitch focusは必要ないため、削除。
		//fprintf(stderr,"SWITCH(%d,%d)  ",NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0));
		//#688 NVI_SwitchFocusTo(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0));
		
		//win タブを閉じたときにサブペインに表示が無くなったら、現在のタブの内容をサブペインに表示する
		if( /*#725 mLeftSideBarDisplayed == true &&*/ mSubTextCurrentDocumentID == kObjectID_Invalid )
		{
			SPI_DisplayInSubText(NVI_GetCurrentTabIndex(),false,kIndex_Invalid,false,false);
		}
	}
	
	//ドキュメントへウインドウ（タブ）が閉じられたことを通知する
	//#379 GetApp().SPI_GetTextDocumentByID(inClosingDocumentID).OWICB_DoWindowClosed(winID);//#0 GetObjectID());
	
	//ウインドウメニュー更新
	//#922 GetApp().SPI_UpdateWindowMenu();
	//ファイルリストウインドウ更新
	//#725 GetApp().SPI_UpdateFileListWindow_DocumentOpened(inClosingDocumentID);
	GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_DocumentClosed,inClosingDocumentID);//#725
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_Text::NVIDO_Create( const ADocumentID inDocumentID )
{
	const ANumber	kWindowPositionOffset = 20;//B0394
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AWindow_Text::NVIDO_Create() started.",this);
	NVM_CreateWindow(kWindowClass_Document);
	//ツールバー生成
	NVI_CreateToolbar(0);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("Window Imp created.",this);
	
	/*#725 コンストラクタへ移動
	//#668 サブペイン表示状態の設定
	if( SPI_IsTabModeMainWindow() == true )
	{
		//メインタブウインドウの場合はappprefから状態取得
		mLeftSideBarDisplayed = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kLeftSideBarDisplayed);
	}
	else
	{
		//メインタブウインドウ以外は、初期状態はサブペイン非表示
		mLeftSideBarDisplayed = false;
	}
	*/
	//==================ウインドウサイズ、位置設定 ==================
	//@note メインカラムの分の位置、サイズ
	ARect	bounds = {0,0,300,300};//B0394
	/*#850
	if( SPI_IsTabModeMainWindow() == true )//#668 GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
	{
		//タブメインウインドウの場合
		
		//タブモードのウインドウサイズをAppPrefから取得
		GetApp().GetAppPref().GetData_Rect(AAppPrefDB::kSingleWindowBounds,bounds);
	}
	else
	{
	//タブモードでない場合
	*/
	
	/*B0344 if( GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_GetModeIndex()).
	GetData_Bool(AModePrefDB::kSaveWindowPosition) == true )
	{*/
	if( mInitialWindowBoundsIsSet == true )//#850
	{
		//------------Initial Bounds設定有りの場合（＝reopenの場合）------------
		
		bounds = mInitialWindowBounds;
	}
	else
	{
		//------------Initial Bounds設定無しの場合（＝reopen以外の場合）------------
		
		//DocPrefから位置データ読み込み
		APoint	pt = {0,0};
		GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Point(ADocPrefDB::kWindowPosition,pt);
		//DocPrefからサイズデータ読み込み #1238
		ANumber	width = GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowWidth);
		ANumber	height = GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowHeight);
		//DocPrefへの位置データ設定有りかどうかを判定
		if( GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().IsStillDefault(ADocPrefDB::kWindowPosition) == true ||
					(pt.x==-1&&pt.y==-1) )
		{
			//------------DocPrefの設定無し、または、タブウインドウ時に保存された(＝-1,-1になっている)場合------------
			
			//最前面ウインドウからの相対位置、もしくは、スクリーンの左上
			AWindowID	frontWindowID = GetApp().NVI_GetMostFrontWindowID(kWindowType_Text);
			if( frontWindowID != kObjectID_Invalid )
			{
				//------------DocPref無し・最前面テキストウインドウ有りの場合------------
				
				//最前面ウインドウからの相対位置を取得
				ARect	frontWindowBounds;
				GetApp().SPI_GetTextWindowByID(frontWindowID).SPI_GetWindowMainColumnBounds(frontWindowBounds);//#850
				pt.x 	= frontWindowBounds.left + kWindowPositionOffset;
				pt.y	= frontWindowBounds.top + kWindowPositionOffset;
			}
			else
			{
				//------------DocPref無し・最前面テキストウインドウ無しの場合------------
				
				//メニューバー・dockを除いたスクリーンboundsの左上位置を取得
				AGlobalRect	screenbounds = {0};
				AWindow::NVI_GetAvailableWindowBoundsInMainScreen(screenbounds);
				pt.x 	= screenbounds.left +16;
				pt.y	= screenbounds.top;
				
				//「ドキュメントはタブに開く」がONの場合、ウインドウは固定位置に開く（前回閉じた時のウインドウ位置）
				if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
				{
					GetApp().GetAppPref().GetData_Point(AAppPrefDB::kNewDocumentWindowPoint,pt);
				}
			}
			
			//DocPrefの設定なしの場合は、サイズデータはkSingleWindowBoundsから取得 #1238
			ARect	b = {0};
			GetApp().GetAppPref().GetData_Rect(AAppPrefDB::kSingleWindowBounds,b);
			width = b.right - b.left;
			height = b.bottom - b.top;
		}
		//新規ウインドウbounds取得
		bounds.left 	= pt.x;
		bounds.top 		= pt.y;
		bounds.right 	= bounds.left + width;//GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowWidth);
		bounds.bottom 	= bounds.top + height;//GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowHeight);
	}
	/*}
	else
	{
	ARect	screenRect;
	CWindowImp::GetAvailableWindowPositioningBounds(screenRect);
	ANumber	offset = GetApp().NVI_GetDocumentCount(kDocumentType_Text)*kNewWindowPositionOffset;
	bounds.left 	= screenRect.left+offset;
	bounds.top 		= screenRect.top+offset+kWindowTitleBarHeight;
	bounds.right 	= bounds.left + GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowWidth);
	bounds.bottom 	= bounds.top +  GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().GetData_Number(ADocPrefDB::kWindowHeight);
	}*/
	//#850 }
	//#567 LuaConsole
	if( GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_GetLuaConsoleMode() == true )
	{
		//LuaConsoleモード設定
		mLuaConsoleMode = true;
		
		//サブペイン・右サイドバー非表示
		mLeftSideBarDisplayed = false;
		mRightSideBarDisplayed = false;
		
		//LuaConsole用boundsを設定
		APoint	pt = {0,0};
		GetApp().NVI_GetAppPrefDB().GetData_Point(AAppPrefDB::kLuaConsoleWindowPosition,pt);
		bounds.left		= pt.x;
		bounds.top		= pt.y;
		bounds.right	= bounds.left +
						GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kLuaConsoleWindowWidth);
		bounds.bottom	= bounds.top +
						GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kLuaConsoleWindowHeight);
	}
	
	//#476 ウインドウが異常に小さくなったときの異常系対策
	if( bounds.right - bounds.left < 100 )   bounds.right = bounds.left + 100;
	if( bounds.bottom - bounds.top < 100 )   bounds.bottom = bounds.top + 100;
	//ウインドウ位置・サイズ設定
	SPI_SetWindowBoundsByMainColumnPosition(bounds);//#850
	//win 最大化状態復元
#if IMPLEMENTATION_FOR_WINDOWS
//MacはNVI_Zoom()に未対応なのでとりあえず#ifで切っておく
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kSingleWindowBounds_Zoomed) == true )
	{
		GetApp().GetAppPref().GetData_Rect(AAppPrefDB::kSingleWindowBounds_UnzoomedBounds,bounds);
		NVI_SetWindowBounds(bounds);
		NVI_Zoom();
	}
#endif
	
	//#385 ウインドウサイズ補正（サイズボックスが押せないと操作に迷う人がけっこういるので）
	NVI_ConstrainWindowPosition(true);//部分的に隠れていてもOK
	NVI_ConstrainWindowSize();
	
	//==================固定View生成==================
	
	//window bounds取得
	ARect	windowbounds = {0};
	NVI_GetWindowBounds(windowbounds);
	
	//背景色描画用view生成
	if( true )
	{
		//右下のスクロールバーが交わる部分のみ背景描画する
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreatePlainView(kControlID_WindowBackground,pt,kVScrollBarWidth+8,kHScrollBarHeight+8);
		NVI_SetShowControl(kControlID_WindowBackground,true);
		NVI_SetControlBindings(kControlID_WindowBackground,false,false,true,true,true,true);
		NVI_SetPlainViewColor(kControlID_WindowBackground,kColor_White,kColor_White,false,false,false,false,kColor_White);
	}
	else
	{
		//ウインドウ全体を背景描画
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreatePlainView(kControlID_WindowBackground,pt,windowbounds.right-windowbounds.left,windowbounds.bottom-windowbounds.top);
		NVI_SetShowControl(kControlID_WindowBackground,true);
		NVI_SetControlBindings(kControlID_WindowBackground,true,true,true,true,false,false);
		NVI_SetPlainViewColor(kControlID_WindowBackground,kColor_White,kColor_White,false,false,false,false,kColor_White);
	}
	//右サイドバー背景
	{
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreatePlainView(kControlID_RightSideBarBackground,pt,10,10);
		NVI_SetShowControl(kControlID_RightSideBarBackground,false);
		NVI_SetControlBindings(kControlID_RightSideBarBackground,false,true,true,true,true,false);
	}
	//左サイドバー背景
	{
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreatePlainView(kControlID_LeftSideBarBackground,pt,10,10);
		NVI_SetShowControl(kControlID_LeftSideBarBackground,false);
		NVI_SetControlBindings(kControlID_LeftSideBarBackground,true,true,false,true,true,false);
	}
	
	//（正しいサイズ、位置は、SPI_UpdateViewBounds()で設定される。）
	AWindowPoint	pt = {-10000,-10000};//#1090
	//Hスクロールバー生成
	//#725 NVI_CreateScrollBar(kControlID_HScrollBar,pt,20,10);
	
	//win
	AText	defaultfontname;
	AFontWrapper::GetDialogDefaultFontName(defaultfontname);//#375
	//行番号ボタン生成 #232 ボタンをAView_Button化
	NVI_CreateButtonView(kControlID_LineNumberButton,pt,kLineNumberButtonWidth,kHeight_Footer-2,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Left,
				kTextStyle_DropShadow,kColor_Black,kColor_Black);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton).SPI_SetDropShadowColor(kColor_Gray80Percent);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,3);
	//場所ボタン生成 #232 ボタンをAView_Button化
	NVI_CreateButtonView(kControlID_PositionButton,pt,kPositionButtonWidth,kHeight_Footer-2,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Left,
				kTextStyle_DropShadow,kColor_Black,kColor_Black);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetDropShadowColor(kColor_Gray80Percent);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,5);
	NVI_GetButtonViewByControlID(kControlID_PositionButton).SPI_SetIcon(kImageID_iconSwList,0,2,18);
	
	//#725
	//サブテキストの行番号ボタン生成
	NVI_CreateButtonView(kControlID_LineNumberButton_SubText,pt,kLineNumberButtonWidth,kHeight_Footer-2,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Left,
				kTextStyle_DropShadow,kColor_Black,kColor_Black);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).SPI_SetDropShadowColor(kColor_Gray80Percent);
	NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,3);
	//サブテキストの場所ボタン生成
	NVI_CreateButtonView(kControlID_PositionButton_SubText,pt,kPositionButtonWidth,kHeight_Footer-2,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Left,
				kTextStyle_DropShadow,kColor_Black,kColor_Black);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetDropShadowColor(kColor_Gray80Percent);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetEllipsisMode(kEllipsisMode_FixedLastCharacters,5);
	NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).SPI_SetIcon(kImageID_iconSwList,0,2,18);
	
	//タブセレクタ
	AViewDefaultFactory<AView_TabSelector>	tabSelectorFactory(GetObjectID(),kControlID_TabSelector);//#199
	NVM_CreateView(kControlID_TabSelector,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,tabSelectorFactory);
	
	//#725
	//フッター
	{
		//
		AViewDefaultFactory<AView_Footer>	factory(GetObjectID(),kControlID_Footer);
		NVM_CreateView(kControlID_Footer,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	/*
	{
		//
		AViewDefaultFactory<AView_FooterSeparator>	factory(GetObjectID(),kControlID_FooterSeparator_MainColumn);
		NVM_CreateView(kControlID_FooterSeparator_MainColumn,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	{
		//
		AViewDefaultFactory<AView_FooterSeparator>	factory(GetObjectID(),kControlID_FooterSeparator_MainColumn2);
		NVM_CreateView(kControlID_FooterSeparator_MainColumn2,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	{
		//
		AViewDefaultFactory<AView_FooterSeparator>	factory(GetObjectID(),kControlID_FooterSeparator_SubTextColumn);
		NVM_CreateView(kControlID_FooterSeparator_SubTextColumn,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	{
		//
		AViewDefaultFactory<AView_FooterSeparator>	factory(GetObjectID(),kControlID_FooterSeparator_SubTextColumn2);
		NVM_CreateView(kControlID_FooterSeparator_SubTextColumn2,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,factory);
	}
	*/
	
	//#725
	//プログレスindicator
	{
		//progress indicator生成
		NVI_CreateProgressIndicator(kControlID_MainColumnProgressIndicator,pt,kWidth_ProgressIndicator,kHeight_ProgressIndicator);
		//progress text生成
		AWindowPoint	pt = {-10000,-10000};//#1090
		NVI_CreateButtonView(kControlID_MainColumnProgressText,pt,10,kHeight_ProgressIndicator,kControlID_Invalid);
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).SPI_SetButtonViewType(kButtonViewType_NoFrame);
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).SPI_SetTextProperty(defaultfontname,9.0,kJustification_Left,
					kTextStyle_DropShadow,kColor_Black,kColor_Black);
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).SPI_SetDropShadowColor(kColor_Gray80Percent);
		
	}
	
	/*#246
	//Toolbar
	AViewDefaultFactory<AView_Toolbar>	toolbarFactory(GetObjectID(),kControlID_ToolbarPlacard);//#199
	NVM_CreateView(kControlID_ToolbarPlacard,pt,10,10,false,toolbarFactory);
	*/
	/*#725 情報ヘッダ廃止
	//InfoHeader
	AViewDefaultFactory<AView_InfoHeader>	infoHeaderFactory(GetObjectID(),kControlID_InfoHeader);//#199
	NVM_CreateView(kControlID_InfoHeader,pt,10,10,kControlID_Invalid,false,infoHeaderFactory);
	*/
	/*#246
	//Ruler
	AViewDefaultFactory<AView_Ruler>	rulerFactory(GetObjectID(),kControlID_Ruler);//#199
	NVM_CreateView(kControlID_Ruler,pt,10,10,false,rulerFactory);
	GetRulerView().SPI_UpdateImageSize();
	*/
	//======================右サイドバー制御ボタン======================
	//サイドバーVSeparatorボタン
	NVI_CreateButtonView(kControlID_RightSideBar_SeparatorButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetButtonViewType(kButtonViewType_NoFrame);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetIcon(kImageID_iconPnHandle,8,3);//#725
	//#725 NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetIcon(kIconID_Mi_VSeparator,1,1);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetButtonBehaviorType(kButtonBehaviorType_VSeparator);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_SeparatorButton).SPI_SetButtonValidXRange(0,32);
	//サイドバー環境設定ボタン
	NVI_CreateButtonView(kControlID_RightSideBar_PrefButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_PrefButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_PrefButton).SPI_SetIcon(kImageID_iconPnOption,10,5,10);//#725
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_PrefButton).SPI_SetMenu(kContextMenuID_RightSideBarPref,kMenuItemID_Invalid);//#725
	//右サイドバー表示非表示ボタン
	NVI_CreateButtonView(kControlID_RightSideBar_ShowHideButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_ShowHideButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
	NVI_GetButtonViewByControlID(kControlID_RightSideBar_ShowHideButton).SPI_SetIcon(kImageID_iconPnRight,8,3);//#725
	
	//======================左サイドバー制御ボタン======================
	//サイドバーVSeparatorボタン
	NVI_CreateButtonView(kControlID_LeftSideBar_SeparatorButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_SeparatorButton).SPI_SetButtonViewType(kButtonViewType_NoFrame);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_SeparatorButton).SPI_SetIcon(kImageID_iconPnHandle,-20,3);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_SeparatorButton).SPI_SetButtonBehaviorType(kButtonBehaviorType_VSeparator);
	//サイドバー環境設定ボタン
	NVI_CreateButtonView(kControlID_LeftSideBar_PrefButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_PrefButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHoverWithFixedWidth);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_PrefButton).SPI_SetIcon(kImageID_iconPnOption,10,5,10);//#725
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_PrefButton).SPI_SetMenu(kContextMenuID_LeftSideBarPref,kMenuItemID_Invalid);//#725
	//右サイドバー表示非表示ボタン
	NVI_CreateButtonView(kControlID_LeftSideBar_ShowHideButton,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_ShowHideButton).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
	NVI_GetButtonViewByControlID(kControlID_LeftSideBar_ShowHideButton).SPI_SetIcon(kImageID_iconPnLeft,8,3);//#725
	
#if IMPLEMENTATION_FOR_WINDOWS
	//sizebox
	NVI_CreateSizeBox(kControlID_SizeBox,pt,kVScrollBarWidth,kHScrollBarHeight);
#endif
	
	AText	text;
	//==================サブテキスト関連 #212==================
	/*#379
	//サブテキストモードボタン
	NVI_CreateButtonView(kSubPaneModeButtonControlID,pt,kSubPaneModeButtonWidth,kSubPaneModeButtonHeight);
	NVI_GetButtonViewByControlID(kSubPaneModeButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center);
	NVI_SetShowControl(kSubPaneModeButtonControlID,false);
	SetSubPaneMode(mSubPaneMode);//ボタンテキスト更新のため
	*/
	//サブテキストファイル名ボタン
	NVI_CreateButtonView(kSubPaneFileNameButtonControlID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetButtonViewType(kButtonViewType_Rect20);
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center,kTextStyle_Normal);
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetIcon(kImageID_iconSwDocument,5,3);
	NVI_SetShowControl(kSubPaneFileNameButtonControlID,false);
	//サブテキスト同期ボタン
	NVI_CreateButtonView(kSubPaneSyncButtonControlID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetButtonViewType(kButtonViewType_Rect20);
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center,kTextStyle_Normal);
	NVI_SetShowControl(kSubPaneSyncButtonControlID,false);
	text.SetLocalizedText("SubPaneSyncButton");
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).NVI_SetText(text);
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetIcon(kImageID_iconSwShow,5,2);
	//サブテキスト入替ボタン
	NVI_CreateButtonView(kSubPaneSwapButtonControlID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetButtonViewType(kButtonViewType_Rect20);
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center,kTextStyle_Normal);
	NVI_SetShowControl(kSubPaneSwapButtonControlID,false);
	text.SetLocalizedText("SubPaneSwapButton");
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).NVI_SetText(text);
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetIcon(kImageID_iconSwReplace,5,2);
	//サブテキストDiffボタン #379
	NVI_CreateButtonView(kSubPaneDiffButtonControlID,pt,10,10,kControlID_Invalid);
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetButtonViewType(kButtonViewType_Rect20);
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetTextProperty(defaultfontname,kButtonFontSize,kJustification_Center,kTextStyle_Normal);
	NVI_SetShowControl(kSubPaneDiffButtonControlID,false);
	text.SetLocalizedText("SubPaneDiffButton");
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).NVI_SetText(text);
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetIcon(kImageID_iconSwCompare,5,2);
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetToggleMode(true);
	NVI_EnableHelpTagCallback(kSubPaneDiffButtonControlID);
	
	//==================カラムセパレータ==================
	//サブテキストVSeparator Overlayウインドウ生成 #219
	if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_ChangeToOverlay(GetObjectID(),true);//win
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_Create(kObjectID_Invalid);
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).SPI_SetTargetWindowID(GetObjectID());
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_None);
		NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_Hide();
		//#725 NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_SetWindowTransparency(1.0);//#634 (0.5);
		//#725 NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).SPI_SetTransparency(0.1);//#634
	}
	//右サイドバーVSeparator Overlayウインドウ生成 #212
	if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_ChangeToOverlay(GetObjectID(),true);//win
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_Create(kObjectID_Invalid);
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).SPI_SetTargetWindowID(GetObjectID());
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_None);
		NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_Hide();
		//#725 NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_SetWindowTransparency(1.0);//#634 0.5);
		//#725 NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).SPI_SetTransparency(0.1);//#634
	}
	//#725 サブテキストカラムVSeparator overlayウインドウ生成
	if( mSubTextColumnVSeparatorWindowID != kObjectID_Invalid )
	{
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_ChangeToOverlay(GetObjectID(),true);//win
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_Create(kObjectID_Invalid);
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).SPI_SetTargetWindowID(GetObjectID());
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_Right);
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_Hide();
		//#725 NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_SetWindowTransparency(1.0);
		//#725 NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).SPI_SetTransparency(0.1);//#634
	}
	
	//==================ヘルプタグ==================
	{
		//#602
		AText	text;
		text.SetLocalizedText("HelpTag_SwapSubPaneText");
		NVI_SetHelpTag(kSubPaneSwapButtonControlID,text);
		text.SetLocalizedText("HelpTag_DisplayToSubPaneText");
		NVI_SetHelpTag(kSubPaneSyncButtonControlID,text);
		//#634
		text.SetLocalizedText("HelpTag_SideBarAppPrefButton");
		NVI_SetHelpTag(kControlID_RightSideBar_PrefButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_SideBarShowHideInfoPaneButton");
		NVI_SetHelpTag(kControlID_RightSideBar_ShowHideButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_SideBarShowHideLeftSideBarButton");
		NVI_SetHelpTag(kControlID_LeftSideBar_ShowHideButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_SideBarInfoPaneVSeparatorButton");
		NVI_SetHelpTag(kControlID_RightSideBar_SeparatorButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_LineButton");
		NVI_SetHelpTag(kControlID_LineNumberButton,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_JumpButton");
		NVI_SetHelpTag(kControlID_PositionButton,text,kHelpTagSide_Top);
		
		//#725
		text.SetLocalizedText("HelpTag_LeftSideBar_VSeparatorButton");
		NVI_SetHelpTag(kControlID_LeftSideBar_SeparatorButton,text,kHelpTagSide_Top);
		
		//#725
		text.SetLocalizedText("HelpTag_LineButton");
		NVI_SetHelpTag(kControlID_LineNumberButton_SubText,text,kHelpTagSide_Top);
		text.SetLocalizedText("HelpTag_JumpButton");
		NVI_SetHelpTag(kControlID_PositionButton_SubText,text,kHelpTagSide_Top);
	}
	
	//==================サイドバー生成==================
	
	//#725 SPI_UpdateSubPaneArray();
	//左サイドバー
	if( mLeftSideBarDisplayed == true )
	{
		CreateLeftSideBar();
		mLeftSideBarCreated = true;
	}
	
	//#725 SPI_UpdateInfoPaneArray();
	//右サイドバー
	if( mRightSideBarDisplayed == true )
	{
		CreateRightSideBar();
		mRightSideBarCreated = true;
	}
	
	//==================diff infoオーバーレイウインドウ==================
	//#379 Diff情報表示ウインドウ生成 
	AWindowDefaultFactory<AWindow_DiffInfo>	diffInfoWindowFactory;
	mDiffInfoWindowID = GetApp().NVI_CreateWindow(diffInfoWindowFactory);
#if IMPLEMENTATION_FOR_WINDOWS
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_SetWindowTransparency(0.1);
#endif
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_ChangeToOverlay(GetObjectID(),true);
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Create(kObjectID_Invalid);
	/*#1255
#if IMPLEMENTATION_FOR_MACOSX
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_SetWindowTransparency(0.1);
#endif
*/
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Hide();
	(dynamic_cast<AWindow_DiffInfo&>(GetApp().NVI_GetWindowByID(mDiffInfoWindowID))).SPI_SetTextWindowID(GetObjectID());
	
	//==================サブテキストシャドウ==================
	//#725
	{
		AWindowDefaultFactory<AWindow_SubTextShadow>	factory;
		mSubTextShadowWindowID = GetApp().NVI_CreateWindow(factory);
#if IMPLEMENTATION_FOR_WINDOWS
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_SetWindowTransparency(0.1);
#endif
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_ChangeToOverlay(GetObjectID(),true);
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_Create(kObjectID_Invalid);
		/*#1255
#if IMPLEMENTATION_FOR_MACOSX
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_SetWindowTransparency(0.1);
#endif
*/
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_Hide();
	}
	
	//==================ツールバー==================
	//#724
	//toolbar項目存在テーブル更新
	UpdateToolbarExistArray();
	//ツールバーのメニューを設定
	if( mToolbarExistArray.Find(kControlID_Toolbar_TextEncoding) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_TextEncoding,kTextArrayMenuID_TextEncoding);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_ReturnCode) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_ReturnCode,kTextArrayMenuID_ReturnCode);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_WrapMode) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_WrapMode,kTextArrayMenuID_WrapMode);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_Mode) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_Mode,kTextArrayMenuID_EnabledMode);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_SubWindows) != kIndex_Invalid )
	{
		NVI_SetControlMenu(kControlID_Toolbar_SubWindows,GetApp().NVI_GetMenuManager().GetContextMenu(kContextMenuID_SubWindows));
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_NewDocument) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_NewDocument,kTextArrayMenuID_EnabledMode);
	}
	if( mToolbarExistArray.Find(kControlID_Toolbar_Pref) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_Pref,kTextArrayMenuID_Pref);
	}
	/*#0 未使用のためコメントアウト
	if( mToolbarExistArray.Find(kControlID_Toolbar_Diff) != kIndex_Invalid )
	{
		NVI_SetControlMenu(kControlID_Toolbar_Diff,GetApp().NVI_GetMenuManager().GetContextMenu(kContextMenuID_Diff));
	}
	*/
	if( mToolbarExistArray.Find(kControlID_Toolbar_FindHighlight) != kIndex_Invalid )
	{
		NVI_RegisterTextArrayMenu(kControlID_Toolbar_FindHighlight,kTextArrayMenuID_TextMarker);
	}
	
	//==================ジャンプリスト==================
	//環境設定で表示状態ON（＝最終的に表示した状態）なら、デフォルトでフローティングジャンプリスト表示
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowFloatingJumpList) == true )
	{
		SPI_ShowHideJumpListWindow(true);
	}
	
	//==================その他==================
	
	//
	///*#199 mJumpListWindow*/SPI_GetJumpListWindow().NVI_Create(kObjectID_Invalid);
	
	//
	//mInfoWindow.NVI_Create(kObjectID_Invalid);
	//mInfoWindow.NVI_Show();
	
	//R0240
	NVI_EnableHelpTagCallback(kControlID_TabSelector);
	
	//#404 フルスクリーンモード適用
	//#476 if( mTabModeMainWindow == true )
	{
		ABool	fullScreenMode = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFullScreenMode);
		NVI_SetFullScreenMode(fullScreenMode);
	}
	
	//#602
	UpdateVSeparatorHelpTag();
	
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AWindow_Text::NVIDO_Create() ended.",this);
}

//#602
/**
カラム区切り線のヘルプタグ更新
*/
void	AWindow_Text::UpdateVSeparatorHelpTag()
{
	AText	text;
	if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		if( mLeftSideBarDisplayed == true )
		{
			text.SetLocalizedText("HelpTag_SubPane_Hide");
		}
		else
		{
			text.SetLocalizedText("HelpTag_SubPane_Show");
		}
		GetApp().NVI_GetWindowByID(mLeftSideBarVSeparatorWindowID).NVI_SetHelpTag(
					GetApp().NVI_GetWindowByID(mLeftSideBarVSeparatorWindowID).NVI_GetMainControlID(),text);
	}
	if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
	{
		if( mRightSideBarDisplayed == true )
		{
			text.SetLocalizedText("HelpTag_InfoPane_Hide");
		}
		else
		{
			text.SetLocalizedText("HelpTag_InfoPane_Show");
		}
		GetApp().NVI_GetWindowByID(mRightSideBarVSeparatorWindowID).NVI_SetHelpTag(
					GetApp().NVI_GetWindowByID(mRightSideBarVSeparatorWindowID).NVI_GetMainControlID(),text);
	}
}

/* B0402 NVMDO_DoTabActivated()でUpdateViewBounds()がコールされるのだから背後のタブについてUpdateViewBounds()する必要はない
void	AWindow_Text::SPI_UpdateViewBoundsAll( const AModeIndex inModeIndex )
{
	GetTabSelector().SPI_UpdateProperty();//R0177
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			UpdateViewBounds(tabIndex);
		}
	}
}
*/

//#379
/**
ViewBounds更新（inDocumentIDに対応するタブのみを更新）
*/
void	AWindow_Text::SPI_UpdateViewBounds( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid )   return;
	
	GetTabSelector().SPI_UpdateProperty();
	UpdateViewBounds(tabIndex);
}

void	AWindow_Text::SPI_UpdateViewBounds()
{
	GetTabSelector().SPI_UpdateProperty();//R0177 B0402 SPI_UpdateViewBoundsAll()から移動
	UpdateViewBounds(NVI_GetCurrentTabIndex());
}

/**
各Controlの位置・サイズ・Show/Hide制御

#246 画面リサイズ時等に、全タブに対して順にUpdateViewBounds()を行うことがあるので、
タブ共通ControlのControl制御はタブがアクティブの場合以外はやってはいけない
また、タブ共通Controlではなくても、Show/Hideの制御はタブがアクティブの場合以外はやってはいけない
*/
void	AWindow_Text::UpdateViewBounds( const AIndex inTabIndex, const AUpdateViewBoundsTriggerType inTriggerType )
{
	//
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	return UpdateViewBounds(inTabIndex,windowBounds,inTriggerType);//#341 #645
}
void	AWindow_Text::UpdateViewBounds( const AIndex inTabIndex, const ARect& windowBounds, 
		const AUpdateViewBoundsTriggerType inTriggerType )//#341 windowBoundsを指定できるように変更
{
	//ウインドウ幅・高さ取得
	ANumber	windowWidth = windowBounds.right - windowBounds.left;
	ANumber	windowHeight = windowBounds.bottom - windowBounds.top;
	
	//#675 UpdateViewBounds抑制状態中は何もしない
	if( mSuppressUpdateViewBoundsCounter > 0 )   return;
	
	//描画更新を停止（高速化）
    ACocoa::DisableScreenUpdates();
	
	/*#725 NVM_RegisterOverlayWindow()にて子オーバーレイウインドウ登録することで、AWindowで自動的にshow/hideすることにしたため、削除
	//ViewCloseボタンOverlayを全てHideする。#675 DoTabActivated()から移動
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
		{
			GetApp().NVI_GetWindowByID(mTabDataArray.GetObject(tabIndex).GetCloseViewButtonWindowID(v)).NVI_Hide();
		}
	}
	*/
	
	//#634
	ABool	panemodesupported = GetApp().SPI_IsSupportPaneMode();
	//現在のタブのモード取得
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	
	//フラグ等
	ABool	subTextPaneDisplayed = false;//#320
	
	//=============================================
	//　　サブウインドウShow/Hide更新
	//=============================================
	//サブウインドウのShow/Hide更新 #725
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		UpdateVisibleSubWindows();
	}
	
	//=============================================
	//　　レイアウト計算
	//=============================================
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(inTabIndex,layout);
	
	//=============================================
	//ウインドウ背景viewの配置
	//=============================================
	if( true )
	{
		//スクロールバーが交わっている部分だけ表示
		if( layout.h_MainHScrollbar > 0 || mRightSideBarDisplayed == false )
		{
			AWindowPoint	pt = {0,0};
			pt.x = layout.pt_VScrollbarArea.x -8;
			pt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - layout.h_Footer -8;
			NVI_SetPlainViewColor(kControlID_WindowBackground,kColor_White,kColor_White,false,!mRightSideBarDisplayed,false,false,kColor_White);
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_WindowBackground,pt);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_WindowBackground,true);
		}
		else
		{
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_WindowBackground,false);
		}
	}
	else
	{
		//ウインドウ全体表示
		AWindowPoint	pt = {0,0};
		pt.x = 0;
		pt.y = layout.h_MacroBar + layout.h_TabSelector;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_WindowBackground,pt);
		NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_WindowBackground,windowWidth,windowHeight
									   - (layout.h_MacroBar + layout.h_TabSelector + layout.h_Footer));
	}
	
	//=============================================
	//　　マクロバー配置
	//=============================================
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		UpdateLayout_MacroBar(inTabIndex,layout.pt_MacroBar,layout.w_MacroBar,layout.h_MacroBar);
	}
	
	//=============================================
	//　　サブテキストカラム配置
	//=============================================
	//サブテキストカラム・分割線配置（サブテキストカラムが存在している場合のみ） #725
	subTextPaneDisplayed = UpdateLayout_SubTextColumn(inTabIndex,layout);
	
	//=============================================
	//　　タブセレクタ配置
	//=============================================
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		if( layout.h_TabSelector > 0 )
		{
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_TabSelector,layout.pt_TabSelector);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_TabSelector,layout.w_TabSelector,layout.h_TabSelector);
		}
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_TabSelector,(layout.h_TabSelector>0));
	}
	
	//=============================================
	//　　ルーラー配置
	//=============================================
	if( true )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(inTabIndex).GetRulerControlID();
		if( layout.h_Ruler > 0 && inTabIndex == NVI_GetCurrentTabIndex() )
		{
			/*
			//ルーラー配置
			GetApp().NVI_GetWindowByID(rulerWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
			layout.pt_Ruler,layout.w_Ruler,layout.h_Ruler);
			//ルーラー表示
			NVI_SetOverlayWindowVisibility(rulerWindowID,true);
			*/
			//
			NVI_SetControlPositionIfCurrentTab(inTabIndex,controlID,layout.pt_Ruler);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,controlID,layout.w_Ruler,layout.h_Ruler);
			NVI_SetShowControl(controlID,true);
			//ルーラー更新
			UpdateRulerProperty(inTabIndex);
		}
		else
		{
			//ルーラー非表示
			//NVI_SetOverlayWindowVisibility(rulerWindowID,false);
			NVI_SetShowControl(controlID,false);
		}
	}
	
	/*
	//各TextViewの高さを決める。（従来の分割比を、現在のウインドウサイズに合わせて適用する。）
	ANumber	newheight = layout.h_MainColumn;//#725 windowBounds.bottom - windowBounds.top - kHScrollBarHeight - y;
	//#92 検索結果View
	if( mFindResultDocumentID != kObjectID_Invalid && mFindResultDisplayPosition == kDisplayPosition_MiddleBottom )
	{
		//View用Heightを小さくする
		newheight -= SPI_GetFindResultHeight() + kSeparatorHeight;//win+1;//#309
		layout.h_MainColumn -= SPI_GetFindResultHeight() + kSeparatorHeight+1;//★暫定
#if IMPLEMENTATION_FOR_MACOSX
		newheight--; //これがないと検索結果表示時にスクロールが重くなる(#309)
#endif
		//#291 検索結果View幅
		ANumber	findResultViewWidth = x2-x0;
		ANumber	findResultSeparatorWidth = x2-x0+kVScrollBarWidth;//#634
		if( mRightSideBarDisplayed == true && sideBarMode == false )//#634
		{
			findResultViewWidth -= mRightSideBarWidth;
			findResultSeparatorWidth -= mRightSideBarWidth+kVScrollBarWidth;
		}
		//FindResultSeparatorを配置
		AWindowPoint	sepapt;
		sepapt.x = x0;
		sepapt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - SPI_GetFindResultHeight() - kSeparatorHeight;
		GetApp().NVI_GetViewByID(mFindResultSeparatorViewID).NVI_SetPosition(sepapt);
		GetApp().NVI_GetViewByID(mFindResultSeparatorViewID).NVI_SetSize(findResultSeparatorWidth,kSeparatorHeight);//#634
		//検索結果用IndexViewを配置
		AWindowPoint	frpt;
		frpt.x = x0;
		frpt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - SPI_GetFindResultHeight();
		GetApp().NVI_GetViewByID(mFindResultViewID).NVI_SetPosition(frpt);
		GetApp().NVI_GetViewByID(mFindResultViewID).NVI_SetSize(findResultViewWidth,SPI_GetFindResultHeight());
		//VScrollBarを配置
		AWindowPoint	scpt;
		scpt.x = windowBounds.right - windowBounds.left - kVScrollBarWidth +1;
		scpt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - SPI_GetFindResultHeight() +1;
		if( mRightSideBarDisplayed == true && sideBarMode == true )//#634
		{
			scpt.x = frpt.x + findResultViewWidth;
		}
		NVI_SetControlPosition(mFindResultVScrollBarControlID,scpt);
		NVI_SetControlSize(mFindResultVScrollBarControlID,kVScrollBarWidth,SPI_GetFindResultHeight());
		//閉じるボタン
		AWindowPoint	cbpt;
		cbpt.x = x0+ 2;
		cbpt.y = scpt.y + 1;
		GetApp().NVI_GetWindowByID(mFindResultCloseButtonWindowID).NVI_SetOffsetOfOverlayWindowAndSize(
				GetObjectID(),cbpt,kVScrollBarWidth,kSplitButtonHeight);
		GetApp().NVI_GetWindowByID(mFindResultCloseButtonWindowID).NVI_Show(false);
		//これをやると、スクロール時に×ボタンもスクロールされてしまうNVI_EmbedControl(GetApp().NVI_GetViewByID(mFindResultViewID).NVI_GetControlID(),mFindResultCloseButtonControlID);//win
	}
	*/
	
	//=============================================
	//　　TextView配置
	//=============================================
	if( inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_SubPaneHSeparator )//#645
	{
		//現在のtab indexの場合のみ
		if( NVI_GetCurrentTabIndex() == inTabIndex )
		{
			UpdateLayout_TextView(inTabIndex,layout.pt_MainColumn,layout.w_MainColumn,layout.h_MainColumn,
								  layout.w_MainTextView,layout.w_LineNumber,layout.w_MainTextMargin,layout.pt_VScrollbarArea);
		}
	}
	
	//=============================================
	//　　左サイドバー配置
	//=============================================
	//#212 サブペインの表示／非表示、サイズ、位置設定
	//#645 サブペインの位置・サイズ設定(NVI_SetOffsetOfOverlayWindowAndSize()等)が必要かどうかの判定
	/*
	if( NVI_IsWindowVisible() == true && inTabIndex == NVI_GetCurrentTabIndex() && panemodesupported == true &&
				inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneVSeparator &&
	inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneVSeparator_Completed )
	*/
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		if( NVI_IsWindowVisible() == true && inTabIndex == NVI_GetCurrentTabIndex() && panemodesupported == true )
		{
			if( UpdateLayout_LeftSideBar(inTabIndex,layout) == true )
			{
				subTextPaneDisplayed = true;
			}
		}
	}
	
	//#320
	//サブテキストペインを表示しなかった場合、サブテキストペインとボタンを非表示にする
	if( subTextPaneDisplayed == false )
	{
		//サブペインモードボタン非表示
		//#379 NVI_SetShowControl(kSubPaneModeButtonControlID,false);
		//サブペインファイル名ボタン非表示
		NVI_SetShowControl(kSubPaneFileNameButtonControlID,false);
		//サブペイン入替ボタン
		NVI_SetShowControl(kSubPaneSwapButtonControlID,false);
		//サブペイン同期ボタン
		NVI_SetShowControl(kSubPaneSyncButtonControlID,false);
		//サブペイン比較ボタン #379
		NVI_SetShowControl(kSubPaneDiffButtonControlID,false);
		//SubTextPane非表示
		AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetSubTextControlID();
		if( textViewControlID != kControlID_Invalid )
		{
			//サブテキストのテキストビュー、行番号ビューを非表示
			NVI_SetShowControl(textViewControlID,false);
			AControlID	lineNumberControlID = mTabDataArray.GetObject(inTabIndex).GetSubTextLineNumberControlID();
			NVI_SetShowControl(lineNumberControlID,false);
			//#725
			//サブテキストの水平スクロールバーを非表示
			AControlID	hScrollbarControlID = mTabDataArray.GetObject(inTabIndex).GetSubTextHScrollbarControlID();
			NVI_SetShowControl(hScrollbarControlID,false);
		}
	}
	
	//=============================================
	//　　右サイドバー配置
	//=============================================
	//#291 右サイドバー
	//#645 右サイドバーの位置・サイズ設定(NVI_SetOffsetOfOverlayWindowAndSize()等)が必要かどうかの判定
	/*
	if( NVI_IsWindowVisible() == true && inTabIndex == NVI_GetCurrentTabIndex() && panemodesupported == true &&
				inTriggerType != kUpdateViewBoundsTriggerType_SubPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_ShowLeftSideBar &&
	inTriggerType != kUpdateViewBoundsTriggerType_HideLeftSideBar )
	*/
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		if( NVI_IsWindowVisible() == true && inTabIndex == NVI_GetCurrentTabIndex() && panemodesupported == true )
		{
			UpdateLayout_RightSideBar(inTabIndex,layout);
		}
	}
	
	//=============================================
	//　　フッター配置
	//=============================================
	
	if( inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		//ステータスバー表示判定
		ABool	showStatusBar = ( layout.h_Footer > 0 );
		
		//-------------フッター（区切り線）配置------------
		if( showStatusBar == true )
		{
			//ステータスバー表示時
			//フッター表示する
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_Footer,layout.pt_Footer);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_Footer,layout.w_Footer,1);//layout.h_Footer);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_Footer,true);
		}
		else
		{
			//ステータスバー非表示時
			//フッター表示しない
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_Footer,false);
		}
		
		//フッター内各ボタンのy位置オフセット
		const ANumber	kFooterButtonsYOffset = 2;
		
		/*#フッター内の区切り線
		//
		{
			AWindowPoint	pt = layout.pt_Footer;
			pt.x = layout.pt_MainColumn.x;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn,pt);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn,kWidth_FooterSeparator,layout.h_Footer);
			if( mLeftSideBarDisplayed == true || mSubTextColumnDisplayed == true )
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn,false);
			}
			else
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn,false);
			}
		}
		//
		{
			AWindowPoint	pt = layout.pt_Footer;
			pt.x = layout.pt_MainColumn.x + kWidth_FooterSeparator+ kLineNumberButtonWidth;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn2,pt);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn2,kWidth_FooterSeparator,layout.h_Footer);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_MainColumn2,false);
		}
		//
		{
			AWindowPoint	pt = layout.pt_Footer;
			pt.x = layout.pt_SubTextPaneColumn.x;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn,pt);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn,kWidth_FooterSeparator,layout.h_Footer);
			if( mSubTextColumnDisplayed == true && mLeftSideBarDisplayed == true )
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn,false);
			}
			else
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn,false);
			}
		}
		//
		{
			AWindowPoint	pt = layout.pt_Footer;
			pt.x = layout.pt_SubTextPaneColumn.x + kWidth_FooterSeparator + kLineNumberButtonWidth;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn2,pt);
			NVI_SetControlSizeIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn2,kWidth_FooterSeparator,layout.h_Footer);
			if( mSubTextColumnDisplayed == true )
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn2,false);
			}
			else
			{
				NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_FooterSeparator_SubTextColumn2,false);
			}
		}
		*/
		
		if( showStatusBar == true )
		{
			//------------------ステータスバー表示時------------------
			
			//-------------右サイドバー制御ボタン配置-------------
			
			if( mRightSideBarDisplayed == true )
			{
				//-------------右サイドバー表示時-------------
				
				//右サイドバー区切りボタン
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_RightSideBar.x;// + kWidth_FooterSeparator;
					pt.y = layout.pt_Footer.y +kFooterButtonsYOffset;
					ANumber	w = 64;//layout.w_RightSideBar - kWidth_SizeBox - kWidth_RightSideBar_PrefButton - kWidth_RightSideBar_ShowHideButton;
					NVI_SetControlPosition(kControlID_RightSideBar_SeparatorButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_SeparatorButton,w,layout.h_Footer);
					NVI_SetShowControl(kControlID_RightSideBar_SeparatorButton,true);
				}
				//右サイドバー表示・非表示ボタン
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_RightSideBar.x + layout.w_RightSideBar - kWidth_SizeBox - kWidth_RightSideBar_PrefButton - kWidth_RightSideBar_ShowHideButton;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_RightSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_ShowHideButton,kWidth_RightSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_RightSideBar_ShowHideButton,true);
				}
				//右サイドバー環境設定ボタン
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_RightSideBar.x + layout.w_RightSideBar - kWidth_SizeBox - kWidth_RightSideBar_PrefButton;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_RightSideBar_PrefButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_PrefButton,kWidth_RightSideBar_PrefButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_RightSideBar_PrefButton,true);
				}
				//
				mFooterProgressRightPosition = layout.pt_MainColumn.x + layout.w_MainColumn - kWidth_SizeBox;
			}
			else
			{
				//-------------右サイドバー非表示時-------------
				
				//右サイドバー区切りボタン非表示
				NVI_SetShowControl(kControlID_RightSideBar_SeparatorButton,false);
				//右サイドバー表示・非表示ボタン
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_MainColumn.x + layout.w_MainColumn - kWidth_SizeBox - kWidth_RightSideBar_ShowHideButton;
					pt.y = layout.pt_Footer.y +kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_RightSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_ShowHideButton,kWidth_RightSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_RightSideBar_ShowHideButton,true);
				}
				//右サイドバー環境設定ボタン
				{
					NVI_SetShowControl(kControlID_RightSideBar_PrefButton,false);
					/*
					AWindowPoint	pt = {0};
					pt.x = inLayout.pt_MainColumn.x + inLayout.w_MainColumn - kWidth_SizeBox - kWidth_RightSideBar_PrefButton;
					pt.y = inLayout.pt_Footer.y +kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_RightSideBar_PrefButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_PrefButton,kWidth_RightSideBar_PrefButton,layout.h_Footer);
					NVI_SetShowControl(kControlID_RightSideBar_PrefButton,true);
					*/
				}
				//
				mFooterProgressRightPosition = layout.pt_MainColumn.x + layout.w_MainColumn
						- kWidth_SizeBox - kWidth_RightSideBar_ShowHideButton;
			}
			
			//-------------左サイドバー制御ボタン配置-------------
			
			if( mLeftSideBarDisplayed == true )
			{
				//-------------左サイドバー表示時-------------
				
				//左サイドバー区切りボタン
				{
					ANumber	w = layout.w_LeftSideBar - kWidth_LeftSideBar_PrefButton - kWidth_LeftSideBar_ShowHideButton;
					AWindowPoint	pt = {0};
					pt.x = layout.pt_LeftSideBar.x + kWidth_LeftSideBar_PrefButton + kWidth_LeftSideBar_ShowHideButton;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_SeparatorButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_SeparatorButton,w,layout.h_Footer);
					NVI_SetShowControl(kControlID_LeftSideBar_SeparatorButton,true);
					NVI_GetButtonViewByControlID(kControlID_LeftSideBar_SeparatorButton).SPI_SetButtonValidXRange(w-40,w);
				}
				/*#1184
				//左サイドバー表示・非表示ボタン
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_LeftSideBar.x + kWidth_LeftSideBar_PrefButton;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
				}
				*/
				//左サイドバー環境設定ボタン
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_LeftSideBar.x;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_PrefButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_PrefButton,kWidth_LeftSideBar_PrefButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_LeftSideBar_PrefButton,true);
				}
			}
			else
			{
				//-------------左サイドバー非表示時-------------
				
				//左サイドバー区切りボタン非表示
				NVI_SetShowControl(kControlID_LeftSideBar_SeparatorButton,false);
				/*#1184
				//左サイドバー表示・非表示ボタン
				if( mSubTextColumnDisplayed == true )
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_SubTextPaneColumn.x;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
				}
				else
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_MainColumn.x;
					pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
					NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
				}
				*/
				//左サイドバー環境設定ボタン
				{
					NVI_SetShowControl(kControlID_LeftSideBar_PrefButton,false);
					/*
					AWindowPoint	pt = {0};
					pt.x = inLayout.pt_MainColumn.x + inLayout.w_MainColumn - kWidth_SizeBox - kWidth_LeftSideBar_PrefButton;
					pt.y = inLayout.pt_Footer.y + kFooterButtonsYOffset;
					NVI_SetControlPosition(kControlID_LeftSideBar_PrefButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_PrefButton,kWidth_LeftSideBar_PrefButton,layout.h_Footer);
					NVI_SetShowControl(kControlID_LeftSideBar_PrefButton,true);
					*/
				}
			}
			//#1184
			//左サイドバー表示・非表示ボタン
			if( mSubTextColumnDisplayed == true )
			{
				AWindowPoint	pt = {0};
				pt.x = layout.pt_SubTextPaneColumn.x;
				pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
				NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
				NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
				NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
			}
			else
			{
				AWindowPoint	pt = {0};
				pt.x = layout.pt_MainColumn.x;
				pt.y = layout.pt_Footer.y + kFooterButtonsYOffset;
				NVI_SetControlPosition(kControlID_LeftSideBar_ShowHideButton,pt);
				NVI_SetControlSize(kControlID_LeftSideBar_ShowHideButton,kWidth_LeftSideBar_ShowHideButton,layout.h_Footer-2);
				NVI_SetShowControl(kControlID_LeftSideBar_ShowHideButton,true);
			}
		}
		else
		{
			//------------------ステータスバー非表示時------------------
			
			if( mRightSideBarDisplayed == true )
			{
				//右サイドバー区切りボタン
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_RightSideBar.x;
					pt.y = windowHeight -20;
					NVI_SetControlPosition(kControlID_RightSideBar_SeparatorButton,pt);
					NVI_SetControlSize(kControlID_RightSideBar_SeparatorButton,64,20);
					NVI_SetShowControl(kControlID_RightSideBar_SeparatorButton,true);
				}
			}
			else
			{
				NVI_SetShowControl(kControlID_RightSideBar_SeparatorButton,false);
			}
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_RightSideBar_ShowHideButton,false);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_RightSideBar_PrefButton,false);
			//
			if( mLeftSideBarDisplayed == true )
			{
				//左サイドバー区切りボタン
				{
					AWindowPoint	pt = {0};
					pt.x = layout.pt_LeftSideBar.x + layout.w_LeftSideBar -65;
					pt.y = windowHeight -20;
					NVI_SetControlPosition(kControlID_LeftSideBar_SeparatorButton,pt);
					NVI_SetControlSize(kControlID_LeftSideBar_SeparatorButton,64,20);
					NVI_SetShowControl(kControlID_LeftSideBar_SeparatorButton,true);
				}
			}
			else
			{
				NVI_SetShowControl(kControlID_LeftSideBar_SeparatorButton,false);
			}
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LeftSideBar_ShowHideButton,false);
			NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LeftSideBar_PrefButton,false);
		}
		
		//=============================================
		//　　下部各コントロール配置
		//=============================================
		
		//各行番号・位置ボタン配置
		UpdateLayout_LineNumberAndPositionButton(inTabIndex,layout);
		
		//フッター内プログレス表示更新
		UpdateFooterProgress();
		
		//メインHスクロールバー配置
		AControlID	hscrollbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetHScrollbarControlID();
		if( layout.h_MainHScrollbar > 0 )
		{
			AWindowPoint	pt = {0};
			pt.x = layout.pt_MainColumn.x;
			pt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight - layout.h_Footer;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,hscrollbarControlID,pt);
			ANumber	scrollwidth = windowWidth - layout.w_RightSideBar - layout.pt_MainColumn.x - kVScrollBarWidth;
			NVI_SetControlSizeIfCurrentTab(inTabIndex,hscrollbarControlID,scrollwidth,kHScrollBarHeight);
			NVI_SetShowControlIfCurrentTab(inTabIndex,hscrollbarControlID,true);
		}
		else
		{
			NVI_SetShowControlIfCurrentTab(inTabIndex,hscrollbarControlID,false);
		}
		
		//サイズボックス配置
#if IMPLEMENTATION_FOR_WINDOWS
		{
			AWindowPoint	pt = {0};
			pt.x = windowWidth - kControlID_SizeBox;
			pt.y = windowBounds.bottom - windowBounds.top - kHScrollBarHeight;
			NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_SizeBox,pt);
		}
#endif
	}
	
	//タブ表示更新
	if( inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneVSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_InfoPaneVSeparator_Completed &&
				inTriggerType != kUpdateViewBoundsTriggerType_SubPaneHSeparator &&
				inTriggerType != kUpdateViewBoundsTriggerType_ShowLeftSideBar &&
				inTriggerType != kUpdateViewBoundsTriggerType_HideLeftSideBar &&//#645
				inTriggerType != kUpdateViewBoundsTriggerType_SubTextColumnVSeparator )
	{
		//R0228
		GetTabSelector().SPI_UpdateProperty();
	}
	
	//#505
	//ウインドウ最小サイズ設定
	//最小幅
	ANumber	minWidth = kMinWidth_MainColumn;
	if( mRightSideBarDisplayed == true )
	{
		minWidth += mRightSideBarWidth;
	}
	if( mLeftSideBarDisplayed == true )
	{
		minWidth += mLeftSideBarWidth;
	}
	if( mSubTextColumnDisplayed == true )
	{
		minWidth += mSubTextColumnWidth;
	}
	//最小高さ
	ANumber	minHeight = kMinHeight_MainColumn;
	minHeight += layout.h_MacroBar + layout.h_TabSelector + layout.h_Ruler + layout.h_Footer;
	//左サイドバーの最小高さのほうが大きければ、そちらを採用
	if( mLeftSideBarDisplayed == true )
	{
		ANumber	leftSideBarMinHeight = layout.h_MacroBar + layout.h_TabSelector + layout.h_Footer + 32;
		for( AIndex i = 0; i < mLeftSideBarArray_Type.GetItemCount(); i++ )
		{
			leftSideBarMinHeight += GetApp().SPI_GetSubWindowMinHeight(mLeftSideBarArray_OverlayWindowID.Get(i));
		}
		if( leftSideBarMinHeight > minHeight )
		{
			minHeight = leftSideBarMinHeight;
		}
	}
	//右サイドバーの最小高さのほうが大きければ、そちらを採用
	if( mRightSideBarDisplayed == true )
	{
		ANumber	rightSideBarMinHeight = layout.h_MacroBar + layout.h_TabSelector + layout.h_Footer + 32;
		for( AIndex i = 0; i < mRightSideBarArray_Type.GetItemCount(); i++ )
		{
			rightSideBarMinHeight += GetApp().SPI_GetSubWindowMinHeight(mRightSideBarArray_OverlayWindowID.Get(i));
		}
		if( rightSideBarMinHeight > minHeight )
		{
			minHeight = rightSideBarMinHeight;
		}
	}
	//最小設定
	NVI_SetWindowMinimumSize(minWidth,minHeight);
	
	//#519 Hスクロールバーの最大最小・現在値更新
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		GetTextViewByControlID(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).
					GetTextViewControlID(0)).NVI_UpdateScrollBar();
	}
	
	//描画更新停止解除
	ACocoa::EnableScreenUpdates();
}

//#725
/**
4paneモードかどうかを取得
*/
ABool	AWindow_Text::Is4PaneMode() const
{
	ABool	fourpaneMode = false;
	switch(mLayoutMode)
	{
	  case kLayoutMode_4Pane_Header11110110:
	  case kLayoutMode_4Pane_Header11110111:
	  case kLayoutMode_4Pane_Header01110111:
	  case kLayoutMode_4Pane_Header11111111:
		{
			fourpaneMode = true;
			break;
		}
	  default:
		{
			//処理無し
			//（3paneモード）
			break;
		}
	}
	return fourpaneMode;
}

//#725
/**
レイアウト計算
*/
void	AWindow_Text::CalculateLayout( const AIndex inTabIndex, ATextWindowLayoutData& outLayout ) 
{
	//縦書きモード取得 #558
	ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetVerticalMode();
	
	//
	ABool	panemodesupported = GetApp().SPI_IsSupportPaneMode();
	//mode index取得
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	
	//ウインドウ幅・高さ取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	ANumber	windowWidth = windowBounds.right - windowBounds.left;
	ANumber	windowHeight = windowBounds.bottom - windowBounds.top;
	
	//フッターサイズ（表示有無）取得
	outLayout.h_Footer = kHeight_Footer;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
	{
		outLayout.h_Footer = 0;
	}
	
	//============= 左サイドバー カラム 計算(1) =============
	outLayout.w_LeftSideBar = 0;
	outLayout.h_LeftSideBar = 0;
	outLayout.pt_LeftSideBar.x = 0;
	outLayout.pt_LeftSideBar.y = 0;
	//左サイドバー表示／非表示判定
	if( mLeftSideBarDisplayed == true )
	{
		//w_LeftSideBarには区切り線部分の幅も含める
		//そのため、サブウインドウの幅はw_LeftSideBarからk_wLeftSideBarVisibleAreaを引いた値を設定する
		outLayout.w_LeftSideBar = mLeftSideBarWidth;
		outLayout.h_LeftSideBarWithMargin = windowHeight - outLayout.h_Footer;
		if( outLayout.h_Footer == 0 )
		{
			outLayout.h_LeftSideBar = outLayout.h_LeftSideBarWithMargin -22;
		}
		else
		{
			outLayout.h_LeftSideBar = outLayout.h_LeftSideBarWithMargin;
		}
		//マクロバー、タブの計算後に、左サイドバーカラム再度計算
	}
	
	//============= マクロバー計算 =============
	outLayout.w_MacroBar = 0;
	outLayout.h_MacroBar = 0;
	outLayout.pt_MacroBar.x = 0;
	outLayout.pt_MacroBar.y = 0;
	//マクロバー表示／非表示判定
	{
		//マクロバー幅
		outLayout.w_MacroBar = windowWidth;
		//レイアウトモードによってマクロバー幅設定
		switch(mLayoutMode)
		{
		  case kLayoutMode_4Pane_Header01110111:
		  case kLayoutMode_3Pane_Header01110111:
			{
				//マクロバーは左サイドバーの右端～ウインドウの右端
				outLayout.pt_MacroBar.x		+= outLayout.w_LeftSideBar;
				outLayout.w_MacroBar		-= outLayout.w_LeftSideBar;
				break;
			}
		  default:
			{
				//処理無し
				//（マクロバーはウインドウ幅いっぱい）
				break;
			}
		}
	}
	//マクロバー高さ設定
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowMacroBar) == true &&
				NVI_IsToolBarShown() == true )
	{
		outLayout.h_MacroBar = CalculateMacroBarHeight(inTabIndex,outLayout.pt_MacroBar,outLayout.w_MacroBar);
	}
	else
	{
		//分割線を表示するため、高さ1のマクロバーを表示する（
		outLayout.h_MacroBar = 1;
	}
	//============= タブselector計算 =============
	outLayout.w_TabSelector = 0;
	outLayout.h_TabSelector = 0;
	outLayout.pt_TabSelector.x = 0;
	outLayout.pt_TabSelector.y = 0;
	//タブ数が2以上ならタブselectorを表示
	if( NVI_GetSelectableTabCount() >= 2 )
	{
		//タブselector位置・幅
		outLayout.pt_TabSelector.x = 0;
		outLayout.pt_TabSelector.y = outLayout.h_MacroBar;
		outLayout.w_TabSelector = windowWidth;
		//レイアウトモードによってタブselector位置・幅設定
		switch(mLayoutMode)
		{
		  case kLayoutMode_4Pane_Header11110110:
		  case kLayoutMode_3Pane_Header11110110:
			{
				//タブは左サイドバーの右端～右サイドバーの左端
				outLayout.pt_TabSelector.x	+= outLayout.w_LeftSideBar;
				outLayout.w_TabSelector		-= outLayout.w_LeftSideBar + mRightSideBarWidth;
				break;
			}
		  case kLayoutMode_4Pane_Header11110111:
		  case kLayoutMode_3Pane_Header11110111:
		  case kLayoutMode_4Pane_Header01110111:
		  case kLayoutMode_3Pane_Header01110111:
			{
				//タブは左サイドバーの右端～ウインドウの右端
				outLayout.pt_TabSelector.x	+= outLayout.w_LeftSideBar;
				outLayout.w_TabSelector		-= outLayout.w_LeftSideBar;
				break;
			}
		  default:
			{
				//処理無し
				//（タブはウインドウ幅いっぱい）
				break;
			}
		}
		//タブselector高さ設定
		outLayout.h_TabSelector = GetTabSelectorConst().SPI_GetHeight(outLayout.w_TabSelector);
	}
	//============= ヘッダー部分高さ計算 =============
	//左サイドバーのヘッダー高さ計算
	ANumber	leftSideBar_headerHeight = 0;
	switch(mLayoutMode)
	{
		//1-1
	  case kLayoutMode_4Pane_Header11111111:
	  case kLayoutMode_3Pane_Header11111111:
		{
			leftSideBar_headerHeight += outLayout.h_MacroBar + outLayout.h_TabSelector;
			break;
		}
		//1-0
	  case kLayoutMode_4Pane_Header11110110:
	  case kLayoutMode_3Pane_Header11110110:
	  case kLayoutMode_4Pane_Header11110111:
	  case kLayoutMode_3Pane_Header11110111:
		{
			leftSideBar_headerHeight += outLayout.h_MacroBar;
			break;
		}
		//0-0
	  default:
		{
			//処理無し
			break;
		}
	}
	//メイン部分のヘッダー高さ計算
	ANumber	main_headerHeight = outLayout.h_MacroBar + outLayout.h_TabSelector;
	//右サイドバーのヘッダー高さ計算
	ANumber	rightSideBar_headerHeight = 0;
	switch(mLayoutMode)
	{
		//1-1
	  case kLayoutMode_4Pane_Header11110111:
	  case kLayoutMode_3Pane_Header11110111:
	  case kLayoutMode_4Pane_Header01110111:
	  case kLayoutMode_3Pane_Header01110111:
	  case kLayoutMode_4Pane_Header11111111:
	  case kLayoutMode_3Pane_Header11111111:
		{
			rightSideBar_headerHeight += outLayout.h_MacroBar + outLayout.h_TabSelector;
			break;
		}
		//1-0
	  case kLayoutMode_4Pane_Header11110110:
	  case kLayoutMode_3Pane_Header11110110:
		{
			rightSideBar_headerHeight += outLayout.h_MacroBar;
			break;
		}
		//0-0
	  default:
		{
			//処理無し
			break;
		}
	}
	
	//============= 左サイドバー カラム 計算(2) =============
	//左サイドバーにヘッダ部分高さ計算結果反映
	if( outLayout.w_LeftSideBar > 0 )
	{
		outLayout.pt_LeftSideBar.y 	+= leftSideBar_headerHeight;
		outLayout.h_LeftSideBar		-= leftSideBar_headerHeight;
	}
	//左サイドバー分割線 計算
	outLayout.pt_VSeparator_LeftSideBar.x = outLayout.pt_LeftSideBar.x + outLayout.w_LeftSideBar;
	outLayout.pt_VSeparator_LeftSideBar.y = outLayout.pt_LeftSideBar.y;
	if( outLayout.w_LeftSideBar > 0 )
	{
		outLayout.pt_VSeparator_LeftSideBar.x -= kSeparatorWidth;
	}
	
	//============= サブテキスト カラム =============
	//水平スクロールバー高さ取得 #457
	outLayout.h_subHScrollbar = kHScrollBarHeight;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowHScrollBar) == false )
	{
		outLayout.h_subHScrollbar = 0;
	}
	//4Paneの場合、カラムとしてサブテキストペインを表示する。
	outLayout.w_SubTextPaneColumn = 0;
	outLayout.h_SubTextPaneColumn = 0;
	outLayout.pt_SubTextPaneColumn.x = 0;
	outLayout.pt_SubTextPaneColumn.y = 0;
	if( Is4PaneMode() == true && mSubTextColumnDisplayed == true )
	{
		outLayout.w_SubTextPaneColumn = mSubTextColumnWidth;
		outLayout.h_SubTextPaneColumn = windowHeight - main_headerHeight - outLayout.h_subHScrollbar - outLayout.h_Footer;
		outLayout.pt_SubTextPaneColumn.x = outLayout.w_LeftSideBar;
		outLayout.pt_SubTextPaneColumn.y = main_headerHeight;
	}
	
	//============= ルーラー =============
	outLayout.pt_Ruler.x = 0;
	outLayout.pt_Ruler.y = 0;
	outLayout.w_Ruler = 0;
	outLayout.h_Ruler = 0;
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayRuler) == true && verticalMode == false )//#558 縦書きモード時はルーラー表示しない
	{
		outLayout.pt_Ruler.x = outLayout.w_LeftSideBar + outLayout.w_SubTextPaneColumn;
		outLayout.pt_Ruler.y = main_headerHeight;
		outLayout.w_Ruler = windowWidth - outLayout.pt_Ruler.x;
		outLayout.h_Ruler = kRulerHeight;
	}
	
	//============= メイン カラム =============
	//メインカラム
	outLayout.pt_MainColumn.x = outLayout.w_LeftSideBar + outLayout.w_SubTextPaneColumn;
	outLayout.pt_MainColumn.y = main_headerHeight + outLayout.h_Ruler;
	outLayout.w_MainColumn = windowWidth - outLayout.pt_MainColumn.x - kVScrollBarWidth;
	outLayout.h_MainColumn = windowHeight - main_headerHeight - outLayout.h_Ruler - kHScrollBarHeight - outLayout.h_Footer;
	//メインビュー用VScrollbar領域位置設定
	outLayout.pt_VScrollbarArea.x = windowWidth - kVScrollBarWidth;
	outLayout.pt_VScrollbarArea.y = main_headerHeight;
	//行番号ビュー幅設定
	outLayout.w_LineNumber = GetLineNumberWidth(inTabIndex);//#450
	
	//============= 右サイドバー カラム =============
	//右サイドバーカラム
	outLayout.w_RightSideBar = 0;
	outLayout.h_RightSideBar_WithBottomMargin = windowHeight - rightSideBar_headerHeight - outLayout.h_Footer;// - kHeight_RightSidebarControlButtons;
	if( outLayout.h_Footer == 0 )
	{
		outLayout.h_RightSideBar = outLayout.h_RightSideBar_WithBottomMargin - 22;
	}
	else
	{
		outLayout.h_RightSideBar = outLayout.h_RightSideBar_WithBottomMargin;
	}
	outLayout.pt_RightSideBar.x = 0;
	outLayout.pt_RightSideBar.y = 0;
	outLayout.pt_VSeparator_RightSideBar.x = 0;
	outLayout.pt_VSeparator_RightSideBar.y = 0;
	if( panemodesupported == true )
	{
		if( mRightSideBarDisplayed == true )
		{
			//右サイドバー・分割線
			outLayout.w_RightSideBar = mRightSideBarWidth;
			outLayout.pt_RightSideBar.x = windowWidth - outLayout.w_RightSideBar;
			outLayout.pt_RightSideBar.y = rightSideBar_headerHeight;
			outLayout.pt_VSeparator_RightSideBar.x = outLayout.pt_RightSideBar.x;
			outLayout.pt_VSeparator_RightSideBar.y = outLayout.pt_RightSideBar.y;
		}
		else
		{
			//右サイドバー分割線
			outLayout.pt_VSeparator_RightSideBar.x = windowWidth - kSeparatorWidth;
			outLayout.pt_VSeparator_RightSideBar.y = rightSideBar_headerHeight;
		}
		//メインビュー等の幅補正
		outLayout.w_Ruler -= outLayout.w_RightSideBar;
		outLayout.w_MainColumn -= outLayout.w_RightSideBar;
		outLayout.pt_VScrollbarArea.x -= outLayout.w_RightSideBar;
	}
	//メインtext viewの幅設定
	outLayout.w_MainTextView = outLayout.w_MainColumn - outLayout.w_LineNumber;
	//メインh scrollbarの高さ設定
	outLayout.h_MainHScrollbar = kHScrollBarHeight;
	if( mTabDataArray.GetObjectConst(inTabIndex).GetViewCount() > 0 && verticalMode == false )//#558 縦書きモードは常にHスクロールバー表示
	{
		//text viewのイメージサイズ幅がtext viewの幅以下なら、hスクロールバーは表示しない
		if( GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).NVM_GetImageWidth() <=
					outLayout.w_MainTextView )
		{
			outLayout.h_MainColumn += outLayout.h_MainHScrollbar;
			outLayout.h_MainHScrollbar = 0;
		}
	}
	//水平スクロールバー非表示設定の場合の処理 #457
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowHScrollBar) == false )
	{
		outLayout.h_MainColumn += outLayout.h_MainHScrollbar;
		outLayout.h_MainHScrollbar = 0;
	}
	//余白計算 #1186
	outLayout.w_MainTextDisplayWidth = outLayout.w_MainTextView;
	outLayout.w_MainTextMargin = 0;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayLeftRightMargin) == true )
	{
		outLayout.w_MainTextDisplayWidth = GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kTextWidthForLeftRightMargin);
		ANumber	a = outLayout.w_LineNumber + outLayout.w_MainTextDisplayWidth;
		if( verticalMode == false )
		{
			outLayout.w_MainTextMargin = (outLayout.w_MainColumn - a)/2;
			if( outLayout.w_MainTextMargin > 0 )
			{
				outLayout.w_LineNumber += outLayout.w_MainTextMargin;
				outLayout.w_MainTextView -= outLayout.w_MainTextMargin;
			}
			else
			{
				outLayout.w_MainTextMargin = 0;
			}
		}
		else
		{
			outLayout.w_MainTextMargin = (outLayout.h_MainColumn - a)/2;
			if( outLayout.w_MainTextMargin > 0 )
			{
				outLayout.w_LineNumber += outLayout.w_MainTextMargin;
				outLayout.w_MainTextView -= outLayout.w_MainTextMargin;
			}
			else
			{
				outLayout.w_MainTextMargin = 0;
			}
		}
	}
	
	//============= フッター =============
	outLayout.pt_Footer.x = 0;
	outLayout.pt_Footer.y = windowHeight - outLayout.h_Footer;
	outLayout.w_Footer = windowWidth;
}

//#450
/**
行番号view幅取得
*/
ANumber	AWindow_Text::GetLineNumberWidth( const AIndex inTabIndex ) const
{
	AItemCount	lineCount = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetLineCount();
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	ANumber	width = AView_LineNumber::kSeparatorOffset + AView_LineNumber::kRightMargin;
	if( mTabDataArray.GetObjectConst(inTabIndex).GetViewCount() > 0 )
	{
		//1 digitの幅取得
		ANumber	digitWidth = SPI_GetLineNumberViewConst(inTabIndex,0).SPI_Get1DigitWidth();
		//行数によって、幅計算
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber) == true )
		{
			ANumber	numwidth = digitWidth*5 + 5;
			if( lineCount > 1000000 )
			{
				numwidth += digitWidth*2;
			}
			else if( lineCount > 100000 )
			{
				numwidth += digitWidth*1;
			}
			width += AView_LineNumber::kFoldingMarkLeftMargin + AView_LineNumber::kFoldingMarkSize + numwidth;
		}
	}
	return width;
}

//#725
/**
メインtext view配置
*/
void	AWindow_Text::UpdateLayout_TextView( const AIndex inTabIndex, 
		const AWindowPoint pt_MainColumn_origin, const ANumber w_MainColumn, const ANumber h_MainColumn,
											 const ANumber w_TextView, const ANumber w_LineNumber, 
											 const ANumber w_MainTextMargin, //#1186
											 AWindowPoint pt_VScrollbarArea_origin )
{
	//縦書きモード取得 #558
	ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetVerticalMode();
	
	//#675 UpdateViewBounds抑制状態中は何もしない
	//#1090 UpdateViewBounds抑制状態中は画面外に配置する
	AWindowPoint pt_MainColumn = pt_MainColumn_origin;
	AWindowPoint	pt_VScrollbarArea = pt_VScrollbarArea_origin;
	if( mSuppressUpdateViewBoundsCounter > 0 )   //return;
	{
		pt_MainColumn.x = -10000;
		pt_MainColumn.y = -10000;
		pt_VScrollbarArea.x = -10000;
		pt_VScrollbarArea.y = -10000;
	}
	
	//
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//各text viewの高さ補正
	ANumber	viewsheight = 0;
	for( AIndex v = 0; v < mTabDataArray.GetObject(inTabIndex).GetViewCount(); v++ )
	{
		viewsheight += mTabDataArray.GetObject(inTabIndex).GetViewHeight(v);
	}
	ANumber	newviewsheight = 0;
	for( AIndex v = 0; v < mTabDataArray.GetObject(inTabIndex).GetViewCount()-1; v++ )
	{
		AFloatNumber	h = mTabDataArray.GetObject(inTabIndex).GetViewHeight(v);
		h /= viewsheight;
		h *= h_MainColumn;
		mTabDataArray.GetObject(inTabIndex).SetViewHeight(v,static_cast<ANumber>(h));
		newviewsheight += static_cast<ANumber>(h);
	}
	mTabDataArray.GetObject(inTabIndex).SetViewHeight(mTabDataArray.GetObject(inTabIndex).GetViewCount()-1, h_MainColumn - newviewsheight);
	
	//y位置
	ANumber	y = pt_MainColumn.y;
	//各縦分割ビュー毎ループ
	AItemCount	viewcount = mTabDataArray.GetObject(inTabIndex).GetViewCount();
	for( AIndex v = 0; v < viewcount; v++ )
	{
		//viewheight取得（水平分割線ビュー分の高さも含んでいる）
		ANumber	viewheight = mTabDataArray.GetObject(inTabIndex).GetViewHeight(v);
		
		//水平分割線ビューの配置
		AControlID	separatorControlID = mTabDataArray.GetObject(inTabIndex).GetSeparatorControlID(v);
		if( v == 0 )
		{
			//最初の水平分割線は表示しない
			NVI_SetControlPosition(separatorControlID,kWindowPoint_00);
			NVI_SetControlSize(separatorControlID,0,0);
		}
		else
		{
			AWindowPoint	pt = {pt_MainColumn.x,y};
			NVI_SetControlPosition(separatorControlID,pt);
			NVI_SetControlSize(separatorControlID,w_MainColumn,kSeparatorHeight);
			y += kSeparatorHeight;
			viewheight -= kSeparatorHeight;
			/*#725
			//分割線の背景色をtext view背景色と同じにする
			AColor	backgroundColor = kColor_White;
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kBackgroundColor,backgroundColor);
			NVI_GetHSeparatorViewByControlID(separatorControlID).SPI_SetBackgroundColor(backgroundColor);
			*/
		}
		//====================TextViewの配置====================
		//TextView縦書きモード設定 #558
		GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_SetVerticalMode(verticalMode);
		if( verticalMode == false )
		{
			//通常配置
			AWindowPoint	pt = {pt_MainColumn.x + w_LineNumber,y};
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v),w_TextView,viewheight);
//★検討			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).SPI_UpdateImageSize();
			//TextViewに、背景画像のOffset(=LineNumberViewの幅)を設定する
			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).SPI_SetBackgroundImageOffset(w_LineNumber);
			//イメージサイズ外にスクロールされていたら補正する
			//（ドキュメントを開いた直後、SPI_DoWrapCalculated()で、viewサイズ未設定なのでサイズがスクロール位置がイメージサイズ外になることがあるため）
			/*
			ANumber	svImageYMaxMargin = GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_GetImageYMaxMargin();
			ANumber	svImageYMinMargin = GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_GetImageYMinMargin();
			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_SetImageYMargin(0,0);
			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_ConstrainScrollToImageSize(false);
			GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).NVI_SetImageYMargin(svImageYMinMargin,svImageYMaxMargin);
			*/
		}
		else
		{
			//縦書きモード時配置 #558
			AWindowPoint	pt = {pt_MainColumn.x,y+w_LineNumber};
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v),w_TextView+w_LineNumber,viewheight-w_LineNumber);
		}
		//TextViewに対応するスクロールバーのコントロールID設定 #558
		GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(v)).
				NVI_SetScrollBarControlID(mTabDataArray.GetObjectConst(inTabIndex).GetHScrollbarControlID(),
										  mTabDataArray.GetObject(inTabIndex).GetVScrollBarControlID(v));
		//==================LineNumberViewの配置==================
		//LineNumberView縦書きモード設定 #558
		GetLineNumberViewByControlID(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v)).NVI_SetVerticalMode(verticalMode);
		if( verticalMode == false )
		{
			//通常配置
			AWindowPoint	pt = {pt_MainColumn.x,y};
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v),w_LineNumber,viewheight);
			//#379
			//差分比較ビュー配置
			if( v == 0 && inTabIndex == NVI_GetCurrentTabIndex() )
			{
				GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
																								  pt,w_LineNumber,viewheight);
				if( NVI_IsWindowVisible() == true )//#1090 CreateTab()から最初にコールされた時に、ウインドウがまだshowされていないので、その場合は、show()しないようにする（子ウインドウをshowすると親もshowされてしまうので）
				{
					GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Show(false);
				}
			}
			//
			GetLineNumberViewByControlID(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v)).SPI_SetLeftMargin(w_MainTextMargin);
		}
		else
		{
			//縦書きモード時配置 #558
			AWindowPoint	pt = {pt_MainColumn.x,y};
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(v),w_TextView+w_LineNumber,w_LineNumber);
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Hide();
		}
		//==================分割ボタン==================
		ANumber	h = 0;
		//SplitButtonの配置
		{
			AWindowPoint	pt = {pt_VScrollbarArea.x,y};
			if( v > 0 )
			{
				//2つめ以降のビューsplitボタンのy位置は4pt上にする
				pt.y -= 4;
			}
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetSplitButtonControlID(v),pt);
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetSplitButtonControlID(v),kVScrollBarWidth,kSplitButtonHeight);
			h += kSplitButtonHeight;
		}
		//==================閉じるボタン==================
		//#844 AControlID	concatControlID = mTabDataArray.GetObject(inTabIndex).GetConcatButtonControlID(v);
		AWindowID	closeViewButtonWindowID = mTabDataArray.GetObject(inTabIndex).GetCloseViewButtonWindowID(v);//win
		if( mTabDataArray.GetObject(inTabIndex).GetViewCount() < 2 )
		{
			//ビューが１つのみの場合
			//Show/Hide制御 
			//#844 NVI_SetShowControl(concatControlID,false);
			//#725 GetApp().NVI_GetWindowByID(closeViewButtonWindowID).NVI_Hide();
			NVI_SetOverlayWindowVisibility(closeViewButtonWindowID,false);//#725
		}
		else
		{
			//位置・サイズ設定
			AWindowPoint	pt = {pt_MainColumn.x,y};
			if( v > 0 )
			{
				//2つめ以降のビュークローズボタンのy位置は4pt上にする
				pt.y -= 3;
			}
			GetApp().NVI_GetWindowByID(closeViewButtonWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
						pt,19,19);
			//Show/Hide制御 
			/*#725
			if( NVI_GetCurrentTabIndex() == inTabIndex )
			{
				NVI_SetShowControl(concatControlID,false);
				GetApp().NVI_GetWindowByID(closeViewButtonWindowID).NVI_Show(false);
			}
			*/
			NVI_SetOverlayWindowVisibility(closeViewButtonWindowID,true);//#725
		}
		//==================Vスクロールバーの配置==================
		{
			AWindowPoint	pt = {pt_VScrollbarArea.x, y + h};
			if( v > 0 )
			{
				//2つめ以降のVスクロールバーボタンのy位置は4pt上にする
				pt.y -= 4;
			}
			NVI_SetControlPosition(mTabDataArray.GetObject(inTabIndex).GetVScrollBarControlID(v),pt);
			ANumber	vscrollheight = viewheight-h;
			if( v > 0 )
			{
				vscrollheight += 4;
			}
			if( v != viewcount-1 )
			{
				vscrollheight += 3;
			}
			NVI_SetControlSize(mTabDataArray.GetObject(inTabIndex).GetVScrollBarControlID(v),kVScrollBarWidth,vscrollheight);
		}
		//次のy位置
		y += viewheight;
	}
}

/**
サブテキスト配置
*/
ABool	AWindow_Text::UpdateLayout_SubTextColumn( const AIndex inTabIndex, const ATextWindowLayoutData& layout )
{
	ABool	subTextPaneDisplayed = false;
	if( layout.w_SubTextPaneColumn > 0 )
	{
		//サブテキストカラム配置
		UpdateLayout_SubTextPane(inTabIndex,layout.pt_SubTextPaneColumn,layout.w_SubTextPaneColumn,layout.h_SubTextPaneColumn,
								 layout.h_subHScrollbar);
		//サブテキストペイン表示済みフラグをON
		subTextPaneDisplayed = true;
		
		//サブテキストカラム分割線配置
		if( mSubTextColumnVSeparatorWindowID != kObjectID_Invalid )
		{
			AWindowPoint	pt = {0};
			pt.x = layout.pt_SubTextPaneColumn.x + layout.w_SubTextPaneColumn - kSeparatorWidth;
			pt.y = layout.pt_SubTextPaneColumn.y;
			NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
						pt,kSeparatorWidth,layout.h_SubTextPaneColumn + layout.h_subHScrollbar);
			NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_Show(false);
			NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_Right);
		}
		
		//サブテキストシャドウ配置
		{
			AWindowPoint	pt = {0};
			pt.x = layout.pt_SubTextPaneColumn.x + layout.w_SubTextPaneColumn - kWidth_SubTextShadow;
			pt.y = layout.pt_SubTextPaneColumn.y + kSubPaneModeButtonHeight;
			GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
						pt, kWidth_SubTextShadow, layout.h_SubTextPaneColumn - kSubPaneModeButtonHeight);
			GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_Show(false);
		}
	}
	else
	{
		//サブテキストカラム分割線非表示
		NVI_GetVSeparatorWindow(mSubTextColumnVSeparatorWindowID).NVI_Hide();
		//サブテキストシャドウ非表示
		GetApp().NVI_GetWindowByID(mSubTextShadowWindowID).NVI_Hide();
	}
	return subTextPaneDisplayed;
}

//#725
/**
サブテキストペイン配置
*/
void	AWindow_Text::UpdateLayout_SubTextPane( const AIndex inTabIndex, const AWindowPoint pt_SubTextPane_origin, 
		const ANumber w_SubTextPane, const ANumber h_SubTextPane, const ANumber h_SubHScrollbar )
{
	//#675 UpdateViewBounds抑制状態中は何もしない
	//#1090 UpdateViewBounds抑制状態中は画面外に配置する
	AWindowPoint pt_SubTextPane = pt_SubTextPane_origin;
	if( mSuppressUpdateViewBoundsCounter > 0 )   //return;
	{
		pt_SubTextPane.x = -10000;
		pt_SubTextPane.y = -10000;
	}
	//タブ毎ループ
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )//win サブペインは現在のタブとは違うタブの内容を表示可能なので
	{
		//mode index取得
		AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex();
		//
		AControlID	textViewControlID = mTabDataArray.GetObject(tabIndex).GetSubTextControlID();
		if( textViewControlID != kControlID_Invalid )
		{
			//行番号幅取得
			ANumber	lineNumberWidth = GetLineNumberWidth(inTabIndex);//#450
			AWindowPoint	pt2 = pt_SubTextPane;
			//上部ボタン配置
			if( mDiffDisplayMode == false )
			{
				//(Diff表示ではない)通常表示の場合のボタン配置
				
				//サブペインファイル名ボタン
				ANumber	w2 = w_SubTextPane-kSubPaneSyncButtonWidth-kSubPaneSwapButtonWidth-kSubPaneDiffButtonWidth;//#379
				NVI_SetControlPosition(kSubPaneFileNameButtonControlID,pt_SubTextPane);
				NVI_SetControlSize(kSubPaneFileNameButtonControlID,w2,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneFileNameButtonControlID,true);
				//サブペインDiffボタン #379
				pt2.x += w2;//#379 kSubPaneModeButtonWidth;
				NVI_SetControlPosition(kSubPaneDiffButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneDiffButtonControlID,kSubPaneDiffButtonWidth,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneDiffButtonControlID,true);
				//比較ドキュメント有無でボタンのEnable制御
				if( NVI_GetCurrentDocumentID() != kObjectID_Invalid )
				{
					if( GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )
					{
						NVI_SetControlEnable(kSubPaneDiffButtonControlID,false);
					}
					else
					{
						NVI_SetControlEnable(kSubPaneDiffButtonControlID,true);
					}
				}
				//サブペイン入替ボタン
				pt2.x += kSubPaneDiffButtonWidth;
				NVI_SetControlPosition(kSubPaneSwapButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneSwapButtonControlID,kSubPaneSwapButtonWidth,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneSwapButtonControlID,true);
				//サブペイン同期ボタン
				pt2.x += kSubPaneSwapButtonWidth;
				NVI_SetControlPosition(kSubPaneSyncButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneSyncButtonControlID,kSubPaneSyncButtonWidth,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneSyncButtonControlID,true);
			}
			else//#379
			{
				//Diff表示の場合のボタン配置
				
				//
				NVI_SetShowControl(kSubPaneFileNameButtonControlID,false);
				//#379 NVI_SetShowControl(kSubPaneModeButtonControlID,false);
				NVI_SetShowControl(kSubPaneSwapButtonControlID,false);
				//サブペインDiffボタン
				ANumber	w2 = w_SubTextPane-kSubPaneSyncButtonWidth;
				NVI_SetControlPosition(kSubPaneDiffButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneDiffButtonControlID,w2+1,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneDiffButtonControlID,true);
				//サブペイン同期ボタン
				pt2.x += w2;
				NVI_SetControlPosition(kSubPaneSyncButtonControlID,pt2);
				NVI_SetControlSize(kSubPaneSyncButtonControlID,kSubPaneSyncButtonWidth,kSubPaneModeButtonHeight);
				NVI_SetShowControl(kSubPaneSyncButtonControlID,true);
			}
			//縦書きモード取得 #558
			ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetVerticalMode();
			//各行行番号配置
			AControlID	lineNumberControlID = mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID();
			pt2 = pt_SubTextPane;
			pt2.y += kSubPaneModeButtonHeight;
			//LineNumberView縦書きモード設定 #558
			GetLineNumberViewByControlID(lineNumberControlID).NVI_SetVerticalMode(verticalMode);
			if( verticalMode == false )
			{
				//通常配置
				NVI_SetControlPosition(lineNumberControlID,pt2);
				NVI_SetControlSize(lineNumberControlID,lineNumberWidth,h_SubTextPane-kSubPaneModeButtonHeight);
			}
			else
			{
				//縦書き用配置 #558
				NVI_SetControlPosition(lineNumberControlID,pt2);
				NVI_SetControlSize(lineNumberControlID,w_SubTextPane,lineNumberWidth);
			}
			//#725
			//サブテキストHスクロールバー配置
			{
				AControlID	hScrollbarControlID = mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID();
				if( h_SubHScrollbar > 0 )
				{
					AWindowPoint	pt = {0};
					pt.x = pt_SubTextPane.x;
					pt.y = pt_SubTextPane.y + h_SubTextPane;
					NVI_SetControlPosition(hScrollbarControlID,pt);
					NVI_SetControlSize(hScrollbarControlID,w_SubTextPane,kHScrollBarHeight);
					NVI_SetShowControl(hScrollbarControlID,true);
				}
				else
				{
					//Hスクロールバー非表示
					NVI_SetShowControl(hScrollbarControlID,false);
				}
				//TextViewに、対応するスクロールバーのコントロールID設定 #558
				GetTextViewByControlID(textViewControlID).NVI_SetScrollBarControlID(hScrollbarControlID,kControlID_Invalid);
			}
			//TextView縦書きモード設定 #558
			GetTextViewByControlID(textViewControlID).NVI_SetVerticalMode(verticalMode);
			//TextView配置
			if( verticalMode == false )
			{
				//通常配置
				pt2.x += lineNumberWidth;
				NVI_SetControlPosition(textViewControlID,pt2);
				NVI_SetControlSize(textViewControlID,w_SubTextPane-lineNumberWidth,h_SubTextPane-kSubPaneModeButtonHeight);
			}
			else
			{
				//縦書き用配置 #558
				pt2.y += lineNumberWidth;
				NVI_SetControlPosition(textViewControlID,pt2);
				NVI_SetControlSize(textViewControlID,w_SubTextPane,h_SubTextPane-kSubPaneModeButtonHeight-lineNumberWidth);
			}
			//TextViewに、背景画像のOffset(=LineNumberViewの幅)を設定する
			GetTextViewByControlID(textViewControlID).SPI_SetBackgroundImageOffset(lineNumberWidth);
			//ImageSize更新
			GetTextViewByControlID(textViewControlID).SPI_UpdateImageSize();
		}
	}
}

/**
左サイドバー各高さ補正
*/
void	AWindow_Text::UpdateLayout_AdjustLeftSideBarHeight( const ANumber inLeftSideBarHeight, const AIndex inAdjustStartIndex )
{
	//==================inAdjustStartIndex以降のサイドバー項目の高さデータの和を計算==================
	AFloatNumber	totalSubPaneHeight = 0;
	for( AIndex index = inAdjustStartIndex; index < mLeftSideBarArray_Height.GetItemCount(); index++ )
	{
		//4ペインモードの場合は、サブテキストペインは表示しない。
		if( Is4PaneMode() == true )
		{
			if( mLeftSideBarArray_Type.Get(index) == kSubWindowType_SubTextPane )
			{
				continue;
			}
		}
		//全体高さ
		if( mLeftSideBarArray_Collapsed.Get(index) == true )
		{
			//折りたたみ時高さを加算
			totalSubPaneHeight += GetApp().SPI_GetSubWindowCollapsedHeight();
		}
		else
		{
			//現在の高さを加算
			totalSubPaneHeight += mLeftSideBarArray_Height.Get(index);
		}
	}
	//==================inAdjustStartIndex以降のサイドバー表示エリアの実際の高さを取得==================
	ANumber	remainSideBarHeight = inLeftSideBarHeight;
	for( AIndex index = 0; index < inAdjustStartIndex; index++ )
	{
		if( mLeftSideBarArray_Collapsed.Get(index) == true )
		{
			remainSideBarHeight -= GetApp().SPI_GetSubWindowCollapsedHeight();
		}
		else
		{
			remainSideBarHeight -= mLeftSideBarArray_Height.Get(index);
		}
	}
	//==================高さ補正==================
	//実際の表示エリアと、上記で計算した全体高さが違う場合は、高さ補正する。
	if( totalSubPaneHeight != remainSideBarHeight && remainSideBarHeight > 0 )//win 最小化解除時にサイズ異常になる問題対策
	{
		//現在のmLeftSideBarArray_Heightデータの高さ比率で設定する
		AFloatNumber	n = 0;
		for( AIndex index = inAdjustStartIndex; index < mLeftSideBarArray_Height.GetItemCount(); index++ )
		{
			if( mLeftSideBarArray_Collapsed.Get(index) == true )
			{
				//折りたたみ時
				n += GetApp().SPI_GetSubWindowCollapsedHeight();
			}
			else
			{
				//4ペインモードの場合は、サブテキストペインは表示しない。
				if( Is4PaneMode() == true )
				{
					if( mLeftSideBarArray_Type.Get(index) == kSubWindowType_SubTextPane )
					{
						continue;
					}
				}
				//高さ計算（最後のみ、比率計算ではなく、残り高さを設定）
				AFloatNumber	heightRatio = static_cast<AFloatNumber>(mLeftSideBarArray_Height.Get(index));
				heightRatio = heightRatio/totalSubPaneHeight;
				AFloatNumber	h = (remainSideBarHeight*heightRatio);
				if( index != mLeftSideBarArray_Height.GetItemCount()-1 )
				{
					mLeftSideBarArray_Height.Set(index,h);
					n += h;
				}
				else
				{
					mLeftSideBarArray_Height.Set(index,remainSideBarHeight-n);
				}
			}
		}
	}
	//DBにサイドバーデータ保存
	SetDataSideBar();
}

/**
右サイドバー各高さ補正
*/
void	AWindow_Text::UpdateLayout_AdjustRightSideBarHeight( const ANumber inRightSideBarHeight, const AIndex inAdjustStartIndex )
{
	//==================inAdjustStartIndex以降のサイドバー項目の高さデータの和を計算==================
	AFloatNumber	totalSubPaneHeight = 0;
	for( AIndex index = inAdjustStartIndex; index < mRightSideBarArray_Height.GetItemCount(); index++ )
	{
		//4ペインモードの場合は、サブテキストペインは表示しない。
		if( Is4PaneMode() == true )
		{
			if( mRightSideBarArray_Type.Get(index) == kSubWindowType_SubTextPane )
			{
				continue;
			}
		}
		//全体高さ
		if( mRightSideBarArray_Collapsed.Get(index) == true )
		{
			//折りたたみ時高さを加算
			totalSubPaneHeight += GetApp().SPI_GetSubWindowCollapsedHeight();
		}
		else
		{
			//現在の高さを加算
			totalSubPaneHeight += mRightSideBarArray_Height.Get(index);
		}
	}
	//
	ANumber	remainSideBarHeight = inRightSideBarHeight;
	for( AIndex index = 0; index < inAdjustStartIndex; index++ )
	{
		if( mRightSideBarArray_Collapsed.Get(index) == true )
		{
			remainSideBarHeight -= GetApp().SPI_GetSubWindowCollapsedHeight();
		}
		else
		{
			remainSideBarHeight -= mRightSideBarArray_Height.Get(index);
		}
	}
	//==================高さ補正==================
	//実際の表示エリアと、上記で計算した全体高さが違う場合は、高さ補正する。
	if( totalSubPaneHeight != remainSideBarHeight && remainSideBarHeight > 0 )//win 最小化解除時にサイズ異常になる問題対策
	{
		//現在のmRightSideBarArray_Heightデータの高さ比率で設定する
		AFloatNumber	n = 0;
		for( AIndex index = inAdjustStartIndex; index < mRightSideBarArray_Height.GetItemCount(); index++ )
		{
			if( mRightSideBarArray_Collapsed.Get(index) == true )
			{
				//折りたたみ時
				n += GetApp().SPI_GetSubWindowCollapsedHeight();
			}
			else
			{
				//4ペインモードの場合は、サブテキストペインは表示しない。
				if( Is4PaneMode() == true )
				{
					if( mRightSideBarArray_Type.Get(index) == kSubWindowType_SubTextPane )
					{
						continue;
					}
				}
				//高さ計算（最後のみ、比率計算ではなく、残り高さを設定）
				AFloatNumber	heightRatio = static_cast<AFloatNumber>(mRightSideBarArray_Height.Get(index));
				heightRatio = heightRatio/totalSubPaneHeight;
				AFloatNumber	h = (remainSideBarHeight*heightRatio);
				if( index != mRightSideBarArray_Height.GetItemCount()-1 )
				{
					mRightSideBarArray_Height.Set(index,h);
					n += h;
				}
				else
				{
					mRightSideBarArray_Height.Set(index,remainSideBarHeight-n);
				}
			}
		}
	}
	//DBにサイドバーデータ保存
	SetDataSideBar();
}

/**
右サイドバー配置
*/
void	AWindow_Text::UpdateLayout_RightSideBar( const AIndex inTabIndex, const ATextWindowLayoutData& inLayout, const ABool inAnimate )
{
	//#675 UpdateViewBounds抑制状態中は何もしない
	if( mSuppressUpdateViewBoundsCounter > 0 )   return;
	
	//
	if( mRightSideBarDisplayed == true )
	{
		//========== 右サイドバーを表示する場合 ==========
		
		//サイドバー背景
		NVI_SetControlPosition(kControlID_RightSideBarBackground,inLayout.pt_RightSideBar);
		NVI_SetControlSize(kControlID_RightSideBarBackground,inLayout.w_RightSideBar,inLayout.h_RightSideBar_WithBottomMargin);
		NVI_SetShowControl(kControlID_RightSideBarBackground,true);
		
		//右サイドバー内項目が無い場合消去
		if( mRightSideBarArray_Type.GetItemCount() == 0 )
		{
			AWindowRect	eraserect = {0};
			eraserect.left		= inLayout.pt_RightSideBar.x;
			eraserect.top		= inLayout.pt_RightSideBar.y;
			eraserect.right		= inLayout.pt_RightSideBar.x + inLayout.w_RightSideBar;
			eraserect.bottom	= inLayout.pt_RightSideBar.y + inLayout.h_RightSideBar;
			NVI_PaintRect(eraserect,kColor_White);
		}
		
		//右サイドバー分割線
		if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )//#319
		{
			NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
						inLayout.pt_VSeparator_RightSideBar,kSeparatorWidth,inLayout.h_RightSideBar);
			NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_Show(false);
			//
			ASeparatorLinePosition	separatorLinePosition = kSeparatorLinePosition_None;
			NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).SPI_SetLinePosition(separatorLinePosition);
		}
		
		//サブウインドウ各高さ補正
		UpdateLayout_AdjustRightSideBarHeight(inLayout.h_RightSideBar);
		
		//開始y位置取得
		AFloatNumber	infopaney = inLayout.pt_RightSideBar.y;
		//各Info Window
		for( AIndex infoPaneIndex = 0; infoPaneIndex < mRightSideBarArray_Type.GetItemCount(); infoPaneIndex++ )
		{
			//サイドバー項目高さ取得
			AFloatNumber	infoPaneHeight = mRightSideBarArray_Height.Get(infoPaneIndex);
			if( mRightSideBarArray_Collapsed.Get(infoPaneIndex) == true )
			{
				//折りたたみ時
				infoPaneHeight = GetApp().SPI_GetSubWindowCollapsedHeight();
			}
			//次の項目y位置
			AFloatNumber	nexty = infopaney + infoPaneHeight;
			ANumber	intNextY = nexty;
			//サイドバー項目データ取得
			ASubWindowType	type = mRightSideBarArray_Type.Get(infoPaneIndex);
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(infoPaneIndex);
			ABool	dragging = mRightSideBarArray_Dragging.Get(infoPaneIndex);
			if( winID != kObjectID_Invalid )
			{
				AWindowPoint	pt = inLayout.pt_RightSideBar;
				pt.x += k_wRightSideBarVisibleArea;
				pt.y = infopaney;
				//ドラッグ中以外の場合、オーバーレイウインドウ配置
				//（ドラッグ中のオーバーレイウインドウの場合は、配置をしない。）
				if( dragging == false )
				{
					//サブウインドウ配置
					GetApp().SPI_UpdateLayoutOverlaySubWindow(GetObjectID(),type,winID,
															  pt,inLayout.w_RightSideBar - k_wRightSideBarVisibleArea,intNextY-pt.y,inAnimate);
					//ジャンプリストの場合、タブselect
					switch(type)
					{
					  case kSubWindowType_JumpList:
						{
							if( NVI_GetCurrentTabIndex() == inTabIndex )
							{
								GetApp().SPI_GetJumpListWindow(winID).NVI_SelectTab(inTabIndex);
							}
							break;
						}
					  default:
						{
							//処理なし
							break;
						}
					}
				}
			}
			//次のyへ
			infopaney = nexty;
		}
	}
	else
	{
		//========== 右サイドバーを表示しない場合 ==========
		
		//サイドバー背景
		NVI_SetShowControl(kControlID_RightSideBarBackground,false);
		
		//右サイドバー分割線非表示
		if( mRightSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			NVI_GetVSeparatorWindow(mRightSideBarVSeparatorWindowID).NVI_Hide();
		}
	}
}

/**
左サイドバー配置
*/
ABool	AWindow_Text::UpdateLayout_LeftSideBar( const AIndex inTabIndex, const ATextWindowLayoutData& inLayout )
{
	//#675 UpdateViewBounds抑制状態中は何もしない
	if( mSuppressUpdateViewBoundsCounter > 0 )   return false;
	
	ABool	subTextPaneDisplayed = false;
	if( inLayout.w_LeftSideBar > 0 )
	{
		//========== 左サイドバーを表示する場合 ==========
		
		//サイドバー背景
		NVI_SetControlPosition(kControlID_LeftSideBarBackground,inLayout.pt_LeftSideBar);
		NVI_SetControlSize(kControlID_LeftSideBarBackground,inLayout.w_LeftSideBar,inLayout.h_LeftSideBarWithMargin);
		NVI_SetShowControl(kControlID_LeftSideBarBackground,true);
		
		//左サイドバー内項目が無い場合消去
		if( mLeftSideBarArray_Type.GetItemCount() == 0 )
		{
			AWindowRect	eraserect = {0};
			eraserect.left		= inLayout.pt_LeftSideBar.x;
			eraserect.top		= inLayout.pt_LeftSideBar.y;
			eraserect.right		= inLayout.pt_LeftSideBar.x + inLayout.w_LeftSideBar;
			eraserect.bottom	= inLayout.pt_LeftSideBar.y + inLayout.h_LeftSideBar;
			NVI_PaintRect(eraserect,kColor_White);
		}
		
		//左サイドバー分割線
		if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			AWindowPoint	pt = {0};
			NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),
											inLayout.pt_VSeparator_LeftSideBar,kSeparatorWidth,inLayout.h_LeftSideBar);
			NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_Show(false);
			NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).SPI_SetLinePosition(kSeparatorLinePosition_None);
		}
		
		//サブウインドウ各高さ補正
		UpdateLayout_AdjustLeftSideBarHeight(inLayout.h_LeftSideBar);
		
		//開始y位置取得
		AFloatNumber	subpaney = inLayout.pt_LeftSideBar.y;
		//各サブウインドウ毎ループ
		for( AIndex subPaneIndex = 0; subPaneIndex < mLeftSideBarArray_Type.GetItemCount(); subPaneIndex++ )
		{
			//サイドバー項目高さ取得
			AFloatNumber	subPaneHeight = mLeftSideBarArray_Height.Get(subPaneIndex);
			if( mLeftSideBarArray_Collapsed.Get(subPaneIndex) == true )
			{
				//折りたたみ時
				subPaneHeight = GetApp().SPI_GetSubWindowCollapsedHeight();
			}
			//次の項目y位置
			AFloatNumber	nexty = subpaney + subPaneHeight;
			ANumber	intNextY = nexty;
			//サイドバー項目データ取得
			ASubWindowType	type = mLeftSideBarArray_Type.Get(subPaneIndex);
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(subPaneIndex);
			ABool	dragging = mLeftSideBarArray_Dragging.Get(subPaneIndex);
			if( winID != kObjectID_Invalid )
			{
				AWindowPoint	pt = inLayout.pt_LeftSideBar;
				pt.y = subpaney;
				//ドラッグ中以外の場合、オーバーレイウインドウ配置
				//（ドラッグ中のオーバーレイウインドウの場合は、配置をしない。）
				if( dragging == false )
				{
					//サブウインドウ配置
					GetApp().NVI_GetWindowByID(winID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),pt,
								inLayout.w_LeftSideBar - k_wLeftSideBarVisibleArea,intNextY-pt.y);
					switch(type)
					{
					  case kSubWindowType_SubTextPane:
						{
							//サブテキストペイン配置
							UpdateLayout_SubTextPane(inTabIndex,pt,
													 inLayout.w_LeftSideBar - k_wLeftSideBarVisibleArea,subPaneHeight,
													 inLayout.h_subHScrollbar);
							//サブテキストペイン表示フラグON
							subTextPaneDisplayed = true;
							break;
						}
						//ジャンプリストの場合、タブselect
					  case kSubWindowType_JumpList:
						{
							if( NVI_GetCurrentTabIndex() == inTabIndex )
							{
								GetApp().SPI_GetJumpListWindow(winID).NVI_SelectTab(inTabIndex);
							}
							break;
						}
					  default:
						{
							//処理なし
							break;
						}
					}
				}
			}
			//次のyへ
			subpaney = nexty;
		}
	}
	else
	{
		//========== 左サイドバーを表示しない場合 ==========
		
		//サイドバー背景
		NVI_SetShowControl(kControlID_LeftSideBarBackground,false);
		
		//サブペインカラム分割線非表示
		if( mLeftSideBarVSeparatorWindowID != kObjectID_Invalid )
		{
			NVI_GetVSeparatorWindow(mLeftSideBarVSeparatorWindowID).NVI_Hide();
		}
	}
	return subTextPaneDisplayed;
}

/**
行番号・場所ボタン配置
*/
void	AWindow_Text::UpdateLayout_LineNumberAndPositionButton( const AIndex inTabIndex, const ATextWindowLayoutData& layout )
{
	//ウインドウ幅・高さ取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	
	//ステータスバーを表示するかどうかを取得
	ABool	showStatusBar = (layout.h_Footer > 0 );
	
	//フッターの各ボタンのy位置オフセット
	const ANumber	kFooterButtonsYOffset = 2;
	
	//行番号ボタン
	if( showStatusBar == true )
	{
		AWindowPoint	pt = {0};
		pt.x = layout.pt_MainColumn.x;// + kWidth_FooterSeparator;
		//#1184 if( mSubTextColumnDisplayed == false && mLeftSideBarDisplayed == false )
		{
			pt.x += kWidth_LeftSideBar_ShowHideButton;
		}
		pt.y = windowBounds.bottom - windowBounds.top - layout.h_Footer + kFooterButtonsYOffset;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_LineNumberButton,pt);
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LineNumberButton,true);
	}
	else
	{
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LineNumberButton,false);
	}
	//場所ボタン
	if( showStatusBar == true )
	{
		AWindowPoint	pt = {0};
		pt.x = layout.pt_MainColumn.x + NVI_GetControlWidth(kControlID_LineNumberButton);// + kWidth_FooterSeparator*2;
		//#1184 if( mSubTextColumnDisplayed == false && mLeftSideBarDisplayed == false )
		{
			pt.x += kWidth_LeftSideBar_ShowHideButton;
		}
		pt.y = windowBounds.bottom - windowBounds.top - layout.h_Footer + kFooterButtonsYOffset;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_PositionButton,pt);
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_PositionButton,true);
		NVI_GetButtonViewByControlID(kControlID_PositionButton).
				SPI_SetWidthToFitTextWidth(layout.pt_MainColumn.x + layout.w_MainColumn - pt.x - 32);
	}
	else
	{
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_PositionButton,false);
	}
	
	//#725
	//サブテキスト行番号ボタン
	if( mSubTextColumnDisplayed == true && showStatusBar == true )
	{
		AWindowPoint	pt = {0};
		pt.x = layout.pt_SubTextPaneColumn.x;// + kWidth_FooterSeparator;
		//#1184 if( mLeftSideBarDisplayed == false )
		{
			pt.x += kWidth_LeftSideBar_ShowHideButton;
		}
		pt.y = windowBounds.bottom - windowBounds.top - layout.h_Footer + kFooterButtonsYOffset;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_LineNumberButton_SubText,pt);
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LineNumberButton_SubText,true);
	}
	else
	{
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_LineNumberButton_SubText,false);
	}
	//サブテキストの場所ボタン
	if( mSubTextColumnDisplayed == true && showStatusBar == true )
	{
		AWindowPoint	pt = {0};
		pt.x = layout.pt_SubTextPaneColumn.x + NVI_GetControlWidth(kControlID_LineNumberButton_SubText);// + kWidth_FooterSeparator*2;
		//#1184 if( mLeftSideBarDisplayed == false )
		{
			pt.x += kWidth_LeftSideBar_ShowHideButton;
		}
		pt.y = windowBounds.bottom - windowBounds.top - layout.h_Footer + kFooterButtonsYOffset;
		NVI_SetControlPositionIfCurrentTab(inTabIndex,kControlID_PositionButton_SubText,pt);
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_PositionButton_SubText,true);
		NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).
				SPI_SetWidthToFitTextWidth(layout.pt_SubTextPaneColumn.x + layout.w_SubTextPaneColumn - pt.x - 32);
	}
	else
	{
		NVI_SetShowControlIfCurrentTab(inTabIndex,kControlID_PositionButton_SubText,false);
	}
}

/**
サブウインドウ表示・非表示更新
*/
void	AWindow_Text::UpdateVisibleSubWindows()
{
	//左サイドバーの表示・非表示に従って、各サブウインドウ表示・非表示
	for( AIndex subPaneIndex = 0; subPaneIndex < mLeftSideBarArray_Type.GetItemCount(); subPaneIndex++ )
	{
		AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(subPaneIndex);
		if( winID != kObjectID_Invalid )
		{
			if( mLeftSideBarDisplayed == true )
			{
				GetApp().NVI_GetWindowByID(winID).NVI_Show(false);
			}
			else
			{
				GetApp().NVI_GetWindowByID(winID).NVI_Hide();
			}
		}
	}
	//右サイドバーの表示・非表示に従って、各サブウインドウ表示・非表示
	for( AIndex infoPaneIndex = 0; infoPaneIndex < mRightSideBarArray_OverlayWindowID.GetItemCount(); infoPaneIndex++ )
	{
		AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(infoPaneIndex);
		if( winID != kObjectID_Invalid )
		{
			if( mRightSideBarDisplayed == true )
			{
				GetApp().NVI_GetWindowByID(winID).NVI_Show(false);
			}
			else
			{
				GetApp().NVI_GetWindowByID(winID).NVI_Hide();
			}
		}
	}
}

/**
各view bindings更新
*/
void	AWindow_Text::UpdateViewBindings( const ATextWindowViewBindings inBindings )
{
	//ウインドウ背景ビュー
	if( true )
	{
		//スクロールバー交わる部分のみ背景描画の場合
		NVI_SetControlBindings(kControlID_WindowBackground,false,false,true,true,true,true);
	}
	else
	{
		//全体に背景描画の場合
		NVI_SetControlBindings(kControlID_WindowBackground,true,true,true,true,false,false);
	}
	//各タブごとのループ
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		//縦書きモード取得 #558
		ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetVerticalMode();
		switch(inBindings)
		{
		  case kTextWindowViewBindings_Normal:
			{
				//各split view
				for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
				{
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v),
										   true,true,true,true,false,false);//上下左右バインド
					if( verticalMode == false )
					{
						//通常用バインド
						NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
											   true,true,false,true,true,false);//左バインド、幅固定、上下バインド
					}
					else
					{
						//縦書き用バインド #558
						NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
											   true,true,true,false,false,true);//上バインド、左右バインド、高さ固定
					}
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetVScrollBarControlID(v),
										   false,true,true,true,true,false);//右バインド、幅固定、上下バインド
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSplitButtonControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSeparatorControlID(v),
										   true,true,true,false,false,true);//上バインド、左右バインド、高さ固定
				}
				//ルーラー
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetRulerControlID(),
									   true,true,true,false,false,true);//上バインド、左右バインド、高さ固定
				//タブセレクタ
				NVI_SetControlBindings(kControlID_TabSelector,
									   true,true,true,false,false,true);//上バインド、左右バインド、高さ固定
				//Hスクロールバー
				NVI_SetControlBindings(mTabDataArray.GetObjectConst(tabIndex).GetHScrollbarControlID(),
									   true,false,true,true,false,true);//下バインド、左右バインド、高さ固定
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID(),
									   true,false,true,true,false,true);//下バインド、左バインド、サイズ固定
				//サブテキスト view
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextControlID(),
									   true,true,false,true,true,false);//左バインド、幅固定、上下バインド
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID(),
									   true,true,false,true,true,false);//左バインド、幅固定、上下バインド
				//サブテキスト制御ボタン
				NVI_SetControlBindings(kSubPaneFileNameButtonControlID,
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneSyncButtonControlID,
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneSwapButtonControlID,
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneDiffButtonControlID,
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				break;
			}
		  case kTextWindowViewBindings_ShowHideLeftSidebar:
			{
				//各split view
				for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
				{
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetVScrollBarControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSplitButtonControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSeparatorControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
				}
				//ルーラー
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetRulerControlID(),
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				//タブセレクタ
				NVI_SetControlBindings(kControlID_TabSelector,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				//Hスクロールバー
				NVI_SetControlBindings(mTabDataArray.GetObjectConst(tabIndex).GetHScrollbarControlID(),
									   true,false,true,true,false,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID(),
									   true,false,true,true,false,true);//右上バインド、サイズ固定
				//サブテキスト view
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextControlID(),
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID(),
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				//サブテキスト制御ボタン
				NVI_SetControlBindings(kSubPaneFileNameButtonControlID,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneSyncButtonControlID,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneSwapButtonControlID,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneDiffButtonControlID,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				break;
			}
		  case kTextWindowViewBindings_ShowRightSidebar:
		  case kTextWindowViewBindings_HideRightSidebar:
			{
				//各split view
				for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
				{
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v),
										   true,true,false,false,true,true);//左上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
										   true,true,false,false,true,true);//左上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetVScrollBarControlID(v),
										   true,true,false,false,true,true);//左上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSplitButtonControlID(v),
										   true,true,false,false,true,true);//左上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSeparatorControlID(v),
										   true,true,false,false,true,true);//左上バインド、サイズ固定
				}
				//ルーラー
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetRulerControlID(),
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				//タブセレクタ
				NVI_SetControlBindings(kControlID_TabSelector,
									   true,true,true,false,false,true);//上バインド、左右バインド、高さ固定
				//Hスクロールバー
				NVI_SetControlBindings(mTabDataArray.GetObjectConst(tabIndex).GetHScrollbarControlID(),
									   true,false,true,true,false,true);//左上バインド、サイズ固定
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID(),
									   true,false,true,true,false,true);//左上バインド、サイズ固定
				//サブテキスト view
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextControlID(),
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID(),
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				//サブテキスト制御ボタン
				NVI_SetControlBindings(kSubPaneFileNameButtonControlID,
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneSyncButtonControlID,
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneSwapButtonControlID,
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneDiffButtonControlID,
									   true,true,false,false,true,true);//左上バインド、サイズ固定
				break;
			}
		  case kTextWindowViewBindings_ShowSubTextColumn:
		  case kTextWindowViewBindings_HideSubTextColumn:
			{
				//各split view
				for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
				{
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetVScrollBarControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSplitButtonControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
					NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSeparatorControlID(v),
										   false,true,true,false,true,true);//右上バインド、サイズ固定
				}
				//ルーラー
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetRulerControlID(),
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				//タブセレクタ
				NVI_SetControlBindings(kControlID_TabSelector,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				//Hスクロールバー
				NVI_SetControlBindings(mTabDataArray.GetObjectConst(tabIndex).GetHScrollbarControlID(),
									   true,false,true,true,false,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextHScrollbarControlID(),
									   true,false,true,true,false,true);//右上バインド、サイズ固定
				//サブテキスト view
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextControlID(),
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID(),
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				//サブテキスト制御ボタン
				NVI_SetControlBindings(kSubPaneFileNameButtonControlID,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneSyncButtonControlID,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneSwapButtonControlID,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				NVI_SetControlBindings(kSubPaneDiffButtonControlID,
									   false,true,true,false,true,true);//右上バインド、サイズ固定
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
}

//
void	AWindow_Text::NVIDO_Show()
{
}

//Hide()時等にウインドウのデータを保存する
void	AWindow_Text::NVIDO_Hide()
{
	ARect	bounds;
	//#1238 kSingleWindowBoundsにはサイドバーなしのサイズを保存するようにする。 NVI_GetWindowBounds(bounds);
	SPI_GetWindowMainColumnBounds(bounds);
	//#404 フルスクリーンモード設定保存
	//#476 if( mTabModeMainWindow == true )
	{
		ABool	fullScreenMode = NVI_GetFullScreenMode();
		//#476 GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kFullScreenMode,fullScreenMode);
		if( fullScreenMode == true )
		{
			//フルスクリーン解除時のウインドウ領域のほうを保存する
			NVI_GetWindowBoundsInStandardState(bounds);
		}
	}
	//
	if( /*#850 GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true &&
		SPI_IsTabModeMainWindow() == true &&//#569 タブメインウインドウ以外ではsingleWindowBounds記憶しない*/
				mLuaConsoleMode == false )//#567 LuaConsoleではsingleWindowBounds記憶しない
	{
		GetApp().GetAppPref().SetData_Rect(AAppPrefDB::kSingleWindowBounds,bounds);
		//最大化状態 win
		NVI_GetWindowBoundsInStandardState(bounds);
		GetApp().GetAppPref().SetData_Rect(AAppPrefDB::kSingleWindowBounds_UnzoomedBounds,bounds);
		GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kSingleWindowBounds_Zoomed,NVI_IsZoomed());
		
		//「ドキュメントはタブに開く」がONのときは、ウインドウ位置を記憶する
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true )
		{
			ARect	maincolumnrect = {0};
			SPI_GetWindowMainColumnBounds(maincolumnrect);
			APoint	pt = {maincolumnrect.left,maincolumnrect.top};
			GetApp().GetAppPref().SetData_Point(AAppPrefDB::kNewDocumentWindowPoint,pt);
		}
	}
	//#567 LuaConsoleモードならkLuaConsoleWindowPosition等を記憶
	//#850 if( mLuaConsoleMode == true )
	else
	{
		APoint	pt = {0,0};
		pt.x = bounds.left;
		pt.y = bounds.top;
		GetApp().NVI_GetAppPrefDB().SetData_Point(AAppPrefDB::kLuaConsoleWindowPosition,pt);
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kLuaConsoleWindowWidth,bounds.right-bounds.left);
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kLuaConsoleWindowHeight,bounds.bottom-bounds.top);
	}
}

//ウインドウ全体の表示更新
void	AWindow_Text::NVIDO_UpdateProperty()
{
	//Viewの位置、サイズ更新
	SPI_UpdateViewBounds();
	
	//
	/*ドキュメントのNVI_UpdateProperty()で実行されるfor( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		SPI_UpdateTextDrawProperty(NVI_GetDocumentIDByTabIndex(tabIndex));
	}*/
	
	//
	//#725 SPI_GetJumpListWindow().NVI_UpdateProperty();
	
	//サブテキスト制御ボタン色更新
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetLetterColor(GetApp().SPI_GetSubTextColumnButtonLetterColor(),
																					 GetApp().SPI_GetSubTextColumnButtonLetterColorDeactivate());
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetDropShadowColor(GetApp().SPI_GetSubTextColumnButtonDropShadowColor());
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetLetterColor(GetApp().SPI_GetSubTextColumnButtonLetterColor(),
																				 GetApp().SPI_GetSubTextColumnButtonLetterColorDeactivate());
	NVI_GetButtonViewByControlID(kSubPaneSyncButtonControlID).SPI_SetDropShadowColor(GetApp().SPI_GetSubTextColumnButtonDropShadowColor());
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetLetterColor(GetApp().SPI_GetSubTextColumnButtonLetterColor(),
																				 GetApp().SPI_GetSubTextColumnButtonLetterColorDeactivate());
	NVI_GetButtonViewByControlID(kSubPaneSwapButtonControlID).SPI_SetDropShadowColor(GetApp().SPI_GetSubTextColumnButtonDropShadowColor());
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetLetterColor(GetApp().SPI_GetSubTextColumnButtonLetterColor(),
																				 GetApp().SPI_GetSubTextColumnButtonLetterColorDeactivate());
	NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetDropShadowColor(GetApp().SPI_GetSubTextColumnButtonDropShadowColor());
	
	//サイドバー背景色更新
	AColor	color = GetApp().SPI_GetSubWindowBackgroundColor(true);
	AColor	separatorColor = GetApp().SPI_GetSideBarFrameColor();
	NVI_SetPlainViewColor(kControlID_RightSideBarBackground,color,color,false,true,false,true,separatorColor);
	NVI_SetPlainViewColor(kControlID_LeftSideBarBackground,color,color,true,false,true,false,separatorColor);
	
}

ABool	AWindow_Text::SPI_IsDocumentVisible( const ADocumentID inDocumentID ) const
{
	return (NVI_GetCurrentDocumentID() == inDocumentID);
}

//B0411
ABool	AWindow_Text::NVIDO_IsChildWindowVisible() const
{
	if( SPI_GetAskingSaveChanges() == true )   return true;
	/*#694
	if( SPI_GetMoveLieWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetFontSizeWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetWrapLetterCountWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetCorrectEncodingWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetPrintOptionWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetPropertyWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetDropWarningWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetAddToolButtonWindowConst().NVI_IsWindowVisible() == true )   return true;
	if( SPI_GetEditByOtherAppWindowConst().NVI_IsWindowVisible() == true )   return true;//R0232
	if( SPI_GetSCMCommitWindowConst().NVI_IsWindowVisible() == true )   return true;//#455
	*/
	return false;
}

//#404
/**
フルスクリーンモード切替
*/
/*#476
void	AWindow_Text::SPI_SwitchFullScreenMode()
{
	if( SPI_IsTabModeMainWindow() == false )   return;
	NVI_SetFullScreenMode(!NVI_GetFullScreenMode());
}
*/

//#675
/**
オーバーレイウインドウを含めて全てRefreshする
*/
void	AWindow_Text::SPI_RefreshWindowAll()
{
	NVI_RefreshWindow();
	//#725 SPI_GetJumpListWindow().NVI_RefreshWindow();
	//#725 GetApp().SPI_GetIdInfoWindow().NVI_RefreshWindow();
	//#725 GetApp().SPI_GetFileListWindow().NVI_RefreshWindow();
}

//#850
/**
メインカラムのboundsによって、ウインドウ位置・サイズを設定
*/
void	AWindow_Text::SPI_SetWindowBoundsByMainColumnPosition( const ARect& inRect )
{
	//サイドバー・サブテキスト表示中なら、boundsをその分拡張する
	ARect	bounds = inRect;
	if( mLeftSideBarDisplayed == true )
	{
		bounds.left -= mLeftSideBarWidth;
	}
	if( mSubTextColumnDisplayed == true )
	{
		bounds.left -= mSubTextColumnWidth;
	}
	if( mRightSideBarDisplayed == true )
	{
		bounds.right += mRightSideBarWidth;
	}
	//ウインドウbounds設定
	NVI_SetWindowBounds(bounds);
}

//#850
/**
メインカラムのboundsを取得
*/
void	AWindow_Text::SPI_GetWindowMainColumnBounds( ARect& outRect ) const
{
	//ウインドウbounds取得
	NVI_GetWindowBounds(outRect);
	//サイドバー・サブテキスト表示中なら、boundsをその分縮める
	if( mLeftSideBarDisplayed == true )
	{
		outRect.left += mLeftSideBarWidth;
	}
	if( mSubTextColumnDisplayed == true )
	{
		outRect.left += mSubTextColumnWidth;
	}
	if( mRightSideBarDisplayed == true )
	{
		outRect.right -= mRightSideBarWidth;
	}
}

#pragma mark ===========================

#pragma mark ---タブ制御

//新規テキストドキュメントタブ生成
void	AWindow_Text::SPNVI_CreateTextDocumentTab( const ADocumentID inTextDocumentID, 
		const ABool inSelect, const ABool inSelectableTab )//#212 #379
{
	NVI_CreateTab(inTextDocumentID,inSelectableTab);//#379
	//B0411 子ウインドウ表示中はタブ切替しない
	if( NVI_IsChildWindowVisible() == false && inSelect == true ) //#212
	{
		NVI_SelectTab(NVI_GetTabCount()-1);
	}
	NVI_DrawControl(kControlID_TabSelector);
}

/**
新規タブ生成
@return outInitialFocusControlID 生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_Text::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//TabDataを新規追加
	mTabDataArray.InsertNew1Object(inTabIndex);
	//キャレット／選択をDocPrefから取得（イリーガルなTextPointだった場合は補正する。）
	//DocPrefのキャレット／選択位置のyは行番号ではなくて段落番号である。（AView_Text::SPI_SavePreProcess()）
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	ABool	caretMode = GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Bool(ADocPrefDB::kCaretMode);
	ATextPoint	caretTextPoint = {0,0}, selectTextPoint = {0,0};
	caretTextPoint.x	= GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Number(ADocPrefDB::kCaretPointX);
	caretTextPoint.y	= GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Number(ADocPrefDB::kCaretPointY);
	//#699 ドキュメント生成直後は行折り返し計算完了していないので、ここでのSPI_CorrectTextPoint()は実行しない。
	//行折り返し未完了時は、キャレット・選択設定はされず、AView_Text::SPI_DoWrapCalculated()でキャレット・選択設定されるので、
	//行折り返し完了時、AView_Text::SPI_DoWrapCalculated()にてSPI_CorrectTextPoint()も実行する。
	//#699 GetApp().SPI_GetTextDocumentConstByID(docID).SPI_CorrectTextPoint(caretTextPoint);
	//
	selectTextPoint.x	= GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Number(ADocPrefDB::kSelectPointX);
	selectTextPoint.y	= GetApp().SPI_GetTextDocumentConstByID(docID).GetDocPrefDBConst().GetData_Number(ADocPrefDB::kSelectPointY);
	//#699 GetApp().SPI_GetTextDocumentConstByID(docID).SPI_CorrectTextPoint(selectTextPoint);
	//#699 CreateSplitView()からcaretMode引数を消したので、caret modeならselectTextPointの値をcaretTextPointと同じにする。
	if( caretMode == true )
	{
		selectTextPoint = caretTextPoint;
	}
	
	//#246
	AWindowPoint	pt = {0,0};
	
	//ツールバー生成
	AControlID	toolbarControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_Toolbar>	toolbarFactory(GetObjectID(),toolbarControlID);
	NVM_CreateView(toolbarControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,toolbarFactory);
	NVI_SetControlBindings(toolbarControlID,true,true,true,false,false,true);//#725
	NVM_RegisterViewInTab(inTabIndex,toolbarControlID);
	mTabDataArray.GetObject(inTabIndex).SetToolbarControlID(toolbarControlID);
	
	//ルーラー生成
	///*#725
	AControlID	rulerControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_Ruler>	rulerFactory(GetObjectID(),rulerControlID);
	NVM_CreateView(rulerControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,rulerFactory);
	NVM_RegisterViewInTab(inTabIndex,rulerControlID);
	mTabDataArray.GetObject(inTabIndex).SetRulerControlID(rulerControlID);
	
	//メインHScrollbar生成
	{
		AWindowPoint	pt = {0};
		AControlID	hscrollbarControlID = NVM_AssignDynamicControlID();
		NVI_CreateScrollBar(hscrollbarControlID,pt,20,10);
		NVM_RegisterViewInTab(inTabIndex,hscrollbarControlID);
		mTabDataArray.GetObject(inTabIndex).SetHScrollbarControlID(hscrollbarControlID);
	}
	
	//*/
	/*
	AWindowDefaultFactory<AWindow_Ruler>	rulerWindowFactory;
	AWindowID	rulerWindowID = GetApp().NVI_CreateWindow(rulerWindowFactory);
	GetApp().NVI_GetWindowByID(rulerWindowID).NVI_ChangeToOverlay(GetObjectID(),false);
	GetApp().NVI_GetWindowByID(rulerWindowID).NVI_Create(kObjectID_Invalid);
	NVM_RegisterOverlayWindow(rulerWindowID,inTabIndex,false);
	mTabDataArray.GetObject(inTabIndex).SetRulerWindowID(rulerWindowID);
	GetRulerView(inTabIndex).SPI_UpdateImageSize();
	*/
	//ジャンプリスト
	//#725 SPI_GetJumpListWindow().NVI_CreateTab(docID);//#291
	//ジャンプリストにタブ生成
	GetApp().SPI_CreateJumpListWindowsTab(GetObjectID(),docID);//#725
	/*#725
	//ジャンプリスト表示フラグ設定
	mTabDataArray.GetObject(inTabIndex).SetJumpListVisible(GetApp().SPI_GetModePrefDB(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeIndex()).
				GetData_Bool(AModePrefDB::kDisplayJumpList));
	*/
	
	//ツールボタン生成
	CreateToolButtons(inTabIndex);
	
	//新規ビュー生成
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	ANumberArray	collapsedLinesArray;
	CreateSplitView(inTabIndex,0,windowBounds.bottom-windowBounds.top-kHScrollBarHeight,/*#699 caretMode,*/
					caretTextPoint,selectTextPoint,kImagePoint_00,collapsedLinesArray);
	//#1090
	//最初のタブについては、ここでSPI_DoViewActivating()（＝ドキュメント読み込み）を行う
	//（下のSPI_DisplayInSubText()内でのSPI_DoViewActivating()実行を、サブテキスト表示中しか行わなくなったので、
	//ここで実行するようにした）
	if( inTabIndex == 0 )
	{
		GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_DoViewActivating();
	}
	//#699
	//初期選択位置設定
	AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(0);
	GetTextViewByControlID(textViewControlID).SPI_SetSelectWithParagraphIndex(
				caretTextPoint.y,caretTextPoint.x,selectTextPoint.y,selectTextPoint.x);
	
	//#212 サブテキストペイン生成
	//#725 生成自体は常に生成するようにする。条件削除。if( mLeftSideBarDisplayed == true )
	//{
	//
	//#699 ATextPoint	spt = {0,0}, ept = {0,0};
	CreateSubTextInEachTab(inTabIndex,/*#699 spt,ept,*/kImagePoint_00);
	//}
	
	//ツールボタン生成
	//win090927 win CreateSplitView()でUpdateViewBounds()を呼んでいるので、先にtoolButtonを作っておく必要がある CreateToolButtons(inTabIndex);
	//初期タブ内フォーカス取得
	//#699 上へ移動 AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(0);
	outInitialFocusControlID = textViewControlID;
	
	/* CreateSplitView()内でtext viewの配置を設定する。
	//B0411 CreateSplitView()で生成したtabについてUpdateViewBounds()してしまうので、再度、現在のtabについてUpdateViewBounds()する。
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex());
	}
	*/
	//#212
	UpdateSubTextFileNameButton();
	//#212 LastFocusedTextViewを設定しておく。裏Openしたときに「入替」ボタンが正しく動作するように。
	//mTabDataArray.GetObject(inTabIndex).SetLastFocusedTextViewControlID(textViewControlID);
	mTabDataArray.GetObject(inTabIndex).SetLatentMainTextViewControlID(textViewControlID);
	mTabDataArray.GetObject(inTabIndex).SetLatentTextViewControlID(textViewControlID);
	
	//win 最初に開いたときにサブペインにも最初のタブの内容を表示する
	if( /*#725 mLeftSideBarDisplayed == true &&*/ mSubTextCurrentDocumentID == kObjectID_Invalid )
	{
		//#728
		//SPI_DisplayInSubText()内部でSPI_DoViewActivated()を呼んでしまうので、
		//先にメインビューでSPI_DoViewActivated()を呼んでおき、メインビューでADocument_Text::InitViewData()を行う。
		//（サブテキストviewは、上のCreateSubTextInEachTab()で既に生成済み）
		//GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_DoViewActivating();
		//		GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).
		//		GetObjectID());
		
		//サブテキストに最初のタブの内容を表示する
		SPI_DisplayInSubText(inTabIndex,false,kIndex_Invalid,false,false);
	}
	//#379
	GetTabSelector().SPI_DoTabCreated(inTabIndex);
	
	//#725
	UpdateViewBindings(kTextWindowViewBindings_Normal);
	
	//#688
	//最初に生成した場合、ツールバーのメニューのタイトルを設定する。（起動時にアプリがdeactivateだと、tab activatedが来ないので、空白のままになってしまうため）
	if( mTabDataArray.GetItemCount() == 1 )
	{
		UpdateToolbarItemValue(inTabIndex);
	}
}

/**
タブ削除
*/
void	AWindow_Text::NVIDO_DeleteTab( const AIndex inTabIndex )
{
	//#725
	//Tabに属するView, Overlay windowはNVI_DeleteTabAndView()で削除される
	//
	mTabDataArray.Delete1Object(inTabIndex);
	//#379
	GetTabSelector().SPI_DoTabDeleted(inTabIndex);
}

//#663
/**
タブ削除完了時処理
*/
void	AWindow_Text::NVIDO_TabDeleted()
{
	//タブ数が減るとtabselectorの高さが変わる可能性があるので、UpdateViewBounds()する
	UpdateViewBounds(NVI_GetCurrentTabIndex());
}

//#725
/**
タブselect時処理
＠note 自動制御されないviewのshow/hideなどを実行
*/
void	AWindow_Text::NVIDO_SelectTab( const AIndex inTabIndex )
{
	//=====================サブテキスト表示更新=====================
	//サブテキストはタブ内viewなので、自動的にタブに従って非表示・表示されてしまうため、
	//ここで現在のサブテキストを表示更新する
	AIndex	diffDocTabIndex = GetDiffDocumentTabIndexFromWorkingTabIndex(inTabIndex);//#379
	if( mDiffDisplayMode == true )//#379
	{
		//#379 サブペインにDiff表示
		if( diffDocTabIndex != kIndex_Invalid )
		{
			SPI_UpdateDiffDisplay(NVI_GetDocumentIDByTabIndex(inTabIndex));
		}
		else
		{
			//diff対象ドキュメントが無い場合はサブペインになにも表示しない
			
			//DiffInfoウインドウは更新（比較中ドキュメントから、比較なしドキュメントに切り替えたときに比較情報を消すため）#611
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
		}
	}
	else
	{
		/*#844
		//#212 サブペイン表示更新（モードに応じて各tabのSubPaneTextの表示・非表示を切り替え）
		if( mSubPaneMode != kSubPaneMode_Manual )
		{
			UpdateSubPaneAutomatic(mSubPaneMode,inTabSwitched);
		}
		else
		 */
		{
			AIndex	tabIndex = NVI_GetTabIndexByDocumentID(mSubTextCurrentDocumentID);
			if( tabIndex != kIndex_Invalid )
			{
				SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,false,false);
			}
		}
	}
}

//win
/**
Tab Activate準備処理
*/
void	AWindow_Text::NVMDO_DoTabActivating( const AIndex inTabIndex )
{
	//#379 ドキュメントにViewActivatingを通知
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_DoViewActivating();
	//			GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).
	//			GetObjectID());
}

//Tab Activated時の処理

//inTabSwitched: true: タブが切り替わった  false: タブは切り替わらずウインドウがactivateされた
void	AWindow_Text::NVMDO_DoTabActivated( const AIndex inTabIndex, const ABool inTabSwitched )//win 080712
{
	if( NVI_IsWindowVisible() == false )   return;
	
	//#688
	if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;
	
	/*#675 UpdateViewBounds()へ移動
	//ViewCloseボタンOverlayを全てHideする。UpdateViewBounds()で自タブだけ表示 win
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
		{
			GetApp().NVI_GetWindowByID(mTabDataArray.GetObject(tabIndex).GetCloseViewButtonWindowID(v)).NVI_Hide();
		}
	}
	*/
	//B0442
	//#725 GetApp().SPI_UpdateFloatingWindowHideAll();
	//
	if( inTabSwitched == true )//#700 Spacesでの別spaceでのファイルオープン時のspace繰り返し移動防止
	{
		UpdateViewBounds(inTabIndex);
	}
	//タブセレクタ表示更新
	NVI_DrawControl(kControlID_TabSelector);
	
	//
	AImagePoint	originOfLocalFrame;
	GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
	//#558 NVI_SetControlNumber(mTabDataArray.GetObjectConst(inTabIndex).GetHScrollbarControlID(),originOfLocalFrame.x);
	GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0)).NVI_UpdateScrollBar();//#558 スクロールバー値更新はAViewで行う（縦書き考慮）
	//ルーラー更新
	UpdateRulerProperty(inTabIndex);
	//#848 下へ移動し、focusのドキュメントに対して処理するよう変更。 GetApp().NVI_GetMenuManager().RealizeDynamicMenu(GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetJumpMenuObjectID());
	//ジャンプリスト更新
	//focusには現在のfocusのテキストビューが入る。（サブテキストペインがfocusならそれが入る）
	AControlID	focus = GetTopMostFocusTextViewControlID();//#212 (inTabIndex);//#137 NVI_GetFocusInTab(inTabIndex);
	if( focus != kControlID_Invalid )
	{
		//#1091対策テスト
		NVI_SetShowControl(focus,true);
		//
		NVI_SwitchFocusTo(focus);//#212 サブペインにフォーカスがあった場合は、NVI_SelectTab()でサブペインがhideされて、focus設定できていないのでここで設定する必要がある
		//
		//#1091-test SPI_SetDefferedFocus(focus);
		//
		AIndex	tabIndex = NVI_GetTabIndexByDocumentID(SPI_GetTextDocument(focus).GetObjectID());//#212
		ATextPoint	spt, ept;
		GetTextViewByControlID(focus).SPI_GetSelect(spt,ept);
		//B0308
		AIndex	jumpMenuItemIndex = SPI_GetTextDocument(focus).SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
		if( jumpMenuItemIndex >= kLimit_MaxJumpMenuItemCount )//#695
		{
			jumpMenuItemIndex = kIndex_Invalid;
		}
		//#857 GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenu(kMenuItemID_Jump,jumpMenuItemIndex);
		GetApp().SPI_UpdateJumpMenu(SPI_GetTextDocument(focus).SPI_GetJumpMenuObjectID(),jumpMenuItemIndex);//#857
		/*#725
		//フローティングジャンプリスト表示復元
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			if( mTabDataArray.GetObjectConst(tabIndex).GetJumpListVisible() == true )
			{
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Show(false);//win
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_SelectTab(tabIndex);
			}
			else
			{
				//ジャンプリストを非表示
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Hide();
			}
		}
		*/
		//フローティングジャンプリスト表示・配置更新
		UpdateFloatingJumpListDisplay();
		//ジャンプリストのタブ選択 #725
		GetApp().SPI_SelectJumpListWindowsTab(GetObjectID(),SPI_GetTextDocument(focus).GetObjectID());
		
		//
		//ルーラーのキャレット更新
		if( tabIndex == inTabIndex && GetRulerView(inTabIndex).NVI_IsVisible() == true )//#212 #1090 ルーラー非表示中はルーラーにキャレット位置設定しない
		{
			AImagePoint	spt, ept;
			GetTextViewByControlID(focus).SPI_GetImageSelect(spt,ept);
			GetRulerView(inTabIndex).SPI_UpdateCaretImageX(spt.x,ept.x);
			//
			AImagePoint	originOfLocalFrame;
			GetTextViewByControlID(focus).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
			originOfLocalFrame.y = 0;
			GetRulerView(inTabIndex).NVI_ScrollTo(originOfLocalFrame);
		}
	}
	//「同じフォルダ」メニュー更新
	AFileAcc	file;
	SPI_GetCurrentTabTextDocument().NVI_GetFile(file);
	AFileAcc	parent;
	parent.SpecifyParent(file);
	GetApp().SPI_UpdateSameFolderMenu(parent);
	//「同じプロジェクト」更新
	AFileAcc	projectFolder;
	SPI_GetCurrentTabTextDocument().SPI_GetProjectFolder(projectFolder);
	GetApp().SPI_UpdateSameProjectMenu(projectFolder,SPI_GetCurrentTabTextDocument().SPI_GetModeIndex());
	//関連ファイルメニュー更新
	GetApp().SPI_UpdateImportFileMenu(SPI_GetCurrentTabTextDocument().SPI_GetImportFileArray());
	//#81
	//自動バックアップメニュー更新
	SPI_GetCurrentTabTextDocument().SPI_UpdateComopareWithAutoBackupMenu();
	//ファイルリスト更新
	//#725 GetApp().SPI_UpdateFileListWindow_DocumentActivated(SPI_GetCurrentTabTextDocument().GetObjectID());
	GetApp().SPI_UpdateFileListWindows(kFileListUpdateType_DocumentActivated,SPI_GetCurrentTabTextDocument().GetObjectID());//#725
	//付箋紙リストウインドウ更新 #138
	//#858 GetApp().SPI_SetFusenListActiveDocument(/*#298 file*/SPI_GetCurrentTabTextDocument().SPI_GetProjectObjectID(),SPI_GetCurrentTabTextDocument().SPI_GetProjectRelativePath());
	//ツールメニュー更新
	GetApp().SPI_UpdateToolMenu(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex());
	//InfoHeader表示更新
	//#725 InfoHeader廃止 SetInfoHeaderTextDefault();
	//行番号ボタンの更新
	SPI_UpdateLineNumberButton(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0));
	//場所ボタンの更新
	UpdatePositionButton(mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0));
	//R0000
	SPI_GetCurrentTabTextDocument().SPI_UpdateFileAttribute();
	//#379 比較ボタン更新
	/*if( mDiffDisplayMode == true &&
				GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )
	{
		SPI_SetDiffDisplayMode(false);
	}*/
	SPI_UpdateDiffButtonText();
	//#725
	//#725 UpdateViewBounds(inTabIndex);
	/*
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(inTabIndex,layout);
	UpdateLayout_LeftSideBar(inTabIndex,layout);
	UpdateLayout_RightSideBar(inTabIndex,layout);
	*/
	//#724
	//ツールバー値更新
	UpdateToolbarItemValue(inTabIndex);
	//#142
	//doc infoウインドウ更新
	SPI_GetCurrentTabTextDocument().SPI_UpdateDocInfoWindows();
	//モードメニューのショートカット（現在のドキュメントのモード）設定
	GetApp().SPI_UpdateModeMenuShortcut();
}

//Tab Deactivated
void	AWindow_Text::NVMDO_DoTabDeactivated( const AIndex inTabIndex )
{
	if( NVI_IsWindowVisible() == false )   return;
	//ジャンプメニュー実体化解除
	GetApp().NVI_GetMenuManager().UnrealizeDynamicMenu(GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetJumpMenuObjectID());
	/*#291
	//ジャンプリストスクロール位置保存
	APoint	pt;
	SPI_GetJumpListWindow().SPI_GetScrollPosition(pt);
	mTabDataArray.GetObject(inTabIndex).SetJumpListScrollPosition(pt);
	*/
	//定義・宣言メニューを削除
	GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByMenuItemID(kMenuItemID_MoveToDefinition);
	//RC2 変換候補ウインドウ消去
	//#717 GetApp().SPI_GetCandidateListWindow().NVI_Hide();
	//各種ポップアップ非表示
	GetPopupIdInfoWindow().NVI_Hide();
	SPI_GetPopupCandidateWindow().NVI_Hide();
	//#81
	//未保存データ保存トリガー
	SPI_GetCurrentFocusTextDocument().SPI_AutoSave_Unsaved();
}

//
void	AWindow_Text::SPI_CloseTab( const AIndex inTabIndex )//win 080709 ABool→void（返り値未使用のため）
{
	TryClose(inTabIndex);
}

//
void	AWindow_Text::SPI_TryCloseDocument( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid )   return;//#909    {_ACThrow("",this);}
	TryClose(tabIndex);
}

//
void	AWindow_Text::SPI_ExecuteCloseDocument( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid )   return;//#909    {_ACThrow("",this);}
	ExecuteClose(inDocumentID);
}

//#357
void	AWindow_Text::SPI_SwitchTabNext()
{
	/*#559
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	tabIndex++;
	for( ; tabIndex != NVI_GetCurrentTabIndex(); tabIndex++ )//#379
	{
		if( tabIndex >= NVI_GetTabCount() )
		{
			tabIndex = 0;
		}
		if( NVI_GetTabSelectable(tabIndex) == true )   break;//#379
	}
	NVI_SelectTab(tabIndex);
	*/
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	if( NVI_GetNextDisplayTab(tabIndex,true) == true )
	{
		NVI_SelectTab(tabIndex);
	}
}

//#357
void	AWindow_Text::SPI_SwitchTabPrev()
{
	/*#559
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	tabIndex--;
	for( ; tabIndex != NVI_GetCurrentTabIndex(); tabIndex-- )//#379
	{
		if( tabIndex < 0 )
		{
			tabIndex = NVI_GetTabCount()-1;
		}
		if( NVI_GetTabSelectable(tabIndex) == true )   break;//#379
	}
	NVI_SelectTab(tabIndex);
	*/
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	if( NVI_GetPrevDisplayTab(tabIndex,true) == true )
	{
		NVI_SelectTab(tabIndex);
	}
}

//#513
/**
タブIndexから表示Indexを取得
*/
AIndex	AWindow_Text::SPI_GetDisplayTabIndex( const AIndex inTabIndex ) const
{
	return GetTabSelectorConst().SPI_GetDisplayTabIndex(inTabIndex);
}

//#559
/**
次の表示タブを取得
*/
ABool	AWindow_Text::NVIDO_GetNextDisplayTab( AIndex& ioIndex, const ABool inLoop ) const
{
	//表示indexを取得
	AIndex	displayIndex = GetTabSelectorConst().SPI_GetDisplayTabIndex(ioIndex);
	//次の表示indexに対応するtabIndexを取得
	displayIndex++;
	AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(displayIndex);
	if( tabIndex == kIndex_Invalid )
	{
		//次が存在しない（最後）の場合
		if( inLoop == true )
		{
			ioIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(0);
			return true;
		}
		else
		{
			return false;
		}
	}
	ioIndex = tabIndex;
	return true;
}

//#559
/**
前の表示タブを取得
*/
ABool	AWindow_Text::NVIDO_GetPrevDisplayTab( AIndex& ioIndex, const ABool inLoop ) const
{
	//表示indexを取得
	AIndex	displayIndex = GetTabSelectorConst().SPI_GetDisplayTabIndex(ioIndex);
	//前の表示indexに対応するtabIndexを取得
	displayIndex--;
	AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(displayIndex);
	if( tabIndex == kIndex_Invalid )
	{
		//前が存在しない（最初）の場合
		if( inLoop == true )
		{
			ioIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(GetTabSelectorConst().SPI_GetDisplayTabCount()-1);
			return true;
		}
		else
		{
			return false;
		}
	}
	ioIndex = tabIndex;
	return true;
}

//#850
/**
タブの表示index取得
*/
AIndex	AWindow_Text::SPI_GetDisplayTabIndexByTabIndex( const AIndex inTabIndex ) const
{
	return GetTabSelectorConst().SPI_GetDisplayTabIndex(inTabIndex);
}

//#850
/**
タブの表示index取得
*/
void	AWindow_Text::SPI_SetDisplayTabIndexByDocumentID( const ADocumentID inDocumentID, const AIndex inTabDisplayIndex )
{
	GetTabSelector().SPI_SetDisplayTabIndex(NVI_GetTabIndexByDocumentID(inDocumentID),inTabDisplayIndex);
}

//#1050
/**
タブセット保存
*/
void	AWindow_Text::SPI_SaveTabSet()
{
	//保存ダイアログ表示
	AText	name;
	name.SetLocalizedText("TabSetFileDefaultName");
	AFileAttribute	attr;
	GetApp().SPI_GetExportFileAttribute(attr);
	AText	kind;
	kind.SetCstring("tabset");
	AText	filter;
	GetApp().SPI_GetDefaultFileFilter(filter);
	AFileAcc	defaultFolder;
	AFileWrapper::GetDesktopFolder(defaultFolder);
	NVI_ShowSaveWindow(name,kObjectID_Invalid,attr,filter,kind,defaultFolder);
}

//#1062
/**
タブをアルファベット順にソートする
*/
void	AWindow_Text::SortTabs()
{
	//タブタイトルを格納してソートするための配列
	ATextArray	titleArray;
	//表示タブごとのループ
	AItemCount	tabDisplayCount = GetTabSelectorConst().SPI_GetDisplayTabCount();
	for( AIndex tabDisplayIndex = 0; tabDisplayIndex < tabDisplayCount; tabDisplayIndex++ )
	{
		//タブタイトル取得
		AIndex	tabIndex = GetTabSelectorConst().SPI_GetTabIndexFromDisplayIndex(tabDisplayIndex);
		ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
		AText	title;
		NVI_GetTitle(tabIndex,title);
		//タイトル内のタブコードをスペースに変換（無いと思うが、一応）
		title.ReplaceChar(kUChar_Tab,kUChar_Space);
		//タブコードの後に、タブindexを追加
		title.AddCstring("\t");
		title.AddNumber(tabIndex);
		//タブタイトル配列に追加
		titleArray.Add(title);
	}
	//タブタイトルをソート
	titleArray.Sort(true);
	
	//ソート後の順番にソートされた表示順配列を格納するための配列
	AArray<AIndex>	tabIndexArray;
	//ソート後の配列要素ごとのループ
	for( AIndex tabDisplayIndex = 0; tabDisplayIndex < tabDisplayCount; tabDisplayIndex++ )
	{
		//タブindex取得用変数
		AIndex	tabIndex = kIndex_Invalid;
		//ソート配列のテキスト取得
		AText	text;
		titleArray.Get(tabDisplayIndex,text);
		//タブコード後に追加された、タブindexを取得（取得できない場合は何もせず終了）
		AIndex	pos = 0;
		if( text.FindCstring(0,"\t",pos) == true )
		{
			pos++;
			text.ParseIntegerNumber(pos,tabIndex);
		}
		else
		{
			return;
		}
		if( tabIndex < 0 || tabIndex >= NVI_GetTabCount() )
		{
			return;
		}
		//表示順配列に追加
		tabIndexArray.Add(tabIndex);
	}
	//タブ表示順更新
	GetTabSelector().SPI_SetDisplayOrderArray(tabIndexArray);
}

#pragma mark ===========================

#pragma mark ---タブ切り離し・結合

//指定タブを別の新規ウインドウへコピー（切り離し）
void	AWindow_Text::SPI_CopyTabToWindow( const AIndex inTabIndex, const AWindowID inNewWindowID )
{
	if( NVI_GetTabSelectable(inTabIndex) == false )   return;//#379
	//#668 ウインドウboundsはDocPrefのデータに従うようにする（subpaneの考慮も考えるとこちらの方が良い） ARect	bounds;
	//#668 ウインドウboundsはDocPrefのデータに従うようにする NVI_GetWindowBounds(bounds);
	AWindow_Text&	newWindow = GetApp().SPI_GetTextWindowByID(inNewWindowID);
	newWindow.SPI_SetInitialSideBarDisplay(false,false,false);//#725 sidebar, subtext表示しない
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	newWindow.NVI_Create(docID);
	newWindow.NVI_SendBehind(GetObjectID());
	//#668 ウインドウboundsはDocPrefのデータに従うようにする newWindow.NVI_SetWindowBounds(bounds);
	//#379 diff対象ドキュメントがあれば、新規ウインドウにDiffドキュメントのタブ（選択不可タブ）を生成
	ADocumentID	diffTargetDocumentID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDiffTargetDocumentID();
	if( diffTargetDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_CreateWindowOrTabForNewTextDocument(diffTargetDocumentID,
				kIndex_Invalid,kIndex_Invalid,false,false,inNewWindowID);
	}
	//#379 GetApp().SPI_GetTextDocumentByID(docID).SPI_RegisterWindow(inNewWindowID);
	SPI_CopyViewDataTo(inTabIndex,inNewWindowID,0);
	newWindow.NVI_Show(false);
}

//指定タブのViewDataをコピーする（コピー元のウインドウでコールする）
void	AWindow_Text::SPI_CopyViewDataTo( const AIndex inSrcTabIndex, const AWindowID inDstWindowID, const AIndex inDstTabIndex )
{
	//Viewの情報を取得
	AArray<ANumber>	viewHeightArray;
	AArray<ATextPoint>	viewCaretTextPointArray;
	AArray<ATextPoint>	viewSelectTextPointArray;
	AArray<AImagePoint>	viewOriginOfLocalFrameArray;
	for( AIndex i = 0; i < mTabDataArray.GetObject(inSrcTabIndex).GetViewCount(); i++ )
	{
		viewHeightArray.Add(mTabDataArray.GetObjectConst(inSrcTabIndex).GetViewHeight(i));
		AControlID	textViewControlID = mTabDataArray.GetObjectConst(inSrcTabIndex).GetTextViewControlID(i);
		ATextPoint	spt, ept;
		GetTextViewByControlID(textViewControlID).SPI_GetSelect(spt,ept);
		viewCaretTextPointArray.Add(spt);
		viewSelectTextPointArray.Add(ept);
		AImagePoint	originOfLocalFrame;
		GetTextViewByControlID(textViewControlID).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
		viewOriginOfLocalFrameArray.Add(originOfLocalFrame);
	}
	//
	GetApp().SPI_GetTextWindowByID(inDstWindowID).
	CopyViewData(inDstTabIndex,viewHeightArray,viewCaretTextPointArray,viewSelectTextPointArray,viewOriginOfLocalFrameArray);
}
//コピー先のウインドウで実行するメソッド
void	AWindow_Text::CopyViewData( const AIndex inTabIndex, const AArray<ANumber>& inViewHeightArray, 
		const AArray<ATextPoint>& inViewCaretTextPointArray, const AArray<ATextPoint>& inViewSelectTextPointArray,
		const AArray<AImagePoint>& inViewOriginOfLocalFrameArray )
{
	//view0
	{
		mTabDataArray.GetObject(inTabIndex).SetViewHeight(0,inViewHeightArray.Get(0));
		AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(0);
		GetTextViewByControlID(textViewControlID).SPI_SetSelect(inViewCaretTextPointArray.Get(0),inViewSelectTextPointArray.Get(0));
		GetTextViewByControlID(textViewControlID).NVI_ScrollTo(inViewOriginOfLocalFrameArray.Get(0));
	}
	AItemCount	viewCount = inViewHeightArray.GetItemCount();
	for( AIndex i = 1; i < viewCount; i++ )
	{
		//新規ビュー生成
		//#699 ABool	caretMode = false;
		ATextPoint	spt = inViewCaretTextPointArray.Get(i);
		ATextPoint	ept = inViewSelectTextPointArray.Get(i);
		ANumberArray	collapsedLinesArray;
		//#699 if( spt.x == ept.x && spt.y == ept.y )   caretMode = true;
		CreateSplitView(inTabIndex,i,inViewHeightArray.Get(i),/*#699 caretMode,*/spt,ept,inViewOriginOfLocalFrameArray.Get(i),collapsedLinesArray);
	}
}

//ウインドウを新規タブにコピー（結合）
void	AWindow_Text::SPNVI_CopyCreateTab( const AWindowID inSrcWindowID, const AIndex inSrcTabIndex )
{
	//diff表示中だと制御がややこしくなるので、diff表示は解除する
	SPI_SetDiffDisplayMode(false);
	//
	AWindow_Text&	srcWindow = GetApp().SPI_GetTextWindowByID(inSrcWindowID);
	if( srcWindow.NVI_GetTabSelectable(inSrcTabIndex) == false )   return;//#379
	ADocumentID	docID = srcWindow.NVI_GetDocumentIDByTabIndex(inSrcTabIndex);
	SPNVI_CreateTextDocumentTab(docID);
	//#379 GetApp().SPI_GetTextDocumentByID(docID).SPI_RegisterWindow(GetObjectID());
	srcWindow.SPI_CopyViewDataTo(inSrcTabIndex,GetObjectID(),NVI_GetTabCount()-1);
	//#379 diff対象ドキュメントがあれば、もう１つ新規タブ（選択不可）生成する
	ADocumentID	diffTargetDocumentID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetDiffTargetDocumentID();
	if( diffTargetDocumentID != kObjectID_Invalid )
	{
		GetApp().SPI_CreateWindowOrTabForNewTextDocument(diffTargetDocumentID,
				kIndex_Invalid,kIndex_Invalid,false,false,GetObjectID());
	}
	//B0000 行情報再計算するときに以前のウインドウがあると、そちらのウインドウ幅で計算してしまうので、先にクローズ
	GetApp().SPI_GetTextWindowByID(inSrcWindowID).SPI_CloseTab(inSrcTabIndex);
	//行情報再計算（ウインドウ幅変わるので）
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(NVI_GetTabCount()-1)).SPI_DoWrapRelatedDataChanged(false);
}

/*#850
//
void	AWindow_Text::SPI_SetTabModeMainWindow( const ABool inTabModeMainWindow )
{
	mTabModeMainWindow = inTabModeMainWindow;
	//
	if( NVI_IsWindowCreated() == true )
	{
		//#725 SPI_UpdateInfoPaneArray();
		//#725 SPI_UpdateSubPaneArray();
		SPI_UpdateViewBounds();
	}
}
*/
#pragma mark ===========================

#pragma mark ---ビュー制御

//
void	AWindow_Text::CreateSplitView( const AIndex inTabIndex, const AIndex inViewIndex, const ANumber inViewHeight,
		//#699 const ABool inCaretMode, 
									  const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, 
									  const AImagePoint& inOriginOfLocalFrame, const ANumberArray& inCollapsedLinesArray )
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::CreateSplitView() started.",this);
	//分割ビュー内View生成
	
	//（正しいサイズ、位置は、NVICB_Update()で設定される。）
	AWindowPoint	pt = {0,0};
	//TextView生成
	//#844 ABool	crosscaretmode = false;
	/*#844
	//R0185
	if( mTabDataArray.GetObject(inTabIndex).GetViewCount() == 0 )
	{
		ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
		AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentConstByID(docID).SPI_GetModeIndex();
        //#844 crosscaretmode = GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kDefaultDisplayCrossCaret);
	}
	else
	{
        //#844 crosscaretmode = IsCrossCaretMode();
	}
	*/
	
	//ImageY余白取得
	ANumber	imageYBottomMargin = kImageYBottomMarginForNormal;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kBottomScrollAt25Percent) == true )
	{
		imageYBottomMargin = kImageYBottomMarginFor25PercentMargin;
	}
	
	//テキストビュー生成（幅は0に設定する。AView_Text::SPI_DoWrapCalculated()にて正しくadjust scrollするため）
	AControlID	textViewControlID = NVM_AssignDynamicControlID();
	ATextViewFactory	textViewFactory(/*#199 this,*this*/GetObjectID(),textViewControlID,
						/*#695 NVI_GetDocumentIDByTabIndex(inTabIndex)*/
						GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)),//#695
						GetObjectID(),//#379
						/*#699 inCaretMode,*///#699 inCaretTextPoint,inSelectTextPoint,
                        inOriginOfLocalFrame,false);//#844 ,crosscaretmode);//R0185 IsCrossCaretMode());//B0345
	AViewID	textViewID = NVM_CreateView(textViewControlID,pt,0,10,kControlID_Invalid,kControlID_Invalid,true,textViewFactory);
	GetTextViewByControlID(textViewControlID).SPI_SetTextWindowID(GetObjectID());
	NVI_SetControlBindings(textViewControlID,false,true,true,true,true,false);
	GetTextViewByControlID(textViewControlID).NVI_SetImageYMargin(0,imageYBottomMargin);//#621
	//#450
	GetTextViewByControlID(textViewControlID).SPI_CollapseLines(inCollapsedLinesArray,true);
	GetTextViewByControlID(textViewControlID).NVI_ScrollTo(inOriginOfLocalFrame);
	//#699 初期選択位置設定。
	GetTextViewByControlID(textViewControlID).SPI_SetSelect(inCaretTextPoint,inSelectTextPoint);
	//LineNumberView生成
	AImagePoint	originOfLocalFrame = inOriginOfLocalFrame;
	originOfLocalFrame.x = 0;
	AControlID	lineNumberControlID = NVM_AssignDynamicControlID();
	ALineNumberViewFactory	lineNumberViewFactory(/*#199 this,*this*/GetObjectID(),lineNumberControlID,/*#199 textViewControlID*/textViewID,NVI_GetDocumentIDByTabIndex(inTabIndex),originOfLocalFrame);
	AViewID	lineNumberViewID = NVM_CreateView(lineNumberControlID,pt,0,10,kControlID_Invalid,kControlID_Invalid,false,lineNumberViewFactory);
	GetLineNumberViewByControlID(lineNumberControlID).SPI_SetDiffDrawEnable();//#379
	GetLineNumberViewByControlID(lineNumberControlID).NVI_SetImageYMargin(0,imageYBottomMargin);//#621
	NVI_SetControlBindings(lineNumberControlID,false,true,true,true,true,false);
	GetTextViewByControlID(textViewControlID).SPI_SetLineNumberView(lineNumberViewID);//#450
	//Vスクロールバー生成
	AControlID	vScrollBarID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(vScrollBarID,pt,10,20);
	//ビュー分割ボタン生成
	AControlID	splitButtonID = NVM_AssignDynamicControlID();
	AText	text;
	NVI_CreateButtonView(splitButtonID,pt,10,10,kControlID_Invalid);
	//#725 NVI_GetButtonViewByControlID(splitButtonID).SPI_SetIcon(kIconID_Mi_SplitView,0,0,16,16);
	NVI_GetButtonViewByControlID(splitButtonID).SPI_SetButtonViewType(kButtonViewType_ScrollBar);
	NVI_GetButtonViewByControlID(splitButtonID).SPI_SetIcon(kImageID_iconPnSeparateVertical,0,1);//#725
	text.SetLocalizedText("HelpTag_SplitView");//#661
	NVI_GetButtonViewByControlID(splitButtonID).SPI_SetHelpTag(text,text);//#661
	/*
	NVI_CreateRoundedRectButton(splitButtonID,pt,10,10);
	*/
	//ビュー結合ボタン生成
	//#844 ビュー結合ボタンは使用しないこととする。（常に「閉じる」ボタンの方を使用）。ただ、処理削除の工数短縮のため、サイズ0,0で残す
	AControlID	concatButtonID = NVM_AssignDynamicControlID();
	NVI_CreateButtonView(concatButtonID,pt,0,0,kControlID_Invalid);
	NVI_GetButtonViewByControlID(concatButtonID).SPI_SetButtonViewType(kButtonViewType_NoFrame);
	//#688 NVI_GetButtonViewByControlID(concatButtonID).SPI_SetIcon(kIconID_Mi_Close,0,0,16,16);
	text.SetLocalizedText("HelpTag_CloseSplitView");//#661
	NVI_GetButtonViewByControlID(concatButtonID).SPI_SetHelpTag(text,text);//#661
	//ビューを閉じるボタンをOverlayWindow化 win
	AWindowDefaultFactory<AWindow_ButtonOverlay>	closeViewButtonWindowFactory;
	AWindowID	closeViewButtonWindowID = GetApp().NVI_CreateWindow(closeViewButtonWindowFactory);
	NVI_GetButtonWindow(closeViewButtonWindowID).NVI_ChangeToOverlay(GetObjectID(),false);//win
	NVI_GetButtonWindow(closeViewButtonWindowID).NVI_Create(kObjectID_Invalid);
	NVM_RegisterOverlayWindow(closeViewButtonWindowID,inTabIndex,false);//#725 親ウインドウに登録。（tab制御による自動show/hide/delete等のため）
	//#725 NVI_GetButtonWindow(closeViewButtonWindowID).NVI_Show(false);//アクティブにしない
	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetTargetWindowID(GetObjectID(),concatButtonID);
//	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetIcon(kImageID_iconPnSeparateVertical,1,1);//#725
	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetButtonViewType(kButtonViewType_NoFrame);
	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetIcon(kImageID_btnEditorClose,0,0,19,19,kImageID_btnEditorClose_h);
	NVI_GetButtonWindow(closeViewButtonWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),pt,16,16);
	text.SetLocalizedText("HelpTag_CloseSplitView");//#661
	NVI_GetButtonWindow(closeViewButtonWindowID).SPI_SetHelpTag(text,text);//#661
	//分割線ビュー生成
	AControlID	separatorControlID = NVM_AssignDynamicControlID();
	AViewDefaultFactory<AView_TextViewHSeparator>	separatorFactory(GetObjectID(),separatorControlID);//#199 #866
	NVM_CreateView(separatorControlID,pt,0,10,kControlID_Invalid,kControlID_Invalid,false,separatorFactory);//#866
	//#866 NVI_CreateHSeparatorView(separatorControlID,kSeparatorLinePosition_Middle,pt,0,10);
	
	//Tab内ViewのコントロールIDを登録
	NVM_RegisterViewInTab(inTabIndex,textViewControlID);
	NVM_RegisterViewInTab(inTabIndex,lineNumberControlID);
	NVM_RegisterViewInTab(inTabIndex,vScrollBarID);
	NVM_RegisterViewInTab(inTabIndex,splitButtonID);
	NVM_RegisterViewInTab(inTabIndex,separatorControlID);
	NVM_RegisterViewInTab(inTabIndex,concatButtonID);
	
	//TabDataに１つのViewData追加
	mTabDataArray.GetObject(inTabIndex).InsertViewData(inViewIndex,
				textViewControlID,lineNumberControlID,vScrollBarID,splitButtonID,separatorControlID,concatButtonID,
			closeViewButtonWindowID,//win
			inViewHeight);
	
	//TextViewに対応するスクロールバーのコントロールID設定
	GetTextViewByControlID(textViewControlID).NVI_SetScrollBarControlID(mTabDataArray.GetObjectConst(inTabIndex).GetHScrollbarControlID(),vScrollBarID);
	
	//Viewの位置、サイズ設定
	//UpdateViewBounds(inTabIndex);
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(inTabIndex,layout);
	UpdateLayout_TextView(inTabIndex,layout.pt_MainColumn,layout.w_MainColumn,layout.h_MainColumn,
						  layout.w_MainTextView,layout.w_LineNumber,layout.w_MainTextMargin,layout.pt_VScrollbarArea);
	
	//TextViewのImageサイズ設定
	GetTextViewByControlID(textViewControlID).SPI_UpdateImageSize();
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ADocument_Text::CreateSplitView() ended.",this);
}

//
void	AWindow_Text::SPI_ConcatSplitView()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		AIndex	viewIndex = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus);
		//R0187 if( viewIndex > 0 )
		if( viewIndex != kIndex_Invalid )//#212
		{
			SPI_ConcatSplitView(viewIndex);
		}
	}
}
//
void	AWindow_Text::SPI_ConcatSplitView( const AIndex inViewIndex )
{
	if( inViewIndex > 0 )//R0187
	{
		SPI_SetViewHeight(inViewIndex-1,SPI_GetViewHeight(inViewIndex-1)+SPI_GetViewHeight(inViewIndex));
		SPI_DeleteSplitView(inViewIndex);
	}
	else if( mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetViewCount() >= 2 )//R0187
	{
		SPI_SetViewHeight(1,SPI_GetViewHeight(0)+SPI_GetViewHeight(1));
		SPI_DeleteSplitView(0);
	}
}
//
void	AWindow_Text::SPI_DeleteSplitView( const AIndex inViewIndex )
{
	//現在のフォーカスのあるViewを削除しようとしているときはフォーカス移動
	if( NVI_GetFocusInTab(NVI_GetCurrentTabIndex()) == mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(inViewIndex) )
	{
		AIndex	focusViewIndex = inViewIndex+1;
		if( focusViewIndex >= mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount() )
		{
			focusViewIndex = inViewIndex-1;
		}
		NVI_SwitchFocusTo(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(focusViewIndex));
	}
	//View削除
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetLineNumberViewControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetVScrollBarControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSplitButtonControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSeparatorControlID(inViewIndex));
	NVM_UnregisterViewInTab(NVI_GetCurrentTabIndex(),mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetConcatButtonControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetLineNumberViewControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetVScrollBarControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSplitButtonControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSeparatorControlID(inViewIndex));
	NVI_DeleteControl(mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetConcatButtonControlID(inViewIndex));
	//CloseViewボタンOverlayWindowを削除 win #725
	AWindowID	closeViewButtonOverlayWindowID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetCloseViewButtonWindowID(inViewIndex);
	NVM_UnregisterOverlayWindow(closeViewButtonOverlayWindowID);//子オーバーレイウインドウ登録解除
	GetApp().NVI_DeleteWindow(closeViewButtonOverlayWindowID);
	//ViewDataの削除
	mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).DeleteViewData(inViewIndex);
	//Viewの位置、サイズ設定
	SPI_UpdateViewBounds();
}

/*#864
//指定ControlIDが属する分割ビューのIndexを取得する
AIndex	AWindow_Text::SPI_GetViewIndexByControlID( const AControlID inControlID, AIndex& outTabIndex ) const//#864
{
	outTabIndex = kIndex_Invalid;
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		AIndex	viewIndex;
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromTextViewControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromLineNumberViewControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromVScrollBarControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromSplitButtonControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
		viewIndex = mTabDataArray.GetObjectConst(tabIndex).FindFromSeparatorControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			outTabIndex = tabIndex;//#864
			return viewIndex;
		}
	}
	return kIndex_Invalid;
}
*/
//指定Indexの分割ビューの高さを取得する
ANumber	AWindow_Text::SPI_GetViewHeight( const AIndex inViewIndex ) const
{
	return mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewHeight(inViewIndex);
}

//指定Indexの分割ビューの高さを設定する（設定後、NVI_Update()必要）
void	AWindow_Text::SPI_SetViewHeight( const AIndex inViewIndex, const ANumber inHeight )
{
	mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).SetViewHeight(inViewIndex,inHeight);
}

//#866
//
const ANumber	kMinHeight_TextView = 32;

//#866
/**
テキストビュー縦分割高さ設定
*/
ANumber	AWindow_Text::ChangeTextViewHeight( const AIndex inTabIndex, const AIndex inViewIndex, const ANumber inDelta,
										  const ABool inCompleted )
{
	//元々の高さを記憶
	ANumber	origHeight = mTabDataArray.GetObjectConst(inTabIndex).GetViewHeight(inViewIndex-1);
	//分割線移動後の各高さを取得
	ANumber	aboveHeight = mTabDataArray.GetObjectConst(inTabIndex).GetViewHeight(inViewIndex-1) + inDelta;
	ANumber	height = mTabDataArray.GetObjectConst(inTabIndex).GetViewHeight(inViewIndex) - inDelta;
	//高さ補正（上のviewの最小値補正）
	if( aboveHeight < kMinHeight_TextView )
	{
		ANumber	adjust = kMinHeight_TextView - aboveHeight;
		aboveHeight = kMinHeight_TextView;
		height += adjust;
	}
	//高さ補正（下のviewの最小値補正）
	if( height < kMinHeight_TextView )
	{
		ANumber	adjust = kMinHeight_TextView - height;
		height = kMinHeight_TextView;
		aboveHeight += adjust;
	}
	//高さ設定
	mTabDataArray.GetObject(inTabIndex).SetViewHeight(inViewIndex-1,aboveHeight);
	mTabDataArray.GetObject(inTabIndex).SetViewHeight(inViewIndex,height);
	//text view配置
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(inTabIndex,layout);
	UpdateLayout_TextView(inTabIndex,layout.pt_MainColumn,layout.w_MainColumn,layout.h_MainColumn,
						  layout.w_MainTextView,layout.w_LineNumber,layout.w_MainTextMargin,layout.pt_VScrollbarArea);
	if( inCompleted == true )
	{
		//描画更新
		NVI_RefreshWindow();
	}
	//実際のdelta計算
	return mTabDataArray.GetObjectConst(inTabIndex).GetViewHeight(inViewIndex-1) - origHeight;
}

//#92
/**
TextView表示エリア全体の高さを取得（separator含む）
*/
ANumber	AWindow_Text::SPI_GetTextViewAreaHeight() const
{
	AItemCount	viewcount = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount();
	ANumber	result = kSeparatorHeight*(viewcount-1);
	for( AIndex i = 0; i < viewcount; i++ )
	{
		result += mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewHeight(i);
	}
	return result;
}

/*#199
//テキスト表示更新（ADocument_Textから指示される）
void	AWindow_Text::SPI_UpdateText( const AObjectID inTextDocumentID, 
		const AIndex inStartLineIndex, const AIndex inEndLineIndex, const AItemCount inInsertedLineCount, 
		const ABool inParagraphCountChanged )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inTextDocumentID);
	if( tabIndex != kObjectID_Invalid )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
		{
			//テキストビュー更新
			GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(viewIndex)).
					SPI_UpdateText(inStartLineIndex,inEndLineIndex,inInsertedLineCount);
			//行番号ビュー更新
			GetLineNumberViewByControlID(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(viewIndex)).
					SPI_UpdateText(inStartLineIndex,inEndLineIndex,inInsertedLineCount,inParagraphCountChanged);
		}
	}
}
*/

//フォント等のText描画プロパティが変更された場合の処理
void	AWindow_Text::SPI_UpdateTextDrawProperty( const AObjectID inTextDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inTextDocumentID);
	if( tabIndex != /*#216 kObjectID_Invalid*/kIndex_Invalid )
	{
		UpdateTextDrawProperty(tabIndex);
	}
}

void	AWindow_Text::SPI_UpdateTextDrawPropertyAll( const AModeIndex inModeIndex )
{
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			UpdateTextDrawProperty(tabIndex);
		}
	}
}

void	AWindow_Text::UpdateTextDrawProperty( const AIndex inTabIndex )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(inTabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	textViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(textViewControlID).SPI_UpdateTextDrawProperty();
		GetTextViewByControlID(textViewControlID).NVI_Refresh();
		if( textViewControlID == focus )
		{
			GetTextViewByControlID(textViewControlID).SPI_AdjustScroll_Center();
		}
		AControlID	lineNumberViewControlID = mTabDataArray.GetObject(inTabIndex).GetLineNumberViewControlID(viewIndex);
		GetLineNumberViewByControlID(lineNumberViewControlID).SPI_UpdateTextDrawProperty();
		GetLineNumberViewByControlID(lineNumberViewControlID).NVI_Refresh();
	}
	if( mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonModeIndex() != 
				GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex() )
	/*#725 リサイズ完了、Wrap等変更時には、ツールバー再作成は不要と思われる、かつ、再作成後の再配置が必要なので、コメントアウト
	もしここで再作成するように戻すなら、再配置も必要。あるいはSPI_RemakeToolButtonsAll()のほうが良いかもしれない。
	{
		//ツールバー再作成
		CreateToolButtons(inTabIndex);
	}
	*/
	//#725 InfoHeader廃止 SetInfoHeaderTextDefault();
	UpdateRulerProperty(inTabIndex);
	//#342
	AControlID	subTextPane = mTabDataArray.GetObject(inTabIndex).GetSubTextControlID();
	if( subTextPane != kControlID_Invalid )
	{
		GetTextViewByControlID(subTextPane).SPI_UpdateTextDrawProperty();
		GetTextViewByControlID(subTextPane).NVI_Refresh();
	}
	AControlID	subLineNumber = mTabDataArray.GetObject(inTabIndex).GetSubTextLineNumberControlID();
	if( subLineNumber != kControlID_Invalid )
	{
		GetLineNumberViewByControlID(subLineNumber).SPI_UpdateTextDrawProperty();
		GetLineNumberViewByControlID(subLineNumber).NVI_Refresh();
	}
	//ツールメニュー更新
	GetApp().SPI_UpdateToolMenu(GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex());
}

//
void	AWindow_Text::SPI_UpdateViewImageSize( const AObjectID inTextDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inTextDocumentID);
	if( tabIndex != /*#216 kObjectID_Invalid*/kIndex_Invalid )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
		{
			GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(viewIndex)).SPI_UpdateImageSize();
			GetLineNumberViewByControlID(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(viewIndex)).SPI_UpdateImageSize();
			GetRulerView(tabIndex).SPI_UpdateImageSize();
		}
		//#212
		AControlID	subTextPane = mTabDataArray.GetObject(tabIndex).GetSubTextControlID();
		if( subTextPane != kControlID_Invalid )
		{
			GetTextViewByControlID(subTextPane).SPI_UpdateImageSize();
		}
		AControlID	subLineNumber = mTabDataArray.GetObject(tabIndex).GetSubTextLineNumberControlID();
		if( subLineNumber != kControlID_Invalid )
		{
			GetLineNumberViewByControlID(subLineNumber).SPI_UpdateImageSize();
		}
	}
}

//#450
/**
*/
void	AWindow_Text::SPI_UpdateLineImageInfo( const AModeIndex inModeIndex )
{
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex 
					|| inModeIndex == kIndex_Invalid )
		{
			for( AIndex v = 0; v < mTabDataArray.GetObject(tabIndex).GetViewCount(); v++ )
			{
				GetTextViewByControlID(mTabDataArray.GetObject(tabIndex).GetTextViewControlID(v)).SPI_InitLineImageInfo();
				AControlID	subpaneControlID = mTabDataArray.GetObject(tabIndex).GetSubTextControlID();
				if( subpaneControlID != kControlID_Invalid )
				{
					GetTextViewByControlID(subpaneControlID).SPI_InitLineImageInfo();
				}
			}
		}
	}
	NVI_RefreshWindow();
}

/*#199
//win 080727
//指定DocumentのTextViewを再描画する
void	AWindow_Text::SPI_RefreshTextViews( const AObjectID inTextDocumentID ) const
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inTextDocumentID);
	if( tabIndex != kObjectID_Invalid )
	{
		for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
		{
			GetTextViewConstByControlID(mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex)).NVI_Refresh();
		}
	}
}
*/

//#199
/**
TextViewID取得

@param inTabIndex TabIndex
@param inViewIndex ViewのIndex
*/
/*
AViewID	AWindow_Text::GetTextViewID( const AIndex inTabIndex, const AIndex inViewIndex )
{
	return mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(inViewIndex);
}
*/

/**
指定documentID, spos, eposをウインドウ内に表示する（ドキュメントはウインドウ内に生成済みのこと）
*/
void	AWindow_Text::SPI_RevealTextViewByDocumentID( const ADocumentID inDocumentID, const AIndex inSpos, const AIndex inEpos,
		const ABool inInAnotherView, const AAdjustScrollType inAdjustScrollType )
{
	//ウインドウ選択
	NVI_SelectWindow();
	//指定ドキュメントのtabIndex取得
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	AControlID	textViewControlID = kControlID_Invalid;
	//現在のフォーカスのテキストビュー取得
	AControlID	currentFocus = GetTopMostFocusTextViewControlID();
	if( SPI_IsSubTextView(currentFocus) == true )
	{
		//------------------現在のフォーカスがサブテキストカラムの場合------------------
		if( inInAnotherView == true )
		{
			//別ビュー表示＝メインカラムに表示する
			
			//タブを選択
			NVI_SelectTab(tabIndex);
			//control ID取得
			textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(0);
		}
		else
		{
			//現在ビュー表示＝サブテキストカラムに表示する
			
			//サブテキストに表示
			SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,true,false);
			//control ID取得
			textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
		}
	}
	else
	{
		//------------------現在のフォーカスがメインテキストビューの場合------------------
		if( inInAnotherView == true )
		{
			//別ビュー表示＝サブテキストカラムに表示する
			
			//サブテキストカラム表示
			ShowHideSubTextColumn(true);
			//サブテキストに表示
			SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,true,false);
			//control ID取得
			textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
		}
		else
		{
			//現在ビュー表示＝メインテキストビューに表示する
			
			if( NVI_GetCurrentTabIndex() == tabIndex )
			{
				//タブが同じ場合は、分割ビューのフォーカスを移動せず、現在のフォーカス上に表示する
				textViewControlID = currentFocus;
			}
			else
			{
				//タブを選択
				NVI_SelectTab(tabIndex);
				//control ID取得
				textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(0);
			}
		}
	}
	if( textViewControlID != kControlID_Invalid )
	{
		//フォーカス
		NVI_SwitchFocusTo(textViewControlID);
		//指定spos,eposを選択
		if( inSpos != kIndex_Invalid && inEpos != kIndex_Invalid )
		{
			GetTextViewByControlID(textViewControlID).SPI_SetSelect(inSpos,inEpos,true);
		}
		//スクロール調整
		GetTextViewByControlID(textViewControlID).SPI_AdjustScroll(inAdjustScrollType);
	}
}

//#844
/**
キャレット表示更新
*/
void	AWindow_Text::SPI_RefreshCaret()
{
	//各メインテキストビューのキャレット表示更新
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObject(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(textViewControlID).SPI_RefreshCaret();
	}
	//サブテキストビューのキャレット表示更新
	if( mSubTextCurrentDocumentID != kObjectID_Invalid )
	{
		AIndex	subTextTabIndex = NVI_GetTabIndexByDocumentID(mSubTextCurrentDocumentID);
		AControlID	textViewControlID = mTabDataArray.GetObjectConst(subTextTabIndex).GetSubTextControlID();
		GetTextViewByControlID(textViewControlID).SPI_RefreshCaret();
	}
}

/**
ウインドウ内一番下のビューかどうかを取得
*/
ABool	AWindow_Text::SPI_GetIsBottomTextView( const AControlID inControlID ) const
{
	if( NVI_ExistView(inControlID) == false )   return false;//#212 Viewが実際に存在しているかどうかのチェック
	if( inControlID == kControlID_Invalid )   return false;//#212
	
	//タブごとのループ
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		//メインカラム内の最後のビューと比較
		AItemCount	viewCount = mTabDataArray.GetObjectConst(tabIndex).GetViewCount();
		if( mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewCount-1) == inControlID )
		{
			return true;
		}
		//サブペインのTextViewと比較
		if( mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID() == inControlID )
		{
			return true;
		}
	}
	return false;
	
}

#pragma mark ===========================

#pragma mark ---サブウインドウ
//#725

/**
サブウインドウ位置移動
*/
void	AWindow_Text::SPI_MoveOverlaySubWindow( const ASubWindowLocationType inSrcType, const AIndex inSrcIndex,
		const ASubWindowLocationType inDstType, const AIndex inDstIndex )
{
	ASubWindowType	type = kSubWindowType_None;
	AFloatNumber	height = 0;
	AWindowID	winID = kObjectID_Invalid;
	//移動元データ取得し、移動元から削除
	switch(inSrcType)
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			//移動元データ取得
			type = mLeftSideBarArray_Type.Get(inSrcIndex);
			height = mLeftSideBarArray_Height.Get(inSrcIndex);
			winID = mLeftSideBarArray_OverlayWindowID.Get(inSrcIndex);
			//移動元データ削除
			mLeftSideBarArray_Type.Delete1(inSrcIndex);
			mLeftSideBarArray_Height.Delete1(inSrcIndex);
			mLeftSideBarArray_OverlayWindowID.Delete1(inSrcIndex);
			mLeftSideBarArray_Dragging.Delete1(inSrcIndex);
			mLeftSideBarArray_Collapsed.Delete1(inSrcIndex);
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			//移動元データ取得
			type = mRightSideBarArray_Type.Get(inSrcIndex);
			height = mRightSideBarArray_Height.Get(inSrcIndex);
			winID = mRightSideBarArray_OverlayWindowID.Get(inSrcIndex);
			//移動元データ削除
			mRightSideBarArray_Type.Delete1(inSrcIndex);
			mRightSideBarArray_Height.Delete1(inSrcIndex);
			mRightSideBarArray_OverlayWindowID.Delete1(inSrcIndex);
			mRightSideBarArray_Dragging.Delete1(inSrcIndex);
			mRightSideBarArray_Collapsed.Delete1(inSrcIndex);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//移動先にデータ追加
	switch(inDstType)
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			mLeftSideBarArray_Type.Insert1(inDstIndex,type);
			mLeftSideBarArray_Height.Insert1(inDstIndex,height);
			mLeftSideBarArray_OverlayWindowID.Insert1(inDstIndex,winID);
			mLeftSideBarArray_Dragging.Insert1(inDstIndex,false);
			mLeftSideBarArray_Collapsed.Insert1(inDstIndex,false);
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			mRightSideBarArray_Type.Insert1(inDstIndex,type);
			mRightSideBarArray_Height.Insert1(inDstIndex,height);
			mRightSideBarArray_OverlayWindowID.Insert1(inDstIndex,winID);
			mRightSideBarArray_Dragging.Insert1(inDstIndex,false);
			mRightSideBarArray_Collapsed.Insert1(inDstIndex,false);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//view bounds更新
	SPI_UpdateViewBounds();
	//サイドバーデータ設定
	SetDataSideBar();
}

/**
サブウインドウ追加
*/
void	AWindow_Text::SPI_AddOverlaySubWindow( const ASubWindowLocationType inLocationType, const AIndex inIndex,
		const ASubWindowType inSubWindowType )
{
	switch(inLocationType)
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			AWindowID	winID = GetApp().SPI_CreateSubWindow(inSubWindowType,0,GetObjectID(),
						kSubWindowLocationType_LeftSideBar,inIndex,false);
			if( winID != kObjectID_Invalid )
			{
				mLeftSideBarArray_Type.Insert1(inIndex,inSubWindowType);
				mLeftSideBarArray_Height.Insert1(inIndex,100);
				mLeftSideBarArray_OverlayWindowID.Insert1(inIndex,winID);
				mLeftSideBarArray_Dragging.Insert1(inIndex,false);
				mLeftSideBarArray_Collapsed.Insert1(inIndex,false);
			}
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			AWindowID	winID = GetApp().SPI_CreateSubWindow(inSubWindowType,0,GetObjectID(),
						kSubWindowLocationType_RightSideBar,inIndex,false);
			if( winID != kObjectID_Invalid )
			{
				mRightSideBarArray_Type.Insert1(inIndex,inSubWindowType);
				mRightSideBarArray_Height.Insert1(inIndex,100);
				mRightSideBarArray_OverlayWindowID.Insert1(inIndex,winID);
				mRightSideBarArray_Dragging.Insert1(inIndex,false);
				mRightSideBarArray_Collapsed.Insert1(inIndex,false);
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//view bounds更新
	SPI_UpdateViewBounds();
	//サイドバーデータ設定
	SetDataSideBar();
}

/**
サブウインドウdrag
*/
ABool	AWindow_Text::SPI_DragSubWindow( const AWindowID inSubWindowID, const AGlobalPoint inMouseGlobalPoint )
{
	//レイアウト取得
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	
	//===============マウス位置取得===============
	//ウインドウ内のマウス位置取得
	APoint	textWindowPosition = {0};
	NVI_GetWindowPosition(textWindowPosition);
	AWindowPoint	mouseWindowPoint = {0};
	mouseWindowPoint.x = inMouseGlobalPoint.x - textWindowPosition.x;
	mouseWindowPoint.y = inMouseGlobalPoint.y - textWindowPosition.y;
	
	//右サイドバー内かどうかの判定
	if( mouseWindowPoint.x >= layout.pt_RightSideBar.x && mouseWindowPoint.x <= layout.pt_RightSideBar.x + layout.w_RightSideBar &&
				mouseWindowPoint.y >= layout.pt_RightSideBar.y && mouseWindowPoint.y <= layout.pt_RightSideBar.y + layout.h_RightSideBar )
	{
		//===============マウスが右サイドバー内にある場合===============
		
		//マウス位置に対応するサイドバー項目のindex検索
		AIndex	currentIndex = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( currentIndex == kIndex_Invalid )
		{
			//------------------右サイドバー内に未配置なら、右サイドバー内に、（dragging状態で）新規配置する。------------------
			
			//サイドバー項目配置データ計算
			AFloatNumber	subWindowHeight = GetApp().NVI_GetWindowByID(inSubWindowID).NVI_GetWindowHeight();
			AFloatNumber	sideBarHeight = layout.h_RightSideBar;
			AFloatNumber	height = subWindowHeight*sideBarHeight/(sideBarHeight-subWindowHeight);
			//サイドバーデータ追加
			currentIndex = mRightSideBarArray_Type.Add(GetApp().SPI_GetSubWindowType(inSubWindowID));
			mRightSideBarArray_Height.Add(height);
			mRightSideBarArray_OverlayWindowID.Add(inSubWindowID);
			mRightSideBarArray_Dragging.Add(true);
			mRightSideBarArray_Collapsed.Add(false);
			//サブウインドウのプロパティを更新
			GetApp().SPI_SetSubWindowProperty(inSubWindowID,kSubWindowLocationType_RightSideBar,currentIndex,GetObjectID());
			//サイドバー項目を追加したので、各サイドバーのindexを更新する。
			SPI_UpdateOverlayWindowLocationProperty();
			//view bounds更新
			SPI_UpdateViewBounds();
		}
		//追加した項目のdraggingフラグをON
		mRightSideBarArray_Dragging.Set(currentIndex,true);
		//------------------マウス位置に応じて項目移動------------------
		//マウス位置に対応するサイドバー項目検索
		ANumber	y = layout.pt_RightSideBar.y;
		for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//次のy位置取得
			ANumber	nexty = y + mRightSideBarArray_Height.Get(i);
			//サイドバー項目検索
			AIndex	dstIndex = kIndex_Invalid;
			if( i == currentIndex-1 && y + mRightSideBarArray_Height.Get(currentIndex) < nexty )
			{
				if( mouseWindowPoint.y >= y && mouseWindowPoint.y <= y + mRightSideBarArray_Height.Get(currentIndex) )
				{
					dstIndex = i;
				}
				else if( mouseWindowPoint.y >= y + mRightSideBarArray_Height.Get(currentIndex) && mouseWindowPoint.y <= nexty )
				{
					dstIndex = i+1;
				}
			}
			else
			{
				if( mouseWindowPoint.y >= y && mouseWindowPoint.y <= nexty )
				{
					dstIndex = i;
				}
			}
			//マウス位置に対応するサイドバー項目位置を見つけたら、項目を移動して終了
			if( dstIndex != kIndex_Invalid )
			{
				if( dstIndex < mRightSideBarArray_OverlayWindowID.GetItemCount() && dstIndex != currentIndex )
				{
					mRightSideBarArray_Type.Move(currentIndex,dstIndex);
					mRightSideBarArray_Height.Move(currentIndex,dstIndex);
					mRightSideBarArray_OverlayWindowID.Move(currentIndex,dstIndex);
					mRightSideBarArray_Dragging.Move(currentIndex,dstIndex);
					mRightSideBarArray_Collapsed.Move(currentIndex,dstIndex);
					//view bounds更新
					SPI_UpdateViewBounds();
					//各サイドバーのindexを更新する。
					SPI_UpdateOverlayWindowLocationProperty();
				}
				//サイドバーデータ設定
				SetDataSideBar();
				return true;
			}
			//次のyへ
			y = nexty;
		}
	}
	
	//左サイドバー内かどうかの判定
	if( mouseWindowPoint.x >= layout.pt_LeftSideBar.x && mouseWindowPoint.x <= layout.pt_LeftSideBar.x + layout.w_LeftSideBar &&
				mouseWindowPoint.y >= layout.pt_LeftSideBar.y && mouseWindowPoint.y <= layout.pt_LeftSideBar.y + layout.h_LeftSideBar )
	{
		//===============マウスが左サイドバー内にある場合===============
		
		//マウス位置に対応するサイドバー項目のindex検索
		AIndex	currentIndex = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( currentIndex == kIndex_Invalid )
		{
			//------------------左サイドバー内に未配置なら、左サイドバー内に、（dragging状態で）新規配置する。------------------
			
			//サイドバー項目配置データ計算
			AFloatNumber	subWindowHeight = GetApp().NVI_GetWindowByID(inSubWindowID).NVI_GetWindowHeight();
			AFloatNumber	sideBarHeight = layout.h_LeftSideBar;
			AFloatNumber	height = subWindowHeight*sideBarHeight/(sideBarHeight-subWindowHeight);
			//サイドバーデータ追加
			currentIndex = mLeftSideBarArray_Type.Add(GetApp().SPI_GetSubWindowType(inSubWindowID));
			mLeftSideBarArray_Height.Add(height);
			mLeftSideBarArray_OverlayWindowID.Add(inSubWindowID);
			mLeftSideBarArray_Dragging.Add(true);
			mLeftSideBarArray_Collapsed.Add(false);
			//サブウインドウのプロパティを更新
			GetApp().SPI_SetSubWindowProperty(inSubWindowID,kSubWindowLocationType_LeftSideBar,currentIndex,GetObjectID());
			//サイドバー項目を追加したので、各サイドバーのindexを更新する。
			SPI_UpdateOverlayWindowLocationProperty();
			//view bounds更新
			SPI_UpdateViewBounds();
		}
		//追加した項目のdraggingフラグをON
		mLeftSideBarArray_Dragging.Set(currentIndex,true);
		//------------------マウス位置に応じて項目移動------------------
		//マウス位置に対応するサイドバー項目検索
		ANumber	y = layout.pt_LeftSideBar.y;
		for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//次のy位置取得
			ANumber	nexty = y + mLeftSideBarArray_Height.Get(i);
			//サイドバー項目検索
			AIndex	dstIndex = kIndex_Invalid;
			if( i == currentIndex-1 && y + mLeftSideBarArray_Height.Get(currentIndex) < nexty )
			{
				if( mouseWindowPoint.y >= y && mouseWindowPoint.y <= y + mLeftSideBarArray_Height.Get(currentIndex) )
				{
					dstIndex = i;
				}
				else if( mouseWindowPoint.y >= y + mLeftSideBarArray_Height.Get(currentIndex) && mouseWindowPoint.y <= nexty )
				{
					dstIndex = i+1;
				}
			}
			else
			{
				if( mouseWindowPoint.y >= y && mouseWindowPoint.y <= nexty )
				{
					dstIndex = i;
				}
			}
			//マウス位置に対応するサイドバー項目位置を見つけたら、項目を移動して終了
			if( dstIndex != kIndex_Invalid )
			{
				if( dstIndex < mLeftSideBarArray_OverlayWindowID.GetItemCount() && dstIndex != currentIndex )
				{
					mLeftSideBarArray_Type.Move(currentIndex,dstIndex);
					mLeftSideBarArray_Height.Move(currentIndex,dstIndex);
					mLeftSideBarArray_OverlayWindowID.Move(currentIndex,dstIndex);
					mLeftSideBarArray_Dragging.Move(currentIndex,dstIndex);
					mLeftSideBarArray_Collapsed.Move(currentIndex,dstIndex);
					//view bounds更新
					SPI_UpdateViewBounds();
					//各サイドバーのindexを更新する。
					SPI_UpdateOverlayWindowLocationProperty();
				}
				//サイドバーデータ設定
				SetDataSideBar();
				return true;
			}
			//次のyへ
			y = nexty;
		}
	}
	
	//===============マウスが右サイドバー内、左サイドバー内どちらにも無い場合===============
	
	//サイドバー内にあれば、フローティングに変更する。
	{
		//右サイドバー項目から検索
		AIndex	currentIndex = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( currentIndex != kIndex_Invalid )
		{
			//右サイドバーデータから削除
			mRightSideBarArray_Type.Delete1(currentIndex);
			mRightSideBarArray_Height.Delete1(currentIndex);
			mRightSideBarArray_OverlayWindowID.Delete1(currentIndex);
			mRightSideBarArray_Dragging.Delete1(currentIndex);
			mRightSideBarArray_Collapsed.Delete1(currentIndex);
			//各サイドバーのindexを更新する。
			SPI_UpdateOverlayWindowLocationProperty();
			//view bounds更新
			SPI_UpdateViewBounds();
			//サブウインドウのプロパティを更新
			GetApp().SPI_SetSubWindowProperty(inSubWindowID,kSubWindowLocationType_Floating,kIndex_Invalid,GetObjectID());
		}
		
	}
	{
		//左サイドバー項目から検索
		AIndex	currentIndex = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( currentIndex != kIndex_Invalid )
		{
			//左サイドバーデータから削除
			mLeftSideBarArray_Type.Delete1(currentIndex);
			mLeftSideBarArray_Height.Delete1(currentIndex);
			mLeftSideBarArray_OverlayWindowID.Delete1(currentIndex);
			mLeftSideBarArray_Dragging.Delete1(currentIndex);
			mLeftSideBarArray_Collapsed.Delete1(currentIndex);
			//各サイドバーのindexを更新する。
			SPI_UpdateOverlayWindowLocationProperty();
			//view bounds更新
			SPI_UpdateViewBounds();
			//サブウインドウのプロパティを更新
			GetApp().SPI_SetSubWindowProperty(inSubWindowID,kSubWindowLocationType_Floating,kIndex_Invalid,GetObjectID());
		}
		
	}
	//サイドバーデータ設定
	SetDataSideBar();
	return false;
}

/**
サブウインドウドラッグ終了
*/
void	AWindow_Text::SPI_EndDragSubWindow( const AWindowID inSubWindowID )
{
	//draggingフラグ消去
	for( AIndex i = 0; i < mRightSideBarArray_Dragging.GetItemCount(); i++ )
	{
		mRightSideBarArray_Dragging.Set(i,false);
	}
	for( AIndex i = 0; i < mLeftSideBarArray_Dragging.GetItemCount(); i++ )
	{
		mLeftSideBarArray_Dragging.Set(i,false);
	}
	//ジャンプリストの場合の処理（フローティングジャンプリスト）
	if( GetApp().SPI_GetSubWindowType(inSubWindowID) == kSubWindowType_JumpList )
	{
		ASubWindowLocationType	locationType = GetApp().SPI_GetSubWindowLocationType(inSubWindowID);
		if( locationType == kSubWindowLocationType_Floating )
		{
			//フローティングジャンプリストを１つのみにするため、従来のジャンプリストは削除する。
			if( inSubWindowID != mFloatingJumpListWindowID && mFloatingJumpListWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_DeleteSubWindow(mFloatingJumpListWindowID);
			}
			mFloatingJumpListWindowID = inSubWindowID;
			//フローティングジャンプリスト表示・非表示の状態をapp pref dbに保存
			GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kShowFloatingJumpList,true);
		}
		else if( locationType == kSubWindowLocationType_LeftSideBar ||
					locationType == kSubWindowLocationType_RightSideBar )
		{
			//今回サイドバーになったのが、従来のフローティングウインドウであれば、
			//mFloatingJumpListWindowIDの値をクリアする。
			if( inSubWindowID == mFloatingJumpListWindowID )
			{
				mFloatingJumpListWindowID = kObjectID_Invalid;
			}
			//フローティングジャンプリスト表示・非表示の状態をapp pref dbに保存
			GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kShowFloatingJumpList,false);
		}
	}
	//view bounds更新
	SPI_UpdateViewBounds();
	//サイドバーデータ設定
	SetDataSideBar();
}

/**
サイドバーデータ更新（indexを順に並べる）
*/
void	AWindow_Text::SPI_UpdateOverlayWindowLocationProperty()
{
	for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
	{
		//
		AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(i);
		GetApp().SPI_SetSubWindowProperty(winID,kSubWindowLocationType_RightSideBar,i,GetObjectID());
	}
	for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
	{
		//
		AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(i);
		GetApp().SPI_SetSubWindowProperty(winID,kSubWindowLocationType_LeftSideBar,i,GetObjectID());
	}
}

/**
サイドバーの折りたたみ
*/
void	AWindow_Text::SPI_ExpandCollapseSubWindow( const AWindowID inSubWindowID )
{
	//右サイドバー
	{
		//検索
		AIndex	index = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( index != kIndex_Invalid )
		{
			//==================折りたたみ状態変更==================
			ABool	collapse = !(mRightSideBarArray_Collapsed.Get(index));
			mRightSideBarArray_Collapsed.Set(index,collapse);
			//==================対象項目のサイズ設定==================
			if( collapse == true )
			{
				//折りたたみ時のサイズに設定
				GetApp().NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(AApp::kSubWindowMinWidth,
							GetApp().SPI_GetSubWindowCollapsedHeight());
			}
			else
			{
				//折りたたみ解除
				//最小サイズ取得
				AFloatNumber	minHeight = GetApp().SPI_GetSubWindowMinHeight(inSubWindowID);
				GetApp().NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(AApp::kSubWindowMinWidth,minHeight);
				//最小サイズ未満なら最小サイズ設定
				if( mRightSideBarArray_Height.Get(index) < minHeight )
				{
					mRightSideBarArray_Height.Set(index,minHeight);
				}
			}
			//==================各項目の高さ補正==================
			//対象項目より後で高さ補正を行う。
			AIndex	adjustStartIndex = index+1;
			//対象項目より後が全て折りたたみ項目なら、全ての項目で高さ補正する。
			ABool	adjustable = false;
			for( AIndex i = adjustStartIndex; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
			{
				if( mRightSideBarArray_Collapsed.Get(i) == false )
				{
					adjustable = true;
				}
			}
			if( adjustable == false )
			{
				adjustStartIndex = 0;
			}
			//高さ補正実行
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetCurrentTabIndex(),layout);
			UpdateLayout_AdjustRightSideBarHeight(layout.h_RightSideBar,adjustStartIndex);
		}
		//view bounds更新
		SPI_UpdateViewBounds();
	}
	//左サイドバー
	{
		//検索
		AIndex	index = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( index != kIndex_Invalid )
		{
			//==================折りたたみ状態変更==================
			ABool	collapse = !(mLeftSideBarArray_Collapsed.Get(index));
			mLeftSideBarArray_Collapsed.Set(index,collapse);
			//==================対象項目のサイズ設定==================
			if( collapse == true )
			{
				//折りたたみ時のサイズに設定
				GetApp().NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(AApp::kSubWindowMinWidth,
							GetApp().SPI_GetSubWindowCollapsedHeight());
			}
			else
			{
				//折りたたみ解除
				//最小サイズ取得
				AFloatNumber	minHeight = GetApp().SPI_GetSubWindowMinHeight(inSubWindowID);
				GetApp().NVI_GetWindowByID(inSubWindowID).NVI_SetWindowMinimumSize(AApp::kSubWindowMinWidth,minHeight);
				//最小サイズ未満なら最小サイズ設定
				if( mLeftSideBarArray_Height.Get(index) < minHeight )
				{
					mLeftSideBarArray_Height.Set(index,minHeight);
				}
			}
			//==================各項目の高さ補正==================
			//対象項目より後で高さ補正を行う。
			AIndex	adjustStartIndex = index+1;
			//対象項目より後が全て折りたたみ項目なら、全ての項目で高さ補正する。
			ABool	adjustable = false;
			for( AIndex i = adjustStartIndex; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
			{
				if( mLeftSideBarArray_Collapsed.Get(i) == false )
				{
					adjustable = true;
				}
			}
			if( adjustable == false )
			{
				adjustStartIndex = 0;
			}
			//高さ補正実行
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetCurrentTabIndex(),layout);
			UpdateLayout_AdjustLeftSideBarHeight(layout.h_LeftSideBar,adjustStartIndex);
		}
		//view bounds更新
		SPI_UpdateViewBounds();
	}
	//サイドバーデータ設定
	SetDataSideBar();
}

/**
サブウインドウが折りたたまれているかどうかを取得
*/
ABool	AWindow_Text::SPI_IsCollapsedSubWindow( const AWindowID inSubWindowID )
{
	{
		AIndex	index = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( index != kIndex_Invalid )
		{
			return mRightSideBarArray_Collapsed.Get(index);
		}
	}
	{
		AIndex	index = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
		if( index != kIndex_Invalid )
		{
			return mLeftSideBarArray_Collapsed.Get(index);
		}
	}
	return false;
}

/**
サイドバーの表示更新
*/
void	AWindow_Text::RefreshSideBar()
{
	if( mRightSideBarDisplayed == true )
	{
		for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(i);
			GetApp().NVI_GetWindowByID(winID).NVI_RefreshWindow();
		}
	}
	if( mLeftSideBarDisplayed == true )
	{
		for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(i);
			GetApp().NVI_GetWindowByID(winID).NVI_RefreshWindow();
		}
	}
}

//#798
/**
サイドバー内サブウインドウクローズ時処理
@note AApp側でクローズされたときの同期処理
*/
void	AWindow_Text::SPI_DoSubWindowClosed( const AWindowID inSubWindowID )
{
	switch(GetApp().SPI_GetSubWindowLocationType(inSubWindowID))
	{
	  case kSubWindowLocationType_LeftSideBar:
		{
			AIndex	index = mLeftSideBarArray_OverlayWindowID.Find(inSubWindowID);
			//
			mLeftSideBarArray_Type.Delete1(index);
			mLeftSideBarArray_Height.Delete1(index);
			mLeftSideBarArray_OverlayWindowID.Delete1(index);
			mLeftSideBarArray_Dragging.Delete1(index);
			mLeftSideBarArray_Collapsed.Delete1(index);
			break;
		}
	  case kSubWindowLocationType_RightSideBar:
		{
			AIndex	index = mRightSideBarArray_OverlayWindowID.Find(inSubWindowID);
			//
			mRightSideBarArray_Type.Delete1(index);
			mRightSideBarArray_Height.Delete1(index);
			mRightSideBarArray_OverlayWindowID.Delete1(index);
			mRightSideBarArray_Dragging.Delete1(index);
			mRightSideBarArray_Collapsed.Delete1(index);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//view bounds更新
	SPI_UpdateViewBounds();
	//サイドバーデータ設定
	SetDataSideBar();
}

//#798
/**
「見出しを検索」
*/
void	AWindow_Text::SPI_SearchInHeaderList()
{
	//１．フローティングウインドウが表示中なら、フローティングウインドウ内で検索
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_IsWindowVisible() == true )
		{
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).SPI_SwitchFocusToSearchBox();
			return;
		}
	}
	//２．表示中のサイドバー内にウインドウがあれば、そのウインドウ内で検索
	if( mRightSideBarDisplayed == true )
	{
		for( AIndex i = 0; i < mRightSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(i);
			if( GetApp().SPI_GetSubWindowType(winID) == kSubWindowType_JumpList )
			{
				GetApp().SPI_GetJumpListWindow(winID).SPI_SwitchFocusToSearchBox();
				return;
			}
		}
	}
	if( mLeftSideBarDisplayed == true )
	{
		for( AIndex i = 0; i < mLeftSideBarArray_OverlayWindowID.GetItemCount(); i++ )
		{
			//
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(i);
			if( GetApp().SPI_GetSubWindowType(winID) == kSubWindowType_JumpList )
			{
				GetApp().SPI_GetJumpListWindow(winID).SPI_SwitchFocusToSearchBox();
				return;
			}
		}
	}
	//３．見出しウインドウ未表示なら、新規に生成して、そのウインドウ内で検索
	SPI_ShowHideJumpListWindow(true);
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).SPI_SwitchFocusToSearchBox();
	}
}

//#725
/**
サイドバーデータ設定
*/
void	AWindow_Text::SetDataSideBar()
{
	if( mLeftSideBarCreated == true )
	{
		//DB内サイドバーデータ全削除
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kLeftSideBarArray_Type);
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kLeftSideBarArray_Height);
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter);
		GetApp().NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed);
		GetApp().NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath);
		//現在のサイドバー項目毎のループ
		AItemCount	itemCount = mLeftSideBarArray_Type.GetItemCount();
		if( itemCount > 0 )
		{
			for( AIndex index = 0; index < itemCount; index++ )
			{
				//データ取得
				ASubWindowType	type = mLeftSideBarArray_Type.Get(index);
				ANumber	height = mLeftSideBarArray_Height.Get(index)*10000.0;
				ANumber	parameter = 0;
				ABool	collapsed = mLeftSideBarArray_Collapsed.Get(index);
				switch(type)
				{
				  case kSubWindowType_FileList:
					{
						parameter = GetApp().SPI_GetFileListWindow(mLeftSideBarArray_OverlayWindowID.Get(index)).SPI_GetCurrentMode();
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
				//現在パスを取得
				AText	currentPath;
				switch(type)
				{
				  case kSubWindowType_FileList:
					{
						GetApp().SPI_GetFileListWindow(mLeftSideBarArray_OverlayWindowID.Get(index)).SPI_GetCurrentPath(currentPath);
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
				//DBに設定
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Type,type);
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Height,height);
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter,parameter);
				GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed,collapsed);
				GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath,currentPath);
			}
		}
		else
		{
			//項目数0のときは、ダミー項目を追加しておく（項目数0で起動したとき、起動時にデフォルトデータ設定するため）
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Type,kSubWindowType_None);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Height,0);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter,0);
			GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed,false);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath,GetEmptyText());
		}
	}
	if( mRightSideBarCreated == true )
	{
		//DB内サイドバーデータ全削除
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kRightSideBarArray_Type);
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kRightSideBarArray_Height);
		GetApp().NVI_GetAppPrefDB().DeleteAll_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter);
		GetApp().NVI_GetAppPrefDB().DeleteAll_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed);
		GetApp().NVI_GetAppPrefDB().DeleteAll_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath);
		//現在のサイドバー項目毎のループ
		AItemCount	itemCount = mRightSideBarArray_Type.GetItemCount();
		if( itemCount > 0 )
		{
			for( AIndex index = 0; index < itemCount; index++ )
			{
				//データ取得
				ASubWindowType	type = mRightSideBarArray_Type.Get(index);
				ANumber	height = mRightSideBarArray_Height.Get(index)*10000.0;
				ANumber	parameter = 0;
				ABool	collapsed = mRightSideBarArray_Collapsed.Get(index);
				switch(type)
				{
				  case kSubWindowType_FileList:
					{
						parameter = GetApp().SPI_GetFileListWindow(mRightSideBarArray_OverlayWindowID.Get(index)).SPI_GetCurrentMode();
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
				//現在パスを取得
				AText	currentPath;
				switch(type)
				{
				  case kSubWindowType_FileList:
					{
						GetApp().SPI_GetFileListWindow(mRightSideBarArray_OverlayWindowID.Get(index)).SPI_GetCurrentPath(currentPath);
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
				//DBに設定
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Type,type);
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Height,height);
				GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter,parameter);
				GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed,collapsed);
				GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath,currentPath);
			}
		}
		else
		{
			//項目数0のときは、ダミー項目を追加しておく（項目数0で起動したとき、起動時にデフォルトデータ設定するため）
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Type,kSubWindowType_None);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Height,0);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter,0);
			GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed,false);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath,GetEmptyText());
		}
	}
}

#pragma mark ===========================

#pragma mark ---サブテキストカラム

/**
サブテキストカラム表示・非表示切り替え、および、それによるサブテキスト表示・diff表示モード切替
*/
void	AWindow_Text::ShowHideSubTextColumn( const ABool inShow )
{
	//サブテキストカラム表示・非表示
	ShowHideSubTextColumnCore(inShow);
	
	if( inShow == true )
	{
		//Diff表示モードかどうかによる分岐
		if( mDiffDisplayMode == false )
		{
			//現在のタブの内容をサブテキストに表示する
			SPI_DisplayTabInSubText(true);
		}
		else
		{
			//Diffをサブテキストに表示する
			SPI_UpdateDiffDisplay(NVI_GetCurrentDocumentID());
		}
	}
	else
	{
		//Diff表示モード解除
		SPI_SetDiffDisplayMode(false);
	}
}

/**
サブテキストカラム表示・非表示切り替え
*/
void	AWindow_Text::ShowHideSubTextColumnCore( const ABool inShow )
{
	//表示・非表示に変更がなければ何もしない
	if( inShow == mSubTextColumnDisplayed )
	{
		return;
	}
	//サブテキストカラム表示状態設定
	mSubTextColumnDisplayed = inShow;
	
	//bounds取得
	ARect	bounds = {0};
	NVI_GetWindowBounds(bounds);
	if( inShow == true )
	{
		// ===================== 表示 =====================
		
		if( NVI_IsFullScreen() == false )
		{
			//サブテキスト分、bounds拡張
			bounds.left -= mSubTextColumnWidth;
			
			//ウインドウサイズ変更
			UpdateViewBindings(kTextWindowViewBindings_ShowSubTextColumn);
			mDoingSetWindowBoundsAnimate = true;
			NVI_SetWindowBounds(bounds,false);
			mDoingSetWindowBoundsAnimate = false;
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
	}
	else
	{
		// ===================== 非表示 =====================
		
		if( NVI_IsFullScreen() == false )
		{
			//サブテキスト分、bounds縮める
			bounds.left += mSubTextColumnWidth;
			
			//ウインドウサイズ変更
			UpdateViewBindings(kTextWindowViewBindings_HideSubTextColumn);
			mDoingSetWindowBoundsAnimate = true;
			NVI_SetWindowBounds(bounds,false);
			mDoingSetWindowBoundsAnimate = false;
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
	}
	//view bounds更新
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		UpdateViewBounds(tabIndex);
	}
	//サブテキストカラム表示状態をDBに記憶
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kSubTextColumnDisplayed,mSubTextColumnDisplayed);
}

/**
サブテキストカラム幅変更
*/
ANumber	AWindow_Text::ChangeSubTextPaneColumnWidth( const ANumber inDelta, const ABool inCompleted )
{
	//元々の幅を取得 #725
	ANumber	origWidth = mSubTextColumnWidth;
	
	//幅変更
	mSubTextColumnWidth += inDelta;
	
	//サブペイン幅補正 #725
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	//最小値補正
	if( mSubTextColumnWidth < kMinWidth_SubTextColumn )
	{
		mSubTextColumnWidth = kMinWidth_SubTextColumn;
	}
	//最大値補正
	if( layout.w_MainColumn - inDelta < kMinWidth_MainColumn )
	{
		mSubTextColumnWidth = layout.w_MainColumn + layout.w_SubTextPaneColumn - kMinWidth_MainColumn;
	}
	
	//変更完了時処理
	if( inCompleted == true )
	{
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			//各タブのdocumentの現在のメインtext view幅を取得
			ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
			//Viewの位置、サイズ更新
			UpdateViewBounds(tabIndex);
			{
				//行情報再計算（ウインドウ幅変わるので）
				//Diff対象ドキュメントは行情報再計算しない（ただ、サブペインの幅は決まっているが、サブペイン幅に従わない設定の時に、この判定は少し問題がある）
				if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_IsDiffTargetDocument() == false )//#379
				{
					//行折り返しありの場合に再計算
					if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetWrapMode() != kWrapMode_NoWrap )
					{
						GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
					}
				}
			}
			//
			UpdateTextDrawProperty(tabIndex);
		}
		//描画更新
		NVI_RefreshWindow();
	}
	//変更中処理
	else
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex(),kUpdateViewBoundsTriggerType_SubTextColumnVSeparator);
		//NVI_RefreshWindow();
		//NVI_UpdateWindow();
		/*
		AIndex	tabIndex = NVI_GetCurrentTabIndex();
		ATextWindowLayoutData	layout = {0};
		CalculateLayout(tabIndex,layout);
		UpdateLayout_TextView(tabIndex,layout.pt_MainColumn,layout.w_MainColumn,layout.h_MainColumn,
					layout.w_MainTextView,layout.w_LineNumber,layout.pt_VScrollbarArea);
		UpdateLayout_SubTextColumn(tabIndex,layout);
		*/
	}
	//サブテキスト幅をDBに記憶
	GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kSubTextColumnWidth,mSubTextColumnWidth);
	//実際のdelta計算 #725
	return mSubTextColumnWidth - origWidth;
}

#pragma mark ===========================

#pragma mark ---サブテキスト

/**
指定controlIDがサブテキストかどうかを取得
*/
ABool	AWindow_Text::SPI_IsSubTextView( const AControlID inTextViewControlID ) const
{
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID() == inTextViewControlID )
		{
			return true;
		}
	}
	return false;
}

/**
現在のサブテキストに表示されているtext viewのcontrol IDを取得
*/
AControlID	AWindow_Text::SPI_GetCurrentSubTextControlID() const
{
	ADocumentID	docID = SPI_GetCurrentSubTextPaneDocumentID();
	if( docID == kObjectID_Invalid )   return kControlID_Invalid;
	
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(docID);
	return mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
}

#pragma mark ===========================

#pragma mark ---左サイドバー
//#212

/**
サブペイン表示・非表示切り替え
*/
void	AWindow_Text::ShowHideLeftSideBar( const ABool inShow )
{
	if( inShow == true )
	{
		//左サイドバー未生成なら生成
		if( mLeftSideBarCreated == false )
		{
			CreateLeftSideBar();
			mLeftSideBarCreated = true;
		}
		
		// ===================== 表示 =====================
		
		if( mLeftSideBarDisplayed == true )   return;
		
		//#0 Paneサポートチェック
		if( GetApp().SPI_IsSupportPaneMode() == false )   return;
		
		//サブペインを表示する
		
		//SubPane表示状態設定
		ABool	oldLeftSideBarDisplayed = mLeftSideBarDisplayed;
		mLeftSideBarDisplayed = true;
		
		if( NVI_IsFullScreen() == false )
		{
			//#341
			ARect	newBounds;
			GetChangedWindowBoundsForSubPane(oldLeftSideBarDisplayed,newBounds);
			
			//#725
			//サイドバーアニメーション用bindings設定
			UpdateViewBindings(kTextWindowViewBindings_ShowHideLeftSidebar);
			
			//サイドバー背景表示
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetCurrentTabIndex(),layout);
			NVI_SetControlPosition(kControlID_LeftSideBarBackground,layout.pt_LeftSideBar);
			NVI_SetControlSize(kControlID_LeftSideBarBackground,layout.w_LeftSideBar,layout.h_LeftSideBarWithMargin);
			NVI_SetShowControl(kControlID_LeftSideBarBackground,true);
			
			{
				//window bounds更新（アニメーション）
				AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
				//
				mDoingSetWindowBoundsAnimate = true;
				NVI_SetWindowBounds(newBounds,true);//#341 #688
				mDoingSetWindowBoundsAnimate = false;
			}
			
			//#725
			//bindingsを通常に戻す
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
		
		/* #725 サブテキストペインは常に生成するように変更。
		//全タブのSubTextPaneを生成
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			//メインTextViewのスクロール位置を取得
			AImagePoint	originPoint;
			AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(0);
			GetTextViewByControlID(mainTextViewControlID).NVI_GetOriginOfLocalFrame(originPoint);
			//SubTextPane生成
			//#699 ATextPoint	spt = {0,0}, ept = {0,0};
			CreateSubTextInEachTab(tabIndex,originPoint);
			//Viewの位置、サイズ設定
			UpdateViewBounds(tabIndex,newBounds,kUpdateViewBoundsTriggerType_ShowLeftSideBar);//#341 #645
		}
		*/
		
		//ウインドウサイズ調整
		//#341 AdjustWindowWidthForSubPane(oldLeftSideBarDisplayed);
		//#688 NVI_SetWindowBounds(newBounds);//#341
		
		//Viewの位置、サイズ設定
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			UpdateViewBounds(tabIndex,kUpdateViewBoundsTriggerType_ShowLeftSideBar);//#645
		}
		
		/*
		//表示
		//サブペイン表示更新
		AIndex	diffTabIndex = GetDiffDocumentTabIndexFromWorkingTabIndex(NVI_GetCurrentTabIndex());//#379
		if( mDiffDisplayMode == true && diffTabIndex != kIndex_Invalid )//#379
		{
			//#379 サブペインにDiff表示
			SPI_UpdateDiffDisplay(NVI_GetCurrentDocumentID());
		}
		else
		{
			if( mSubPaneMode != kSubPaneMode_Manual )
			{
				UpdateSubPaneAutomatic(mSubPaneMode,true);
			}
			else
			{
				SPI_DisplayTabInSubText(true);
			}
		}
		*/
	}
	else
	{
		// ===================== 非表示 =====================
		
		if( mLeftSideBarDisplayed == false )   return;
		
		//サブペインを非表示にする
		
		/*
		//#379 サブペイン非表示にしたらDiffモードも解除
		SPI_SetDiffDisplayMode(false);
		*/
		
		//SubPane表示状態設定
		ABool	oldLeftSideBarDisplayed = mLeftSideBarDisplayed;
		mLeftSideBarDisplayed = false;
		
		if( NVI_IsFullScreen() == false )
		{
			//#725
			//先にサブウインドウをHideする。
			//アニメーション実施中に、サブウインドウを表示しないようにするため。
			UpdateVisibleSubWindows();
			
			//#341
			ARect	newBounds;
			GetChangedWindowBoundsForSubPane(oldLeftSideBarDisplayed,newBounds);
			
			/* #725 サブテキストペインは常に生成するように変更。
		//全タブのSubTextPaneを削除
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			DeleteSubTextInEachTab(tabIndex);
			//Viewの位置、サイズ設定
			UpdateViewBounds(tabIndex,kUpdateViewBoundsTriggerType_HideLeftSideBar);//#645
		}
		*/
			
			//#725
			//サイドバーアニメーション用bindings設定
			UpdateViewBindings(kTextWindowViewBindings_ShowHideLeftSidebar);
			
			{
				//window bounds更新（アニメーション）
				AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
				//
				mDoingSetWindowBoundsAnimate = true;
				NVI_SetWindowBounds(newBounds,true);//#341 #688
				mDoingSetWindowBoundsAnimate = false;
			}
			
			//#725
			//bindingsを通常に戻す
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
		
		//Viewの位置、サイズ設定 #725
		for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
		{
			UpdateViewBounds(tabIndex,kUpdateViewBoundsTriggerType_HideLeftSideBar);
		}
		
	}
	
	//設定保存
	//#850 /*#667 NVIDO_Hide()でAAppPrefDB::kCreateTabInsteadOfCreateWindow保存と一緒に保存するように変更
	//#569 保存すべきか判断
	ABool	shouldSave = true;
	/*#850 
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true &&
				SPI_IsTabModeMainWindow() == false )
	{
		//タブウインドウモードの場合、メインウインドウ以外では保存しない
		shouldSave = false;
	}
	*/
	if( mLuaConsoleMode == true )
	{
		//Lua Consoleでは保存しない
		shouldSave = false;
	}
	//保存
	if( shouldSave == true )
	{
		GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kLeftSideBarDisplayed,mLeftSideBarDisplayed);
	}
	//#602
	UpdateVSeparatorHelpTag();
	
	//doc infoウインドウ更新 #1053
	SPI_GetCurrentTabTextDocument().SPI_UpdateDocInfoWindows();
}

/**
指定Tab内にサブテキストペイン生成
*/
void	AWindow_Text::CreateSubTextInEachTab( const AIndex inTabIndex, 
		/*#699 const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, */
		const AImagePoint& inOriginOfLocalFrame )
{
	//生成済みならすぐにリターン
	if( mTabDataArray.GetObject(inTabIndex).GetSubTextControlID() != kControlID_Invalid )   return;
	
	//（正しいサイズ、位置は、NVICB_Update()で設定される。）
	AWindowPoint	pt = {0,0};
	//CrossCaretモード取得
    //#844 ABool	crosscaretmode = false;
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentConstByID(docID).SPI_GetModeIndex();
    //#844 crosscaretmode = GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kDefaultDisplayCrossCaret);
	//TextView生成（幅は0に設定する。AView_Text::SPI_DoWrapCalculated()にて正しくadjust scrollするため）
	AControlID	textViewControlID = NVM_AssignDynamicControlID();
	ATextViewFactory	textViewFactory(GetObjectID(),textViewControlID,
						/*#695NVI_GetDocumentIDByTabIndex(inTabIndex)*/
						GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)),//#695
						GetObjectID(),//#379
						/*#699 ComparePoint(inCaretTextPoint,inSelectTextPoint),*///#699inCaretTextPoint,inSelectTextPoint,
                        inOriginOfLocalFrame,false);//#844 ,crosscaretmode);
	AViewID	textViewID = NVM_CreateView(textViewControlID,pt,0,10,kControlID_Invalid,kControlID_Invalid,true,textViewFactory);
	GetTextViewByControlID(textViewControlID).SPI_SetTextWindowID(GetObjectID());
	NVI_SetControlBindings(textViewControlID,false,true,true,true,true,false);
	//LineNumberView生成
	AImagePoint	originOfLocalFrame = inOriginOfLocalFrame;
	originOfLocalFrame.x = 0;
	AControlID	lineNumberControlID = NVM_AssignDynamicControlID();
	ALineNumberViewFactory	lineNumberViewFactory(GetObjectID(),lineNumberControlID,textViewID,NVI_GetDocumentIDByTabIndex(inTabIndex),originOfLocalFrame);
	AViewID	lineNumberViewID = NVM_CreateView(lineNumberControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false,lineNumberViewFactory);
	GetTextViewByControlID(textViewControlID).SPI_SetLineNumberView(lineNumberViewID);//#450
	NVI_SetControlBindings(lineNumberControlID,false,true,true,true,true,false);
	
	//#725
	//サブテキストHスクロールバー生成
	AControlID	hScrollbarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(hScrollbarControlID,pt,20,10);
	
	//Tab内ViewのコントロールIDを登録
	NVM_RegisterViewInTab(inTabIndex,textViewControlID);
	NVM_RegisterViewInTab(inTabIndex,lineNumberControlID);
	NVM_RegisterViewInTab(inTabIndex,hScrollbarControlID);//#725
	
	//基本的にHide状態
	NVI_SetShowControl(textViewControlID,false);
	NVI_SetShowControl(lineNumberControlID,false);
	NVI_SetShowControl(hScrollbarControlID,false);//#725
	//SelectTab()で自動表示されないようにする（表示制御はTabActivated時に、サブペインモードに従って表示する）
	NVI_SetLatentVisible(inTabIndex,textViewControlID,false);
	NVI_SetLatentVisible(inTabIndex,lineNumberControlID,false);
	NVI_SetLatentVisible(inTabIndex,hScrollbarControlID,false);//#725
	
	//TabDataに設定
	mTabDataArray.GetObject(inTabIndex).SetSubTextControlID(textViewControlID,lineNumberControlID,hScrollbarControlID);
	
	//TextViewのImageサイズ設定
	GetTextViewByControlID(textViewControlID).SPI_UpdateImageSize();
	
	//TextViewに、対応するスクロールバーのコントロールID設定
	GetTextViewByControlID(textViewControlID).NVI_SetScrollBarControlID(hScrollbarControlID,kControlID_Invalid);
	
}

/**
横分割解除
*/
void	AWindow_Text::DeleteSubTextInEachTab( const AIndex inTabIndex )
{
	//未生成ならすぐにリターン
	if( mTabDataArray.GetObject(inTabIndex).GetSubTextControlID() == kControlID_Invalid )   return;
	
	//mSubTextCurrentDocumentIDの設定解除
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	if( docID == mSubTextCurrentDocumentID )
	{
		mSubTextCurrentDocumentID = kObjectID_Invalid;
	}
	
	//View削除
	NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSubTextControlID());//win NVI_DeleteView()はmTabDataArray_ControlIDsからの削除等を行わないのでNG
	NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSubTextLineNumberControlID());//win NVI_DeleteView()はmTabDataArray_ControlIDsからの削除等を行わないのでNG
	NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSubTextHScrollbarControlID());//#725
	
	//TabDataに設定
	mTabDataArray.GetObject(inTabIndex).SetSubTextControlID(kControlID_Invalid,kControlID_Invalid,kControlID_Invalid);
}

/**
SubPane表示状態に応じてウインドウサイズ変更後のウインドウbounds取得 #341
*/
void	AWindow_Text::GetChangedWindowBoundsForSubPane( const ABool inOldSubTextPaneDisplayed, ARect& outBounds )//#341
{
	NVI_GetWindowBounds(outBounds);//#341
	if( inOldSubTextPaneDisplayed == false )
	{
		//SubTextPane表示中でなければウインドウを左に広げる
		if( mLeftSideBarDisplayed == true )
		{
			/*#341
			ARect	bounds;
			NVI_GetWindowBounds(bounds);
			*/
			outBounds.left -= mLeftSideBarWidth;
			//ウインドウの左端が見えるように調整
			AGlobalRect	availableBounds;
			//#688 CWindowImp::GetAvailableWindowPositioningBounds(availableBounds);
			NVI_GetAvailableWindowBoundsInSameScreen(availableBounds);//#688
			if( outBounds.left < availableBounds.left )
			{
				ANumber	delta = availableBounds.left - outBounds.left;
				outBounds.left += delta;
				outBounds.right += delta;
			}
			//設定
			//#341 NVI_SetWindowBounds(bounds);
		}
	}
	else
	{
		//SubTextPane表示中ならウインドウをせばめる
		if( mLeftSideBarDisplayed == false )
		{
			/*#341
			ARect	bounds;
			NVI_GetWindowBounds(bounds);
			*/
			outBounds.left += mLeftSideBarWidth;
			//念のため
			if( outBounds.right-outBounds.left < 100 )   outBounds.right = outBounds.left + 100;
			//設定
			//#341 NVI_SetWindowBounds(bounds);
		}
	}
}

/**
サブペイン幅移動
*/
ANumber	AWindow_Text::ChangeLeftSideBarWidth( const ANumber inDelta, const ABool inCompleted )//#409
{
	//ウインドウサイズ取得
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	//レイアウトデータ取得
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	
	//元々の幅を記憶 #725
	ANumber	origWidth = mLeftSideBarWidth;
	
	//サブペイン幅変更
	mLeftSideBarWidth += inDelta;
	
	//サブペイン幅補正
	if( mLeftSideBarWidth < kMinWidth_SideBar )
	{
		mLeftSideBarWidth = kMinWidth_SideBar;
	}
	if( mLeftSideBarWidth > layout.w_MainColumn + layout.w_LeftSideBar - kMinWidth_MainColumn )
	{
		mLeftSideBarWidth = layout.w_MainColumn + layout.w_LeftSideBar - kMinWidth_MainColumn;
	}
	//
	if( inCompleted == false )//#409
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex());
		NVI_UpdateWindow();
	}
	else
	{
		//==================サイズ変更完了時==================
		
		//サイズが前回の完了時と変わっている場合のみ、行情報再計算等の処理を行う（間違ってクリックだけ（ドラッグなし）したときに処理しないようにするため）
		if( mLeftSideBarWidth != mPrevLeftSideBarWidth )
		{
			//
			mPrevLeftSideBarWidth = mLeftSideBarWidth;
			
			//全タブのViewの位置、サイズ設定
			for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
			{
				UpdateViewBounds(tabIndex);
				//行情報再計算（有効ビュー幅変わるので） #324
				GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
			}
			//#324
			NVI_RefreshWindow();
			
			//DBに保存
			GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kLeftSideBarWidth,mLeftSideBarWidth);
		}
	}
	//実際のdelta計算 #725
	return mLeftSideBarWidth - origWidth;
}

/**
左サイドバー項目高さ変更
*/
ANumber	AWindow_Text::ChangeLeftSideBarHeight( const AIndex inItemIndex, const ANumber inDelta )
{
	//元々の高さ記憶
	AFloatNumber	origHeight = mLeftSideBarArray_Height.Get(inItemIndex);
	if( inDelta < 0 )
	{
		//==================高さを小さくする==================
		
		//指定サイドバー項目の最小高さ、現在の高さを取得
		AWindowID	winID0 = mLeftSideBarArray_OverlayWindowID.Get(inItemIndex);
		AFloatNumber	minHeight0 = GetApp().SPI_GetSubWindowMinHeight(winID0);
		AFloatNumber	height0 = mLeftSideBarArray_Height.Get(inItemIndex);
		//指定サイドバー項目の次以降でcollapsedではない項目を見つけ、高さを取得
		AFloatNumber	height1 = 0;
		AIndex	changeIndex = kIndex_Invalid;
		AItemCount	itemCount = mLeftSideBarArray_Height.GetItemCount();
		for( changeIndex = inItemIndex+1; changeIndex < itemCount; changeIndex++ )
		{
			if( mLeftSideBarArray_Collapsed.Get(changeIndex) == false )
			{
				height1 = mLeftSideBarArray_Height.Get(changeIndex);
				break;
			}
		}
		if( height1 != 0 )
		{
			//高さ変更（指定項目の高さを小さくし、次（以降のうち最初にcollapsedでないもの）の項目の高さをその分大きくする）
			if( height0 + inDelta >= minHeight0 )
			{
				//変更後、最小高さよりは大きい場合
				height0 += inDelta;
				height1 -= inDelta;
			}
			else
			{
				//変更後、最小高さになる場合
				height1 += height0 - minHeight0;
				height0 = minHeight0;
			}
			//高さ設定
			mLeftSideBarArray_Height.Set(inItemIndex,height0);
			mLeftSideBarArray_Height.Set(changeIndex,height1);
		}
	}
	else
	{
		//==================高さを大きくする==================
		
		//変更すべきサイズの残りをremainDeltaに格納する。
		//次以降の項目のサイズを出来る限り小さくしていって、inDeltaの値が確保できるまで続ける
		ANumber	remainDelta = inDelta;
		//次以降の項目でループ
		AItemCount	itemCount = mLeftSideBarArray_Height.GetItemCount();
		for( AIndex i = inItemIndex+1; i < itemCount; i++ )
		{
			//collapsedな項目は小さくできないので、何もしない
			if( mLeftSideBarArray_Collapsed.Get(i) == true )
			{
				continue;
			}
			//項目の最小高さ、高さを取得
			AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(i);
			AFloatNumber	minHeight = GetApp().SPI_GetSubWindowMinHeight(winID);
			AFloatNumber	height = mLeftSideBarArray_Height.Get(i);
			//出来る限り小さくして、小さくした分の値をremainDeltaから引く
			if( height - remainDelta >= minHeight )
			{
				height = height - remainDelta;
				remainDelta = 0;
			}
			else
			{
				remainDelta -= height-minHeight;
				height = minHeight;
			}
			//高さ更新
			mLeftSideBarArray_Height.Set(i,height);
			//remainDeltaが0になったら終了
			if( remainDelta <= 0 )
			{
				break;
			}
		}
		//次以降の項目を小さくできた分だけ、指定項目を大きくする
		AFloatNumber	height = mLeftSideBarArray_Height.Get(inItemIndex);
		height += inDelta-remainDelta;
		mLeftSideBarArray_Height.Set(inItemIndex,height);
	}
	
	//サイドバー再配置
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	UpdateLayout_LeftSideBar(NVI_GetCurrentTabIndex(),layout);
	UpdateViewBounds(NVI_GetCurrentTabIndex());
	//サイドバーデータ設定
	SetDataSideBar();
	//実際のdelta計算
	return mLeftSideBarArray_Height.Get(inItemIndex) - origHeight;
}

/**
サブペインで指定tabのテキストを表示する
*/
void	AWindow_Text::SPI_DisplayInSubText( const AIndex inTabIndex, 
		const ABool inScroll, const AIndex inScrollToParagraphIndex, //#450
		const ABool inSwitchFocus, const ABool inDiffDisplay )//#379
{
	//#379 サブペイン未表示なら何もせずリターン
	if( mTabDataArray.GetObjectConst(inTabIndex).GetSubTextControlID() == kControlID_Invalid )   return;
	
	//#1090
	//サブテキストカラム表示中の場合のみ、表示処理を行う。
	if( mSubTextColumnDisplayed == false )   return;
	
	//#379 ドキュメントにViewActivatedを通知
	GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_DoViewActivating();
	//			GetTextViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetSubTextControlID()).
	//			GetObjectID());
	
	//Diff表示以外なら、Diff表示モードを解除する
	if( inDiffDisplay == false && mDiffDisplayMode == true )
	{
		SPI_SetDiffDisplayMode(false);
	}
	//
	ABool	currentFocusIsInSubText = SPI_IsSubTextView(NVI_GetCurrentFocus());
	//
	mSubTextCurrentDocumentID = kObjectID_Invalid;
	//指定タブのSubPaneだけを表示する（現在の表示Tabを無視して表示・非表示する）
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		AControlID	textViewControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
		AControlID	lineNumberControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextLineNumberControlID();
		AControlID	hScrollbarControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextHScrollbarControlID();//#725
		if( textViewControlID != kControlID_Invalid )
		{
			if( inTabIndex == tabIndex )
			{
				NVI_SetShowControl(textViewControlID,true);
				NVI_SetShowControl(lineNumberControlID,true);
				//#457 NVI_SetShowControl(hScrollbarControlID,true);//#725
				//Hスクロールバーのminmax設定
				GetTextViewByControlID(textViewControlID).NVI_UpdateScrollBar();
				//Hスクロールバーの現在位置設定
				AImagePoint	originOfLocalFrame = {0};
				GetTextViewByControlID(textViewControlID).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
				//#558 NVI_SetControlNumber(hScrollbarControlID,originOfLocalFrame.x);
				GetTextViewByControlID(textViewControlID).NVI_UpdateScrollBar();//#558 スクロールバー値更新はAViewで行う（縦書き考慮）
				//スクロール
				if( inScroll == true )
				{
					GetTextViewByControlID(textViewControlID).SPI_ScrollToParagraph(inScrollToParagraphIndex,0);//#450 NVI_ScrollTo(inOriginPoint);
				}
				//サブペインファイル名ボタン更新
				mSubTextCurrentDocumentID = NVI_GetDocumentIDByTabIndex(tabIndex);
				UpdateSubTextFileNameButton();
				//サブテキストに最後に表示されていた（比較以外の）documentを記憶する（比較解除時にそれを表示するため）
				if( GetApp().SPI_GetTextDocumentByID(mSubTextCurrentDocumentID).SPI_IsDiffTargetDocument() == false )
				{
					mLastNormalSubTextDocumentID = mSubTextCurrentDocumentID;
				}
				//Focus移動
				if( inSwitchFocus == true || currentFocusIsInSubText == true )
				{
					NVI_SwitchFocusTo(textViewControlID);
				}
				//表示するタイミングにならないとViewBounds設定できないので、ここでUpdateViewBounds()しないと、
				//Open後TabをSelectしていなかったときにサブペイン表示できない
				UpdateViewBounds(tabIndex);
			}
			else
			{
				NVI_SetShowControl(textViewControlID,false);
				NVI_SetShowControl(lineNumberControlID,false);
				//#457 NVI_SetShowControl(hScrollbarControlID,false);//#725
			}
		}
	}
	//
	AControlID	subTextControlID = SPI_GetCurrentSubTextControlID();
	SPI_UpdateLineNumberButton(subTextControlID);
	UpdatePositionButton(subTextControlID);
}

/**
サブペインで現在のtabのテキストを表示する（メインtext viewと同じスクロール位置を表示）
*/
void	AWindow_Text::SPI_DisplayTabInSubText( const ABool inAdjustScroll )
{
	SPI_DisplayTabInSubText(NVI_GetCurrentTabIndex(),inAdjustScroll);
	/*#450 処理共通化
	//#450 AImagePoint	originPoint = kImagePoint_00;
	//タブ内の最終フォーカスTextViewを取得
	AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
	//もし最終フォーカスが未設定、または、SubPaneTextなら、メインペイン内の最初の分割ビューにする
	if( mainTextViewControlID == kControlID_Invalid ||
				mainTextViewControlID == mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetSubTextControlID() )
	{
		mainTextViewControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0);
	}
	//メインTextViewのスクロール位置を取得
	AIndex	originParagraph = kIndex_Invalid;//#450 
	if( IsTextViewControlID(mainTextViewControlID) == true )
	{
		originParagraph = GetTextViewByControlID(mainTextViewControlID).SPI_GetOriginParagraph();//#450 NVI_GetOriginOfLocalFrame(originPoint);
	}
	//サブペインに同じスクロール位置で表示
	SPI_DisplayInSubText(NVI_GetCurrentTabIndex(),inAdjustScroll,originParagraph);//#450 originPoint);
	*/
}

//#333
/**
サブペインに指定tabのテキストを表示する（メインtext viewと同じスクロール位置を表示）
*/
void	AWindow_Text::SPI_DisplayTabInSubText( const AIndex inTabIndex, const ABool inAdjustScroll )
{
	//#450 AImagePoint	originPoint = kImagePoint_00;
	//タブ内のメインテキストの最終フォーカスTextViewを取得
	AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(inTabIndex).GetLatentMainTextViewControlID();
	//もし最終フォーカスが未設定なら、メインペイン内の最初の分割ビューにする
	if( IsTextViewControlID(mainTextViewControlID) == false )
	{
		mainTextViewControlID = mTabDataArray.GetObjectConst(inTabIndex).GetTextViewControlID(0);
	}
	//メインTextViewのスクロール位置を取得
	AIndex	originParagraph = kIndex_Invalid;//#450 
	if( IsTextViewControlID(mainTextViewControlID) == true )
	{
		originParagraph = GetTextViewByControlID(mainTextViewControlID).SPI_GetOriginParagraph();//#450 NVI_GetOriginOfLocalFrame(originPoint);
	}
	//サブペインに同じスクロール位置で表示
	SPI_DisplayInSubText(inTabIndex,inAdjustScroll,originParagraph,false,false);//#450 originPoint);
}

/**
サブペインモードボタンテキスト更新
*/
/*#899
void	AWindow_Text::SetSubPaneMode( const ASubPaneMode inSubPaneMode )
{
	//モード更新
	mSubPaneMode = inSubPaneMode;
	*#379
	//ボタン更新
	AText	text;
	switch(mSubPaneMode)
	{
	  case kSubPaneMode_PreviousDocument:
		{
			text.SetLocalizedText("SubPaneMode_PreviousDocument");
			break;
		}
	  case kSubPaneMode_SameDocument:
		{
			text.SetLocalizedText("SubPaneMode_SameDocument");
			break;
		}
	  case kSubPaneMode_Manual:
		{
			text.SetLocalizedText("SubPaneMode_Manual");
			break;
		}
	}
	NVI_GetButtonViewByControlID(kSubPaneModeButtonControlID).NVI_SetText(text);
	*
	//DB更新
	GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kSubPaneMode,static_cast<ANumber>(mSubPaneMode));
}
*/
/**
サブペイン表示更新（指定サブペインモードに従って各タブのサブペインの表示／非表示を行う）
*/
/*#899
void	AWindow_Text::UpdateSubPaneAutomatic( const ASubPaneMode inSubPaneMode, const ABool inAdjustScroll )
{
	switch(inSubPaneMode)
	{
	  case kSubPaneMode_PreviousDocument:
		{
			if( NVI_GetSelectableTabCount() > 1 )
			{
				AIndex	previousTabIndex = NVI_GetTabIndexByZOrderIndex(1);
				if( previousTabIndex != kIndex_Invalid )
				{
					//メインTextViewのスクロール位置を取得
					AImagePoint	originPoint = kImagePoint_00;
					AControlID	mainTextViewControlID = mTabDataArray.GetObjectConst(previousTabIndex).GetTextViewControlID(0);
					//#450 GetTextViewByControlID(mainTextViewControlID).NVI_GetOriginOfLocalFrame(originPoint);
					AIndex	paragraphIndex = GetTextViewByControlID(mainTextViewControlID).SPI_GetOriginParagraph();
					//サブペインに同じスクロール位置で表示
					SPI_DisplayInSubText(previousTabIndex,inAdjustScroll,paragraphIndex,false,false);//#450 originPoint);
				}
			}
			break;
		}
	  case kSubPaneMode_SameDocument:
		{
			SPI_DisplayTabInSubText(inAdjustScroll);
			break;
		}
	}
}
*/

//#725
/**
左サイドバー生成（起動時等にコールされる。DBデータに基づいて生成）
*/
void	AWindow_Text::CreateLeftSideBar()
{
	if( GetApp().SPI_IsSupportPaneMode() == false )   return;
	
	//左サイドバーサブウインドウ数取得
	AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kLeftSideBarArray_Type);
	for( AIndex index = 0; index < itemCount; index++ )
	{
		CreateLeftSideBarItem(index);
	}
}

//#725
/**
左サイドバー項目生成（起動時等にコールされる。DBデータに基づいて生成）
*/
void	AWindow_Text::CreateLeftSideBarItem( const AIndex inIndex )
{
	//サブウインドウタイプ・高さ取得、メンバ変数への設定
	ASubWindowType	type = (ASubWindowType)(GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kLeftSideBarArray_Type,inIndex));
	AFloatNumber	height = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kLeftSideBarArray_Height,inIndex);
	ANumber	parameter = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter,inIndex);
	ABool	collapsed =  GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed,inIndex);
	if( type != kSubWindowType_None )
	{
		//サブウインドウ生成
		AWindowID	winID = GetApp().SPI_CreateSubWindow(type,parameter,GetObjectID(),
					kSubWindowLocationType_LeftSideBar,inIndex,collapsed);
		if( winID != kObjectID_Invalid )
		{
			mLeftSideBarArray_Type.Add(type);
			mLeftSideBarArray_Height.Add(height/10000.0);
			mLeftSideBarArray_Dragging.Add(false);//#725
			mLeftSideBarArray_Collapsed.Add(collapsed);//#725
			mLeftSideBarArray_OverlayWindowID.Add(winID);
		}
		//現在パスを設定
		AText	currentPath;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath,inIndex,currentPath);
		switch(type)
		{
		  case kSubWindowType_FileList:
			{
				GetApp().SPI_GetFileListWindow(winID).SPI_SetCurrentPath(currentPath);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
}

//#725
/**
左サイドバー内全サブウインドウ削除
*/
void	AWindow_Text::DeleteLeftSideBar()
{
	for( AIndex index = 0; index < mLeftSideBarArray_Type.GetItemCount(); index++ )
	{
		AWindowID	winID = mLeftSideBarArray_OverlayWindowID.Get(index);
		if( winID != kObjectID_Invalid )
		{
			GetApp().SPI_DeleteSubWindow(winID);
		}
	}
	//データ削除
	mLeftSideBarArray_Type.DeleteAll();
	mLeftSideBarArray_Height.DeleteAll();
	mLeftSideBarArray_OverlayWindowID.DeleteAll();
	mLeftSideBarArray_Dragging.DeleteAll();//#725
	mLeftSideBarArray_Collapsed.DeleteAll();//#725
}

//#725
/**
左サイドバー項目追加
*/
void	AWindow_Text::AddNewSubWindowInLeftSideBar( const ASubWindowType inSubWindowType, const ANumber inParameter,
		const ANumber inHeight )
{
	//DBに項目データ追加
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Type,inSubWindowType);
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Height,inHeight*10000);
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kLeftSideBarArray_Parameter,inParameter);
	GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kLeftSideBarArray_Collapsed,false);
	GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kLeftSideBarArray_CurrentPath,GetEmptyText());
	//左サイドバー項目生成
	CreateLeftSideBarItem(GetApp().NVI_GetAppPrefDB().GetItemCount_NumberArray(AAppPrefDB::kLeftSideBarArray_Type)-1);
	//view bounds更新
	UpdateViewBounds(NVI_GetCurrentTabIndex());
	//サイドバーデータ設定
	SetDataSideBar();
}

//#725
/**
左サイドバー項目を全てデフォルトに戻す
*/
void	AWindow_Text::SetDefaultSubWindowInLeftSideBar()
{
	//左サイドバーデータ削除
	DeleteLeftSideBar();
	//DBデータをデフォルトに戻す
	GetApp().GetAppPref().SetDefaultLeftSideBarArray();
	//左サイドバー生成
	CreateLeftSideBar();
}

/*#725
//
void	AWindow_Text::DeleteSubPaneArray()
{
	for( AIndex i = 0; i < mLeftSideBarArray_Type.GetItemCount(); i++ )
	{
		GetApp().NVI_DeleteWindow(mLeftSideBarArray_HSeparatorWindowID.Get(i));
	}
	mLeftSideBarArray_Type.DeleteAll();
	mLeftSideBarArray_Height.DeleteAll();
	mLeftSideBarArray_HSeparatorWindowID.DeleteAll();
}
*/

/**
サブペインファイル名ボタン更新
*/
void	AWindow_Text::UpdateSubTextFileNameButton()
{
	//サブテキストペインのファイル名ボタンのメニューを設定
	ATextArray	titleArray, actionTextArray;
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		//#379 Diff対象ドキュメントはメニューに追加しない（デバッグ時、ここをコメントアウトするとDiff対象ドキュメントがリストに追加できる）
		if( NVI_GetTabSelectable(tabIndex) == false )   continue;
		//タイトル
		AText	title;
		NVI_GetTitle(tabIndex,title);
		titleArray.Add(title);
		//DocIDをActionTextとして設定
		AText	actionText;
		actionText.AddNumber(tabIndex);
		actionTextArray.Add(actionText);
	}
	//メニュー設定
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).SPI_SetMenuTextArray(titleArray,actionTextArray,kMenuItemID_SubTextPaneMenu);
	
	//サブテキストファイル名ボタンテキスト更新
	UpdateSubTextFileNameButtonText();
}

/**
サブテキストファイル名ボタンテキスト更新
*/
void	AWindow_Text::UpdateSubTextFileNameButtonText()
{
	//ボタンテキスト更新
	AText	name;
	if( mSubTextCurrentDocumentID != kObjectID_Invalid )
	{
		GetApp().NVI_GetDocumentByID(mSubTextCurrentDocumentID).NVI_GetTitle(name);
		//段落数
		AText	paraText;
		paraText.SetLocalizedText("TitleText_Paragraph");
		paraText.ReplaceParamText('0',GetApp().SPI_GetTextDocumentByID(mSubTextCurrentDocumentID).SPI_GetParagraphCount());
		name.AddText(paraText);
	}
	//テキスト設定
	NVI_GetButtonViewByControlID(kSubPaneFileNameButtonControlID).NVI_SetText(name);
}

//#668
/**
サブペインカラムを含まないwindow boundsを取得
*/
/*#725
void	AWindow_Text::SPI_GetWindowBoundsWithoutSubPane( ARect& outRect ) const
{
	NVI_GetWindowBounds(outRect);
	if( mLeftSideBarDisplayed == true )
	{
		outRect.left += mLeftSideBarWidth;
	}
}
*/

#pragma mark ===========================

#pragma mark ---右サイドバー
//#291

/**
右サイドバーの表示・非表示設定
*/
void	AWindow_Text::ShowHideRightSideBar( const ABool inShow )
{
	//
	if( inShow == true )
	{
		//右サイドバー未生成なら生成
		if( mRightSideBarCreated == false )
		{
			CreateRightSideBar();
			mRightSideBarCreated = true;
		}
		
		//================ 表示 ================
		
		if( mRightSideBarDisplayed == true )   return;
		
		//#0 Paneサポートチェック
		if( GetApp().SPI_IsSupportPaneMode() == false )   return;
		
		//右サイドバー表示
		
		//InfoPane表示状態設定
		ABool	oldInfoPaneDisplayed = mRightSideBarDisplayed;
		mRightSideBarDisplayed = true;
		
		if( NVI_IsFullScreen() == false )
		{
			//右サイドバーアニメーション用bindings設定
			UpdateViewBindings(kTextWindowViewBindings_ShowRightSidebar);
			
			//サイドバー背景表示
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetCurrentTabIndex(),layout);
			NVI_SetControlPosition(kControlID_RightSideBarBackground,layout.pt_RightSideBar);
			NVI_SetControlSize(kControlID_RightSideBarBackground,layout.w_RightSideBar,layout.h_RightSideBar_WithBottomMargin);
			NVI_SetShowControl(kControlID_RightSideBarBackground,true);
			
			//window bounds設定
			ARect	bounds = {0};
			NVI_GetWindowBounds(bounds);
			bounds.right += mRightSideBarWidth;
			//
			{
				//window bounds更新（アニメーション）
				AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
				//
				mDoingSetWindowBoundsAnimate = true;
				NVI_SetWindowBounds(bounds,true);
				mDoingSetWindowBoundsAnimate = false;
			}
			
			//通常bindings設定
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
	}
	else
	{
		//================ 非表示 ================
		
		if( mRightSideBarDisplayed == false )   return;
		
		//右サイドバー非表示
		
		//InfoPane表示状態設定
		ABool	oldInfoPaneDisplayed = mRightSideBarDisplayed;
		mRightSideBarDisplayed = false;
		
		if( NVI_IsFullScreen() == false )
		{
			//先にサブウインドウをHideする。
			//アニメーション実施中に、サブウインドウを表示しないようにするため。
			UpdateVisibleSubWindows();
			
			//右サイドバーアニメーション用bindings設定
			UpdateViewBindings(kTextWindowViewBindings_HideRightSidebar);
			
			//window bounds設定
			ARect	bounds = {0};
			NVI_GetWindowBounds(bounds);
			bounds.right -= mRightSideBarWidth;
			//
			{
				//window bounds更新（アニメーション）
				AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
				//
				mDoingSetWindowBoundsAnimate = true;
				NVI_SetWindowBounds(bounds,true);
				mDoingSetWindowBoundsAnimate = false;
			}
			
			//通常bindings設定
			UpdateViewBindings(kTextWindowViewBindings_Normal);
		}
	}
	
	//
	//#326 UpdateViewBounds(NVI_GetCurrentTabIndex());
	//#326
	//全タブのViewの位置、サイズ設定
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		UpdateViewBounds(tabIndex);
		//行情報再計算（有効ビュー幅変わるので）
		//#725 サイドバー化により有効ビュー幅変わらない GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
	}
	NVI_RefreshWindow();
	
	//設定保存
	//#569 保存すべきか判断
	ABool	shouldSave = true;
	/*#850
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCreateTabInsteadOfCreateWindow) == true &&
				SPI_IsTabModeMainWindow() == false )
	{
		//タブウインドウモードの場合、メインウインドウ以外では保存しない
		shouldSave = false;
	}
	*/
	if( mLuaConsoleMode == true )
	{
		//Lua Consoleでは保存しない
		shouldSave = false;
	}
	//保存
	if( shouldSave == true )
	{
		GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kRightSideBarDisplayed,mRightSideBarDisplayed);
	}
	//#602
	UpdateVSeparatorHelpTag();
	
	//doc infoウインドウ更新 #1053
	SPI_GetCurrentTabTextDocument().SPI_UpdateDocInfoWindows();
}

/**
右サイドバー幅移動
*/
ANumber	AWindow_Text::ChangeRightSideBarWidth( const ANumber inDelta, const ABool inCompleted )//#409
{
	//ウインドウサイズ取得
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	//レイアウトデータ取得
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	
	//元々の幅を記憶 #725
	ANumber	origWidth = mRightSideBarWidth;
	
	//右サイドバー幅変更
	mRightSideBarWidth -= inDelta;
	
	//右サイドバー幅補正
	if( mRightSideBarWidth < kMinWidth_SideBar )
	{
		mRightSideBarWidth = kMinWidth_SideBar;
	}
	if( mRightSideBarWidth > layout.w_MainColumn + layout.w_RightSideBar - kMinWidth_MainColumn )
	{
		mRightSideBarWidth = layout.w_MainColumn + layout.w_RightSideBar - kMinWidth_MainColumn;
	}
	//
	if( inCompleted == false )//#409
	{
		UpdateViewBounds(NVI_GetCurrentTabIndex(),kUpdateViewBoundsTriggerType_InfoPaneVSeparator);//#645
		NVI_UpdateWindow();
	}
	else
	{
		//==================サイズ変更完了時==================
		
		//サイズが前回の完了時と変わっている場合のみ、行情報再計算等の処理を行う（間違ってクリックだけ（ドラッグなし）したときに処理しないようにするため）
		if( mRightSideBarWidth != mPrevRightSideBarWidth )
		{
			//
			mPrevRightSideBarWidth = mRightSideBarWidth;
			
			//全タブのViewの位置、サイズ設定
			for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
			{
				UpdateViewBounds(tabIndex,kUpdateViewBoundsTriggerType_InfoPaneVSeparator_Completed);//#645
				//行情報再計算（有効ビュー幅変わるので）
				GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_DoWrapRelatedDataChanged(false);
			}
			//#324
			NVI_RefreshWindow();
			
			//DBに保存
			GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kRightSideBarWidth,mRightSideBarWidth);
		}
	}
	//実際のdelta計算 #725
	return origWidth - mRightSideBarWidth;
}

/**
右サイドバー高さ変更
*/
ANumber	AWindow_Text::ChangeRightSideBarHeight( const AIndex inItemIndex, const ANumber inDelta )
{
	//元々の高さ記憶
	AFloatNumber	origHeight = mRightSideBarArray_Height.Get(inItemIndex);
	if( inDelta < 0 )
	{
		//==================高さを小さくする==================
		
		//指定サイドバー項目の最小高さ、現在の高さを取得
		AWindowID	winID0 = mRightSideBarArray_OverlayWindowID.Get(inItemIndex);
		AFloatNumber	minHeight0 = GetApp().SPI_GetSubWindowMinHeight(winID0);
		AFloatNumber	height0 = mRightSideBarArray_Height.Get(inItemIndex);
		//指定サイドバー項目の次以降でcollapsedではない項目を見つけ、高さを取得
		AFloatNumber	height1 = 0;
		AIndex	changeIndex = kIndex_Invalid;
		AItemCount	itemCount = mRightSideBarArray_Height.GetItemCount();
		for( changeIndex = inItemIndex+1; changeIndex < itemCount; changeIndex++ )
		{
			if( mRightSideBarArray_Collapsed.Get(changeIndex) == false )
			{
				height1 = mRightSideBarArray_Height.Get(changeIndex);
				break;
			}
		}
		if( height1 != 0 )
		{
			//高さ変更（指定項目の高さを小さくし、次（以降のうち最初にcollapsedでないもの）の項目の高さをその分大きくする）
			if( height0 + inDelta >= minHeight0 )
			{
				//変更後、最小高さよりは大きい場合
				height0 += inDelta;
				height1 -= inDelta;
			}
			else
			{
				//変更後、最小高さになる場合
				height1 += height0 - minHeight0;
				height0 = minHeight0;
			}
			//高さ設定
			mRightSideBarArray_Height.Set(inItemIndex,height0);
			mRightSideBarArray_Height.Set(changeIndex,height1);
		}
	}
	else
	{
		//==================高さを大きくする==================
		
		//変更すべきサイズの残りをremainDeltaに格納する。
		//次以降の項目のサイズを出来る限り小さくしていって、inDeltaの値が確保できるまで続ける
		ANumber	remainDelta = inDelta;
		//次以降の項目でループ
		AItemCount	itemCount = mRightSideBarArray_Height.GetItemCount();
		for( AIndex i = inItemIndex+1; i < itemCount; i++ )
		{
			//collapsedな項目は小さくできないので、何もしない
			if( mRightSideBarArray_Collapsed.Get(i) == true )
			{
				continue;
			}
			//項目の最小高さ、高さを取得
			AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(i);
			AFloatNumber	minHeight = GetApp().SPI_GetSubWindowMinHeight(winID);
			AFloatNumber	height = mRightSideBarArray_Height.Get(i);
			//出来る限り小さくして、小さくした分の値をremainDeltaから引く
			if( height - remainDelta >= minHeight )
			{
				height = height - remainDelta;
				remainDelta = 0;
			}
			else
			{
				remainDelta -= height-minHeight;
				height = minHeight;
			}
			//高さ更新
			mRightSideBarArray_Height.Set(i,height);
			//remainDeltaが0になったら終了
			if( remainDelta <= 0 )
			{
				break;
			}
		}
		//次以降の項目を小さくできた分だけ、指定項目を大きくする
		AFloatNumber	height = mRightSideBarArray_Height.Get(inItemIndex);
		height += inDelta-remainDelta;
		mRightSideBarArray_Height.Set(inItemIndex,height);
	}
	
	//サイドバー再配置
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(NVI_GetCurrentTabIndex(),layout);
	UpdateLayout_RightSideBar(NVI_GetCurrentTabIndex(),layout);
	UpdateViewBounds(NVI_GetCurrentTabIndex());
	//サイドバーデータ設定
	SetDataSideBar();
	//実際のdelta計算
	return mRightSideBarArray_Height.Get(inItemIndex) - origHeight;
}

/**
右サイドバーサブウインドウ生成
*/
void	AWindow_Text::CreateRightSideBar()
{
	if( GetApp().SPI_IsSupportPaneMode() == false )   return;
	
	//右サイドバーサブウインドウ数取得
	AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_Array(AAppPrefDB::kRightSideBarArray_Type);
	for( AIndex index = 0; index < itemCount; index++ )
	{
		CreateRightSideBarItem(index);
	}
}

/**
右サイドバー項目生成（起動時等にコールされる。DBデータに基づいて生成）
*/
void	AWindow_Text::CreateRightSideBarItem( const AIndex inIndex )
{
	//サブウインドウタイプ・高さ取得、メンバ変数への設定
	ASubWindowType	type = (ASubWindowType)(GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kRightSideBarArray_Type,inIndex));
	AFloatNumber	height = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kRightSideBarArray_Height,inIndex);
	ANumber	parameter = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter,inIndex);
	ABool	collapsed =  GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed,inIndex);
	if( type != kSubWindowType_None )
	{
		//サブウインドウ生成
		AWindowID	winID = GetApp().SPI_CreateSubWindow(type,parameter,GetObjectID(),
					kSubWindowLocationType_RightSideBar,inIndex,collapsed);
		if( winID != kObjectID_Invalid )
		{
			mRightSideBarArray_Type.Add(type);
			mRightSideBarArray_Height.Add(height/10000.0);
			mRightSideBarArray_Dragging.Add(false);
			mRightSideBarArray_Collapsed.Add(collapsed);
			mRightSideBarArray_OverlayWindowID.Add(winID);
		}
		//現在パスを設定
		AText	currentPath;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath,inIndex,currentPath);
		switch(type)
		{
		  case kSubWindowType_FileList:
			{
				GetApp().SPI_GetFileListWindow(winID).SPI_SetCurrentPath(currentPath);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
}

//#725
/**
右サイドバーサブウインドウ削除
*/
void	AWindow_Text::DeleteRightSideBar()
{
	//サブウインドウ削除
	for( AIndex index = 0; index < mRightSideBarArray_Type.GetItemCount(); index++ )
	{
		AWindowID	winID = mRightSideBarArray_OverlayWindowID.Get(index);
		if( winID != kObjectID_Invalid )
		{
			GetApp().SPI_DeleteSubWindow(winID);
		}
	}
	//データ削除
	mRightSideBarArray_Type.DeleteAll();
	mRightSideBarArray_Height.DeleteAll();
	mRightSideBarArray_OverlayWindowID.DeleteAll();
	mRightSideBarArray_Dragging.DeleteAll();
	mRightSideBarArray_Collapsed.DeleteAll();
}

//#725
/**
右サイドバー項目追加
*/
void	AWindow_Text::AddNewSubWindowInRightSideBar( const ASubWindowType inSubWindowType, const ANumber inParameter,
		const ANumber inHeight )
{
	//DBに項目データ追加
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Type,inSubWindowType);
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Height,inHeight*10000);
	GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kRightSideBarArray_Parameter,inParameter);
	GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kRightSideBarArray_Collapsed,false);
	GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kRightSideBarArray_CurrentPath,GetEmptyText());
	//右サイドバー項目生成
	CreateRightSideBarItem(GetApp().NVI_GetAppPrefDB().GetItemCount_NumberArray(AAppPrefDB::kRightSideBarArray_Type)-1);
	//view bounds更新
	UpdateViewBounds(NVI_GetCurrentTabIndex());
	//サイドバーデータ設定
	SetDataSideBar();
}

//#725
/**
右サイドバー項目を全てデフォルトに戻す
*/
void	AWindow_Text::SetDefaultSubWindowInRightSideBar()
{
	//右サイドバーデータ削除
	DeleteRightSideBar();
	//DBデータをデフォルトに戻す
	GetApp().GetAppPref().SetDefaultRightSideBarArray();
	//右サイドバー生成
	CreateRightSideBar();
}

/*#725
//
void	AWindow_Text::DeleteInfoPaneArray()
{
	for( AIndex i = 0; i < mRightSideBarArray_Type.GetItemCount(); i++ )
	{
		GetApp().NVI_DeleteWindow(mRightSideBarArray_HSeparatorWindowID.Get(i));
	}
	mRightSideBarArray_Type.DeleteAll();
	mRightSideBarArray_Height.DeleteAll();
	mRightSideBarArray_HSeparatorWindowID.DeleteAll();
}
*/

/*#725 
//
void	AWindow_Text::RevertFromOverlay_IdInfoWindow()
{
	GetApp().SPI_GetIdInfoWindow().NVI_ChangeToFloating();
	//Show/Hide復元
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayIdInfoWindow) == true )
	{
		GetApp().SPI_GetIdInfoWindow().NVI_Show(false);
	}
	else
	{
		GetApp().SPI_GetIdInfoWindow().NVI_Hide();
	}
}

//
void	AWindow_Text::RevertFromOverlay_FileListWindow()
{
	GetApp().SPI_GetFileListWindow().NVI_ChangeToFloating();
	//Show/Hide復元
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayWindowList) == true )
	{
		GetApp().SPI_GetFileListWindow().NVI_Show(false);
	}
	else
	{
		GetApp().SPI_GetFileListWindow().NVI_Hide();
	}
}
*/

/*#725
void	AWindow_Text::SPI_UpdateOverlayWindowAlpha()
{
	SPI_GetJumpListWindow().SPI_UpdateOverlayWindowAlpha();
}
*/

#pragma mark ===========================

#pragma mark ---ポップアップウインドウ

//各ポップアップウインドウサイズ
const ANumber	kPopupCandidateWindowHeight = 150;
const ANumber	kPopupIdInfoWindowHeight = 200;
const ANumber	kPopupIdInfoWindowWidth = 300;
const ANumber	kPopupAdjustThreasholdHeight = 30;

/**
キーワード情報ポップアップウインドウを表示／非表示
*/
void	AWindow_Text::SPI_ShowIdInfoPopupWindow( const ABool inShow )
{
	//text view全体のglobal rect取得
	AGlobalRect	viewGlobalRect = {0};
	SPI_GetCurrentFocusTextView().NVI_GetGlobalViewRect(viewGlobalRect);
	
	if( inShow == true && GetPopupIdInfoWindow().SPI_GetIdInfoView().SPI_GetIdInfoItemCount() > 0 && //mProhibitPopup == false &&
	   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup) == false &&
	   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPopupIdInfo) == true )
	{
		//レイアウト計算
		ATextWindowPopupLayoutData	layout = {0};
		mCurrentPopupLayoutPattern = CalcTextWindowPopupLayout(layout);
		//配置・表示
		GetPopupIdInfoWindow().NVI_SetWindowBounds(layout.idInfoWindowRect);
		GetPopupIdInfoWindow().NVI_Show(false);
	}
	else
	{
		//非表示
		GetPopupIdInfoWindow().NVI_Hide();
	}
}

/**
ポップアップIdInfoウインドウ取得
*/
AWindow_IdInfo&	AWindow_Text::GetPopupIdInfoWindow()
{
	//未生成なら生成
	if( mPopupIdInfoWindowID == kObjectID_Invalid )
	{
		mPopupIdInfoWindowID = GetApp().SPI_CreateSubWindow(kSubWindowType_IdInfo,0,GetObjectID(),
					kSubWindowLocationType_Popup,kIndex_Invalid,false);
		GetPopupIdInfoWindow().NVI_Create(kObjectID_Invalid);
	}
	//IdInfoウインドウ取得
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_IdInfo,kWindowType_IdInfo,mPopupIdInfoWindowID);
}

/**
候補ポップアップ
*/
void	AWindow_Text::SPI_ShowCandidatePopupWindow( const ABool inShow, const ABool inAbbrevInputMode )
{
	//ポップアップ位置設定取得
	ABool	showBelowInputText = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPopupCandidateBelowInputText);
	ABool	showNearInputText = GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPopupCandidateNearInputText);
	//
	if( inShow == true && (showBelowInputText==true||showNearInputText==true) &&// mProhibitPopup == false )
	   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup) == false )
	{
		//レイアウト計算
		ATextWindowPopupLayoutData	layout = {0};
		mCurrentPopupLayoutPattern = CalcTextWindowPopupLayout(layout);
		//配置・表示
		SPI_GetPopupCandidateWindow().NVI_SetWindowBounds(layout.candidateWindowRect);
		SPI_GetPopupCandidateWindow().NVI_Show(false);
	}
	else
	{
		//非表示
		SPI_GetPopupCandidateWindow().NVI_Hide();
	}
}

/**
ポップアップの候補リストウインドウ取得
*/
AWindow_CandidateList&	AWindow_Text::SPI_GetPopupCandidateWindow()
{
	//未生成なら生成
	if( mPopupCandidateWindowID == kObjectID_Invalid )
	{
		mPopupCandidateWindowID = GetApp().SPI_CreateSubWindow(kSubWindowType_CandidateList,0,GetObjectID(),
					kSubWindowLocationType_Popup,kIndex_Invalid,false);
		SPI_GetPopupCandidateWindow().NVI_Create(kObjectID_Invalid);
	}
	//ポップアップウインドウ生成
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_CandidateList,kWindowType_CandidateList,mPopupCandidateWindowID);
}

/**
ポップアップ補完候補ウインドウ表示中かどうかを取得
*/
ABool	AWindow_Text::SPI_IsPopupCandidateWindowVisible()
{ 
	if( mPopupCandidateWindowID == kObjectID_Invalid )   return false;
	return SPI_GetPopupCandidateWindow().NVI_IsWindowVisible();
}

/**
ポップアップウインドウの位置を現在のキャレット位置に合わせて調整
*/
void	AWindow_Text::SPI_AdjustPopupWindowsPosition()
{
	if( GetPopupIdInfoWindow().NVI_IsWindowVisible() == true )
	{
		//現在のfocusのtext viewの選択範囲global rectを取得
		AGlobalRect	textViewSelectionGlobalRect = {0};
		SPI_GetCurrentFocusTextView().SPI_GetSelectGlobalRect(textViewSelectionGlobalRect);
		//キーワード情報ポップアップウインドウのbounds取得
		ARect	winrect = {0};
		GetPopupIdInfoWindow().NVI_GetWindowBounds(winrect);
		if( textViewSelectionGlobalRect.top > winrect.top - kPopupAdjustThreasholdHeight && 
					textViewSelectionGlobalRect.top < winrect.bottom + kPopupAdjustThreasholdHeight )
		{
			//現在のレイアウトパターン
			ATextWindowPopupLayoutPattern	oldPattern = mCurrentPopupLayoutPattern;
			//レイアウト計算
			ATextWindowPopupLayoutData	layout = {0};
			mCurrentPopupLayoutPattern = CalcTextWindowPopupLayout(layout);
			
			//レイアウトパターンA→Cの場合は、ポップアップを閉じる
			ABool	shouldShow = true;
			if( (oldPattern == kTextWindowPopupLayoutPatternA && mCurrentPopupLayoutPattern == kTextWindowPopupLayoutPatternC) )
			{
				shouldShow = false;
			}
			
			//キーワード情報の項目が0ならポップアップを閉じる
			if( GetPopupIdInfoWindow().SPI_GetIdInfoView().SPI_GetIdInfoItemCount() == 0 )
			{
				shouldShow = false;
			}
			
			//表示・非表示
			SPI_ShowIdInfoPopupWindow(shouldShow);
		}
	}
	
}

/**
ツールリストポップアップウインドウ取得
*/
AWindow_KeyToolList&	AWindow_Text::SPI_GetPopupKeyToolListWindow()
{
	//未生成なら生成
	if( mPopupKeyToolListWindowID == kObjectID_Invalid )
	{
		mPopupKeyToolListWindowID = GetApp().SPI_CreateSubWindow(kSubWindowType_KeyToolList,0,GetObjectID(),
					kSubWindowLocationType_Popup,kIndex_Invalid,false);
		SPI_GetPopupKeyToolListWindow().NVI_Create(kObjectID_Invalid);
	}
	//ポップアップウインドウ生成
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_KeyToolList,kWindowType_KeyToolList,mPopupKeyToolListWindowID);
}

/**
ポップアップウインドウのレイアウト計算
*/
ATextWindowPopupLayoutPattern	AWindow_Text::CalcTextWindowPopupLayout( ATextWindowPopupLayoutData& outLayout ) 
{
	//text viewの現在の選択範囲のgloba rectを取得
	AGlobalRect	textViewCurrentWordGlobalRect = {0};
	SPI_GetCurrentFocusTextView().SPI_GetCurrentWordGlobalRect(textViewCurrentWordGlobalRect);
	
	//text view全体のglobal rect取得
	AGlobalRect	viewGlobalRect = {0};
	SPI_GetCurrentFocusTextView().NVI_GetGlobalViewRect(viewGlobalRect);
	
	//==================候補ウインドウ配置==================
	
	//
	ABool	candidateWindowDisplayedBelowSelection = true;
	
	//候補ウインドウの最大高さ（viewのイメージ高さ＋テキストパネル高さ）を取得
	ANumber	candidateWindowMaxHeight = SPI_GetPopupCandidateWindow().SPI_GetMaxWindowHeight();
	//最大高さよりは高くならないようにする
	ANumber	candidateWindowHeight = kPopupCandidateWindowHeight;
	if( candidateWindowHeight > candidateWindowMaxHeight )
	{
		candidateWindowHeight = candidateWindowMaxHeight;
	}
	
	//
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPopupCandidateNearInputText) == true )
	{
		//------------------テキスト入力位置の”近く”にポップアップ------------------
		
		//
		const ANumber	kCandidateWindowYOffset = 80;
		const ANumber	kCandidateWindowRightMargin = 5;
		
		//
		outLayout.candidateWindowRect.right 	= viewGlobalRect.right - kCandidateWindowRightMargin;
		outLayout.candidateWindowRect.left 		= outLayout.candidateWindowRect.right - SPI_GetPopupCandidateWindow().NVI_GetWindowWidth();
		
		//上か下かを判定
		//（現在の選択範囲下の表示可能領域の高さ＞候補ウインドウの高さ＋オフセットなら下に表示）
		if( viewGlobalRect.bottom - textViewCurrentWordGlobalRect.bottom > kCandidateWindowYOffset + /*candidateWindowHeight*/kPopupCandidateWindowHeight )
		{
			//下に表示可能なら下に表示する
			
			//
			outLayout.candidateWindowRect.top		= textViewCurrentWordGlobalRect.bottom + kCandidateWindowYOffset;
			outLayout.candidateWindowRect.bottom	= outLayout.candidateWindowRect.top + candidateWindowHeight;
		}
		else
		{
			//下に表示不可能なら上に表示する
			
			//
			outLayout.candidateWindowRect.bottom	= textViewCurrentWordGlobalRect.top - kCandidateWindowYOffset;
			outLayout.candidateWindowRect.top		= outLayout.candidateWindowRect.bottom - candidateWindowHeight;
			
			//
			candidateWindowDisplayedBelowSelection = false;
		}
	}
	else
	{
		//------------------テキスト入力位置のすぐ下（または上）にポップアップ------------------
		
		//
		const ANumber	kCandidateWindowYOffset = 16;
		
		//
		ANumber	keywordStartOffset = SPI_GetPopupCandidateWindow().SPI_GetCandidateListViewConst().SPI_GetKeywordStartX();
		AWindowPoint	listViewPoint = {0};
		SPI_GetPopupCandidateWindow().NVI_GetControlPosition(
			SPI_GetPopupCandidateWindow().SPI_GetCandidateListViewConst().NVI_GetControlID(),listViewPoint);
		
		//
		outLayout.candidateWindowRect.left		= textViewCurrentWordGlobalRect.left - keywordStartOffset - listViewPoint.x;
		outLayout.candidateWindowRect.right		= outLayout.candidateWindowRect.left + SPI_GetPopupCandidateWindow().NVI_GetWindowWidth();
		
		//上か下かを判定
		//（現在の選択範囲下の表示可能領域の高さ＞候補ウインドウの高さ＋オフセットなら下に表示）
		if( viewGlobalRect.bottom - textViewCurrentWordGlobalRect.bottom > kCandidateWindowYOffset + /*candidateWindowHeight*/kPopupCandidateWindowHeight )
		{
			//下に表示可能なら下に表示する
			
			//
			outLayout.candidateWindowRect.top		= textViewCurrentWordGlobalRect.bottom + kCandidateWindowYOffset;
			outLayout.candidateWindowRect.bottom	= outLayout.candidateWindowRect.top + candidateWindowHeight;
		}
		else
		{
			//下に表示不可能なら上に表示する
			
			//
			outLayout.candidateWindowRect.bottom	= textViewCurrentWordGlobalRect.top - kCandidateWindowYOffset;
			outLayout.candidateWindowRect.top		= outLayout.candidateWindowRect.bottom - candidateWindowHeight;
			
			//
			candidateWindowDisplayedBelowSelection = false;
		}
	}
	
	//==================キーワード情報ウインドウ配置==================
	
	const ANumber	kIdInfoWindowYOffset1 = 80;
	const ANumber	kIdInfoWindowYOffset2 = 20;
	
	//キーワード情報ウインドウの最大高さを取得
	ANumber	idInfoWindowMaxHeight = GetPopupIdInfoWindow().SPI_GetMaxWindowHeight();
	//キーワード情報ウインドウ項目の最大高さを取得
	ANumber	idInfoBoxMaxHeight = GetPopupIdInfoWindow().SPI_GetIdInfoView().SPI_GetMaxItemHeight();
	//最大高さよりは高くならないようにし、項目最大高さよりは低くならないようにする
	ANumber	idInfoWindowHeight = kPopupIdInfoWindowHeight;
	if( idInfoWindowHeight < idInfoBoxMaxHeight )
	{
		idInfoWindowHeight = idInfoBoxMaxHeight;
	}
	if( idInfoWindowHeight > idInfoWindowMaxHeight )
	{
		idInfoWindowHeight = idInfoWindowMaxHeight;
	}
	
	//
	outLayout.idInfoWindowRect.right = viewGlobalRect.right - 2;
	outLayout.idInfoWindowRect.left = outLayout.idInfoWindowRect.right - kPopupIdInfoWindowWidth;
	
	//上部の表示可能領域を計算
	ANumber	spaceAboveSelection = textViewCurrentWordGlobalRect.top - viewGlobalRect.top;
	if( candidateWindowDisplayedBelowSelection == false )
	{
		spaceAboveSelection = outLayout.candidateWindowRect.top - viewGlobalRect.top;
	}
	
	//
	ATextWindowPopupLayoutPattern	layoutPattern = kTextWindowPopupLayoutPatternA;
	
	//上に表示可能かどうかを判定
	if( spaceAboveSelection > kIdInfoWindowYOffset1 + idInfoWindowHeight )
	{
		//上に表示可能なら、上に表示
		
		//
		if( candidateWindowDisplayedBelowSelection == false )
		{
			//候補ウインドウを上に表示する場合は、候補ウインドウの上に表示する。（パターンB）
			outLayout.idInfoWindowRect.bottom = outLayout.candidateWindowRect.top - kIdInfoWindowYOffset2;
			outLayout.idInfoWindowRect.top = outLayout.idInfoWindowRect.bottom - idInfoWindowHeight;
			layoutPattern = kTextWindowPopupLayoutPatternB;
		}
		else
		{
			//候補ウインドウを下に表示する場合は、選択範囲の上に表示する。（パターンA）
			outLayout.idInfoWindowRect.bottom = textViewCurrentWordGlobalRect.top - kIdInfoWindowYOffset1;
			outLayout.idInfoWindowRect.top = outLayout.idInfoWindowRect.bottom - idInfoWindowHeight;
			layoutPattern = kTextWindowPopupLayoutPatternA;
		}
	}
	else
	{
		//上に表示不可能なら、候補ウインドウの下に表示（パターンC）
		
		outLayout.idInfoWindowRect.top = outLayout.candidateWindowRect.top + kIdInfoWindowYOffset2;
		outLayout.idInfoWindowRect.bottom = outLayout.idInfoWindowRect.top + idInfoWindowHeight;
		layoutPattern = kTextWindowPopupLayoutPatternC;
	}
	return layoutPattern;
}

/**
全てのポップアップウインドウを閉じる
*/
void	AWindow_Text::SPI_HideAllPopupWindows()
{
	GetPopupIdInfoWindow().NVI_Hide();
	SPI_GetPopupCandidateWindow().NVI_Hide();
}

#pragma mark ===========================

#pragma mark ---Diff表示
//#379

/**
Diff表示を更新（diffデータ更新時）
*/
void	AWindow_Text::SPI_UpdateDiffDisplay( const ADocumentID inDocumentID )
{
	//現在表示中のドキュメントが対象ドキュメントの場合のみ更新
	if( inDocumentID != NVI_GetCurrentDocumentID() )   return;
	
	//Workingのほうのタブを取得
	AIndex	workingTabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	//diff対象のほうのタブを取得
	AIndex	diffDocTabIndex = GetDiffDocumentTabIndexFromWorkingTabIndex(workingTabIndex);
	if( mDiffDisplayMode == true && diffDocTabIndex != kIndex_Invalid )
	{
		//Repositoryをサブペインに表示
		//#450 AImagePoint	origin = {0,0};
		SPI_DisplayInSubText(diffDocTabIndex,false,kIndex_Invalid,false,true);
		//スクロール位置を合わせる
		AControlID	view0 = mTabDataArray.GetObjectConst(workingTabIndex).GetTextViewControlID(0);
		SPI_AdjustDiffDisplayScroll(inDocumentID,view0,kIndex_Invalid);
	}
	//Diff表示更新
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
	//Diffボタン表示更新
	SPI_UpdateDiffButtonText();
}

/**
Diff表示のスクロール合わせ（と行番号View内のDiff対応表示更新）を行う
*/
void	AWindow_Text::SPI_AdjustDiffDisplayScroll( const ADocumentID inWorkingDocID, const AControlID inWorkingViewControlID, const AIndex inCurrentDiffIndex )
{
	//dirty時はスクロール合わせしない
	if( GetApp().SPI_GetTextDocumentByID(inWorkingDocID).NVI_IsDirty() == true )
	{
		return;
	}
	
	//分割ビューの２番目以降の場合はスクロール動機させない
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inWorkingDocID);
	if( mTabDataArray.GetObjectConst(tabIndex).FindFromTextViewControlID(inWorkingViewControlID) > 0 )   return;
	
#if 0
	if( mIsMainSubDiffMode == true )
	{
		//スクロールしたドキュメントがmain textでない場合は、行番号Viewの表示更新だけして終了
		if( inWorkingViewControlID != mMainSubDiff_MainTextControlID )
		{
			//Diff表示更新 #379
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
#if IMPLEMENTATION_FOR_WINDOWS
			//★ Macではこれがないほうがちらつきなし。Windowsではこれがあるほうがちらつきなし。
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_UpdateWindow();
#endif
			return;
		}
		
		//Y方向同期
		AImagePoint	rightOrigin = {0};
		GetTextViewByControlID(mMainSubDiff_MainTextControlID).NVI_GetOriginOfLocalFrame(rightOrigin);
		//Main Textのフレーム開始段落index取得
		AIndex	viewStartParagraphIndex = GetTextViewByControlID(mMainSubDiff_MainTextControlID).SPI_GetViewStartParagraphIndex();
		AIndex	viewEndParagraphIndex = GetTextViewByControlID(mMainSubDiff_MainTextControlID).SPI_GetViewEndParagraphIndex();
		//右側最初のDiffIndexを取得し、右側最初のDiffがあれば、左側の対応するDiffがちょうど同じ高さに表示されるようにスクロール
		for( AIndex paraIndex = viewStartParagraphIndex; paraIndex < viewEndParagraphIndex; paraIndex++ )
		{
			AIndex	rightFirstDiffIndex = SPI_MainSubDiff_GetDiffIndexByStartParagraphIndex(paraIndex);
			if( rightFirstDiffIndex != kIndex_Invalid )
			{
				AIndex	rightFirstDiffLineIndex = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID).
						SPI_GetLineIndexByParagraphIndex(mMainSubDiff_StartParagraphIndexArray.Get(rightFirstDiffIndex));
				AIndex	leftFirstDiffLineIndex = SPI_GetTextDocumentConst(mMainSubDiff_SubTextControlID).
						SPI_GetLineIndexByParagraphIndex(mMainSubDiff_OldStartParagraphIndexArray.Get(rightFirstDiffIndex));
				//
				ANumber	offset = GetTextViewByControlID(mMainSubDiff_MainTextControlID).GetImageYByLineIndex(rightFirstDiffLineIndex)
						- rightOrigin.y;
				AImagePoint	leftOrigin = {0};
				GetTextViewByControlID(mMainSubDiff_SubTextControlID).NVI_GetOriginOfLocalFrame(leftOrigin);
				leftOrigin.y = GetTextViewByControlID(mMainSubDiff_SubTextControlID).GetImageYByLineIndex(leftFirstDiffLineIndex)
						- offset;
				GetTextViewByControlID(mMainSubDiff_SubTextControlID).NVI_ScrollTo(leftOrigin);
				break;
			}
		}
		//X方向同期
		AImagePoint	leftOrigin = {0};
		GetTextViewByControlID(mMainSubDiff_SubTextControlID).NVI_GetOriginOfLocalFrame(leftOrigin);
		leftOrigin.x = rightOrigin.x;
		//スクロール実行
		GetTextViewByControlID(mMainSubDiff_SubTextControlID).NVI_ScrollTo(leftOrigin);
		//Diff表示更新
		GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
#if IMPLEMENTATION_FOR_WINDOWS
		//★ Macではこれがないほうがちらつきなし。Windowsではこれがあるほうがちらつきなし。
		GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_UpdateWindow();
#endif
		//#611 diffInfoWindowの表示と、本ウインドウの表示のタイミングが微妙にずれる問題の対策
		NVI_UpdateWindow();
	}
	//Diff表示モード判定
	else 
#endif
	if( mDiffDisplayMode == true )
	{
		//スクロールしたドキュメント(inWorkingDocID)がDiff対象ドキュメントの場合は、行番号Viewの表示更新だけして終了
		ADocumentID	workingDocID = GetApp().SPI_GetTextDocumentByID(inWorkingDocID).SPI_GetDiffWorkingDocumentID();
		if( workingDocID != kObjectID_Invalid )
		{
			//Diff表示更新 #379
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
#if IMPLEMENTATION_FOR_WINDOWS
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_UpdateWindow();
#endif
			NVI_UpdateWindow();
			return;
		}
		
		//==================Working側のスクロール位置に合わせてDiff側のスクロール位置を調整する==================
		//（ここ以降は、inWorkingDocIDは常に、working側のドキュメントである。）
		
		//Diff対象ドキュメント取得
		ADocumentID	diffTargetDocID = GetApp().SPI_GetTextDocumentByID(inWorkingDocID).SPI_GetDiffTargetDocumentID();
		if( diffTargetDocID == kObjectID_Invalid )   return;
		
		//Workingドキュメントのフレーム開始段落index取得
		AIndex	viewStartParagraphIndex = GetTextViewByControlID(inWorkingViewControlID).SPI_GetViewStartParagraphIndex();
		AIndex	viewEndParagraphIndex = GetTextViewByControlID(inWorkingViewControlID).SPI_GetViewEndParagraphIndex();
		//対応するRepositoryドキュメントでの段落index取得
		AIndex	diffViewStartParaIndex = GetApp().SPI_GetTextDocumentByID(inWorkingDocID).
				SPI_GetDiffTargetParagraphIndexFromThisDocumentParagraphIndex(viewStartParagraphIndex);
		AIndex	diffViewTabIndex = NVI_GetTabIndexByDocumentID(diffTargetDocID);
		if( diffViewTabIndex == kIndex_Invalid )   return;
		AControlID	diffTextViewControlID = mTabDataArray.GetObjectConst(diffViewTabIndex).GetSubTextControlID();
		if( diffTextViewControlID != kControlID_Invalid )
		{
			if( GetTextViewByControlID(diffTextViewControlID).NVI_IsVisible() == true )
			{
				//Workingドキュメントのorigin取得
				AImagePoint	workingViewOrigin = {0,0};
				GetTextViewByControlID(inWorkingViewControlID).NVI_GetOriginOfLocalFrame(workingViewOrigin);
				//
				ATextPoint	selSpt = {0}, selEpt = {0};
				GetTextViewByControlID(inWorkingViewControlID).SPI_GetSelect(selSpt,selEpt);
				AImagePoint	selImageSpt = {0}, selImageEpt = {0}; 
				GetTextViewByControlID(inWorkingViewControlID).SPI_GetImageSelect(selImageSpt,selImageEpt);
				AImageRect	workingImageFrame = {0};
				GetTextViewByControlID(inWorkingViewControlID).NVI_GetImageViewRect(workingImageFrame);
				if( selImageSpt.y >= workingImageFrame.top && selImageSpt.y <= workingImageFrame.bottom )
				{
					//------------------working側選択開始位置が現在の表示フレーム内の場合------------------
					
					ANumber	offset = selImageSpt.y - workingImageFrame.top;
					//diff側の対応するtext point取得
					ATextPoint	diffSelSpt = {0};
					GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
							SPI_GetCorrespondingTextPointByDiffTargetTextPoint(selSpt,diffSelSpt);
					//
					AImagePoint	diffSelIpt = {0};
					GetTextViewByControlID(diffTextViewControlID).SPI_GetImagePointFromTextPoint(diffSelSpt,diffSelIpt);
					//
					AImagePoint	diffOrigin = {0};
					GetTextViewByControlID(diffTextViewControlID).NVI_GetOriginOfLocalFrame(diffOrigin);
					//
					AWindowPoint	workingViewWpt = {0};
					NVI_GetControlPosition(inWorkingViewControlID,workingViewWpt);
					AWindowPoint	diffViewWpt = {0};
					NVI_GetControlPosition(diffTextViewControlID,diffViewWpt);
					ANumber	viewPositionOffset = diffViewWpt.y - workingViewWpt.y;
					//
					diffOrigin.y = diffSelIpt.y - offset + viewPositionOffset;
					GetTextViewByControlID(diffTextViewControlID).NVI_ScrollTo(diffOrigin,true,false);
				}
				//Diff Index指定モード判定
				else if( inCurrentDiffIndex != kIndex_Invalid )
				{
					//3.現在のキャレット位置に対応するDiffパートを表示できるようにDiff側のスクロール調整
					AIndex	diffStartLineIndex = GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
							SPI_GetDiffStartLineIndexByDiffIndex(inCurrentDiffIndex);
					AIndex	diffEndLineIndex = GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
							SPI_GetDiffEndLineIndexByDiffIndex(inCurrentDiffIndex);
					GetTextViewByControlID(diffTextViewControlID).
							SPI_AdjustScroll_Center(diffStartLineIndex,diffEndLineIndex,workingViewOrigin.x);
				}
				else 
				{
					//Y方向同期
					if( diffViewStartParaIndex != kIndex_Invalid )
					{
						//1. 左右同じ行が対応するようなoriginImagePtを計算
						
						AImagePoint	originImagePt1 = {0,0};
						//行番号取得
						AIndex	diffViewStartLineIndex = GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
								SPI_GetLineIndexByParagraphIndex(diffViewStartParaIndex);
						//
						ALocalPoint	lpt = {0,0};
						AWindowPoint	wpt = {0,0};
						NVI_GetWindowPointFromControlLocalPoint(inWorkingViewControlID,lpt,wpt);
						//
						ALocalPoint	difflpt = {0,0};
						AWindowPoint	diffwpt = {0,0};
						NVI_GetWindowPointFromControlLocalPoint(diffTextViewControlID,difflpt,diffwpt);
						//
						originImagePt1.y = GetTextViewByControlID(diffTextViewControlID).
								GetImageYByLineIndex(diffViewStartLineIndex) + diffwpt.y-wpt.y;
						if( originImagePt1.y < 0 )   originImagePt1.y =0;
						
						//2. 右側最初のDiffに対応する左側行がtopに表示されるようなoriginImagePtを計算
						
						//右側最初のDiffIndexを取得し、右側最初のDiffがあれば、左側の行番号を取得
						AIndex	rightFirstDiffIndex = kIndex_Invalid;
						AIndex	leftFirstDiffLineIndex = kIndex_Invalid;
						for( AIndex paraIndex = viewStartParagraphIndex; paraIndex < viewEndParagraphIndex; paraIndex++ )
						{
							rightFirstDiffIndex = GetApp().SPI_GetTextDocumentByID(inWorkingDocID).
									SPI_GetDiffIndexByStartParagraphIndex(paraIndex);
							if( rightFirstDiffIndex != kIndex_Invalid )
							{
								leftFirstDiffLineIndex = GetApp().SPI_GetTextDocumentByID(diffTargetDocID).
										SPI_GetDiffStartLineIndexByDiffIndex(rightFirstDiffIndex);
								break;
							}
						}
						//
						AImagePoint	originImagePt2 = {0,0};
						if( leftFirstDiffLineIndex != kIndex_Invalid )
						{
							leftFirstDiffLineIndex -= 3;
							if( leftFirstDiffLineIndex < 0 )   leftFirstDiffLineIndex = 0;
							originImagePt2.y = GetTextViewByControlID(diffTextViewControlID).
									GetImageYByLineIndex(leftFirstDiffLineIndex);
						}
						
						//2<1になったら、2を採用。それ以外は1を採用。
						if( leftFirstDiffLineIndex != kIndex_Invalid && originImagePt2.y < originImagePt1.y )
						{
							GetTextViewByControlID(diffTextViewControlID).NVI_ScrollTo(originImagePt2,true,false);
						}
						else
						{
							GetTextViewByControlID(diffTextViewControlID).NVI_ScrollTo(originImagePt1,true,false);
						}
					}
					//X方向同期
					AImagePoint	diffViewOrigin = {0,0};
					GetTextViewByControlID(diffTextViewControlID).NVI_GetOriginOfLocalFrame(diffViewOrigin);
					diffViewOrigin.x = workingViewOrigin.x;
					//スクロール実行
					GetTextViewByControlID(diffTextViewControlID).NVI_ScrollTo(diffViewOrigin,true,false);
				}
			}
			//Diff表示更新
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
#if IMPLEMENTATION_FOR_WINDOWS
			GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_UpdateWindow();
#endif
		}
		//#611 diffInfoWindowの表示と、本ウインドウの表示のタイミングが微妙にずれる問題の対策
		NVI_UpdateWindow();
	}
}

/**
（Diff側）段落indexから、WindowPointを取得
*/
void	AWindow_Text::SPI_GetDiffViewWindowPointByParagraph( const ADocumentID inDocID, const AIndex inDiffParagraphStart,
		AWindowPoint& outWindowPoint ) const
{
	outWindowPoint.x = 0;
	outWindowPoint.y = 0;
	//Diff対象ドキュメント・タブ・ビュー取得
	ADocumentID	diffTargetDocID = GetApp().SPI_GetTextDocumentByID(inDocID).SPI_GetDiffTargetDocumentID();
	AIndex	diffTargetDocTabIndex = NVI_GetTabIndexByDocumentID(diffTargetDocID);
	AControlID	diffTextViewControlID = mTabDataArray.GetObjectConst(diffTargetDocTabIndex).GetSubTextControlID();
	if( diffTextViewControlID != kControlID_Invalid )
	{
		if( GetTextViewConstByControlID(diffTextViewControlID).NVI_IsVisible() == true )
		{
			//（Diff側）段落indexから、WindowPointを取得
			AIndex	lineIndex = GetApp().SPI_GetTextDocumentConstByID(diffTargetDocID).SPI_GetLineIndexByParagraphIndex(inDiffParagraphStart);
			ALocalPoint	lpt = {0,0};
			lpt.y = GetTextViewConstByControlID(diffTextViewControlID).SPI_GetLocalYByLineIndex(lineIndex);
			NVI_GetWindowPointFromControlLocalPoint(diffTextViewControlID,lpt,outWindowPoint);
		}
	}
}

/**
WorkingドキュメントのTabIndexからRepositoryドキュメントのTabIndexを取得
*/
AIndex	AWindow_Text::GetDiffDocumentTabIndexFromWorkingTabIndex( const AIndex inTabIndex )
{
	ADocumentID	workDocID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	ADocumentID	diffTargetDocID = GetApp().SPI_GetTextDocumentByID(workDocID).SPI_GetDiffTargetDocumentID();
	if( diffTargetDocID == kObjectID_Invalid )   return kIndex_Invalid;
	return NVI_GetTabIndexByDocumentID(diffTargetDocID);
}

/**
Diff表示モード設定
*/
void	AWindow_Text::SPI_SetDiffDisplayMode( const ABool inDiffDisplayMode )
{
	if( GetApp().SPI_IsSupportPaneMode() == false )   return;//未サポートならリターン
	
	if( mDiffDisplayMode != inDiffDisplayMode )
	{
		mDiffDisplayMode = inDiffDisplayMode;
		NVI_GetButtonViewByControlID(kSubPaneDiffButtonControlID).SPI_SetToogleOn(inDiffDisplayMode);
		//
		//#899 SetSubPaneMode(kSubPaneMode_Manual);
		if( inDiffDisplayMode ==true )
		{
			//==================表示==================
			
			//サブテキスト表示
			ShowHideSubTextColumnCore(true);
			//Diffをサブテキストに表示する
			SPI_UpdateDiffDisplay(NVI_GetCurrentDocumentID());
		}
		else
		{
			//==================非表示==================
			
			//サブテキスト表示中の場合は、そこに通常サブペインを表示する
			if( mSubTextColumnDisplayed == true )
			{
				//最後に表示されていたドキュメントをサブテキストに表示する
				AIndex	tabIndex = NVI_GetTabIndexByDocumentID(mLastNormalSubTextDocumentID);
				if( tabIndex == kIndex_Invalid )
				{
					tabIndex = NVI_GetCurrentTabIndex();
				}
				//通常サブペイン表示
				SPI_DisplayTabInSubText(tabIndex,false);
			}
		}
	}
	//Diffボタンテキスト更新
	SPI_UpdateDiffButtonText();
	//Diff表示更新
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
	//
	NVI_RefreshWindow();
}

/**
Diffボタンテキスト更新
*/
void	AWindow_Text::SPI_UpdateDiffButtonText()
{
	if( mDiffDisplayMode == false )
	{
		AText	t;
		t.SetLocalizedText("SubPaneDiffButton");
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(NVI_GetCurrentTabIndex())).NVI_IsDirty() == false )
		{
			//差分数テキストを追加
			t.AddCstring(" (");
			t.AddNumber(GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(NVI_GetCurrentTabIndex())).SPI_GetDiffCount());
			t.AddCstring(")");
		}
		//
		NVI_SetControlText(kSubPaneDiffButtonControlID,t);
	}
	else
	{
		AText	t;
		GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(NVI_GetCurrentTabIndex())).
				SPI_GetDiffStatusText(t);
		NVI_SetControlText(kSubPaneDiffButtonControlID,t);
	}
	//比較ドキュメント有無でボタンのEnable制御
	if( NVI_GetCurrentDocumentID() != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetCurrentDocumentID()).SPI_GetDiffTargetDocumentID() == kObjectID_Invalid )
		{
			NVI_SetControlEnable(kSubPaneDiffButtonControlID,false);
		}
		else
		{
			NVI_SetControlEnable(kSubPaneDiffButtonControlID,true);
		}
	}
}

#pragma mark ===========================

#pragma mark ---フォーカス制御

//#137
/**
現在のFocusのTextViewを取得
（FocusがTextViewではないときはInvalidを返す）
@note 優先順位：現在のフォーカスtext view→最後のフォーカスtext view
*/
AControlID	AWindow_Text::GetTopMostFocusTextViewControlID() const
{
	AControlID	focus = NVI_GetCurrentFocus();
	if( IsTextViewControlID(focus) == true )
	{
		return focus;
	}
	else
	{
		if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return kControlID_Invalid;//#379
		//AControlID	lastFocusedTextView = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetLastFocusedTextViewControlID();
		if( IsTextViewControlID(mLatentTextViewContolID) == true )//#212 現在も有効かどうかを判定
		{
			return mLatentTextViewContolID;
		}
		else //#212
		{
			return kControlID_Invalid;
		}
	}
}

//#137
/**
指定TabIndexの現在のFocusのTextViewを取得
（FocusがTextViewではないときはInvalidを返す）
@note サブテキストペインを含めて、現在のfocusのtext viewを取得。text view以外がfocusならinvalidを返す。
*/
AControlID	AWindow_Text::GetTopMostFocusTextViewControlID( const AIndex inTabIndex ) const
{
	AControlID	focus = NVI_GetFocusInTab(inTabIndex);
	if( IsTextViewControlID(focus) == true )
	{
		return focus;
	}
	else
	{
		AControlID	lastFocusedTextView = mTabDataArray.GetObjectConst(inTabIndex).GetLatentTextViewControlID();
		if( IsTextViewControlID(lastFocusedTextView) == true )//#212 現在も有効かどうかを判定
		{
			return lastFocusedTextView;
		}
		else //#212
		{
			return kControlID_Invalid;
		}
	}
}

//#137
/**
*/
ABool	AWindow_Text::IsTextViewControlID( const AControlID inControlID ) const
{
	if( NVI_ExistView(inControlID) == false )   return false;//#212 Viewが実際に存在しているかどうかのチェック
	if( inControlID == kControlID_Invalid )   return false;//#212
	
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
	{
		if( mTabDataArray.GetObjectConst(tabIndex).FindFromTextViewControlID(inControlID) != kIndex_Invalid )
		{
			return true;
		}
		//#212 サブペインのTextViewと比較
		if( mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID() == inControlID )
		{
			return true;
		}
	}
	return false;
}

//#137
/**
フォーカス変更時にコールされる

検索対象TextViewを記憶するために使用する
*/
void	AWindow_Text::EVTDO_DoViewFocusActivated( const AControlID inFocusControlID )
{
	//#688 現在タブ設定済みチェック
	if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;
	//
	if( IsTextViewControlID(inFocusControlID) == true )
	{
		//ドキュメント、タブ取得
		ADocumentID	docID = GetTextViewByControlID(inFocusControlID).SPI_GetTextDocument().GetObjectID();
		AIndex	tabIndex = NVI_GetTabIndexByDocumentID(docID);
		//タブごとのメインテキストの最後のフォーカスとして記憶
		if( SPI_IsSubTextView(inFocusControlID) == false )
		{
			mTabDataArray.GetObject(tabIndex).SetLatentMainTextViewControlID(inFocusControlID);
		}
		//タブごとの最後のフォーカスとして記憶
		mTabDataArray.GetObject(tabIndex).SetLatentTextViewControlID(inFocusControlID);
		//このウインドウの最後のフォーカスとして記憶
		mLatentTextViewContolID = inFocusControlID;
	}
}

#pragma mark ===========================

#pragma mark ---コントロール制御

/**
行番号ボタンの更新
*/
void	AWindow_Text::SPI_UpdateLineNumberButton( const AControlID inTextViewControlID )
{
	//ステータスバー非表示なら何もせず終了
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
	{
		return;
	}
	
	//指定control IDがテキストビューでなければ何もせず終了
	if( IsTextViewControlID(inTextViewControlID) == false )
	{
		return;
	}
	
	if( SPI_IsSubTextView(inTextViewControlID) == false )
	{
		//==================メインテキストの場合==================
		ATextPoint	spt, ept;
		GetTextViewByControlID(inTextViewControlID).SPI_GetSelect(spt,ept);
		AText	text;
		if( GetApp().SPI_GetModePrefDB(SPI_GetTextDocument(inTextViewControlID).SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber_AsParagraph/*#844kDisplayLineNumberButton_AsParagraph*/) == true )
		{
			//text.SetLocalizedText("LineNumberButton_Paragraph");
			AText	numbertext;
			numbertext.SetFormattedCstring("%d/%d",
						SPI_GetTextDocument(inTextViewControlID).SPI_GetParagraphIndexByLineIndex(spt.y)+1,SPI_GetTextDocument(inTextViewControlID).SPI_GetParagraphCount());
			text.AddText(numbertext);
		}
		else
		{
			//text.SetLocalizedText("LineNumberButton_Line");
			AText	numbertext;
			numbertext.SetFormattedCstring("%d/%d",spt.y+1,SPI_GetTextDocument(inTextViewControlID).SPI_GetLineCount());
			text.AddText(numbertext);
		}
		//
		if( mCacheLineNumberButtonText.Compare(text) == false )
		{
			NVI_GetButtonViewByControlID(kControlID_LineNumberButton).NVI_SetText(text);
			mCacheLineNumberButtonText.SetText(text);
		}
	}
	//#725
	else if( mSubTextColumnDisplayed == true )
	{
		//==================サブテキストの場合==================
		//
		ATextPoint	spt, ept;
		GetTextViewByControlID(inTextViewControlID).SPI_GetSelect(spt,ept);
		AText	text;
		if( GetApp().SPI_GetModePrefDB(SPI_GetTextDocument(inTextViewControlID).SPI_GetModeIndex()).
					GetModeData_Bool(AModePrefDB::kDisplayEachLineNumber_AsParagraph) == true )
		{
			//text.SetLocalizedText("LineNumberButton_Paragraph");
			AText	numbertext;
			numbertext.SetFormattedCstring("%d/%d",
						SPI_GetTextDocument(inTextViewControlID).SPI_GetParagraphIndexByLineIndex(spt.y)+1,
						SPI_GetTextDocument(inTextViewControlID).SPI_GetParagraphCount());
			text.AddText(numbertext);
		}
		else
		{
			//text.SetLocalizedText("LineNumberButton_Line");
			AText	numbertext;
			numbertext.SetFormattedCstring("%d/%d",spt.y+1,SPI_GetTextDocument(inTextViewControlID).SPI_GetLineCount());
			text.AddText(numbertext);
		}
		//
		if( mCacheLineNumberButtonText_SubText.Compare(text) == false )
		{
			NVI_GetButtonViewByControlID(kControlID_LineNumberButton_SubText).NVI_SetText(text);
			mCacheLineNumberButtonText_SubText.SetText(text);
		}
	}
}

//場所ボタンの更新
void	AWindow_Text::UpdatePositionButton( const AControlID inTextViewControlID )
{
	//ステータスバー非表示なら何もせず終了
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayStatusBar) == false )
	{
		return;
	}
	
	//指定control IDがテキストビューでなければ何もせず終了
	if( IsTextViewControlID(inTextViewControlID) == false )
	{
		return;
	}
	
	//表示テキストを変更したかどうかのフラグ
	ABool	textchanged = false;
	//
	if( SPI_IsSubTextView(inTextViewControlID) == false )
	{
		//==================メインテキストの場合==================
		//
		ATextPoint	spt, ept;
		GetTextViewByControlID(inTextViewControlID).SPI_GetSelect(spt,ept);
		AIndex	menuItemIndex = SPI_GetTextDocument(inTextViewControlID).SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
		AText	text;
		if( menuItemIndex != kIndex_Invalid )
		{
			SPI_GetTextDocument(inTextViewControlID).SPI_GetJumpMenuTextByMenuItemIndex(menuItemIndex,text);
		}
		/*
		AText	t;
		t.SetLocalizedText("PositionButtonTitle");
		text.InsertText(0,t);
		*/
		//
		if( mCachePositionButtonText.Compare(text) == false )
		{
			NVI_GetButtonViewByControlID(kControlID_PositionButton).NVI_SetText(text);
			mCachePositionButtonText.SetText(text);
			//
			textchanged = true;
		}
	}
	//#725
	else if( mSubTextColumnDisplayed == true )
	{
		//==================サブテキストの場合==================
		//
		ATextPoint	spt, ept;
		GetTextViewByControlID(inTextViewControlID).SPI_GetSelect(spt,ept);
		AIndex	menuItemIndex = SPI_GetTextDocument(inTextViewControlID).SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
		AText	text;
		if( menuItemIndex != kIndex_Invalid )
		{
			SPI_GetTextDocument(inTextViewControlID).SPI_GetJumpMenuTextByMenuItemIndex(menuItemIndex,text);
		}
		/*
		AText	t;
		t.SetLocalizedText("PositionButtonTitle");
		text.InsertText(0,t);
		*/
		//
		if( mCachePositionButtonText_SubText.Compare(text) == false )
		{
			NVI_GetButtonViewByControlID(kControlID_PositionButton_SubText).NVI_SetText(text);
			mCachePositionButtonText_SubText.SetText(text);
			//
			textchanged = true;
		}
	}
	//行番号・場所ボタンレイアウト更新
	if( textchanged == true && NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		ATextWindowLayoutData	layout = {0};
		CalculateLayout(NVI_GetCurrentTabIndex(),layout);
		UpdateLayout_LineNumberAndPositionButton(NVI_GetCurrentTabIndex(),layout);
	}
}

#pragma mark ===========================

#pragma mark ---ウインドウタイトル設定／取得

//
void	AWindow_Text::NVIDO_UpdateTitleBar()
{
	if( NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;//#379
	
	//タイトルテキスト更新
	SPI_UpdateWindowTitleBarText();
	
	//ドキュメントID取得
	ADocumentID	docID = NVI_GetCurrentDocumentID();
	if( docID == kObjectID_Invalid )   return;
		
	//ファイル設定
	//ファイル指定され、かつ、スクリーニング完了後の場合のみ、ウインドウファイル設定する
	if( AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsFileSpecifiedAndScreened() == true )
	{
		AFileAcc	file;
		AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_GetFile(file);
		GetImp().SetWindowFile(file);
	}
	else
	{
		GetImp().ClearWindowFile();
	}
	NVI_SetModified(AApplication::GetApplication().NVI_GetDocumentByID(docID).NVI_IsDirty());
	
	//タイトルバーをUpdateするタイミングでタブセレクタもUpdateする。
	NVI_DrawControl(kControlID_TabSelector);
}

/**
*/
void	AWindow_Text::SPI_UpdateWindowTitleBarText()
{
	//タイトル取得
	AText	title;
	//#688 タイトルはGetTitleText()で取得 NVI_GetTitle(NVI_GetCurrentTabIndex(),title);
	
	//ドキュメントID取得
	ADocumentID	docID = NVI_GetCurrentDocumentID();
	if( docID == kObjectID_Invalid )   return;
	
	//タイトル取得
	SPI_GetTitleText(docID,title);
	
	//タイトル設定
	GetImp().SetTitle(title);
	
	//サブテキストファイル名ボタンテキスト更新
	UpdateSubTextFileNameButtonText();
}

/**
タイトル取得
*/
void	AWindow_Text::SPI_GetTitleText( const ADocumentID inDocumentID, AText& outText )
{
	//
	ADocument_Text&	textdoc = GetApp().SPI_GetTextDocumentByID(inDocumentID);
	//
#if IMPLEMENTATION_FOR_MACOSX
	if( textdoc.SPI_IsODBMode() == true )
	{
		//ODBモードの場合、ODBパスを取得
		AText	odbcustompath;
		textdoc.SPI_GetODBCustomPath(odbcustompath);
		outText.AddText(odbcustompath);
	}
	else
#endif
	{
		//ファイルspecifiedならファイルパスを取得
		if( textdoc.NVI_IsFileSpecified() == true )
		{
			AFileAcc	file;
			textdoc.NVI_GetFile(file);
			AText	filepath;
			if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayFullPathTitleBar) == true )//#1182
			{
				GetApp().GetAppPref().GetFilePathForDisplay(file,filepath);//B0389
			}
			//#1182
			else
			{
				textdoc.NVI_GetTitle(filepath);
			}
			outText.AddText(filepath);
			
		}
		//リモートファイルの場合はURL #1247
		else if( textdoc.SPI_IsRemoteFileMode() == true )
		{
			textdoc.SPI_GetURL(outText);
		}
		//ファイルnot specifiedならデフォルトタイトルを取得
		else
		{
			AText	title;
			textdoc.NVI_GetTitle(title);
			outText.AddText(title);
		}
	}
	
	if( mToolbarExistArray.Find(kControlID_Toolbar_Mode) == kIndex_Invalid )
	{
		//"-"を追加
		outText.AddCstring(" - ");
		//モード名を取得
		AText	modename;
		textdoc.SPI_GetModeDisplayName(modename);
		outText.AddText(modename);
	}
	outText.AddCstring("  ");
	
	//ファイル指定され、かつ、スクリーニング完了後の場合のみ、ファイル情報取得、表示する
	if( textdoc.NVI_IsFileSpecifiedAndScreened() == true )
	{
		//ドキュメントがread onlyかつ、行折り返しスレッド計算中によるread onlyでない場合、"書き込み不可"テキスト追加
		if( textdoc.NVI_IsReadOnly() == true &&
		   textdoc.SPI_GetWrapCalculatorProgressPercent() == kIndex_Invalid )//#699
		{
			AText	t;
			t.SetLocalizedText("ReadOnly");
			outText.AddText(t);
		}
		
		//最終変更日時
		//"("
		outText.AddCstring(" (");
		//最終変更日時
		AText	datetime;
		ADateTimeWrapper::GetDateTimeText(textdoc.SPI_GetLastModifiedDateTime(),datetime);
		outText.AddText(datetime);
		//")"
		outText.AddCstring(") ");
		
		//SCM状態
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
		{
			AFileAcc	file;
			textdoc.NVI_GetFile(file);
			AText	t;
			GetApp().SPI_GetSCMFolderManager().GetFileStateText(file,t);
			if( t.GetItemCount() > 0 )
			{
				outText.AddCstring("  ");
				outText.AddText(t);
			}
		}
	}
	
	//行折り返し進捗取得
	ANumber	wrapCalcPercent = textdoc.SPI_GetWrapCalculatorProgressPercent();
	if( wrapCalcPercent != kIndex_Invalid )//行折り返しスレッド計算中はkIndex_Invalid以外になる
	{
		if( wrapCalcPercent != 0 )//ドキュメントを開いた直後に、「折り返し計算中」「書き込み禁止」どちらも表示させないため
		{
			AText	t;
			t.SetNumber(wrapCalcPercent);
			AText	wraptext;
			wraptext.SetLocalizedText("TitleText_WrapCalculating");
			wraptext.ReplaceParamText('0',t);
			//タイトルテキストに追加
			outText.InsertText(0,wraptext);
		}
	}
	
	//段落数
	if( textdoc.SPI_IsWrapCalculating() == false )
	{
		AText	paraText;
		paraText.SetLocalizedText("TitleText_Paragraph");
		paraText.ReplaceParamText('0',textdoc.SPI_GetParagraphCount());
		outText.AddText(paraText);
	}
}

#pragma mark ===========================

#pragma mark ---ジャンプリスト

/**
ジャンプリスト表示・非表示
inShowがfalseであれば、フローティングジャンプリストは削除する。
表示状態＝フローティングジャンプリストウインドウ生成済み状態
非表示状態＝フローティングジャンプリストウインドウ未生成状態
*/
void	AWindow_Text::SPI_ShowHideJumpListWindow( const ABool inShow )
{
	//
	if( inShow == false )
	{
		//==================表示する場合：フローティングジャンプリストを削除する==================
		if( mFloatingJumpListWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_DeleteSubWindow(mFloatingJumpListWindowID);
			mFloatingJumpListWindowID = kObjectID_Invalid;
		}
	}
	else
	{
		//==================非表示の場合：フローティングジャンプリストが未生成であれば、ウインドウ生成する。==================
		if( mFloatingJumpListWindowID == kObjectID_Invalid )
		{
			mFloatingJumpListWindowID = GetApp().SPI_CreateSubWindow(kSubWindowType_JumpList,0,GetObjectID(),
																	 kSubWindowLocationType_Floating,kIndex_Invalid,false);
		}
	}
	
	//ジャンプリスト表示・配置更新
	UpdateFloatingJumpListDisplay();
	
	//ジャンプリストのタブ選択 #1109
	GetApp().SPI_SelectJumpListWindowsTab(GetObjectID(),NVI_GetCurrentDocumentID());
	
	//表示・非表示の状態をapp pref dbに保存
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kShowFloatingJumpList,inShow);
	
	/*#725
	//
	mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).SetJumpListVisible(
				GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_IsWindowVisible());
	*/
	//
	/*#725
	//Overlayなら何もしない
	if( SPI_GetJumpListWindow().NVI_GetOverlayMode() == true )   return;
	
	//
	if( SPI_GetJumpListWindow().NVI_IsWindowVisible() == true )
	{
		SPI_GetJumpListWindow().NVI_Hide();
	}
	else
	{
		SPI_GetJumpListWindow().NVI_Show();
		SPI_GetJumpListWindow().NVI_SelectTab(NVI_GetCurrentTabIndex());
	}
	//
	mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).SetJumpListVisible(SPI_GetJumpListWindow().NVI_IsWindowVisible());
	*/
}

/**
ジャンプリスト表示・配置更新
ジャンプリスト生成済みであれば表示し、正しいオフセット位置に表示する。
*/
void	AWindow_Text::UpdateFloatingJumpListDisplay()
{
	if( mFloatingJumpListWindowID != kObjectID_Invalid )
	{
		//このフローティングジャンプリスト以外のジャンプリストを非表示にする
		GetApp().SPI_HideOtherFloatingJumpListWindows(mFloatingJumpListWindowID);
		//ジャンプリスト表示
		if( GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_IsWindowVisible() == false )
		{
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_Show(false);
		}
		//ジャンプリスト位置更新
		GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).SPI_UpdatePosition();
		/*
		//ジャンプリストタブ選択
		if( NVI_GetCurrentDocumentID() != kObjectID_Invalid )
		{
			ADocumentID	docID = SPI_GetCurrentFocusTextDocument().GetObjectID();
			GetApp().SPI_GetJumpListWindow(mFloatingJumpListWindowID).NVI_SelectTabByDocumentID(docID);
		}
		*/
	}
}

//
void	AWindow_Text::SPI_DoJumpListSelected( const ADocumentID inDocumentID,
											 const AIndex inIndex, const AModifierKeys inModifierKeys )
{
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true )
	{
		SPI_SplitView(false);
	}
	
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_DoJumpListSelected(inIndex);
	}
}

/*#725
//
void	AWindow_Text::SPI_UpdatePropertyJumpList()
{
	//
	SPI_GetJumpListWindow().NVI_UpdateProperty();
}
*/

#pragma mark ===========================

#pragma mark ---ツールバー

//マクロバー項目サイズ
const ANumber	kWidth_AddMacroButton = 30;
const ANumber	kHeight_AddMacroButton = 20;
const ANumber	kMarginWidth_AddMacroButton = 3;

//
void	AWindow_Text::SPI_UpdateToolbarItemValue( const AObjectID inDocumentID )
{
	if( inDocumentID == NVI_GetCurrentDocumentID() )
	{
		UpdateToolbarItemValue(NVI_GetCurrentTabIndex());
	}
}

/**
ツールバーの値を更新
*/
void	AWindow_Text::UpdateToolbarItemValue( const AIndex inTabIndex )
{
	ADocumentID	docID = NVI_GetDocumentIDByTabIndex(inTabIndex);
	//テキストエンコーディングメニュー更新
	if( mToolbarExistArray.Find(kControlID_Toolbar_TextEncoding) != kIndex_Invalid )
	{
		AText	tecname;
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextEncoding(tecname);
		NVI_SetControlText(kControlID_Toolbar_TextEncoding,tecname);
	}
	//改行コードメニュー更新
	if( mToolbarExistArray.Find(kControlID_Toolbar_ReturnCode) != kIndex_Invalid )
	{
		NVI_SetControlNumber(kControlID_Toolbar_ReturnCode,GetApp().SPI_GetTextDocumentByID(docID).SPI_GetReturnCode());
	}
	//行折り返しメニュー更新
	if( mToolbarExistArray.Find(kControlID_Toolbar_WrapMode) != kIndex_Invalid )
	{
		ANumber	wrapMode = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetWrapMode();
		NVI_SetControlNumber(kControlID_Toolbar_WrapMode,wrapMode);//#1113
		/*
		switch(wrapMode)
		{
		  case 0:
		  case 1:
			{
				NVI_SetControlNumber(kControlID_Toolbar_WrapMode,wrapMode);
				break;
			}
		  case 2:
			{
				NVI_SetControlNumber(kControlID_Toolbar_WrapMode,wrapMode);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		*/
	}
	//モードメニュー更新
	if( mToolbarExistArray.Find(kControlID_Toolbar_Mode) != kIndex_Invalid )
	{
		AText	modename;
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeRawName(modename);
		NVI_SetControlText(kControlID_Toolbar_Mode,modename);
	}
	//設定
	if( mToolbarExistArray.Find(kControlID_Toolbar_Pref) != kIndex_Invalid )
	{
		AText	modename;
		GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeRawName(modename);
		NVI_SetControlText(kControlID_Toolbar_Pref,modename);
	}
	//ポップアップ禁止トグル状態
	if( mToolbarExistArray.Find(kControlID_Toolbar_ProhibitPopup) != kIndex_Invalid )
	{
		NVI_SetControlBool(kControlID_Toolbar_ProhibitPopup,
						   GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kProhibitPopup));
	}
	//フローティング表示とグル状態
	if( mToolbarExistArray.Find(kControlID_Toolbar_ShowHideFloatingWindows) != kIndex_Invalid )
	{
		NVI_SetControlBool(kControlID_Toolbar_ShowHideFloatingWindows,
						   !GetApp().SPI_GetProhibitFloatingSubWindow());
	}
	
	/*#724
	//ツールボタン値更新
	AItemCount	toolButtonCount = mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonCount();
	for( AIndex i = 0; i < toolButtonCount; i++ )
	{
		AToolbarItemType	type = mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonType(i);
		switch(type)
		{
		  case kToolbarItemType_TextEncoding:
			{
				AText	tecname;
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetTextEncoding(tecname);
				//#232 NVI_SetControlText(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),tecname);
				NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).NVI_SetText(tecname);
				break;
			}
		  case kToolbarItemType_ReturnCode:
			{
				NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).
						SPI_SetTextFromMenuIndex(GetApp().SPI_GetTextDocumentByID(docID).SPI_GetReturnCode());
				break;
			}
		  case kToolbarItemType_WrapMode:
			{
				ANumber	wrapMode = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetWrapMode();
				//#232 NVI_SetControlNumber(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),wrapMode);
				NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).SPI_SetTextFromMenuIndex(wrapMode);
				if( wrapMode == 1 )
				{
					AText	text;
					text.SetLocalizedText("BuiltinToolButtonTitle_WordWrap");
					//#232 NVI_SetControlText(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),text,true);
					NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).NVI_SetText(text);
				}
				else if( wrapMode == 2 )
				{
					AText	text;
					text.SetLocalizedText("BuiltinToolButtonTitle_WordWrapWithLetterCount");
					AText	numtext;
					numtext.SetFormattedCstring("%d",GetApp().SPI_GetTextDocumentByID(docID).SPI_GetWrapLetterCount());
					text.ReplaceParamText('0',numtext);
					//#232 NVI_SetControlText(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),text,true);
					NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).NVI_SetText(text);
				}
				break;
			}
		  case kToolbarItemType_Mode:
			{
				AText	modename;
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetModeName(modename);
				//#232 NVI_SetControlText(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i),modename);
				NVI_GetButtonViewByControlID(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i)).NVI_SetText(modename);//#232
				break;
			}
		}
	}
	*/
}

//#688
/**
ツールバー項目追加／削除時処理（削除時はinID=kControlID_Invalid)
*/
void	AWindow_Text::EVTDO_ToolbarItemAdded( const AControlID inID )
{
	switch(inID)
	{
	  case kControlID_Toolbar_TextEncoding:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_TextEncoding,kTextArrayMenuID_TextEncoding);
			break;
		}
	  case kControlID_Toolbar_ReturnCode:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_ReturnCode,kTextArrayMenuID_ReturnCode);
			break;
		}
	  case kControlID_Toolbar_WrapMode:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_WrapMode,kTextArrayMenuID_WrapMode);
			break;
		}
	  case kControlID_Toolbar_Mode:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_Mode,kTextArrayMenuID_EnabledMode);
			break;
		}
	  case kControlID_Toolbar_SubWindows:
		{
			NVI_SetControlMenu(kControlID_Toolbar_SubWindows,GetApp().NVI_GetMenuManager().GetContextMenu(kContextMenuID_SubWindows));
			break;
		}
	  case kControlID_Toolbar_NewDocument:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_NewDocument,kTextArrayMenuID_EnabledMode);
			break;
		}
	  case kControlID_Toolbar_Pref:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_Pref,kTextArrayMenuID_Pref);
			break;
		}
		/* #0 未使用のためコメントアウト
	  case kControlID_Toolbar_Diff:
		{
			NVI_SetControlMenu(kControlID_Toolbar_Diff,GetApp().NVI_GetMenuManager().GetContextMenu(kContextMenuID_Diff));
			break;
		}
		*/
	  case kControlID_Toolbar_FindHighlight:
		{
			NVI_RegisterTextArrayMenu(kControlID_Toolbar_FindHighlight,kTextArrayMenuID_TextMarker);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//toolbar項目存在テーブル更新
	UpdateToolbarExistArray();
	//ツールバー値更新
	if( NVI_GetCurrentTabIndex() != kIndex_Invalid )
	{
		UpdateToolbarItemValue(NVI_GetCurrentTabIndex());
	}
}

/**
toolbar項目存在テーブル更新
*/
void	AWindow_Text::UpdateToolbarExistArray()
{
	//ツールバー存在するかどうかをメンバ変数に記憶（NVI_ExistControl()は時間がかかるので）
	mToolbarExistArray.DeleteAll();
	if( NVI_ExistControl(kControlID_Toolbar_TextEncoding) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_TextEncoding);
	}
	if( NVI_ExistControl(kControlID_Toolbar_ReturnCode) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_ReturnCode);
	}
	if( NVI_ExistControl(kControlID_Toolbar_WrapMode) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_WrapMode);
	}
	if( NVI_ExistControl(kControlID_Toolbar_Mode) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_Mode);
	}
	/*#0 未使用のためコメントアウト
	if( NVI_ExistControl(kControlID_Toolbar_Diff) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_Diff);
	}
	*/
	if( NVI_ExistControl(kControlID_Toolbar_SearchField) == true )
	{
		mToolbarExistArray.Add(kControlID_Toolbar_SearchField);
	}
	for( AControlID controlID = kControlID_Toolbar_ButtonStart; controlID <= kControlID_Toolbar_ButtonEnd; controlID++ )
	{
		if( NVI_ExistControl(controlID) == true )
		{
			mToolbarExistArray.Add(controlID);
		}
	}
}

//
void	AWindow_Text::SPI_RemakeToolButtonsAll( const AModeIndex inModeIndex )
{
	//B0312 ツールボタン全削除
	//ツール再構成時、ツールボタンのアイコンを全て登録解除してからここに来る。CreateToolButtons()をタブごとに実行すると、
	//中でSetIcon()を実行するのだが、そのなかでDrawControl()を実行したときに、（おそらく）重なっている別のタブのアイコンのredrawをしようとしてしまう。
	//このとき、まだCreateToolButtons()を未実行のタブのツールボタンのアイコンはすでに登録解除なので、強制終了が発生していた。
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			for( AIndex i = 0; i < mTabDataArray.GetObjectConst(tabIndex).GetToolButtonCount(); i++ )
			{
				NVM_UnregisterViewInTab(tabIndex,mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(i));
				NVI_DeleteControl(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(i));
			}
			mTabDataArray.GetObject(tabIndex).DeleteAllToolButtonData();
		}
	}
	//
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		if( GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetModeIndex() == inModeIndex || inModeIndex == kIndex_Invalid )
		{
			CreateToolButtons(tabIndex);
			//#275
			UpdateViewBounds(tabIndex);
		}
	}
}

//ツールボタン生成
void	AWindow_Text::CreateToolButtons( const AIndex inTabIndex )
{
	//ツールボタン全削除
	for( AIndex i = 0; i < mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonCount(); i++ )
	{
		NVM_UnregisterViewInTab(inTabIndex,mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i));
		NVI_DeleteControl(mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(i));
	}
	mTabDataArray.GetObject(inTabIndex).DeleteAllToolButtonData();
	//検索ボックス・ボタン削除 #243
	/*#724
	if( mTabDataArray.GetObject(inTabIndex).GetSearchButtonControlID() != kControlID_Invalid )
	{
		NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSearchButtonControlID());
		mTabDataArray.GetObject(inTabIndex).SetSearchButtonControlID(kControlID_Invalid);
	}
	if( mTabDataArray.GetObject(inTabIndex).GetSearchBoxControlID() != kControlID_Invalid )
	{
		NVI_DeleteControl(mTabDataArray.GetObject(inTabIndex).GetSearchBoxControlID());
		mTabDataArray.GetObject(inTabIndex).SetSearchBoxControlID(kControlID_Invalid);
	}
	*/
	//
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//#232 ボタンの位置を記憶（ToolBarでのDrag&Drop用）
	mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().DeleteAll();
	//
	ANumber	toolButtonX = 4;
	AWindowPoint	pt;
	AControlID	toolbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolbarControlID();//#246 
	NVI_GetControlPosition(/*#246 kControlID_ToolbarPlacard*/toolbarControlID,pt);
	ANumber	y = pt.y;
	//「マクロ追加」ボタン
	{
		AControlID	toolButtonControlID = NVM_AssignDynamicControlID();
		AWindowPoint	pt = {0,0};
		AFileAcc	file;
		mTabDataArray.GetObject(inTabIndex).AddToolButton(toolButtonControlID,kToolbarItemType_AddToolButton,file,kIndex_Invalid);
		NVI_CreateButtonView(toolButtonControlID,pt,kWidth_AddMacroButton,kHeight_MacroBarItem,toolbarControlID);
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	title;
		title.SetLocalizedText("AddMacro");
		NVI_GetButtonViewByControlID(toolButtonControlID).NVI_SetText(title);
		NVM_RegisterViewInTab(inTabIndex,toolButtonControlID);
	}
	//「マクロ：」ボタン
	{
		AControlID	toolButtonControlID = NVM_AssignDynamicControlID();
		AWindowPoint	pt = {0,0};
		AFileAcc	file;
		mTabDataArray.GetObject(inTabIndex).AddToolButton(toolButtonControlID,kToolbarItemType_MacroTitle,file,kIndex_Invalid);
		NVI_CreateButtonView(toolButtonControlID,pt,0,kHeight_MacroBarItem,toolbarControlID);
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetMenu(kContextMenuID_ToolBar,kMenuItemID_Invalid);
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		AText	title;
		title.SetLocalizedText("MacroBarTitle");
		NVI_GetButtonViewByControlID(toolButtonControlID).NVI_SetText(title);
		NVM_RegisterViewInTab(inTabIndex,toolButtonControlID);
	}
	
	//========================各マクロバー項目毎のループ========================
	AItemCount	toolButtonCount = GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItemCount();
	for( AIndex toolButtonDBIndex = 0; toolButtonDBIndex < toolButtonCount; toolButtonDBIndex++ )
	{
		//------------------ツールバーデータの取得------------------
		AToolbarItemType	type = kToolbarItemType_File;
		AFileAcc	file;
		AText	name;
		ABool	enabled = false;
		GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItem(toolButtonDBIndex,type,file,name,enabled);
		//enabledでない、または、ファイル／フォルダ項目以外の場合は、何もしない
		if( enabled == false || (type != kToolbarItemType_File && type != kToolbarItemType_Folder) )
		{
			continue;
		}
		//
		AControlID	toolButtonControlID = NVM_AssignDynamicControlID();
		//ボタンデータ追加
		mTabDataArray.GetObject(inTabIndex).AddToolButton(toolButtonControlID,type,file,toolButtonDBIndex);
		//ボタン生成
		AWindowPoint	pt = {0};
		NVI_CreateButtonView(toolButtonControlID,pt,0,kHeight_MacroBarItem,toolbarControlID);
		//#724 マクロバー化
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetButtonViewType(kButtonViewType_TextWithOvalHover);
		//
		NVM_RegisterViewInTab(inTabIndex,toolButtonControlID);//#135 上から移動
		NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetEnableDrag(true);//#232
		NVI_SetShowControl(toolButtonControlID,false);
	}
	//マクロボタン各項目テキスト設定
	UpdateMacroBarItemContent(inTabIndex);
}

//#724
/**
マクロボタン各項目テキスト設定
*/
void	AWindow_Text::UpdateMacroBarItemContent( const AIndex inTabIndex )
{
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//
	AIndex	toolDisplayIndex = 2;
	AItemCount	toolButtonCount = GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItemCount();
	for( AIndex toolButtonDBIndex = 0; toolButtonDBIndex < toolButtonCount; toolButtonDBIndex++ )
	{
		//------------------ツールバーデータの取得------------------
		AToolbarItemType	type = kToolbarItemType_File;
		AFileAcc	file;
		AText	name;
		ABool	enabled = false;
		GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItem(toolButtonDBIndex,type,file,name,enabled);
		//enabledでない、または、ファイル／フォルダ項目以外の場合は、何もしない
		if( enabled == false || (type != kToolbarItemType_File && type != kToolbarItemType_Folder) )
		{
			continue;
		}
		//マクロバーのcontrol ID取得
		AControlID	toolButtonControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolButtonControlID(toolDisplayIndex);
		toolDisplayIndex++;
		switch( type )
		{
		  case kToolbarItemType_File:
			{
				//右クリックメニュー設定
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetContextMenu(kContextMenuID_ToolButton);
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetContextMenuEnableItem(0,true);
				//タイトル取得
				AText	title;
				file.GetFilename(title);
				//ショートカットキーテキスト追加
				if( false )//★時間がかかるのでとりあえずショートカットキー表示はしない
				{
					if( toolButtonDBIndex >= 0 && toolButtonDBIndex < 9 )
					{
						AKeyBindAction	action = keyAction_macro1 + toolButtonDBIndex;
						AText	keyText;
						if( GetApp().SPI_GetModePrefDB(modeIndex).GetKeyTextFromKeybindAction(action,keyText) == true )
						{
							title.AddCstring(" (");
							title.AddText(keyText);
							title.AddCstring(")");
						}
					}
				}
				//タイトル設定
				NVI_GetButtonViewByControlID(toolButtonControlID).NVI_SetText(title);
				break;
			}
		  case kToolbarItemType_Folder:
			{
				//メニューtextとactiontextを生成して、Buttonに設定
				const ATextArray&	filenamearray = GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItemFilenameArray(toolButtonDBIndex);
				ATextArray	actiontextarray;
				for( AIndex j = 0; j < filenamearray.GetItemCount(); j++ )
				{
					AFileAcc	f;
					f.SpecifyChild(file,filenamearray.GetTextConst(j));
					AText	path;
					f.GetPath(path);
					actiontextarray.Add(path);
				}
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetMenuTextArray(filenamearray,actiontextarray,kMenuItemID_ToolBar_Folder);
				//右クリックメニュー
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetContextMenu(kContextMenuID_ToolButton);
				NVI_GetButtonViewByControlID(toolButtonControlID).SPI_SetContextMenuEnableItem(0,false);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
}

/**
マクロボタン各項目テキスト設定
*/
void	AWindow_Text::SPI_UpdateMacroBarItemContent()
{
	for( AIndex tabIndex = 0; tabIndex < NVI_GetTabCount(); tabIndex++ )
	{
		UpdateMacroBarItemContent(tabIndex);
	}
}

//#725
/**
マクロバー高さ計算
*/
ANumber	AWindow_Text::CalculateMacroBarHeight( const AIndex inTabIndex,
		const AWindowPoint pt_MacroBar, const ANumber w_MacroBar ) 
{
	//各マクロバーの位置を計算してtabデータに格納
	UpdateMacroBarItemPositions(inTabIndex,w_MacroBar);
	//
	ANumber	y = kMacroBar_TopMargin;
	AItemCount	itemCount = mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().GetItemCount();
	//最大のy位置を取得する
	for( AIndex i = 0; i < itemCount; i++ )
	{
		ANumber	cy = mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().Get(i).y;
		if( cy > y )   y = cy;
	}
	return y+kHeight_MacroBarItem+kMacroBar_BottomMargin;
}

//#725
/**
マクロバーの配置
*/
void	AWindow_Text::UpdateLayout_MacroBar( const AIndex inTabIndex, 
		const AWindowPoint pt_MacroBar, const ANumber w_MacroBar, const ANumber h_MacroBar )
{
	//各マクロバーの位置を計算してtabデータに格納
	UpdateMacroBarItemPositions(inTabIndex,w_MacroBar);
	
	//#675 UpdateViewBounds抑制状態中は何もしない
	if( mSuppressUpdateViewBoundsCounter > 0 )   return;
	//現在タブでなければ何もしない
	if( NVI_GetCurrentTabIndex() != inTabIndex )
	{
		return;
	}
	//モードindex取得
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//マクロバー配置
	AControlID	toolbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolbarControlID();
	if(  GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowMacroBar) == true &&
	   NVI_IsToolBarShown() == true )
	{
		//================マクロバー表示時================
		
		//マクロバー
		NVI_SetControlPosition(toolbarControlID,pt_MacroBar);
		NVI_SetControlSize(toolbarControlID,w_MacroBar,h_MacroBar);
		NVI_SetShowControlIfCurrentTab(inTabIndex,toolbarControlID,true);//#246
		//各マクロ配置
		AItemCount	count = mTabDataArray.GetObject(inTabIndex).GetToolButtonCount();
		for( AIndex i = 0; i < count; i++ )
		{
			AIndex	index = i;
			//dragging中のindexを取得
			if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
			{
				index = mDraggingMacroBarDisplayIndexArray.Get(i);
			}
			//
			ALocalPoint	macroBarItemPoint = mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().Get(i);
			//コントロールID取得
			AControlID	controlID = mTabDataArray.GetObject(inTabIndex).GetToolButtonControlID(index);
			//位置計算
			AWindowPoint	pt = { pt_MacroBar.x + macroBarItemPoint.x,
							pt_MacroBar.y + macroBarItemPoint.y };
			//配置
			NVI_SetControlPosition(controlID,pt);
			//表示
			NVI_SetShowControlIfCurrentTab(inTabIndex,controlID,true);
		}
	}
	else
	{
		//================マクロバー非表示時================
		
		//マクロバー表示（高さ1のマクロバーを表示する。分割線表示のため。（setShowsBaselineSeparatorを使うと、なぜかマウスホバーできなくなるため））
		NVI_SetControlPosition(toolbarControlID,pt_MacroBar);
		NVI_SetControlSize(toolbarControlID,w_MacroBar,h_MacroBar);
		NVI_SetShowControlIfCurrentTab(inTabIndex,toolbarControlID,true);//#246
		//各マクロ非表示
		AItemCount	count = mTabDataArray.GetObject(inTabIndex).GetToolButtonCount();
		for( AIndex i = 0; i < count; i++ )
		{
			AControlID	controlID = mTabDataArray.GetObject(inTabIndex).GetToolButtonControlID(i);
			NVI_SetShowControlIfCurrentTab(inTabIndex,controlID,false);
		}
	}
}

//#724
/**
各マクロバー項目の位置を計算・tabデータに保存
*/
void	AWindow_Text::UpdateMacroBarItemPositions( const AIndex inTabIndex, const ANumber w_MacroBar )
{
	//すでに計算済みなら何もしない
	if( mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().GetItemCount() > 0 )
	{
		return;
	}
	
	//フォント取得
	AText	fontname;
	AFontWrapper::GetDialogDefaultFontName(fontname);
	//モードindex取得
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	//マクロバー各項目ごとのループ
	AControlID	toolbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolbarControlID();
	if(  GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowMacroBar) == true &&
		   NVI_IsToolBarShown() == true )
	{
		//=============「マクロ追加」ボタン=============
		ANumber	macroItemsWidth = w_MacroBar-kWidth_AddMacroButton-kMarginWidth_AddMacroButton*2;
		{
			ALocalPoint	toolButtonPt = {macroItemsWidth+kMarginWidth_AddMacroButton,kMacroBar_TopMargin};
			//
			mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().Add(toolButtonPt);
			//コントロールID取得
			AControlID	controlID = mTabDataArray.GetObject(inTabIndex).GetToolButtonControlID(0);
			//ボタンのプロパティ設定
			AText	fontname;
			AFontWrapper::GetDialogDefaultFontName(fontname);
			NVI_GetButtonViewByControlID(controlID).SPI_SetTextProperty(fontname,10.0,kJustification_Center,
																		0/*#1079 kTextStyle_Bold|kTextStyle_DropShadow*/,
																		kColor_Black,kColor_Gray60Percent);//#1079
			NVI_GetButtonViewByControlID(controlID).SPI_SetDropShadowColor(kColor_Gray80Percent);
		}
		//
		AItemCount	count = mTabDataArray.GetObject(inTabIndex).GetToolButtonCount();
		//=============各マクロボタン配置=============
		ALocalPoint	toolButtonPt = {kMacroBar_LeftMargin,kMacroBar_TopMargin};
		for( AIndex i = 1; i < count; i++ )
		{
			AIndex	index = i;
			//dragging中のindexを取得
			if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
			{
				index = mDraggingMacroBarDisplayIndexArray.Get(i);
			}
			//コントロールID取得
			AControlID	controlID = mTabDataArray.GetObject(inTabIndex).GetToolButtonControlID(index);
			//ボタンのプロパティ設定
			AText	fontname;
			AFontWrapper::GetDialogDefaultFontName(fontname);
			NVI_GetButtonViewByControlID(controlID).SPI_SetTextProperty(fontname,10.0,kJustification_Center,
																		0/*#1079 kTextStyle_Bold|kTextStyle_DropShadow*/,
																		kColor_Black,kColor_Gray60Percent);//#1079
			NVI_GetButtonViewByControlID(controlID).SPI_SetDropShadowColor(kColor_Gray80Percent);
			//dragging中で非表示の場合は、temporaryで非表示にする
			NVI_GetButtonViewByControlID(controlID).SPI_SetTemporaryInvisible(i==mDraggingMacroBarCurrentItemDisplayIndex);
			//表示幅取得
			ANumber	width = NVI_GetControlWidth(controlID);
			//表示pt取得
			if( toolButtonPt.x + width + kMacroBar_RightMargin >= macroItemsWidth )
			{
				toolButtonPt.x = kMacroBar_LeftMargin;
				toolButtonPt.y += kHeight_MacroBarItem + kMarginHeight_MacroBarItem;
			}
			//tabデータに追加
			mTabDataArray.GetObject(inTabIndex).GetToolButtonPositionArray().Add(toolButtonPt);
			//次のX
			toolButtonPt.x += width;
		}
		//#232 現在のモードIndex等をToolBarに設定
		GetToolbarView(inTabIndex).SPI_SetInfo(modeIndex);
	}
}

//#724
/**
マクロバー項目をinPointから検索
*/
AIndex	AWindow_Text::FindMacroBarIndex( const AWindowPoint& inPoint ) const
{
	//★未レビュー
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	//マクロバー個数取得
	AItemCount	count = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonCount();
	//==================drag中==================
	if( mDraggingMacroBarCurrentItemDisplayIndex != kIndex_Invalid )
	{
		//drag中項目のindexを取得
		AIndex	currentIndex = mDraggingMacroBarDisplayIndexArray.Get(mDraggingMacroBarCurrentItemDisplayIndex);
		ANumber	w_current = NVI_GetControlWidth(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(currentIndex));
		//
		AIndex	prevDisplayIndex = mDraggingMacroBarCurrentItemDisplayIndex-1;
		if( prevDisplayIndex >= 0 && prevDisplayIndex < count )
		{
			AIndex	previndex = prevDisplayIndex;
			if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
			{
				previndex = mDraggingMacroBarDisplayIndexArray.Get(prevDisplayIndex);
			}
			//
			AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(previndex);
			//
			AWindowPoint	controlPoint = {0};
			NVI_GetControlPosition(controlID,controlPoint);
			ANumber	w = NVI_GetControlWidth(controlID);
			ANumber	h = NVI_GetControlHeight(controlID);
			if( w_current < w )
			{
				if( inPoint.x >= controlPoint.x + w_current && inPoint.x <= controlPoint.x + w &&
							inPoint.y >= controlPoint.y && inPoint.y <= controlPoint.y + h )
				{
					return mDraggingMacroBarCurrentItemDisplayIndex;
				}
			}
		}
		//
		AIndex	nextDisplayIndex = mDraggingMacroBarCurrentItemDisplayIndex+1;
		if( nextDisplayIndex >= 0 && nextDisplayIndex < count )
		{
			AIndex	nextindex = nextDisplayIndex;
			if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
			{
				nextindex = mDraggingMacroBarDisplayIndexArray.Get(nextDisplayIndex);
			}
			//
			AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(nextindex);
			//
			AWindowPoint	controlPoint = {0};
			NVI_GetControlPosition(controlID,controlPoint);
			ANumber	w = NVI_GetControlWidth(controlID);
			ANumber	h = NVI_GetControlHeight(controlID);
			if( w_current < w )
			{
				if( inPoint.x >= controlPoint.x + w_current && inPoint.x <= controlPoint.x + w &&
							inPoint.y >= controlPoint.y && inPoint.y <= controlPoint.y + h )
				{
					return mDraggingMacroBarCurrentItemDisplayIndex;
				}
			}
		}
	}
	//
	for( AIndex i = kFixMacroItemCount; i < count; i++ )
	{
		//
		AIndex	index = i;
		if( mDraggingMacroBarDisplayIndexArray.GetItemCount() > 0 )
		{
			index = mDraggingMacroBarDisplayIndexArray.Get(i);
		}
		//コントロールID取得
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonControlID(index);
		//
		AWindowPoint	controlPoint = {0};
		NVI_GetControlPosition(controlID,controlPoint);
		ANumber	w = NVI_GetControlWidth(controlID);
		ANumber	h = NVI_GetControlHeight(controlID);
		if( inPoint.x >= controlPoint.x && inPoint.x <= controlPoint.x + w &&
					inPoint.y >= controlPoint.y && inPoint.y <= controlPoint.y + h )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//#724
/**
*/
void	AWindow_Text::SPI_MacroBarItemDragging( const AViewID inButtonViewID, const AWindowPoint& inDropPoint )
{
	//★未レビュー
	
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	AControlID	controlID = GetApp().NVI_GetViewByID(inButtonViewID).NVI_GetControlID();
	AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(controlID);
	if( toolButtonIndex == kIndex_Invalid )   return;
	
	AIndex	newIndex = FindMacroBarIndex(inDropPoint);
	if( newIndex == kIndex_Invalid )
	{
		return;
	}
	//
	if( mDraggingMacroBarCurrentItemDisplayIndex == kIndex_Invalid )
	{
		//Drag最初のコール時
		mDraggingMacroBarCurrentItemDisplayIndex = toolButtonIndex;
		//
		mDraggingMacroBarDisplayIndexArray.DeleteAll();
		AItemCount	count = mTabDataArray.GetObject(tabIndex).GetToolButtonCount();
		for( AIndex i = 0; i < count; i++ )
		{
			mDraggingMacroBarDisplayIndexArray.Add(i);
		}
	}
	else
	{
		//Drag2回目以降のコール時
		
		//
		mDraggingMacroBarDisplayIndexArray.Move(mDraggingMacroBarCurrentItemDisplayIndex,newIndex);
		//
		mDraggingMacroBarCurrentItemDisplayIndex = newIndex;
		//
		mTabDataArray.GetObject(tabIndex).GetToolButtonPositionArray().DeleteAll();
		//
		ATextWindowLayoutData	layout = {0};
		CalculateLayout(tabIndex,layout);
		UpdateMacroBarItemPositions(tabIndex,layout.w_MacroBar);
		UpdateLayout_MacroBar(tabIndex,layout.pt_MacroBar,layout.w_MacroBar,layout.h_MacroBar);
	}
}

//#724
/**
*/
void	AWindow_Text::SPI_MacroBarItemStopDragging()
{
	//★未レビュー
	
	mDraggingMacroBarCurrentItemDisplayIndex = kIndex_Invalid;
	mDraggingMacroBarDisplayIndexArray.DeleteAll();
	//
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	//
	mTabDataArray.GetObject(tabIndex).GetToolButtonPositionArray().DeleteAll();
	//
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(tabIndex,layout);
	UpdateMacroBarItemPositions(tabIndex,layout.w_MacroBar);
	UpdateLayout_MacroBar(tabIndex,layout.pt_MacroBar,layout.w_MacroBar,layout.h_MacroBar);
}

//#232
/**
ツールボタンDropによるボタン順序変更
*/
void	AWindow_Text::SPI_MacroBarItemDrop( const AViewID inButtonViewID, const AWindowPoint& inDropPoint )
{
	//★未レビュー
	
	AIndex	newIndex = FindMacroBarIndex(inDropPoint);
	//
	SPI_MacroBarItemStopDragging();
	//
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	AControlID	controlID = GetApp().NVI_GetViewByID(inButtonViewID).NVI_GetControlID();
	AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(controlID);
	if( toolButtonIndex == kIndex_Invalid )   return;
	
	if( newIndex == kIndex_Invalid )
	{
		return;
	}
	//
	if( newIndex == toolButtonIndex )   return;
	//
	AIndex	oldDBIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex);//#277 #724
	AIndex	newDBIndex = mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(newIndex);//#277 #724
	GetApp().SPI_GetModePrefDB(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex()).MoveToolbarItem(oldDBIndex,newDBIndex);//#277
}

AView_Toolbar&		AWindow_Text::GetToolbarView( const AIndex inTabIndex )
{
	AControlID	toolbarControlID = mTabDataArray.GetObjectConst(inTabIndex).GetToolbarControlID();//#246
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Toolbar,kViewType_Toolbar,toolbarControlID);//#246 kControlID_ToolbarPlacard);
}

#if 0
#pragma mark ===========================

#pragma mark ---検索結果表示ペイン

/**
*/
ADocumentID	AWindow_Text::SPI_GetOrCreateFindResultDocument()
{
	if( mFindResultDocumentID == kObjectID_Invalid )
	{
		CreateFindResultDocument();
		SPI_UpdateViewBounds();
	}
	return mFindResultDocumentID;
}

/**
*/
void	AWindow_Text::CreateFindResultDocument()
{
	//Height設定
	mFindResultHeight = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kFindResultViewHeight);
	
	//Document生成
	AIndexWindowDocumentFactory	docfactory(this);
	mFindResultDocumentID = GetApp().NVI_CreateDocument(docfactory);
	
	//View生成
	AControlID	findResultViewControlID = NVM_AssignDynamicControlID();//#271
	AIndexViewFactory	indexViewFactory(GetObjectID(),/*#271kControlID_Invalid*/findResultViewControlID,mFindResultDocumentID);
	AWindowPoint	pt = {0,0};
	//#271 mFindResultViewID = NVI_CreateView(indexViewFactory,pt,10,10,true);
	mFindResultViewID = NVI_CreateView(findResultViewControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,false/*312 true*/,indexViewFactory);
	GetApp().NVI_GetViewByID(mFindResultViewID).NVI_SetBorrowerView(true);
	NVI_EnableHelpTagCallback(findResultViewControlID);//#541
	
	//FindResultSeparator生成
	AControlID	findResultSeparatorControlID = NVM_AssignDynamicControlID();//#271
	AViewDefaultFactory<AView_FindResultSeparator>	separatorfactory(GetObjectID(),/*#271kControlID_Invalid*/findResultSeparatorControlID);
	//#271 mFindResultSeparatorViewID = NVI_CreateView(separatorfactory,pt,10,10,true);
	mFindResultSeparatorViewID = NVI_CreateView(findResultSeparatorControlID,pt,10,10,kControlID_Invalid,kControlID_Invalid,true,separatorfactory);
	
	//VScrollBar生成
	mFindResultVScrollBarControlID = NVM_AssignDynamicControlID();
	NVI_CreateScrollBar(mFindResultVScrollBarControlID,pt,10,20);
	
	//閉じるボタン
	/*win
	NVI_CreateButtonView(mFindResultCloseButtonControlID,pt,10,10);
	NVI_GetButtonViewByControlID(mFindResultCloseButtonControlID).SPI_SetIcon(kIconID_Close,0,0,16,16);
	*/
	/*#725
	AWindowDefaultFactory<AWindow_ButtonOverlay>	findResultCloseButtonWindowFactory;
	mFindResultCloseButtonWindowID = GetApp().NVI_CreateWindow(findResultCloseButtonWindowFactory);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).NVI_ChangeToOverlay(GetObjectID(),false);//win
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).NVI_Create(kObjectID_Invalid);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).NVI_Show(false);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).SPI_SetTargetWindowID(GetObjectID(),kVirtualControlID_FindResultCloseButon);
	//#725 NVI_GetButtonWindow(mFindResultCloseButtonWindowID).SPI_SetIcon(kIconID_Mi_Close,0,0,16,16);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).NVI_SetOffsetOfOverlayWindowAndSize(GetObjectID(),pt,16,16);
	NVI_GetButtonWindow(mFindResultCloseButtonWindowID).SPI_SetButtonViewType(kButtonViewType_Rect16);
	*/
	//ScrollBarをViewにリンク
	GetApp().NVI_GetViewByID(mFindResultViewID).NVI_SetScrollBarControlID(kControlID_Invalid,mFindResultVScrollBarControlID);
}

/**

*/
void	AWindow_Text::DeleteFindResultDocument()
{
	//閉じるボタン削除
	/*win
	NVI_DeleteControl(mFindResultCloseButtonControlID);
	mFindResultCloseButtonControlID = kControlID_Invalid;
	*/
	/*#725
	GetApp().NVI_DeleteWindow(mFindResultCloseButtonWindowID);//win
	mFindResultCloseButtonWindowID = kObjectID_Invalid;//win
	*/
	//VScrollBar削除
	NVI_DeleteControl(mFindResultVScrollBarControlID);
	mFindResultVScrollBarControlID = kControlID_Invalid;
	//FindResultSeparator削除
	//win NVI_DeleteViewByViewID(mFindResultSeparatorViewID);
	NVI_DeleteControl(GetApp().NVI_GetViewByID(mFindResultSeparatorViewID).NVI_GetControlID());//win
	mFindResultSeparatorViewID = kObjectID_Invalid;
	//View削除
	//win NVI_DeleteViewByViewID(mFindResultViewID);
	NVI_DeleteControl(GetApp().NVI_GetViewByID(mFindResultViewID).NVI_GetControlID());//win
	mFindResultViewID = kObjectID_Invalid;
	/*#379
	//Document削除
	GetApp().NVI_DeleteDocument(mFindResultDocumentID);
	*/
	//#725 mFindResultDocumentID = kObjectID_Invalid;
	
	//
	SPI_UpdateViewBounds();
}

/**

*/
ANumber	AWindow_Text::SPI_GetFindResultHeight() const
{
	return mFindResultHeight;
}

/**
検索結果Viewの高さ決定
*/
void	AWindow_Text::SPI_SetFindResultHeight( const ANumber inHeight )
{
	if( inHeight <= 0 )
	{
		DeleteFindResultDocument();
	}
	else
	{
		mFindResultHeight = inHeight;
		GetApp().NVI_GetAppPrefDB().SetData_Number(AAppPrefDB::kFindResultViewHeight,inHeight);
	}
	SPI_UpdateViewBounds();
}
#endif

#pragma mark ===========================

#pragma mark <所有クラス(AWindow_AddToolButton)>

#pragma mark ===========================

/**
マクロバー項目追加ウインドウ表示（選択範囲テキストをマクロに追加）
*/
void	AWindow_Text::SPI_ShowAddToolButtonWindow()
{
	//タイトル取得
	AText	title, text;
	AIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
	title.SetLocalizedText("MacroDefaultName");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	title.AddText(t);
	GetApp().SPI_GetModePrefDB(modeIndex).GetUniqToolbarName(title);
	//選択範囲テキスト追加
	SPI_GetCurrentFocusTextView().SPI_GetSelectedText(text);
	//マクロバー項目追加ウインドウ表示
	SPI_ShowAddToolButtonWindow(title,text,true);
}

/**
マクロバー項目追加ウインドウ表示
*/
void	AWindow_Text::SPI_ShowAddToolButtonWindow( const AText& inTitle, const AText& inToolText, const ABool inEditWithTextWindow )
{
	//
	SPI_GetAddToolButtonWindow().NVI_CreateAndShow();
	SPI_GetAddToolButtonWindow().SPI_SetMode(SPI_GetCurrentTabTextDocument().SPI_GetModeIndex(),inEditWithTextWindow,inTitle,inToolText);
	SPI_GetAddToolButtonWindow().NVI_SwitchFocusToFirst();
}

/**
キー記録からマクロバー項目追加
*/
void	AWindow_Text::SPI_ShowAddToolButtonWindowFromKeyRecord()
{
	//タイトル取得
	AText	title, text;
	AIndex	modeIndex = SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
	title.SetLocalizedText("MacroDefaultName_KeyRecord");
	AText	t;
	ADateTimeWrapper::Get6LettersDateText(t);
	title.AddText(t);
	GetApp().SPI_GetModePrefDB(modeIndex).GetUniqToolbarName(title);
	//キー記録取得
	GetApp().SPI_GetRecordedText(text);
	//マクロバー項目追加ウインドウ表示
	SPI_ShowAddToolButtonWindow(title,text,false);
}

#pragma mark ===========================

#pragma mark ---

/*#199
void	AWindow_Text::SPI_EditPreProcess( const AObjectID inDocumentID, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(controlID).SPI_EditPreProcess(inTextIndex,inInsertedCount);
	}
}

void	AWindow_Text::SPI_EditPostProcess( const AObjectID inDocumentID, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(controlID).SPI_EditPostProcess(inTextIndex,inInsertedCount);
	}
}
*/

void	AWindow_Text::SPI_SplitView( const ABool inFocusToUpperView )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		AIndex	viewIndex = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus);
		if( viewIndex != kIndex_Invalid )//#212
		{
			SPI_SplitView(NVI_GetCurrentTabIndex(),viewIndex,inFocusToUpperView);
		}
	}
}

void	AWindow_Text::SPI_SplitView( const AIndex inTabIndex, const AIndex inViewIndex, const ABool inFocusToUpperView )
{
	//ViewHeightを取得する
	ANumber	originalViewHeight = mTabDataArray.GetObject(inTabIndex).GetViewHeight(inViewIndex);
	ANumber	newViewHeight = originalViewHeight/2;
	//クリックしたボタンに対応するViewのHeightを設定する
	mTabDataArray.GetObject(inTabIndex).SetViewHeight(inViewIndex,originalViewHeight-newViewHeight);
	//クリックしたボタンに対応するViewのキャレット、選択範囲を取得する
	ATextPoint	spt, ept;
	/*#699 ABool	caretMode = */
	GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex)).SPI_GetSelect(spt,ept);
	//クリックしたボタンに対応するViewのLocalFrameのオフセットを取得する
	AImagePoint	originOfLocalFrame;
	GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex)).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
	//#450
	//現在のviewの折りたたみ状態を取得
	ANumberArray	collapsedLinesArray;
	GetTextViewByControlID(mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex)).
			SPI_GetFoldingCollapsedLines(collapsedLinesArray);
	//新規ビュー生成
	CreateSplitView(inTabIndex,inViewIndex,newViewHeight,/*#699 caretMode,*/spt,ept,originOfLocalFrame,collapsedLinesArray);//#450
	//
	AControlID	upperTextViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex);
	AControlID	downerTextViewControlID = mTabDataArray.GetObject(inTabIndex).GetTextViewControlID(inViewIndex+1);
	//フォーカスを設定する
	if( inFocusToUpperView == true )
	{
		NVI_SwitchFocusTo(upperTextViewControlID);
	}
	else
	{
		NVI_SwitchFocusTo(downerTextViewControlID);
	}
	//AdjustScroll
	//GetTextViewByControlID(upperTextViewControlID).SPI_AdjustScroll_Center();
	GetTextViewByControlID(downerTextViewControlID).SPI_AdjustScroll_Center();
}

void	AWindow_Text::SPI_SwitchFocus_SplitView( const ABool inNext )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		AIndex	viewIndex = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus);
		if( viewIndex != kIndex_Invalid )
		{
			if( inNext == true )
			{
				viewIndex++;
			}
			else
			{
				viewIndex--;
			}
			if( viewIndex < 0 )   viewIndex = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount()-1;
			if( viewIndex >= mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetViewCount() )   viewIndex = 0;
			focus = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(viewIndex);
			NVI_SwitchFocusTo(focus);
		}
	}
}

ABool	AWindow_Text::SPI_IsCaretMode() const
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		return GetTextViewConstByControlID(focus).SPI_IsCaretMode();
	}
	return false;
}

//行移動
void	AWindow_Text::SPI_MoveToLine( const AIndex inLineIndex, const ABool inParagraphMode )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_MoveToLine(inLineIndex,inParagraphMode);
	}
}

void	AWindow_Text::SPI_ShowSaveWindow()
{
	//
	AText	filename;
	SPI_GetCurrentFocusTextDocument().NVI_GetTitle(filename);//#933 対象は現在のフォーカスのドキュメントに変更（以下この関数内各箇所に適用）
	
	//B0000 新規ドキュメントの場合は拡張子をつける
	if( SPI_GetCurrentFocusTextDocument().NVI_IsFileSpecified() == false )
	{
		AModeIndex	modeIndex = SPI_GetCurrentFocusTextDocument().SPI_GetModeIndex();
		/*#207
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kSuffix) > 0 )
		{
			AText	suffix;
			GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kSuffix,0,suffix);
			filename.AddText(suffix);
		}
		*/
		//拡張子設定の最初の項目を取得
		//#844 if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_Bool(AModePrefDB::kUseDefaultSuffixForSave) == true )
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_TextArray(AModePrefDB::kSuffix) > 0 )//#844
		{
			AText	suffix;
			//#844 GetApp().SPI_GetModePrefDB(modeIndex).GetData_Text(AModePrefDB::kDefaultSuffixForSaveText,suffix);
			GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kSuffix,0,suffix);//#844
			filename.AddText(suffix);
		}
	}
	
	AFileAttribute	attr;
	SPI_GetCurrentFocusTextDocument().SPI_GetFileAttribute(attr);
	//デフォルトフォルダの設定
	AFileAcc	defaultfolder;
	//#844 if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kSaveAsDefaultFolderIsOriginalFileFolder) == true )
	{
		if( SPI_GetCurrentFocusTextDocument().NVI_IsFileSpecified() == true )
		{
			//------------------既存ファイルをコピー保存の場合------------------
			//現在のファイルの親フォルダ
			AFileAcc	file;
			SPI_GetCurrentFocusTextDocument().NVI_GetFile(file);
			defaultfolder.SpecifyParent(file);
		}
		else
		{
			//------------------新規ドキュメントを保存の場合------------------
			//最後に開いたファイルの親フォルダ
			defaultfolder.SpecifyParent(GetApp().SPI_GetLastOpenedTextFile());
		}
	}
	/*#844
	else//#429
	{
		//最後に開いたファイルの親フォルダ
		defaultfolder.SpecifyParent(GetApp().SPI_GetLastOpenedTextFile());
	}
	*/
	AText	filter;//win 080709
	GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
	NVI_ShowSaveWindow(filename,SPI_GetCurrentFocusTextDocument().GetObjectID(),attr,filter,GetEmptyText(),defaultfolder);
}

//ドキュメントのSave前処理(ADocument_TextのSPI_Save()からコールされる)
//ウインドウ位置や、キャレット位置等、Window, Viewオブジェクトが保持するデータを、ドキュメントのPrefDBへ格納する処理等
void	AWindow_Text::SPI_SavePreProcess( const AObjectID inDocumentID )
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_SavePreProcess();
	}
	ARect	bounds;
	//#850 NVI_GetWindowBounds(bounds);
	SPI_GetWindowMainColumnBounds(bounds);//#850
	APoint	pt = {-1,-1};
	if( NVI_GetSelectableTabCount() == 1 )//#850 タブ表示になっていないときのみ、現在の位置を保存する
	{
		pt.x = bounds.left;
		pt.y = bounds.top;
	}
	/*#850
	//#668 サブペインの部分はウインドウ位置・サイズには含めない
	ANumber	width = bounds.right-bounds.left;
	if( mLeftSideBarDisplayed == true )
	{
		pt.x += mLeftSideBarWidth;
		width -= mLeftSideBarWidth;
	}
	*/
	//
	GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().SetData_Point(ADocPrefDB::kWindowPosition,pt);
	GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().
			SetData_Number(ADocPrefDB::kWindowWidth,bounds.right-bounds.left);//#850 width);//#668 bounds.right-bounds.left);
	GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().
			SetData_Number(ADocPrefDB::kWindowHeight,bounds.bottom-bounds.top);
	/*#850
	//#668
	if( SPI_IsTabModeMainWindow() == false && 
				GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSaveSubPaneModeIntoDocument) == true )
	{
		GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().
				SetData_Bool(ADocPrefDB::kDisplaySubPane,mLeftSideBarDisplayed);
		GetApp().SPI_GetTextDocumentByID(inDocumentID).GetDocPrefDB().
				SetData_Number(ADocPrefDB::kSubPaneWidth,mLeftSideBarWidth);
	}
	*/
}

//
void	AWindow_Text::SaveFTPAlias()
{
	AText	title;
	SPI_GetCurrentTabTextDocument().NVI_GetTitle(title);
	AText	filename;
	title.GetFilename(filename);
	filename.AddCstring(".miremote");//#1073
	AFileAttribute	attr;
	//win attr.creator = 'MMKE';
	//win attr.type = 'ftpa';
	GetApp().SPI_GetFTPAliasFileAttribute(attr);
	AText	refText;
	refText.SetCstring("FTPAlias");
	AFileAcc	defaultfolder;
	if( SPI_GetCurrentTabTextDocument().NVI_IsFileSpecified() == true )
	{
		AFileAcc	file;
		SPI_GetCurrentTabTextDocument().NVI_GetFile(file);
		defaultfolder.SpecifyParent(file);
	}
	else
	{
		defaultfolder.SpecifyParent(GetApp().SPI_GetLastOpenedTextFile());
	}
	AText	filter;//win 080709
	GetApp().SPI_GetDefaultFileFilter(filter);//win 080709
	NVI_ShowSaveWindow(filename,NVI_GetCurrentDocumentID(),attr,filter,refText,defaultfolder);
}

/*#199
//
void	AWindow_Text::SPI_SelectText( const AObjectID inDocumentID, const AIndex inStartIndex, const AItemCount inLength )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	NVI_SelectTab(tabIndex);
	NVI_SelectWindow();
	AControlID	focus = NVI_GetFocusInTab(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_SelectText(inStartIndex,inLength);
	}
}
*/

//
void	AWindow_Text::SPI_GetSelectedText( const AObjectID inDocumentID, AText& outText ) const
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewConstByControlID(focus).SPI_GetSelectedText(outText);
	}
}

void	AWindow_Text::SPI_GetSelect( const AObjectID inDocumentID, AArray<ATextIndex>& outStart, AArray<ATextIndex>& outEnd ) const
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewConstByControlID(focus).SPI_GetSelect(outStart,outEnd);
	}
}

//B0000
void	AWindow_Text::SPI_GetSelect( const AObjectID inDocumentID, AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewConstByControlID(focus).SPI_GetSelect(outStartIndex,outEndIndex);
	}
}

void	AWindow_Text::SPI_SetSelect( const AObjectID inDocumentID, const AIndex inStart, const AIndex inEnd )
{
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_SetSelect(inStart,inEnd);
		//B0329
		GetTextViewByControlID(focus).SPI_AdjustScroll_Center();
	}
}

//Scroll位置調整
void	AWindow_Text::SPI_AdjustScroll_Center( const ADocumentID inDocumentID )
{
	//
	AControlID	focus = /*#137 NVI_GetFocusInTab*/GetTopMostFocusTextViewControlID(NVI_GetTabIndexByDocumentID(inDocumentID));
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_AdjustScroll_Center();
	}
}

/**
TextViewの幅取得
@note ビューの実際のwidth設定前でも正しいView widthを返す（UpdateViewBounds()と同じロジック）
*/
ANumber	AWindow_Text::SPI_GetTextViewWidth( const ADocumentID inDocumentID ) 
{
	//縦書きモード取得 #558
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	ABool	verticalMode = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(tabIndex)).SPI_GetVerticalMode();
	
	//対象ドキュメントがDiffTargetDocumentかどうか判定
	if( GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_IsDiffTargetDocument() == false )
	{
		if( verticalMode == false )
		{
			//通常時　レイアウト計算からのtext view幅取得
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetTabIndexByDocumentID(inDocumentID),layout);
			return layout.w_MainTextView - layout.w_MainTextMargin;
		}
		else
		{
			//縦書きモードの場合 #558
			ATextWindowLayoutData	layout = {0};
			CalculateLayout(NVI_GetTabIndexByDocumentID(inDocumentID),layout);
			return layout.h_MainColumn-layout.w_LineNumber -10 - layout.w_MainTextMargin;
		}
	}
	else
	{
		//ATextWindowLayoutData	layout = {0};
		//CalculateLayout(NVI_GetTabIndexByDocumentID(inDocumentID),layout);
		//mode index取得
		AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(inDocumentID).SPI_GetModeIndex();
		//行番号幅取得
		ANumber	lineNumberWidth = GetLineNumberWidth(NVI_GetTabIndexByDocumentID(inDocumentID));//#450
		//
		if( Is4PaneMode() == true )
		{
			//4paneモードの場合、サブテキストは、サブテキストカラムに表示される。
			return mSubTextColumnWidth - lineNumberWidth -1;
		}
		else
		{
			//3 paneモードの場合、サブテキストは、左サイドバーに表示される。
			return mLeftSideBarWidth - lineNumberWidth -1;
		}
	}
}

/*#199
//
void	AWindow_Text::SPI_DoLineInfoChangeStart( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(controlID).SPI_DoLineInfoChangeStart();
	}
}

//
void	AWindow_Text::SPI_DoLineInfoChangeEnd( const ADocumentID inDocumentID )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	for( AIndex viewIndex = 0; viewIndex < mTabDataArray.GetObjectConst(tabIndex).GetViewCount(); viewIndex++ )
	{
		AControlID	controlID = mTabDataArray.GetObjectConst(tabIndex).GetTextViewControlID(viewIndex);
		GetTextViewByControlID(controlID).SPI_DoLineInfoChangeEnd();
	}
}
*/

void	AWindow_Text::SPI_ShowWrapLetterCountWindow( const AWrapMode inWrapMode )//#1113
{
	/*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_Create(NVI_GetCurrentDocumentID());
	SPI_GetWrapLetterCountWindow().SPI_SetWrapMode(inWrapMode);//#1113
	/*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_Show();
	/*#199 mWrapLetterCountWindow*/SPI_GetWrapLetterCountWindow().NVI_SwitchFocusToFirst();
}

void	AWindow_Text::SPI_ShowTextEncodingCorrectWindow( const ABool inModify, 
		const ATextEncodingFallbackType inFallbackType )//#473
{
	SPI_GetCorrectEncodingWindow().SPNVI_CreateAndShow(NVI_GetCurrentDocumentID(),(inModify==false),inFallbackType);//#473
}

#if 0
void	AWindow_Text::SPI_ShowPrintOptionWindow()
{
	/*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_Create(NVI_GetCurrentDocumentID());
	/*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_Show();
	/*#199 mPrintOptionWindow*/SPI_GetPrintOptionWindow().NVI_SwitchFocusToFirst();
}
#endif

#if 0
void	AWindow_Text::SPI_ShowDocPropertyWindow()
{
	/*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_Create(NVI_GetCurrentDocumentID());
	/*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_Show();
	/*#199 mDocPropertyWindow*/SPI_GetPropertyWindow().NVI_SwitchFocusToFirst();
}
#endif

void	AWindow_Text::SPI_ShowDropWarningWindow( const AText& inText )
{
	/*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().NVI_Create(NVI_GetCurrentDocumentID());
	/*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().NVI_Show();
	/*#199 mDropWarningWindow*/SPI_GetDropWarningWindow().SPI_SetToolText(inText);
}

/*B0000 080810 void	AWindow_Text::SPI_ClearCursorRow()
{
	mJumpListWindow.SPI_ClearCursorRow();
}*/

void	AWindow_Text::SPI_ShowChildErrorWindow( const ADocumentID inDocumentID, const AText& inMessage1, const AText& inMessage2,
											   const ABool inLongDialog )
{
	AIndex	tabIndex = NVI_GetTabIndexByDocumentID(inDocumentID);
	if( tabIndex == kIndex_Invalid )   return;
	NVI_SelectTab(tabIndex);
	NVI_SelectWindow();
	NVI_ShowChildWindow_OK(inMessage1,inMessage2,inLongDialog);
}

void	AWindow_Text::SPI_DoTool( const AFileAcc& inFile, const AModifierKeys inModifierKeys, const ABool inDontShowDropWarning )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_DoTool(inFile,inModifierKeys,inDontShowDropWarning);
	}
}

void	AWindow_Text::SPI_DoTool_Text( const ADocumentID inDocumentID, const AText& inText, const ABool inDontShowDropWarning )
{
	if( inDocumentID == NVI_GetCurrentDocumentID() )
	{
		AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
		if( focus != kControlID_Invalid )
		{
			GetTextViewByControlID(focus).SPI_DoTool_Text(inText,inDontShowDropWarning);
		}
	}
}

//RC2
//省略入力の候補のIndexをTextViewの外側（例えば候補ウインドウ）から決める（テキストがその候補文字に入れ替わる）
void	AWindow_Text::SPI_SetCandidateIndex( const AControlID inTextViewControlID, const AIndex inIndex )
{
	//指定テキストビューがcontrol IDでなければ何もしない
	if( IsTextViewControlID(inTextViewControlID) == false )
	{
		return;
	}
	//候補index設定
	GetTextViewByControlID(inTextViewControlID).SPI_SetCandidateIndex(inIndex);
}

//R0073
void	AWindow_Text::SPI_KeyTool( const AControlID inTextViewControlID, const AUChar inKeyChar )
{
	GetTextViewByControlID(inTextViewControlID).SPI_KeyTool(inKeyChar);
}

//R0232
//ファイルが他のアプリで編集されたことを示すダイアログを表示
void	AWindow_Text::SPI_ShowAskApplyEditByOtherApp( const ADocumentID inDocumentID )
{
	/*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().NVI_Create(kObjectID_Invalid);
	/*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().SPI_AddDocument(inDocumentID);
	/*#199 mEditByOtherAppWindow*/SPI_GetEditByOtherAppWindow().NVI_Show();
}

//RC3
//外部コメントダイアログ表示
/* #138
void	AWindow_Text::SPI_ShowExternalCommentWindow( const AText& inModuleName, const AIndex inOffset )
{
	//ダイアログ表示
	mExternalCommentWindow.NVI_Create(kObjectID_Invalid);
	mExternalCommentWindow.SPNVI_Show(NVI_GetCurrentDocumentID(),inModuleName,inOffset);
}
*/
#pragma mark ===========================

#pragma mark ---Ruler

AView_Ruler&	AWindow_Text::GetRulerView( const AIndex inTabIndex )//#246
{
	/*
	AWindowID	winID = mTabDataArray.GetObjectConst(inTabIndex).GetRulerWindowID();
	return (dynamic_cast<AWindow_Ruler&>(GetApp().NVI_GetWindowByID(winID))).SPI_GetRulerView();
	*/
	///*#725
	AControlID	rulerControlID = mTabDataArray.GetObjectConst(inTabIndex).GetRulerControlID();//#246
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_Ruler,kViewType_Ruler,rulerControlID);
	//*/
	/*#199
	return dynamic_cast<AView_Ruler&>(NVI_GetViewByControlID(kControlID_Ruler));
	*/
}

//目盛り等更新
void	AWindow_Text::UpdateRulerProperty( const AIndex inTabIndex )
{
	AModeIndex	modeIndex = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetModeIndex();
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayRuler) == true )
	{
		//#1186 ANumber	x1 = GetLineNumberWidth(inTabIndex);//#450
		//#1186
		ATextWindowLayoutData	layout = {0};
		CalculateLayout(inTabIndex,layout);
		ANumber	x1 = layout.w_LineNumber;
		//
		AItemCount	rulerScaleCount = 5;
		if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kRulerScaleWithTabLetter) == true )
		{
			rulerScaleCount = GetApp().SPI_GetTextDocumentByID(NVI_GetDocumentIDByTabIndex(inTabIndex)).SPI_GetTabWidth();
		}
		GetRulerView(inTabIndex).SPI_UpdateProperty(NVI_GetDocumentIDByTabIndex(inTabIndex),x1,rulerScaleCount);
	}
}

#pragma mark ===========================

#pragma mark ---検索

ABool	AWindow_Text::SPI_FindNext( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_FindNext(inParam);
	}
	return false;
}

ABool	AWindow_Text::SPI_FindPrevious( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_FindPrevious(inParam);
	}
	return false;
}

ABool	AWindow_Text::SPI_FindFromFirst( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_FindFromFirst(inParam);
	}
	return false;
}

void	AWindow_Text::SPI_FindList( const AFindParameter& inParam, const ABool inExtractMatchedText )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		GetTextViewByControlID(focus).SPI_FindList(inParam,inExtractMatchedText);
	}
}

ABool	AWindow_Text::SPI_ReplaceNext( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_ReplaceNext(inParam);
	}
	return false;
}

AItemCount	AWindow_Text::SPI_ReplaceAfterCaret( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_ReplaceAfterCaret(inParam);
	}
	return 0;
}

AItemCount	AWindow_Text::SPI_ReplaceInSelection( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_ReplaceInSelection(inParam);
	}
	return 0;
}

AItemCount	AWindow_Text::SPI_ReplaceAll( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_ReplaceAll(inParam);
	}
	return 0;
}

ABool	AWindow_Text::SPI_ReplaceAndFind( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_ReplaceAndFind(inParam);
	}
	return false;
}

ABool	AWindow_Text::SPI_ReplaceSelectedTextOnly( const AFindParameter& inParam )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		return GetTextViewByControlID(focus).SPI_ReplaceSelectedTextOnly(inParam);
	}
	return false;
}

//#935
/**
一括置換
*/
void	AWindow_Text::SPI_BatchReplace( const AFindParameter& inParam, const AText& inBatchText )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		NVI_SwitchFocusTo(focus);//#137 検索ボックスにフォーカスがある場合はTextViewにフォーカスを移す
		GetTextViewByControlID(focus).SPI_BatchReplace(inParam,inBatchText);
	}
}

#pragma mark ===========================

#pragma mark ---ウインドウ削除

/**

保存／保存しない／キャンセル確認ダイアログを表示
ダイアログはモーダル
保存の場合：保存実行。返り値はtrue
保存しない場合：何もしない。返り値はtrue
キャンセルの場合：何もしない。返り値はfalse
*/
ABool	AWindow_Text::NVIDO_AskSaveForAllTabs()
{
	//AskSaveChangesウインドウ表示中なら何もせずリターン
	if( mAskingSaveChanges == true )    return false;
	
	//タブごとのループ（z-order手前から）
	AItemCount	displayTabCount = NVI_GetSelectableTabCount();
	for( AIndex zorder = 0; zorder < displayTabCount; zorder++ )
	{
		//タブ、ドキュメント取得
		AIndex	tabIndex = NVI_GetTabIndexByZOrderIndex(zorder);
		ADocumentID	docID = NVI_GetDocumentIDByTabIndex(tabIndex);
		//dirtyなら保存ダイアログ表示
		if( GetApp().SPI_GetTextDocumentByID(docID).NVI_IsDirty() == true )
		{
			//ウインドウ、タブ選択
			NVI_SelectWindow();
			NVI_SelectTab(tabIndex);
			//モーダルAskSaveChangesウインドウを開いて、保存するかどうかを尋ねる
			AText	filename;
			GetApp().SPI_GetTextDocumentByID(docID).NVI_GetTitle(filename);
			mAskingSaveChanges = true;
			AAskSaveChangesResult	result = NVI_ShowAskSaveChangesWindow(filename,docID,true);//modal
			mAskingSaveChanges = false;
			//結果に応じた処理
			ABool	canceled = false;
			switch(result)
			{
			  case kAskSaveChangesResult_Save:
				{
					//保存実行
					//SaveAsダイアログが開かれた場合など、保存出来なかった場合は、canceledフラグをONにする。→ループ中断して、返り値falseで返す。
					canceled = (GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false)==false);
					break;
				}
			  case kAskSaveChangesResult_DontSave:
				{
					//処理無し
					break;
				}
			  case kAskSaveChangesResult_Cancel:
				{
					canceled = true;
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
			if( canceled == true )
			{
				return false;
			}
		}
	}
	return true;
}

/**
閉じる
*/
void	AWindow_Text::NVIDO_ExecuteClose()
{
	//AskSaveChangesウインドウ表示中なら何もせずリターン
	if( mAskingSaveChanges == true )    return;
	
	//view bounds更新の抑制
	AStSuppressTextWindowUpdateViewBounds	s(GetObjectID());
	
	//タブそれぞれについて、閉じる処理を行う
	while( NVI_GetTabCount() > 0 )
	{
		//最背面タブ（表示タブのうちの最背面）のドキュメントを取得
		AIndex	closingTabIndex = NVI_GetMostBackTabIndex();//#579
		ADocumentID	closingDocID = NVI_GetDocumentIDByTabIndex(closingTabIndex);//#579 0);
		//Close実行（強制クローズ）
		ExecuteClose(closingDocID);
	}
}

#pragma mark ===========================

#pragma mark ---DiffInfoWindow

//#379
/**
Diff情報表示ウインドウ表示更新
*/
void	AWindow_Text::SPI_RefreshDiffInfoWindow() const
{
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_RefreshWindow();
}

/**
diff info ウインドウ表示
*/
void	AWindow_Text::SPI_ShowDiffWindow()
{
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Show(false);
}

/**
diff info ウインドウ非表示
*/
void	AWindow_Text::SPI_HideDiffWindow()
{
	GetApp().NVI_GetWindowByID(mDiffInfoWindowID).NVI_Hide();
}


#pragma mark ===========================

#pragma mark ---左右比較
//#737

//drop
#if 0
/**
*/
void	AWindow_Text::SPI_CompareWithSubText( const ABool inCompareWithSelectionInSubText )
{
	AControlID	mainTextControlID = mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).GetTextViewControlID(0);
	AIndex	subPaneTabIndex = NVI_GetTabIndexByDocumentID(mSubTextCurrentDocumentID);
	AControlID	subTextControlID = mTabDataArray.GetObjectConst(subPaneTabIndex).GetSubTextControlID();
	//
	AText	mainText;
	GetTextViewByControlID(mainTextControlID).SPI_GetSelectedText(mainText);
	ATextPoint	mainTextSpt = {0}, mainTextEpt = {0};
	GetTextViewByControlID(mainTextControlID).SPI_GetSelect(mainTextSpt,mainTextEpt);
	mMainSubDiff_DiffRangeMainStartParagraphIndex = 
			SPI_GetTextDocumentConst(mainTextControlID).SPI_GetParagraphIndexByLineIndex(mainTextSpt.y);
	mMainSubDiff_DiffRangeMainEndParagraphIndex = 
			SPI_GetTextDocumentConst(mainTextControlID).SPI_GetParagraphIndexByLineIndex(mainTextEpt.y);
	//
	AText	subText;
	ATextPoint	subTextSpt = {0}, subTextEpt = {0};
	if( inCompareWithSelectionInSubText == true )
	{
		GetTextViewByControlID(subTextControlID).SPI_GetSelectedText(subText);
		GetTextViewByControlID(subTextControlID).SPI_GetSelect(subTextSpt,subTextEpt);
	}
	else
	{
		SPI_GetTextDocumentConst(subTextControlID).SPI_GetText(subText);
		subTextSpt.y = 0;
		subTextEpt.y = SPI_GetTextDocumentConst(subTextControlID).SPI_GetLineCount()-1;
	}
	mMainSubDiff_DiffRangeSubStartParagraphIndex = 
			SPI_GetTextDocumentConst(subTextControlID).SPI_GetParagraphIndexByLineIndex(subTextSpt.y);
	mMainSubDiff_DiffRangeSubEndParagraphIndex = 
			SPI_GetTextDocumentConst(subTextControlID).SPI_GetParagraphIndexByLineIndex(subTextEpt.y);
	//
	mMainSubDiff_StartParagraphIndexArray.DeleteAll();
	mMainSubDiff_EndParagraphIndexArray.DeleteAll();
	mMainSubDiff_OldStartParagraphIndexArray.DeleteAll();
	mMainSubDiff_OldEndParagraphIndexArray.DeleteAll();
	//
	AFileAcc	diffTempFolder;
	GetApp().SPI_GetUniqTempFolder(diffTempFolder);
	AArray<AIndex>	startParagraphIndexArray, endParagraphIndexArray;
	ADocument_Text::GetDiffDataFor2Texts(diffTempFolder,
				subText,mainText,mMainSubDiff_DiffTypeArray,startParagraphIndexArray,endParagraphIndexArray,
				mMainSubDiff_OldStartParagraphIndexArray,mMainSubDiff_OldEndParagraphIndexArray,mMainSubDiff_DeletedTextArray);
	diffTempFolder.DeleteFileOrFolderRecursive();
	//
	AItemCount	itemCount = mMainSubDiff_DiffTypeArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		mMainSubDiff_StartParagraphIndexArray.Add(startParagraphIndexArray.Get(i)+mMainSubDiff_DiffRangeMainStartParagraphIndex);
		mMainSubDiff_EndParagraphIndexArray.Add(endParagraphIndexArray.Get(i)+mMainSubDiff_DiffRangeMainStartParagraphIndex);
		mMainSubDiff_OldStartParagraphIndexArray.Set(i,mMainSubDiff_OldStartParagraphIndexArray.Get(i)+mMainSubDiff_DiffRangeSubStartParagraphIndex);
		mMainSubDiff_OldEndParagraphIndexArray.Set(i,mMainSubDiff_OldEndParagraphIndexArray.Get(i)+mMainSubDiff_DiffRangeSubStartParagraphIndex);
	}
	//
	mIsMainSubDiffMode = true;
	mMainSubDiff_MainTextControlID = mainTextControlID;
	mMainSubDiff_SubTextControlID = subTextControlID;
	//paragraph対照表を作成
	mMainSubDiff_ParagraphIndexArray.DeleteAll();
	ADocument_Text::SPI_GetDiffParagraphIndexArray(SPI_GetTextDocumentConst(mainTextControlID).SPI_GetParagraphCount(),
			mMainSubDiff_DiffTypeArray,
			mMainSubDiff_StartParagraphIndexArray,mMainSubDiff_EndParagraphIndexArray,
			mMainSubDiff_OldStartParagraphIndexArray,mMainSubDiff_OldEndParagraphIndexArray,
			mMainSubDiff_ParagraphIndexArray);
	//
	NVI_RefreshWindow();
}

/**
*/
ABool	AWindow_Text::SPI_IsMainSubDiffMode( const AControlID inTextControlID ) const
{
	if( mIsMainSubDiffMode == true )
	{
		if( inTextControlID == mMainSubDiff_MainTextControlID || inTextControlID == mMainSubDiff_SubTextControlID )
		{
			return true;
		}
	}
	return false;
}

/**
行IndexからDiffPartインデックスを取得
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByIncludingLineIndex(  const AControlID inTextControlID , 
		const AIndex inLineIndex, ADiffType& outDiffType ) const
{
	//
	AIndex	diffIndex = kIndex_Invalid;
	outDiffType = kDiffType_None;
	//
	AIndex	paraIndex = SPI_GetTextDocumentConst(inTextControlID).SPI_GetParagraphIndexByLineIndex(inLineIndex);
	//
	if( inTextControlID == mMainSubDiff_MainTextControlID )
	{
		//
		if( paraIndex < mMainSubDiff_DiffRangeMainStartParagraphIndex || paraIndex >= mMainSubDiff_DiffRangeMainEndParagraphIndex )
		{
			outDiffType = kDiffType_OutOfDiffRange;
			return diffIndex;
		}
		//
		AItemCount	itemcount = mMainSubDiff_StartParagraphIndexArray.GetItemCount();
		for( AIndex i = 0; i < itemcount; i++ )
		{
			if( paraIndex >= mMainSubDiff_StartParagraphIndexArray.Get(i) &&
						paraIndex < mMainSubDiff_EndParagraphIndexArray.Get(i) )
			{
				diffIndex = i;
				outDiffType = mMainSubDiff_DiffTypeArray.Get(diffIndex);
				return diffIndex;
			}
		}
	}
	else if( inTextControlID == mMainSubDiff_SubTextControlID )
	{
		//
		if( paraIndex < mMainSubDiff_DiffRangeSubStartParagraphIndex || paraIndex >= mMainSubDiff_DiffRangeSubEndParagraphIndex )
		{
			outDiffType = kDiffType_OutOfDiffRange;
			return kIndex_Invalid;
		}
		//
		AItemCount	itemcount = mMainSubDiff_OldStartParagraphIndexArray.GetItemCount();
		for( AIndex i = 0; i < itemcount; i++ )
		{
			if( paraIndex >= mMainSubDiff_OldStartParagraphIndexArray.Get(i) &&
				paraIndex < mMainSubDiff_OldEndParagraphIndexArray.Get(i) )
			{
				diffIndex = i;
				outDiffType = mMainSubDiff_DiffTypeArray.Get(diffIndex);
				switch(outDiffType)
				{
				  case kDiffType_Added:
					{
						outDiffType = kDiffType_Deleted;
						break;
					}
				  case kDiffType_Deleted:
					{
						outDiffType = kDiffType_Added;
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
				return diffIndex;
			}
		}
	}
	return kIndex_Invalid;
}

//#379
/**
DiffPartのインデックスからDiffTypeを取得
*/
ADiffType	AWindow_Text::SPI_MainSubDiff_GetDiffTypeByDiffIndex( const AIndex inDiffIndex ) const
{
	return mMainSubDiff_DiffTypeArray.Get(inDiffIndex);
}

/**
DiffPart開始行からDiffPartインデックスを取得
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByStartLineIndex( const AIndex inLineIndex ) const
{
	const ADocument_Text&	doc = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID);
	//段落開始行以外なら検索しない
	if( inLineIndex > 0 )
	{
		if( doc.SPI_GetLineExistLineEnd(inLineIndex-1) == false )   return kIndex_Invalid;
	}
	//指定行から始まるDiffIndexを検索
	AIndex	paraIndex = doc.SPI_GetParagraphIndexByLineIndex(inLineIndex);
	return mMainSubDiff_StartParagraphIndexArray.Find(paraIndex);
}

/**
DiffPart終了行からDiffPartインデックスを取得
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByEndLineIndex( const AIndex inLineIndex ) const
{
	const ADocument_Text&	doc = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID);
	//段落開始行以外なら検索しない
	if( inLineIndex > 0 )
	{
		if( doc.SPI_GetLineExistLineEnd(inLineIndex-1) == false )   return kIndex_Invalid;
	}
	//指定行で終了するDiffIndexを検索
	AIndex	paraIndex = doc.SPI_GetParagraphIndexByLineIndex(inLineIndex);
	return mMainSubDiff_EndParagraphIndexArray.Find(paraIndex);
}

/**
DiffPartのインデックスからそのPartの開始行を取得
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffStartLineIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	const ADocument_Text&	doc = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID);
	AIndex	paraIndex = mMainSubDiff_StartParagraphIndexArray.Get(inDiffIndex);
	return doc.SPI_GetLineIndexByParagraphIndex(paraIndex);
}

/**
DiffPartのインデックスからそのPartの終了行を取得
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffEndLineIndexByDiffIndex( const AIndex inDiffIndex ) const
{
	const ADocument_Text&	doc = SPI_GetTextDocumentConst(mMainSubDiff_MainTextControlID);
	AIndex	paraIndex = mMainSubDiff_EndParagraphIndexArray.Get(inDiffIndex);
	return doc.SPI_GetLineIndexByParagraphIndex(paraIndex);
}

/**
このドキュメントの段落indexに対応する、Diff対象ドキュメントの段落indexを取得
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffTargetParagraphIndexFromMainTextParagraphIndex( const AIndex inParagraphIndex ) const
{
	//
	if( inParagraphIndex >= mMainSubDiff_ParagraphIndexArray.GetItemCount() )
	{
		return kIndex_Invalid;
	}
	//mDiffParagraphIndexArrayの指定indexの値を返す
	return mMainSubDiff_ParagraphIndexArray.Get(inParagraphIndex);
}

/**
DiffPart開始段落からDiffPartインデックスを取得
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByStartParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mMainSubDiff_StartParagraphIndexArray.Find(inParagraphIndex);
}

/**
DiffPart終了行からDiffPartインデックスを取得
*/
AIndex	AWindow_Text::SPI_MainSubDiff_GetDiffIndexByEndParagraphIndex( const AIndex inParagraphIndex ) const
{
	return mMainSubDiff_EndParagraphIndexArray.Find(inParagraphIndex);
}

/**
（Diff側）段落indexから、WindowPointを取得
*/
void	AWindow_Text::SPI_MainSubDiff_GetSubTextWindowPointByParagraph( const AIndex inDiffParagraphStart,
		AWindowPoint& outWindowPoint ) const
{
	outWindowPoint.x = 0;
	outWindowPoint.y = 0;
	//Diff対象ドキュメント・タブ・ビュー取得
	if( mMainSubDiff_SubTextControlID != kControlID_Invalid )
	{
		if( GetTextViewConstByControlID(mMainSubDiff_SubTextControlID).NVI_IsVisible() == true )
		{
			//（Diff側）段落indexから、WindowPointを取得
			AIndex	lineIndex = SPI_GetTextDocumentConst(mMainSubDiff_SubTextControlID).SPI_GetLineIndexByParagraphIndex(inDiffParagraphStart);
			ALocalPoint	lpt = {0,0};
			lpt.y = GetTextViewConstByControlID(mMainSubDiff_SubTextControlID).SPI_GetLocalYByLineIndex(lineIndex);
			NVI_GetWindowPointFromControlLocalPoint(mMainSubDiff_SubTextControlID,lpt,outWindowPoint);
		}
	}
}
#endif

#pragma mark ===========================

#pragma mark ---SCM

//#455
/**
コミットウインドウ表示
*/
void	AWindow_Text::SPI_ShowCommitWindow( const ADocumentID inDocID )
{
	if( GetApp().SPI_GetTextDocumentByID(inDocID).SPI_CanCommit() == false )   return;
	
	SPI_GetSCMCommitWindow().NVI_Create(inDocID);
	SPI_GetSCMCommitWindow().NVI_Show();
}

//#455
/**
次の変更箇所
*/
void	AWindow_Text::SPI_NextDiff()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_NextDiff();
	}
}

//#455
/**
次の変更箇所
*/
void	AWindow_Text::SPI_PrevDiff()
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_PrevDiff();
	}
}

#pragma mark ===========================

#pragma mark ---Progress表示

/**
インポートファイルプログレス表示
*/
void	AWindow_Text::SPI_ShowImportFileProgress( const ANumber inCount )
{
	mFooterProgress_ShowImportFileProgress = true;
	mFotterProgress_ImportFileProgressRemainCount = inCount;
	UpdateFooterProgress();
}

/**
インポートファイルプログレス非表示
*/
void	AWindow_Text::SPI_HideImportFileProgress()
{
	mFooterProgress_ShowImportFileProgress = false;
	mFotterProgress_ImportFileProgressRemainCount = 0;
	UpdateFooterProgress();
}

/**
フッター内プログレス更新
*/
void	AWindow_Text::UpdateFooterProgress()
{
	//レイアウト取得
	ATextWindowLayoutData	layout = {0};
	CalculateLayout(0,layout);
	//
	ABool	showProgressIndicator = false;
	ABool	showProgressText = false;
	AText	progressText;
	if( mFooterProgress_ShowImportFileProgress == true )//&& layout.h_Footer > 0  )
	{
		//プログレステキスト取得
		progressText.SetLocalizedText("Progress_ImportFile");
		/*
		//残りファイル数テキスト
		AText	t;
		t.SetFormattedCstring(" (%d)",mFotterProgress_ImportFileProgressRemainCount);
		progressText.AddText(t);
		*/
		//表示フラグON
		showProgressIndicator = true;
		showProgressText = true;
	}
	//
	if( showProgressText == true )
	{
		//プログレステキスト設定、表示
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).NVI_SetText(progressText);
		NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).SPI_SetWidthToFitTextWidth();
		ANumber	textWidth = NVI_GetButtonViewByControlID(kControlID_MainColumnProgressText).NVI_GetWidth();
		AWindowPoint	pt = {0};
		pt.x = layout.pt_MacroBar.x + layout.w_MacroBar - textWidth - kImportProgressRightMargin;//mFooterProgressRightPosition - kRightMargin_ProgressText - textWidth;
		pt.y = layout.pt_MacroBar.y + 2;//layout.pt_Footer.y + kTopMargin_ProgressIndicator +1;
		NVI_SetControlPosition(kControlID_MainColumnProgressText,pt);
		NVI_SetShowControl(kControlID_MainColumnProgressText,true);
		//プログレスindicator表示・非表示
		if( showProgressIndicator == true )
		{
			pt.x -= kWidth_ProgressIndicator;
			pt.y = layout.pt_MacroBar.y;
			//pt.y = layout.pt_Footer.y + kTopMargin_ProgressIndicator;
			NVI_SetControlPosition(kControlID_MainColumnProgressIndicator,pt);
			NVI_SetShowControl(kControlID_MainColumnProgressIndicator,true);
			//
			NVI_SetControlBool(kControlID_MainColumnProgressIndicator,true);
		}
		else
		{
			NVI_SetControlBool(kControlID_MainColumnProgressIndicator,false);
		}
	}
	else
	{
		//プログレステキスト非表示
		NVI_SetShowControl(kControlID_MainColumnProgressText,false);
		NVI_SetShowControl(kControlID_MainColumnProgressIndicator,false);
	}
}


#pragma mark ===========================

#pragma mark <所有クラス(View)インターフェイス>

#pragma mark ===========================

#pragma mark ---処理通知コールバック

//Viewがスクロールされたとき、その結果を他のViewに反映させる処理（スクロールバーはTextView内で反映済み）
void	AWindow_Text::SPI_ViewScrolled( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY,
										 const ABool inRedraw, const ABool inConstrainToImageSize )
{
	//==================メインtext viewスクロール時処理==================
	for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )//win アクティブタブ以外でもスクロールするようにする（ドキュメントを開くときの描画）
	{
		AIndex	viewIndex = mTabDataArray.GetObject(tabIndex).FindFromTextViewControlID(inControlID);
		if( viewIndex != kIndex_Invalid )
		{
			AImagePoint	originOfLocalFrame;
			GetTextViewByControlID(inControlID).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
			//他のTextViewのH方向スクロール位置同期
			/*#これをやると、Dragスクロールの場合はうまくいくのだが、スクロールバーの場合にうまくいかない。（スクロールバーは元々AWindowで同時スクロールしているので、動作が重複する）
			AItemCount	viewCount = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetViewCount();
			for( AIndex i = 0; i < viewCount; i++ )
			{
			AControlID	otherViewControlID = mTabDataArray.GetObject(NVI_GetCurrentTabIndex()).GetTextViewControlID(i);
			AImagePoint	otherViewOrigin;
			GetTextViewByControlID(otherViewControlID).NVI_GetOriginOfLocalFrame(otherViewOrigin);
			if( otherViewOrigin.x != originOfLocalFrame.x )
			{
			otherViewOrigin.x = originOfLocalFrame.x;
			GetTextViewByControlID(otherViewControlID).NVI_ScrollTo(otherViewOrigin);
			}
			}*/
			//行番号Viewのスクロール位置同期
			AImagePoint	pt = originOfLocalFrame;
			pt.x = 0;
			GetLineNumberViewByControlID(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(viewIndex)).
					NVI_ScrollTo(pt,inRedraw,inConstrainToImageSize);
			if( inDeltaX != 0 )
			{
				AModeIndex	modeIndex = GetTextViewByControlID(inControlID).SPI_GetTextDocument().SPI_GetModeIndex();
				if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true )
				{
					GetLineNumberViewByControlID(mTabDataArray.GetObject(tabIndex).GetLineNumberViewControlID(viewIndex)).NVI_Refresh();
				}
			}
			//ルーラーのスクロール位置同期
			if( GetRulerView(tabIndex).NVI_IsVisible() == true )//B0393 
			{
				pt = originOfLocalFrame;
				pt.y = 0;
				GetRulerView(tabIndex).NVI_ScrollTo(pt,inRedraw);
			}
		}
	}
	//==================サブtext viewスクロール時処理==================
	//#291 サブペインの行番号表示エリア更新
	//#725 if( mLeftSideBarDisplayed == true )
	{
		//各サブペインのTextViewと比較
		for( AIndex tabIndex = 0; tabIndex < mTabDataArray.GetItemCount(); tabIndex++ )
		{
			if( mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID() == inControlID )
			{
				AImagePoint	originOfLocalFrame;
				GetTextViewByControlID(inControlID).NVI_GetOriginOfLocalFrame(originOfLocalFrame);
				//サブペインのTextViewのControlIDと一致したら対応する行番号Viewをスクロールする
				AControlID	subPaneLineNumberControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextLineNumberControlID();
				AImagePoint	pt = originOfLocalFrame;
				pt.x = 0;
				GetLineNumberViewByControlID(subPaneLineNumberControlID).NVI_ScrollTo(pt,inRedraw,inConstrainToImageSize);
				//X方向のスクロールの場合、背景再描画する
				if( inDeltaX != 0 )
				{
					AModeIndex	modeIndex = GetTextViewByControlID(inControlID).SPI_GetTextDocument().SPI_GetModeIndex();
					if( GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true )
					{
						GetLineNumberViewByControlID(subPaneLineNumberControlID).NVI_Refresh();
					}
				}
			}
		}
	}
}

//
void	AWindow_Text::NVIDO_SelectionChanged( const AControlID inControlID )
{
	//#852
	//ビュー非表示なら何もしない
	if( NVI_GetViewByControlID(inControlID).NVI_IsVisible() == false )
	{
		return;
	}
	//
	if( NVI_GetViewByControlID(inControlID).NVI_GetViewType() == kViewType_Text )
	{
		//focusには現在のfocusのテキストビューが入る。（サブテキストペインがfocusならそれが入る）
		AControlID	focus = GetTopMostFocusTextViewControlID();//#137 NVI_GetCurrentFocus();
		if( focus != kControlID_Invalid )
		{
			ATextPoint	spt, ept;
			GetTextViewByControlID(focus).SPI_GetSelect(spt,ept);
			//ジャンプリストのタブ選択 #725
			GetApp().SPI_SelectJumpListWindowsTab(GetObjectID(),SPI_GetTextDocument(focus).GetObjectID());
			//ジャンプリストの選択更新
			//#291 SPI_UpdateJumpListSelection(spt.y);
			SPI_GetTextDocument(focus).SPI_UpdateJumpListSelection(spt.y,true);//#291
			//行番号ボタンの更新
			SPI_UpdateLineNumberButton(focus);
			//場所ボタンの更新
			UpdatePositionButton(focus);
			if( mTabDataArray.GetObjectConst(NVI_GetCurrentTabIndex()).FindFromTextViewControlID(focus) != kIndex_Invalid )//#212
			{
				//ルーラーのキャレット更新
				{
					AImagePoint	spt, ept;
					GetTextViewByControlID(focus).SPI_GetImageSelect(spt,ept);
					if( spt.x == ept.x )//選択範囲ドラッグに時間がかかってしまうのを防ぐために、選択状態時はルーラーキャレット更新しない
					{
						GetRulerView(NVI_GetCurrentTabIndex()).SPI_UpdateCaretImageX(spt.x,ept.x);
					}
				}
			}
			//B0308
			if( NVI_IsWindowActive() == true )
			{
				//
				AIndex	jumpMenuItemIndex = SPI_GetTextDocument(focus).SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
				//#857 if( jumpMenuItemIndex != mJumpMenuItemIndex )//B0000 高速化
				//#857 {
				//
				if( jumpMenuItemIndex >= kLimit_MaxJumpMenuItemCount )//#695
				{
					jumpMenuItemIndex = kIndex_Invalid;
				}
				//#857 GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenu(kMenuItemID_Jump,jumpMenuItemIndex);
				GetApp().SPI_UpdateJumpMenu(SPI_GetTextDocument(focus).SPI_GetJumpMenuObjectID(),jumpMenuItemIndex);//#857
				//
				//#857 mJumpMenuItemIndex = jumpMenuItemIndex;//B0000
				//#857 }
			}
			//}
			//#723 CallGraf更新
			//現在位置のfunction名等取得
			SPI_GetTextDocument(focus).SPI_SetCallGrafCurrentFunction(GetObjectID(),spt.y,false);
			
			if( mDiffDisplayMode == true && SPI_GetTextDocumentConst(focus).NVI_IsDirty() == false )
			{
				if( SPI_GetTextDocumentConst(focus).SPI_IsDiffTargetDocument() == false )
				{
					//現在のフォーカスがDiff側ドキュメントでない場合、Diff側ドキュメントの選択位置を、対応する箇所に設定する
					
					ADocumentID	diffDocID = SPI_GetTextDocument(focus).SPI_GetDiffTargetDocumentID();
					ADocumentID	workingDocID = SPI_GetTextDocument(focus).GetObjectID();
					if( diffDocID != kObjectID_Invalid )
					{
						AIndex	tabIndex = NVI_GetTabIndexByDocumentID(diffDocID);
						AControlID	subTextControlID = mTabDataArray.GetObjectConst(tabIndex).GetSubTextControlID();
						GetTextViewByControlID(subTextControlID).SPI_SetSelectByDiffTargetDocumentSelection(spt,ept);
						//スクロール調整
						SPI_AdjustDiffDisplayScroll(workingDocID,focus,kIndex_Invalid);
					}
				}
			}
			//#734
			//プレビュー更新
			if( SPI_IsSubTextView(focus) == false )//メインビューの場合のみプレビュー更新
			{
				if( GetApp().SPI_PreviewWindowExist(GetObjectID()) == true )
				{
					SPI_GetTextDocument(focus).SPI_UpdatePreview(spt.y);
				}
			}
		}
	}
}

/**
ボタンドラッグキャンセル時処理
*/
void	AWindow_Text::NVIDO_ButtonViewDragCanceled( const AControlID inControlID )
{
	AIndex	tabIndex = NVI_GetCurrentTabIndex();
	AIndex	toolButtonIndex = mTabDataArray.GetObjectConst(tabIndex).FindToolButtonIndexByID(inControlID);
	if( toolButtonIndex != kIndex_Invalid )
	{
		//マクロ項目をドラッグキャンセルしたら非表示にする
		GetCurrentTabModePrefDB().SetEnableToolbarItem(mTabDataArray.GetObjectConst(tabIndex).GetToolButtonDBIndex(toolButtonIndex),false);
	}
}

/**
ジャンプリスト選択を更新
*/
void	AWindow_Text::SPI_UpdateJumpListSelection( const ABool inAdjustScroll )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		ATextPoint	spt, ept;
		GetTextViewByControlID(focus).SPI_GetSelect(spt,ept);
		//ジャンプリストの選択更新
		SPI_GetTextDocument(focus).SPI_UpdateJumpListSelection(spt.y,inAdjustScroll);
	}
}

/**
ジャンプリストホバー更新時処理
*/
void	AWindow_Text::SPI_DoListViewHoverUpdated( const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex )
{
	AControlID	focus = GetTopMostFocusTextViewControlID();
	if( focus != kControlID_Invalid )
	{
		GetTextViewByControlID(focus).SPI_DoListViewHoverUpdated(inHoverStartDBIndex,inHoverEndDBIndex);
	}
}

/**
view bounds更新抑制クラス　コンストラクタ
*/
AStSuppressTextWindowUpdateViewBounds::AStSuppressTextWindowUpdateViewBounds( const AWindowID inTextWindowID ) : mTextWindowID(inTextWindowID)
{
	GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IncrementSuppressUpdateViewBoundsCounter();
}

/**
view bounds更新抑制クラス　デストラクタ
*/
AStSuppressTextWindowUpdateViewBounds::~AStSuppressTextWindowUpdateViewBounds()
{
	if( GetApp().NVI_ExistWindow(mTextWindowID) == true )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DecrementSuppressUpdateViewBoundsCounter();
	}
}


