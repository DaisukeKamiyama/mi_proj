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

AView_CallGrafHeader
#723

*/

#include "stdafx.h"

#include "AView_CallGrafHeader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AView_CallGrafHeader
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_CallGrafHeader::AView_CallGrafHeader( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mStartLevel(1)
{
	NVMC_SetOffscreenMode(true);
}

//デストラクタ
AView_CallGrafHeader::~AView_CallGrafHeader()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_CallGrafHeader::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_CallGrafHeader::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
描画要求時のコールバック(AView用)
*/
void	AView_CallGrafHeader::EVTDO_DoDraw()
{
	//フォント設定
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	NVMC_SetDefaultTextProperty(labelfontname,9,kColor_Black,kTextStyle_Normal,true);
	
	//背景色描画
	AColor	backgroundColor = GetApp().SPI_GetSubWindowHeaderBackgroundColor();
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	NVMC_PaintRect(frameRect,backgroundColor);
	
	//文字色取得
	AColor	letterColor = kColor_Black;//#1316 ★機能復活するならダークモード対応必要GetApp().SPI_GetSubWindowHeaderLetterColor();
	
	//カラムの幅を計算
	ANumber	columnWidth = (frameRect.right-frameRect.left)/2;
	
	//各カラム毎のループ
	for( AIndex column = 0; column <= 1; column++ )
	{
		//レベル取得
		AIndex	level = mStartLevel - column;
		
		//描画
		const ANumber	kMarginWidth = 2;
		const ANumber	kMarginHeight = 1;
		
		//1行目（レベル表示）
		{
			AText	text;
			text.SetLocalizedText("CallGrafLevel");
			text.ReplaceParamText('0',-level);
			ALocalRect	rect = frameRect;
			rect.top		= frameRect.top + kMarginHeight + 1;
			rect.bottom		= rect.top + kLineHeight;
			rect.left 		= column*columnWidth + kMarginWidth;
			rect.right 		= (column+1)*columnWidth - kMarginWidth;
			NVMC_DrawText(rect,text,letterColor,kTextStyle_Bold,kJustification_Center);
		}
		
		//2行目（各列タイトル）
		{
			//テキスト取得
			AText	text;
			if( level == 0 )
			{
				//レベル0用テキスト
				text.SetLocalizedText("CallGrafLevel0");
			}
			else if( level-1 < mCurrentWordArray.GetItemCount() )
			{
				//const ANumber	kMaxWordDisplayLength = 16;
				//各レベル用テキスト
				text.SetLocalizedText("CallGrafLevelX");
				AText	t;
				mCurrentWordArray.Get(level-1,t);
				text.ReplaceParamText('0',t);
			}
			
			//描画
			ALocalRect	rect = frameRect;
			rect.top		= frameRect.top + kMarginHeight + kLineHeight;
			rect.bottom		= rect.top + kLineHeight;
			rect.left 		= column*columnWidth + kMarginWidth;
			rect.right 		= (column+1)*columnWidth - kMarginWidth;
			AText	ellipsisText;
			NVI_GetEllipsisTextWithFixedFirstCharacters(text,rect.right-rect.left-8,8,ellipsisText);
			NVMC_DrawText(rect,ellipsisText,letterColor,kTextStyle_Bold,kJustification_Center);
		}
	}
	
	//左レベルボタン描画
	{
		ALocalPoint	pt = {0};
		pt.x = frameRect.left + 2;
		pt.y = frameRect.top + 2;
		NVMC_DrawImage(kImageID_btnNaviLeft,pt);
	}
	//右レベルボタン描画
	if( mStartLevel >= 2 )
	{
		ALocalPoint	pt = {0};
		pt.x = frameRect.right -18;
		pt.y = frameRect.top + 2;
		NVMC_DrawImage(kImageID_btnNaviRight,pt);
	}
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_CallGrafHeader::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//左レベル移動
	if( inLocalPoint.x < frameRect.left + 18 )
	{
		NVM_GetWindow().EVT_Clicked(AWindow_CallGraf::kControlID_LeftLevelButton,0);
	}
	//右レベル移動
	else if( inLocalPoint.x > frameRect.right - 18 )
	{
		NVM_GetWindow().EVT_Clicked(AWindow_CallGraf::kControlID_RightLevelButton,0);
	}
	
	return false;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
現在のデータ設定
*/
void	AView_CallGrafHeader::SPI_SetCurrentStatus( const AIndex inStartLevel, const ATextArray& inCurrentWordArray )
{
	mStartLevel = inStartLevel;
	mCurrentWordArray.SetFromTextArray(inCurrentWordArray);
	NVI_Refresh();
}


