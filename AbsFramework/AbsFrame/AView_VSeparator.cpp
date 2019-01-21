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

AView_VSeparator

*/

#include "stdafx.h"

#include "AView_VSeparator.h"
//#include "AWindow_Text.h"
#include "../Imp.h"
#include "../Wrapper.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AView_VSeparator
#pragma mark ===========================
//分割線ビュー

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AView_VSeparator::AView_VSeparator( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mMouseTrackingMode(kMouseTrackingMode_None), /*#688 mMouseTrackByLoop(true),*/ mHover(false)
		,mLinePosition(kSeparatorLinePosition_Middle)
,mEventTargetWindowID(inWindowID),  mEnableBackgroundColor(false), mBackgroundColor(kColor_White)//#864 , mTransparency(0.5)
{
	NVI_SetDefaultCursor(kCursorType_ResizeLeftRight);
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	NVMC_EnableMouseLeaveEvent();
}

/**
デストラクタ
*/
AView_VSeparator::~AView_VSeparator()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_VSeparator::NVIDO_Init()
{
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_VSeparator::NVIDO_DoDeleted()
{
}

/**
ターゲットウインドウ設定
*/
void	AView_VSeparator::SPI_SetTargetWindowID( const AWindowID inTargetWindowID )
{
	mEventTargetWindowID = inTargetWindowID;
}

#pragma mark ===========================

#pragma mark <イベント処理>

const ANumber kViewDeleteHeight = 16;

#pragma mark ===========================

//マウスボタン押下時のコールバック(AView用)win 080709 drag loop書き換え
ABool	AView_VSeparator::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( inClickCount == 2 )
	{
		NVM_GetWindow().NVI_SeparatorDoubleClicked(mEventTargetWindowID,NVI_GetControlID());
		return true;
	}
	
	//前回マウス位置の保存
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mPreviousMouseGlobalPoint);
	
	//Windowsの場合は、ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	NVM_SetMouseTrackingMode(true);
	return false;
	//#688 }
	/*#688
	//マウスボタンが放されるまでループ
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//ローカル座標に変換
		ALocalPoint	mouseLocalPoint;
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		//
		DoMouseTrack(mouseLocalPoint);
	}
	//完了通知 #409
	NVM_GetWindow().NVI_SeparatorMoved(mEventTargetWindowID,NVI_GetControlID(),0,true);
	return false;
	*/
}

void	AView_VSeparator::DoMouseTrack( const ALocalPoint& inLocalPoint )
{
	//グローバル座標に変換
	AGlobalPoint	mouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
	//前回マウス位置と同じなら何もせずループ継続
	if( mouseGlobalPoint.x == mPreviousMouseGlobalPoint.x && mouseGlobalPoint.y == mPreviousMouseGlobalPoint.y )
	{
		return;
	}
	//TargetWindowへSeparator移動を通知
	ANumber	widthDelta = mouseGlobalPoint.x-mPreviousMouseGlobalPoint.x;
	ANumber	resultDelta = NVM_GetWindow().NVI_SeparatorMoved(mEventTargetWindowID,NVI_GetControlID(),widthDelta,false);//#409
	mouseGlobalPoint.x = mPreviousMouseGlobalPoint.x + resultDelta;
	//前回マウス位置の保存
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

/**
マウス移動イベント
*/
ABool	AView_VSeparator::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( inLocalPoint.x >= 0 && inLocalPoint.x <= NVI_GetViewWidth() )
	{
		if( mHover == false )
		{
			mHover = true;
			NVI_Refresh();
		}
	}
	else
	{
		if( mHover == true )
		{
			mHover = false;
			NVI_Refresh();
		}
	}
	return true;
}

/**
マウス領域外移動イベント
*/
void	AView_VSeparator::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	mHover = false;
	NVI_Refresh();
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_VSeparator::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	DoMouseTrack(inLocalPoint);
}

//Mouse Tracking終了（マウスボタンUp）
void	AView_VSeparator::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//完了通知 #409
	NVM_GetWindow().NVI_SeparatorMoved(mEventTargetWindowID,NVI_GetControlID(),0,true);
	//TrackingMode解除（DoMouseTrackEnd()の前にしないと、DoMouseTrackEnd()でSeparatorコントロールが削除されてしまうので）
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

/**
描画要求時のコールバック(AView用)
*/
void	AView_VSeparator::EVTDO_DoDraw()
{
	//背景塗りつぶし
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	if( mEnableBackgroundColor == true )//#864
	{
		NVMC_PaintRect(viewRect,mBackgroundColor);
	}
	else
	{
		//NVMC_EraseRect(viewRect);
		//EraseRectにすると、描画しない部分がマウスイベント透過されてしまうので、非常に薄い色で描画する。
		//#1316 目立たないようにするため、ダークモードは黒で表示。また、αもさらに下げる。
		AColor	color = kColor_White;
		if( AApplication::GetApplication().NVI_IsDarkMode() == true )
		{
			color = kColor_Black;
		}
		NVMC_PaintRect(viewRect,color,0.001);
	}
	
	//
	if( mLinePosition == kSeparatorLinePosition_None )   return;
	
	//分割線描画
	ALocalPoint	spt,ept;
	spt.x = viewRect.left+GetLineOffset();
	spt.y = viewRect.top-1;
	ept.x = viewRect.left+GetLineOffset();
	ept.y = viewRect.bottom;
	/*#725
	if( mHover == true )
	{
		AColor	color;
		AColorWrapper::GetHighlightColor(color);
		NVMC_DrawLine(spt,ept,color,1.0,0.0,2.0);
	}
	else
	*/
	{
		NVMC_DrawLine(spt,ept,kColor_Gray70Percent,0.3,0.0,1.0);
	}
}

#pragma mark ===========================

#pragma mark --- 線の位置設定

/**
線の位置設定
*/
void	AView_VSeparator::SPI_SetLinePosition( const ASeparatorLinePosition inPosition )
{
	mLinePosition = inPosition;
	NVI_Refresh();
}

/**
線の位置取得
*/
ANumber	AView_VSeparator::GetLineOffset() const
{
	ANumber	result = 0;
	switch(mLinePosition)
	{
	  case kSeparatorLinePosition_Left:
		{
			result = 0;
			break;
		}
	  case kSeparatorLinePosition_Right:
		{
			result = NVI_GetViewWidth()-1;
			break;
		}
	  case kSeparatorLinePosition_Middle:
	  default:
		{
			result = NVI_GetViewWidth()/2;
			break;
		}
	}
	return result;
}



