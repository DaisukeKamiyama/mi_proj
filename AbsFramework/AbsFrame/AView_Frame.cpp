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

AView_Frame

*/

#include "stdafx.h"

#include "../Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AView_Frame
#pragma mark ===========================
/**
枠表示のためのView
*/

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

@param inWindowID 親ウインドウのID
@param inID ViewのControlID
@param inContentControlID 中に表示するViewのControlID
*/
AView_Frame::AView_Frame( const AWindowID inWindowID, const AControlID inID, const AControlID inContentControlID ) 
		: AView(inWindowID,inID), mContentControlID(inContentControlID)
//#1316 ,mBackgroundColor(kColor_White), mFrameColor(kColor_Gray), mFrameColorDeactive(kColor_Gray70Percent)
		,mFocused(false)//#135
//#1316		,mEnableFrameBackgroundColor(false),mFrameBackgroundColor(kColor_White)//win
,mEnableFocusRing(true), mFrameViewType(/*#1316 kFrameViewType_Normal*/kFrameViewType_ToolTip) //#798 #725
//#1316 ,mBackgroundTransparency(1.0)//#688
,mExistText(false)//#1316
{
}

/**
デストラクタ
*/
AView_Frame::~AView_Frame()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_Frame::NVIDO_Init()
{
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_Frame::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
マウスボタン押下時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
@param inClickCout クリック回数
*/
ABool	AView_Frame::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//Frameの中のコントロールへマウスダウンイベントを転送する
	ALocalPoint	pt = {-1,-1};
	return NVM_GetWindow().EVT_DoMouseDown(mContentControlID,pt,inModifierKeys,inClickCount);
}

//描画要求時のコールバック(AView用)
void	AView_Frame::EVTDO_DoDraw()
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	
	//#135
	ALocalRect	realFrameRect = localFrameRect;
	realFrameRect.left		+= kFocusWidth;
	realFrameRect.right		-= kFocusWidth;
	realFrameRect.top		+= kFocusWidth;
	realFrameRect.bottom	-= kFocusWidth;
	
	//==============フレーム描画==============
	
	switch(mFrameViewType)
	{
		//tool tipの場合
	  case kFrameViewType_ToolTip://#1316 kFrameViewType_Normal:
		{
			AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("F7F7F7");
			AColor	frameColor = AColorWrapper::GetColorByHTMLFormatColor("EEEEEE");
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("282828");
				frameColor = AColorWrapper::GetColorByHTMLFormatColor("404040");
			}
			NVMC_PaintRect(realFrameRect,backgroundColor);
			NVMC_FrameRect(realFrameRect,frameColor);
			break;
		}
		//edit boxの場合
	  case kFrameViewType_EditBox:
		{
			//背景描画
			AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
			}
			NVMC_PaintRect(realFrameRect,backgroundColor);
			//コントロールがenableの場合のみフレーム描画
			if( NVI_GetEnabled() == true )
			{
				if( AApplication::GetApplication().NVI_IsDarkMode() == false )
				{
					NVMC_FrameRect(realFrameRect,AColorWrapper::GetColorByHTMLFormatColor("B1B1B1"));
					ALocalRect	frameRect2 = realFrameRect;
					frameRect2.left += 1;
					frameRect2.right -= 1;
					frameRect2.top += 1;
					frameRect2.bottom -= 1;
					NVMC_FrameRect(frameRect2,AColorWrapper::GetColorByHTMLFormatColor("F0F0F0"));
				}
				else
				{
					NVMC_FrameRect(realFrameRect,AColorWrapper::GetColorByHTMLFormatColor("434343"));
					ALocalRect	frameRect2 = realFrameRect;
					frameRect2.bottom = realFrameRect.top + 1;
					NVMC_FrameRect(frameRect2,AColorWrapper::GetColorByHTMLFormatColor("4A4A4A"));
					ALocalRect	frameRect3 = realFrameRect;
					frameRect3.top = realFrameRect.top + 1;
					frameRect3.bottom = realFrameRect.top + 2;
					NVMC_FrameRect(frameRect3,AColorWrapper::GetColorByHTMLFormatColor("383838"));
					ALocalRect	frameRect4 = realFrameRect;
					frameRect4.top = realFrameRect.bottom - 2;
					frameRect4.bottom = realFrameRect.bottom - 1;
					NVMC_FrameRect(frameRect4,AColorWrapper::GetColorByHTMLFormatColor("404040"));
					ALocalRect	frameRect5 = realFrameRect;
					frameRect5.top = realFrameRect.bottom - 1;
					NVMC_FrameRect(frameRect5,AColorWrapper::GetColorByHTMLFormatColor("666666"));
				}
			}
			break;
		}
		//filter boxの場合
	  case kFrameViewType_FilterBox:
		{
			//
			AColor	frameColor = AColorWrapper::GetColorByHTMLFormatColor("D0D0D0");
			AColor	fillColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				frameColor = AColorWrapper::GetColorByHTMLFormatColor("505050");
				fillColor = AColorWrapper::GetColorByHTMLFormatColor("202020");
			}
			ALocalRect	rect = realFrameRect;
			rect.left += 4;
			rect.right -= 4;
			rect.bottom -= 2;
            AFloatNumber	r = 10.0;
			if( mExistText == true )
			{
				NVMC_PaintRoundedRect(rect,fillColor,fillColor,
									  kGradientType_None,1.0,1.0,r,true,true,true,true);
			}
			NVMC_FrameRoundedRect(rect,frameColor,1.0,r,true,true,true,true,true,true,true,true,1.0);
			//虫眼鏡アイコン
			{
				ALocalPoint	pt = {rect.left + 4, 6};
				NVMC_DrawImage(kImageID_iconSwSearch,pt);
			}
			/*#1316
			//上の線描画
			{
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.top;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.top;
				NVMC_DrawLine(spt,ept,frameColor);//#1316 AColorWrapper::GetColorByHTMLFormatColor("bbc6d2"));//"4a91d5"));//"bbc6d2"));
			}
			//下の線描画
			{
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.bottom-1;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,frameColor);//#1316 AColorWrapper::GetColorByHTMLFormatColor("bbc6d2"));//"4a91d5"));//"bbc6d2"));
			}
			*/
			break;
		}
		//フレーム描画無し
	  case kFrameViewType_NoFrameDraw:
		{
			AColor	backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
			if( AApplication::GetApplication().NVI_IsDarkMode() == true )
			{
				backgroundColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
			}
			NVMC_PaintRect(realFrameRect,backgroundColor);
			break;
		}
	}
	
	//==============フォーカスリング描画==============
	
#if IMPLEMENTATION_FOR_MACOSX
	if( mFocused == true && mEnableFocusRing == true )//#798
	{
		NVMC_DrawFocusFrame(realFrameRect);
	}
#endif
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
色設定
@param inBackgroundColor 背景色
@param inFrameColor 枠色
@param inFrameColorDeactive deactivate時の枠色
*/
/*#1316
void	AView_Frame::SPI_SetColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive,
		const AFloatNumber inTransparency )
{
	mBackgroundColor = inBackColor;
	mFrameColor = inFrameColor;
	mFrameColorDeactive = inFrameColorDeactive;
	mBackgroundTransparency = inTransparency;
}
*/

/**
EditBoxの中身のテキストが存在しているかどうかを設定
*/
void	AView_Frame::SPI_SetExistText( const ABool inExist )
{
	mExistText = inExist;
}

//win
/**
フレーム部分背景色設定
*/
/*#1316
void	AView_Frame::SPI_SetFrameBackgroundColor( const ABool inEnableFrameBackgroundColor, const AColor inColor )
{
	mEnableFrameBackgroundColor = inEnableFrameBackgroundColor;
	mFrameBackgroundColor = inColor;
}
*/

//#135
/**
*/
void	AView_Frame::SPI_SetFocused( const ABool inFocused )
{
	mFocused = inFocused;
	NVI_Refresh();
}

//#135
/**
実際の描画FramのBoundを取得

AView_FramwはFocus描画分だけBoundsが大きくなるため
*/
void	AView_Frame::SPI_GetRealFrameBounds( AWindowRect& outRect ) const
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	NVM_GetWindowConst().NVI_GetWindowRectFromControlLocalRect(NVI_GetControlID(),localFrameRect,outRect);
	outRect.left	+= kFocusWidth;
	outRect.right	-= kFocusWidth;
	outRect.top		+= kFocusWidth;
	outRect.bottom	-= kFocusWidth;
}

//#135
/**
実際の描画Frameのサイズ・位置を設定
*/
void	AView_Frame::SPI_SetRealFramePositionAndSize( const AWindowPoint& inRealFramePoint, const ANumber inRealFrameWidth, const ANumber inRealFrameHeight )
{
	AWindowPoint	framePoint = inRealFramePoint;
	framePoint.x -= kFocusWidth;
	framePoint.y -= kFocusWidth;
	ANumber	frameWidth = inRealFrameWidth + kFocusWidth*2;
	ANumber	frameHeight = inRealFrameHeight + kFocusWidth*2;
	NVI_SetPosition(framePoint);
	NVI_SetSize(frameWidth,frameHeight);
}



