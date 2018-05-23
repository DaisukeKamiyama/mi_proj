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

AView_Notification
#725

*/

#include "stdafx.h"

#include "AView_Notification.h"
#include "AApp.h"

//各高さ
const ANumber	kHeight_ItemBox = 38;
const ANumber	kHeight_Header = 18;
const ANumber	kHeight_ItemMargin = 4;
const ANumber	kHeight_ItemBoxWithMargin = kHeight_ItemBox + kHeight_ItemMargin;

//ヘッダー部分の上下左右マージン
const ANumber	kLeftMargin_HeaderText = 12;
const ANumber	kRightMargin_HeaderText = 12;
const ANumber	kTopMargin_HeaderText = 2;
const ANumber	kBottomMargin_HeaderText = 2;
//内容部分の上下左右マージン
const ANumber	kLeftMargin_ContentText = 14;
const ANumber	kRightMargin_ContentText = 14;
const ANumber	kTopMargin_ContentText = 2;
const ANumber	kBottomMargin_ContentText = 2;

//角丸の半径 #1079
const AFloatNumber	kRoundedRectRad = 3.0f;

#pragma mark ===========================
#pragma mark [クラス]AView_Notification
#pragma mark ===========================
/*
各種メッセージ表示用ポップアップ
各text view毎にAWindow_Notificationポップアップウインドウを持ち、そのウインドウ内にAView_Notificationがある
*/

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_Notification::AView_Notification( const AWindowID inWindowID, const AControlID inID ) : AView(inWindowID,inID)
{
	NVMC_SetOffscreenMode(true);//win
}

//デストラクタ
AView_Notification::~AView_Notification()
{
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_Notification::NVIDO_Init()
{
	NVMC_EnableMouseLeaveEvent();
}

/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_Notification::NVIDO_DoDeleted()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//描画要求時のコールバック(AView用)
void	AView_Notification::EVTDO_DoDraw()
{
	//フォント取得
	//ヘッダ部分フォント
	AText	labelfontname;
	AFontWrapper::GetDialogDefaultFontName(labelfontname);
	
	//frame rect取得
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//表示alpha取得
	//AFloatNumber	backgroundAlpha = GetApp().SPI_GetPopupWindowAlpha();
	
	//view全体消去
	NVMC_EraseRect(frameRect);
	
	//==================色取得==================
	
	//描画色設定
	AColor	boxBaseColor1 = kColor_White, boxBaseColor2 = kColor_White, boxBaseColor3 = kColor_White;
	boxBaseColor1 = AColorWrapper::ChangeHSV(kColor_White,0,1.0,1.2);
	boxBaseColor2 = AColorWrapper::ChangeHSV(kColor_White,0,1.0,0.95);
	boxBaseColor3 = AColorWrapper::ChangeHSV(kColor_White,0,1.0,0.93);
	
	//各項目毎のループ
	for( AIndex index = 0; index < mArray_Text.GetItemCount(); index++ )
	{
		//項目のrect取得
		ALocalRect	itemLocalRect = {0};
		GetItemBox(index,itemLocalRect);
		
		//項目毎のα値取得
		AFloatNumber	alpha = mArray_Alpha.Get(index);
		
		//項目毎の色取得
		AColor	color = mArray_Color.Get(index);
		
		//==================全体rounded rect描画==================
		//round rect描画
		NVMC_PaintRoundedRect(itemLocalRect,
							  //kColor_White,kColor_Gray95Percent,
							  boxBaseColor1,boxBaseColor3,
							  kGradientType_Vertical,alpha,alpha,
							  kRoundedRectRad,true,true,true,true);
		
		//==================ヘッダ描画==================
		//フォント設定
		NVMC_SetDefaultTextProperty(labelfontname,9.5,kColor_Black,kTextStyle_DropShadow,true,alpha);
		//ヘッダrect取得
		ALocalRect	headerLocalRect = itemLocalRect;
		headerLocalRect.bottom = headerLocalRect.top + kHeight_Header;
		//ヘッダ描画
		AColor	headercolor_start = AColorWrapper::ChangeHSV(color,0,0.8,1.0);
		AColor	headercolor_end = AColorWrapper::ChangeHSV(color,0,1.2,1.0);
		NVMC_PaintRoundedRect(headerLocalRect,headercolor_start,headercolor_end,kGradientType_Vertical,0.15,0.15,
							  kRoundedRectRad,true,true,true,true);
		//ヘッダタイトル取得
		AText	title;
		mArray_Title.Get(index,title);
		//ヘッダタイトル描画領域取得
		ALocalRect	headerTextLocalRect = headerLocalRect;
		headerTextLocalRect.left 	+= kLeftMargin_HeaderText;
		headerTextLocalRect.right 	-= kRightMargin_HeaderText;
		headerTextLocalRect.top 	+= kTopMargin_HeaderText;
		headerTextLocalRect.bottom 	-= kBottomMargin_HeaderText;
		//ヘッダタイトル描画
		AText	ellipsisTitle;
		NVI_GetEllipsisTextWithFixedLastCharacters(title,headerTextLocalRect.right-headerTextLocalRect.left,5,ellipsisTitle);
		NVMC_DrawText(headerTextLocalRect,ellipsisTitle);
		
		//==================content描画==================
		//フォント設定
		NVMC_SetDefaultTextProperty(labelfontname,10.0,kColor_Gray20Percent,kTextStyle_DropShadow,true,alpha);
		//内容テキスト取得
		AText	text;
		mArray_Text.Get(index,text);
		//内容rect取得
		ALocalRect	contentLocalRect = itemLocalRect;
		contentLocalRect.top += kHeight_Header;
		//内容テキスト描画領域取得
		ALocalRect	contentTextLocalRect = contentLocalRect;
		contentTextLocalRect.left 	+= kLeftMargin_ContentText;
		contentTextLocalRect.right 	-= kRightMargin_ContentText;
		contentTextLocalRect.top 	+= kTopMargin_ContentText;
		contentTextLocalRect.bottom -= kBottomMargin_ContentText;
		//内容テキスト描画
		AText	ellipsisText;
		NVI_GetEllipsisTextWithFixedLastCharacters(text,contentTextLocalRect.right-contentTextLocalRect.left,5,ellipsisText);
		NVMC_DrawText(contentTextLocalRect,ellipsisText);
		
		//==================フレーム描画==================
		//フレーム描画
		NVMC_FrameRoundedRect(itemLocalRect,kColor_Gray50Percent,1.0,kRoundedRectRad,true,true,true,true);
	}
}

/**
項目のrect取得
*/
void	AView_Notification::GetItemBox( const AIndex inIndex, ALocalRect& outRect ) const
{
	//frame rect取得
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	
	//項目rect取得
	outRect = frameRect;
	outRect.top		= inIndex*kHeight_ItemBoxWithMargin;
	outRect.bottom 	= outRect.top + kHeight_ItemBox;
}

/**
local pointから項目のindexを検索
*/
AIndex	AView_Notification::FindItemBox( const ALocalPoint& inLocalPoint ) const
{
	AItemCount	itemCount = mArray_Text.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		//項目のrectを取得
		ALocalRect	boxrect = {0};
		GetItemBox(i,boxrect);
		//指定pointがrect内なら現在のindexを返す。
		if( IsPointInRect(inLocalPoint,boxrect) == true )
		{
			return i;
		}
	}
	return kIndex_Invalid;
}

//マウスボタン押下時のコールバック(AView用)
ABool	AView_Notification::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//項目をクリックしたら、項目を閉じる
	AIndex	index = FindItemBox(inLocalPoint);
	if( index != kIndex_Invalid )
	{
		//URLスキームがあればURLスキーム実行 #1003
		AText	uri;
		mArray_URI.Get(index,uri);
		if( uri.GetItemCount() > 0 )
		{
			GetApp().NVI_ExecuteURIScheme(uri);
		}
		//閉じる
		CloseBox(index);
	}
	return true;
}

//
void	AView_Notification::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
		const ALocalPoint inLocalPoint )
{
}

ABool	AView_Notification::EVTDO_DoTextInput( const AText& inText, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	return false;
}

//カーソル
ABool	AView_Notification::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return true;
}

void	AView_Notification::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
}

/**
マウスボタン押下時のコールバック(AView用)
*/
ABool	AView_Notification::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return true;
}

//#507
/**
ヘルプタグ
*/
ABool	AView_Notification::EVTDO_DoGetHelpTag( const ALocalPoint& inPoint, AText& outText1, AText& outText2, ALocalRect& outRect, AHelpTagSide& outTagSide ) 
{
	return false;
}

/**
カーソルタイプ取得
*/
ACursorType	AView_Notification::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	return kCursorType_Arrow;
}

/**
tickタイマー
*/
void	AView_Notification::SPI_DoTickTimer()
{
	//各項目毎のループ
	for( AIndex i = 0; i < mArray_HideCounter.GetItemCount(); )
	{
		//カウンタデクリメント（ただし、max値が設定されている項目はデクリメントしない）
		ANumber	count = mArray_HideCounter.Get(i);
		if( count == kNumber_MaxNumber )
		{
			i++;
			continue;
		}
		count--;
		mArray_HideCounter.Set(i,count);
		//カウンタ16以下になったらα値を下げていく
		if( count <= 16 )
		{
			mArray_Alpha.Set(i,0.9*count/16);
			NVI_Refresh();
			NVM_GetWindow().NVI_UpdateWindow();
		}
		//カウンタ0になったら項目を閉じる
		if( count <= 0 )
		{
			CloseBox(i);
		}
		else
		{
			i++;
		}
	}
}

/**
ウインドウhide時処理
*/
void	AView_Notification::SPI_DoWindowHide()
{
	//タイマーによって閉じるべき項目のみ全て閉じる
	//各項目毎のループ
	for( AIndex i = 0; i < mArray_HideCounter.GetItemCount(); )
	{
		ANumber	count = mArray_HideCounter.Get(i);
		if( count != kNumber_MaxNumber )
		{
			CloseBox(i);
		}
		else
		{
			i++;
		}
	}
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

/**
置換結果を表示
*/
void	AView_Notification::SPI_SetNotification_ReplaceResult( const ABool inNotAborted, const ANumber inCount,
		const AText& inFindText, const AText& inReplaceText )
{
	//テキスト取得
	AText	text, title;
	AColor	color = kColor_Blue;
	if( inNotAborted == true )
	{
		//置換OK時
		text.SetLocalizedText("Notification_ReplaceResult_OK");
		title.SetLocalizedText("Notification_ReplaceResult_OK_Title");
	}
	else
	{
		//置換abort時
		text.SetLocalizedText("Notification_ReplaceResult_Aborted");
		title.SetLocalizedText("Notification_ReplaceResult_Aborted_Title");
		color = kColor_Red;
	}
	//置換個数
	text.ReplaceParamText('0',inCount);
	//検索テキスト
	AText	findtext;
	findtext.SetText(inFindText);
	findtext.ReplaceChar(kUChar_LineEnd,kUChar_Space);
	findtext.ReplaceChar(kUChar_Tab,kUChar_Space);
	//置換テキスト
	AText	replacetext;
	replacetext.SetText(inReplaceText);
	replacetext.ReplaceChar(kUChar_LineEnd,kUChar_Space);
	replacetext.ReplaceChar(kUChar_Tab,kUChar_Space);
	//検索・置換テキストをヘッダタイトルにつける
	title.AddCstring(" (");
	title.AddText(findtext);
	title.AddCstring("->");
	title.AddText(replacetext);
	title.AddCstring(")");
	//項目追加
	mArray_Text.Add(text);
	mArray_Title.Add(title);
	mArray_HideCounter.Add(80);
	mArray_Color.Add(color);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_ReplaceResult);
	mArray_URI.Add(GetEmptyText());//#1003
	//描画更新
	NVI_Refresh();
	//ウインドウ高さ更新
	SPI_UpdateWindowHeight();
}

/**
検索結果を表示
*/
void	AView_Notification::SPI_SetNotification_FindResult( const ABool inFoundInNext, const ABool inFoundInPrev,
		const AText& inFindText )
{
	//テキスト取得
	AText	text, title;
	AColor	color = kColor_Gray70Percent;
	if( inFoundInNext == false && inFoundInPrev == false )
	{
		//検索結果なし（前にも後にもなし）
		text.SetLocalizedText("Notification_FindResult_NotFoundBoth");
		title.SetLocalizedText("Notification_FindResult_NotFoundBoth_Title");
	}
	else if( inFoundInNext == false && inFoundInPrev == true )
	{
		//検索結果なし（後にはないが前にはある）
		text.SetLocalizedText("Notification_FindResult_NotFoundNext");
		title.SetLocalizedText("Notification_FindResult_NotFoundNext_Title");
		color = kColor_Blue;
	}
	else if( inFoundInNext == true && inFoundInPrev == false )
	{
		//検索結果なし（前にはないが後にはある）
		text.SetLocalizedText("Notification_FindResult_NotFoundPrev");
		title.SetLocalizedText("Notification_FindResult_NotFoundPrev_Title");
		color = kColor_Blue;
	}
	//検索テキスト取得、タイトルに表示
	AText	findtext;
	findtext.SetText(inFindText);
	findtext.ReplaceChar(kUChar_LineEnd,kUChar_Space);
	findtext.ReplaceChar(kUChar_Tab,kUChar_Space);
	title.AddCstring(" (");
	title.AddText(findtext);
	title.AddCstring(")");
	//同じ内容の項目がなければ、項目追加 （タイトルと本文いずれかが違っていれば、項目追加 #967）
	//#967 AIndex	index = mArray_Text.Find(text);
	//#967 if( index == kIndex_Invalid )
	if( mArray_Text.Find(text) == kIndex_Invalid || mArray_Title.Find(title) == kIndex_Invalid )//#967
	{
		//項目追加
		mArray_Text.Add(text);
		mArray_Title.Add(title);
		mArray_HideCounter.Add(60);
		mArray_Color.Add(color);
		mArray_Alpha.Add(1.0);
		mArray_Type.Add(kNotificationType_FindResult);
		mArray_URI.Add(GetEmptyText());//#1003
	}
	//描画更新
	NVI_Refresh();
	//ウインドウ高さ更新
	SPI_UpdateWindowHeight();
}

/**
未保存データがあるときの表示
*/
void	AView_Notification::SPI_SetNotification_UnsavedFile()
{
	//テキスト取得
	AText	text,title;
	text.SetLocalizedText("Notification_UnsavedFile");
	title.SetLocalizedText("Notification_UnsavedFile_Title");
	//未保存データのnotification未表示の場合のみ、項目追加する
	AIndex	index = mArray_Type.Find(kNotificationType_UnsavedFile);
	if( index == kIndex_Invalid )
	{
		mArray_Text.Add(text);
		mArray_Title.Add(title);
		mArray_HideCounter.Add(kNumber_MaxNumber);
		mArray_Color.Add(kColor_Red);
		mArray_Alpha.Add(1.0);
		mArray_Type.Add(kNotificationType_UnsavedFile);
		mArray_URI.Add(GetEmptyText());//#1003
	}
	//描画更新
	NVI_Refresh();
	//ウインドウ高さ更新
	SPI_UpdateWindowHeight();
}

/**
フォルダラベル（環境設定の「フォルダラベル」で設定したラベル）表示
*/
void	AView_Notification::SPI_SetNotification_FolderLabel( const AText& inFolderPath, const AText& inLabelText )
{
	//テキスト取得
	AText	text;
	text.SetLocalizedText("Notification_FolderLabel");
	text.ReplaceParamText('0',inFolderPath);
	//項目追加
	mArray_Text.Add(text);
	mArray_Title.Add(inLabelText);
	mArray_HideCounter.Add(kNumber_MaxNumber);
	mArray_Color.Add(kColor_Orange);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_FolderLabel);
	mArray_URI.Add(GetEmptyText());//#1003
	//描画更新
	NVI_Refresh();
	//ウインドウ高さ更新
	SPI_UpdateWindowHeight();
}

//#912
/**
インデント文字タイプ設定更新表示
*/
void	AView_Notification::SPI_SetNotification_IndentCharacterChanged( const AModeIndex inModeIndex,
																		const AText& inURI )//#1003
{
	AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(inModeIndex);
	//テキスト取得
	AText	text;
	if( modePrefDB.GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == false )
	{
		text.SetLocalizedText("Notification_IndentCharacterChanged_Tab");
	}
	else
	{
		text.SetLocalizedText("Notification_IndentCharacterChanged_Space");
		text.ReplaceParamText('0',modePrefDB.GetData_Number(AModePrefDB::kIndentWidth));
	}
	//
	AText	title;
	title.SetLocalizedText("Notification_IndentCharacterChanged_Title");
	//項目追加
	mArray_Text.Add(text);
	mArray_Title.Add(title);
	mArray_HideCounter.Add(60);
	mArray_Color.Add(kColor_Yellow);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_IndentCharacterChanged);
	mArray_URI.Add(inURI);//#1003
	//描画更新
	NVI_Refresh();
	//ウインドウ高さ更新
	SPI_UpdateWindowHeight();
}

//#912
/**
他アプリ編集結果読み込み表示
*/
void	AView_Notification::SPI_SetNotification_ReloadEditByOtherApp( const AText& inFileName )
{
	//テキスト取得
	AText	text;
	text.SetLocalizedText("Notification_ReloadEditByOtherApp");
	text.ReplaceParamText('0',inFileName);
	//
	AText	title;
	title.SetLocalizedText("Notification_ReloadEditByOtherApp_Title");
	//項目追加
	mArray_Text.Add(text);
	mArray_Title.Add(title);
	mArray_HideCounter.Add(60);
	mArray_Color.Add(kColor_Blue);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_ReloadEditByOtherApp);
	mArray_URI.Add(GetEmptyText());//#1003
	//描画更新
	NVI_Refresh();
	//ウインドウ高さ更新
	SPI_UpdateWindowHeight();
}

/**
汎用notification
*/
void	AView_Notification::SPI_SetNotification_General( const AText& inTitle, const AText& inText,
														 const AText& inURI )//#1003
{
	//項目追加
	mArray_Text.Add(inText);
	mArray_Title.Add(inTitle);
	mArray_HideCounter.Add(60);
	mArray_Color.Add(kColor_Blue);
	mArray_Alpha.Add(1.0);
	mArray_Type.Add(kNotificationType_General);
	mArray_URI.Add(inURI);//#1003
	//描画更新
	NVI_Refresh();
	//ウインドウ高さ更新
	SPI_UpdateWindowHeight();
}

/**
項目を閉じる（削除）
*/
void	AView_Notification::CloseBox( const AIndex inIndex )
{
	//項目削除
	mArray_Text.Delete1(inIndex);
	mArray_Title.Delete1(inIndex);
	mArray_HideCounter.Delete1(inIndex);
	mArray_Color.Delete1(inIndex);
	mArray_Alpha.Delete1(inIndex);
	mArray_Type.Delete1(inIndex);
	mArray_URI.Add(GetEmptyText());//#1003
	//描画更新
	NVI_Refresh();
	//ウインドウ高さ更新
	SPI_UpdateWindowHeight();
}

/**
ウインドウ高さ取得
*/
ANumber	AView_Notification::SPI_GetWindowHeight() const
{
	return mArray_Text.GetItemCount() * kHeight_ItemBoxWithMargin;
}

/**
ウインドウ高さ更新
*/
void	AView_Notification::SPI_UpdateWindowHeight()
{
	ANumber	height = SPI_GetWindowHeight();
	ARect	windowbounds = {0};
	NVM_GetWindow().NVI_GetWindowBounds(windowbounds);
	windowbounds.bottom = windowbounds.top + height;
	NVM_GetWindow().NVI_SetWindowBounds(windowbounds);
}


