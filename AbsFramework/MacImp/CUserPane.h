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

CUserPane

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "../Wrapper.h"
#include "../AbsFrame/AbsFrame.h"

class AView;
class CWindowImp;
class AWindow;
class CTextDrawData;

typedef CGContextRef ADrawContextRef;

class CUserPaneCocoaObjects;

/**
UserPane基本クラス
*/
class CUserPane: public AObjectArrayItem
{
	//コンストラクタ／デストラクタ
  public:
	CUserPane( AObjectArrayItem* inParent, CWindowImp& inWindow, //#688const AControlRef inControlRef, 
				const AControlID inControlID/*#688 , const ABool inInstallEventHandler = true*/ );
	virtual ~CUserPane();
	
	//AWindow取得
  public:
	AWindow&				GetAWin();
	
	//CocoaObject #1034
  public:
	CUserPaneCocoaObjects&	GetCocoaObjects() { return (*CocoaObjects); }
	const CUserPaneCocoaObjects&	GetCocoaObjectsConst() const { return (*CocoaObjects); }
  protected:
	CUserPaneCocoaObjects*				CocoaObjects;
	
	//コントロール情報取得
  public:
	AControlID				GetControlID() const {return mControlID;}//#688
	void					GetControlPosition( AWindowPoint& outPoint ) const;
	void					GetControlLocalFrameRect( ALocalRect& outRect ) const;
	void					GetControlWindowFrameRect( AWindowRect& outRect ) const;
	AWindowRef				GetWindowRef();
	//#688 AControlRef				GetControlRef() const;//#688 {return mControlRef;}
  protected:
	CGContextRef			GetCGContextRef() const {return mCGContextRef;}
  private:
	//#688 AControlRef							mControlRef;
	AControlID							mControlID;//#688
	CGContextRef						mCGContextRef;
	
#if SUPPORT_CARBON
	//イベントハンドラ
  public:
	virtual ABool			APICB_DoDraw() = 0;
	virtual ABool			APICB_DoControlBoundsChanged( const EventRef inEventRef );
	virtual ABool			DoMouseDown( const EventRef inEventRef ) = 0;
	virtual ABool			DoMouseWheel( const EventRef inEventRef );
	virtual ABool			DoMouseMoved( const EventRef inEventRef );
	virtual ABool			DoAdjustCursor( const EventRef inEventRef );
	virtual ABool			DoMouseExited( const EventRef inEventRef );
	virtual void			DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys ) {}
	virtual void			DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys ) {}
	virtual void			DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys ) {}
	virtual void			DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys ) {}
	virtual void			DoWindowActivated() {}
	virtual void			DoWindowDeactivated() {}
	
	//メニューコマンド処理
  public:
	virtual ABool			DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys );
	virtual void			UpdateMenu();
	
	//コントロール操作インターフェイス
  public:
	virtual void			SetFocus();
	virtual void			ResetFocus();
	/*#360
	ABool					GetSupportFocus() const { return mSupportFocus; }
	void					SetSupportFocus( const ABool inSupportFocus ) { mSupportFocus = inSupportFocus; }
  private:
	ABool								mSupportFocus;
	*/
#endif
	
	//コントロール属性設定インターフェイス
  public:
	/*#688virtual*/void		SetEnableControl( const ABool inEnable );
	virtual void			PreProcess_SetEnableControl( const ABool inEnable );//#688
	/*#688virtual*/ABool	IsControlEnabled() const;
	/*#688virtual*/void		SetShowControl( const ABool inShow );
	virtual void			PreProcess_SetShowControl( const ABool inShow );//#688
	/*#688virtual*/ABool	IsControlVisible() const;
	virtual ABool			IsControlActive() const;
	virtual void			SetTextFont( const AText& inFontName, const AFloatNumber inFontSize ) {_AError("not implemented",this);}//win
	virtual void			SetTextStyle( const ABool inBold ) {_AError("not implemented",this);}
	virtual void			SetCatchReturn( const ABool inCatchReturn ) {mCatchReturn = inCatchReturn;}
	virtual ABool			IsCatchReturn() const {return mCatchReturn;}
	virtual void			SetCatchTab( const ABool inCatchTab ) {mCatchTab = inCatchTab;}
	virtual ABool			IsCatchTab() const {return mCatchTab;}
	virtual void			SetAutoSelectAllBySwitchFocus( ABool inAutoSelectAll ) {mAutoSelectAllBySwitchFocus = inAutoSelectAll;}
	virtual ABool			IsAutoSelectAllBySwitchFocus() const {return mAutoSelectAllBySwitchFocus;}
	void					SetDefaultCursor( const ACursorType inCursorType );
	void					EnableMouseLeaveEvent();//win 080713
	void					SetControlSize( const ANumber inWidth, const ANumber inHeight );//#282
	void					SetControlPosition( const AWindowPoint& inWindowPoint );//#282
	void					SetControlBindings( 
							const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
							const ABool inWidthFixed, const ABool inHeightFixed );//#455 #688
	void					SetAcceptFocus( const ABool inAcceptFocus );//#688
	void					SetOpaque( const ABool inOpaque );//#1090
  private:
	ABool								mCatchReturn;
	ABool								mCatchTab;
	ABool								mAutoSelectAllBySwitchFocus;
  protected:
	ABool								mEnableMouseLeaveEvent;//B0000 080810
	
	//左下が丸いのを考慮したスクロールをするかどうか設定 #947
  public:
	void					SetAdjustScrollForRoundedCorner( const ABool inAdjust ) { mAdjustScrollForRoundedCorner = inAdjust; }
	ABool					GetAdjustScrollForRoundedCorner() const { return mAdjustScrollForRoundedCorner; }
  private:
	ABool								mAdjustScrollForRoundedCorner;
	
	//コントロール値Set/Getインターフェイス（以下のうち必要なものだけをサブクラスで実装）
  public:
	virtual void			GetText( AText& outText ) const {_AError("not implemented",this);}
	virtual void			SetText( const AText& inText ) {_AError("not implemented",this);}
	virtual void			AddText( const AText& inText ) {_AError("not implemented",this);}
	virtual void			InputText( const AText& inText ) {_AError("not implemented",this);}
	virtual void			SetBool( const ABool inBool ) {_AError("not implemented",this);}
	virtual void			GetBool( ABool& outBool ) const  {_AError("not implemented",this);}
	virtual void			GetColor( AColor& outColor ) const {_AError("not implemented",this);}
	virtual void			SetColor( const AColor& inColor ) {_AError("not implemented",this);}
	virtual void			GetNumber( ANumber& outNumber ) const {_AError("not implemented",this);}
	virtual void			SetNumber( const ANumber inNumber ) {_AError("not implemented",this);}
	virtual void			GetFloatNumber( AFloatNumber& outNumber ) const {_AError("not implemented",this);}
	virtual void			SetFloatNumber( const AFloatNumber inNumber ) {_AError("not implemented",this);}
	virtual void			SetEmptyValue() {_AError("not implemented",this);}
	virtual void			ResetUndoStack() {}
	
	//コントロール内選択インターフェイス
  public:
	virtual void			SetSelection( const AIndex inStart, const AIndex inEnd ) {}
	virtual void			SetSelectionAll() {}
	
	//描画属性設定
  public:
	void					SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, 
							const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing, //win
							const AFloatNumber inAlpha = 1.0 );
	void					SetDefaultTextProperty( const AColor& inColor, const ATextStyle inStyle, //win
							const AFloatNumber inAlpha = 1.0 );
	//#1034 ATSUStyle				GetATSUTextStyle() const;//#852
  protected:
	//#688 AFontNumber						mFont;
	AText							mFontName;//#688
	AFloatNumber					mFontSize;
	AColor							mColor;
	ATextStyle						mStyle;
	ABool							mAntiAliasing;
	AFloatNumber					mTextAlpha;
  private:
	//#1034 ATSUStyle						mATSUTextStyle;
	void					UpdateFont();//#1034
	//mutable ABool					mATSUTextStyleInited;//#852
	void					UpdateMetrics();//#0
	ANumber							mLineHeight;//#0
	ANumber							mLineAscent;//#0
	static AHashTextArray			sMetricsCacheArray_Key;//#1090
	static ANumberArray				sMetricsCacheArray_LineHeight;//#1090
	static ANumberArray				sMetricsCacheArray_LineAscent;//#1090
	
	//描画ルーチン
  public:
	void					DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, 
							const AText& inText, const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
							const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification = kJustification_Left );
	void					DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, 
									  const AJustification inJustification = kJustification_Left );
	/*#1012
	void					DrawIcon( const ALocalRect& inRect, const AIconID inIconID, const ABool inEnabledColor, const ABool inDefaultSize = false ) const;//win 080722
	*/
	void					DrawIconFromFile( const ALocalRect& inRect, const AFileAcc& inFile, const ABool inEnabledColor ) const;
	void					PaintPoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha );//#379
	void					FramePoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha );//#688
	void					PaintRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha = 1.0,
							const ABool inDropShadow = false );
	void					PaintRectWithVerticalGradient( const ALocalRect& inRect, 
							const AColor& inTopColor, const AColor& inBottomColor, 
							const AFloatNumber inTopAlpha, const AFloatNumber inBottomAlpha );//#634
	void					PaintRectWithHorizontalGradient( const ALocalRect& inRect, 
							const AColor& inLeftColor, const AColor& inRightColor, const AFloatNumber inLeftAlpha, const AFloatNumber inRightAlpha );//#634
	void					EraseRect( const ALocalRect& inRect );//#688 {}
	void					FrameRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha,
									  const ABool inDrawLeftLine, const ABool inDrawTopLine,
									  const ABool inDrawRightLine, const ABool inDrawBottomLine,
									  const AFloatNumber inLineWidth  );
	void					PaintRoundedRect( const ALocalRect& inRect, 
											 const AColor& inStartColor, const AColor& inEndColor, const AGradientType inGradientType, 
											 const AFloatNumber inStartAlpha, const AFloatNumber inEndAlpha,
											 const ANumber inR, 
											 const ABool inLeftTopSide, const ABool inRightTopSide, 
											 const ABool inLeftBottomSide, const ABool inRightBottomSide );//#724
	void					FrameRoundedRect( const ALocalRect& inRect, 
											 const AColor& inColor, const AFloatNumber inAlpha,
											 const ANumber inR, 
											 const ABool inLeftTopSide, const ABool inRightTopSide, 
											 const ABool inLeftBottomSide, const ABool inRightBottomSide,
											 const ABool inDrawLeftLine = true, const ABool inDrawTopLine = true,
											 const ABool inDrawRightLine = true, const ABool inDrawBottomLine = true,
											 const AFloatNumber inLineWidth = 0.2 );//#724
	void					DrawCircle( const ALocalRect& inDrawRect, const AColor& inColor, const ANumber inLineWidth, const AFloatNumber inAlpha );//#871
	void					DrawFocusFrame( const ALocalRect& inRect );//win
	void					DrawBackgroundImage( const ALocalRect& inDrawRect, const AImageRect& inImageFrameRect, 
							const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha );
	void					DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
							const AFloatNumber inAlpha = 1.0, const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0,
							const ABool inDropShadow = false );
	void					DrawCurvedLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
							const AFloatNumber inAlpha = 1.0, const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0 );//#723
	void					SetDrawLineMode( const AColor& inColor, const AFloatNumber inAlpha = 1.0, 
							const AFloatNumber inLineDash = 0.0, const AFloatNumber inLineWidth = 1.0 );//B0000
	void					RestoreDrawLineMode();//B0000
	void					DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint );//B0000
	void					DrawXorCaretLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const ABool inClipToFrame, 
											  const ABool inFlush, const ABool inDash, const ANumber inPenSize = 1 );//#1034
	void					DrawXorCaretRect( const ALocalRect& inLocalRect, const ABool inClipToFrame, const ABool inFlush );//#1034
	void					TransferOffscreen() {}//win
	void					RedrawImmediately();//B0000
	ABool					IsInLiveResize() const;//#688
	void					SetDropShadowColor( const AColor& inColor ) { mDropShadowColor = inColor; }//#725
  protected:
	/*#1034
	void					DrawTextCore( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AJustification inJustification,
										 const ATSUTextLayout inTextLayout, const ABool inDrawSelection, 
										 const AIndex inSelectionStart, const AIndex inSelectionEnd, const AColor inSelectionColor, const AFloatNumber inSelectionAlpha );
	ATSUTextLayout			CreateTextLayout( const AText& inUTF16Text, const ABool inUseFontFallback ) const;//#703
	*/
	//#1034
	void					DrawTextCore( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AJustification inJustification,
										  const CTLineRef inCTLineRef, const ABool inDrawSelection, 
										  const AIndex inSelectionStart, const AIndex inSelectionEnd, const AColor inSelectionColor, const AFloatNumber inSelectionAlpha );
	CTLineRef				GetCTLineFromTextDrawData( CTextDrawData& inTextDrawData );
	CTLineRef				CreateDefaultCTLine( const AText& inText ) ;

	CGPoint					GetCGPointFromAPoint( const ALocalPoint& inPoint ) const;
	CGRect					GetCGRectFromARect( const ALocalRect& inRect ) const;
#if SUPPORT_CARBON
	void					SetupCGContextOutOfDoDraw();
	void					EndCGContextOutOfDoDraw();
#endif
	ADrawContextRef						mContextRef;
#if SUPPORT_CARBON
	RgnHandle							mDrawUpdateRegion;
#endif
  private:
	void					AddVerticalAttribute( NSMutableAttributedString *inAttrString, NSString* inString );//#558
	CGColorRef				CreateCGColor( const AColor inColor, const AFloatNumber inAlpha ) const;//#634
	AColor								mDropShadowColor;//#725
	
	//イメージ #725
  public:
	static void				RegisterImageByFile( const AImageID inImageID, const AFileAcc& inImageFile );
	static void				RegisterImageByName( const AImageID inImageID, const AText& inImageFileName );
	static void				RegisterImageByName( const AImageID inImageID, const AUCharPtr inImageFileName );
	static AIndex			LoadImage( const AImageID inImageID );//#1090
	static void				UnregisterImage( const AImageID inImageID );
	void					DrawImage( const AImageID inImageID, const ALocalPoint& inPoint,
									   const ANumber inWidth = -1, const ANumber inHeight = -1 );
	void					DrawImageFlexibleWidth( const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
							const ALocalPoint& inPoint, const ANumber inWidth );
	void					DrawImageFlexibleHeight( const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
							const ALocalPoint& inPoint, const ANumber inHeight );
	void					DrawImageFlexibleWidthAndHeight( 
							const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
							const AImageID inImageID3, const AImageID inImageID4, const AImageID inImageID5,
							const AImageID inImageID6, const AImageID inImageID7, const AImageID inImageID8,
							const ALocalPoint& inPoint, const ANumber inWidth, const ANumber inHeight );
	ANumber					GetImageWidth( const AImageID inImageID ) const;
	ANumber					GetImageHeight( const AImageID inImageID ) const;
	//Refresh
  public:
	void					RefreshRect( const ALocalRect& inLocalRect );
	void					RefreshControl();
	void					FlushRect( const ALocalRect& inLocalRect ) {}
	void					FlushControl() {}//win
	void					ExecuteDoDrawImmediately();//#688
	
	//描画用情報取得
  public:
	ANumber					GetImageXByTextPosition( CTextDrawData& inTextDrawData, const AIndex inTextPosition );
	ANumber					GetImageXByUnicodeOffset( CTextDrawData& inTextDrawData, const AIndex inUnicodeOffset );
	AIndex					GetTextPositionByImageX( CTextDrawData& inTextDrawData, const ANumber inImageX );
	AFloatNumber			GetDrawTextWidth( const AText& inText ) ;
	AFloatNumber			GetDrawTextWidth( CTextDrawData& inTextDrawData ) ;
	ABool					IsRectInDrawUpdateRegion( const ALocalRect& inLocalRect ) const;
	void					GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) ;
	void					GetMetricsForDefaultTextProperty( AFloatNumber& outLineHeight, AFloatNumber& outLineAscent );//#1316
	//win void					GetMousePoint( ALocalPoint& outLocalPoint ) ;
	ALocalRect				GetDrawUpdateRect() const { return mCocoaDrawDirtyRect; }//#852
  private:
	AText								mTextForMetricsCalculation;//#636
	//スクロール
  public:
	void					Scroll( const ANumber inDeltaX, const ANumber inDeltaY );
	
	//Drag
  public:
	void					EnableDrop( const AArray<AScrapType>& inScrapType );//#236 {}//win 080713
	ABool					DragText( const ALocalPoint& inMousePoint, 
							const AArray<ALocalRect>& inDragRect, const AArray<AScrapType>& inScrapType, const ATextArray& inData,
							const AImageID inImageID );
	ABool					DragTextWithWindowImage( const ALocalPoint& inMousePoint,
							const AArray<AScrapType>& inScrapType, const ATextArray& inData, 
							const AWindowRect& inWindowRect, const ANumber inImageWidth, const ANumber inImageHeight );//#688
	static void				SetDropped( const ABool inDropped ) { sDropped = inDropped; }
  private:
	static ABool						sDropped;
 #if SUPPORT_CARBON
 protected:
	AArray<AScrapType>					mScrapTypeArray;//#364
#endif
	
	//Contextual menu #688
  public:
	void					ShowContextMenu( const AContextMenuID inContextMenuID, const AGlobalPoint& inMousePoint );
	
	//カーソル win
  public:
	void					ObscureCursor() {ACursorWrapper::ObscureCursor();}
	
	//
	public:
	void					SetRedirectTextInput( const AObjectID inWindowID, const AControlID inControlID ) {/*Mac版は処理無し*/}
	
	//
  public:
	void					SetMouseTrackingMode( const ABool inSet ) {}
	
	//Offscreenモード／直接描画モード
  public:
	void					SetOffscreenMode( const ABool inOffscreenMode ) {}
	
	//#558
	//縦書きモード
  public:
	void					SetVerticalMode( const ABool inVerticalMode );
	ABool					GetVerticalMode() const { return mVerticalMode; }
  private:
	void					ChangeToVerticalCTM( CGContextRef inContextRef );
	ABool								mVerticalMode;
	
	//#1309
	//サービスメニュー可否
  public:
	void					SetServiceMenuAvailable( const ABool inAvailable );
	
	/*#688
  public:
	virtual void			TellChangedToWindow();
	*/
	//#688
  public:
	void					ResetSelectedRangeForTextInput();
	
	CWindowImp&				GetWin() { return mWindowImp; }
	CWindowImp&				GetWinConst() const  { return mWindowImp; }
	CWindowImp&							mWindowImp;
	
	//イベントハンドラ（各種イベント発生時のCocoaからのコールバック）#688
  public:
	void					APICB_CocoaDrawRect( const ALocalRect& inDirtyRect );
  private:
	ALocalRect							mCocoaDrawDirtyRect;
	
#if SUPPORT_CARBON
	//イベントハンドラ（OSコールバック用static）
  public:
	static pascal OSStatus	STATIC_APICB_DrawHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	static pascal OSStatus	STATIC_APICB_ControlBoundsChangedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
	//B0000 080810 static pascal OSStatus STATIC_APICB_MouseExitedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData );
  private:
	virtual void			InstallEventHandler();
#endif
	
	//自動Test用
  public:
	ANumber					GetDrawTextCount() const { return mDrawTextCount; }
	void					ClearDrawTextCount() { mDrawTextCount = 0; }
  private:
	ANumber								mDrawTextCount;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "CUserPane"; }
	
};

typedef AAbstractFactoryForObjectArray<CUserPane>	CUserPaneFactory;

