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

CPaneForAView

*/

#include "CPaneForAView.h"
//#1012 #include "StSetPort.h"
#include "../Frame.h"
#include "CWindowImpCocoaObjects.h"
#import "CocoaUserPaneView.h"

#pragma mark ===========================
#pragma mark [クラス]CPaneForAView
#pragma mark ===========================
//AView用コントロール

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
CPaneForAView::CPaneForAView( AObjectArrayItem* inParent, CWindowImp& inWindow, const AControlID inControlID )//#688const AControlRef inControlRef ) 
: CUserPane(inParent,inWindow,inControlID)//#688inControlRef)
{
	/*#852
	AText	fontname;//win
	AFontWrapper::GetDialogDefaultFontName(fontname);//win #375
	SetDefaultTextProperty(fontname,9.0,kColor_Black,kTextStyle_Normal,true);
	*/
/* 	HIViewTrackingAreaRef	ref;
	::HIViewNewTrackingArea(GetControlRef(),NULL,1,&ref);*/
}

/**
デストラクタ
*/
CPaneForAView::~CPaneForAView()
{
}

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---イベントハンドラ

/**
描画callback
*/
ABool	CPaneForAView::APICB_DoDraw()
{
	if( IsControlVisible() == false )   return true;
	//Quartz完全移行まではPortも設定する
	//#1012 StSetPort	s(GetWindowRef());
	//（AWindowを経由して）対応するAViewのイベントハンドラを実行
	GetAWin().EVT_DoDraw(GetControlID());
	return true;
}

/**
マウスクリックに対する処理
*/
ABool	CPaneForAView::DoMouseDown( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( IsControlVisible() == false )   return true;
	//Quartz完全移行まではPortも設定する
	//#1012 StSetPort	s(GetWindowRef());
	//ウインドウ座標系のクリック位置取得
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//UserPaneの位置取得
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//ローカル座標系（UserPaneの左上を原点とする座標系）に変換
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//キー取得
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//クリック回数取得
	UInt32	clickCount = 1;
	err = ::GetEventParameter(inEventRef,kEventParamClickCount,typeUInt32,NULL,sizeof(clickCount),NULL,&clickCount);
	//コンテクストメニューイベントなら、EVT_DoContextMenu()をコールしてみる
	if( ::IsShowContextualMenuEvent(inEventRef) == true )
	{
		if( GetAWin().EVT_DoContextMenu(GetControlID(),localPoint,modifiers,clickCount) == true )
		{
			return true;
		}
	}
	//（AWindowを経由して）対応するAViewのIIFCB_DoMouseDown()を実行
	if( GetAWin().EVT_DoMouseDown(GetControlID(),localPoint,modifiers,clickCount) == false )//#232 返り値がfalseの場合だけEVT_Clicked()実行
	{
		//
		if( clickCount == 1 )
		{
			GetAWin().EVT_Clicked(GetControlID(),modifiers);
		}
		else if( clickCount == 2 )
		{
			GetAWin().EVT_Clicked(GetControlID(),modifiers);//B0303 ウインドウアクティベートクリックが１回目になってしまい、EVT_Clicked()を呼べないことがあるので
			GetAWin().EVT_DoubleClicked(GetControlID());
		}
	}
	//（AWindowを経由して）対応するAViewのEVT_GetCursorType()を実行
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(GetControlID(),localPoint,modifiers));
	return true;
}

/**
マウスホイールcallback
*/
ABool	CPaneForAView::DoMouseWheel( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( IsControlVisible() == false )   return true;
	long	delta = 0;
	err = ::GetEventParameter(inEventRef,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
	EventMouseWheelAxis	axis = kEventMouseWheelAxisY;
	err = ::GetEventParameter(inEventRef,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
	ANumber	deltaX = 0;
	ANumber	deltaY = 0;
	if( axis == kEventMouseWheelAxisY )
	{
		deltaY = -delta;
	}
	else
	{
		deltaX = -delta;
	}
	//Quartz完全移行まではPortも設定する
	//#1012 StSetPort	s(GetWindowRef());
	//キー取得
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//ウインドウ座標系のマウス位置取得
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//UserPaneの位置取得
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//ローカル座標系（UserPaneの左上を原点とする座標系）に変換
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//（AWindowを経由して）対応するAViewのイベントハンドラを実行
	GetAWin().EVT_DoMouseWheel(GetControlID(),deltaX,deltaY,modifiers,localPoint);
	return true;
}

/**
マウス移動時のcallback
*/
ABool	CPaneForAView::DoMouseMoved( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( IsControlVisible() == false )   return true;
	//Quartz完全移行まではPortも設定する
	//#1012 StSetPort	s(GetWindowRef());
	//ウインドウ座標系のマウス位置取得
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//UserPaneの位置取得
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//ローカル座標系（UserPaneの左上を原点とする座標系）に変換
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//キー取得
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//B0000 080810
	//MouseTracking
	if( mEnableMouseLeaveEvent == true )
	{
		CAppImp::SetMouseTracking(GetWin().GetWindowRef(),GetControlID());
	}
	//B0000 080810
	DoAdjustCursor(inEventRef);//MouseMovedをtrueで返すとAdjustCursorがOSから来ないので、ここで実行
	//
	return GetAWin().EVT_DoMouseMoved(GetControlID(),localPoint,modifiers);
}

/**
カーソル調整callback
*/
ABool	CPaneForAView::DoAdjustCursor( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	if( IsControlVisible() == false )   return true;
	//Quartz完全移行まではPortも設定する
	//#1012 StSetPort	s(GetWindowRef());
	//ウインドウ座標系のマウス位置取得
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	if( err != noErr )   _ACErrorNum("GetEventParameter() returned error: ",err,this);
	//UserPaneの位置取得
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//ローカル座標系（UserPaneの左上を原点とする座標系）に変換
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//キー取得
	UInt32	modifiers = 0;
	err = ::GetEventParameter(inEventRef,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(modifiers),NULL,&modifiers);
	//（AWindowを経由して）対応するAViewのEVT_GetCursorType()を実行
	ACursorWrapper::SetCursor(GetAWin().EVT_GetCursorType(GetControlID(),localPoint,modifiers));
	return true;
}

/**
領域変更イベントハンドラ
*/
ABool	CPaneForAView::APICB_DoControlBoundsChanged( const EventRef inEventRef )
{
	GetAWin().EVT_DoControlBoundsChanged(GetControlID());
	return true;
}

/**
DragTracking callback
*/
void	CPaneForAView::DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
{
	//#364 EnableDrop()で登録したScrapTypeのみ受け付ける
	ABool	found = false;
	for( AIndex i = 0; i < mScrapTypeArray.GetItemCount(); i++ )
	{
		if( AScrapWrapper::ExistDragData(inDragRef,mScrapTypeArray.Get(i)) == true )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//
	GetAWin().EVT_DoDragTracking(GetControlID(),inDragRef,inMousePoint,inModifierKeys,outIsCopyOperation);
}

/**
DragEnter callback
*/
void	CPaneForAView::DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys )
{
	//#364 EnableDropで登録したScrapTypeのみ受け付ける
	ABool	found = false;
	for( AIndex i = 0; i < mScrapTypeArray.GetItemCount(); i++ )
	{
		if( AScrapWrapper::ExistDragData(inDragRef,mScrapTypeArray.Get(i)) == true )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//
	GetAWin().EVT_DoDragEnter(GetControlID(),inDragRef,inMousePoint,inModifierKeys);
}

/**
DragLeave callback
*/
void	CPaneForAView::DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys )
{
	//#364 EnableDropで登録したScrapTypeのみ受け付ける
	ABool	found = false;
	for( AIndex i = 0; i < mScrapTypeArray.GetItemCount(); i++ )
	{
		if( AScrapWrapper::ExistDragData(inDragRef,mScrapTypeArray.Get(i)) == true )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//
	GetAWin().EVT_DoDragLeave(GetControlID(),inDragRef,inMousePoint,inModifierKeys);
}

/**
DragReceive callback
*/
void	CPaneForAView::DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inMousePoint, const AModifierKeys inModifierKeys )
{
	//#364 EnableDropで登録したScrapTypeのみ受け付ける
	ABool	found = false;
	for( AIndex i = 0; i < mScrapTypeArray.GetItemCount(); i++ )
	{
		if( AScrapWrapper::ExistDragData(inDragRef,mScrapTypeArray.Get(i)) == true )
		{
			found = true;
			break;
		}
	}
	if( found == false )   return;
	//
	GetAWin().EVT_DoDragReceive(GetControlID(),inDragRef,inMousePoint,inModifierKeys);
}

//B0000 080810
/**
マウス領域外移動callback
*/
ABool	CPaneForAView::DoMouseExited( const EventRef inEventRef )
{
	OSStatus	err = noErr;
	
	//R0228
	if( IsControlVisible() == false )   return true;
	//Quartz完全移行まではPortも設定する
	//#1012 StSetPort	s(GetWindowRef());
	//ウインドウ座標系のクリック位置取得
	HIPoint	windowPoint;
	err = ::GetEventParameter(inEventRef,kEventParamWindowMouseLocation,typeHIPoint,NULL,sizeof(windowPoint),NULL,&windowPoint);
	//UserPaneの位置取得
	AWindowPoint	controlPoint;
	GetControlPosition(controlPoint);
	//ローカル座標系（UserPaneの左上を原点とする座標系）に変換
	ALocalPoint	localPoint;
	/*#688
	err = ::HIViewConvertPoint(&windowPoint,NULL,GetControlRef());
	if( err != noErr )   _ACErrorNum("HIViewConvertPoint() returned error: ",err,this);
	localPoint.x = windowPoint.x;
	localPoint.y = windowPoint.y;
	*/
	GetWinConst().GetControlLocalPointFromEventParamWindowMouseLocation(GetControlID(),windowPoint,localPoint);//#688
	//
	GetAWin().EVT_DoMouseLeft(GetControlID(),localPoint);
	return true;
}
#endif
//#138
/**
コントロールShow/Hide
*/
void	CPaneForAView::PreProcess_SetShowControl( const ABool inShow )//#688
{
	/*#688
	//継承処理：コントロールに対する処理
	CUserPane::SetShowControl(inShow);
	*/
	//AbsFrameイベント通知
	GetAWin().EVT_PreProcess_SetShowControl(GetControlID(),inShow);
}

#if SUPPORT_CARBON
#pragma mark ===========================

#pragma mark ---コントロールフォーカス処理

//#135
/**
フォーカス設定時にCWindowImpからコールされる
*/
void	CPaneForAView::SetFocus()
{
	GetAWin().EVT_DoViewFocusActivated(GetControlID());
}

//#135
/**
フォーカス解除
*/
void	CPaneForAView::ResetFocus()
{
	GetAWin().EVT_DoViewFocusDeactivated(GetControlID());
}
#endif
#pragma mark ===========================

//R0231
#pragma mark ---辞書

/**
辞書表示
*/
void	CPaneForAView::ShowDictionary( const AText& inWord, const ALocalPoint inLocalPoint ) const
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_6 )
	{
		//#1026 Mac OS X 10.9以降でHIDictionaryWindowShow()が動作しないので、CocoaのAPIに置き換え（showDefinitionForAttributedStringは10.6以降）
		
		//対象単語のAttributedString生成
		NSMutableAttributedString *attrString = [[NSMutableAttributedString alloc] initWithString:inWord.CreateNSString(true)];
		[attrString autorelease];
		
		//NSMutableAttributedStringの編集開始
		[attrString beginEditing];
		//range設定
		NSRange	range = NSMakeRange(0,[attrString length]);
		//通常フォント
		[attrString addAttribute:NSFontAttributeName value:CocoaObjects->GetNormalFont() range:range];
		if( GetVerticalMode() == true )
		{
			[attrString addAttribute:NSVerticalGlyphFormAttributeName value:[NSNumber numberWithInt:1] range:range];
		}
		//NSMutableAttributedStringの編集終了
		[attrString endEditing];
		
		//CocoaUserPaneView取得
		CocoaUserPaneView*	view = GetWinConst().GetCocoaObjects().FindCocoaUserPaneViewByTag(GetControlID());
		if( view == nil )   {_ACError("",NULL);return;}
		//座標変換
		ALocalPoint	localPoint = inLocalPoint;
		if( GetVerticalMode() == true )
		{
			localPoint = GetWinConst().ConvertVerticalCoordinateFromAppToImp(GetControlID(),localPoint);
		}
		//辞書表示
		NSPoint	pt = {localPoint.x,localPoint.y};
		[view showDefinitionForAttributedString:attrString atPoint:pt];
	}
	else
	{
		CFStringRef	str = inWord.CreateCFString();
		if( str == NULL )   {_ACError("",NULL);return;}
		CFRange	sel;
		sel.location = 0;
		sel.length = ::CFStringGetLength(str);
		/*#688
		AText	fontname;
		AFontWrapper::GetFontName(mFont,fontname);
		 */
		CFStringRef	fontnamestr = mFontName.CreateCFString();
		if( fontnamestr == NULL ) {_ACError("",NULL);::CFRelease(str);return;}
		CTFontRef	font = ::CTFontCreateWithName(fontnamestr,mFontSize,NULL);
		AGlobalPoint	globalpt;
		GetWinConst().GetGlobalPointFromControlLocalPoint(GetControlID(),inLocalPoint,globalpt);
		CGPoint	pt;
		pt.x = globalpt.x;
		pt.y = globalpt.y;
		::HIDictionaryWindowShow(NULL,str,sel,font,pt,false,NULL);
		::CFRelease(font);
		::CFRelease(fontnamestr);
		::CFRelease(str);
	}
}




