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

AView_SubWindowHeader
#723

*/

#include "stdafx.h"

#include "AView_SubWindowHeader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AView_SubWindowHeader
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_SubWindowHeader::AView_SubWindowHeader( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID)
{
	NVMC_SetOffscreenMode(true);
}

//デストラクタ
AView_SubWindowHeader::~AView_SubWindowHeader()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_SubWindowHeader::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_SubWindowHeader::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
描画要求時のコールバック(AView用)
*/
void	AView_SubWindowHeader::EVTDO_DoDraw()
{
	//=========================描画データ取得=========================
	//フォント取得
	//内容部分フォント
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//ヘッダ部分フォント
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	
	//フォント設定
	NVMC_SetDefaultTextProperty(headerfontname,9.0,kColor_Black,kTextStyle_Normal,true);
	
	//背景色描画
	AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("EEEEEE");//#1316 GetApp().SPI_GetSubWindowHeaderBackgroundColor();
	if( GetApp().NVI_IsDarkMode() == true )
	{
		backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
	}

	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	NVMC_PaintRect(frameRect,backgroundColor);
	
	//text描画rect取得
	ALocalRect	textRect = frameRect;
	textRect.top += 1;
	textRect.bottom -= 1;
	textRect.left += 8;
	textRect.right -= 3;
	
	//文字色取得
	AColor	letterColor = AColorWrapper::GetColorByHTMLFormatColor("303030");//#1316 GetApp().SPI_GetSubWindowHeaderLetterColor();
	if( GetApp().NVI_IsDarkMode() == true )
	{
		letterColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
	}
	
	//テキスト取得
	AText	ellipsisTitle;
	NVI_GetEllipsisTextWithFixedFirstCharacters(mText,textRect.right-textRect.left-16,5,ellipsisTitle);
	
	//テキスト描画
	NVMC_DrawText(textRect,ellipsisTitle,letterColor,kTextStyle_Bold,kJustification_Left);
	
	
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_SubWindowHeader::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	return false;
}

/**
テキスト設定
*/
void	AView_SubWindowHeader::NVIDO_SetText( const AText& inText )
{
	mText.SetText(inText);
	NVI_Refresh();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================


