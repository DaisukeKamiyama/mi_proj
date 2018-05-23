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

AWindow_DiffInfo
#379

*/

#include "stdafx.h"

#include "AWindow_DiffInfo.h"
#include "AView_DiffInfo.h"
#include "AApp.h"

//
const AControlID	kInfoViewControlID = 1;

#pragma mark ===========================
#pragma mark [クラス]AWindow_DiffInfo
#pragma mark ===========================
//Diff情報ウインドウのクラス

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_DiffInfo::AWindow_DiffInfo():AWindow()
{
}
//デストラクタ
AWindow_DiffInfo::~AWindow_DiffInfo()
{
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

/**
ウインドウ生成
*/
void	AWindow_DiffInfo::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ実体生成
	NVM_CreateWindow(kWindowClass_Overlay,kOverlayWindowLayer_None,false,false,false,false,true,false,false);
}

/**
新規タブ生成
@param inTabIndex 生成するタブのインデックス　※ここに来た時、まだ、NVI_GetCurrentTabIndex()は生成するタブのインデックスではない
*/
void	AWindow_DiffInfo::NVIDO_CreateTab( const AIndex inTabIndex, AControlID& outInitialFocusControlID )
{
	AWindowPoint	pt = {0,0};
	//IdInfoView生成
	AViewDefaultFactory<AView_DiffInfo>	infoViewFactory(GetObjectID(),kInfoViewControlID);
	NVM_CreateView(kInfoViewControlID,pt,500,500,kControlID_Invalid,kControlID_Invalid,false,infoViewFactory);
	//#291 サイズbinding設定（区切り線変更時のちらつき防止）
	NVI_SetControlBindings(kInfoViewControlID,true,true,true,true,false,false);
}

/**
ウインドウサイズ変更通知時のコールバック
*/
void	AWindow_DiffInfo::EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds )
{
	//view bounds設定
	ARect	rect = {0};
	NVI_GetWindowBounds(rect);
	GetDiffInfoView().NVI_SetSize(rect.right-rect.left,rect.bottom-rect.top);
	AWindowPoint	pt = {0,0};
	GetDiffInfoView().NVI_SetPosition(pt);
}

/**
*/
void	AWindow_DiffInfo::SPI_SetTextWindowID( const AWindowID inTextWindowID )
{
	GetDiffInfoView().SPI_SetTextWindowID(inTextWindowID);
}

/**
*/
AView_DiffInfo&	AWindow_DiffInfo::GetDiffInfoView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(AView_DiffInfo,kViewType_DiffInfo,kInfoViewControlID);
}

