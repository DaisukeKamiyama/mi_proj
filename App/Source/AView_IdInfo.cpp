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

AView_IdInfo

*/

#include "stdafx.h"

#include "AView_IdInfo.h"
#include "AApp.h"

//ポップアップid infoのhideタイマー値
const ANumber	kTimerForHidePopupIdInfo = 70;//#1354 80→70
const ANumber	kTimerForHidePopupIdInfo_AfterArgIndexChanged = 150;//#1354 160→150

//角丸の半径 #1079
const AFloatNumber	kRoundedRectRad = 3.0f;

#pragma mark ===========================
#pragma mark [クラス]AView_IdInfo
#pragma mark ===========================
//情報ウインドウのメインView

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_IdInfo::AView_IdInfo( /* #199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
	: AView(/*#199 inParent,inWindow*/inWindowID,inID), mCurrentModeIndex(kIndex_Invalid), mArgIndex(kIndex_Invalid), mCurrentHoverCursorIndex(kIndex_Invalid)
	,mFixDisplay(false)//#238
//#725	,mTransparency(1.0)//#291
	,mContextMenuItemID(kIndex_Invalid)//#442
,mCurrentSelectionIndex(kIndex_Invalid)//#853
,mMouseTrackStartLocalPoint(kLocalPoint_00), mMouseDownResultIsDrag(false)
,mHideCounter(0),mAlpha(1.0), mDisableAutoHide(false)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 初期化はNVIDO_Init()へ移動
	
	//データ全削除
	SPI_DeleteAllIdInfo();
}

//デストラクタ
AView_IdInfo::~AView_IdInfo()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_IdInfo::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_IdInfo::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/*#853
#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kFileNameFontSize = 9.0;
const AFloatNumber	kCategoryNameFontSize = 8.0;
const AFloatNumber	kInfoTextFontSize = 8.5;
const AFloatNumber	kCommentTextFontSize = 8;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AFloatNumber	kFileNameFontSize = 7.8;
const AFloatNumber	kCategoryNameFontSize = 7.8;
const AFloatNumber	kInfoTextFontSize = 8.0;
const AFloatNumber	kCommentTextFontSize = 8.0;
#endif
*/
const ANumber	kHeaderLeftMargin = 6;
const ANumber	kHeaderRightMargin = 6;
const ANumber	kHeaderTopMargin = 2;
const ANumber	kHeaderBottomMargin = 2;
const ANumber	kInfoTextLeftMargin = 8;
const ANumber	kInfoTextLeftMargin2 = 9;
const ANumber	kInfoTextRightMargin = 3;
const ANumber	kInfoTextTopMargin = 2;
const ANumber	kInfoTextBottomMargin = 1;
const ANumber	kCommentTextLeftMargin = 8;
const ANumber	kCommentTextRightMargin = 3;
const ANumber	kCommentTextTopMargin = 2;
const ANumber	kCommentTextBottomMargin = 1;
//const ANumber	kFilenameRightMargin = 16;

//#643
const ANumber	kItemBoxLeftMargin = 5;
const ANumber	kItemBoxRightMargin = 5;
const ANumber	kItemBoxTopMargin = 2;
const ANumber	kItemBoxBottomMargin = 1;

const ANumber	kVScrollUnit = 8;

//描画要求時のコールバック(AView用)
void	AView_IdInfo::EVTDO_DoDraw()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	
	//=========================描画データ取得=========================
	//フォント取得
	//内容部分フォント
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//ヘッダ部分フォント
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	
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
	
	//==================色取得==================
	
	//描画色設定
	AColor	letterColor = kColor_Black;
	AColor	dropShadowColor = kColor_White;
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	NVMC_SetDropShadowColor(dropShadowColor);
	
	//=========================各項目毎ループ=========================
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//項目rect取得
		AImageRect	itemImageRect = {0};
		GetItemBoxImageRect(i,itemImageRect);//#643
		ALocalRect	itemLocalRect = {0};
		NVM_GetLocalRectFromImageRect(itemImageRect,itemLocalRect);
		
		//updateRect内にない場合はcontinue
		if( NVMC_IsRectInDrawUpdateRegion(itemLocalRect) == false )   continue;
		
		//キーワード取得
		AText	keywordText;
		mKeywordTextArray.Get(i,keywordText);
		
		//最後の項目以外は、隙間を背景色（α:0.2）で描画
		if( i != mInfoTextArray.GetItemCount() -1 )
		{
			ALocalRect	gapLocalRect = {0};
			gapLocalRect.left		= itemLocalRect.left + 5;
			gapLocalRect.top		= itemLocalRect.bottom;
			gapLocalRect.right		= itemLocalRect.right - 5;
			gapLocalRect.bottom		= itemLocalRect.bottom + kItemBoxTopMargin + kItemBoxBottomMargin;
			NVMC_PaintRect(gapLocalRect,/*kColor_White*/boxBaseColor2,0.2);
		}
		/*test
		if( mCurrentHoverCursorIndex == i )
		{
			{
				ALocalRect	shadowLocalRect = itemLocalRect;
				shadowLocalRect.bottom		= shadowLocalRect.top;
				shadowLocalRect.left		-= 5;
				shadowLocalRect.top			-= 5;
				shadowLocalRect.right		+= 5;
				NVMC_PaintRoundedRect(shadowLocalRect,kColor_White,kColor_White,kGradientType_Vertical,0.0,0.8,
									  5,true,true,false,false);
			}
			{
				ALocalRect	shadowLocalRect = itemLocalRect;
				shadowLocalRect.top			= shadowLocalRect.bottom;
				shadowLocalRect.bottom		+= 5;
				shadowLocalRect.left		-= 5;
				shadowLocalRect.right		+= 5;
				NVMC_PaintRoundedRect(shadowLocalRect,kColor_White,kColor_White,kGradientType_Vertical,0.8,0.0,
									  5,false,false,true,true);
			}
			{
				ALocalRect	shadowLocalRect = itemLocalRect;
				shadowLocalRect.right		= shadowLocalRect.left;
				shadowLocalRect.left		-= 5;
				NVMC_PaintRoundedRect(shadowLocalRect,kColor_White,kColor_White,kGradientType_Horizontal,0.0,0.8,
									  5,false,false,false,false);
			}
			{
				ALocalRect	shadowLocalRect = itemLocalRect;
				shadowLocalRect.left		= shadowLocalRect.right;
				shadowLocalRect.right		+= 5;
				NVMC_PaintRoundedRect(shadowLocalRect,kColor_White,kColor_White,kGradientType_Horizontal,0.8,0.0,
									  5,false,false,false,false);
			}
		}
		*/
		//ボックス描画#643
		NVMC_PaintRoundedRect(itemLocalRect,
							  //kColor_White,kColor_Gray95Percent
							  boxBaseColor1,boxBaseColor3,
							  kGradientType_Vertical,backgroundAlpha*mAlpha,backgroundAlpha*mAlpha,
							  kRoundedRectRad,true,true,true,true);
		NVMC_FrameRoundedRect(itemLocalRect,
							  //kColor_Gray20Percent
							  kColor_Gray50Percent,
							  mAlpha,kRoundedRectRad,true,true,true,true);//,true,true,true,true,1.0);
		//ホバー描画
		if( mCurrentHoverCursorIndex == i )
		{
			/*
			AColor	selcolor;
			AColorWrapper::GetHighlightColor(selcolor);
			NVMC_PaintRoundedRect(itemLocalRect,selcolor,selcolor,kGradientType_Vertical,0.1,0.1,5,true,true,true,true);
			NVMC_FrameRoundedRect(itemLocalRect,selcolor,1.0,5,true,true,true,true);
			*/
			//ホバー色でrounded rectで囲む
			AColor	hoverColor = GetApp().SPI_GetSubWindowRoundedRectBoxHoverColor();
			NVMC_FrameRoundedRect(itemLocalRect,hoverColor,0.3,kRoundedRectRad,true,true,true,true,true,true,true,true,1.0);
		}
		//選択描画
		if( mCurrentSelectionIndex == i )
		{
			/*
			AColor	selcolor = kColor_Blue;
			AColorWrapper::GetHighlightColor(selcolor);
			NVMC_PaintRoundedRect(itemLocalRect,selcolor,selcolor,kGradientType_Vertical,0.3,0.3,5,true,true,true,true);
			NVMC_FrameRoundedRect(itemLocalRect,selcolor,1.0,5,true,true,true,true);
			*/
			//選択色でrounded rectで囲む
			AColor	boxFrameColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
			NVMC_FrameRoundedRect(itemLocalRect,boxFrameColor,1.0,kRoundedRectRad,true,true,true,true,true,true,true,true,1.0);
		}
		
		//=========================ヘッダ部分描画=========================
		
		//モード設定更新とスレッド実行とのすれ違いの場合にカテゴリindexがオーバーする可能性もあるので、念のため。
		AIndex	categoryIndex = mCategoryIndexArray.Get(i);
		if( categoryIndex < 0 || categoryIndex >= GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryCount() )
		{
			categoryIndex = 0;
		}
		
		//ヘッダ色取得
		AColor	headercolor;
		AText	filename;
		switch(mScopeTypeArray.Get(i))
		{
		  case kScopeType_Local:
			{
				filename.SetLocalizedText("Scope_Local");
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryLocalColor(categoryIndex,headercolor);
				break;
			}
		  case kScopeType_Global:
			{
				mFilePathArray.GetTextConst(i).GetFilename(filename);
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryImportColor(categoryIndex,headercolor);
				break;
			}
		  case kScopeType_Import:
			{
				mFilePathArray.GetTextConst(i).GetFilename(filename);
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryImportColor(categoryIndex,headercolor);
				break;
			}
		  default://#147
			{
				filename.SetText(keywordText);
				//キーワードにカラースロット指定があればその色を取得
				AIndex	colorSlotIndex = mColorSlotIndexArray.Get(i);
				if( colorSlotIndex != kIndex_Invalid )
				{
					ATextStyle	style = kTextStyle_Normal;
					GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetColorSlotData(colorSlotIndex,headercolor,style);
				}
				//カラースロット指定がなければカテゴリー色を取得
				else
				{
					GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryColor(categoryIndex,headercolor);
				}
				break;
			}
		}
		//ヘッダrect取得
		AImageRect	headerImageRect = itemImageRect;//#643
		headerImageRect.bottom = itemImageRect.top + mDrawData_HeaderHeight.Get(i);//#643
		ALocalRect	headerLocalRect = {0};
		NVM_GetLocalRectFromImageRect(headerImageRect,headerLocalRect);
		//ヘッダrect描画
		AColor	headercolor_start = AColorWrapper::ChangeHSV(headercolor,0,0.8,1.0);//#643
		AColor	headercolor_end = AColorWrapper::ChangeHSV(headercolor,0,1.2,1.0);//#643
		NVMC_PaintRoundedRect(headerLocalRect,headercolor_start,headercolor_end,kGradientType_Vertical,0.15*mAlpha,0.15*mAlpha,
							  kRoundedRectRad,true,true,true,true);
		
		//=========================カテゴリ名・項目番号テキスト描画=========================
		
		//ファイル名フォント取得
		NVMC_SetDefaultTextProperty(labelfontname,fontsize-0.5,/*kColor_Black*/letterColor,kTextStyle_Normal,true,mAlpha);
		ANumber	labelfontheight = 9 ,labelfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(labelfontheight,labelfontascent);
		//カテゴリー名フォント取得
		NVMC_SetDefaultTextProperty(labelfontname,fontsize-1.0,/*kColor_Black*/letterColor,kTextStyle_Normal,true,mAlpha);//#1354 0.5→1.0
		ANumber	categoryfontheight = 9, categoryfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(categoryfontheight,categoryfontascent);
		//カテゴリテキスト取得
		AText	categoryname;
		GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryName(categoryIndex,categoryname);
		//項目番号描画
		categoryname.AddFormattedCstring("  (%d/%d)",i+1,mInfoTextArray.GetItemCount());
		//カテゴリ名描画rect取得
		AImageRect	categorynameImageRect = itemImageRect;//#643
		categorynameImageRect.right		= itemImageRect.right - kHeaderRightMargin;
		categorynameImageRect.top 		= itemImageRect.top + labelfontascent - categoryfontascent +kHeaderTopMargin;//#643
		categorynameImageRect.bottom 	= itemImageRect.top + labelfontheight +kHeaderTopMargin;//#643
		ALocalRect	categorynameLocalRect = {0};
		NVM_GetLocalRectFromImageRect(categorynameImageRect,categorynameLocalRect);
		//カテゴリ名描画
		NVMC_DrawText(categorynameLocalRect,categoryname,/*kColor_Black*/letterColor,kTextStyle_Normal,kJustification_Right);
		
		//=========================ファイル名テキスト描画=========================
		
		//ファイル名描画rect取得
		AImageRect	filenameImageRect = itemImageRect;//#643
		filenameImageRect.left		= itemImageRect.left + kHeaderLeftMargin;//#643
		filenameImageRect.bottom 	= itemImageRect.top + labelfontheight +kHeaderTopMargin;//#643
		filenameImageRect.top		+= kHeaderTopMargin;
		filenameImageRect.right		-= NVMC_GetDrawTextWidth(categoryname) + 4;//#1354 32→4
		ALocalRect	filenameLocalRect = {0};
		NVM_GetLocalRectFromImageRect(filenameImageRect,filenameLocalRect);
		//filenameが空の場合はキーワードを格納
		if( filename.GetItemCount() == 0 )
		{
			filename.SetText(mKeywordTextArray.GetTextConst(i));
		}
		//右側の「カテゴリ名」＋「項目番号」部分を除いた領域内でファイル名を描画（描画しきれない場合は...で省略）
		AText	ellipsisFilename;
		NVI_GetEllipsisTextWithFixedLastCharacters(filename,filenameLocalRect.right-filenameLocalRect.left,5,ellipsisFilename);
		//ファイル名テキスト描画
		if( filename.GetItemCount() > 0 )
		{
			NVMC_DrawText(filenameLocalRect,ellipsisFilename,/*kColor_Black*/letterColor,kTextStyle_Bold/*|kTextStyle_DropShadow*/,kJustification_Left);
		}
		
		//クラス名表示 (◇クラス名)
		AText	parentKeyword;
		mParentKeywordArray.Get(i,parentKeyword);
		if( parentKeyword.GetItemCount() > 0 )
		{
			ALocalRect	classNameLocalRect = filenameLocalRect;
			classNameLocalRect.left += NVMC_GetDrawTextWidth(ellipsisFilename) +16;
			AText	classname;
			classname.SetCstring("(");
			AText	t;
			if( mRequestedParentKeyword.Find(parentKeyword) != kIndex_Invalid )
			{
				//一致表示（◆）
				t.SetLocalizedText("Letter_ParentKeyword_Matched");
			}
			else
			{
				//不一致表示（◇）
				t.SetLocalizedText("Letter_ParentKeyword");
			}
			classname.AddText(t);
			classname.AddText(parentKeyword);
			classname.AddCstring(")");
			AText	ellipsisClassname;
			NVI_GetEllipsisTextWithFixedLastCharacters(classname,classNameLocalRect.right-classNameLocalRect.left,5,ellipsisClassname);
			NVMC_DrawText(classNameLocalRect,ellipsisClassname,/*kColor_Black*/letterColor,kTextStyle_Bold/*|kTextStyle_DropShadow*/,kJustification_Left);
		}
		
		
		//=========================コメント（@note）部分描画=========================
		
		//CommentText
		if( mDrawData_CommentTextLineStart.GetObject(i).GetItemCount() > 0 )
		{
			//フォント設定
			NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Red,kTextStyle_Normal,true,mAlpha);
			ANumber	commenttextfontheight = 9 ,commenttextfontascent = 7;
			NVMC_GetMetricsForDefaultTextProperty(commenttextfontheight,commenttextfontascent);
			//各行描画
			for( AIndex lineIndex = 0; lineIndex < mDrawData_CommentTextLineStart.GetObjectConst(i).GetItemCount(); lineIndex++ )
			{
				//行データ取得
				AIndex	start 	= mDrawData_CommentTextLineStart .GetObjectConst(i).Get(lineIndex);
				AItemCount	len = mDrawData_CommentTextLineLength.GetObjectConst(i).Get(lineIndex);
				AText	text;
				mCommentTextArray.GetTextConst(i).GetText(start,len,text);
				//textdata設定
				CTextDrawData	textDrawData(false);
				textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
				//描画rect取得
				AImageRect	lineImageRect = itemImageRect;//#643
				lineImageRect.left		= itemImageRect.left + kCommentTextLeftMargin;//#643
				lineImageRect.top 		= itemImageRect.top + mDrawData_HeaderHeight.Get(i) 
						+ mDrawData_InfoTextHeight.Get(i)
						+ lineIndex*commenttextfontheight + kInfoTextTopMargin;//#643
				lineImageRect.bottom	= lineImageRect.top + commenttextfontheight + kInfoTextTopMargin;//#643
				ALocalRect	lineLocalRect = {0};
				NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
				NVMC_DrawText(lineLocalRect,text,kColor_Red,kTextStyle_Bold);//#835 test textDrawData);
			}
		}
		
		//=========================InfoText部分（本体部分）描画=========================
		
		//カテゴリ色を暗くして彩度を上げた色を取得（ポップアップウインドウではない場合のみ）
		AColor	keywordColor = headercolor;
		if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) != kSubWindowLocationType_Popup )
		{
			keywordColor = AColorWrapper::ChangeHSV(headercolor,0,3.0,0.7);
		}
		
		//InfoTextフォント設定
		NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,/*kColor_Black*/letterColor,kTextStyle_Normal,true,mAlpha);
		ANumber	infotextfontheight = 9, infotextfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
		//
		ANumber	lineleftmargin = kInfoTextLeftMargin;
		ABool	argmode = false;
		ABool	argstartdone = false;
		AIndex	currentargindex = kIndex_Invalid;
		//各行ごとループ
		for( AIndex lineIndex = 0; lineIndex < mDrawData_InfoTextLineStart.GetObjectConst(i).GetItemCount(); lineIndex++ )
		{
			//行データ取得
			AIndex	start 	= mDrawData_InfoTextLineStart .GetObjectConst(i).Get(lineIndex);
			AItemCount	len = mDrawData_InfoTextLineLength.GetObjectConst(i).Get(lineIndex);
			AText	text;
			mInfoTextArray.GetTextConst(i).GetText(start,len,text);
			//text draw data取得
			CTextDrawData	textDrawData(false);
			textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
			//行の最初のattr設定（前行からアンダーライン継続ならアンダーラインにする）
			//#853if( currentargindex != mArgIndex || mArgIndex == kIndex_Invalid )
			{
				textDrawData.attrPos.Add(0);
				textDrawData.attrColor.Add(letterColor);//kColor_Black);
				textDrawData.attrStyle.Add(kTextStyle_Normal);
			}
			/*#853 対象引数ハイライト化
			else
			{
				textDrawData.attrPos.Add(0);
				textDrawData.attrColor.Add(letterColor);//kColor_Black);
				textDrawData.attrStyle.Add(kTextStyle_Underline);
			}
			*/
			//前行からハイライト継続なら、最初からハイライト
			AIndex	argStartX = kIndex_Invalid, argEndX = kIndex_Invalid;
			if( currentargindex == mArgIndex && mArgIndex != kIndex_Invalid )
			{
				//ハイライト開始位置設定
				argStartX = 0;
			}
			//
			AItemCount	textlen = text.GetItemCount();
			for( AIndex pos = 0; pos < textlen; )
			{
				if( text.SkipTabSpace(pos,textlen) == false )   break;
				AText	token;
				AIndex	tokenspos = pos;
				text.GetToken(pos,token);
				//キーワード部分をカテゴリ色で描画
				if( token.Compare(keywordText) == true )
				{
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos));
					textDrawData.attrColor.Add(keywordColor);//categoryColorDraken);//headercolor);
					textDrawData.attrStyle.Add(kTextStyle_Bold/*|kTextStyle_DropShadow*/);
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(pos));
					textDrawData.attrColor.Add(letterColor);//kColor_Black);
					textDrawData.attrStyle.Add(kTextStyle_Normal);
				}
				//引数のアンダーライン描画
				else if( token.GetItemCount() == 1 )
				{
					AUChar	ch = token.Get(0);
					if( GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetSyntaxDefinition().IsIdInfoStartChar(ch) == true && argstartdone == false )
					{
						argmode = true;
						argstartdone = true;
						currentargindex = 0;
						if( mArgIndex == currentargindex )
						{
							/*#853 対象引数ハイライト化
							textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(pos));
							textDrawData.attrColor.Add(letterColor);//kColor_Black);
							textDrawData.attrStyle.Add(kTextStyle_Underline);
							*/
							//ハイライト開始位置設定
							argStartX = pos;
						}
					}
					if( GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetSyntaxDefinition().IsIdInfoEndChar(ch) == true && argmode == true )
					{
					
						argmode = false;
						if( mArgIndex == currentargindex )
						{
							/*#853 対象引数ハイライト化
							textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos));
							textDrawData.attrColor.Add(letterColor);//kColor_Black);
							textDrawData.attrStyle.Add(kTextStyle_Normal);
							*/
							//ハイライト終了位置設定
							argEndX = tokenspos;
						}
						currentargindex = kIndex_Invalid;
					}
					if( GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetSyntaxDefinition().IsIdInfoDelimiterChar(ch) == true && argmode == true )
					{
						if( mArgIndex == currentargindex )
						{
							/*#853
							textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos));
							textDrawData.attrColor.Add(letterColor);//kColor_Black);
							textDrawData.attrStyle.Add(kTextStyle_Normal);
							*/
							//ハイライト終了位置設定
							argEndX = tokenspos;
						}
						currentargindex++;
						if( mArgIndex == currentargindex )
						{
							/*#853
							textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(pos));
							textDrawData.attrColor.Add(letterColor);//kColor_Black);
							textDrawData.attrStyle.Add(kTextStyle_Underline);
							*/
							//ハイライト開始位置設定
							argStartX = pos;
						}
					}
				}
			}
			//引数ハイライト
			if( argStartX != kIndex_Invalid )
			{
				//終了位置未設定なら行最後まで
				if( argEndX == kIndex_Invalid )
				{
					argEndX = textlen;
				}
				//描画rect取得
				AImageRect	imageRect = itemImageRect;
				imageRect.left			= itemImageRect.left + lineleftmargin + NVMC_GetImageXByTextPosition(textDrawData,argStartX);
				imageRect.right			= itemImageRect.left + lineleftmargin + NVMC_GetImageXByTextPosition(textDrawData,argEndX);
				imageRect.top 			= itemImageRect.top + mDrawData_HeaderHeight.Get(i) +
										lineIndex*infotextfontheight + kInfoTextTopMargin;//#643
				imageRect.bottom		= imageRect.top + infotextfontheight + kInfoTextTopMargin;//#643
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				//描画
				NVMC_PaintRect(localRect,kColor_Yellow,0.3);
			}
			//行描画
			AImageRect	lineImageRect = itemImageRect;//#643
			lineImageRect.left		= itemImageRect.left + lineleftmargin;//#643
			lineImageRect.top 		= itemImageRect.top + mDrawData_HeaderHeight.Get(i) +
									lineIndex*infotextfontheight + kInfoTextTopMargin;//#643
			lineImageRect.bottom	= lineImageRect.top + infotextfontheight + kInfoTextTopMargin;//#643
			ALocalRect	lineLocalRect = {0};
			NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
			NVMC_DrawText(lineLocalRect,textDrawData);
			//
			//#147
			if( mScopeTypeArray.Get(i) != kScopeType_ModeKeyword )
			{
				lineleftmargin = kInfoTextLeftMargin2;
			}
		}
	}
}

//#643
/**
各ItemBoxのimagerect取得
*/
void	AView_IdInfo::GetItemBoxImageRect( const AIndex inIndex, AImageRect& outImageRect ) const
{
	outImageRect.left		= kItemBoxLeftMargin;
	outImageRect.top		= mDrawData_Y.Get(inIndex) + kItemBoxTopMargin;
	outImageRect.right		= NVI_GetViewWidth() - kItemBoxRightMargin;
	outImageRect.bottom		= outImageRect.top + mDrawData_Height.Get(inIndex);
}

//DrawData計算
void	AView_IdInfo::CalcDrawData()
{
	//
	mDrawData_Y.DeleteAll();
	mDrawData_Height.DeleteAll();
	mDrawData_HeaderHeight.DeleteAll();
	mDrawData_InfoTextHeight.DeleteAll();
	mDrawData_InfoTextLineStart.DeleteAll();
	mDrawData_InfoTextLineLength.DeleteAll();
	mDrawData_CommentTextHeight.DeleteAll();
	mDrawData_CommentTextLineStart.DeleteAll();
	mDrawData_CommentTextLineLength.DeleteAll();
	
	//=========================描画データ取得=========================
	//フォント取得
	//内容部分フォント
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//ヘッダ部分フォント
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	//
	ANumber	viewwidth = NVI_GetViewWidth();
	
	//=========================項目毎ループ=========================
	ANumber	y = 0;
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		mDrawData_Y.Add(y);
		mDrawData_InfoTextLineStart .AddNewObject();
		mDrawData_InfoTextLineLength.AddNewObject();
		mDrawData_CommentTextLineStart .AddNewObject();
		mDrawData_CommentTextLineLength.AddNewObject();
		
		//=========================ヘッダ部分計算=========================
		NVMC_SetDefaultTextProperty(labelfontname,fontsize,kColor_Black,kTextStyle_Bold,true);
		ANumber	headerfontheight = 9, headerfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(headerfontheight,headerfontascent);
		mDrawData_HeaderHeight.Add(kHeaderTopMargin+headerfontheight+kHeaderBottomMargin);
		
		//=========================InfoText部分計算=========================
		//InfoText取得
		AText	infoText;
		mInfoTextArray.Get(i,infoText);
		//InfoText行折り返し計算
		CWindowImp::CalcLineBreaks(infoText,fontname,fontsize-0.5,true,
								   viewwidth-kItemBoxLeftMargin-kItemBoxRightMargin -kInfoTextLeftMargin2 - kInfoTextRightMargin,
								   false,//#1113
								   mDrawData_InfoTextLineStart.GetObject(i),mDrawData_InfoTextLineLength.GetObject(i));
		//フォント高さ取得
		NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Black,kTextStyle_Normal,true);
		ANumber	infotextfontheight = 9, infotextfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
		//InfoText全体の高さ計算
		mDrawData_InfoTextHeight.Add(kInfoTextTopMargin+
					infotextfontheight*mDrawData_InfoTextLineStart.GetObject(i).GetItemCount() +
					kInfoTextBottomMargin);
		
		//=========================comment text分計算=========================
		AText	commentText;
		mCommentTextArray.Get(i,commentText);
		if( commentText.GetItemCount() > 0 )//#844 && GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetData_Bool(AModePrefDB::kIdInfoWindowDisplayComment) == true )
		{
			//フォント高さ取得
			NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Black,kTextStyle_Normal,true);
			ANumber	commenttextfontheight = 9, commenttextfontascent = 7;
			NVMC_GetMetricsForDefaultTextProperty(commenttextfontheight,commenttextfontascent);
			//行折り返し計算
			CWindowImp::CalcLineBreaks(commentText,fontname,fontsize-0.5,true,
									   viewwidth-kItemBoxLeftMargin-kItemBoxRightMargin -kCommentTextLeftMargin - kCommentTextRightMargin,
									   false,//#1113
									   mDrawData_CommentTextLineStart.GetObject(i),mDrawData_CommentTextLineLength.GetObject(i));
			//comment text全体の高さ計算
			mDrawData_CommentTextHeight.Add(kCommentTextTopMargin+commenttextfontheight*mDrawData_CommentTextLineStart.GetObject(i).GetItemCount()+kCommentTextBottomMargin);
		}
		else
		{
			//comment text無しなら、comment textの高さは0を設定
			mDrawData_CommentTextHeight.Add(0);
		}
		
		//項目全体の高さ設定
		mDrawData_Height.Add(mDrawData_HeaderHeight.Get(i) + mDrawData_InfoTextHeight.Get(i) + mDrawData_CommentTextHeight.Get(i) + 2);
		//次の項目のy位置を計算
		y += mDrawData_Height.Get(i);
		y += kItemBoxTopMargin + kItemBoxBottomMargin;//#643
	}
	//ImageSize設定
	NVM_SetImageSize(viewwidth,y);
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(0,kVScrollUnit,0,rect.bottom-rect.top-kVScrollUnit);
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

//マウスボタン押下時のコールバック(AView用)
ABool	AView_IdInfo::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	/*
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		if( imagept.y > mDrawData_Y.Get(i) && imagept.y < mDrawData_Y.Get(i) + mDrawData_Height.Get(i) )
		{
			SPI_OpenItem(i,inModifierKeys);//#853
			break;
		}
	}
	return false;
	*/
	//tracking開始時マウス位置記憶
	mMouseTrackStartLocalPoint = inLocalPoint;
	mMouseDownResultIsDrag = false;
	
	//Mouse tracking開始
	NVM_SetMouseTrackingMode(true);
	return false;
}

/**
マウスボタン押下中TrackingのOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_IdInfo::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	/*IdInfoをドラッグしたら別ウインドウで開くようにしようとしたが、使い勝手があまりよく無さそうなのでやめた
	IdInfoを別ビューにドラッグしたらそのビューで開く機能はありかもしれないので、そちらは別途検討。
	//tracking開始時と違うマウス位置になったらdrag開始
	if( inLocalPoint.x != mMouseTrackStartLocalPoint.x || inLocalPoint.y != mMouseTrackStartLocalPoint.y )
	{
		//ドラッグ開始
		mMouseDownResultIsDrag = true;
		StartDrag(mMouseTrackStartLocalPoint,inModifierKeys);
		//TrackingMode解除
		NVM_SetMouseTrackingMode(false);
	}
	NVI_Refresh();
	*/
}

/**
マウスボタン押下中Tracking終了時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_IdInfo::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mMouseDownResultIsDrag == false )
	{
		//Drag開始していない場合は、項目を開く
		OpenItem(inLocalPoint,inModifierKeys);
		
		//Tracking解除
		NVM_SetMouseTrackingMode(false);
		
		//ポップアップウインドウの場合はid infoウインドウをhide
		if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) == kSubWindowLocationType_Popup )
		{
			NVM_GetWindow().NVI_Hide();
		}
	}
}

/**
Drag開始
*/
/*IdInfoをドラッグしたら別ウインドウで開くようにしようとしたが、使い勝手があまりよく無さそうなのでやめた
void	AView_IdInfo::StartDrag( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AIndex	itemIndex = FindItem(inLocalPoint);
	if( itemIndex == kIndex_Invalid )   return;
	//フレーム取得
	ALocalRect	frame = {0};
	NVM_GetLocalViewRect(frame);
	//Scrap設定（表示Indexを渡す）
	AArray<AScrapType>	scrapType;
	ATextArray	data;
	scrapType.Add(kScrapType_IdInfoItem);
	//Dragデータ生成
	AText	filepath;
	mFileArray.GetObjectConst(itemIndex).GetPath(filepath);
	AText	text;
	text.SetText(filepath);
	text.Add(kUChar_Tab);
	text.AddNumber(mStartIndexArray.Get(itemIndex));
	text.Add(kUChar_Tab);
	text.AddNumber(mEndIndexArray.Get(itemIndex));
	data.Add(text);
	//Dragイメージ範囲取得
	AImageRect	boxImageRect = {0};
	GetItemBoxImageRect(itemIndex,boxImageRect);
	ALocalRect	boxLocalRect = {0};
	NVM_GetLocalRectFromImageRect(boxImageRect,boxLocalRect);
	AWindowRect	boxWindowRect = {0};
	NVM_GetWindow().NVI_GetWindowRectFromControlLocalRect(NVI_GetControlID(),boxLocalRect,boxWindowRect);
	//Drag実行
	if( MVMC_DragTextWithWindowImage(inLocalPoint,scrapType,data,boxWindowRect,
					boxWindowRect.right-boxWindowRect.left,boxWindowRect.bottom-boxWindowRect.top) == false )
	{
		//Dropされなかったときは、新規ウインドウで項目開く
		
		AGlobalPoint	mouseGlobalPoint = {0};
		AWindow::NVI_GetCurrentMouseLocation(mouseGlobalPoint);
		//
		OpenItem(inLocalPoint,inModifierKeys,true);
	}
}
*/

/**
Loacal pointから項目検索
*/
AIndex	AView_IdInfo::FindItem( const ALocalPoint inLocalPoint )
{
	AImagePoint	imagept = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		if( imagept.y > mDrawData_Y.Get(i) && imagept.y < mDrawData_Y.Get(i) + mDrawData_Height.Get(i) )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

/**
項目を開く（Local point指定）
*/
void	AView_IdInfo::OpenItem( const ALocalPoint inLocalPoint, const AModifierKeys inModifierKeys )
{
	AIndex	index = FindItem(inLocalPoint);
	if( index != kIndex_Invalid )
	{
		SPI_OpenItem(index,inModifierKeys);
	}
}

//#853
/**
項目に表示されたキーワードの定義位置をオープンする
*/
void	AView_IdInfo::SPI_OpenItem( const AIndex inIndex, const AModifierKeys inModifierKeys )
{
	if( inIndex == kIndex_Invalid )   return;
	//現在の位置をNavigate登録する #146
	GetApp().SPI_RegisterNavigationPosition();
	//ファイル取得
	AText	filepath;
	mFilePathArray.Get(inIndex,filepath);
	//項目データ取得
	AText	keywordText;
	mKeywordTextArray.Get(inIndex,keywordText);
	AText	parentKeywordText;
	mParentKeywordArray.Get(inIndex,parentKeywordText);
	AIndex	spos = 0, epos = 0;
	spos = mStartIndexArray.Get(inIndex);
	epos = mEndIndexArray.Get(inIndex);
	//Call Grafへ通知
	GetApp().SPI_NotifyToCallGrafByIdInfoJump(mCallerFuncIdText,filepath,parentKeywordText,keywordText,spos,epos);
	//項目を開く
	GetApp().SPI_OpenOrRevealTextDocument(filepath,spos,epos,kObjectID_Invalid,
				(AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true),
				kAdjustScrollType_Top);
	/*
	//
	AObjectID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,file);
	if( docID == kObjectID_Invalid )
	{
		docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,GetEmptyText(),true,spos,epos,false);
	}
	if( docID != kObjectID_Invalid )
	{
		//#856 test GetApp().SPI_CreateNewWindowForExistingTextDocument(docID,spos,epos);
		AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
		if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
		{
			AIndex	tabIndex = GetApp().SPI_GetTextWindowByID(winID).NVI_GetTabIndexByDocumentID(docID);
			//サブペインカラム表示中ならサブペイン内に表示する #212
			if( GetApp().SPI_GetTextWindowByID(winID).SPI_IsLeftSideBarDisplayed() == true &&
						AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == false )
			{
				//サブペイン内にそのドキュメントを表示
				GetApp().SPI_GetTextWindowByID(winID).SPI_DisplayInSubText(tabIndex,false,kIndex_Invalid,true);
			}
			else
			{
				//タブ選択してビュー分割（そのビューにフォーカスが移る）
				GetApp().SPI_GetTextWindowByID(winID).NVI_SelectTab(tabIndex);
				GetApp().SPI_GetTextWindowByID(winID).SPI_SplitView(false);
			}
		}
		//選択
		GetApp().SPI_GetTextDocumentByID(docID).SPI_SelectText(spos,epos-spos);
		//
		GetApp().SPI_NotifyToCallGrafByIdInfoJump(mCallerFuncIdText,filepath,parentKeywordText,keywordText,spos,epos);
	}
	*/
	/*if( mScopeTypeArray.Get(i) != kScopeType_Import )
	{
	ADocumentID	docID = GetApp().NVI_GetDocumentIDByFile(kDocumentType_Text,mFileArray.GetObjectConst(i));
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true )
	{
	GetApp().SPI_GetTextWindowByID(winID).SPI_SplitView(false);
	}
	GetApp().SPI_GetTextWindowByID(winID).SPI_SetSelect(docID,spos,epos);
	GetApp().SPI_GetTextWindowByID(winID).SPI_AdjustScroll_Center(docID);
	GetApp().SPI_GetTextWindowByID(winID).SPI_SelectText();
	}
	else
	{
	GetApp().SPI_SelectTextDocument(mFileArray.GetObjectConst(i),spos,epos-spos);
	}*/
}

//
void	AView_IdInfo::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
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
	//Hover更新
	EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys);
}

ABool	AView_IdInfo::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

//カーソル
ABool	AView_IdInfo::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//ホバー設定
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		if( imagept.y > mDrawData_Y.Get(i) && imagept.y < mDrawData_Y.Get(i) + mDrawData_Height.Get(i) )
		{
			if( mCurrentHoverCursorIndex != i )
			{
				mCurrentHoverCursorIndex = i;
				NVI_Refresh();
			}
			return true;
		}
	}
	ClearHoverCursor();
	return true;
}

void	AView_IdInfo::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

void	AView_IdInfo::ClearHoverCursor()
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		mCurrentHoverCursorIndex = kIndex_Invalid;
		NVI_Refresh();
	}
}

//#442
/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_IdInfo::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	//#688 AApplication::GetApplication().NVI_GetMenuManager().
	//#688 ShowContextMenu(mContextMenuItemID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(mContextMenuItemID,globalPoint);//#688
	return true;
}

//#507
/**
ヘルプタグ
*/
ABool	AView_IdInfo::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	//#688 NVM_GetWindowConst().NVI_HideHelpTag();//#643
	outTagSide = kHelpTagSide_Right;//#643
	
	ANumber	viewwidth = NVI_GetViewWidth();
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//
		AImageRect	itemImageRect;
		ANumber	y = mDrawData_Y.Get(i);
		itemImageRect.left = 0;
		itemImageRect.right = viewwidth;
		itemImageRect.top = y;
		itemImageRect.bottom = y + mDrawData_Height.Get(i);
		ALocalRect	itemLocalRect;
		NVM_GetLocalRectFromImageRect(itemImageRect,itemLocalRect);
		//
		if( inPoint.x >= itemLocalRect.left && inPoint.x <= itemLocalRect.right &&
					inPoint.y >= itemLocalRect.top && inPoint.y <= itemLocalRect.bottom )
		{
			AText	text;
			AText	path;
			mFilePathArray.Get(i,path);
			//カテゴリ取得
			AIndex	categoryIndex = mCategoryIndexArray.Get(i);
			//モード設定更新とスレッド実行とのすれ違いの場合にカテゴリindexがオーバーする可能性もあるので、念のためカテゴリindex値チェック。
			if( categoryIndex < 0 || categoryIndex >= GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryCount() )
			{
				categoryIndex = 0;
			}
			//カテゴリ名取得
			AText	categoryname;
			GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryName(categoryIndex,categoryname);
			//ヘルプタグテキスト設定
			text.SetLocalizedText("IdInfoHelpTag");
			text.ReplaceParamText('0',mKeywordTextArray.GetTextConst(i));//識別子
			text.ReplaceParamText('1',path);//ファイル
			text.ReplaceParamText('2',categoryname);//カテゴリー
			text.ReplaceParamText('3',mParentKeywordArray.GetTextConst(i));//親キーワード
			text.ReplaceParamText('4',mRawCommentTextArray.GetTextConst(i));//コメント
			text.ReplaceParamText('5',mInfoTextArray.GetTextConst(i));//情報テキスト
			text.ReplaceParamText('6',mTypeTextArray.GetTextConst(i));
			switch(mScopeTypeArray.Get(i))
			{
			  case kScopeType_Global:
				{
					text.ReplaceParamText('7',"(Global)");
					break;
				}
			  case kScopeType_Import:
				{
					text.ReplaceParamText('7',"(Import)");
					break;
				}
			  case kScopeType_ModeKeyword:
				{
					text.ReplaceParamText('7',"(Keyword)");
					break;
				}
			  default:
				{
					text.ReplaceParamText('7',"");
					break;
				}
			}
			outText1.AddText(text);
			outText2.AddText(text);
			outRect = itemLocalRect;
			return true;
		}
	}
	return false;
}

/**
カーソルタイプ取得
*/
ACursorType	AView_IdInfo::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		return kCursorType_PointingHand;
	}
	return kCursorType_Arrow;
}
#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================


const AItemCount kHistoryMax	= 256;

#if 0
void	AView_IdInfo::SPI_SetIdInfo( const AText& inIdText, const ADocumentID inTextDocumentID )
/*#238
		const AModeIndex inModeIndex, 
		const ATextArray& inInfoTextArray, const AObjectArray<AFileAcc>& inFileArray,
		const ATextArray& inCommentTextArray, const AArray<AIndex>& inCategoryIndexArray,
		const AArray<AScopeType>& inScopeTypeArray, const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
		const ATextArray& inParentKeywordArray )
*/
{
	if( mFixDisplay == true )   return;//#238
	
	//#238
	//Document存在チェック
	if( GetApp().NVI_IsDocumentValid(inTextDocumentID) == false )   return;
	if( GetApp().NVI_GetDocumentTypeByID(inTextDocumentID) != kDocumentType_Text )   return;
	
	//#238 mCurrentModeIndex = inModeIndex;
	mInfoTextArray.DeleteAll();
	mCommentTextArray.DeleteAll();
	mFileArray.DeleteAll();
	mCategoryIndexArray.DeleteAll();
	mScopeTypeArray.DeleteAll();
	mStartIndexArray.DeleteAll();
	mEndIndexArray.DeleteAll();
	mParentKeywordArray.DeleteAll();
	mIdText.SetText(inIdText);
	//#238
	mCurrentModeIndex = GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetModeIndex();
	GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetIdInfoArray(mIdText,
			mInfoTextArray,mFileArray,mCommentTextArray,mCategoryIndexArray,mScopeTypeArray,mStartIndexArray,mEndIndexArray,mParentKeywordArray);
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		if( mInfoTextArray.GetTextConst(i).GetItemCount() == 0 )
		{
			mInfoTextArray.Set(i,mIdText);
		}
	}
	/*#238
	//
	for( AIndex i = 0; i < inInfoTextArray.GetItemCount(); i++ )
	{
		AText	infoText;
		infoText.SetText(inInfoTextArray.GetTextConst(i));
		if( infoText.GetItemCount() == 0 )
		{
			infoText.SetText(inIdText);
		}
		mInfoTextArray.Add(infoText);
		//
		AText	commentText;
		commentText.SetText(inCommentTextArray.GetTextConst(i));
		mCommentTextArray.Add(commentText);
		//
		mFileArray.GetObject(mFileArray.AddNewObject()).CopyFrom(inFileArray.GetObjectConst(i));
		//
		mCategoryIndexArray.Add(inCategoryIndexArray.Get(i));
		//
		mScopeTypeArray.Add(inScopeTypeArray.Get(i));
		//
		mStartIndexArray.Add(inStartIndexArray.Get(i));
		mEndIndexArray.Add(inEndIndexArray.Get(i));
		//
		mParentKeywordArray.Add(inParentKeywordArray.GetTextConst(i));
	}
	*/
	mArgIndex = kIndex_Invalid;
	mCurrentHoverCursorIndex = kIndex_Invalid;
	CalcDrawData();
	NVI_Refresh();
	
	//#238 履歴追加
	if( mInfoTextArray.GetItemCount() > 0 )
	{
		mIdTextHistory.Insert1(0,inIdText);
		AFileAcc	file;
		GetApp().NVI_GetDocumentByID(inTextDocumentID).NVI_GetFile(file);
		AText	path;
		file.GetPath(path);
		path.Add(kUChar_LineEnd);
		path.AddText(inIdText);
		mPathHistory.Insert1(0,path);
		//同じ項目があったら削除
		for( AIndex i = 1; i < mPathHistory.GetItemCount(); i++ )
		{
			if( mPathHistory.GetTextConst(i).Compare(path) == true )
			{
				mIdTextHistory.Delete1(i);
				mPathHistory.Delete1(i);
				break;
			}
		}
		//履歴maxより多ければ一番最後のを削除
		if( mIdTextHistory.GetItemCount() >= kHistoryMax )
		{
			mIdTextHistory.Delete1(mIdTextHistory.GetItemCount()-1);
			mPathHistory.Delete1(mPathHistory.GetItemCount()-1);
		}
		//IdInfoHistoryメニュー更新
		GetApp().NVI_GetMenuManager().SetContextMenuTextArray(kContextMenuID_IdInfoHistory,mIdTextHistory,mPathHistory);
	}
	
	//#238 履歴ボタン更新
	GetApp().SPI_GetIdInfoWindow().SPI_UpdateHistoryButton();
}
#endif

/**
データ設定
@note note1
@warning warning1
@note note2
@caution caution3
*/
void	AView_IdInfo::SPI_SetIdInfo( const AText& inIdText, const ATextArray& inParentKeyword,
		const ADocumentID inTextDocumentID, 
		const ATextArray& inKeywordTextArray, const ATextArray& inParentKeywordTextArray,
		const ATextArray& inTypeTextArray, const ATextArray& inInfoTextArray, const ATextArray& inCommentTextArray, 
		const AArray<AIndex>& inCategoryIndexArray, const AArray<AIndex>& inColorSlotIndexArray,
		const AArray<AScopeType>& inScopeArray,
		const AArray<AIndex>& inStartIndexArray, const AArray<AIndex>& inEndIndexArray,
		const ATextArray& inFilePathArray, const AIndex inArgIndex, const AText& inCallerFuncIdText )
{
	//ロック中なら何もしない
	if( mFixDisplay == true )   return;//#238
	
	//改行文字のみなら何もしない #0 対症療法だが、OriginalTextArray_UnicodeOffsetのindexエラーが発生する問題の対策
	if( inIdText.Compare("\r") == true )   return;
	
	//#238
	//Document存在チェック
	if( GetApp().NVI_IsDocumentValid(inTextDocumentID) == false )   return;
	if( GetApp().NVI_GetDocumentTypeByID(inTextDocumentID) != kDocumentType_Text )   return;
	
	//==================現在表示中のid textと同じならid info更新しない==================
	if( inIdText.Compare(mIdText) == true )
	{
		if( mArgIndex != inArgIndex )
		{
			//------------------キーワードは同じで引数indexのみ異なる場合------------------
			//引数index更新
			mArgIndex = inArgIndex;
			//hideカウンタ設定
			ResetHideCounter(kTimerForHidePopupIdInfo_AfterArgIndexChanged);
		}
		else
		{
			//hideカウンタ設定
			ResetHideCounter(kTimerForHidePopupIdInfo);
		}
		NVI_Refresh();
		return;
	}
	
	//==================各種パラメータ設定==================
	
	//現在引数更新
	mArgIndex = inArgIndex;
	//自動hideをenable
	mDisableAutoHide = false;
	//hideカウンタ設定
	ResetHideCounter(kTimerForHidePopupIdInfo);
	
	//==================データ設定==================
	mIdText.SetText(inIdText);
	mRequestedParentKeyword.SetFromTextArray(inParentKeyword);
	mKeywordTextArray.SetFromTextArray(inKeywordTextArray);
	mParentKeywordArray.SetFromTextArray(inParentKeywordTextArray);
	mTypeTextArray.SetFromTextArray(inTypeTextArray);
	mInfoTextArray.SetFromTextArray(inInfoTextArray);
	mCommentTextArray.SetFromTextArray(inCommentTextArray);
	mRawCommentTextArray.SetFromTextArray(inCommentTextArray);
	mCategoryIndexArray.SetFromObject(inCategoryIndexArray);
	mColorSlotIndexArray.SetFromObject(inColorSlotIndexArray);
	mScopeTypeArray.SetFromObject(inScopeArray);
	mStartIndexArray.SetFromObject(inStartIndexArray);
	mEndIndexArray.SetFromObject(inEndIndexArray);
	mFilePathArray.SetFromTextArray(inFilePathArray);
	//FuncId記憶
	mCallerFuncIdText.SetText(inCallerFuncIdText);
	//モードindex取得
	mCurrentModeIndex = GetApp().SPI_GetTextDocumentByID(inTextDocumentID).SPI_GetModeIndex();
	//infotextの補正
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//infotext取得
		AText	infoText;
		mInfoTextArray.Get(i,infoText);
		//infotextが空の場合はキーワードを設定しておく
		if( infoText.GetItemCount() == 0 )
		{
			mInfoTextArray.Set(i,inIdText);
		}
		//infotextのサイズが制限値以上のときは、制限値以降を削除する
		if( infoText.GetItemCount() > kLimit_IdInfo_InfoTextLength )
		{
			AIndex	p = infoText.GetCurrentCharPos(kLimit_IdInfo_InfoTextLength);
			infoText.DeleteAfter(p);
			mInfoTextArray.Set(i,infoText);
		}
	}
	//#835 @note等
	//元々のcomment textから、@note等の行を抜き出して、comment textに再設定
	//設定データ取得、text arrayに展開
	AText	notePrefText;
	GetApp().NVI_GetAppPrefDB().GetData_Text(AAppPrefDB::kIdInfoAtNoteText,notePrefText);
	ATextArray	noteTextArray;
	noteTextArray.ExplodeFromText(notePrefText,kUChar_LineEnd);
	AItemCount	noteTextArrayItemCount = noteTextArray.GetItemCount();
	//各IdInfo項目毎のループ
	for( AIndex i = 0; i < mCommentTextArray.GetItemCount(); i++ )
	{
		//項目の元々のcomment text取得
		AText	originalCommentText;
		mCommentTextArray.Get(i,originalCommentText);
		//comment text
		AText	commentText;
		//各@note設定text array毎のループ
		for( AIndex j = 0; j < noteTextArrayItemCount; j++ )
		{
			//note text取得
			AText	noteText;
			noteTextArray.Get(j,noteText);
			//comment text内から@note以降を検索
			AItemCount	len = originalCommentText.GetItemCount();
			for( AIndex pos = 0; pos < len; )
			{
				if( originalCommentText.FindText(pos,noteText,pos) == true )
				{
					//@noteから行末までを取得
					AText	t;
					originalCommentText.GetLine(pos,t);
					commentText.AddText(t);
					commentText.Add(kUChar_LineEnd);
				}
				else
				{
					break;
				}
			}
		}
		//抜き出した部分をcommentTextとして設定
		mCommentTextArray.Set(i,commentText);
	}
	//現在引数位置等を初期化
	mCurrentHoverCursorIndex = kIndex_Invalid;
	mCurrentSelectionIndex = kIndex_Invalid;
	//描画データ更新
	CalcDrawData();
	NVI_Refresh();
}

//#725
/**
全データ削除
*/
void	AView_IdInfo::SPI_DeleteAllIdInfo()
{
	//id info検索スレッドの現在の要求の中断
	GetApp().SPI_GetIdInfoFinderThread().AbortCurrentRequest();
	
	//各データ削除
	mKeywordTextArray.DeleteAll();
	mTypeTextArray.DeleteAll();
	mInfoTextArray.DeleteAll();
	mCommentTextArray.DeleteAll();
	mRawCommentTextArray.DeleteAll();
	mFilePathArray.DeleteAll();
	mCategoryIndexArray.DeleteAll();
	mColorSlotIndexArray.DeleteAll();
	mScopeTypeArray.DeleteAll();
	mStartIndexArray.DeleteAll();
	mEndIndexArray.DeleteAll();
	mParentKeywordArray.DeleteAll();
	mCallerFuncIdText.DeleteAll();
	mArgIndex = kIndex_Invalid;
	mCurrentModeIndex = kIndex_Invalid;
	mIdText.DeleteAll();
	mCurrentHoverCursorIndex = kIndex_Invalid;
	mCurrentSelectionIndex = kIndex_Invalid;
	mMouseTrackStartLocalPoint = kLocalPoint_00;
	mMouseDownResultIsDrag = false;
	mFixDisplay = false;
	//描画データ更新
	CalcDrawData();
}

//#853
/**
項目の最大高さを取得
*/
ANumber	AView_IdInfo::SPI_GetMaxItemHeight() const
{
	//最大高さ
	ANumber	maxHeight = 0;
	//各項目毎のループ
	AItemCount	itemCount = mDrawData_Height.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		if( maxHeight < mDrawData_Height.Get(i) )
		{
			maxHeight = mDrawData_Height.Get(i);
		}
	}
	return maxHeight + kItemBoxTopMargin + kItemBoxBottomMargin;
}

/**
選択位置設定
*/
void	AView_IdInfo::SPI_SetSelect( const AIndex inIndex )
{
	if( inIndex >= 0 && inIndex < mKeywordTextArray.GetItemCount() )
	{
		//選択設定
		mCurrentSelectionIndex = inIndex;
		//描画更新
		NVI_Refresh();
	}
}

/**
次を選択
*/
void	AView_IdInfo::SPI_SelectNext()
{
	//項目無しならリターン
	if( mKeywordTextArray.GetItemCount() == 0 )   return;
	//現在未選択なら最初を選択
	if( mCurrentSelectionIndex == kIndex_Invalid )
	{
		SPI_SetSelect(0);
	}
	//次を選択
	else
	{
		if( mCurrentSelectionIndex+1 < mKeywordTextArray.GetItemCount() )
		{
			mCurrentSelectionIndex++;
		}
		/*
		else
		{
			mCurrentSelectionIndex = 0;
		}
		*/
	}
	//スクロール調整
	AdjustScroll();
	//描画更新
	NVI_Refresh();
}

/**
前を選択
*/
void	AView_IdInfo::SPI_SelectPrev()
{
	//項目無しならリターン
	if( mKeywordTextArray.GetItemCount() == 0 )   return;
	//現在未選択なら最後を選択
	if( mCurrentSelectionIndex == kIndex_Invalid )
	{
		SPI_SetSelect(mKeywordTextArray.GetItemCount()-1);
	}
	//前を選択
	else
	{
		if( mCurrentSelectionIndex-1 >= 0 )
		{
			mCurrentSelectionIndex--;
		}
		/*
		else
		{
			mCurrentSelectionIndex = mKeywordTextArray.GetItemCount()-1;
		}
		*/
	}
	//スクロール調整
	AdjustScroll();
	//描画更新
	NVI_Refresh();
}

/**
スクロール調整
*/
void	AView_IdInfo::AdjustScroll()
{
	//選択無しなら最初にスクロール
	if( mCurrentSelectionIndex == kIndex_Invalid )
	{
		AImagePoint	pt = {0,0};
		NVI_ScrollTo(pt);
		return;
	}
	//選択項目のrectを取得
	AImageRect	itemImageRect = {0};
	GetItemBoxImageRect(mCurrentSelectionIndex,itemImageRect);
	AImageRect	frameImageRect = {0};
	NVM_GetImageViewRect(frameImageRect);
	//選択項目が画面外なら、選択項目が中央に来るようにスクロール
	if( (itemImageRect.top-kItemBoxTopMargin) < frameImageRect.top || 
		(itemImageRect.bottom+kItemBoxBottomMargin) > frameImageRect.bottom )
	{
		AImagePoint	pt = {0,0};
		pt.x = 0;
		pt.y = ((itemImageRect.top-kItemBoxTopMargin)+(itemImageRect.bottom+kItemBoxBottomMargin))/2 
				- (itemImageRect.bottom-itemImageRect.top)/2;
		NVI_ScrollTo(pt);
	}
}

//#853
/**
ハイライト引数index設定（テキストビューで引数位置でマウスホバーした場合などにコールされる）
*/
ABool	AView_IdInfo::SPI_HighlightIdInfoArgIndex( const AText& inKeyword, const AIndex inArgIndex )
{
	//hideカウンタ設定
	//#1340 引数位置以外でもマウスを動かすとここに来るのでここでタイマーリセットすべきでない ResetHideCounter(kTimerForHidePopupIdInfo_AfterArgIndexChanged);
	//
	if( mIdText.Compare(inKeyword) == true )
	{
		//引数位置がかわったときに、hideカウンターをリセットする #1340
		if( mArgIndex != inArgIndex )
		{
			ResetHideCounter(kTimerForHidePopupIdInfo_AfterArgIndexChanged);
		}
		//
		mArgIndex = inArgIndex;
		NVI_Refresh();
		return true;
	}
	else
	{
		return false;
	}
}

/**
*/
void	AView_IdInfo::SPI_UpdateDisplayComment()
{
	CalcDrawData();
	NVI_Refresh();
}

/**
*/
void	AView_IdInfo::SPI_UpdateDrawProperty()
{
	CalcDrawData();
	NVI_Refresh();
}

/**
自動hideをdisable状態にする（ポップアップウインドウマウスホバー時）
*/
void	AView_IdInfo::SPI_SetDisableAutoHide()
{
	//disableフラグON
	mDisableAutoHide = true;
	//タイマーにmax設定
	mHideCounter = kNumber_MaxNumber;
	//α値クリア
	mAlpha = 1.0;
	//描画更新
	NVI_Refresh();
}

/**
タイマー設定
*/
void	AView_IdInfo::ResetHideCounter( const ANumber inCounter )
{
	//ポップアップウインドウでない場合はタイマーにmax設定
	if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) != kSubWindowLocationType_Popup )
	{
		mHideCounter = kNumber_MaxNumber;
	}
	//環境設定で自動hide enable、かつ、自動hide disable状態でなければタイマー設定
	else if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kHideIdInfoPopupByIdle) == true && mDisableAutoHide == false )
	{
		mHideCounter = inCounter;
	}
	//自動hide disableならタイマーにmax設定
	else
	{
		mHideCounter = kNumber_MaxNumber;
	}
	
	//α値クリア
	mAlpha = 1.0;
}

/**
tickタイマー時処理
*/
void	AView_IdInfo::SPI_DoTickTimer()
{
	//ポップアップウインドウ以外は何もしない
	if( GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()) != kSubWindowLocationType_Popup )
	{
		return;
	}
	//タイマーmax時は何もしない
	if( mHideCounter == kNumber_MaxNumber )
	{
		return;
	}
	
	//タイマーカウンタデクリメント
	mHideCounter--;
	//タイマーカウンタが16以下ならα値減少
	if( mHideCounter <= 10 )//#1354 16→10
	{
		mAlpha = 0.9*mHideCounter/10;//#1354 16→10
		NVI_Refresh();
		NVM_GetWindow().NVI_UpdateWindow();
	}
	//タイマーカウンタが0になったらウインドウhide
	if( mHideCounter <= 0 )
	{
		NVM_GetWindow().NVI_Hide();
	}
}


