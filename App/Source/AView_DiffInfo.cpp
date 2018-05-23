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

AView_DiffInfo
#379

*/

#include "stdafx.h"

#include "AView_DiffInfo.h"
#include "AView_LineNumber.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AView_DiffInfo
#pragma mark ===========================
//情報ウインドウのメインView

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_DiffInfo::AView_DiffInfo( const AWindowID inWindowID, const AControlID inID ) 
	: AView(inWindowID,inID), mTextWindowID(kObjectID_Invalid)
{
	NVMC_SetOffscreenMode(true);
}

//デストラクタ
AView_DiffInfo::~AView_DiffInfo()
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
void	AView_DiffInfo::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_DiffInfo::NVIDO_DoDeleted()
{
}


/**
描画要求時のコールバック(AView用)
*/
void	AView_DiffInfo::EVTDO_DoDraw()
{
	//
	ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
#if IMPLEMENTATION_FOR_WINDOWS
	//MacOSXの場合は、オーバーレイウインドウはもともと透明
	//Windowsの場合は、透明色で塗りつぶす。透明度は背景ウインドウで設定する。
	//（オーバーレイの場合）透明色で全体消去
	NVMC_EraseRect(frameRect);//win
#endif
	
	//
	if( mTextWindowID == kObjectID_Invalid )   return;
	if( GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetCurrentTabIndex() == kIndex_Invalid )   return;
	if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentViewCount() <= 0 )   return;
	
	//
	ADocumentID	docID = GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetCurrentDocumentID();
	if( docID == kObjectID_Invalid )   return;
	
	//
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//
	//ALocalRect	localFrameRect;
	//NVM_GetLocalViewRect(localFrameRect);
	//NVMC_PaintRect(localFrameRect,kColor_Red,1);
	//色取得
	AColor	diffcolor_added, diffcolor_changed, diffcolor_deleted;
	GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Changed,diffcolor_changed);
	GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Added,diffcolor_added);
	GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Deleted,diffcolor_deleted);
	//LineNumberViewからDiff表示情報取得
	AArray<ADiffType>	diffTypeArray;
	AArray<AIndex>	diffIndexArray;
	AArray<ALocalPoint>	diffStartLeftPointArray;
	AArray<ALocalPoint>	diffStartRightPointArray;
	AArray<ALocalPoint>	diffEndLeftPointArray;
	AArray<ALocalPoint>	diffEndRightPointArray;
	GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentLineNumberView(0).
			SPI_GetDiffDrawData(diffTypeArray,diffIndexArray,diffStartLeftPointArray,diffStartRightPointArray,
								diffEndLeftPointArray,diffEndRightPointArray);
	//現在のフォーカスのモード取得
	AModeIndex	modeIndex = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentFocusTextDocument().SPI_GetModeIndex();
	//diff表示α値取得
	AFloatNumber	diffAlpha = GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Number(AModePrefDB::kDiffColorOpacity);
	diffAlpha /= 300;//0.0～0.33にする
	//濃さ設定が大きいほど彩度を下げて、明度を上げる
	AFloatNumber	diffSChange = 1.0 - diffAlpha*2.0;
	AFloatNumber	diffVChange = 1.0 + diffAlpha*3.0;
	diffcolor_added = AColorWrapper::ChangeHSV(diffcolor_added,0.0,diffSChange,diffVChange);
	diffcolor_deleted = AColorWrapper::ChangeHSV(diffcolor_deleted,0.0,diffSChange,diffVChange);
	diffcolor_changed = AColorWrapper::ChangeHSV(diffcolor_changed,0.0,diffSChange,diffVChange);
	//フレーム（行全体、差分文字）のα倍率
	ANumber	diffPartFrameLineAlphaMultiply = 6;
	//Diff表示描画
	for( AIndex i = 0; i < diffTypeArray.GetItemCount(); i++ )
	{
		//
		ALocalPoint	startLeftPoint = diffStartLeftPointArray.Get(i);
		ALocalPoint	startRightPoint = diffStartRightPointArray.Get(i);
		ALocalPoint	endLeftPoint = diffEndLeftPointArray.Get(i);
		ALocalPoint	endRightPoint = diffEndRightPointArray.Get(i);
		if( IsPointInRect(startLeftPoint,frameRect) == true || IsPointInRect(startRightPoint,frameRect) == true ||
					IsPointInRect(endLeftPoint,frameRect) == true || IsPointInRect(endRightPoint,frameRect) == true )
		{
			AArray<ALocalPoint>	path;
			switch(diffTypeArray.Get(i))
			{
			  case kDiffType_Added:
				{
					endLeftPoint.y += 3;
					path.Add(startLeftPoint);
					path.Add(startRightPoint);
					path.Add(endRightPoint);
					path.Add(endLeftPoint);
					NVMC_PaintPoly(path,diffcolor_added,diffAlpha);
					//上下の線を描画
					if( IsPointInRect(startLeftPoint,frameRect) == true ||
					   IsPointInRect(startRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(startLeftPoint,startRightPoint,diffcolor_added,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					if( IsPointInRect(endLeftPoint,frameRect) == true ||
					   IsPointInRect(endRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(endLeftPoint,endRightPoint,diffcolor_added,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					break;
				}
			  case kDiffType_Deleted:
				{
					endRightPoint.x = startRightPoint.x;
					endRightPoint.y = startRightPoint.y +3;
					path.Add(startLeftPoint);
					path.Add(startRightPoint);
					path.Add(endRightPoint);
					path.Add(endLeftPoint);
					NVMC_PaintPoly(path,diffcolor_deleted,diffAlpha);
					//上下の線を描画
					if( IsPointInRect(startLeftPoint,frameRect) == true ||
					   IsPointInRect(startRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(startLeftPoint,startRightPoint,diffcolor_deleted,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					if( IsPointInRect(endLeftPoint,frameRect) == true ||
					   IsPointInRect(endRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(endLeftPoint,endRightPoint,diffcolor_deleted,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					break;
				}
			  case kDiffType_Changed:
				{
					path.Add(startLeftPoint);
					path.Add(startRightPoint);
					path.Add(endRightPoint);
					path.Add(endLeftPoint);
					NVMC_PaintPoly(path,diffcolor_changed,diffAlpha);
					//上下の線を描画
					if( IsPointInRect(startLeftPoint,frameRect) == true ||
					   IsPointInRect(startRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(startLeftPoint,startRightPoint,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					if( IsPointInRect(endLeftPoint,frameRect) == true ||
					   IsPointInRect(endRightPoint,frameRect) == true )
					{
						NVMC_DrawLine(endLeftPoint,endRightPoint,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply);
					}
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
		}
	}
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_DiffInfo::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//LineNumberViewへリダイレクト
	return GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentLineNumberView(0).
			EVTDO_DoMouseDown(inLocalPoint,inModifierKeys,inClickCount);
}


