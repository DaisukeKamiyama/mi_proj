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

AWindow_VSeparatorOverlay

*/

#include "stdafx.h"

#include "AWindow_VSeparatorOverlay.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_VSeparatorOverlay
#pragma mark ===========================
//VSperator表示オーバーレイウインドウ

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_VSeparatorOverlay::AWindow_VSeparatorOverlay():AWindow()
{
}
/**
デストラクタ
*/
AWindow_VSeparatorOverlay::~AWindow_VSeparatorOverlay()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_VSeparatorOverlay::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//
	UpdateViewBounds();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

/**
ウインドウ生成
*/
void	AWindow_VSeparatorOverlay::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
}

/**
新規タブ生成
@param inTabIndex 生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_VSeparatorOverlay::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	NVI_CreateVSeparatorView(kVSperatorViewControlID,kSeparatorLinePosition_Right,pt,10,10);
	UpdateViewBounds();
}

/**
View位置サイズ設定
*/
void	AWindow_VSeparatorOverlay::UpdateViewBounds()
{
	//
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	//
	AWindowPoint	pt = {0,0};
	NVI_SetControlPosition(kVSperatorViewControlID,pt);
	NVI_SetControlSize(kVSperatorViewControlID,windowBounds.right-windowBounds.left, windowBounds.bottom-windowBounds.top);
}

/**
Targetウインドウ設定
*/
void	AWindow_VSeparatorOverlay::SPI_SetTargetWindowID( const AWindowID inTargetWindowID )
{
	NVI_GetVSeparatorViewByControlID(kVSperatorViewControlID).SPI_SetTargetWindowID(inTargetWindowID);
}

/**
Separator位置設定
*/
void	AWindow_VSeparatorOverlay::SPI_SetLinePosition( const ASeparatorLinePosition inLinePosition )
{
	NVI_GetVSeparatorViewByControlID(kVSperatorViewControlID).SPI_SetLinePosition(inLinePosition);
}

//#634
/**
背景透過率設定
*/
/*#688
void	AWindow_VSeparatorOverlay::SPI_SetTransparency( const AFloatNumber inTransparency )
{
	NVI_GetVSeparatorViewByControlID(kVSperatorViewControlID).SPI_SetTransparency(inTransparency);
}
*/

