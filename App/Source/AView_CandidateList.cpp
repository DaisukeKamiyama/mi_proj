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

AView_CandidateList

*/

#include "stdafx.h"

#include "AView_CandidateList.h"
#include "AApp.h"

const ANumber	kLeftMargin = 4;
const ANumber	kRightMargin = 4;

/*#717
const ANumber	kFileColumnWidth = 120;

#if IMPLEMENTATION_FOR_MACOSX
const AFloatNumber	kFontSize = 9.0;
#else
const AFloatNumber	kFontSize = 8.0;
#endif
*/

#pragma mark ===========================
#pragma mark [クラス]AView_CandidateList
#pragma mark ===========================
//情報ウインドウのメインView

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_CandidateList::AView_CandidateList( /* #199 AObjectArrayItem* inParent, AWindow& inWindow*/const AWindowID inWindowID, const AControlID inID ) 
	: AView(/* #199 inParent,inWindow*/inWindowID,inID), mIndex(kIndex_Invalid), mLineHeight(12), mCurrentHoverCursorIndex(kIndex_Invalid)
,mCurrentModeIndex(kIndex_Invalid), mCurrentTextViewControlID(kControlID_Invalid), mCurrentTextWindowID(kObjectID_Invalid)//#717
,mKeywordStartX(0), mTransparency(1.0)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 初期化はNVIDO_Init()へ移動
	
	//データ全削除
	SPI_DeleteAllData();
}

//デストラクタ
AView_CandidateList::~AView_CandidateList()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_CandidateList::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_CandidateList::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>


#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_CandidateList::EVTDO_DoDraw()
{
	//フレームrect取得
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//=========================描画データ等取得=========================
	//ファイル列幅取得
	ANumber fileColumnWidth = (frameRect.right-frameRect.left)/4;
	//フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	//番号表示幅取得
	AText	digit("0");
	ANumber	numColumnWidth = NVMC_GetDrawTextWidth(digit)*3 + kLeftMargin + kRightMargin;
	
	//=========================背景情報取得（このviewでは背景は描画しない）=========================
	//背景情報取得
	//サブウインドウ用背景色取得
	AColor	backgroundColor = kColor_White;//GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	AFloatNumber	backgroundAlpha = 1.0;
	//サブウインドウLocationTypeによって、それぞれの背景描画
	switch(GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()))
	{
	  case kSubWindowLocationType_Popup:
		{
			//ポップアップの指定透過率で背景消去
			//ポップアップ表示中のリサイズに対応するため、paintにする。(eraseだと描画した領域以外透明になってしまう）NVMC_EraseRect(frameRect);
			backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha();
			break;
		}
	  case kSubWindowLocationType_Floating:
		{
			//フローティングの指定透過率で背景消去
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			break;
		}
	  default:
		{
			//ポップアップ以外ならサブウインドウ背景色で消去
			break;
		}
	}
	//背景paint
	//NVMC_PaintRect(frameRect,backgroundColor,backgroundAlpha);
	
	//データ未設定ならここで終了
	if( mCurrentModeIndex == kIndex_Invalid )   return;
	
	//==================色取得==================
	
	//描画色設定
	AColor	letterColor = AColorWrapper::GetColorByHTMLFormatColor("303030");//#1316
	//#1316
	if( GetApp().NVI_IsDarkMode() == true )
	{
		letterColor = AColorWrapper::GetColorByHTMLFormatColor("F0F0F0");
	}
	//#1316 AColor	dropShadowColor = kColor_White;
	/*#1316
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	*/
    //#1316 NVMC_SetDropShadowColor(dropShadowColor);
	
	//=========================候補項目毎のループ=========================
	for( AIndex i = 0; i < mInfoTextArray.GetItemCount(); i++ )
	{
		//キーワード部分のスタイル
		ATextStyle	keywordTextStyle = kTextStyle_Bold;//kTextStyle_Normal;
		//左揃え表示するかどうかのフラグ（識別子以外（＝モード設定のキーワード）の場合は全て左揃えにする）
		ABool	justificationleft = false;//#147
		
		//項目の描画rect取得
		AImageRect	imagerect = {0};
		GetItemImageRect(i,imagerect);
		ALocalRect	localItemRect = {0};
		NVM_GetLocalRectFromImageRect(imagerect,localItemRect);
		
		//項目が描画領域に入っているかの判定
		if( NVMC_IsRectInDrawUpdateRegion(localItemRect) == false )   continue;
		
		/*#1316 スコープによる色分けはダークモードなどで見えづらくなるので、必要性も薄いので削除する。
		//=========================項目の背景描画=========================
		//項目の背景色取得
		AColor	bgcolor = kColor_White;//#725
		//優先順位によって背景色変更
		ABool	priorityColor = true;
		switch( mPriorityArray.Get(i) )
		{
		  case kCandidatePriority_Local:
			{
				bgcolor = kColor_Red;
				break;
			}
		  case kCandidatePriority_ParentKeywordWithHistory:
			{
				bgcolor = kColor_Blue;
				break;
			}
		  case kCandidatePriority_ParentKeyword:
			{
				bgcolor = kColor_Blue;
				break;
			}
		  case kCandidatePriority_History:
			{
				bgcolor = kColor_Yellow;
				break;
			}
		  default:
			{
				priorityColor = false;
				break;
			}
		}
		//項目の背景描画
		//白を設定α値で描画（ポップアップの場合は上でEraseRect()しているので、白透過となる）
		//NVMC_PaintRect(localItemRect,kColor_White,backgroundAlpha/2);
		//スコープごとの色で描画
		if( priorityColor == true )
		{
			NVMC_PaintRect(localItemRect,bgcolor,0.05);
		}
		*/
		
		//=========================選択描画=========================
		//選択描画
		/*選択描画は黄色でキーワードハイライトすることにする
		if( i == mIndex )
		{
			
			AColor	color = kColor_Blue;
			AColorWrapper::GetHighlightColor(color);
			AColor	color2 = AColorWrapper::ChangeHSV(color,0,0.3,1.0);
			NVMC_PaintRectWithVerticalGradient(localItemRect,color2,color,1.0,1.0);
		}
		*/
		//ホバー描画
		if( i == mCurrentHoverCursorIndex )
		{
			AColor	color = kColor_Blue;
			AColorWrapper::GetHighlightColor(color);
			NVMC_PaintRect(localItemRect,color,0.4);
		}
		
		//=========================項目描画データ取得=========================
		//InfoTextまたはKeywordTextのTextDrawData生成
		CTextDrawData	textDrawData(false);
		//const AText&	infotext = ((mInfoTextArray.GetTextConst(i).GetItemCount()>0)?(mInfoTextArray.GetTextConst(i)):(mKeywordTextArray.GetTextConst(i)));;
		//各種データ取得
		AScopeType	scopeType = mScopeTypeArray.Get(i);
		AText	keywordText;
		mKeywordTextArray.Get(i,keywordText);
		AText	originalInforText;
		mInfoTextArray.Get(i,originalInforText);
		//infotext設定
		AText	infotext;
		if( originalInforText.GetItemCount() > 0 )
		{
			//識別子にInfoTextが存在する場合
			if( scopeType == kScopeType_ModeKeyword )
			{
				//scope:モードキーワードの場合、keyword : infotextにする
				infotext.SetText(keywordText);
				infotext.AddCstring(" : ");
				infotext.AddText(originalInforText);
			}
			else
			{
				//infotextを設定
				infotext.SetText(originalInforText);
			}
		}
		else
		{
			//識別子にInfoTextが存在しない場合keywordを設定
			infotext.SetText(keywordText);
		}
		textDrawData.MakeTextDrawDataWithoutStyle(infotext,1,0,true,true,false,0,infotext.GetItemCount());
		//モード設定更新とスレッド実行とのすれ違いの場合にカテゴリindexがオーバーする可能性もあるので、念のため。
		AIndex	categoryIndex = mCategoryIndexArray.Get(i);
		if( categoryIndex < 0 || categoryIndex >= GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryCount() )
		{
			categoryIndex = 0;
		}
		//
		//カテゴリーカラー取得
		//（カテゴリ=-1はSPI_SetInfo()にて排除済み）
		AColor	categorycolor = letterColor;
		switch(mScopeTypeArray.Get(i))
		{
		  case kScopeType_Local:
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryLocalColor(categoryIndex,categorycolor);
				break;
			}
		  case kScopeType_Global:
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryColor(categoryIndex,categorycolor);
				break;
			}
		  case kScopeType_Import:
		  case kScopeType_Import_Near:
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryImportColor(categoryIndex,categorycolor);
				break;
			}
		  case kScopeType_ModeKeyword:
		  case kScopeType_ModeSystemHeader:
			{
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryColor(categoryIndex,categorycolor);//★colorslot対応
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		//カテゴリ色を暗くして彩度を上げた色を取得
		//AColor	categoryColorDrarken = AColorWrapper::ChangeHSV(categorycolor,0,3.0,0.5);
		
		//==================スコープに応じたデータ取得==================
		AText	filename;
		switch(mScopeTypeArray.Get(i))
		{
		  case kScopeType_Local:
			{
				//"ローカル"を表示
				filename.SetLocalizedText("Scope_Local");
				break;
			}
		  case kScopeType_Global:
			{
				//filename.SetLocalizedText("Scope_Global");
				//#717 mFileArray.GetObjectConst(i).GetFilename(filename);
				AText	path;
				mFilePathArray.Get(i,path);
				path.GetFilename(filename);
				break;
			}
		  case kScopeType_Import:
		  case kScopeType_Import_Near:
			{
				//#717 mFileArray.GetObjectConst(i).GetFilename(filename);
				AText	path;
				mFilePathArray.Get(i,path);
				path.GetFilename(filename);
				break;
			}
			//キーワード
		  default:
			{
				//モード設定更新とスレッド実行とのすれ違いの場合にカテゴリindexがオーバーする可能性もあるので、念のため。
				AIndex	categoryIndex = mCategoryIndexArray.Get(i);
				if( categoryIndex < 0 || categoryIndex >= GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryCount() )
				{
					categoryIndex = 0;
				}
				//
				//固定キーワードの場合、ファイル名の欄にはカテゴリー名を設定する
				GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetCategoryName(categoryIndex,filename);
				//左揃え表示
				justificationleft = true;
				break;
			}
		}
		//==================描画テキスト取得==================
		AIndex	idtextspos = 0;
		const AText&	idtext = mKeywordTextArray.GetTextConst(i);
		AItemCount	textlen = infotext.GetItemCount();
		//KeywordText==InfoTextかどうかの判定
		if( idtext.Compare(infotext) == true )
		{
			//InfoText存在しないので、キーワードのみを表示
			textDrawData.attrPos.Add(0);
			textDrawData.attrColor.Add(categorycolor);//categoryColorDrarken);
			textDrawData.attrStyle.Add(keywordTextStyle);
		}
		else
		{
			//InfoTextありなので、InfoTextを表示
			textDrawData.attrPos.Add(0);
			textDrawData.attrColor.Add(letterColor);//kColor_Black);
			textDrawData.attrStyle.Add(kTextStyle_Normal);
			for( AIndex pos = 0; pos < textlen; )
			{
				if( infotext.SkipTabSpace(pos,textlen) == false )   break;
				AText	token;
				AIndex	tokenspos = pos;
				infotext.GetToken(pos,token);//問題有り。この方法では、たとえばTeXの\で始まるキーワードに色づけできない
				if( token.Compare(idtext) == true )
				{
					idtextspos = tokenspos;
					//キーワード部分をカテゴリカラーで描画
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(tokenspos));
					textDrawData.attrColor.Add(categorycolor);//categoryColorDrarken);
					textDrawData.attrStyle.Add(keywordTextStyle);
					textDrawData.attrPos.Add(textDrawData.OriginalTextArray_UnicodeOffset.Get(pos));
					textDrawData.attrColor.Add(letterColor);//kColor_Black);
					textDrawData.attrStyle.Add(kTextStyle_Normal);
				}
			}
		}
		//=========================ファイル名描画=========================
		//ファイル名の欄に「他」を表示
		if( mMatchedCountArray.Get(i) > 1 )
		{
			AText	andsoon;
			andsoon.SetLocalizedText("CandidateList_AndSoOn");
			filename.AddCstring(" ");
			filename.AddText(andsoon);
		}
		//優先順位によってはファイル名の欄に親キーワード等を表示
		switch( mPriorityArray.Get(i) )
		{
		  case kCandidatePriority_ParentKeyword:
		  case kCandidatePriority_ParentKeywordWithHistory:
			{
				filename.SetLocalizedText("Letter_ParentKeyword_Matched");
				//
				filename.AddText(mParentKeywordArray.GetTextConst(i));
				AText	t;
				t.SetLocalizedText("CandidateList_ParentKeywordMatched");
				filename.AddText(t);
				break;
			}
		  case kCandidatePriority_History:
			{
				AText	t;
				t.SetLocalizedText("CandidateList_FoundInHistory");
				filename.AddText(t);
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		//ファイル名描画（1ポイント小さい文字で描画）
		ALocalRect	drawRect = localItemRect;
		drawRect.left = localItemRect.right - fileColumnWidth + kLeftMargin;
		drawRect.right = localItemRect.right - kRightMargin;
		drawRect.top += 1;
		NVMC_SetDefaultTextProperty(fontname,fontsize-1.0,letterColor,kTextStyle_Normal,true);
		NVMC_DrawText(drawRect,filename);
		
		//=========================分割線描画=========================
		//分割線
		ALocalPoint	spt = {0}, ept = {0};
		spt.x = localItemRect.right - fileColumnWidth;
		spt.y = drawRect.top;
		ept.x = localItemRect.right - fileColumnWidth;
		ept.y = drawRect.bottom;
		NVMC_DrawLine(spt,ept,kColor_LightGray,1.0,1.0);
		
		//=========================テキスト描画=========================
		//
		NVMC_SetDefaultTextProperty(fontname,fontsize,letterColor,kTextStyle_Normal,true);
		//描画領域計算
		drawRect = localItemRect;
		drawRect.left = localItemRect.left + numColumnWidth + kLeftMargin;
		drawRect.right = localItemRect.right - fileColumnWidth - kRightMargin;
		//
		mKeywordStartX = drawRect.left;
		//
		ALocalRect	textRect = drawRect;
		textRect.top += 1;
		if( justificationleft == false )//#147
		{
			//==================識別子用align（識別子部分の頭を合わせて中央付近に表示）==================
			//
			mKeywordStartX = drawRect.left + (drawRect.right-drawRect.left)/5;
			//
			AText	t;
			infotext.GetText(0,idtextspos,t);
			ANumber	tw = static_cast<ANumber>(NVMC_GetDrawTextWidth(t));
			textRect.left = mKeywordStartX - tw;
		}
		//選択項目のキーワード部分を黄色ハイライト
		if( i == mIndex )
		{
			//カラースロットの選択色不透明度を取得し、それに従ってハイライト濃度設定
			AFloatNumber	selectionOpacity = GetApp().SPI_GetModePrefDB(mCurrentModeIndex).GetModeData_Number(AModePrefDB::kSelectionOpacity);
			selectionOpacity /= 100;
			//
			ALocalRect	rect = textRect;
			rect.left = mKeywordStartX;
			rect.right = mKeywordStartX + NVMC_GetDrawTextWidth(keywordText);
			AFloatNumber alpha = 0.3*selectionOpacity;//#1316
			if( GetApp().NVI_IsDarkMode() == false )
			{
				//ダークモードではない場合のみ、アルファを+0.2する
				alpha += 0.2;
			}
			NVMC_PaintRect(rect,kColor_Yellow,alpha);
		}
		//テキスト描画
		NVMC_DrawText(textRect,drawRect,textDrawData);
		
		//=========================分割線描画=========================
		//分割線
		spt.x = localItemRect.left + numColumnWidth;
		spt.y = drawRect.top;
		ept.x = localItemRect.left + numColumnWidth;
		ept.y = drawRect.bottom;
		NVMC_DrawLine(spt,ept,kColor_LightGray,1.0,1.0);
		
		//==================番号描画==================
		drawRect = localItemRect;
		drawRect.left = localItemRect.left + kLeftMargin;
		drawRect.right = localItemRect.left + numColumnWidth - kRightMargin;
		drawRect.top += 1;
		NVMC_SetDefaultTextProperty(fontname,fontsize-1.0,letterColor,kTextStyle_Normal,true);
		AText	numtext;
		numtext.SetNumber(i+1);
		NVMC_DrawText(drawRect,numtext,kJustification_Right);
	}
	/*
	for( AIndex lineIndex = 0; lineIndex < mDrawData_LineStart.GetItemCount(); lineIndex++ )
	{
		AIndex	start 	= mDrawData_LineStart .Get(lineIndex);
		AItemCount	len = mDrawData_LineLength.Get(lineIndex);
		AText	text;
		mText.GetText(start,len,text);
		ALocalRect	rect;
		rect.left		= kLeftMargin;
		rect.top		= lineIndex*fontheight;
		rect.right		= viewwidth;
		rect.bottom		= lineIndex*fontheight + fontheight;
		//
		NVMC_DrawText(rect,text);
	}
	*/
}

//マウスボタン押下時のコールバック(AView用)
ABool	AView_CandidateList::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//TextWindowID, TextViewのControlIDがinvalidなら何もしない
	if( mCurrentTextWindowID == kObjectID_Invalid || mCurrentTextViewControlID == kControlID_Invalid )
	{
		return false;
	}
	//ウインドウが非存在なら何もしない
	if( GetApp().NVI_ExistWindow(mCurrentTextWindowID) == false )
	{
		return false;
	}
	//
	AImagePoint	imagept;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	AIndex	index = imagept.y / mLineHeight;
	if( index >= 0 && index < mInfoTextArray.GetItemCount() )
	{
		GetApp().SPI_GetTextWindowByID(mCurrentTextWindowID).SPI_SetCandidateIndex(mCurrentTextViewControlID,index);
	}
	return true;
}

//
void	AView_CandidateList::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
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

ABool	AView_CandidateList::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

//カーソル
ABool	AView_CandidateList::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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

void	AView_CandidateList::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

void	AView_CandidateList::ClearHoverCursor()
{
	if( mCurrentHoverCursorIndex != kIndex_Invalid )
	{
		mCurrentHoverCursorIndex = kIndex_Invalid;
		NVI_Refresh();
	}
}

/**
項目のimageRectを取得
*/
void	AView_CandidateList::GetItemImageRect( const AIndex inIndex, AImageRect& outImageRect ) const
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	outImageRect.left		= 0;
	outImageRect.top		= inIndex*mLineHeight;
	outImageRect.right		= frameRect.right - frameRect.left;
	outImageRect.bottom		= (inIndex+1)*mLineHeight;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

void	AView_CandidateList::SPI_SetInfo( const AModeIndex inModeIndex,
		const AWindowID inTextWindowID, const AControlID inTextViewControlID,
		const ATextArray& inIdTextArray, const ATextArray& inInfoTextArray,
		const AArray<AIndex>& inCategoryIndexArray, const AArray<AScopeType>& inScopeTypeArray,
		const ATextArray& inFilePathArray, const AArray<ACandidatePriority>& inPriorityArray,
		const ATextArray& inParentKeywordArray,
		const AArray<AItemCount>& inMatchedCountArray )//#717
{
	//inCandidateArray.Implode(kUChar_Space,mText);
	
	NVMC_SetRedirectTextInput(inTextWindowID,inTextViewControlID);
	
	mCurrentModeIndex = inModeIndex;
	mCurrentTextWindowID = inTextWindowID;
	mCurrentTextViewControlID = inTextViewControlID;
	
	mInfoTextArray.DeleteAll();
	mKeywordTextArray.DeleteAll();
	mScopeTypeArray.DeleteAll();
	mCategoryIndexArray.DeleteAll();
	//#717 mFileArray.DeleteAll();
	mFilePathArray.DeleteAll();//#717
	mPriorityArray.DeleteAll();//#717
	mMatchedCountArray.DeleteAll();
	mParentKeywordArray.DeleteAll();
	mInfoTextIsEmpty = true;
	for( AIndex i = 0; i < inInfoTextArray.GetItemCount(); i++ )
	{
		//カテゴリ=-1の場合(ImportIdentifier(#include等)）は追加しない
		if( inCategoryIndexArray.Get(i) == kIndex_Invalid )
		{
			continue;
		}
		//info text取得
		AText	infoText;
		inInfoTextArray.Get(i,infoText);
		infoText.ReplaceChar(kUChar_CR,kUChar_Space);
		infoText.ReplaceChar(kUChar_Tab,kUChar_Space);
		mInfoTextArray.Add(infoText);
		if( infoText.GetItemCount() > 0 )   mInfoTextIsEmpty = false;
		//keyword text取得
		AText	keywordText;
		inIdTextArray.Get(i,keywordText);
		keywordText.ReplaceChar(kUChar_CR,kUChar_Space);
		keywordText.ReplaceChar(kUChar_Tab,kUChar_Space);
		mKeywordTextArray.Add(keywordText);
		//scope, category, file path取得
		mScopeTypeArray.Add(inScopeTypeArray.Get(i));
		mCategoryIndexArray.Add(inCategoryIndexArray.Get(i));
		//#717 mFileArray.GetObject(mFileArray.AddNewObject()).CopyFrom(inFileArray.GetObjectConst(i));
		mFilePathArray.Add(inFilePathArray.GetTextConst(i));//#717
		mPriorityArray.Add(inPriorityArray.Get(i));//#717
		mMatchedCountArray.Add(inMatchedCountArray.Get(i));
		mParentKeywordArray.Add(inParentKeywordArray.GetTextConst(i));//#717
	}
	mIndex = kIndex_Invalid;//#717
	CalcDrawData();
	NVI_Refresh();
}

/**
データ全削除
*/
void	AView_CandidateList::SPI_DeleteAllData()
{
	//★スレッドでの検索中断必要？
	
	mIndex = kIndex_Invalid;
	mCurrentModeIndex = kIndex_Invalid;
	mCurrentTextWindowID = kObjectID_Invalid;
	mCurrentTextViewControlID = kControlID_Invalid;
	
	mInfoTextArray.DeleteAll();
	mKeywordTextArray.DeleteAll();
	mScopeTypeArray.DeleteAll();
	mCategoryIndexArray.DeleteAll();
	mFilePathArray.DeleteAll();//#717
	mPriorityArray.DeleteAll();//#717
	mMatchedCountArray.DeleteAll();
	mParentKeywordArray.DeleteAll();//#717
	mInfoTextIsEmpty = true;
	
	mCurrentHoverCursorIndex = kIndex_Invalid;
	
	CalcDrawData();
	NVI_Refresh();
}

//
void	AView_CandidateList::SPI_SetIndex( const AIndex inIndex )
{
	mIndex = inIndex;
	AdjustScroll();
	NVI_Refresh();
	NVMC_FlushControl();//win
	//ステータスバー更新
	GetApp().SPI_GetCandidateListWindow(NVM_GetWindow().GetObjectID()).SPI_UpdateStatusBar();
}

void	AView_CandidateList::AdjustScroll()
{
	ALocalRect	frameRect;
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imagerect;
	NVM_GetImageRectFromLocalRect(frameRect,imagerect);
	AIndex	startIndex = imagerect.top/mLineHeight;
	AIndex	endIndex = imagerect.bottom/mLineHeight;
	if( mIndex != kIndex_Invalid )
	{
		if( mIndex <= startIndex || endIndex <= mIndex )
		{
			AImagePoint	pt = {0,0};
			pt.y = mIndex*mLineHeight - (frameRect.bottom - frameRect.top)/2;
			if( pt.y < 0 )   pt.y = 0;
			NVI_ScrollTo(pt);
		}
	}
}

void	AView_CandidateList::CalcDrawData()
{
	/*mDrawData_LineStart.DeleteAll();
	mDrawData_LineLength.DeleteAll();
	//
	AText	fixwidthfontname;
	AFontWrapper::GetFixWidthFontName(fixwidthfontname);
	CTextDrawData	textDrawData;
	textDrawData.MakeTextDrawDataWithoutStyle(mText,
			4,
			0,true,true,false,0,mText.GetItemCount());
	NVMC_SetDefaultTextProperty(fixwidthfontname,kFontSize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontheight, fontascent;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	ANumber	viewwidth = NVI_GetViewWidth();
	//行折り返し計算
	AIndex	startOffset = 0;
	ANumber	lineleftmargin = kLeftMargin;
	AItemCount	lineCount = 0;
	while( textDrawData.UTF16DrawText.GetItemCount()/sizeof(AUTF16Char) - startOffset > 0 )
	{
		AItemCount	endOffset = CWindowImp::GetLineBreak(textDrawData,startOffset,fixwidthfontname,kFontSize,true,viewwidth-lineleftmargin);
		if( endOffset <= startOffset )   endOffset = startOffset+1;
		mDrawData_LineStart .Add(textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(startOffset));
		mDrawData_LineLength.Add(textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(endOffset)-textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(startOffset));
		startOffset = endOffset;
		lineCount++;
	}*/
	//フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	
	ANumber	viewwidth = NVI_GetViewWidth();
	NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontheight, fontascent;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	fontheight += 1;
	mLineHeight = fontheight;
	//ImageSize設定
	NVM_SetImageSize(viewwidth,mInfoTextArray.GetItemCount()*fontheight+5);
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(0,fontheight,0,rect.bottom-rect.top-fontheight);
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

void	AView_CandidateList::SPI_UpdateDrawProperty()
{
	CalcDrawData();
	NVI_Refresh();
}



