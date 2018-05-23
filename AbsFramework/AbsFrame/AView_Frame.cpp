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
		: AView(inWindowID,inID), mContentControlID(inContentControlID), 
		mBackgroundColor(kColor_White), mFrameColor(kColor_Gray), mFrameColorDeactive(kColor_Gray70Percent)
		,mFocused(false)//#135
		,mEnableFrameBackgroundColor(false),mFrameBackgroundColor(kColor_White)//win
,mEnableFocusRing(true), mFrameViewType(kFrameViewType_Normal) //#798 #725
,mBackgroundTransparency(1.0)//#688
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
	
	//==============背景描画==============
	
	//全体を背景色消去 win
	if( mEnableFrameBackgroundColor == false )
	{
		NVMC_EraseRect(localFrameRect);
	}
	else
	{
		NVMC_PaintRect(localFrameRect,mFrameBackgroundColor);
	}
	//
	AColor	framecolor = mFrameColor;
	if( NVM_GetWindow().NVI_IsWindowActive() == false )
	{
		framecolor = mFrameColorDeactive;
	}
	//Disable時はDeactive色にする
	if( NVI_GetEnabled() == false )
	{
		framecolor = mFrameColorDeactive;
	}
#if IMPLEMENTATION_FOR_WINDOWS
	//win
	if( mFocused == true && mEnableFocusRing == true )//#798
	{
		AColorWrapper::GetHighlightColor(framecolor);
	}
#endif
	//#135
	ALocalRect	realFrameRect = localFrameRect;
	realFrameRect.left		+= kFocusWidth;
	realFrameRect.right		-= kFocusWidth;
	realFrameRect.top		+= kFocusWidth;
	realFrameRect.bottom	-= kFocusWidth;
	//背景描画
	NVMC_PaintRect(realFrameRect,mBackgroundColor,mBackgroundTransparency);
	
	//==============フレーム描画==============
	
	switch(mFrameViewType)
	{
	  case kFrameViewType_Normal:
		{
			NVMC_FrameRect(realFrameRect,framecolor);
			break;
		}
		//edit boxの場合
	  case kFrameViewType_EditBox:
		{
			//コントロールがenableの場合のみフレーム描画
			if( NVI_GetEnabled() == true )
			{
				//Mac OS X 10.7に近いフレーム表示
				//フレーム
				NVMC_FrameRect(realFrameRect,AColorWrapper::GetColorByHTMLFormatColor("9e9e9e"));
				ALocalPoint	spt = {0}, ept = {0};
				//左シャドウ
				spt.x = realFrameRect.left+1;
				spt.y = realFrameRect.top +1;
				ept.x = realFrameRect.left+1;
				ept.y = realFrameRect.bottom-2;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("f7f7f7"));
				//右シャドウ
				spt.x = realFrameRect.right-2;
				spt.y = realFrameRect.top+1;
				ept.x = realFrameRect.right-2;
				ept.y = realFrameRect.bottom-2;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("f7f7f7"));
				//下シャドウ
				spt.x = realFrameRect.left+1;
				spt.y = realFrameRect.bottom-2;
				ept.x = realFrameRect.right-3;
				ept.y = realFrameRect.bottom-2;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("fdfdfd"));
				//上シャドウ
				ALocalRect	r = realFrameRect;
				r.left		+= 1;
				r.right		-= 1;
				r.top		+= 1;
				r.bottom	= r.top+2;
				NVMC_PaintRectWithVerticalGradient(r,AColorWrapper::GetColorByHTMLFormatColor("e0e0e0"),
													 AColorWrapper::GetColorByHTMLFormatColor("f8f8f8"),1.0,1.0);
				/*
				ALocalPoint	spt = {0}, ept = {0};
				//上線
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.top;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.top;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("c1c1c1"));
				//左線
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.top+1;
				ept.x = realFrameRect.left;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("c1c1c1"));
				//右線
				spt.x = realFrameRect.right-1;
				spt.y = realFrameRect.top +1;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("c1c1c1"));
				//下線
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.bottom-1;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("c1c1c1"));
				//上シャドウ
				spt.x = realFrameRect.left+1;
				spt.y = realFrameRect.top+1;
				ept.x = realFrameRect.right-2;
				ept.y = realFrameRect.top+1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("d4d4d4"),0.8);
				*/
				
			}
			break;
		}
		//filter boxの場合
	  case kFrameViewType_FilterBox:
		{
			//虫眼鏡アイコン
			{
				ALocalPoint	pt = {realFrameRect.left + 3, 6};
				NVMC_DrawImage(kImageID_iconSwSearch,pt);
			}
			//上の線描画
			{
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.top;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.top;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("bbc6d2"));//"4a91d5"));//"bbc6d2"));
			}
			//下の線描画
			{
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = realFrameRect.left;
				spt.y = realFrameRect.bottom-1;
				ept.x = realFrameRect.right-1;
				ept.y = realFrameRect.bottom-1;
				NVMC_DrawLine(spt,ept,AColorWrapper::GetColorByHTMLFormatColor("bbc6d2"));//"4a91d5"));//"bbc6d2"));
			}
			break;
		}
		//フレーム描画無し
	  case kFrameViewType_NoFrameDraw:
		{
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
void	AView_Frame::SPI_SetColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive,
		const AFloatNumber inTransparency )
{
	//
	mBackgroundColor = inBackColor;
	mFrameColor = inFrameColor;
	mFrameColorDeactive = inFrameColorDeactive;
	mBackgroundTransparency = inTransparency;
}

//win
/**
フレーム部分背景色設定
*/
void	AView_Frame::SPI_SetFrameBackgroundColor( const ABool inEnableFrameBackgroundColor, const AColor inColor )
{
	mEnableFrameBackgroundColor = inEnableFrameBackgroundColor;
	mFrameBackgroundColor = inColor;
}

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



