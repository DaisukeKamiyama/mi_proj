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

AWindow_ButtonOverlay

*/

#include "stdafx.h"

#include "AWindow_ButtonOverlay.h"

const AControlID	kButtonViewControlID = 101;

#pragma mark ===========================
#pragma mark [クラス]AWindow_ButtonOverlay
#pragma mark ===========================
//HSperator表示オーバーレイウインドウ

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AWindow_ButtonOverlay::AWindow_ButtonOverlay():AWindow()
,mClickEventTargetWindowID(kObjectID_Invalid),mClickEventVirtualControlID(kControlID_Invalid)
{
}
/**
デストラクタ
*/
AWindow_ButtonOverlay::~AWindow_ButtonOverlay()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
クリック時の処理（リダイレクト）
*/
ABool	AWindow_ButtonOverlay::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	return AApplication::GetApplication().NVI_GetWindowByID(mClickEventTargetWindowID).EVT_Clicked(mClickEventVirtualControlID,inModifierKeys);;
}

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_ButtonOverlay::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//位置更新 #688
	AWindowPoint	pt = {0,0};
	NVI_GetButtonViewByControlID(kButtonViewControlID).NVI_SetPosition(pt);
	//サイズ更新
	NVI_GetButtonViewByControlID(kButtonViewControlID).
			NVI_SetSize(inCurrentBounds.right-inCurrentBounds.left,inCurrentBounds.bottom-inCurrentBounds.top);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

/**
ウインドウ生成
*/
void	AWindow_ButtonOverlay::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_Top,false,false,false,false,false,false,false);
}

/**
新規タブ生成
@param inTabIndex 生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_ButtonOverlay::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	//ボタン生成
	AWindowPoint	pt = {0,0};
	NVI_CreateButtonView(kButtonViewControlID,pt,NVI_GetWindowWidth(),NVI_GetWindowHeight(),kControlID_Invalid);
	NVI_GetButtonViewByControlID(kButtonViewControlID).SPI_SetAlwaysActiveColors(true);
}

/**
Targetウインドウ設定
*/
void	AWindow_ButtonOverlay::SPI_SetTargetWindowID( const AWindowID inTargetWindowID, const AControlID inVirtualControlID )
{
	mClickEventTargetWindowID = inTargetWindowID;
	mClickEventVirtualControlID = inVirtualControlID;
}

/**
ボタンアイコン設定
*/
void	AWindow_ButtonOverlay::SPI_SetIcon( const AImageID inIconID, const ANumber inLeftOffset, const ANumber inTopOffset,
		const ANumber inWidth, const ANumber inHeight, const AImageID inHoverIconImageID, const AImageID inToggleOnImageID )
{
	NVI_GetButtonViewByControlID(kButtonViewControlID).SPI_SetIcon(inIconID,inLeftOffset,inTopOffset,inWidth,inHeight,false,inHoverIconImageID,inToggleOnImageID);
}

/**
ボタンタイプ設定
*/
void	AWindow_ButtonOverlay::SPI_SetButtonViewType( const AButtonViewType inType )
{
	NVI_GetButtonViewByControlID(kButtonViewControlID).SPI_SetButtonViewType(inType);
}

//#661
/**
ヘルプタグ設定
*/
void	AWindow_ButtonOverlay::SPI_SetHelpTag( const AText& inText1, const AText& inText2, const AHelpTagSide inTagSide )
{
	NVI_GetButtonViewByControlID(kButtonViewControlID).SPI_SetHelpTag(inText1,inText2,inTagSide);
}

