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

AView_KeyToolList
R0073
*/

#include "stdafx.h"

#include "AView_KeyToolList.h"
#include "AApp.h"

const ANumber	kLeftMargin = 8;
const ANumber	kRightMargin = 8;
const ANumber	kFileColumnWidth = 120;

const AFloatNumber	kFontSize = 9.0;

#pragma mark ===========================
#pragma mark [クラス]AView_KeyToolList
#pragma mark ===========================
//情報ウインドウのメインView

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_KeyToolList::AView_KeyToolList( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
	: AView(/*#199 inParent,inWindow*/inWindowID,inID), mLineHeight(12), mCurrentHoverCursorIndex(kIndex_Invalid)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 初期化はNVIDO_Init()へ移動
}

//デストラクタ
AView_KeyToolList::~AView_KeyToolList()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_KeyToolList::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_KeyToolList::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>


#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_KeyToolList::EVTDO_DoDraw()
{
	ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
	
	//=========================背景情報取得（このviewでは背景は描画しない）=========================
	//背景情報取得
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	AFloatNumber	backgroundAlpha = 1.0;
	switch(GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()))
	{
	  case kSubWindowLocationType_Popup:
		{
			backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha();
			//NVMC_EraseRect(frameRect);
			break;
		}
	  case kSubWindowLocationType_Floating:
		{
			//フローティングの指定透過率で背景消去
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			//NVMC_PaintRect(frameRect,backgroundColor,backgroundAlpha);
			break;
		}
	  default:
		{
			//NVMC_PaintRect(frameRect,backgroundColor);
			break;
		}
	}
	
	//=========================描画データ取得=========================
	//フォント取得
	//内容部分フォント
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//#1316
	AColor	letterColor = AColorWrapper::GetColorByHTMLFormatColor("000000");
	if( AApplication::GetApplication().NVI_IsDarkMode() == true )
	{
		letterColor = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
	}
	//
	ANumber	viewwidth = NVI_GetViewWidth();
	NVMC_SetDefaultTextProperty(fontname,fontsize,letterColor,kTextStyle_Normal,true);//#1316
	
	//=========================各項目毎ループ=========================
	//
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//行rect取得、update範囲かどうかを判定
		AImageRect	imagerect;
		imagerect.left		= 0;
		imagerect.top		= i*mLineHeight;
		imagerect.right		= viewwidth;
		imagerect.bottom	= (i+1)*mLineHeight;
		ALocalRect	localrect;
		NVM_GetLocalRectFromImageRect(imagerect,localrect);
		if( NVMC_IsRectInDrawUpdateRegion(localrect) == false )   continue;
		
		//ホバー表示
		if( i == mCurrentHoverCursorIndex )
		{
			AColor	color;
			AColorWrapper::GetHighlightColor(color);
			NVMC_PaintRect(localrect,color,0.4);
		}
		const AText&	infotext = mInfoTextArray.GetTextConst(i);
		ATextStyle	textstyle = kTextStyle_Normal;
		if( mIsUserToolArray.Get(i) == true )
		{
			textstyle = kTextStyle_Bold;
		}
		/*
		//描画データ作成
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(infotext,4,0,true,true,false,0,infotext.GetItemCount());
		*/
		//テキスト描画rect生成
		ALocalRect	drawRect = localrect;
		drawRect.left	= localrect.left + kLeftMargin;
		drawRect.right	= localrect.right - kRightMargin;;
		ALocalRect	textRect = drawRect;
		//テキスト描画
		//NVMC_DrawText(textRect,drawRect,textDrawData);
		NVMC_DrawText(textRect,infotext,letterColor,textstyle);//#1316
	}
}

//マウスボタン押下時のコールバック(AView用)
ABool	AView_KeyToolList::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	AIndex	index = imagept.y / mLineHeight;
	if( index >= 0 && index < mInfoTextArray.GetItemCount() )
	{
		//TextWindowにkeytool入力を伝える
		GetApp().SPI_GetTextWindowByID(mCurrentTextWindowID).SPI_KeyTool(mCurrentTextViewControlID,mInfoTextArray.GetTextConst(index).Get(0));
	}
	return false;
}

//マウスホイール
void	AView_KeyToolList::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
										const ALocalPoint inLocalPoint )//win 080706
{
	//スクロール率を設定から取得する
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true 
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win 080702
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd);
	}
	else
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent);
	}
	//スクロール実行
	NVI_Scroll(0,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);
}

ABool	AView_KeyToolList::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

//カーソル
ABool	AView_KeyToolList::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	AIndex	index = imagept.y / mLineHeight;
	if( index >= 0 && index < mInfoTextArray.GetItemCount() )
	{
		mCurrentHoverCursorIndex = index;
		NVI_Refresh();
		return true;
	}
	ClearHoverCursor();
	return true;
}

void	AView_KeyToolList::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

void	AView_KeyToolList::ClearHoverCursor()
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		mCurrentHoverCursorIndex = kIndex_Invalid;
		NVI_Refresh();
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

//
void	AView_KeyToolList::SPI_SetInfo( const AWindowID inTextWindowID, const AControlID inTextViewControlID,
									   const ATextArray& inInfoTextArray, const ABoolArray& inIsUserToolArray )
{
	mCurrentTextWindowID = inTextWindowID;
	mCurrentTextViewControlID = inTextViewControlID;
	mInfoTextArray.DeleteAll();
	mIsUserToolArray.DeleteAll();
	for( AIndex i = 0; i < inInfoTextArray.GetItemCount(); i++ )
	{
		AText	infoText;
		infoText.SetText(inInfoTextArray.GetTextConst(i));
		infoText.ReplaceChar(kUChar_CR,kUChar_Space);
		mInfoTextArray.Add(infoText);
		mIsUserToolArray.Add(inIsUserToolArray.Get(i));
	}
	CalcDrawData();
	//ポップアップ表示時は、ウインドウサイズをviewのimage sizeに合わせて更新
	if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) == kSubWindowLocationType_Popup )
	{
		ARect	windowBounds = {0};
		NVM_GetWindow().NVI_GetWindowBounds(windowBounds);
		windowBounds.right = windowBounds.left + NVI_GetImageWidth() + 15;
		windowBounds.bottom = windowBounds.top + NVI_GetImageHeight() + 15;
		NVM_GetWindow().NVI_SetWindowBounds(windowBounds);
	}
	//
	NVM_GetWindow().NVI_UpdateProperty();
	NVI_Refresh();
}

void	AView_KeyToolList::AdjustScroll()
{
	/*ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imagerect;
	NVM_GetImageRectFromLocalRect(frameRect,imagerect);
	AIndex	startIndex = imagerect.top/mLineHeight;
	AIndex	endIndex = imagerect.bottom/mLineHeight;
	if( mIndex <= startIndex || endIndex <= mIndex )
	{
		AImagePoint	pt = {0,0};
		pt.y = mIndex*mLineHeight - (frameRect.bottom - frameRect.top)/2;
		if( pt.y < 0 )   pt.y = 0;
		NVI_ScrollTo(pt);
	}*/
}

//各種描画データ（imagesize等）の設定
void	AView_KeyToolList::CalcDrawData()
{
	//フォント、lineheight等の設定
	//ANumber	viewwidth = NVI_GetViewWidth();
	AText	fixwidthfontname;
	AFontWrapper::GetAppDefaultFontName(fixwidthfontname);//#375
	NVMC_SetDefaultTextProperty(fixwidthfontname,kFontSize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontheight, fontascent;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	fontheight += 4;
	mLineHeight = fontheight;
	//イメージサイズの幅は、リストのテキストのうちの最大の幅にする
	ANumber	maxWidth = 0;
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		AText	infoText;
		ANumber	w = NVMC_GetDrawTextWidth(mInfoTextArray.GetTextConst(i));
		if( w > maxWidth )
		{
			maxWidth = w;
		}
	}
	//ImageSize設定
	NVM_SetImageSize(maxWidth+25,mInfoTextArray.GetItemCount()*fontheight);
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(0,fontheight,0,rect.bottom-rect.top-fontheight);
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

void	AView_KeyToolList::SPI_UpdateDrawProperty()
{
	CalcDrawData();
	NVI_Refresh();
}



