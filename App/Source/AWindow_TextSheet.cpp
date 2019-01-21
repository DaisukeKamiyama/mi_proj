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

AWindow_TextSheet

*/

#include "stdafx.h"

#include "AWindow_TextSheet.h"
#include "ADocument_Text.h"
#include "AApp.h"
//#include "AUtil.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_CorrectEncoding
#pragma mark （テキストエンコーディング修正ウインドウ）
#pragma mark ===========================

/*
テキストエンコーディング修正ウインドウは以下の３つの場合に表示される
1. 「エンコーディングを変更して再読込」メニューを選択
2. 全てのエンコーディングを試したがどれもUTF-8に変換できなかった
3. 自動認識でエンコーディングを確定できなかった（モード設定で「テキストエンコーディングを確定できないときはダイアログを表示する」をOnにした場合のみ）
*/

//コンストラクタ
AWindow_TextSheet_CorrectEncoding::AWindow_TextSheet_CorrectEncoding(/*#199  AWindow_Text& inTextWindow */ const AWindowID inTextWindowID ):AWindow(/*#175NULL*/) /*#199, mTextWindow(inTextWindow)*/,mTextWindowID(inTextWindowID)
,mFallbackType(kTextEncodingFallbackType_None)//#473
,mWithAlert(false)
{
}

//ウインドウ生成
void	AWindow_TextSheet_CorrectEncoding::NVIDO_Create( const ADocumentID inDocumentID )
{
	mTextDocumentID = inDocumentID;
	NVI_SetFixTitle(true);//win 080726
	if( mWithAlert == false )
	{
		NVM_CreateWindow("main/TextEncodingCorrect");
	}
	else
	{
		NVM_CreateWindow("main/TextEncodingCorrect_WithAlert");
	}
	NVI_SetDefaultOKButton(kOKButton);
	NVI_SetDefaultCancelButton(kCancelButton);//B0430
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	
	GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_GetTextEncoding(mSavedTextEncodingName);
	
	NVI_RegisterTextArrayMenu(kTextEncodingMenu,kTextArrayMenuID_TextEncoding);
	NVI_SetControlText(kTextEncodingMenu,mSavedTextEncodingName);
	NVI_SetShowControl(kTextEncodingError,false);
	NVI_SetShowControl(kTextEncodingErrorIcon,false);
	
	NVI_RegisterToFocusGroup(kTextEncodingMenu,true);//#353
	NVI_RegisterToFocusGroup(kCancelButton,true);//#353
	NVI_RegisterToFocusGroup(kOKButton,true);//#353
#if IMPLEMENTATION_FOR_MACOSX
	NVI_RegisterToFocusGroup(90001,true);//#353
#endif
	
	//#473
	NVI_SetControlEnable(kErrorCharactersButton,false);
	
	//ヘルプボタン登録
	NVI_RegisterHelpAnchor(90001,"kihon.htm#textencoding");
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_CorrectEncoding::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

//#473
/**
ウインドウ生成・表示
*/
void	AWindow_TextSheet_CorrectEncoding::SPNVI_CreateAndShow( const ADocumentID inDocumentID,
															   const ABool inWithAlert, const ATextEncodingFallbackType inFallbackType )
{
	//変換エラー時画面か、エンコーディングを変換して再読込時画面かのフラグ
	mWithAlert = inWithAlert;
	//ウインドウ生成
	NVI_Create(inDocumentID);
	//
	if( inWithAlert == true )
	{
		//==================ドキュメントロード時、変換できなかった、または、フォールバック使用==================
		//アラート付きダイアログを表示
		AText	text1, text2;
		//
		if( inFallbackType == kTextEncodingFallbackType_None )
		{
			//------------------変換できなかった時------------------
			text1.SetLocalizedText("ModifyTextEncoding_CannotConvert_Title1");
			text2.SetLocalizedText("ModifyTextEncoding_CannotConvert_Title2");
			//キャンセル不可
			NVI_SetControlEnable(kCancelButton,false);
		}
		else
		{
			//------------------フォールバック使用時------------------
			text1.SetLocalizedText("ModifyTextEncoding_Fallback_Title1");
			text2.SetLocalizedText("ModifyTextEncoding_Fallback_Title2");
			//キャンセル可
			NVI_SetControlEnable(kCancelButton,true);
		}
		//テキスト設定
		NVI_SetControlText(kAlertTitle1,text1);
		NVI_SetControlText(kAlertTitle2,text2);
	}
	else
	{
		//==================「エンコーディングを変更して再読込」==================
		//キャンセル可
		NVI_SetControlEnable(kCancelButton,true);
	}
	//フォールバックタイプメッセージ更新
	UpdateFallbackTypeMessage(inFallbackType);
	//ウインドウ表示
	NVI_Show(true);
}

//#473
/**
フォールバックタイプメッセージ更新
*/
void	AWindow_TextSheet_CorrectEncoding::UpdateFallbackTypeMessage( const ATextEncodingFallbackType inFallbackType )
{
	//
	NVI_SetControlEnable(kErrorCharactersButton,false);
	//
	mFallbackType = inFallbackType;
	switch(mFallbackType)
	{
	  case kTextEncodingFallbackType_None:
		{
			NVI_SetShowControl(kTextEncodingError,false);
			NVI_SetShowControl(kTextEncodingErrorIcon,false);
			//エラー一覧ボタンdisable
			NVI_SetControlEnable(kErrorCharactersButton,false);
			break;
		}
	  case kTextEncodingFallbackType_Normal:
		{
			NVI_SetShowControl(kTextEncodingError,true);
			NVI_SetShowControl(kTextEncodingErrorIcon,true);
			AText	text;
			text.SetLocalizedText("ModifyTextEncoding_NormalFallback");
			NVI_SetControlText(kTextEncodingError,text);
			//エラー一覧ボタンenable
			NVI_SetControlEnable(kErrorCharactersButton,true);
			break;
		}
	  case kTextEncodingFallbackType_Lossless:
		{
			NVI_SetShowControl(kTextEncodingError,true);
			NVI_SetShowControl(kTextEncodingErrorIcon,true);
			AText	text;
			text.SetLocalizedText("ModifyTextEncoding_LosslessFallback");
			NVI_SetControlText(kTextEncodingError,text);
			//
			NVI_SetControlEnable(kErrorCharactersButton,true);
			break;
		}
	}
}

void	AWindow_TextSheet_CorrectEncoding::NVIDO_Show()
{
	/*#199 mTextWindow*/GetTextWindow().NVI_SelectWindow();
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_CorrectEncoding::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kTextEncodingMenu:
		{
			AText	text;
			NVI_GetControlText(kTextEncodingMenu,text);
			ATextEncodingFallbackType	resultFallbackType = kTextEncodingFallbackType_None;//#473
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_ModifyTextEncoding(text,resultFallbackType);//#473
			UpdateFallbackTypeMessage(resultFallbackType);//#473
			result = true;
			break;
		}
	  case kOKButton:
		{
			NVI_Close();
			result = true;
			break;
		}
	  case kCancelButton:
		{
			ATextEncodingFallbackType	resultFallbackType = kTextEncodingFallbackType_None;//#473
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_ModifyTextEncoding(mSavedTextEncodingName,resultFallbackType);
			NVI_Close();
			result = true;
			break;
		}
		//#473
	  case kErrorCharactersButton:
		{
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_ReportTextEncodingErrors();
			result = true;
			break;
		}
	}
	return result;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_CorrectEncoding::EVTDO_DoCloseButton()
{
	NVI_Close();
}

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_MoveLine
#pragma mark （行移動）
#pragma mark ===========================

//コンストラクタ
AWindow_TextSheet_MoveLine::AWindow_TextSheet_MoveLine( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID ):AWindow(/*#175NULL*/), /*#199 mTextWindow(inTextWindow)*/mTextWindowID(inTextWindowID)
{
}

//ウインドウ生成
void	AWindow_TextSheet_MoveLine::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ生成
	NVI_SetFixTitle(true);//win 080726
	NVM_CreateWindow("main/MoveLine");
	//#688 NVI_CreateEditBoxView(kNumber);
	NVI_RegisterToFocusGroup(kNumber);
	NVM_SetControlDataType(kNumber,kDataType_Number);
	NVI_SetDefaultOKButton(kOKButton);
	NVI_SetDefaultCancelButton(kCancelButton);//B0430
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	NVI_RegisterToFocusGroup(kCancelButton,true);//#353
	NVI_RegisterToFocusGroup(kOKButton,true);//#353
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_MoveLine::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

//ウインドウ表示
void	AWindow_TextSheet_MoveLine::SPNVI_Show( const AIndex inLineIndex, const AItemCount inLineCount, const ABool inParagraphMode )
{
	//
	mLineCount = inLineCount;
	mParagraphMode = inParagraphMode;
	//最初表示時の行（段落）番号を設定
	AText	text;
	text.SetFormattedCstring("%d",inLineIndex+1);//#1115
	NVI_SetControlText(kNumber,text);//#1115
	//#1115 NVI_SetControlNumber(kNumber,inLineIndex+1);
	//段落モードなら表示を”段落”にする
	if( mParagraphMode == true )
	{
		AText	text;
		text.SetLocalizedText("MoveParagraph1");
		NVI_SetControlText(kTitle,text);
	}
	text.SetFormattedCstring("/%d",inLineCount);
	NVI_SetControlText(kMax,text);
	//
	NVI_Show();
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_MoveLine::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kOKButton:
		{
			AIndex	lineIndex = NVI_GetControlNumber(kNumber)-1;
			if( lineIndex < 0 )   lineIndex = 0;
			if( lineIndex > mLineCount )  lineIndex = mLineCount-1;
			GetTextWindow().SPI_MoveToLine(lineIndex,mParagraphMode);
			NVI_Close();
			result = true;
			break;
		}
	  case kCancelButton:
		{
			NVI_Close();
			result = true;
			break;
		}
	}
	return result;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_MoveLine::EVTDO_DoCloseButton()
{
	NVI_Close();
}

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_FontSize
#pragma mark （フォントサイズ）
#pragma mark ===========================

//コンストラクタ
AWindow_TextSheet_FontSize::AWindow_TextSheet_FontSize( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID )
		: AWindow(/*#175NULL*/), /*#199 mTextWindow(inTextWindow)*/mTextWindowID(inTextWindowID)
{
}

//ウインドウ生成
void	AWindow_TextSheet_FontSize::NVIDO_Create( const ADocumentID inDocumentID )
{
	mTextDocumentID = inDocumentID;
	NVI_SetFixTitle(true);//win 080726
	NVM_CreateWindow("main/FontSize");
	NVI_CreateEditBoxView(kNumber);
	NVM_SetControlDataType(kNumber,kDataType_FloatNumber);
	NVI_SetDefaultOKButton(kOKButton);
	NVI_SetDefaultCancelButton(kCancelButton);//B0430
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	NVI_RegisterToFocusGroup(kCancelButton,true);//#353
	NVI_RegisterToFocusGroup(kOKButton,true);//#353
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_FontSize::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

//ウインドウ表示
void	AWindow_TextSheet_FontSize::NVIDO_Show()
{
	NVI_SetControlFloatNumber(kNumber,GetApp().SPI_GetTextDocumentByID(mTextDocumentID).GetDocPrefDB().GetData_FloatNumber(ADocPrefDB::kTextFontSize));
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_FontSize::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kOKButton:
		{
			AFloatNumber	fontsize;
			NVI_GetControlFloatNumber(kNumber,fontsize);
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetFontSize(fontsize);
			NVI_Close();
			result = true;
			break;
		}
	  case kCancelButton:
		{
			NVI_Close();
			result = true;
			break;
		}
	}
	return result;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_FontSize::EVTDO_DoCloseButton()
{
	NVI_Close();
}

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_WrapLetterCount
#pragma mark （行折り返し文字数）
#pragma mark ===========================

//コンストラクタ
AWindow_TextSheet_WrapLetterCount::AWindow_TextSheet_WrapLetterCount( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID ):AWindow(/*#175NULL*/), /*#199 mTextWindow(inTextWindow)*/mTextWindowID(inTextWindowID)
,mWrapMode(kWrapMode_LetterWrapByLetterCount)//#1113
{
}

//ウインドウ生成（AWindow::Show()からコールされる）
void	AWindow_TextSheet_WrapLetterCount::NVIDO_Create( const ADocumentID inDocumentID )
{
	mTextDocumentID = inDocumentID;
	NVI_SetFixTitle(true);//win 080726
	NVM_CreateWindow("main/WrapLetterCount");
	//#688 NVI_CreateEditBoxView(kNumber);
	NVM_SetControlDataType(kNumber,kDataType_FloatNumber);
	NVI_SetDefaultOKButton(kButton_OK);
	NVI_SetDefaultCancelButton(kButton_Cancel);//B0430
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	NVI_SetControlNumber(kNumber,GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_GetWrapLetterCount());
	NVI_RegisterToFocusGroup(kButton_Cancel,true);//#353
	NVI_RegisterToFocusGroup(kButton_OK,true);//#353
} 

//#1113
/**
行折り返しモード設定（OKクリック時のドキュメントへの設定値）
*/
void	AWindow_TextSheet_WrapLetterCount::SPI_SetWrapMode( const AWrapMode inWrapMode )
{
	mWrapMode = inWrapMode;
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_WrapLetterCount::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_WrapLetterCount::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kButton_OK:
		{
			ANumber num = NVI_GetControlNumber(kNumber);
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetWrapMode(mWrapMode,num);//#1113
			NVI_Close();
			result = true;
			break;
		}
	  case kButton_Cancel:
		{
			NVI_Close();
			result = true;
			break;
		}
	}
	return result;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_WrapLetterCount::EVTDO_DoCloseButton()
{
	NVI_Close();
}

#if 0
#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_Property
#pragma mark （プロパティ）
#pragma mark ===========================

//コンストラクタ
AWindow_TextSheet_Property::AWindow_TextSheet_Property( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID ):AWindow(/*#175 NULL*/), /*#199 mTextWindow(inTextWindow)*/mTextWindowID(inTextWindowID)
{
}

//ウインドウ生成（AWindow::Show()からコールされる）
void	AWindow_TextSheet_Property::NVIDO_Create( const ADocumentID inDocumentID )
{
	mTextDocumentID = inDocumentID;
	NVI_SetFixTitle(true);//win 080726
	NVM_CreateWindow("main/DocProp");
	NVI_RegisterToFocusGroup(kPopup_TextFontName,true);//#353
	NVI_CreateEditBoxView(kText_TextFontSize);//#244 ADocPrefDB::kTextFontSize);
	NVI_CreateEditBoxView(kText_TabWidth);//#244 ADocPrefDB::kTabWidth);
	NVI_CreateEditBoxView(kText_IndentWidth);//#244 ADocPrefDB::kIndentWidth);
	NVI_RegisterToFocusGroup(kPopup_AntiAliasMode,true);//#353
#if IMPLEMENTATION_FOR_MACOSX
	NVI_RegisterToFocusGroup(kCheck_SetCreatorType,true);//#353
	NVI_CreateEditBoxView(kText_Creator);
	NVI_CreateEditBoxView(kText_Type);
#endif
	NVI_RegisterToFocusGroup(kButton_Cancel,true);//#353
	NVI_RegisterToFocusGroup(kButton_OK,true);//#353
#if IMPLEMENTATION_FOR_MACOSX
	NVI_RegisterToFocusGroup(90001,true);//#353
#endif
	//win NVI_RegisterFontMenu(ADocPrefDB::kTextFont);
	NVI_RegisterFontMenu(kPopup_TextFontName,true);//#244 ADocPrefDB::kTextFontName);//win #375
	NVI_SetDefaultOKButton(kButton_OK);
	NVI_SetDefaultCancelButton(kButton_Cancel);
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	
	ADataBase&	docDB = GetApp().SPI_GetTextDocumentByID(mTextDocumentID).GetDocPrefDB();
	//win mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kTextFont,			docDB,ADocPrefDB::kTextFont			);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kTextFontName,		docDB,ADocPrefDB::kTextFontName		);//win
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kTextFontSize,		docDB,ADocPrefDB::kTextFontSize		);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kTabWidth,			docDB,ADocPrefDB::kTabWidth			);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kIndentWidth,		docDB,ADocPrefDB::kIndentWidth		);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kAntiAliasMode,		docDB,ADocPrefDB::kAntiAliasMode	);
	
	mBackupDataBase.CopyDataFromOtherDB(mLocalDataBase);
	
	//win NVM_RegisterDBData(ADocPrefDB::kTextFont,		true);
	NVM_RegisterDBData(kPopup_TextFontName,	ADocPrefDB::kTextFontName,	true);//win #244
	NVM_RegisterDBData(kText_TextFontSize,	ADocPrefDB::kTextFontSize,	true);//#244
	NVM_RegisterDBData(kText_TabWidth,		ADocPrefDB::kTabWidth,		true);//#244
	NVM_RegisterDBData(kText_IndentWidth,	ADocPrefDB::kIndentWidth,	true);//#244
	NVM_RegisterDBData(kPopup_AntiAliasMode,ADocPrefDB::kAntiAliasMode,	true);//#244
	
#if IMPLEMENTATION_FOR_MACOSX
	OSType	creator = GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_GetFileCreator();
	OSType	type = GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_GetFileType();
	AText	text;
	//AUtil::GetATextFromOSType(creator,text);
	text.SetFromOSType(creator);
	NVI_SetControlText(kText_Creator,text);
	//AUtil::GetATextFromOSType(type,text);
	text.SetFromOSType(type);
	NVI_SetControlText(kText_Type,text);
	
	if( creator == kOSTypeNULL && type == kOSTypeNULL )
	{
		 NVI_SetControlBool(kCheck_SetCreatorType,false);
	}
	else
	{
		NVI_SetControlBool(kCheck_SetCreatorType,true);
	}
#endif
	mSavedDirty = GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_IsDirty();
	
	//ヘルプボタン登録
	NVI_RegisterHelpAnchor(90001,"pref.htm#docpref");
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_Property::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

void	AWindow_TextSheet_Property::Adjust( const AControlID inControlID )
{
	switch(inControlID)
	{
	  /* win case ADocPrefDB::kTextFont:
		{
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetFont(mLocalDataBase.GetData_Font(ADocPrefDB::kTextFont));
			break;
		}*/
	  case kPopup_TextFontName://#244ADocPrefDB::kTextFontName:
		{
			AText	fontname;
			mLocalDataBase.GetData_Text(ADocPrefDB::kTextFontName,fontname);
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetFontName(fontname);
			break;
		}
	  case kText_TextFontSize://#244ADocPrefDB::kTextFontSize:
		{
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetFontSize(mLocalDataBase.GetData_FloatNumber(ADocPrefDB::kTextFontSize));
			break;
		}
	  case kPopup_AntiAliasMode://#244ADocPrefDB::kAntiAliasMode:
		{
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).
			SPI_SetAntiAliasMode(mLocalDataBase.GetData_Number(ADocPrefDB::kAntiAliasMode));
			break;
		}
	  case kText_TabWidth://#244ADocPrefDB::kTabWidth:
		{
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetTabWidth(mLocalDataBase.GetData_Number(ADocPrefDB::kTabWidth));
			break;
		}
	  case kText_IndentWidth://#244ADocPrefDB::kIndentWidth:
		{
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetIndentWidth(mLocalDataBase.GetData_Number(ADocPrefDB::kIndentWidth));
			break;
		}
	}
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_Property::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kButton_OK:
		{
			NVM_StoreToDBAll();
			//win Adjust(ADocPrefDB::kTextFont);
			Adjust(kPopup_TextFontName);//#244 ADocPrefDB::kTextFontName);//win
			Adjust(kText_TextFontSize);//#244 ADocPrefDB::kTextFontSize);
			Adjust(kPopup_AntiAliasMode);//#244 ADocPrefDB::kAntiAliasMode);
			Adjust(kText_TabWidth);//#244 ADocPrefDB::kTabWidth);
			Adjust(kText_IndentWidth);//#244 ADocPrefDB::kIndentWidth);
#if IMPLEMENTATION_FOR_MACOSX
			if( NVI_GetControlBool(kCheck_SetCreatorType) == true )
			{
				AText	text;
				NVI_GetControlText(kText_Creator,text);
				GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetFileCreator(text.GetOSTypeFromText());//AUtil::GetOSTypeFromAText(text));
				NVI_GetControlText(kText_Type,text);
				GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetFileType(text.GetOSTypeFromText());//AUtil::GetOSTypeFromAText(text));
			}
			else
			{
				GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetFileCreator(NULL);
				GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_SetFileType(NULL);
			}
#endif
			NVI_Close();
			result = true;
			break;
		}
	  case kButton_Cancel:
		{
			mLocalDataBase.CopyDataFromOtherDB(mBackupDataBase);
			//win Adjust(ADocPrefDB::kTextFont);
			Adjust(kPopup_TextFontName);//#244 ADocPrefDB::kTextFontName);//win
			Adjust(kText_TextFontSize);//#244 ADocPrefDB::kTextFontSize);
			Adjust(kPopup_AntiAliasMode);//#244 ADocPrefDB::kAntiAliasMode);
			Adjust(kText_TabWidth);//#244 ADocPrefDB::kTabWidth);
			Adjust(kText_IndentWidth);//#244 ADocPrefDB::kIndentWidth);
			NVI_UpdateProperty();//DB->UI（多分NVI_CloseでNVM_StoreToDBAllが呼ばれてUI->DBが実行されてしまうため）
			NVI_Close();
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_SetDirty(mSavedDirty);
			result = true;
			break;
		}
	}
	return result;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_Property::EVTDO_DoCloseButton()
{
	NVI_Close();
}

void	AWindow_TextSheet_Property::NVMDO_NotifyDataChanged( const AControlID inControlID, const AModificationType inModificationType, const AIndex inIndex,
		const AControlID inTriggerControlID/*B0406*/ )
{
	Adjust(inControlID);
}

void	AWindow_TextSheet_Property::NVMDO_UpdateControlStatus()
{
#if IMPLEMENTATION_FOR_MACOSX
	ABool	b = NVI_GetControlBool(kCheck_SetCreatorType);
	NVI_SetControlEnable(kText_Creator,b);
	NVI_SetControlEnable(kText_Type,b);
#endif
}
#endif
#if 0

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_PrintOption
#pragma mark （印刷オプション）
#pragma mark ===========================

//コンストラクタ
AWindow_TextSheet_PrintOption::AWindow_TextSheet_PrintOption( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID ) : AWindow(/*#175 NULL*/), /*#199 mTextWindow(inTextWindow)*/mTextWindowID(inTextWindowID)
{
}

//ウインドウ生成
void	AWindow_TextSheet_PrintOption::NVIDO_Create( const ADocumentID inDocumentID )
{
	mTextDocumentID = inDocumentID;
	NVI_SetFixTitle(true);//win 080726
	NVM_CreateWindow("main/PrintOption");
	//★
	/*#688
	NVI_RegisterToFocusGroup(kControl_UsePrintFont,true);//#353
	NVI_RegisterToFocusGroup(kControl_PrintFontName,true);//#353
	NVI_CreateEditBoxView(kControl_PrintFontSize);//#244 ADocPrefDB::kPrintOption_PrintFontSize);
	NVI_RegisterToFocusGroup(kControl_PrintPageNumber,true);//#353
	NVI_RegisterToFocusGroup(kControl_PageNumberFontName,true);//#353
	NVI_CreateEditBoxView(kControl_PageNumberFontSize);//#244 ADocPrefDB::kPrintOption_PageNumberFontSize);
	NVI_RegisterToFocusGroup(kControl_PrintFileName,true);//#353
	NVI_RegisterToFocusGroup(kControl_FileNameFontName,true);//#353
	NVI_CreateEditBoxView(kControl_FileNameFontSize);//#244 ADocPrefDB::kPrintOption_FileNameFontSize);
	NVI_RegisterToFocusGroup(kControl_PrintLineNumber,true);//#353
	NVI_RegisterToFocusGroup(kControl_LineNumberFontName,true);//#353
	NVI_RegisterToFocusGroup(kControl_ForceWordWrap,true);//#353
	NVI_RegisterToFocusGroup(kControl_PrintSeparateLine,true);//#353
	NVI_CreateEditBoxView(kControl_LineMargin);//#244 ADocPrefDB::kPrintOption_LineMargin);
	NVI_RegisterToFocusGroup(kButton_Cancel,true);//#353
	NVI_RegisterToFocusGroup(kButton_OK,true);//#353
#if IMPLEMENTATION_FOR_MACOSX
	NVI_RegisterToFocusGroup(90001,true);//#353
#endif
	NVI_RegisterFontMenu(kControl_PrintFontName,true);//#244 ADocPrefDB::kPrintOption_PrintFontName); #434
	NVI_RegisterFontMenu(kControl_PageNumberFontName,true);//#244 ADocPrefDB::kPrintOption_PageNumberFontName); #434
	NVI_RegisterFontMenu(kControl_FileNameFontName,true);//#244 ADocPrefDB::kPrintOption_FileNameFontName); #434
	NVI_RegisterFontMenu(kControl_LineNumberFontName,true);//#244 ADocPrefDB::kPrintOption_LineNumberFontName); #434
	*/
	NVI_SetDefaultOKButton(kButton_OK);
	NVI_SetDefaultCancelButton(kButton_Cancel);
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	
	ADataBase&	docDB = GetApp().SPI_GetTextDocumentByID(mTextDocumentID).GetDocPrefDB();
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_UsePrintFont,			docDB,ADocPrefDB::kPrintOption_UsePrintFont			);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_PrintPageNumber,		docDB,ADocPrefDB::kPrintOption_PrintPageNumber		);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_PrintFileName,			docDB,ADocPrefDB::kPrintOption_PrintFileName		);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_PrintLineNumber,		docDB,ADocPrefDB::kPrintOption_PrintLineNumber		);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_PrintSeparateLine,		docDB,ADocPrefDB::kPrintOption_PrintSeparateLine	);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_ForceWordWrap,			docDB,ADocPrefDB::kPrintOption_ForceWordWrap		);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_PrintFontName,			docDB,ADocPrefDB::kPrintOption_PrintFontName		);//win
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_PrintFontSize,			docDB,ADocPrefDB::kPrintOption_PrintFontSize		);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_PageNumberFontName,		docDB,ADocPrefDB::kPrintOption_PageNumberFontName	);//win
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_PageNumberFontSize,		docDB,ADocPrefDB::kPrintOption_PageNumberFontSize	);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_FileNameFontName,		docDB,ADocPrefDB::kPrintOption_FileNameFontName		);//win
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_FileNameFontSize,		docDB,ADocPrefDB::kPrintOption_FileNameFontSize		);
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_LineNumberFontName,		docDB,ADocPrefDB::kPrintOption_LineNumberFontName	);//win
	mLocalDataBase.CopyDataFromOtherDB(ADocPrefDB::kPrintOption_LineMargin,				docDB,ADocPrefDB::kPrintOption_LineMargin			);
	
	//★
	/*#688
	NVM_RegisterDBData(kControl_UsePrintFont,		ADocPrefDB::kPrintOption_UsePrintFont,		false);
	NVM_RegisterDBData(kControl_PrintFontName,		ADocPrefDB::kPrintOption_PrintFontName,		false,kControl_UsePrintFont);//win
	NVM_RegisterDBData(kControl_PrintFontSize,		ADocPrefDB::kPrintOption_PrintFontSize,		false,kControl_UsePrintFont);
	NVM_RegisterDBData(kControl_PrintPageNumber,	ADocPrefDB::kPrintOption_PrintPageNumber,	false);
	NVM_RegisterDBData(kControl_PageNumberFontName,	ADocPrefDB::kPrintOption_PageNumberFontName,false,kControl_PrintPageNumber);//win
	NVM_RegisterDBData(kControl_PageNumberFontSize,	ADocPrefDB::kPrintOption_PageNumberFontSize,false,kControl_PrintPageNumber);
	NVM_RegisterDBData(kControl_PrintFileName,		ADocPrefDB::kPrintOption_PrintFileName,		false);
	NVM_RegisterDBData(kControl_FileNameFontName,	ADocPrefDB::kPrintOption_FileNameFontName,	false,kControl_PrintFileName);//win
	NVM_RegisterDBData(kControl_FileNameFontSize,	ADocPrefDB::kPrintOption_FileNameFontSize,	false,kControl_PrintFileName);
	NVM_RegisterDBData(kControl_PrintLineNumber,	ADocPrefDB::kPrintOption_PrintLineNumber,	false);
	NVM_RegisterDBData(kControl_LineNumberFontName,	ADocPrefDB::kPrintOption_LineNumberFontName,false,kControl_PrintLineNumber);//win
	NVM_RegisterDBData(kControl_PrintSeparateLine,	ADocPrefDB::kPrintOption_PrintSeparateLine,	false);
	NVM_RegisterDBData(kControl_ForceWordWrap,		ADocPrefDB::kPrintOption_ForceWordWrap,		false);
	NVM_RegisterDBData(kControl_LineMargin,			ADocPrefDB::kPrintOption_LineMargin,		false);
	*/
	mSavedDirty = GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_IsDirty();
	
	//ヘルプボタン登録
	NVI_RegisterHelpAnchor(90001,"topic.htm#print");
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_PrintOption::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_PrintOption::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kButton_OK:
		{
			NVM_StoreToDBAll();
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).GetDocPrefDB().CopyDataFromOtherDB(mLocalDataBase);
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_SetDirty(true);
			NVI_Close();
			result = true;
			break;
		}
	  case kButton_Cancel:
		{
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_SetDirty(mSavedDirty);
			NVI_Close();
			result = true;
			break;
		}
	}
	return result;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_PrintOption::EVTDO_DoCloseButton()
{
	NVI_Close();
}
#endif

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_DropWarning
#pragma mark （DropWarning）
#pragma mark ===========================

//コンストラクタ
AWindow_TextSheet_DropWarning::AWindow_TextSheet_DropWarning( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID ):AWindow(/*#175 NULL*/), /*#199 mTextWindow(inTextWindow)*/mTextWindowID(inTextWindowID)
{
}

//ウインドウ生成（AWindow::Show()からコールされる）
void	AWindow_TextSheet_DropWarning::NVIDO_Create( const ADocumentID inDocumentID )
{
	mTextDocumentID = inDocumentID;
	NVI_SetFixTitle(true);//win 080726
	NVM_CreateWindow("main/ToolDropWarning");
	NVI_SetDefaultOKButton(kButton_Cancel);
	NVI_SetDefaultCancelButton(kButton_Cancel);//B0430
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	NVI_RegisterToFocusGroup(kButton_DontSaveExecute,true);//#353
	NVI_RegisterToFocusGroup(kButton_Cancel,true);//#353
	NVI_RegisterToFocusGroup(kButton_SaveExecute,true);//#353
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_DropWarning::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_DropWarning::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kButton_SaveExecute:
		{
			NVI_Close();
			GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_Save(false);
			/*#199 mTextWindow*/GetTextWindow().SPI_DoTool_Text(mTextDocumentID,mToolText,true);
			result = true;
			break;
		}
	  case kButton_DontSaveExecute:
		{
			NVI_Close();
			/*#199 mTextWindow*/GetTextWindow().SPI_DoTool_Text(mTextDocumentID,mToolText,true);
			result = true;
			break;
		}
	  case kButton_Cancel:
		{
			NVI_Close();
			result = true;
			break;
		}
	}
	return result;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_DropWarning::EVTDO_DoCloseButton()
{
	NVI_Close();
}

void	AWindow_TextSheet_DropWarning::SPI_SetToolText( const AText& inText )
{
	mToolText.SetText(inText);
}

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_EditByOtherApp
#pragma mark （他アプリ編集の反映）R0232
#pragma mark ===========================

//コンストラクタ
AWindow_TextSheet_EditByOtherApp::AWindow_TextSheet_EditByOtherApp( /*#199 AWindow_Text& inTextWindow*/const AWindowID inTextWindowID ):AWindow(/*#175 NULL*/), /*#199 mTextWindow(inTextWindow)*/mTextWindowID(inTextWindowID)
{
}

//ウインドウ生成（AWindow::Show()からコールされる）
void	AWindow_TextSheet_EditByOtherApp::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);
	NVM_CreateWindow("main/EditByOtherApp");
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	NVI_RegisterToFocusGroup(kButton_Reload,true);//#353
	NVI_RegisterToFocusGroup(kButton_NoReload,true);//#353
	//#545
	NVI_SetDefaultOKButton(kButton_Reload);
	//ヘルプボタン登録
	NVI_RegisterHelpAnchor(90000,"topic.htm#editbyotherapp");
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_EditByOtherApp::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_EditByOtherApp::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kButton_Reload:
		{
			GetApp().SPI_GetTextDocumentByID(mDocumentIDArray.Get(0)).SPI_ReloadEditByOtherApp(true);
			mDocumentIDArray.Delete1(0);
			if( mDocumentIDArray.GetItemCount() == 0 )
			{
				NVI_Close();
			}
			else
			{
				Update();
			}
			result = true;
			break;
		}
	  case kButton_NoReload:
		{
			GetApp().SPI_GetTextDocumentByID(mDocumentIDArray.Get(0)).SPI_ReloadEditByOtherApp(false);
			mDocumentIDArray.Delete1(0);
			if( mDocumentIDArray.GetItemCount() == 0 )
			{
				NVI_Close();
			}
			else
			{
				Update();
			}
			result = true;
			break;
		}
	}
	return result;
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_EditByOtherApp::EVTDO_DoCloseButton()
{
	NVI_Close();
}

//
void	AWindow_TextSheet_EditByOtherApp::SPI_AddDocument( const ADocumentID inDocumentID )
{
	//#655 既にキュー追加されているdocIDは再追加しない
	if( mDocumentIDArray.Find(inDocumentID) != kIndex_Invalid )
	{
		return;
	}
	//
	mDocumentIDArray.Add(inDocumentID);
	Update();
}

//
void	AWindow_TextSheet_EditByOtherApp::Update()
{
	if( mDocumentIDArray.GetItemCount() == 0 )   return;
	
	//
	ADocumentID	activeDocID = mDocumentIDArray.Get(0);
	//表示更新
	AText	title;
	GetApp().SPI_GetTextDocumentByID(activeDocID).NVI_GetTitle(title);
	AText	text;
	text.SetLocalizedText("AskEditByOtherApp");
	text.ReplaceParamText('0',title);
	NVI_SetControlText(kStaticText,text);
	//タブ選択
	AIndex	tabIndex = /*#199 mTextWindow*/GetTextWindow().NVI_GetTabIndexByDocumentID(activeDocID);
	/*#199 mTextWindow*/GetTextWindow().NVI_SelectTab(tabIndex);
	/*#199 mTextWindow*/GetTextWindow().NVI_SelectWindow();
}

#pragma mark ===========================
#pragma mark [クラス]AWindow_TextSheet_SCMCommit
#pragma mark （フォントサイズ）
#pragma mark ===========================
//#455

//コンストラクタ
AWindow_TextSheet_SCMCommit::AWindow_TextSheet_SCMCommit( const AWindowID inTextWindowID )
		: AWindow(), mTextWindowID(inTextWindowID), mButtonIsCommit(true), mTextDocumentID(kObjectID_Invalid)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kSCMCommitWindowPosition);
	NVI_AddToTimerActionWindowArray();
}

//ウインドウ生成
void	AWindow_TextSheet_SCMCommit::NVIDO_Create( const ADocumentID inDocumentID )
{
	mTextDocumentID = inDocumentID;
	NVI_SetFixTitle(true);
	NVM_CreateWindow("main/SCMCommit");
	//
	NVI_SetWindowStyleToSheet(GetTextWindow().GetObjectID());
	//
	AText	defaultfontname;
	AFontWrapper::GetAppDefaultFontName(defaultfontname);
	//
	NVI_CreateEditBoxView(kMessageText,false,false,true);
	NVI_GetEditBoxView(kMessageText).SPI_SetEnableEdiit(true);
    NVI_GetEditBoxView(kMessageText).SPI_SetTextDrawProperty(defaultfontname,9.0);//#1316,kColor_Black,kColor_Gray20Percent);
	NVI_GetEditBoxView(kMessageText).NVI_SetCatchReturn(true);
	NVI_GetEditBoxView(kMessageText).NVI_SetCatchTab(true);
	NVI_SetControlBindings(kMessageText,true,false,true,true,false,true);
	if( GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kCommitRecentMessage) > 0 )
	{
		AText	message;
		GetApp().GetAppPref().GetData_TextArray(AAppPrefDB::kCommitRecentMessage,0,message);
		NVI_SetControlText(kMessageText,message);
	}
	NVI_GetEditBoxView(kMessageText).NVI_SetSelectAll();
	//
	NVI_CreateEditBoxView(kAdditionalArgText,false,false,true);
	NVI_GetEditBoxView(kAdditionalArgText).SPI_SetEnableEdiit(true);
    NVI_GetEditBoxView(kAdditionalArgText).SPI_SetTextDrawProperty(defaultfontname,9.0);//#1316 ,kColor_Black,kColor_Gray20Percent);
	NVI_SetControlBindings(kAdditionalArgText,true,false,true,true,false,true);
	//
	NVI_CreateEditBoxView(kDiffText,true,true,true);
	NVI_GetEditBoxView(kDiffText).SPI_SetEnableEdiit(false);
    NVI_GetEditBoxView(kDiffText).SPI_SetTextDrawProperty(defaultfontname,9.0);//#1316 ,kColor_Black,kColor_Gray20Percent);
	NVI_SetControlBindings(kDiffText,true,true,true,true,false,false);
	//
	NVI_CreateEditBoxView(kResultText,false,false,true);
	NVI_GetEditBoxView(kResultText).SPI_SetEnableEdiit(false);
    NVI_GetEditBoxView(kResultText).SPI_SetTextDrawProperty(defaultfontname,9.0);//#1316 ,kColor_Black,kColor_Gray20Percent);
	NVI_SetControlBindings(kResultText,true,false,true,true,false,true);
	//
	NVI_SetDefaultOKButton(kCommitButton);
	NVI_SetDefaultCancelButton(kCancelButton);
	NVI_SetControlBindings(kCommitButton,false,false,true,true,true,true);
	NVI_SetControlBindings(kCancelButton,false,false,true,true,true,true);
	//
	NVI_SetControlIcon(kPrevDiff,kIconID_Tool_Up);
	NVI_SetControlIcon(kNextDiff,kIconID_Tool_Down);
	NVI_SetControlBindings(kPrevDiff,true,false,false,true,true,true);
	NVI_SetControlBindings(kNextDiff,true,false,false,true,true,true);
	//
	NVI_SetControlBindings(kDiffCount,true,true,false,false,true,true);
	NVI_SetControlBindings(kStaticText1,true,false,false,true,true,true);
	NVI_SetControlBindings(kStaticText2,true,false,false,true,true,true);
	NVI_SetControlBindings(kStaticText3,true,false,false,true,true,true);
	//
	NVI_RegisterToFocusGroup(kMessageText,false);
	NVI_RegisterToFocusGroup(kAdditionalArgText,false);
	NVI_RegisterToFocusGroup(kCancelButton,true);
	NVI_RegisterToFocusGroup(kCommitButton,true);
	NVI_RegisterToFocusGroup(kPrevDiff,true);
	NVI_RegisterToFocusGroup(kNextDiff,true);
	NVI_RegisterToFocusGroup(kRecentMessageMenu,true);
	
	//repository path設定
	AFileAcc	file;
	GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_GetFile(file);
	AText	filename;
	file.GetFilename(filename);
	AText	path;
	path.SetText(GetApp().SPI_GetSCMFolderManager().GetRepositoryPath(file));
	path.Add('/');
	path.AddText(filename);
	NVI_SetControlText(kPathText,path);
	
	//diff取得・設定
	UpdateDiff();
	
	//追加引数
	AText	text;
	GetApp().GetAppPref().GetData_Text(AAppPrefDB::kCommitAdditionalArgs,text);
	NVI_SetControlText(kAdditionalArgText,text);
	
	//最近記述したメッセージ更新
	UpdateRecentMessageMenu();
	NVI_SetControlBindings(kRecentMessageMenu,true,false,true,true,false,true);
	
	//Commitボタンのモード
	mButtonIsCommit = true;
	
	//下部のtab viewをメッセージ等入力するほうのタブにする
	NVI_SetControlBool(kTabViewSelect_Normal,true);
	
	//ウインドウ最小サイズ設定
	NVI_SetWindowMinimumSize(500,400);
	
	//
	NVI_SetWindowWidth(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCommitWindowWidth));
	NVI_SetWindowHeight(GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCommitWindowHeight));
	
	//フォーカス設定
	NVI_SwitchFocusTo(kMessageText);
}

/**
*/
void	AWindow_TextSheet_SCMCommit::NVIDO_Hide()
{
	//ウインドウ幅の保存
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kCommitWindowWidth,NVI_GetWindowWidth());
	GetApp().GetAppPref().SetData_Number(AAppPrefDB::kCommitWindowHeight,NVI_GetWindowHeight());
	//
	AText	text;
	NVI_GetControlText(kAdditionalArgText,text);
	GetApp().GetAppPref().SetData_Text(AAppPrefDB::kCommitAdditionalArgs,text);
}

/**
diff取得・設定
*/
void	AWindow_TextSheet_SCMCommit::UpdateDiff()
{
	//
	AFileAcc	file, folder;
	GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_GetFile(file);
	folder.SpecifyParent(file);
	AText	filename;
	file.GetFilename(filename);
	ATextEncoding	tec = GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_GetTextEncoding();
	
	//SCMタイプ取得 #589
	ASCMFolderType	scmFolderType = GetApp().SPI_GetSCMFolderManager().GetSCMFolderType(file);
	
	AText	command;
	ATextArray	argArray;
	switch(scmFolderType)
	{
	  case kSCMFolderType_SVN:
		{
			//コマンドとしてはenvを実行。日本語ファイル名をsvnに渡すため。 #1087
			command.SetCstring("/usr/bin/env");
			argArray.Add(command);
			AText	t;
			t.SetCstring("LANG=UTF-8");
			argArray.Add(t);
			//svnコマンドパス
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kSVNPath,t);
			argArray.Add(t);
			//引数
			t.SetCstring("diff");
			argArray.Add(t);
			/*
			if( false )
			{
				//外部diff指定
				t.SetCstring("--diff-cmd");
				argArray.Add(t);
				GetApp().GetAppPref().GetData_Text(AAppPrefDB::kDiffPath,t);
				argArray.Add(t);
				//diffオプション指定
				t.SetCstring("-x");
				argArray.Add(t);
				GetApp().GetAppPref().GetData_Text(AAppPrefDB::kDiffOption,t);//ダブルクオーテーションはシェル経由ではないので不要
				if( t.GetItemCount() > 0 )
				{
					t.Add(' ');
				}
				t.AddCstring("--normal");
				argArray.Add(t);
			}
			*/
			//ファイル名
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			//★SVNと同様の#1087対応必要かも
			//gitコマンドパス
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kGitPath,command);
			//引数
			argArray.Add(command);
			AText	t;
			t.SetCstring("diff");
			argArray.Add(t);
			//ファイル名
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//カレントディレクトリ
	AText	dirpath;
	folder.GetNormalizedPath(dirpath);
	//実行
	AText	resulttext;
	GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,dirpath,resulttext);
	//UTF-8へ変換（対象ドキュメントのTextEncodingから変換）
	resulttext.ConvertToUTF8(tec);
	resulttext.ConvertReturnCodeToCR();
	//行スタイル設定・変更点カウント
	AItemCount	diffcount = 0;
	AArray<ATextStyle>	styleArray;
	AArray<AColor>	colorArray;
	for( AIndex pos = 0; pos < resulttext.GetItemCount(); )
	{
		AText	line;
		resulttext.GetLine(pos,line);
		ATextStyle	style = kTextStyle_Normal;
		AColor	color = AColorWrapper::GetColorByHTMLFormatColor("000000");//#1316 kColor_Black;
		if( AApplication::GetApplication().NVI_IsDarkMode() == true )
		{
			color = AColorWrapper::GetColorByHTMLFormatColor("FFFFFF");
		}
		if( line.GetItemCount() > 0 )
		{
			AUChar	ch = line.Get(0);
			switch(ch)
			{
			  case '@':
				{
					style = kTextStyle_Underline | kTextStyle_Bold;
					//#1316 GetApp().NVI_GetAppPrefDB().GetData_Color(AAppPrefDB::kDiffColor_Changed,color);
					color = AColorWrapper::GetColorByHTMLFormatColor("007500");
					if( AApplication::GetApplication().NVI_IsDarkMode() == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("00D000");
					}
					diffcount++;
					break;
				}
			  case '+':
				{
					//#1316 GetApp().NVI_GetAppPrefDB().GetData_Color(AAppPrefDB::kDiffColor_Added,color);
					color = AColorWrapper::GetColorByHTMLFormatColor("0000FF");
					if( AApplication::GetApplication().NVI_IsDarkMode() == true )
					{
						color = AColorWrapper::GetColorByHTMLFormatColor("00C0FF");
					}
					break;
				}
			  case '-':
				{
					//#1316 GetApp().NVI_GetAppPrefDB().GetData_Color(AAppPrefDB::kDiffColor_Deleted,color);
					color = AColorWrapper::GetColorByHTMLFormatColor("FF0000");
					break;
				}
			}
			styleArray.Add(style);
			colorArray.Add(color);
		}
	}
	//Diffテキストボックス設定
	NVI_GetEditBoxView(kDiffText).NVI_SetText(resulttext);
	NVI_GetEditBoxView(kDiffText).SPI_SetLineStyle(styleArray,colorArray);
	//Diffカウント設定
	AText	diffcounttext;
	//#0 diffcounttext.SetFormattedCstring("(%d)",diffcount);
	diffcounttext.SetLocalizedText("CommitWindow_DiffCount");
	AText	diffcountnum;
	diffcountnum.SetNumber(diffcount);
	diffcounttext.ReplaceParamText('0',diffcountnum);
	NVI_SetControlText(kDiffCount,diffcounttext);
}

//TextWindow取得
AWindow_Text&	AWindow_TextSheet_SCMCommit::GetTextWindow()
{
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Text,kWindowType_Text,mTextWindowID);
}

//コントロールのクリック時のコールバック
ABool	AWindow_TextSheet_SCMCommit::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kCommitButton:
		{
			if( mButtonIsCommit == true )
			{
				//コミットボタンクリック
				NVI_SetControlEnable(kCommitButton,false);
				NVI_SetControlEnable(kRecentMessageMenu,false);
				NVI_SetControlEnable(kMessageText,false);
				NVI_SetControlEnable(kAdditionalArgText,false);
				NVI_SetControlEnable(kCancelButton,false);
				//#0 ここはタブ切り替え前なので下へ移動 NVI_SwitchFocusTo(kResultText);
				NVI_RefreshWindow();
				NVI_UpdateWindow();
				AddRecentMessage();
				Commit();
				
				//下部のtab viewを結果表示するほうのタブにする
				NVI_SetControlBool(kTabViewSelect_Result,true);
				NVI_SwitchFocusTo(kResultText);//#0
				
				//
				AText	text("OK");
				NVI_SetControlText(kCommitButton,text);
				NVI_SetControlEnable(kCommitButton,true);
				mButtonIsCommit = false;
				//
				AFileAcc	file;
				GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_GetFile(file);
				GetApp().SPI_GetSCMFolderManager().UpdateStatus(file);
				GetApp().SPI_GetTextDocumentByID(mTextDocumentID).SPI_DoRepositoryUpdated();
				GetApp().NVI_RefreshAllWindow();
			}
			else
			{
				//OKボタンクリック
				NVI_Close();
			}
			result = true;
			break;
		}
	  case kCancelButton:
		{
			NVI_Close();
			result = true;
			break;
		}
	  case kOKButton:
		{
			NVI_Close();
			result = true;
			break;
		}
	  case kPrevDiff:
		{
			GetTextWindow().SPI_PrevDiff();
			result = true;
			break;
		}
	  case kNextDiff:
		{
			GetTextWindow().SPI_NextDiff();
			result = true;
			break;
		}
		//最近記述したメッセージ
	  case kRecentMessageMenu:
		{
			AIndex	index = NVI_GetControlNumber(kRecentMessageMenu);
			if( index-1 >= GetApp().GetAppPref().GetItemCount_TextArray(AAppPrefDB::kCommitRecentMessage) )
			{
				//メニューを消去
				GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kCommitRecentMessage);
				UpdateRecentMessageMenu();
			}
			else if( index > 0 )
			{
				//メッセージ設定
				AText	text;
				NVI_GetControlText(kRecentMessageMenu,text);
				NVI_SetControlText(kMessageText,text);
			}
			NVI_SetControlNumber(kRecentMessageMenu,0);
			result = true;
			break;
		}
	}
	return result;
}

/**
コミット実行
*/
void	AWindow_TextSheet_SCMCommit::Commit()
{
	//メッセージ取得
	AText	message;
	NVI_GetControlText(kMessageText,message);
	
	//
	AFileAcc	file, folder;
	GetApp().SPI_GetTextDocumentByID(mTextDocumentID).NVI_GetFile(file);
	folder.SpecifyParent(file);
	AText	filename;
	file.GetFilename(filename);
	
	//SCMタイプ取得 #589
	ASCMFolderType	scmFolderType = GetApp().SPI_GetSCMFolderManager().GetSCMFolderType(file);
	
	AText	command;
	ATextArray	argArray;
	switch(scmFolderType)
	{
	  case kSCMFolderType_SVN:
		{
			//コマンドとしてはenvを実行。日本語ファイル名をsvnに渡すため。 #1087
			command.SetCstring("/usr/bin/env");
			argArray.Add(command);
			AText	t;
			t.SetCstring("LANG=UTF-8");
			argArray.Add(t);
			//svnコマンドパス
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kSVNPath,t);
			argArray.Add(t);
			//引数
			t.SetCstring("commit");
			argArray.Add(t);
			//--encoding
			t.SetCstring("--encoding");
			argArray.Add(t);
			//UTF-8
			t.SetCstring("UTF-8");
			argArray.Add(t);
			//-m
			t.SetCstring("-m");
			argArray.Add(t);
			//message
			argArray.Add(message);
			//file
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			break;
		}
		//#589
	  case kSCMFolderType_Git:
		{
			//★SVNと同様の#1087対応必要かも
			//gitコマンドパス
			GetApp().GetAppPref().GetData_Text(AAppPrefDB::kGitPath,command);
			//引数
			argArray.Add(command);
			AText	t;
			t.SetCstring("commit");
			argArray.Add(t);
			//-m
			t.SetCstring("-m");
			argArray.Add(t);
			//message
			argArray.Add(message);
			//空文字メッセージを許す
			t.SetCstring("--allow-empty-message");
			argArray.Add(t);
			//file
			AFileAcc::GetNormalizedPathString(filename);//#1087
			argArray.Add(filename);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//カレントディレクトリ
	AText	dirpath;
	folder.GetNormalizedPath(dirpath);
	//実行
	AText	resulttext;
	GetApp().SPI_GetChildProcessManager().ExecuteGeneralSyncCommand(command,argArray,dirpath,resulttext);
	
	//結果
	resulttext.ConvertLineEndToCR();
	NVI_SetControlText(kResultText,resulttext);
}

/**
ウインドウ閉じるボタン
*/
void	AWindow_TextSheet_SCMCommit::EVTDO_DoCloseButton()
{
	NVI_Close();
}

/**
最近記述したメッセージメニューの更新
*/
void	AWindow_TextSheet_SCMCommit::UpdateRecentMessageMenu()
{
	ATextArray	textArray;
	textArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kCommitRecentMessage));
	AText	text;
	text.SetLocalizedText("Commit_RecentMessage");
	textArray.Insert1(0,text);
	text.SetLocalizedText("RecentlyMenus_DeleteAll");
	textArray.Add(text);
	NVI_SetMenuItemsFromTextArray(kRecentMessageMenu,textArray);
}

/**
最近記述したメッセージの追加
*/
void	AWindow_TextSheet_SCMCommit::AddRecentMessage()
{
	AText	text;
	NVI_GetControlText(kMessageText,text);
	GetApp().GetAppPref().AddRecentlyUsed_TextArray(AAppPrefDB::kCommitRecentMessage,text,128);
	UpdateRecentMessageMenu();
}

