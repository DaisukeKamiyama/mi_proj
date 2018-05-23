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

AView_SizeBox

*/

#include "stdafx.h"

#include "AView_SizeBox.h"
#include "../Imp.h"
#include "../Wrapper.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AView_SizeBox
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AView_SizeBox::AView_SizeBox( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mMouseTrackingMode(kMouseTrackingMode_None), mHover(false), mBackgroundColor(kColor_White)
,mClickLocalPoint(kLocalPoint_00),mPreviousMouseGlobalPoint(kGlobalPoint_00)
{
	NVI_SetDefaultCursor(kCursorType_ResizeNWSE);
	NVMC_EnableMouseLeaveEvent();
}

/**
デストラクタ
*/
AView_SizeBox::~AView_SizeBox()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_SizeBox::NVIDO_Init()
{
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_SizeBox::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_SizeBox::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//前回マウス位置の保存
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,mPreviousMouseGlobalPoint);
	//最初のクリック時のlocal point保存
	mClickLocalPoint = inLocalPoint;
	
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	NVM_SetMouseTrackingMode(true);
	//対象ウインドウ（未設定なら親ウインドウ）のサイズを変更
	AWindowID	winID = NVM_GetWindow().GetObjectID();
	if( NVM_GetWindow().NVI_GetSizeBoxTargetWindowID() != kObjectID_Invalid )
	{
		winID = NVM_GetWindow().NVI_GetSizeBoxTargetWindowID();
	}
	//リサイズ中を設定
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SetVirtualLiveResizing(true);
	return false;
}

/**
マウス移動イベント
*/
ABool	AView_SizeBox::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return true;
}

/**
マウス領域外移動イベント
*/
void	AView_SizeBox::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
}

//Mouse Tracking
void	AView_SizeBox::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	//グローバル座標に変換
	AGlobalPoint	mouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
	//前回マウス位置と同じなら何もせずループ継続
	if( mouseGlobalPoint.x == mPreviousMouseGlobalPoint.x && mouseGlobalPoint.y == mPreviousMouseGlobalPoint.y )
	{
		return;
	}
	//対象ウインドウ（未設定なら親ウインドウ）取得
	AWindowID	winID = NVM_GetWindow().GetObjectID();
	if( NVM_GetWindow().NVI_GetSizeBoxTargetWindowID() != kObjectID_Invalid )
	{
		winID = NVM_GetWindow().NVI_GetSizeBoxTargetWindowID();
	}
	//ウインドウリサイズ
	ARect	bounds = {0};
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_GetWindowBounds(bounds);
	bounds.right = mouseGlobalPoint.x + (viewRect.right - viewRect.left - mClickLocalPoint.x);
	bounds.bottom = mouseGlobalPoint.y + (viewRect.bottom - viewRect.top - mClickLocalPoint.y);
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SetWindowBounds(bounds);
	//前回マウス位置の保存
	mPreviousMouseGlobalPoint = mouseGlobalPoint;
}

//Mouse Tracking終了（マウスボタンUp）
void	AView_SizeBox::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	//グローバル座標に変換
	AGlobalPoint	mouseGlobalPoint = {0};
	AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
	//対象ウインドウ（未設定なら親ウインドウ）取得
	AWindowID	winID = NVM_GetWindow().GetObjectID();
	if( NVM_GetWindow().NVI_GetSizeBoxTargetWindowID() != kObjectID_Invalid )
	{
		winID = NVM_GetWindow().NVI_GetSizeBoxTargetWindowID();
	}
	//リサイズ中状態を解除
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SetVirtualLiveResizing(false);
	//ウインドウリサイズ
	ARect	bounds = {0};
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_GetWindowBounds(bounds);
	bounds.right = mouseGlobalPoint.x + (viewRect.right - viewRect.left - mClickLocalPoint.x);
	bounds.bottom = mouseGlobalPoint.y + (viewRect.bottom - viewRect.top - mClickLocalPoint.y);
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_SetWindowBounds(bounds);
	//
	AApplication::GetApplication().NVI_GetWindowByID(winID).NVI_ConstrainWindowPosition(true);
	
	//TrackingMode解除（DoMouseTrackEnd()の前にしないと、DoMouseTrackEnd()でSeparatorコントロールが削除されてしまうので）
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}

/**
描画要求時のコールバック(AView用)
*/
void	AView_SizeBox::EVTDO_DoDraw()
{
	//背景塗りつぶし
	ALocalRect	viewRect = {0};
	NVM_GetLocalViewRect(viewRect);
	NVMC_PaintRoundedRect(viewRect,mBackgroundColor,mBackgroundColor,kGradientType_Horizontal,1.0,1.0,
						  5.0,false,false,false,true);
	
}

