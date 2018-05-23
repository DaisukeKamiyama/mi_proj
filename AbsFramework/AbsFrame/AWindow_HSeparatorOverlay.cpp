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

AWindow_HSeparatorOverlay

*/

#include "stdafx.h"

#include "AWindow_HSeparatorOverlay.h"

const AControlID	kHSperatorViewControlID = 101;

#pragma mark ===========================
#pragma mark [クラス]AWindow_HSeparatorOverlay
#pragma mark ===========================
//HSperator表示オーバーレイウインドウ

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_HSeparatorOverlay::AWindow_HSeparatorOverlay():AWindow()
{
}
/**
デストラクタ
*/
AWindow_HSeparatorOverlay::~AWindow_HSeparatorOverlay()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_HSeparatorOverlay::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
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
void	AWindow_HSeparatorOverlay::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
}

/**
新規タブ生成
@param inTabIndex 生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_HSeparatorOverlay::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	NVI_CreateHSeparatorView(kHSperatorViewControlID,kSeparatorLinePosition_Middle,pt,10,10);
	UpdateViewBounds();
}

/**
View位置サイズ設定
*/
void	AWindow_HSeparatorOverlay::UpdateViewBounds()
{
	//
	ARect	windowBounds;
	NVI_GetWindowBounds(windowBounds);
	//
	AWindowPoint	pt = {0,0};
	NVI_SetControlPosition(kHSperatorViewControlID,pt);
	NVI_SetControlSize(kHSperatorViewControlID,windowBounds.right-windowBounds.left, windowBounds.bottom-windowBounds.top);
}

/**
Targetウインドウ設定
*/
void	AWindow_HSeparatorOverlay::SPI_SetTargetWindowID( const AWindowID inTargetWindowID )
{
	NVI_GetHSeparatorViewByControlID(kHSperatorViewControlID).SPI_SetTargetWindowID(inTargetWindowID);
}

/**
Separator位置設定
*/
void	AWindow_HSeparatorOverlay::SPI_SetLinePosition( const ASeparatorLinePosition inLinePosition )
{
	NVI_GetHSeparatorViewByControlID(kHSperatorViewControlID).SPI_SetLinePosition(inLinePosition);
}

//#634
/**
背景透過率設定
*/
/*#688
void	AWindow_HSeparatorOverlay::SPI_SetTransparency( const AFloatNumber inTransparency )
{
	NVI_GetHSeparatorViewByControlID(kHSperatorViewControlID).SPI_SetTransparency(inTransparency);
}
*/
