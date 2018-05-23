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

AView_Plain

*/

#include "stdafx.h"

#include "../Frame.h"
#include "AView_Plain.h"

#pragma mark ===========================
#pragma mark [クラス]AView_Plain
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

@param inWindowID 親ウインドウのID
@param inID ViewのControlID
*/
AView_Plain::AView_Plain( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mColor(kColor_White), mColorDeactive(kColor_White)
,mLeftBottomRounded(false), mRightBottomRounded(false)
,mForRightSideBarSeparator(false), mForLeftSideBarSeparator(false)
{
	//viewを不透明に設定 #1090
	NVMC_SetOpaque(true);
}

/**
デストラクタ
*/
AView_Plain::~AView_Plain()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_Plain::NVIDO_Init()
{
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_Plain::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_Plain::EVTDO_DoDraw()
{
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	if( NVM_GetWindow().NVI_IsWindowActive() == true )
	{
		//ウインドウactiveの場合
		NVMC_PaintRoundedRect(localFrameRect,
							  mColor,mColor,kGradientType_None,1.0,1.0,
							  7.0,false,false,mLeftBottomRounded,mRightBottomRounded);//#1275 kGradientType_Horizontal→kGradientType_None
	}
	else
	{
		//ウインドウdeactiveの場合
		NVMC_PaintRoundedRect(localFrameRect,
							  mColorDeactive,mColorDeactive,kGradientType_None,1.0,1.0,
							  7.0,false,false,mLeftBottomRounded,mRightBottomRounded);//#1275 kGradientType_Horizontal→kGradientType_None
	}
	//右サイドバーの場合、左側に分割線を表示
	if( mForRightSideBarSeparator == true )
	{
		NVMC_FrameRect(localFrameRect,mSeparatorLineColor,1.0,true,false,false,false);
	}
	//左サイドバーの場合、右側に分割線を表示
	if( mForLeftSideBarSeparator == true )
	{
		NVMC_FrameRect(localFrameRect,mSeparatorLineColor,1.0,false,false,true,false);
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
色設定
@param inColor 背景色
@param inColorDeactive ウインドウdeactive時の背景色
*/
void	AView_Plain::SPI_SetColor( const AColor inColor, const AColor inColorDeactive, 
								  const ABool inLeftBottomRounded, const ABool inRightBottomRounded,
								  const ABool inForLeftSideBarSeparator, const ABool inForRightSideBarSeparator, const AColor inSeparatorLineColor )
{
	mColor = inColor;
	mColorDeactive = inColorDeactive;
	//rounded 
	mLeftBottomRounded = inLeftBottomRounded;
	mRightBottomRounded = inRightBottomRounded;
	//サイドバー用セパレータ
	mForRightSideBarSeparator = inForRightSideBarSeparator;
	mForLeftSideBarSeparator = inForLeftSideBarSeparator;
	mSeparatorLineColor = inSeparatorLineColor;
}

