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

	CWindowImp.h

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "../AbsBase/ADataBase.h"
#include "../AbsFrame/AbsFrame.h"
//#688 #include "CTableView.h"
//#688 #include "CMLTEPane.h"
#include "CPaneForAView.h"
//#688 #include "CColorPickerPane.h"
#include "AObjectArrayItem.h"

class CWindowImp;
class AWindow;
class AText;
class AUniText;

class	CChildSheetWindowImp;

@class CocoaWindow;

//UserPaneタイプ
typedef ANumber AUserPaneType;
const AUserPaneType	kUserPaneType_Invalid			= -1;
const AUserPaneType	kUserPaneType_UserPane			= 0;
const AUserPaneType	kUserPaneType_MLTEPane			= 1;
const AUserPaneType	kUserPaneType_ColorPickerPane	= 2;
const AUserPaneType	kUserPaneType_PaneForAView		= 3;
const AUserPaneType	kUserPaneType_TableView			= 4;

enum AWindowClass
{
	kWindowClass_Document = 0,
	kWindowClass_Toolbar,
	kWindowClass_Floating,
	kWindowClass_Overlay,
};

//#688
/**
オーバーレイウインドウレイヤー
*/
typedef AIndex AOverlayWindowLayer;
const AOverlayWindowLayer	kOverlayWindowLayer_None = 0;
const AOverlayWindowLayer	kOverlayWindowLayer_Bottom = 1;
const AOverlayWindowLayer	kOverlayWindowLayer_Middle = 2;
const AOverlayWindowLayer	kOverlayWindowLayer_Top = 3;

typedef IconRef AIconRef;

//コントロールtab選択用ツールバーの最初のツールバー項目
const AControlID	kControlID_FirstTabSelectorToolbarItem = 999000;
const AControlID	kControlID_MaxTabSelectorToolbarItem = 999099;

//各タブのbottom marker
const AControlID	kControlID_FirstTabBottoMarker = 997000;
const AControlID	kControlID_MaxTabBottoMarker = 997099;

//
const AControlID	kControlID_StepperOffset = 980000;

//#688
class CWindowImpCocoaObjects;

#pragma mark ===========================
#pragma mark [クラス]CWindowImp
/**
WindowImpクラス（OS APIを直接コールし、OS間の違いを吸収する）
*/
class CWindowImp: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	CWindowImp( AObjectArrayItem* inParent, AWindow& inAWin );
	~CWindowImp();
	
	//AWindow
  public:
	AWindow&				GetAWin() { return mAWin; }
	AWindow&				GetAWinConst() const { return mAWin; }
  private:
	AWindow&							mAWin;
	AWindowClass						mWindowClass;//win
	ABool								mIgnoreClick;//#1133
	ABool								mNonFloatingFloating;//#1133
	
	//#688
  public:
	CWindowImpCocoaObjects&	GetCocoaObjects() { return (*CocoaObjects); }
	const CWindowImpCocoaObjects&	GetCocoaObjectsConst() const { return (*CocoaObjects); }
  private:
	CWindowImpCocoaObjects*				CocoaObjects;
	
	//CocoaWindow取得
  public:
	CocoaWindow*			GetCocoaWindow();//#1034
	
	//B0000 080810
	//CWindowImpオブジェクト検索
  public:
	static CWindowImp&		GetWindowImpByWindowRef( const AWindowRef inWindowRef );
	static ABool			ExistWindowImpForWindowRef( const AWindowRef inWindowRef );
	static CWindowImp*		FindFloatingWindowByGlobalPoint( const AGlobalPoint& inPoint );
	
	//ウインドウ制御
  public:
	void					Create( AConstCharPtr inWindowName, const AWindowClass inWindowClass = kWindowClass_Document,//win
							const ABool inTabWindow = false, const AItemCount inTabCount = 0 );//win
	void					Create( const AWindowClass inWindowClass = kWindowClass_Document,//#138
							const ABool inHasCloseButton = true, const ABool inHasCollapseButton = true, //#291
							const ABool inHasZoomButton = true, const ABool inHasGrowBox = true, //#291
							const ABool inIgnoreClick = false,//#379
							const ABool inHasShadow = true, const ABool inHasTitlebar = true,//#688
							const AWindowRef inParentWindowRef = NULL,
							const AOverlayWindowLayer inOverlayWindowLayer = kOverlayWindowLayer_None );//#291 #688
	void					Show();
	void					Hide();
	//#688 void					SetWindowStyleToDrawer( CWindowImp& inParent );
	void					SetWindowStyleToSheet( CWindowImp& inParent );
	void					CreateToolbar( const AIndex inType );
	void					Select();
	void					SendBehind( CWindowImp& inBeforeWindow );
	void					SendBehindAll();//#567
	void					SendAbove( CWindowImp& inBeforeWindow );//#688
	void					SendAboveAll();//#688
	void					ReorderOverlayChildWindows();//#688
	void					CloseWindow();
	void					RefreshWindow() const;
	//#688 void					RefreshWindowRect( const AWindowRect& inWindowRect );
	ABool					IsPrintMode() const;
	void					CollapseWindow( const ABool inCollapse );//B0395
	void					DragWindow( const AControlID inControlID, const ALocalPoint& inMouseLocalPoint );//#182
	//#688 WindowGroupRef			RegisterOverlayWindow( const AWindowRef inChildWindow );//#291 #338
	void					SetOverlayWindowMoveTogether( const ABool inMoveTogether );//#291
	void					SetOverlayLayerTogether( const ABool inLayerTogether );//#320
	//#688 ABool					IsOverlayWindow() const { return mIsOverlayWindow; }//#291
	void					PaintRect( const AWindowRect& inRect, const AColor& inColor );//#688 {}//win
	void					UpdateWindow();//#409
	void					Zoom();//win
	void					SetFullScreenMode( const ABool inFullScreenMode );//#404
	ABool					GetFullScreenMode() const { return mFullScreenMode; }//#404
	void					SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight );//#505
	void					SetWindowMaximumSize( const ANumber inWidth, const ANumber inHeight );//#505
	void					ChangeToFloatingWindow();
	void					ChangeToOverlayWindow( const AWindowRef inParentWindowRef );
	void					OptimizeWindowBounds( const AControlID inBottomMarker, const AControlID inRightMarker );//#872
	void					ShowHideToolbar();//#688
	void					ShowToolbarCustomize();//#688
	ABool					IsToolBarShown() const;//#688
	void					ShowFontPanel( const AControlID inVirtualControlID, const AText& inFontName, const AFloatNumber inFontSize );//#688
	ABool					IsFullScreen() const;//#688
  private:
	AWindowRef							mWindowRef;
	AWindowRef							mParentWindowRef;
	CWindowImp*							mParentWindow;//★ID化検討
	//#688 TSMDocumentID						mTSMID;
	void					SetHasSheet( const ABool inHasSheet ) { mHasSheet = inHasSheet; }
	ABool								mHasSheet;
	//#688 WindowGroupRef						mOverlayWindowsGroupRef;
	//#688 AWindowRef							mOverlayParentWindowRef;//#669
	//#688 ABool								mIsOverlayWindow;//#291
	ABool								mFullScreenMode;//#404
	ARect								mWindowBoundsInStandardState;//#404
	AControlID							mMouseMovedTrackingControlID;//#688
	ANumber								mMinWidth;//#688
	ANumber								mMinHeight;//#688
	ANumber								mMaxWidth;//#688
	ANumber								mMaxHeight;//#688
	AControlID							mFontPanelVirtualControlID;//#688
	
	//#846
  public:
	void					StartModalSession();
	ABool					CheckContinueingModalSession();
	void					EndModalSession();
	void					StopModalSession();
	void					RunModalWindow();
	
	//ウインドウ情報設定
  public:
	void					SetTitle( const AText& inText );
	void					SetWindowTransparency( const AFloatNumber inTransparency );
	void					SetModified( const ABool inModified );
	void					SetDefaultOKButton( const AControlID inID );
	void					SetDefaultCancelButton( const AControlID inID );
	void					SetControlPosition( const AControlID inID, const AWindowPoint& inPoint );
	void					SetWindowFile( const AFileAcc& inFile );
	void					ClearWindowFile();
	//win void					SetFloating() {}
	//win void					SetTopMost() {}
	void					SetCloseByEscapeKey( const ABool inCloseByEscapeKey ) { mCloseByEscapeKey = inCloseByEscapeKey; }//#986
  protected:
	AControlID							mDefaultOKButton;
  private:
	AControlID							mDefaultCancelButton;
	ABool								mModified;
	ANumber								mWindowStyle;
	//#688 AObjectID							mSheetParentTextDocumentID;
	AText								mCurrentWindowFilePath;//#932
	ABool								mCloseByEscapeKey;//#986
	
	//ウインドウ情報取得
  public:
	ABool					ExistControl( const AControlID inID ) const;
	void					GetTitle( AText& outText ) const;
	ABool					IsWindowCreated() const;
	ABool					IsVisible() const;
	ABool					IsActive() const;
	ABool					IsFocusActive() const;//#688
	ABool					HasCloseButton() const;
	ABool					IsFloatingWindow() const;
	ABool					HasTitleBar() const;
	ABool					IsModalWindow() const ;
	ABool					IsResizable() const;
	ABool					IsZoomable() const;
	ABool					IsZoomed() const;
	//#688 AControlRef				GetWindowContentView() const;
	AWindowRef				GetWindowRef() const;
	void					GetWindowBoundsInStandardState( ARect& outBounds ) const;//win
	
	//ウインドウサイズ／位置Get/Set
  public:
	//#688 void					GetWindowBounds( Rect& outRect ) const;
	void					GetWindowBounds( ARect& outRect ) const;
	void					GetWindowBounds( AGlobalRect& outRect ) const;//#688
	//#688 void					SetWindowBounds( Rect &inRect );
	void					SetWindowBounds( const ARect& inRect, const ABool inAnimate = false );//#688
	void					SetWindowBounds( const AGlobalRect& inRect, const ABool inAnimate = false );//#404 #688
	ANumber					GetWindowWidth() const;
	void					SetWindowWidth( const ANumber inWidth );
	ANumber					GetWindowHeight() const;
	void					SetWindowHeight( const ANumber& inHeight );
	void					SetWindowSize( const ANumber inWidth, const ANumber inHeight, const ABool inAnimate = false );//#688
	void					GetWindowPosition( APoint& outPoint ) const;
	void					SetWindowPosition( const APoint& inPoint );
	void					ConstrainWindowPosition( const ABool inAllowPartial );//#370
	void					ConstrainWindowPosition( const ABool inAllowPartial, APoint& ioPoint ) const;//#688
	void					ConstrainWindowSize();//#385
	ABool					IsWindowCollapsed() const;//B0395
	
	//コントロール制御
  public:
	void					DrawControl( const AControlID inID );
	
	//コントロール情報Set
  public:
	void					SetEnableControl( const AControlID inID, const ABool inEnable );
	ABool					IsControlEnabled( const AControlID inID ) const;
	void					SetShowControl( const AControlID inID, const ABool inShow );
	ABool					IsControlVisible( const AControlID inID ) const;
	void					SetTextStyle( const AControlID inID, const ABool inBold );
	void					SetTextFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize );//win
	void					SetCatchReturn( const AControlID inID, const ABool inCatchReturn );
	ABool					IsCatchReturn( const AControlID inID ) const;
	void					SetCatchTab( const AControlID inID, const ABool inCatchTab );
	ABool					IsCatchTab( const AControlID inID ) const;
	void					SetAutoSelectAllBySwitchFocus( const AControlID inID, ABool inAutoSelectAll );
	ABool					IsAutoSelectAllBySwitchFocus( const AControlID inID ) const;
	void					SetDataType( const AControlID inControlID, const ADataType inDataType );
	void					SetControlSize( const AControlID inID, const ANumber inWidth, const ANumber inHeight );
	void					SetHelpTag( const AControlID inID, const AText& inText, const AHelpTagSide inTagSide );//#634
	void					HideHelpTag() const;//R0246
	void					EnableHelpTagCallback( const AControlID inID, const ABool inShowOnlyWhenNarrow );//R0240 #507
	void					EmbedControl( const AControlID inParent, const AControlID inChild );
	void					SetTextJustification( const AControlID inID, const AJustification inJustification ) {/*MacはInterfaceBuilderで設定*/}
	void					RegisterDropEnableControl( const AControlID inControlID );//#236
	void					SetControlZOrderToTop( const AControlID inControlID );//#291
	void					SetControlBindings( const AControlID inControlID,
							const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
							const ABool inWidthFixed, const ABool inHeightFixed );//#291 #688
	void					DisableEventCatch( const AControlID inControlID ) {}//win
	void					EnableFileDrop( const AControlID inControlID );//#565
	void					RefreshControlRect( const AControlID inControlID, const ALocalRect inLocalRect );//#688
	void					ScrollControl( const AControlID inControlID, const ANumber inDeltaX, const ANumber inDeltaY );//#688
	void					SetProgressIndicatorProgress( const AIndex inProgressIndex, const ANumber inProgress, const ABool inShow );//#688
  private:
	AArray<AControlID>					mDataType_ControlID;
	AArray<ADataType>					mDataType_DataType;
	
	//コントロール情報Get
  public:
	void					GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const;
	ANumber					GetControlWidth( const AControlID inID ) const;
	ANumber					GetControlHeight( const AControlID inID ) const;
	
	//座標系変換
  public:
	void					GetWindowPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AWindowPoint& inWindowPoint ) const;
	void					GetWindowRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AWindowRect& outWindowRect ) const;
	void					GetControlLocalPointFromWindowPoint( const AControlID inID, const AWindowPoint& inWindowPoint, ALocalPoint& outLocalPoint ) const;
	void					GetControlLocalRectFromWindowRect( const AControlID inID, const AWindowRect& inWindowRect, ALocalRect& outLocalRect ) const;
	void					GetGlobalPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AGlobalPoint& outGlobalPoint ) const;
	void					GetGlobalRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AGlobalRect& outGlobalRect ) const;
	AIndex					GetControlEmbeddedTabIndex( const AControlID inID ) const;//win 080618
	/*#688
	void					GetControlLocalPointFromEventParamWindowMouseLocation( 
	const AControlID inID, const HIPoint& inPoint, ALocalPoint& outControlPoint ) const;//#688
	*/
	
	//コントロール値のSet/Getインターフェイス
  public:
	void					GetText( const AControlID inID, AText& outText ) const;
	void					SetText( const AControlID inID, const AText& inText, const ABool inOnlyText = false );
	void 					SetPopupButtonTitleText( const AControlID inID, const AText& inText );//#1335
	void					AddText( const AControlID inID, const AText& inText );
	void					InputText( const AControlID inID, const AText& inText );
	void					GetBool( const AControlID inID, ABool& outBool ) const;
	ABool					GetBool( const AControlID inID ) const;
	void					SetBool( const AControlID inID, const ABool inBool );
	void					GetColor( const AControlID inID, AColor& outColor ) const;
	void					SetColor( const AControlID inID, const AColor& inColor );
	void					GetNumber( const AControlID inID, ANumber& outNumber ) const;
	ANumber					GetNumber( const AControlID inID ) const;
	void					GetFloatNumber( const AControlID inID, AFloatNumber& outNumber ) const;
	void					SetNumber( const AControlID inID, const ANumber inNumber );
	void					SetFloatNumber( const AControlID inID, const AFloatNumber inNumber );
	void					SetFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize );//#688
	void					GetFont( const AControlID inID, AText& outFontName, AFloatNumber& outFontSize ) const;//#688
	void					SetEmptyValue( const AControlID inID );
	void					SetIcon( const AControlID inControlID, const AIconID inIconID,
							const ABool inDefaultSize = false, const ANumber inLeftOffset = 4, const ANumber inTopOffset = 4 );//win 080721
	//void					SetIconByImageIndex( const AControlID inControlID, const AIndex inImageIndex );
	void					SetMinMax( const AControlID inControlID, const ANumber inMin, const ANumber inMax );
	void					SetScrollBarPageUnit( const AControlID inControlID, const ANumber inUnit );//#725
	//void					SetMouseTrackingRect( const AControlID inControlID, const AArray<ALocalRect>& inLocalRectArray );//#688
	void					SetControlMenu( const AControlID inControlID, const AMenuRef inMenuRef );//#725
	void					ShowControlFontPanel( const AControlID inControlID );//#688
  private:
	AHashArray<AControlID>				mMinMaxArray_ControlID;//#688
	AArray<ANumber>						mMinMaxArray_Min;//#688
	AArray<ANumber>						mMinMaxArray_Max;//#688
	
	//コントロール内選択インターフェイス
  public:
	void					SetSelection( const AControlID inID, const AIndex inStart, const AIndex inEnd );
	void					SetSelectionAll( const AControlID inID );
	
	//コントロール生成／削除
  public:
	/*#688
	void					CreateBevelButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
	const AText& inTitle, const ABool inEnableMenu );
	*/
	/* AView_Buttonを作ったため不要 void					CreateCustomButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
							const AText& inTitle, const ABool inEnableMenu, const AControlID inParentID, const ABool inToggle );//win 080618*/
	//void					CreateIconButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIconID inIconID );
	void					CreateScrollBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex = kIndex_Invalid );//win 080618
	void					CreateRoundedRectButton( const AControlID inID, 
							const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex );//#688
	void					CreateProgressIndicator( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex );//#688
	void					RegisterScrollBar( const AControlID inID );
	//void					CreateSearchField( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight );//R0191
	//void					CreatePlacard( const AControlID inID, const APoint& inPoint, const ANumber inWidth, const ANumber inHeight );
	void					DeleteControl( const AControlID inID );
	void					APICB_DoScrollBarAction( const AControlID inID, const ControlPartCode inPartCode );
  private:
	static void				STATIC_APICB_ScrollBarActionProc( ControlRef theControl, ControlPartCode partCode );
	
	//フォーカス管理
  public:
	/*#135
	void					SwitchFocusTo( const AControlID inControlID, const ABool inReverseSearch = false );//B0429
	void					SwitchFocusToFirst();
	*/
	AControlID				GetCurrentFocus() const;
	/*#135
	void					RegisterFocusGroup( const AControlID inControlID );
	void					SwitchFocusToNext();
	void					SwitchFocusToPrev();//B0429
	*/
	void					SetFocus( const AControlID inControlID );
	void					SetFocusNext();//#688
	void					SetFocusPrev();//#688
	void					ClearFocus( const ABool inAlsoClearLatentFocus = true );//#212
  private:
	void					UpdateFocus();
	/*#688
	AControlID							mCurrentFocus;
	AControlID							mLatentFocus;
	*/
	//#135 AArray<AControlID>					mFocusGroup;
	
	//タブView管理
  public:
	void					SetTabControlValue( const AIndex inTabIndex );//B0000
  private:
	void					UpdateTab();
	ABool					IsEmbededInTabView( const AControlID inChildControlID, const AIndex inTabIndex ) const;
	//#688 AControlRef				GetTabControlRef( const AIndex inTabIndex ) const;
	
	//WebView #734
  public:
	void					CreateWebView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight );
	void					LoadURLToWebView( const AControlID inID, const AText& inURL );
	void					ReloadWebView( const AControlID inID );
	void					GetCurrentWebViewURL( const AControlID inID, AText& outURL ) const;
	void					ExecuteJavaScriptInWebView( const AControlID inID, const AText& inText );
	void					SetWebViewFontMultiplier( const AControlID inID, const AFloatNumber inMultiplier );
	
	//UserPane管理
  public:
	/*#688
	void					RegisterTextEditPane( const AControlID inID, const ABool inWrapMode = false , 
	const ABool inVScrollbar = false, const ABool inHScrollBar = false, const ABool inHasFrame = true );
	*/
	void					ResetUndoStack( const AControlID inID );//B0323
	void					RegisterColorPickerPane( const AControlID inID );
	void					RegisterPaneForAView( const AControlID inID, const ABool inSupportInputMethod );//#688
	void					CreatePaneForAView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, /*#360 const ABool inSupportFocus,*/
							const AControlID inParentControlID, const AControlID inSiblingControlID,//#688
							const AIndex inTabIndex, const ABool inFront, const ABool inSupportInputMethod );//#688 win 080618
	/*#688
	void					CB_UserPaneValueChanged( const AControlID inID );
	void					CB_UserPaneTextInputted( const AControlID inID );
	*/
	CUserPane&				GetUserPane( const AControlID inControlID );
	const CUserPane&				GetUserPaneConst( const AControlID inControlID ) const;
  private:
	ABool					IsUserPane( const AControlID inControlID ) const;
	AUserPaneType			GetUserPaneType( const AControlID inControlID ) const;
	AObjectArray<CUserPane>				mUserPaneArray;
	AHashArray<AControlID>				mUserPaneArray_ControlID;//#645 高速化
	AArray<AControlID>					mUserPaneArray_Type;
	//#688 AArray<AControlRef>					mUserPaneArray_ControlRef;//#688
	
//#205 #if USE_IMP_TABLEVIEW
	//テーブルView管理
  public:
#if SUPPORT_CARBON
	CTableView&				GetTableViewByID( const AControlID inControlID );
	const CTableView&				GetTableViewConstByID( const AControlID inControlID ) const;
#endif
	ABool					IsTableView( const AControlID inControlID ) const;
	//TableView登録
	void					RegisterTableView( const AControlID inControlID, const AControlID inUp, const AControlID inDown,
							const AControlID inTop, const AControlID inBottom );
	//列を登録
	void					RegisterTableViewColumn( const AControlID inControlID, const APrefID inColumnID, const ANumber inDataType );
	//テーブルデータ全設定
	void					BeginSetTable( const AControlID inControlID );
	void					SetTable_Text( const AControlID inControlID, const APrefID inColumnID, const ATextArray& inTextArray );
	void					SetTable_Bool( const AControlID inControlID, const APrefID inColumnID, const ABoolArray& inBoolArray );
	void					SetTable_Number( const AControlID inControlID, const APrefID inColumnID, const ANumberArray& inNumberArray );
	void					SetTable_Color( const AControlID inControlID, const APrefID inColumnID, const AColorArray& inColorArray );
	//階層モード実装途中void	SetTable_IsContainer( const AControlID inControlID, const ABoolArray& inIsContainer );
	void					EndSetTable( const AControlID inControlID );
	//行追加
	void					BeginInsertRows( const AControlID inControlID );
	void					InsertRow_Text( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const AText& inText );
	void					InsertRow_Text_SwapContent( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, AText& ioText );
	void					InsertRow_Number( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const ANumber inNumber );
	//階層モード実装途中void	InsertRow_IsContainer( const AControlID inControlID, const AIndex inIndex, const ABool inIsContainer );
	void					EndInsertRows( const AControlID inControlID, const AIndex inRowIndex, const AIndex inRowCount );
	
	void	EnterColumnEditMode( const AControlID inControlID, const ADataID inColumnID );
	
	void					UpdateTableView( const AControlID inControlID );
	void					SetColumnIcon( const AControlID inControlID, const ADataID inColumnID, const ANumberArray& inIconIDArray );
	const ATextArray&		GetColumn_TextArray( const AControlID inControlID, const APrefID inColumnID ) const;
	const ABoolArray&		GetColumn_BoolArray( const AControlID inControlID, const APrefID inColumnID ) const;
	const ANumberArray&		GetColumn_NumberArray( const AControlID inControlID, const APrefID inColumnID ) const;
	const AColorArray&		GetColumn_ColorArray( const AControlID inControlID, const ADataID inColumnID ) const;
	void					SetColumnWidth( const AControlID inControlID, const APrefID inColumnID, const ANumber inWidth );
	ANumber					GetColumnWidth( const AControlID inControlID, const APrefID inColumnID ) const;
	void					SetTableFont( const AControlID inControlID, /*win const AFont inFont*/const AText& inFontName, const AFloatNumber inFontSize );
	void					SetInhibit0LengthForTable( const AControlID inControlID, const ABool inInhibit0Length );//#11
	void					SetTableScrollPosition( const AControlID inControlID, const APoint& inPoint );
	void					GetTableScrollPosition( const AControlID inControlID, APoint& outPoint ) const;
	void					SetTableEditable( const AControlID inControlID, const ABool inEditable );//B0367
	void					CB_TableViewChanged( const AControlID inControlID );
	void					CB_TableViewDoubleClicked( const AControlID inControlID );
	void					CB_TableViewStateChanged( const AControlID inControlID );
	void					CB_TableViewSelectionChanged( const AControlID inControlID );//#353
  private:
	AArray<AControlID>					mTableViewRowUp_ButtonControlID;
	AArray<AControlID>					mTableViewRowUp_TableControlID;
	AArray<AControlID>					mTableViewRowDown_ButtonControlID;
	AArray<AControlID>					mTableViewRowDown_TableControlID;
	AArray<AControlID>					mTableViewRowTop_ButtonControlID;
	AArray<AControlID>					mTableViewRowTop_TableControlID;
	AArray<AControlID>					mTableViewRowBottom_ButtonControlID;
	AArray<AControlID>					mTableViewRowBottom_TableControlID;
//#205 #endif
	
	//ポップアップメニューボタン管理
  public:
	void					SetMenuItemsFromTextArray( const AControlID inID, const ATextArray& inTextArray );
	void					InsertMenuItem( const AControlID inID, const AIndex inMenuItemIndex, const AText& inText );//win
	void					DeleteMenuItem( const AControlID inID, const AIndex inMenuItemIndex );//win
	void					RegisterFontMenu( const AControlID inID, const ABool inEnableDefaultFontItem );//#375
	void					RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID );
	void					UnregisterTextArrayMenu( const AControlID inControlID );
	//#232 static void				UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, const ATextArray& inTextArray );
	//#232 static void				RegisterTextArrayMenuID( const ATextArrayMenuID inTextArrayMenuID );
  private:
	AArray<AControlID>					mTextArrayMenuArray_ControlID;
	//#232 AArray<ATextArrayMenuID>			mTextArrayMenuArray_TextArrayMenuID;
	/*#232
	static AArray<ACarbonMenuRef>				sTextArrayMenu_MenuRef;
	static AArray<AControlRef>			sTextArrayMenu_ControlRef;
	static AArray<ATextArrayMenuID>		sTextArrayMenu_TextArrayMenuID;
	static ADataBase					sTextArrayMenuDB;
	*/
	/*#688 今回、フォントメニューが無くなるので、とりあえずmValueIndependentMenuArray_ValueArrayは使わない
	//#375
	AArray<AControlID>					mValueIndependentMenuArray_ControlID;
	AObjectArray< AHashTextArray >		mValueIndependentMenuArray_ValueArray;
	*/
	
	/*#349
	//ラジオボタングループ
  public:
	void					RegisterRadioGroup();
	void					AddToLastRegisteredRadioGroup( const AControlID inID );
	void					UpdateRadioGroup( const AControlID inControlID );
	void					UpdateRadioButtonDisabled( const AControlID inControlID );
  private:
	AObjectArray< AArray<AControlID> >	mRadioGroup;
	*/
	
	//イベントハンドラ
  public:
	void					APICB_CocoaWindowBoundsChanged( const AGlobalRect& inNewFrame );//#688
	void					APICB_CocoaWindowResizeCompleted();//#688
	void					APICB_CocoaDoButtonAction( const AControlID inControlID );//#688
	void					APICB_CocoaDoPopupMenuAction( const AControlID inControlID );//#688
	void					APICB_CocoaDoTextFieldAction( const AControlID inControlID );//#688
	void					APICB_CocoaDoTextFieldTextChanged( const AControlID inControlID );//#688
	void					APICB_CocoaDoFontFieldChanged( const AControlID inControlID );//#688
	void					APICB_CocoaDoFontPanelFontChanged();//#688
	void					APICB_CocoaToolbarItemAction( const AControlID inControlID );//#688
	void					APICB_CocoaDoToolbarItemAddedAction( const AControlID inControlID );//#688
	void					APICB_CocoaBecomeMainWindow();//#688
	void					APICB_CocoaResignMainWindow();//#688
	void					APICB_CocoaBecomeKeyWindow();//#688
	void					APICB_CocoaResignKeyWindow();//#688
	void					APICB_CocoaMiniaturize();//#688
	void					APICB_CocoaDoInlineInput( const AControlID inControlID, 
							const AText& inText, const AItemCount inFixLen,
							const AArray<AIndex>& inHiliteLineStyleIndex, 
							const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
							const AItemCount inReplaceLengthInUTF16 );//#688
	void					APICB_CocoaDoInlineInputGetSelectedText( const AControlID inControlID, AText& outText ) const;//#688
	void					APICB_CocoaFirstRectForCharacterRange( const AControlID inControlID, 
							const AIndex inStart, const AIndex inEnd, AGlobalRect& outRect ) ;//#688
	void					APICB_CocoaCharacterIndexForPoint( const AControlID inControlID, 
							const AGlobalPoint& inGlobalPoint, AIndex& outOffsetUTF8 ) const;//#688
	ABool					APICB_CocoaDoKeyDown( const AControlID inControlID, const AText& inText, const AModifierKeys inModifierKeys );//#688 #1080
	ABool					APICB_CocoaDoTextInput( const AControlID inControlID, const AText& inText,
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction );//#688
	void					APICB_CocoaDrawRect( const AControlID inControlID, const ALocalRect& inDirtyRect );//#688
	void					APICB_CocoaMouseDown( const AControlID inControlID,
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );//#688
	void					APICB_CocoaRightMouseDown( const AControlID inControlID,
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount );//#688
	void					APICB_CocoaMouseDragged( const AControlID inControlID,
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys );//#688
	void					APICB_CocoaMouseUp( const AControlID inControlID, const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys );//#688
	void					APICB_CocoaMouseWheel( const AControlID inControlID,
							const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );//#688
	void					APICB_CocoaMouseMoved( const AControlID inControlID,
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys );//#688
	void					APICB_CocoaMouseExited( const AControlID inControlID, const ALocalPoint inLocalPoint );//#688
	void					APICB_CocoaDraggingEntered( const AControlID inControlID, const ADragRef inDragRef, 
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys );//#688
	void					APICB_CocoaDraggingUpdated( const AControlID inControlID, const ADragRef inDragRef,
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys, ABool& outIsCopyOperation );//#688
	void					APICB_CocoaDraggingExited( const AControlID inControlID, const ADragRef inDragRef,
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys );//#688
	void					APICB_CocoaPerformDragOperation( const AControlID inControlID, const ADragRef inDragRef,
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys );//#688
	void					APICB_DoScrollBarAction( const AControlID inID, const AScrollBarPart inPart );//#688
	void					APICB_CocoaViewFrameChanged( const AControlID inControlID );//#688
	void					APICB_CocoaDoCancelOperation( const AModifierKeys inModifierKeys );//#688
	void					APICB_CocoaViewBecomeFirstResponder( const AControlID inControlID );//#688
	void					APICB_CocoaViewResignFirstResponder( const AControlID inControlID );//#688
	void					APICB_CocoaDoWindowShouldClose();//#688
	void					SetSwitchingFocusNow( const ABool inSwitchingFocusNow );
	void					APICB_CocoaQuickLook( const AControlID inControlID,
							const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys );//#1026
	ABool					APICB_CocoaDoWriteSelectionToPasteboard( const AScrapRef inScrap );//#1309
	ABool					APICB_CocoaDoReadSelectionFromPasteboard( const AScrapRef inScrap );//#1309
#if SUPPORT_CARBON
	ABool					APICB_DoControlTrack( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoMouseDown( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoMouseWheel( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	virtual ABool			APICB_DoCommand( const EventHandlerCallRef inCallRef, const EventRef inEventRef, const HICommand inCommand );
	ABool					APICB_DoKeyDown( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	virtual ABool			APICB_DoTextInput( const EventHandlerCallRef inCallRef, const EventRef inEventRef, ABool& outUpdateMenu );
	ABool					APICB_DoTextInputUpdateActiveInputArea( const EventHandlerCallRef inCallRef, const EventRef inEventRef, ABool& outUpdateMenu );
	ABool					APICB_DoTextInputOffsetToPos( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoTextInputPosToOffset( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoTextInputGetSelectedText( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoWindowActivated( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoWindowDeactivated( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoWindowCollapsed( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoWindowCloseButton( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoWindowBoundsChanged( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoWindowResizeCompleted( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoWindowCursorChange( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoWindowFocusAcquiredHandler( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	void					APICB_DoDragTracking( const DragTrackingMessage inMessage, const DragRef inDragRef );
	void					APICB_DoDragReceive( const DragRef inDragRef );
	ABool					APICB_DoMouseMoved( const EventHandlerCallRef inCallRef, const EventRef inEventRef );
	ABool					APICB_DoServiceGetTypes( const EventHandlerCallRef inCallRef, const EventRef inEventRef );//B0370
	ABool					APICB_DoServiceCopy( const EventHandlerCallRef inCallRef, const EventRef inEventRef );//B0370
	ABool					APICB_DoServicePaste( const EventHandlerCallRef inCallRef, const EventRef inEventRef );//B0370
	AArray<AIndex>						mCurrentInlineInputUTF8PosIndexArray;
	AArray<AIndex>						mCurrentInlineInputUTF16PosIndexArray;
#endif
  private:
	AGlobalRect							mCurrentWindowBounds;//#688
	
	//#688
  public:
	ABool					IsDoingResizeCompleted() const { return mDoingResizeCompleted; }
  private:
	ABool								mDoingResizeCompleted;
	
  public:
#if SUPPORT_CARBON
  private:
	AControlID				FindDroppedControl( const AWindowPoint inPoint ) const;//#236
	AArray<AControlID>					mDropEnableControlArray;//#236
#endif
	AControlID							mDragTrackingControl;//#236
	
	//
  public:
	void					DoMouseExited( const AControlID inID, const EventRef inEventRef );//B0000 080810 R0228
	
	/*#688
	//コントロールID/Ref変換
  public:
	AControlID				GetControlID( const AControlRef inControlRef ) const;
	AControlRef				GetControlRef( const AControlID inID ) const;
  private:
	AControlRef				GetControlRef( const OSType inSignature, const AControlID inID ) const;
	ACarbonMenuRef			GetMenuRef( const AControlID inID ) const;
	*/
	
	//ファイル／フォルダ選択
  public:
	void					ShowChooseFolderWindow( const AFileAcc& inDefaultFolder,const AText& inMessage,
							const AControlID inVirtualControlID, const ABool inSheet );//#551
	void					ShowChooseFileWindow( const AFileAcc& inDefaultFolder, const AText& inMessage,
							const AControlID inVirtualControlID, const ABool inOnlyApp = false, 
							const ABool inSheet = true );
	void					ShowChooseFileWindowWithFileFilter( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inSheet,
							const AFileAttribute inFilterAttribute );
	/*#1034 void					ShowAskSaveChangesWindow( const AText& inFileName, const AObjectID inDocumentID,
													 const ABool inModal = false );//#216AControlID inVirtualControlID );*/
	//OSX10.4でクラッシュするのでやめる。NavDialogDispose()がNG？ void	CloseAskSaveChanges();//B0000
	void					ShowSaveWindow( const AText& inFileName, const ADocumentID inDocumentID, 
							const AFileAttribute inFileAttr, const AText& inFilter, const AText& inRefText, const AFileAcc& inDefaultFolder );
  private:
	//Nav関連
	//#1034 void					DoNavChooseFolderCB( const NavEventCallbackMessage selector, const NavCBRecPtr params );
	//#1034 void					DoNavChooseFileCB( const NavEventCallbackMessage selector, const NavCBRecPtr params );
	//#1034 void					DoAskSaveChangesCB( const NavEventCallbackMessage selector, const NavCBRecPtr params );
	//#1034 void					DoNavPutDialogCB( const NavEventCallbackMessage selector, const NavCBRecPtr params );
  public:
	void					APICB_ShowChooseFolderWindowAction( const AText& inPath );//#1034
	void					APICB_ShowChooseFileWindowAction( const AText& inPath );//#1034
	void					APICB_ShowSaveWindowAction( const AText& inPath );//#1034
	//#1034 static pascal void		STATIC_NavChooseFolderCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD );
	//#1034 static Boolean			STATIC_NavFilterCB_ApplicationFile ( AEDesc * inDesc, void * inInfo, void * inCallBackUD, NavFilterModes inFilterMode );
	//#1034 static Boolean			STATIC_NavFilterCB_CreatorTypeFilter( AEDesc * inDesc, void * inInfo, void * inCallBackUD, NavFilterModes inFilterMode );
	//#1034 static pascal void		STATIC_NavChooseFileCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD );
	//#1034 static pascal void		STATIC_NavAskSaveChangesCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD );
	//#1034 static pascal void		STATIC_NavPutDialogCB( NavEventCallbackMessage selector, NavCBRecPtr params, void *callbackUD );
  private:
	/*#1034
	NavDialogRef	mNavSheet;
	AFileAcc	mDefaultFile;
	*/
	AControlID	mChooseFileVirtualControlID;
	AControlID	mChooseFolderVirtualControlID;
	//#1034 AObjectID	mAskSaveChangesDocumentID;
	AObjectID	mSaveWindowDocumentID;
	AText		mSaveWindowRefText;
	//#1034 AFileAcc	mSaveWindowDefaultFolder;
	AFileAttribute	mSaveWindowFileAttribute;
	/*#1034
	OSType		mNavFileFilter_Creator;
	OSType		mNavFileFilter_Type;
	*/
	//子Sheetウインドウ管理
  public:
	/*#291
	void	ShowChildWindow_OK( const AText& inMessage1, const AText& inMessage2 );
	void	ShowChildWindow_OKCancel( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
			const AControlID inOKButtonVirtualControlID );
	void	ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
			const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID );//#8
	void	CloseChildWindow();
	*/
	ABool	ChildWindowVisible() const;
  private:
	//#291 AObjectArray<CChildSheetWindowImp>	mChildSheetWindow;
	
	//背景
  public:
	//#688 void	GetBackgroundData( const AIndex inBackgroundImageIndex, GWorldPtr& outGWorldPtr, Rect& outBoundRect ) const;
	static AIndex	RegisterBackground( const AFileAcc& inBackgroundImageFile );
	void	DrawBackgroundImage( const AControlID inControlID, const ALocalRect& inEraseRect, const AImageRect& inImageFrameRect, 
			const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha );//#688
	/*#688
  private:
	static AArray<GWorldPtr>	sBackgroundImageArray;
	static AObjectArray<AFileAcc>	sBackgroundImageArray_File;
	static AArray<ARect>	sBackgroundImageArray_Bound;
	*/
	//
  public:
	ABool	IsInputBackslashByYenKeyMode() {return mInputBackslashByYenKeyMode;}
	void	SetInputBackslashByYenKeyMode( const ABool inMode ) { mInputBackslashByYenKeyMode = inMode; }
  private:
	ABool	mInputBackslashByYenKeyMode;
	
	/*#688
	//イメージ
  public:
	static AIndex	RegisterImage( const AFileAcc& inImageFile );
  private:
	static PicHandle	GetImage( const AIndex inImageIndex );
	static AArray<PicHandle>	sImagePicHandle;
	*/
	//アイコン登録
  public:
	static void				RegisterIcon( const AIconID inIconID, const AFileAcc& inIconFile, const AConstCstringPtr inName = "" );//#232
	/*#1034
	static AIconID			RegisterIconFromResouce( const AIndex inResID );
	static void				RegisterIconFromResouce( const AIconID inIconID, const AIndex inResID );
	*/
	//#1012 static AIconID			RegisterDynamicIconFromImageFile( const AFileAcc& inFile );//R0217
	static AIconID			RegisterDynamicIconFromFile( const AFileAcc& inIconFile, const ABool inLoadEvenIfNotCustomIcon = true );//#232
	static void				UnregisterDynamicIcon( const AIconID inIconID );
	static AIconID			GetIconIDByName( const AText& inName );//#232
	static void				CopyIconToFile( const AIconID inIconID, AFileAcc& inDestIconFile );//#232
	static void				GetIconFileSuffix( AText& outSuffix );//#232
	static AIconID			AssignDynamicIconID();
	static IconRef			GetIconRef( const AIconID inIconID );
	//#1034 static CIconHandle		GetCIconHandle( const AIconID inIconID );
	static ABool			IsDynamicIcon( const AIconID inIconID );//#232
  private:
	static AHashArray<AIconID>			sIconIDArray;
	static AArray<IconRef>				sIconRefArray;
	//#1034 static AArray<CIconHandle>			sCIconHandleArray;
	static AObjectArray<AFileAcc>		sIconFileArray;//#232
	static AHashTextArray				sIconNameArray;//#232
	static AIconID						sNextDynamicIconID;
	static const AIconID	kDynamicIconIDStart = 10000;
	static const AIconID	kDynamicIconIDEnd = kNumber_MaxNumber;
	
	//イベントハンドラ（OSコールバック用static）
  public:
	static pascal OSStatus	STATIC_APICB_MouseDownHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_CommandHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_KeyDownHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_TextInputHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_TextInputUpdateActiveInputAreaHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus 	STATIC_APICB_TextInputOffsetToPosHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_TextInputPosToOffsetHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_TextInputGetSelectedText( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_TSMDocumentAccessLockDocument( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0330
	static pascal OSStatus	STATIC_APICB_TSMDocumentAccessUnlockDocument( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0330
	static pascal OSStatus	STATIC_APICB_TSMDocumentAccessGetLength( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0330
	static pascal OSStatus	STATIC_APICB_TSMDocumentAccessGetSelectedRange( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0330
	static pascal OSStatus	STATIC_APICB_TSMDocumentAccessGetCharacters( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0330
	static pascal OSStatus	STATIC_APICB_TSMDocumentAccessGetFont( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0330
	static pascal OSStatus	STATIC_APICB_TSMDocumentAccessGetGlyphInfo( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0330
	static pascal OSStatus	STATIC_APICB_WindowActivatedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowDeactivatedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowCollapsedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowBoundsChangedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowResizeCompletedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowZoomedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowCursorChangeHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowMouseExitedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_DoMouseMoved( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowCloseHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_WindowFocusAcquiredHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_MouseWheelHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static OSErr			STATIC_APICB_DragTracking( DragTrackingMessage inMessage, AWindowRef inWindowRef, void * inHandlerRefCon, DragRef inDragRef );
	static OSErr			STATIC_APICB_DragReceive( AWindowRef inWindowRef, void * inHandlerRefCon, DragRef inDragRef );
	static pascal OSStatus	STATIC_APICB_ServiceGetTypes( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0370
	static pascal OSStatus	STATIC_APICB_ServiceCopy( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0370
	static pascal OSStatus	STATIC_APICB_ServicePaste( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );//B0370
	static pascal OSStatus	STATIC_APICB_ControlHelpTagCallback( ControlRef inControl,
					Point inGlobalMouse, HMContentRequest inRequest,
					HMContentProvidedType *outContentProvided,
					HMHelpContentPtr ioHelpContent );//R0240
	void					InstallEventHandler();
	void					UninstallEventHandler();
	static pascal OSStatus	STATIC_APICB_ControlTrackHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
  private:
	AArray<EventHandlerRef>				mInstalledEventHandler;
	
	/*#539
	//ヘルプ
  public:
	void	RegisterHelpAnchor( const ANumber inHelpNumber, AConstCstringPtr inAnchor );
  private:
	void	DisplayHelpPage( const ANumber inHelpNumber );
	ANumberArray	mHelpAnchor_Number;
	ATextArray	mHelpAnchor_Anchor;
	*/
	//
  public:
	static ABool			ExistWindowImp( CWindowImp* inWindowImp ) 
	{ return (sAliveWindowArray.Find(inWindowImp)!=kIndex_Invalid); }//#688
	static AArray<CWindowImp*>	sAliveWindowArray;
	
	//WindowStyle
  public:
	static const ANumber	kWindowStyle_Normal		= 0;
	//#688 static const ANumber	kWindowStyle_Drawer		= 1;
	static const ANumber	kWindowStyle_Sheet		= 2;
	
	//ユーティリティ
  public:
	static void				GetCurrentMouseLocation( AGlobalPoint& outGlobalPoint );//#725
	
	//画面サイズ取得
  public:
	static void				GetAvailableWindowBoundsInMainScreen( AGlobalRect& outRect );
	void					GetAvailableWindowBoundsInSameScreen( AGlobalRect& outRect ) const;
	static void				GetAvailableWindowBounds( const AIndex inScreenIndex, AGlobalRect& outRect );
	static AIndex			FindScreenIndexFromPoint( const APoint inPoint );
	
	//LineBreak取得
  public:
	/*win
	static AItemCount		GetLineBreak( const AText& inText, const AText& inFontName, 
							const AFloatNumber inFontSize, const ABool inAntiAlias,
							const ANumber inViewWidth );
	*/
	static AItemCount		GetLineBreak( CTextDrawData& ioTextDrawData, AItemCount inStartOffset, 
										  const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
										  const ANumber inViewWidth, const ABool inLetterWrap );//#1113
	static void				CalcLineBreaks( const AText& inText, 
											const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
											const ANumber inViewWidth, const ABool inLetterWrap, //#1113
											AArray<AIndex>& outUTF8LineStartArray, AArray<AItemCount>& outUTF8LineLengthArray );//#853
	static void				CalcLineBreaks( CTextDrawData& ioTextDrawData, AItemCount inStartOffset, 
											const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
											const ANumber inViewWidth, const ABool inLetterWrap,//#1113
											AArray<AIndex>& outUTF16StartOffsetArray, AArray<AIndex>& outUTF16EndOffsetArray );//#853
	
	//#1090
	//NSFont取得
  public:
	static NSFont*			GetNSFont( const AText& inFontName, const AFloatNumber inFontSize, const ATextStyle inTextStyle );
  private:
	static AHashTextArray				sNSFontCache_Key;
	static AArray<NSFont*>				sNSFontCache_NSFont;
	//最後に使ったフォント #1275
	static NSFont*						sLastNSFont;
	static AText						sLastNSFont_FontName;
	static AFloatNumber					sLastNSFont_FontSize;
	static ATextStyle					sLastNSFont_TextStyle;
	
	//インライン入力
  public:
	void	FixInlineInput( const AControlID inControlID );
	ABool	GetInlineInputMode() {return true;}//#448
	
	/*#1034
	//ATSUユーティリティ
  public:
	static void		SetATSUStyle( ATSUStyle& ioATSUTextStyle, const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAliasing,
					const AColor inColor, const AFloatNumber inAlpha, const ATextStyle inTextStyle );//B0000
	static ATSUTextLayout	CreateTextLayout( const ATSUStyle inATSUStyle, const AText& inUTF16Text, 
							const ABool inAntiAliasing, const ABool inUseFontFallback, ADrawContextRef inContextRef );//#703
	*/
	/*#688 
	//#353
	//フルキーボードアクセス
  public:
	static ABool			STATIC_CheckIsFocusGroup2ndPattern();
	static ABool			STATIC_GetIsFocusGroup2ndPattern() { return sIsFocusGroup2ndPattern; }
  private:
	static ABool						sIsFocusGroup2ndPattern;
	*/
	
	//縦書き用座標変換 #558
  public:
	ALocalPoint			ConvertVerticalCoordinateFromAppToImp( const AControlID inControlID, const ALocalPoint& inAppPoint ) const;
	ALocalRect			ConvertVerticalCoordinateFromAppToImp( const AControlID inControlID, const ALocalRect& inAppRect ) const;
	ALocalPoint			ConvertVerticalCoordinateFromImpToApp( const AControlID inControlID, const ALocalPoint& inImpPoint ) const;
	ALocalRect			ConvertVerticalCoordinateFromImpToApp( const AControlID inControlID, const ALocalRect& inImpRect ) const;
	
  public:
	static void				SetMouseTracking( CWindowImp* inWindowImp, const AControlID inControlID );
	static void				ProcessMouseExitToAnotherControl( CWindowImp* inWindowImp, 
							const AControlID inControlID, const ALocalPoint inLocalPoint );
  private:
	static ABool						sMouseTrackingMode;
	static CWindowImp*					sMouseTrackingWindowImp;
	static AControlID					sMouseTrackingControlID;
	
	//win
	//ウインドウ取得
  public:
	static AWindowRef		GetMostFrontWindow();
	static AWindowRef		GetNextOrderWindow( const AWindowRef inWindowRef );
	static AWindowRef		GetNthWindow( const ANumber inNumber );
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "CWindowImp"; }
	
};

//CarbonCore/FixMath.h
#define __fixed1              ((Fixed) 0x00010000L)
#define __fract1              ((Fract) 0x40000000L)
#define __positiveInfinity    ((long)  0x7FFFFFFFL)
#define __negativeInfinity    ((long)  0x80000000L)

#define ConvertFixedRound(a)       ((short)(((Fixed)(a) + __fixed1/2) >> 16))
#define ConvertFixedSquareRoot(a)  (((Fixed)FractSquareRoot(a) + 64) >> 7)
#define ConvertFixedTruncate(a)    ((short)((Fixed)(a) >> 16))
#define ConvertFixedToFract(a)     ((Fract)(a) << 14)
#define ConvertFractToFixed(a)     (((Fixed)(a) + 8192L) >> 14)
#define ConvertFixedToInt(a)       ((short)(((Fixed)(a) + __fixed1/2) >> 16))
#define ConvertIntToFixed(a)       ((Fixed)(a) << 16)
#define ConvertFixedToFloat(a)     ((float)(a) / __fixed1)
#define ConvertFloatToFixed(a)     ((Fixed)((float)(a) * __fixed1))
#define ConvertFractToFloat(a)     ((float)(a) / __fract1)
#define ConvertFloatToFract(a)     ((Fract)((float)(a) * __fract1))
#define ConvertColorToFract(a)     (((Fract) (a) << 14) + ((Fract)(a) + 2 >> 2))
#define ConvertFractToColor(a)     ((gxColorValue) ((a) - ((a) >> 16) + 8191 >> 14))
