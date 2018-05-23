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

AView_TextViewHSeparator

*/

#include "stdafx.h"

#include "AView_TextViewHSeparator.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AView_Text.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AView_TextViewHSeparator
#pragma mark ===========================
//分割線ビュー

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AView_TextViewHSeparator::AView_TextViewHSeparator( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mMouseTrackingMode(kMouseTrackingMode_None), mHover(false)
		,mLinePosition(kSeparatorLinePosition_Middle),mTargetWindowID(inWindowID)
{
	NVI_SetDefaultCursor(kCursorType_ResizeUpDown);
	NVMC_EnableMouseLeaveEvent();
}

/**
デストラクタ
*/
AView_TextViewHSeparator::~AView_TextViewHSeparator()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_TextViewHSeparator::NVIDO_Init()
{
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_TextViewHSeparator::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

const ANumber kViewDeleteHeight = 16;

#pragma mark ===========================

//マウスボタン押下時のコールバック(AView用)win 080709 drag loop書き換え
ABool	AView_TextViewHSeparator::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( inClickCount == 2 )
	{
		NVM_GetWindow().NVI_SeparatorDoubleClicked(mTargetWindowID,NVI_GetControlID());
		return true;
	}
	
	//前回マウス位置の保存
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mPreviousMouseGlobalPoint);
	
	//Windowsの場合は、ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	NVM_SetMouseTrackingMode(true);
	return false;
}

void	AView_TextViewHSeparator::DoMouseTrack( const ALocalPoint& inLocalPoint )
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
	ANumber	widthDelta = mouseGlobalPoint.y-mPreviousMouseGlobalPoint.y;
	ANumber	resultDelta = NVM_GetWindow().NVI_SeparatorMoved(mTargetWindowID,NVI_GetControlID(),widthDelta,false);//#409
	mouseGlobalPoint.y = mPreviousMouseGlobalPoint.y + resultDelta;
	//前回マウス位置の保存
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

/**
マウス移動イベント
*/
ABool	AView_TextViewHSeparator::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( inLocalPoint.y >= 0 && inLocalPoint.y <= NVI_GetViewHeight() )
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
void	AView_TextViewHSeparator::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	mHover = false;
	NVI_Refresh();
}

//Mouse Tracking
void	AView_TextViewHSeparator::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	DoMouseTrack(inLocalPoint);
}

//Mouse Tracking終了（マウスボタンUp）
void	AView_TextViewHSeparator::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//完了通知 #409
	NVM_GetWindow().NVI_SeparatorMoved(mTargetWindowID,NVI_GetControlID(),0,true);
	//TrackingMode解除（DoMouseTrackEnd()の前にしないと、DoMouseTrackEnd()でSeparatorコントロールが削除されてしまうので）
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}

/**
描画要求時のコールバック(AView用)
*/
void	AView_TextViewHSeparator::EVTDO_DoDraw()
{
	AWindowID	winID = NVM_GetWindow().GetObjectID();
	AModeIndex	modeIndex = GetApp().SPI_GetTextWindowByID(winID).SPI_GetCurrentTabTextDocument().SPI_GetModeIndex();
	//
	AColor	backgroundColor = kColor_White;
	GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kBackgroundColor,backgroundColor);
	//背景塗りつぶし
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	NVMC_PaintRect(viewRect,backgroundColor);
	
	//
	if( mLinePosition == kSeparatorLinePosition_None )   return;
	
	//分割線描画
	ALocalPoint	spt,ept;
	spt.x = viewRect.left;
	spt.y = viewRect.top+GetLineOffset();
	ept.x = viewRect.right;
	ept.y = viewRect.top+GetLineOffset();
	
	//gradient paint
	ALocalRect	rect = viewRect;
	rect.bottom = spt.y;
	//NVMC_PaintRectWithVerticalGradient(rect,kColor_White,kColor_Gray85Percent,0.8,0.8);
	NVMC_PaintRectWithVerticalGradient(rect,kColor_Gray70Percent,kColor_Gray70Percent,0.0,0.5);
	//線描画
	NVMC_DrawLine(spt,ept,kColor_Gray70Percent,0.8,0.0,1.0);
}

#pragma mark ===========================

#pragma mark --- 線の位置設定

/**
線の位置設定
*/
void	AView_TextViewHSeparator::SPI_SetLinePosition( const ASeparatorLinePosition inPosition )
{
	mLinePosition = inPosition;
	NVI_Refresh();
}

/**
線の位置取得
*/
ANumber	AView_TextViewHSeparator::GetLineOffset() const
{
	ANumber	result = 0;
	switch(mLinePosition)
	{
	  case kSeparatorLinePosition_Top:
		{
			result = 0;
			break;
		}
	  case kSeparatorLinePosition_Bottom:
		{
			result = NVI_GetViewHeight()-1;
			break;
		}
	  case kSeparatorLinePosition_Middle:
	  default:
		{
			result = NVI_GetViewHeight()/2;
			break;
		}
	}
	return result;
}



