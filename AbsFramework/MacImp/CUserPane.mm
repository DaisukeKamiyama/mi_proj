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

#include "CUserPane.h"
#include "CWindowImp.h"
//#1012 #include "StSetPort.h"
#include "../Frame.h"
#include "CTextDrawData.h"
#include "CWindowImpCocoaObjects.h"
#include <CoreFoundation/CoreFoundation.h>

#import <Cocoa/Cocoa.h>
#import "CocoaUserPaneView.h"
#import "AppDelegate.h"

extern NSRect	ConvertFromWindowRectToNSRect( NSView* inContentView, const AWindowRect inWindowRect );
extern NSPoint	ConvertFromWindowPointToNSPoint( NSView* inContentView, const AWindowPoint inWindowPoint );

//一行に表示できるUTF16バイト数の最大 20000バイト(1000000文字) #572 #1045 1万→100万
//#1045 const AByteCount	kLineTextDrawLengthMax = 2000000;//#1045 20000→2000000

#pragma mark ===========================
#pragma mark [クラス]CUserPane
#pragma mark ===========================
//UserPaneは、OSが標準サポートしていないコントロールを実現するための各クラスの基底クラス
//Interface Builder上ではUser Paneとして定義し、描画やクリック時の動作等をサブクラスで定義する。

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
CUserPane::CUserPane( AObjectArrayItem* inParent, CWindowImp& inWindowImp, //688const AControlRef inControlRef
			const AControlID inControlID /*#688 , const ABool inInstallEventHandler*/ ) 
: AObjectArrayItem(inParent), mWindowImp(inWindowImp), /*#688 mControlRef(inControlRef)*/ mControlID(inControlID), 
		mCatchReturn(false), mCatchTab(false), mAutoSelectAllBySwitchFocus(true), mContextRef(NULL),mDrawTextCount(0),
		mEnableMouseLeaveEvent(false)//B0000 080810
		//#360,mSupportFocus(true)//#291
//#688,mDrawUpdateRegion(NULL)
,mCocoaDrawDirtyRect(kLocalRect_0000)//#688
,mDropShadowColor(kColor_White)//#725
,mFontSize(9.0), mColor(kColor_Black), mStyle(kTextStyle_Normal), mAntiAliasing(true)//#852
//#1034 ,mATSUTextStyle(NULL)//,mATSUTextStyleInited(false)//#852
,CocoaObjects(NULL)//#1034
,mTextAlpha(1.0)
,mLineHeight(0),mLineAscent(0)
,mAdjustScrollForRoundedCorner(true)//#947
,mVerticalMode(false)//#558
{
	//Cocoaオブジェクト生成 #1034
	CocoaObjects = new CUserPaneCocoaObjects();
	
	OSStatus	err = noErr;
	
	/*#852
	err = ::ATSUCreateStyle(&mATSUTextStyle);
	if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,this);
	*/
	//mATSUTextStyle = CAppImp::CreateDefaultATSUStyle();//#852
#if SUPPORT_CARBON
	if( inInstallEventHandler == true )
	{
		InstallEventHandler();
	}
#endif
	//#636
	mTextForMetricsCalculation.AddFromUCS4Char(0x0079);//'y'
	mTextForMetricsCalculation.AddFromUCS4Char(0xFFE3);//'￣'
	//mTextForMetricsCalculation.AddFromUCS4Char(0xFF3F);//'＿'
	mTextForMetricsCalculation.ConvertFromUTF8ToUTF16();
	//#852
	//デフォルトフォント名取得
	AFontWrapper::GetDialogDefaultFontName(mFontName);
}

/**
デストラクタ
*/
CUserPane::~CUserPane()
{
	//Cocoaオブジェクト削除 #1034
	delete CocoaObjects;
	CocoaObjects = NULL;
	
	/*#1034
	OSStatus	err = noErr;
	if( mATSUTextStyle != NULL )//#852
	{
		err = ::ATSUDisposeStyle(mATSUTextStyle);
		if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,this);
	}
	*/
}

/**
AWindow取得
*/
AWindow&	CUserPane::GetAWin()
{
	return mWindowImp.GetAWin();
}

#pragma mark ===========================

#pragma mark ---コントロール情報取得

/**
自分のControlID取得
*/
/*#688
AControlID	CUserPane::GetControlID() const
{
	return GetWinConst().GetControlID(mControlRef);
}
*/

//#688
/**
ControlRefを取得
Cocoa対応によりUserPaneではControlRefは保持しないようにするので、WindowImpから取得
*/
/*#688
AControlRef	CUserPane::GetControlRef() const
{
	return GetWinConst().GetControlRef(mControlID);
}
*/

/**
コントロールの位置取得
*/
void	CUserPane::GetControlPosition( AWindowPoint& outPoint ) const
{
	GetWinConst().GetControlPosition(GetControlID(),outPoint);
}

/**
コントロール全体のRect取得(LocalRect)
*/
void	CUserPane::GetControlLocalFrameRect( ALocalRect& outRect ) const
{
	outRect.left 	= 0;
	outRect.top 	= 0;
	outRect.right 	= GetWinConst().GetControlWidth(GetControlID());
	outRect.bottom 	= GetWinConst().GetControlHeight(GetControlID());
}

/**
コントロール全体のRect取得(WindowRect)
*/
void	CUserPane::GetControlWindowFrameRect( AWindowRect& outRect ) const
{
	ALocalRect	localRect;
	GetControlLocalFrameRect(localRect);
	GetWinConst().GetWindowRectFromControlLocalRect(GetControlID(),localRect,outRect);
}

/**
WindowRef取得
*/
AWindowRef	CUserPane::GetWindowRef()
{
	return mWindowImp.GetWindowRef();
}

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark --- イベントハンドラ

/**
領域変更イベントハンドラ
*/
ABool	CUserPane::APICB_DoControlBoundsChanged( const EventRef inEventRef )
{
	//デフォルト動作：なし
	return true;
}

/**
マウスホイールイベントコールバック
*/
ABool	CUserPane::DoMouseWheel( const EventRef inEventRef )
{
	//デフォルト動作：なし
	return false;
}

/**
マウス移動イベントコールバック
*/
ABool	CUserPane::DoMouseMoved( const EventRef inEventRef )
{
	//デフォルト動作：なし
	return false;
}

/**
カーソル調整コールバック
*/
ABool	CUserPane::DoAdjustCursor( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	err = ::SetThemeCursor(kThemeArrowCursor);
	if( err != noErr )   _ACErrorNum("SetThemeCursor() returned error: ",err,this);
	return true;
}

/**
マウス領域外移動コールバック
*/
ABool	CUserPane::DoMouseExited( const EventRef inEventRef )
{
	//デフォルト動作：なし
	return true;
}

#pragma mark ===========================

#pragma mark ---コントロールフォーカス処理

//virtual, public
/**
フォーカス獲得
*/
void	CUserPane::SetFocus() 
{
	//デフォルト動作：処理無し
}

//virtual, public
/**
フォーカス解除
*/
void	CUserPane::ResetFocus()
{
	//デフォルト動作：処理無し
}

#pragma mark ===========================

#pragma mark ---メニューコマンド処理

//virtual, public
/**
メニューコマンド処理
*/
ABool	CUserPane::DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	//デフォルト動作：処理無し
	return false;
}

/**
メニュー更新処理
*/
void	CUserPane::UpdateMenu()
{
	//デフォルト動作：処理無し
	return;
}
#endif

#pragma mark ===========================

#pragma mark --- コントロール属性設定インターフェイス

/**
コントロールEnable/Disable
*/
void	CUserPane::SetEnableControl( const ABool inEnable )
{
	return GetWin().SetEnableControl(GetControlID(),inEnable);//#688 CWindowImp側にコントロールの処理を集約
}

void	CUserPane::PreProcess_SetEnableControl( const ABool inEnable )//#688
{
	/*#688
	//#530
	if( ::IsControlEnabled(mControlRef) == inEnable )   return;
	//
	if( inEnable == true )
	{
		::EnableControl(mControlRef);
	}
	else
	{
		::DisableControl(mControlRef);
	}
	RefreshControl();
	*/
}

/**
コントロールがEnableかどうかを取得
*/
ABool	CUserPane::IsControlEnabled() const
{
	//#688 return ::IsControlEnabled(mControlRef);
	return GetWinConst().IsControlEnabled(GetControlID());//#688 CWindowImp側にコントロールの処理を集約
}

/**
コントロールShow/Hide
*/
void	CUserPane::SetShowControl( const ABool inShow )
{
	return GetWin().SetShowControl(GetControlID(),inShow);//#688 CWindowImp側にコントロールの処理を集約
}

void	CUserPane::PreProcess_SetShowControl( const ABool inShow )//#688
{
	/*#688
	if( inShow )
	{
		::ShowControl(mControlRef);
	}
	else
	{
		::HideControl(mControlRef);
	}
	*/
}

/**
コントロールのShow/Hide状態取得
*/
ABool	CUserPane::IsControlVisible() const
{
	//#688 return ::IsControlVisible(mControlRef);
	return GetWinConst().IsControlVisible(GetControlID());//#688 CWindowImp側にコントロールの処理を集約
}

/**
コントロールがActiveかどうかを取得
*/
ABool	CUserPane::IsControlActive() const
{
	return GetWinConst().IsActive();
}

//win対応
/**
デフォルトのカーソルを設定
*/
void	CUserPane::SetDefaultCursor( const ACursorType inCursorType )
{
	//MacOSXの場合は、処理無し
}

/**
マウス領域外移動イベント可否設定
*/
void	CUserPane::EnableMouseLeaveEvent()
{
	mEnableMouseLeaveEvent = true;
	/*
	下記方法では動作しないので方式変更
	AApplicationのEVTDO_MouseMovedを使用する
	MouseTrackingRegionID	id;
	id.signature = 0;
	id.id = GetControlID();
	RgnHandle	rectrgn = ::NewRgn();
	Rect	frameRect;
	frameRect.left 		= 0;
	frameRect.top 		= 0;
	frameRect.right 	= GetWinConst().GetControlWidth(GetControlID());
	frameRect.bottom 	= GetWinConst().GetControlHeight(GetControlID());
	::RectRgn(rectrgn,&frameRect);
	MouseTrackingRef	trackingRef = NULL;
	::CreateMouseTrackingRegion(GetWindowRef(),rectrgn,NULL,kMouseTrackingOptionsLocalClip,id,NULL,NULL,&trackingRef);
	::DisposeRgn(rectrgn);
	
	OSStatus	err;
	EventHandlerRef	handlerRef;
	EventTypeSpec	typeSpec;
	typeSpec.eventClass = kEventClassMouse;
	typeSpec.eventKind = kEventMouseExited;
	err = ::InstallEventHandler(::GetControlEventTarget(mControlRef),::NewEventHandlerUPP(CUserPane::STATIC_APICB_MouseExitedHandler),
			1,&typeSpec,&mWindowImp,&handlerRef);
	if( err != noErr )   _ACError("",this);
	*/
}

//#282
/**
Controlサイズ設定
*/
void	CUserPane::SetControlSize( const ANumber inWidth, const ANumber inHeight )
{
	GetWin().SetControlSize(GetControlID(),inWidth,inHeight);
}

//#282
/**
Control位置設定
*/
void	CUserPane::SetControlPosition( const AWindowPoint& inWindowPoint )
{
	GetWin().SetControlPosition(GetControlID(),inWindowPoint);
}

//#455
/**
Binding設定
*/
void	CUserPane::SetControlBindings( 
		const ABool inLeftMarginFixed, const ABool inTopMarginFixed, const ABool inRightMarginFixed, const ABool inBottomMarginFixed,
		const ABool inWidthFixed, const ABool inHeightFixed )
{
	GetWin().SetControlBindings(GetControlID(),inLeftMarginFixed,inTopMarginFixed,inRightMarginFixed,inBottomMarginFixed,inWidthFixed,inHeightFixed);
}

/**
キーフォーカス可否を設定
*/
void	CUserPane::SetAcceptFocus( const ABool inAcceptFocus )
{
	//viewを取得
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return; }
	//フォーカス可否を設定
	[view setAcceptFocus:inAcceptFocus];
}

//#1090
/**
viewが不透明かどうかを設定
*/
void	CUserPane::SetOpaque( const ABool inOpaque )
{
	//viewを取得
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return; }
	//不透明かどうかを設定
	[view setOpaque:inOpaque];
}

/*#688
#pragma mark ===========================

#pragma mark --- 変更通知
*/
/**
ウインドウへ変更を通知する
*/
/*#688
void	CUserPane::TellChangedToWindow()
{
	GetWin().CB_UserPaneValueChanged(GetControlID());//#688GetWin().GetControlID(mControlRef));
}
 */

#pragma mark ===========================

#pragma mark --- InlineInput用

//#688
/**
InputMethodに選択位置リセットを通知する
*/
void	CUserPane::ResetSelectedRangeForTextInput()
{
	//viewを取得
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return; }
	//選択位置リセット
	[view resetSelectedRangeForTextInput];
}

#pragma mark ===========================

#pragma mark ---描画属性設定

//テキストデフォルト属性設定
/*win void	CUserPane::SetDefaultTextProperty( const AFont inFont, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing )
{
	//
	mFont = inFont;
	mFontSize = inFontSize;
	mColor = inColor;
	mStyle = inStyle;
	mAntiAliasing = inAntiAliasing;
	
	//mATSUStyle設定
	CWindowImp::SetATSUStyle(mATSUTextStyle,mFont,mFontSize,mAntiAliasing);
	
}*/

//#1034
/**
フォント更新
*/
void	CUserPane::UpdateFont()
{
	//通常フォント取得
	NSFont*	normalfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Normal);
	CocoaObjects->SetNormalFont(normalfont);
	//boldフォント取得
	NSFont*	boldfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Bold);
	CocoaObjects->SetBoldFont(boldfont);
	//italicフォント取得
	NSFont*	italicfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Italic);
	CocoaObjects->SetItalicFont(italicfont);
	//bold+italicフォント取得
	NSFont*	bolditalicfont = CWindowImp::GetNSFont(mFontName,mFontSize,kTextStyle_Bold|kTextStyle_Italic);
	CocoaObjects->SetBoldItalicFont(bolditalicfont);
	/*
	NSLog([normalfont displayName]);
	NSLog([boldfont displayName]);
	NSLog([italicfont displayName]);
	NSLog([bolditalicfont displayName]);
	*/
}

/**
テキストデフォルト属性設定
*/
void	CUserPane::SetDefaultTextProperty( const AText& inFontName, const AFloatNumber inFontSize, 
		const AColor& inColor, const ATextStyle inStyle, const ABool inAntiAliasing, const AFloatNumber inAlpha )
{
	/*#1034
	//ATSUStyle未生成なら生成
	ABool	atsuTextStyleCreated = false;
	if( mATSUTextStyle == NULL )
	{
		OSStatus err = ::ATSUCreateStyle(&mATSUTextStyle);
		if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,this);
		atsuTextStyleCreated = true;
	}
	*/
	//#852
	//mATSUTextStyleInited = true;
	
	/*#688
	//
	AFontNumber	fontnum = 0;//#450
	if( AFontWrapper::GetFontByName(inFontName,fontnum) == false )//#450
	{
	}
	*/
	//どれかのパラメータが違うときだけATSUStyle再設定 #450
	if( /*#1034atsuTextStyleCreated == true || */
		CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL ||
		mFontName.Compare(inFontName) == false || mFontSize != inFontSize || 
		AColorWrapper::CompareColor(mColor,inColor) == false || mStyle != inStyle || mAntiAliasing != inAntiAliasing ||//#450
		mTextAlpha != inAlpha )
	{
		mFontName.SetText(inFontName);//#688
		mFontSize = inFontSize;
		mColor = inColor;
		mStyle = inStyle;
		mAntiAliasing = inAntiAliasing;
		mTextAlpha = inAlpha;
		
		/*#1034
		//mATSUStyle設定
		CWindowImp::SetATSUStyle(mATSUTextStyle,mFontName,mFontSize,mAntiAliasing,mColor,mTextAlpha,mStyle);
		*/
		//フォント更新 #1034
		UpdateFont();
	}
	
	//#0
	UpdateMetrics();
}
/**
テキストデフォルト属性設定
*/
void	CUserPane::SetDefaultTextProperty( const AColor& inColor, const ATextStyle inStyle, const AFloatNumber inAlpha )
{
	/*#1034
	//ATSUStyle未生成なら生成
	ABool	atsuTextStyleCreated = false;
	if( mATSUTextStyle == NULL )
	{
		OSStatus err = ::ATSUCreateStyle(&mATSUTextStyle);
		if( err != noErr )   _ACErrorNum("ATSUCreateStyle() returned error: ",err,this);
		atsuTextStyleCreated = true;
	}
	//#852
	//mATSUTextStyleInited = true;
	*/
	//どれかのパラメータが違うときだけATSUStyle再設定 #450
	if( /*#1034 atsuTextStyleCreated == true ||*/ 
		CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL ||
		AColorWrapper::CompareColor(mColor,inColor) == false || mStyle != inStyle ||//#450
		inAlpha != mTextAlpha )
	{
		//
		mColor = inColor;
		mStyle = inStyle;
		mTextAlpha = inAlpha;
		
		/*#1034
		//mATSUStyle設定
		CWindowImp::SetATSUStyle(mATSUTextStyle,mFontName,mFontSize,mAntiAliasing,mColor,mTextAlpha,mStyle);
		*/
		//フォント更新 #1034
		UpdateFont();
	}
	
	//#0
	UpdateMetrics();
}

#if 0
//#852
/**
ATSUStyle取得
*/
ATSUStyle	CUserPane::GetATSUTextStyle() const
{
	/*
	if( mATSUTextStyleInited == false )
	{
		//CWindowImp::SetATSUStyle(mATSUTextStyle,mFontName,mFontSize,mAntiAliasing,mColor,mStyle);
		mATSUTextStyle = CAppImp::CreateDefaultATSUStyle();
		mATSUTextStyleInited = true;
	}
	return mATSUTextStyle;
	*/
	if( mATSUTextStyle == NULL )
	{
		//未設定ならデフォルトATSUStyleを返す
		return CAppImp::GetDefaultATSUStyle();
	}
	else
	{
		return mATSUTextStyle;
	}
}
#endif

#pragma mark ===========================

#pragma mark ---描画ルーチン

/**
テキスト表示
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const AText& inText, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inText,inJustification);
}

/**
テキスト表示
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AText& inText, const AJustification inJustification )
{
	OSStatus	err = noErr;
	
	//return DrawText(inDrawRect,inClipRect,inText,mColor,mStyle,inJustification);
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	/*#1034
	//B0000 高速化　DrawTextDataを作成せず、また、デフォルトのmATSUStyleのみを使用しrun styleを設定しない
	//TextLayout生成
	AText	utf16text;
	utf16text.SetText(inText);
	utf16text.ConvertFromUTF8ToUTF16();
	ATSUTextLayout	textLayout = CreateTextLayout(utf16text,true);
	*/
	
	//描画
	CTLineRef	line = CreateDefaultCTLine(inText);
	DrawTextCore(inDrawRect,inClipRect,inJustification,line,false,kIndex_Invalid,kIndex_Invalid,kColor_Black,0.8);
	
	/*#1034
	//TextLayout削除
	err = ::ATSUDisposeTextLayout(textLayout);
	if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,this);
	*/
	
	//CTLine解放 #1034
	::CFRelease(line);
}

/**
テキスト表示
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inText,inColor,inStyle,inJustification);
}

/**
テキスト表示
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, 
		const AText& inText, const AColor& inColor, const ATextStyle inStyle, const AJustification inJustification )
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(inText);
	textDrawData.UTF16DrawText.ConvertFromUTF8ToUTF16();
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(inColor);
	textDrawData.attrStyle.Add(inStyle);
	DrawText(inDrawRect,inClipRect,textDrawData,inJustification);
}

/**
テキスト表示
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, CTextDrawData& inTextDrawData, const AJustification inJustification )
{
	DrawText(inDrawRect,inDrawRect,inTextDrawData,inJustification);
}

/**
テキスト表示
*/
void	CUserPane::DrawText( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, CTextDrawData& inTextDrawData, const AJustification inJustification )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	/* ★CoreTextでは問題発生しないかもしれないので保留
	//#572 1行が10000文字を超える場合は、1000000文字までのみを表示する。 #1045 1万→100万
	//UTF16ではないテキストを間違えてUTF-16として表示すると1行が異常に長くなり、異常に表示時間がかかるため。
	if( inTextDrawData.UTF16DrawText.GetItemCount() > kLineTextDrawLengthMax )
	{
		//2000000バイトに切り詰め
		AText	truncatedUTF16Text;
		inTextDrawData.UTF16DrawText.GetText(0,kLineTextDrawLengthMax,truncatedUTF16Text);
		
		//TextLayout生成
		inTextDrawData.ClearTextLayout();
		inTextDrawData.SetTextLayout(CreateTextLayout(truncatedUTF16Text,false));
		
		//描画
		DrawTextCore(inDrawRect,inClipRect,inJustification,
					 inTextDrawData.GetTextLayout(),
					 inTextDrawData.drawSelection,
					 inTextDrawData.selectionStart,inTextDrawData.selectionEnd,inTextDrawData.selectionColor,inTextDrawData.selectionAlpha);
		return;
	}
	*/
	
	//★font fallback
	
	//描画
	DrawTextCore(inDrawRect,inClipRect,inJustification,GetCTLineFromTextDrawData(inTextDrawData),
				 inTextDrawData.drawSelection,
				 inTextDrawData.selectionStart,inTextDrawData.selectionEnd,inTextDrawData.selectionColor,inTextDrawData.selectionAlpha, inTextDrawData.selectionFrameAlpha,inTextDrawData.selectionBackgroundColor);//#1316
	
	//R0199　スペルチェック
	if( mVerticalMode == false )//#558
	{
		for( AIndex i = 0; i < inTextDrawData.misspellStartArray.GetItemCount(); i++ )
		{
			ANumber	lineHeight,lineAscent;
			GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
			
			//R0243
			//ascentラインの1つ下から赤線引くのがベスト（ascentラインから引くと、文字と近すぎて、文字が読めない。）
			//1つ下から2 pixelの線を引くので、ascentから3pt必要
			//もし、lineHeightがlineAscent+3より小さい場合は、ascentラインから引くことにする。
			ANumber	yoffset = 1;
			if( lineHeight < lineAscent+3 )
			{
				yoffset = 0;
			}
			
			ALocalPoint	spt, ept;
			spt.x = inDrawRect.left;
			spt.y = inDrawRect.top+lineAscent+yoffset;//R0243 +1;//+2;
			ept = spt;
			CGFloat	offset2 = 0;
			CGFloat	offset = ::CTLineGetOffsetForStringIndex(GetCTLineFromTextDrawData(inTextDrawData),inTextDrawData.misspellStartArray.Get(i),&offset2);
			spt.x += offset;
			offset = ::CTLineGetOffsetForStringIndex(GetCTLineFromTextDrawData(inTextDrawData),inTextDrawData.misspellEndArray.Get(i),&offset2);
			ept.x += offset;
			DrawLine(spt,ept,kColor_Red,0.8,3.0,2.0);//R0199 2pt幅の線にする
		}
	}
}

//#1034
/**
デフォルトフォントでのCTLine生成
*/
CTLineRef	CUserPane::CreateDefaultCTLine( const AText& inText ) 
{
	//フォント未設定なら設定する
	if( CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL )
	{
		UpdateFont();
	}
	
	//表示テキスト取得
	AStCreateNSStringFromAText	str(inText);
	//表示テキストのNSMutableAttributedString取得
	NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:str.GetNSString()];
	//attrStringはイベントループ終了で解放。CTLineでコピーしているか参照しているかは不明だが、参照しているならretain()しているはず。
	[attrString autorelease];
	//NSMutableAttributedStringの編集開始
	[attrString beginEditing];
	
	// ==================== テキスト属性設定 ====================
	//range設定
	NSRange	range = NSMakeRange(0,[attrString length]);
	//フォント設定
	if( (mStyle&kTextStyle_Bold) == 0 )
	{
		if( (mStyle&kTextStyle_Italic) == 0 )
		{
			//通常フォント
			[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetNormalFont() range:range];
		}
		else
		{
			//italicフォント
			[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetItalicFont() range:range];
		}
	}
	else
	{
		if( (mStyle&kTextStyle_Italic) == 0 )
		{
			//boldフォント
			[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldFont() range:range];
		}
		else
		{
			//bold+italicフォント
			[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldItalicFont() range:range];
		}
	}
	//#558
	//縦書き属性設定
	if( mVerticalMode == true )
	{
		AddVerticalAttribute(attrString,str.GetNSString());
	}
	
	//アンダーライン
	if( (mStyle&kTextStyle_Underline) != 0 )
	{
		[attrString addAttribute:NSUnderlineStyleAttributeName 
		value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:range];
	}
	
	// 文字色設定
	[attrString addAttribute:NSForegroundColorAttributeName 
	value:[NSColor colorWithCalibratedRed:(mColor.red/65535.0) green:(mColor.green/65535.0) 
	blue:(mColor.blue/65535.0) alpha:mTextAlpha] range:range];
	
	//NSMutableAttributedStringの編集終了
	[attrString endEditing];
	
	//CTLine生成
	return ::CTLineCreateWithAttributedString((CFAttributedStringRef)attrString);
}

//#1034
/**
TextDrawDataからCTLine取得（未生成なら生成）
*/
CTLineRef	CUserPane::GetCTLineFromTextDrawData( CTextDrawData& inTextDrawData )
{
	//フォント未設定なら設定する
	if( CocoaObjects->GetNormalFont() == NULL || CocoaObjects->GetBoldFont() == NULL || 
		CocoaObjects->GetItalicFont() == NULL || CocoaObjects->GetBoldItalicFont() == NULL )
	{
		UpdateFont();
	}
	
	if( inTextDrawData.GetCTLineRef() == NULL )
	{
		// ==================== 最初の属性にデフォルトを入れる ====================
		//デフォルトのフォント・色を設定するために、最初の項目に属性を入れる
		inTextDrawData.attrPos.Insert1(0,0);
		inTextDrawData.attrColor.Insert1(0,mColor);
		inTextDrawData.attrStyle.Insert1(0,mStyle);
		
		// ==================== 表示テキスト取得 ====================
		
		//表示テキスト取得
		AStCreateNSStringFromUTF16Text	str(inTextDrawData.UTF16DrawText);
		//表示テキストのNSMutableAttributedString取得
		NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:str.GetNSString()];
		//attrStringはイベントループ終了で解放。CTLineでコピーしているか参照しているかは不明だが、参照しているならretain()しているはず。
		[attrString autorelease];
		//NSMutableAttributedStringの編集開始
		[attrString beginEditing];
		
		// ==================== テキスト属性(RunStyle)設定 ====================
		//inTextDrawDataに記載された属性を設定
		for( AIndex i = 0; i < inTextDrawData.attrPos.GetItemCount(); i++ )
		{
			// -------------------- Range取得 --------------------
			//lenに、処理するRunLengthの、UniCharでの長さを格納する。
			AItemCount	len;
			if( i+1 < inTextDrawData.attrPos.GetItemCount() )
			{
				//最後以外の要素の場合
				len = inTextDrawData.attrPos.Get(i+1) - inTextDrawData.attrPos.Get(i);
			}
			else
			{
				//最後の要素の場合
				len = inTextDrawData.UTF16DrawText.GetItemCount()/sizeof(UniChar) - inTextDrawData.attrPos.Get(i);
			}
			//RunLengthの長さが0なら何もしない
			if( len <= 0 )   continue;
			//range設定
			NSRange	range = NSMakeRange(inTextDrawData.attrPos.Get(i),len);
			
			// -------------------- 属性設定 --------------------
			
			//フォント設定
			if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Bold) == 0 )
			{
				if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Italic) == 0 )
				{
					//通常フォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetNormalFont() range:range];
				}
				else
				{
					//italicフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetItalicFont() range:range];
				}
			}
			else
			{
				if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Italic) == 0 )
				{
					//boldフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldFont() range:range];
				}
				else
				{
					//bold+italicフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldItalicFont() range:range];
				}
			}
			
			//アンダーライン
			if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_Underline) != 0 )
			{
				[attrString addAttribute:NSUnderlineStyleAttributeName 
				value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:range];
			}
			
			//ドロップシャドウ
			if( (inTextDrawData.attrStyle.Get(i)&kTextStyle_DropShadow) != 0 )
			{
				NSShadow *shadow = [[[NSShadow alloc] init] autorelease];
				[shadow setShadowColor:[NSColor colorWithCalibratedRed:(mDropShadowColor.red/65535.0) 
				green:(mDropShadowColor.green/65535.0) 
				blue:(mDropShadowColor.blue/65535.0) 
				alpha:1.0]];
				[shadow setShadowBlurRadius:1.0];
				[shadow setShadowOffset:NSMakeSize(0.0f, -1.01f)];
				[attrString addAttribute:NSShadowAttributeName value:shadow range:range];
			}
			
			// 文字色設定
			RGBColor	c = inTextDrawData.attrColor.Get(i);
			[attrString addAttribute:NSForegroundColorAttributeName 
			value:[NSColor colorWithCalibratedRed:(c.red/65535.0) green:(c.green/65535.0) blue:(c.blue/65535.0) alpha:mTextAlpha] range:range];
			
		}
		
		//#1316
		// ==================== テキスト属性(RunStyle)設定　追加分 ====================
		//inTextDrawDataに記載された属性を設定
		for( AIndex i = 0; i < inTextDrawData.additionalAttrPos.GetItemCount(); i++ )
		{
			// -------------------- Range取得 --------------------
			//lenに、処理するRunLengthの、UniCharでの長さを格納する。
            AItemCount	len = inTextDrawData.additionalAttrLength.Get(i);
			//RunLengthの長さが0なら何もしない
			if( len <= 0 )   continue;
			//range設定
			NSRange	range = NSMakeRange(inTextDrawData.additionalAttrPos.Get(i),len);
			
			// -------------------- 属性設定 --------------------
			
			//フォント設定
			if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_Bold) == 0 )
			{
				if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_Italic) == 0 )
				{
					//通常フォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetNormalFont() range:range];
				}
				else
				{
					//italicフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetItalicFont() range:range];
				}
			}
			else
			{
				if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_Italic) == 0 )
				{
					//boldフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldFont() range:range];
				}
				else
				{
					//bold+italicフォント
					[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetBoldItalicFont() range:range];
				}
			}
			
			//アンダーライン
			if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_Underline) != 0 )
			{
				[attrString addAttribute:NSUnderlineStyleAttributeName 
				value:[NSNumber numberWithInteger:NSUnderlineStyleSingle] range:range];
			}
			
			//ドロップシャドウ
			if( (inTextDrawData.additionalAttrStyle.Get(i)&kTextStyle_DropShadow) != 0 )
			{
				NSShadow *shadow = [[[NSShadow alloc] init] autorelease];
				[shadow setShadowColor:[NSColor colorWithCalibratedRed:(mDropShadowColor.red/65535.0) 
						green:(mDropShadowColor.green/65535.0) 
						blue:(mDropShadowColor.blue/65535.0) 
				alpha:1.0]];
				[shadow setShadowBlurRadius:1.0];
				[shadow setShadowOffset:NSMakeSize(0.0f, -1.01f)];
				[attrString addAttribute:NSShadowAttributeName value:shadow range:range];
			}
			
			// 文字色設定
			RGBColor	c = inTextDrawData.additionalAttrColor.Get(i);
			[attrString addAttribute:NSForegroundColorAttributeName 
			value:[NSColor colorWithCalibratedRed:(c.red/65535.0) green:(c.green/65535.0) blue:(c.blue/65535.0) alpha:mTextAlpha] range:range];
			
		}
		
		//#558
		//縦書き属性設定
		if( mVerticalMode == true )
		{
			AddVerticalAttribute(attrString,str.GetNSString());
		}
		//NSMutableAttributedStringの編集終了
		[attrString endEditing];
		
		//CTLine生成
		CTLineRef line = ::CTLineCreateWithAttributedString((CFAttributedStringRef)attrString);
		inTextDrawData.SetCTLineRef(line);
	}
	return inTextDrawData.GetCTLineRef();
}

//#558
/**
縦書き属性設定
*/
void	CUserPane::AddVerticalAttribute( NSMutableAttributedString *inAttrString, NSString* inString )
{
	//NSVerticalGlyphFormAttributeNameを設定
	NSRange	range = {0,[inString length]};
	[inAttrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
	
	/*#558 U+0400以上の場合のみ縦書き属性にするテスト
	→全体に縦書き設定してもフォント次第では、アルファベット等のみ横書き表示になってくれるので、全体に縦書き設定することにする
	ABool	verticalCharMode = false;
	AItemCount	count = [inString length];
	NSRange	range = {0,0};
	for( AIndex i = 0; i < count; i++ )
	{
		unichar	ch = [inString characterAtIndex:i];
		if( ch >= 0x400 )
		{
			if( verticalCharMode == false )
			{
				range.location = i;
			}
			verticalCharMode = true;
		}
		else
		{
			if( verticalCharMode == true )
			{
				range.length = i - range.location;
				//
				[inAttrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
			}
			verticalCharMode = false;
		}
	}
	if( verticalCharMode == true )
	{
		range.length = count - range.location;
		[inAttrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
	}
	*/
}

#if 0
/**
テキスト表示
*/
void	CUserPane::DrawTextCore( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AJustification inJustification,
								const ATSUTextLayout inTextLayout, const ABool inDrawSelection, 
								const AIndex inSelectionStart, const AIndex inSelectionEnd, const AColor inSelectionColor, const AFloatNumber inSelectionAlpha )
{
	OSStatus	status = noErr;
	
	//lineAscentは、ATSUGetUnjustifiedBounds()で取得すると、表示する文字によってascentが変化するので、下記で取得。
	ANumber	lineHeight,lineAscent;
	GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	
	//
	ANumber	x = inDrawRect.left;
	ANumber	y = inDrawRect.top;//#450
	if( inJustification != kJustification_Left )
	{
		ATSUTextMeasurement	start, end, ascent, descent;
		status = ::ATSUGetUnjustifiedBounds(inTextLayout,kATSUFromTextBeginning,kATSUToTextEnd,&start,&end,&ascent,&descent);
		//ANumber	textWidth = ConvertFixedToInt(end-start);
		{
			switch(inJustification)
			{
			  case kJustification_Right:
				{
					x = inDrawRect.right - ConvertFixedToInt(end-start);
					break;
				}
			  case kJustification_RightCenter://#450
				{
					x = inDrawRect.right - ConvertFixedToInt(end-start);
					y = inDrawRect.top + (inDrawRect.bottom-inDrawRect.top)/2 - lineHeight/2;
					break;
				}
			  case kJustification_Center:
				{
					x = (inDrawRect.left+inDrawRect.right)/2 - ConvertFixedToInt(end-start)/2;
					break;
				}
			  case kJustification_LeftTruncated://#315
				{
					if( ConvertFixedToInt(end-start) > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - ConvertFixedToInt(end-start);
					}
					break;
				}
			  case kJustification_CenterTruncated://#315
				{
					if( ConvertFixedToInt(end-start) > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - ConvertFixedToInt(end-start);
					}
					else
					{
						x = (inDrawRect.left+inDrawRect.right)/2 - ConvertFixedToInt(end-start)/2;
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
	::CGContextSaveGState(mContextRef);
	
	//Clip
	::CGContextBeginPath(mContextRef);
	::CGContextAddRect(mContextRef,GetCGRectFromARect(inClipRect));
	::CGContextClosePath(mContextRef);
	::CGContextClip(mContextRef);
	
	//Selection描画
	if( inDrawSelection == true )
	{
		ALocalRect	rect = inDrawRect;
		Boolean	caretIsSplit;
		ATSUCaret	mainCaret, secondCaret;
		if( inSelectionStart != kIndex_Invalid )
		{
			status = ::ATSUOffsetToPosition(inTextLayout,inSelectionStart,true,&mainCaret,&secondCaret,&caretIsSplit);
			rect.left = inDrawRect.left + ::Fix2Long(mainCaret.fX);
		}
		if( inSelectionEnd != kIndex_Invalid )
		{
			status = ::ATSUOffsetToPosition(inTextLayout,inSelectionEnd,true,&mainCaret,&secondCaret,&caretIsSplit);
			rect.right = inDrawRect.left + ::Fix2Long(mainCaret.fX);
		}
		PaintRect(rect,inSelectionColor,inSelectionAlpha);
	}
	
	//Text描画
	
	status = ::ATSUDrawText(inTextLayout,kATSUFromTextBeginning, kATSUToTextEnd,
							ConvertIntToFixed(x), -(ConvertIntToFixed(y)+ConvertIntToFixed(lineAscent)) );
	
	::CGContextRestoreGState(mContextRef);
	/*test
	::CGContextScaleCTM(mContextRef, 1.0, 1.0);
	AStCreateNSStringFromAText	str(inText);
	NSPoint	pt = {0};
	pt.x = x;
	pt.y = y;// + lineAscent;
	[str.GetNSString() drawAtPoint:pt withAttributes:nil];
	::CGContextScaleCTM(mContextRef, 1.0, -1.0);
	*/
	//TextLayout削除
	//高速化::ATSUDisposeTextLayout(textLayout);
	//textLayoutはCTextDrawDataのデストラクタで破棄する
	
	//UnitTest用
	mDrawTextCount++;
	
	//B0000 高速化 return textWidth;
}
#endif

//#1034
/**
テキスト表示
*/
void	CUserPane::DrawTextCore( const ALocalRect& inDrawRect, const ALocalRect& inClipRect, const AJustification inJustification,
								 const CTLineRef inCTLineRef, const ABool inDrawSelection, 
								 const AIndex inSelectionStart, const AIndex inSelectionEnd, const AColor inSelectionColor, const AFloatNumber inSelectionAlpha, const AFloatNumber inSelectionFrameAlpha, const AColor inSelectionBackgroundColor )//#1316
{
	OSStatus	status = noErr;
	
	//lineAscentは、ATSUGetUnjustifiedBounds()で取得すると、表示する文字によってascentが変化するので、下記で取得。
	AFloatNumber	lineHeight,lineAscent;//#1316 ANumber→AFloatNumber
	GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	
	//
	AFloatNumber	x = inDrawRect.left;//#1316 ANumber→AFloatNumber
	AFloatNumber	y = inDrawRect.top;//#450 #1316 ANumber→AFloatNumber
	if( inJustification != kJustification_Left )
	{
		CGFloat	ascent = 9, descent = 3, leading = 3;
		double	width = ::CTLineGetTypographicBounds(inCTLineRef,&ascent,&descent,&leading);
		{
			switch(inJustification)
			{
			  case kJustification_Right:
				{
					x = inDrawRect.right - width;
					break;
				}
			  case kJustification_RightCenter://#450
				{
					x = inDrawRect.right - width;
					y = inDrawRect.top + (inDrawRect.bottom-inDrawRect.top)/2 - lineHeight/2;
					break;
				}
			  case kJustification_Center:
				{
					x = (inDrawRect.left+inDrawRect.right)/2 - width/2;
					break;
				}
			  case kJustification_LeftTruncated://#315
				{
					if( width > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - width;
					}
					break;
				}
			  case kJustification_CenterTruncated://#315
				{
					if( width > inDrawRect.right-inDrawRect.left )
					{
						x = inDrawRect.right - width;
					}
					else
					{
						x = (inDrawRect.left+inDrawRect.right)/2 - width/2;
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
	::CGContextSaveGState(mContextRef);
	
	//Clip
	::CGContextBeginPath(mContextRef);
	::CGContextAddRect(mContextRef,GetCGRectFromARect(inClipRect));
	::CGContextClosePath(mContextRef);
	::CGContextClip(mContextRef);
	
	//Selection描画
	if( inDrawSelection == true )
	{
		ALocalRect	rect = inDrawRect;
		if( inSelectionStart != kIndex_Invalid )
		{
			CGFloat	offset2 = 0;
			CGFloat	offset = ::CTLineGetOffsetForStringIndex(inCTLineRef,inSelectionStart,&offset2);
			rect.left = inDrawRect.left + offset;
		}
		if( inSelectionEnd != kIndex_Invalid )
		{
			CGFloat	offset2 = 0;
			CGFloat	offset = ::CTLineGetOffsetForStringIndex(inCTLineRef,inSelectionEnd,&offset2);
			rect.right = inDrawRect.left + offset;
		}
		if( inSelectionAlpha > 0.0 )
		{
			//選択色
			PaintRect(rect,inSelectionColor,inSelectionAlpha);
		}
		if( inSelectionFrameAlpha > 0.0 )
		{
			//すでに描画されている透明色と重なると文字が見えづらくなるので、一旦背景色で不透明ペイントする。
			PaintRect(rect,inSelectionBackgroundColor,1.0);
			//選択色
			PaintRect(rect,inSelectionColor,inSelectionAlpha);
			//フレーム
			FrameRect(rect,inSelectionColor,inSelectionFrameAlpha,true,true,true,true,1.0);
		}
	}
	
	//Text描画
	::CGContextSetTextMatrix(mContextRef, CGAffineTransformIdentity);
	AFloatNumber	texty = y+lineAscent;//#1316 ANumber→AFloatNumber
	if( mVerticalMode == true )
	{
		//縦書きの場合ちょい下（左）になるようにする（とりあえず試行錯誤による調整・・・） #558
		texty = y+lineAscent + (lineHeight-lineAscent)/4;
	}
	::CGContextSetTextPosition(mContextRef,x,-(texty));
	::CTLineDraw(inCTLineRef,mContextRef);
	
	::CGContextRestoreGState(mContextRef);
	
	//CTLineRefはCTextDrawDataのデストラクタで破棄する
	
	//UnitTest用
	mDrawTextCount++;
}

#if 0
/**
TextLayout生成
*/
ATSUTextLayout	CUserPane::CreateTextLayout( const AText& inUTF16Text, const ABool inUseFontFallback ) const //#703
{
	return CWindowImp::CreateTextLayout(/*#852 mATSUTextStyle*/GetATSUTextStyle(),inUTF16Text,mAntiAliasing,inUseFontFallback,mContextRef);
}
#endif

//#1012
#if 0 
/**
Icon描画
*/
void	CUserPane::DrawIcon( const ALocalRect& inRect, const AIconID inIconID, const ABool inEnabledColor,
		const ABool inDefaultSize ) const //win 080722
{
	OSStatus	err = noErr;
	
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	IconTransformType	type = kTransformNone;
	if( inEnabledColor == false )
	{
		type = kTransformDisabled;
	}
	AIconRef	iconRef = CWindowImp::GetIconRef(inIconID);
	if( iconRef != NULL )
	{
		CGRect	cgrect = GetCGRectFromARect(inRect);
		err = ::PlotIconRefInContext(mContextRef,&cgrect,/*kAlignAbsoluteCenter*/kAlignNone,type,/*#232 &kColor_White*/NULL,kPlotIconRefNormalFlags,iconRef);
		if( err != noErr )   _ACErrorNum("PlotIconRefInContext() returned error: ",err,this);
	}
	else
	{
		RgnHandle	svRgn;
		svRgn = ::NewRgn();
		::GetClip(svRgn);
		
		AWindowRect	localFrameWindowRect;
		GetControlWindowFrameRect(localFrameWindowRect);
		Rect	frameRect;
		frameRect.left		= localFrameWindowRect.left;
		frameRect.top		= localFrameWindowRect.top;
		frameRect.right		= localFrameWindowRect.right;
		frameRect.bottom	= localFrameWindowRect.bottom;
		::ClipRect(&frameRect);
		
		//
		CIconHandle	iconhandle = CWindowImp::GetCIconHandle(inIconID);
		AWindowRect	windowRect;
		GetWinConst().GetWindowRectFromControlLocalRect(GetControlID(),inRect,windowRect);
		Rect	rect;
		rect.left 		= ((windowRect.left+windowRect.right)/2)-16;
		rect.right		= ((windowRect.left+windowRect.right)/2)+16;
		rect.top		= ((windowRect.top+windowRect.bottom)/2)-16;
		rect.bottom		= ((windowRect.top+windowRect.bottom)/2)+16;
		err = ::PlotCIconHandle(&rect,kAlignAbsoluteCenter,type,iconhandle);
		if( err != noErr )   _ACErrorNum("PlotCIconHandle() returned error: ",err,this);
		
		//Clip復元
		::SetClip(svRgn);
		::DisposeRgn(svRgn);
	}
}
#endif

/**
Icon描画（ファイルから）
*/
void	CUserPane::DrawIconFromFile( const ALocalRect& inRect, const AFileAcc& inFile, const ABool inEnabledColor ) const
{
	OSStatus	err = noErr;
	
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	/*#1012
	FSSpec	fsspec;
	inFile.GetFSSpec(fsspec);
	IconRef	iconRef;
	SInt16	label;
	if( ::GetIconRefFromFile(&fsspec,&iconRef,&label) == noErr )
	{
		CGRect	cgrect = GetCGRectFromARect(inRect);
		IconTransformType	type = kTransformNone;
		if( inEnabledColor == false )
		{
			type = kTransformDisabled;
		}
		err = ::PlotIconRefInContext(mContextRef,&cgrect,kAlignAbsoluteCenter,type,&kColor_White,kPlotIconRefNormalFlags,iconRef);
	}
	*/
	//ファイルパス取得
	AText	path;
	inFile.GetPath(path);
	AStCreateNSStringFromAText	str(path);
	//ファイルアイコンのimage取得
	NSImage*	image = [[NSWorkspace sharedWorkspace] iconForFile:str.GetNSString()];
	//描画先サイズ取得
	ANumber	width = inRect.right-inRect.left;
	ANumber	height = inRect.bottom-inRect.top;
	//描画先rect取得
	NSRect	drawrect = {0};
	drawrect.origin.x = inRect.left;
	drawrect.origin.y = -(inRect.top+height);
	drawrect.size.width 	= width;
	drawrect.size.height 	= height;
	//イメージrect
	NSRect	srcrect = {0};
	srcrect.origin.x = 0;
	srcrect.origin.y = 0;
	srcrect.size.width 		= [image size].width;
	srcrect.size.height 	= [image size].height;
	//イメージ描画
	[image drawInRect:drawrect fromRect:srcrect operation:NSCompositeSourceOver fraction:1.0];
}

/**
多角形描画
*/
void	CUserPane::PaintPoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBFillColor(mContextRef,red,green,blue,inAlpha);
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inPath.Get(inPath.GetItemCount()-1).x,-inPath.Get(inPath.GetItemCount()-1).y-1);
	for( AIndex i = 0; i < inPath.GetItemCount(); i++ )
	{
		::CGContextAddLineToPoint(mContextRef,inPath.Get(i).x,-inPath.Get(i).y-1);
	}
	::CGContextSetShouldAntialias(mContextRef,true);
	::CGContextFillPath(mContextRef);
	::CGContextRestoreGState(mContextRef);
}

/**
多角形Frame描画
*/
void	CUserPane::FramePoly( const AArray<ALocalPoint>& inPath, const AColor& inColor, const AFloatNumber inAlpha )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inPath.Get(inPath.GetItemCount()-1).x,-inPath.Get(inPath.GetItemCount()-1).y-1);
	for( AIndex i = 0; i < inPath.GetItemCount(); i++ )
	{
		::CGContextAddLineToPoint(mContextRef,inPath.Get(i).x,-inPath.Get(i).y-1);
	}
	::CGContextSetShouldAntialias(mContextRef,true);
	::CGContextStrokePath(mContextRef);
	::CGContextRestoreGState(mContextRef);
}

//#724
/**
PaintRoundedRectをpaint
*/
void	CUserPane::PaintRoundedRect( const ALocalRect& inRect, 
									const AColor& inStartColor, const AColor& inEndColor, const AGradientType inGradientType, 
									const AFloatNumber inStartAlpha, const AFloatNumber inEndAlpha,
									const ANumber inR, 
									const ABool inLeftTopSide, const ABool inRightTopSide, 
									const ABool inLeftBottomSide, const ABool inRightBottomSide )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	//色設定
	float	red = inStartColor.red;
	float	green = inStartColor.green;
	float	blue = inStartColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBFillColor(mContextRef,red,green,blue,(inStartAlpha+inEndAlpha)/2.0);
	//パス開始
	::CGContextBeginPath(mContextRef);
	//
	CGFloat	left = inRect.left;
	CGFloat	right = inRect.right;
	CGFloat	top = -inRect.top;
	CGFloat	bottom = -inRect.bottom;
	CGFloat	rad = (top-bottom)/2;
	if( inR < rad )
	{
		rad = inR;
	}
	CGFloat	x1 = left + rad;
	CGFloat	x2 = right - rad;
	CGFloat	y1 = top - rad;
	CGFloat y2 = bottom + rad;
	//上の線
	::CGContextMoveToPoint(mContextRef,x2,top);
	::CGContextAddLineToPoint(mContextRef,x1,top);
	//左上
	if( inLeftTopSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,left,top,left,y1,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,left,top);
	}
	//左の線
	::CGContextAddLineToPoint(mContextRef,left,y2);
	//左下
	if( inLeftBottomSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,left,bottom,x1,bottom,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,left,bottom);
	}
	//下の線
	::CGContextAddLineToPoint(mContextRef,x2,bottom);
	//右下
	if( inRightBottomSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,right,bottom,right,y2,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,right,bottom);
	}
	//右の線
	::CGContextAddLineToPoint(mContextRef,right,y1);
	//右上
	if( inRightTopSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,right,top,x2,top,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,right,top);
		::CGContextAddLineToPoint(mContextRef,x2,top);
	}
	//パス終了
	::CGContextClosePath(mContextRef);
	//アンチエイリアスON
	::CGContextSetShouldAntialias(mContextRef,true);
	//gradient有無
	if( inGradientType == kGradientType_None || AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_5 ||
		(inStartColor.red == inEndColor.red && inStartColor.green == inEndColor.green && inStartColor.blue == inEndColor.blue &&
		 inStartAlpha == inEndAlpha ) ) //#1275 startとendの色・アルファが同じ場合はgradientなしとして扱う
	{
		//gradient無し
		
		::CGContextFillPath(mContextRef);
	}
	else
	{
		//gradient有り
		//CGContextDrawLinearGradient()は10.5未満は未対応
		
		::CGContextClip(mContextRef);
		//gradient生成
		CGColorRef	topColor = CreateCGColor(inStartColor,inStartAlpha);
		CGColorRef	bottomColor = CreateCGColor(inEndColor,inEndAlpha);
		const void *colorRefs[2] = {topColor,bottomColor};
		CFArrayRef colorArray = ::CFArrayCreate(kCFAllocatorDefault, colorRefs, 2, &kCFTypeArrayCallBacks);
		CGFloat locations[2] = {0.0,1.0};
		CGGradientRef gradient = ::CGGradientCreateWithColors(NULL, colorArray, locations);
		//gradient描画
		CGPoint	spt = {inRect.left,-inRect.top};
		CGPoint	ept = {inRect.right,-inRect.top};
		if( inGradientType == kGradientType_Vertical )
		{
			spt.x = inRect.left;
			spt.y = -inRect.top;
			ept.x = inRect.left;
			ept.y = -inRect.bottom;
		}
		::CGContextDrawLinearGradient(mContextRef,gradient,spt,ept,0);
		//gradient解放
		::CGGradientRelease(gradient);
		::CFRelease(colorArray);
		::CFRelease(bottomColor);
		::CFRelease(topColor);
	}
	//GStateリストア
	::CGContextRestoreGState(mContextRef);
}

//#724
/**
PaintRoundedRectをframe
*/
void	CUserPane::FrameRoundedRect( const ALocalRect& inRect, 
									const AColor& inColor, const AFloatNumber inAlpha,
									const ANumber inR, 
									const ABool inLeftTopSide, const ABool inRightTopSide, 
									const ABool inLeftBottomSide, const ABool inRightBottomSide,
									const ABool inDrawLeftLine, const ABool inDrawTopLine,
									const ABool inDrawRightLine, const ABool inDrawBottomLine,
									const AFloatNumber inLineWidth )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	//色設定
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	/*test
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		CGSize	offset = {1.5,-1.5};
		CGColorRef	color = CreateCGColor(mDropShadowColor,1.0);//
		::CGContextSetShadowWithColor(mContextRef,offset,1.0,color);
		::CFRelease(color);
	}
	*/
	//パス開始
	::CGContextBeginPath(mContextRef);
	//
	ALocalRect	rect = inRect;
	/*test
	rect.top		+= inLineWidth;
	rect.left		+= inLineWidth;
	rect.bottom		-= inLineWidth;
	rect.right		-= inLineWidth;
	*/
	//
	CGFloat	left = rect.left;
	CGFloat	right = rect.right;
	CGFloat	top = -rect.top;
	CGFloat	bottom = -rect.bottom;
	CGFloat	rad = (top-bottom)/2;
	if( inR < rad )
	{
		rad = inR;
	}
	CGFloat	x1 = left + rad;
	CGFloat	x2 = right - rad;
	CGFloat	y1 = top - rad;
	CGFloat y2 = bottom + rad;
	//上の線
	if( inDrawTopLine == true )
	{
		::CGContextMoveToPoint(mContextRef,x2,top);
		::CGContextAddLineToPoint(mContextRef,x1,top);
	}
	else
	{
		::CGContextMoveToPoint(mContextRef,x1,top);
	}
	//左上
	if( inLeftTopSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,left,top,left,y1,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,left,top);
	}
	//左の線
	if( inDrawLeftLine == true )
	{
		::CGContextAddLineToPoint(mContextRef,left,y2);
	}
	else
	{
		::CGContextMoveToPoint(mContextRef,left,y2);
	}
	//左下
	if( inLeftBottomSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,left,bottom,x1,bottom,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,left,bottom);
	}
	//下の線
	if( inDrawBottomLine == true )
	{
		::CGContextAddLineToPoint(mContextRef,x2,bottom);
	}
	else
	{
		::CGContextMoveToPoint(mContextRef,x2,bottom);
	}
	//右下
	if( inRightBottomSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,right,bottom,right,y2,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,right,bottom);
	}
	//右の線
	if( inDrawRightLine == true )
	{
		::CGContextAddLineToPoint(mContextRef,right,y1);
	}
	else
	{
		::CGContextMoveToPoint(mContextRef,right,y1);
	}
	//右上
	if( inRightTopSide == true )
	{
		::CGContextAddArcToPoint(mContextRef,right,top,x2,top,rad);
	}
	else
	{
		::CGContextAddLineToPoint(mContextRef,right,top);
		::CGContextAddLineToPoint(mContextRef,x2,top);
	}
	//パス終了
	//::CGContextClosePath(mContextRef);
	//アンチエイリアスON
	::CGContextSetShouldAntialias(mContextRef,true);
	//線幅0.2
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	//描画
	::CGContextStrokePath(mContextRef);
	//GStateリストア
	::CGContextRestoreGState(mContextRef);
}

/**
*/
void	CUserPane::EraseRect( const ALocalRect& inRect )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	/*frame viewが黒消去になってしまう
	::CGContextSaveGState(mContextRef);
	::CGContextClearRect(mContextRef,GetCGRectFromARect(inRect));
	::CGContextRestoreGState(mContextRef);
	*/
	//とりあえず処理無しとする
}

/**
Rect塗りつぶし
*/
void	CUserPane::PaintRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha,
		const ABool inDropShadow )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	if( inDropShadow == true && AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		CGSize	offset = {1.5,-1.5};
		CGColorRef	color = CreateCGColor(mDropShadowColor,1.0);//
		::CGContextSetShadowWithColor(mContextRef,offset,1.0,color);
		::CFRelease(color);
	}
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBFillColor(mContextRef,red,green,blue,inAlpha);
	//#1275 NSBezierPathの描画パラメータの方が速いかもしれないのでNSBezierPathを使うことにする ::CGContextFillRect(mContextRef,GetCGRectFromARect(inRect));
	::CGContextSetShouldAntialias(mContextRef,false);
	[NSBezierPath fillRect:NSRectFromCGRect(GetCGRectFromARect(inRect))];//#1275
	::CGContextRestoreGState(mContextRef);
}

//#634
/**
垂直方向グラデーションでPaintRect
*/
void	CUserPane::PaintRectWithVerticalGradient( const ALocalRect& inRect, 
		const AColor& inTopColor, const AColor& inBottomColor, 
		const AFloatNumber inTopAlpha, const AFloatNumber inBottomAlpha )
{
	/*
	if( inDrawUsingLines == true )
	{
		//線を複数書くことで実現する
		//OS X 10.5未満でも使用可能。
		//ただし行数が多いと処理時間がかかる
		AFloatNumber	percent = 0;
		AFloatNumber	h = (inRect.bottom-inRect.top-1);
		AFloatNumber	percentInc = 100.0/h;
		for( ANumber y = inRect.top; y < inRect.bottom; y++ )
		{
			AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,percent);
			ALocalPoint	spt = {inRect.left,y};
			ALocalPoint	ept = {inRect.right-1,y};
			DrawLine(spt,ept,color,inAlpha);
			percent += percentInc;
		}
	}
	else
	{
	*/
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_5 )
	{
		//CGContextDrawLinearGradient()は10.5未満は未対応なので50%でPaintする
		AColor	color = AColorWrapper::GetGradientColor(inTopColor,inBottomColor,50.0);
		PaintRect(inRect,color,(inTopAlpha+inBottomAlpha)/2.0);
	}
	else
	{
		if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
		
		//GState保存
		::CGContextSaveGState(mContextRef);
		//Rectパス
		::CGContextBeginPath(mContextRef);
		::CGContextAddRect(mContextRef,GetCGRectFromARect(inRect));
		::CGContextClip(mContextRef);
		//gradient生成
		CGColorRef	topColor = CreateCGColor(inTopColor,inTopAlpha);
		CGColorRef	bottomColor = CreateCGColor(inBottomColor,inBottomAlpha);
		const void *colorRefs[2] = {topColor,bottomColor};
		CFArrayRef colorArray = ::CFArrayCreate(kCFAllocatorDefault, colorRefs, 2, &kCFTypeArrayCallBacks);
		CGFloat locations[2] = {0.0,1.0};
		CGGradientRef gradient = ::CGGradientCreateWithColors(NULL, colorArray, locations);
		//gradient描画
		CGPoint	spt = {inRect.left,-inRect.top};
		CGPoint	ept = {inRect.left,-inRect.bottom};
		::CGContextDrawLinearGradient(mContextRef,gradient,spt,ept,0);
		//gradient解放
		::CGGradientRelease(gradient);
		::CFRelease(colorArray);
		::CFRelease(bottomColor);
		::CFRelease(topColor);
		//GStateリストア
		::CGContextRestoreGState(mContextRef);
	}
	//}
}

//#634
/**
水平方向グラデーションでPaintRect
*/
void	CUserPane::PaintRectWithHorizontalGradient( const ALocalRect& inRect, 
		const AColor& inLeftColor, const AColor& inRightColor, const AFloatNumber inLeftAlpha, const AFloatNumber inRightAlpha )
{
	if( AEnvWrapper::GetOSVersion() < kOSVersion_MacOSX_10_5 )
	{
		//CGContextDrawLinearGradient()は10.5未満は未対応なので50%でPaintする
		AColor	color = AColorWrapper::GetGradientColor(inLeftColor,inRightColor,50.0);
		PaintRect(inRect,color,(inLeftAlpha+inRightAlpha)/2.0);
	}
	else
	{
		if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
		
		//GState保存
		::CGContextSaveGState(mContextRef);
		//Rectパス
		::CGContextBeginPath(mContextRef);
		::CGContextAddRect(mContextRef,GetCGRectFromARect(inRect));
		::CGContextClip(mContextRef);
		//gradient生成
		CGColorRef	topColor = CreateCGColor(inLeftColor,inLeftAlpha);
		CGColorRef	bottomColor = CreateCGColor(inRightColor,inRightAlpha);
		const void *colorRefs[2] = {topColor,bottomColor};
		CFArrayRef colorArray = ::CFArrayCreate(kCFAllocatorDefault, colorRefs, 2, &kCFTypeArrayCallBacks);
		CGFloat locations[2] = {0.0,1.0};
		CGGradientRef gradient = ::CGGradientCreateWithColors(NULL, colorArray, locations);
		//gradient描画
		CGPoint	spt = {inRect.left,-inRect.top};
		CGPoint	ept = {inRect.right,-inRect.top};
		::CGContextDrawLinearGradient(mContextRef,gradient,spt,ept,0);
		//gradient解放
		::CGGradientRelease(gradient);
		::CFRelease(colorArray);
		::CFRelease(bottomColor);
		::CFRelease(topColor);
		//GStateリストア
		::CGContextRestoreGState(mContextRef);
	}
}

//#634
/**
AColorからCGColor取得
CGColorはコール元で解放必要
@note Mac OS X 10.5以上のみ使用可能
*/
CGColorRef	CUserPane::CreateCGColor( const AColor inColor, const AFloatNumber inAlpha ) const
{
	CGFloat	red = inColor.red;
	CGFloat	green = inColor.green;
	CGFloat	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	return ::CGColorCreateGenericRGB(red,green,blue,inAlpha);
}

/**
Rectのフレームを描画
*/
void	CUserPane::FrameRect( const ALocalRect& inRect, const AColor& inColor, const AFloatNumber inAlpha,
							 const ABool inDrawLeftLine, const ABool inDrawTopLine,
							 const ABool inDrawRightLine, const ABool inDrawBottomLine,
							 const AFloatNumber inLineWidth )
{
	//Retinaでの表示残りを発生させないため、線で実現するのではなく、rectで実現する
	if( inDrawLeftLine == true )
	{
		ALocalRect	leftEdge = inRect;
		leftEdge.right = leftEdge.left+inLineWidth;
		if( inDrawTopLine == true )
		{
			leftEdge.top += inLineWidth;//#1316 透明色で描くと四隅が濃くなる問題対策
		}
		if( inDrawBottomLine == true )
		{
			leftEdge.bottom -= inLineWidth;//#1316 透明色で描くと四隅が濃くなる問題対策
		}
		PaintRect(leftEdge,inColor,inAlpha);
	}
	if( inDrawRightLine == true )
	{
		ALocalRect	rightEdge = inRect;
		rightEdge.left = rightEdge.right-inLineWidth;
		if( inDrawTopLine == true )
		{
			rightEdge.top += inLineWidth;//#1316 透明色で描くと四隅が濃くなる問題対策
		}
		if( inDrawBottomLine == true )
		{
			rightEdge.bottom -= inLineWidth;//#1316 透明色で描くと四隅が濃くなる問題対策
		}
		PaintRect(rightEdge,inColor,inAlpha);
	}
	if( inDrawTopLine == true )
	{
		ALocalRect	topEdge = inRect;
		topEdge.bottom = topEdge.top+inLineWidth;
		PaintRect(topEdge,inColor,inAlpha);
	}
	if( inDrawBottomLine == true )
	{
		ALocalRect	bottomEdge = inRect;
		bottomEdge.top = bottomEdge.bottom-inLineWidth;
		PaintRect(bottomEdge,inColor,inAlpha);
	}
}

//#871
/**
円を描画
*/
void	CUserPane::DrawCircle( const ALocalRect& inDrawRect, const AColor& inColor, const ANumber inLineWidth, const AFloatNumber inAlpha )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextBeginPath(mContextRef);
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	::CGContextSetShouldAntialias(mContextRef,true);
	CGPoint	pt = {0};
	pt.x = (inDrawRect.left+inDrawRect.right);
	pt.x /= 2;
	pt.y = (inDrawRect.top+inDrawRect.bottom);
	pt.y /= 2;
	CGFloat	radius = (inDrawRect.right-inDrawRect.left)/2;
	::CGContextAddArc(mContextRef,pt.x,-pt.y,radius,0,2*3.15,0);
	::CGContextStrokePath(mContextRef);
	::CGContextRestoreGState(mContextRef);
	//::CGContextFlush(mContextRef);
}

/**
フォーカスリングを描画
*/
void	CUserPane::DrawFocusFrame( const ALocalRect& inRect )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	::CGContextSetShouldAntialias(mContextRef,true);
	::CGContextSetRGBFillColor(mContextRef,1.0,1.0,1.0,0.0);
	//反転座標系ではウインドウの上のほうでフォーカスリングを書くと下が欠けてしまうので、非反転座標系に戻す #1082
	::CGContextScaleCTM(mContextRef, 1.0, -1.0);
	//非反転座標系のrect取得
	CGRect	r;
	r.origin.x = inRect.left;
	r.origin.y = inRect.top;
	r.size.width = inRect.right-inRect.left;
	r.size.height = inRect.bottom-inRect.top;
	//フォーカスリング描画
	NSSetFocusRingStyle(NSFocusRingOnly);
	::CGContextFillRect(mContextRef,r);
	::CGContextRestoreGState(mContextRef);
}

/**
inDrawRectで指定する領域を背景画像で描画する
*/
void	CUserPane::DrawBackgroundImage( const ALocalRect& inEraseRect, const AImageRect& inImageFrameRect, 
		const ANumber inLeftOffset, const AIndex inBackgroundImageIndex, const AFloatNumber inAlpha )
{
	if( true )
	{
		GetWin().DrawBackgroundImage(mControlID,inEraseRect,inImageFrameRect,inLeftOffset,inBackgroundImageIndex,inAlpha);
		PaintRect(inEraseRect,kColor_White,inAlpha);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		StSetPort	s(GetWindowRef());
		
		//
		AWindowRect	eraseWindowRect;
		GetWin().GetWindowRectFromControlLocalRect(GetControlID(),inEraseRect,eraseWindowRect);
		Rect	eraseRect;
		eraseRect.left		= eraseWindowRect.left;
		eraseRect.top		= eraseWindowRect.top;
		eraseRect.right		= eraseWindowRect.right;
		eraseRect.bottom	= eraseWindowRect.bottom;
		
		//Paneの左上のポイントのウインドウ座標をlocalOriginWindowPointに取得する
		ALocalPoint	localOrigin = {0,0};
		AWindowPoint	localOriginWindowPoint;
		GetWin().GetWindowPointFromControlLocalPoint(GetControlID(),localOrigin,localOriginWindowPoint);
		
		//UpdateRegionと、DrawRectとの重複部分でClipする。
		//UpdateRegionでClipしないと、CGContextのほうはデフォルトでupdateエリアでクリップされているので、bitmapだけコピーされるエリアが出来てしまう。
		//また、mDrawUpdateRegionはControlのローカル座標系なので、ウインドウ座標系に変換する。
		RgnHandle	svRgn = ::NewRgn();
		::GetClip(svRgn);
		RgnHandle	updateWindowRgn = ::NewRgn();
		::CopyRgn(mDrawUpdateRegion,updateWindowRgn);
		::OffsetRgn(updateWindowRgn,localOriginWindowPoint.x,localOriginWindowPoint.y);
		RgnHandle	rectrgn = ::NewRgn();
		::RectRgn(rectrgn,&eraseRect);
		RgnHandle	rgn = ::NewRgn();
		::SectRgn(updateWindowRgn,rectrgn,rgn);
		::SetClip(rgn);
		::DisposeRgn(updateWindowRgn);
		::DisposeRgn(rgn);
		::DisposeRgn(rectrgn);
		
		GWorldPtr	gw;
		Rect	picrect;
		GetWin().GetBackgroundData(inBackgroundImageIndex,gw,picrect);
		
		PixMapHandle	pix = ::GetGWorldPixMap(gw);
		
		short	picwidth,picheight;
		picwidth = picrect.right - picrect.left;
		picheight = picrect.bottom - picrect.top;
		if( picwidth == 0 )   picwidth = 1;
		
		ANumber	startx = inImageFrameRect.left/picwidth;
		startx *= picwidth;
		ANumber	starty = inImageFrameRect.top/picheight;
		starty *= picheight;
		for( ANumber x = startx; x < inImageFrameRect.right + inLeftOffset; x += picwidth )
		{
			for( ANumber y = starty; y < inImageFrameRect.bottom; y += picheight )
			{
				//Imageを貼る位置をImage座標系で取得
				AImageRect	pictimage;
				pictimage.left 		= x - inLeftOffset;
				pictimage.top		= y;
				pictimage.right		= x - inLeftOffset + picwidth;
				pictimage.bottom	= y + picheight;
				//Window座標系に変換
				Rect	rect;
				rect.left		= pictimage.left 	- inImageFrameRect.left 	+ localOriginWindowPoint.x;
				rect.top		= pictimage.top		- inImageFrameRect.top 		+ localOriginWindowPoint.y;
				rect.right		= pictimage.right	- inImageFrameRect.left 	+ localOriginWindowPoint.x;
				rect.bottom		= pictimage.bottom	- inImageFrameRect.top 		+ localOriginWindowPoint.y;
				Rect	tmpRect;
				if( ::SectRect(&eraseRect,&rect,&tmpRect) == true ) 
				{
					::LockPixels(pix);
					::CopyBits((BitMap*)*pix,::GetPortBitMapForCopyBits(::GetWindowPort(GetWindowRef())),&picrect,&rect,srcCopy,NULL);
					::UnlockPixels(pix);
				}
			}
		}
		::SetClip(svRgn);
		::DisposeRgn(svRgn);
		
		if( true )
		{
			PaintRect(inEraseRect,kColor_White,inAlpha);
		}
	}
#endif
}

/**
線描画
*/
void	CUserPane::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
		const AFloatNumber inAlpha, const AFloatNumber inLineDash, const AFloatNumber inLineWidth,
		const ABool inDropShadow )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	//#924
	//縦線、横線はPaintRectで対応する
	if( inStartPoint.x == inEndPoint.x && inLineDash == 0.0 )
	{
		//縦線
		ALocalRect	rect = {0};
		rect.left		= inStartPoint.x;
		rect.top		= inStartPoint.y;
		rect.right		= inEndPoint.x + inLineWidth;
		rect.bottom		= inEndPoint.y + inLineWidth;
		PaintRect(rect,inColor,inAlpha,inDropShadow);
		return;
	}
	if( inStartPoint.y == inEndPoint.y && inLineDash == 0.0 )
	{
		//横線
		ALocalRect	rect = {0};
		rect.left		= inStartPoint.x;
		rect.top		= inStartPoint.y;
		rect.right		= inEndPoint.x + inLineWidth;
		rect.bottom		= inEndPoint.y + inLineWidth;
		PaintRect(rect,inColor,inAlpha,inDropShadow);
		return;
	}
	
	::CGContextSaveGState(mContextRef);
	//
	if( inDropShadow == true && AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		CGSize	offset = {1.5,-1.5};
		CGColorRef	color = CreateCGColor(mDropShadowColor,1.0);
		::CGContextSetShadowWithColor(mContextRef,offset,1.0,color);
		::CFRelease(color);
	}
	//
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	/*#1275 NSBezierPathの描画パラメータの方が速いかもしれないのでNSBezierPathを使うことにする
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inStartPoint.x,-inStartPoint.y-1);//win 080618
	::CGContextAddLineToPoint(mContextRef,inEndPoint.x,-inEndPoint.y-1);//win 080618
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	::CGContextSetShouldAntialias(mContextRef,false);
	if( inLineDash != 0.0 )
	{
		CGFloat	lengths[2];
		lengths[0] = inLineDash;
		lengths[1] = inLineDash;
		::CGContextSetLineDash(mContextRef,0.0,lengths,2);
	}
	::CGContextStrokePath(mContextRef);
	*/
	NSBezierPath*	path = [[[NSBezierPath alloc] init] autorelease];
	[path moveToPoint:NSMakePoint(inStartPoint.x,-inStartPoint.y-1)];
	[path lineToPoint:NSMakePoint(inEndPoint.x,-inEndPoint.y-1)];
	[path setLineWidth:inLineWidth];
	if( inLineDash != 0.0 )
	{
		CGFloat	lengths[2];
		lengths[0] = inLineDash;
		lengths[1] = inLineDash;
        [path setLineDash:lengths count:2 phase:0.0];
	}
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextSetShouldAntialias(mContextRef,false);
	[path stroke];
	::CGContextRestoreGState(mContextRef);
}

//#723
/**
曲線描画
*/
void	CUserPane::DrawCurvedLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const AColor& inColor, 
		const AFloatNumber inAlpha, const AFloatNumber inLineDash, const AFloatNumber inLineWidth )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	//xかyが同じ時は通常の直線にする
	if( inStartPoint.x == inEndPoint.x || inStartPoint.y == inEndPoint.y )
	{
		ALocalPoint	spt = inStartPoint;
		ALocalPoint	ept = inEndPoint;
		ept.x -= inLineWidth;
		DrawLine(spt,ept,inColor,inAlpha,inLineDash,inLineWidth);
		return;
	}
	
	//開始位置・終了位置・中点
	CGPoint	startPoint = {inStartPoint.x,-inStartPoint.y-1};
	CGPoint	endPoint = {inEndPoint.x,-inEndPoint.y-1};
	CGPoint	middlePoint = {(startPoint.x+endPoint.x)/2,(startPoint.y+endPoint.y)/2};
	//gstate保存
	::CGContextSaveGState(mContextRef);
	//色取得
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	//パス開始
	::CGContextBeginPath(mContextRef);
	//ベジェ描画
	::CGContextMoveToPoint(mContextRef,startPoint.x,startPoint.y);
	::CGContextAddArcToPoint(mContextRef,middlePoint.x,startPoint.y,middlePoint.x,middlePoint.y,5.0);
	::CGContextAddArcToPoint(mContextRef,middlePoint.x,endPoint.y,endPoint.x,endPoint.y,5.0);
	//色設定
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	//行幅
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	//アンチエイリアスON
	::CGContextSetShouldAntialias(mContextRef,true);
	//line dash
	if( inLineDash != 0.0 )
	{
		CGFloat	lengths[2];
		lengths[0] = inLineDash;
		lengths[1] = inLineDash;
		::CGContextSetLineDash(mContextRef,0.0,lengths,2);
	}
	//パス描画
	::CGContextStrokePath(mContextRef);
	//gstate復帰
	::CGContextRestoreGState(mContextRef);
}

//B0000 高速化
/**
線描画モード設定
*/
void	CUserPane::SetDrawLineMode( const AColor& inColor, const AFloatNumber inAlpha, 
		const AFloatNumber inLineDash, const AFloatNumber inLineWidth )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextSaveGState(mContextRef);
	float	red = inColor.red;
	float	green = inColor.green;
	float	blue = inColor.blue;
	red /= 65535.0;
	green /= 65535.0;
	blue /= 65535.0;
	::CGContextSetRGBStrokeColor(mContextRef,red,green,blue,inAlpha);
	::CGContextSetLineWidth(mContextRef,inLineWidth);
	::CGContextSetShouldAntialias(mContextRef,false);
	if( inLineDash != 0.0 )
	{
		CGFloat	lengths[2];
		lengths[0] = inLineDash;
		lengths[1] = inLineDash;
		::CGContextSetLineDash(mContextRef,0.0,lengths,2);
	}
}

//B0000 高速化
/**
線描画モード解除
*/
void	CUserPane::RestoreDrawLineMode()
{
	::CGContextRestoreGState(mContextRef);
}

//B0000 高速化
/**
線描画
*/
void	CUserPane::DrawLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint )
{
	if( mContextRef == NULL )   {_ACError("not in APICB_DoDraw()",this);return;}
	
	::CGContextBeginPath(mContextRef);
	::CGContextMoveToPoint(mContextRef,inStartPoint.x,-inStartPoint.y-1);//win 080618
	::CGContextAddLineToPoint(mContextRef,inEndPoint.x,-inEndPoint.y-1);//win 080618
	::CGContextStrokePath(mContextRef);
}

/**
キャレット用XOR線描画（APICB_DoDraw()外の描画用）
*/
void	CUserPane::DrawXorCaretLine( const ALocalPoint& inStartPoint, const ALocalPoint& inEndPoint, const ABool inClipToFrame, 
									 const ABool inFlush, const ABool inDash, const ANumber inPenSize, const AFloatNumber inAlpha )//#1398
{
	//コントロール非表示時は何もしない
	if( IsControlVisible() == false )
	{
		return;
	}
	
	//#1012 10.3は対応しない if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )//CGContextSetBlendModeは10.4以降対応。Retina対応のためにはQDは使用出来ない。
	{
		//GraphicsContext取得
		//現在、drawRect()内なら、そのcontextを使う
		CGContextRef	contextRef = mContextRef;
		ABool	focusLocked = false;
		if( contextRef == nil )
		{
			//drawRect()外なら、viewをlockFocusして、context取得
			CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
			if( [view lockFocusIfCanDraw] == NO )   return;
			//context取得
			contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
			//drawRect()内の描画と合わせるため座標変換する
			::CGContextScaleCTM(contextRef, 1.0, -1.0);
			focusLocked = true;
			
			//#558
			//縦書き座標系にする
			if( mVerticalMode == true )
			{
				ChangeToVerticalCTM(contextRef);
			}
		}
		//save gstate
		::CGContextSaveGState(contextRef);
		//clip
		if( inClipToFrame == true )
		{
			//viewのサイズ取得
			ANumber	viewwidth = GetWin().GetControlWidth(mControlID);
			ANumber	viewheight = GetWin().GetControlHeight(mControlID);
			//縦書きモードの場合は、ビュー範囲を縦横変換 #558
			if( mVerticalMode == true )
			{
				ANumber	swap = viewwidth;
				viewwidth = viewheight;
				viewheight = swap;
			}
			//ビューにclip
			CGRect	cliprect = {0};
			cliprect.origin.x = 0;
			cliprect.origin.y = -viewheight;
			cliprect.size.width = viewwidth;
			cliprect.size.height = viewheight;
			::CGContextClipToRect(contextRef,cliprect);
		}
		//描画
		::CGContextBeginPath(contextRef);
		::CGContextMoveToPoint(contextRef,inStartPoint.x,-inStartPoint.y-1);
		::CGContextAddLineToPoint(contextRef,inEndPoint.x,-inEndPoint.y-1);
		::CGContextSetRGBStrokeColor(contextRef,1.0,1.0,1.0,inAlpha);//#1398
		::CGContextSetLineWidth(contextRef,inPenSize);
		::CGContextSetShouldAntialias(contextRef,false);
		::CGContextSetBlendMode(contextRef,kCGBlendModeExclusion);//kCGBlendModeDifference);//kCGBlendModeExclusion);
		if( inDash == true )
		{
			CGFloat	lengths[2];
			lengths[0] = 1.0;
			lengths[1] = 1.0;
			::CGContextSetLineDash(contextRef,0.0,lengths,2);
		}
		::CGContextStrokePath(contextRef);
		::CGContextSynchronize(contextRef);
		//#1034
		if( inFlush == true )
		{
			::CGContextFlush(contextRef);
		}
		//restore gstate
		::CGContextRestoreGState(contextRef);
		//contextはrelease不要と思われる。このタイミングでautoreleaseするとエラー表示されるため。
		if( focusLocked == true )
		{
			CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
			[view unlockFocus];
		}
	}
	//#1012 10.3は対応しない
#if 0
	else
	{
		//caretタイマータイムアウト処理はAPICB_DoDraw()外なので、CGContextが使えない。
		StSetPort	s(GetWindowRef());
		//::RGBForeColor(&kColor_Black);
		
		//フレームでClip
		RgnHandle	svRgn;
		svRgn = ::NewRgn();
		::GetClip(svRgn);
		
		//clip
		if( inClipToFrame == true )
		{
			//Local座標でviewのフレームを取得
			ALocalRect	localFrame = {0};
			ANumber	viewwidth = GetWin().GetControlWidth(mControlID);
			ANumber	viewheight = GetWin().GetControlHeight(mControlID);
			localFrame.left		= 0;
			localFrame.top		= 0;
			localFrame.right	= viewwidth;
			localFrame.bottom	= viewheight;
			//Window座標へ変換
			AWindowRect	localFrameWindowRect = {0};
			GetWin().GetWindowRectFromControlLocalRect(mControlID,localFrame,localFrameWindowRect);
			//clip
			Rect	frameRect = {0};
			frameRect.left		= localFrameWindowRect.left;
			frameRect.top		= localFrameWindowRect.top;
			frameRect.right		= localFrameWindowRect.right;
			frameRect.bottom	= localFrameWindowRect.bottom;
			::ClipRect(&frameRect);
			if( mContextRef != NULL )
			{
				frameRect.left	= -1000;
				frameRect.top = -1000;
				frameRect.right = 1000;
				frameRect.bottom = 1000;
				::EraseRect(&frameRect);
			}
		}
		
		//引数をWindow座標系に変換して、描画
		AWindowPoint	spt,ept;
		GetWin().GetWindowPointFromControlLocalPoint(mControlID,inStartPoint,spt);
		GetWin().GetWindowPointFromControlLocalPoint(mControlID,inEndPoint,ept);
		if( inDash == true )
		{
			Pattern	pat;
			::StuffHex(&pat,"\pAA55AA55AA55AA55");
			::PenPat(&pat);
		}
		else
		{
			Pattern	pat;
			::PenPat(::GetQDGlobalsBlack(&pat));
		}
		::PenSize(inPenSize,inPenSize);
		::PenMode(patXor);
		::MoveTo(spt.x,spt.y);
		::LineTo(ept.x,ept.y);
		if( mContextRef != NULL )
		{
			::MoveTo(0,0);
			::LineTo(1000,1000);
			::MoveTo(100,0);
			::LineTo(1000,-1000);
		}
		::PenMode(0);
		::PenSize(1,1);
		
		//Clip復元
		::SetClip(svRgn);
		::DisposeRgn(svRgn);
	}
#endif
}

/**
キャレット用XOR線描画（APICB_DoDraw()外の描画用）
*/
void	CUserPane::DrawXorCaretRect( const ALocalRect& inLocalRect, const ABool inClipToFrame, const ABool inFlush )
{
	//コントロール非表示時は何もしない
	if( IsControlVisible() == false )
	{
		return;
	}
	//#1012 10.3は対応しない if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )//CGContextSetBlendModeは10.4以降対応。Retina対応のためにはQDは使用出来ない。
	{
		//GraphicsContext取得
		//現在、drawRect()内なら、そのcontextを使う
		CGContextRef	contextRef = mContextRef;
		ABool	focusLocked = false;
		if( contextRef == nil )
		{
			//drawRect()外なら、viewをlockFocusして、context取得
			CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
			[view lockFocus];
			//context取得
			contextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
			//drawRect()内の描画と合わせるため座標変換する
			::CGContextScaleCTM(contextRef, 1.0, -1.0);
			focusLocked = true;
			
			//#558
			//縦書き座標系にする
			if( mVerticalMode == true )
			{
				ChangeToVerticalCTM(contextRef);
			}
		}
		//save gstate
		::CGContextSaveGState(contextRef);
		//clip
		if( inClipToFrame == true )
		{
			//viewのサイズ取得
			ANumber	viewwidth = GetWin().GetControlWidth(mControlID);
			ANumber	viewheight = GetWin().GetControlHeight(mControlID);
			//縦書きモードの場合は、ビュー範囲を縦横変換 #558
			if( mVerticalMode == true )
			{
				ANumber	swap = viewwidth;
				viewwidth = viewheight;
				viewheight = swap;
			}
			//ビューにclip
			CGRect	cliprect = {0};
			cliprect.origin.x = 0;
			cliprect.origin.y = -viewheight;
			cliprect.size.width = viewwidth;
			cliprect.size.height = viewheight;
			::CGContextClipToRect(contextRef,cliprect);
		}
		
		//描画
		::CGContextSetRGBStrokeColor(contextRef,1.0,1.0,1.0,1.0);
		::CGContextSetLineWidth(contextRef,1);
		::CGContextSetShouldAntialias(contextRef,false);
		::CGContextSetBlendMode(contextRef,kCGBlendModeExclusion);
		::CGContextStrokeRect(contextRef,GetCGRectFromARect(inLocalRect));
		::CGContextSynchronize(contextRef);
		//#1034
		if( inFlush == true )
		{
			::CGContextFlush(contextRef);
		}
		//restore gstate
		::CGContextRestoreGState(contextRef);
		//contextはrelease不要と思われる。このタイミングでautoreleaseするとエラー表示されるため。
		if( focusLocked == true )
		{
			CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
			[view unlockFocus];
		}
	}
	//#1012 10.3は対応しない
#if 0
	else
	{
		//caretタイマータイムアウト処理はAPICB_DoDraw()外なので、CGContextが使えない。
		StSetPort	s(GetWindowRef());
		//::RGBForeColor(&kColor_Black);
		
		//フレームでClip
		RgnHandle	svRgn;
		svRgn = ::NewRgn();
		::GetClip(svRgn);
		
		if( inClipToFrame == true )
		{
			AWindowRect	localFrameWindowRect;
			GetControlWindowFrameRect(localFrameWindowRect);
			Rect	frameRect;
			frameRect.left		= localFrameWindowRect.left;
			frameRect.top		= localFrameWindowRect.top;
			frameRect.right		= localFrameWindowRect.right;
			frameRect.bottom	= localFrameWindowRect.bottom;
			::ClipRect(&frameRect);
		}
		
		//引数をWindow座標系に変換して、描画
		AWindowRect	windowRect;
		GetWin().GetWindowRectFromControlLocalRect(GetControlID(),inLocalRect,windowRect);
		Rect	rect;
		rect.left	= windowRect.left;
		rect.top	= windowRect.top;
		rect.right	= windowRect.right;
		rect.bottom	= windowRect.bottom;
		::PenMode(patXor);
		::FrameRect(&rect);
		::PenMode(0);
		
		//Clip復元
		::SetClip(svRgn);
		::DisposeRgn(svRgn);
	}
#endif
}

/**
LocalPoint->CGPoint
*/
CGPoint	CUserPane::GetCGPointFromAPoint( const ALocalPoint& inPoint ) const
{
	CGPoint	point;
	point.x = inPoint.x;
	point.y = -inPoint.y;
	return point;
}

/**
LocalRect->CGRect
*/
CGRect	CUserPane::GetCGRectFromARect( const ALocalRect& inRect ) const
{
	CGRect	rect;
	rect.origin.x = inRect.left;
	rect.origin.y = -inRect.bottom;
	rect.size.width = inRect.right-inRect.left;
	rect.size.height = inRect.bottom-inRect.top;
	return rect;
}

#if SUPPORT_CARBON
/**
DoDraw()外でCGContextを設定
*/
void	CUserPane::SetupCGContextOutOfDoDraw()
{
	if( mContextRef != NULL )   return;
	::QDBeginCGContext(::GetWindowPort(GetWindowRef()),&mContextRef);
	//contextの座標系を左上が(0,0)にする
	//grafPortの原点がずれていても必ず(0,0)が左上
	Rect	winRect;
	::GetWindowBounds(GetWindowRef(),kWindowContentRgn,&winRect);
	::CGContextTranslateCTM(mContextRef,0.0,winRect.bottom-winRect.top);
}

/**
DoDraw()外で設定したCGContextを解放
*/
void	CUserPane::EndCGContextOutOfDoDraw()
{
	if( mContextRef == NULL )   return;
	::QDEndCGContext(::GetWindowPort(GetWindowRef()),&mContextRef);
	mContextRef = NULL;
}
#endif

//B0000
/**
即時描画フラッシュする

OS Xは次のイベントトランザクションが帰ってくるまで描画イメージを内部バッファにためている
*/
void	CUserPane::RedrawImmediately()
{
	if( true )
	{
		GetWin().UpdateWindow();
	}
#if SUPPORT_CARBON
	else
	{
		::QDFlushPortBuffer(::GetWindowPort(GetWindowRef()),NULL);
	}
#endif
}

//#688
/**
viewがlive resize中かどうかを返す
*/
ABool	CUserPane::IsInLiveResize() const
{
	//Resize completed処理中（＝viewDidEndLiveResize）も、inLiveResizeはYESになってしまうので、
	//Resize completed処理中はフラグを使用して、falseを返すようにする。
	if( GetWinConst().IsDoingResizeCompleted() == true )
	{
		return false;
	}
	
	//viewを取得
	CocoaUserPaneView*	view = GetWinConst().GetCocoaObjectsConst().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return false; }
	
	//live resize中かどうか
	return ([view inLiveResize]==YES);
}

#pragma mark ===========================

#pragma mark ---イメージ
//#725

//イメージデータ
AHashArray<AImageID>	gImageIDArray;
AArray<NSImage*>	gImageArray;
AArray<NSImage*>	gDarkImageArray;//#1316
ATextArray	gImageArray_FilePath;//#1090
ATextArray	gImageArray_Name;//#1090
AArray<ANumber>	gImageArray_Width;//#1090
AArray<ANumber>	gImageArray_Height;//#1090
//AObjectArray<AFileAcc>	gImageArray_File;

/**
イメージ登録
*/
void	CUserPane::RegisterImageByFile( const AImageID inImageID, const AFileAcc& inImageFile )
{
	//ファイルパス取得
	AText	filepath;
	inImageFile.GetPath(filepath);
	//Arrayに格納 #1090
	gImageIDArray.Add(inImageID);
	gImageArray.Add(nil);
	gDarkImageArray.Add(nil);//#1316
	gImageArray_FilePath.Add(filepath);
	gImageArray_Name.Add(GetEmptyText());
	gImageArray_Width.Add(0);
	gImageArray_Height.Add(0);
}

/**
イメージ登録
*/
void	CUserPane::RegisterImageByName( const AImageID inImageID, const AText& inImageFileName )
{
	//Arrayに格納 #1090
	gImageIDArray.Add(inImageID);
	gImageArray.Add(nil);
	gDarkImageArray.Add(nil);//#1316
	gImageArray_FilePath.Add(GetEmptyText());
	gImageArray_Name.Add(inImageFileName);
	gImageArray_Width.Add(0);
	gImageArray_Height.Add(0);
}
void	CUserPane::RegisterImageByName( const AImageID inImageID, const AUCharPtr inImageFileName )
{
	AText	imageFileName(inImageFileName);
	RegisterImageByName(inImageID,imageFileName);
}

/**
イメージをオンデマンドでロードする
*/
AIndex	CUserPane::LoadImage( const AImageID inImageID )
{
	//イメージのindex取得
	AIndex	index = gImageIDArray.Find(inImageID);
	//イメージ未ロードなら、イメージ取得・設定
	if( gImageArray.Get(index) == nil )
	{
		//イメージファイルパス取得
		AText	filepath;
		gImageArray_FilePath.Get(index,filepath);
		if( filepath.GetItemCount() > 0 )
		{
			//ファイルパスで指定されている場合
			
			//ファイルパスNSString取得
			AStCreateNSStringFromAText	filepathstr(filepath);
			//NSImage生成
			NSImage*	image = [[NSImage alloc] initByReferencingFile:filepathstr.GetNSString()];//ポインタを保持するのでretainCount=1のまま(initによる)にする
			//image, width, height設定
			gImageArray.Set(index,image);
			NSSize	size = [image size];
			gImageArray_Width.Set(index,size.width);
			gImageArray_Height.Set(index,size.height);
		}
		else
		{
			//イメージ名で指定されている場合
			
			//イメージ名取得
			AText	imageFileName;
			gImageArray_Name.Get(index,imageFileName);
			//suffix削除
			imageFileName.DeleteAfter(imageFileName.GetSuffixStartPos());
			//ファイル名NSString取得
			AStCreateNSStringFromAText	filenamestr(imageFileName);
			//NSImage生成
			NSImage*	image = [NSImage imageNamed:filenamestr.GetNSString()];//ポインタを保持するのでretainCount=1のまま(initによる)にする
			//image, width, height設定
			gImageArray.Set(index,image);
			NSSize	size = [image size];
			gImageArray_Width.Set(index,size.width);
			gImageArray_Height.Set(index,size.height);
			
			//ダークモード用イメージ #1316
			AText	darkImageFileName;
			darkImageFileName.SetText(imageFileName);
			darkImageFileName.AddCstring("_dark");
			AStCreateNSStringFromAText	darkfilenamestr(darkImageFileName);
			NSImage*	darkImage = [NSImage imageNamed:darkfilenamestr.GetNSString()];
			gDarkImageArray.Set(index,darkImage);
		}
	}
	return index;
}

/**
*/
void	CUserPane::UnregisterImage( const AImageID inImageID )
{
	AIndex	index = gImageIDArray.Find(inImageID);
	if( index != kIndex_Invalid )
	{
		gImageIDArray.Delete1(index);
		gImageArray.Delete1(index);
		gDarkImageArray.Delete1(index);//#1316
		gImageArray_FilePath.Delete1(index);//#1090
		gImageArray_Name.Delete1(index);//#1090
		gImageArray_Width.Delete1(index);//#1090
		gImageArray_Height.Delete1(index);//#1090
	}
}

/**
イメージ描画
*/
void	CUserPane::DrawImage( const AImageID inImageID, const ALocalPoint& inPoint, 
							  const ANumber inWidth, const ANumber inHeight )
{
	//NSImage取得
	AIndex	index = LoadImage(inImageID);
	NSImage*	image = gImageArray.Get(index);
	//#1316
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		NSImage*	darkImage = gDarkImageArray.Get(index);
		if( darkImage != nil )
		{
			image = darkImage;
		}
	}
	ANumber	imagewidth = gImageArray_Width.Get(index);//#1090 [image size].width;
	ANumber	imageheight = gImageArray_Height.Get(index);//#1090 [image size].height;
	//描画サイズ取得（引数指定がなければイメージサイズを使用）
	ANumber	drawwidth = imagewidth;
	ANumber	drawheight = imageheight;
	if( inWidth > 0 )
	{
		drawwidth = inWidth;
	}
	if( inHeight > 0 )
	{
		drawheight = inHeight;
	}
	//サイズ0なら何もしない
	if( imagewidth == 0 || imageheight == 0 )   return;
	
	//描画先rect取得
	NSRect	drawrect = {0};
	drawrect.origin.x = inPoint.x;
	drawrect.origin.y = -(inPoint.y+drawheight);
	drawrect.size.width 	= drawwidth;
	drawrect.size.height 	= drawheight;
	//イメージrect
	NSRect	srcrect = {0};
	srcrect.origin.x = 0;
	srcrect.origin.y = 0;
	srcrect.size.width 		= imagewidth;
	srcrect.size.height 	= imageheight;
	//描画
	[image drawInRect:drawrect fromRect:srcrect operation:NSCompositeSourceOver fraction:1.0];
}

/**
イメージサイズ取得
*/
ANumber	CUserPane::GetImageWidth( const AImageID inImageID ) const
{
	AIndex	index = LoadImage(inImageID);
	/*#1090
	NSImage*	image = gImageArray.Get(index);
	return [image size].width;
	*/
	return gImageArray_Width.Get(index);//#1090
}
ANumber	CUserPane::GetImageHeight( const AImageID inImageID ) const
{
	AIndex	index = LoadImage(inImageID);
	/*#1090
	NSImage*	image = gImageArray.Get(index);
	return [image size].height;
	*/
	return gImageArray_Height.Get(index);//#1090
}

/**
イメージ描画（NSImage*指定）
*/
void	DrawImage( NSImage* inImage, const ALocalPoint inPoint, const ANumber inWidth, const ANumber inHeight )
{
	//描画先rect取得
	NSRect	drawrect = {0};
	drawrect.origin.x = inPoint.x;
	drawrect.origin.y = -(inPoint.y+inHeight);
	drawrect.size.width 	= inWidth;
	drawrect.size.height 	= inHeight;
	//イメージrect
	NSRect	srcrect = {0};
	srcrect.origin.x = 0;
	srcrect.origin.y = 0;
	srcrect.size.width 		= inWidth;
	srcrect.size.height 	= inHeight;
	//描画
	[inImage drawInRect:drawrect fromRect:srcrect operation:NSCompositeSourceOver fraction:1.0];
}

/**
イメージ描画（横繰り返し）
*/
void	CUserPane::DrawImageFlexibleWidth( const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
		const ALocalPoint& inPoint, const ANumber inWidth )
{
	//image, width, height取得
	//#1090
	//index取得
	AIndex	index[3] = {0};
	index[0] = LoadImage(inImageID0);
	index[1] = LoadImage(inImageID1);
	index[2] = LoadImage(inImageID2);
	//
	NSImage*	image[3] = {0};
	/*#1090
	image[0] = gImageArray.Get(gImageIDArray.Find(inImageID0));
	image[1] = gImageArray.Get(gImageIDArray.Find(inImageID1));
	image[2] = gImageArray.Get(gImageIDArray.Find(inImageID2));
	*/
	ANumber	width[3] = {0}, height[3] = {0};
	for( AIndex i = 0; i < 3; i++ )
	{
		//image取得
		image[i] = gImageArray.Get(index[i]);
		//width, height取得
		width[i] = gImageArray_Width.Get(index[i]);//#1090 [image[i] size].width;
		if( width[i] == 0 )   return;
		height[i] = gImageArray_Height.Get(index[i]);//#1090 [image[i] size].height;
		if( height[i] == 0 )   return;
	}
	//pt
	ALocalPoint	pt = inPoint;
	//Image0描画
	::DrawImage(image[0],pt,width[0],height[0]);
	//x移動
	pt.x += width[0];
	//繰り返しカウント取得
	ANumber	flWidth = inWidth - width[0] - width[2];
	ANumber	flWidthCount = (flWidth+width[1]-1)/width[1];
	for( AIndex i = 0; i < flWidthCount; i++ )
	{
		ANumber	w = width[1];
		//繰り返し最後の項目の場合、サイズを調整する。
		if( i == flWidthCount-1 )
		{
			w = inPoint.x + inWidth - width[2] - pt.x;
		}
		//Image1描画
		::DrawImage(image[1],pt,w,height[1]);
		//x移動
		pt.x += width[1];
	}
	//x移動
	pt.x = inPoint.x + inWidth - width[2];
	//Image2描画
	::DrawImage(image[2],pt,width[2],height[2]);
}

/**
イメージ描画（縦繰り返し）
*/
void	CUserPane::DrawImageFlexibleHeight( const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
		const ALocalPoint& inPoint, const ANumber inHeight )
{
	//image, width, height取得
	//#1090
	//index取得
	AIndex	index[3] = {0};
	index[0] = LoadImage(inImageID0);
	index[1] = LoadImage(inImageID1);
	index[2] = LoadImage(inImageID2);
	//
	NSImage*	image[3] = {0};
	/*#1090
	image[0] = gImageArray.Get(gImageIDArray.Find(inImageID0));
	image[1] = gImageArray.Get(gImageIDArray.Find(inImageID1));
	image[2] = gImageArray.Get(gImageIDArray.Find(inImageID2));
	*/
	ANumber	width[3] = {0}, height[3] = {0};
	for( AIndex i = 0; i < 3; i++ )
	{
		//image取得
		image[i] = gImageArray.Get(index[i]);
		//width, height取得
		width[i] = gImageArray_Width.Get(index[i]);//#1090 [image[i] size].width;
		if( width[i] == 0 )   return;
		height[i] = gImageArray_Height.Get(index[i]);//#1090 [image[i] size].height;
		if( height[i] == 0 )   return;
	}
	//pt
	ALocalPoint	pt = inPoint;
	//Image0描画
	::DrawImage(image[0],pt,width[0],height[0]);
	//y移動
	pt.y += height[0];
	//繰り返しカウント取得
	ANumber	flHeight = inHeight - height[0] - height[2];
	ANumber	flHeightCount = (flHeight+height[1]-1)/height[1];
	for( AIndex i = 0; i < flHeightCount; i++ )
	{
		ANumber	h = height[1];
		//繰り返し最後の項目の場合、サイズを調整する。
		if( i == flHeightCount-1 )
		{
			h = inPoint.y + inHeight - height[2] - pt.y;
		}
		//Image1描画
		::DrawImage(image[1],pt,width[1],h);
		//y移動
		pt.y += height[1];
	}
	//y移動
	pt.y = inPoint.y + inHeight - height[2];
	//Image2描画
	::DrawImage(image[2],pt,width[2],height[2]);
}

/**
イメージ描画（縦横繰り返し）
*/
void	CUserPane::DrawImageFlexibleWidthAndHeight( 
		const AImageID inImageID0, const AImageID inImageID1, const AImageID inImageID2,
		const AImageID inImageID3, const AImageID inImageID4, const AImageID inImageID5,
		const AImageID inImageID6, const AImageID inImageID7, const AImageID inImageID8,
		const ALocalPoint& inPoint, const ANumber inWidth, const ANumber inHeight )
{
	//image, width, height取得
	//#1090
	AIndex	index[9] = {0};
	index[0] = LoadImage(inImageID0);
	index[1] = LoadImage(inImageID1);
	index[2] = LoadImage(inImageID2);
	index[3] = LoadImage(inImageID3);
	index[4] = LoadImage(inImageID4);
	index[5] = LoadImage(inImageID5);
	index[6] = LoadImage(inImageID6);
	index[7] = LoadImage(inImageID7);
	index[8] = LoadImage(inImageID8);
	//
	NSImage*	image[9] = {0};
	/*#1090
	image[0] = gImageArray.Get(gImageIDArray.Find(inImageID0));
	image[1] = gImageArray.Get(gImageIDArray.Find(inImageID1));
	image[2] = gImageArray.Get(gImageIDArray.Find(inImageID2));
	image[3] = gImageArray.Get(gImageIDArray.Find(inImageID3));
	image[4] = gImageArray.Get(gImageIDArray.Find(inImageID4));
	image[5] = gImageArray.Get(gImageIDArray.Find(inImageID5));
	image[6] = gImageArray.Get(gImageIDArray.Find(inImageID6));
	image[7] = gImageArray.Get(gImageIDArray.Find(inImageID7));
	image[8] = gImageArray.Get(gImageIDArray.Find(inImageID8));
	*/
	ANumber	width[9] = {0}, height[9] = {0};
	for( AIndex i = 0; i < 9; i++ )
	{
		//image取得
		image[i] = gImageArray.Get(index[i]);
		//width, height取得
		width[i] = gImageArray_Width.Get(index[i]);//#1090 [image[i] size].width;
		if( width[i] == 0 )   return;
		height[i] = gImageArray_Height.Get(index[i]);//#1090 [image[i] size].height;
		if( height[i] == 0 )   return;
	}
	//pt
	ALocalPoint	pt = inPoint;
	
	//=============上段=============
	
	{
		//Image0描画
		::DrawImage(image[0],pt,width[0],height[0]);
		//x移動
		pt.x += width[0];
		//繰り返しカウント取得
		ANumber	flWidth = inWidth - width[0] - width[2];
		ANumber	flWidthCount = (flWidth+width[1]-1)/width[1];
		for( AIndex i = 0; i < flWidthCount; i++ )
		{
			ANumber	w = width[1];
			//繰り返し最後の項目の場合、サイズを調整する。
			if( i == flWidthCount-1 )
			{
				w = inPoint.x + inWidth - width[2] - pt.x;
			}
			//Image1描画
			::DrawImage(image[1],pt,w,height[1]);
			//x移動
			pt.x += width[1];
		}
		//x移動
		pt.x = inPoint.x + inWidth - width[2];
		//Image2描画
		::DrawImage(image[2],pt,width[2],height[2]);
		//x,y移動
		pt.x = 0;
		pt.y += height[0];
	}
	
	//=============中段=============
	
	//繰り返しカウント取得
	ANumber	flHeight = inHeight - height[0] - height[6];
	ANumber	flHeightCount = (flHeight+height[3]-1)/height[3];
	for( AIndex j = 0; j < flHeightCount; j++ )
	{
		//Image3描画
		::DrawImage(image[3],pt,width[3],height[3]);
		//x移動
		pt.x += width[3];
		//繰り返しカウント取得
		ANumber	flWidth = inWidth - width[3] - width[5];
		ANumber	flWidthCount = (flWidth+width[4]-1)/width[4];
		for( AIndex i = 0; i < flWidthCount; i++ )
		{
			ANumber	w = width[4];
			ANumber	h = height[4];
			//繰り返し最後の項目の場合、サイズを調整する。
			if( i == flWidthCount-1 )
			{
				w = inPoint.x + inWidth - width[5] - pt.x;
			}
			if( j == flHeightCount-1 )
			{
				h = inPoint.y + inHeight - height[6] - pt.y;
			}
			//Image4描画
			::DrawImage(image[4],pt,w,h);
			//x移動
			pt.x += width[4];
		}
		//x移動
		pt.x = inPoint.x + inWidth - width[5];
		//Image5描画
		::DrawImage(image[5],pt,width[5],height[5]);
		//x,y移動
		pt.x = 0;
		pt.y += height[3];
	}
	
	//=============下段=============
	
	{
		//x,y移動
		pt.x = 0;
		pt.y = inPoint.y + inHeight - height[6];
		//Image6描画
		::DrawImage(image[6],pt,width[6],height[6]);
		//x移動
		pt.x += width[0];
		//繰り返しカウント取得
		ANumber	flWidth = inWidth - width[0] - width[8];
		ANumber	flWidthCount = (flWidth+width[7]-1)/width[7];
		for( AIndex i = 0; i < flWidthCount; i++ )
		{
			ANumber	w = width[7];
			//繰り返し最後の項目の場合、サイズを調整する。
			if( i == flWidthCount-1 )
			{
				w = inPoint.x + inWidth - width[8] - pt.x;
			}
			//Image7描画
			::DrawImage(image[7],pt,w,height[7]);
			//x移動
			pt.x += width[7];
		}
		//x移動
		pt.x = inPoint.x + inWidth - width[8];
		//Image8描画
		::DrawImage(image[8],pt,width[8],height[8]);
	}
}

#pragma mark ===========================

#pragma mark ---Refresh

/**
描画Refresh（指定Rect）
*/
void	CUserPane::RefreshRect( const ALocalRect& inLocalRect )
{
	GetWin().RefreshControlRect(GetControlID(),inLocalRect);//#688
	/*
	OSStatus	err = noErr;
	
	* HIRect	rect;
	rect.origin.x = inLocalRect.left;
	rect.origin.y = inLocalRect.top;
	rect.size.width = inLocalRect.right - inLocalRect.left;
	rect.size.height = inLocalRect.bottom - inLocalRect.top;
	::HIViewSetNeedsDisplayInRect(mControlRef,&rect,true);
	::HIViewRender(mControlRef);*
	
	ALocalRect	localFrameRect;
	GetControlLocalFrameRect(localFrameRect);
	//R0108高速化 フレームサイズトリミングの結果、localRect.top > localRect.bottom となると、DoDraw時のUpdateRegion判定で、全範囲対象になってしまう？ので
	//RefreshRectがフレーム外のときは何もしないようにする
	if( inLocalRect.bottom < localFrameRect.top )   return;
	if( inLocalRect.top > localFrameRect.bottom )   return;
	if( inLocalRect.right < localFrameRect.left )   return;
	if( inLocalRect.left > localFrameRect.right )   return;
	//OSX10.3ではHIViewSetNeedsDisplayInRectが使えないので、下記に変更
	//フレームサイズでトリミングしておかないとHIViewSetNeedsDisplayInRegionのほうはうまくいかないことがある
	ALocalRect	localRect = inLocalRect;
	if( localRect.top < localFrameRect.top )   localRect.top = localFrameRect.top;
	if( localRect.bottom > localFrameRect.bottom )   localRect.bottom = localFrameRect.bottom;
	if( localRect.left < localFrameRect.left )   localRect.left = localFrameRect.left;
	if( localRect.right > localFrameRect.right )   localRect.right = localFrameRect.right;
	Rect	rect;
	rect.left	= localRect.left;
	rect.right	= localRect.right;
	rect.top	= localRect.top;
	rect.bottom	= localRect.bottom;
	RgnHandle	rgn = ::NewRgn();
	::RectRgn(rgn,&rect);
	err = ::HIViewSetNeedsDisplayInRegion(mControlRef,rgn,true);
	if( err != noErr )   _ACErrorNum("HIViewSetNeedsDisplayInRegion() returned error: ",err,this);
	::DisposeRgn(rgn);
	err = ::HIViewRender(mControlRef);
	if( err != noErr )   _ACErrorNum("HIViewRender() returned error: ",err,this);
	*/
}

/**
描画リフレッシュ（コントロール全体）
*/
void	CUserPane::RefreshControl()
{
	if( IsControlVisible() == false )   return;
	ALocalRect	rect;
	GetControlLocalFrameRect(rect);
	RefreshRect(rect);
}

//#688
/**
描画すべき領域を今すぐ描画する(EVT_DoDraw()実行する)（バッファへの描画となる）
*/
void	CUserPane::ExecuteDoDrawImmediately()
{
	//fprintf(stderr,"%16X ExecuteDoDrawImmediately() - Start\n",(int)(GetObjectID().val));
	
	//viewを取得
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	
	//描画実行
	[view displayIfNeeded];
	
	//fprintf(stderr,"%16X ExecuteDoDrawImmediately() - End\n",(int)(GetObjectID().val));
}

#pragma mark ===========================

#pragma mark ---描画用情報取得

/**
テキストの位置から描画時のX座標を得る
*/
ANumber	CUserPane::GetImageXByTextPosition( CTextDrawData& inTextDrawData, const AIndex inTextPosition )
{
	/*#1045
	//#572
	if( inTextDrawData.UTF16DrawText.GetItemCount() > kLineTextDrawLengthMax )
	{
		return 0;
	}
	*/
	
	/*#1034
	//TextLayout生成
	if( inTextDrawData.GetTextLayout() == NULL )
	{
		inTextDrawData.SetTextLayout(CreateTextLayout(inTextDrawData.UTF16DrawText,inTextDrawData.IsFontFallbackEnabled()));
	}
	
	OSStatus	status = noErr;
	ATSUCaret	mainCaret, secondCaret;
	Boolean	caretIsSplit;//検討
	*/
	UniCharArrayOffset	uniOffset = 0;
	if( inTextPosition < inTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount() )
	{
		uniOffset = inTextDrawData.OriginalTextArray_UnicodeOffset.Get(inTextPosition);
	}
	else
	{
		uniOffset = inTextDrawData.OriginalTextArray_UnicodeOffset.Get(inTextDrawData.OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	}
	//テキスト位置からX座標を取得
	CGFloat	offset2 = 0;
	CGFloat	offset = ::CTLineGetOffsetForStringIndex(GetCTLineFromTextDrawData(inTextDrawData),uniOffset,&offset2);
	return offset;
	
	/*#1034
	status = ::ATSUOffsetToPosition(inTextDrawData.GetTextLayout(),offset,true,&mainCaret,&secondCaret,&caretIsSplit);
	//高速化（CTextDrawDataデストラクタで削除される）::ATSUDisposeTextLayout(textLayout);
	//mainCaret.fX += ConvertFloatToFixed(0.5);
	return ::Fix2Long(mainCaret.fX);
	*/
}

/**
テキストの位置から描画時のX座標を得る
*/
ANumber	CUserPane::GetImageXByUnicodeOffset( CTextDrawData& inTextDrawData, const AIndex inUnicodeOffset )
{
	/*#1045
	//#572
	if( inTextDrawData.UTF16DrawText.GetItemCount() > kLineTextDrawLengthMax )
	{
		return 0;
	}
	*/
	
	/*#1034
	//TextLayout生成
	if( inTextDrawData.GetTextLayout() == NULL )
	{
		inTextDrawData.SetTextLayout(CreateTextLayout(inTextDrawData.UTF16DrawText,inTextDrawData.IsFontFallbackEnabled()));
	}
	
	OSStatus	status = noErr;
	ATSUCaret	mainCaret, secondCaret;
	Boolean	caretIsSplit;//検討
	status = ::ATSUOffsetToPosition(inTextDrawData.GetTextLayout(),inUnicodeOffset,true,&mainCaret,&secondCaret,&caretIsSplit);
	//高速化（CTextDrawDataデストラクタで削除される）::ATSUDisposeTextLayout(textLayout);
	//mainCaret.fX += ConvertFloatToFixed(0.5);
	return ::Fix2Long(mainCaret.fX);
	*/
	
	//テキスト位置からX座標を取得
	CGFloat	offset2 = 0;
	CGFloat	offset = ::CTLineGetOffsetForStringIndex(GetCTLineFromTextDrawData(inTextDrawData),inUnicodeOffset,&offset2);
	return offset;
}

/**
描画時のX座標からテキストの位置を得る
*/
AIndex	CUserPane::GetTextPositionByImageX( CTextDrawData& inTextDrawData, const ANumber inImageX )
{
	/*#1045
	//#572
	if( inTextDrawData.UTF16DrawText.GetItemCount() > kLineTextDrawLengthMax )
	{
		return 0;
	}
	*/
	
	/*#1034
	//TextLayout生成
	if( inTextDrawData.GetTextLayout() == NULL )
	{
		inTextDrawData.SetTextLayout(CreateTextLayout(inTextDrawData.UTF16DrawText,inTextDrawData.IsFontFallbackEnabled()));
	}
	//
	OSStatus	status = noErr;
	UniCharArrayOffset	primaryOffset,secondaryOffset;
	primaryOffset = 0;
	Boolean	isLeading;
	status = ::ATSUPositionToOffset(inTextDrawData.GetTextLayout(),::Long2Fix(inImageX-1),Long2Fix(0),&primaryOffset,&isLeading,&secondaryOffset);
	//高速化（CTextDrawDataデストラクタで削除される）::ATSUDisposeTextLayout(textLayout);
	
	return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(primaryOffset);
	*/
	
	//X座標からテキスト位置を取得
	CGPoint	point = {inImageX,0};
	//#1147 Mac OS X 10.11beta対策
	if( point.x <= 0 )
	{
		point.x = 0.1;
	}
	CGFloat	ascent = 9, descent = 3, leading = 3;
	double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(inTextDrawData),&ascent,&descent,&leading);
	if( point.x >= width )
	{
		point.x = width - 0.1;
	}
	//
	AIndex uniOffset = ::CTLineGetStringIndexForPosition(GetCTLineFromTextDrawData(inTextDrawData),point);
	if( uniOffset > 0 )
	{
		return inTextDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(uniOffset);
	}
	else
	{
		return 0;
	}
}

/**
テキスト描画時の幅を取得
*/
AFloatNumber	CUserPane::GetDrawTextWidth( const AText& inText ) 
{
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(inText);
	textDrawData.UTF16DrawText.ConvertFromUTF8ToUTF16();
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(mColor);
	textDrawData.attrStyle.Add(mStyle);
	
	/*#1034
	textDrawData.SetTextLayout(CreateTextLayout(textDrawData.UTF16DrawText,true));
	ATSUTextMeasurement	start, end, ascent, descent;
	::ATSUGetUnjustifiedBounds(textDrawData.GetTextLayout(),kATSUFromTextBeginning,kATSUToTextEnd,&start,&end,&ascent,&descent);
	//高速化（CTextDrawDataデストラクタで削除される）::ATSUDisposeTextLayout(textLayout);
	return ConvertFixedToFloat(end-start);
	*/
	return GetDrawTextWidth(textDrawData);
}

/**
テキスト描画時の幅を取得
*/
AFloatNumber	CUserPane::GetDrawTextWidth( CTextDrawData& inTextDrawData ) 
{
	/*#1034
	//TextLayout生成
	if( inTextDrawData.GetTextLayout() == NULL )
	{
		inTextDrawData.SetTextLayout(CreateTextLayout(inTextDrawData.UTF16DrawText,inTextDrawData.IsFontFallbackEnabled()));
	}
	//width取得
	ATSUTextMeasurement	start, end, ascent, descent;
	::ATSUGetUnjustifiedBounds(inTextDrawData.GetTextLayout(),kATSUFromTextBeginning,kATSUToTextEnd,&start,&end,&ascent,&descent);
	return ConvertFixedToFloat(end-start);
	*/
	
	//テキスト幅取得
	CGFloat	ascent = 9, descent = 3, leading = 3;
	double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(inTextDrawData),&ascent,&descent,&leading);
	return width;
}

/**
Drawすべき領域内かどうかの判定
*/
ABool	CUserPane::IsRectInDrawUpdateRegion( const ALocalRect& inLocalRect ) const
{
	if( true )
	{
		//Cocoaウインドウの場合
		//Cocoaの場合、drawRectの引数がrectであり、regionは旧apiなので、mDrawUpdateRegionは使わず、
		//mCocoaDrawDirtyRectを使う。
		
		ALocalRect	intersect = {0,0,0,0};
		return GetIntersectRect(inLocalRect,mCocoaDrawDirtyRect,intersect);
	}
#if SUPPORT_CARBON
	else
	{
		//Carbonウインドウの場合
		
		if( mDrawUpdateRegion == NULL )   {_ACError("not in APICB_DoDraw()",this);return false;}
		Rect	rect;
		rect.left	= inLocalRect.left;
		rect.top	= inLocalRect.top;
		rect.right	= inLocalRect.right;
		rect.bottom	= inLocalRect.bottom;
		//test
		/*AWindowRect	windowRect;
		GetWinConst().GetWindowRectFromControlLocalRect(GetControlID(),inLocalRect,windowRect);
		rect.left	= windowRect.left;
		rect.top	= windowRect.top;
		rect.right	= windowRect.right;
		rect.bottom	= windowRect.bottom;*/
		return ::RectInRgn(&rect,mDrawUpdateRegion);
	}
#endif
}

/**
現在のTextPropertyでの行の高さ等の情報を取得
*/
void	CUserPane::GetMetricsForDefaultTextProperty( ANumber& outLineHeight, ANumber& outLineAscent ) 
{
	if( mLineHeight == 0 )
	{
		UpdateMetrics();
	}
	outLineHeight = mLineHeight;
	outLineAscent = mLineAscent;
}
//#1316
void	CUserPane::GetMetricsForDefaultTextProperty( AFloatNumber& outLineHeight, AFloatNumber& outLineAscent ) 
{
	if( mLineHeight == 0 )
	{
		UpdateMetrics();
	}
	outLineHeight = mLineHeight;
	outLineAscent = mLineAscent;
}

/**
行の高さ等の情報更新
*/
void	CUserPane::UpdateMetrics()
{
	//#1090
	//すでにmetrics取得したことのあるフォント名・フォントサイズ・スタイルの組み合わせの場合は、キャッシュから取得する
	//フォント名・フォントサイズ・スタイルの組み合わせのkey取得
	AText	key;
	key.AddText(mFontName);
	key.Add(kUChar_Tab);
	key.AddNumber((ANumber)(mFontSize*10.0));
	key.Add(kUChar_Tab);
	key.AddNumber((ANumber)(mStyle));
	key.Add(kUChar_Tab);//#558
	key.AddNumber((ANumber)(mVerticalMode?1:0));//#558
	//キャッシュ検索
	AIndex	cacheIndex = sMetricsCacheArray_Key.Find(key);
	if( cacheIndex != kIndex_Invalid )
	{
		//キャッシュから取得
		mLineHeight = sMetricsCacheArray_LineHeight.Get(cacheIndex);
		mLineAscent = sMetricsCacheArray_LineAscent.Get(cacheIndex);
	}
	else
	{
		CTextDrawData	textDrawData(false);
		textDrawData.UTF16DrawText.SetText(mTextForMetricsCalculation);
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(mColor);
		textDrawData.attrStyle.Add(mStyle);
		CGFloat	ascent = 9, descent = 3, leading = 3;
		double	width = ::CTLineGetTypographicBounds(GetCTLineFromTextDrawData(textDrawData),&ascent,&descent,&leading);
		//leadingを入れると行の高さが高くなりすぎるので、ascent+descentにする。
		mLineHeight = ceil(ascent+descent+2);
		mLineAscent = ceil(ascent+1);
		//キャッシュへ追加
		sMetricsCacheArray_Key.Add(key);
		sMetricsCacheArray_LineHeight.Add(mLineHeight);
		sMetricsCacheArray_LineAscent.Add(mLineAscent);
	}
	
#if 0 
	OSStatus	err = noErr;
	
	ATSUTextMeasurement	ascent = ::Long2Fix(10);
	ATSUTextMeasurement	descent = ::Long2Fix(5);
	ATSUTextMeasurement	leading = ::Long2Fix(2);
	
	// ================ ascentの取得（ATSUGetUnjustifiedBounds()で取得）================ 
	
	//#636 代替フォントが本来のフォントよりもサイズが大きくなって、ascent内に収まらないことがある（Helvetica 11pt)
	//たとえば、Helvetica 24ptの場合、英字のみのテキストでATSUGetUnjustifiedBounds()するとascent:18 pixel, descent:6 pixelだが、
	//日本語全角を含むと、ascent:21.1 pixel, descent:14.9 pixelとなる。
	//対策として、￣を含むテキストでATSUGetUnjustifiedBounds()を実行し、ascentはこちらを使用する
	//descentも含むと行の高さが高くなりすぎ（行間が空きすぎに見える）ので、descent(+leading)はATSUGetAttributeで
	//取得した値のほうを使用する。
	CTextDrawData	textDrawData(false);
	textDrawData.UTF16DrawText.SetText(mTextForMetricsCalculation);
	textDrawData.attrPos.Add(0);
	textDrawData.attrColor.Add(mColor);
	textDrawData.attrStyle.Add(mStyle);
	textDrawData.SetTextLayout(CreateTextLayout(textDrawData.UTF16DrawText,true));
	ATSUTextMeasurement	start = 0, end = 0;
	::ATSUGetUnjustifiedBounds(textDrawData.GetTextLayout(),kATSUFromTextBeginning,kATSUToTextEnd,&start,&end,&ascent,&descent);
	
	// ================ descent, leadingの取得（ATSUGetAttribute()で取得）================ 
	
	//なお、ATSUGetAttribute()はデフォルト値のままの場合、kATSUNotSetErrを返すことがある
	ByteCount	actSize;
	//#636 ascentはATSUGetUnjustifiedBounds()で取得err = ::ATSUGetAttribute(mATSUTextStyle,kATSUAscentTag,sizeof(ATSUTextMeasurement),&ascent,&actSize);
	//if( err != noErr )   _ACErrorNum("ATSUGetAttribute() returned error: ",err,this);
	err = ::ATSUGetAttribute(/*#852 mATSUTextStyle*/GetATSUTextStyle(),kATSUDescentTag,sizeof(ATSUTextMeasurement),&descent,&actSize);
	//if( err != noErr )   _ACErrorNum("ATSUGetAttribute() returned error: ",err,this);
	err = ::ATSUGetAttribute(/*#852 mATSUTextStyle*/GetATSUTextStyle(),kATSULeadingTag,sizeof(ATSUTextMeasurement),&leading,&actSize);
	//if( err != noErr )   _ACErrorNum("ATSUGetAttribute() returned error: ",err,this);
	mLineHeight = ::Fix2Long(ascent) +::Fix2Long(descent) + ::Fix2Long(leading) +1;
	mLineAscent =::Fix2Long(ascent) +1;
#endif
}
//metricsキャッシュ
AHashTextArray	CUserPane::sMetricsCacheArray_Key;//#1090
ANumberArray		CUserPane::sMetricsCacheArray_LineHeight;//#1090
ANumberArray		CUserPane::sMetricsCacheArray_LineAscent;//#1090

/**
現在のマウス位置取得
*/
/*win
void	CUserPane::GetMousePoint( ALocalPoint& outLocalPoint ) 
{
	StSetPort	s(GetWindowRef());
	Point	pt;
	::GetMouse(&pt);
	AWindowPoint	windowPoint;
	windowPoint.x = pt.h;
	windowPoint.y = pt.v;
	GetWin().GetControlLocalPointFromWindowPoint(GetControlID(),windowPoint,outLocalPoint);
}
*/

#pragma mark ===========================

#pragma mark ---スクロール

/**
Pane全体をスクロールする

なお、PaneがDragBoxにかぶさっていると、なぜか、スクロール時にUpdateRgnがDragBox以外のPane全体になってしまうので、DragBoxとかぶさらないようにする必要がある。
*/
void	CUserPane::Scroll( const ANumber inDeltaX, const ANumber inDeltaY )
{
	GetWin().ScrollControl(GetControlID(),inDeltaX,inDeltaY);//#688
	/*#688
	OSStatus	err = noErr;
	
	err = ::HIViewScrollRect(mControlRef,NULL,inDeltaX,inDeltaY);
	if( err != noErr )   _ACErrorNum("HIViewScrollRect() returned error: ",err,this);
	*/
}

#pragma mark ===========================

#pragma mark ---Drag

//#236
/**
Drop可能に設定する
*/
void	CUserPane::EnableDrop( const AArray<AScrapType>& inScrapType )
{
	if( true )
	{
		//viewを取得
		CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
		if( view == nil )   { _ACError("",NULL); return; }
		//scrapタイプをarrayに設定
		NSMutableArray *array = [[[NSMutableArray alloc] initWithCapacity:256] autorelease];
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			[array addObject:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
		}
		//scrapタイプ設定
		[view registerForDraggedTypes:array];
	}
#if SUPPORT_CARBON
	else
	{
		GetWin().RegisterDropEnableControl(GetControlID());
		//#364
		mScrapTypeArray.DeleteAll();
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			mScrapTypeArray.Add(inScrapType.Get(i));
		}
	}
#endif
}

/**
ウインドウの一部のイメージをdrag imageとしてDrag実行
*/
ABool	CUserPane::DragTextWithWindowImage( const ALocalPoint& inMousePoint,
			const AArray<AScrapType>& inScrapType, const ATextArray& inData, 
			const AWindowRect& inWindowRect, const ANumber inImageWidth, const ANumber inImageHeight )
{
	//viewを取得
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return false; }
	
	//Drag用pasteboard取得
	NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
	//scrapタイプ設定
	NSMutableArray *array = [[[NSMutableArray alloc] initWithCapacity:256] autorelease];
	for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
	{
		[array addObject:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
	}
	[pboard declareTypes:array owner:view];
	//データ（UTF-8テキスト）設定
	//DragText()に渡されるデータは#688対応によりUTF-8テキストに変更（前はUTF-16とlegacy encoding）
	for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
	{
		AText	data;
		inData.Get(i,data);
		//#931
		//改行コードをLFへ変換
		data.ConvertLineEndToLF();
		//
		AStCreateNSStringFromAText	datastr(data);
		[pboard setString:datastr.GetNSString() forType:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
	}
	//
	//Image生成
	NSView*	contentView = GetWin().GetCocoaObjects().GetContentView();
	NSData* theData = [contentView dataWithPDFInsideRect:ConvertFromWindowRectToNSRect(contentView,inWindowRect)];//★release確認
	NSPDFImageRep* pdfRep = [NSPDFImageRep imageRepWithData:theData];//★release確認
	NSImage* pdfImage = [[[NSImage alloc] initWithSize:NSMakeSize(inImageWidth,inImageHeight)] autorelease];
	[pdfImage addRepresentation:pdfRep];
	
	//Dragの結果としてviewが削除される可能性があるので、ここでretainしておく。（autoreleaseにより後で削除される）
	[[view retain] autorelease];
	//DroppedフラグOFF
	sDropped = false;
	//Drag実行
	NSPoint	dragpt = {0};
	dragpt.x = inMousePoint.x;
	dragpt.y = inMousePoint.y;
	[view dragImage:pdfImage at:dragpt offset:NSMakeSize(0,0)
	event:[NSApp currentEvent] pasteboard:pboard source:view slideBack:YES];
	return sDropped;
}

//Droppedフラグ
ABool	CUserPane::sDropped = false;

/**
Drag処理
@param inData UTF-8テキスト
*/
ABool	CUserPane::DragText( const ALocalPoint& inMousePoint, 
		const AArray<ALocalRect>& inDragRect, const AArray<AScrapType>& inScrapType, const ATextArray& inData,
		const AImageID inImageID )
{
	if( true )
	{
		//viewを取得
		CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
		if( view == nil )   { _ACError("",NULL); return false; }
		
		//Drag用pasteboard取得
		NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
		//scrapタイプ設定
		NSMutableArray *array = [[[NSMutableArray alloc] initWithCapacity:256] autorelease];
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			[array addObject:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
		}
		[pboard declareTypes:array owner:view];
		//データ（UTF-8テキスト）設定
		//DragText()に渡されるデータは#688対応によりUTF-8テキストに変更（前はUTF-16とlegacy encoding）
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			AText	data;
			inData.Get(i,data);
			//#931
			//改行コードをLFへ変換
			data.ConvertLineEndToLF();
			//
			AStCreateNSStringFromAText	datastr(data);
			[pboard setString:datastr.GetNSString() forType:(ACocoa::GetPasteboardTypeNSString(inScrapType.Get(i)))];
		}
		//dragpt
		NSPoint	dragpt = {0};
		//Image生成
		NSImage*	dragimage = nil;
		if( inImageID != kImageID_Invalid )
		{
			//Image指定の場合
			
			//(0,0)
			dragpt.x = 0;
			dragpt.y = 0;
			//Image取得
			AIndex	imageIndex = LoadImage(inImageID);
			dragimage = gImageArray.Get(imageIndex);
		}
		else
		{
			//Image未指定の場合
			
			dragpt.x = 0;
			dragpt.y = [view frame].size.height;
			//Image sizeはviewのサイズにする
			NSSize	imagesize = [view frame].size;
			dragimage = [[[NSImage alloc] initWithSize:imagesize] autorelease];
			//context取得
			[dragimage lockFocus];
			mContextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
			::CGContextScaleCTM(mContextRef, 1.0, -1.0);
			//drag imageを描画
			//lockFocusFlippedは10.6以降しか使えないので、下の処理でflip（imageheight-）を行っている。
			ANumber	imageheight = imagesize.height;
			AItemCount	dragRectItemCount = inDragRect.GetItemCount();
			if( dragRectItemCount > 0 )
			{
				AArray<ALocalPoint>	polyPoint;
				ALocalPoint	pt = {0};
				if( mVerticalMode == false )
				{
					//
					pt.x = inDragRect.Get(0).left;
					pt.y = imageheight - inDragRect.Get(0).top;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(0).right;
					pt.y = imageheight - inDragRect.Get(0).top;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(0).right;
					pt.y = imageheight - inDragRect.Get(dragRectItemCount-1).top;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(dragRectItemCount-1).right;
					pt.y = imageheight - inDragRect.Get(dragRectItemCount-1).top;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(dragRectItemCount-1).right;
					pt.y = imageheight - inDragRect.Get(dragRectItemCount-1).bottom;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(dragRectItemCount-1).left;
					pt.y = imageheight - inDragRect.Get(dragRectItemCount-1).bottom;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(dragRectItemCount-1).left;
					pt.y = imageheight - inDragRect.Get(0).bottom;
					polyPoint.Add(pt);
					//
					pt.x = inDragRect.Get(0).left;
					pt.y = imageheight - inDragRect.Get(0).bottom;
					polyPoint.Add(pt);
				}
				else
				{
					//縦書きモード用 #558
					//各rectの座標系変換
					AArray<ALocalRect>	dragRect;
					for( AIndex i = 0; i < inDragRect.GetItemCount(); i++ )
					{
						ALocalRect	r = inDragRect.Get(i);
						r = GetWin().ConvertVerticalCoordinateFromAppToImp(GetControlID(),r);
						dragRect.Add(r);
					}
					//
					pt.x = dragRect.Get(0).right;
					pt.y = imageheight - dragRect.Get(0).top -2;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(0).right;
					pt.y = imageheight - dragRect.Get(0).bottom;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(dragRectItemCount-1).right;
					pt.y = imageheight - dragRect.Get(0).bottom;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(dragRectItemCount-1).right;
					pt.y = imageheight - dragRect.Get(dragRectItemCount-1).bottom;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(dragRectItemCount-1).left;
					pt.y = imageheight - dragRect.Get(dragRectItemCount-1).bottom;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(dragRectItemCount-1).left;
					pt.y = imageheight - dragRect.Get(dragRectItemCount-1).top -2;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(0).left;
					pt.y = imageheight - dragRect.Get(dragRectItemCount-1).top -2;
					polyPoint.Add(pt);
					//
					pt.x = dragRect.Get(0).left;
					pt.y = imageheight - dragRect.Get(0).top -2;
					polyPoint.Add(pt);
				}
				//
				FramePoly(polyPoint,kColor_Gray50Percent,0.5);
			}
			//context解放
			mContextRef = NULL;
			[dragimage unlockFocus];
		}
		//Dragの結果としてviewが削除される可能性があるので、ここでretainしておく。（autoreleaseにより後で削除される）
		[[view retain] autorelease];
		//DroppedフラグOFF
		sDropped = false;
		//Drag実行
		[view dragImage:dragimage at:dragpt offset:NSMakeSize(0,0)
		event:[NSApp currentEvent] pasteboard:pboard source:view slideBack:YES];
		return sDropped;
	}
#if SUPPORT_CARBON
	else
	{
		OSStatus	err = noErr;
		
		DragRef	dragRef;
		err = ::NewDrag(&dragRef);
		if( err != noErr )   _ACErrorNum("NewDrag() returned error: ",err,this);
		
		RgnHandle	rgnHandle = ::NewRgn();
		::OpenRgn();
		for( AIndex i = 0; i < inDragRect.GetItemCount(); i++ )
		{
			AGlobalRect	globalDragRect;
			GetWin().GetGlobalRectFromControlLocalRect(GetControlID(),inDragRect.Get(i),globalDragRect);
			Rect	rect;
			rect.left		= globalDragRect.left;
			rect.top		= globalDragRect.top;
			rect.right		= globalDragRect.right;
			rect.bottom		= globalDragRect.bottom;
			::FrameRect(&rect);
		}
		::CloseRgn(rgnHandle);
		RgnHandle	innerRgn = ::NewRgn();
		::CopyRgn(rgnHandle,innerRgn);
		::InsetRgn(innerRgn,1,1);
		::DiffRgn(rgnHandle,innerRgn,rgnHandle); 
		::DisposeRgn(innerRgn);
		Rect	regionBounds;
		::GetRegionBounds(rgnHandle,&regionBounds);
		::SetDragItemBounds(dragRef,1,&regionBounds);
		if( err != noErr )   _ACErrorNum("SetDragItemBounds() returned error: ",err,this);
		
		AGlobalPoint	globalMousePoint;
		GetWin().GetGlobalPointFromControlLocalPoint(GetControlID(),inMousePoint,globalMousePoint);
		
		for( AIndex i = 0; i < inScrapType.GetItemCount(); i++ )
		{
			AText	data;
			inData.Get(i,data);
			AStTextPtr	ptr(data,0,data.GetItemCount());
			err = ::AddDragItemFlavor(dragRef,1,inScrapType.Get(i),ptr.GetPtr(),ptr.GetByteCount(),0);
			if( err != noErr )   _ACErrorNum("AddDragItemFlavor() returned error: ",err,this);
		}
		
		EventRecord	event;
		event.what = mouseDown;
		event.message = 0;
		event.when = TickCount();
		event.where.h = globalMousePoint.x;
		event.where.v = globalMousePoint.y;
		
		err = ::TrackDrag(dragRef,&event,rgnHandle);
		if( err != noErr )   _ACErrorNum("TrackDrag() returned error: ",err,this);
		err = ::DisposeDrag(dragRef);
		if( err != noErr )   _ACErrorNum("DisposeDrag() returned error: ",err,this);
	}
#endif
}

#pragma mark ===========================

#pragma mark ---Contextual menu

/**
コンテキストメニュー表示
*/
void	CUserPane::ShowContextMenu( const AContextMenuID inContextMenuID, const AGlobalPoint& inMousePoint )
{
	//view取得
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	if( view == nil )   { _ACError("",NULL); return; }
	//context menu取得
	AMenuRef	menuRef = AApplication::GetApplication().NVI_GetMenuManager().GetContextMenu(inContextMenuID);
	//context menu表示
	CAppImp::SetContextMenuTarget(GetAWin().GetObjectID(),mControlID);
	[NSMenu popUpContextMenu:(ACocoa::GetNSMenu(menuRef)) withEvent:[NSApp currentEvent] forView:view];
	CAppImp::SetContextMenuTarget(kObjectID_Invalid,kControlID_Invalid);
}

#pragma mark ===========================

#pragma mark ---イベントハンドラ（各種イベント発生時のCocoaからのコールバック）
//#688

/*
CocoaUserPaneView内の各コールバック（-mouseDown:等）→CWindowImp内のメソッド→ここのメソッド
のルートでコールされる。
*/

/**
描画
*/
void	CUserPane::APICB_CocoaDrawRect( const ALocalRect& inDirtyRect )
{
	//コントロールがVisibleでなければ何もしない
	if( IsControlVisible() == false )   return;
	
	//Context設定
	mCocoaDrawDirtyRect = inDirtyRect;
	mContextRef = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	
	//上記で取得したContextの座標系は、HIViewの左上が原点、y軸のプラスは下方向となっている。
	//しかし、Quartzでは、y軸のプラスが上方向であることが前提となっており、
	//このままだと、Textやピクチャの描画が上下逆さまになってしまう。
	//これを回避するため、y軸のプラスが上方向になるように、下記処理を行う。
	//（このため、Contextへの描画は、HIViewの左上が原点ではあるが、yはプラスマイナスを逆に変換する必要がある。）
	::CGContextScaleCTM(mContextRef, 1.0, -1.0);
	
	//#558
	//縦書き座標系にする
	if( mVerticalMode == true )
	{
		//座標系変換
		ChangeToVerticalCTM(mContextRef);
		//dirty rect座標系変換
		mCocoaDrawDirtyRect = mWindowImp.ConvertVerticalCoordinateFromImpToApp(GetControlID(),inDirtyRect);
	}
	
	//AWindow::EVT_DoDraw()実行
	GetAWin().EVT_DoDraw(GetControlID());
	
	//Context初期化
	mCocoaDrawDirtyRect = kLocalRect_0000;
	mContextRef = NULL;
}

#if SUPPORT_CARBON

#pragma mark ===========================

#pragma mark ---イベントハンドラ（OSコールバック用static）（Carbon）

/**
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus	CUserPane::STATIC_APICB_DrawHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(UP-DH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( CWindowImp::sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	OSStatus	result = eventNotHandledErr;
	ControlRef	controlRef = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&controlRef);
	if( controlRef == NULL )
	{
		_ACError("cannot get control",NULL);
		return result;
	}
	CGContextRef	contextRef = NULL;
	//CGContext取得
	//なお、ウインドウはCompositeにする必要がある。でないと、CGContextは取得できない。（contextRefがNULLとなる）
	err = ::GetEventParameter(inEventRef,kEventParamCGContextRef,typeCGContextRef,NULL,sizeof(CGContextRef),NULL,&(contextRef));
	if( contextRef == NULL )
	{
		_ACError("cannot get context (window is not composite?)",NULL);
		return result;
	}
	//
	//ローカル座標で取得される
	RgnHandle	rgnHandle = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamRgnHandle,typeQDRgnHandle,NULL,sizeof(RgnHandle),NULL,&rgnHandle);
	
	//CUserPaneオブジェクトインスタンスのイベントハンドラ実行
	try
	{
		//上記で取得したContextの座標系は、HIViewの左上が原点、y軸のプラスは下方向となっている。
		//しかし、Quartzでは、y軸のプラスが上方向であることが前提となっており、
		//このままだと、Textやピクチャの描画が上下逆さまになってしまう。
		//これを回避するため、y軸のプラスが上方向になるように、下記処理を行う。
		//（このため、Contextへの描画は、HIViewの左上が原点ではあるが、yはプラスマイナスを逆に変換する必要がある。）
		::CGContextScaleCTM(contextRef, 1.0, -1.0);
		AControlID	controlID = window->GetControlID(controlRef);
		window->GetUserPane(controlID).mContextRef = contextRef;
		window->GetUserPane(controlID).mDrawUpdateRegion = rgnHandle;
		if( window->GetUserPane(controlID).APICB_DoDraw() == true )
		{
			result = noErr;
		}
		window->GetUserPane(controlID).mContextRef = NULL;
		window->GetUserPane(controlID).mDrawUpdateRegion = NULL;
	}
	catch(...)
	{
		_ACError("CUserPane::STATIC_APICB_DrawHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[UP-DH]");
	return result;
}

/**
イベントハンドラ（OSコールバック用static）
*/
pascal OSStatus CUserPane::STATIC_APICB_ControlBoundsChangedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	OSStatus	err = noErr;
	
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(UP-CBCH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( CWindowImp::sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	OSStatus	result = eventNotHandledErr;
	ControlRef	controlRef = NULL;
	err = ::GetEventParameter(inEventRef,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&controlRef);
	if( controlRef == NULL )
	{
		_ACError("cannot get control",NULL);
		return result;
	}
	
	//CUserPaneオブジェクトインスタンスのイベントハンドラ実行
	try
	{
		AControlID	controlID = window->GetControlID(controlRef);
		if( window->GetUserPane(controlID).APICB_DoControlBoundsChanged(inEventRef) == true )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CUserPane::STATIC_APICB_ControlBoundsChangedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[UP-CBCH]");
	return result;
}

//イベントハンドラ（OSコールバック用static）
/*B0000 080810 pascal OSStatus CUserPane::STATIC_APICB_MouseExitedHandler( EventHandlerCallRef inCallRef, EventRef inEventRef, void *inUserData )
{
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"(UP-MEH)");
	//（各コールバック共通処理部分）
	//アプリ定常動作中でなければ何もせずリターン
	if( AApplication::GetApplication().NVI_IsWorking() == false )   return noErr;
	//イベントトランザクション前処理／後処理
	AStEventTransactionPrePostProcess	oneevent(false);
	//CWindowImpオブジェクトの生存チェック
	CWindowImp	*window = (CWindowImp*)inUserData;
	if( CWindowImp::sAliveWindowArray.Find(window) == kIndex_Invalid )   return noErr;
	
	OSStatus	result = eventNotHandledErr;
	ControlRef	controlRef = NULL;
	::GetEventParameter(inEventRef,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&controlRef);
	if( controlRef == NULL )
	{
		_ACError("cannot get control",NULL);
		return result;
	}
	
	//CUserPaneオブジェクトインスタンスのイベントハンドラ実行
	try
	{
		AControlID	controlID = window->GetControlID(controlRef);
		if( window->GetUserPane(controlID).DoMouseExited(inEventRef) == true )
		{
			result = noErr;
		}
	}
	catch(...)
	{
		_ACError("CUserPane::STATIC_APICB_MouseExitedHandler() caught exception.",NULL);//#199
	}
	if( AApplication::NVI_GetEnableAPICBTraceMode() == true )   fprintf(stderr,"[UP-MEH]");
	return result;
}
*/

/**
イベントハンドラインストール
*/
void CUserPane::InstallEventHandler()
{
	//
	OSStatus	err;
	EventHandlerRef	handlerRef;
	EventTypeSpec	typeSpec;
	typeSpec.eventClass = kEventClassControl;
	typeSpec.eventKind = kEventControlDraw;
	err = ::InstallEventHandler(::GetControlEventTarget(GetControlRef()),::NewEventHandlerUPP(CUserPane::STATIC_APICB_DrawHandler),
			1,&typeSpec,&mWindowImp,&handlerRef);
	if( err != noErr )   _ACError("",this);
	
	typeSpec.eventClass = kEventClassControl;
	typeSpec.eventKind = kEventControlBoundsChanged;
	err = ::InstallEventHandler(::GetControlEventTarget(GetControlRef()),::NewEventHandlerUPP(CUserPane::STATIC_APICB_ControlBoundsChangedHandler),
			1,&typeSpec,&mWindowImp,&handlerRef);
	if( err != noErr )   _ACError("",this);
	
}
#endif

#pragma mark ===========================

#pragma mark ---縦書き

//#558
/**
縦書き用座標系変換
*/
void	CUserPane::ChangeToVerticalCTM( CGContextRef inContextRef )
{
	ALocalRect	localRect = {0};
	GetControlLocalFrameRect(localRect);
	CGContextRotateCTM(inContextRef,-M_PI/2.0);
	CGContextTranslateCTM(inContextRef,0,localRect.right);
}

//#558
/**
縦書き設定
*/
void	CUserPane::SetVerticalMode( const ABool inVerticalMode )
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_7 )
	{
		//縦書きフラグ設定
		mVerticalMode = inVerticalMode;
		
		//CocoaUserPaneViewに縦書きモード設定
		CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
		[view setVerticalMode:inVerticalMode];
		
		//metrics更新
		UpdateMetrics();
	}
}

//#1309
/**
サービスメニュー可否
*/
void	CUserPane::SetServiceMenuAvailable( const ABool inAvailable )
{
	CocoaUserPaneView*	view = GetWin().GetCocoaObjects().FindCocoaUserPaneViewByTag(mControlID);
	[view setServiceMenuAvailable:inAvailable];
}

