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

AWindow_ToolTip

*/

#include "stdafx.h"

#include "AWindow_ToolTip.h"

const AControlID	kControlID_Text = 101;

#pragma mark ===========================
#pragma mark [クラス]AWindow_ToolTip
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_ToolTip::AWindow_ToolTip():AWindow()
{
}
/**
デストラクタ
*/
AWindow_ToolTip::~AWindow_ToolTip()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_ToolTip::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//view bounds更新
	UpdateViewBounds();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

/**
ウインドウ生成
*/
void	AWindow_ToolTip::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Floating,kOverlayWindowLayer_Top,false,false,false,false,false,true,false);
	
	//テキストボックス生成
	{
		AText	headerfontname;
		AFontWrapper::GetDialogDefaultFontName(headerfontname);
		AColor	backgrondColor = kColor_Gray97Percent;//#1079 AColorWrapper::GetColorByHTMLFormatColor("fcfac7");
		AWindowPoint	pt = {0,0};
		NVI_CreateEditBoxView(kControlID_Text,pt,10,10,kControlID_Invalid,kIndex_Invalid,false,false,false,true,kEditBoxType_ThinFrame);
		NVI_GetEditBoxView(kControlID_Text).SPI_SetBackgroundColor(
					backgrondColor,backgrondColor,backgrondColor);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetTextFont(headerfontname,10.0);
		NVI_GetEditBoxView(kControlID_Text).SPI_SetTransparency(0.98);//#1079 0.8から変更
		NVI_GetEditBoxView(kControlID_Text).SPI_SetEnableFocusRing(false);
		NVI_GetEditBoxView(kControlID_Text).NVI_SetAutomaticSelectBySwitchFocus(false);
		NVI_GetEditBoxView(kControlID_Text).SPI_SetReadOnly();
		NVI_SetControlBindings(kControlID_Text,true,true,true,true,false,false);
	}
	
}

/**
新規タブ生成
@param inTabIndex 生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_ToolTip::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
}

/**
View位置サイズ設定
*/
void	AWindow_ToolTip::UpdateViewBounds()
{
	//bounds取得
	ARect	windowBounds = {0};
	NVI_GetWindowBounds(windowBounds);
	//テキストview配置
	AWindowPoint	pt = {0,0};
	NVI_SetControlPosition(kControlID_Text,pt);
	NVI_SetControlSize(kControlID_Text,windowBounds.right-windowBounds.left, windowBounds.bottom-windowBounds.top);
}

/**
ツールチップテキスト設定
*/
void	AWindow_ToolTip::SPI_SetToolTipText( const AText& inToolTipText )
{
	//テキスト設定
	NVI_GetEditBoxView(kControlID_Text).NVI_SetText(inToolTipText);
	//ウインドウサイズ調整
	AdjustWindowSize();
	//テキストスクロール位置
	AImagePoint	pt = {0,0};
	NVI_GetEditBoxView(kControlID_Text).NVI_ScrollTo(pt);
}

/**
ウインドウサイズ調整
*/
void	AWindow_ToolTip::AdjustWindowSize()
{
	ANumber	w = NVI_GetEditBoxView(kControlID_Text).SPI_GetMaxDisplayWidth();
	ANumber	h = NVI_GetEditBoxView(kControlID_Text).NVM_GetImageHeight();
	w += 16;
	h += 8;
	NVI_SetWindowSize(w,h);
}



