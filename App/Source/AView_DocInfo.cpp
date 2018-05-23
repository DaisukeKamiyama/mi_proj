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

AView_DocInfo
#142

*/

#include "stdafx.h"

#include "AView_DocInfo.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AView_DocInfo
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ

@param inWindowID 親ウインドウのID
@param inID ViewのControlID
*/
AView_DocInfo::AView_DocInfo( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID)
,mCurrentChar(0),mCurrentCanonicalDecomp(0),mSelectedColor(kColor_Black)
{
	mTemplateText_Total1.SetLocalizedText("DocInfo_Total1");
	mTemplateText_Total2.SetLocalizedText("DocInfo_Total2");
	mTemplateText_Total3.SetLocalizedText("DocInfo_Total3");
	mTemplateText_Total4.SetLocalizedText("DocInfo_Total4");
	mTemplateText_Selected1.SetLocalizedText("DocInfo_Selected1");
	mTemplateText_Selected2.SetLocalizedText("DocInfo_Selected2");
	mTemplateText_Selected3.SetLocalizedText("DocInfo_Selected3");
	mTemplateText_Selected4.SetLocalizedText("DocInfo_Selected4");
	mTemplateText_CurrentCharReturnCode.SetLocalizedText("DocInfo_CurrentCharReturnCode");
	mTemplateText_CurrentChar.SetLocalizedText("DocInfo_CurrentChar");
	mTemplateText_HeaderPath.SetLocalizedText("DocInfo_HeaderPath");
	mTemplateText_HeaderPathParagraph.SetLocalizedText("DocInfo_HeaderPathParagraph");
	mTemplateText_HeaderPathTriangle.SetLocalizedText("DocInfo_HeaderPathTriangle");
	mTemplateText_SelectedTextColor.SetLocalizedText("DocInfo_SelectedTextColor");
	mTemplateText_SelectedTextNumber.SetLocalizedText("DocInfo_SelectedTextNumber");
}

/**
デストラクタ
*/
AView_DocInfo::~AView_DocInfo()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_DocInfo::NVIDO_Init()
{
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_DocInfo::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//余白データ
const ANumber	kLeftMargin = 8;
const ANumber	kTopMargin = 5;
const ANumber	kRightMargin = 0;
const ANumber	kLineMargin = 2;

/**
描画要求時のコールバック(AView用)
*/
void	AView_DocInfo::EVTDO_DoDraw()
{
	//文字色取得
	AColor	subwindowNormalColor = GetApp().SPI_GetSubWindowLetterColor();
	
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	//=========================背景情報取得（このviewでは背景は描画しない）=========================
	//背景情報取得
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	//サブウインドウLocationTypeによって、それぞれの背景描画
	switch(GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()))
	{
	  case kSubWindowLocationType_Floating:
		{
			//フローティングの指定透過率で背景消去
			//NVMC_PaintRect(localFrameRect,backgroundColor,GetApp().SPI_GetFloatingWindowAlpha());
			break;
		}
	  default:
		{
			//ポップアップ以外ならサブウインドウ背景色で消去
			//NVMC_PaintRect(localFrameRect,backgroundColor);
			break;
		}
	}
	
	
	//サブウインドウフォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//
	NVMC_SetDefaultTextProperty(fontname,fontsize,subwindowNormalColor,kTextStyle_Normal,true,1.0);
	ANumber	lineHeight = 9, lineAscent = 3;
	NVMC_GetMetricsForDefaultTextProperty(lineHeight,lineAscent);
	//
	ANumber	y = kTopMargin;
	
	//==================トータル情報==================
	{
		//テキスト設定
		AText	text;
		text.AddText(mTemplateText_Total1);
		AIndex	charCountStart = text.GetItemCount();
		text.AddText(mTotalCharCountText);
		AIndex	charCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Total2);
		AIndex	wordCountStart = text.GetItemCount();
		text.AddText(mTotalWordCountText);
		AIndex	wordCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Total3);
		AIndex	paraCountStart = text.GetItemCount();
		text.AddText(mTotalParagraphCountText);
		AIndex	paraCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Total4);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//太字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(charCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//通常文字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(charCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//太字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(wordCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//通常文字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(wordCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//太字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(paraCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//通常文字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(paraCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	
	//==================選択範囲情報==================
	{
		//テキスト設定
		AText	text;
		text.AddText(mTemplateText_Selected1);
		AIndex	charCountStart = text.GetItemCount();
		text.AddText(mSelectedCharCountText);
		AIndex	charCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Selected2);
		AIndex	wordCountStart = text.GetItemCount();
		text.AddText(mSelectedWordCountText);
		AIndex	wordCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Selected3);
		AIndex	paraCountStart = text.GetItemCount();
		text.AddText(mSelectedParagraphCountText);
		AIndex	paraCountEnd = text.GetItemCount();
		text.AddText(mTemplateText_Selected4);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//太字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(charCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//通常文字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(charCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//太字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(wordCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//通常文字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(wordCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//太字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(paraCountStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//通常文字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(paraCountEnd));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	
	//==================現在の文字==================
	//現在文字が改行の場合
	if( mCurrentChar == kUChar_LineEnd )
	{
		//テキスト設定
		AText	text;
		text.SetText(mTemplateText_CurrentCharReturnCode);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	//現在文字が改行以外の場合
	else
	{
		//テキスト設定
		AText	text;
		text.AddText(mTemplateText_CurrentChar);
		AIndex	currentCharStart = text.GetItemCount();
		text.AddText(mCurrentCharText);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//太字
		textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(currentCharStart));
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Bold);
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	
	//==================色==================
	if( mSelectedColorText.GetItemCount() > 0 )
	{
		AText	text;
		text.AddText(mTemplateText_SelectedTextColor);
		text.AddText(mSelectedColorText);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(mSelectedColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
		
		//
		ALocalRect	colorRect = rect;
		colorRect.left	= rect.left + NVMC_GetDrawTextWidth(text) + 16;
		colorRect.right = colorRect.left + 64;
		NVMC_PaintRect(colorRect,AColorWrapper::GetColorByHTMLFormatColor(mSelectedColorText));
	}
	
	//==================数値==================
	if( mSelectedNumberText.GetItemCount() > 0 )
	{
		AText	text;
		text.AddText(mTemplateText_SelectedTextNumber);
		text.AddText(mSelectedNumberText);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(mSelectedColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	
	//==================見出しパス==================
	{
		AText	text;
		text.AddText(mTemplateText_HeaderPath);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	for( AIndex i = mHeaderTextArray.GetItemCount()-1; i >= 0 ; i-- )
	{
		AText	text;
		//見出しindex分だけスペース文字を前につける
		AText	spaceText;
		spaceText.SetRepeatedCstring(" ",mHeaderTextArray.GetItemCount()-i);
		text.AddText(spaceText);
		//"段落："
		text.AddText(mTemplateText_HeaderPathParagraph);
		//段落番号
		AText	t;
		t.SetNumber(mHeaderParagraphIndexArray.Get(i)+1);
		text.AddText(t);
		text.AddCstring(" ");
		//右三角
		text.AddText(mTemplateText_HeaderPathTriangle);
		text.AddCstring(" ");
		//見出しテキスト
		AText	headerText;
		mHeaderTextArray.Get(i,headerText);
		text.AddText(headerText);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	
	//==================デバッグ情報==================
	for( AIndex pos = 0; pos < mDebugText.GetItemCount(); )
	{
		AText	text;
		mDebugText.GetLine(pos,text);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	
	//==================プラグインテキスト情報==================
	for( AIndex pos = 0; pos < mPluginText.GetItemCount(); )
	{
		AText	text;
		mPluginText.GetLine(pos,text);
		
		//text draw data取得
		CTextDrawData	textDrawData(false);
		textDrawData.MakeTextDrawDataWithoutStyle(text,1,0,true,true,false,0,text.GetItemCount());
		//通常文字
		textDrawData.attrPos.Add(0);
		textDrawData.attrColor.Add(subwindowNormalColor);
		textDrawData.attrStyle.Add(kTextStyle_Normal);
		
		//テキスト描画
		ALocalRect	rect = localFrameRect;
		rect.left += kLeftMargin;
		rect.right -= kRightMargin;
		rect.top = y;
		rect.bottom = y + lineHeight;
		NVMC_DrawText(rect,textDrawData);
		//次のy
		y += lineHeight + kLineMargin;
	}
	
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
DocInfo更新（total）
*/
void	AView_DocInfo::SPI_UpdateDocInfo_TotalCount( const AItemCount inTotalCharCount, 
													const AItemCount inTotalWordCount,
													const AItemCount inTotalParagraphCount )
{
	//トータル情報更新
	mTotalCharCountText.SetNumber(inTotalCharCount);
	mTotalWordCountText.SetNumber(inTotalWordCount);
	mTotalParagraphCountText.SetNumber(inTotalParagraphCount);
	//描画更新
	NVI_Refresh();
}

/**
DocInfo更新（選択範囲）
*/
void	AView_DocInfo::SPI_UpdateDocInfo_SelectedCount( const AItemCount inSelectedCharCount, 
													   const AItemCount inSelectedWordCount,
													   const AItemCount inSelectedParagraphCount, 
													   const AText& inSelectedText )
{
	//選択範囲情報更新
	mSelectedCharCountText.SetNumber(inSelectedCharCount);
	mSelectedWordCountText.SetNumber(inSelectedWordCount);
	mSelectedParagraphCountText.SetNumber(inSelectedParagraphCount);
	//選択テキストで示される色
	mSelectedColorText.DeleteAll();
	mSelectedColor = kColor_Black;
	//HTMLフォーマット最初の#を削る #1095
	AText	HTMLColorText(inSelectedText);
	if( HTMLColorText.GetItemCount() == 7 )
	{
		if( HTMLColorText.Get(0) == '#' )
		{
			HTMLColorText.Delete(0,1);
		}
	}
	//HTMLフォーマットの場合、選択テキスト色設定
	if( HTMLColorText.GetItemCount() == 6 )
	{
		ABool	isHTMLColor = true;
		for( AIndex i = 0; i < 6; i++ )
		{
			AUChar	ch = HTMLColorText.Get(i);
			if( ch >= '0' && ch <= '9' )
			{
			}
			else if( ch >= 'A' && ch <= 'F' )
			{
			}
			else if( ch >= 'a' && ch <= 'f' )
			{
			}
			else
			{
				isHTMLColor = false;
				break;
			}
		}
		if( isHTMLColor == true )
		{
			mSelectedColorText.SetText(HTMLColorText);
			mSelectedColor = AColorWrapper::GetColorByHTMLFormatColor(HTMLColorText);
		}
	}
	//選択テキストで示される数値
	mSelectedNumberText.DeleteAll();
	{
		ANumber	num = inSelectedText.GetNumber();
		if( num != 0 )
		{
			mSelectedNumberText.SetFormattedCstring("%d 0x%X",num,num);
			//
			if( num >= 0x20 && num <= 0x10FFFF )
			{
				mSelectedNumberText.AddCstring(" '");
				mSelectedNumberText.AddFromUCS4Char(num);
				mSelectedNumberText.AddCstring("'");
			}
		}
	}
	//0x
	if( inSelectedText.GetItemCount() > 2 )
	{
		if( inSelectedText.Get(0) == '0' && inSelectedText.Get(1) == 'x' )
		{
			AText	hextext;
			inSelectedText.GetText(2,inSelectedText.GetItemCount()-2,hextext);
			ANumber	num = hextext.GetIntegerFromHexText();
			mSelectedNumberText.SetFormattedCstring("%d 0x%X",num,num);
			//
			if( num >= 0x20 && num < 0x200000 )
			{
				mSelectedNumberText.AddCstring(" '");
				mSelectedNumberText.AddFromUCS4Char(num);
				mSelectedNumberText.AddCstring("'");
			}
		}
	}
	//描画更新
	NVI_Refresh();
}

/**
DocInfo更新（現在文字）
*/
void	AView_DocInfo::SPI_UpdateDocInfo_CurrentChar( const AUCS4Char inChar, 
													 const AUCS4Char inCanonicalDecomp, 
													 const ATextArray& inHeaderTextArray, 
													 const AArray<AIndex>& inHeaderParagraphIndexArray, 
													 const AText& inDebugText )
{
	//==================現在文字情報更新==================
	mCurrentChar = inChar;
	mCurrentCanonicalDecomp = inCanonicalDecomp;
	//現在文字が改行以外の場合
	if( mCurrentChar != kUChar_LineEnd )
	{
		//現在文字テキスト更新
		mCurrentCharText.SetText(mTemplateText_CurrentChar);
		//------------------U+～を設定------------------
		AText	t;
		if( mCurrentChar >= 0x10000 )
		{
			t.SetFormattedCstring("U+%05X",mCurrentChar);
		}
		else
		{
			t.SetFormattedCstring("U+%04X",mCurrentChar);
		}
		//canonical decomp文字があるときは、",U+～"を設定
		if( mCurrentCanonicalDecomp != 0 )
		{
			if( mCurrentCanonicalDecomp >= 0x10000 )
			{
				t.AddFormattedCstring(",U+%05X (Canonical Decomposition)",mCurrentCanonicalDecomp);
			}
			else
			{
				t.AddFormattedCstring(",U+%04X (Canonical Decomposition)",mCurrentCanonicalDecomp);
			}
		}
		mCurrentCharText.SetText(t);
		//------------------現在文字を設定------------------
		if( mCurrentChar >= 0x20 )
		{
			AText	chtext;
			chtext.AddFromUCS4Char(mCurrentChar);
			if( mCurrentCanonicalDecomp != 0 )
			{
				//canonical decomp文字がある場合はその文字も追加
				chtext.AddFromUCS4Char(mCurrentCanonicalDecomp);
			}
			mCurrentCharText.AddCstring("  '");
			mCurrentCharText.AddText(chtext);
			mCurrentCharText.AddCstring("'");
		}
	}
	
	//==================デバッグ情報==================
	//デバッグ情報テキスト設定
	mDebugText.SetText(inDebugText);
	
	//==================見出しパス==================
	//見出しパスデータ保存
	mHeaderTextArray.SetFromTextArray(inHeaderTextArray);
	mHeaderParagraphIndexArray.SetFromObject(inHeaderParagraphIndexArray);
	
	//描画更新
	NVI_Refresh();
}

/**
JavaScriptプラグインからのテキスト設定
*/
void	AView_DocInfo::SPI_UpdateDocInfo_PluginText( const AText& inPluginText )
{
	mPluginText.SetText(inPluginText);
	//描画更新
	NVI_Refresh();
}


