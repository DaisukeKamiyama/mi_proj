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

AView_CallGraf
#723

*/

#include "stdafx.h"

#include "AView_CallGraf.h"
#include "AView_CallGrafHeader.h"
#include "AApp.h"

//余白等サイズ
const ANumber	kLeftMargin_Box = 5;
const ANumber	kRightMargin_Box = 3;
const ANumber	kTopMargin_Box = 3;
const ANumber	kBottomMargin_Box = 3;
const ANumber	kCallerCalleeLineWidth = 8;
const ANumber	kTopPadding_Header = 3;
const ANumber	kBottomPadding_Header = 0;
const ANumber	kTopPadding_Content = 3;
const ANumber	kBottomPadding_Content = 0;

//Import File Progress表示エリア高さ
const ANumber	kProgressTextAreaHeight = 16;

#pragma mark ===========================
#pragma mark [クラス]AView_CallGraf
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ
/**
コンストラクタ
*/
AView_CallGraf::AView_CallGraf( const AWindowID inWindowID, const AControlID inID ) 
: AView(inWindowID,inID), mCurrentSelection_LevelIndex(kIndex_Invalid), mCurrentSelection_ItemIndex(kIndex_Invalid),
		mCurrentHover_LevelIndex(kIndex_Invalid), mCurrentHover_ItemIndex(kIndex_Invalid),
		mAllFilesSearched(false), mCurrentDisplayStartLevel(1),
		mContentFontHeight(9), mHeaderFontHeight(9), mBoxWidth(100), mContentHeight(30), mHeaderHeight(10), mBoxHeight(40)
,mWordsListFinderThreadID(kObjectID_Invalid),mIsFinding(false)
{
	NVMC_SetOffscreenMode(true);
	
	//level0 array追加
	mCallerArray_FilePath.AddNewObject();
	mCallerArray_FunctionName.AddNewObject();
	mCallerArray_ClassName.AddNewObject();
	mCallerArray_StartIndex.AddNewObject();
	mCallerArray_EndIndex.AddNewObject();
	mCallerArray_ModeIndex.AddNewObject();
	mCallerArray_CategoryIndex.AddNewObject();
	mCallerArray_EdittedFlag.AddNewObject();
	mCallerArray_ClickedFlag.AddNewObject();
	mCallerArray_CallerFuncId.AddNewObject();
	//arrayの現在選択index（コール先item）
	mCallerArray_CalleeItemIndex.Add(kIndex_Invalid);
	//arrayの現在単語
	mCurrentWordArray.Add(GetEmptyText());
	
	//ワードリスト検索スレッド生成・起動
	AThread_WordsListFinderFactory	factory(this);
	mWordsListFinderThreadID = GetApp().NVI_CreateThread(factory);
	GetWordsListFinderThread().SetWordsListFinderType(kWordsListFinderType_ForCallGraf);
	GetWordsListFinderThread().NVI_Run(true);
	
	//全データ削除
	SPI_DeleteAllData();
	
	//履歴データを読みこみ
	AItemCount	itemCount = GetApp().NVI_GetAppPrefDB().GetItemCount_TextArray(AAppPrefDB::kCallGrafHistory_FilePath);
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AText	filepath, classname, functionname;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kCallGrafHistory_FilePath,i,filepath);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kCallGrafHistory_ClassName,i,classname);
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kCallGrafHistory_FunctionName,i,functionname);
		AIndex	startIndex = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kCallGrafHistory_StartIndex,i);
		AIndex	endIndex = GetApp().NVI_GetAppPrefDB().GetData_NumberArray(AAppPrefDB::kCallGrafHistory_EndIndex,i);
		AText	modename;
		GetApp().NVI_GetAppPrefDB().GetData_TextArray(AAppPrefDB::kCallGrafHistory_ModeName,i,modename);
		AIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(modename);
		if( modeIndex == kIndex_Invalid )   modeIndex = kStandardModeIndex;
		//ABool	edittedFlag = GetApp().NVI_GetAppPrefDB().GetData_BoolArray(AAppPrefDB::kCallGrafHistory_EdittedFlag,i);
		//funcID取得
		AText	funcIdText;
		AView_CallGraf::SPI_GetFuncIdText(filepath,classname,functionname,funcIdText);
		//履歴追加
		mCallerArray_FilePath.GetObject(0).Add(filepath);
		mCallerArray_FunctionName.GetObject(0).Add(functionname);
		mCallerArray_ClassName.GetObject(0).Add(classname);
		mCallerArray_StartIndex.GetObject(0).Add(startIndex);
		mCallerArray_EndIndex.GetObject(0).Add(endIndex);
		mCallerArray_ModeIndex.GetObject(0).Add(modeIndex);
		mCallerArray_CategoryIndex.GetObject(0).Add(kIndex_Invalid);//category indexは変わっている可能性があるので、記憶も復帰もしない
		mCallerArray_EdittedFlag.GetObject(0).Add(false);//edittedFlag);
		mCallerArray_ClickedFlag.GetObject(0).Add(true);
		mCallerArray_CallerFuncId.GetObject(0).Add(GetEmptyText());
		mLevel0FuncIdTextArray.Add(funcIdText);
	}
	//Image size更新
	UpdateImageSize();
}

/**
デストラクタ
*/
AView_CallGraf::~AView_CallGraf()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_CallGraf::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_CallGraf::NVIDO_DoDeleted()
{
	//ワードリスト検索スレッド終了、削除
	if( mWordsListFinderThreadID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteThread(mWordsListFinderThreadID);
		mWordsListFinderThreadID = kObjectID_Invalid;
	}
	//履歴の記憶
	GetApp().NVI_GetAppPrefDB().DeleteAllRows_Table(AAppPrefDB::kCallGrafHistory_FilePath);
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == false )
	{
		for( AIndex i = 0; i < mCallerArray_FilePath.GetObjectConst(0).GetItemCount(); i++ )
		{
			AText	filepath, classname, functionname;
			mCallerArray_FilePath.GetObjectConst(0).Get(i,filepath);
			mCallerArray_ClassName.GetObjectConst(0).Get(i,classname);
			mCallerArray_FunctionName.GetObjectConst(0).Get(i,functionname);
			ANumber	startIndex = mCallerArray_StartIndex.GetObjectConst(0).Get(i);
			ANumber	endIndex = mCallerArray_EndIndex.GetObjectConst(0).Get(i);
			AIndex	modeIndex = mCallerArray_ModeIndex.GetObjectConst(0).Get(i);
			AText	modename;
			GetApp().SPI_GetModePrefDB(modeIndex,false).GetModeRawName(modename);
			ABool	edittedFlag = mCallerArray_EdittedFlag.GetObjectConst(0).Get(i);
			//
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kCallGrafHistory_FilePath,filepath);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kCallGrafHistory_ClassName,classname);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kCallGrafHistory_FunctionName,functionname);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kCallGrafHistory_StartIndex,startIndex);
			GetApp().NVI_GetAppPrefDB().Add_NumberArray(AAppPrefDB::kCallGrafHistory_EndIndex,endIndex);
			GetApp().NVI_GetAppPrefDB().Add_TextArray(AAppPrefDB::kCallGrafHistory_ModeName,modename);
			GetApp().NVI_GetAppPrefDB().Add_BoolArray(AAppPrefDB::kCallGrafHistory_EdittedFlag,edittedFlag);
		}
	}
}

/**
ワードリスト検索スレッド取得
*/
AThread_WordsListFinder&	AView_CallGraf::GetWordsListFinderThread()
{
	return (dynamic_cast<AThread_WordsListFinder&>(GetApp().NVI_GetThreadByID(mWordsListFinderThreadID)));
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
描画要求時のコールバック(AView用)
*/
void	AView_CallGraf::EVTDO_DoDraw()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	
	//描画データ更新
	UpdateDrawProperty();
	
	//=========================描画データ取得=========================
	//フォント取得
	//内容部分フォント
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//ヘッダ部分フォント
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	
	//=========================背景情報取得（このviewでは背景は描画しない）=========================
	//背景情報取得
	AColor	backgroundColor = GetApp().SPI_GetSubWindowBackgroundColor(NVM_GetWindow().NVI_IsWindowActive());
	AFloatNumber	backgroundAlpha = 1.0;
	switch(GetApp().SPI_GetSubWindowLocationType(NVM_GetWindow().GetObjectID()))
	{
	  case kSubWindowLocationType_Floating:
		{
			backgroundAlpha = GetApp().SPI_GetFloatingWindowAlpha();
			break;
		}
	  case kSubWindowLocationType_Popup:
		{
			backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//
	//NVMC_PaintRect(frameRect,backgroundColor,backgroundAlpha);
	
	//=========================各項目描画=========================
	//各カラム毎ループ
	for( AIndex columnIndex = 0; columnIndex <= 1; columnIndex++ )
	{
		//カラムからlevel取得
		AIndex	level = GetLevelFromColumn(columnIndex);
		
		//対象levelのデータ未生成なら何もしない
		if( level >= mCallerArray_FilePath.GetItemCount() )   continue;
		
		//各項目毎ループ
		for( AIndex index = 0; index < mCallerArray_FilePath.GetObjectConst(level).GetItemCount(); index++ )
		{
			//box取得
			AImageRect	box = {0};
			GetBoxRect(columnIndex,index,box);
			ALocalRect	localBox = {0};
			NVM_GetLocalRectFromImageRect(box,localBox);
			
			//Update領域判定（１つ先のboxまで描画する。（線描画のため）
			ALocalRect	updateBox = localBox;
			updateBox.top -= (localBox.bottom - localBox.top)*2;
			ABool	insideUpdateRegion = NVMC_IsRectInDrawUpdateRegion(updateBox);
			
			//update領域内の場合の描画処理
			if( insideUpdateRegion == true )
			{
				//項目データ取得
				AText	filepath;
				mCallerArray_FilePath.GetObjectConst(level).Get(index,filepath);
				AText	classname;
				mCallerArray_ClassName.GetObjectConst(level).Get(index,classname);
				AText	funcname;
				mCallerArray_FunctionName.GetObjectConst(level).Get(index,funcname);
				AIndex	modeIndex = mCallerArray_ModeIndex.GetObjectConst(level).Get(index);
				AIndex	categoryIndex = mCallerArray_CategoryIndex.GetObjectConst(level).Get(index);
				//FuncId取得
				AText	funcIdText;
				SPI_GetFuncIdText(level,index,funcIdText);
				
				//項目描画
				DrawItem(localBox,modeIndex,categoryIndex,filepath,classname,funcname,
						 //(level == mCurrentSelection_LevelIndex && index == mCurrentSelection_ItemIndex),
						 (mCallerArray_CalleeItemIndex.Get(level) == index ),//選択項目かどうか
							(level == mCurrentHover_LevelIndex && index == mCurrentHover_ItemIndex),//ホバー
							//(mVisitedFuncIdTextArray.Find(funcIdText) != kIndex_Invalid),
							(mCallerArray_ClickedFlag.GetObjectConst(level).Get(index)==true&&level!=0),//訪問済みフラグ
							mCallerArray_EdittedFlag.GetObjectConst(level).Get(index),//編集有りフラグ
							backgroundAlpha);
				
				//==================キーワード情報によるジャンプ線==================
				//Id infoクリックによる移動の場合に、移動先と移動元の関数を線で結ぶ（レベル0内）
				if( level == 0 && index+1 < mCallerArray_FilePath.GetObjectConst(level).GetItemCount() )
				{
					AText	nextItemFuncId;
					SPI_GetFuncIdText(level,index+1,nextItemFuncId);
					if( mCallerArray_CallerFuncId.GetObjectConst(level).Compare(index,nextItemFuncId) == true )
					{
						ALocalPoint	spt = {0}, ept = {0};
						spt.x = (localBox.left+localBox.right)/2 -3;
						spt.y = localBox.bottom;
						ept = spt;
						ept.y += 5;
						NVMC_DrawLine(spt,ept,kColor_Gray20Percent,0.7,0.0,3.0);
					}
				}
			}
			//Calleeへ最低１本の線を表示するためにindex==0の項目はupdate領域範囲外でも常に表示するようにする
			if( insideUpdateRegion == true || index == 0 )
			{
				//==================Caller->Callee線描画（カラムindexが0のときのみ）==================
				if( columnIndex == 0 )
				{
					AColor	lineColor = kColor_Gray70Percent;
					AFloatNumber	lineWidth = 1.0;
					AFloatNumber	lineAlpha = 1.0;
					/*
					if( mCallerArray_CalleeItemIndex.Get(level) == index )
					{
						lineColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor();
						lineWidth = 2.0;
						lineAlpha = 1.0;
					}
					*/
					//呼ばれ先box取得
					AImageRect	calleeBox = {0};
					GetBoxRect(columnIndex+1,mCallerArray_CalleeItemIndex.Get(GetLevelFromColumn(columnIndex+1)),calleeBox);
					ALocalRect	localCalleeBox = {0};
					NVM_GetLocalRectFromImageRect(calleeBox,localCalleeBox);
					//線描画
					ALocalPoint	spt = {0}, ept = {0};
					spt.x = localBox.right;
					spt.y = (localBox.top+localBox.bottom)/2;
					ept.x = localCalleeBox.left;
					ept.y = (localCalleeBox.top+localCalleeBox.bottom)/2;
					NVMC_DrawCurvedLine(spt,ept,lineColor,lineAlpha,0.0,lineWidth);
				}
			}
		}
	}
	//==================選択中の場合、選択項目と呼ばれ先を青線で描画==================
	//綺麗な青で描画するために、他の線を全て書いた後に、α=1.0で描画する
	if( mCurrentSelection_LevelIndex > 0 )
	{
		AIndex	columnIndex = 0;
		AIndex	level = GetLevelFromColumn(columnIndex);
		if( mCallerArray_CalleeItemIndex.GetItemCount() > level )
		{
			AIndex	index = mCallerArray_CalleeItemIndex.Get(level);
			if( index != kIndex_Invalid )
			{
				//==================選択項目と呼ばれ先を青線で描画==================
				//呼び元box取得
				AImageRect	box = {0};
				GetBoxRect(columnIndex,index,box);
				ALocalRect	localBox = {0};
				NVM_GetLocalRectFromImageRect(box,localBox);
				//呼ばれ先box取得
				AImageRect	calleeBox = {0};
				GetBoxRect(columnIndex+1,mCallerArray_CalleeItemIndex.Get(GetLevelFromColumn(columnIndex+1)),calleeBox);
				ALocalRect	localCalleeBox = {0};
				NVM_GetLocalRectFromImageRect(calleeBox,localCalleeBox);
				//選択色取得
				AColor	selColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
				//線描画
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = localBox.right;
				spt.y = (localBox.top+localBox.bottom)/2;
				ept.x = localCalleeBox.left;
				ept.y = (localCalleeBox.top+localCalleeBox.bottom)/2;
				NVMC_DrawCurvedLine(spt,ept,selColor,1.0,0.0,2.0);
				
				//==================選択項目の左に線を描画==================
				spt.x = frameRect.left;
				spt.y = (localBox.top+localBox.bottom)/2;
				ept.x = localBox.left;
				ept.y = spt.y;
				NVMC_DrawCurvedLine(spt,ept,kColor_Gray70Percent,1.0,0.0,2.0);
			}
		}
	}
	//==================右列選択項目の右に線を描画==================
	{
		AIndex	columnIndex = 1;
		AIndex	level = GetLevelFromColumn(columnIndex);
		if( level > 0 && mCallerArray_CalleeItemIndex.GetItemCount() > level )
		{
			AIndex	index = mCallerArray_CalleeItemIndex.Get(level);
			if( index != kIndex_Invalid )
			{
				//選択色取得
				AColor	selColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
				//呼び元box取得
				AImageRect	box = {0};
				GetBoxRect(columnIndex,index,box);
				ALocalRect	localBox = {0};
				NVM_GetLocalRectFromImageRect(box,localBox);
				//線描画
				ALocalPoint	spt = {0}, ept = {0};
				spt.x = localBox.right;
				spt.y = (localBox.top+localBox.bottom)/2;
				ept.x = frameRect.right;
				ept.y = spt.y;
				NVMC_DrawCurvedLine(spt,ept,selColor,1.0,0.0,2.0);
			}
		}
	}
}

/**
カラムindexからlevel idnex取得
*/
AIndex	AView_CallGraf::GetLevelFromColumn( const AIndex inColumnIndex ) const
{
	return (mCurrentDisplayStartLevel - inColumnIndex);
}

/**
*/
void	AView_CallGraf::UpdateDrawProperty()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//サブウインドウ用フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	//フォントの高さ等取得
	NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(mContentFontHeight,fontascent);
	//ヘッダ部分フォント
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//ヘッダ部分フォント高さ取得
	NVMC_SetDefaultTextProperty(headerfontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	headerfontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(mHeaderFontHeight,headerfontascent);
	
	//ヘッダ・コンテント部分の高さ
	mHeaderHeight = kTopPadding_Header + mHeaderFontHeight + kBottomPadding_Header;
	mContentHeight = kTopPadding_Content + mContentFontHeight*2 + kBottomPadding_Content;
	
	//boxの幅・高さ
	mBoxWidth = (frameRect.right - frameRect.left - (kLeftMargin_Box + kRightMargin_Box + kCallerCalleeLineWidth)) /2;
	mBoxHeight = mHeaderHeight + mContentHeight;
}

/**
項目描画
*/
void	AView_CallGraf::DrawItem( const ALocalRect& inLocalBoxRect, 
		const AIndex inModeIndex, const AIndex inCategoryIndex,
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName,
		const ABool inCurrent, const ABool inHover, const ABool inVisited, const ABool inEdited, const AFloatNumber inAlpha ) 
{
	//サブウインドウ用フォント取得
	AText	fontname;
	AFloatNumber	fontsize = 9.0;
	GetApp().SPI_GetSubWindowsFont(fontname,fontsize);
	fontsize -= 0.5;
	//フォントの高さ等取得
	NVMC_SetDefaultTextProperty(fontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	fontheight = 9, fontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(fontheight,fontascent);
	//ヘッダ部分フォント
	AText	headerfontname;
	AFontWrapper::GetDialogDefaultFontName(headerfontname);
	//ヘッダ部分フォント高さ取得
	NVMC_SetDefaultTextProperty(headerfontname,fontsize,kColor_Black,kTextStyle_Normal,true);
	ANumber	headerfontheight = 9, headerfontascent = 7;
	NVMC_GetMetricsForDefaultTextProperty(headerfontheight,headerfontascent);
	
	//==================色取得==================
	
	//描画色設定
	AColor	letterColor = kColor_Black;
	AColor	dropShadowColor = kColor_White;
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	GetApp().SPI_GetSubWindowBoxColor(NVM_GetWindow().GetObjectID(),letterColor,dropShadowColor,boxBaseColor1,boxBaseColor2,boxBaseColor3);
	NVMC_SetDropShadowColor(dropShadowColor);
	
	//カテゴリ色取得
	AColor	categoryColor = kColor_Black;//#1316 ★機能復活するならダークモード対応必要GetApp().SPI_GetSubWindowsBoxHeaderColor();//背景黒の場合も含めた色設計が難しいので、固定色 kColor_Black;
	/*
	if( inCategoryIndex != kIndex_Invalid )
	{
		GetApp().SPI_GetModePrefDB(inModeIndex).GetCategoryColor(inCategoryIndex,categoryColor);
	}
	*/
	
	//ハイライト色取得
	AColor	highlightColor = kColor_Blue;
	AColorWrapper::GetHighlightColor(highlightColor);
	
	//===================全体 box描画===================
	
	//content部分色取得
	AColor	contentColor_start = kColor_White;
	AColor	contentColor_end = kColor_Gray95Percent;
	//選択項目ならハイライト色で描画
	/*
	if( inCurrent == true )
	{
		//アクティブなサブウインドウかどうかで選択表示色を変える
		if( NVM_GetWindow().GetObjectID() == GetApp().SPI_GetActiveSubWindowForItemSelector() )
		{
			contentColor_end = highlightColor;
		}
		else
		{
			contentColor_end = kColor_Gray80Percent;
		}
	}
	*/
	//box描画
	NVMC_PaintRoundedRect(inLocalBoxRect,
						  //contentColor_start,contentColor_end,
						  boxBaseColor1,boxBaseColor3,
						  kGradientType_Vertical,inAlpha,inAlpha,
						  5,true,true,true,true);
	
	//==================ホバー時描画==================
	if( inHover == true )
	{
		/*
		NVMC_PaintRoundedRect(inLocalBoxRect,highlightColor,highlightColor,kGradientType_None,0.3,0.3,
							  5,true,true,true,true);
							  */
		NVMC_FrameRoundedRect(inLocalBoxRect,kColor_Blue,0.3,5,true,true,true,true,true,true,true,true,1.0);
	}
	//===================header box描画===================
	
	//header box rect取得
	ALocalRect	headerRect = inLocalBoxRect;
	headerRect.bottom = inLocalBoxRect.top + mHeaderHeight;
	
	//編集ありならヘッダー色を変えて表示
	AFloatNumber	headerAlpha = 0.15;
	if( inEdited == true )
	{
		//categoryColor = kColor_Red;
		headerAlpha = 0.3;
	}
	//header部分色取得
	AColor	headerColor_start = AColorWrapper::ChangeHSV(categoryColor,0,0.8,1.0);
	AColor	headerColor_end = AColorWrapper::ChangeHSV(categoryColor,0,1.2,1.0);
	//訪問済みの場合の色取得
	if( inVisited == true )
	{
		headerColor_start = AColorWrapper::ChangeHSV(categoryColor,0,0.2,0.5);
		headerColor_end = AColorWrapper::ChangeHSV(categoryColor,0,0.4,0.5);
	}
	
	//box描画
	NVMC_PaintRoundedRect(headerRect,headerColor_start,headerColor_end,kGradientType_Vertical,headerAlpha,headerAlpha,
						  5,true,true,true,true);
	
	//フォント設定
	NVMC_SetDefaultTextProperty(headerfontname,fontsize,letterColor,kTextStyle_Normal,true);
	//ファイル名描画
	ALocalRect	fileNameRect = headerRect;
	fileNameRect.left += 5;
	fileNameRect.right -= 1;
	fileNameRect.top += 1;
	AText	filename;
	inFilePath.GetFilename(filename);
	NVMC_DrawText(fileNameRect,filename,letterColor,kTextStyle_Bold|kTextStyle_DropShadow,kJustification_Left);
	
	//===================content 描画===================
	
	//content部分rect取得
	ALocalRect	contentRect = inLocalBoxRect;
	contentRect.top = inLocalBoxRect.top + mHeaderHeight;
	
	//フォント設定
	NVMC_SetDefaultTextProperty(fontname,fontsize,letterColor,kTextStyle_Normal,true);
	
	//class名描画
	if( inClassName.GetItemCount() > 0 )
	{
		ALocalRect	classNameRect = contentRect;
		classNameRect.left += 5;
		classNameRect.right -= 1;
		classNameRect.top += 0;
		AText	classname;
		classname.SetText(inClassName);
		AText	t;
		t.SetLocalizedText("Letter_ParentKeyword");
		classname.InsertText(0,t);
		NVMC_DrawText(classNameRect,classname,letterColor,kTextStyle_DropShadow);
	}
	
	//カテゴリ色を暗くして彩度を上げた色を取得
	AColor	categoryColorDraken = kColor_Black;//#1316 ★機能復活するならダークモード対応必要GetApp().SPI_GetSubWindowBoxFunctionNameColor();//背景黒の場合も含めた色設計が難しいので、固定色 AColorWrapper::ChangeHSV(categoryColor,0,3.0,0.3);
	//func名描画
	ALocalRect	funcNameRect = contentRect;
	funcNameRect.left += 8;
	funcNameRect.top += mContentFontHeight;
	AText	funcname;
	funcname.SetText(inFunctionName);
	NVMC_DrawText(funcNameRect,funcname,categoryColorDraken,kTextStyle_Bold);
	
	//==================フレーム描画（選択項目は選択色で囲む）==================
	if( inCurrent == true )
	{
		AColor	selColor = GetApp().SPI_GetSubWindowRoundedRectBoxSelectionColor(NVM_GetWindow().GetObjectID());
		NVMC_FrameRoundedRect(inLocalBoxRect,selColor,1.0,5,true,true,true,true,true,true,true,true,1.0);
	}
	else
	{
		NVMC_FrameRoundedRect(inLocalBoxRect,kColor_Gray50Percent,1.0,5,true,true,true,true);
	}
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_CallGraf::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	AImagePoint	imagept = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	
	//クリックしたlevel, itemを計算
	AIndex	level = kIndex_Invalid;
	AIndex	itemIndex = kIndex_Invalid;
	FindBoxIndexByPoint(imagept,level,itemIndex);
	
	//項目を開く
	SPI_OpenItem(level,itemIndex,inModifierKeys);
	
	return true;
}

/**
項目を開く
*/
void	AView_CallGraf::SPI_OpenItem( const AIndex inLevel, const AIndex inItemIndex, const AModifierKeys inModifierKeys )
{
	AIndex	level = inLevel;
	AIndex	itemIndex = inItemIndex;
	
	//項目クリック
	if( level != kIndex_Invalid && itemIndex != kIndex_Invalid )
	{
		//クリック済みフラグON
		mCallerArray_ClickedFlag.GetObject(level).Set(itemIndex,true);
		
		if( level == 0 )//&& itemIndex == 0 )
		{
			//------------------レベル0の場合は、最新項目へ順序入れ替え------------------
			
			//選択解除
			mCurrentSelection_LevelIndex = kIndex_Invalid;
			mCurrentSelection_ItemIndex = kIndex_Invalid;
			
			if( itemIndex != 0 )
			{
				//最新項目へ移動
				mCallerArray_FilePath.GetObject(0).MoveObject(itemIndex,0);
				mCallerArray_FunctionName.GetObject(0).MoveObject(itemIndex,0);
				mCallerArray_ClassName.GetObject(0).MoveObject(itemIndex,0);
				mCallerArray_StartIndex.GetObject(0).Move(itemIndex,0);
				mCallerArray_EndIndex.GetObject(0).Move(itemIndex,0);
				mCallerArray_ModeIndex.GetObject(0).Move(itemIndex,0);
				mCallerArray_CategoryIndex.GetObject(0).Move(itemIndex,0);
				mCallerArray_EdittedFlag.GetObject(0).Move(itemIndex,0);
				mCallerArray_ClickedFlag.GetObject(0).Move(itemIndex,0);
				mCallerArray_CallerFuncId.GetObject(0).MoveObject(itemIndex,0);
				//
				mLevel0FuncIdTextArray.MoveObject(itemIndex,0);
				//(0,0)にスクロール
				AImagePoint	originpt = {0,0};
				NVI_ScrollTo(originpt);
				//
				itemIndex = 0;
			}
		}
		else
		{
			//------------------レベル0以外の場合は、選択項目設定------------------
			mCurrentSelection_LevelIndex = level;
			mCurrentSelection_ItemIndex = itemIndex;
			
			//アクティブなサブウインドウとして設定（cmd+option+↑↓の対象となる）
			GetApp().SPI_SetActiveSubWindowForItemSelector(NVM_GetWindow().GetObjectID());
		}
		
		//各level毎の選択を記憶
		mCallerArray_CalleeItemIndex.Set(level,itemIndex);
		
		//クリック項目のデータを取得
		AText	filepath;
		mCallerArray_FilePath.GetObjectConst(level).Get(itemIndex,filepath);
		AText	classname;
		mCallerArray_ClassName.GetObjectConst(level).Get(itemIndex,classname);
		AText	funcname;
		mCallerArray_FunctionName.GetObjectConst(level).Get(itemIndex,funcname);
		AIndex	spos = mCallerArray_StartIndex.GetObjectConst(level).Get(itemIndex);
		AIndex	epos = mCallerArray_EndIndex.GetObjectConst(level).Get(itemIndex);
		AIndex	modeIndex = mCallerArray_ModeIndex.GetObjectConst(level).Get(itemIndex);
		//AIndex	categoryIndex = mCallerArray_CategoryIndex.GetObjectConst(level).Get(itemIndex);
		
		//FuncId取得
		AText	funcIdText;
		SPI_GetFuncIdText(level,itemIndex,funcIdText);
		//訪問済みに追加
		AddToVisited(funcIdText);
		
		//==================項目を開く==================
		//項目を開く
		GetApp().SPI_OpenOrRevealTextDocument(filepath,spos,epos,kObjectID_Invalid,
					(AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true),
					kAdjustScrollType_Center);
		
		//==================コール元表示を更新==================
		UpdateArray(level+1,modeIndex,funcname);
		
	}
	//描画更新
	NVI_Refresh();
}

/**
マウスホイール時処理
*/
void	AView_CallGraf::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
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
	//Hover更新
	EVTDO_DoMouseMoved(inLocalPoint,inModifierKeys);
}

/**
マウスホバー時処理
*/
ABool	AView_CallGraf::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//マウス位置のimage point取得
	AImagePoint	imagept = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagept);
	
	//ホバー項目level, item取得
	AIndex	index = kIndex_Invalid;
	AIndex	level = kIndex_Invalid;
	FindBoxIndexByPoint(imagept,level,index);
	//ホバー項目更新
	if( mCurrentHover_LevelIndex != level || mCurrentHover_ItemIndex != index )
	{
		mCurrentHover_LevelIndex = level;
		mCurrentHover_ItemIndex = index;
		NVI_Refresh();
	}
	//一致する項目が無ければホバー消去
	else if( index == kIndex_Invalid )
	{
		ClearHoverCursor();
	}
	return true;
}

/**
マウスleave時処理
*/
void	AView_CallGraf::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	ClearHoverCursor();
}

/**
ホバー消去
*/
void	AView_CallGraf::ClearHoverCursor()
{
	mCurrentHover_LevelIndex = kIndex_Invalid;
	mCurrentHover_ItemIndex = kIndex_Invalid;
	NVI_Refresh();
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_CallGraf::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
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
ABool	AView_CallGraf::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	return false;
}

/**
テキスト入力時処理
*/
ABool	AView_CallGraf::EVTDO_DoTextInput( const AText& inText, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

/**
各項目のbox rectを取得
*/
void	AView_CallGraf::GetBoxRect( const AIndex inColumnIndex, const AIndex inItemIndex, AImageRect& outRect ) const
{
	//
	outRect.left	= kLeftMargin_Box + inColumnIndex*(mBoxWidth + kRightMargin_Box + kCallerCalleeLineWidth);
	outRect.top		= kTopMargin_Box + inItemIndex*(kTopMargin_Box + mBoxHeight + kBottomMargin_Box);
	outRect.right	= outRect.left + mBoxWidth - kRightMargin_Box;
	outRect.bottom	= outRect.top + mBoxHeight;
}

/**
クリック位置から項目indexを検索
*/
void	AView_CallGraf::FindBoxIndexByPoint( const AImagePoint& inPoint, AIndex& outLevelIndex, AIndex& outItemIndex ) const
{
	outLevelIndex = kIndex_Invalid;
	outItemIndex = kIndex_Invalid;
	//各カラム毎のループ
	for( AIndex columnIndex = 0; columnIndex <= 1; columnIndex++ )
	{
		//カラムからlevelを取得
		AIndex	level = GetLevelFromColumn(columnIndex);
		//データ未設定levelならcontinue
		if( level >= mCallerArray_FilePath.GetItemCount() )
		{
			continue;
		}
		//各項目毎のループ
		for( AIndex index = 0; index < mCallerArray_FilePath.GetObjectConst(level).GetItemCount(); index++ )
		{
			//box rect内にpointがあれば、それを返す。
			AImageRect	box = {0,0,0,0};
			GetBoxRect(columnIndex,index,box);
			if( inPoint.x >= box.left && inPoint.x <= box.right &&
						inPoint.y >= box.top && inPoint.y <= box.bottom )
			{
				outLevelIndex = level;
				outItemIndex = index;
				return;
			}
		}
	}
}

/**
項目のデータを取得
*/
void	AView_CallGraf::GetItemData( const AIndex inLevelIndex, const AIndex inItemIndex,
		AText& outFilePath, AIndex& outStartIndex, AIndex& outEndIndex,
		AIndex& outModeIndex, AIndex& outCategoryIndex ) const
{
	outFilePath.DeleteAll();
	outStartIndex = kIndex_Invalid;
	outEndIndex = kIndex_Invalid;
	outModeIndex = kIndex_Invalid;
	outCategoryIndex = kIndex_Invalid;
	//選択項目のデータを取得
	mCallerArray_FilePath.GetObjectConst(inLevelIndex).Get(inItemIndex,outFilePath);
	outStartIndex = mCallerArray_StartIndex.GetObjectConst(inLevelIndex).
			Get(inItemIndex);
	outEndIndex = mCallerArray_EndIndex.GetObjectConst(inLevelIndex).
			Get(inItemIndex);
	outModeIndex = mCallerArray_ModeIndex.GetObjectConst(inLevelIndex).
			Get(inItemIndex);
	outCategoryIndex = mCallerArray_CategoryIndex.GetObjectConst(inLevelIndex).
			Get(inItemIndex);
}

/**
マウスカーソル設定
*/
ACursorType	AView_CallGraf::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mCurrentHover_ItemIndex != kIndex_Invalid )
	{
		return kCursorType_PointingHand;
	}
	return kCursorType_Arrow;
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
テキストでの現在選択されたfuncionを設定
@param inProjectFolder 検索対象フォルダ（プロジェクトフォルダでない場合は、このフォルダ内を非再帰的に検索）
@param inStartIndex 関数の定義開始位置
@param inEndIndex 関数の定義終了位置
*/
void	AView_CallGraf::SPI_SetCurrentFunction( const AFileAcc& inProjectFolder, 
		const AIndex inModeIndex, const AIndex inCategoryIndex, 
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
		const AIndex inStartIndex, const AIndex inEndIndex, const ABool inByEdit )
{
	//Id Infoクリックによるジャンプの場合には、callerFuncIdに移動元関数のfunc idを入れる
	//（Id Infoクリック時に、SPI_NotifyToCallGrafByIdInfoJump()にて、移動先関数が記憶されているので、
	// 一致すれば、そのときに記憶した移動元関数をcallerFuncIdに格納する）
	AText	callerFuncId;
	if( mIdInfoJumpMemory_FilePath.Compare(inFilePath) == true &&
				mIdInfoJumpMemory_ClassName.Compare(inClassName) == true &&
				mIdInfoJumpMemory_FunctionName.Compare(inFunctionName) == true )
	{
		callerFuncId.SetText(mIdInfoJumpMemory_CallerFuncId);
	}
	
	//今回のfuncIDを取得
	AText	funcIdText;
	AView_CallGraf::SPI_GetFuncIdText(inFilePath,inClassName,inFunctionName,funcIdText);
	
	//編集フラグが変更されていれば、編集フラグONを設定
	AIndex	foundIndexInLevel0 = mLevel0FuncIdTextArray.Find(funcIdText);
	if( inByEdit == true && foundIndexInLevel0 != kIndex_Invalid )
	{
		if( mCallerArray_EdittedFlag.GetObject(0).Get(foundIndexInLevel0) == false )
		{
			mCallerArray_EdittedFlag.GetObject(0).Set(foundIndexInLevel0,true);
			NVI_Refresh();
		}
	}
	
	//==================履歴更新するかどうかの判定==================
	
	//履歴の最後の項目（＝level=0, item index=0）と同じ関数なら何もせずリターン
	AText	filepath;
	AIndex	spos = 0, epos = 0;
	AIndex	modeIndex = kIndex_Invalid, categoryIndex = kIndex_Invalid;
	if( mCallerArray_FilePath.GetObjectConst(0).GetItemCount() > 0 )
	{
		GetItemData(0,0,filepath,spos,epos,modeIndex,categoryIndex);
		//ファイルパスと開始位置が同じなら同じ関数と判断（終了位置はチェックしない。これにより関数名の部分を編集しているときに履歴がどんどん追加されることを防止）
		if( filepath.Compare(inFilePath) == true  && spos == inStartIndex )
		{
			return;
		}
	}
	
	//最後に設定した要求データを記憶（ロック解除時に再度要求するために使用する）
	mLastSetCurrentFunction_ProjectFolder = inProjectFolder;
	mLastSetCurrentFunction_ModeIndex = inModeIndex;
	mLastSetCurrentFunction_CategoryIndex = inCategoryIndex;
	mLastSetCurrentFunction_FilePath.SetText(inFilePath);
	mLastSetCurrentFunction_ClassName.SetText(inClassName);
	mLastSetCurrentFunction_FunctionName.SetText(inFunctionName);
	mLastSetCurrentFunction_StartIndex = inStartIndex;
	mLastSetCurrentFunction_EndIndex = inEndIndex;
	mLastSetCurrentFunction_ByEdit = inByEdit;
	
	//現在の選択項目がlevel1以上の場合（＝ロック中）は何もせずリターン（動作選択オプション（ATypes.hにて設定）ONの場合のみ）
	if( kOption_CallGraf_DontUpdateHistoryWhenItemSelected == true )
	{
		if( mCurrentSelection_LevelIndex > 0 )
		{
			return;
		}
	}
	
	//履歴（レベル0）挿入位置
	AIndex	historyItemInsertIndex = 0;
	
	//動作オプションが、ロック時、最初の履歴項目だけロックするというオプションになっている場合、
	//index:1の位置に挿入する
	if( kOption_CallGraf_FixFirstHistoryItem == true )
	{
		if( mCurrentSelection_LevelIndex > 0 )
		{
			if( mCallerArray_FilePath.GetObject(0).GetItemCount() > 0 )
			{
				historyItemInsertIndex++;
			}
		}
	}
	
	//現在の選択レベル内に同じ項目があれば何もせずにリターン
	if( inByEdit == false && mCurrentSelection_LevelIndex != kIndex_Invalid )
	{
		AItemCount	itemCount = mCallerArray_FilePath.GetObjectConst(mCurrentSelection_LevelIndex).GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AText	currentFuncIdText;
			SPI_GetFuncIdText(mCurrentSelection_LevelIndex,i,currentFuncIdText);
			if( funcIdText.Compare(currentFuncIdText) == true )
			{
				return;
			}
		}
	}
	
	/*
	//現在の選択項目と同じなら何もしない
	if( mCurrentSelection_LevelIndex != kIndex_Invalid && mCurrentSelection_ItemIndex != kIndex_Invalid )
	{
		AText	currentFuncIdText;
		SPI_GetFuncIdText(mCurrentSelection_LevelIndex,mCurrentSelection_ItemIndex,currentFuncIdText);
		if( funcIdText.Compare(currentFuncIdText) == true )
		{
			return;
		}
	}
	*/
	
	//==================履歴更新==================
	
	//現在のprojectfolder, mode index, category indexを設定
	mCurrentProjectFolder.CopyFrom(inProjectFolder);
	
	//level0内の同じ内容のindexを検索し、存在していたらその項目は削除（spos, eposデータを更新しておきたいので、移動ではなく、削除・追加にする）
	ABool	edited = false;
	if( foundIndexInLevel0 != kIndex_Invalid )
	{
		//------------------削除------------------
		//編集済みフラグ記憶
		edited = mCallerArray_EdittedFlag.GetObject(0).Get(foundIndexInLevel0);
		//項目削除
		mCallerArray_FilePath.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_FunctionName.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_ClassName.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_StartIndex.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_EndIndex.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_ModeIndex.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_CategoryIndex.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_EdittedFlag.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_ClickedFlag.GetObject(0).Delete1(foundIndexInLevel0);
		mCallerArray_CallerFuncId.GetObject(0).Delete1(foundIndexInLevel0);
		//
		mLevel0FuncIdTextArray.Delete1(foundIndexInLevel0);
	}
	//------------------履歴の最初に追加------------------
	mCallerArray_FilePath.GetObject(0).Insert1(historyItemInsertIndex,inFilePath);
	mCallerArray_FunctionName.GetObject(0).Insert1(historyItemInsertIndex,inFunctionName);
	mCallerArray_ClassName.GetObject(0).Insert1(historyItemInsertIndex,inClassName);
	mCallerArray_StartIndex.GetObject(0).Insert1(historyItemInsertIndex,inStartIndex);
	mCallerArray_EndIndex.GetObject(0).Insert1(historyItemInsertIndex,inEndIndex);
	mCallerArray_ModeIndex.GetObject(0).Insert1(historyItemInsertIndex,inModeIndex);
	mCallerArray_CategoryIndex.GetObject(0).Insert1(historyItemInsertIndex,inCategoryIndex);
	mCallerArray_EdittedFlag.GetObject(0).Insert1(historyItemInsertIndex,(inByEdit||edited));
	mCallerArray_ClickedFlag.GetObject(0).Insert1(historyItemInsertIndex,false);
	mCallerArray_CallerFuncId.GetObject(0).Insert1(historyItemInsertIndex,callerFuncId);
	//
	mLevel0FuncIdTextArray.Insert1(historyItemInsertIndex,funcIdText);
	
	//level0のcallee item index（＝線のつながっているところ）を更新する
	//（ただし、非選択状態なら、この下の部分で、コール元表示は自動更新されるので、callee item indexも0に更新される。）
	if( historyItemInsertIndex == 0 )//ロック中で、履歴2番目に項目追加した場合は、callee itemは項目1番目なので変化しない。
	{
		AIndex	origCurrentIndex = mCallerArray_CalleeItemIndex.Get(0);
		if( origCurrentIndex != kIndex_Invalid )
		{
			if( foundIndexInLevel0 == kIndex_Invalid )
			{
				//履歴を新規追加した場合
				//callee item indexを+1
				mCallerArray_CalleeItemIndex.Set(0,origCurrentIndex+1);
			}
			else if( foundIndexInLevel0 == origCurrentIndex )
			{
				//履歴を移動し、かつ、その項目がcallee itemの場合
				//callee item indexは0
				mCallerArray_CalleeItemIndex.Set(0,0);
			}
			else if( foundIndexInLevel0 > origCurrentIndex )
			{
				//履歴を移動し、かつ、その項目がcallee itemより後だった場合
				//callee item indexを+1
				mCallerArray_CalleeItemIndex.Set(0,origCurrentIndex+1);
			}
		}
	}
	
	//制限数オーバーなら最後の項目を削除
	AItemCount	historyLimitCount = kLimit_CallGrafLevel0ItemCount;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory) == true )
	{
		historyLimitCount = 1;
	}
	while( mCallerArray_FilePath.GetObject(0).GetItemCount() > historyLimitCount )
	{
		mCallerArray_FilePath.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_FunctionName.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_ClassName.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_StartIndex.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_EndIndex.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_ModeIndex.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_CategoryIndex.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_EdittedFlag.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_ClickedFlag.GetObject(0).Delete1(historyLimitCount);
		mCallerArray_CallerFuncId.GetObject(0).Delete1(historyLimitCount);
		//
		mLevel0FuncIdTextArray.Delete1(historyLimitCount);
	}
	
	//訪問済みに追加
	AddToVisited(funcIdText);
	
	//非選択状態の場合に限り、コール元表示を、自動的に履歴最新のコール元表示に更新する。
	if( mCurrentSelection_LevelIndex == kIndex_Invalid || mCurrentSelection_ItemIndex == kIndex_Invalid )
	{
		UpdateCallerArrayForTheLatestHistory();
	}
	//Image size更新
	UpdateImageSize();
	
	//描画更新
	NVI_Refresh();
}

/**
編集有りフラグを設定（既に追加済みの履歴項目から検索して、存在していればその項目の編集フラグをONにする）
*/
void	AView_CallGraf::SPI_SetEditFlag( const AText& inFilePath, const AText& inClassName, const AText& inFunctionName )
{
	//今回のfuncIDを取得
	AText	funcIdText;
	AView_CallGraf::SPI_GetFuncIdText(inFilePath,inClassName,inFunctionName,funcIdText);
	
	//編集フラグが変更されていれば、編集フラグONを設定
	AIndex	foundIndexInLevel0 = mLevel0FuncIdTextArray.Find(funcIdText);
	if( foundIndexInLevel0 != kIndex_Invalid )
	{
		if( mCallerArray_EdittedFlag.GetObject(0).Get(foundIndexInLevel0) == false )
		{
			//編集フラグON
			mCallerArray_EdittedFlag.GetObject(0).Set(foundIndexInLevel0,true);
			//描画更新
			NVI_Refresh();
		}
	}
}

/**
履歴最新項目についてcaller表示する
*/
void	AView_CallGraf::UpdateCallerArrayForTheLatestHistory()
{
	//レベル0の現在項目を最新の項目に設定する
	mCallerArray_CalleeItemIndex.Set(0,0);
	
	//(0,0)にスクロール
	AImagePoint	originpt = {0,0};
	NVI_ScrollTo(originpt);
	
	//最新履歴の項目のデータを取得
	AIndex	modeIndex = mCallerArray_ModeIndex.GetObjectConst(0).Get(0);
	AText	functionname;
	mCallerArray_FunctionName.GetObjectConst(0).Get(0,functionname);
	//レベル1のarrayを更新（上でmCallerArray_CalleeItemIndexに0を設定しているので、履歴最新からコール元表示される。）
	UpdateArray(1,modeIndex,functionname);
}

/**
指定レベルのarrayを更新（指定functionを使用する場所の一覧を作成）
*/
void	AView_CallGraf::UpdateArray( const AIndex inLevelIndex, const AIndex inModeIndex, const AText& inFunctionName )
{
	//指定レベルのarrayが存在していなければ、arrayを生成
	while( inLevelIndex >= mCallerArray_FilePath.GetItemCount() )
	{
		//各array
		mCallerArray_FilePath.AddNewObject();
		mCallerArray_FunctionName.AddNewObject();
		mCallerArray_ClassName.AddNewObject();
		mCallerArray_StartIndex.AddNewObject();
		mCallerArray_EndIndex.AddNewObject();
		mCallerArray_ModeIndex.AddNewObject();
		mCallerArray_CategoryIndex.AddNewObject();
		mCallerArray_EdittedFlag.AddNewObject();
		mCallerArray_ClickedFlag.AddNewObject();
		mCallerArray_CallerFuncId.AddNewObject();
		//array内選択index
		mCallerArray_CalleeItemIndex.Add(0);
		//arrayの現在単語
		mCurrentWordArray.Add(GetEmptyText());
	}
	//指定level以上の全項目を削除
	for( AIndex i = inLevelIndex; i < mCallerArray_FilePath.GetItemCount(); i++ )
	{
		mCallerArray_FilePath.GetObject(i).DeleteAll();
		mCallerArray_FunctionName.GetObject(i).DeleteAll();
		mCallerArray_ClassName.GetObject(i).DeleteAll();
		mCallerArray_StartIndex.GetObject(i).DeleteAll();
		mCallerArray_EndIndex.GetObject(i).DeleteAll();
		mCallerArray_ModeIndex.GetObject(i).DeleteAll();
		mCallerArray_CategoryIndex.GetObject(i).DeleteAll();
		mCallerArray_EdittedFlag.GetObject(i).DeleteAll();
		mCallerArray_ClickedFlag.GetObject(i).DeleteAll();
		mCallerArray_CallerFuncId.GetObject(i).DeleteAll();
		//呼ばれ先indexをクリア
		mCallerArray_CalleeItemIndex.Set(i,kIndex_Invalid);
		//
		mCurrentWordArray.Set(i,GetEmptyText());
	}
	//arrayの現在単語設定
	mCurrentWordArray.Set(inLevelIndex-1,inFunctionName);
	
	//==================スレッドにワーズリスト検索要求==================
	{
		//ワードリストスレッドに検索要求
		//mCurrentProjectFolderがプロジェクトフォルダで無い場合、スレッド内（というかSPI_GetImportFileDataIDArrayForWordsList()にて、
		//同じフォルダを対象に検索を行う。
		GetWordsListFinderThread().SetRequestDataForCallGraf(mCurrentProjectFolder,
					inModeIndex,NVM_GetWindowConst().GetObjectID(),inFunctionName,inLevelIndex);
		//スレッドunpause
		GetWordsListFinderThread().NVI_UnpauseIfPaused();
		//検索中フラグON、ウインドウ描画更新
		mIsFinding = true;
		NVM_GetWindow().NVI_UpdateProperty();
	}
	
	//ヘッダビュー更新
	UpdateHeaderView();
}

/**
検索中断
*/
void	AView_CallGraf::SPI_AbortCurrentWordsListFinderRequest()
{
	GetWordsListFinderThread().AbortCurrentRequest();
}

/**
データ全削除
*/
void	AView_CallGraf::SPI_DeleteAllData()
{
	//現在の検索を中断する
	SPI_AbortCurrentWordsListFinderRequest();
	
	//検索中フラグOFF
	mIsFinding = false;
	//現在の選択・ホバーをクリア
	mCurrentSelection_LevelIndex = kIndex_Invalid;
	mCurrentSelection_ItemIndex = kIndex_Invalid;
	mCurrentHover_LevelIndex = kIndex_Invalid;
	mCurrentHover_ItemIndex = kIndex_Invalid;
	//表示開始カラムを1に設定
	mCurrentDisplayStartLevel = 1;
	//現在のプロジェクトフォルダクリア
	mCurrentProjectFolder.Unspecify();
	//レベル1以上のデータを削除（レベル0は履歴なので（カテゴリデータ以外）クリアしない。）
	AItemCount	levelCount = mCallerArray_FilePath.GetItemCount();
	for( AIndex i = 1; i < levelCount; i++ )
	{
		//項目クリア
		mCallerArray_FilePath.GetObject(i).DeleteAll();
		mCallerArray_FunctionName.GetObject(i).DeleteAll();
		mCallerArray_ClassName.GetObject(i).DeleteAll();
		mCallerArray_StartIndex.GetObject(i).DeleteAll();
		mCallerArray_EndIndex.GetObject(i).DeleteAll();
		mCallerArray_ModeIndex.GetObject(i).DeleteAll();
		mCallerArray_CategoryIndex.GetObject(i).DeleteAll();
		mCallerArray_EdittedFlag.GetObject(i).DeleteAll();
		mCallerArray_ClickedFlag.GetObject(i).DeleteAll();
		mCallerArray_CallerFuncId.GetObject(i).DeleteAll();
		//calleeクリア
		mCallerArray_CalleeItemIndex.Set(i,kIndex_Invalid);
		//現在単語クリア
		mCurrentWordArray.Set(i,GetEmptyText());
	}
	//全検索済みフラグクリア
	mAllFilesSearched = false;
	//訪問済みfunc idクリア
	mVisitedFuncIdTextArray.DeleteAll();
	
	//mLevel0FuncIdTextArrayはクリアしない。（レベル0なので）
	
	//レベル0のカテゴリindexをクリア
	AItemCount	itemCount = mCallerArray_CategoryIndex.GetObjectConst(0).GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		mCallerArray_CategoryIndex.GetObject(0).Set(i,kIndex_Invalid);
	}
	
	//id infoからの通知記録クリア
	mIdInfoJumpMemory_FilePath.DeleteAll();
	mIdInfoJumpMemory_ClassName.DeleteAll();
	mIdInfoJumpMemory_FunctionName.DeleteAll();
	mIdInfoJumpMemory_CallerFuncId.DeleteAll();
	
	//Image size更新
	UpdateImageSize();
}

/**
スレッドのワーズリスト検索結果反映
*/
void	AView_CallGraf::SPI_DoWordsListFinderPaused()
{
	//レベル0のデータがなしなら何もせずリターン
	if( mCallerArray_FilePath.GetObjectConst(0).GetItemCount() == 0 )
	{
		return;
	}
	
	//検索結果取得
	AWindowID	callGrafWindowID;
	AIndex	requestLevelIndex = kIndex_Invalid;
	ATextArray	filePathArray;
	ATextArray	classNameArray;
	ATextArray	functionNameArray;
	AArray<AIndex>	startArray;
	AArray<AIndex>	endArray;
	ABool	allFilesSearched = GetWordsListFinderThread().GetResultForCallGraf(callGrafWindowID,requestLevelIndex,filePathArray,classNameArray,functionNameArray,startArray,endArray);
	//コール先項目のデータ取得
	AText	currentSelectedItem_FilePath;
	AIndex	currentSelectedItem_StartIndex = 0;
	AIndex	currentSelectedItem_EndIndex = 0;
	AIndex	currentSelectedItem_ModeIndex = kIndex_Invalid;
	AIndex	currentSelectedItem_CategoryIndex = kIndex_Invalid;
	//コール先項目（＝要求レベルの１つ右のレベルの現在項目）のデータ取得
	GetItemData(requestLevelIndex-1,mCallerArray_CalleeItemIndex.Get(requestLevelIndex-1),
				currentSelectedItem_FilePath,currentSelectedItem_StartIndex,currentSelectedItem_EndIndex,
				currentSelectedItem_ModeIndex,currentSelectedItem_CategoryIndex);
	//スレッド検索結果を追加
	for( AIndex i = 0; i < filePathArray.GetItemCount(); i++ )
	{
		//スレッド検索結果データ取得
		AText	filepath;
		filePathArray.Get(i,filepath);
		AIndex	spos = startArray.Get(i);
		AIndex	epos = endArray.Get(i);
		//現在の選択項目のデータと同じ項目は、arrayに追加しない
		if( filepath.Compare(currentSelectedItem_FilePath) == true &&
					spos == currentSelectedItem_StartIndex &&
					epos == currentSelectedItem_EndIndex )
		{
			continue;
		}
		//arrayに追加
		mCallerArray_FilePath.GetObject(requestLevelIndex).Add(filepath);
		mCallerArray_ClassName.GetObject(requestLevelIndex).Add(classNameArray.GetTextConst(i));
		mCallerArray_FunctionName.GetObject(requestLevelIndex).Add(functionNameArray.GetTextConst(i));
		mCallerArray_StartIndex.GetObject(requestLevelIndex).Add(spos);
		mCallerArray_EndIndex.GetObject(requestLevelIndex).Add(epos);
		mCallerArray_ModeIndex.GetObject(requestLevelIndex).Add(currentSelectedItem_ModeIndex);
		mCallerArray_CategoryIndex.GetObject(requestLevelIndex).Add(currentSelectedItem_CategoryIndex);
		mCallerArray_EdittedFlag.GetObject(requestLevelIndex).Add(false);
		mCallerArray_ClickedFlag.GetObject(requestLevelIndex).Add(false);
		mCallerArray_CallerFuncId.GetObject(requestLevelIndex).Add(GetEmptyText());
	}
	//未検索項目がないかどうかのフラグ（importFileData生成中は、falseになる）
	mAllFilesSearched = allFilesSearched;
	//Image size更新
	UpdateImageSize();
	//検索中フラグOFF、プログレス表示更新
	mIsFinding = false;
	NVM_GetWindow().NVI_UpdateProperty();
	//描画更新
	NVI_Refresh();
}

/**
IdInfoクリックによるジャンプ時にコールされる
*/
void	AView_CallGraf::SPI_NotifyToCallGrafByIdInfoJump( const AText& inCallerFuncIdText, 
		const AText& inFilePath, const AText& inClassName, const AText& inFunctionName, 
		const AIndex inStartIndex, const AIndex inEndIndex )
{
	mIdInfoJumpMemory_FilePath.SetText(inFilePath);
	mIdInfoJumpMemory_ClassName.SetText(inClassName);
	mIdInfoJumpMemory_FunctionName.SetText(inFunctionName);
	mIdInfoJumpMemory_CallerFuncId.SetText(inCallerFuncIdText);
}

/**
ドキュメント編集時にコールされる
*/
void	AView_CallGraf::SPI_NotifyToCallGrafByTextDocumentEdited( const AFileAcc& inFile, const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	//更新ファイルを取得
	AText	filepath;
	inFile.GetPath(filepath);
	//各レベル毎のループ
	AItemCount	levelCount = mCallerArray_FilePath.GetItemCount();
	for( AIndex level = 0; level < levelCount; level++ )
	{
		//更新ファイルに一致する項目のindexを取得
		AArray<AIndex>	indexArray;
		mCallerArray_FilePath.GetObjectConst(level).FindAll(filepath,0,filepath.GetItemCount(),indexArray);
		//各項目毎のループ
		AItemCount	itemCount = indexArray.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AIndex	index = indexArray.Get(i);
			//start indexが更新位置以降の場合は、start index, end indexを更新
			AIndex	spos = mCallerArray_StartIndex.GetObjectConst(level).Get(index);
			if( spos > inTextIndex )
			{
				mCallerArray_StartIndex.GetObject(level).Set(index,spos+inInsertedCount);
				mCallerArray_EndIndex.GetObject(level).Set(index,
							mCallerArray_EndIndex.GetObjectConst(level).Get(index)+inInsertedCount);
			}
		}
	}
}

/**
ヘッダービューの描画更新
*/
void	AView_CallGraf::UpdateHeaderView()
{
	//ヘッダビューの描画更新
	GetApp().SPI_GetCallGrafWindow(NVM_GetWindow().GetObjectID()).SPI_GetCallGrafHeaderView().
			SPI_SetCurrentStatus(mCurrentDisplayStartLevel,mCurrentWordArray);
}

/**
現在項目設定
*/
/*
void	AView_CallGraf::SetSelect( const AIndex inLevel, const AIndex inItemIndex )
{
	//現在の選択位置を記憶
	mCurrentSelection_LevelIndex = inLevel;
	mCurrentSelection_ItemIndex = inItemIndex;
	
	//各level毎の選択を記憶
	mCallerArray_CalleeItemIndex.Set(inLevel,inItemIndex);
}
*/

/**
ホームへ移動するキーをクリック時処理
*/
void	AView_CallGraf::SPI_ReturnHome()
{
	//選択解除
	mCurrentSelection_LevelIndex = kIndex_Invalid;
	mCurrentSelection_ItemIndex = kIndex_Invalid;
	//表示カラム初期化
	mCurrentDisplayStartLevel = 1;
	//履歴最新項目についてコール元表示
	UpdateCallerArrayForTheLatestHistory();
	//イメージサイズ更新
	UpdateImageSize();
	//描画更新
	NVI_Refresh();
	//ヘッダビュー更新
	UpdateHeaderView();
	//最後の要求データ（＝ロック中なので実行されなかった）を、要求する
	if( kOption_CallGraf_DontUpdateHistoryWhenItemSelected == true )
	{
		AFileAcc	projectFolder = mLastSetCurrentFunction_ProjectFolder;
		AIndex	modeIndex = mLastSetCurrentFunction_ModeIndex;
		AIndex	categoryIndex = mLastSetCurrentFunction_CategoryIndex;
		AText	filePath = mLastSetCurrentFunction_FilePath;
		AText	className = mLastSetCurrentFunction_ClassName;
		AText	functionName = mLastSetCurrentFunction_FunctionName;
		AIndex	startIndex = mLastSetCurrentFunction_StartIndex;
		AIndex	endIndex = mLastSetCurrentFunction_EndIndex;
		ABool	byEdit = mLastSetCurrentFunction_ByEdit;
		SPI_SetCurrentFunction(projectFolder,modeIndex,categoryIndex,filePath,className,functionName,startIndex,endIndex,byEdit);
	}
}

/**
左レベルスクロール
*/
void	AView_CallGraf::SPI_GoLeftLevel()
{
	//カラムレベルインクリメント
	mCurrentDisplayStartLevel++;
	//イメージサイズ更新
	UpdateImageSize();
	//描画更新
	NVI_Refresh();
	//ヘッダビュー更新
	UpdateHeaderView();
}

/**
右レベルスクロール
*/
void	AView_CallGraf::SPI_GoRightLevel()
{
	if( mCurrentDisplayStartLevel >= 2 )
	{
		//カラムレベルデクリメント
		mCurrentDisplayStartLevel--;
		//イメージサイズ更新
		UpdateImageSize();
		//描画更新
		NVI_Refresh();
		//ヘッダビュー更新
		UpdateHeaderView();
	}
}

/**
訪問済みに追加
*/
void	AView_CallGraf::AddToVisited( const AText& inFuncIdText )
{
	if( mVisitedFuncIdTextArray.Find(inFuncIdText) == kIndex_Invalid )
	{
		mVisitedFuncIdTextArray.Add(inFuncIdText);
	}
}

/**
イメージサイズ更新
*/
void	AView_CallGraf::UpdateImageSize()
{
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//image高さ
	ANumber	imageHeight = 0;
	//各カラム毎ループ
	for( AIndex columnIndex = 0; columnIndex <= 1; columnIndex++ )
	{
		//カラムからlevel取得
		AIndex	level = GetLevelFromColumn(columnIndex);
		
		//対象levelのデータ未生成なら何もしない
		if( level >= mCallerArray_FilePath.GetItemCount() )   continue;
		
		//項目数取得
		AItemCount	itemCount = mCallerArray_FilePath.GetObjectConst(level).GetItemCount();
		if( itemCount == 0 )   continue;
		
		//最後の項目のbox rect取得
		AImageRect	box = {0};
		GetBoxRect(columnIndex,itemCount-1,box);
		//image高さ更新
		ANumber	columnImageHeight = box.bottom + kBottomMargin_Box;
		if( imageHeight < columnImageHeight )
		{
			imageHeight = columnImageHeight;
		}
	}
	
	//イメージサイズ設定
	NVM_SetImageSize(frameRect.right - frameRect.left,imageHeight);
	
	//scroll設定
	ALocalRect	rect = {0};
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(16,16,rect.right-rect.left-16,rect.bottom-rect.top-16);
}

/**
FuncId取得
*/
void	AView_CallGraf::SPI_GetFuncIdText( const AIndex inLevel, const AIndex inItemIndex, AText& outFuncIdText ) const
{
	AText	filepath;
	mCallerArray_FilePath.GetObjectConst(inLevel).Get(inItemIndex,filepath);
	AText	classname;
	mCallerArray_ClassName.GetObjectConst(inLevel).Get(inItemIndex,classname);
	AText	funcname;
	mCallerArray_FunctionName.GetObjectConst(inLevel).Get(inItemIndex,funcname);
	//FuncId取得
	AView_CallGraf::SPI_GetFuncIdText(filepath,classname,funcname,outFuncIdText);
}

/**
FuncId取得
*/
void	AView_CallGraf::SPI_GetFuncIdText( const AText& inFilePath, const AText& inClassName, const AText& inFuncName,
		AText& outFuncIdText )
{
	outFuncIdText.SetText(inFilePath);
	outFuncIdText.Add(kUChar_Tab);
	outFuncIdText.AddText(inClassName);
	outFuncIdText.Add(kUChar_Tab);
	outFuncIdText.AddText(inFuncName);
}

/**
次の項目を選択
*/
void	AView_CallGraf::SPI_SelectNextItem()
{
	if( mCurrentSelection_LevelIndex != kIndex_Invalid )
	{
		if( mCurrentSelection_ItemIndex >= 0 && mCurrentSelection_ItemIndex+1 < mCallerArray_FilePath.GetObject(mCurrentSelection_LevelIndex).GetItemCount() )
		{
			//現在選択項目インデックスをインクリメント
			mCurrentSelection_ItemIndex++;
			//項目を開く
			SPI_OpenItem(mCurrentSelection_LevelIndex,mCurrentSelection_ItemIndex,0);
			//スクロール調整
			SPI_AdjustScroll();
		}
	}
}

/**
前の項目を選択
*/
void	AView_CallGraf::SPI_SelectPreviousItem()
{
	if( mCurrentSelection_LevelIndex != kIndex_Invalid )
	{
		if( mCurrentSelection_ItemIndex-1 >= 0 && mCurrentSelection_ItemIndex < mCallerArray_FilePath.GetObject(mCurrentSelection_LevelIndex).GetItemCount() )
		{
			//現在選択項目インデックスをデクリメント
			mCurrentSelection_ItemIndex--;
			//項目を開く
			SPI_OpenItem(mCurrentSelection_LevelIndex,mCurrentSelection_ItemIndex,0);
			//スクロール調整
			SPI_AdjustScroll();
		}
	}
}

/**
スクロール調整
*/
void	AView_CallGraf::SPI_AdjustScroll()
{
	if( mCurrentSelection_LevelIndex != kIndex_Invalid && mCurrentSelection_ItemIndex != kIndex_Invalid )
	{
		//項目のrectを取得
		AImageRect	box = {0};
		GetBoxRect(mCurrentSelection_LevelIndex,mCurrentSelection_ItemIndex,box);
		//フレームのimage rectを取得
		AImageRect	imageFrameRect = {0};
		NVM_GetImageViewRect(imageFrameRect);
		//frameよりも下にあるとき
		if( box.bottom > imageFrameRect.bottom )
		{
			AImagePoint	imagept = {imageFrameRect.left,box.top-(imageFrameRect.bottom-imageFrameRect.top)/2};
			if( imagept.y < 0 )   imagept.y = 0;
			NVI_ScrollTo(imagept);
		}
		//frameよりも上にあるとき
		if( box.top < imageFrameRect.top )
		{
			AImagePoint	imagept = {imageFrameRect.left,box.top-(imageFrameRect.bottom-imageFrameRect.top)/2};
			if( imagept.y < 0 )   imagept.y = 0;
			NVI_ScrollTo(imagept);
		}
	}
}


