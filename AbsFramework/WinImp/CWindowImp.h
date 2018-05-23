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

	CWindowImp (Windows)

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "../AbsFrame/AbsFrame.h"
#include "../Wrapper.h"
#include "CTextDrawData.h"
#include <windows.h>

class AWindow;
class CUserPane;
class CTableView;

//UserPaneタイプ
typedef ANumber AUserPaneType;
const AUserPaneType	kUserPaneType_Invalid			= -1;
const AUserPaneType	kUserPaneType_UserPane			= 0;
const AUserPaneType	kUserPaneType_PaneForAView		= 1;
const AUserPaneType	kUserPaneType_ColorPickerPane	= 2;
const AUserPaneType	kUserPaneType_RebarContent		= 3;
const AUserPaneType	kUserPaneType_CustomButton		= 4;

//Controlタイプ
typedef ANumber AControlType;
const AControlType	kControlType_Invalid			= -1;
const AControlType	kControlType_UserPane			= 0;
const AControlType	kControlType_ScrollBar			= 1;
const AControlType	kControlType_Button				= 2;
const AControlType	kControlType_Edit				= 3;
const AControlType	kControlType_ComboBox			= 4;
const AControlType	kControlType_Static				= 5;
const AControlType	kControlType_Slider				= 6;
const AControlType	kControlType_ProgressBar		= 7;

enum AWindowClass
{
	kWindowClass_Document = 0,
	kWindowClass_Toolbar,
	kWindowClass_Floating,
	kWindowClass_Overlay
};

#pragma mark ===========================
#pragma mark [クラス]CWindowImp
/**
ウインドウ実装クラス
*/
class CWindowImp: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	CWindowImp( AObjectArrayItem* inParent, AWindow& inAWin );
	~CWindowImp();
  private:
	AWindow&							mAWin;
	
	//AWindowを取得
  public:
	AWindow&				GetAWin();
	AWindow&				GetAWinConst() const;
	
	//ウインドウ制御
  public:
	void					Create( const AWindowClass inWindowClass = kWindowClass_Document,//#138
							const ABool inHasCloseButton = true, const ABool inHasCollapseButton = true, //#219
							const ABool inHasZoomButton = true, const ABool inHasGrowBox = true,
							const ABool inIgnoreClick = false,//#379
							const AWindowRef inParentWindowRef = NULL, const ABool inCocoa = false );//#219
	void					Create( AConstCharPtr inWindowName,
							const AWindowClass inWindowClass = kWindowClass_Document,//win
							const ABool inTabWindow = false, const AItemCount inTabCount = 0, const ABool inCocoa = false );
	void					Show( const int nCmdShow = SW_SHOW );
	void					Hide();
	void					SetWindowStyleToDrawer( CWindowImp& inParent );
	void					SetWindowStyleToSheet( CWindowImp& inParent );
	void					Select();
	void					SendBehind( CWindowImp& inBeforeWindow );
	void					SendBehindAll();//#567
	//win void					SetTopMost();
	void					CloseWindow();
	void					RefreshWindow();
	//#688 void					RefreshWindowRect( const AWindowRect& inWindowRect );
	ABool					IsPrintMode() const;
	void					CollapseWindow( const ABool inCollapse );//B0395
	void					RegisterOverlayWindow( const AWindowRef inChildWindow, const AWindowRef inChildBackgroundWindow );//#291
	void					UnregisterOverlayWindow( const AWindowRef inChildWindow );
	void					SetOverlayWindowMoveTogether( const ABool inMoveTogether );//#291
	void					SetOverlayLayerTogether( const ABool inLayerTogether );//#320
	void					UpdateOverlayBackgroundWindow();
	ABool					IsOverlayWindow() const { return (mWindowClass==kWindowClass_Overlay); }//#291
	HWND					GetOverlayBackgroungWindowHWnd() const { return mHWndOverlayBackgroundWindow; }
	const AColor&			GetOverlayBackgroundColor() const { return mOverlayBackgroundColor; }
	HWND					GetParentWindowHWnd() const { return mParentWindowRef; }
	void					TryMouseLeaveForAllUserPanes();
	void					TryMouseLeaveForAllUserPanesExceptFor( const AControlID inExceptControlID );
	void					TryMouseLeaveForAllWindowsExceptFor( const AControlID inExceptControlID );
	void					PaintRect( const AWindowRect& inRect, const AColor& inColor );
	void					UpdateWindow();//#409
	void					Zoom();
	void					SetFullScreenMode( const ABool inFullScreenMode );//#404
	ABool					GetFullScreenMode() const { return mFullScreenMode; }//#404
	void					DragWindow( const AControlID inControlID, const ALocalPoint& inMouseLocalPoint ) {}//#182
	void					SetWindowMinimumSize( const ANumber inWidth, const ANumber inHeight );//#505
	void					SetWindowMaximumSize( const ANumber inWidth, const ANumber inHeight );//#505
	void					ChangeToFloatingWindow();
  private:
	static BOOL CALLBACK	STATIC_APICB_EnumChildProc( HWND hwnd, LPARAM lParam);
	void					RegisterControl( const HWND hwnd );
	void					UnregisterControl( const AControlID inID );
	void					SetOverlayWindowOffset( const AWindowRef inChildWindow );
	AArray<AControlID>					mControlArray_ID;
	AArray<HWND>						mControlArray_HWnd;
	AArray<HFONT>						mControlArray_HFont;
	AArray<ADataType>					mControlArray_DataType;
	AArray<WNDPROC>						mControlArray_OriginalProc;
	AArray<AControlType>				mControlArray_ControlType;
	AArray<ABool>						mControlArray_CatchReturn;
	AArray<ABool>						mControlArray_CatchTab;
	AArray<ABool>						mControlArray_AutoSelectAllBySwitchFocus;
	AWindowClass						mWindowClass;
	AGlobalRect							mCurrentWindowBounds;//#291
	ANumber								mCurrentWindowSize_Width;//win
	ANumber								mCurrentWindowSize_Height;//win
	AWindowRef							mParentWindowRef;//#291
	AArray<AWindowRef>					mOverlayWindowGroup_WindowRef;//#291
	AArray<AWindowRef>					mOverlayWindowGroup_BackgroundWindowRef;
	AArray<ANumber>						mOverlayWindowGroup_XOffset;//#291
	AArray<ANumber>						mOverlayWindowGroup_YOffset;//#291
	AColor								mOverlayBackgroundColor;
	HWND								mHWndOverlayBackgroundWindow;
	BYTE								mAlpha;
	ABool								mNoActivate;//ウインドウをactiveにしない
	ABool								mZoomed;//maximized状態かどうか
	AGlobalRect							mUnzoomedBounds;//maximized状態前のウインドウサイズ
	ABool								mFullScreenMode;//#404
	ARect								mWindowBoundsInStandardState;//#404 通常状態でのウインドウ領域
	ANumber								mWindowMinWidth;//#505
	ANumber								mWindowMinHeight;//#505
	
	//Modal
  private:
	AWindowRef							mWindowModalParentWindowRef;//WindowModal時の親ウインドウのWindowRef
	void					SetWindowModalParentMode( const ABool inWindowModalParentMode )
	{ mWindowModalParentMode = inWindowModalParentMode; }
	ABool								mWindowModalParentMode;
	
	//ウインドウサブクラス化
  private:
	LRESULT					APICB_ControlSubclassProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK	STATIC_APICB_ControlSubclassProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp );
	
public:
	static void	InitWindowClass();
	
	//ウインドウ属性設定
  public:
	void	SetTitle( const AText& inText );
	void	SetWindowTransparency( const AFloatNumber inTransparency );
	void	SetModified( const ABool inModified );
	void	SetDefaultOKButton( const AControlID inID );
	void	SetDefaultCancelButton( const AControlID inID );
	void	SetControlPosition( const AControlID inID, const AWindowPoint& inPoint );
	void	SetWindowFile( const AFileAcc& inFile );
	void	ClearWindowFile();
	void	SetInputBackslashByYenKeyMode( const ABool inMode ) { mInputBackslashByYenKeyMode = inMode; }
	ABool	IsInputBackslashByYenKeyMode() {return mInputBackslashByYenKeyMode;}
	//win void	SetFloating();
	HBRUSH	GetBackgroundBlash() const;
  private:
	AControlID							mDefaultOKButton;
	AControlID							mDefaultCancelButton;
	ABool								mInputBackslashByYenKeyMode;
	
	//ウインドウ情報取得
  public:
	ABool	ExistControl( const AControlID inID ) const;
	void	GetTitle( AText& outText ) const;
	ABool	IsWindowCreated() const;
	ABool	IsVisible() const;
	ABool	IsActive() const;
	ABool	HasCloseButton() const;
	ABool	IsFloatingWindow() const;
	ABool	HasTitleBar() const;
	ABool	IsModalWindow() const ;
	ABool	IsResizable() const;
	ABool	IsZoomable() const;
	ABool	IsZoomed() const;
	void	GetWindowBoundsInStandardState( ARect& outBounds ) const;
	AWindowRef	GetWindowRef() const;
	
	//ウインドウサイズ／位置Get/Set
  public:
	void	GetWindowBounds( ARect& outRect ) const;
	void	GetWindowBounds( AGlobalRect& outRect ) const;
	void	SetWindowBounds( const ARect& inRect );
	void	SetWindowBounds( const AGlobalRect& inRect );
	void	SetWindowBounds( const AWindowRef inWindowRef, const ARect& inRect );
	void	SetWindowBounds( const AWindowRef inWindowRef, const AGlobalRect& inRect );
	ANumber	GetWindowWidth() const;
	void	SetWindowWidth( const ANumber inWidth );
	ANumber	GetWindowHeight() const;
	void	SetWindowHeight( const ANumber& inHeight );
	void					SetWindowSize( const ANumber inWidth, const ANumber inHeight );
	void	GetWindowPosition( APoint& outPoint ) const;
	void	SetWindowPosition( const APoint& inPoint );
	void					ConstrainWindowPosition( const ABool inAllowPartial );//#370
	void					ConstrainWindowSize();//#385
	ABool	IsWindowCollapsed() const;//B0395
	
	//コントロール制御
  public:
	void	DrawControl( const AControlID inID );
	
	//コントロール属性設定インターフェイス
  public:
	void	SetEnableControl( const AControlID inID, const ABool inEnable );
	ABool	IsControlEnabled( const AControlID inID ) const;
	void	SetShowControl( const AControlID inID, const ABool inShow );
	ABool	IsControlVisible( const AControlID inID ) const;
	void	SetTextStyle( const AControlID inID, const ABool inBold );
	void	SetTextFont( const AControlID inID, const AText& inFontName, const AFloatNumber inFontSize );
	void	SetTextJustification( const AControlID inControlID, const AJustification inJustification );
	void	SetCatchReturn( const AControlID inID, const ABool inCatchReturn );
	ABool	IsCatchReturn( const AControlID inID ) const;
	void	SetCatchTab( const AControlID inID, const ABool inCatchTab );
	ABool	IsCatchTab( const AControlID inID ) const;
	void	SetAutoSelectAllBySwitchFocus( const AControlID inID, ABool inAutoSelectAll );
	ABool	IsAutoSelectAllBySwitchFocus( const AControlID inID ) const;
	void	SetDataType( const AControlID inControlID, const ADataType inDataType );
	void	SetControlSize( const AControlID inID, const ANumber inWidth, const ANumber inHeight );
	void	SetHelpTag( const AControlID inID, const AText& inText, const AHelpTagSide inTagSide );//#643
	void	HideHelpTag() const;//R0246
	void	EnableHelpTagCallback( const AControlID inID, const ABool inShowOnlyWhenNarrow );//R0240 #507
	void	EmbedControl( const AControlID inParent, const AControlID inChild );
	void	SetControlZOrder( const AControlID inID, const ABool inFront );
	void					RegisterDropEnableControl( const AControlID inControlID );//#236
	void					SetControlZOrderToTop( const AControlID inControlID );//#291
	void					SetControlBindings( const AControlID inControlID,
							const ABool inLeft, const ABool inTop, const ABool inRight, const ABool inBottom );//#291
	void					DisableEventCatch( const AControlID inControlID );//win
	void					EnableFileDrop( const AControlID inControlID );//#565
  private:
	AArray<AControlID>					mEventCatchDisabledControlArray;//win
	AArray<AControlID>					mBindings_ControlID;
	ABoolArray							mBindings_Left;
	ABoolArray							mBindings_Top;
	ABoolArray							mBindings_Right;
	ABoolArray							mBindings_Bottom;
	
	//コントロール情報取得
  public:
	void	GetControlPosition( const AControlID inID, AWindowPoint& outPoint ) const;
	ANumber	GetControlWidth( const AControlID inID ) const;
	ANumber	GetControlHeight( const AControlID inID ) const;
	void	GetWindowPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AWindowPoint& inWindowPoint ) const;
	void	GetWindowRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AWindowRect& outWindowRect ) const;
	void	GetControlLocalPointFromWindowPoint( const AControlID inID, const AWindowPoint& inWindowPoint, ALocalPoint& outLocalPoint ) const;
	void	GetControlLocalRectFromWindowRect( const AControlID inID, const AWindowRect& inWindowRect, ALocalRect& outLocalRect ) const;
	void	GetGlobalPointFromControlLocalPoint( const AControlID inID, const ALocalPoint& inLocalPoint, AGlobalPoint& outGlobalPoint ) const;
	void	GetGlobalRectFromControlLocalRect( const AControlID inID, const ALocalRect& inLocalRect, AGlobalRect& outGlobalRect ) const;
	AControlID	FindControl( const AGlobalPoint& inGlobalPoint ) const;
	HWND	GetHWndByControlID( const AControlID inID ) const;
	AControlType	GetControlTypeByID( const AControlID inID ) const;
	AControlID	GetControlIDByHWnd( const HWND inHWnd ) const;
	AIndex	GetControlEmbeddedTabIndex( const AControlID inID ) const;
  private:
	void	GetComboListText( const AControlID inID, const AIndex inIndex, AText& outText ) const;
	
	//コントロール値のSet/Getインターフェイス
  public:
	void	GetText( const AControlID inID, AText& outText ) const;
	void	SetText( const AControlID inID, const AText& inText, const ABool inOnlyText = false );
	void	AddText( const AControlID inID, const AText& inText );
	void	GetBool( const AControlID inID, ABool& outBool ) const;
	ABool	GetBool( const AControlID inID ) const;
	void	SetBool( const AControlID inID, const ABool inBool );
	void	GetColor( const AControlID inID, AColor& outColor ) const;
	void	SetColor( const AControlID inID, const AColor& inColor );
	void	GetNumber( const AControlID inID, ANumber& outNumber ) const;
	ANumber	GetNumber( const AControlID inID ) const;
	void	GetFloatNumber( const AControlID inID, AFloatNumber& outNumber ) const;
	void	SetNumber( const AControlID inID, const ANumber inNumber );
	void	SetFloatNumber( const AControlID inID, const AFloatNumber inNumber );
	void	SetIcon( const AControlID inControlID, const AIconID inIconID, 
			const ABool inDefaultSize = false, const ANumber inLeftOffset = 4, const ANumber inTopOffset = 4 );
	void	SetMinMax( const AControlID inControlID, const ANumber inMin, const ANumber inMax );
	void	SetScrollBarPageSize( const AControlID inControlID, const ANumber inPageSize );
	
	//コントロール内選択インターフェイス
  public:
	void	SetSelection( const AControlID inID, const AIndex inStart, const AIndex inEnd );
	void	SetSelectionAll( const AControlID inID );
	
	//コントロール生成／削除
  public:
	void	CreateBevelButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
			const AText& inTitle, const ABool inEnableMenu );
	/* AView_Buttonを作ったため不要 void	CreateCustomButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, 
			const AText& inTitle, const ABool inEnableMenu, const AControlID inParentID, const ABool inToggle );*/
	//void	CreateIconButton( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIconID inIconID );
	void	CreateScrollBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, const AIndex inTabIndex = kIndex_Invalid );
	void	RegisterScrollBar( const AControlID inID );
	void	CreateStatusBar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight );
	void	CreateSizeBox( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight );
	void	CreateToolbar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight );
	void	CreateRebar( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight );
	void	DeleteControl( const AControlID inID );
  private:
	static AHashTextArray	sControlWindowClass_Name;
	static AArray<AControlType>	sControlWindowClass_Type;
	
	//タブView管理
  public:
	void	SetTabControlValue( const AIndex inTabIndex );//B0000
  private:
	AArray<HWND>	mTabControlArray;
	AIndex			mCurrentTabIndex;
	HWND			mTabControlHWnd;
	
	//フォーカス管理
  public:
	/*#135
	void	SwitchFocusTo( const AControlID inControlID );
	void	SwitchFocusToFirst();
	void	RegisterFocusGroup( const AControlID inControlID );
	*/
	AControlID		GetCurrentFocus() const;
	void	SetFocus( const AControlID inControlID );
	//#135 void	SwitchFocusToNext();
	void	ClearFocus( const ABool inAlsoClearLatentFocus = true );
  private:
	void	UpdateFocus();
	AControlID	mCurrentFocus;
	AControlID	mLatentFocus;
	//#135 AArray<AControlID>	mFocusGroup;
	
	//タブView管理
  private:
	void	UpdateTab();
	
	//UserPane管理
  public:
	/*#688
	void	RegisterTextEditPane( const AControlID inID, const ABool inWrapMode = false , 
	const ABool inVScrollbar = false, const ABool inHScrollBar = false, const ABool inHasFrame = true );
	*/
	void	ResetUndoStack( const AControlID inID );//B0323
	void	RegisterColorPickerPane( const AControlID inID );
	void	RegisterPaneForAView( const AControlID inID/*#135, const ABool inSupportFocus*/ );
	void	CreatePaneForAView( const AControlID inID, const AWindowPoint& inPoint, const ANumber inWidth, const ANumber inHeight, /*#360 const ABool inSupportFocus,*/
							const AIndex inTabIndex, const ABool inFront );
	void	CB_UserPaneValueChanged( const AControlID inID );
	void	CB_UserPaneTextInputted( const AControlID inID );
	CUserPane&	GetUserPane( const AControlID inControlID );
	const CUserPane&	GetUserPaneConst( const AControlID inControlID ) const;
  private:
	ABool	IsUserPane( const AControlID inControlID ) const;
	AUserPaneType	GetUserPaneType( const AControlID inControlID ) const;
	AObjectArray<CUserPane>				mUserPaneArray;
	AArray<AControlID>					mUserPaneArray_ControlID;
	AArray<AUserPaneType>				mUserPaneArray_Type;
	
	//テーブルView管理（Windowsは未対応）
  public:
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
	void					EndSetTable( const AControlID inControlID );
	//行追加
	void					BeginInsertRows( const AControlID inControlID );
	void					InsertRow_Text( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const AText& inText );
	void					InsertRow_Text_SwapContent( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, AText& ioText );
	void					InsertRow_Number( const AControlID inControlID, const APrefID inColumnID, const AIndex inIndex, const ANumber inNumber );
	void					EndInsertRows( const AControlID inControlID, const AIndex inRowIndex, const AIndex inRowCount );
	
	void					EnterColumnEditMode( const AControlID inControlID, const ADataID inColumnID );
	
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
	
	//ポップアップメニューボタン管理
  public:
	void					SetMenuItemsFromTextArray( const AControlID inID, const ATextArray& inTextArray );
	void					InsertMenuItem( const AControlID inID, const AIndex inMenuItemIndex, const AText& inText );//win
	void					DeleteMenuItem( const AControlID inID, const AIndex inMenuItemIndex );//win
	void					RegisterFontMenu( const AControlID inID, const ABool inEnableDefaultFontItem );//#375
	void					RegisterTextArrayMenu( const AControlID inControlID, const ATextArrayMenuID inTextArrayMenuID );
	void					UnregisterTextArrayMenu( const AControlID inControlID );
	/*#232
	static void				UpdateTextArrayMenu( const ATextArrayMenuID inTextArrayMenuID, const ATextArray& inTextArray );
	static void				RegisterTextArrayMenuID( const ATextArrayMenuID inTextArrayMenuID );
	*/
	static const ATextArray&	GetFontNameArray();
  private:
	static void				SetComboStringFromTextArray( const HWND inHWnd, const ATextArray& inTextArray );
	static int CALLBACK		STATIC_APICB_EnumFontFamExProc( ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam );
	AArray<AControlID>					mTextArrayMenuArray_ControlID;
	/*#232
	AArray<ATextArrayMenuID>			mTextArrayMenuArray_TextArrayMenuID;
	static AArray<HWND>					sTextArrayMenu_HWnd;
	static AArray<ATextArrayMenuID>		sTextArrayMenu_TextArrayMenuID;
	static ADataBase					sTextArrayMenuDB;
	*/
	static ATextArray					sFontNameArray;
	//#375
	AArray<AControlID>					mValueIndependentMenuArray_ControlID;
	AObjectArray< AHashTextArray >		mValueIndependentMenuArray_TitleArray;
	AObjectArray< AHashTextArray >		mValueIndependentMenuArray_ValueArray;
	
	/*#349
	//ラジオボタングループ
  public:
	void	RegisterRadioGroup();
	void	AddToLastRegisteredRadioGroup( const AControlID inID );
	void	UpdateRadioGroup( const AControlID inControlID );
	void	UpdateRadioButtonDisabled( const AControlID inControlID );
  private:
	AObjectArray< AArray<AControlID> >	mRadioGroup;
	*/
	//ファイル／フォルダ選択
  public:
	void					ShowChooseFolderWindow( const AFileAcc& inDefaultFolder,
							const AControlID inVirtualControlID, const ABool inSheet );//#551
	void	ShowChooseFileWindow( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inOnlyApp = false, 
			const ABool inSheet = true );
	void	ShowChooseFileWindowWithFileFilter( const AFileAcc& inDefaultFile, const AControlID inVirtualControlID, const ABool inSheet,
			const AFileAttribute inFilterAttribute );
	void					ShowAskSaveChangesWindow( const AText& inFileName, const AObjectID inDocumentID );//#216AControlID inVirtualControlID );
	void	CloseAskSaveChanges(){}
	void	ShowSaveWindow( const AText& inFileName, const ADocumentID inDocumentID, 
			const AFileAttribute inFileAttr, const AText& inFilter, const AText& inRefText, const AFileAcc& inDefaultFolder );
	void	ShowFileOpenWindow( const AText& inFilter, const ABool inShowInvisibleFile );
	
	//ウインドウ管理
  public:
/* #291によりAWindow_OK等へ変更
	void	ShowChildWindow_OK( const AText& inMessage1, const AText& inMessage2 );
	void	ShowChildWindow_OKCancel( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
			const AControlID inOKButtonVirtualControlID );
	void	ShowChildWindow_OKCancelDiscard( const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
			const AControlID inOKButtonVirtualControlID, const AControlID inDiscardButtonVirtualControlID );//#8
	void	CloseChildWindow();
	*/
	ABool	ChildWindowVisible() const;
	
	//背景
  public:
	static AIndex			RegisterBackground( const AFileAcc& inBackgroundImageFile );
	static HBITMAP			GetBackgoundBitmap( const AIndex inBackgroundImageIndex );
  private:
	static AObjectArray<AFileAcc>			sBackgroundImageArray_File;
	static AArray<HBITMAP>					sBackgroundImageArray_HBitmap;
	
	//LineBreak取得
  public:
	static AItemCount		GetLineBreak( CTextDrawData& ioTextDrawData, AItemCount inStartOffset, 
							const AText& inFontName, const AFloatNumber inFontSize, const ABool inAntiAlias,
							const ANumber inViewWidth );
	static void				InitLineBreakOffscreen();
  private:
	static HDC				sLineBreakOffscreenDC;
	static HBITMAP			sLineBreakOffscreenBitmap;
	static HFONT			sLineBreakHFont;
	static AText			sLineBreakHFont_FontName;
	static AFloatNumber		sLineBreakHFont_FontSize;
	static ABool			sLineBreakHFont_AntiAlias;
	
	//インライン入力
  public:
	void	FixInlineInput( const AControlID inControlID );
	ABool	GetInlineInputMode();//#448
	
	/*#539
	//help
  public:
	void					RegisterHelpAnchor( const ANumber inHelpNumber, AConstCstringPtr inAnchor );
	*/
	//アイコン登録
  public:
	static void				RegisterIcon( const AIconID inIconID, LPCTSTR inIconName, const ACstringPtr inName = "" );
	static AIconID			RegisterIconFromResouce( const AIndex inResID );
	static void				RegisterIconFromResouce( const AIconID inIconID, const AIndex inResID );
	//#1012 static AIconID			RegisterDynamicIconFromImageFile( const AFileAcc& inFile );//R0217
	static AIconID			RegisterDynamicIconFromFile( const AFileAcc& inIconFile, const ABool inLoadEvenIfNotCustomIcon );
	static void				UnregisterDynamicIcon( const AIconID inIconID );
	static void				CopyIconToFile( const AIconID inIconID, AFileAcc& inDestIconFile );//#232
	static void				GetIconFileSuffix( AText& outSuffix );//#232
	static AIconID			AssignDynamicIconID();
	static HICON			GetHIconByIconID( const AIconID inIconID, const ABool inGray = false );
	static ABool			IsDynamicIcon( const AIconID inIconID );
	static AIconID			GetIconIDByName( const AText& inName );
  private:
	static AArray<AIconID>		sIconArray_ID;
	static AArray<HICON>		sIconArray_HIcon;
	static AArray<HICON>		sIconArray_HIcon_Gray;
	static AIconID				sNextDynamicIconID;
	static AHashTextArray		sIconNameArray;
	static const AIconID		kDynamicIconIDStart = 10000;
	static const AIconID		kDynamicIconIDEnd = kNumber_MaxNumber;
	
	//フローティング管理 
  private:
	static AArray<HWND>	sFloatingWindowArray;

	//APIコールバック
  public:
	ABool	APICB_WndProc( UINT message, WPARAM wParam, LPARAM lParam, LRESULT& outResult );
	void	DoOwnerDraw( const AControlID inID, const LPDRAWITEMSTRUCT drawitem );
	
	//OverlayWindowリダイレクト
  public:
	ABool					RedirectToOverlayWindow( UINT message, WPARAM wParam, LPARAM lParam, const AGlobalPoint inGlobalPoint );
	ABool					DoOverlayWindowRedirect( UINT message, WPARAM wParam, LPARAM lParam, const AGlobalPoint inGlobalPoint );
	
	//APIコールバック(static)
  private:
	static LRESULT CALLBACK STATIC_APICB_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK STATIC_APICB_WndProcOverlayBackground( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	
  public:
	HWND	GetHWnd() const { return mHWnd; }
  private:
	HWND	mHWnd;
	HMENU	mEachMenu;
	
	//
  public:
	static HFONT	CreateFont_( const HDC inHDC, const AText& inFontName, const AFloatNumber inFontSize,
					ATextStyle inStyle, const ABool inAntiAlias );
	
  private:
	static AArray<CWindowImp*>	sAliveWindowArray;
	static AArray<HWND>	sAliveWindowArray_HWnd;
	static AArray<HWND>	sAliveWindowArray_HWnd_OverlayBackgroundWindow;
	
	/*#688
	//#353
  public:
	static ABool			STATIC_CheckIsFocusGroup2ndPattern() { return true; }//Windowsの場合は常にボタン等もタブストップ対象
	static ABool			STATIC_GetIsFocusGroup2ndPattern() { return true; }//Windowsの場合は常にボタン等もタブストップ対象
	*/
	
	//
  private:
	void					ApplyBindings( const AGlobalRect& prev, const AGlobalRect& current );
	
	//ユーティリティ
	
	//画面サイズ取得
  public:
	static void				GetMainDeviceScreenBounds( AGlobalRect& outBounds );//B0000
	static void				GetAvailableWindowPositioningBounds( AGlobalRect& outRect );
	
	//ウインドウ取得
  public:
	static AWindowRef		GetMostFrontWindow();
	static AWindowRef		GetNextOrderWindow( const AWindowRef inWindowRef );
	static AWindowRef		GetNthWindow( const ANumber inNumber );
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "CWindowImp"; }
};

