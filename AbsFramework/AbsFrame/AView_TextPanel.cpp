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

AView_TextPanel

*/

#include "stdafx.h"

#include "AView_TextPanel.h"
#include "../Imp.h"
#include "../Wrapper.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AView_TextPanel
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

@param inWindowID 親ウインドウのID
@param inID ViewのControlID
*/
AView_TextPanel::AView_TextPanel( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mFontSize(9.0)
,mTextColor(kColor_Black), mTextStyle(kTextStyle_Normal), mTextJustification(kJustification_Left)
{
}

/**
デストラクタ
*/
AView_TextPanel::~AView_TextPanel()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_TextPanel::NVIDO_Init()
{
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_TextPanel::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_TextPanel::EVTDO_DoDraw()
{
	//背景描画
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	
	//白色ステータスバー
	NVMC_PaintRoundedRect(localFrameRect,
						  kColor_White,kColor_Gray95Percent,kGradientType_Vertical,mBackgroundTransparency,mBackgroundTransparency,
						  5.0,false,false,true,false);
	NVMC_FrameRoundedRect(localFrameRect,kColor_Gray20Percent,1.0,5.0,false,false,true,false);
	
	//区切り線
	ALocalPoint	spt = {localFrameRect.left,localFrameRect.top};
	ALocalPoint	ept = {localFrameRect.right,localFrameRect.top};
	NVMC_DrawLine(spt,ept,kColor_Gray80Percent);
	
	//テキスト描画
	ALocalRect	drawRect = localFrameRect;
	drawRect.top	+= 2;
	drawRect.left	+= 10;
	drawRect.bottom	-= 2;
	drawRect.right	-= 3;
	NVMC_SetDefaultTextProperty(mFontName,mFontSize,mTextColor,mTextStyle,true);
	NVMC_DrawText(drawRect,mText,mTextJustification);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
フォント設定
*/
void	AView_TextPanel::NVIDO_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize )
{
	mFontName = inFontName;
	mFontSize = inFontSize;
}

/**
テキスト設定
*/
void	AView_TextPanel::NVIDO_SetText( const AText& inText )
{
	mText.SetText(inText);
	NVI_Refresh();
}

/**
テキストプロパティ設定
*/
void	AView_TextPanel::NVIDO_SetTextProperty( const AColor& inColor, const ATextStyle inStyle , const AJustification inJustification )
{
	mTextColor = inColor;
	mTextStyle = inStyle;
	mTextJustification = inJustification;
	NVI_Refresh();
}



