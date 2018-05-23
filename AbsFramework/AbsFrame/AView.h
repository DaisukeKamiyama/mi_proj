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

AView

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "../Wrapper.h"
#include "../Imp.h"

class CWindowImp;
class CPaneForAView;
class AWindow;
class AView_Frame;

#pragma mark ===========================
#pragma mark [クラス]AView
/**
Viewの基底クラス
*/
class AView : public AObjectArrayItem
{
  public:
	//コンストラクタ、デストラクタ
  public:
	AView( const AWindowID inWindowID, const AControlID inID, const AImagePoint& inOriginOfLocalFrame = kImagePoint_00 );
	virtual ~AView();
	void					NVI_Init() { NVIDO_Init(); } //win 080707
	AControlID				NVI_GetControlID() const {return mControlID;}
	void					NVI_SetControlID( const AControlID inControlID ) { mControlID = inControlID; }//#92
	void					DoDeleted();//#92
  private:
	virtual void			NVIDO_Init() = 0;//win 080707 #92 派生クラスで必須にするよう変更
	virtual void			NVIDO_DoDeleted() = 0;//#92 派生クラスで必須にするよう変更
	//#199 AWindow&							mWindow;
	AWindowID							mWindowID;//#199
	AControlID							mControlID;
	CPaneForAView&						mViewImp;//#271
	AWindow&							mWindow;//#271 高速化のためWindowオブジェクトの参照を保持する
	
	//<関連オブジェクト取得>
  public:
	AWindowID				NVI_GetWindowID() const { return mWindowID; }
	AWindow&				NVI_GetWindow() {return mWindow;}//#699
  protected:
	AWindow&				NVM_GetWindow() {return mWindow;}//#271
	const AWindow&			NVM_GetWindowConst() const {return mWindow;}//#271
	AWindowID				NVM_GetWindowID() const { return mWindowID; }
	
	//<イベント処理>
  public:
	ABool					EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	//#390 { return EVTDO_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys); }
	void					EVT_UpdateMenu();
	//#390 { EVTDO_UpdateMenu(); }
	ABool					EVT_DoInlineInput( const AText& inText, const AItemCount inFixLen,
							const AArray<AIndex>& inHiliteLineStyleIndex, 
							const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
							const AItemCount inReplaceLengthInUTF16,//#688
							ABool& outUpdateMenu )
	{ return EVTDO_DoInlineInput(inText,inFixLen,inHiliteLineStyleIndex,inHiliteStartPos,inHiliteEndPos,inReplaceLengthInUTF16,outUpdateMenu); }//#688
	ABool					EVT_DoInlineInputOffsetToPos( const AIndex inOffset, ALocalPoint& outPos )
	{ return EVTDO_DoInlineInputOffsetToPos(inOffset,outPos); }
	ABool					EVT_DoInlineInputPosToOffset( const ALocalPoint& inPos, AIndex& outOffset )
	{ return EVTDO_DoInlineInputPosToOffset(inPos,outOffset); }
	ABool					EVT_DoInlineInputGetSelectedText( AText& outText )
	{ return EVTDO_DoInlineInputGetSelectedText(outText); }
	ABool					EVT_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction, 
							ABool& outUpdateMenu )
	{ return EVTDO_DoTextInput(inText,/*#688 inOSKeyEvent*/inKeyBindKey,inModifierKeys,inKeyBindAction,outUpdateMenu); }
	ABool					EVT_DoKeyDown( /*#688 const AUChar inChar*/ const AText& inText, const AModifierKeys inModifierKeys )//#1080
	{ return EVTDO_DoKeyDown(/*#688 inChar*/inText,inModifierKeys); }
	ABool					EVT_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
	{ return EVTDO_DoMouseDown(inLocalPoint,inModifierKeys,inClickCount); }
	ABool					EVT_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
	{ return EVTDO_DoContextMenu(inLocalPoint,inModifierKeys,inClickCount); }
	ACursorType				EVT_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
	{ return EVTDO_GetCursorType(inLocalPoint,inModifierKeys); }
	ABool					EVT_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
	{ return EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys); }
	void					EVT_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
	{
		if( mMouseTrackingMode == false )   return;//#688
		return EVTDO_DoMouseTracking(inLocalPoint,inModifierKeys);
	}
	void					EVT_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
	{
		if( mMouseTrackingMode == false )   return;//#688
		return EVTDO_DoMouseTrackEnd(inLocalPoint,inModifierKeys);
	}
	void					EVT_QuickLook( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) //#1026
	{
		EVTDO_QuickLook(inLocalPoint,inModifierKeys);
	}
	void					EVT_DoDraw();//win
	void					EVT_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
							const ALocalPoint inLocalPoint );//win 080706
	//#138 mousewheelイベント転送 { EVTDO_DoMouseWheel(inDeltaX,inDeltaY,inModifierKeys,inLocalPoint); }
	void					EVT_DoControlBoundsChanged();
	void					EVT_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
							ABool& outIsCopyOperation )
			{ EVTDO_DoDragTracking(inDragRef,inLocalPoint,inModifierKeys,outIsCopyOperation); }
	void					EVT_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
			{ EVTDO_DoDragEnter(inDragRef,inLocalPoint,inModifierKeys); }
	void					EVT_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
			{ EVTDO_DoDragLeave(inDragRef,inLocalPoint,inModifierKeys); }
	void					EVT_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
			{ EVTDO_DoDragReceive(inDragRef,inLocalPoint,inModifierKeys); }
	void					EVT_DoScrollBarAction( const ABool inV, const AScrollBarPart inPart );
	void					EVT_DoTickTimer() {EVTDO_DoTickTimer();}
	void					EVT_DrawPreProcess() {EVTDO_DrawPreProcess();}//win 080712
	void					EVT_DrawPostProcess() {EVTDO_DrawPostProcess();}//win 080712
	void					EVT_PreProcess_SetShowControl( const ABool inVisible ) { EVTDO_PreProcess_SetShowControl(inVisible); }//#138
	void					EVT_DoMouseLeft( const ALocalPoint& inLocalPoint )//win 080712
	{ return EVTDO_DoMouseLeft(inLocalPoint); }
	ABool					EVT_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide )//R0240 #643
	{ return EVTDO_DoGetHelpTag(inPoint,outText1,outText2,outRect,outTagSide); }
	
  private:
	virtual ABool			EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys ) { return false; }
	virtual void			EVTDO_UpdateMenu() {}
	virtual ABool			EVTDO_DoInlineInput( const AText& inText, const AItemCount inFixLen,
							const AArray<AIndex>& inHiliteLineStyleIndex, 
							const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
							const AItemCount inReplaceLengthInUTF16,//#688
							ABool& outUpdateMenu ) { return false; }
	virtual ABool			EVTDO_DoInlineInputOffsetToPos( const AIndex inOffset, ALocalPoint& outPos ) { return false; }
	virtual ABool			EVTDO_DoInlineInputPosToOffset( const ALocalPoint& inPos, AIndex& outOffset ) { return false; }
	virtual ABool			EVTDO_DoInlineInputGetSelectedText( AText& outText ) { return false; }
	virtual ABool			EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
							const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
							ABool& outUpdateMenu ) = 0;
	virtual ABool			EVTDO_DoKeyDown( /*#688 const AUChar inChar*/ const AText& inText, const AModifierKeys inModifierKeys ) { return false; }//#1080
	virtual ABool			EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount ) = 0;
	virtual ABool			EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount ) { return false;}
	virtual ABool			EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) { return false; }
	virtual ACursorType		EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) { return mDefaultCursor; }//#725kCursorType_Arrow; }
	virtual void			EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) { }
	virtual void			EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) { }
	virtual void			EVTDO_QuickLook( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) { } //#1026
	virtual void			EVTDO_DoDraw() = 0;
	virtual void			EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys, const ALocalPoint inLocalPoint ) = 0;//win 080706
	virtual void			EVTDO_DoControlBoundsChanged() {}
	virtual void			EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
							ABool& outIsCopyOperation ) {}
	virtual void			EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) {}
	virtual void			EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) {}
	virtual void			EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys ) {}
	virtual void			EVTDO_DoTickTimer() {}
	virtual void			EVTDO_DrawPreProcess() {}//win
	virtual void			EVTDO_DrawPostProcess() {}//win
	virtual void			EVTDO_PreProcess_SetShowControl( const ABool inVisible ) {}//#138
	virtual void			EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint ) {}//win 080712
	virtual ABool			EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) { return false; }//R0240 #643
	
	//<インターフェイス>
	
	//View情報取得
  public:
	AViewType				NVI_GetViewType() const { return NVIDO_GetViewType(); }
  private:
	virtual AViewType		NVIDO_GetViewType() const { return kViewType_Invalid; }
	
	//表示制御 #137
  public:
	void					NVI_SetShow( const ABool inShow );
	void					NVI_SetEnabled( const ABool inEnable );
	ABool					NVI_GetEnabled() const { return NVMC_IsControlEnabled(); }
  private:
	virtual void			NVIDO_SetShow( const ABool inShow ) {}
	virtual void			NVIDO_SetEnabled( const ABool inEnable ) {}
	
	//スクロール
  public:
	void					NVI_Scroll( const ANumber inDeltaX, const ANumber inDeltaY, const ABool inRedraw = true,
										const ABool inConstrainToImageSize = true,
										const AScrollTrigger inScrollTrigger = kScrollTrigger_General );//#1031
	void					NVI_ScrollTo( const AImagePoint& inNewOrigin, const ABool inRedraw = true,
										  const ABool inConstrainToImageSize = true,
										  const AScrollTrigger inScrollTrigger = kScrollTrigger_General );//#1031
	void					NVI_ConstrainScrollToImageSize( const ABool inRedraw );
	void					NVI_SetScrollBarControlID( const AControlID inHScrollBar, const AControlID inVScrollBar );
	void					NVI_SetScrollBarUnit( const ANumber inHScrollUnit, const ANumber inVScrollUnit, 
							const ANumber inHScrollPageUnit, const ANumber inVScrollPageUnit );
	AControlID				NVI_GetVScrollBarControlID() const {return mControlID_VScrollBar;}
	AControlID				NVI_GetHScrollBarControlID() const {return mControlID_HScrollBar;}
	ANumber					NVI_GetVScrollBarUnit() const {return mVScrollBarUnit;}
	ANumber					NVI_GetHScrollBarUnit() const {return mHScrollBarUnit;}
	ANumber					NVI_GetVScrollBarPageUnit() const {return mVScrollBarPageUnit;}
	ANumber					NVI_GetHScrollBarPageUnit() const {return mHScrollBarPageUnit;}
  private:
	void					ScrollCore( const ANumber inDeltaX, const ANumber inDeltaY, 
										const ABool inRedraw, const ABool inConstrainToImageSize,
										const AScrollTrigger inScrollTrigger = kScrollTrigger_General );//#1031
	virtual void			NVIDO_ScrollPreProcess( const ANumber inDeltaX, const ANumber inDeltaY, 
													const ABool inRedraw, const ABool inConstrainToImageSize,
													const AScrollTrigger inScrollTrigger ) {}//#138 #1031
	virtual void			NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY, 
													 const ABool inRedraw, const ABool inConstrainToImageSize,
													 const AScrollTrigger inScrollTrigger ) {}//#138 #1031
	AControlID							mControlID_HScrollBar;
	AControlID							mControlID_VScrollBar;
	ANumber								mHScrollBarUnit;
	ANumber								mVScrollBarUnit;
	ANumber								mHScrollBarPageUnit;
	ANumber								mVScrollBarPageUnit;
	
	//フォーカス制御
  public:
	void					EVT_DoViewFocusActivated();
	void					EVT_DoViewFocusDeactivated();
	ABool					NVI_IsFocusActive() const;//#688 { return mFocusActive; }
	ABool					NVI_IsLatentFocus() const;//#312
	ABool					NVI_IsFocusInTab() const;//#312
  private:
	virtual void			EVTDO_DoViewFocusActivated() {}
	virtual void			EVTDO_DoViewFocusDeactivated() {}
	//#688 ABool					mFocusActive;
	
	//位置・サイズ
  public:
	void					NVI_SetPosition( const AWindowPoint& inWindowPoint );
	void					NVI_SetSize( const ANumber inWidth, const ANumber inHeight );
	AWindowPoint			NVI_GetPosition() const;
	ANumber					NVI_GetWidth() const;
	ANumber					NVI_GetHeight() const;
	void					NVI_SetControlBindings( const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
							const ABool inWidthFixed, const ABool inHeightFixed );//#455 #688
  private:
	virtual void			NVIDO_SetPosition( const AWindowPoint& inWindowPoint ) {}
	virtual void			NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight ) {}
	
	//値設定／取得
  public:
	void					NVI_SetText( const AText& inText ) { NVIDO_SetText(inText); }
	void					NVI_AddText( const AText& inText ) { NVIDO_AddText(inText); }//#182
	void					NVI_GetText( AText& outText ) const { NVIDO_GetText(outText); }
	void					NVI_SetNumber( const ANumber inNumber ) { NVIDO_SetNumber(inNumber); }//#182
	ANumber					NVI_GetNumber() const { return NVIDO_GetNumber(); }//#182
	void					NVI_SetBool( const ABool inBool ) { NVIDO_SetBool(inBool); }//#182
	ABool					NVI_GetBool() const { return NVIDO_GetBool(); }//#182
	void					NVI_SetColor( const AColor& inColor ) { return NVIDO_SetColor(inColor); }//#182
	void					NVI_GetColor( AColor& outColor ) const { return NVIDO_GetColor(outColor); }//#182
	void					NVI_SetFloatNumber( const AFloatNumber inNumber ) { NVIDO_SetFloatNumber(inNumber); }//#182
	AFloatNumber			NVI_GetFloatNumber() const { return NVIDO_GetFloatNumber(); }//#182
	void					NVI_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize ) { NVIDO_SetTextFont(inFontName,inFontSize); }//#182
	void					NVI_SetTextProperty( const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification )
	{ NVIDO_SetTextProperty(inColor,inStyle,inJustification); }//#725
  private:
	virtual void			NVIDO_SetText( const AText& inText ) { _ACError("",this); }
	virtual void			NVIDO_AddText( const AText& inText ) { _ACError("",this); }//#182
	virtual void			NVIDO_GetText( AText& outText ) const { _ACError("",this); }
	virtual void			NVIDO_SetNumber( const ANumber inNumber ) { _ACError("",this); }//#182
	virtual ANumber			NVIDO_GetNumber() const { _ACError("",this); return 0; }//#182
	virtual void			NVIDO_SetBool( const ABool inBool ) { _ACError("",this); }//#182
	virtual ABool			NVIDO_GetBool() const { _ACError("",this); return false; }//#182
	virtual void			NVIDO_SetColor( const AColor& inColor ) { _ACError("",this); }//#182
	virtual void			NVIDO_GetColor( AColor& outColor ) const { _ACError("",this); }//#182
	virtual void			NVIDO_SetFloatNumber( const AFloatNumber inNumber ) { _ACError("",this); }//#182
	virtual AFloatNumber	NVIDO_GetFloatNumber() const { _ACError("",this); return 0.0; }//#182
	virtual void			NVIDO_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize ) { _ACError("",this); }//#182
	virtual void			NVIDO_SetTextProperty( const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification ) {_ACError("",this); }//#725
	//
  public:
	void					NVI_Refresh();
	
	ABool					NVI_IsVisible() const;
	
	void					NVI_RevealView( const ABool inSelectWindow = true, const ABool inSwitchFocus = true );//#92 #291 #492
	
	//#92
  public:
	ABool					NVI_IsBorrowerView() const { return mBorrowerView; }
	void					NVI_SetBorrowerView( const ABool inBorrowerView ) { mBorrowerView = inBorrowerView; }
  private:
	ABool								mBorrowerView;
	
	//#92
  public:
	void					NVI_TryCloseWindow();
	void					NVI_ExecuteCloseWindow();
	void					NVI_SetInhibitCloseWindow( const ABool inInhibit );
	
	//#92
  public:
	void					NVI_UpdateWindowTitleBar();
	
	//#135
  public:
	void					NVI_SetSelectAll() {NVIDO_SetSelectAll();}
	void					NVI_AutomaticSelectBySwitchFocus() 
	{
		if( mAutomaticSelectBySwitchFocus == true )   NVI_SetSelectAll();
	}
	void					NVI_SetAutomaticSelectBySwitchFocus( const ABool inEnable ) { mAutomaticSelectBySwitchFocus = inEnable; }
	ABool					NVI_GetAutomaticSelectBySwitchFocus() const { return mAutomaticSelectBySwitchFocus; }
  private:
	virtual void			NVIDO_SetSelectAll() {}
	ABool								mAutomaticSelectBySwitchFocus;
	
	//#688
  public:
	ABool					NVI_IsInLiveResize() const;
	
	//Undo情報を削除 #135
  public:
	void					NVI_ClearUndoInfo() { NVIDO_ClearUndoInfo(); }
  private:
	virtual void			NVIDO_ClearUndoInfo() {}
	
	//リターン・タブコードをCatchするかどうか #137
  public:
	void					NVI_SetCatchReturn( const ABool inCatchReturn ) { mCatchReturn = inCatchReturn; }
	ABool					NVI_GetCatchReturn() const { return mCatchReturn; }
	void					NVI_SetCatchTab( const ABool inCatchTab ) { mCatchTab = inCatchTab; }
	ABool					NVI_GetCatchTab() const { return mCatchTab; }
  private:
	ABool								mCatchReturn;
	ABool								mCatchTab;
	
	//バックスラッシュ入力モード #137 #182
  public:
	void					NVI_SetInputBackslashByYenKeyMode( const ABool inMode ) { mInputBackslashByYenKeyMode = inMode; }
	ABool					NVI_GetInputBackslashByYenKeyMode() const { return mInputBackslashByYenKeyMode; }
  private:
	ABool								mInputBackslashByYenKeyMode;
	
	//
  public:
	void					NVI_SetDataType( const ADataType inDataType ) { mDataType = inDataType; } 
	ADataType				NVI_GetDataType() const { return mDataType; }
  private:
	ADataType							mDataType;
	
	//<汎化メソッド>
	
	//Imageサイズ
  public:
	ANumber					NVI_GetImageWidth() const {return mImageWidth;}
	ANumber					NVI_GetImageHeight() const {return mImageHeight;}
	void					NVI_UpdateScrollBar();//#519
	void					NVI_SetImageYMargin( const ANumber inMinMargin, const ANumber inMaxMargin )
	{ mImageYMinMargin = inMinMargin; mImageYMaxMargin = inMaxMargin; }//#621
	ANumber					NVI_GetImageYMaxMargin() const { return mImageYMaxMargin; }
	ANumber					NVI_GetImageYMinMargin() const { return mImageYMinMargin; }
  //protected:
	void					NVM_SetImageSize( const ANumber inWidth, const ANumber inHeight );
	ANumber					NVM_GetImageWidth() const {return mImageWidth;}
	ANumber					NVM_GetImageHeight() const {return mImageHeight;}
  protected:
	ANumber								mImageWidth;
	ANumber								mImageHeight;
	ANumber								mImageYMinMargin;//#621
	ANumber								mImageYMaxMargin;//#621
	
	//座標系変換
  protected:
	void					NVM_GetImagePointFromLocalPoint( const ALocalPoint& inLocalPoint, AImagePoint& outImagePoint ) const;
	void					NVM_GetImageRectFromLocalRect( const ALocalRect& inLocalRect, AImageRect& outImageRect ) const;
	void					NVM_GetLocalPointFromImagePoint( const AImagePoint& inImagePoint, ALocalPoint& outLocalPoint ) const;
	void					NVM_GetLocalRectFromImageRect( const AImageRect& inImageRect, ALocalRect& outLocalRect ) const;
	
	//ViewRect情報取得
	//
  public:
	void					NVI_GetOriginOfLocalFrame( AImagePoint& outImagePoint ) const {outImagePoint = mOriginOfLocalFrame;}
	void					NVI_SetOriginOfLocalFrame( const AImagePoint& inImagePoint ) 
	{ mOriginOfLocalFrame = inImagePoint; }//#532
	ANumber					NVI_GetViewWidth() const;
	ANumber					NVI_GetViewHeight() const;
	void					NVI_GetGlobalViewRect( AGlobalRect& outGlobalRect ) const;//#717
	void					NVI_GetImageViewRect( AImageRect& outImageRect ) const
	{ NVM_GetImageViewRect(outImageRect); }
  protected:
	void					NVM_GetLocalViewRect( ALocalRect& outLocalRect ) const;
	void					NVM_GetImageViewRect( AImageRect& outImageRect ) const;
	ABool					NVM_IsImagePointInViewRect( const AImagePoint& inImagePoint ) const;
  private:
	AImagePoint							mOriginOfLocalFrame;
	
	//マウスクリックTrackingモード設定 #688
  public:
	ABool					NVI_GetMouseTrackingMode() { return mMouseTrackingMode; }
	void					NVM_SetMouseTrackingMode( const ABool inSet );
  private:
	ABool								mMouseTrackingMode;
	
	//Control表示 #182
  protected:
	void					NVMC_SetShowControl( const ABool inShow )
	{ mViewImp.SetShowControl(inShow); }
	void					NVMC_SetEnable( const ABool inEnable )
	{ mViewImp.SetEnableControl(inEnable); }
	//Control情報取得 #182
  protected:
	ABool					NVMC_IsControlVisible() const
	{ return mViewImp.IsControlVisible(); }
	ABool					NVMC_IsControlEnabled() const
	{ return mViewImp.IsControlEnabled(); }
	//Control属性設定 #182
  public:
	void					NVI_SetDefaultCursor( const ACursorType inCursorType )
	{ mDefaultCursor = inCursorType; mViewImp.SetDefaultCursor(inCursorType); }
  private:
	ACursorType							mDefaultCursor;
  protected:
	/*#725
	void					NVMC_SetDefaultCursor( const ACursorType inCursorType )
	{ mViewImp.SetDefaultCursor(inCursorType); }
	*/
	void					NVMC_EnableMouseLeaveEvent()
	{ mViewImp.EnableMouseLeaveEvent(); }
	void					NVMC_SetControlSize( const ANumber inWidth, const ANumber inHeight )
	{ mViewImp.SetControlSize(inWidth,inHeight); }
	void					NVMC_SetControlPosition( const AWindowPoint& inWindowPoint )
	{ mViewImp.SetControlPosition(inWindowPoint); }
	void					NVMC_SetControlBindings( 
													const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
													const ABool inWidthFixed, const ABool inHeightFixed )//#688
	{ mViewImp.SetControlBindings(inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed); }//#455
	//左下が丸いのを考慮したスクロールをするかどうかを設定 #947
  public:
	void					NVI_SetAdjustScrollForRoundedCorner( const ABool inAdjust )
	{ mViewImp.SetAdjustScrollForRoundedCorner(inAdjust); }
	//描画属性設定 #182
  protected:
	void					NVMC_SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, 
							const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing, const AFloatNumber inAlpha = 1.0  )
	{ mViewImp.SetDefaultTextProperty(inFontName,inFontSize,inColor,inStyle,inAntiAliasing,inAlpha); }
	void					NVMC_SetDefaultTextProperty( const AColor& inColor, const ATextStyle inStyle, const AFloatNumber inAlpha = 1.0 )
	{ mViewImp.SetDefaultTextProperty(inColor,inStyle,inAlpha); }
	//描画 #182
  protected:
	void					NVMC_DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification = kJustification_Left )
	{ mViewImp.DrawText(inDrawRect,inText,inJustification); }
	void					NVMC_DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification = kJustification_Left )
	{ mViewImp.DrawText(inDrawRect,inClipRect,inText,inJustification); }
	void					NVMC_DrawText( const ALocalRect& inDrawRect, 
							const AText& inText, const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification = kJustification_Left )
	{ mViewImp.DrawText(inDrawRect,inText,inColor,inStyle,inJustification); }
	void					NVMC_DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
							const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification = kJustification_Left )
	{ mViewImp.DrawText(inDrawRect,inClipRect,inText,inColor,inStyle,inJustification); }
	void					NVMC_DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification = kJustification_Left )
	{ mViewImp.DrawText(inDrawRect,inTextDrawData,inJustification); }
	void					NVMC_DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, 
							const AJustification inJustification = kJustification_Left )
	{ mViewImp.DrawText(inDrawRect,inClipRect,inTextDrawData,inJustification); }
	/*#1012
	void					NVMC_DrawIcon( const ALocalRect& inRect, const AIconID inIconID, const ABool inEnabledColor, const ABool inDefaultSize = false ) const
	{ mViewImp.DrawIcon(inRect,inIconID,inEnabledColor,inDefaultSize); }
	*/
	void					NVMC_DrawIconFromFile( const ALocalRect& inRect, const AFileAcc& inFile, const ABool inEnabledColor ) const
	{ mViewImp.DrawIconFromFile(inRect,inFile,inEnabledColor); }
	void					NVMC_PaintPoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha )
	{ mViewImp.PaintPoly(inPath,inColor,inAlpha); }//#379
	void					NVMC_PaintRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha = 1.0,
							const ABool inDropShadow = false )
	{ mViewImp.PaintRect(inRect,inColor,inAlpha,inDropShadow); }
	void					NVMC_PaintRectWithVerticalGradient( const ALocalRect& inRect, 
							const AColor& inTopColor, const AColor& inBottomColor, 
							const AFloatNumber inTopAlpha, const AFloatNumber inBottomAlpha )
	{ mViewImp.PaintRectWithVerticalGradient(inRect,inTopColor,inBottomColor,inTopAlpha,inBottomAlpha); }//#634
	void					NVMC_PaintRectWithHorizontalGradient( const ALocalRect& inRect, 
							const AColor& inTopColor, const AColor& inBottomColor, const AFloatNumber inLeftAlpha, const AFloatNumber inRightAlpha )
	{ mViewImp.PaintRectWithHorizontalGradient(inRect,inTopColor,inBottomColor,inLeftAlpha,inRightAlpha); }//#634
	void					NVMC_EraseRect( const ALocalRect& inRect )//win
	{ mViewImp.EraseRect(inRect); }
	void					NVMC_FrameRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha = 1.0,
										   const ABool inDrawLeftLine = true, const ABool inDrawTopLine = true,
										   const ABool inDrawRightLine = true, const ABool inDrawBottomLine = true,
										   const AFloatNumber inLineWidth = 1.0 )
	{ mViewImp.FrameRect(inRect,inColor,inAlpha,inDrawLeftLine,inDrawTopLine,inDrawRightLine,inDrawBottomLine,inLineWidth); }
	void					NVMC_PaintRoundedRect( const ALocalRect& inRect, 
												  const AColor& inStartColor, const AColor& inEndColor, 
												  const AGradientType inGradientType, 
												  const AFloatNumber inStartAlpha, const AFloatNumber inEndAlpha,
												  const ANumber inR, 
												  const ABool inLeftTopSide, const ABool inRightTopSide, 
												  const ABool inLeftBottomSide, const ABool inRightBottomSide )
	{ mViewImp.PaintRoundedRect(inRect,inStartColor,inEndColor,inGradientType,inStartAlpha,inEndAlpha,
	inR,inLeftTopSide,inRightTopSide,inLeftBottomSide,inRightBottomSide); }//#724
	void					NVMC_FrameRoundedRect( const ALocalRect& inRect, 
												  const AColor& inColor, const AFloatNumber inAlpha,
												  const ANumber inR, 
												  const ABool inLeftTopSide, const ABool inRightTopSide, 
												  const ABool inLeftBottomSide, const ABool inRightBottomSide,
												  const ABool inDrawLeftLine = true, const ABool inDrawTopLine = true,
												  const ABool inDrawRightLine = true, const ABool inDrawBottomLine = true,
												  const AFloatNumber inLineWidth = 0.2 )
	{ mViewImp.FrameRoundedRect(inRect,inColor,inAlpha,inR,inLeftTopSide,inRightTopSide,inLeftBottomSide,inRightBottomSide,
		inDrawLeftLine,inDrawTopLine,inDrawRightLine,inDrawBottomLine,inLineWidth); }//#724
	void					NVMC_DrawCircle( const ALocalRect& inDrawRect, const AColor& inColor, const ANumber inLineWidth = 1.0, const AFloatNumber inAlpha = 1.0 )//#871
	{ mViewImp.DrawCircle(inDrawRect,inColor,inLineWidth,inAlpha); }
	void					NVMC_DrawFocusFrame( const ALocalRect& inRect )//win
	{ mViewImp.DrawFocusFrame(inRect); }
	void					NVMC_DrawBackgroundImage( const ALocalRect& inDrawRect, const AImageRect& inImageFrameRect, 
							const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha )
	{ mViewImp.DrawBackgroundImage(inDrawRect,inImageFrameRect,inLeftOffset,inBackgroundImageIndex,inAlpha); }
	void					NVMC_DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
							const AFloatNumber inAlpha = 1.0, const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0,
							const ABool inDropShadow = false )
	{ mViewImp.DrawLine(inStartPoint,inEndPoint,inColor,inAlpha,inLineDash,inLineWidth,inDropShadow); }
	void					NVMC_DrawCurvedLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
							const AFloatNumber inAlpha = 1.0, const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0 )
	{ mViewImp.DrawCurvedLine(inStartPoint,inEndPoint,inColor,inAlpha,inLineDash,inLineWidth); }//#723
	void					NVMC_SetDrawLineMode( const AColor& inColor, const AFloatNumber inAlpha = 1.0, 
							const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0 )
	{ mViewImp.SetDrawLineMode(inColor,inAlpha,inLineDash,inLineWidth); }
	void					NVMC_RestoreDrawLineMode()
	{ mViewImp.RestoreDrawLineMode(); }
	void					NVMC_DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint )
	{ mViewImp.DrawLine(inStartPoint,inEndPoint); }
	void					NVMC_DrawXorCaretLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const ABool inClipToFrame, 
												   const ABool inFlush, const ABool inDash = false, const ANumber inPenSize = 1 )//#1034
	{ mViewImp.DrawXorCaretLine(inStartPoint,inEndPoint,inClipToFrame,inFlush,inDash,inPenSize); }
	void					NVMC_DrawXorCaretRect( const ALocalRect& inLocalRect, const ABool inClipToFrame, const ABool inFlush )//#1034
	{ mViewImp.DrawXorCaretRect(inLocalRect,inClipToFrame,inFlush); }
	void					NVMC_TransferOffscreen()
	{ mViewImp.TransferOffscreen(); }
	void					NVMC_RedrawImmediately()
	{ mViewImp.RedrawImmediately(); }
	void					NVMC_RefreshRect( const ALocalRect& inLocalRect )
	{ mViewImp.RefreshRect(inLocalRect); mRefreshing = true; }//#1034
	void					NVMC_FlushRect( const ALocalRect& inLocalRect )
	{ mViewImp.FlushRect(inLocalRect); }
	void					NVMC_RefreshControl()
	{ mViewImp.RefreshControl(); mRefreshing = true; }//#1034
	void					NVMC_FlushControl()//win
	{ mViewImp.FlushControl(); }
	void					NVMC_SetDropShadowColor( const AColor& inColor )
	{ mViewImp.SetDropShadowColor(inColor); }//#725
  protected:
	ABool								mRefreshing;//#1034
	//描画用情報取得 #182
  protected:
	ANumber					NVMC_GetImageXByTextPosition( CTextDrawData& inTextDrawData, const AIndex inTextPosition )
	{ return mViewImp.GetImageXByTextPosition(inTextDrawData,inTextPosition); }
	ANumber					NVMC_GetImageXByUnicodeOffset( CTextDrawData& inTextDrawData, const AIndex inUnicodeOffset )
	{ return mViewImp.GetImageXByUnicodeOffset(inTextDrawData,inUnicodeOffset); }
	AIndex					NVMC_GetTextPositionByImageX( CTextDrawData& inTextDrawData, const ANumber inImageX )
	{ return mViewImp.GetTextPositionByImageX(inTextDrawData,inImageX); }
	AFloatNumber			NVMC_GetDrawTextWidth( const AText& inText ) const
	{ return mViewImp.GetDrawTextWidth(inText); }
	AFloatNumber			NVMC_GetDrawTextWidth( CTextDrawData& inTextDrawData ) const
	{ return mViewImp.GetDrawTextWidth(inTextDrawData); }
	ABool					NVMC_IsRectInDrawUpdateRegion( const ALocalRect& inLocalRect ) const
	{ return mViewImp.IsRectInDrawUpdateRegion(inLocalRect); }
	ALocalRect				NVMC_GetDrawUpdateRect() const
	{ return mViewImp.GetDrawUpdateRect(); }
	void					NVMC_GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) const
	{ mViewImp.GetMetricsForDefaultTextProperty(outLineHeight,outLineAscent); }
	//スクロール #182
  protected:
	void					NVMC_Scroll( const ANumber inDeltaX, const ANumber inDeltaY )
	{ mViewImp.Scroll(inDeltaX,inDeltaY); }
	//#688
	//Draw処理を今すぐ実行する（OS経由でEVT_DoDraw()が今すぐ実行される）
  protected:
	void					NVMC_ExecuteDoDrawImmediately()
	{ mViewImp.ExecuteDoDrawImmediately(); }
	//Drag #182
  protected:
	void					NVMC_EnableDrop( const AArray<AScrapType>& inScrapType )
	{ mViewImp.EnableDrop(inScrapType); }
	ABool					NVMC_DragText( const ALocalPoint& inMousePoint, 
							const AArray<ALocalRect>& inDragRect, const AArray<AScrapType>& inScrapType, const ATextArray& inData,
							const AImageID inImageID = kImageID_Invalid )
	{ return mViewImp.DragText(inMousePoint,inDragRect,inScrapType,inData,inImageID); }
	ABool					MVMC_DragTextWithWindowImage( const ALocalPoint& inMousePoint,
							const AArray<AScrapType>& inScrapType, const ATextArray& inData, 
							const AWindowRect& inWindowRect, const ANumber inImageWidth, const ANumber inImageHeight )
	{ return mViewImp.DragTextWithWindowImage(inMousePoint,inScrapType,inData,inWindowRect,inImageWidth,inImageHeight); }//#688
	//#688
  protected:
	void					NVMC_ShowContextMenu( const AContextMenuID inContextMenuID, const AGlobalPoint& inMousePoint )
	{ mViewImp.ShowContextMenu(inContextMenuID,inMousePoint); }
	//#688
  protected:
	void					NVMC_SetAcceptFocus( const ABool inAcceptFocus )
	{ mViewImp.SetAcceptFocus(inAcceptFocus); }
	//viewが不透明かどうかを設定 #1090
  protected:
	void					NVMC_SetOpaque( const ABool inOpaque )
	{ mViewImp.SetOpaque(inOpaque); }
	//カーソル win
  protected:
	void					NVMC_ObscureCursor()
	{ mViewImp.ObscureCursor(); }
	//その他 #182
  protected:
	void					NVMC_SetRedirectTextInput( const AObjectID inWindowID, const AControlID inControlID )
	{ mViewImp.SetRedirectTextInput(inWindowID,inControlID); }
	/*#688 void					NVMC_SetMouseTrackingMode( const ABool inSet )
	{ mViewImp.SetMouseTrackingMode(inSet); }*/
	void					NVMC_ShowDictionary( const AText& inWord, const ALocalPoint inLocalPoint ) const
	{ mViewImp.ShowDictionary(inWord,inLocalPoint); }
	//自動テスト用 #182
  public:
	ANumber					NVI_GetDrawTextCount() const
	{ return mViewImp.GetDrawTextCount(); }
	void					NVI_ClearDrawTextCount()
	{ return mViewImp.ClearDrawTextCount(); }
	
	//win
  protected:
	void					NVMC_SetOffscreenMode( const ABool inOffscreenMode )
	{ mViewImp.SetOffscreenMode(inOffscreenMode); }
	
	//#688
	//InlineInput用。選択範囲変更時にコールする。
  protected:
	void					NVMC_ResetSelectedRangeForTextInput()
	{ mViewImp.ResetSelectedRangeForTextInput(); }
	
	//#725
	//Image描画
  protected:
	void					NVMC_DrawImage( const AImageID inImageID, const ALocalPoint& inPoint,
											const ANumber inWidth = -1 , const ANumber inHeight = -1 )//#1012
	{ mViewImp.DrawImage(inImageID,inPoint,inWidth,inHeight); }
	ANumber					NVMC_GetImageWidth( const AImageID inImageID ) const
	{ return mViewImp.GetImageWidth(inImageID); }
	ANumber					NVMC_GetImageHeight( const AImageID inImageID ) const
	{ return mViewImp.GetImageHeight(inImageID); }
	void					NVMC_DrawImageFlexibleWidth( const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
							const ALocalPoint& inPoint, const ANumber inWidth )
	{ mViewImp.DrawImageFlexibleWidth(inImageID0,inImageID1,inImageID2,inPoint,inWidth); }
	void					NVMC_DrawImageFlexibleHeight( const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
							const ALocalPoint& inPoint, const ANumber inHeight )
	{ mViewImp.DrawImageFlexibleHeight(inImageID0,inImageID1,inImageID2,inPoint,inHeight); }
	void					NVMC_DrawImageFlexibleWidthAndHeight( 
							const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
							const AImageID inImageID3, const AImageID inImageID4, const AImageID inImageID5,
							const AImageID inImageID6, const AImageID inImageID7, const AImageID inImageID8,
							const ALocalPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
	{ mViewImp.DrawImageFlexibleWidthAndHeight(inImageID0,inImageID1,inImageID2,inImageID3,inImageID4,inImageID5,
	inImageID6,inImageID7,inImageID8,inPoint,inWidth,inHeight); }
	
	//#725
	//テキスト描画
  public:
	void					NVI_GetEllipsisTextWithFixedLastCharacters( const AText& inText, CTextDrawData& inTextDrawData, const ANumber inWidth,
																	   const AItemCount inLastCharCount, AText& outText,
																	   const ABool inEllipsisRepeatChar = false ) ;
	void					NVI_GetEllipsisTextWithFixedLastCharacters( const AText& inText, const ANumber inWidth,
																	   const AItemCount inLastCharCount, AText& outText,
																	   const ABool inEllipsisRepeatChar = false ) ;
	void					NVI_GetEllipsisTextWithFixedFirstCharacters( const AText& inText, CTextDrawData& inTextDrawData, const ANumber inWidth,
							const AItemCount inFirstCharCount, AText& outText ) ;
	void					NVI_GetEllipsisTextWithFixedFirstCharacters( const AText& inText, const ANumber inWidth,
																		const AItemCount inFirstCharCount, AText& outText ) ;
	void					NVI_GetTextInWidth( const AText& inText, const ANumber inWidth, AText& outText ) ;
	void					NVI_DrawTextMultiLine( const ALocalRect& inLocalRect, const AText& inText,
							const AText& inFontName, const AFloatNumber inFontSize, 
							const AColor inColor, const ATextStyle inStyle, const ABool inAntiAlias );
	
	//背景色／透明度 #725
  public:
	void					NVI_SetBackgroundColor( const AColor& inColor, const AColor& inDeactiveColor )
	{ mBackgroundActiveColor = inColor; mBackgroundDeactiveColor = inDeactiveColor; }
	void					NVI_SetBackgroundTransparency( const AFloatNumber inTransparency ) { mBackgroundTransparency = inTransparency; }
  protected:
	AFloatNumber						mBackgroundTransparency;
	AColor								mBackgroundActiveColor;
	AColor								mBackgroundDeactiveColor;
	
	//イベントRedirect
  public:
	void					NVI_SetMouseWheelEventRedirectViewID( const AViewID inViewID );
  private:
	virtual void			NVIDO_SetMouseWheelEventRedirectViewID( const AViewID inViewID ) {}
	AViewID								mMouseWheelEventRedirectViewID;
	
	//Frame View #135
  public:
	void					NVI_CreateFrameView( const ANumber inLeftMargin = 4, const ANumber inRightMargin = 4, 
							const ANumber inTopMargin = 3, const ANumber inBottomMargin = 3 );
	void					NVI_SetFrameViewColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive );
	void					NVI_SetFrameBackgroundColor( const ABool inEnableFrameBackgroundColor, const AColor inColor );//win
	ANumber					NVI_GetFrameLeftMargin() const { return mFrameLeftMargin; }
	ANumber					NVI_GetFrameRightMargin() const { return mFrameRightMargin; }
	ANumber					NVI_GetFrameTopMargin() const { return mFrameTopMargin; }
	ANumber					NVI_GetFrameBottomMargin() const { return mFrameBottomMargin; }
	AControlID				NVI_GetFrameViewControlID() const { return mFrameViewControlID; }
	AView_Frame&			NVI_GetFrameView() { return GetFrameView(); }
	ABool					NVI_ExistFrameView() const { return ExistFrameView(); }
  private:
	//#688 void					UpdateFrameViewBounds();
	void					DeleteFrameView();
	AView_Frame&			GetFrameView();
	ABool					ExistFrameView() const { return (mFrameViewControlID!=kControlID_Invalid); }
	AControlID							mFrameViewControlID;
	ANumber								mFrameLeftMargin;
	ANumber								mFrameRightMargin;
	ANumber								mFrameTopMargin;
	ANumber								mFrameBottomMargin;
	
	//ScrollBar #135
  public:
	void					NVI_CreateVScrollBar( const ANumber inWidth = 15, const ABool inHasHScrollbar = false );
	void					NVI_CreateHScrollBar( const ANumber inHeight = 15, const ABool inHasVScrollbar = false );
  private:
	AControlID							mVScrollBarControlID;
	AControlID							mHScrollBarControlID;
	ANumber								mVScrollBarWidth;
	ANumber								mHScrollBarHeight;
	
	//Help tag
  public:
	void					NVI_SetEnableHelpTag( const ABool inEnable ) { mEnableHelpTag = inEnable; }
	ABool					NVI_GetEnableHelpTag() const { return mEnableHelpTag; }
  private:
	ABool								mEnableHelpTag;
	
	//#558
	//縦書きモード
  public:
	void					NVI_SetVerticalMode( const ABool inVerticalMode ) 
	{
		mViewImp.SetVerticalMode(inVerticalMode);
	}
	ABool					NVI_GetVerticalMode() const
	{
		return mViewImp.GetVerticalMode();
	}
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AView"; }
	
};

typedef AAbstractFactoryForObjectArray<AView>	AViewFactory;

//#688 enumから変更
//Mouse trackingモード（mouse tracking中の処理を決める）
typedef AIndex AMouseTrackingMode;
const AMouseTrackingMode	kMouseTrackingMode_None = 0;
const AMouseTrackingMode	kMouseTrackingMode_SingleClick = 1;
const AMouseTrackingMode	kMouseTrackingMode_SingleClickInSelect = 2;
const AMouseTrackingMode	kMouseTrackingMode_DoubleClick = 3;
const AMouseTrackingMode	kMouseTrackingMode_TripleClick = 4;
const AMouseTrackingMode	kMouseTrackingMode_QuadClick = 5;

#pragma mark ===========================
#pragma mark [クラス]AViewDefaultFactory
template<class T> class AViewDefaultFactory : public AViewFactory
{
  public:
	AViewDefaultFactory<T>( const AWindowID inWindowID, const AControlID inID ) : mWindowID(inWindowID), mControlID(inID)
	{
	}
	T*	Create() 
	{
		return new T(mWindowID,mControlID);
	}
  private:
	AWindowID							mWindowID;
	AControlID							mControlID;
};

