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

AView_SubTextShadow

*/

#include "stdafx.h"

#include "AView_SubTextShadow.h"
#include "AView_LineNumber.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AView_SubTextShadow
#pragma mark ===========================
/*
メインテキストカラムとサブテキストカラムの間に分割線（シャドウ）を表示
*/

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_SubTextShadow::AView_SubTextShadow( const AWindowID inWindowID, const AControlID inID ) : AView(inWindowID,inID)
{
	NVMC_SetOffscreenMode(true);
}

//デストラクタ
AView_SubTextShadow::~AView_SubTextShadow()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================



#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_SubTextShadow::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_SubTextShadow::NVIDO_DoDeleted()
{
}


/**
描画要求時のコールバック(AView用)
*/
void	AView_SubTextShadow::EVTDO_DoDraw()
{
	//frame取得
	ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
	
	//左から右へα値を変更するgradient表示
	ALocalRect	shadowrect = frameRect;
	NVMC_PaintRectWithHorizontalGradient(shadowrect,kColor_Gray,kColor_Gray,0.0,0.3);
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_SubTextShadow::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return false;
}


