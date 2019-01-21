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

AView_WordsList
#723

*/

#include "stdafx.h"

#include "AView_WordsList.h"
#include "AApp.h"

//描画用データ
//項目内各部分のマージン
const ANumber	kHeaderLeftMargin = 6;
const ANumber	kHeaderRightMargin = 6;
const ANumber	kHeaderTopMargin = 2;
const ANumber	kHeaderBottomMargin = 2;
const ANumber	kInfoTextLeftMargin_Header = 8;
const ANumber	kInfoTextLeftMargin = 12;
const ANumber	kInfoTextRightMargin = 3;
const ANumber	kInfoTextTopMargin = 2;
const ANumber	kInfoTextBottomMargin = 1;
const ANumber	kCommentTextLeftMargin = 8;
const ANumber	kCommentTextRightMargin = 3;
const ANumber	kCommentTextTopMargin = 2;
const ANumber	kCommentTextBottomMargin = 1;
//サマリー表示部のマージン
const ANumber	kSummaryLeftMargin = 8;
const ANumber	kSummaryTopMargin = 4;
const ANumber	kSummaryHeight = 100;
//item boxのマージン
const ANumber	kItemBoxLeftMargin = 5;
const ANumber	kItemBoxRightMargin = 5;
const ANumber	kItemBoxTopMargin = 2;
const ANumber	kItemBoxBottomMargin = 1;

const ANumber	kVScrollUnit = 8;

#pragma mark ===========================
#pragma mark [クラス]AView_WordsList
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AView_WordsList::AView_WordsList( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mAllFilesSearched(false), mWordsListFinderThreadID(kObjectID_Invalid)
,mCurrentHoverCursorIndex(kIndex_Invalid),mCurrentSelectionIndex(kIndex_Invalid), mMouseTrackStartLocalPoint(kLocalPoint_00)
,mModeIndex(kIndex_Invalid),mIsFinding(false),mLockedMode(false), mDrawDataAllCalculated(false), mSearchInProjectFolder(false)
,mLastRequest_ModeIndex(kIndex_Invalid),mMouseClickCount(0)
{
	NVMC_SetOffscreenMode(true);
	
	//ワードリスト検索スレッド生成、起動
	AThread_WordsListFinderFactory	factory(this);
	mWordsListFinderThreadID = GetApp().NVI_CreateThread(factory);
	GetWordsListFinderThread().SetWordsListFinderType(kWordsListFinderType_ForWordsList);
	GetWordsListFinderThread().NVI_Run(true);
	
	//全データ削除
	SPI_DeleteAllData();
}

/**
デストラクタ
*/
AView_WordsList::~AView_WordsList()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_WordsList::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_WordsList::NVIDO_DoDeleted()
{
	//ワードリスト検索スレッド終了、削除
	if( mWordsListFinderThreadID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteThread(mWordsListFinderThreadID);
	}
}

/**
ワードリスト検索スレッド取得
*/
AThread_WordsListFinder&	AView_WordsList::GetWordsListFinderThread()
{
	return (dynamic_cast<AThread_WordsListFinder&>(GetApp().NVI_GetThreadByID(mWordsListFinderThreadID)));
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
描画要求時のコールバック(AView用)
*/
void	AView_WordsList::EVTDO_DoDraw()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	
	//==================色取得==================
	
	//描画色設定
	AColor	letterColor = kColor_Black;
	AColor	dropShadowColor = kColor_White;
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	NVMC_SetDropShadowColor(dropShadowColor);
	
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
	  case kSubWindowLocationType_Floating:
		{
			//フローティングの指定透過率で背景消去
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			//NVMC_PaintRect(frameRect,backgroundColor,backgroundAlpha);
			break;
		}
	  default:
		{
			//不透過で背景消去
			//NVMC_PaintRect(frameRect,backgroundColor);
			break;
		}
	} 
	
	//=========================各項目毎ループ=========================
	for( AIndex i = 0; i < mContentTextArray.GetItemCount(); i++ )
	{
		//項目rect取得
		AImageRect	itemImageRect = {0};
		GetItemBoxImageRect(i,itemImageRect);//#643
		ALocalRect	itemLocalRect = {0};
		NVM_GetLocalRectFromImageRect(itemImageRect,itemLocalRect);
		
		//updateRect内にない場合はcontinue
		if( NVMC_IsRectInDrawUpdateRegion(itemLocalRect) == false )   continue;
		
		//index:32以降の項目は、高速化のために、SPI_DoWordsListFinderPaused()の段階ではCalcDrawData()しないので、
		//計算済みフラグmDrawDataAllCalculatedがfalseのときは、ここで計算する。
		if( i >= 32 && mDrawDataAllCalculated == false )
		{
			//index:32以降の項目の行計算
			CalcDrawData(true);
			//項目rectの再取得
			GetItemBoxImageRect(i,itemImageRect);
			NVM_GetLocalRectFromImageRect(itemImageRect,itemLocalRect);
		}
		
		//最後の項目以外は、隙間を白（α:0.2）で描画（ポップアップウインドウの場合に、ホイールスクロールを隙間でも有効にするため。透明だとホイールがが効かない。）現状、ワードリストにポップアップはないが、将来対応するかもしれないので残す。
		if( i != mContentTextArray.GetItemCount() -1 )
		{
			ALocalRect	gapLocalRect = {0};
			gapLocalRect.left		= itemLocalRect.left + 5;
			gapLocalRect.top		= itemLocalRect.bottom;
			gapLocalRect.right		= itemLocalRect.right - 5;
			gapLocalRect.bottom		= itemLocalRect.bottom + kItemBoxTopMargin + kItemBoxBottomMargin;
			NVMC_PaintRect(gapLocalRect,boxBaseColor2,0.2);
		}
		
		//ボックス描画#643
		//content部分色取得
		AColor	contentColor_start = kColor_White;
		AColor	contentColor_end = kColor_Gray95Percent;
		/*選択項目は選択色で囲むことにする
		if( mCurrentSelectionIndex == i )
		{
			//現在アクティブなサブウインドウかどうか（cmd+option+↑↓対象か）によって、色を変える
			if( NVM_GetWindow().GetObjectID() == GetApp().SPI_GetActiveSubWindowForItemSelector() )
			{
				//アクティブなサブウインドウ
				AColorWrapper::GetHighlightColor(contentColor_end);
			}
			else
			{
				//非アクティブなサブウインドウ
				contentColor_start = kColor_Gray90Percent;
				contentColor_end = kColor_Gray90Percent;
			}
		}
		*/
		//ボックス描画
		NVMC_PaintRoundedRect(itemLocalRect,
							  //contentColor_start,contentColor_end,
							  boxBaseColor1,boxBaseColor3,
							  kGradientType_Vertical,backgroundAlpha,backgroundAlpha,
							  5,true,true,true,true);
		//==================フレーム表示（選択項目の場合、選択色で囲み表示）==================
		if( mCurrentSelectionIndex == i )
		{
			//選択色で囲み表示
			AColor	boxFrameColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
			NVMC_FrameRoundedRect(itemLocalRect,boxFrameColor,1.0,5,true,true,true,true,true,true,true,true,1.0);
		}
		else
		{
			//通常色囲み
			NVMC_FrameRoundedRect(itemLocalRect,kColor_Gray50Percent,1.0,5,true,true,true,true);
		}
		
		//ホバー描画
		if( mCurrentHoverCursorIndex == i )
		{
			/*
			AColor	selcolor;
			AColorWrapper::GetHighlightColor(selcolor);
			NVMC_PaintRect(itemLocalRect,selcolor,0.1);
			NVMC_FrameRect(itemLocalRect,selcolor,1.0);
			*/
			//ホバー色で囲み表示
			AColor	hoverColor = GetApp().SPI_GetSubWindowRoundedRectBoxHoverColor();
			NVMC_FrameRoundedRect(itemLocalRect,hoverColor,0.3,5,true,true,true,true,true,true,true,true,1.0);
		}
		
		//=========================ヘッダ部分描画=========================
		
		//ヘッダ文字色
		AColor	headerLetterColor = letterColor;
		//ヘッダ色取得
		AColor	headercolor = kColor_Blue;//#1316 ★機能復活するならダークモード対応必要 GetApp().SPI_GetSubWindowsBoxHeaderColor();;//青色固定（カテゴリ色にするのは、背景黒系の色などで色彩設計が難しいので） mHeaderColorArray.Get(i);
		if( mClickedArray.Get(i) == true )
		{
			headercolor = kColor_Gray;
		}
		//ヘッダrect取得
		AImageRect	headerImageRect = itemImageRect;//#643
		headerImageRect.bottom = itemImageRect.top + mDrawData_HeaderHeight.Get(i);//#643
		ALocalRect	headerLocalRect = {0};
		NVM_GetLocalRectFromImageRect(headerImageRect,headerLocalRect);
		//ヘッダrect描画
		AColor	headercolor_start = AColorWrapper::ChangeHSV(headercolor,0,0.8,1.0);//#643
		AColor	headercolor_end = AColorWrapper::ChangeHSV(headercolor,0,1.2,1.0);//#643
		//
		AFloatNumber	headerAlpha = 0.15;
		/*test
		if( mCurrentSelectionIndex == i )
		{
			AColorWrapper::GetListHighlightColor(headercolor_end);
			headercolor_start = AColorWrapper::ChangeHSV(headercolor_end,0,0.6,1.0);
			headerLetterColor = kColor_White;
			headerAlpha = 1.0;
		}
		*/
		//
		NVMC_PaintRoundedRect(headerLocalRect,headercolor_start,headercolor_end,kGradientType_Vertical,headerAlpha,headerAlpha,
							  5,true,true,true,true);
		
		//==================番号描画==================
		//ファイル名フォント取得
		NVMC_SetDefaultTextProperty(labelfontname,fontsize-0.5,headerLetterColor,kTextStyle_Normal,true);
		ANumber	labelfontheight = 9 ,labelfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(labelfontheight,labelfontascent);
		//ヘッダrect取得
		AImageRect	labelImageRect = itemImageRect;//#643
		labelImageRect.left		= itemImageRect.left + kHeaderLeftMargin;//#643
		labelImageRect.right	= itemImageRect.right - kHeaderRightMargin;
		labelImageRect.bottom 	= itemImageRect.top + labelfontheight +kHeaderTopMargin;//#643
		labelImageRect.top		+= kHeaderTopMargin;
		ALocalRect	labelLocalRect = {0};
		NVM_GetLocalRectFromImageRect(labelImageRect,labelLocalRect);
		//項目番号描画
		AText	itemnumtext;
		itemnumtext.AddFormattedCstring("(%d/%d)",i+1,mContentTextArray.GetItemCount());
		NVMC_DrawText(labelLocalRect,itemnumtext,headerLetterColor,kTextStyle_Normal,kJustification_Right);
		
		//=========================ファイル名テキスト描画=========================
		
		//
		ALocalRect	filenameLocalRect = labelLocalRect;
		filenameLocalRect.right -= NVMC_GetDrawTextWidth(itemnumtext);
		//ファイル名取得
		AText	filename;
		mFilePathArray.GetTextConst(i).GetFilename(filename);
		//ファイル名テキスト描画
		if( filename.GetItemCount() > 0 )
		{
			NVMC_DrawText(filenameLocalRect,filename,headerLetterColor,kTextStyle_Bold|kTextStyle_DropShadow,kJustification_Left);
		}
		
		//==================クラス名テキスト描画==================
		
		//親キーワード取得
		AText	parentKeyword;
		mClassNameArray.Get(i,parentKeyword);
		if( parentKeyword.GetItemCount() > 0 )
		{
			ALocalRect	classLocalRect = filenameLocalRect;
			classLocalRect.left += NVMC_GetDrawTextWidth(filename) +8;
			//クラス名テキスト
			AText	classnameText;
			classnameText.SetCstring("(");
			classnameText.AddLocalizedText("Letter_ParentKeyword");
			classnameText.AddText(parentKeyword);
			classnameText.AddCstring(")");
			//クラス名テキスト描画
			NVMC_DrawText(classLocalRect,classnameText,headerLetterColor,kTextStyle_Bold|kTextStyle_DropShadow,kJustification_Left);
		}
		
		//=========================InfoText部分（見出し部分）描画=========================
		
		//InfoText（見出し）フォント設定
		{
			NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Black,kTextStyle_Bold,true);//#1316 ★機能復活するならダークモード対応必要 
			ANumber	infotextfontheight = 9, infotextfontascent = 7;
			NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
			//関数名取得
			AText	headerText;
			mFunctionNameArray.Get(i,headerText);
			//見出し描画
			AImageRect	lineImageRect = itemImageRect;
			lineImageRect.left		= itemImageRect.left + kInfoTextLeftMargin_Header;
			lineImageRect.top 		= itemImageRect.top + mDrawData_HeaderHeight.Get(i) + kInfoTextTopMargin;
			lineImageRect.bottom	= lineImageRect.top + infotextfontheight;
			ALocalRect	lineLocalRect = {0};
			NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
			NVMC_DrawText(lineLocalRect,headerText);
		}
		
		//=========================InfoText部分（本体部分）描画=========================
		
		//InfoTextフォント設定
		NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,letterColor,kTextStyle_Normal,true);
		ANumber	infotextfontheight = 9, infotextfontascent = 7;
		NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
		//
		ANumber	lineleftmargin = kInfoTextLeftMargin;
		//各行ごとループ
		for( AIndex lineIndex = 0; lineIndex < mDrawData_InfoTextLineStart.GetObjectConst(i).GetItemCount(); lineIndex++ )
		{
			//行データ取得
			AIndex	start 	= mDrawData_InfoTextLineStart .GetObjectConst(i).Get(lineIndex);
			AItemCount	len = mDrawData_InfoTextLineLength.GetObjectConst(i).Get(lineIndex);
			AText	text;
			mContentTextArray.GetTextConst(i).GetText(start,len,text);
			//text draw data取得
			CTextDrawData	textDrawData(false);
			textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
			//行の最初のattr設定
			{
				textDrawData.attrPos.Add(0);
				textDrawData.attrColor.Add(letterColor);
				textDrawData.attrStyle.Add(kTextStyle_Normal);
			}
			//ワード一致箇所を赤文字表示する
			AIndex	contentTextSpos = mContentTextStartIndexArray.Get(i);//周辺テキストの開始text index
			if( contentTextSpos != kIndex_Invalid )
			{
				//ワード一致箇所の、現在行でのx位置を取得
				AIndex	s = mStartArray.Get(i)-contentTextSpos - start;
				AIndex	e = mEndArray.Get(i)-contentTextSpos - start;
				if( s >= 0 && s < len && e >= 0 && e < len )
				{
					//ワード一致箇所が現在行の中にあれば、赤色にする
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(s));
					textDrawData.attrColor.Add(kColor_Red);//#1316 ★機能復活するならダークモード対応必要GetApp().SPI_GetSubWindowBoxMatchedColor());
					textDrawData.attrStyle.Add(kTextStyle_Bold);
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(e));
					textDrawData.attrColor.Add(letterColor);
					textDrawData.attrStyle.Add(kTextStyle_Normal);
				}
			}
			//行描画
			AImageRect	lineImageRect = itemImageRect;//#643
			lineImageRect.left		= itemImageRect.left + lineleftmargin;//#643
			lineImageRect.top 		= itemImageRect.top + mDrawData_HeaderHeight.Get(i) +
									infotextfontheight +
									lineIndex*infotextfontheight + kInfoTextTopMargin;//#643
			lineImageRect.bottom	= lineImageRect.top + infotextfontheight + kInfoTextTopMargin;//#643
			ALocalRect	lineLocalRect = {0};
			NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
			NVMC_DrawText(lineLocalRect,textDrawData);
		}
	}
	
	//==================サマリ表示==================
	//サマリ表示rect取得
	AImageRect	summaryRect = {0};
	summaryRect.left	= kSummaryLeftMargin;
	summaryRect.top		= kSummaryTopMargin;
	if( mDrawData_Y.GetItemCount() > 0 )
	{
		summaryRect.top += mDrawData_Y.Get(mDrawData_Y.GetItemCount()-1) + mDrawData_Height.Get(mDrawData_Height.GetItemCount()-1);
	}
	summaryRect.right	= NVI_GetViewWidth();
	summaryRect.bottom	= summaryRect.top + kSummaryHeight;
	ALocalRect	summaryLocalRect = {0};
	NVM_GetLocalRectFromImageRect(summaryRect,summaryLocalRect);
	//サマリテキスト取得
	AText	summaryText;
	if( mContentTextArray.GetItemCount() == 0 )
	{
		summaryText.SetLocalizedText("WordsListSummary_NoResult");
	}
	else
	{
		if( mSearchInProjectFolder == true )
		{
			summaryText.SetLocalizedText("WordsListSummary_ProjectFolder");
		}
		else
		{
			summaryText.SetLocalizedText("WordsListSummary_SameFolder");
		}
		AText	t;
		t.SetLocalizedText("WordsListSummary_WholeWord");
		summaryText.AddText(t);
		//
		if( mContentTextArray.GetItemCount() >= kLimit_WordsListResultCount )
		{
			t.SetLocalizedText("WordsListSummary_TooManyItems");
			summaryText.AddText(t);
		}
		else if( GetApp().SPI_GetWordsListWindow(NVM_GetWindow().GetObjectID()).SPI_GetShowImportFileProgress() == true )
		{
			t.SetLocalizedText("WordsListSummary_ImportFileProgress");
			summaryText.AddText(t);
		}
	}
	//サマリ描画（マルチライン）
	AColor	summaryColor = GetApp().SPI_GetSubWindowHeaderLetterColor();
	NVI_DrawTextMultiLine(summaryLocalRect,summaryText,labelfontname,fontsize,summaryColor,kTextStyle_Bold,true);
}

/**
各ItemBoxのimagerect取得
*/
void	AView_WordsList::GetItemBoxImageRect( const AIndex inIndex, AImageRect& outImageRect ) const
{
	outImageRect.left		= kItemBoxLeftMargin;
	outImageRect.top		= mDrawData_Y.Get(inIndex) + kItemBoxTopMargin;
	outImageRect.right		= NVI_GetViewWidth() - kItemBoxRightMargin;
	outImageRect.bottom		= outImageRect.top + mDrawData_Height.Get(inIndex);
}

//DrawData計算
void	AView_WordsList::CalcDrawData( const ABool inCalcAll )
{
	//描画データ削除
	mDrawData_Y.DeleteAll();
	mDrawData_Height.DeleteAll();
	mDrawData_HeaderHeight.DeleteAll();
	mDrawData_InfoTextHeight.DeleteAll();
	mDrawData_InfoTextLineStart.DeleteAll();
	mDrawData_InfoTextLineLength.DeleteAll();
	mDrawDataAllCalculated = inCalcAll;
	
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
	
	//=========================ヘッダ部分１行の高さ計算=========================
	NVMC_SetDefaultTextProperty(labelfontname,fontsize,kColor_Black,kTextStyle_Bold,true);
	ANumber	headerfontheight = 9, headerfontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(headerfontheight,headerfontascent);
	
	//=========================InfoText部分１行の高さ計算、text property保存=========================
	//フォント高さ取得
	NVMC_SetDefaultTextProperty(fontname,fontsize-0.5,kColor_Black,kTextStyle_Normal,true);
	ANumber	infotextfontheight = 9, infotextfontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(infotextfontheight,infotextfontascent);
	
	//=========================項目毎ループ=========================
	ANumber	y = 0;
	for( AIndex i = 0; i < mContentTextArray.GetItemCount(); i++ )
	{
		mDrawData_Y.Add(y);
		mDrawData_InfoTextLineStart .AddNewObject();
		mDrawData_InfoTextLineLength.AddNewObject();
		
		//=========================ヘッダ部分データ保存=========================
		mDrawData_HeaderHeight.Add(kHeaderTopMargin+headerfontheight+kHeaderBottomMargin);
		
		//=========================InfoText部分計算=========================
		//InfoText取得
		AText	infoText;
		mContentTextArray.Get(i,infoText);
		//InfoText行折り返し計算
		if( inCalcAll == true || i < 32 )
		{
			/*
			CWindowImp::CalcLineBreaks(infoText,fontname,fontsize-0.5,true,
						viewwidth-kItemBoxLeftMargin-kItemBoxRightMargin -kInfoTextLeftMargin2 - kInfoTextRightMargin,
						mDrawData_InfoTextLineStart.GetObject(i),mDrawData_InfoTextLineLength.GetObject(i));
			*/
			//行折り返し無しで計算する
			infoText.CalcParagraphBreaks(mDrawData_InfoTextLineStart.GetObject(i),mDrawData_InfoTextLineLength.GetObject(i));
			//最終行は空行なので削除する
			AItemCount	itemCount = mDrawData_InfoTextLineStart.GetObject(i).GetItemCount();
			if( itemCount > 0 )
			{
				mDrawData_InfoTextLineStart.GetObject(i).DeleteAfter(itemCount-1);
				mDrawData_InfoTextLineLength.GetObject(i).DeleteAfter(itemCount-1);
			}
		}
		else
		{
			//inCalcAllがfalseのときは、index:32未満の行のみ計算する（描画時に必要に応じてindex:32以降を計算する）
			mDrawData_InfoTextLineStart.GetObject(i).Add(0);
			mDrawData_InfoTextLineLength.GetObject(i).Add(0);
		}
		//
		//InfoText全体の高さ計算
		mDrawData_InfoTextHeight.Add(kInfoTextTopMargin+
					infotextfontheight*mDrawData_InfoTextLineStart.GetObject(i).GetItemCount() +
					kInfoTextBottomMargin);
		
		//項目全体の高さ設定
		mDrawData_Height.Add(mDrawData_HeaderHeight.Get(i) + mDrawData_InfoTextHeight.Get(i) + 2 + infotextfontheight);
		//次の項目のy位置を計算
		y += mDrawData_Height.Get(i);
		y += kItemBoxTopMargin + kItemBoxBottomMargin;//#643
	}
	//ImageSize設定
	NVM_SetImageSize(viewwidth,y + kSummaryHeight);
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(0,kVScrollUnit,0,rect.bottom-rect.top-kVScrollUnit);
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_WordsList::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//tracking開始時マウス位置記憶
	mMouseTrackStartLocalPoint = inLocalPoint;
	mMouseClickCount = inClickCount;
	
	//Mouse tracking開始
	NVM_SetMouseTrackingMode(true);
	return false;
}

/**
マウスボタン押下中TrackingのOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_WordsList::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
}

/**
マウスボタン押下中Tracking終了時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
*/
void	AView_WordsList::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//クリック位置の項目index取得
	AIndex	index = FindItem(inLocalPoint);
	if( index != kIndex_Invalid )
	{
		//ロックモード設定
		GetApp().SPI_GetWordsListWindow(NVM_GetWindow().GetObjectID()).SPI_SetLockedMode(mMouseClickCount==1);
		//クリックした項目を開く
		SPI_OpenItem(index,inModifierKeys);
	}
	//Tracking解除
	NVM_SetMouseTrackingMode(false);
}

/**
Loacal pointから項目検索
*/
AIndex	AView_WordsList::FindItem( const ALocalPoint inLocalPoint ) const
{
	AImagePoint	imagept = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mContentTextArray.GetItemCount(); i++ )
	{
		if( imagept.y > mDrawData_Y.Get(i) && imagept.y < mDrawData_Y.Get(i) + mDrawData_Height.Get(i) )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

/**
次の項目を選択
*/
void	AView_WordsList::SPI_SelectNextItem()
{
	if( mCurrentSelectionIndex >= 0 && mCurrentSelectionIndex+1 < mContentTextArray.GetItemCount() )
	{
		//現在選択位置を次にする
		mCurrentSelectionIndex++;
		//項目を開く
		SPI_OpenItem(mCurrentSelectionIndex,0);
		//スクロール調整
		SPI_AdjustScroll();
	}
}

/**
前の項目を選択
*/
void	AView_WordsList::SPI_SelectPreviousItem()
{
	if( mCurrentSelectionIndex-1 >= 0 && mCurrentSelectionIndex < mContentTextArray.GetItemCount() )
	{
		//現在選択位置を前にする
		mCurrentSelectionIndex--;
		//項目を開く
		SPI_OpenItem(mCurrentSelectionIndex,0);
		//スクロール調整
		SPI_AdjustScroll();
	}
}

/**
項目選択状態解除
*/
void	AView_WordsList::SPI_CancelSelect()
{
	mCurrentSelectionIndex = kIndex_Invalid;
	NVI_Refresh();
}

//#853
/**
項目に表示されたキーワードの定義位置をオープンする
*/
void	AView_WordsList::SPI_OpenItem( const AIndex inIndex, const AModifierKeys inModifierKeys )
{
	//Appにこのサブウインドウがアクティブサブウインドウであることを設定
	GetApp().SPI_SetActiveSubWindowForItemSelector(NVM_GetWindow().GetObjectID());
	//現在選択位置を設定
	mCurrentSelectionIndex = inIndex;
	//kIndex_Invalidなら終了
	if( inIndex == kIndex_Invalid )
	{
		return;
	}
	//クリック済みを記憶
	mClickedArray.Set(inIndex,true);
	//描画
	NVI_Refresh();
	
	//現在の位置をNavigate登録する #146
	GetApp().SPI_RegisterNavigationPosition();
	//ファイル取得
	AText	filepath;
	mFilePathArray.Get(inIndex,filepath);
	//項目データ取得
	AIndex	spos = 0, epos = 0;
	spos = mStartArray.Get(inIndex);
	epos = mEndArray.Get(inIndex);
	//項目を開く
	GetApp().SPI_OpenOrRevealTextDocument(filepath,spos,epos,kObjectID_Invalid,
				(AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true),
				kAdjustScrollType_Center);
}

/**
マウスホイール時処理
*/
void	AView_WordsList::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
		const ALocalPoint inLocalPoint )
{
	//スクロール率を設定から取得する
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true 
				|| AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercentCmd);
	}
	else
	{
		scrollPercent = GetApp().GetAppPref().GetData_Number(AAppPrefDB::kWheelScrollPercent);
	}
	//スクロール実行
	NVI_Scroll(0,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);
	//ホバー更新前の項目indexを取得
	AIndex	svHoverIndex = mCurrentHoverCursorIndex;
	//Hover更新
	EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys);
	//ホバー項目indexに変化がある場合は、ヘルプタグも更新
	if( svHoverIndex != mCurrentHoverCursorIndex )
	{
		NVM_GetWindow().NVI_UpdateHelpTag(NVI_GetControlID(),inLocalPoint,inModifierKeys,true);
	}
}

/**
マウスホバー時処理
*/
ABool	AView_WordsList::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//ホバー設定
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	for( AIndex i = 0; i < mContentTextArray.GetItemCount(); i++ )
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

/**
マウスleave時処理
*/
void	AView_WordsList::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

/**
ホバー消去
*/
void	AView_WordsList::ClearHoverCursor()
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		mCurrentHoverCursorIndex = kIndex_Invalid;
		NVI_Refresh();
	}
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_WordsList::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	/*
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	AApplication::GetApplication().NVI_GetMenuManager().
	ShowContextMenu(mContextMenuItemID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	*/
	return true;
}

/**
ヘルプタグ
*/
ABool	AView_WordsList::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	AIndex	index = FindItem(inPoint);
	if( index != kIndex_Invalid )
	{
		outTagSide = kHelpTagSide_Right;
		mContentTextArray.Get(index,outText1);
		//ヘルプタグテキスト設定
		outText1.SetLocalizedText("WordsListHelpTag");
		outText1.ReplaceParamText('1',mFilePathArray.GetTextConst(index));//ファイル
		outText1.ReplaceParamText('2',mClassNameArray.GetTextConst(index));//親キーワード
		outText1.ReplaceParamText('3',mFunctionNameArray.GetTextConst(index));//見出し
		outText1.ReplaceParamText('4',mContentTextArray.GetTextConst(index));//テキスト
		AImageRect	imageRect = {0};
		imageRect.left		= 0;
		imageRect.right		= NVI_GetViewWidth();
		imageRect.top		= mDrawData_Y.Get(index);
		imageRect.bottom	= imageRect.top + 10;//mDrawData_Height.Get(index);
		NVM_GetLocalRectFromImageRect(imageRect,outRect);
		return true;
	}
	else
	{
		return false;
	}
}

/**
テキスト入力時処理
*/
ABool	AView_WordsList::EVTDO_DoTextInput( const AText& inText, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

/**
マウスカーソル設定
*/
ACursorType	AView_WordsList::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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

/**
ワードリスト検索要求
*/
void	AView_WordsList::SPI_RequestWordsList( const AFileAcc& inProjectFolder, const AIndex inModeIndex, const AText& inWord )
{
	//前回検索ワードと同じ、かつ、プロジェクトも同じなら何もしない
	if( mWord.Compare(inWord) == true && mCurrentProjectFolder.Compare(inProjectFolder) == true )
	{
		return;
	}
	
	//最後の要求データ記憶
	mLastRequest_ProjectFolder = inProjectFolder;
	mLastRequest_ModeIndex = inModeIndex;
	mLastRequest_Word.SetText(inWord);
	
	//ロックモード中は何もしない
	if( mLockedMode == true )
	{
		return;
	}
	
	//検索結果データ全削除
	SPI_DeleteAllData();
	
	//スクロール調整
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
	//再描画
	NVI_Refresh();
	
	//検索パラメータ記憶
	mCurrentProjectFolder.CopyFrom(inProjectFolder);
	mModeIndex = inModeIndex;
	mWord.SetText(inWord);
	//プロジェクトフォルダの検索かどうかを記憶
	mSearchInProjectFolder = (GetApp().SPI_GetSameProjectArrayIndex(inProjectFolder)!=kIndex_Invalid);
	
	//スレッドにワーズリスト検索要求
	if( inWord.GetItemCount() > 0 )
	{
		GetWordsListFinderThread().SetRequestDataForWordsList(mCurrentProjectFolder,inModeIndex,NVM_GetWindowConst().GetObjectID(),inWord);
		//スレッドunpause
		GetWordsListFinderThread().NVI_UnpauseIfPaused();
		
		//検索中フラグON
		mIsFinding = true;
	}
	
	//ヘッダビュー更新
	AText	path;
	mCurrentProjectFolder.GetPath(path);
	GetApp().SPI_GetWordsListWindow(NVM_GetWindow().GetObjectID()).SPI_SetHeaderText(mWord,path,mModeIndex);
}

/**
検索中断
*/
void	AView_WordsList::SPI_AbortCurrentWordsListFinderRequest()
{
	GetWordsListFinderThread().AbortCurrentRequest();
}

/**
検索結果全削除
*/
void	AView_WordsList::SPI_DeleteAllData()
{
	//スレッドでの現在の検索を中断
	SPI_AbortCurrentWordsListFinderRequest();
	
	//現在選択項目等クリア
	mCurrentHoverCursorIndex = kIndex_Invalid;
	mCurrentSelectionIndex = kIndex_Invalid;
	mMouseTrackStartLocalPoint = kLocalPoint_00;
	//要求データクリア
	mWord.DeleteAll();
	mCurrentProjectFolder.Unspecify();
	mModeIndex = kIndex_Invalid;
	mSearchInProjectFolder = false;
	//状態クリア
	mLockedMode = false;
	mIsFinding = false;
	//検索結果全削除
	mFilePathArray.DeleteAll();
	mFunctionNameArray.DeleteAll();
	mClassNameArray.DeleteAll();
	mClassNameArray.DeleteAll();
	mStartArray.DeleteAll();
	mEndArray.DeleteAll();
	mHeaderColorArray.DeleteAll();
	mContentTextArray.DeleteAll();
	mContentTextStartIndexArray.DeleteAll();
	mClickedArray.DeleteAll();
	mAllFilesSearched = false;
	//描画データクリア
	CalcDrawData(false);
}

/**
スレッドのワーズリスト検索結果反映
*/
void	AView_WordsList::SPI_DoWordsListFinderPaused()
{
	//検索結果取得
	ABool	allFilesSearched = GetWordsListFinderThread().GetResultForWordsList(mFilePathArray,mClassNameArray,mFunctionNameArray,mStartArray,mEndArray,mHeaderColorArray,mContentTextArray,mContentTextStartIndexArray);
	//現在選択項目クリア
	mCurrentSelectionIndex = kIndex_Invalid;
	//クリック済みフラグクリア
	for( AIndex i = 0; i < mFilePathArray.GetItemCount(); i++ )
	{
		mClickedArray.Add(false);
	}
	//全ファイル検索済みフラグ記憶
	mAllFilesSearched = allFilesSearched;
	//描画データ計算（index:32未満まで。高速化のため。それ以降は描画時に必要に応じて描画）
	CalcDrawData(false);
	//スクロール調整
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
	//検索中フラグOFF
	mIsFinding = false;
	//ウインドウの描画プロパティ更新
	NVM_GetWindow().NVI_UpdateProperty();
	//再描画
	NVI_Refresh();
}

/**
ドキュメント編集時にコールされる。テキスト位置の更新。
*/
void	AView_WordsList::SPI_NotifyToWordsListByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//更新ファイルを取得
	AText	filepath;
	inFile.GetPath(filepath);
	//更新ファイルに一致する項目のindexを取得
	AArray<AIndex>	indexArray;
	mFilePathArray.FindAll(filepath,0,filepath.GetItemCount(),indexArray);
	//各項目毎のループ
	AItemCount	itemCount = indexArray.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AIndex	index = indexArray.Get(i);
		//start indexが更新位置以降の場合は、start index, end indexを更新
		AIndex	spos = mStartArray.Get(index);
		if( spos > inTextIndex )
		{
			mStartArray.Set(index,spos+inInsertedCount);
			mEndArray.Set(index,mEndArray.Get(index)+inInsertedCount);
		}
	}
}

/**
描画データ更新（ウインドウbounds変更時等）
*/
void	AView_WordsList::SPI_UpdateDrawProperty()
{
	CalcDrawData(true);
	NVI_Refresh();
}

/**
スクロール調整
*/
void	AView_WordsList::SPI_AdjustScroll()
{
	//現在選択項目が中央に表示されるようにスクロール調整
	if( mCurrentSelectionIndex != kIndex_Invalid )
	{
		//現在選択項目のrectを取得
		AImageRect	box = {0};
		GetItemBoxImageRect(mCurrentSelectionIndex,box);
		AImageRect	imageFrameRect = {0};
		NVM_GetImageViewRect(imageFrameRect);
		//frameより下に項目がある場合
		if( box.bottom > imageFrameRect.bottom )
		{
			AImagePoint	imagept = {imageFrameRect.left,box.top-(imageFrameRect.bottom-imageFrameRect.top)/2};
			if( imagept.y < 0 )   imagept.y = 0;
			NVI_ScrollTo(imagept);
		}
		//frameより上に項目がある場合
		if( box.top < imageFrameRect.top )
		{
			AImagePoint	imagept = {imageFrameRect.left,box.top-(imageFrameRect.bottom-imageFrameRect.top)/2};
			if( imagept.y < 0 )   imagept.y = 0;
			NVI_ScrollTo(imagept);
		}
	}
}

/**
ロックモードに設定
*/
void	AView_WordsList::SPI_SetLockedMode( const ABool inLocked )
{
	mLockedMode = inLocked;
	SPI_CancelSelect();
	//解除時は、最後の要求を適用する
	if( inLocked == false && mLastRequest_Word.GetItemCount() > 0 )
	{
		AFileAcc	projectFolder = mLastRequest_ProjectFolder;
		AIndex	modeIndex = mLastRequest_ModeIndex;
		AText	word = mLastRequest_Word;
		SPI_RequestWordsList(projectFolder,modeIndex,word);
	}
}

