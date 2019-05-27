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

AView_Text

*/
/*
AView_Text設計ポリシー

・AView_Text生成時、常にドキュメントが存在する。（ドキュメントが存在しない状態でViewを動作させることは元々考慮されていないため）(110323)

*/

#include "stdafx.h"

#include "AView_Text.h"
#include "AWindow_Text.h"
#include "ADocument_Text.h"
#include "AApp.h"
//#include "../Wrapper.h"
//#include "../Imp.h"
//#include "AUtil.h"
#include "ADocument_IndexWindow.h"
//#858 #include "AWindow_Fusen.h"//#138
//#858 #include "AWindow_FusenEdit.h"//#138
#include "AView_IdInfo.h"//#238
#include "AView_LineNumber.h"//#450
#include "AView_CallGraf.h"
#include "AWindow_Notification.h"
#include "AView_Notification.h"

#if IMPLEMENTATION_FOR_MACOSX
const ANumber kBraceCheckTimerCount = 5;//#688 20;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const ANumber kBraceCheckTimerCount = 3;
#endif

//#456 ドラッグスクロールを行う部分を画面上端下端よりも少し内側にする
const ANumber	kDragScrollMargin = 8;

//#867
//idle work timerタイマー値
const ANumber	kKeyIdleWorkTimerValue = 30;
const ANumber	kMouseClikcIdleWorkTimerValue = 3;

#pragma mark ===========================
#pragma mark [クラス]AView_Text
#pragma mark ===========================
//テキスト表示View
//親ウインドウはAWindowであれば良く、AWindow_Text以外でも使用可

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_Text::AView_Text( /*#199 AObjectArrayItem* inParent, AWindow& inWindow*/
			const AWindowID inWindowID, const AControlID inID, 
			/*#695 const ADocumentID inTextDocumentID*/ADocument_Text& inTextDocument, 
			const AWindowID inTextWindowID, //#379
			/*#699 const ABool inCaretMode, */
			/*#699 const ATextPoint& inCaretTextPoint, const ATextPoint& inSelectTextPoint, */const AImagePoint& inOriginOfLocalFrame,
			const ABool inCrossCaretMode )//B0345
: AView(/*#199 inParent,inWindow*/inWindowID,inID,inOriginOfLocalFrame), //#695 mTextDocumentID(inTextDocumentID),
		mCaretMode(/*#699 inCaretMode*/false), 
		mCaretTextPoint(/*#699 inCaretTextPoint*/kTextPoint_00), 
		mSelectTextPoint(/*#699 inSelectTextPoint*/kTextPoint_00), 
		mCaretImagePoint(kImagePoint_00),mSelectImagePoint(kImagePoint_00),//#831
		mCaretTickCount(0), mCaretBlinkStateVisible(false), mInlineInputNotFixedTextLen(0), 
		mOldArrowImageX(0), mForceSelectMode(false), mDragDrawn(false), mDragWordDragDrop(false), mPrinting(false), mBackgroundImageOffset(0),
		/*#717 mLastAbbrevKeywordIndex(kIndex_Invalid), */mLineMargin(0), /*#379mTextWindowID(kObjectID_Invalid),*/ mDisplayingBraceCheck(false), mKeyToolMode(false),
		mKukeiSelectionMode(false), mKukeiSelected(false), mCrossCaretMode(true/*inCrossCaretMode*/), mCrossCaretHVisible(false), mCrossCaretVVisible(false), mDragging(false),
		mMouseTrackingMode(kMouseTrackingMode_None), /*#688 mMouseTrackByLoop(true),*/ mTempCaretHideCount(0)
		,mSelectionByFind(false)//#262
		,mToolData_DontShowDropWarning(false)//#308 ここが問題の原因かどうかは不明だが
		,mFigMode(kFigMode_None)//#478
		,mLineNumberViewID(kObjectID_Invalid)//#450
		,mTextWindowID(inTextWindowID)//#379
		,mDisableAdjustDiffDisplayScrollAtScrollPostProcess(false)//#611
,mTextDocumentRef(inTextDocument)//#695
,mLineImageInfo_Height(this,kLineInfoInitialAllocation,kLineInfoAllocationStep)//#695
,mLineImageInfo_ImageY(this,kLineInfoInitialAllocation,kLineInfoAllocationStep)//#695
,mLineImageInfo_Collapsed(this,kLineInfoInitialAllocation,kLineInfoAllocationStep)//#695
,mReservedSelection_StartTextIndex(kIndex_Invalid),mReservedSelection_EndTextIndex(kIndex_Invalid)//#699
,mReservedSelection_StartTextPoint(kTextPoint_Invalid)//#699
,mReservedSelection_EndTextPoint(kTextPoint_Invalid)//#699
,mReservedSelection_IsSelectWithParagraph(false)//#699
,mAbbrevInputMode(false),mAbbrevCurrentCandidateIndex(kIndex_Invalid),mAbbrevRequestInputKeyCount(0)//#717
,mIdInfoCurrentArgHighlighted(false),mIdInfoCurrentArgStartTextPoint(kTextPoint_00),mIdInfoCurrentArgEndTextPoint(kTextPoint_00)//#853
,mKeyIdleWorkType(kKeyIdleWorkType_None),mKeyIdleWorkTimer(0)//#867
,mAutoSaveUnsavedCheckTickCount(0)//#81
,mPopupNotificationWindowID(kObjectID_Invalid)//#725
,mCurrentHeaderPathJumpMenuItemIndex(kIndex_Invalid)//#906
,mCurrentCaretStateColorSlotIndex(kIndex_Invalid)
,mMouseDownTick(0)
,mKeyToolMenuObjectID_AppTool(kObjectID_Invalid),mKeyToolMenuStartIndex_AppTool(kIndex_Invalid),mKeyToolMenuEndIndex_AppTool(kIndex_Invalid)
,mKeyToolMenuObjectID_UserTool(kObjectID_Invalid),mKeyToolMenuStartIndex_UserTool(kIndex_Invalid),mKeyToolMenuEndIndex_UserTool(kIndex_Invalid)
,mSuppressRequestCandidateFinder(false)
,mAbbrevCandidateState_Requesting(false),mAbbrevCandidateState_Displayed(false)
,mCurrentMouseTrackModifierKeys(0)//#795
,mCurrentMouseTrackInsideLineNumber(false)
,mCandidateRequestTimer(0)
,mLastCaretLocalPoint(kLocalPoint_00)//#1031
,mACharWidth(9)//#1186
,mZenkakuAWidth(18)//#1385
,mBraceHighlighted(false),mBraceHighlightStartTextPoint1(kTextPoint_00),mBraceHighlightEndTextPoint1(kTextPoint_00),mBraceHighlightStartTextPoint2(kTextPoint_00),mBraceHighlightEndTextPoint2(kTextPoint_00)//#1406
{
	NVMC_SetOffscreenMode(true);//win
	//#92 初期化はNVIDO_Init()へ移動
	
	//選択設定（ドキュメントの行折り返し計算未完了なら選択予約になる）#699
	//text view生成時には選択設定しない。#699
	//#699 SetSelect(inCaretTextPoint,inSelectTextPoint);
	
	//viewを不透明に設定 #1090
	NVMC_SetOpaque(true);
	
	//#913
	//マーク初期化
	mMarkStartTextPoint.x = mMarkStartTextPoint.y = -1;
	mMarkEndTextPoint.x = mMarkEndTextPoint.y = -1;
	//左下が丸いのを考慮したスクロールをしないように設定（text viewでは必要ないので） #947
	NVI_SetAdjustScrollForRoundedCorner(false);
	
	//サービスメニュー可否設定 #1309
	NVI_SetServiceMenuAvailable();
}

//デストラクタ
AView_Text::~AView_Text()
{
	//#92 NVIDO_DoDeleted()へ移動 HideCandidateWindow();
}

//#138
void	AView_Text::NVIDO_DoDeleted()
{
	//#725
	//ポップアップnotificationウインドウ削除
	if( mPopupNotificationWindowID != kObjectID_Invalid )
	{
		GetApp().NVI_DeleteWindow(mPopupNotificationWindowID);
	}
	/*#858
	//#138
	//付箋紙ウインドウを全て削除
	for( AIndex i = 0; i < mFusenArray_WindowID.GetItemCount(); i++ )
	{
		GetApp().NVI_DeleteWindow(mFusenArray_WindowID.Get(i));
	}
	mFusenArray_WindowID.DeleteAll();
	*/
	//DocumentオブジェクトのView登録を解除
	GetTextDocument().NVI_UnregisterView(GetObjectID());//#92 SPI_UnregisterTextViewID(GetObjectID());
	//#92
	//補完入力状態解除
	ClearAbbrevInputMode(true,false);
}

//win 080709
void	AView_Text::NVIDO_Init()
{
	mZenkakuSpaceText.SetLocalizedText("Text_ZenkakuSpace");
	mLineEndDisplayText.SetLocalizedText("ReturnCodeText");
	mLineEndDisplayText_UTF16.SetText(mLineEndDisplayText);//B0000
	mLineEndDisplayText_UTF16.ConvertFromUTF8ToUTF16();//B0000
	
	//#699 SPI_UpdateTextDrawProperty();
	//#699 SPI_InitLineImageInfo();//#450
	GetTextDocument().NVI_RegisterView(GetObjectID());//#379
	
	//縦書きモード設定 #558
	NVI_SetVerticalMode(GetTextDocumentConst().SPI_GetVerticalMode());
	
	//
	SPI_UpdateTextDrawProperty();
	SPI_InitLineImageInfo();//#450
	
	//DocumentオブジェクトへViewを登録
	//#379 GetTextDocument().NVI_RegisterView(GetObjectID());//#92 SPI_RegisterTextViewID(GetObjectID());//#138
	
	/*#831 高速化。AView_Text生成時にはcaret pointを設定しないようにしたので、ここでのimage point取得は必要ない。
	GetImagePointFromTextPoint(GetCaretTextPoint(),mCaretImagePoint);
	//R0108
	GetImagePointFromTextPoint(GetSelectTextPoint(),mSelectImagePoint);
	*/
	//win
	NVI_SetDefaultCursor(kCursorType_IBeam);
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	//#236 NVMC_EnableDrop(kScrapType_UnicodeText);
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_UnicodeText);
	scrapTypeArray.Add(kScrapType_File);//win
	scrapTypeArray.Add(kScrapType_TabSelector);//#859
	scrapTypeArray.Add(kScrapType_IdInfoItem);
	scrapTypeArray.Add(kScrapType_CallGrafItem);
	NVMC_EnableDrop(scrapTypeArray);
	
	//caret point初期化
	SetCaretTextPoint(kTextPoint_00);
}

//#379
/**
TextViewの幅（ウインドウ幅行折り返し計算用）取得
*/
ANumber	AView_Text::SPI_GetTextViewWidth() const
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		//InfoPange表示等を考慮したビュー幅を返す
		return GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetTextViewWidth(/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID());
	}
	else
	{
		return NVI_GetViewWidth();
	}
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

/*#695
//TextDocument取得
ADocument_Text&	AView_Text::GetTextDocument()
{
	MACRO_RETURN_DOCUMENT_DYNAMIC_CAST(ADocument_Text,kDocumentType_Text,mTextDocumentID);//#199
	//#199 return GetApp().SPI_GetTextDocumentByID(mTextDocumentID);
}
//TextDocument取得
const ADocument_Text&	AView_Text::GetTextDocumentConst() const
{
	MACRO_RETURN_CONSTDOCUMENT_DYNAMIC_CAST(ADocument_Text,kDocumentType_Text,mTextDocumentID);//#199
	//#199 return GetApp().SPI_GetTextDocumentConstByID(mTextDocumentID);
}
*/
//DocPrefDB取得
ADocPrefDB&	AView_Text::GetDocPrefDB()
{
	return GetTextDocument().GetDocPrefDB();
}

//ModePrefDB取得
AModePrefDB&	AView_Text::GetModePrefDB()
{
	return GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex());
}

//#450
/**
*/
void	AView_Text::SPI_SetLineNumberView( const AViewID inLineNumberViewID )
{
	mLineNumberViewID = inLineNumberViewID;
}

//#450
/**
*/
AView_LineNumber&	AView_Text::GetLineNumberView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_LineNumber,kViewType_LineNumber,mLineNumberViewID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//メニュー選択時処理
ABool	AView_Text::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	RestoreFromBraceCheck();
	//補完入力状態解除
	ClearAbbrevInputMode(true,false);//RC2
	//
	ABool	result = true;
	switch(inMenuItemID)
	{
		//Undo
	  case kMenuItemID_Undo:
		{
			UndoRedo(true);
			break;
		}
		//Redo
	  case kMenuItemID_Redo:
		{
			UndoRedo(false);
			break;
		}
		//Save
	  case kMenuItemID_Save:
		{
			GetTextDocument().SPI_Save(false);
			break;
		}
		//SaveAs
	  case kMenuItemID_SaveAs:
		{
			GetTextDocument().SPI_Save(true);
			break;
		}
		//印刷 #524 AWindow_Textから移動
	  case kMenuItemID_Print:
		{
			GetTextDocument().SPI_Print();
			break;
		}
		//選択範囲を印刷 #524
	  case kMenuItemID_PrintRange:
	  case kMenuItemID_CM_PrintRange:
		{
			ATextPoint	spt = {0,0}, ept = {0,0};
			SPI_GetSelect(spt,ept);
			AIndex	start = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
			AIndex	end = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y);
			if( ept.x != 0 )
			{
				end++;
			}
			GetTextDocument().SPI_PrintWithRange(start,end);
			break;
		}
		//コピー
	  case kMenuItemID_Copy:
	  case kMenuItemID_CM_Copy:
		{
			//#900
			//キー記録中なら記録
			GetApp().SPI_RecordKeybindAction(keyAction_Copy,GetEmptyText());
			//
			Copy(false);
			break;
		}
		//追加コピー
	  case kMenuItemID_AdditionalCopy:
		{
			//#900
			//キー記録中なら記録
			GetApp().SPI_RecordKeybindAction(keyAction_additionalcopy,GetEmptyText());
			//
			AText	scrapText;
			AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,GetTextDocument().SPI_GetPreferLegacyTextEncoding(),true);
			AText	text;
			GetSelectedText(text);
			scrapText.AddText(text);
			AScrapWrapper::SetClipboardTextScrap(scrapText,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocument().SPI_GetPreferLegacyTextEncoding(),true);
			break;
		}
		//カット
	  case kMenuItemID_Cut:
	  case kMenuItemID_CM_Cut:
		{
			//#900
			//キー記録中なら記録
			GetApp().SPI_RecordKeybindAction(keyAction_Cut,GetEmptyText());
			//
			Cut(false);
			break;
		}
		//ペースト
	  case kMenuItemID_Paste:
	  case kMenuItemID_CM_Paste:
		{
			//#900
			//キー記録中なら記録
			GetApp().SPI_RecordKeybindAction(keyAction_Paste,GetEmptyText());
			//
			Paste();
			break;
		}
		//#929
	  case kMenuItemID_PasteAtEndOfEachLine:
		{
			PasteAtEndOfEachLine();
			break;
		}
		//削除
	  case kMenuItemID_Clear:
	  case kMenuItemID_CM_Clear:
		{
			DeleteSelect(undoTag_DELKey);
			break;
		}
		//全てを選択
	  case kMenuItemID_SelectAll:
	  case kMenuItemID_CM_SelectAll:
		{
			SetSelect(0,GetTextDocumentConst().SPI_GetTextLength());
			break;
		}
		//#878
		//FIFOクリップボードモード
	  case kMenuItemID_SwitchFIFOClipboardMode:
		{
			SwitchFIFOClipboardMode();
			break;
		}
		//インデント
	  case kMenuItemID_Indent:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			DoIndent(/*#650 false,*/undoTag_Indent,inputSpaces,kIndentTriggerType_IndentFeature);//#249 #639 #650
			break;
		}
		//右へシフト
	  case kMenuItemID_ShiftRight:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(true,inputSpaces);
			break;
		}
		//左へシフト
	  case kMenuItemID_ShiftLeft:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(false,inputSpaces);
			break;
		}
		//バックスラッシュ入力
	  case kMenuItemID_InputBackslashYen:
		{
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true )
			{
				AText	yen;
				yen.SetLocalizedText("Text_Yen");
				TextInput(undoTag_Typing,yen);
			}
			else
			{
				AText	backslash;
				backslash.Add(kUChar_Backslash);
				TextInput(undoTag_Typing,backslash);
			}
			break;
		}
		
		//次を検索
	  case kMenuItemID_FindNext:
		{
			SPI_FindNext();
			result = true;
			break;
		}
		//前を検索
	  case kMenuItemID_FindPrevious:
		{
			SPI_FindPrev();
			result = true;
			break;
		}
		//最初から検索
	  case kMenuItemID_FindFromFirst:
		{
			SPI_FindFromFirst();//#724
			result = true;
			break;
		}
		//置換
	  case kMenuItemID_ReplaceSelectedTextOnly:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceSelectedTextOnly(param);
			result = true;
			break;
		}
		//置換
	  case kMenuItemID_ReplaceAndFind:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceAndFind(param);
			result = true;
			break;
		}
		//次を置換
	  case kMenuItemID_ReplaceNext:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceNext(param);
			break;
		}
		//キャレット以降を置換
	  case kMenuItemID_ReplaceAfterCaret:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceAfterCaret(param);
			break;
		}
		//選択範囲内を置換
	  case kMenuItemID_ReplaceInSelection:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceInSelection(param);
			break;
		}
		//全て置換
	  case kMenuItemID_ReplaceAll:
		{
			AFindParameter	param;
			GetApp().SPI_GetFindParam(param);
			SPI_ReplaceAll(param);
			break;
		}
		//選択文字列を検索文字列に登録
	  case kMenuItemID_SetFindText:
		{
			SetFindText();
			break;
		}
		//選択文字列を置換文字列に登録
	  case kMenuItemID_SetReplaceText:
		{
			if( IsCaretMode() == true )   break;
			AText	text;
			GetSelectedText(text);
			GetApp().SPI_SetReplaceText(text);
			break;
		}
		//検索ハイライト消去
	  case kMenuItemID_EraseFindHighlight:
		{
			GetTextDocument().SPI_ClearFindHighlight();
			break;
		}
		//ジャンプ
	  case kMenuItemID_Jump:
		{
			AUniqID	identifierObjectID;//#216 = inDynamicMenuActionText.GetNumber();
			identifierObjectID.val = inDynamicMenuActionText.GetNumber();//#216
			Jump(identifierObjectID);
			break;
		}
		//
	  case kMenuItemID_JumpNextItem:
		{
			JumpNext();//#150
			break;
		}
		//
	  case kMenuItemID_JumpPreviousItem:
		{
			JumpPrev();//#150
			break;
		}
		
		//選択部分を開く
	  case kMenuItemID_OpenSelected:
		{
			OpenSelected(inModifierKeys);
			break;
		}
		//ツール
	  case kMenuItemID_Tool:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			SPI_DoTool(file,inModifierKeys,false);
			break;
		}
		//ツール
	  case kMenuItemID_Tool_StandardMode:
		{
			AFileAcc	file;
			file.Specify(inDynamicMenuActionText);
			SPI_DoTool(file,inModifierKeys,false);
			break;
		}
		//しおり
	  case kMenuItemID_Bookmark:
		{
			ATextPoint	spt, ept;
			SPI_GetSelect(spt,ept);
			GetTextDocument().SPI_SetBookmarkLineIndex(spt.y);
			NVM_GetWindow().NVI_RefreshWindow();
			break;
		}
		//しおりに移動
	  case kMenuItemID_MoveToBookmark:
		{
			SPI_MoveToLine(GetTextDocument().SPI_GetBookmarkLineIndex(),false);
			break;
		}
		//対応文字列入力
	  case kMenuItemID_CorrespondText:
		{
			InputCorrespondence();
			break;
		}
		//#359
	  case kMenuItemID_SelectBetweenCorrespondText:
		{
			SelectBetweenCorrespondTexts();
			break;
		}
		//ツール入力モード
	  case kMenuItemID_KeyToolMode:
		{
			StartKeyToolMode();
			break;
		}
		//矩形選択モード
	  case kMenuItemID_KukeiSelectionMode:
		{
			if( mKukeiSelectionMode == true )
			{
				if( IsCaretMode() == false && mKukeiSelected == true && mKukeiSelected_Start.GetItemCount() > 0 )
				{
					ATextPoint	spt, ept;
					GetKukeiContainRange(spt,ept);
					SetSelect(spt,ept);
				}
				mKukeiSelected_Start.DeleteAll();
				mKukeiSelected_End.DeleteAll();
				mKukeiSelected = false;
				mKukeiSelectionMode = false;
			}
			else
			{
				mKukeiSelected_Start.DeleteAll();
				mKukeiSelected_End.DeleteAll();
				mKukeiSelected = false;
				mKukeiSelectionMode = true;
				if( IsCaretMode() == false )
				{
					ATextPoint	spt, ept;
					SPI_GetSelect(spt,ept);
					AImagePoint	imagespt, imageept;
					GetImagePointFromTextPoint(spt,imagespt);
					GetImagePointFromTextPoint(ept,imageept);
					SetKukeiSelect(imagespt,imageept);
/*					
					for( AIndex lineIndex = spt.y; lineIndex <= ept.y; lineIndex++ )
					{
						AImagePoint	ispt = imagespt;
						ispt.y = GetImageYByLineIndex(lineIndex);
						AImagePoint	iept = imageept;
						iept.y = ispt.y;
						ATextPoint	start, end;
						GetTextPointFromImagePoint(ispt,start);
						GetTextPointFromImagePoint(iept,end);
						mKukeiSelected_Start.Add(start);
						mKukeiSelected_End.Add(end);
					}
					mKukeiSelected = true;
					mCaretMode = false;
					NVI_Refresh();*/
				}
			}
			break;
		}
		/*#800
		//矩形ペースト
	  case kMenuItemID_KukeiPaste:
		{
			PasteKukei();
			break;
		}
		*/
		//行折り返し位置で改行
	  case kMenuItemID_InsertLineEndAtWrap:
		{
			ATextPoint	spt, ept;
			SPI_GetSelect(spt,ept);
			if( spt.y != ept.y )
			{
				GetTextDocument().SPI_InsertLineEndAtWrap(spt.y,ept.y);
			}
			break;
		}
		//Transliterate
	  case kMenuItemID_Transliterate:
		{
			Transliterate(inDynamicMenuActionText);
			/*#1035
			ATextIndex	spos,epos;
			GetSelect(spos,epos);
			AText	origtext, text;
			GetSelectedText(origtext);
			GetApp().SPI_Transliterate(inDynamicMenuActionText,origtext,text);
			TextInput(undoTag_Transliterate,text);
			SetSelect(spos,spos+text.GetItemCount());
			*/
			break;
		}
	  case kMenuItemID_Transliterate_ToLower:
		{
			TransliterateToLower();
			/*#1035
			ATextIndex	spos,epos;
			GetSelect(spos,epos);
			AText	text;
			GetSelectedText(text);
			text.ChangeCaseLower();
			TextInput(undoTag_Transliterate,text);
			SetSelect(spos,spos+text.GetItemCount());
			*/
			break;
		}
	  case kMenuItemID_Transliterate_ToUpper:
		{
			TransliterateToUpper();
			/*#1035
			ATextIndex	spos,epos;
			GetSelect(spos,epos);
			AText	text;
			GetSelectedText(text);
			text.ChangeCaseUpper();
			TextInput(undoTag_Transliterate,text);
			SetSelect(spos,spos+text.GetItemCount());
			*/
			break;
		}
		//定義場所
	  case kMenuItemID_MoveToDefinition:
		{
			//現在の位置をNavigate登録する #146
			GetApp().SPI_RegisterNavigationPosition();
			//
			AIndex	index = inDynamicMenuActionText.GetNumber();
			ATextIndex	spos, epos;
			ABool	import;
			AFileAcc	file;
			GetTextDocument().SPI_GetCurrentIdentifierDefinitionRange(index,spos,epos,import,file);
			if( import == false )
			{
				if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
				{
					if( mTextWindowID != kObjectID_Invalid )
					{
						GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SplitView(false);
					}
				}
				SetSelect(spos,epos);
				AdjustScroll_Top(GetCaretTextPoint());
			}
			else
			{
				//#725 GetApp().SPI_SelectTextDocument(file,spos,epos-spos);
				AText	path;
				file.GetPath(path);
				GetApp().SPI_OpenOrRevealTextDocument(path,spos,epos,kObjectID_Invalid,false,kAdjustScrollType_Center);
			}
			break;
		}
		//文法再認識
	  case kMenuItemID_ReRecognize:
		{
			GetTextDocument().SPI_ReRecognizeSyntax();
			break;
		}
		//R0199
	  case kMenuItemID_CorrectSpell:
		{
			if( GetTextDocument().NVI_IsReadOnly() == true )   break;
			if( IsCaretMode() == true )
			{
				SelectWord();
			}
			TextInput(undoTag_Typing,inDynamicMenuActionText);
			break;
		}
		//R0006
	  case kMenuItemID_SCMCompareWithLatest:
		{
			DiffWithRepository();//#379
			break;
		}
	  case kMenuItemID_SCMNextDiff:
		{
			NextDiff();
			break;
		}
	  case kMenuItemID_SCMPrevDiff:
		{
			PrevDiff();
			break;
		}
		//#390
	  case kMenuItemID_KeyRecordStart:
		{
			GetApp().SPI_ShowKeyRecordWindow();
			GetApp().SPI_StartRecord();
			break;
		}
	  case kMenuItemID_KeyRecordStop:
		{
			GetApp().SPI_StopRecord();
			break;
		}
	  case kMenuItemID_KeyRecordPlay:
		{
			SPI_PlayKeyRecord();
			break;
		}
		//#478
	  case kMenuItemID_FigMode_None:
		{
			mFigMode = kFigMode_None;
			break;
		}
	  case kMenuItemID_FigMode_Line:
		{
			mFigMode = kFigMode_Line;
			break;
		}
		//#450
		//折りたたみ・展開
	  case kMenuItemID_Folding_Collapse:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			SPI_ExpandCollapseAtCurrentLine(spt.y,true);
			break;
		}
	  case kMenuItemID_Folding_Expand:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			SPI_ExpandCollapseAtCurrentLine(spt.y,false);
			break;
		}
	  case kMenuItemID_Folding_CollapseAllAtCurrentLevel:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			SPI_ExpandCollapseAllAtCurrentLevel(spt.y,true,false);
			break;
		}
	  case kMenuItemID_Folding_ExpandAllAtCurrentLevel:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			SPI_ExpandCollapseAllAtCurrentLevel(spt.y,false,false);
			break;
		}
	  case kMenuItemID_Folding_CollapseAllHeaders:
		{
			CollapseAllHeaders();
			break;
		}
	  case kMenuItemID_Folding_CollapseAllHeadersWithNoChange:
		{
			CollapseAllHeadersWithMNoChange();
			break;
		}
	  case kMenuItemID_Folding_ExpandAll:
		{
			ExpandAllFoldings();
			break;
		}
	  case kMenuItemID_Folding_SelectFoldingRange:
		{
			//未実装
			break;
		}
	  case kMenuItemID_Folding_SetToDefault:
		{
			SPI_SetFoldingToDefault();
			break;
		}
		//見出し移動
	  case kMenuItemID_Folding_GoToParent:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindParentHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = spt.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
	  case kMenuItemID_Folding_GoToNext:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindNextHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = ept.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
	  case kMenuItemID_Folding_GoToPrev:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindPrevHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = ept.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
	  case kMenuItemID_Folding_GoToNextAtSameLevel:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindSameOrHigherLevelNextHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = ept.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
	  case kMenuItemID_Folding_GoToPrevAtSameLevel:
		{
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			spt.y = FindSameOrHigherLevelPrevHeader(spt.y);
			ept.y = spt.y +1;
			spt.x = ept.x = 0;
			if( spt.y != kIndex_Invalid )
			{
				SPI_SetSelect(spt,ept);
				SPI_AdjustScroll_Center();
			}
			break;
		}
		
		//コンテクストメニュー
		//マルチファイル検索
	  case kMenuItemID_CM_MultiFileFind:
		{
			AText	text;
			GetSelectedText(text);
			GetApp().SPI_ShowMultiFileFindWindow(text);
			break;
		}
		//
	  case kMenuItemID_CM_Find:
		{
			AText	text;
			GetSelectedText(text);
			GetApp().SPI_ShowFindWindow(text);
			break;
		}
		//
	  case kMenuItemID_CM_FindNext:
		{
			//Findパラメータ設定
			AFindParameter	param;
			GetSelectedText(param.findText);
			param.regExp = true;
			param.loop = false;
			//検索実行
			SPI_FindNext(param);
			break;
		}
		//
	  case kMenuItemID_CM_FindPrevious:
		{
			//Findパラメータ設定
			AFindParameter	param;
			GetSelectedText(param.findText);
			param.regExp = true;
			param.loop = false;
			//検索実行
			SPI_FindPrevious(param);
			break;
		}
		//
	  case kMenuItemID_CM_IdInfo:
		{
			//B0432
			if( IsCaretMode() == true )
			{
				SelectWord();
			}
			
			//#291 GetApp().SPI_GetIdInfoWindow().NVI_Show(false);
			/*#853
			GetApp().SPI_ShowHideIdInfoWindow(true);//#291
			AText	text;
			GetSelectedText(text);
			SetIdInfo(text);
			*/
			DisplayIdInfo(true,kIndex_Invalid,kIndex_Invalid);//#853
			break;
		}
#if IMPLEMENTATION_FOR_MACOSX
		//R0231
	  case kMenuItemID_CM_Dictionary:
		{
			//B0432
			if( IsCaretMode() == true )
			{
				SelectWord();
			}
			
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
			{
				DictionaryPopup();
			}
			else
			{
				Dictionary();
			}
			break;
		}
#endif
		/*#858
		//RC3
	  case kMenuItemID_CM_ExternalComment:
		{
			ExternalComment();
			break;
		}
		*/
		//#467 define定義設定ウインドウを開く
	  case kMenuItemID_CM_OpenDefineValueWindow:
		{
			AText	text;
			GetSelectedText(text);
			GetApp().SPI_GetEnabledDefinesWindow().SPI_SetDefineEditBox(text);
			GetApp().SPI_GetEnabledDefinesWindow().NVI_Show();
			break;
		}
		//RC1
	  case kMenuItemID_ArrowToDef:
		{
			GetTextDocument().SPI_SetDrawArrowToDef(!GetTextDocumentConst().SPI_GetDrawArrowToDef());
			NVM_GetWindow().NVI_RefreshWindow();
			break;
		}
		//#564
	  case kMenuItemID_CopyFilePath:
		{
			if( GetTextDocumentConst().NVI_IsFileSpecified() == true )
			{
				AFileAcc	file;
				GetTextDocument().NVI_GetFile(file);
				AText	path;
				file.GetPath(path);
				AScrapWrapper::SetClipboardTextScrap(path,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), false);//#1300
			}
			break;
		}
		//選択範囲とクリップボードを比較
	  case kMenuItemID_CompareSelectionAndClipboard:
		{
			//クリップボード取得
			AText	text;
			AScrapWrapper::GetClipboardTextScrap(text);
			//選択範囲取得
			AIndex	start = kIndex_Invalid;
			AIndex	end = kIndex_Invalid;
			SPI_GetSelect(start,end);
			//ドキュメントに比較モード設定
			GetTextDocument().SPI_SetDiffMode_TextWithRange(text,start,end,kCompareMode_Clipboard);
			//ウインドウを比較モードに設定
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
			}
			break;
		}
		//#112
		//ファイルと比較
	  case kMenuItemID_CompareWithFile:
		{
			//ファイル選択ダイアログ表示
			//デフォルトファイル：ドキュメントのファイル
			AFileAcc	defaultFile;
			GetTextDocument().NVI_GetFile(defaultFile);
			AText	message;
			message.SetLocalizedText("ChooseFileMessage_CompareFile");
			NVM_GetWindow().NVI_ShowChooseFileWindow(defaultFile,message,AWindow_Text::kVirtualControlID_CompareFileChoosen);
			break;
		}
		//#81
		//比較なし
	  case kMenuItemID_NoCompare:
		{
			//ドキュメントに比較モード設定
			GetTextDocument().SPI_SetDiffMode_None();
			//ウインドウの比較モード解除
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(false);
			}
			break;
		}
		//バックアップの項目（動的メニュー項目）を選択時
	  case kMenuItemID_CompareWithAutoBackup_Monthly:
		{
			//比較先ファイル取得
			AFileAcc	diffTargetFile;
			if( GetTextDocument().SPI_GetAutoBackupMonthlyFile(inDynamicMenuActionText,diffTargetFile) == true )
			{
				//ドキュメントに比較モード設定
				GetTextDocument().SPI_SetDiffMode_File(diffTargetFile,kCompareMode_AutoBackupMonthly);
				//ウインドウを比較モードに設定
				if( mTextWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
	  case kMenuItemID_CompareWithAutoBackup_Daily:
		{
			//比較先ファイル取得
			AFileAcc	diffTargetFile;
			if( GetTextDocument().SPI_GetAutoBackupDailyFile(inDynamicMenuActionText,diffTargetFile) == true )
			{
				//ドキュメントに比較モード設定
				GetTextDocument().SPI_SetDiffMode_File(diffTargetFile,kCompareMode_AutoBackupDaily);
				//ウインドウを比較モードに設定
				if( mTextWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
	  case kMenuItemID_CompareWithAutoBackup_Hourly:
		{
			//比較先ファイル取得
			AFileAcc	diffTargetFile;
			if( GetTextDocument().SPI_GetAutoBackupHourlyFile(inDynamicMenuActionText,diffTargetFile) == true )
			{
				//ドキュメントに比較モード設定
				GetTextDocument().SPI_SetDiffMode_File(diffTargetFile,kCompareMode_AutoBackupHourly);
				//ウインドウを比較モードに設定
				if( mTextWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
	  case kMenuItemID_CompareWithAutoBackup_Minutely:
		{
			//比較先ファイル取得
			AFileAcc	diffTargetFile;
			if( GetTextDocument().SPI_GetAutoBackupMinutelyFile(inDynamicMenuActionText,diffTargetFile) == true )
			{
				//ドキュメントに比較モード設定
				GetTextDocument().SPI_SetDiffMode_File(diffTargetFile,kCompareMode_AutoBackupMinutely);
				//ウインドウを比較モードに設定
				if( mTextWindowID != kObjectID_Invalid )
				{
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(true);
				}
			}
			break;
		}
	  case kMenuItemID_CompareWithDailyBackupByDefault:
		{
			//未実装
			break;
		}
		//比較（比較モードきりかえ）
	  case kMenuItemID_Compare:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).
						SPI_SetDiffDisplayMode(!(GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetDiffDisplayMode()));
			}
			break;
		}
		//Speech #851
	  case kMenuItemID_StartSpeaking:
		{
			AText	text;
			GetSelectedText(text);
			//選択中でない場合はすべてスピーチ #1313
			if( text.GetItemCount() == 0 )
			{
				GetTextDocumentConst().SPI_GetText(text);
			}
			//
			GetApp().NVI_SpeakText(text);
			break;
		}
	  case kMenuItemID_StopSpeaking:
		{
			GetApp().NVI_StopSpeaking();
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	return result;
}

//
void	AView_Text::OpenSelected( const AModifierKeys inModifierKeys )
{
	AText	path;
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	GetTextDocumentConst().SPI_GetTextForOpenSelected(spt,ept,path);
	//
	AIndex	foundpos;
	if( path.FindCstring(0,"://",foundpos) == true )
	{
		ALaunchWrapper::OpenURL(path);
		return;
	}
	//
	if( GetTextDocumentConst().SPI_IsRemoteFileMode() == true )
	{
		//
		if( path.GetItemCount() > 0 )
		{
			AText	url;
			GetTextDocumentConst().SPI_GetRemoteFileURL(url);
			url.CatFTPURLPathText(path);
			GetApp().SPI_RequestOpenFileToRemoteConnection(url,false);//#361
		}
	}
	else 
	{
		//B0389 OK フルパスで開く
		AFileAcc	fullpathfile;
		fullpathfile.Specify(path);
		if( fullpathfile.Exist() == true && fullpathfile.IsFolder() == false )
		{
			GetApp().GetAppPref().LaunchAppWithFile(fullpathfile,inModifierKeys);
			return;
		}
		
		if( GetTextDocumentConst().NVI_IsFileSpecified() == true )
		{
			//
			AFileAcc	docfile;
			GetTextDocumentConst().NVI_GetFile(docfile);
			AFileAcc	folder;
			folder.SpecifyParent(docfile);
			//
			AFileAcc	file;
			file.SpecifyChild(folder,path);//B0389 OK ,kFilePathType_1);kFilePathType_Defaultでも../.解釈できるので問題なし。
			if( file.Exist() == true && file.IsFolder() == false )
			{
				GetApp().GetAppPref().LaunchAppWithFile(file,inModifierKeys);
				return;
			}
			//
			AText	text;
			docfile.GetPath(text);
			AObjectArray<AFileAcc>	candidate;
			GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetOpenSelectedFileCandidate(text,candidate);
			if( candidate.GetItemCount() > 0 )
			{
				//GetApp().SPNVI_CreateDocumentFromLocalFile(candidate.GetObject(0));
				GetApp().GetAppPref().LaunchAppWithFile(candidate.GetObject(0),inModifierKeys);
			}
			//R0000 最初のインポートファイルを開く
			else
			{
				if( GetTextDocumentConst().SPI_GetImportFileArray().GetItemCount() > 0 )
				{
					GetApp().GetAppPref().LaunchAppWithFile(GetTextDocumentConst().SPI_GetImportFileArray().GetObjectConst(0),inModifierKeys);
				}
			}
		}
	}
}

//
void	AView_Text::EVTDO_UpdateMenu()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetAskingSaveChanges() == true )   return;
		if( GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_IsPrintMode() == true )   return;
	}
	
	ABool	b;
	AText	text;
	
	//Undo
	b = GetTextDocument().SPI_GetUndoText(text);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Undo,b);
	GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_Undo,text);
	//Redo
	b = GetTextDocument().SPI_GetRedoText(text);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Redo,b);
	GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_Redo,text);
	//Save
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Save,GetTextDocument().NVI_IsDirty());
	//SaveAs
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SaveAs,true);
	//印刷 #524
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Print,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_PrintRange,(IsCaretMode()==false));
	//コピー
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Copy,(IsCaretMode()==false));
	//追加コピー
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_AdditionalCopy,(IsCaretMode()==false)&&
				(GetApp().SPI_IsFIFOClipboardMode()==false));//#878 FIFOクリップボードモード時は追加コピー不可
	//カット
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Cut,((IsCaretMode()==false)&&(GetTextDocument().NVI_IsReadOnly()==false)));
	//ペースト
	if( GetApp().SPI_IsFIFOClipboardMode() == false )
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Paste,
					((GetTextDocument().NVI_IsReadOnly()==false)&&(AScrapWrapper::ExistClipboardTextScrapData()==true)));
		//#929
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_PasteAtEndOfEachLine,
														((GetTextDocument().NVI_IsReadOnly()==false)&&(AScrapWrapper::ExistClipboardTextScrapData()==true)));
	}
	//#878 FIFOクリップモード時のペースト
	else
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Paste,
					((GetTextDocument().NVI_IsReadOnly()==false)&&
						(GetApp().SPI_GetFIFOClipboardCount()>0||AScrapWrapper::ExistClipboardTextScrapData()==true)));
		//#929
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_PasteAtEndOfEachLine,false);
	}
	//削除
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Clear,((IsCaretMode()==false)&&(GetTextDocument().NVI_IsReadOnly()==false)));
	//全てを選択
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SelectAll,true);
	//インデント
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Indent,(GetTextDocument().NVI_IsReadOnly()==false));
	//右シフト
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShiftRight,(GetTextDocument().NVI_IsReadOnly()==false));
	//左シフト
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ShiftLeft,(GetTextDocument().NVI_IsReadOnly()==false));
	//バックスラッシュ入力
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_InputBackslashYen,(GetTextDocumentConst().SPI_IsJISTextEncoding() == false));//#940
		AText	text;
		if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true )
		{
			text.SetLocalizedText("InputBackslash_InputYen");
		}
		else
		{
			text.SetLocalizedText("InputBackslash_InputBackslash");
#if IMPLEMENTATION_FOR_WINDOWS
			AText	t("Backslash");
			text.ReplaceChar(kUChar_Backslash,t);
#endif
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_InputBackslashYen,text);
	}
	
	//検索
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	b = (param.findText.GetItemCount()>0);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindNext,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindPrevious,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindFromFirst,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FindList,b);
	b = ((GetTextDocumentConst().NVI_IsReadOnly()==false) && (IsCaretMode() == false) && (param.findText.GetItemCount()>0));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceSelectedTextOnly,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAndFind,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceInSelection,b);
	b = ((GetTextDocumentConst().NVI_IsReadOnly()==false) && (param.findText.GetItemCount()>0));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceNext,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAfterCaret,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReplaceAll,b);
	b = (IsCaretMode()==false);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetFindText,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SetReplaceText,b);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_EraseFindHighlight,GetTextDocument().SPI_IsFindHighlightSet());
	
	//選択部分を開く
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_OpenSelected,true);
	//しおり
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Bookmark,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_MoveToBookmark,(GetTextDocument().SPI_GetBookmarkLineIndex()!=kIndex_Invalid));
	//対応文字列入力
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CorrespondText,(GetTextDocument().NVI_IsReadOnly()==false));
	//#359
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SelectBetweenCorrespondText,true);
	//ツール入力モード
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyToolMode,true);
	//ツール
	//#129 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Tool,true);
	//矩形選択モード
	{
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KukeiSelectionMode,true);
		AText	text;
		if( mKukeiSelectionMode == true )
		{
			text.SetLocalizedText("RectSelect_Off");
		}
		else
		{
			text.SetLocalizedText("RectSelect_On");
		}
		GetApp().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_KukeiSelectionMode,text);
	}
	
	/*#800
	//矩形ペースト
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KukeiPaste,
			((GetTextDocument().NVI_IsReadOnly()==false)&&(AScrapWrapper::ExistClipboardTextScrapData()==true)));
			*/
	//行折り返し位置で改行
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_InsertLineEndAtWrap,(GetTextDocument().NVI_IsReadOnly()==false));
	//Transliterate
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Transliterate,(GetTextDocument().NVI_IsReadOnly()==false)&&(IsCaretMode()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Transliterate_ToLower,(GetTextDocument().NVI_IsReadOnly()==false)&&(IsCaretMode()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Transliterate_ToUpper,(GetTextDocument().NVI_IsReadOnly()==false)&&(IsCaretMode()==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_TransliterateRootMenu,(GetTextDocument().NVI_IsReadOnly()==false)&&(IsCaretMode()==false));
	
	//ジャンプ次
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_JumpNextItem,true);
	//ジャンプ前
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_JumpPreviousItem,true);
	//
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Jump,true);
	
	//文法再認識
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ReRecognize,true);
	
	//#129 Enable/Disable更新
	//#129 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Tool_StandardMode,true);
	//#129 GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Tool,true);
	GetApp().NVI_GetMenuManager().UpdateDynamicMenuEnableMenuItem(kMenuItemID_Tool_StandardMode);//#129
	GetApp().NVI_GetMenuManager().UpdateDynamicMenuEnableMenuItem(kMenuItemID_Tool);//#129
	
	//RC1
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_ArrowToDef,true);
	
	//#390
	ABool	keyRecording = GetApp().SPI_IsKeyRecording();
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyRecordStart,(keyRecording==false));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyRecordStop,(keyRecording==true));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_KeyRecordPlay,true);
	
	//#478
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FigMode_None,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_FigMode_Line,true);
	
	//#564
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CopyFilePath,
				(GetTextDocumentConst().NVI_IsFileSpecified()==true));
	
	//#450
	//折りたたみ関連
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_Collapse,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_Expand,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_CollapseAllAtCurrentLevel,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_ExpandAllAtCurrentLevel,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_CollapseAllHeaders,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_CollapseAllHeadersWithNoChange,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_ExpandAll,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToParent,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToNext,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToPrev,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToNextAtSameLevel,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_GoToPrevAtSameLevel,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_SelectFoldingRange,true);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Folding_SetToDefault,true);
	
	//#878
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SwitchFIFOClipboardMode,true);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SwitchFIFOClipboardMode,GetApp().SPI_IsFIFOClipboardMode());
	
	//------------------比較関連------------------
	//R0006
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kUseSCM) == true )
	{
		AFileAcc	f;//#379
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMCompareWithLatest,
					GetTextDocumentConst().SPI_CanDiffWithRepositiory() &&
					((GetTextDocumentConst().SPI_IsRemoteFileMode()==false)&&(GetTextDocumentConst().NVI_IsFileSpecified()==true)));
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMNextDiff,true);
		GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SCMPrevDiff,true);
	}
	
	//
	ABool	wrapCalculateCompleted = (GetTextDocumentConst().SPI_IsWrapCalculating()==false);
	//#737
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareSelectionAndClipboard,
													((AScrapWrapper::ExistClipboardTextScrapData()==true)&&wrapCalculateCompleted));
	//#81
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_NoCompare,wrapCalculateCompleted);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithFile,wrapCalculateCompleted);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithAutoBackup,
													(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kEnableAutoBackup)&&
													 wrapCalculateCompleted));
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithAutoBackup_Monthly,wrapCalculateCompleted);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithAutoBackup_Daily,wrapCalculateCompleted);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithAutoBackup_Hourly,wrapCalculateCompleted);
	//GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_CompareWithDailyBackupByDefault,true);
	//
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_NoCompare,false);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SCMCompareWithLatest,false);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithFile,false);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareSelectionAndClipboard,false);
	GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,false);
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Minutely,GetEmptyText());
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Hourly,GetEmptyText());
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Daily,GetEmptyText());
	GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Monthly,GetEmptyText());
	AText	filename;
	switch(GetTextDocumentConst().SPI_GetCompareMode(filename))
	{
	  case kCompareMode_NoCompare:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_NoCompare,true);
			break;
		}
	  case kCompareMode_Repository:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_SCMCompareWithLatest,true);
			break;
		}
	  case kCompareMode_File:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithFile,true);
			break;
		}
	  case kCompareMode_Clipboard:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareSelectionAndClipboard,true);
			break;
		}
	  case kCompareMode_AutoBackupMinutely:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,true);
			GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Minutely,filename);
			break;
		}
	  case kCompareMode_AutoBackupHourly:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,true);
			GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Hourly,filename);
			break;
		}
	  case kCompareMode_AutoBackupDaily:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,true);
			GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Daily,filename);
			break;
		}
	  case kCompareMode_AutoBackupMonthly:
		{
			GetApp().NVI_GetMenuManager().SetCheckMark(kMenuItemID_CompareWithAutoBackup,true);
			GetApp().NVI_GetMenuManager().SetCheckMarkToDynamicMenuWithActionText(kMenuItemID_CompareWithAutoBackup_Monthly,filename);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	//比較
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Compare,(GetTextDocumentConst().SPI_GetDiffTargetDocumentID() != kObjectID_Invalid));
	//Speech #851
	/*#1313
	AText	selectedText;
	GetSelectedText(selectedText);
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_StartSpeaking,(selectedText.GetItemCount()>0));
	*/
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_StartSpeaking,true);//#1313
	GetApp().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_StopSpeaking,GetApp().NVI_IsSpeaking());
}

//R0246
ABool	AView_Text::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	/*#688
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_HideHelpTag();
	}
	*/
	/*#233 テスト
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	mouseTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,mouseTextPoint);
	if( mHoverCaretTextPoint.x != mouseTextPoint.x || mHoverCaretTextPoint.y != mouseTextPoint.y )
	{
		XorHoverCaret(mHoverCaretTextPoint);
		//
		mHoverCaretTextPoint = mouseTextPoint;
		XorHoverCaret(mHoverCaretTextPoint);
	}
	*/
	
	//#853
	//キーワード情報 引数ハイライト
	AImagePoint	hoverImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,hoverImagePoint);
	ATextPoint	hoverTextPoint = {0};
	GetTextPointFromImagePoint(hoverImagePoint,hoverTextPoint);
	IdInfoHighlightArg(GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(hoverTextPoint));
	
	return false;
}

//マウスボタン押下時のコールバック(AView用)
ABool	AView_Text::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	return SPI_DoMouseDown(inLocalPoint,inModifierKeys,inClickCount,false);
}
ABool	AView_Text::SPI_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount,
									const ABool inClickedAtLineNumberArea )
{
	RestoreFromBraceCheck();
	//補完入力状態解除
	ClearAbbrevInputMode(true,false);//RC2
	//
	EndKeyToolMode();//R0073
	
	//選択予約を解除 #699
	CancelReservedSelection();
	
	//fprintf(stderr,"EVTDO_DoMouseDown() ");
	//自ViewがActiveでない場合は、Windowへフォーカスの移動を依頼する
	if( NVI_IsFocusActive() == false )
	{
		NVM_GetWindow().NVI_SwitchFocusTo(NVI_GetControlID());
	}
	
	if( inClickCount == 1 )
	{
		DoMouseDownSingleClick(inLocalPoint,inModifierKeys,inClickedAtLineNumberArea);
	}
	else if( inClickCount == 2 )
	{
		DoMouseDownDoubleClick(inLocalPoint,inModifierKeys);
	}
	else if( inClickCount == 3 )
	{
		DoMouseDownTripleClick(inLocalPoint,inModifierKeys,inClickedAtLineNumberArea);
	}
	else if( inClickCount == 4 && 
	GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kSelectInsideByQuadClick) == true )
	{
		DoMouseDownQuadClick(inLocalPoint,inModifierKeys);
	}
	
	//win Caretちらつき対策（途中で描画せずに、ここでまとめて表示）
	//Caret表示予約があれば表示する
	ShowCaretIfReserved();
	
	return false;
}

//マウスボタン押下時のコールバック(AView用)
ABool	AView_Text::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	RestoreFromBraceCheck();
	//補完入力状態解除
	ClearAbbrevInputMode(true,false);//RC2
	//
	DoContextMenu(inLocalPoint,inModifierKeys);
	return true;
}

//マウスクリック（シングルクリック）
void	AView_Text::DoMouseDownSingleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
										   const ABool inInsideLineNumber )
{
	//#606 手のひらツール
	if( IsHandToolMode() == true )
	{
		DoMouseDownTenohira(inLocalPoint,inModifierKeys);
		return;
	}
	
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	//行番号クリックの場合は、imageの左端をクリックしたことにする
	if( inInsideLineNumber == true )
	{
		clickImagePoint.x = 0;
	}
	
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//#478 最初のクリック位置を記憶
	mMouseDownImagePoint = clickImagePoint;
	mMouseDownTick = GetApp().SPI_GetAppElapsedTick();
	
	//選択範囲内クリック（Dragまたはキャレット設定）
	if( IsInSelect(clickImagePoint,true) == true )
	{
		//#688 if( mMouseTrackByLoop == false )
		//#688{
		//TrackingMode設定
		mMouseTrackingMode = kMouseTrackingMode_SingleClickInSelect;
		mMouseTrackStartImagePoint = clickImagePoint;
		mMouseTrackImagePoint = clickImagePoint;
		mMouseTrackResultIsDrag = false;
		NVM_SetMouseTrackingMode(true);
		return;
		//#688}
		/*#688 
		//最初のマウス位置取得（この時点でマウスボタンが放されていたらリターン。TrackMouseDown()でマウスボタンリリースのイベントを取得したら、次はもう来ない。B0260）
		AWindowPoint	mouseWindowPoint;
		AModifierKeys	modifiers;
		if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
		
		AWindowPoint	firstMouseWindowPoint = mouseWindowPoint;
		mMouseTrackResultIsDrag = false;
		while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
		{
			if( firstMouseWindowPoint.x != mouseWindowPoint.x || firstMouseWindowPoint.y != mouseWindowPoint.y )
			{
				//マウスボタン押しっぱなしでマウス位置が少しでも移動したらDrag
				mMouseTrackResultIsDrag = true;
				break;
			}
		}
		
		//Image座標に変換
		ALocalPoint	mouseLocalPoint;
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		AImagePoint	mouseImagePoint;
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		
		//Drag開始またはキャレット設定
		DoMouseTrackEnd_SingleClickInSelect(mouseImagePoint,0);
		
		return;
		*/
	}
	
	//キャレット設定
	if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true )
	{
		SetSelectTextPoint(clickTextPoint);
	}
	else
	{
		SetCaretTextPoint(clickTextPoint);
		ShowCaret();
		/*#725 InfoHeader廃止
		//R0214
		if( false )
		{
			ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			AText	word;
			ATextIndex	spos = pos, epos = pos;
			GetTextDocument().SPI_FindWord(pos,spos,epos);
			GetTextDocument().SPI_GetText(spos,epos,word);
			SetInfoHeader(word);
		}
		*/
	}
	
	//マウスドラッグによる選択
	
	//#688 if( mMouseTrackByLoop == false )
	//#688{
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
	mMouseTrackImagePoint = clickImagePoint;
	NVM_SetMouseTrackingMode(true);
	return;
	//#688}
	/*#688
	//最初のマウス位置取得（この時点でマウスボタンが放されていたらリターン。TrackMouseDown()でマウスボタンリリースのイベントを取得したら、次はもう来ない。B0260）
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	
	//Image座標に変換
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
	//前回マウス位置の保存
	AImagePoint	previousImagePoint = mouseImagePoint;
	//
	ATickCount	firsttickcount = ATimerWrapper::GetTickCount();
	ABool	sameposasfirst = true;
	//マウスボタンが放されるまでループ
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Image座標に変換
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		//CM
		if( sameposasfirst == true )
		{
			if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCMMouseButton) == true &&
			ATimerWrapper::GetTickCount() > firsttickcount + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCMMouseButtonTick) )
			{
				DoContextMenu(mouseLocalPoint,inModifierKeys);
				return;
			}
		}
		//前回マウス位置と同じなら何もせずループ継続
		if( mouseImagePoint.x == previousImagePoint.x && mouseImagePoint.y == previousImagePoint.y )
		{
			continue;
		}
		sameposasfirst = false;
		
		//Mouse Track
		DoMouseTrack_SingleClick(mouseImagePoint,modifiers);
		
		//前回マウス位置の保存
		previousImagePoint = mouseImagePoint;
	}
	DoMouseTrackEnd_SingleClick(mouseImagePoint,modifiers);
	*/
}

//Mouse Track
void	AView_Text::DoMouseTrack_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//表示フレームをImage座標で取得
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//表示フレーム外にマウス移動したときはスクロールする(Y方向)
		if( mouseImagePoint.y < (frameImageRect.top+kDragScrollMargin) )//#456
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > (frameImageRect.bottom-kDragScrollMargin) )//#456
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.bottom - 1;
			}
		}
		//表示フレーム外にマウス移動したときはスクロールする(X方向)
		if( mouseImagePoint.x < frameImageRect.left )
		{
			if( frameImageRect.left > 0 )
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.left - NVI_GetHScrollBarUnit() + 1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.left;
			}
		}
		else if( mouseImagePoint.x > frameImageRect.right )
		{
			if( frameImageRect.right < NVM_GetImageWidth() )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		switch(mFigMode)
		{
		  case kFigMode_None:
			{
				//テキスト選択
				if( mKukeiSelectionMode == false && 
				   (((inModifierKeys&kModifierKeysMask_Option)==0)/*&&((inModifierKeys&kModifierKeysMask_Shift)==0)*/) )//#795
				{
					ATextPoint	selectTextPoint;
					GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
					SetSelectTextPoint(selectTextPoint);
				}
				else
				{
					SetKukeiSelect(mCaretImagePoint,mouseImagePoint);
				}
				break;
			}
			//#478
		  case kFigMode_Line:
			{
				
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
}

//#478
/**
*/
void	AView_Text::DoMouseTrackEnd_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	switch(mFigMode)
	{
	  case kFigMode_None:
		{
			//文法認識とローカル範囲設定
			TriggerSyntaxRecognize();
			/*#853 クリックはその単語のキーワード情報表示とする。引数情報はホバーで表示。
			EVTDO_DoMouseTrackEnd()で処理する。
			//識別し情報ウインドウ引数アンダーライン表示
			if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() == true )
			{
				IdInfoAutoArg();
			}
			*/
			break;
		}
	  case kFigMode_Line:
		{
			FigMode_DrawLine(mMouseDownImagePoint,inMouseImagePoint);
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//選択範囲内クリック後のマウストラック終了（マウスボタンUpまたはドラッグ開始）
void	AView_Text::DoMouseTrackEnd_SingleClickInSelect( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	ALocalPoint	localPoint;
	NVM_GetLocalPointFromImagePoint(inMouseImagePoint,localPoint);
	ATextPoint	textPoint;
	GetTextPointFromImagePoint(inMouseImagePoint,textPoint);
	if( mMouseTrackResultIsDrag == true )
	{
		//マウスボタン押下中にマウスを動かした場合
		//Drag領域設定
		AArray<ALocalRect>	dragRect;
		if( mKukeiSelected == false )
		{
			//通常選択ドラッグの場合（矩形ドラッグではない場合）
			ATextPoint	spt, ept;
			SPI_GetSelect(spt,ept);
			AImageRect	viewImageRect;
			NVM_GetImageViewRect(viewImageRect);
			if( spt.y < GetLineIndexByImageY(viewImageRect.top) )   spt.y = GetLineIndexByImageY(viewImageRect.top);
			if( ept.y > GetLineIndexByImageY(viewImageRect.bottom) )   ept.y = GetLineIndexByImageY(viewImageRect.bottom);
			AIndex	sx = spt.x;
			for( AIndex y = spt.y; y <= ept.y; y++ )
			{
				AIndex	ex = 0;
				if( y == ept.y )
				{
					ex = ept.x;
				}
				ATextPoint	s, e;
				s.x = sx;
				s.y = y;
				e.x = ex;
				e.y = y;
				AImagePoint	imageStartPoint, imageEndPoint;
				GetImagePointFromTextPoint(s,imageStartPoint);
				GetImagePointFromTextPoint(e,imageEndPoint);
				AImageRect	imageRect;
				imageRect.left		= imageStartPoint.x;
				imageRect.top		= imageStartPoint.y;
				imageRect.right		= imageEndPoint.x;
				imageRect.bottom	= imageEndPoint.y + GetLineHeightWithMargin(y);//#450 GetLineHeightWithMargin();
				if( y != ept.y )
				{
					imageRect.right = viewImageRect.right;
				}
				ALocalRect	localRect;
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				dragRect.Add(localRect);
				sx = 0;
			}
		}
		else
		{
			//矩形選択ドラッグの場合
			for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
			{
				ATextPoint	spt = mKukeiSelected_Start.Get(i);
				ATextPoint	ept = mKukeiSelected_End.Get(i);
				AImagePoint	imageStartPoint, imageEndPoint;
				GetImagePointFromTextPoint(spt,imageStartPoint);
				GetImagePointFromTextPoint(ept,imageEndPoint);
				AImageRect	imageRect;
				imageRect.left		= imageStartPoint.x;
				imageRect.top		= imageStartPoint.y;
				imageRect.right		= imageEndPoint.x;
				imageRect.bottom	= imageEndPoint.y + GetLineHeightWithMargin(ept.y);//#450 GetLineHeightWithMargin();
				ALocalRect	localRect;
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				dragRect.Add(localRect);
			}
		}
		//Scrap設定
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		//選択中のテキスト取得
		AText	text;
		GetSelectedText(text);
		//#688
		//Scrap type指定
		scrapType.Add(kScrapType_UnicodeText);
		data.Add(text);
		/*#688
		//UTF16でのScrap追加
		AText	utf16text;
		utf16text.SetText(text);
		utf16text.ConvertFromUTF8ToUTF16();
		scrapType.Add(kScrapType_UnicodeText);
		data.Add(utf16text);
		//レガシーテキストエンコーディングでのScrap追加
		AText	legacyText;
		legacyText.SetText(text);
		legacyText.ConvertFromUTF8(GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding());//win
		//win ATextEncodingWrapper::GetLegacyTextEncodingFromFont(GetTextDocumentConst().SPI_GetFont()));//B0337
		scrapType.Add(kScrapType_Text);
		data.Add(legacyText);
		*/
		//Drag実行
		mDragging = true;//B0347
		NVMC_DragText(localPoint,dragRect,scrapType,data);
		mDragging = false;//B0347
		//ゴミ箱判定：★対応検討
	}
	else
	{
		//キャレット設定
		SetCaretTextPoint(textPoint);
		ShowCaret();
	}
}

//マウスクリック（ダブルクリック）
void	AView_Text::DoMouseDownDoubleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//単語選択
	mClickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
	ATextIndex	wordStart, wordEnd;
	GetTextDocument().SPI_FindWord(mClickTextIndex,wordStart,wordEnd);
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,mFirstSelectStart,true);
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,mFirstSelectEnd,false);
	//括弧選択
	ABool	braceSelected = DoubleClickBrace(mFirstSelectStart,mFirstSelectEnd);//#688
	//
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	//#688
	//ダブルクリックによる括弧選択時はドラッグ選択せず、ここで終了
	if( braceSelected == true )
	{
		return;
	}
	
	/*
	//#450
	//折りたたみ波線内クリックなら行展開
	if( IsMouseInFoldingCollapsedLine(inLocalPoint) == true )
	{
		SPI_ExpandCollapse(clickTextPoint.y,kExpandCollapseType_Expand,true);
		return;
	}
	*/
	
	//単語取得
	AText	word;
	GetTextDocument().SPI_GetText(wordStart,wordEnd,word);
	/*#725 InfoHeader廃止
	if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() == true )//RC2
	{
		SetIdInfo(word);//RC2
	}
	else
	{
		SetInfoHeader(word);
	}
	*/
	//定義場所メニュー更新
	GetTextDocument().SPI_UpdateCurrentIdentifierDefinitionList(word);
	
	//マウスドラッグによる選択
	
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_DoubleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
	mMouseTrackImagePoint = clickImagePoint;
	NVM_SetMouseTrackingMode(true);
	return;
	//#688}
	/*#688
	//最初のマウス位置取得（この時点でマウスボタンが放されていたらリターン。TrackMouseDown()でマウスボタンリリースのイベントを取得したら、次はもう来ない。B0260）
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	//Image座標に変換
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
	//前回マウス位置の保存
	AImagePoint	previousImagePoint = mouseImagePoint;
	//マウスボタンが放されるまでループ
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Image座標に変換
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		//前回マウス位置と同じなら何もせずループ継続
		if( mouseImagePoint.x == previousImagePoint.x && mouseImagePoint.y == previousImagePoint.y )
		{
			continue;
		}
		
		//Mouse Track
		DoMouseTrack_DoubleClick(mouseImagePoint,modifiers);
		
		//前回マウス位置の保存
		previousImagePoint = mouseImagePoint;
	}
	*/
}

//Mouse Track (ダブルクリック)
void	AView_Text::DoMouseTrack_DoubleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//表示フレームをImage座標で取得
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//表示フレーム外にマウス移動したときはスクロールする(Y方向)
		if( mouseImagePoint.y < (frameImageRect.top+kDragScrollMargin) )//#456
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > (frameImageRect.bottom-kDragScrollMargin) )//#456
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.bottom - 1;
			}
		}
		//表示フレーム外にマウス移動したときはスクロールする(X方向)
		if( mouseImagePoint.x < frameImageRect.left )
		{
			if( frameImageRect.left > 0 )
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.left - NVI_GetHScrollBarUnit() + 1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.left;
			}
		}
		else if( mouseImagePoint.x > frameImageRect.right )
		{
			if( frameImageRect.right < NVM_GetImageWidth() )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		//Dragによる単語選択
		ATextPoint	selectTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
		ATextIndex	selectTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(selectTextPoint);
		ATextIndex	wordStart, wordEnd;
		GetTextDocument().SPI_FindWord(selectTextIndex,wordStart,wordEnd);
		ATextPoint	wordStartPoint, wordEndPoint;
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,wordStartPoint,true);
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,wordEndPoint,false);
		if( wordEnd <= mClickTextIndex )
		{
			if( GetCaretTextPoint().x != mFirstSelectEnd.x || GetCaretTextPoint().y != mFirstSelectEnd.y )
			{
				SetCaretTextPoint(mFirstSelectEnd);
			}
			SetSelectTextPoint(wordStartPoint);
		}
		else
		{
			if( GetCaretTextPoint().x != mFirstSelectStart.x || GetCaretTextPoint().y != mFirstSelectStart.y )
			{
				SetCaretTextPoint(mFirstSelectStart);
			}
			SetSelectTextPoint(wordEndPoint);
		}
	}
}

//マウスクリック（Tripleクリック）
void	AView_Text::DoMouseDownTripleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
										   const ABool inInsideLineNumber )
{
	//
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectParagraphByTripleClick) == false || 
	   inInsideLineNumber == true )
	{
		//行選択
		mFirstSelectStart.x = 0;
		mFirstSelectStart.y = clickTextPoint.y;
		if( clickTextPoint.y+1 >= GetTextDocumentConst().SPI_GetLineCount() )
		{
			mFirstSelectEnd.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(clickTextPoint.y);
			mFirstSelectEnd.y = clickTextPoint.y;
		}
		else
		{
			mFirstSelectEnd.x = 0;
			mFirstSelectEnd.y = clickTextPoint.y+1;
		}
	}
	else //#222
	{
		//段落選択
		AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(clickTextPoint.y);
		mFirstSelectStart.x = 0;
		mFirstSelectStart.y = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex);
		if( paraIndex+1 >= GetTextDocumentConst().SPI_GetParagraphCount() )
		{
			mFirstSelectEnd.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(GetTextDocumentConst().SPI_GetLineCount()-1);
			mFirstSelectEnd.y = GetTextDocumentConst().SPI_GetLineCount()-1;
		}
		else
		{
			mFirstSelectEnd.x = 0;
			mFirstSelectEnd.y = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex+1);
		}
	}
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	mClickTextLineIndex = clickTextPoint.y;
	
	//マウスドラッグによる選択
	
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_TripleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
	mMouseTrackImagePoint = clickImagePoint;
	NVM_SetMouseTrackingMode(true);
	return;
	//#688 }
	/*#688
	//最初のマウス位置取得
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers);
	//Image座標に変換
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
	//前回マウス位置の保存
	AImagePoint	previousImagePoint = mouseImagePoint;
	//マウスボタンが放されるまでループ
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Image座標に変換
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		//前回マウス位置と同じなら何もせずループ継続
		if( mouseImagePoint.x == previousImagePoint.x && mouseImagePoint.y == previousImagePoint.y )
		{
			continue;
		}
		
		//Mouse Track
		DoMouseTrack_TripleClick(mouseImagePoint,modifiers);
		
		//前回マウス位置の保存
		previousImagePoint = mouseImagePoint;
	}
	*/
}

//Mouse Track (トリプルクリック)
void	AView_Text::DoMouseTrack_TripleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//表示フレームをImage座標で取得
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//表示フレーム外にマウス移動したときはスクロールする(Y方向)
		if( mouseImagePoint.y < (frameImageRect.top+kDragScrollMargin) )//#456
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > (frameImageRect.bottom-kDragScrollMargin) )//#456
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.bottom - 1;
			}
		}
		//表示フレーム外にマウス移動したときはスクロールする(X方向)
		if( mouseImagePoint.x < frameImageRect.left )
		{
			if( frameImageRect.left > 0 )
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.left - NVI_GetHScrollBarUnit() + 1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.left;
			}
		}
		else if( mouseImagePoint.x > frameImageRect.right )
		{
			if( frameImageRect.right < NVM_GetImageWidth() )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		//Dragによる行選択
		ATextPoint	selectTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
		if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectParagraphByTripleClick) == false || 
		   mCurrentMouseTrackInsideLineNumber == true )
		{
			//==================行選択==================
			if( selectTextPoint.y < mClickTextLineIndex )
			{
				ATextPoint	pt;
				pt.x = 0;
				pt.y = selectTextPoint.y;
				if( GetCaretTextPoint().x != mFirstSelectEnd.x || GetCaretTextPoint().y != mFirstSelectEnd.y )
				{
					SetCaretTextPoint(mFirstSelectEnd);
				}
				SetSelectTextPoint(pt);
			}
			else
			{
				ATextPoint	pt;
				if( selectTextPoint.y+1 >= GetTextDocumentConst().SPI_GetLineCount() )
				{
					pt.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(selectTextPoint.y);
					pt.y = selectTextPoint.y;
				}
				else
				{
					pt.x = 0;
					pt.y = selectTextPoint.y+1;
				}
				if( GetCaretTextPoint().x != mFirstSelectStart.x || GetCaretTextPoint().y != mFirstSelectStart.y )
				{
					SetCaretTextPoint(mFirstSelectStart);
				}
				SetSelectTextPoint(pt);
			}
		}
		else //#222
		{
			//==================段落選択==================
			//段落開始位置、終了位置取得
			ATextPoint	start = {0}, end = {0};
			AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(selectTextPoint.y);
			start.x = 0;
			start.y = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex);
			if( paraIndex+1 >= GetTextDocumentConst().SPI_GetParagraphCount() )
			{
				end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(GetTextDocumentConst().SPI_GetLineCount()-1);
				end.y = GetTextDocumentConst().SPI_GetLineCount()-1;
			}
			else
			{
				end.x = 0;
				end.y = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex+1);
			}
			//前方向へドラッグの場合
			if( selectTextPoint.y < mClickTextLineIndex )
			{
				if( GetCaretTextPoint().x != mFirstSelectEnd.x || GetCaretTextPoint().y != mFirstSelectEnd.y )
				{
					SetCaretTextPoint(mFirstSelectEnd);
				}
				SetSelectTextPoint(start);
			}
			//後方向へドラッグの場合
			else
			{
				if( GetCaretTextPoint().x != mFirstSelectStart.x || GetCaretTextPoint().y != mFirstSelectStart.y )
				{
					SetCaretTextPoint(mFirstSelectStart);
				}
				SetSelectTextPoint(end);
			}
		}
	}
}

//マウスクリック（4回クリック）
void	AView_Text::DoMouseDownQuadClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//quad click選択
	mClickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
	ATextIndex	wordStart, wordEnd;
	GetTextDocument().SPI_FindQuadWord(mClickTextIndex,wordStart,wordEnd);
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,mFirstSelectStart,true);
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,mFirstSelectEnd,false);
	/*#688 4回クリック時の括弧選択無し
	//括弧選択
	DoubleClickBrace(mFirstSelectStart,mFirstSelectEnd);
	*/
	//
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	
	return;
	/*#688 4回クリック時のマウスドラッグは不要と思われるのでコメントアウト
	//
	AText	word;
	GetTextDocument().SPI_GetText(wordStart,wordEnd,word);
	if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() == true )//RC2
	{
		SetIdInfo(word);//RC2
	}
	else
	{
		SetInfoHeader(word);
	}
	
	//マウスドラッグによる選択
	
	//Windowsの場合は、ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_QuadClick;
	mMouseTrackStartImagePoint = clickImagePoint;
	mMouseTrackImagePoint = clickImagePoint;
	NVM_SetMouseTrackingMode(true);
	return;
	*/
	//#688}
	/*#688
	//最初のマウス位置取得（この時点でマウスボタンが放されていたらリターン。TrackMouseDown()でマウスボタンリリースのイベントを取得したら、次はもう来ない。B0260）
	AWindowPoint	mouseWindowPoint;
	AModifierKeys	modifiers;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	//Image座標に変換
	ALocalPoint	mouseLocalPoint;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
	//前回マウス位置の保存
	AImagePoint	previousImagePoint = mouseImagePoint;
	//マウスボタンが放されるまでループ
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//Image座標に変換
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
		NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
		//前回マウス位置と同じなら何もせずループ継続
		if( mouseImagePoint.x == previousImagePoint.x && mouseImagePoint.y == previousImagePoint.y )
		{
			continue;
		}
		
		//Mouse Track
		DoMouseTrack_QuadClick(mouseImagePoint,modifiers);
		
		//前回マウス位置の保存
		previousImagePoint = mouseImagePoint;
	}
	*/
}

//Mouse Track (4回クリック)
void	AView_Text::DoMouseTrack_QuadClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//表示フレームをImage座標で取得
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//表示フレーム外にマウス移動したときはスクロールする(Y方向)
		if( mouseImagePoint.y < (frameImageRect.top+kDragScrollMargin) )//#456
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > (frameImageRect.bottom-kDragScrollMargin) )//#456
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + 
						DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragScrollMargin),inModifierKeys);//#456
			}
			else
			{
				mouseImagePoint.y = frameImageRect.bottom - 1;
			}
		}
		//表示フレーム外にマウス移動したときはスクロールする(X方向)
		if( mouseImagePoint.x < frameImageRect.left )
		{
			if( frameImageRect.left > 0 )
			{
				NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.left - NVI_GetHScrollBarUnit() + 1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.left;
			}
		}
		else if( mouseImagePoint.x > frameImageRect.right )
		{
			if( frameImageRect.right < NVM_GetImageWidth() )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		//Dragによる単語選択
		ATextPoint	selectTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
		ATextIndex	selectTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(selectTextPoint);
		ATextIndex	wordStart, wordEnd;
		GetTextDocument().SPI_FindQuadWord(selectTextIndex,wordStart,wordEnd);
		ATextPoint	wordStartPoint, wordEndPoint;
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,wordStartPoint,true);
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,wordEndPoint,false);
		if( wordEnd <= mClickTextIndex )
		{
			if( GetCaretTextPoint().x != mFirstSelectEnd.x || GetCaretTextPoint().y != mFirstSelectEnd.y )
			{
				SetCaretTextPoint(mFirstSelectEnd);
			}
			SetSelectTextPoint(wordStartPoint);
		}
		else
		{
			if( GetCaretTextPoint().x != mFirstSelectStart.x || GetCaretTextPoint().y != mFirstSelectStart.y )
			{
				SetCaretTextPoint(mFirstSelectStart);
			}
			SetSelectTextPoint(wordEndPoint);
		}
	}
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_Text::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	SPI_DoMouseTracking(inLocalPoint,inModifierKeys,false);
}
void	AView_Text::SPI_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
										  const ABool inClickedAtLineNumberArea )
{
	//#688
	//てのひらツール
	if( mMouseTrackingMode == kMouseTrackingMode_TenohiraTool )
	{
		//変化取得
		ANumber	deltaX = inLocalPoint.x - mMouseTenohiraPreviousLocalPoint.x;
		ANumber	deltaY = inLocalPoint.y - mMouseTenohiraPreviousLocalPoint.y;
		
		//前回マウス位置と同じなら何もせずループ継続
		if( deltaX == 0 && deltaY == 0 )
		{
			return;
		}
		
		//スクロール
		NVI_Scroll(-deltaX,-deltaY,true);
		
		//previousLocalPointには１つ前のマウス位置を記憶
		mMouseTenohiraPreviousLocalPoint = inLocalPoint;
		
		return;
	}
	//
	AImagePoint	imagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,imagePoint);
	if( imagePoint.x == mMouseTrackImagePoint.x && imagePoint.y == mMouseTrackImagePoint.y )   return;
	
	//#795
	//modifierを記憶
	mCurrentMouseTrackModifierKeys = inModifierKeys;
	//行番号内をクリックしたかどうかを記憶
	mCurrentMouseTrackInsideLineNumber = inClickedAtLineNumberArea;
	//image pointを記憶
	mMouseTrackImagePoint = imagePoint;
	DoMouseTrack(inModifierKeys);
}

void	AView_Text::DoMouseTrack( const AModifierKeys inModifierKeys )
{
	//#688
	//長押しによるコンテキストメニュー実行。（実行したらtrack解除）
	if( JudgeContextMenuByLeftClickLongTime(mMouseTrackImagePoint,inModifierKeys) == true )
	{
		mMouseTrackingMode = kMouseTrackingMode_None;
		NVM_SetMouseTrackingMode(false);
		return;
	}
	
	//
	ALocalPoint	originalLocalPoint;
	NVM_GetLocalPointFromImagePoint(mMouseTrackImagePoint,originalLocalPoint);
	switch(mMouseTrackingMode)
	{
	  case kMouseTrackingMode_SingleClick:
		{
			//Mouse Track
			DoMouseTrack_SingleClick(mMouseTrackImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_SingleClickInSelect:
		{
			if( mMouseTrackImagePoint.x != mMouseTrackStartImagePoint.x || mMouseTrackImagePoint.y != mMouseTrackStartImagePoint.y )
			{
				mMouseTrackResultIsDrag = true;
				//TrackingMode解除 #688 位置をDoMouseTrackEnd_SingleClickInSelect()の下から移動
				mMouseTrackingMode = kMouseTrackingMode_None;
				NVM_SetMouseTrackingMode(false);
				//
				DoMouseTrackEnd_SingleClickInSelect(mMouseTrackImagePoint,inModifierKeys);
			}
			break;
		}
	  case kMouseTrackingMode_DoubleClick:
		{
			//Mouse Track
			DoMouseTrack_DoubleClick(mMouseTrackImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_TripleClick:
		{
			//Mouse Track
			DoMouseTrack_TripleClick(mMouseTrackImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_QuadClick:
		{
			//Mouse Track
			DoMouseTrack_QuadClick(mMouseTrackImagePoint,inModifierKeys);
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
	NVM_GetImagePointFromLocalPoint(originalLocalPoint,mMouseTrackImagePoint);
}

//Mouse Tracking終了（マウスボタンUp）
void	AView_Text::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	//
	if( mMouseTrackingMode == kMouseTrackingMode_SingleClickInSelect )
	{
		DoMouseTrackEnd_SingleClickInSelect(mouseImagePoint,inModifierKeys);
	}
	//
	if( mMouseTrackingMode == kMouseTrackingMode_SingleClick )
	{
		DoMouseTrackEnd_SingleClick(mouseImagePoint,inModifierKeys);
	}
	//#823
	/*idle workタイマーにより実行
	//現在単語ハイライト
	SetCurrentWordHighlight();
	//#853
	//キーワード情報
	DisplayIdInfo(false,kIndex_Invalid,kIndex_Invalid);
	//#723
	//ワーズリストへ検索要求
	RequestWordsList();
	*/
	//idle work timer起動
	TriggerKeyIdleWork(kKeyIdleWorkType_MouseClick,kMouseClikcIdleWorkTimerValue);
	
	//TrackingMode解除
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

//#688
/**
マウスクリック長押し処理
*/
ABool	AView_Text::JudgeContextMenuByLeftClickLongTime( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	//
	if( inMouseImagePoint.x == mMouseDownImagePoint.x && inMouseImagePoint.y == mMouseDownImagePoint.y )
	{
		//elapsed tickは58.2msごとに+1、mouse button tick設定は16.7ms単位。
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kCMMouseButton) == true &&
					GetApp().SPI_GetAppElapsedTick() > mMouseDownTick + GetApp().GetAppPref().GetData_Number(AAppPrefDB::kCMMouseButtonTick)/3.5 )
		{
			//コンテキストメニュー実行
			ALocalPoint	mouseLocalPoint = {0};
			NVM_GetLocalPointFromImagePoint(inMouseImagePoint,mouseLocalPoint);
			DoContextMenu(mouseLocalPoint,inModifierKeys);
			//
			mMouseDownImagePoint.x = mMouseDownImagePoint.y = 0;
			mMouseDownTick = 0;
			return true;
		}
	}
	return false;
}

//B0260
ANumber	AView_Text::DragScrollV( const ANumber inMouseOffset, const AModifierKeys inModifierKeys )
{
	ANumber	delta = GetLineHeightPlusMargin();//#450 GetLineHeightWithMargin();
	if( true /*#844 GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kChangeScrollSpeedForMouseDrag) == true*/ )
	{
		ANumber	div = 30;
		ANumber	mouseOffset = inMouseOffset;
		if( mouseOffset > 3*div )  mouseOffset = 3*div;
		if( mouseOffset < -3*div )  mouseOffset = -3*div;
		delta *= inMouseOffset;
		delta /= div;
	}
	else
	{
		if( inMouseOffset < 0 )   delta *= -1;
	}
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) )   delta *= 5;
	
	NVI_Scroll(0,delta);
	return delta;
}

//#1026
/**
QuickLookイベント（辞書で調べる）
*/
void	AView_Text::EVTDO_QuickLook( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mCaretMode == false )
	{
		//テキスト選択中は、その選択文字列の辞書を表示
		DictionaryPopup();
	}
	else
	{
		//テキストを選択していないときは、クリック位置の単語を選択して、その単語の辞書を表示
		
		//クリック位置のテキスト位置取得
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		ATextPoint	clickTextPoint;
		GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
		
		//単語選択
		ATextIndex	clickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
		ATextIndex	wordStart, wordEnd;
		GetTextDocument().SPI_FindWord(clickTextIndex,wordStart,wordEnd);
		ATextPoint	spt, ept;
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,spt,true);
		GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,ept,false);
		AText	text;
		GetTextDocument().SPI_GetText(wordStart,wordEnd,text);
		
		//単語開始位置の表示位置を取得
		AImagePoint	ipt;
		GetImagePointFromTextPoint(spt,ipt);
		ALocalPoint	pt;
		NVM_GetLocalPointFromImagePoint(ipt,pt);
		if( NVI_GetVerticalMode() == false )
		{
			//横書き時はascent分をプラスする
			pt.y += GetLineAscent(spt.y);
		}
		else
		{
			//縦書き時は行高さ分をプラスする
			pt.y += GetLineHeightWithoutMargin(spt.y);
		}
		//辞書表示
		NVMC_ShowDictionary(text,pt);
	}
}

//コンテキストメニュー
void	AView_Text::DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	const	AItemCount	kLimitLength = 15;
	
	//#688
	//サブメニューdetach元保存変数
	AArray<AMenuRef>	subMenuArray;
	AArray<AMenuRef>	oldParentMenuArray;
	AArray<AIndex>	oldParentMenuItemIndexArray;
	//
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	globalPoint.y += GetLineHeightPlusMargin();//#450 GetLineHeightWithoutMargin();
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	if( IsInSelect(mouseImagePoint,true) == false && GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kSelectWordByContextMenu) == false )//B0432
	{
		//R0231
		const AIndex	kJumpToDef	= 0;
		//const AIndex	kIdInfo		= 1;
		//const AIndex	kDictionary	= 3;
		const AIndex	kCorrectSpell = 4;
		//const AIndex	kSelectAll	= 6;
		const AIndex	kPaste 		= 7;
		const AIndex	kTool		= 9;
		const AIndex	kRelatedFile= 11;
		
#if IMPLEMENTATION_FOR_WINDOWS
		//未対応項目
		GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextUnselected,3,false);
		GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextUnselected,4,false);
		GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextUnselected,9,false);
#endif
		
		//キャレット設定、単語取得
		AImagePoint	clickImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
		ATextPoint	clickTextPoint;
		GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
		SetSelect(clickTextPoint,clickTextPoint);//B0432
		ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
		ATextIndex	spos,epos;
		GetTextDocument().SPI_FindWord(textpos,spos,epos);
		AText	selectedtext;
		GetTextDocumentConst().SPI_GetText(spos,epos,selectedtext);
		//「定義場所に移動」
		GetTextDocument().SPI_UpdateCurrentIdentifierDefinitionList(selectedtext);
		GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextUnselected,kJumpToDef,
					GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_MoveToDefinition),
					subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
		//「ペースト」
		AText	text;
		text.SetLocalizedText("CMText_Paste");
		AText	pastetext;
		AScrapWrapper::GetClipboardTextScrap(pastetext);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
		pastetext.ReplaceChar(kUChar_LineEnd,kUChar_Space);//win
		pastetext.ReplaceChar(kUChar_Tab,kUChar_Space);//win
		//B0000 右クリックで落ちたり時間がかかる問題の対策
		if( pastetext.GetItemCount() >= kLimitLength )
		{
			pastetext.LimitLength(0,kLimitLength-3);
			pastetext.AddCstring("...");
		}
		if( pastetext.CheckUTF8Text(pastetext.GetItemCount()) == false )   pastetext.DeleteAll();
		
		//「ツール」
		text.ReplaceParamText('0',pastetext);
		GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextUnselected,/*R0231 1*/kPaste,text);
		GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextUnselected,/*R0231 3*/kTool,
					GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetToolMenuRootObjectID(),
					subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
		//「関連するファイル」
		GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextUnselected,/*R0231 5*/kRelatedFile,
					GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_OpenImportFile),
					subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
#if IMPLEMENTATION_FOR_MACOSX
		if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
		{
			ATextArray	candidateArray;
			ASpellCheckerWrapper::GuessWord(selectedtext,candidateArray,
					GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Number(AModePrefDB::kSpellCheckLanguage));
			//現在のメニュー項目を削除
			AObjectID	correctSpellMenuObjectID = GetApp().SPI_GetCorrectSpellMenuObjectID();
			GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByObjectID(correctSpellMenuObjectID);
			//
			for( AIndex i = 0; i < candidateArray.GetItemCount(); i++ )
			{
				AText	menutext;
				candidateArray.Get(i,menutext);
				GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(correctSpellMenuObjectID,menutext,menutext,kObjectID_Invalid,NULL,0,false);
			}
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextUnselected,
						kCorrectSpell,correctSpellMenuObjectID,
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
		}
#endif
		/*#858
		{
			//#138
			//「付箋紙」
			AIndex	commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(clickTextPoint.y);
			if( commentIndex == kIndex_Invalid )
			{
				text.SetLocalizedText("CMText_Fusen_New");
			}
			else
			{
				text.SetLocalizedText("CMText_Fusen_Edit");
			}
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextUnselected,kExternalComment,text);
		}
		*/
		//コンテキストメニュー表示
		//B0000
		//#688 GetApp().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_TextUnselected,globalPoint,NVM_GetWindow().NVI_GetWindowRef());//win 080712
		NVMC_ShowContextMenu(kContextMenuID_TextUnselected,globalPoint);//#688
		
		//#688 return;
	}
	else
	{
		if( IsInSelect(mouseImagePoint,true) == false )
		{
			//単語選択
			AImagePoint	clickImagePoint;
			NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
			ATextPoint	clickTextPoint;
			GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
			ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			ATextIndex	spos,epos;
			GetTextDocument().SPI_FindWord(textpos,spos,epos);
			SetSelect(spos,epos);
		}
		{
			//R0231
			const AIndex	kJumpToDef	= 0;
			//const AIndex	kIdInfo		= 1;
			//const AIndex	kDefineValue= 2;
			//const AIndex	kDictionary	= 4;
			const AIndex	kCorrectSpell = 5;
			//const AIndex	kCopy 		= 7;
			//const AIndex	kCut 		= 8;
			const AIndex	kPaste		= 9;
			//const AIndex	kClear		= 10;
			const AIndex	kFind		= 12;
			//const AIndex	kMFFind		= 13;
			const AIndex	kNext		= 14;
			const AIndex	kPrev		= 15;
			const AIndex	kTransliterate= 17;
			const AIndex	kTool		= 18;
			const AIndex	kRelatedFile= 20;
			//機能制限const AIndex	kCorrectSpell=14;
			//const AIndex	kPrintRange	= 23;//#524
			
#if IMPLEMENTATION_FOR_WINDOWS
			//未対応項目
			GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextSelected,3,false);
			GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextSelected,4,false);
			GetApp().NVI_GetMenuManager().SetContextMenuEnableMenuItem(kContextMenuID_TextSelected,16,false);
#endif
			
			//「ペースト」
			AText	text;
			text.SetLocalizedText("CMText_Paste");
			AText	pastetext;
			AScrapWrapper::GetClipboardTextScrap(pastetext);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
			pastetext.ReplaceChar(kUChar_LineEnd,kUChar_Space);//win
			pastetext.ReplaceChar(kUChar_Tab,kUChar_Space);//win
			//B0000 右クリックで落ちたり時間がかかる問題の対策
			if( pastetext.GetItemCount() >= kLimitLength )
			{
				pastetext.LimitLength(0,kLimitLength-3);
				pastetext.AddCstring("...");
			}
			if( pastetext.CheckUTF8Text(pastetext.GetItemCount()) == false )   pastetext.DeleteAll();
			
			text.ReplaceParamText('0',pastetext);
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,/*R0231 2*/kPaste,text);
			//「検索」
			text.SetLocalizedText("CMText_Find");
			AText	selectedtext;
			GetSelectedText(selectedtext);
			//B0000 
			AText	selectedtext1;
			selectedtext1.SetText(selectedtext);
			selectedtext1.ReplaceChar(kUChar_LineEnd,kUChar_Space);//win
			selectedtext1.ReplaceChar(kUChar_Tab,kUChar_Space);//win
			if( selectedtext1.GetItemCount() >= kLimitLength )
			{
				selectedtext1.LimitLength(0,kLimitLength-3);
				selectedtext1.AddCstring("...");
			}
			if( selectedtext1.CheckUTF8Text(selectedtext.GetItemCount()) == false )   selectedtext1.DeleteAll();
			
			text.ReplaceParamText('0',selectedtext1);
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,/*R0231 5*/kFind,text);
			//「次を検索」
			text.SetLocalizedText("CMText_FindNext");
			text.ReplaceParamText('0',selectedtext1);
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,/*R0231 7*/kNext,text);
			//「前を検索」
			text.SetLocalizedText("CMText_FindPrev");
			text.ReplaceParamText('0',selectedtext1);
			GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,/*R0231 8*/kPrev,text);
			//「ツール」
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,/*R0231 12*/kTool,
						GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetToolMenuRootObjectID(),
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			//「文字種変換」
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,/*R0231 11*/kTransliterate,
						GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_Transliterate),
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			//「定義場所に移動」
			GetTextDocument().SPI_UpdateCurrentIdentifierDefinitionList(selectedtext);
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,/*R0231 10*/kJumpToDef,
						GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_MoveToDefinition),
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			//「関連するファイル」
			GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,/*R0231 14*/kRelatedFile,
						GetApp().NVI_GetMenuManager().GetDynamicMenuObjectIDByMenuItemID(kMenuItemID_OpenImportFile),
						subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			//R0199
#if IMPLEMENTATION_FOR_MACOSX
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_4 )
			{
				ATextArray	candidateArray;
				ASpellCheckerWrapper::GuessWord(selectedtext,candidateArray,
							GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Number(AModePrefDB::kSpellCheckLanguage));
				//現在のメニュー項目を削除
				AObjectID	correctSpellMenuObjectID = GetApp().SPI_GetCorrectSpellMenuObjectID();
				GetApp().NVI_GetMenuManager().DeleteAllDynamicMenuItemsByObjectID(correctSpellMenuObjectID);
				//
				for( AIndex i = 0; i < candidateArray.GetItemCount(); i++ )
				{
					AText	menutext;
					candidateArray.Get(i,menutext);
					GetApp().NVI_GetMenuManager().AddDynamicMenuItemByObjectID(correctSpellMenuObjectID,menutext,menutext,kObjectID_Invalid,NULL,0,false);
				}
				GetApp().NVI_GetMenuManager().SetContextMenuItemSubMenuFromDynamicMenu(kContextMenuID_TextSelected,kCorrectSpell,correctSpellMenuObjectID,
							subMenuArray,oldParentMenuArray,oldParentMenuItemIndexArray);//#688
			}
#endif
			/*#858
			{
				//#138
				//「付箋紙」
				ATextPoint	spt, ept;
				SPI_GetSelect(spt,ept);
				AIndex	commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(spt.y);
				if( commentIndex == kIndex_Invalid )
				{
					text.SetLocalizedText("CMText_Fusen_New");
				}
				else
				{
					text.SetLocalizedText("CMText_Fusen_Edit");
				}
				GetApp().NVI_GetMenuManager().SetContextMenuItemText(kContextMenuID_TextSelected,kExternalComment,text);
			}
			*/
			//コンテキストメニュー表示
			//B0000
			//#688 GetApp().NVI_GetMenuManager().ShowContextMenu(kContextMenuID_TextSelected,globalPoint,NVM_GetWindow().NVI_GetWindowRef());//win 080712
			NVMC_ShowContextMenu(kContextMenuID_TextSelected,globalPoint);//#688
		}
	}
	//#688
	//サブメニュー復元（元のメニューにattachする）
	for( AIndex i = 0; i < subMenuArray.GetItemCount(); i++ )
	{
		//コンテキストメニューからdetach
		AMenuRef	menuRef = NULL;
		AIndex	menuItemIndex = kIndex_Invalid;
		AMenuWrapper::DetachSubMenu(subMenuArray.Get(i),menuRef,menuItemIndex);
		//元のメニューにattach
		if( oldParentMenuArray.Get(i) != NULL )
		{
			AMenuWrapper::SetSubMenu(oldParentMenuArray.Get(i),oldParentMenuItemIndexArray.Get(i),subMenuArray.Get(i));
		}
	}
}

//#606 手のひらツール
/**
手のひらツールモードかどうかを判定
*/
ABool	AView_Text::IsHandToolMode() const
{
	//現在の手のひらツールモード状態を取得
	ABool	handtoolmode = GetApp().SPI_GetHandToolMode();
	//コマンドキー押下なら反転
	if( AKeyWrapper::IsCommandKeyPressed() == true )
	{
		handtoolmode = !handtoolmode;
	}
	return handtoolmode;
}

//カーソル
ACursorType	AView_Text::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//手のひらツールモードならOpen handカーソル #606
	/*
	if( IsHandToolMode() == true )
	{
		return kCursorType_OpenHand;
	}
	*/
	//
	if( IsCaretMode() == false )
	{
		AImagePoint	mouseImagePoint;
		NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
		ATextPoint	mouseTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,mouseTextPoint);
		if( IsInSelect(mouseImagePoint,true) == true )
		{
			if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || 
						AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true ||
						AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win
			{
				return kCursorType_ArrowCopy;
			}
			else
			{
				return kCursorType_Arrow;
			}
		}
	}
	/*
	//#450
	//折りたたみ波線位置なら上下リサイズカーソル
	if( IsMouseInFoldingCollapsedLine(inLocalPoint) == true )
	{
		return kCursorType_ResizeUpDown;
	}
	*/
	//テキストの上であってもIビームカーソルにしない設定 #1208
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kInhibitIbeamCursor) == true )
	{
		return kCursorType_Arrow;
	}
	//縦書きモードならマウスカーソルを縦書き用に変更 #558
	if( NVI_GetVerticalMode() == true )
	{
		return kCursorType_IBeamForVertical;
	}
	//
	return kCursorType_IBeam;
}

/**
折りたたみ波線位置かどうかを判定
*/
ABool	AView_Text::IsMouseInFoldingCollapsedLine( const ALocalPoint& inLocalPoint ) 
{
	AImagePoint	mouseImagePoint = {0};
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	ATextPoint	textPoint = {0};
	GetTextPointFromImagePoint(mouseImagePoint,textPoint);
	if( SPI_IsCollapsedLine(textPoint.y) == true )
	{
		//折りたたみ行の下3ptならtrueを返す
		AImageRect	lineImageRect = {0};
		GetLineImageRect(textPoint.y,lineImageRect);
		if( mouseImagePoint.y >= lineImageRect.bottom-3 && mouseImagePoint.y <= lineImageRect.bottom )
		{
			return true;
		}
	}
	return false;
}

//#606
/**
マウスクリック（手のひらツール）時処理
*/
void	AView_Text::DoMouseDownTenohira( const ALocalPoint& inFirstLocalPoint , const AModifierKeys inModifierKeys )
{
	//手のひら（つかむ）カーソル設定
	ACursorWrapper::SetCursor(kCursorType_ClosedHand);
	
	//#688
	//手のひらツールドラッグ開始
	mMouseTrackingMode = kMouseTrackingMode_TenohiraTool;
	mMouseTenohiraPreviousLocalPoint = inFirstLocalPoint;
	NVM_SetMouseTrackingMode(true);
	return;
	
	/*#688
	//最初のマウス位置取得（この時点でマウスボタンが放されていたらリターン。TrackMouseDown()でマウスボタンリリースのイベントを取得したら、次はもう来ない。B0260）
	AWindowPoint	mouseWindowPoint = {0,0};
	AModifierKeys	modifiers = 0;
	if( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == false )   return;
	
	//previousLocalPointには１つ前のマウス位置を記憶
	ALocalPoint	previousLocalPoint = {0,0};
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,previousLocalPoint);
	
	//マウスボタンが放されるまでループ
	while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
	{
		//マウス位置取得
		ALocalPoint	localPoint = {0,0};
		NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,localPoint);
		
		//変化取得
		ANumber	deltaX = localPoint.x - previousLocalPoint.x;
		ANumber	deltaY = localPoint.y - previousLocalPoint.y;
		
		//前回マウス位置と同じなら何もせずループ継続
		if( deltaX == 0 && deltaY == 0 )
		{
			continue;
		}
		
		//スクロール
		NVI_Scroll(-deltaX,-deltaY,true);
		
		//previousLocalPointには１つ前のマウス位置を記憶
		previousLocalPoint = localPoint;
	}
	
	//カーソル戻す
	ACursorWrapper::SetCursor(kCursorType_Arrow);
	*/
}

//#823
/**
現在の描画範囲を取得
*/
void	AView_Text::SPI_GetDrawLineRange( AIndex& outStartLineIndex, AIndex& outEndLineIndex ) const
{
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	outStartLineIndex = GetLineIndexByImageY(imageFrameRect.top);
	outEndLineIndex = GetLineIndexByImageY(imageFrameRect.bottom);
}

//描画データ
const AFloatNumber	kFindHighlightAlpha = 0.8;
const AFloatNumber	kIdInfoArgHighlightAlpha = 0.3;
const AFloatNumber	kCurrentWordHighlightAlpha = 0.3;
const AFloatNumber	kTextMarkerAlpha = 0.8;

//描画要求時のコールバック(AView用)
void	AView_Text::EVTDO_DoDraw()
{
	//win EVTDO_DrawPreProcess()へ移動 TempHideCaret();
	
	//非表示中は何もしない win
	if( NVI_IsVisible() == false )   return;
	
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//#621
	ANumber	imageTop = imageFrameRect.top;
	if( imageTop < 0 )
	{
		imageTop = 0;
	}
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	AIndex	startLineIndex = GetLineIndexByImageY(imageTop);
	//#450 AIndex	endLineIndex = imageFrameRect.bottom/GetLineHeightWithMargin();//存在しない行にもウインドウ分は背景を表示させるためGetLineIndexByImageY(rect.bottom);
	/*AText	title;
	document.NVI_GetTitle(title);
	title.OutputToStderr();
	fprintf(stderr,"-start:%d \n",startLineIndex);*/
	//
	const ADocument_Text&	document = GetTextDocumentConst();
	const AModeIndex	modeIndex = document.SPI_GetModeIndex();
	const AModePrefDB&	modePrefDB = GetApp().SPI_GetModePrefDB(modeIndex);
	//const AAppPrefDB&	appPrefDB = GetApp().GetAppPref();
	ABool	drawTab = ((modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayTabCode)==true)&&IsPrinting()==false);
	ABool	drawSpace = ((modePrefDB.GetModeData_Bool(AModePrefDB::kDisplaySpaceCode)==true)&&IsPrinting()==false);
	ABool	drawZenkakuSpace = ((modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayZenkakuSpaceCode)==true)&&IsPrinting()==false);
	ABool	drawMasume = (modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayGenkoyoshi) == true);
	ABool	drawLineEndCode = (modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayReturnCode) == true);
	AWrapMode	wrapMode = document.SPI_GetWrapMode();
	AItemCount	wrapLetterCount = document.SPI_GetWrapLetterCount();
	//背景描画（背景はlocalFrame全体（のうちUpdateRegion内）を一気に描く。）
	AIndex	backgroundImageIndex = modePrefDB.GetBackgroundImageIndex();
	if( modePrefDB.GetModeData_Bool(AModePrefDB::kDisplayBackgroundImage) == true && backgroundImageIndex != kIndex_Invalid )
	{
		AFloatNumber	alpha = modePrefDB.GetModeData_Number(AModePrefDB::kBackgroundImageTransparency);
		alpha /= 100;
		NVMC_DrawBackgroundImage(localFrameRect,imageFrameRect,mBackgroundImageOffset,backgroundImageIndex,alpha);
	}
	else
	{
		AColor	color;
		modePrefDB.GetModeData_Color(AModePrefDB::kBackgroundColor,color);
		NVMC_PaintRect(localFrameRect,color,1.0);
	}
	//イメージ範囲よりも前は背景色でクリアする
	if( imageFrameRect.top < 0 )
	{
		AImageRect	minusAreaImageRect = imageFrameRect;
		minusAreaImageRect.bottom = 0;
		ALocalRect	minusAreaLocalRect = {0};
		NVM_GetLocalRectFromImageRect(minusAreaImageRect,minusAreaLocalRect);
		AColor	color = kColor_White;
		modePrefDB.GetModeData_Color(AModePrefDB::kBackgroundColor,color);
		NVMC_PaintRect(minusAreaLocalRect,color,1.0);
	}
	//
	/*
	ALocalPoint	shadowpt = {localFrameRect.right-22,localFrameRect.top};
	NVMC_DrawImageFlexibleHeight(kImageID_SubTextShadow,kImageID_SubTextShadow,kImageID_SubTextShadow,shadowpt,localFrameRect.bottom-localFrameRect.top);
	*/
	//選択色取得
	AColor	selectionColor;
	//#1316 if( NVI_IsFocusActive() == true )
	{
		AColorWrapper::GetHighlightColor(selectionColor);
	}
	/*#1316
	else
	{
		selectionColor = kColor_Gray90Percent;
	}
	*/
	//選択色不透明度取得
	AFloatNumber	selectionOpacity = GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Number(AModePrefDB::kSelectionOpacity);
	selectionOpacity /= 100.0;
	//R0006 diff色取得
	AColor	diffcolor_added, diffcolor_changed, diffcolor_deleted;
	GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Changed,diffcolor_changed);
	//#379 Diff対象ドキュメント側は削除色と追加色入れ替え
	//AFloatNumber	diffDelAlpha = 0.15;
	if( GetTextDocumentConst().SPI_IsDiffTargetDocument() == false )
	{
		GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Added,diffcolor_added);
		GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Deleted,diffcolor_deleted);
	}
	else
	{
		GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Deleted,diffcolor_added);
		GetApp().GetAppPref().GetData_Color(AAppPrefDB::kDiffColor_Added,diffcolor_deleted);
		//diffDelAlpha = 0.1;
	}
	//diff表示α値取得
	AFloatNumber	diffAlpha = modePrefDB.GetModeData_Number(AModePrefDB::kDiffColorOpacity);
	diffAlpha /= 300;//0.0～0.33にする
	//濃さ設定が大きいほど彩度を下げて、明度を上げる
	AFloatNumber	diffSChange = 1.0 - diffAlpha*1.5;
	AFloatNumber	diffVChange = 1.0 + diffAlpha*2.0;
	diffcolor_added = AColorWrapper::ChangeHSV(diffcolor_added,0.0,diffSChange,diffVChange);
	diffcolor_deleted = AColorWrapper::ChangeHSV(diffcolor_deleted,0.0,diffSChange,diffVChange);
	diffcolor_changed = AColorWrapper::ChangeHSV(diffcolor_changed,0.0,diffSChange,diffVChange);
	//ウインドウがdiff modeかどうかを取得
	ABool	windowIsDiffMode = false;
	if( mTextWindowID != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetDiffDisplayMode() == true )
		{
			windowIsDiffMode = true;
		}
	}
	//#450
	AText	fontname;
	//#868 GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
	GetTextDocumentConst().SPI_GetFontName(fontname);//#868
	AColor	lettercolor;
	GetModePrefDB().GetModeData_Color(AModePrefDB::kLetterColor,lettercolor);
	AFloatNumber	normalfontsize = GetTextDocumentConst().SPI_GetFontSize();//#868
	ABool	isAntiAlias = GetTextDocument().SPI_IsAntiAlias();
	//#1142
	//制御文字色取得
	AColor	spcolor = kColor_Gray50Percent;
	GetModePrefDB().GetModeData_Color(AModePrefDB::kControlCodeColor,spcolor);
	//フォント設定
	NVMC_SetDefaultTextProperty(fontname,normalfontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
	
	//initial系状態のときは描画しない
	if( GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		/*
		NVMC_SetDefaultTextProperty(fontname,12.0,lettercolor,kTextStyle_Normal,isAntiAlias);
		ALocalRect	rect = {0};
		rect.left		= 10;
		rect.top		= 10;
		rect.right		= 1000;
		rect.bottom		= 80;
		AText	loading;
		loading.SetLocalizedText("TextView_FileLoading");
		NVMC_DrawText(rect,loading);
		*/
		return;
	}
	
	AIndex	lineCount = document.SPI_GetLineCount();//#450
	
	//マス目描画
	//#1132 行がない部分にも描画するよう修正
	if( drawMasume == true )
	{
		//#1316
		AColor	masumeColor = kColor_Gray85Percent;//#1316 80%→85%
		if( AApplication::GetApplication().NVI_IsDarkMode() == true )
		{
			masumeColor = kColor_Gray30Percent;
		}
		//行ごとのループ（最終行以降1000行までループ。ただし、frame外に出たらbreak）
		for( AIndex lineIndex = startLineIndex; lineIndex < lineCount + 1000; lineIndex++ )
		{
			//
			AImageRect	lineImageRect = {0};
			if( lineIndex < lineCount )
			{
				//行image rect取得
				GetLineImageRect(lineIndex,lineImageRect);
			}
			else
			{
				//行がない部分は、最終行の位置と高さから、行image rectを作成する。
				GetLineImageRect(lineCount-1,lineImageRect);
				ANumber	lineHeight = GetImageYByLineIndex(lineCount) - GetImageYByLineIndex(lineCount-1);
				lineImageRect.top		+= lineHeight*(lineIndex-lineCount);
				lineImageRect.bottom	+= lineHeight*(lineIndex-lineCount);
			}
			//終了判定（今表示しようとしている行のimagerectがframe外だったら終了
			if( lineImageRect.top > imageFrameRect.bottom )   break;
			//Update領域判定
			ALocalRect	lineLocalRect = {0};
			NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
			if( NVMC_IsRectInDrawUpdateRegion(lineLocalRect) == false )   continue;
			
			AIndex	start 	= imageFrameRect.left  / mZenkakuAWidth;//#1385 mZenkakuSpaceWidth;
			AIndex	end 	= imageFrameRect.right / mZenkakuAWidth;//#1385 mZenkakuSpaceWidth;
			if( wrapMode == kWrapMode_WordWrapByLetterCount || wrapMode == kWrapMode_LetterWrapByLetterCount )//#1113
			{
				end = wrapLetterCount/2;
			}
			ALocalPoint	spt, ept;
			AImagePoint	imagespt, imageept;
			for( AIndex i = start; i <= end; i++ )
			{
				imagespt.x = (ANumber)(mZenkakuAWidth * i);//#1385 mZenkakuSpaceWidth * i;
				imagespt.y = lineImageRect.top;//win 080618
				imageept.x = (ANumber)(mZenkakuAWidth * i);//#1385 mZenkakuSpaceWidth * i;
				imageept.y = imagespt.y + mLineHeight -1;//win 080618
				NVM_GetLocalPointFromImagePoint(imagespt,spt);
				NVM_GetLocalPointFromImagePoint(imageept,ept);
				NVMC_DrawLine(spt,ept,masumeColor);
			}
			//
			imagespt.x = lineImageRect.left;
			imagespt.y = lineImageRect.top;//win 080618
			imageept.x = (ANumber)(mZenkakuAWidth * end);//#1385 mZenkakuSpaceWidth * end;
			imageept.y = lineImageRect.top;//win 080618
			NVM_GetLocalPointFromImagePoint(imagespt,spt);
			NVM_GetLocalPointFromImagePoint(imageept,ept);
			NVMC_DrawLine(spt,ept,masumeColor);
			//
			imagespt.y += mLineHeight;
			imageept.y += mLineHeight;
			NVM_GetLocalPointFromImagePoint(imagespt,spt);
			NVM_GetLocalPointFromImagePoint(imageept,ept);
			NVMC_DrawLine(spt,ept,masumeColor);
		}
	}
	
	//文法パートタグと、文法チェックワーニングの配色 #1316
	AColor	warningTagBackgroundStart = kColor_White;
	AColor	warningTagBackgroundEnd = kColor_Gray95Percent;
	AColor	warningTagFrame = kColor_Gray30Percent;
	AColor	warningTagLetterColor = kColor_Gray30Percent;
	if( GetApp().NVI_IsDarkMode() == true )
	{
		warningTagBackgroundStart = AColorWrapper::GetColorByHTMLFormatColor("2C2C2C");
		warningTagBackgroundEnd = AColorWrapper::GetColorByHTMLFormatColor("282828");
		warningTagFrame = AColorWrapper::GetColorByHTMLFormatColor("909090");
		warningTagLetterColor = AColorWrapper::GetColorByHTMLFormatColor("A0A0A0");
	}
	
	//各行描画
	for( AIndex lineIndex = startLineIndex; lineIndex < lineCount/*#450endLineIndex*/; lineIndex++ )//#450
	{
		AImageRect	lineImageRect;
		GetLineImageRect(lineIndex,lineImageRect);
		//#450 非表示行判定
		if( lineImageRect.top == lineImageRect.bottom )   continue;
		//#450 終了判定（今表示しようとしている行のimagerectがframe外だったら終了
		if( lineImageRect.top > imageFrameRect.bottom )   break;
		//
		ALocalRect	lineLocalRect;
		NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
		//#688
		//描画右端はフレーム右端までにする。（差分gradient表示のため＋その他も幾分高速化になるかもしれない）
		lineLocalRect.right = localFrameRect.right;
		//Update領域判定
		if( NVMC_IsRectInDrawUpdateRegion(lineLocalRect) == false )   continue;
		
		//==================文法パートの表示==================
		{
			//現在行の文法パートindex取得
			ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
			if( syntaxDefinition.GetSyntaxPartCount() >= 2 )
			{
				AIndex	stateIndex = GetTextDocumentConst().SPI_GetStateIndexByLineIndex(lineIndex);
				AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
				if( syntaxPartIndex != kIndex_Invalid && GetTextDocumentConst().SPI_GetLineRecognized(lineIndex) == true )
				{
					//文法パート区切り線表示判定
					ABool	syntaxPartShouldShown = true;
					if( lineIndex > 0 )
					{
						if( GetTextDocumentConst().SPI_GetLineRecognized(lineIndex-1) == false )
						{
							//文法未認識なら必ず区切り表示しない
							syntaxPartShouldShown = false;
						}
						else
						{
							//前行の文法パートと、現在行の文法パートの分割線表示名称が同じだったら区切り表示しない
							AIndex	prevLineStateIndex = GetTextDocumentConst().SPI_GetStateIndexByLineIndex(lineIndex-1);
							AIndex	prevLineSyntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(prevLineStateIndex).GetStateSyntaxPartIndex();
							AText	prevLineSyntaxSeparatorName, currentLineSyntaxSeparatorName;
							if( prevLineSyntaxPartIndex != kIndex_Invalid )
							{
								syntaxDefinition.GetSyntaxPartSeparatorName(prevLineSyntaxPartIndex,prevLineSyntaxSeparatorName);
							}
							if( syntaxPartIndex != kIndex_Invalid )
							{
								syntaxDefinition.GetSyntaxPartSeparatorName(syntaxPartIndex,currentLineSyntaxSeparatorName);
							}
							if( prevLineSyntaxSeparatorName.Compare(currentLineSyntaxSeparatorName) == true )
							{
								syntaxPartShouldShown = false;
							}
						}
					}
					//
					if( syntaxPartShouldShown == true )
					{
						//#1316
						AColor	separatorColor = AColorWrapper::GetColorByHTMLFormatColor("f4edbb");
						if( GetApp().NVI_IsDarkMode() == true )
						{
							separatorColor = AColorWrapper::GetColorByHTMLFormatColor("303030");
						}
						//文法パート区切り描画
						ALocalRect	separatorRect = lineLocalRect;
						separatorRect.bottom = separatorRect.top+2;
						NVMC_PaintRectWithVerticalGradient(separatorRect,separatorColor,separatorColor,0.7,0.0);//#1316 α=0.8→0.7
						//文法パート名取得
						AText	syntaxName;
						syntaxDefinition.GetSyntaxPartName(syntaxPartIndex,syntaxName);
						NVMC_SetDefaultTextProperty(fontname,6.0,warningTagLetterColor,kTextStyle_Normal,true);//#1316
						//文法パート名フレーム描画
						ANumber	w = NVMC_GetDrawTextWidth(syntaxName);
						ALocalRect	textFrameRect = lineLocalRect;
						textFrameRect.left		= textFrameRect.right - w - 8;
						textFrameRect.bottom	= textFrameRect.top + 10;
						NVMC_PaintRoundedRect(textFrameRect,warningTagBackgroundStart,warningTagBackgroundEnd,kGradientType_Vertical,0.7,0.7,//#1316
											  3,false,false,true,false);
						NVMC_FrameRoundedRect(textFrameRect,warningTagFrame,1.0,5,false,false,true,false);//#1316
						//文法パート名描画
						ALocalRect	textDrawRect = textFrameRect;
						textDrawRect.left		+= 3;
						textDrawRect.top		+= 1;
						textDrawRect.right		-= 3;
						textDrawRect.bottom		-= 1;
						NVMC_DrawText(textDrawRect,syntaxName,kJustification_Center);
						
						//通常のフォントに戻す
						NVMC_SetDefaultTextProperty(fontname,normalfontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
					}
				}
			}
		}
		
		//#844
		//現在行描画
		{
			ATextPoint	spt = {0,0}, ept = {0,0};
			SPI_GetSelect(spt,ept);
			AColor	color = kColor_LightGray;
			GetApp().NVI_GetAppPrefDB().GetData_Color(AAppPrefDB::kCurrentLineColor,color);
			ABool	currentLine = ((spt.y==ept.y)&&(lineIndex==spt.y));
			if( currentLine == true )
			{
				//現在行（paintの場合）
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kPaintLineForCurrentLine) == true )
				{
					NVMC_PaintRect(lineLocalRect,color,0.3);
				}
				//#496 現在行（下線の場合）
				if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDrawLineForCurrentLine) == true )//#844
				{
					ALocalRect	r = lineLocalRect;
					r.top = r.bottom - 1;
					NVMC_PaintRect(r,color,0.7);
				}
			}
		}
		
		/*drop
		//#450
		AFloatNumber	fontsize = normalfontsize * document.SPI_GetLineFontSizeMultiply(lineIndex);
		NVMC_SetDefaultTextProperty(fontname,fontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
		*/
		
		//fprintf(stderr,"DRaw:%d ",lineIndex);
		//char	str[256];
		//sprintf(str,"Draw:%d \n",lineIndex);
		//OutputDebugStringA(str);
		
		//
		CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		ATextPoint	caretPoint, selectPoint;
		GetCaretTextPoint(caretPoint);
		GetSelectTextPoint(selectPoint);
		ABool	selection = (IsCaretMode()==false);
		if( mKukeiSelected == true )
		{
			selection = false;
			for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
			{
				ATextPoint	spt = mKukeiSelected_Start.Get(i);
				if( spt.y == lineIndex )
				{
					ATextPoint	ept = mKukeiSelected_End.Get(i);
					caretPoint = spt;
					selectPoint = ept;
					selection = true;
					break;
				}
			}
		}
		
		//live resize中ならstyleなしのdrawのみを行う。（高速化） #688
		if( NVI_IsInLiveResize() == true )
		{
			document.SPI_GetTextDrawDataWithoutStyle(lineIndex,textDrawData);
			NVMC_DrawText(lineLocalRect,textDrawData);
			continue;
		}
		
		AAnalyzeDrawData	analyzeDrawData;//RC1
		document.SPI_GetTextDrawDataWithStyle(lineIndex,textDrawData,selection,caretPoint,selectPoint,analyzeDrawData,GetObjectID());//RC1
		
		//背景色取得
		AColor	backgroundColor = kColor_White;//#1316
		modePrefDB.GetModeData_Color(AModePrefDB::kBackgroundColor,backgroundColor);//#1316
		
		//
		ATextIndex	lineStart = document.SPI_GetLineStart(lineIndex);
		ATextIndex	lineEnd = lineStart + document.SPI_GetLineLengthWithoutLineEnd(lineIndex);
		
		//前行開始位置取得
		ATextIndex	prevLineStart = lineStart;
		if( lineIndex-1 >= 0 )
		{
			prevLineStart = document.SPI_GetLineStart(lineIndex-1);
		}
		//次行終了位置取得
		ATextIndex	nextLineEnd = lineEnd;
		if( lineIndex+1 < document.SPI_GetLineCount() )
		{
			nextLineEnd = document.SPI_GetLineStart(lineIndex+1) 
					+ document.SPI_GetLineLengthWithoutLineEnd(lineIndex+1);
		}
		
		//==================文法チェッカーワーニングの表示==================
		//#992
		if( GetTextDocumentConst().SPI_IsParagraphStartLine(lineIndex) == true )
		{
			//段落番号取得
			AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(lineIndex);
			//文法チェッカーワーニングデータ取得
			ANumberArray	colIndexArray;
			ATextArray	titleArray,detailArray;
			AColorArray	colorArray;
			ABoolArray	displayInTextViewArray;
			GetTextDocumentConst().SPI_GetCheckerPluginWarnings(paraIndex,colIndexArray,titleArray,detailArray,colorArray,displayInTextViewArray);
			//ワーニングデータが1つ以上あるかどうかの判定
			if( colIndexArray.GetItemCount() > 0 )
			{
				//TextView内に表示するかどうかのフラグ（1つでもTextViewに表示すべきワーニングデータがあれば、フラグONにする）
				ABool	displayInTextView = false;
				for( AIndex i = 0; i < displayInTextViewArray.GetItemCount(); i++ )
				{
					if( displayInTextViewArray.Get(i) == true )
					{
						/*
						★未対応（colIndex→text positionへの変換検討必要。表示デザイン検討必要。
						if( colIndexArray.Get(i) >= 0 )
						{
							//
							AImagePoint	colImagePoint = {0};
							colImagePoint.x = NVMC_GetImageXByTextPosition(textDrawData,colIndexArray.Get(i));
							colImagePoint.y = GetImageYByLineIndex(lineIndex)+1;
							ALocalPoint	spt = {0}, ept = {0};
							NVM_GetLocalPointFromImagePoint(colImagePoint,spt);
							ept = spt;
							ept.y += 2;
							NVMC_DrawLine(spt,ept,kColor_Gray50Percent,0.5);
						}
						*/
						//フラグON
						displayInTextView = true;
					}
				}
				//TextView内に表示するかどうかの判定
				if( displayInTextView == true )
				{
					//ワーニングテキスト取得（最初のワーニングのデータを取得）
					AText	warningTitle;
					if( titleArray.GetItemCount() > 0 )
					{
						titleArray.Get(0,warningTitle);
					}
					//本文の右端の位置を取得
					AImagePoint	imagePoint = {0};
					imagePoint.x = NVMC_GetImageXByTextPosition(textDrawData,lineEnd-lineStart);
					imagePoint.y = GetImageYByLineIndex(lineIndex)+1;
					ALocalPoint	localPoint = {0};
					NVM_GetLocalPointFromImagePoint(imagePoint,localPoint);
					//ワーニング表示領域可能幅取得
					ANumber	maxWidthForWarning = localFrameRect.right - localPoint.x - 32;
					//表示可能かどうかの判定
					if( maxWidthForWarning > 16 )
					{
						//ワーニング表示領域は画面の1/5を最大とする
						if( maxWidthForWarning > (lineLocalRect.right - lineLocalRect.left)/5 )
						{
							maxWidthForWarning = (lineLocalRect.right - lineLocalRect.left)/5;
						}
						//フォント設定
						AFloatNumber	fs = 9.5;//#0 9.0→9.5
						if( fs > normalfontsize )   fs = normalfontsize;
						AText	f("Helvetica");
						NVMC_SetDefaultTextProperty(f,fs,warningTagLetterColor,kTextStyle_Normal,true);//#1316
						//テキスト表示幅取得
						ANumber	w = NVMC_GetDrawTextWidth(warningTitle);
						//テキスト表示幅がワーニング表示領域を超える場合は...表示にする
						AText	warningTitleEllipsis;
						if( w > maxWidthForWarning )
						{
							w = maxWidthForWarning;
							NVI_GetEllipsisTextWithFixedLastCharacters(warningTitle,w,0,warningTitleEllipsis,true);
						}
						else
						{
							warningTitleEllipsis.SetText(warningTitle);
						}
						//ワーニング表示領域計算
						ALocalRect	textFrameRect = lineLocalRect;
						textFrameRect.left = textFrameRect.right - w - 16;
						textFrameRect.right -= 2;
						textFrameRect.top++;
						if( textFrameRect.bottom - textFrameRect.top > 16 )//#0 14→16
						{
							textFrameRect.bottom	= textFrameRect.top + 16;//#0 14→16
						}
						//枠描画
						NVMC_PaintRoundedRect(textFrameRect,warningTagBackgroundStart,warningTagBackgroundEnd,kGradientType_Vertical,0.7,0.7,//#1316
											  3,true,true,true,true);
						NVMC_FrameRoundedRect(textFrameRect,warningTagFrame,1.0,5,true,true,true,true);//#1316
						//テキスト描画
						ALocalRect	textDrawRect = textFrameRect;
						textDrawRect.left		+= 6;
						textDrawRect.top		+= 1;
						textDrawRect.right		-= 8;
						textDrawRect.bottom		-= 1;
						NVMC_DrawText(textDrawRect,warningTitleEllipsis,kJustification_Left);
						
						//通常のフォントに戻す
						NVMC_SetDefaultTextProperty(fontname,normalfontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
					}
				}
			}
		}
		//==================diff表示==================
		//現在行のdiffタイプ取得
		ADiffType	difftype = GetTextDocumentConst().SPI_GetDiffTypeByIncludingLineIndex(lineIndex);
		/*
		//#790
		ADiffType	difftype = kDiffType_None;
		AIndex	diffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(lineIndex);
		if( diffIndex != kIndex_Invalid )
		{
			difftype = GetTextDocumentConst().SPI_GetDiffTypeByDiffIndex(diffIndex);
		}
		
		//
		if( mTextWindowID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsMainSubDiffMode(NVI_GetControlID()) == true )
			{
				diffIndex = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_MainSubDiff_GetDiffIndexByIncludingLineIndex(
							NVI_GetControlID(),lineIndex,difftype);
			}
		}
		*/
		//#790
		//diffパート枠線表示有無取得
		ABool	diffPartStartLine = false;
		ABool	diffPartEndLine = false;
		AFloatNumber	diffPartFrameLineAlphaMultiply = 3.0;//#1394 ANumber→AFloatNumber
		AFloatNumber	diffLetterChangedLetterMultiply = 0.5;//変更文字乗数 下のnot changedからの増加分 #1316 1.0→0.5
		AFloatNumber	diffLetterChangedLinesMultiply = 0.0;//変更行乗数 下のnot changedからの増加分 #1316 0.3→0.0
		AFloatNumber	diffLetterNotChangedLetterMultiply = 0.4;//変更なし文字乗数 #1316 
		if( GetApp().NVI_IsDarkMode() == false )
		{
			//ライトモード時はダークモード時よりも濃いめ。
			diffLetterChangedLetterMultiply += 0.2;
			diffLetterChangedLinesMultiply += 0.3;
		}
		
		//#1316 比較モードでない場合も上下線は表示する if( windowIsDiffMode == true )
		{
			//開始線表示判定
			if( lineIndex-1 >= 0 )
			{
				if( GetTextDocumentConst().SPI_GetDiffTypeByIncludingLineIndex(lineIndex-1) != difftype )
				{
					diffPartStartLine = true;
				}
			}
			else
			{
				diffPartStartLine = true;
			}
			//終了線表示判定
			if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
			{
				if( GetTextDocumentConst().SPI_GetDiffTypeByIncludingLineIndex(lineIndex+1) != difftype )
				{
					diffPartEndLine = true;
				}
			}
			else
			{
				diffPartEndLine = true;
			}
			/*
			if( diffPartStartLine == true || diffPartEndLine == true )
			{
				AIndex	lineDiffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(lineIndex);
				ATextPoint	spt = {0,0}, ept = {0,0};
				SPI_GetSelect(spt,ept);
				AIndex	selDiffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(spt.y);
				if( lineDiffIndex == selDiffIndex )
				{
					diffPartFrameLineAlphaMultiply = 8;
				}
			}
			*/
			//#1316
			//比較モードでない場合、上下線は少し薄く表示する
			if( windowIsDiffMode == false )
			{
				if( GetApp().NVI_IsDarkMode() == false )
				{
					//#1394 diffPartFrameLineAlphaMultiply *= 0.5;
					diffPartFrameLineAlphaMultiply = 1.0;//#1394 1→1.0で変更なし
				}
				else
				{
					//#1394 diffPartFrameLineAlphaMultiply *= 0.7;
					diffPartFrameLineAlphaMultiply = 1.5;//#1394 2→1.5
				}
			}
		}
		//
		if( difftype != kDiffType_None )
		{
			/*描画更新が必要になるので、とりあえずコメントアウト
			//#790
			AIndex	diffStartLineIndex = GetTextDocumentConst().SPI_GetDiffStartLineIndexByDiffIndex(diffIndex);
			AIndex	diffEndLineIndex = GetTextDocumentConst().SPI_GetDiffEndLineIndexByDiffIndex(diffIndex);
			AFloatNumber	alphaMul = 1.0;
			if( caretPoint.y >= diffStartLineIndex && caretPoint.y <= diffEndLineIndex )
			{
				alphaMul = 2.0;
			}
			*/
			//
			switch(difftype)
			{
			  case kDiffType_Added:
				{
					AColor	color2 = AColorWrapper::ChangeHSV(diffcolor_added,0,1.6,1.0);
					//NVMC_PaintRectWithHorizontalGradient(lineLocalRect,diffcolor_added,color2,0.08,0.08);
					NVMC_PaintRect(lineLocalRect,diffcolor_added,diffAlpha*0.5);//#1316 追加行の色は少し薄くする α1.0→0.5
					//枠線描画
					//#1316 if( windowIsDiffMode == true )
					{
						if( diffPartStartLine == true )
						{
							ALocalRect	frameLine = lineLocalRect;
							frameLine.bottom = frameLine.top + 1;
							NVMC_PaintRect(frameLine,diffcolor_added,diffAlpha*diffPartFrameLineAlphaMultiply);
						}
						if( diffPartEndLine == true )
						{
							ALocalRect	frameLine = lineLocalRect;
							frameLine.top = frameLine.bottom - 1;
							NVMC_PaintRect(frameLine,diffcolor_added,diffAlpha*diffPartFrameLineAlphaMultiply);
						}
					}
					break;
				}
			  case kDiffType_Deleted:
				{
					if( GetTextDocumentConst().SPI_GetParagraphStartTextIndexFromLineIndex(lineIndex) ==
								GetTextDocumentConst().SPI_GetLineStart(lineIndex ) )//#379 段落開始行のみ
					{
						ALocalRect	aboverect = lineLocalRect;
						aboverect.bottom = aboverect.top + 3;
						NVMC_PaintRect(aboverect,diffcolor_deleted,diffAlpha);
						//枠線描画
						//#1316 if( windowIsDiffMode == true )
						{
							if( diffPartStartLine == true )
							{
								ALocalRect	frameLine = aboverect;
								frameLine.bottom = frameLine.top + 1;
								NVMC_PaintRect(frameLine,diffcolor_deleted,diffAlpha*diffPartFrameLineAlphaMultiply);
							}
							if( diffPartEndLine == true )
							{
								ALocalRect	frameLine = aboverect;
								frameLine.top = frameLine.bottom - 1;
								NVMC_PaintRect(frameLine,diffcolor_deleted,diffAlpha*diffPartFrameLineAlphaMultiply);
							}
						}
					}
					break;
				}
			  case kDiffType_Changed:
				{
					//NVMC_PaintRect(lineLocalRect,diffcolor_changed,0.1);
					//AColor	color2 = AColorWrapper::ChangeHSV(diffcolor_changed,0,1.6,1.0);
					//NVMC_PaintRectWithHorizontalGradient(lineLocalRect,diffcolor_changed,color2,0.08,0.08);
					NVMC_PaintRect(lineLocalRect,diffcolor_changed,diffAlpha*diffLetterNotChangedLetterMultiply);
					
					//文字ごと差分描画 #379
					AArray<AIndex>	charDiffStartArray, charDiffEndArray;
					GetTextDocument().SPI_GetCharDiffInfo(lineIndex,charDiffStartArray,charDiffEndArray);
					for( AIndex i = 0; i < charDiffStartArray.GetItemCount(); i++ )
					{
						//
						ATextIndex	charDiffStart = charDiffStartArray.Get(i);
						ATextIndex	charDiffEnd = charDiffEndArray.Get(i);
						if( charDiffEnd <= lineStart || charDiffStart > lineEnd )   continue;
						//
						AImageRect	imageRect = lineImageRect;
						if( charDiffStart > lineStart && charDiffStart <= lineEnd )
						{
							imageRect.left = NVMC_GetImageXByTextPosition(textDrawData,charDiffStart-lineStart);
						}
						if( charDiffEnd > lineStart && charDiffEnd <= lineEnd )
						{
							imageRect.right	= NVMC_GetImageXByTextPosition(textDrawData,charDiffEnd-lineStart);
						}
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						if( localFrameRect.right < localRect.left )   break;//win これ以上は描画不要なので
						NVMC_PaintRect(localRect,diffcolor_changed,diffAlpha*diffLetterChangedLetterMultiply);
						//ウインドウが左右比較中の場合は、文字毎の差分部分を四角で囲む
						if( windowIsDiffMode == true )
						{
							NVMC_FrameRect(localRect,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply*10);//#1316 文字毎比較の範囲は目立たせたいので、他の枠線の10倍のalphaにする。（他の枠線を目立たせると見づらくなるので、文字毎枠線だけにする。）
						}
						//
						if( localRect.left == localRect.right )
						{
							ALocalPoint	spt = {localRect.left,localRect.top-1};
							ALocalPoint	ept = {localRect.left,localRect.bottom-1};
							NVMC_DrawLine(spt,ept,diffcolor_changed,0.5,0.0,2.0);
						}
						/*
						{
							ALocalPoint	spt = {localRect.right,localRect.top};
							ALocalPoint	ept = {localRect.right,localRect.bottom};
							NVMC_DrawLine(spt,ept,diffcolor_changed,0.5);
						}
						*/
					}
					if( charDiffStartArray.GetItemCount() == 0 )
					{
						NVMC_PaintRect(lineLocalRect,diffcolor_changed,diffAlpha*diffLetterChangedLinesMultiply);
					}
					//枠線描画
					//#1316 if( windowIsDiffMode == true )
					{
						if( diffPartStartLine == true )
						{
							ALocalRect	frameLine = lineLocalRect;
							frameLine.bottom = frameLine.top + 1;
							NVMC_PaintRect(frameLine,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply);
						}
						if( diffPartEndLine == true )
						{
							ALocalRect	frameLine = lineLocalRect;
							frameLine.top = frameLine.bottom - 1;
							NVMC_PaintRect(frameLine,diffcolor_changed,diffAlpha*diffPartFrameLineAlphaMultiply);
						}
					}
					break;
				}
				//選択範囲比較時の比較範囲外
			  case kDiffType_OutOfDiffRange:
				{
					NVMC_PaintRect(lineLocalRect,kColor_Gray,diffAlpha*3);
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
		}
		
		//
		if( lineIndex >= document.SPI_GetLineCount() )   continue;
		
		//#450
		//折りたたみ波線表示
		if( SPI_GetFoldingCollapsed(lineIndex) == true )
		{
			ALocalPoint	foldingLineSpt, foldingLineEpt;
			foldingLineSpt.x = lineLocalRect.left;
			foldingLineSpt.y = lineLocalRect.bottom - 3;
			foldingLineEpt.x = lineLocalRect.right;
			foldingLineEpt.y = lineLocalRect.bottom - 3;
			//NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.8,3.0);
			foldingLineSpt.x += 3;
			foldingLineSpt.y++;
			foldingLineEpt.y++;
			NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.7,3.0);
			foldingLineSpt.x -= 3;
			foldingLineSpt.y++;
			foldingLineEpt.y++;
			NVMC_DrawLine(foldingLineSpt,foldingLineEpt,kColor_Orange,0.7,3.0);
		}
		
		//#9 改行コード描画の位置を検索ハイライト描画後から移動
		//改行コード描画 B0000 高速化
		//#889 AColor	spcolor;
		//#889 modePrefDB.GetData_Color(AModePrefDB::kSpecialCodeColor,spcolor);
		if( document.SPI_GetLineExistLineEnd(lineIndex) == true && drawLineEndCode == true && IsPrinting() == false )
		{
			textDrawData.AddText(mLineEndDisplayText,mLineEndDisplayText_UTF16,spcolor);//#1142 kColor_Gray);//#889 spcolor);//#532
		}
		//==================制御コード赤表示==================
		/*#473 textDrawData内のcontrolCodeStartUTF8Index/controlCodeEndUTF8Indexに従うように変更
		if( drawControlCode == true )
		{
			for( AIndex pos = lineStart; pos < lineEnd; pos = document.SPI_GetNextCharTextIndex(pos) )
			{
				AUChar	ch = document.SPI_GetTextChar(pos);
				if( ch < 0x20 && ch != kUChar_Tab && ch != kUChar_CR )
				{
					AImageRect	imageRect = lineImageRect;
					imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart) + 1;
					imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+1) - 1;
					imageRect.top++;
					imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);//#450 GetLineHeightWithoutMargin();
					ALocalRect	localRect;
					NVM_GetLocalRectFromImageRect(imageRect,localRect);
					if( localFrameRect.right < localRect.left )   break;//win これ以上は描画不要なので
					NVMC_PaintRect(localRect,kColor_Red,0.8);
				}
			}
		}
		*/
		//#473 textDrawData内のcontrolCodeStartUTF8Index/controlCodeEndUTF8Indexに従って制御コード赤表示
		AItemCount	controlCodeCount = textDrawData.controlCodeStartUTF8Index.GetItemCount();
		for( AIndex i = 0; i < controlCodeCount; i++ )
		{
			//開始・終了位置取得
			AIndex	s = textDrawData.controlCodeStartUTF8Index.Get(i);
			AIndex	e = textDrawData.controlCodeEndUTF8Index.Get(i);
			//赤表示領域計算
			AImageRect	imageRect = lineImageRect;
			imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,s) + 1;
			imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,e) - 1;
			imageRect.top++;
			imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
			ALocalRect	localRect;
			NVM_GetLocalRectFromImageRect(imageRect,localRect);
			if( localFrameRect.right < localRect.left )   break;
			//描画
			NVMC_PaintRoundedRect(localRect,kColor_Red,kColor_Red,kGradientType_None,0.8,0.8,3,true,true,true,true);
			//フレーム外になったらループ終了（高速化のため）
			if( imageRect.left > imageFrameRect.right )
			{
				break;
			}
		}
		
		//==================ヒント表示==================
		AHashNumberArray	hintTextStartUTF8IndexHashArray;
		AItemCount	hintTextCount = textDrawData.hintTextStartUTF8Index.GetItemCount();
		/*#1316 高速化。行ごとに取得する必要がないので。
		AColor	selectionColor = kColor_Black;
		AColorWrapper::GetHighlightColor(selectionColor);
		*/
		for( AIndex i = 0; i < hintTextCount; i++ )
		{
			//
			AIndex	s = textDrawData.hintTextStartUTF8Index.Get(i);
			hintTextStartUTF8IndexHashArray.Add(s);
			AIndex	e = textDrawData.hintTextEndUTF8Index.Get(i);
			//赤表示領域計算
			AImageRect	imageRect = lineImageRect;
			imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,s) + 1;
			imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,e) - 1;
			imageRect.top++;
			imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
			ALocalRect	localRect;
			NVM_GetLocalRectFromImageRect(imageRect,localRect);
			if( localFrameRect.right < localRect.left )   break;
			//描画
			NVMC_PaintRoundedRect(localRect,selectionColor,selectionColor,kGradientType_None,0.5,0.5,999,true,true,true,true);
			NVMC_FrameRoundedRect(localRect,kColor_Blue,1.0,999,true,true,true,true);
			//フレーム外になったらループ終了（高速化のため）
			if( imageRect.left > imageFrameRect.right )
			{
				break;
			}
		}
		
        //段落開始位置・終了位置取得
        AIndex    paragraphStartLineIndex = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(lineIndex);
        //AIndex    paragraphEndLineIndex = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(lineIndex);
        ATextIndex    paraStart = GetTextDocumentConst().SPI_GetLineStart(paragraphStartLineIndex);
        ATextIndex    paraEnd = GetTextDocumentConst().SPI_GetNextLineEndCharIndex(paraStart);//折り返し計算対象行の場合、1行にたくさんの文字があるので、改行コードまでに限定する
        
		//#1316 処理位置を検索ハイライトの後から前へ移動。
		//#750
		//==================現在単語ハイライト==================
		{
			AColor	color = kColor_Yellow;
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kCurrentWordHighlightColor,color);//#844
			//GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);
			//B0358 段落全体で検索するように修正
			//段落内で検索するが、前行の最初から次行の最後までを最大範囲とする
			AIndex	start = paraStart;
			AIndex	end = paraEnd;
			if( start < prevLineStart )
			{
				//前行の最初から検索する
				start = prevLineStart;
			}
			if( end > nextLineEnd )
			{
				//次行の最後までを検索する
				end = nextLineEnd;
			}
			for( ATextIndex pos = start; pos < end; )
			{
				//#695 段落が長すぎる場合に処理時間かかりすぎるのを防止
				//if( pos > paraStart + kLimit_TextHighlightFindRange )   break;
				//
				ATextIndex	spos, epos;
				if( GetTextDocument().SPI_FindForCurrentWordHighlight(pos,end,spos,epos) == false )   break;
				if( epos > spos )
				{
					pos = epos;
					if( spos < lineStart )   spos = lineStart;
					if( epos > lineEnd )   epos = lineEnd;
					if( epos < lineStart )   epos = lineStart;//不要だと思うが念のため
					if( spos > lineEnd )   spos = lineEnd;//不要だと思うが念のため
					if( spos < epos )
					{
						AImageRect	imageRect = lineImageRect;
						imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,spos-lineStart);
						imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,epos-lineStart);
						imageRect.top;
						imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						//ダークモードかどうかの判定 #1316
						if( GetApp().NVI_IsDarkMode() == false )
						{
							NVMC_PaintRect(localRect,color,selectionOpacity/2.0);
						}
						else
						{
							//ダークモードの場合は、塗りつぶしを薄くする #1316
							NVMC_PaintRect(localRect,color,selectionOpacity*0.5);
							NVMC_FrameRect(localRect,color,0.05);
						}
						//フレーム外になったらループ終了（高速化のため）
						if( imageRect.left > imageFrameRect.right )
						{
							break;
						}
					}
				}
				else
				{
					//spos==eposの場合（正規表現検索ではありうる）
					pos += GetTextDocument().SPI_GetNextCharTextIndex(pos);
				}
			}
		}
		
		//==================検索ハイライト（前回一致）==================
		
		//検索ハイライト2 R0244
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindHighlight2) == true )
		{
			//R0244
			AColor	color;
			//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor2,color);
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kFindHighlightColorPrev,color);//#844
			//B0358 段落全体で検索するように修正
			for( ATextIndex pos = paraStart; pos < paraEnd; )
			{
				//#695 段落が長すぎる場合に処理時間かかりすぎるのを防止
				if( pos > paraStart + kLimit_TextHighlightFindRange )   break;
				//
				ATextIndex	spos, epos;
				if( GetTextDocument().SPI_FindForHighlight2(pos,paraEnd,spos,epos) == false )   break;
				if( epos > spos )
				{
					pos = epos;
					if( spos < lineStart )   spos = lineStart;
					if( epos > lineEnd )   epos = lineEnd;
					if( epos < lineStart )   epos = lineStart;//不要だと思うが念のため
					if( spos > lineEnd )   spos = lineEnd;//不要だと思うが念のため
					if( spos < epos )
					{
						AImageRect	imageRect = lineImageRect;
						imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,spos-lineStart);
						imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,epos-lineStart);
						imageRect.top;
						imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);//#450 GetLineHeightWithoutMargin();
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						/*R0244 AColor	color;
						GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);*/
						//ダークモードかどうかの判定 #1316
						if( GetApp().NVI_IsDarkMode() == false )
						{
							NVMC_PaintRect(localRect,color,selectionOpacity);
						}
						else
						{
							//ダークモードの場合は、塗りつぶしを薄くする #1316
							NVMC_PaintRect(localRect,backgroundColor,1.0);//他の塗りつぶしをキャンセルするためまず背景色で塗る
							NVMC_PaintRect(localRect,color,selectionOpacity*0.5);
							NVMC_FrameRect(localRect,color,0.05);
						}
						//フレーム外になったらループ終了（高速化のため）
						if( imageRect.left > imageFrameRect.right )
						{
							break;
						}
					}
				}
				else
				{
					//spos==eposの場合（正規表現検索ではありうる）
					pos += GetTextDocument().SPI_GetNextCharTextIndex(pos);
				}
			}
		}
		//==================検索ハイライト==================
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindHighlight) == true )
		{
			//R0244
			AColor	color;
			//#844 GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kFindHighlightColor,color);//#844
			//B0358 段落全体で検索するように修正
			for( ATextIndex pos = paraStart; pos < paraEnd; )
			{
				//#695 段落が長すぎる場合に処理時間かかりすぎるのを防止
				if( pos > paraStart + kLimit_TextHighlightFindRange )   break;
				//
				ATextIndex	spos, epos;
				if( GetTextDocument().SPI_FindForHighlight(pos,paraEnd,spos,epos) == false )   break;
				if( epos > spos )
				{
					pos = epos;
					if( spos < lineStart )   spos = lineStart;
					if( epos > lineEnd )   epos = lineEnd;
					if( epos < lineStart )   epos = lineStart;//不要だと思うが念のため
					if( spos > lineEnd )   spos = lineEnd;//不要だと思うが念のため
					if( spos < epos )
					{
						AImageRect	imageRect = lineImageRect;
						imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,spos-lineStart);
						imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,epos-lineStart);
						imageRect.top;
						imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);//#450 GetLineHeightWithoutMargin();
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						/*R0244 AColor	color;
						GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);*/
						//ダークモードかどうかの判定 #1316
						if( GetApp().NVI_IsDarkMode() == false )
						{
							NVMC_PaintRect(localRect,color,selectionOpacity);
						}
						else
						{
							//ダークモードの場合は、塗りつぶしを薄くする #1316
							NVMC_PaintRect(localRect,backgroundColor,1.0);//他の塗りつぶしをキャンセルするためまず背景色で塗る
							NVMC_PaintRect(localRect,color,selectionOpacity*0.5);
							NVMC_FrameRect(localRect,color,0.05);
						}
						//フレーム外になったらループ終了（高速化のため）
						if( imageRect.left > imageFrameRect.right )
						{
							break;
						}
					}
				}
				else
				{
					//spos==eposの場合（正規表現検索ではありうる）
					pos += GetTextDocument().SPI_GetNextCharTextIndex(pos);
				}
			}
		}
		//#853
		//==================キーワード情報引数ハイライト==================
		if( mIdInfoCurrentArgHighlighted == true )
		{
			//ハイライトかどうかの判定
			if( mIdInfoCurrentArgStartTextPoint.y == lineIndex && mIdInfoCurrentArgEndTextPoint.y == lineIndex )
			{
				//ハイライト描画
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mIdInfoCurrentArgStartTextPoint.x);
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mIdInfoCurrentArgEndTextPoint.x);
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				AColor	color = kColor_Yellow;
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kCurrentWordHighlightColor,color);//#844
				NVMC_PaintRect(localRect,color,selectionOpacity/2.0);
			}
		}
		//#1406
		//==================括弧ハイライト==================
		if( mBraceHighlighted == true )
		{
			//開始括弧ハイライト
			if( mBraceHighlightStartTextPoint1.y == lineIndex && mBraceHighlightEndTextPoint1.y == lineIndex )
			{
				//ハイライト描画
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mBraceHighlightStartTextPoint1.x);
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mBraceHighlightEndTextPoint1.x);
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				AColor	color = kColor_Yellow;
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kFirstSelectionColor,color);
				NVMC_PaintRect(localRect,color,selectionOpacity);
			}
			//終了括弧ハイライト
			if( mBraceHighlightStartTextPoint2.y == lineIndex && mBraceHighlightEndTextPoint2.y == lineIndex )
			{
				//ハイライト描画
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mBraceHighlightStartTextPoint2.x);
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mBraceHighlightEndTextPoint2.x);
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				AColor	color = kColor_Yellow;
				GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kFirstSelectionColor,color);
				NVMC_PaintRect(localRect,color,selectionOpacity);
			}
		}
		//#750
		//==================テキストハイライト==================
		{
			AColor	color = kColor_Bisque;
			GetApp().SPI_GetModePrefDB(modeIndex).GetModeData_Color(AModePrefDB::kTextMarkerHightlightColor,color);//#844
			//GetApp().GetAppPref().GetData_Color(AAppPrefDB::kFindHighlightColor,color);
			//B0358 段落全体で検索するように修正
			//段落内で検索するが、前行の最初から次行の最後までを最大範囲とする
			AIndex	start = paraStart;
			AIndex	end = paraEnd;
			if( start < prevLineStart )
			{
				//前行の最初から検索する
				start = prevLineStart;
			}
			if( end > nextLineEnd )
			{
				//次行の最後までを検索する
				end = nextLineEnd;
			}
			for( ATextIndex pos = start; pos < end; )
			{
				//#695 段落が長すぎる場合に処理時間かかりすぎるのを防止
				//if( pos > paraStart + kLimit_TextHighlightFindRange )   break;
				//
				ATextIndex	spos, epos;
				if( GetTextDocument().SPI_FindForTextMarker(pos,end,spos,epos) == false )   break;
				if( epos > spos )
				{
					pos = epos;
					if( spos < lineStart )   spos = lineStart;
					if( epos > lineEnd )   epos = lineEnd;
					if( epos < lineStart )   epos = lineStart;//不要だと思うが念のため
					if( spos > lineEnd )   spos = lineEnd;//不要だと思うが念のため
					if( spos < epos )
					{
						AImageRect	imageRect = lineImageRect;
						imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,spos-lineStart);
						imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,epos-lineStart);
						imageRect.top;
						imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
						ALocalRect	localRect;
						NVM_GetLocalRectFromImageRect(imageRect,localRect);
						NVMC_PaintRect(localRect,color,selectionOpacity);
						//フレーム外になったらループ終了（高速化のため）
						if( imageRect.left > imageFrameRect.right )
						{
							break;
						}
					}
				}
				else
				{
					//spos==eposの場合（正規表現検索ではありうる）
					pos += GetTextDocument().SPI_GetNextCharTextIndex(pos);
				}
			}
		}
		//#9 改行コード描画の位置を検索ハイライト描画前へ移動
		
		//#913
		//==================マーク==================
		{
			//マーク開始位置描画
			if( mMarkStartTextPoint.y == lineIndex )
			{
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mMarkStartTextPoint.x) - 1;
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mMarkStartTextPoint.x) + 1;
				imageRect.top;
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				NVMC_PaintRoundedRect(localRect,kColor_Red,kColor_Red,kGradientType_None,0.8,0.8,2,true,false,true,false);
			}
			//マーク終了位置描画
			if( mMarkEndTextPoint.y == lineIndex )
			{
				AImageRect	imageRect = lineImageRect;
				imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,mMarkEndTextPoint.x) - 1;
				imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,mMarkEndTextPoint.x) + 1;
				imageRect.top;
				imageRect.bottom	= lineImageRect.top + GetLineHeightWithoutMargin(lineIndex);
				ALocalRect	localRect = {0};
				NVM_GetLocalRectFromImageRect(imageRect,localRect);
				NVMC_PaintRoundedRect(localRect,kColor_Red,kColor_Red,kGradientType_None,0.8,0.8,2,false,true,false,true);
			}
		}
		
		//==================テキスト描画==================
		textDrawData.selectionColor = AColorWrapper::ChangeHSV(selectionColor,0,1,1);
		textDrawData.selectionAlpha = selectionOpacity;
		textDrawData.selectionBackgroundColor = backgroundColor;//#1316
		//#262
		//検索直後選択範囲表示
		if( /*#844 GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kEnableSelectionColorForFind) == true &&*/
					mSelectionByFind == true )
		{
			//#844 GetApp().NVI_GetAppPrefDB().GetData_Color(AAppPrefDB::kSelectionColorForFind,textDrawData.selectionColor);
			modePrefDB.GetModeData_Color(AModePrefDB::kFirstSelectionColor,textDrawData.selectionColor);
			//ダークモードの場合はα=1.0にして文字色反転させる #1316
			if( GetApp().NVI_IsDarkMode() == true )
			{
				if( textDrawData.drawSelection == true )
				{
					textDrawData.selectionAlpha = 1.0;
					textDrawData.AddAttributeWithUnicodeOffset(textDrawData.selectionStart,textDrawData.selectionEnd,backgroundColor);
				}
			}
		}
		//テキスト描画実行
		/*B0000 高速化 ANumber	textWidth = */NVMC_DrawText(lineLocalRect,textDrawData);
		
		//==================特殊文字表示==================
		NVMC_SetDrawLineMode(spcolor);//#1142 lettercolor,0.3);//#889 spcolor);//B0000 高速化
		AItemCount	spaceToIndentSpaceCount = GetTextDocumentConst().SPI_GetIndentWidth();//#117
		ABool   startSpaceToIndent = modePrefDB.GetData_Bool(AModePrefDB::kStartSpaceToIndent);//#117
		for( AIndex pos = lineStart; pos < lineEnd; pos = document.SPI_GetNextCharTextIndex(pos) )
		{
			//制限値以降は描画しない#695
			if( pos > lineStart+kLimit_TextViewDoDraw_MaxXPosToDrawSpecialCharacters )
			{
				break;
			}
			//#117
			if( startSpaceToIndent == true )
			{
				if( textDrawData.startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.Find(pos-lineStart) != kIndex_Invalid )
				{
					if( drawTab == true )
					{
						AImagePoint	imageStartPoint, imageEndPoint;
						imageStartPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart);
						imageStartPoint.y = GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ +1;
						imageEndPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+spaceToIndentSpaceCount) - 2;
						imageEndPoint.y = imageStartPoint.y;
						ALocalPoint	spt, ept;
						NVM_GetLocalPointFromImagePoint(imageStartPoint,spt);
						NVM_GetLocalPointFromImagePoint(imageEndPoint,ept);
						NVMC_DrawLine(spt,ept);
						spt.x = ept.x;
						spt.y = ept.y -3;
						NVMC_DrawLine(spt,ept);
						//フレーム外になったらループ終了（高速化のため）
						if( imageStartPoint.x > imageFrameRect.right )
						{
							break;
						}
					}
					pos += spaceToIndentSpaceCount - 1;
					continue;
				}
			}
			//
			AUChar	ch = document.SPI_GetTextChar(pos);
			//改行を見つけたらそれ以降は描画しない#699
			if( ch == kUChar_LineEnd )
			{
				break;
			}
			//
			switch(ch)
			{
				//タブ描画
			  case kUChar_Tab:
				{
					if( drawTab == true )
					{
						AImagePoint	imageStartPoint, imageEndPoint;
						imageStartPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart);
						imageStartPoint.y = GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ +1;
						imageEndPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+1) - 2;
						imageEndPoint.y = imageStartPoint.y;
						ALocalPoint	spt, ept;
						NVM_GetLocalPointFromImagePoint(imageStartPoint,spt);
						NVM_GetLocalPointFromImagePoint(imageEndPoint,ept);
						NVMC_DrawLine(spt,ept);//B0000,spcolor);
						spt.x = ept.x - 1;//mLineAscent/5;
#if IMPLEMENTATION_FOR_WINDOWS
						spt.x--;
#endif
						spt.y = ept.y - 2;//mLineAscent/4;
						NVMC_DrawLine(spt,ept);//B0000,spcolor);
						//フレーム外になったらループ終了（高速化のため）
						if( imageStartPoint.x > imageFrameRect.right )
						{
							break;
						}
					}
					break;
				}
				//半角スペース描画
			  case kUChar_Space:
				{
					if( drawSpace == true )
					{
						AImagePoint	imageStartPoint, imageEndPoint; 
						imageStartPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart);
						imageStartPoint.y = GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ +1;
						imageEndPoint.x = NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+1) - 2;
						imageEndPoint.y = imageStartPoint.y;
						ALocalPoint	spt, ept;
						NVM_GetLocalPointFromImagePoint(imageStartPoint,spt);
						NVM_GetLocalPointFromImagePoint(imageEndPoint,ept);
						NVMC_DrawLine(spt,ept);//B0000,spcolor);
						ALocalPoint	pt;
						pt.x = spt.x;
						pt.y = spt.y - 2;
						NVMC_DrawLine(spt,pt);//B0000,spcolor);
						pt.x = ept.x;
						pt.y = ept.y - 2;
						NVMC_DrawLine(ept,pt);//B0000,spcolor);
						//フレーム外になったらループ終了（高速化のため）
						if( imageStartPoint.x > imageFrameRect.right )
						{
							break;
						}
					}
					break;
				}
				//全角スペース描画
			  case 0xE3:
				{
					if( drawZenkakuSpace == true )
					{
						if( pos+2 < lineEnd )
						{
							if( document.SPI_GetTextChar(pos+1) == 0x80 && document.SPI_GetTextChar(pos+2) == 0x80 )
							{
								//#450
								AFloatNumber	z = mZenkakuSpaceWidth;
								z *= mLineImageInfo_Height.Get(lineIndex);
								z /= (mLineHeight+mLineMargin);
								//
								AImageRect	imageRect = lineImageRect;
								imageRect.left		= NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart) + z/5;
								imageRect.right		= NVMC_GetImageXByTextPosition(textDrawData,pos-lineStart+3) - z/5;
								imageRect.top		= GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ - z*4/5;
								imageRect.bottom	= GetImageYByLineIndex(lineIndex) + GetLineAscent(lineIndex)/*#450 mLineAscent*/ +1;
								ALocalRect	localRect;
								NVM_GetLocalRectFromImageRect(imageRect,localRect);
								NVMC_FrameRect(localRect,spcolor,1.0);//#1142 lettercolor,0.3);//#889 spcolor,1.0);
								//フレーム外になったらループ終了（高速化のため）
								if( imageRect.left > imageFrameRect.right )
								{
									break;
								}
							}
						}
					}
					break;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
		}
		NVMC_RestoreDrawLineMode();//B0000 高速化
		//==================InlineInput ハイライト下線表示==================
		if( document.SPI_GetDisableInlineInputHilite() == false )//#187
		{
			for( AIndex index = 0; index < mHiliteLineStyleIndex.GetItemCount(); index++ )
			{
				AIndex	hiliteLineStyleIndex = mHiliteLineStyleIndex.Get(index);
				if( hiliteLineStyleIndex == kIndex_Invalid )   continue;
				/*#836
				AColor	color;
				GetApp().SPI_GetInlineInputHiliteColor(hiliteLineStyleIndex,color);
				*/
				//#836
				//背景黒時の下線が見やすいようにする。
				AColor	color = lettercolor;
				AFloatNumber	lineWidth = 2.0;//#1018
				//AFloatNumber	h = 0, s = 0, v = 0;
				//AColorWrapper::GetHSVFromRGBColor(lettercolor,h,s,v);
				switch( hiliteLineStyleIndex )
				{
					//v2.1でgrayだったもの（これはCocoaでは未使用かもしれない）
				  case 0:
				  case 1:
					{
						color = kColor_Gray;
						lineWidth = 1.0;//#1018
						break;
					}
					//v2.1で60% grayだったもの
				  case 2:
					{
						color = kColor_Gray60Percent;
						lineWidth = 1.0;//#1018
						break;
					}
					//v2.1でblackだったもの
				  default:
					{
						//処理無し（lettercolorのまま）
						break;
					}
				}
				//
				ATextPoint	spt, ept;
				document.SPI_GetTextPointFromTextIndex(mHiliteStartPos.Get(index),spt);
				document.SPI_GetTextPointFromTextIndex(mHiliteEndPos.Get(index),ept);
				if( lineIndex >= spt.y && lineIndex <= ept.y )
				{
					ATextPoint	lineTextStart, lineTextEnd;
					if( lineIndex == spt.y )
					{
						lineTextStart.x = spt.x;
					}
					else
					{
						lineTextStart.x = 0;
					}
					lineTextStart.y = lineIndex;
					if( lineIndex == ept.y )
					{
						lineTextEnd.x = ept.x;
					}
					else
					{
						lineTextEnd.x = document.SPI_GetLineLengthWithoutLineEnd(lineIndex);
					}
					lineTextEnd.y = lineIndex;
					AImagePoint	lineImageStart, lineImageEnd;
					GetImagePointFromTextPoint(lineTextStart,lineImageStart);
					GetImagePointFromTextPoint(lineTextEnd,lineImageEnd);
					ALocalPoint	lineLocalStart, lineLocalEnd;
					NVM_GetLocalPointFromImagePoint(lineImageStart,lineLocalStart);
					NVM_GetLocalPointFromImagePoint(lineImageEnd,lineLocalEnd);
					lineLocalStart.y += GetLineHeightWithoutMargin(lineIndex) -2;//#450 GetLineHeightWithoutMargin() -1;
					lineLocalEnd.y += GetLineHeightWithoutMargin(lineIndex) -2;//#450 GetLineHeightWithoutMargin() -1;
					lineLocalStart.x += 2;//#1018
					lineLocalEnd.x -= 2;//#185
					if( lineLocalEnd.x < lineLocalStart.x )   lineLocalEnd.x = lineLocalStart.x;//#185
					NVMC_DrawLine(lineLocalStart,lineLocalEnd,color,1.0,0.0,lineWidth);
				}
			}
		}
	}
	//win　EVTDO_DrawPostProcess()へ移動 RestoreTempHideCaret();
	
	//RC1
	const AAnalyzeDrawData&	localAnalyzeDrawData = document.SPI_GetCurrentLocalIdentifierAnalyzeDrawData();
	if( document.SPI_GetDrawArrowToDef() == true )
	{
		for( AIndex i = 0; i < localAnalyzeDrawData.defStartArrowToDef.GetItemCount(); i++ )
		{
			ATextPoint	tdefspt, tdefept, trefspt, trefept;
			tdefspt = localAnalyzeDrawData.defStartArrowToDef.Get(i);
			tdefept = localAnalyzeDrawData.defEndArrowToDef.Get(i);
			trefspt = localAnalyzeDrawData.refStartArrowToDef.Get(i);
			trefept = localAnalyzeDrawData.refEndArrowToDef.Get(i);
			AImagePoint	idefspt, idefept, irefspt, irefept;
			GetImagePointFromTextPoint(tdefspt,idefspt);
			GetImagePointFromTextPoint(tdefept,idefept);
			GetImagePointFromTextPoint(trefspt,irefspt);
			GetImagePointFromTextPoint(trefept,irefept);
			ALocalPoint	defspt, defept, refspt, refept;
			NVM_GetLocalPointFromImagePoint(idefspt,defspt);
			NVM_GetLocalPointFromImagePoint(idefept,defept);
			NVM_GetLocalPointFromImagePoint(irefspt,refspt);
			NVM_GetLocalPointFromImagePoint(irefept,refept);
			defspt.y += GetLineHeightWithoutMargin(tdefspt.y);//#450 GetLineHeightWithoutMargin();
			defept.y += GetLineHeightWithoutMargin(tdefspt.y);//#450 GetLineHeightWithoutMargin();
			refspt.y += GetLineHeightWithoutMargin(trefspt.y);//#450 GetLineHeightWithoutMargin();
			refept.y += GetLineHeightWithoutMargin(trefspt.y);//#450 GetLineHeightWithoutMargin();
			NVMC_DrawLine(defspt,defept,kColor_Red,0.5);
			NVMC_DrawLine(refspt,refept,kColor_Red,0.5,1.0);
			ALocalPoint	spt, ept;
			ept.x = (defspt.x+defept.x)/2;
			ept.y = defspt.y;
			spt.x = (refspt.x+refept.x)/2;
			spt.y = refspt.y;
			NVMC_DrawLine(spt,ept,kColor_Red,0.5,1.0);
		}
	}
	
	//ドラッグ中のドラッグ先キャレット描画 #1332 XorDragCaret()の内容をここに移動。マウスドラッグ中にlockFocus描画反映されないようなので、drawRect()ルートで描画する。
	if( mDragDrawn == true )
	{
		AImagePoint	caretImageStart, caretImageEnd;
		GetImagePointFromTextPoint(mDragCaretTextPoint,caretImageStart);
		caretImageEnd = caretImageStart;
		caretImageEnd.y += GetLineHeightWithMargin(mDragCaretTextPoint.y);//#450 GetLineHeightWithMargin();
		ALocalPoint	caretLocalStart, caretLocalEnd;
		NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
		NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
		//キャレット色取得（文字色と同じにする）
		AColor	modeLetterColor = kColor_Black;
		GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Color(AModePrefDB::kLetterColor,modeLetterColor);
		//キャレット太さ判定
		if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kBigCaret) == false )//#722
		{
			//太さ1
			NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true,false,1);//#1034 常にflushする
		}
		//#722 drag時のキャレット太さを通常と同じにする
		else
		{
			//太さ2
			caretLocalStart.y -= 1;
			caretLocalEnd.y += 1;
			NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true,false,2);//#1034 常にflushする
		}
	}
	//ドラッグ中のワードドラッグ先キャレット描画 #1332 XorDragCaret()の内容をここに移動。マウスドラッグ中にlockFocus描画反映されないようなので、drawRect()ルートで描画する。
	if( mDragWordDragDrop == true )
	{
		///キャレット色取得（文字色と同じにする）
		AColor	modeLetterColor = kColor_Black;
	  GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Color(AModePrefDB::kLetterColor,modeLetterColor);
		//
		AImageRect	imageRect;
		GetLineImageRect(mDragCaretTextPoint.y,imageRect);
		AImagePoint	caretImagePoint, selectImagePoint;
		GetImagePointFromTextPoint(mDragCaretTextPoint,caretImagePoint);
		GetImagePointFromTextPoint(mDragSelectTextPoint,selectImagePoint);
		imageRect.left	= caretImagePoint.x;
		imageRect.right	= selectImagePoint.x;
		ALocalRect	localRect;
		NVM_GetLocalRectFromImageRect(imageRect,localRect);
		NVMC_DrawXorCaretRect(localRect,true,true);//#1034 常にflushする
	}
}

//win
//描画前処理
void	AView_Text::EVTDO_DrawPreProcess()
{
	TempHideCaret();
}

//win
//描画後処理
void	AView_Text::EVTDO_DrawPostProcess()
{
	RestoreTempHideCaret();
	
	//#138
	//付箋紙
	//#858 SPI_UpdateFusen(false);
}

//#138
//表示／非表示切り替え
void	AView_Text::EVTDO_PreProcess_SetShowControl( const ABool inVisible )
{
	//付箋紙
	//#858 SPI_UpdateFusenVisible();
}

//マウスホイール
void	AView_Text::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
										const ALocalPoint inLocalPoint )//win 080706
{
	//fprintf(stderr,"%16X EVTDO_DoMouseWheel() - Start\n",(int)(GetObjectID().val));
	
	//#306 Option+ホイールでフォントサイズ変更
	if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true )
	{
		//現在のサイズ取得
		AFloatNumber	fontsize = GetTextDocument().SPI_GetFontSize();
		//0.5単位で変更
		/* #966 0.5単位にならない問題を修正
		ANumber	delta1 = inDeltaY*5;
		AFloatNumber	delta2 = delta1;
		delta2 = delta2/10;
		fontsize += delta2;
		*/
		if( inDeltaY > 0 )
		{
			fontsize += 0.5;
		}
		else if( inDeltaY < 0 )
		{
			fontsize -= 0.5;
		}
		//サイズ設定
		GetTextDocument().SPI_SetTemporaryFontSize(fontsize);//#966 一時フォントサイズ変更へ変更
		//メニュー更新
		GetApp().NVI_UpdateMenu();
		return;
	}
	
	//水平方向ホイール禁止なら常にdeltaXは0にする
	ANumber	deltaX = inDeltaX;
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisableHorizontalWheelScroll) == true )
	{
		deltaX = 0;
	}
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
	NVI_Scroll(deltaX*NVI_GetHScrollBarUnit()*scrollPercent/100,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100,
			   true,true,kScrollTrigger_Wheel);
	
	//fprintf(stderr,"%16X EVTDO_DoMouseWheel() - End\n",(int)(GetObjectID().val));
}

//
ABool	AView_Text::EVTDO_DoInlineInput( const AText& inText, const AItemCount inFixLen,
		const AArray<AIndex>& inHiliteLineStyleIndex, const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
		const AItemCount inReplaceLengthInUTF16,//#688
		ABool& outUpdateMenu )
{
	/*
	かなモード時にcontrol+option+@,[,]を入力すると0x1D等が入力される問題を対策しようとしたが、
	inFixLenが0になっていない（変換中状態になっている）。めんどいのでとりあえず対策しない。
	AModifierKeys modifierKeys = AKeyWrapper::GetCurrentModifierKeys();
	if( mInlineInputNotFixedTextLen == 0 && inFixLen == 1 && inText.GetItemCount() == 1 )
	{
		AUChar	ch = inText.Get(0);
		if( ch >= 0x01 && ch < 0x20 && ch != 0x09 && ch != 0x0D && modifierKeys != 0 )
		{
			ABool	updateMenu = false;
			if( EVTDO_DoTextInput(inText,kKeyBindKey_Invalid,modifierKeys,keyAction_NOP,updateMenu) == true )
			{
				return true;
			}
		}
	}
	*/
	
	//#867
	//Idle workタイマークリア
	ResetKeyIdleWorkTimer(kKeyIdleWorkTimerValue);
	
	RestoreFromBraceCheck();
	//補完入力状態解除
	ClearAbbrevInputMode(true,false);//RC2
	//
	if( KeyTool(inText) == true )   return true;
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return false;
	//B0000
	//win 下へ移動 ACursorWrapper::ObscureCursor();
	
	//#688
	//tick timerよりも前に文字入力された場合に、ここでreserveされたcaret描画を実行する。
	//（ここで描画しないとオートリピートしたとき等、キャレット描画が遅れて見える）
	ShowCaretIfReserved();
	
	//元々dirtyならメニュー状態は変わらない
	if( GetTextDocumentConst().NVI_IsDirty() == true )
	{
		outUpdateMenu = false;
	}
	else
	{
		outUpdateMenu = true;
	}
	
	//InlineInputハイライト下線の情報を削除（DeleteTextFromDocument()でDrawが発生するので、そこで矛盾が発生しないよう）
	mHiliteLineStyleIndex.DeleteAll();
	mHiliteStartPos.DeleteAll();
	mHiliteEndPos.DeleteAll();
	
	//強制確定の場合 win 
	if( inFixLen == kIndex_Invalid )
	{
		mInlineInputNotFixedTextLen = 0;
		NVMC_RefreshControl();
		return true;
	}
	
	//Undoアクションタグ記録（前回InlineInputからの続きではない場合のみUndoアクションタグ記録）
	if( mInlineInputNotFixedTextLen == 0 )
	{
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Typing);
	}
	
	//#1298
	//入力データ前処理
	//・kInputBackslashByYenKeyがtrueの場合や、JIS系の場合は、半角￥（U+00A5）をバックスラッシュ（U+005C）へ変換する
	//また、fixLenや、ハイライト位置を変換に応じてずらす。
	//（ADocument_Text::SPI_InsertText()にて変換を行っていたが、fixLenや、ハイライト位置をずらす必要があるので、ここであらかじめ処理しておくことにする。
	AText	insertText;
	insertText.SetText(inText);
	AItemCount	fixLen = inFixLen;
	AArray<AIndex>	hiliteLineStyleIndex, hiliteStartPos, hiliteEndPos;
	hiliteLineStyleIndex.SetFromObject(inHiliteLineStyleIndex);
	hiliteStartPos.SetFromObject(inHiliteStartPos);
	hiliteEndPos.SetFromObject(inHiliteEndPos);
	
	//入力文字列一文字ごとのループ
	for( AIndex pos = 0; pos < insertText.GetItemCount(); pos += insertText.GetNextCharPos(pos) )
	{
		//半角￥（U+00A5）かどうかの判定
		if( insertText.GetUTF8ByteCount(pos) == 2 )
		{
			AUChar	ch1 = insertText.Get(pos);
			AUChar	ch2 = insertText.Get(pos+1);
			if( ch1 == 0xC2 && ch2 == 0xA5 )
			{
				//kInputBackslashByYenKeyがtrue、または、JIS系の場合は、半角￥（U+00A5）をバックスラッシュ（U+005C）へ変換する
				if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true || GetTextDocument().SPI_IsJISTextEncoding() == true )
				{
					//半角￥（U+00A5）をバックスラッシュ（U+005C）へ変換
					insertText.Delete(pos,2);
					insertText.Insert1(pos,kUChar_Backslash);
					//fixLenや、ハイライト位置が、変換位置よりも後ろにある場合は、ずらす。
					if( pos < fixLen )
					{
						fixLen--;
					}
					for( AIndex i = 0; i < hiliteStartPos.GetItemCount(); i++ )
					{
						if( pos < hiliteStartPos.Get(i) )
						{
							hiliteStartPos.Set(i,hiliteStartPos.Get(i)-1);
						}
						if( pos < hiliteEndPos.Get(i) )
						{
							hiliteEndPos.Set(i,hiliteEndPos.Get(i)-1);
						}
					}
				}
			}
		}
	}
	
	AIndex	insertionTextIndex;
	AIndex	inlineInputFirstPos;
	if( mInlineInputNotFixedTextLen > 0 )
	{
		//前回InlineInputからの続きの場合
		//前回未確定分は削除する
		//DeleteTextFromDocument(mInlineInputNotFixedTextPos,mInlineInputNotFixedTextPos+mInlineInputNotFixedTextLen);
		//前回未確定部分のテキストを取得する
		AText	text;
		GetTextDocumentConst().SPI_GetText(mInlineInputNotFixedTextPos,mInlineInputNotFixedTextPos+mInlineInputNotFixedTextLen,text);
		//前回未確定テキストと、今回テキストを比較し、最初から何文字一致しているかを取得する
		AIndex	samecount = 0;
		for( ; samecount < text.GetItemCount(); samecount++ )
		{
			if( samecount >= insertText.GetItemCount() )   break;
			if( text.Get(samecount) != insertText.Get(samecount) )   break;
		}
		samecount = text.GetCurrentCharPos(samecount);
		//削除範囲計算
		AIndex	spos = mInlineInputNotFixedTextPos + samecount;
		AIndex	epos = mInlineInputNotFixedTextPos+mInlineInputNotFixedTextLen;
		if( spos < epos )
		{
			GetTextDocument().SPI_DeleteText(spos,epos,false,false);//(insertText.GetItemCount()>0));//080928 バグあるのでひとまず元に戻す（行折り返し有りで、ああああ入力→削除で、行が減る場合に次の行の表示が更新されない）,(insertText.GetItemCount()>0));//B0000 080922高速化 挿入文字列があればredrawしない
		}
		//挿入位置は前回未確定部分のうち削除しなかった部分の最後から
		insertionTextIndex = mInlineInputNotFixedTextPos + samecount;
		inlineInputFirstPos = mInlineInputNotFixedTextPos;
		//
		mInlineInputNotFixedTextPos = mInlineInputNotFixedTextPos+fixLen;
		mInlineInputNotFixedTextLen = insertText.GetItemCount() - fixLen;
		//
		insertText.Delete(0,samecount);
	}
	else
	{
		//InlineInput最初からの場合
		ATextPoint	insertionPoint;
		if( IsCaretMode() == true )
		{
			//#688
			//英数2度押し対応
			if( inReplaceLengthInUTF16 > 0 )
			{
				AIndex	spos = 0, epos = 0;
				SPI_GetSelect(spos,epos);
				for( AIndex i = 0; i < inReplaceLengthInUTF16; i++ )
				{
					spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos,false);
				}
				DeleteTextFromDocument(spos,epos,false,false);
			}
			//
			GetCaretTextPoint(insertionPoint);
		}
		else
		{
			ATextPoint	ept;
			SPI_GetSelect(insertionPoint,ept);
			DeleteTextFromDocument(insertionPoint,ept,false,false);
			GetCaretTextPoint(insertionPoint);//B0349 削除の結果、キャレットのTextPointは変わることがある。（行折り返しありの場合）
		}
		insertionTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(insertionPoint);
		inlineInputFirstPos = GetTextDocument().SPI_GetTextIndexFromTextPoint(insertionPoint);
		//
		mInlineInputNotFixedTextPos = insertionTextIndex+fixLen;
		mInlineInputNotFixedTextLen = insertText.GetItemCount() - fixLen;
	}
	
	if( mInlineInputNotFixedTextLen > 0 )//B0351 AquaSKK対策　全確定ならハイライト下線情報は全削除状態にする
	{
		//InlineInputハイライト下線の情報を保存
		mHiliteLineStyleIndex.SetFromObject(hiliteLineStyleIndex);
		mHiliteStartPos.SetFromObject(hiliteStartPos);
		mHiliteEndPos.SetFromObject(hiliteEndPos);
		//mHiliteStartPos, mHiliteEndPosはテキスト全体のインデックスにしておく
		for( AIndex i = 0; i < mHiliteLineStyleIndex.GetItemCount(); i++ )
		{
			mHiliteStartPos.Set(i,mHiliteStartPos.Get(i)+inlineInputFirstPos);
			mHiliteEndPos.Set(i,mHiliteEndPos.Get(i)+inlineInputFirstPos);
		}
	}
	
	/*#1298
	//B0332
	if( insertText.GetItemCount() >= 2 )
	{
		AUChar	ch1 = insertText.Get(0);
		AUChar	ch2 = insertText.Get(1);
		if( ch1 == 0xC2 && ch2 == 0xA5 )
		{
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true )
			{
				insertText.Delete(0,2);
				insertText.Insert1(0,kUChar_Backslash);
				//B0380 Fix部分が0の場合は、バイト数を減らした部分はNotFixedの部分のはずなので、mInlineInputNotFixedTextLenを1byte分減らす
				if( fixLen == 0 )
				{
					mInlineInputNotFixedTextLen--;
				}
			}
		}
	}
	*/
	
	//テキスト挿入位置にヒントテキストがあれば削除する
	RemoveHintText(insertionTextIndex);
	//テキスト挿入
	//InsertTextToDocument(insertionTextIndex,insertText);
	GetTextDocument().SPI_InsertText(insertionTextIndex,insertText);
	
	//キャレット設定
	ATextPoint	caretPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertionTextIndex+insertText.GetItemCount(),caretPoint);
	
	//Hilite情報にキャレット情報が含まれている場合はそれに従う
	if( mInlineInputNotFixedTextLen > 0 )//B0351 AquaSKK対策　全確定ならキャレットは入力文字列最後のままにしておく
	{
		for( AIndex i = 0; i < hiliteLineStyleIndex.GetItemCount(); i++ )
		{
			if( hiliteLineStyleIndex.Get(i) == kIndex_Invalid )
			{
				GetTextDocument().SPI_GetTextPointFromTextIndex(inlineInputFirstPos+hiliteStartPos.Get(i),caretPoint);
			}
		}
	}
	
	//
	SetCaretTextPoint(caretPoint);
	AdjustScroll(GetCaretTextPoint());
	
	//B0000 キー入力に対する反応速度を上げるために、ここで一旦、実際の画面への画面更新を行う
	//#688 NVMC_ExecuteDoDrawImmediately();
	//#688 NVMC_RedrawImmediately();
	
	//
	ATextPoint	insertionTextPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertionTextIndex,insertionTextPoint);
	ATextPoint	oldNotFixedStartTextPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos-fixLen,oldNotFixedStartTextPoint);
	if( oldNotFixedStartTextPoint.y < insertionTextPoint.y )
	{
		//
		AImageRect	imageLineRect;
		GetLineImageRect(oldNotFixedStartTextPoint.y,insertionTextPoint.y,imageLineRect);
		ALocalRect	localLineRect;
		NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
		//描画範囲はviewの右端までにする
		ALocalRect	localFrameRect = {0};
		NVM_GetLocalViewRect(localFrameRect);
		if( localLineRect.right < localFrameRect.right )
		{
			localLineRect.right = localFrameRect.right;
		}
		//
		NVMC_RefreshRect(localLineRect);
	}
	
	//#390 キーマクロ記録
	if( fixLen > 0 && GetApp().SPI_IsKeyRecording() == true )
	{
		AText	text;
		GetTextDocumentConst().SPI_GetText(mInlineInputNotFixedTextPos-fixLen,mInlineInputNotFixedTextPos,text);
		GetApp().SPI_RecordKeyText(text);
	}
	
	//win
	//win ACursorWrapper::ObscureCursor();
	NVMC_ObscureCursor();
	
	//win Caretちらつき対策（途中で描画せずに、ここでまとめて表示）
	//Caret表示予約があれば表示する
	//#1093 ShowCaretIfReserved();
	
	return true;
}

/**
インライン入力　テキスト範囲→ローカル座標
#1305 
*/
ABool	AView_Text::EVTDO_DoInlineInputOffsetToPos( const AIndex inStartOffset, const AIndex inEndOffset, ALocalRect& outRect )
{
	//テキスト範囲取得
	ATextPoint	startTextPoint = {0}, endTextPoint = {0};
	if( mInlineInputNotFixedTextLen > 0 )
	{
		GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos+inStartOffset,startTextPoint,true);
		GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos+inEndOffset,endTextPoint,true);
	}
	else
	{
		GetCaretTextPoint(startTextPoint);
		GetCaretTextPoint(endTextPoint);
	}
	//イメージ座標取得
	AImagePoint	startImagePoint = {0}, endImagePoint = {0};
	GetImagePointFromTextPoint(startTextPoint,startImagePoint);
	GetImagePointFromTextPoint(endTextPoint,endImagePoint);
	//複数行にわたる場合は出力Rectの右端は、最初の行の右端にする
	if( endTextPoint.y > startTextPoint.y )
	{
		ATextPoint	startTextPoint2 = startTextPoint;
		startTextPoint2.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(startTextPoint.y);
		AImagePoint	startImagePoint2 = {0};
		GetImagePointFromTextPoint(startTextPoint2,startImagePoint2);
		endImagePoint.x = startImagePoint2.x;
	}
	//ローカル座標取得
	AImageRect	imageRect = {0};
	imageRect.left		= startImagePoint.x;
	imageRect.top		= startImagePoint.y;
	imageRect.right		= endImagePoint.x;
	imageRect.bottom	= endImagePoint.y + GetLineHeightWithoutMargin(endTextPoint.y);
	NVM_GetLocalRectFromImageRect(imageRect,outRect);
	return true;
}

ABool	AView_Text::EVTDO_DoInlineInputPosToOffset( const ALocalPoint& inPos, AIndex& outOffset )
{
	AImagePoint	imagePoint;
	NVM_GetImagePointFromLocalPoint(inPos,imagePoint);
	ATextPoint	textPoint;
	GetTextPointFromImagePoint(imagePoint,textPoint);
	ATextIndex	textIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(textPoint);
	if( mInlineInputNotFixedTextLen > 0 )
	{
		outOffset = textIndex - mInlineInputNotFixedTextPos;
	}
	else
	{
		outOffset = 0;
	}
	return true;
}

ABool	AView_Text::EVTDO_DoInlineInputGetSelectedText( AText& outText )
{
	GetSelectedText(outText);
	outText.LimitLength(0,kLimit_InlineInputGetSelectedText_MaxByteCount);//#695 10000 byteを限度にする
	return true;
}

void	AView_Text::SPI_GetSelectedText( AText& outText ) const
{
	GetSelectedText(outText);
}

void	AView_Text::GetSelectedText( AText& outText ) const
{
	outText.DeleteAll();
	if( IsCaretMode() == false )
	{
		if( mKukeiSelected == false )
		{
			ATextPoint	caretPoint, selectPoint;
			GetCaretTextPoint(caretPoint);
			GetSelectTextPoint(selectPoint);
			GetTextDocumentConst().SPI_GetText(caretPoint,selectPoint,outText);
		}
		else
		{
			for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
			{
				ATextPoint	spt, ept;
				spt = mKukeiSelected_Start.Get(i);
				ept = mKukeiSelected_End.Get(i);
				AText	text;
				GetTextDocumentConst().SPI_GetText(spt,ept,text);
				outText.AddText(text);
				outText.Add(kUChar_LineEnd);
			}
		}
	}
	//B0330 TEST
	/*else
	{
		ATextIndex	spos, epos;
		GetSelect(spos,epos);
		AIndex pos = spos-1;
		for( ; pos > 0; pos-- )
		{
			AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(pos);
			if( (ch>='A'&&ch<='Z') || (ch>='a'&&ch<='z') || ch == '-' )
			{
				continue;
			}
			else
			{
				pos++;
				break;
			}
		}
		if( pos < spos )
		{
			GetTextDocumentConst().SPI_GetText(pos,spos,outText);
		}
	}*/
}

ABool	AView_Text::EVTDO_DoKeyDown( /*#688 const AUChar inChar*/ const AText& inText, const AModifierKeys inModifierKeys )//#1080
{
	/*#688 
	AText	text;
	text.Add(inChar);
	*/
	if( KeyTool(inText) == true )   return true;
	//#1416
	//Control+Qは、emacsでいうところのquoted-insert動作に割り当てられていると思われる。
	//（System/Library/Frameworks/AppKit.framework/Resources/StandardKeyBinding.dictには記述がないが、macOSのデフォルト動作として組み込まれているものと思われる。）
	//quoted-insertは次に入力した文字をそのまま（Control+Aなら0x01のまま）入力するという動作だが、miでは、制御コードの入力はAView_Text::TextInput()で抑制している(関連：#1080)ので、quoted-insertは動作しない。
	//そのため、#1416の対策として、Control+Qのみ、ここで捕まえて処理し、CocoaUserPaneView.mm/keydownで、interpretKeyEventsを動作させないようにする。
	//他のキーをここで捕まえると、OSのキーバインド内容を取得できないので、「OSのキーバインドからの差分を設定する」設定をONにした場合の動作ができない。そのため、他のキーは従来通り、interpretKeyEventsを実行し、それぞれdeleteBackward等経由でEVTDO_DoTextInput()をコールするようにする。
	//Control+半角数字、スラッシュ、BSキーもここで処理することにする(#1252)
	if( inText.GetItemCount() > 0 )
	{
		//入力文字(UCS4)取得
		AUCS4Char ch = 0;
		inText.Convert1CharToUCS4(0, ch);
		if( inModifierKeys == kModifierKeysMask_Control )
		{
			//キー取得
			AKeyBindKey	key = kKeyBindKey_Invalid;
			switch(ch)
			{
			  case 0x11://Control+Q
				{
					key = kKeyBindKey_Q;
					break;
				}
			  case '0':
				{
					key = kKeyBindKey_0;
					break;
				}
			  case '1':
				{
					key = kKeyBindKey_1;
					break;
				}
			  case '2':
				{
					key = kKeyBindKey_2;
					break;
				}
			  case '3':
				{
					key = kKeyBindKey_3;
					break;
				}
			  case '4':
				{
					key = kKeyBindKey_4;
					break;
				}
			  case '5':
				{
					key = kKeyBindKey_5;
					break;
				}
			  case '6':
				{
					key = kKeyBindKey_6;
					break;
				}
			  case '7':
				{
					key = kKeyBindKey_7;
					break;
				}
			  case '8':
				{
					key = kKeyBindKey_8;
					break;
				}
			  case '9':
				{
					key = kKeyBindKey_9;
					break;
				}
			  case '/':
				{
					key = kKeyBindKey_Slash;
					break;
				}
			  case 0x7F:
				{
					key = kKeyBindKey_BS;
					break;
				}
			}
			if( key != kKeyBindKey_Invalid )
			{
				//キー入力処理（キーバインド）
				ABool	updateMenu = false;
				ABool	result = EVTDO_DoTextInput(inText,key,kModifierKeysMask_Control,keyAction_NOP,updateMenu);
				if( updateMenu == true )
				{
					AApplication::GetApplication().NVI_UpdateMenu();
				}
				return result;
			}
		}
	}
	//
	return false;
}

//
ABool	AView_Text::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	//#688
	//メニュー更新フラグ初期化
	outUpdateMenu = true;
	
	//#867
	//Idle workタイマークリア
	ResetKeyIdleWorkTimer(kKeyIdleWorkTimerValue);
	
	//#688
	//tick timerよりも前に文字入力された場合に、ここでreserveされたcaret描画を実行する。
	//（ここで描画しないとオートリピートしたとき等、キャレット描画が遅れて見える）
	ShowCaretIfReserved();
	
	//
	ABool	result = true;//win 処理したかどうかを返す
	
	RestoreFromBraceCheck();
	if( KeyTool(inText) == true )   return true;//win WinはEVTDO_DoKeyDown()を通らないので。
	AKeyBindKey	keyBindKey = inKeyBindKey;//#688 AKeyWrapper::GetKeyBindKey(inOSKeyEvent);
	AModifierKeys	modifiers = inModifierKeys;//#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent);
	
	//キーバインド未確定の場合、かつ、1文字のみで、0x01-0x1Fのときは、それぞれに応じたキーバインドキーを設定する
	//control+shift+[等をここで対応するため。 #1017
	//対症療法的ではあるが、どうせ何もしなければ、制御コード入力になってしまう文字なので、対応しないよりはしたほうが
	//問題のあるキーバインドが減る。
	if( inText.GetItemCount() == 1 && inKeyBindAction == keyAction_NOP && keyBindKey == kKeyBindKey_Invalid )
	{
		AUChar	ch = inText.Get(0);
		if( ch >= 0x01 && ch < 0x20 && ch != 0x09 && ch != 0x0D && inModifierKeys != 0 )
		{
			//0x01:A
			//0x1A:Z
			//0x1B:@
			//0x1C:]
			//0x1D:[
			//0x1E:不明
			//0x1F:-
			switch(ch)
			{
			  default:
				{
					keyBindKey = (AKeyBindKey)(kKeyBindKey_A+ch-0x01);
					break;
				}
			  case 0x1B:
				{
					keyBindKey = kKeyBindKey_Atmark;
					break;
				}
			  case 0x1C:
				{
					keyBindKey = kKeyBindKey_BracketEnd;
					break;
				}
			  case 0x1D:
				{
					keyBindKey = kKeyBindKey_BracketStart;
					break;
				}
			  case 0x1E:
				{
					//処理なし
					break;
				}
			  case 0x1F:
				{
					keyBindKey = kKeyBindKey_Minus;
					break;
				}
			}
		}
	}
	
	//#204 補完入力ウインドウ表示中のキー動作
	//#717 if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kCandidateWindowKeyBind) == true )
	{
		//#717 if( GetApp().SPI_GetCandidateListWindow().NVI_IsWindowVisible() == true )
		if( mAbbrevInputMode == true )//#717
		{
			switch(keyBindKey)
			{
			  case kKeyBindKey_Escape:
				{
					//現在のUndoアクションが補完入力、かつ、そのアクションタグの後にテキスト挿入削除が存在した場合、Undoを実行
					//（アクションタグの後にテキスト挿入削除が存在しない場合にUndo実行すると、その前のアクションをUndoしてしまうため、dangling判断必要）
					if( GetTextDocument().SPI_GetCurrentUndoActionTag() == undoTag_AbbrevInput &&
								GetTextDocumentConst().SPI_IsCurrentUndoActionDangling() == false )//#314
					{
						UndoRedo(true);
					}
					//補完入力状態解除 #717
					ClearAbbrevInputMode(true,false);
					//win ACursorWrapper::ObscureCursor();
					NVMC_ObscureCursor();
					return true;
				}
			  case kKeyBindKey_Up:
				{
					InputAbbrev(false);
					//win ACursorWrapper::ObscureCursor();
					NVMC_ObscureCursor();
					return true;
				}
			  case kKeyBindKey_Down:
				{
					InputAbbrev(true);
					//win ACursorWrapper::ObscureCursor();
					NVMC_ObscureCursor();
					return true;
				}
			  case kKeyBindKey_Return:
				{
					//補完入力状態解除 #717
					ClearAbbrevInputMode(true,true);
					//win ACursorWrapper::ObscureCursor();
					NVMC_ObscureCursor();
					return true;
				}
			  default:
				{
					//処理なし
					break;
				}
			}
		}
	}
	//補完ポップアップ後に常に上下キーで補完選択する場合の処理
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAbbrevInputByArrowKeyAfterPopup) == true )
	{
		if( mTextWindowID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsPopupCandidateWindowVisible() == true )
			{
				switch(keyBindKey)
				{
				  case kKeyBindKey_Up:
					{
						InputAbbrev(false);
						NVMC_ObscureCursor();
						return true;
					}
				  case kKeyBindKey_Down:
					{
						InputAbbrev(true);
						NVMC_ObscureCursor();
						return true;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
			}
		}
	}
	
	/*#1160
	//==================候補キー設定による候補入力==================
	//候補キー設定による候補入力（次の候補）
	{
		//候補キー設定に対応するキーが押されているかどうかをチェック
		ABool	abbrevKey = false;
		switch( GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAbbrevNextKey) )
		{
		  case kAbbrevKeyType_ShiftSpace:
			{
				if( keyBindKey == kKeyBindKey_Space && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_ShiftTab:
			{
				if( keyBindKey == kKeyBindKey_Tab && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_Tab:
			{
				if( keyBindKey == kKeyBindKey_Tab && modifiers == kModifierKeysMask_None )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_Escape:
			{
				if( keyBindKey == kKeyBindKey_Escape && modifiers == kModifierKeysMask_None )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_ShiftEscape:
			{
				if( keyBindKey == kKeyBindKey_Escape && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		//候補キーが押されていたら、候補処理
		if( abbrevKey == true )
		{
			//候補があった場合、リターンする
			if( InputAbbrev(true) == true )
			{
				NVMC_ObscureCursor();
				return true;
			}
		}
	}
	
	//候補キー設定による候補入力（前の候補）
	{
		//候補キー設定に対応するキーが押されているかどうかをチェック
		ABool	abbrevKey = false;
		switch( GetApp().NVI_GetAppPrefDB().GetData_Number(AAppPrefDB::kAbbrevPrevKey) )
		{
		  case kAbbrevKeyType_ShiftSpace:
			{
				if( keyBindKey == kKeyBindKey_Space && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_ShiftTab:
			{
				if( keyBindKey == kKeyBindKey_Tab && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_Tab:
			{
				if( keyBindKey == kKeyBindKey_Tab && modifiers == kModifierKeysMask_None )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_Escape:
			{
				if( keyBindKey == kKeyBindKey_Escape && modifiers == kModifierKeysMask_None )
				{
					abbrevKey = true;
				}
				break;
			}
		  case kAbbrevKeyType_ShiftEscape:
			{
				if( keyBindKey == kKeyBindKey_Escape && modifiers == kModifierKeysMask_Shift )
				{
					abbrevKey = true;
				}
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		//候補キーが押されていたら、候補処理
		if( abbrevKey == true )
		{
			//候補があった場合、リターンする
			if( InputAbbrev(false) == true )
			{
				NVMC_ObscureCursor();
				return true;
			}
		}
	}
	*/
	
	AKeyBindAction	action;
	AText	actiontext;
	if( keyBindKey != kKeyBindKey_Invalid )
	{
		if( GetApp().GetAppPref().GetData_DefaultKeyBindAction(keyBindKey,modifiers,action,actiontext) == false )
		{
			keyBindKey = kKeyBindKey_Invalid;
		}
		//RC2
		if( action != keyAction_abbrevprev && action != keyAction_abbrevnext )
		{
			//補完入力状態解除 #717
			ClearAbbrevInputMode(false,false);//RC2
		}
	}
	if( keyBindKey == kKeyBindKey_Invalid )
	{
		if( inText.GetItemCount() == 0 )//win
		{
			result = false;
		}
		else
		{
			//補完入力状態解除 #717
			ClearAbbrevInputMode(false,false);//RC2
			if( AKeyWrapper::IsCommandKeyPressed(modifiers) != true )//該当メニューコマンドが使用不可能な場合のコマンドショートカットキーの場合、文字入力として送られてくる
			{
				AText	yen;
				yen.SetLocalizedText("Text_Yen");
				if( yen.Compare(inText) == true )
				{
					//
					AText	backslash;
					backslash.Add(kUChar_Backslash);
					if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kInputBackslashByYenKey) == true )
					{
						TextInput(undoTag_Typing,backslash);
					}
					else
					{
						TextInput(undoTag_Typing,inText);
					}
				}
				else
				{
                    //==================通常テキスト入力（キーバインド不可キー）==================
					if( GetTextDocument().NVI_IsDirty() == true && mCaretMode == true )   outUpdateMenu = false;
					TextInput(undoTag_Typing,inText);
					
					//#390 キーマクロ記録
					GetApp().SPI_RecordKeyText(inText);
				}
			}
			result = true;
		}
	}
	else
	{
        //==================キーバインド処理実行==================
		result = KeyBindAction(keyBindKey,modifiers,inKeyBindAction,inText,outUpdateMenu);//win
	}
	
	//win ACursorWrapper::ObscureCursor();
	NVMC_ObscureCursor();
	
	//win Caretちらつき対策（途中で描画せずに、ここでまとめて表示）
	//Caret表示予約があれば表示する
	//#688 ShowCaretIfReserved();
	
	return result;
}

void	AView_Text::DeleteSelect( const AUndoTag inUndoActionTag )
{
	if( IsCaretMode() == true )   return;
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(inUndoActionTag);
	//選択範囲削除実行
	DeleteTextFromDocument();
}

void	AView_Text::TextInput( const AUndoTag inUndoActionTag, const AText& inText )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//0x20未満の文字は入力しない #1080
	if( inText.GetItemCount() == 1 )
	{
		AUChar	ch = inText.Get(0);
		if( ch < 0x20 && ch != 0x09 && ch != 0x0D )
		{
			return;
		}
	}
	
	//Undoアクションタグ記録
	if( inUndoActionTag != undoTag_NOP )//B0381
	{
		GetTextDocument().SPI_RecordUndoActionTag(inUndoActionTag);
	}
	
	//#478
	//テーブル編集モード
	ABool	tableMode = false;
	AArray<AIndex>	surrondingTableRowArray_LineIndex;
	AArray<ABool>	surrondingTableRowArray_IsRuledLine;
	AArray<AIndex>	surrondingTableRowArray_ColumnEndX;
	if( false )
	{
		//囲んでいるテーブルの情報を取得
		tableMode = GetTextDocumentConst().SPI_GetSurroundingTable(mCaretTextPoint,
					surrondingTableRowArray_LineIndex,surrondingTableRowArray_IsRuledLine,surrondingTableRowArray_ColumnEndX);
	}
	
	//選択予約解除#699
	CancelReservedSelection();
	
	//選択範囲削除、テキスト挿入
	DeleteAndInsertTextToDocument(inText);
	
	//#478
	//テーブル編集モード
	if( tableMode == true )
	{
		//テーブルに囲まれている場合
		
		//テーブル各行毎の処理
		for( AIndex index = 0; index < surrondingTableRowArray_LineIndex.GetItemCount(); index++ )
		{
			//行index取得
			AIndex	lineIndex = surrondingTableRowArray_LineIndex.Get(index);
			//キャレット行は何もしない
			if( lineIndex == mCaretTextPoint.y )
			{
				continue;
			}
			//罫線行の処理
			if( surrondingTableRowArray_IsRuledLine.Get(index) == true )
			{
				//-を挿入
				AIndex	x = surrondingTableRowArray_ColumnEndX.Get(index);
				ATextPoint	tpt = {x,lineIndex};
				AText	t("-");
				GetTextDocument().SPI_InsertText(tpt,t);
			}
			//罫線でない行の処理
			else
			{
				//スペースを挿入
				AIndex	x = surrondingTableRowArray_ColumnEndX.Get(index);
				ATextPoint	tpt = {x,lineIndex};
				AText	t(" ");
				GetTextDocument().SPI_InsertText(tpt,t);
			}
		}
	}
	
	//#717
	//リアルタイム補完候補要求
	if( inUndoActionTag == undoTag_Typing )
	{
		//FindCandidateRealtime();
		mCandidateRequestTimer = 9;
	}
	
	//インデント実行文字による自動インデント実行
	//（インデントタイプが文法定義インデント、正規表現インデントの場合のみ）
	if( inText.GetItemCount() == 1 )
	{
		if( (GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
				GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent ) && 
				GetApp().SPI_GetAutoIndentMode() == true )//#638
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			AText	text;
			GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Text(AModePrefDB::kIndentExeLetters,text);
			if( text.IsCharContained(inText.Get(0)) == true )
			{
				DoIndent(undoTag_NOP,inputSpaces,kIndentTriggerType_AutoIndentCharacter);//#249 #639 #650 正規表現一致の場合のみインデント適用
			}
		}
	}
	
	//
	if( inText.GetItemCount() == 1 && 
				GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kCheckBrace) == true )
	{
		ATextPoint	pt;
		ABool	checkOK;
		if( GetTextDocument().SPI_CheckBrace(GetCaretTextPoint(),pt,checkOK) == true )
		{
			if( checkOK == true )
			{
				NVI_GetOriginOfLocalFrame(mBraceCheckSavedOrigin);
				mBraceCheckSavedCaretTextPoint = GetCaretTextPoint();
				ATextPoint	ept = pt;
				ept.x++;
				SetSelect(pt,ept,(GetApp().NVI_IsDarkMode()==true));//#1316 ダークモードの場合は反転ハイライトを使う
				//#396 AdjustScroll_Center(pt);
				AdjustScroll(pt);//#396
				mBraceCheckStartTickCount = kBraceCheckTimerCount;//win 080713 20;
				mDisplayingBraceCheck = true;
			}
			else
			{
				ASoundWrapper::Beep();
			}
		}
	}
	//RC2
	if( inText.GetItemCount() == 1 )
	{
		//#853 if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() )
		{
			AUChar	ch = inText.Get(0);
			//キーワード表示文字の後なら、キーワード情報表示
			if( GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetSyntaxDefinition().IsIdInfoAutoDisplayChar(ch) == true )
			{
				IdInfoAuto();
			}
			//引数デリミタの後なら、引数ハイライト
			if( GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetSyntaxDefinition().IsIdInfoDelimiterChar(ch) == true )
			{
				IdInfoAutoArg();
			}
		}
	}
	//R0214
	/*ヘッダ内容がすぐに変わってしまうので返って使いづらい
	ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	if( pos > 0 )
	{
		if( inText.GetItemCount() == 1 && GetTextDocumentConst().SPI_IsWordAlphabet(pos-1) )
		{
			pos--;
			AText	word;
			ATextIndex	spos = pos, epos = pos;
			GetTextDocument().SPI_FindWord(pos,spos,epos);
			GetTextDocument().SPI_GetText(spos,epos,word);
			SetInfoHeader(word);
		}
	}*/
}

void	AView_Text::KukeiInput( const AText& inText )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//選択範囲削除
	DeleteTextFromDocument();
	//
	mKukeiSelected_Start.DeleteAll();
	mKukeiSelected_End.DeleteAll();
	//
	AImagePoint	imagePoint;
	GetImagePointFromTextPoint(GetCaretTextPoint(),imagePoint);
	HideCaret(true,true);
	mKukeiSelected = true;
	mCaretMode = false;
	mSelectTextPoint = GetCaretTextPoint();//念のため
	AIndex	lineIndex = GetCaretTextPoint().y;
	for( AIndex pos = 0; pos < inText.GetItemCount();  )
	{
		AText	lineText;
		inText.GetLine(pos,lineText);
		AImagePoint	ipt;
		ipt.x = imagePoint.x;
		ipt.y = GetImageYByLineIndex(lineIndex);
		ATextPoint	textpoint;
		GetTextPointFromImagePoint(ipt,textpoint);
		GetTextDocument().SPI_InsertText(textpoint,lineText);
		//
		mKukeiSelected_Start.Add(textpoint);
		ATextPoint	end = textpoint;
		end.x += lineText.GetItemCount();
		mKukeiSelected_End.Add(end);
		//
		lineIndex++;
		if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount() )
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			GetTextDocument().SPI_InsertText(GetTextDocumentConst().SPI_GetTextLength(),text);
		}
	}
	NVI_Refresh();
}

//インデント実行
void	AView_Text::DoIndent( /*#650 const ABool inUseCurrentParagraphIndentForRegExp, */const AUndoTag inUndoTag, 
		const ABool inInputSpaces, const AIndentTriggerType inIndentTriggerType )//B0363 B0381 #249 #639 #650
{
	ABool	svCaretMode = mCaretMode;
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	ATextPoint	pt = spt;
	pt.x = 0;
	SetCaretTextPoint(pt);
	DoIndent(/*#650 inUseCurrentParagraphIndentForRegExp,*/inUndoTag,spt,ept,inInputSpaces,inIndentTriggerType);//#249 #639 #650
	if( spt.y >= GetTextDocumentConst().SPI_GetLineCount() )   spt.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	if( ept.y >= GetTextDocumentConst().SPI_GetLineCount() )   ept.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	//元々がキャレット状態の場合、インデント後に選択状態にならないように、補正する
	//（DoIndent()の選択終了位置補正処理が不完全なためだが、今回はひとまずこの対策にする）
	if( svCaretMode == true )
	{
		//spt, eptのうち、前の方をキャレット位置に設定する
		if( GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt) < GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept) )
		{
			ept = spt;
		}
		else
		{
			spt = ept;
		}
	}
	//
	SetSelect(spt,ept);
}
//B0381
void	AView_Text::DoIndent( /*#650 const ABool inUseCurrentParagraphIndentForRegExp, */const AUndoTag inUndoTag, 
		ATextPoint& ioStartTextPoint, ATextPoint& ioEndTextPoint, const ABool inInputSpaces,
		const AIndentTriggerType inIndentTriggerType )//#249 #639 #650
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//Undoアクションタグ記録
	if( inUndoTag != undoTag_NOP )//B0381
	{
		GetTextDocument().SPI_RecordUndoActionTag(inUndoTag);
	}
	
	//#639
	ABool	indent0ForEmptyLine = false;
	if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndent0ForEmptyLine) == true )
	{
		switch(inIndentTriggerType)
		{
		  case kIndentTriggerType_IndentFeature:
		  case kIndentTriggerType_ReturnKey_CurrentLine:
			{
				indent0ForEmptyLine = true;
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	
	/*R0208 GetCurrentParagraphStartLineIndex()が重い
	AIndex	startParagraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ioStartTextPoint.y);
	AIndex	startParagraphStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(startParagraphIndex);
	AIndex	endParagraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ioEndTextPoint.y)+1;
	AIndex	endParagraphStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(endParagraphIndex);*/
	AIndex	startParagraphStartLineIndex = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(ioStartTextPoint.y);
	//B0000 行全体を選択してインデントし、インデントが増えた場合、次の行までインデント対象となる問題を修正
	AIndex	endlineindex = ioEndTextPoint.y;
	if( ioEndTextPoint.x == 0 && ioEndTextPoint.y > 0 && ioEndTextPoint.y > ioStartTextPoint.y )   endlineindex--;
	AIndex	endParagraphStartLineIndex = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(endlineindex);
	
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_Indent,true,false,true);
	try
	{
		//#649 spt固定条件（選択であり、x==0から始まる）
		ABool	fixStartPoint = ((ComparePoint(ioStartTextPoint,ioEndTextPoint) == false) && (ioStartTextPoint.x == 0));
		//
		AIndex	prevIndentCount = kIndex_Invalid;
		for( AIndex lineIndex = startParagraphStartLineIndex; lineIndex < endParagraphStartLineIndex; lineIndex++ )
		{
			//モーダルセッション継続判定
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Indent,0,true,
																	  lineIndex-startParagraphStartLineIndex,
																	  endParagraphStartLineIndex-startParagraphStartLineIndex) == false )
			{
				break;
			}
			if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount() )   break;//★
			//段落開始行以外ではインデント処理を行わない
			if( GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(lineIndex) != lineIndex )   continue;
			//
			AItemCount	nextIndentCount;
			AItemCount	indentCount = GetTextDocument().SPI_CalcIndentCount(lineIndex,prevIndentCount,nextIndentCount,
																			//#650 inUseCurrentParagraphIndentForRegExp);//B0363
																			inIndentTriggerType);//#650
			prevIndentCount = nextIndentCount;
			if( prevIndentCount < 0 )   prevIndentCount = 0;
			//#639 空行ならインデント0にする
			if( indent0ForEmptyLine == true )
			{
				//現在のパラグラフのテキスト取得
				AText	paraText;
				GetTextDocumentConst().SPI_GetParagraphTextByLineIndex(lineIndex,paraText);
				//空白のみかどうかを判定
				ABool	onlySpace = true;
				AItemCount	len = paraText.GetItemCount();
				for( AIndex p = 0; p < len; p++ )
				{
					AUChar	ch = paraText.Get(p);
					if( ch != kUChar_Space && ch != kUChar_Tab && ch != kUChar_LineEnd )
					{
						onlySpace = false;
						break;
					}
				}
				//空白のみならインデントを0に設定
				if( onlySpace == true )
				{
					indentCount = 0;
				}
			}
			//選択範囲ずらし用データ記憶（インデント実行前データ記憶） #996
			AIndex	currentParaIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(lineIndex);
			AIndex	startParaIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ioStartTextPoint.y);
			AIndex	startParaStartTextIndex = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(startParaIndex);
			AIndex	startOffset = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioStartTextPoint) - startParaStartTextIndex;
			AIndex	endParaIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ioEndTextPoint.y);
			AIndex	endParaStartTextIndex = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(endParaIndex);
			AIndex	endOffset = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioEndTextPoint) - endParaStartTextIndex;
			//
			ATextIndex	svEndTextIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioEndTextPoint);
			//AItemCount	svLineCount = GetTextDocumentConst().SPI_GetLineCount();
			//インデント実行
			AItemCount	insertedCount = GetTextDocument().SPI_Indent(lineIndex,indentCount,inInputSpaces,true);//#249
			//AItemCount	insertedLineCount = GetTextDocumentConst().SPI_GetLineCount() - svLineCount;
			//fprintf(stderr,"%d ",lineIndex);
			/*#649 この判定だとx==0から複数行選択時、eptの位置調整がされない問題がある。
		if( ioStartTextPoint.x == 0 && ioEndTextPoint.y > ioStartTextPoint.y )//B0416 複数行を選択中に「右シフト」を行った場合、選択範囲中最初の行の行頭タブ文字が、選択範囲から外れてしまう
		{
			//処理なし
		}
		else
		{
		*/
			//現在処理中の段落が、選択範囲の開始段落であれば、選択範囲の開始をずらす　#996 行単位の処理から段落単位の処理へ変更
			//選択開始位置をずらす
			if( startParaIndex == currentParaIndex && fixStartPoint == false )//#649
			{
				startOffset += insertedCount;
				if( startOffset < 0 )
				{
					startOffset = 0;
				}
				if( startOffset > GetTextDocument().SPI_GetParagraphLengthWithoutLineEnd(startParaIndex) )
				{
					startOffset = GetTextDocument().SPI_GetParagraphLengthWithoutLineEnd(startParaIndex);
				}
				GetTextDocument().SPI_GetTextPointFromTextIndex(startParaStartTextIndex+startOffset,ioStartTextPoint);
			}
			//選択終了位置をずらす
			if( endParaIndex == currentParaIndex )
			{
				endOffset += insertedCount;
				if( endOffset < 0 )
				{
					endOffset = 0;
				}
				if( endOffset > GetTextDocument().SPI_GetParagraphLengthWithoutLineEnd(endParaIndex) )
				{
					endOffset = GetTextDocument().SPI_GetParagraphLengthWithoutLineEnd(endParaIndex);
				}
				GetTextDocument().SPI_GetTextPointFromTextIndex(endParaStartTextIndex+endOffset,ioEndTextPoint);
			}
			//#649}
			//選択終了位置が今回のインデント行より後の場合、選択終了位置を補正する
			if( endParaIndex > currentParaIndex )
			{
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(svEndTextIndex + insertedCount,ioEndTextPoint);
				//
				AIndex	endlineindex = ioEndTextPoint.y;
				if( ioEndTextPoint.x == 0 && ioEndTextPoint.y > 0 && ioEndTextPoint.y > ioStartTextPoint.y )   endlineindex--;
				endParagraphStartLineIndex = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(endlineindex);
			}
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	//テキスト情報ウインドウ等を更新（高速化のため、上の処理でSPI_Indent()の引数にサブウインドウ描画しないように設定しているので、ここでまとめて描画）
	GetTextDocument().SPI_DeferredRefreshSubWindowsAfterInsertDeleteText();
}

//シフト
void	AView_Text::ShiftRightLeft( const ABool inRight, const ABool inInputSpaces, const ABool inIsTabKey )//#249 #1188
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Shift);
	
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	//#649 spt固定条件（選択であり、x==0から始まる）
	ABool	fixStartPoint = ((ComparePoint(spt,ept) == false) && (spt.x == 0));
	//
	ATextPoint	pt = spt;
	pt.x = 0;
	SetCaretTextPoint(pt);
	AIndex	startParagraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
	AIndex	startParagraphStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(startParagraphIndex);
	AIndex	endParagraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y);//B0334+1;
	if( (spt.x == ept.x && spt.y == ept.y) || ept.x > 0 )
	{
		endParagraphIndex++;
	}
	AIndex	endParagraphStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(endParagraphIndex);
	for( AIndex lineIndex = startParagraphStartLineIndex; lineIndex < endParagraphStartLineIndex; lineIndex++ )
	{
		if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount() )   break;//★
		if( GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(lineIndex) != lineIndex )   continue;
		AItemCount	indentCount = GetTextDocumentConst().SPI_GetCurrentIndentCount(lineIndex);
		if( inRight == true )
		{
			//#1188 「空行の場合インデント量を０にする」をONにしている場合、空行なら右シフトしない
			if( GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(lineIndex) == 0 &&
				GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndent0ForEmptyLine) == true &&
				inIsTabKey == false )
			{
				continue;
			}
			//#1407 indentCount += GetTextDocumentConst().SPI_GetIndentWidth();
			//インデントを＋１ #1407 インデント位置に吸着するようにする
			indentCount /= GetTextDocumentConst().SPI_GetIndentWidth();
			indentCount += 1;
			indentCount *= GetTextDocumentConst().SPI_GetIndentWidth();
		}
		else
		{
			//#1407 indentCount -= GetTextDocumentConst().SPI_GetIndentWidth();
			//インデントを－１ #1407 インデント位置に吸着するようにする
            indentCount /= GetTextDocumentConst().SPI_GetIndentWidth();
            indentCount -= 1;
            indentCount *= GetTextDocumentConst().SPI_GetIndentWidth();
		}
		AItemCount	insertedCount = GetTextDocument().SPI_Indent(lineIndex,indentCount,inInputSpaces,true);//#249
		/*#649
		if( spt.x == 0 && ept.y > spt.y )//B0416
		{
			//処理なし
		}
		else
		{
		*/
		if( spt.y == lineIndex && fixStartPoint == false )//#649
		{
			spt.x += insertedCount;
			if( spt.x < 0 )   spt.x = 0;
			if( spt.x > GetTextDocument().SPI_GetLineLengthWithoutLineEnd(spt.y) )   spt.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(spt.y);
		}
		//#649 }//#581
		if( ept.y == lineIndex )
		{
			ept.x += insertedCount;
			if( ept.x < 0 )   ept.x = 0;
			if( ept.x > GetTextDocument().SPI_GetLineLengthWithoutLineEnd(ept.y) )   ept.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(ept.y);
		}
		//#581}
	}
	//テキスト情報ウインドウ等を更新（高速化のため、上の処理でSPI_Indent()の引数にサブウインドウ描画しないように設定しているので、ここでまとめて描画）
	GetTextDocument().SPI_DeferredRefreshSubWindowsAfterInsertDeleteText();
	//
	SetSelect(spt,ept);
}

void	AView_Text::DeleteNextChar()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( IsCaretMode() == false )
	{
		DeleteSelect(undoTag_DELKey);
	}
	else
	{
		AIndex	start = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
		AIndex	end = GetTextDocumentConst().SPI_GetNextCharTextIndex(start);
		if( end > GetTextDocumentConst().SPI_GetTextLength() )   return;
		//Undoアクションタグ記録
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_DELKey);
		//
		DeleteTextFromDocument(start,end,false,false);
	}
	
	//リアルタイム補完候補検索 #717
	//FindCandidateRealtime();
	mCandidateRequestTimer = 9;
	
	//ヒントテキストが存在していたら補完検索要求
	RequestCandidateFinderIfHintExist();
}

void	AView_Text::DeletePreviousChar( const ABool inIncludingDecomp )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( IsCaretMode() == false )
	{
		DeleteSelect(undoTag_DELKey);
	}
	else
	{
		//左シフトにするか、１文字削除にするかの判定
		//左シフト設定ONかつ行頭なら、左シフトにする
		ABool	shiftLeft = false;
		if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kShiftleftByBackspace) == true )
		{
			ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
			if(	GetIndentStart(GetCaretTextPoint()) >= pos && 
			   GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(GetCaretTextPoint().y) == GetCaretTextPoint().y &&
			   GetCaretTextPoint().x > 0 )
			{
				shiftLeft = true;
			}
		}
		//自動インデントOFF時は左シフト無効
		if( GetApp().SPI_GetAutoIndentMode() == false )
		{
			shiftLeft = false;
		}
		//左シフト
		if( shiftLeft == true )
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(false,inputSpaces);
		}
		//前一文字削除
		else
		{
			//
			AIndex	end = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
			AIndex	start = GetTextDocumentConst().SPI_GetPrevCharTextIndex(end,inIncludingDecomp);
			if( start < 0 )   return;
			//Undoアクションタグ記録
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_BSKey);
			//
			DeleteTextFromDocument(start,end,false,false);
		}
	}
	
	//リアルタイム補完候補検索 #717
	//FindCandidateRealtime();
	mCandidateRequestTimer = 9;
}

void	AView_Text::DeleteToLineEnd()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(end.y);
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
	//テキスト削除
	DeleteTextFromDocument(start,end);
}

//R0213
void	AView_Text::DeleteToParagraphEnd()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	end.y = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(end.y)-1;
	end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(end.y);
	//段落の最後の場合は改行コードを削除する
	ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(start);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(end);
	if( spos == epos )
	{
		if( epos >= GetTextDocumentConst().SPI_GetTextLength() )   return;
		epos++;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,start);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,end);
	}
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
	//テキスト削除
	DeleteTextFromDocument(start,end);
}

//#913
/**
行頭までを削除
*/
void	AView_Text::DeleteToLineStart()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	if( start.x == 0 && end.x == 0 && start.y == end.y )
	{
		//#1207 行頭の場合は前の文字を削除
		//Undoアクションタグ記録
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
		//前の文字を削除
		DeletePreviousChar();
	}
	else
	{
		start.x = 0;
		//Undoアクションタグ記録
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
		//テキスト削除
		DeleteTextFromDocument(start,end);
	}
}

//#913
/**
段落頭までを削除
*/
void	AView_Text::DeleteToParagraphStart()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	start.y = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(start.y);
	start.x = 0;
	//段落の最初の場合は直前の改行コードを削除する
	ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(start);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(end);
	if( spos == epos )
	{
		if( spos == 0 )   return;
		spos--;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,start);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,end);
	}
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
	//テキスト削除
	DeleteTextFromDocument(start,end);
}

//#124
void	AView_Text::DeleteCurrentParagraph()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.x != ept.x || spt.y != ept.y )//B0327 条件追加（空行でキャレット状態の場合はept.y--しないようにする）
	{
		if( ept.x == 0 && ept.y > 0 )   ept.y--;
	}
	ATextIndex	spos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y));
	ATextIndex	epos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y)+1);
	if( spos < epos )
	{
		ATextPoint	start, end;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,start);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,end);
		//Undoアクションタグ記録
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
		//テキスト削除
		DeleteTextFromDocument(start,end);
	}
}

void	AView_Text::CutToLineEnd()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(end.y);
	//Scrap保存
	AText	text;
	GetTextDocumentConst().SPI_GetText(start,end,text);
	if( text.GetItemCount() == 0 )   return;//B0000
AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Cut);
	//テキスト削除
	DeleteTextFromDocument(start,end);
}

//R0213
void	AView_Text::CutToParagraphEnd( const ABool inAdditional )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	end.y = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(end.y)-1;
	end.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(end.y);
	//段落の最後の場合は改行コードを削除する
	ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(start);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(end);
	if( spos == epos )
	{
		if( epos >= GetTextDocumentConst().SPI_GetTextLength() )   return;
		epos++;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,start);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,end);
	}
	//Scrap保存
	AText	text;
	GetTextDocumentConst().SPI_GetText(start,end,text);
	if( text.GetItemCount() == 0 )   return;//B0000
	if( inAdditional == true )
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
		text.InsertText(0,scrapText);
	}
AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Cut);
	//テキスト削除
	DeleteTextFromDocument(start,end);
}

void	AView_Text::CutToLineStart()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	start.x = 0;
	//Scrap保存
	AText	text;
	GetTextDocumentConst().SPI_GetText(start,end,text);
	if( text.GetItemCount() == 0 )   return;//B0000
AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Cut);
	//テキスト削除
	DeleteTextFromDocument(start,end);
}

//#913
/**
マークまでを削除
*/
void	AView_Text::DeleteToMark()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( GetTextDocumentConst().SPI_IsTextPointValid(mMarkStartTextPoint) == true &&
	   GetTextDocumentConst().SPI_IsTextPointValid(mMarkEndTextPoint) == true )
	{
		//Undoアクションタグ記録
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Delete);
		//テキスト削除
		ATextPoint	spt = {0}, ept = {0};
		SPI_GetSelect(spt,ept);
		ATextPoint	dummy = {0};
		ADocument_Text::OrderTextPoint(mMarkStartTextPoint,spt,spt,dummy);
		ADocument_Text::OrderTextPoint(mMarkEndTextPoint,ept,dummy,ept);
		DeleteTextFromDocument(spt,ept);
	}
	//マーク消去
	mMarkStartTextPoint.x = mMarkStartTextPoint.y = -1;
	mMarkEndTextPoint.x = mMarkEndTextPoint.y = -1;
}

//#913
/**
Yank
*/
void	AView_Text::Yank()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//キルバッファからデータ取得
	AText	text;
	GetApp().SPI_GetKillBuffer(text);
	//テキスト入力
	TextInput(undoTag_Typing,text);
}

//IndentStart(行頭のタブ、スペースが終了する位置)を取得する
ATextIndex	AView_Text::GetIndentStart( const ATextPoint& inTextPoint ) const
{
	//#318
	AText	zenkakuSpaceText;
	zenkakuSpaceText.SetZenkakuSpace();
	ABool	zenkakuSpaceEnable = (GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true &&
			GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true);
	//
	AIndex	paraStartLineIndex = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(inTextPoint.y);
	ATextIndex	paraStartTextIndex = GetTextDocumentConst().SPI_GetLineStart(paraStartLineIndex);
	ATextIndex	lineLength = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(paraStartLineIndex);
	for( ATextIndex pos = paraStartTextIndex; pos < paraStartTextIndex+lineLength; pos = GetTextDocumentConst().SPI_GetNextCharTextIndex(pos) )
	{
		AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(pos);
		if( ch == kUChar_Space || ch == kUChar_Tab )   continue;
		else
		{
			//#318
			if( zenkakuSpaceEnable == true )
			{
				AText	text;
				GetTextDocumentConst().SPI_GetText1CharText(pos,text);
				if( text.Compare(zenkakuSpaceText) == true )
				{
					continue;
				}
			}
			return pos;
		}
	}
	return paraStartTextIndex+lineLength;
}

ABool	AView_Text::KeyBindAction( const AKeyBindKey inKeyBindKey, const AModifierKeys inModiferKeys, 
								  const AKeyBindAction inKeyBindAction,
                                  const AText& inTextForInsertTextAction,
								  ABool& outUpdateMenu )
{
	ABool	result = true;//win
	//モード設定から、キーに対応するキーアクションを取得
	AKeyBindAction	action = keyAction_NOP;
	AText	actionText;
	ABool	isDefault = false, isAvailable = false;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).
			GetData_KeyBindAction(inKeyBindKey,inModiferKeys,action,actionText,isDefault,isAvailable);
	
	//#481
	//OS(AppKit)キーバインドからの差分を設定する場合は、キーバインド設定が無い（デフォルト）場合は、
	//OSから渡されたアクション（inKeyBindAction）を使用する。
	if( GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).
       GetData_Bool(AModePrefDB::kKeyBindDefaultIsOSKeyBind) == true && isDefault == true )
	{
		//OSから渡されたアクションを設定
		action = inKeyBindAction;
		//OSの次のアクションを実行するために、返り値にfalseを返す
		result = false;
	}
	//OSから渡された文字列挿入アクション(insertText:)の場合は、OSから渡されたテキストをactionTextに設定（下でキーマクロに記憶される）
	if( action == keyAction_InsertText )
	{
		actionText.SetText(inTextForInsertTextAction);
	}
	
	//#1336
	//escキー、かつ、アクションがない場合、フローティング検索結果ウインドウとキーワード情報ポップアップをhideする。
	if( inKeyBindKey == kKeyBindKey_Escape && action == keyAction_NOP )
	{
		//キーワード情報ポップアップをhide
		GetApp().SPI_HideFloatingIdInfoWindow();
		//検索結果をhide
		if( GetApp().SPI_IsMultiFileFindWorking() == false )
		{
			AWindowID	winID = GetApp().SPI_GetFloatingFindResultWindowID();
			if( winID != kObjectID_Invalid )
			{
				GetApp().SPI_GetFindResultWindow(winID).NVI_Hide();
				return true;
			}
		}
	}
	
	//#390 キーマクロ記録
	GetApp().SPI_RecordKeybindAction(action,actionText);
	
	//メニュー更新するかどうかの判定のために現在の状態を記憶
	ABool	svDirty = GetTextDocument().NVI_IsDirty();
	ABool	svCaretMode = mCaretMode;
	//
	ABool	select = (mForceSelectMode==true);//#388 ここでキー判定してはいけない（キーバインド設定に従うべき）((AKeyWrapper::IsShiftKeyPressed(inModiferKeys)==true) || (mForceSelectMode==true));
	ABool	edge = false;//#388 ここでキー判定してはいけない（キーバインド設定に従うべき）AKeyWrapper::IsCommandKeyPressed(inModiferKeys);
	switch(action)
	{
		//#481
		//OSから渡された文字列挿入アクション
	  case keyAction_InsertText:
		{
			TextInput(undoTag_Typing,actionText);
			break;
		}
        //
	  case keyAction_string:
		{
			SPI_DoTool_Text(actionText,true);
			break;
		}
	  case keyAction_return:
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			//GetTextDocument().SPI_RecognizeSyntaxIfDirty();
			//#905 GetTextDocument().SPI_UpdateImportFileDataIfDirty();
			//#905
			//文法認識とローカル範囲設定
			//TriggerSyntaxRecognize();
			break;
		}
	  case keyAction_indentreturn:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			//B0381 改行挿入行のインデント実行（改行入力後でないと、たとえば、</p>の前で改行した場合など、結果が正しくならない。）
			/*R0000 SDFでも改行挿入行のインデント実行して問題ないはず＆インデント実行すべき。ツールコマンドで</p><<<INDENT-RETURNの場合など。
			ABool	useSyntaxDef = 
					(GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseSyntaxDefinitionIndent) == true &&
			GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseBuiltinSyntaxDefinitionFile) == true);*/
			if( GetApp().SPI_GetAutoIndentMode() == true )//#638
			{
				if( GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
					GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent /*B0381 インデントN/Aなら改行挿入行のインデントはしない*/ )
				{
					if( GetCaretTextPoint().y > 0 ) 
					{
						ATextPoint	spt, ept;
						spt = GetCaretTextPoint();
						spt.y--;
						ept = spt;
						DoIndent(undoTag_NOP,spt,ept,inputSpaces,kIndentTriggerType_ReturnKey_CurrentLine);//#249 #639 #650
					}
				}
				DoIndent(undoTag_NOP,inputSpaces,kIndentTriggerType_ReturnKey_NewLine);//B0381 undoTagをNOPに #249 #639 #650
			}
			//#905
			//文法認識とローカル範囲設定
			//TriggerSyntaxRecognize();
			break;
		}
	  case keyAction_indent_byspace_return://#249
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			if( GetApp().SPI_GetAutoIndentMode() == true )//#638
			{
				if( GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
					GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent  )
				{
					if( GetCaretTextPoint().y > 0 ) 
					{
						ATextPoint	spt, ept;
						spt = GetCaretTextPoint();
						spt.y--;
						ept = spt;
						DoIndent(/*#650 false,*/undoTag_NOP,spt,ept,true,kIndentTriggerType_ReturnKey_CurrentLine);//#249 #639 #650
					}
				}
				
				DoIndent(/*#650 false,*/undoTag_NOP,true,kIndentTriggerType_ReturnKey_NewLine);//#249 #639
			}
			//#905
			//文法認識とローカル範囲設定
			//TriggerSyntaxRecognize();
			break;
		}
	  case keyAction_indent_onlynewline_return://#249
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			
			if( GetApp().SPI_GetAutoIndentMode() == true )//#638
			{
				DoIndent(/*#650 false,*/undoTag_NOP,inputSpaces,kIndentTriggerType_ReturnKey_NewLine);//#249 #639 #650
			}
			//#905
			//文法認識とローカル範囲設定
			//TriggerSyntaxRecognize();
			break;
		}
	  case keyAction_indent_onlynewline_byspace_return://#249
		{
			AText	text;
			text.Add(kUChar_LineEnd);
			TextInput(undoTag_Typing,text);
			
			if( GetApp().SPI_GetAutoIndentMode() == true )//#638
			{
				DoIndent(/*#650 false,*/undoTag_NOP,true,kIndentTriggerType_ReturnKey_NewLine);//#249 #639 #650
			}
			//#905
			//文法認識とローカル範囲設定
			//TriggerSyntaxRecognize();
			break;
		}
		//タブ
	  case keyAction_tab:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			InputTab(inputSpaces);//#249
			break;
		}
	  case keyAction_tab_byspace://#249
		{
			InputTab(true);
			break;
		}
		//インデント／タブ
	  case keyAction_indenttab:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ATextPoint	spt = {0}, ept = {0};
			SPI_GetSelect(spt,ept);
			if( spt.y == ept.y )
			{
				IndentTab(inputSpaces,inputSpaces);//#249
			}
			//#815
			else
			{
				//複数行選択時は左右シフト
				ShiftRightLeft((inModiferKeys==kModifierKeysMask_None),inputSpaces,true);//#1188
			}
			break;
		}
	  case keyAction_indent_byspace_tab://#249
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);
			IndentTab(true,inputSpaces);
			break;
		}
	  case keyAction_indent_byspace_tab_byspace://#249
		{
			IndentTab(true,true);
			break;
		}
		//インデント
	  case keyAction_indent://#249
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			//#255 DoIndent(false,undoTag_NOP,inputSpaces);
			Indent(inputSpaces);//#255
			break;
		}
	  case keyAction_indent_byspace://#249
		{
			//#255 DoIndent(false,undoTag_NOP,true);
			Indent(true);//#255
			break;
		}
		//
	  case keyAction_space:
		{
			AText	text;
			text.Add(kUChar_Space);
			TextInput(undoTag_Typing,text);
			break;
		}
	  case keyAction_abbrevnext:
		{
			//#1160
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseUserDefinedSDF) == false &&
				GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetKeywordListItemCount() == 0 )
			{
				//キーワードが無い場合は、キー入力文字をそのまま入力する（Shift+spaceなど）
				TextInput(undoTag_Typing,inTextForInsertTextAction);
				break;
			}
			//
			InputAbbrev(true);
			break;
		}
	  case keyAction_abbrevprev:
		{
			//#1160
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseUserDefinedSDF) == false &&
				GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetKeywordListItemCount() == 0 )
			{
				//キーワードが無い場合は、キー入力文字をそのまま入力する（Shift+spaceなど）
				TextInput(undoTag_Typing,inTextForInsertTextAction);
				break;
			}
			//
			InputAbbrev(false);
			break;
		}
	  case keyAction_deletenext:
		{
			DeleteNextChar();
			break;
		}
	  case keyAction_deleteprev:
		{
			DeletePreviousChar();
			break;
		}
	  case keyAction_deleteBackwardByDecomposingPreviousCharacter://#913
		{
			DeletePreviousChar(false);
			break;
		}
	  case keyAction_deletelineend:
		{
			DeleteToLineEnd();
			break;
		}
	  case keyAction_cutlineend:
		{
			outUpdateMenu = true;
			CutToLineEnd();
			break;
		}
	  case keyAction_cutlinestart:
		{
			outUpdateMenu = true;
			CutToLineStart();
			break;
		}
	  case keyAction_deleteparagraphend://R0213
		{
			DeleteToParagraphEnd();
			break;
		}
	  case keyAction_cutparagraphend://R0213
		{
			CutToParagraphEnd(false);
			break;
		}
		//#913
	  case keyAction_deletelinestart:
		{
			DeleteToLineStart();
			break;
		}
		//#913
	  case keyAction_deleteparagraphstart:
		{
			DeleteToParagraphStart();
			break;
		}
		//#913
	  case keyAction_deleteToMark:
		{
			DeleteToMark();
			break;
		}
		//#913
	  case keyAction_yank:
		{
			Yank();
			break;
		}
		//
	  case keyAction_additionalcutparagraphend://R0213
		{
			CutToParagraphEnd(true);
			break;
		}
	  case keyAction_caretup:
	  case keyAction_caretdown:
	  case keyAction_caretleft:
	  case keyAction_caretright:
		{
			if( select == true && edge == true )
			{
				ArrowKeyEdge(action,true,false);
			}
			else if( edge == true )
			{
				ArrowKeyEdge(action,false,false);
			}
			else if( select == true )
			{
				ArrowKeySelect(action);
			}
			else
			{
				ArrowKey(action);
			}
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_pageup:
		{
			PageupKey(false);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_pagedown:
		{
			PagedownKey(false);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_pageupwithcaret:
		{
			PageupKey(true);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_pagedownwithcaret:
		{
			PagedownKey(true);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_caretlinestart:
		{
			ArrowKeyEdge(keyAction_caretleft,select,false);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_caretlineend:
		{
			ArrowKeyEdge(keyAction_caretright,select,false);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_caretparagraphstart:
		{
			ArrowKeyEdge(keyAction_caretleft,select,true);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_caretparagraphend:
		{
			ArrowKeyEdge(keyAction_caretright,select,true);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_selectlinestart:
		{
			ArrowKeyEdge(keyAction_caretleft,true,false);
			break;
		}
	  case keyAction_selectlineend:
		{
			ArrowKeyEdge(keyAction_caretright,true,false);
			break;
		}
		//#913
	  case keyAction_selectparagraphstart:
		{
			ArrowKeyEdge(keyAction_caretleft,true,true);
			break;
		}
		//#913
	  case keyAction_selectparagraphend:
		{
			ArrowKeyEdge(keyAction_caretright,true,true);
			break;
		}
		//#913
	  case keyAction_selectLine:
		{
			SelectWholeLine();
			break;
		}
		//#913
	  case keyAction_selectParagraph:
		{
			SelectWholeParagraph();
			break;
		}
		//#913
	  case keyAction_selectWord:
		{
			SelectWord();
			break;
		}
		//#913
	  case keyAction_selectAll:
		{
			SetSelect(0,GetTextDocumentConst().SPI_GetTextLength());
			break;
		}
		//#815
	  case keyAction_shiftleft:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(false,inputSpaces);
			break;
		}
	  case keyAction_shiftright:
		{
			ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
			ShiftRightLeft(true,inputSpaces);
			break;
		}
		//
	  case keyAction_home:
		{
			HomeKey();
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_end:
		{
			EndKey();
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_recenter:
		{
			Recenter();
			break;
		}
	  case keyAction_correspondence:
		{
			InputCorrespondence();
			break;
		}
	  case keyAction_carethome:
		{
			ArrowKeyEdge(keyAction_caretup,select,false);
			break;
		}
	  case keyAction_caretend:
		{
			ArrowKeyEdge(keyAction_caretdown,select,false);
			break;
		}
	  case keyAction_selecthome:
		{
			ArrowKeyEdge(keyAction_caretup,true,false);
			break;
		}
	  case keyAction_selectend:
		{
			ArrowKeyEdge(keyAction_caretdown,true,false);
			break;
		}
	  case keyAction_selectbrace:
		{
			SelectBrace();
			break;
		}
	  case keyAction_previousword:
		{
			ArrowKeyWord(keyAction_caretleft,select,false);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_nextword:
		{
			ArrowKeyWord(keyAction_caretright,select,false);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_previousword_linestop:
		{
			ArrowKeyWord(keyAction_caretleft,select,true);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_nextword_linestop:
		{
			ArrowKeyWord(keyAction_caretright,select,true);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_selectpreviousword_linestop:
		{
			ArrowKeyWord(keyAction_caretleft,true,true);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_selectnextword_linestop:
		{
			ArrowKeyWord(keyAction_caretright,true,true);
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_selectleft:
		{
			ArrowKeySelect(keyAction_caretleft);
			break;
		}
	  case keyAction_selectright:
		{
			ArrowKeySelect(keyAction_caretright);
			break;
		}
	  case keyAction_selectup:
		{
			ArrowKeySelect(keyAction_caretup);
			break;
		}
	  case keyAction_selectdown:
		{
			ArrowKeySelect(keyAction_caretdown);
			break;
		}
	  case keyAction_selectnextword:
		{
			ArrowKeyWord(keyAction_caretright,true,false);
			break;
		}
	  case keyAction_selectprevword:
		{
			ArrowKeyWord(keyAction_caretleft,true,false);
			break;
		}
		//#913
	  case keyAction_setMark:
		{
			SetMark();
			break;
		}
		//#913
	  case keyAction_swapWithMark:
		{
			SwapMark();
			break;
		}
		//#913
	  case keyAction_selectToMark:
		{
			SelectToMark();
			break;
		}
		//
	  case keyAction_Undo:
		{
			outUpdateMenu = true;
			UndoRedo(true);
			break;
		}
	  case keyAction_Redo:
		{
			outUpdateMenu = true;
			UndoRedo(false);
			break;
		}
	  case keyAction_Cut:
		{
			outUpdateMenu = true;
			Cut(false);
			break;
		}
	  case keyAction_Copy:
		{
			outUpdateMenu = true;
			Copy(false);
			break;
		}
	  case keyAction_Paste:
		{
			Paste();
			break;
		}
	  case keyAction_additionalcopy://R0213
		{
			outUpdateMenu = true;
			Copy(true);
			break;
		}
	  case keyAction_additionalcut://R0213
		{
			outUpdateMenu = true;
			Cut(true);
			break;
		}
	  case keyAction_sameletterup:
		{
			SameLetterUp();
			break;
		}
	  case keyAction_duplicateline:
		{
			DuplicateLine();
			break;
		}
	  case keyAction_insertline:
		{
			InsertEmptyLine();
			break;
		}
	  case keyAction_selectmode:
		{
			mForceSelectMode = !mForceSelectMode;
			break;
		}
	  case keyAction_scrollleft:
		{
			NVI_Scroll(-NVI_GetHScrollBarUnit(),0);
			break;
		}
	  case keyAction_scrollright:
		{
			NVI_Scroll(NVI_GetHScrollBarUnit(),0);
			break;
		}
	  case keyAction_scrollup:
		{
			NVI_Scroll(0,-NVI_GetVScrollBarUnit(),true,true,kScrollTrigger_ScrollKey);//#1031
			break;
		}
	  case keyAction_scrolldown:
		{
			NVI_Scroll(0,NVI_GetVScrollBarUnit(),true,true,kScrollTrigger_ScrollKey);//#1031
			break;
		}
		//R0006
	  case keyAction_prevdiff:
		{
			PrevDiff();
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_nextdiff:
		{
			NextDiff();
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
		//RC2
	  case keyAction_idinfo:
		{
			//キーワード情報
			//#853 IdInfo();
			DisplayIdInfo(true,kIndex_Invalid,kIndex_Invalid);//#853
			break;
		}
		//#124
	  case keyAction_deletewholeparagraph:
		{
			DeleteCurrentParagraph();
			break;
		}
		//#150
	  case keyAction_nextheader:
		{
			JumpNext();
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
	  case keyAction_prevheader:
		{
			JumpPrev();
			//キャレット表示 #688
			ShowCaretIfReserved();
			break;
		}
		//#357
	  case keyAction_switch_nexttab:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchTabNext();
			}
			break;
		}
	  case keyAction_switch_prevtab:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SwitchTabPrev();
			}
			break;
		}
		//#594
	  case keyAction_linedialog:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowMoveLineWindow();
			}
			break;
		}
		//#638
	  case keyAction_autoindentmode:
		{
			GetApp().SPI_SetAutoIndentMode(!GetApp().SPI_GetAutoIndentMode());
			break;
		}
		//#724
	  case keyAction_macro1:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(0);
			}
			break;
		}
	  case keyAction_macro2:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(1);
			}
			break;
		}
	  case keyAction_macro3:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(2);
			}
			break;
		}
	  case keyAction_macro4:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(3);
			}
			break;
		}
	  case keyAction_macro5:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(4);
			}
			break;
		}
	  case keyAction_macro6:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(5);
			}
			break;
		}
	  case keyAction_macro7:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(6);
			}
			break;
		}
	  case keyAction_macro8:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(7);
			}
			break;
		}
	  case keyAction_macro9:
		{
			if( mTextWindowID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DoMacroButton(8);
			}
			break;
		}
		//#1154
	  case keyAction_swapLetter:
		{
			SwapLetter();
			break;
		}
		//#1397
	  case keyAction_findnext:
		{
            SPI_FindNext();
			break;
		}
	  case keyAction_findprev:
		{
			SPI_FindPrev();
			break;
		}
		//#1399
	  case keyAction_caretprevparagraphstart:
		{
			ArrowKeyNextPrevEdge(keyAction_caretup,false);
			break;
		}
	  case keyAction_caretnextparagraphend:
		{
			ArrowKeyNextPrevEdge(keyAction_caretdown,false);
			break;
		}
	  case keyAction_selectprevparagraphstart:
		{
			ArrowKeyNextPrevEdge(keyAction_caretup,true);
			break;
		}
	  case keyAction_selectnextparagraphend:
		{
			ArrowKeyNextPrevEdge(keyAction_caretdown,true);
			break;
		}
		//
	  default:
		{
			result = false;//win
			break;
		}
	}
	//状態変化が無ければメニュー更新しない
	if( svDirty == GetTextDocument().NVI_IsDirty() && svCaretMode == mCaretMode )
	{
		outUpdateMenu = false;
	}
	return result;//win
}

//#558
/**
縦書きモード用 矢印キーの変換
*/
AKeyBindAction	AView_Text::ConvertArrowKeyForVerticalMode( const AKeyBindAction inAction )
{
	AKeyBindAction	action = inAction;
	switch(inAction)
	{
	  case keyAction_caretleft:
		{
			action = keyAction_caretdown;
			break;
		}
	  case keyAction_caretright:
		{
			action = keyAction_caretup;
			break;
		}
	  case keyAction_caretup:
		{
			action = keyAction_caretleft;
			break;
		}
	  case keyAction_caretdown:
		{
			action = keyAction_caretright;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	return action;
}

//矢印キー（ノーマル）
void	AView_Text::ArrowKey( const AKeyBindAction inAction )
{
	//選択予約解除#699
	CancelReservedSelection();
	//補完入力状態解除 #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//縦書きモードなら矢印キーを変換する
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	//ArrowKeySelectと同じアルゴリズムじゃダメ？検討必要
	ATextPoint	caretTextPoint;
	AImagePoint	imagePoint;
	switch(action)
	{
		//左
	  case keyAction_caretleft:
		{
			if( IsCaretMode() == true )
			{
				GetCaretTextPoint(caretTextPoint);
				/*B0000 if( caretTextPoint.x == 0 )
				{
					if( caretTextPoint.y == 0 )
					{
						caretTextPoint.x = 0;
					}
					else
					{
						caretTextPoint.y--;
						caretTextPoint.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y);
					}
				}
				else
				{
					ATextIndex	index = GetTextDocument().SPI_GetTextIndexFromTextPoint(caretTextPoint);
					index = GetTextDocument().SPI_GetPrevCharTextIndex(index);
					GetTextDocument().SPI_GetTextPointFromTextIndex(index,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);*/
				//B0000 行折り返しの行頭の場合、行頭１文字目の後の←キーで行頭移動、行頭での←キーで前行の最後の文字の前へ移動するように修正
				ATextIndex	index = GetTextDocument().SPI_GetTextIndexFromTextPoint(caretTextPoint);
				if( index == 0 )
				{
					//処理無し
				}
				else
				{
					index = GetTextDocument().SPI_GetPrevCharTextIndex(index);
					GetTextDocument().SPI_GetTextPointFromTextIndex(index,caretTextPoint,true);
					//#450
					//移動先text pointが折りたたみ中の行なら、折りたたまれていない行の行末まで戻る
					if( mLineImageInfo_Height.Get(caretTextPoint.y) == 0 )
					{
						for( ; caretTextPoint.y > 0; caretTextPoint.y-- )
						{
							if( mLineImageInfo_Height.Get(caretTextPoint.y) > 0 )
							{
								break;
							}
						}
						//行末
						caretTextPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y);
					}
					//キャレット設定
					SetCaretTextPoint(caretTextPoint);
				}
			}
			else
			{
				ATextPoint	pt1, pt2;
				ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt1);
			}
			break;
		}
		//右
	  case keyAction_caretright:
		{
			if( IsCaretMode() == true )
			{
				GetCaretTextPoint(caretTextPoint);
				/*B0000 if( caretTextPoint.x >= GetTextDocument().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y) )
				{
					if( caretTextPoint.y >= GetTextDocument().SPI_GetLineCount()-1 )
					{
						caretTextPoint.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y);
					}
					else
					{
						caretTextPoint.y++;
						caretTextPoint.x = 0;
					}
				}
				else
				{
					ATextIndex	index = GetTextDocument().SPI_GetTextIndexFromTextPoint(caretTextPoint);
					index = GetTextDocument().SPI_GetNextCharTextIndex(index);
					GetTextDocument().SPI_GetTextPointFromTextIndex(index,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);*/
				//B0000 行折り返しの行末の場合、行末１文字前の後の→キーで行頭移動、行末での→キーで次行の行頭１文字目の後へ移動するように修正(TextEditに合わせる）
				ATextIndex	index = GetTextDocument().SPI_GetTextIndexFromTextPoint(caretTextPoint);
				if( index >= GetTextDocumentConst().SPI_GetTextLength() )
				{
					//処理無し
				}
				else
				{
					index = GetTextDocument().SPI_GetNextCharTextIndex(index);
					GetTextDocument().SPI_GetTextPointFromTextIndex(index,caretTextPoint,true);
					//#450
					//移動先text pointが折りたたみ中の行なら、折りたたまれていない行の行頭まで進む
					if( mLineImageInfo_Height.Get(caretTextPoint.y) == 0 )
					{
						for( ; caretTextPoint.y < GetTextDocument().SPI_GetLineCount()-1; caretTextPoint.y++ )
						{
							if( mLineImageInfo_Height.Get(caretTextPoint.y) > 0 )
							{
								break;
							}
						}
						//行頭
						caretTextPoint.x = 0;
					}
					//キャレット設定
					SetCaretTextPoint(caretTextPoint);
				}
			}
			else
			{
				ATextPoint	pt1, pt2;
				ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt2);
			}
			break;
		}
		//上
	  case keyAction_caretup:
		{
			if( IsCaretMode() == true )
			{
				ANumber	svOldArrowImageX = GetOldArrowImageX();
				GetCaretTextPoint(caretTextPoint);
				if( caretTextPoint.y == 0 )
				{
					caretTextPoint.x = 0;
				}
				else
				{
					caretTextPoint.y--;
					//#450
					//移動先text pointが折りたたみ中の行なら、折りたたまれていない行まで戻る
					for( ; caretTextPoint.y > 0; caretTextPoint.y-- )
					{
						if( mLineImageInfo_Height.Get(caretTextPoint.y) > 0 )
						{
							break;
						}
					}
					//
					imagePoint.y = GetImageYByLineIndex(caretTextPoint.y)+2;//#450 caretTextPoint.y * GetLineHeightWithMargin() +2;
					imagePoint.x = GetOldArrowImageX();
					GetTextPointFromImagePoint(imagePoint,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);
				SetOldArrowImageX(svOldArrowImageX);
			}
			else
			{
				ATextPoint	pt1, pt2;
				ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt1);
			}
			break;
		}
		//下
	  case keyAction_caretdown:
		{
			if( IsCaretMode() == true )
			{
				ANumber	svOldArrowImageX = GetOldArrowImageX();
				GetCaretTextPoint(caretTextPoint);
				if( caretTextPoint.y >= GetTextDocument().SPI_GetLineCount()-1 )
				{
					caretTextPoint.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(caretTextPoint.y);
				}
				else
				{
					caretTextPoint.y++;
					//
					imagePoint.y = GetImageYByLineIndex(caretTextPoint.y)+2;//#450 caretTextPoint.y * GetLineHeightWithMargin() +2;
					imagePoint.x = GetOldArrowImageX();
					GetTextPointFromImagePoint(imagePoint,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);
				SetOldArrowImageX(svOldArrowImageX);
			}
			else
			{
				ATextPoint	pt1, pt2;
				ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt2);
			}
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
	//スクロール位置補正
	AdjustScroll(GetCaretTextPoint());
}

//矢印キー（選択）
void	AView_Text::ArrowKeySelect( const AKeyBindAction inAction )
{
	//補完入力状態解除 #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//縦書きモードなら矢印キーを変換する
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	ATextPoint	selectPoint = GetCaretTextPoint();
	if( IsCaretMode() == false )
	{
		selectPoint = GetSelectTextPoint();
	}
	//#108
	//矩形選択中は矩形範囲の最後のポイントから開始する
	if( mCaretMode == false && mKukeiSelectionMode == true )
	{
		selectPoint = GetSelectTextPoint();
		if( mKukeiSelected_End.GetItemCount() > 0 )
		{
			selectPoint = mKukeiSelected_End.Get(mKukeiSelected_End.GetItemCount()-1);
		}
	}
	//
	ATextIndex	selectPos = GetTextDocument().SPI_GetTextIndexFromTextPoint(selectPoint);
	switch(action)
	{
		//左
	  case keyAction_caretleft:
		{
			selectPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(selectPos);
			GetTextDocument().SPI_GetTextPointFromTextIndex(selectPos,selectPoint,true);//B0000
			//#450
			//移動先text pointが折りたたみ中の行なら、折りたたまれていない行の行末まで戻る
			if( mLineImageInfo_Height.Get(selectPoint.y) == 0 )
			{
				for( ; selectPoint.y > 0; selectPoint.y-- )
				{
					if( mLineImageInfo_Height.Get(selectPoint.y) > 0 )
					{
						break;
					}
				}
				//行末
				selectPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(selectPoint.y);
			}
			break;
		}
		//右
	  case keyAction_caretright:
		{
			selectPos = GetTextDocumentConst().SPI_GetNextCharTextIndex(selectPos);
			GetTextDocument().SPI_GetTextPointFromTextIndex(selectPos,selectPoint,true);//B0000
			//#450
			//移動先text pointが折りたたみ中の行なら、折りたたまれていない行の行頭まで進む
			if( mLineImageInfo_Height.Get(selectPoint.y) == 0 )
			{
				for( ; selectPoint.y < GetTextDocument().SPI_GetLineCount()-1; selectPoint.y++ )
				{
					if( mLineImageInfo_Height.Get(selectPoint.y) > 0 )
					{
						break;
					}
				}
				//行頭
				selectPoint.x = 0;
			}
			break;
		}
		//上
	  case keyAction_caretup:
		{
			//B0362
			if( selectPoint.y == 0 )
			{
				selectPoint.x = 0;
			}
			else
			{
				AImagePoint	imagePoint;
				GetImagePointFromTextPoint(selectPoint,imagePoint);
				//#450
				//移動先text pointが折りたたみ中の行なら、折りたたまれていない行まで戻る
				for( ; selectPoint.y-1 > 0 ; selectPoint.y-- )
				{
					if( mLineImageInfo_Height.Get(selectPoint.y-1) > 0 )
					{
						break;
					}
				}
				//
				imagePoint.y -= GetLineHeightWithMargin(selectPoint.y-1);//#450 GetLineHeightWithMargin();
				GetTextPointFromImagePoint(imagePoint,selectPoint);
			}
			break;
		}
		//下
	  case keyAction_caretdown:
		{
			//B0362
			if( selectPoint.y >= GetTextDocument().SPI_GetLineCount()-1 )
			{
				selectPoint.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(selectPoint.y);
			}
			else
			{
				AImagePoint	imagePoint;
				GetImagePointFromTextPoint(selectPoint,imagePoint);
				//
				imagePoint.y += GetLineHeightWithMargin(selectPoint.y) + 2;//#450 GetLineHeightWithMargin();
				GetTextPointFromImagePoint(imagePoint,selectPoint);
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	if( mCaretMode == true || mKukeiSelectionMode == false )
	{
		SetSelect(GetCaretTextPoint(),selectPoint);
	}
	else //#108
	{
		//矩形選択中の場合
		AImagePoint	spt = mCaretImagePoint;
		if( mKukeiSelected_Start.GetItemCount() > 0 )
		{
			GetImagePointFromTextPoint(mKukeiSelected_Start.Get(0),spt);
		}
		AImagePoint	ept = {0};
		GetImagePointFromTextPoint(selectPoint,ept);
		SetKukeiSelect(spt,ept);
	}
	
	//スクロール位置補正
	AdjustScroll(GetSelectTextPoint(),0);//B0434
}

//矢印キー（単語移動）
void	AView_Text::ArrowKeyWord( const AKeyBindAction inAction, const ABool inSelect, const ABool inLineStop )
{
	//補完入力状態解除 #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//縦書きモードなら矢印キーを変換する
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	ATextPoint	textpoint = GetCaretTextPoint();
	if( IsCaretMode() == false )
	{
		if( inSelect == false )
		{
			ArrowKey(inAction);
			return;
		}
		textpoint = GetSelectTextPoint();
	}
	ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(textpoint);
	switch(action)
	{
		//左
	  case keyAction_caretleft:
		{
			if( textpos == 0 )   break;
			textpos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textpos);
			ATextIndex	spos,epos;
			GetTextDocument().SPI_FindWord(textpos,spos,epos);
			while( spos > 0 )
			{
				if( inLineStop == true && GetTextDocumentConst().SPI_GetTextChar(spos-1) == kUChar_LineEnd )   break;
				if( GetTextDocumentConst().SPI_IsAsciiAlphabet(spos) == true || GetTextDocumentConst().SPI_IsUTF8Multibyte(spos) == true )   break;
				spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
				GetTextDocument().SPI_FindWord(spos,spos,epos);
			}
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,textpoint,true);//B0000
			//#450
			//移動先text pointが折りたたみ中の行なら、折りたたまれていない行の行末まで戻る
			if( mLineImageInfo_Height.Get(textpoint.y) == 0 )
			{
				for( ; textpoint.y > 0; textpoint.y-- )
				{
					if( mLineImageInfo_Height.Get(textpoint.y) > 0 )
					{
						break;
					}
				}
				//行末
				textpoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
			}
			//
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//右
	  case keyAction_caretright:
		{
			ATextIndex	spos,epos;
			GetTextDocument().SPI_FindWord(textpos,spos,epos);
			while( epos < GetTextDocumentConst().SPI_GetTextLength() )
			{
				if( inLineStop == true && GetTextDocumentConst().SPI_GetTextChar(epos) == kUChar_LineEnd )   break;
				if( GetTextDocumentConst().SPI_IsAsciiAlphabet(epos-1) == true || GetTextDocumentConst().SPI_IsUTF8Multibyte(epos-1) == true )   break;//B0000
				GetTextDocument().SPI_FindWord(epos,spos,epos);
			}
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,textpoint,true);//B0000
			//#450
			//移動先text pointが折りたたみ中の行なら、折りたたまれていない行の行頭まで進む
			if( mLineImageInfo_Height.Get(textpoint.y) == 0 )
			{
				for( ; textpoint.y < GetTextDocument().SPI_GetLineCount()-1; textpoint.y++ )
				{
					if( mLineImageInfo_Height.Get(textpoint.y) > 0 )
					{
						break;
					}
				}
				//行頭
				textpoint.x = 0;
			}
			//
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//上、下
	  case keyAction_caretup:
	  case keyAction_caretdown:
		{
			if( inSelect == true )
			{
				ArrowKeySelect(inAction);
			}
			else
			{
				ArrowKey(inAction);
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	
	//スクロール位置補正
	textpoint = GetCaretTextPoint();
	if( inSelect == true )   textpoint = GetSelectTextPoint();
	AdjustScroll(textpoint);
}

//矢印キー（端まで移動）
void	AView_Text::ArrowKeyEdge( const AKeyBindAction inAction, const ABool inSelect, const ABool inParagraphEdge )
{
	//補完入力状態解除 #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//縦書きモードなら矢印キーを変換する
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	ATextPoint	textpoint;
	switch(action)
	{
		//左
	  case keyAction_caretleft:
		{
			textpoint = start;
			if( inParagraphEdge == true )
			{
				textpoint.y = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(start.y);
			}
			textpoint.x = 0;
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//右
	  case keyAction_caretright:
		{
			textpoint = end;
			if( inParagraphEdge == true )
			{
				textpoint.y = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(start.y)-1;
			}
			textpoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//上
	  case keyAction_caretup:
		{
			textpoint.x = 0;
			textpoint.y = 0;
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//下
	  case keyAction_caretdown:
		{
			textpoint.y = GetTextDocumentConst().SPI_GetLineCount()-1;
			textpoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	
	//スクロール位置補正
	textpoint = GetCaretTextPoint();
	if( inSelect == true )   textpoint = GetSelectTextPoint();
	AdjustScroll(textpoint);
}

//#1399
/**
前段落頭、次段落末へ移動／選択
*/
void	AView_Text::ArrowKeyNextPrevEdge( const AKeyBindAction inAction, const ABool inSelect )
{
	//補完入力状態解除 #717
	ClearAbbrevInputMode(true,false);
	
	//#558
	//縦書きモードなら矢印キーを変換する
	AKeyBindAction	action = inAction;
	if( NVI_GetVerticalMode() == true )
	{
		action = ConvertArrowKeyForVerticalMode(inAction);
	}
	
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	ATextPoint	textpoint;
	switch(action)
	{
		//上
	  case keyAction_caretup:
		{
			//選択中の場合はselect pointから移動する（ただし、キャレット移動の場合はstartから移動する）
			if( IsCaretMode() == false && inSelect == true )
			{
				GetSelectTextPoint(textpoint);
			}
			else
			{
				textpoint = start;
			}
			//現在左端の場合は、前の行の左端へ移動する
			if( textpoint.x == 0 && textpoint.y > 0 )
			{
				textpoint.y--;
			}
			//現在段落の最初へ移動する
			textpoint.y = GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(textpoint.y);
			textpoint.x = 0;
			//キャレット設定／セレクト設定
			if( inSelect == true )
			{
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
		//下
	  case keyAction_caretdown:
		{
			//選択中の場合はselect pointから移動する（ただし、キャレット移動の場合はendから移動する）
			if( IsCaretMode() == false && inSelect == true )
			{
				GetSelectTextPoint(textpoint);
			}
			else
			{
				textpoint = end;
			}
			//現在右端の場合は、次の行へ移動する
			if( textpoint.x == GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y) &&
				textpoint.y+1 < GetTextDocumentConst().SPI_GetLineCount() )
			{
				textpoint.y++;
			}
			//次の段落の１つ前の行の右端（つまり現在段落の最後）へ移動する
			textpoint.y = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(textpoint.y)-1;
			textpoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
			//キャレット設定／セレクト設定
			if( inSelect == true )
			{
				//次の行の最初へ移動する（キャレット移動と動作が異なっており統一性がないが、OSの標準動作がこうなっているので合わせる。）
				if( textpoint.y + 1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					textpoint.x = 0;
					textpoint.y++;
				}
				//
				SetSelectTextPoint(textpoint);
			}
			else
			{
				SetCaretTextPoint(textpoint);
			}
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	
	//スクロール位置補正
	textpoint = GetCaretTextPoint();
	if( inSelect == true )   textpoint = GetSelectTextPoint();
	AdjustScroll(textpoint);
}

void	AView_Text::HomeKey()
{
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

void	AView_Text::EndKey()
{
	AImagePoint	pt;
	pt.x = 0;
	pt.y = NVM_GetImageHeight();//NVI_Scrollの中で補正される
	NVI_ScrollTo(pt);
}

void	AView_Text::PageupKey( const ABool inWithCaret )
{
	NVI_Scroll(0,-NVI_GetVScrollBarPageUnit());
	if( inWithCaret == true && NVM_IsImagePointInViewRect(mCaretImagePoint) == false )
	{
		AImagePoint	origin;
		NVI_GetOriginOfLocalFrame(origin);
		AImagePoint	imagePoint;
		imagePoint.x = mCaretImagePoint.x;
		imagePoint.y = origin.y + NVI_GetViewHeight()/2;
		ATextPoint	caretTextPoint;
		GetTextPointFromImagePoint(imagePoint,caretTextPoint);
		SetCaretTextPoint(caretTextPoint);
	}
	NVM_GetWindow().NVI_UpdateWindow();//#420
}

void	AView_Text::PagedownKey( const ABool inWithCaret )
{
	NVI_Scroll(0,NVI_GetVScrollBarPageUnit());
	if( inWithCaret == true && NVM_IsImagePointInViewRect(mCaretImagePoint) == false )
	{
		AImagePoint	origin;
		NVI_GetOriginOfLocalFrame(origin);
		AImagePoint	imagePoint;
		imagePoint.x = mCaretImagePoint.x;
		imagePoint.y = origin.y + NVI_GetViewHeight()/2;
		ATextPoint	caretTextPoint;
		GetTextPointFromImagePoint(imagePoint,caretTextPoint);
		SetCaretTextPoint(caretTextPoint);
	}
	NVM_GetWindow().NVI_UpdateWindow();//#420
}

void	AView_Text::Recenter()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	AImagePoint	imagespt;
	GetImagePointFromTextPoint(spt,imagespt);
	ANumber	viewHeight = NVI_GetViewHeight();
	AImagePoint	pt;
	pt.x = 0;
	pt.y = imagespt.y - viewHeight/2;
	NVI_ScrollTo(pt);
}

void	AView_Text::UndoRedo( const ABool inUndo )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	/*#182
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_FixInlineInput();//B0079
	}
	*/
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());//#688
	//
	AIndex	selectTextIndex;
	AItemCount	selectTextLength;
	if( inUndo == true )
	{
		AText	text;
		if( GetTextDocumentConst().SPI_GetUndoText(text) == false )   return;
		GetTextDocument().SPI_Undo(selectTextIndex,selectTextLength);
	}
	else
	{
		AText	text;
		if( GetTextDocumentConst().SPI_GetRedoText(text) == false )   return;
		GetTextDocument().SPI_Redo(selectTextIndex,selectTextLength);
	}
	if( selectTextIndex == kIndex_Invalid )   return;
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(selectTextIndex,caretPoint);
	SetCaretTextPoint(caretPoint);
	if( selectTextLength > 0 )
	{
		GetTextDocument().SPI_GetTextPointFromTextIndex(selectTextIndex+selectTextLength,selectPoint);
		SetSelectTextPoint(selectPoint);
	}
	AdjustScroll_Center(GetCaretTextPoint());
}

void	AView_Text::Cut( const ABool inAdditional )//R0213
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	AText	text;
	GetSelectedText(text);
	if( text.GetItemCount() == 0 )   return;//#1400
	if( inAdditional == true )//R0213
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);
		text.InsertText(0,scrapText);
	}
	//#878
	//FIFOクリップボードモードならFIFOにキュー
	if( GetApp().SPI_IsFIFOClipboardMode() == true )
	{
		EnqueueFIFOClipboard(text);
	}
	//クリップボードに設定
	AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	DeleteSelect(undoTag_Cut);
}

void	AView_Text::Copy( const ABool inAdditional )//R0213
{
	AText	text;
	GetSelectedText(text);
	if( text.GetItemCount() == 0 )   return;
	//#150
	//クリップボード履歴に追加
	GetApp().SPI_AddClipboardHistory(text);
	//
	if( inAdditional == true )//R0213
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
		text.InsertText(0,scrapText);
	}
	//#878
	//FIFOクリップボードモードならFIFOにキュー
	if( GetApp().SPI_IsFIFOClipboardMode() == true )
	{
		EnqueueFIFOClipboard(text);
	}
	//クリップボードに設定
	AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win 
}

/**
FIFOクリップボードに追加
*/
void	AView_Text::EnqueueFIFOClipboard( const AText& inText )
{
	//FIFOにつむ
	GetApp().SPI_EnqueueFIFOClipboard(inText);
	//notification表示
	AText	firsttext;
	GetApp().SPI_GetFirstTextInFIFOClipboard(firsttext);
	AText	infotext;
	infotext.SetLocalizedText("Notification_CurrentPaste");
	infotext.ReplaceParamText('0',firsttext);
	AText	notificationTitle;
	notificationTitle.SetLocalizedText("Notification_FIFOCopy");
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_General(notificationTitle,infotext,GetEmptyText());
		SPI_ShowNotificationPopupWindow(true);
	}
}

/**
FIFOクリップボードから取り出し
*/
void	AView_Text::DequeueFIFOClipboard( AText& outText )
{
	if( GetApp().SPI_GetFIFOClipboardCount() > 0 )
	{
		//FIFOキューからテキスト取得
		GetApp().SPI_DequeueFIFOClipboard(outText);
		//キューから取得したテキストをクリップボードに設定
		AScrapWrapper::SetClipboardTextScrap(outText,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300
	}
	else
	{
		//クリップボードからテキスト取得
		AScrapWrapper::GetClipboardTextScrap(outText);
	}
	//notification表示
	AText	firsttext;
	if( GetApp().SPI_GetFIFOClipboardCount() > 0 )
	{
		GetApp().SPI_GetFirstTextInFIFOClipboard(firsttext);
	}
	else
	{
		AScrapWrapper::GetClipboardTextScrap(firsttext);
	}
	AText	infotext;
	infotext.SetLocalizedText("Notification_CurrentPaste");
	infotext.ReplaceParamText('0',firsttext);
	AText	notificationTitle;
	notificationTitle.SetLocalizedText("Notification_FIFOCopy");
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_General(notificationTitle,infotext,GetEmptyText());
		SPI_ShowNotificationPopupWindow(true);
	}
}

//R0015 B0364
void	AView_Text::CopyWithHTMLColor()
{
	AText	text;
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	spt.x = 0;
	if( ept.x != 0 )
	{
		ept.y++;
		ept.x = 0;
	}
	SetSelect(spt,ept);
	text.AddCstring("<pre class=\"\"><code>");
	for( AIndex lineIndex = spt.y; lineIndex < ept.y; lineIndex++ )
	{
		CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		ATextPoint	tmp1, tmp2;
		AAnalyzeDrawData	tmp3;//RC1
		GetTextDocumentConst().SPI_GetTextDrawDataWithStyle(lineIndex,textDrawData,false,tmp1,tmp2,tmp3,GetObjectID());
		AText	t;
		t.SetText(textDrawData.UTF16DrawText);
		t.ConvertToUTF8FromUTF16();
		AIndex	unicodeCharIndex = 0;
		ABool	first = true;
		for( AIndex i = 0; i < t.GetItemCount(); i = t.GetNextCharPos(i) )
		{
			AIndex	attrIndex = kIndex_Invalid;
			for( AIndex j = 0; j < textDrawData.attrPos.GetItemCount(); j++ )
			{
				if( textDrawData.attrPos.Get(j) == unicodeCharIndex )
				{
					attrIndex = j;
				}
			}
			if( attrIndex != kIndex_Invalid )
			{
				if( first == false )
				{
					AText	addtext;
					addtext.SetCstring("</span>");
					t.InsertText(i,addtext);
					i += addtext.GetItemCount();
				}
				AText	colortext;
				AColorWrapper::GetHTMLFormatColorText(textDrawData.attrColor.Get(attrIndex),colortext);
				AText	addtext;
				addtext.SetCstring("<span style=\"color:#");
				addtext.AddText(colortext);
				addtext.AddCstring("\">");
				t.InsertText(i,addtext);
				i += addtext.GetItemCount();
				first = false;
			}
			//<>等書き換え
			AUChar	ch = t.Get(i);
			if( ch == '<' )
			{
				AText	ht;
				ht.SetCstring("&lt;");
				t.Delete1(i);
				t.InsertText(i,ht);
				i += ht.GetItemCount()-1;
			}
			else if( ch == '>' )
			{
				AText	ht;
				ht.SetCstring("&gt;");
				t.Delete1(i);
				t.InsertText(i,ht);
				i += ht.GetItemCount()-1;
			}
			else if( ch == '&' )
			{
				AText	ht;
				ht.SetCstring("&amp;");
				t.Delete1(i);
				t.InsertText(i,ht);
				i += ht.GetItemCount()-1;
			}
			//
			unicodeCharIndex++;
		}
		if( first == false )
		{
			AText	addtext;
			addtext.SetCstring("</span>");
			t.AddText(addtext);
		}
		text.AddText(t);
		text.Add(kUChar_LineEnd);
	}
	text.AddCstring("</code></pre>");
	AScrapWrapper::SetClipboardTextScrap(text,true,GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kAutoConvertToCanonicalComp), GetTextDocumentConst().SPI_ShouldConvertFrom5CToA5ForCopy());//#1300 #688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
}

void	AView_Text::Paste()
{
	//#800
	//矩形モード中なら矩形ペーストする
	if( mKukeiSelectionMode == true )
	{
		PasteKukei();
		return;
	}
	//
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	AText	text;
	//#878
	//FIFOクリップモードならデキューして、クリップボードに設定
	if( GetApp().SPI_IsFIFOClipboardMode() == true )
	{
		DequeueFIFOClipboard(text);
	}
	else
	{
		//クリップボードからテキスト取得
		AScrapWrapper::GetClipboardTextScrap(text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	}
	ATextPoint	spt1 = {0,0}, ept1 = {0,0};//#646
	SPI_GetSelect(spt1,ept1);//#646
	TextInput(undoTag_Paste,text);
	//行折り返しスレッド動作中判定
	if( GetTextDocumentConst().SPI_IsWrapCalculating() == false )//#699 行折り返しスレッド動作した場合は、ペースト時の自動インデントはしない
	{
		//自動インデントONの場合のみペースト時自動インデント実行
		if( GetApp().SPI_GetAutoIndentMode() == true )
		{
			//#646
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndentWhenPaste) == true )
			{
				//インデント実行
				ATextPoint	spt2 = {0,0}, ept2 = {0,0};
				SPI_GetSelect(spt2,ept2);
				ATextPoint	spt = spt1, ept = ept2;
				ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);
				DoIndent(undoTag_NOP,spt,ept,inputSpaces,kIndentTriggerType_IndentFeature);
				if( ept.y >= GetTextDocumentConst().SPI_GetLineCount() )   ept.y = GetTextDocumentConst().SPI_GetLineCount()-1;
				SetSelect(ept,ept);
			}
		}
	}
	//#905
	//文法認識とローカル範囲設定
	TriggerSyntaxRecognize();
}

void	AView_Text::PasteKukei()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	AText	text;
	AScrapWrapper::GetClipboardTextScrap(text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Paste);
	//矩形入力
	KukeiInput(text);
	//#905
	//文法認識とローカル範囲設定
	TriggerSyntaxRecognize();
}

/**
クリップボードFIFOモード切替
*/
void	AView_Text::SwitchFIFOClipboardMode()
{
	//FIFOモード切替
	GetApp().SPI_SetFIFIOClipboardMode(!(GetApp().SPI_IsFIFOClipboardMode()));
	
	//FIFOモードONに変更した場合、最初に現在のクリップボードの内容をFIFOに積む
	if( GetApp().SPI_IsFIFOClipboardMode() == true )
	{
		//テキストをクリップボードから取得
		AText	text;
		AScrapWrapper::GetClipboardTextScrap(text);
		if( text.GetItemCount() > 0 )
		{
			//最初にキューに追加
			EnqueueFIFOClipboard(text);
		}
	}
}

//#929
/**
各段落の最後にペースト
*/
void	AView_Text::PasteAtEndOfEachLine()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//クリップボードテキスト取得
	AText	text;
	AScrapWrapper::GetClipboardTextScrap(text);
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Paste);
	//
	AText	lineText;
	AIndex	pos = 0;
	//開始段落取得
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	AIndex	paraIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
	//各段落ごとループ
	AItemCount	paraCount = GetTextDocumentConst().SPI_GetParagraphCount();
	for( ; paraIndex < paraCount; paraIndex++ )
	{
		//ペーストテキスト全てペースト済みなら終了
		if( pos >= text.GetItemCount() )
		{
			break;
		}
		//ペーストテキストから段落取得
		text.GetLine(pos,lineText);
		//段落最後に挿入
		AIndex	paraStartLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(paraIndex);
		AIndex	paraEndLineIndex = GetTextDocumentConst().SPI_GetNextParagraphStartLineIndex(paraStartLineIndex) -1;
		AIndex	textIndex = GetTextDocumentConst().SPI_GetLineStart(paraEndLineIndex) +
				GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(paraEndLineIndex);
		GetTextDocument().SPI_InsertText(textIndex,lineText);
	}
	//文法認識とローカル範囲設定
	TriggerSyntaxRecognize();
}

void	AView_Text::InputTab( const ABool inInputSpaces )//#249
{
	//#249 if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == false )
	if( inInputSpaces == false )
	{
		AText	text;
		text.Add(kUChar_Tab);
		TextInput(undoTag_Typing,text);
	}
	else
	{
		CTextDrawData	textdrawdata(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
		GetTextDocumentConst().SPI_GetTextDrawDataWithoutStyle(GetCaretTextPoint().y,textdrawdata);
		AItemCount	tabletters = textdrawdata.OriginalTextArray_TabLetters.Get(GetCaretTextPoint().x);
		AText	text;
		text.Add(kUChar_Space);
		tabletters++;
		while( (tabletters%GetTextDocumentConst().SPI_GetTabWidth()) != 0 )
		{
			text.Add(kUChar_Space);
			tabletters++;
		}
		//#318
		if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey) == true &&
					GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kZenkakuSpaceIndent) == true )
		{
			//
			AText	zenkakuSpaceText;
			zenkakuSpaceText.SetZenkakuSpace();
			//
			AText	t;
			//
			AItemCount	zenkakuSpaceCount = text.GetItemCount()/2;
			AItemCount	spaceCount = text.GetItemCount()-zenkakuSpaceCount*2;
			for( AIndex i = 0; i < zenkakuSpaceCount; i++ )
			{
				t.AddText(zenkakuSpaceText);
			}
			for( AIndex i = 0; i < spaceCount; i++ )
			{
				t.Add(kUChar_Space);
			}
			//
			TextInput(undoTag_Typing,t);
		}
		else
		{
			TextInput(undoTag_Typing,text);
		}
	}
}

/*
B0381
タブ・インデント仕様・設計まとめ
2008.1.1

【設定】
以下の２つのモード設定がある。
①正規表現インデント有無
②文法インデントEnable/Disable（＋文法定義使用Enable/Disable）
注意点
・②の設定がEnableの場合は、そちらが優先され、①は無効となる。
・ADocument_Text::SPI_GetIndentAvailable()は、①か②が有効の場合trueとなる。
以下、「インデント設定無し」とは、①も②も無効であることをさす。

【インデントの仕様】
「インデント」の計算方法(DoIndent(), SPI_CalcIndentCount())
　A 文法インデントEnableの場合
　　適当な段落（Global識別子が存在する段落）まで戻ってそこから文法定義に従ってインデント計算
　B 文法インデントDisableの場合
　　B-1 正規表現インデント有りの場合
　　　前の段落の現在のインデント量を基準として、前の段落と現在段落の内容をそれぞれ正規表現と比較して、インデント計算
　　B-2 インデント設定無し
　　　現在段落のインデントが0の場合のみ、前の段落のインデント量に合わせる

※SPI_CalcIndentCount()の引数inUseCurrentParagraphIndentForRegExpをtrueにした場合は、現在段落の現在のインデントを基準に計算するが、
使用しないことにした。（インデントリターンの場合の改行を挿入した段落について、これを適用することにより、フリーなインデントとの共生を図ろうとしたが、
正規表現インデントの機能とうまく適合しない。

【各機能の仕様】
タブ・インデント関連各機能の仕様
①ツールコマンドINDENT-TAB、キーバインド動作keyAction_indenttab
　A インデント設定無しの場合
　　常に「タブ」挿入
　B インデント設定有りの場合
　　B-1 キャレットモードの場合
　　　「タブ」挿入
　　B-2 キャレットモード以外の場合
　　　B-2-1 段落の途中の場合
　　　　「タブ」挿入
　　　B-2-2 段落のはじめの場合
　　　　インデント量が本来のインデント量よりも少なければインデント、そうでなければ「タブ」挿入
要するに、「インデント設定があり、かつ、キャレットが本来のインデントよりも左にあればインデント、それ以外は、タブ」

②ツールコマンドAUTOTAB
設定に関わらず常に前の段落のインデント量に合わせる

③ツールコマンドINDENT、メニューからインデント、文法インデント実行文字の入力
インデント

④ツールコマンドINDENT-RETURN、キーバインド動作keyAction_indentreturn
改行挿入＋改行挿入行と次行のインデント

*/
//ツールコマンドINDENT-TAB（タブキーのデフォルトキーバインド動作）
void	AView_Text::IndentTab( const ABool inInputSpacesForIndent, const ABool inInputSpacesForTab )//#249
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	/*#912 kIndentTabIsAlwaysTabIfIndentOffは削除。V3ではインデントタイプを「自動インデントしない」に設定することで、
	「インデント／タブ」キーを常にタブ動作とすることができる。（厳密には行頭の場合は常に「右シフト」動作となる。）
	//#281 AModePrefDB::kIndentTabIsAlwaysTabIfIndentOffがtrueの場合は「インデント／タブ」は常にタブ動作とする
	//（#255でコメントアウトしたが、既存動作と動作が変わってしまうので、設定により変更できるようにした。デフォルトは既存（2.1.8と同じ）動作）
	if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kIndentTabIsAlwaysTabIfIndentOff) == true )
	{
		//B0381 文法インデントも正規表現インデントも無しの場合は、タブ
		//コメント：2.1.13b2にてモード設定のレイアウト変更を行ったが、正規表現インデントを使用する場合で正規表現設定が無いときも、
		//下記の対象となるので、レイアウトどおりの動作になっていない。ただ、従来動作と変えたくないということもあるので、
		//とりあえず現状通りにしておく。
		if( GetTextDocument().SPI_GetIndentAvailable() == false )
		{
			InputTab(inInputSpacesForTab);//#249
			return;
		}
	}
	*/
	if( IsCaretMode() == false )
	{
		//【選択中の場合】タブ
		InputTab(inInputSpacesForTab);//#249
	}
	else
	{
		//【キャレット表示中の場合】行頭ならばインデント、行頭以外の場合はタブ
		ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
		if(	GetIndentStart(GetCaretTextPoint()) < pos || 
			GetTextDocumentConst().SPI_GetCurrentParagraphStartLineIndex(GetCaretTextPoint().y) != GetCaretTextPoint().y )
		{
			//【行頭ではない場合】タブ
			InputTab(inInputSpacesForTab);//#249
		}
		else
		{
			Indent(inInputSpacesForIndent);//#255
		}
	}
}

//#255
/**
タブキー等によるインデント
*/
void	AView_Text::Indent( const ABool inInputSpacesForIndent )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	/*B0381 if( inAlwaysIndentIfLineStart == false )
	{*/
	//【行頭の場合】キャレット位置がインデント位置より左ならインデント、それ以上なら右シフト(#255)
	CTextDrawData	textdrawdata(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	GetTextDocumentConst().SPI_GetTextDrawDataWithoutStyle(GetCaretTextPoint().y,textdrawdata);
	AItemCount	tabletters = textdrawdata.OriginalTextArray_TabLetters.Get(GetCaretTextPoint().x);
	AItemCount	tmp;
	if( tabletters < GetTextDocument().SPI_CalcIndentCount(GetCaretTextPoint().y,kIndex_Invalid,tmp,/*#650false*/kIndentTriggerType_IndentKey) &&
				GetApp().SPI_GetAutoIndentMode() == true )//#638
	{
		DoIndent(/*#650 false,*/undoTag_Indent,inInputSpacesForIndent,kIndentTriggerType_IndentKey);//#249
	}
	else
	{
		//#255 InputTab(inInputSpacesForTab);//#249
		ShiftRightLeft(true,inInputSpacesForIndent, true);//#255 #1188
	}
	/*B0381 }
	else
	{
	DoIndent(false);
	}*/
	//インデント位置へキャレット移動
	AIndex pos = GetIndentStart(GetCaretTextPoint());
	ATextPoint	textpoint;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(pos,textpoint);
	SetCaretTextPoint(textpoint);
}

//B0381
//ツールコマンドAUTOTAB（常に前の行のインデント量に合わせる）変な機能だがインデント実行するならINDENTがあるので、過去の動作に合わせる
void	AView_Text::AutoTab( const ABool inInputSpaces )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	AIndex	currentLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(
			GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetCaretTextPoint().y));
	AItemCount	indentCount = 0;
	if( currentLineIndex > 0 )
	{
		AIndex	prevLineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(
				GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(currentLineIndex-1));
		indentCount = GetTextDocumentConst().SPI_GetCurrentIndentCount(prevLineIndex);
	}
	AItemCount	insertedCount = GetTextDocument().SPI_Indent(currentLineIndex,indentCount,inInputSpaces,false);//#249
	ATextPoint	pt = GetCaretTextPoint();
	pt.x += insertedCount;
	if( pt.x < 0 )   pt.x = 0;
	if( pt.x > GetTextDocument().SPI_GetLineLengthWithoutLineEnd(pt.y) )   pt.x = GetTextDocument().SPI_GetLineLengthWithoutLineEnd(pt.y);
	SetCaretTextPoint(pt);
}

void	AView_Text::InputCorrespondence()
{
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return;
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	ATextPoint	hitspt, hitept;//R0000
	AText	text;
	if( GetTextDocumentConst().SPI_GetCorrespondText(spt,text,hitspt,hitept) == true )
	{
		TextInput(undoTag_InputCorrespond,text);
		//R0000
		/*NVI_GetOriginOfLocalFrame(mBraceCheckSavedOrigin);
		mBraceCheckSavedCaretTextPoint = GetCaretTextPoint();
		SetSelect(hitspt,hitept);
		AdjustScroll_Center(hitspt);
		mBraceCheckStartTickCount = 50;
		mDisplayingBraceCheck = true;*/
		AText	line, para;
		line.SetFormattedCstring("%d",hitspt.y+1);
		para.SetFormattedCstring("%d",GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(hitspt.y)+1);
		AText	infotext;
		infotext.SetLocalizedText("Correspnd_StartLine");
		infotext.ReplaceParamText('0',line);
		infotext.ReplaceParamText('1',para);
		AText	notificationTitle;
		notificationTitle.SetLocalizedText("Notification_Correspond");
		if( mTextWindowID != kObjectID_Invalid )
		{
			SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
					SPI_SetNotification_General(notificationTitle,infotext,GetEmptyText());
			SPI_ShowNotificationPopupWindow(true);
		}
		
		//インデント実行文字による自動インデント実行
		//（インデントタイプが文法定義インデント、正規表現インデントの場合のみ）
		//#213 対応文字列入力時、インデント実行文字が含まれていたら、インデント実行
		if( (GetTextDocumentConst().SPI_GetIndentType() == kIndentType_SDFIndent || 
				GetTextDocumentConst().SPI_GetIndentType() == kIndentType_RegExpIndent ) && 
				GetApp().SPI_GetAutoIndentMode() == true )//#638
		{
			AText	indentletters;
			GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Text(AModePrefDB::kIndentExeLetters,indentletters);
			for( AIndex i = 0; i < indentletters.GetItemCount(); i++ )
			{
				if( text.IsCharContained(indentletters.Get(i)) == true )
				{
					ABool	inputSpaces = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kInputSpacesByTabKey);//#249
					DoIndent(/*#650 false,*/undoTag_NOP,inputSpaces,kIndentTriggerType_AutoIndentCharacter);//#249 #639 #650 正規表現一致の場合のみインデント適用
					break;
				}
			}
		}
	}
}

//#359
/**
*/
void	AView_Text::SelectBetweenCorrespondTexts()
{
	ATextPoint	cspt, cept;
	SPI_GetSelect(cspt,cept);
	ATextPoint	spt,ept;
	if( GetTextDocumentConst().SPI_GetCorrespondenceSelection(cspt,cept,spt,ept) == false )
	{
		//#419 対応がとれていない場合
		ASoundWrapper::Beep();
	}
	SetSelect(spt,ept);
}

void	AView_Text::SameLetterUp()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.y == 0 )   return;
	ATextPoint	pt;
	pt.x = spt.x;
	pt.y = spt.y-1;
	ATextIndex	pos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(pt);
	AText	ch;
	GetTextDocumentConst().SPI_GetText1CharText(pos,ch);
	TextInput(undoTag_Typing,ch);
}

void	AView_Text::DuplicateLine()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	AIndex	currentpara = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
	ATextIndex	spos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(currentpara);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(currentpara+1);
	AText	text;
	GetTextDocumentConst().SPI_GetText(spos,epos,text);
	ATextIndex	pos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(currentpara);
	ATextPoint	textpoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(pos,textpoint);
	SetCaretTextPoint(textpoint);
	TextInput(undoTag_Typing,text);
}

void	AView_Text::InsertEmptyLine()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	AIndex	currentpara = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y);
	ATextIndex	pos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(currentpara);
	ATextPoint	textpoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(pos,textpoint);
	SetCaretTextPoint(textpoint);
	AText	text;
	text.Add(kUChar_LineEnd);
	TextInput(undoTag_Typing,text);
}

#if 0
//RC2
//識別子情報ウインドウへinKeywordに対応する識別子の情報を設定
void	AView_Text::SetIdInfo( const AText& inKeyword )
{
	AImagePoint	ipt;
	GetImagePointFromTextPoint(GetCaretTextPoint(),ipt);
	ALocalPoint	lpt;
	NVM_GetLocalPointFromImagePoint(ipt,lpt);
	lpt.x = 16;
	AGlobalPoint	gpt;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lpt,gpt);
	ARect	rect = {gpt.x+500,gpt.y-200,gpt.x+800,gpt.y-30};
	ALocalRect	frameRect = {0};
	NVM_GetLocalViewRect(frameRect);
	AGlobalRect	grect = {0};
	NVM_GetWindow().NVI_GetGlobalRectFromControlLocalRect(NVI_GetControlID(),frameRect,grect);
	rect.right = grect.right - 5;
	rect.left = rect.right - 300;
	GetApp().SPI_GetPopupIdInfoWindow().NVI_SetWindowBounds(rect);
	GetApp().SPI_GetPopupIdInfoWindow().NVI_Show(false);
	
	//#725if( mTextWindowID != kObjectID_Invalid )
	{
		//#725 GetApp().SPI_GetIdInfoView().SPI_SetIdInfo(inKeyword,/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID());
		ABool	idExist = GetApp().SPI_UpdateIdInfoWindows(inKeyword,mTextDocumentRef.GetObjectID(),mTextWindowID);//#725
		if( idExist == false )
		{
			GetApp().SPI_GetPopupIdInfoWindow().NVI_Hide();
		}
	}
}
#endif

//
void	AView_Text::SelectBrace()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	ATextPoint	origspt = spt;
	ATextPoint	origept = ept;
	if( GetTextDocumentConst().SPI_GetBraceSelection(spt,ept) == true )
	{
		//Brace内計算が現在の選択範囲と同じ場合は１文字広げて再度実行
		if( spt.x == origspt.x && spt.y == origspt.y && ept.x == origept.x && ept.y == origept.y )
		{
			ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
			ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept);
			if( spos > 0 )   spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
			if( epos < GetTextDocumentConst().SPI_GetTextLength()-1 )   epos = GetTextDocumentConst().SPI_GetNextCharTextIndex(epos);
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,spt);
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,ept);
			if( GetTextDocumentConst().SPI_GetBraceSelection(spt,ept) == true )
			{
				SetSelect(spt,ept);
			}
		}
		else
		{
			SetSelect(spt,ept);
		}
	}
}

//
ABool	AView_Text::DoubleClickBrace( ATextPoint& ioStart, ATextPoint& ioEnd )//#688
{
	//#844 if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kSelectInsideBraceByDoubleClick) == false )   return false; 
	ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioStart);
	ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ioEnd);
	if( epos-spos != 1 )   return false;
	if( GetTextDocumentConst().SPI_IsCodeChar(ioStart) == false )   return false;
	AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(spos);
	ATextPoint	spt, ept;
	if( ch == '{' || ch == '(' || ch == '[' )
		{
			spt = ioEnd;
		}
	else if( ch == '}' || ch == ')' || ch == ']' )
	{
		spt = ioStart;
	}
	else return false;
	ept = spt;
	GetTextDocumentConst().SPI_GetBraceSelection(spt,ept);
	//#1357
	//括弧も含む設定ONなら、括弧も含める
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kSelectBraceItselfByDoubleClick) == true )
	{
		ATextIndex	spos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
		ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept);
		spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
		epos = GetTextDocumentConst().SPI_GetNextCharTextIndex(epos);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,spt,false);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,ept,false);
	}
	//
	ioStart = spt;
	ioEnd = ept;
	return true;
}

//#1406
/**
括弧ハイライト
*/
void	AView_Text::HighlightBrace()
{
	//モード設定OFFならハイライトを消して終了
	if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kHighlightBrace) == false )
	{
		SetHighlightBraceVisible(false);
		return;
	}
	
	ATextPoint	spt = {0}, ept = {0};
	
	//キャレット位置の前の文字（＝括弧文字位置）を取得
	ATextIndex	caretPos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
	ATextIndex	kakkoPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(caretPos);
	if( kakkoPos < 0 || kakkoPos >= GetTextDocumentConst().SPI_GetTextLength() )
	{
		SetHighlightBraceVisible(false);
		return;
	}
	AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(kakkoPos);
	//括弧開始文字ならspt, eptをキャレット位置に設定
	if( ch == '{' || ch == '(' || ch == '[' )
	{
		spt = ept = GetTextDocumentConst().SPI_GetTextPointFromTextIndex(caretPos);
	}
	//括弧終了文字ならspt, eptを括弧文字位置に設定
	else if( ch == '}' || ch == ')' || ch == ']' )
	{
		spt = ept = GetTextDocumentConst().SPI_GetTextPointFromTextIndex(kakkoPos);
	}
	//それ以外の文字ならハイライト消去して終了
	else
	{
		SetHighlightBraceVisible(false);
		return;
	}
	//括弧文字位置がコードでなければハイライト消去して終了
	ATextPoint	kakkoPoint = GetTextDocumentConst().SPI_GetTextPointFromTextIndex(kakkoPos);
	if( GetTextDocumentConst().SPI_IsCodeChar(kakkoPoint) == false )
	{
		SetHighlightBraceVisible(false);
		return;
	}
	
	//括弧範囲取得
	if( GetTextDocumentConst().SPI_GetBraceSelection(spt, ept) == true )
	{
		//開始括弧ハイライト範囲設定
		mBraceHighlightStartTextPoint1 = GetTextDocumentConst().SPI_GetPrevCharTextPoint(spt);
		mBraceHighlightEndTextPoint1 = spt;
		//終了括弧ハイライト範囲設定
		mBraceHighlightStartTextPoint2 = ept;
		mBraceHighlightEndTextPoint2 = GetTextDocumentConst().SPI_GetNextCharTextPoint(ept);
		//ハイライト表示
		SetHighlightBraceVisible(true);
		return;
	}
	//括弧範囲が取得できないとき、バランスがとれていないとき等はハイライト消去して終了
	SetHighlightBraceVisible(false);
}

//#1406
/**
括弧ハイライトを表示・消去する
*/
void	AView_Text::SetHighlightBraceVisible( const ABool inVisible )
{
	if( mBraceHighlighted != inVisible )
	{
		mBraceHighlighted = inVisible;
		//ハイライトする（していた）行を再描画
		SPI_RefreshLines(mBraceHighlightStartTextPoint1.y, mBraceHighlightEndTextPoint1.y);
		SPI_RefreshLines(mBraceHighlightStartTextPoint2.y, mBraceHighlightEndTextPoint2.y);
	}
}


//
void	AView_Text::EVTDO_DoTickTimer()
{
	//#606
	//手のひらツールモード用マウスカーソル設定
	if( IsHandToolMode() == true )
	{
		if( mMouseTrackingMode == kMouseTrackingMode_TenohiraTool )
		{
			//手のひらドラッグ中
			if( ACursorWrapper::GetCurrentCursorType() != kCursorType_ClosedHand )
			{
				ACursorWrapper::SetCursor(kCursorType_ClosedHand);
			}
		}
		else
		{
			//通常時
			//コマンドキーをクリックして手のひらになるのが違和感があるので、コメントアウト
			/*
			if( ACursorWrapper::GetCurrentCursorType() != kCursorType_OpenHand )
			{
				ACursorWrapper::SetCursor(kCursorType_OpenHand);
			}
			*/
		}
	}
	else
	{
		if( ACursorWrapper::GetCurrentCursorType() == kCursorType_OpenHand )
		{
			ACursorWrapper::SetCursor(kCursorType_Arrow);
		}
	}
	//Caret表示予約があれば表示する
	//当初はDoDraw()の最後で実行していたが、改行時にキャレット描画のほうが先行して見えるので、tick timer時、および、text inputイベント実行の最初で行うようにした。
	ShowCaretIfReserved();
	//
	mCaretTickCount++;
	if( mCaretTickCount >= ATimerWrapper::GetCaretTime() )//#688 >から>=へ変更
	{
		if( NVI_IsFocusActive() == true )
		{
			//キャレットブリンク
			BlinkCaret();
		}
		else
		{
			//フォーカスがないときはキャレット非表示 #688
			HideCaret(false,false);
		}
	}
	if( mDisplayingBraceCheck == true )
	{
		mBraceCheckStartTickCount--;
		if( mBraceCheckStartTickCount == 0 )
		{
			RestoreFromBraceCheck();
		}
	}
//#688 #if IMPLEMENTATION_FOR_WINDOWS
	//win 080713
	//スクロールドラッグ中にマウスを止めてもスクロールするようにする
	if( mMouseTrackingMode == kMouseTrackingMode_SingleClick ||
				mMouseTrackingMode == kMouseTrackingMode_DoubleClick || //#688
				mMouseTrackingMode == kMouseTrackingMode_TripleClick || //#688
				mMouseTrackingMode == kMouseTrackingMode_QuadClick )//#688
	{
		DoMouseTrack(mCurrentMouseTrackModifierKeys);
	}
	//#688 #endif
	
	//
	if( mKeyIdleWorkTimer > 0 )
	{
		//
		mKeyIdleWorkTimer--;
		//
		if( mKeyIdleWorkTimer == 0 )
		{
			//
			if( (mKeyIdleWorkType&kKeyIdleWorkType_UpdateLocalRange) != 0 )
			{
				GetTextDocument().SPI_SetCurrentLocalTextPoint(GetCaretTextPoint(),GetObjectID());
			}
			//マウスクリック後によるタイマー経過処理
			if( (mKeyIdleWorkType&kKeyIdleWorkType_MouseClick) != 0 )
			{
				//キーワード情報
				DisplayIdInfo(false,kIndex_Invalid,kIndex_Invalid);
				//ワーズリストへ検索要求
				RequestWordsList();
			}
			//現在単語ハイライト設定
			SetCurrentWordHighlight();
			//#905
			//文法認識とローカル範囲設定
			TriggerSyntaxRecognize();
		}
	}
	else
	{
		//
		mKeyIdleWorkType = kKeyIdleWorkType_None;
		mKeyIdleWorkTimer = 0;
	}
	
	//リアルタイム補完候補表示
	if( mCandidateRequestTimer > 0 )
	{
		//カウントダウン
		mCandidateRequestTimer--;
		//0なら実行
		if( mCandidateRequestTimer == 0 )
		{
			//補完候補要求
			FindCandidateRealtime();
		}
	}
	
	//#81
	//約1分ごとに未保存データチェック
	mAutoSaveUnsavedCheckTickCount++;
	if( mAutoSaveUnsavedCheckTickCount > 1030 )
	{
		GetTextDocument().SPI_AutoSave_Unsaved();
		mAutoSaveUnsavedCheckTickCount = 0;
	}
}

//#867
/**
*/
void	AView_Text::TriggerKeyIdleWork( const AKeyIdleWorkType inWorkType, const ANumber inTimerValue )
{
	mKeyIdleWorkType |= inWorkType;
	mKeyIdleWorkTimer = inTimerValue;
}

/**
*/
void	AView_Text::ResetKeyIdleWorkTimer( const ANumber inTimerValue )
{
	if( mKeyIdleWorkTimer > 0 )
	{
		mKeyIdleWorkTimer = inTimerValue;
	}
}

//
void	AView_Text::RestoreFromBraceCheck()
{
	if( mDisplayingBraceCheck == true )
	{
		SetCaretTextPoint(mBraceCheckSavedCaretTextPoint);
		NVI_ScrollTo(mBraceCheckSavedOrigin);
		mDisplayingBraceCheck = false;
	}
}

//ドキュメントへテキスト挿入
void	AView_Text::DeleteAndInsertTextToDocument( const AText& inText )
{
	if( IsCaretMode() == false )
	{
		DeleteTextFromDocument();
	}
	ATextPoint	insertionPoint = GetCaretTextPoint();
	//テキスト挿入
	InsertTextToDocument(insertionPoint,inText);
}

//ドキュメントへテキスト挿入
void	AView_Text::InsertTextToDocument( const ATextIndex inInsertIndex, const AText& inText, const ABool inDontRedraw )//#846
{
	ATextPoint	insertPoint = {0,0};
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inInsertIndex,insertPoint);
	InsertTextToDocument(insertPoint,inText,inDontRedraw);
}
void	AView_Text::InsertTextToDocument( const ATextPoint& inInsertPoint, const AText& inText, const ABool inDontRedraw )//#846
{
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return;
	//
	//キャレット処理検討
	
	//
	ATextIndex	insertIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(inInsertPoint);//#695
	//テキスト挿入位置にヒントテキストがあれば削除する
	//ただし、補完入力中の場合は、削除しない（ヒントテキストが連続しているばあいに、補完の次の候補に変換する時に、
	//次のヒントテキストが削除されるのを防ぐため。補完入力キーを押したときにInputAbbrev()内で一度RemoveHintText()を実行済み）
	if( mAbbrevInputMode == false )
	{
		RemoveHintText(insertIndex);
	}
	//
	AText	insertText(inText);
	/*
	//分かち書きモード
	ABool	wakachiMode = true;
	if( wakachiMode == true )
	{
		if( insertIndex > 0 && insertText.GetItemCount() > 0 )
		{
			AIndex	prevCharIndex = GetTextDocumentConst().SPI_GetPrevCharTextIndex(insertIndex);
			AUCS4Char	prevChar = GetTextDocumentConst().SPI_GetText1UCS4Char(prevCharIndex);
			AUCS4Char	firstChar = 0;
			insertText.Convert1CharToUCS4(0,firstChar);
			if( AApplication::GetApplication().NVI_GetUnicodeData().IsWideWidth(prevChar) == true && 
				AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabet(firstChar) == true )
			{
				insertText.InsertCstring(0," ");
			}
		}
		if( insertIndex < GetTextDocumentConst().SPI_GetTextLength() && insertText.GetItemCount() > 0 )
		{
			AIndex	nextCharIndex = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(insertIndex);
			AUCS4Char	nextChar = GetTextDocumentConst().SPI_GetText1UCS4Char(nextCharIndex);
			AUCS4Char	lastChar = 0;
			insertText.Convert1CharToUCS4(insertText.GetPrevCharPos(insertText.GetItemCount()-1),lastChar);
			if( AApplication::GetApplication().NVI_GetUnicodeData().IsWideWidth(nextChar) == true && 
				AApplication::GetApplication().NVI_GetUnicodeData().IsAlphabet(lastChar) == true )
			{
				insertText.AddCstring(" ");
			}
		}
	}
	*/
	//
	AItemCount	insertedLineCount = GetTextDocument().SPI_InsertText(/*#695 inInsertIndex*/inInsertPoint,insertText);
	//キャレット設定
	/*#695
	ATextPoint	pt;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertIndex+insertText.GetItemCount(),pt);
	SetCaretTextPoint(pt);
	*/
	//#846 置換時等、redrawしないようにする
	if( inDontRedraw == false )
	{
		//挿入テキストの直後にキャレット移動（ただしスレッドで行計算中は、SetSelect()内で移動リザーブされる）#695
		SetSelect(insertIndex+insertText.GetItemCount(),insertIndex+insertText.GetItemCount());
		//
		AdjustScroll(GetCaretTextPoint());
	}
	//#567 LuaConsole コマンド実行処理 #1170
	if( GetTextDocumentConst().SPI_GetLuaConsoleMode() == true && insertedLineCount > 0 )
	{
		AIndex	promptTextIndex = GetTextDocumentConst().SPI_GetLuaPromptTextIndex();
		if( insertIndex >= promptTextIndex )
		{
			AText	luaCommand;
			GetTextDocument().SPI_GetText(promptTextIndex,insertIndex+insertText.GetItemCount(),luaCommand);
			AText	errorText;
			if( GetApp().NVI_Lua_dostring(luaCommand,errorText) != 0 )
			{
				GetTextDocument().SPI_Lua_Output(errorText);
			}
			GetTextDocument().SPI_Lua_Prompt();
		}
	}
	
	if( insertedLineCount > 0 )
	{
		//補完入力状態解除 #717
		ClearAbbrevInputMode(true,false);
	}
}

//ドキュメントからテキスト削除
void	AView_Text::DeleteTextFromDocument( const ATextIndex inStart, const ATextIndex inEnd, const ABool inDontRedraw,
											const ABool inStoreToKillBuffer )
{
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return;
	
	//#913
	//inStoreToKillBufferがtrueの場合は、キルバッファーに保存する
	if( inStoreToKillBuffer == true )
	{
		ATextPoint	spt = {0};
		GetTextDocument().SPI_GetTextPointFromTextIndex(inStart,spt);
		AText	deletedText;
		GetTextDocumentConst().SPI_GetText(inStart,inEnd,deletedText);
		GetApp().SPI_AddToKillBuffer(GetTextDocumentConst().GetObjectID(),spt,deletedText);
	}
	
	//
	GetTextDocument().SPI_DeleteText(inStart,inEnd,false,inDontRedraw);
	//#846 置換時等、redrawしないようにする
	if( inDontRedraw == false )
	{
		//キャレット設定
		ATextPoint	pt;
		GetTextDocument().SPI_GetTextPointFromTextIndex(inStart,pt);
		SetCaretTextPoint(pt);
		//
		AdjustScroll(GetCaretTextPoint());
	}
	
}
void	AView_Text::DeleteTextFromDocument( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inDontRedraw,
										   const ABool inStoreToKillBuffer )
{
	DeleteTextFromDocument(GetTextDocument().SPI_GetTextIndexFromTextPoint(inStartPoint),GetTextDocument().SPI_GetTextIndexFromTextPoint(inEndPoint),
			inDontRedraw,inStoreToKillBuffer);
}
//ドキュメントからテキスト削除（選択範囲）
void	AView_Text::DeleteTextFromDocument( const ABool inStoreToKillBuffer )
{
	if( GetTextDocumentConst().NVI_IsReadOnly() == true )   return;
	if( mKukeiSelected == true )
	{
		for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
		{
			ATextPoint	spt = mKukeiSelected_Start.Get(i);
			ATextPoint	ept = mKukeiSelected_End.Get(i);
			AIndex	start = GetTextDocument().SPI_GetTextIndexFromTextPoint(spt);
			AIndex	end = GetTextDocument().SPI_GetTextIndexFromTextPoint(ept);
			GetTextDocument().SPI_DeleteText(start,end);
		}
		//キャレット設定
		SetCaretTextPoint(GetCaretTextPoint());
		//
		AdjustScroll(GetCaretTextPoint());
	}
	else
	{
		ATextPoint	spt, ept;
		SPI_GetSelect(spt,ept);
		DeleteTextFromDocument(spt,ept,false,inStoreToKillBuffer);
	}
}

/**
ヒントテキストがあれば削除する
*/
void	AView_Text::RemoveHintText( const ATextIndex inTextIndex )
{
	if( GetTextDocumentConst().SPI_FindHintIndexFromTextIndex(inTextIndex) != kIndex_Invalid )
	{
		if( GetTextDocumentConst().SPI_GetTextChar(inTextIndex) == kHintTextChar )
		{
			GetTextDocument().SPI_DeleteText(inTextIndex,inTextIndex+1);
		}
	}
}

//Drag&Drop処理（Drag中）
//#1034 XOR描画処理内でCGContextFlushを実行するようにしたので、drag中にちらつくようになったので、差分がある場合のみ描画するようにした
void	AView_Text::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )//#688
{
	//表示フレームをImage座標で取得
	AImageRect	frameImageRect;
	NVM_GetImageViewRect(frameImageRect);
	//
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	//scroll
	{
		//Windows等の場合、イベントドリブンでドラッグスクロールさせる
		if( clickImagePoint.y < frameImageRect.top+kDragDropScrollRange && 
					frameImageRect.top > 0 )
		{
			//一回だけスクロールし、ここでリターン
			DragScrollV(clickImagePoint.y-(frameImageRect.top+kDragDropScrollRange),inModifierKeys);
			return;
		}
		else if( clickImagePoint.y > frameImageRect.bottom-kDragDropScrollRange && 
					frameImageRect.bottom < NVM_GetImageHeight() )
		{
			//一回だけスクロールし、ここでリターン
			DragScrollV(clickImagePoint.y-(frameImageRect.bottom-kDragDropScrollRange),inModifierKeys);
			return;
		}
	}
	//
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	if( IsInSelect(clickImagePoint,false) == false )
	{
		// -------------------- 選択範囲外へドラッグした場合 --------------------
		
		//新規Dragキャレット情報格納用変数 #1034
		ATextPoint	newDragCaretTextPoint = {0}, newDragSelectTextPoint = {0};
		ABool	newDragWordDragDrop = false;
		
		//単語置き換えDrag&Drop
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true && mKukeiSelected == false )
		{
			ATextIndex	clickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			ATextIndex	wordStart, wordEnd;
			GetTextDocument().SPI_FindWord(clickTextIndex,wordStart,wordEnd);
			if( wordStart != wordEnd )
			{
				//Dragキャレット位置取得
				GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,newDragCaretTextPoint,true);
				GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,newDragSelectTextPoint,false);
				//単語置き換えモード設定
				newDragWordDragDrop = true;
			}
			else
			{
				//Dragキャレット位置取得
				newDragCaretTextPoint = clickTextPoint;
				newDragSelectTextPoint = mDragSelectTextPoint;
			}
		}
		//通常Drag&Drop
		else
		{
			//Dragキャレット位置取得
			newDragCaretTextPoint = clickTextPoint;
			newDragSelectTextPoint = mDragSelectTextPoint;
		}
		//Dragキャレット情報に差分があるかどうかの判定 #1034
		//差分があれば、描画を行う
		if( newDragWordDragDrop != mDragWordDragDrop || 
			newDragCaretTextPoint.x != mDragCaretTextPoint.x || newDragCaretTextPoint.y != mDragCaretTextPoint.y ||
			newDragSelectTextPoint.x != mDragSelectTextPoint.x || newDragSelectTextPoint.y != mDragSelectTextPoint.y )
		{
			// -------------------- Dragキャレット消去 --------------------
			if( mDragDrawn == true )
			{
				//Erase
				XorDragCaret();
				mDragDrawn = false;
			}
			if( mDragWordDragDrop == true )
			{
				//Erase
				XorWordDragDrop();
				mDragWordDragDrop = false;
			}
			// -------------------- Dragキャレット情報設定 --------------------
			mDragWordDragDrop = newDragWordDragDrop;
			mDragCaretTextPoint = newDragCaretTextPoint;
			mDragSelectTextPoint = newDragSelectTextPoint;
			// -------------------- Dragキャレット描画 --------------------
			if( mDragWordDragDrop == true )
			{
				//Draw
				mDragWordDragDrop = false;
				XorWordDragDrop();
				mDragWordDragDrop = true;
			}
			else
			{
				//Draw
				mDragDrawn = false;
				XorDragCaret();
				mDragDrawn = true;
			}
		}
	}
	else
	{
		// -------------------- 選択範囲内へドラッグした場合 --------------------
		//Dragキャレットを消去
		if( mDragDrawn == true )
		{
			//Erase
			XorDragCaret();
			mDragDrawn = false;
		}
		if( mDragWordDragDrop == true )
		{
			//Erase
			XorWordDragDrop();
			mDragWordDragDrop = false;
		}
	}
	//#859
	if( AScrapWrapper::ExistDragData(inDragRef,kScrapType_TabSelector) == true )
	{
		if( mTextWindowID != kObjectID_Invalid )
		{
			if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsSubTextView(NVI_GetControlID()) == true )
			{
				//#688 ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
				outIsCopyOperation = true;
			}
		}
	}
	//
	else if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true ||
				AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true      ||//win 080709
				mDragging == false )//B0347
	{
		//#688 ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
		outIsCopyOperation = true;
	}
	else
	{
		//#688 ACursorWrapper::SetCursor(kCursorType_Arrow);
		outIsCopyOperation = false;
	}
}

//Drag&Drop処理（DragがViewに入った）
void	AView_Text::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	GetTextPointFromImagePoint(clickImagePoint,mDragCaretTextPoint);
	if( IsInSelect(clickImagePoint,false) == false )
	{
		//Draw
		mDragDrawn = false;
		XorDragCaret();
		mDragDrawn = true;
	}
}

//Drag&Drop処理（DragがViewから出た）
void	AView_Text::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	if( mDragDrawn == true )
	{
		//Erase
		XorDragCaret();
		mDragDrawn = false;
	}
	if( mDragWordDragDrop == true )
	{
		//Erase
		XorWordDragDrop();
		mDragWordDragDrop = false;
	}
}

//Drag&Drop処理（Drop）
void	AView_Text::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	//DragCaret消去
	if( mDragDrawn == true )
	{
		//Erase
		XorDragCaret();
		mDragDrawn = false;
	}
	if( mDragWordDragDrop == true )
	{
		//Erase
		XorWordDragDrop();
		mDragWordDragDrop = false;
	}
	if( IsInSelect(clickImagePoint,false) == true )   return;
	
	/*#182
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_FixInlineInput();//B0079
	}
	*/
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());//#688
	
	//URL Drop
	AText	url, title;
	if( AScrapWrapper::GetURLDragData(inDragRef,url,title) == true )
	{
		//DragCaret位置をキャレット位置として設定
		SetCaretTextPoint(clickTextPoint);
		//
		AText	text;
		AIndex	caretIndex = kIndex_Invalid;
		AFileAcc	tmp;
		MakeDropText(tmp,url,title,text,caretIndex);
		TextInput(undoTag_DragDrop,text);
		if( caretIndex != kIndex_Invalid )
		{
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			textindex += caretIndex;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,clickTextPoint);
			SetCaretTextPoint(clickTextPoint);
		}
		return;
	}
	
	//#859
	//タブからのdrag&drop
	AText	docUniqIDText;
	AScrapWrapper::GetDragData(inDragRef,kScrapType_TabSelector,docUniqIDText);//tab index取得
	if( docUniqIDText.GetItemCount() > 0 && mTextWindowID != kObjectID_Invalid )
	{
		AUniqID	uniqID = kUniqID_Invalid;//#850
		uniqID.val = docUniqIDText.GetNumber();
		ADocumentID	docID = GetApp().NVI_GetDocumentIDByUniqID(uniqID);//#850
		//
		if( NVM_GetWindow().NVI_GetTabIndexByDocumentID(docID) == kIndex_Invalid )
		{
			//-----------------ドラッグ中のドキュメントがこのウインドウ内で表示されていない場合-----------------
			
			//ドラッグ中のタブをこのウインドウへ移動する
			
			//ドラッグ中のタブのウインドウ、タブを取得
			AWindowID	srcWindowID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			AIndex	srcTabIndex = GetApp().NVI_GetWindowByID(srcWindowID).NVI_GetTabIndexByDocumentID(docID);
			//ドラッグ先へタブコピー
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPNVI_CopyCreateTab(srcWindowID, srcTabIndex);
		}
		else
		{
			//-----------------ドラッグ中のドキュメントがこのウインドウ内で表示されている場合-----------------
			
			//サブテキストへのdropの場合のみ処理する。
			if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsSubTextView(NVI_GetControlID()) == true )
			{
				if( docID != kObjectID_Invalid )
				{
					//#850
					AIndex	tabIndex = NVM_GetWindow().NVI_GetTabIndexByDocumentID(docID);
					//サブテキストへ表示
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_DisplayTabInSubText(tabIndex,false);
				}
			}
		}
	}
	
	/*IdInfoを別ビューにドラッグしたらそのビューで開く機能→ひとまずdrop
	//#853
	//キーワード情報からのdrag&drop
	AText	openPosText;
	AScrapWrapper::GetDragData(inDragRef,kScrapType_IdInfoItem,openPosText);
	if( openPosText.GetItemCount() > 0 && mTextWindowID != kObjectID_Invalid )
	{
		ATextArray	ta;
		ta.ExplodeFromText(openPosText,kUChar_Tab);
		AText	filepath, sposText, eposText;
		ta.Get(0,filepath);
		ta.Get(1,sposText);
		ta.Get(2,eposText);
		ANumber	spos = sposText.GetNumber();
		ANumber	epos = eposText.GetNumber();
		GetApp().SPI_OpenOrRevealTextDocument(filepath,spos,epos,
					mTextWindowID,GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsSubTextView(NVI_GetControlID()),
					kAdjustScrollType_Top);
	}
	*/
	
	AText	text;
	AScrapWrapper::GetTextDragData(inDragRef,text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding());//win
	if( text.GetItemCount() > 0 )//B0348
	{
		//
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true && mKukeiSelected == false )//B0347 ifブロック順序入れ替え（shift優先）
		{
			ATextIndex	clickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			ATextIndex	wordStart, wordEnd;
			GetTextDocument().SPI_FindWord(clickTextIndex,wordStart,wordEnd);
			ATextPoint	wordStartPoint, wordEndPoint;
			GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,wordStartPoint,true);
			GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,wordEndPoint,false);
			//
			SetSelect(wordStartPoint,wordEndPoint);
			//
			TextInput(undoTag_DragDrop,text);
			//
			ATextPoint	ept;
			GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart+text.GetItemCount(),ept,false);
			//#231 すでにwordStartPointは正しくない可能性がある（行折り返しありの場合、文字挿入後は、挿入文字列開始位置が前行に移動する）
			ATextPoint	spt;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(wordStart,spt);
			SetSelect(/*#231 wordStartPoint*/spt,ept);
			
		}
		else if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || 
					AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true ||//win 080709
					mDragging == false )//B0347 自ビューでDrag中でなければ常にコピー動作
		{
			ABool	svKukeiSelected = mKukeiSelected;
			//DragCaret位置をキャレット位置として設定
			SetCaretTextPoint(clickTextPoint);
			//#231
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			//
			if( svKukeiSelected == false )
			{
				TextInput(undoTag_DragDrop,text);
				//
				ATextPoint	ept = GetCaretTextPoint();
				//#231 すでにclickTextPointは正しくない可能性がある（行折り返しありの場合、文字挿入後は、挿入文字列開始位置が前行に移動する）
				ATextPoint	spt;
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,spt);
				SetSelect(/*#231 clickTextPoint*/spt,ept);
			}
			else
			{
				//Undoアクションタグ記録
				GetTextDocument().SPI_RecordUndoActionTag(undoTag_DragDrop);
				//
				KukeiInput(text);
			}
		}
		else
		{
			//移動Drag
			ABool	svKukeiSelected = mKukeiSelected;
			//移動先TextIndexを保存
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			//
			ATextIndex	spos, epos;
			GetSelect(spos,epos);
			
			//==================Samrt drag==================
			//#888 アルファベット判定処理修正
			if( GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetModeData_Bool(AModePrefDB::kAddRemoveSpaceByDragDrop) == true &&
						text.GetItemCount() > 0 && //#888
						mKukeiSelected == false )
			{
				//==================ドロップ先のスペース調整==================
				
				//移動先の前後の文字がアルファベットなら、dropする文字列にスペース文字を追加
				if( textindex > 0 )
				{
					//------------------挿入するテキストの最初がアルファベット、かつ、ドロップ先の前の文字がアルファベットなら、挿入テキストの最初にスペース追加------------------
					
					//挿入テキストの最初の文字取得
					AUCS4Char	ucs4char = 0;
					text.Convert1CharToUCS4(0,ucs4char);
					//挿入テキストの最初の文字のアルファベット判定
					if( unicodeData.IsAlphabetOrNumber(ucs4char) == true )
					{
						//ドロップ先の前の文字を取得
						ATextIndex	prevtextindex = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textindex);
						ucs4char = GetTextDocumentConst().SPI_GetText1UCS4Char(prevtextindex);
						//ドロップ先の前の文字のアルファベット判定
						if( unicodeData.IsAlphabetOrNumber(ucs4char) == true )
						{
							text.Insert1(0,kUChar_Space);
						}
					}
				}
				if( textindex < GetTextDocumentConst().SPI_GetTextLength() )
				{
					//------------------挿入するテキストの最後がアルファベット、かつ、ドロップ先の後の文字がアルファベットなら、挿入テキストの最初にスペース追加------------------
					
					//挿入テキストの最後の文字取得
					AUCS4Char	ucs4char = 0;
					text.Convert1CharToUCS4(text.GetPrevCharPos(text.GetItemCount()),ucs4char);
					//挿入テキストの最後の文字のアルファベット判定
					if( unicodeData.IsAlphabetOrNumber(ucs4char) == true )
					{
						//ドロップ先の後の文字を取得
						ucs4char = GetTextDocumentConst().SPI_GetText1UCS4Char(textindex);
						//ドロップ先の後の文字のアルファベット判定
						if( unicodeData.IsAlphabetOrNumber(ucs4char) == true )
						{
							//スペース追加
							text.Add(kUChar_Space);
						}
					}
				}
				
				//==================ドラッグ元のスペース調整（前後のスペース削除）==================
				ABool	smart1 = false, smart2 = false;
				ABool	del1 = false, del2 = false;
				if( spos >= 1 )
				{
					//ドラッグ元テキストの直前の文字を取得
					AUCS4Char	ucs4char = 0;
					ATextIndex	prevtextindex = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
					ucs4char = GetTextDocumentConst().SPI_GetText1UCS4Char(prevtextindex);
					//ドラッグ元テキストの直前の文字のアルファベット判定
					if( unicodeData.IsAlphabetOrNumber(ucs4char) == false )
					{
						smart1 = true;
						if( GetTextDocumentConst().SPI_GetTextChar(spos-1) == kUChar_Space )   del1 = true;
					}
				}
				if( epos < GetTextDocumentConst().SPI_GetTextLength() )
				{
					//ドラッグ元テキストの直後の文字を取得
					AUCS4Char	ucs4char = 0;
					ucs4char = GetTextDocumentConst().SPI_GetText1UCS4Char(epos);
					//ドラッグ元テキストの直後の文字のアルファベット判定
					if( unicodeData.IsAlphabetOrNumber(ucs4char) == false )
					{
						smart2 = true;
						if( GetTextDocumentConst().SPI_GetTextChar(epos) == kUChar_Space )   del2 = true;
					}
				}
				if( smart1 == true && smart2 == true )
				{
					if( del1 == true )   spos--;
					else if( del2 == true )   epos++;
					SetSelect(spos,epos);
				}
			}
			//削除後のTextIndexを計算（移動先が、現在の選択範囲以降の場合、削除するとその分、前に移動しなくてはいけない）
			ATextIndex	newtextindex = textindex;
			if( textindex >= epos )
			{
				newtextindex = textindex - (epos-spos);
			}
			//Undoアクションタグ記録
			GetTextDocument().SPI_RecordUndoActionTag(undoTag_DragDrop);
			//削除
			DeleteTextFromDocument(false);
			//DragCaret位置をキャレット位置として設定
			ATextPoint	textpoint;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(newtextindex,textpoint);
			SetCaretTextPoint(textpoint);
			//
			if( svKukeiSelected == false )
			{
				//テキスト挿入
				InsertTextToDocument(textpoint,text);
				//
				ATextPoint	ept = GetCaretTextPoint();
				//#231 すでにtextpointは正しくない可能性がある（行折り返しありの場合、文字挿入後は、挿入文字列開始位置が前行に移動する）
				ATextPoint	spt;
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(newtextindex,spt);
				SetSelect(/*#231 textpoint*/spt,ept);
			}
			else
			{
				//
				KukeiInput(text);
			}
		}
	}
	//B0348 ファイルDrop判定は最後にする（text clippingファイルをDropした場合、テキストが入っているのでそちら優先）
	else
	{
		//ファイルDrop
		//#384 AFileAcc	file;
		//#384 if( AScrapWrapper::GetFileDragData(inDragRef,file) == true )
		AObjectArray<AFileAcc>	fileArray;//#384
		AScrapWrapper::GetFileDragData(inDragRef,fileArray);//#384
		if( fileArray.GetItemCount() > 0 )//#384
		{
			if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || 
						AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )//win 080709
			{
				//#384 複数のファイルを同時に開く
				for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
				{
					GetApp().SPNVI_CreateDocumentFromLocalFile(fileArray.GetObjectConst(i));
				}
				return;
			}
			else
			{
				//DragCaret位置をキャレット位置として設定
				SetCaretTextPoint(clickTextPoint);
				//
				AIndex	lastCaretTextIndex = kIndex_Invalid;//#788
				//#384 パス挿入等は最初のファイルを対象
				//#788 ファイルDrag&Dropのパス挿入も、複数ファイルに対応する。
				for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
				{
					AFileAcc	file;
					file.CopyFrom(fileArray.GetObjectConst(i));//#788 0));
					
					//
					AText	text;
					AIndex	caretIndex = kIndex_Invalid;
					/*#329 
					if( MakeDropText(file,GetEmptyText(),GetEmptyText(),text,caretIndex) == false &&
					GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Number(AModePrefDB::kDragDrop_ActionForNoSuffix) == 0 )
					*/
					if( MakeDropText(file,GetEmptyText(),GetEmptyText(),text,caretIndex) == false )
					{
						switch( GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Number(AModePrefDB::kDragDrop_ActionForNoSuffix) )
						{
						  case 0://ファイルを開く
							{
								//#91
								//#384 複数のファイルを同時に開く
								//#788 for( AIndex i = 0; i < fileArray.GetItemCount(); i++ )
								//#788 {
								GetApp().SPNVI_CreateDocumentFromLocalFile(file);//#788 fileArray.GetObjectConst(i));
								//#788 }
								//#788 return;
								continue;//#788
							}
						  case 1://絶対パス
							{
								file.GetPath(text);
								break;
							}
						  case 2://相対パス
							{
								AFileAcc	docFile;
								if( GetTextDocumentConst().NVI_GetFile(docFile) == true )
								{
									file.GetPartialPath(docFile,text);
								}
								else
								{
									file.GetPath(text);
								}
								break;
							}
						  default:
							{
								//処理なし
								break;
							}
						}
					}
					//#788
					AIndex	caretTextIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
					//
					TextInput(undoTag_DragDrop,text);
					//
					if( caretIndex != kIndex_Invalid )
					{
						/*#788
						ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
						textindex += caretIndex;
						GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,clickTextPoint);
						SetCaretTextPoint(clickTextPoint);
						*/
						lastCaretTextIndex = caretTextIndex + caretIndex;
					}
				}
				//#788
				if( lastCaretTextIndex != kIndex_Invalid )
				{
					ATextPoint	caretTextPoint = kTextPoint_00;
					GetTextDocumentConst().SPI_GetTextPointFromTextIndex(lastCaretTextIndex,caretTextPoint);
					SetCaretTextPoint(caretTextPoint);
				}
			}
		}
  }
	//#905
	//文法認識とローカル範囲設定
	TriggerSyntaxRecognize();
}

/**
ドラッグ中のドラッグ先キャレット描画
*/
void	AView_Text::XorDragCaret()
{
	//画面描画指示 #1332 マウスドラッグ中にlockFocus描画反映されないようなので、drawRect()ルートで描画する。
	NVMC_RefreshControl();
	/*#1332
	AImagePoint	caretImageStart, caretImageEnd;
	GetImagePointFromTextPoint(mDragCaretTextPoint,caretImageStart);
	caretImageEnd = caretImageStart;
	caretImageEnd.y += GetLineHeightWithMargin(mDragCaretTextPoint.y);//#450 GetLineHeightWithMargin();
	ALocalPoint	caretLocalStart, caretLocalEnd;
	NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
	NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
	//キャレット色取得（文字色と同じにする）
	AColor	modeLetterColor = kColor_Black;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Color(AModePrefDB::kLetterColor,modeLetterColor);
	//キャレット太さ判定
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kBigCaret) == false )//#722
	{
		//太さ1
		NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true,false,1);//#1034 常にflushする
	}
	//#722 drag時のキャレット太さを通常と同じにする
	else
	{
		//太さ2
		caretLocalStart.y -= 1;
		caretLocalEnd.y += 1;
		NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true,false,2);//#1034 常にflushする
	}
	*/
}

/**
ドラッグ中のワードドラッグ先キャレット描画
*/
void	AView_Text::XorWordDragDrop()
{
	//画面描画指示 #1332 マウスドラッグ中にlockFocus描画反映されないようなので、drawRect()ルートで描画する。
	NVMC_RefreshControl();
	/*#1332
	///キャレット色取得（文字色と同じにする）
	AColor	modeLetterColor = kColor_Black;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Color(AModePrefDB::kLetterColor,modeLetterColor);
	//
	AImageRect	imageRect;
	GetLineImageRect(mDragCaretTextPoint.y,imageRect);
	AImagePoint	caretImagePoint, selectImagePoint;
	GetImagePointFromTextPoint(mDragCaretTextPoint,caretImagePoint);
	GetImagePointFromTextPoint(mDragSelectTextPoint,selectImagePoint);
	imageRect.left	= caretImagePoint.x;
	imageRect.right	= selectImagePoint.x;
	ALocalRect	localRect;
	NVM_GetLocalRectFromImageRect(imageRect,localRect);
	NVMC_DrawXorCaretRect(localRect,true,true);//#1034 常にflushする
	*/
}

/**
ツール入力モード開始（コマンド＋Y押下時処理）
*/
void	AView_Text::StartKeyToolMode()
{
	mKeyToolMode = true;
	//キャレット位置の文法パート取得
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(mCaretTextPoint);
	AIndex	syntaxPartIndex = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetStateSyntaxPartIndex();
	//ユーザーツールのmenu object idと開始位置、終了位置を取得
	GetModePrefDB().GetKeyToolStartMenu(syntaxPartIndex,true,mKeyToolMenuObjectID_UserTool,
										mKeyToolMenuStartIndex_UserTool,mKeyToolMenuEndIndex_UserTool);
	//アプリ組み込みツールのmenu object idと開始位置、終了位置を取得
	GetModePrefDB().GetKeyToolStartMenu(syntaxPartIndex,false,mKeyToolMenuObjectID_AppTool,
										mKeyToolMenuStartIndex_AppTool,mKeyToolMenuEndIndex_AppTool);
	//キーツールリストウインドウ更新
	UpdateKeyToolListWindow();
}

/**
ツール入力モードでのキー入力処理
*/
ABool	AView_Text::KeyTool( const AText& inText )
{
	if( mKeyToolMode == false )   return false;
	if( inText.GetItemCount() == 0 )   return false;
	
	//==================入力キー取得==================
	AText	t;
	t.SetText(inText);
	t.ChangeCaseUpper();
	AUChar	keych = t.Get(0);
	
	//==================ユーザーツール検索==================
	ABool	foundFileInUserTool = false;
	ABool	foundFolderInUserTool = false;
	AFileAcc	userToolFile;
	if( mKeyToolMenuObjectID_UserTool != kObjectID_Invalid )
	{
		//
		for( AIndex i = mKeyToolMenuStartIndex_UserTool; i < mKeyToolMenuEndIndex_UserTool; i++ )
		{
			//メニューテキストの最初の一文字を取得
			AText	menutext;
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mKeyToolMenuObjectID_UserTool,i,menutext);
			if( menutext.GetItemCount() < 2 )   continue;
			AText	t;
			t.SetText(menutext);
			t.ChangeCaseUpper();
			AUChar	ch = t.Get(0);
			AUChar	ch2 = t.Get(1);
			if( ch == keych && ch2 == kUChar_Space )
			{
				//------------------入力キーと一致していた場合------------------
				AObjectID	subMenuObjectID = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemSubMenuObjectIDByObjectID(mKeyToolMenuObjectID_UserTool,i);
				if( subMenuObjectID != kObjectID_Invalid )
				{
					//------------------サブメニュー有り------------------
					//フォルダ発見、ループ終了
					foundFolderInUserTool = true;
					mKeyToolMenuObjectID_UserTool = subMenuObjectID;
					mKeyToolMenuStartIndex_UserTool = 0;
					mKeyToolMenuEndIndex_UserTool = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemCountByObjectID(mKeyToolMenuObjectID_UserTool);
					break;
				}
				else
				{
					//------------------サブメニュー無し------------------
					//ファイル発見、ループ終了
					foundFileInUserTool = true;
					AText	actiontext;
					GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mKeyToolMenuObjectID_UserTool,i,actiontext);
					userToolFile.Specify(actiontext);
					break;
				}
			}
		}
	}
	if( foundFolderInUserTool == false )
	{
		mKeyToolMenuObjectID_UserTool = kObjectID_Invalid;
	}
	//==================アプリ内ツール検索==================
	ABool	foundFileInAppTool = false;
	ABool	foundFolderInAppTool = false;
	AFileAcc	appToolFile;
	if( mKeyToolMenuObjectID_AppTool != kObjectID_Invalid )
	{
		//
		for( AIndex i = mKeyToolMenuStartIndex_AppTool; i < mKeyToolMenuEndIndex_AppTool; i++ )
		{
			//メニューテキストの最初の一文字を取得
			AText	menutext;
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mKeyToolMenuObjectID_AppTool,i,menutext);
			if( menutext.GetItemCount() < 2 )   continue;
			AText	t;
			t.SetText(menutext);
			t.ChangeCaseUpper();
			AUChar	ch = t.Get(0);
			AUChar	ch2 = t.Get(1);
			if( ch == keych && ch2 == kUChar_Space )
			{
				//------------------入力キーと一致していた場合------------------
				AObjectID	subMenuObjectID = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemSubMenuObjectIDByObjectID(mKeyToolMenuObjectID_AppTool,i);
				if( subMenuObjectID != kObjectID_Invalid )
				{
					//------------------サブメニュー有り------------------
					//フォルダ発見、ループ終了
					foundFolderInAppTool = true;
					mKeyToolMenuObjectID_AppTool = subMenuObjectID;
					mKeyToolMenuStartIndex_AppTool = 0;
					mKeyToolMenuEndIndex_AppTool = GetApp().NVI_GetMenuManager().
							GetDynamicMenuItemCountByObjectID(mKeyToolMenuObjectID_AppTool);
					break;
				}
				else
				{
					//------------------サブメニュー無し------------------
					//ファイル発見、ループ終了
					foundFileInAppTool = true;
					AText	actiontext;
					GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(mKeyToolMenuObjectID_AppTool,i,actiontext);
					appToolFile.Specify(actiontext);
					break;
				}
			}
		}
	}
	if( foundFolderInAppTool == false )
	{
		mKeyToolMenuObjectID_AppTool = kObjectID_Invalid;
	}
	
	//==================ユーザーツール選択時処理==================
	if( foundFileInUserTool == true )
	{
		SPI_DoTool(userToolFile,0,false);
		EndKeyToolMode();
		return true;
	}
	//==================アプリ内組み込みツール選択時処理==================
	if( foundFileInAppTool == true )
	{
		SPI_DoTool(appToolFile,0,false);
		EndKeyToolMode();
		return true;
	}
	//==================フォルダ選択時処理==================
	if( foundFolderInUserTool == true || foundFolderInAppTool == true )
	{
		UpdateKeyToolListWindow();
		return true;
	}
	//==================どれにも一致しなかった場合==================
	EndKeyToolMode();
	return true;//#499 入力エラー時の文字はTextInputしないfalse;
}

//R0073
void	AView_Text::EndKeyToolMode()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetPopupKeyToolListWindow().NVI_Hide();
	}
	mKeyToolMode = false;
}

//
void	AView_Text::UpdateKeyToolListWindow()
{
	//キー入力モード 選択リスト作成
	AHashTextArray	textArray;
	ABoolArray	isUserToolArray;
	//==================アプリ内ツールから作成==================
	if( mKeyToolMenuObjectID_AppTool != kObjectID_Invalid )
	{
		for( AIndex i = mKeyToolMenuStartIndex_AppTool; i < mKeyToolMenuEndIndex_AppTool; i++ )
		{
			//メニューテキストの最初の一文字取得
			AText	menutext;
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mKeyToolMenuObjectID_AppTool,i,menutext);
			if( menutext.GetItemCount() < 2 )   continue;
			AText	t;
			t.SetText(menutext);
			t.ChangeCaseUpper();
			AUChar	ch = t.Get(0);
			AUChar	ch2 = t.Get(1);
			AText	text;
			if( ch >= 0x80 || ch2 != kUChar_Space )
			{
				//アスキー文字＋スペース以外は何もしない
				continue;
			}
			else
			{
				//リストに追加
				text.Add(ch);
				text.Add(kUChar_Colon);
				menutext.Delete1(0);
				text.AddText(menutext);
			}
			//サブメニュー有りなら右三角文字追加
			if( GetApp().NVI_GetMenuManager().GetDynamicMenuItemSubMenuObjectIDByObjectID(mKeyToolMenuObjectID_AppTool,i) != kObjectID_Invalid )
			{
				text.AddCstring(" ");
				AText	existSubMenuText;
				existSubMenuText.SetLocalizedText("ExistSubMenu");
				text.AddText(existSubMenuText);
			}
			//項目追加
			textArray.Add(text);
			isUserToolArray.Add(false);
		}
	}
	/*
	if( textArray.GetItemCount() > 0 )
	{
		AText	separatorText("---");
		textArray.Add(separatorText);
	}
	*/
	//==================ユーザーツールから作成==================
	if( mKeyToolMenuObjectID_UserTool != kObjectID_Invalid )
	{
		for( AIndex i = mKeyToolMenuStartIndex_UserTool; i < mKeyToolMenuEndIndex_UserTool; i++ )
		{
			//メニューテキストの最初の一文字取得
			AText	menutext;
			GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(mKeyToolMenuObjectID_UserTool,i,menutext);
			if( menutext.GetItemCount() < 2 )   continue;
			AText	t;
			t.SetText(menutext);
			t.ChangeCaseUpper();
			AUChar	ch = t.Get(0);
			AUChar	ch2 = t.Get(1);
			AText	text;
			if( ch >= 0x80 || ch2 != kUChar_Space )
			{
				//アスキー文字＋スペース以外は何もしない
				continue;
			}
			else
			{
				//リストに追加
				text.Add(ch);
				text.Add(kUChar_Colon);
				menutext.Delete1(0);
				text.AddText(menutext);
			}
			//サブメニュー有りなら右三角文字追加
			if( GetApp().NVI_GetMenuManager().GetDynamicMenuItemSubMenuObjectIDByObjectID(mKeyToolMenuObjectID_UserTool,i) != kObjectID_Invalid )
			{
				text.AddCstring(" ");
				AText	existSubMenuText;
				existSubMenuText.SetLocalizedText("ExistSubMenu");
				text.AddText(existSubMenuText);
			}
			//
			AIndex	foundIndex = textArray.Find(text);
			if( foundIndex != kIndex_Invalid )
			{
				isUserToolArray.Set(foundIndex,true);
			}
			else
			{
				textArray.Add(text);
				isUserToolArray.Add(true);
			}
		}
	}
	//==================ウインドウ表示位置決定==================
	AImagePoint	ipt;
	GetImagePointFromTextPoint(GetCaretTextPoint(),ipt);
	ALocalPoint	lpt;
	NVM_GetLocalPointFromImagePoint(ipt,lpt);
	lpt.x = 16;
	AGlobalPoint	gpt;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lpt,gpt);
	//==================ポップアップウインドウ表示==================
	if( mTextWindowID != kObjectID_Invalid )
	{
		//ポップアップウインドウ未生成なら生成する
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetPopupKeyToolListWindow();
	}
	//==================リストデータ設定==================
	ATextArray	ta;
	ta.SetFromTextArray(textArray);
	GetApp().SPI_SetKeyToolListInfo(mTextWindowID,NVI_GetControlID(),
									gpt,GetLineHeightWithMargin(GetCaretTextPoint().y),ta,isUserToolArray);
	//==================keytoolウインドウ表示==================
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetPopupKeyToolListWindow().NVI_Show(false);
	}
}

//R0073
void	AView_Text::SPI_KeyTool( const AUChar inKeyChar )
{
	AText	text;
	text.Add(inKeyChar);
	KeyTool(text);
}

//#379
/**
*/
void	AView_Text::DiffWithRepository()
{
	//ドキュメントのDiffモード設定
	ADiffTargetMode	oldDiffTargetMode = GetTextDocument().SPI_GetDiffMode();
	if( oldDiffTargetMode != kDiffTargetMode_RepositoryLatest )
	{
		GetTextDocument().SPI_SetDiffMode_Repository();
	}
	//サブペインDiff表示切り替え
	if( mTextWindowID != kObjectID_Invalid )
	{
		/*#725 diffでは、サイドバー表示は制御しない。
		//サブペイン表示
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowHideLeftSideBarColumn(true);
		*/
		//Diff表示にするかどうかの判定
		ABool	showDiffDisplay = false;
		if( oldDiffTargetMode != kDiffTargetMode_RepositoryLatest ||
					GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetDiffDisplayMode() == false )
		{
			//Diff対象の切り替え、または、現在Diff表示していない場合は、Diff表示へ切り替え
			showDiffDisplay = true;
		}
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetDiffDisplayMode(showDiffDisplay);
	}
}

void	AView_Text::NextDiff()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	/*#512
	ATextPoint	pt;
	pt.x = 0;
	pt.y = GetTextDocumentConst().SPI_GetNextDiffLineIndex(spt.y);
	*/
	AIndex	start = kIndex_Invalid, end = kIndex_Invalid;//#512
	GetTextDocumentConst().SPI_GetNextDiffLineIndex(spt.y,start,end);//#512
	if( /*#512pt.y*/start != kIndex_Invalid )
	{
		ATextPoint	pt;
		pt.x = 0;
		pt.y = start;
		//#688 下へ移動（SetCaretTextPoint()内でサブテキストのスクロール位置調整発生してしまうので） SetCaretTextPoint(pt);
		//SPI_AdjustScroll_Center()からNVIDO_ScrollPostProcess()経由でSPI_AdjustDiffDisplayScroll()
		//がcurrentDiffIndex=kIndex_Invalidでコールされ、サブペインがスクロールされる。
		//そして、refresh前に再度下記でスクロールするので、正常に描画更新されない。
		//この問題対策のため、フラグをもうけて、NVIDO_ScrollPostProcess()経由でSPI_AdjustDiffDisplayScroll()
		//を呼ばないようにする。#611
		mDisableAdjustDiffDisplayScrollAtScrollPostProcess = true;//#611
		SPI_AdjustScroll_Center(start,end,0);//#512 pt);
		mDisableAdjustDiffDisplayScrollAtScrollPostProcess = false;//#611
		/*#688
		//#379 Diff側ドキュメントを、Diff Partが見えるようにスクロール調整
		if( mTextWindowID != kObjectID_Invalid )
		{
			AIndex	currentDiffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(pt.y);
			GetApp().SPI_GetTextWindowByID(mTextWindowID).
					SPI_AdjustDiffDisplayScroll(mTextDocumentRef.GetObjectID(),NVI_GetControlID(),currentDiffIndex);
		}
		*/
		//キャレット設定（＋サブテキストのスクロール位置調整）
		SetCaretTextPoint(pt);
		AdjustScroll_Center(mCaretTextPoint);//折りたたみがある場合に、上では正しくadjustされないことがあるため
	}
}

void	AView_Text::PrevDiff()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	/*#512
	ATextPoint	pt;
	pt.x = 0;
	pt.y = GetTextDocumentConst().SPI_GetPrevDiffLineIndex(spt.y);
	*/
	AIndex	start = kIndex_Invalid, end = kIndex_Invalid;//#512
	GetTextDocumentConst().SPI_GetPrevDiffLineIndex(spt.y,start,end);//#512
	if( /*#512pt.y*/start != kIndex_Invalid )
	{
		ATextPoint	pt;
		pt.x = 0;
		pt.y = start;
		//#688 下へ移動（SetCaretTextPoint()内でサブテキストのスクロール位置調整発生してしまうので）SetCaretTextPoint(pt);
		mDisableAdjustDiffDisplayScrollAtScrollPostProcess = true;//#611
		SPI_AdjustScroll_Center(start,end,0);//#512 pt); 
		mDisableAdjustDiffDisplayScrollAtScrollPostProcess = false;//#611
		/*#688
		//#379 Diff側ドキュメントを、Diff Partが見えるようにスクロール調整
		if( mTextWindowID != kObjectID_Invalid )
		{
			AIndex	currentDiffIndex = GetTextDocumentConst().SPI_GetDiffIndexByIncludingLineIndex(pt.y);
			GetApp().SPI_GetTextWindowByID(mTextWindowID).
					SPI_AdjustDiffDisplayScroll(mTextDocumentRef.GetObjectID(),NVI_GetControlID(),currentDiffIndex);
		}
		*/
		//キャレット設定（＋サブテキストのスクロール位置調整）
		SetCaretTextPoint(pt);
		AdjustScroll_Center(mCaretTextPoint);//折りたたみがある場合に、上では正しくadjustされないことがあるため
	}
}

/*#725
//RC2
//識別子情報表示（現在のキャレット位置周辺の単語の識別子情報を表示する）
void	AView_Text::IdInfo()
{
	//#291 GetApp().SPI_GetIdInfoWindow().NVI_Show(false);
	GetApp().SPI_ShowHideIdInfoWindow(true);//#291
	AIndex	spos, epos;
	GetSelect(spos,epos);
	AText	text;
	if( spos == epos )
	{
		GetTextDocument().SPI_FindWord(spos,spos,epos);
	}
	GetTextDocumentConst().SPI_GetText(spos,epos,text);
	SetIdInfo(text);
}
*/

#if IMPLEMENTATION_FOR_MACOSX
//R0231
//辞書で調べる（ポップアップ表示）
void	AView_Text::DictionaryPopup()
{
	if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_5 )
	{
		AText	text;
		GetSelectedText(text);
		ATextPoint	spt,ept;
		SPI_GetSelect(spt,ept);
		AImagePoint	ipt;
		GetImagePointFromTextPoint(spt,ipt);
		ALocalPoint	pt;
		NVM_GetLocalPointFromImagePoint(ipt,pt);
		if( NVI_GetVerticalMode() == false )
		{
			//横書き時はascent分をプラスする
			pt.y += GetLineAscent(spt.y);//#450 mLineAscent;
		}
		else
		{
			//縦書き時は行高さ分をプラスする
			pt.y += GetLineHeightWithoutMargin(spt.y);
		}
		NVMC_ShowDictionary(text,pt);
	}
}

//R0231
//辞書で調べる（辞書アプリで表示）
void	AView_Text::Dictionary()
{
	AText	text;
	GetSelectedText(text);
	text.ConvertToURLEscape();
	text.InsertCstring(0,"dict:///");
	ALaunchWrapper::OpenURL(text);
}
#endif

#if 0
//RC2
//キー入力による自動IdInfo表示
void	AView_Text::IdInfoAuto()
{
	//キャレット位置取得
	AIndex	spos, epos;
	GetSelect(spos,epos);
	//キャレット位置より前でアルファベットが存在する位置を取得する
	for( spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos); spos > 0; spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos) )
	{
		if( GetTextDocumentConst().SPI_IsAsciiAlphabet(spos) == true )
		{
			break;
		}
	}
	//単語位置取得
	GetTextDocument().SPI_FindWord(spos,spos,epos);
	//上記で取得した単語について、識別子情報表示
	AText	text;
	GetTextDocumentConst().SPI_GetText(spos,epos,text);
	SetIdInfo(text);
	GetApp().SPI_GetIdInfoView().SPI_SetArgIndex(0);
}

//RC2
void	AView_Text::IdInfoAutoArg()
{
	//★
	/*#725
	
	//#225 識別子情報ウインドウ非表示なら何もしない
	if( GetApp().SPI_GetIdInfoWindow().NVI_IsWindowVisible() == false )   return;
	
	const ADocument_Text& document = GetTextDocumentConst();
	//単語
	AIndex	argindex = 0;
	//検索終了位置取得
	AIndex	localRangeStartLineIndex = document.SPI_GetCurrentLocalIdentifierStartLineIndex();
	if( localRangeStartLineIndex == kIndex_Invalid )   return;
	//キャレット位置取得
	AIndex	spos, epos;
	GetSelect(spos,epos);
	ATextIndex	localRangeStart = document.SPI_GetLineStart(localRangeStartLineIndex);
	//#695
	if( localRangeStart < spos - 10000 )
	{
		localRangeStart = document.SPI_GetCurrentCharTextIndex(spos-10000);
	}
	//キャレット位置より前で'('等を検索する
	const ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetSyntaxDefinition();
	if( syntaxDefinition.GetIdInfoAutoDisplayCharCount() == 0 )   return;//#225 IsIdInfoAutoDisplayCharが存在しないならすぐに終了（これがないと最初までbreakしない）
	AIndex	level = 0;
	for( spos = document.SPI_GetPrevCharTextIndex(spos); spos > localRangeStart; spos = document.SPI_GetPrevCharTextIndex(spos) )
	{
		AUChar	ch = document.SPI_GetTextChar(spos);
		//levelが0のときのみ
		if( level == 0 )
		{
			//IsIdInfoAutoDisplayChar（C言語の場合は")"）ならもう１つ前に戻して終了
			if( syntaxDefinition.IsIdInfoAutoDisplayChar(ch) == true )
			{
				spos = document.SPI_GetPrevCharTextIndex(spos);
				break;
			}
			//IsIdInfoDelimiterChar（C言語の場合は","）ならargindexを+1して継続
			if( syntaxDefinition.IsIdInfoDelimiterChar(ch) == true )
			{
				argindex++;
			}
		}
		//()内を無視するためにlevelを上下
		if( syntaxDefinition.IsIdInfoStartChar(ch) == true )   level--;
		if( syntaxDefinition.IsIdInfoEndChar(ch) == true )   level++;
	}
	//識別子取得
	document.SPI_FindWord(spos,spos,epos);
	AText	text;
	//現在表示中の識別子と同じなら、引数情報更新
	document.SPI_GetText(spos,epos,text);
	if( GetApp().SPI_GetIdInfoView().SPI_GetCurrentIdText().Compare(text) == true )
	{
		//引数インデックス設定
		GetApp().SPI_GetIdInfoView().SPI_SetArgIndex(argindex);
	}
	*/
}
#endif

//B0432
//単語選択
void	AView_Text::SelectWord()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	ATextIndex	spos,epos;
	GetTextDocument().SPI_FindWord(textpos,spos,epos);
	SetSelect(spos,epos);
}

//#858
#if 0
//RC3
//マイコメント機能　
void	AView_Text::ExternalComment()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		//プロジェクトフォルダ未設定の場合はワーニングを出して終了
		if( GetTextDocumentConst().SPI_GetProjectObjectID() == kObjectID_Invalid )
		{
			//デフォルトフォルダを取得
			AFileAcc	defaultFolder;
			GetTextDocumentConst().SPI_GetParentFolder(defaultFolder);
			//ワーニング
			GetApp().SPI_ShowProjectFolderAlertWindow(defaultFolder);
			return;
		}
		//現在の選択範囲取得
		ATextPoint	spt, ept;
		SPI_GetSelect(spt,ept);
		//#138 新規付箋紙／または既存付箋紙編集
		EditOrNewFusen(spt.y);
		/*
		//現在位置のモジュールとオフセットを取得
		AText	moduleName;
		AIndex	identifierLineIndex = kIndex_Invalid;
		GetTextDocument().SPI_GetModuleNameAndOffsetByLineIndex(spt.y,moduleName,identifierLineIndex);
		//マイコメントダイアログ表示
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowExternalCommentWindow(moduleName,spt.y-identifierLineIndex);
		*/
	}
}
#endif

//#146
/**
ジャンプ
*/
void	AView_Text::Jump( const AUniqID inJumpIdentifier )
{
	//現在の位置をNavigate登録する
	GetApp().SPI_RegisterNavigationPosition();
	//ジャンプ
	ATextPoint	spt,ept;
	GetTextDocument().SPI_GetGlobalIdentifierRange(inJumpIdentifier,spt,ept);
	AdjustScroll_Top(spt);
	SetSelect(spt,ept,true);
	//#130 アウトライン テスト（指定見出し範囲以外を非表示）
	/*
	AIndex	jumpIndex = GetTextDocumentConst().SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	ANumber	imageY = 0;
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		//高さ・ImageY設定
		ANumber	lineHeight = CalcLineHeight(lineIndex);
		if( GetTextDocumentConst().SPI_GetJumpMenuItemIndexByLineIndex(lineIndex) != jumpIndex )
		{
			lineHeight = 0;
		}
		mLineImageInfo_Height.Set(lineIndex,lineHeight);
		mLineImageInfo_ImageY.Set(lineIndex,imageY);
		imageY += lineHeight;
	}
	mLineImageInfo_Height.Set(lineCount,0);
	mLineImageInfo_ImageY.Set(lineCount,imageY);
	//ImageSize更新
	SPI_UpdateImageSize();
	NVI_Refresh();
	*/
}

//#150
/**
次へジャンプ
*/
void	AView_Text::JumpNext()
{
	AIndex menuItemIndex = GetTextDocument().SPI_GetJumpMenuItemIndexByLineIndex(GetCaretTextPoint().y);
	if( menuItemIndex+1 >= GetTextDocument().SPI_GetJumpMenuTextArray().GetItemCount() )   return;
	AUniqID	identifierObjectID = GetTextDocument().SPI_GetJumpMenuIdentifierUniqID(menuItemIndex+1);
	Jump(identifierObjectID);
}

//#150
/**
前へジャンプ
*/
void	AView_Text::JumpPrev()
{
	AIndex menuItemIndex = GetTextDocument().SPI_GetJumpMenuItemIndexByLineIndex(GetCaretTextPoint().y);
	if( menuItemIndex-1 < 0 )   return;
	AUniqID	identifierObjectID = GetTextDocument().SPI_GetJumpMenuIdentifierUniqID(menuItemIndex-1);
	Jump(identifierObjectID);
}

//win
/**
選択文字列設定
*/
void	AView_Text::SetFindText()
{
	if( IsCaretMode() == true )   return;
	AText	text;
	GetSelectedText(text);
	GetApp().SPI_SetFindText(text);
	//
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	GetTextDocument().SPI_SetFindHighlight(param);
	//検索ボックスにも設定 #137
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_SetSearchBoxText(text);
	}
}

/*#233 テスト
void	AView_Text::XorHoverCaret( const ATextPoint& inHoverCaret )
{
	AImagePoint	caretImageStart, caretImageEnd;
	GetImagePointFromTextPoint(inHoverCaret,caretImageStart);
	caretImageEnd = caretImageStart;
	caretImageEnd.y += GetLineHeightWithMargin();
	ALocalPoint	caretLocalStart, caretLocalEnd;
	NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
	NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
	NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,false,2);
}
*/

//#725
/**
NVI_SetShow()コール時の処理
*/
void	AView_Text::NVIDO_SetShow( const ABool inShow )
{
	if( mPopupNotificationWindowID == kObjectID_Invalid )
	{
		return;
	}
	if( SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetItemCount() == 0 )
	{
		return;
	}
	
	//notification ウインドウを表示／非表示
	SPI_ShowNotificationPopupWindow(inShow);
}

//#725
/**
NVI_SetPosition()コール時の処理
*/
void	AView_Text::NVIDO_SetPosition( const AWindowPoint& inWindowPoint )
{
	if( mPopupNotificationWindowID == kObjectID_Invalid )
	{
		return;
	}
	if( SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetItemCount() == 0 )
	{
		return;
	}
	
	//notificationウインドウのbounds更新
	UpdateNotificationPopupWindowBounds();
}

//#725
/**
NVI_SetSize()コール時の処理
*/
void	AView_Text::NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight )
{
	if( mPopupNotificationWindowID == kObjectID_Invalid )
	{
		return;
	}
	if( SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetItemCount() == 0 )
	{
		return;
	}
	
	//notificationウインドウのbounds更新
	UpdateNotificationPopupWindowBounds();
}

//#905
/**
文法認識実行トリガー
*/
void	AView_Text::TriggerSyntaxRecognize()
{
	//文法認識実行（unrecognizedな行をサーチして全て（スレッドで）認識する）
	GetTextDocument().SPI_RecognizeSyntaxUnrecognizedLines();
	//ローカル識別子
	GetTextDocument().SPI_SetCurrentLocalTextPoint(GetCaretTextPoint(),GetObjectID());
}

//テスト用文字列
//あああああああああああ漢字ああああ
//どどどどどど漢字どどどどどどどど
//漢字漢字漢字漢字漢字漢字あああああ
//aaaうううううううううううううう

//#1035
/**
文字種変換
*/
void	AView_Text::Transliterate( const AText& inTitle )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( mKukeiSelected == false )
	{
		//通常選択時
		
		//選択範囲取得
		ATextIndex	spos,epos;
		GetSelect(spos,epos);
		//選択範囲テキスト取得
		AText	origtext, text;
		GetSelectedText(origtext);
		//文字種変換実行
		GetApp().SPI_Transliterate(inTitle,origtext,text);
		//テキスト置換
		TextInput(undoTag_Transliterate,text);
		//選択範囲更新
		SetSelect(spos,spos+text.GetItemCount());
	}
	else
	{
		//矩形選択時
		
		//undoタグ
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Transliterate);
		//矩形選択範囲取得
		AArray<ATextIndex>	sposArray, eposArray;
		SPI_GetSelect(sposArray,eposArray);
		AItemCount	totalOffset = 0;
		for( AIndex i = 0; i < sposArray.GetItemCount(); i++ )
		{
			//選択範囲取得
			ATextIndex	spos = sposArray.Get(i)+totalOffset;
			ATextIndex	epos = eposArray.Get(i)+totalOffset;
			//選択範囲テキスト取得
			AText	origtext, text;
			GetTextDocumentConst().SPI_GetText(spos,epos,origtext);
			//文字種変換実行
			GetApp().SPI_Transliterate(inTitle,origtext,text);
			//テキスト置換
			DeleteTextFromDocument(spos,epos);
			InsertTextToDocument(spos,text);
			//オフセット更新
			totalOffset += text.GetItemCount() - origtext.GetItemCount();
			//選択範囲更新
			sposArray.Set(i,spos);
			eposArray.Set(i,spos+text.GetItemCount());
		}
		//矩形選択範囲更新
		SetKukeiSelect(sposArray,eposArray);
	}
}

//#1035
/**
文字種変換（小文字化）
*/
void	AView_Text::TransliterateToLower()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( mKukeiSelected == false )
	{
		//通常選択時
		
		//選択範囲取得
		ATextIndex	spos,epos;
		GetSelect(spos,epos);
		//選択範囲テキスト取得
		AText	text;
		GetSelectedText(text);
		//文字種変換実行
		text.ChangeCaseLower();
		//テキスト置換
		TextInput(undoTag_Transliterate,text);
		//選択範囲更新
		SetSelect(spos,spos+text.GetItemCount());
	}
	else
	{
		//矩形選択時
		
		//undoタグ
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Transliterate);
		//矩形選択範囲取得
		AArray<ATextIndex>	sposArray, eposArray;
		SPI_GetSelect(sposArray,eposArray);
		AItemCount	totalOffset = 0;
		for( AIndex i = 0; i < sposArray.GetItemCount(); i++ )
		{
			//選択範囲取得
			ATextIndex	spos = sposArray.Get(i)+totalOffset;
			ATextIndex	epos = eposArray.Get(i)+totalOffset;
			//選択範囲テキスト取得
			AText	origtext, text;
			GetTextDocumentConst().SPI_GetText(spos,epos,origtext);
			text.SetText(origtext);
			//文字種変換実行
			text.ChangeCaseLower();
			//テキスト置換
			DeleteTextFromDocument(spos,epos);
			InsertTextToDocument(spos,text);
			//オフセット更新
			totalOffset += text.GetItemCount() - origtext.GetItemCount();
			//選択範囲更新
			sposArray.Set(i,spos);
			eposArray.Set(i,spos+text.GetItemCount());
		}
		//矩形選択範囲更新
		SetKukeiSelect(sposArray,eposArray);
	}
}

//#1035
/**
文字種変換（大文字化）
*/
void	AView_Text::TransliterateToUpper()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	if( mKukeiSelected == false )
	{
		//通常選択時
		
		//選択範囲取得
		ATextIndex	spos,epos;
		GetSelect(spos,epos);
		//選択範囲テキスト取得
		AText	text;
		GetSelectedText(text);
		//文字種変換実行
		text.ChangeCaseUpper();
		//テキスト置換
		TextInput(undoTag_Transliterate,text);
		//選択範囲更新
		SetSelect(spos,spos+text.GetItemCount());
	}
	else
	{
		//矩形選択時
		
		//undoタグ
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_Transliterate);
		//矩形選択範囲取得
		AArray<ATextIndex>	sposArray, eposArray;
		SPI_GetSelect(sposArray,eposArray);
		AItemCount	totalOffset = 0;
		for( AIndex i = 0; i < sposArray.GetItemCount(); i++ )
		{
			//選択範囲取得
			ATextIndex	spos = sposArray.Get(i)+totalOffset;
			ATextIndex	epos = eposArray.Get(i)+totalOffset;
			//選択範囲テキスト取得
			AText	origtext, text;
			GetTextDocumentConst().SPI_GetText(spos,epos,origtext);
			text.SetText(origtext);
			//文字種変換実行
			text.ChangeCaseUpper();
			//テキスト置換
			DeleteTextFromDocument(spos,epos);
			InsertTextToDocument(spos,text);
			//オフセット更新
			totalOffset += text.GetItemCount() - origtext.GetItemCount();
			//選択範囲更新
			sposArray.Set(i,spos);
			eposArray.Set(i,spos+text.GetItemCount());
		}
		//矩形選択範囲更新
		SetKukeiSelect(sposArray,eposArray);
	}
}

//#1154
/**
キャレットの左右の文字入れ替え
*/
void	AView_Text::SwapLetter()
{
	//
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	//選択中以外の場合は何もせずリターン
	if( IsCaretMode() == false )   return;
	//キャレット位置取得
	ATextPoint	spt,ept;
	SPI_GetSelect(spt,ept);
	ATextIndex	startIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	//キャレット位置がドキュメントの最初または最後の場合は何もしない
	if( startIndex == 0 || startIndex >= GetTextDocumentConst().SPI_GetTextLength() )   return;
	//前の文字の位置を取得
	ATextIndex	prevIndex = GetTextDocumentConst().SPI_GetPrevCharTextIndex(startIndex);
	//前の文字を取得
	AText	swapText;
	GetTextDocumentConst().SPI_GetText(prevIndex,startIndex,swapText);
	//次の文字の位置を取得
	ATextIndex	nextIndex = GetTextDocumentConst().SPI_GetNextCharTextIndex(startIndex);
	ATextPoint	nextpt;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(nextIndex,nextpt);
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
	//次の文字の位置へ、前の文字を挿入
	InsertTextToDocument(nextIndex,swapText);
	//前の文字を削除（前行は後ろへ削除しているのでテキストの位置は変化していない）
	DeleteTextFromDocument(prevIndex,startIndex);
	//キャレット位置補正。（削除した開始位置の次の文字位置を取得）
	ATextIndex	caretTextIndex = GetTextDocumentConst().SPI_GetNextCharTextIndex(prevIndex);
	ATextPoint	caretpt;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(caretTextIndex,caretpt);
	SetCaretTextPoint(caretpt);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---Update

//
/**
表示更新
*/
void	AView_Text::SPI_UpdateText( const AIndex inStartLineIndex, const AIndex inEndLineIndex,
		const AItemCount inInsertedLineCount, const ABool inParagraphCountChanged )//#450
{
	//背景描画ありの場合、全て描画せざるを得ないので、スクロールでの行挿入・削除描画はしない
	
	//
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//
	AImageRect	imageLineRect;
	GetLineImageRect(inStartLineIndex,inEndLineIndex,imageLineRect);
	ALocalRect	localLineRect;
	NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
	//行挿入・行削除で、更新開始行(inStartLineIndex)が、フレームより前だった場合は、挿入行分だけredrawなしスクロールする（表示更新はRefreshRect()で行う）
	if( inInsertedLineCount != 0 && imageLineRect.top < imageFrameRect.top )
	{
		NVI_Scroll(0,inInsertedLineCount*GetLineHeightPlusMargin()/*#450 GetLineHeightWithMargin()*/,false);
	}
	//更新最終行が、テキスト全体の最終行、かつ、行削除によるものであった場合は、ビューの最下部まで更新
	if( inEndLineIndex >= GetTextDocumentConst().SPI_GetLineCount() && inInsertedLineCount < 0 )
	{
		localLineRect.bottom = localFrameRect.bottom;
	}
	//更新範囲はviewの右端までにする
	if( localLineRect.right < localFrameRect.right )
	{
		localLineRect.right = localFrameRect.right;
	}
	//
	//更新
	NVMC_RefreshRect(localLineRect);
	//#450 行番号ビュー表示更新
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_UpdateText(inStartLineIndex,inEndLineIndex,inInsertedLineCount,inParagraphCountChanged);
	}
}

//#450
/**
表示更新
*/
void	AView_Text::SPI_RefreshTextView()
{
	if( NVI_IsVisible() == false )   return;//#530
	//テキストビュー表示更新
	NVI_Refresh();
	//行番号ビュー表示更新
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().NVI_Refresh();
	}
	//DiffInfoWindow表示更新 #379
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_RefreshDiffInfoWindow();
	}
}

//#695
/**
表示更新
*/
void	AView_Text::SPI_RefreshLines( const AIndex inStartLineIndex, const AIndex inEndLineIndex )
{
	//
	if( NVI_IsVisible() == false )   return;//#530
	
	//指定行の範囲取得し、RefreshRectする
	AImageRect	imageLineRect = {0,0,0,0};
	GetLineImageRect(inStartLineIndex,inEndLineIndex,imageLineRect);
	ALocalRect	localLineRect = {0,0,0,0};
	NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
	//更新範囲はviewの右端までにする
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	if( localLineRect.right < localFrameRect.right )
	{
		localLineRect.right = localFrameRect.right;
	}
	//
	NVMC_RefreshRect(localLineRect);
	//行番号ペインの描画更新
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_RefreshLines(inStartLineIndex,inEndLineIndex);
	}
	//DiffInfoWindow表示更新 #379
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_RefreshDiffInfoWindow();
	}
}

//#496
/**
指定行の描画更新
*/
void	AView_Text::SPI_RefreshLine( const AIndex inLineIndex )
{
	//指定行の範囲取得し、RefreshRectする
	AImageRect	imageLineRect;
	GetLineImageRect(inLineIndex,imageLineRect);
	ALocalRect	localLineRect;
	NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
	//更新範囲はviewの右端までにする
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	if( localLineRect.right < localFrameRect.right )
	{
		localLineRect.right = localFrameRect.right;
	}
	//
	NVMC_RefreshRect(localLineRect);
	//行番号ペインの描画更新
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_RefreshLine(inLineIndex);
	}
}

//
void	AView_Text::SPI_UpdateImageSize()
{
	//#450 LineImageInfo未作成なら何もせずリターン
	if( mLineImageInfo_ImageY.GetItemCount() == 0 )   return;
	
	ANumber	width = 20000;//★ 10000→20000 #1069
	ANumber	height = mLineImageInfo_ImageY.Get(mLineImageInfo_ImageY.GetItemCount()-1);//#450 GetTextDocument().SPI_GetLineCount() * GetLineHeightWithMargin();
	if( (GetTextDocument().SPI_GetWrapMode() != kWrapMode_NoWrap) && 
		(GetTextDocument().SPI_GetWrapMode() != kWrapMode_WordWrapByLetterCount) &&
		(GetTextDocument().SPI_GetWrapMode() != kWrapMode_LetterWrapByLetterCount) ) //#1113
	{
		width = NVI_GetViewWidth();
		//#531 サブペインで行折り返しドキュメントを左右スクロールできるようにするため
		if( mTextWindowID != kObjectID_Invalid )
		{
			width = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetTextViewWidth(/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID());
		}
	}
	//#612 文字数wrapのときは、Viewの幅は"W"の文字を指定文字数分とする。（ただしウインドウ幅よりは小さくしない）
	if( GetTextDocumentConst().SPI_GetWrapMode() == kWrapMode_WordWrapByLetterCount ||
		GetTextDocumentConst().SPI_GetWrapMode() == kWrapMode_LetterWrapByLetterCount ) //#1113
	{
		AText	w("a");//#1186 文字をw→aに変更。（縦書き原稿表示時にView幅が広くなりすぎて、スクロールバーが表示されてしまう問題を修正するため。）
		width = static_cast<ANumber>(NVMC_GetDrawTextWidth(w) * GetTextDocumentConst().SPI_GetWrapLetterCount());
		ANumber	textviewwidth = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetTextViewWidth(/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID());
		if( width < textviewwidth )
		{
			width = textviewwidth;
		}
	}
	//
	NVM_SetImageSize(width,height);
	//#379 Documentからの更新イベントはTextViewがLineNumberへも通知するようにする
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_UpdateImageSize();
	}
	//行折り返しスレッドpaused時にスクロールバー幅を更新するために、Scrollbar unitを更新 #688
	SPI_UpdateScrollBarUnit();
}

//
void	AView_Text::SPI_UpdateScrollBarUnit()
{
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(kHScrollBarUnit,GetLineHeightPlusMargin()//#450 GetLineHeightWithMargin()
			,rect.right-rect.left-kHScrollBarUnit*5,rect.bottom-rect.top-/*#450 GetLineHeightWithMargin()*/GetLineHeightPlusMargin()*5);
}

//フォント等のText描画プロパティが変更された場合の処理
void	AView_Text::SPI_UpdateTextDrawProperty()
{
	//win
	AText	fontname;
	//#868 GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
	GetTextDocumentConst().SPI_GetFontName(fontname);//#868
	
	AColor	color;
	GetModePrefDB().GetModeData_Color(AModePrefDB::kLetterColor,color);
	NVMC_SetDefaultTextProperty(
			fontname,//win
			GetTextDocumentConst().SPI_GetFontSize(),
			color,
			kTextStyle_Normal,
			GetTextDocument().SPI_IsAntiAlias());
	NVMC_GetMetricsForDefaultTextProperty(mLineHeight,mLineAscent);
	mLineMargin = GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Number(AModePrefDB::kLineMargin);
	/*B0000 if( GetTextDocument().SPI_IsAntiAlias() == true && GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kInsertLineMarginForAntiAlias) == true )
	{
		mLineMargin++;
		mLineHeight++;
	}*/
	//
	SPI_UpdateScrollBarUnit();
	//LineHeightが変更されたのでImageSizeも変更必要
	SPI_UpdateImageSize();
	mZenkakuSpaceWidth = static_cast<ANumber>(NVMC_GetDrawTextWidth(mZenkakuSpaceText));
	//aの文字幅を記憶 #1186
	AText	achar("a");
	mACharWidth = static_cast<ANumber>(NVMC_GetDrawTextWidth(achar));
	//「あ」の文字幅を記憶 #1385 マス目表示には全角スペースだとOsakaフォント等で不正確になるので「あ」を使う。
	AText	zenkakuachar;
	zenkakuachar.AddFromUCS4Char(0x3042);
	mZenkakuAWidth = NVMC_GetDrawTextWidth(zenkakuachar);
}

//#699
/**
行折り返し計算スレッド動作中（pause時）処理
*/
void	AView_Text::SPI_DoWrapCalculating()
{
	//行番号ボタンの表示更新
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_UpdateLineNumberButton(NVI_GetControlID());
	}
}

//#699
/**
行折り返し計算スレッド計算完了時処理
*/
void	AView_Text::SPI_DoWrapCalculated()
{
	/*
	//下のAdjustScroll_Center()にて、viewのサイズが空だとadjustされず、また、小さすぎると変な位置にadjustされてしまうので、
	//十分なサイズをとっておく。（高さは300にしておく。これで上から150のあたりにキャレットが来るようになる）
	if( NVI_GetWidth() == 0 )
	{
		NVI_SetSize(10000,300);
	}
	*/
	
	//選択予約実行
	if( mReservedSelection_StartTextIndex != kIndex_Invalid )
	{
		//選択
		SetSelect(mReservedSelection_StartTextIndex,mReservedSelection_EndTextIndex);
		//スクロール調整
		AdjustScroll_Center(mCaretTextPoint);
	}
	if( mReservedSelection_StartTextPoint.y != kIndex_Invalid )
	{
		if( mReservedSelection_IsSelectWithParagraph == true )
		{
			//段落indexによる選択予約
			AIndex	spos = GetTextDocumentConst().
					SPI_GetParagraphStartTextIndex(mReservedSelection_StartTextPoint.y) +
					mReservedSelection_StartTextPoint.x;
			AIndex	epos = GetTextDocumentConst().
					SPI_GetParagraphStartTextIndex(mReservedSelection_EndTextPoint.y) +
					mReservedSelection_EndTextPoint.x;
			SetSelect(spos,epos);
		}
		else
		{
			//text point補正
			GetTextDocumentConst().SPI_CorrectTextPoint(mReservedSelection_StartTextPoint);
			GetTextDocumentConst().SPI_CorrectTextPoint(mReservedSelection_EndTextPoint);
			//選択
			SetSelect(mReservedSelection_StartTextPoint,mReservedSelection_EndTextPoint);
		}
		//スクロール調整
		AdjustScroll_Center(mCaretTextPoint);
	}
}

//#699
/**
選択予約解除
*/
void	AView_Text::CancelReservedSelection()
{
	mReservedSelection_StartTextIndex = kIndex_Invalid;
	mReservedSelection_EndTextIndex = kIndex_Invalid;
	mReservedSelection_StartTextPoint = kTextPoint_Invalid;
	mReservedSelection_EndTextPoint = kTextPoint_Invalid;
	mReservedSelection_IsSelectWithParagraph = false;
}

/**
ウインドウタイトルバーテキスト更新
*/
void	AView_Text::SPI_UpdateWindowTitleBarText()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_UpdateWindowTitleBarText();
	}
}

#pragma mark ===========================

#pragma mark ---スクロール

//
void	AView_Text::NVIDO_ScrollPreProcess( const ANumber inDeltaX, const ANumber inDeltaY,
											const ABool inRedraw, const ABool inConstrainToImageSize,
											const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//fprintf(stderr,"%16X NVIDO_ScrollPreProcess() - Start\n",(int)(GetObjectID().val));
	
//	TempHideCaret();
	
	//#138 付箋紙編集ウインドウをHide
	//#858 GetApp().SPI_GetFusenEditWindow().NVI_Hide(); 
	
	//#717
	//補完入力状態解除
	ClearAbbrevInputMode(true,false);
	
	//fprintf(stderr,"%16X NVIDO_ScrollPreProcess() - End\n",(int)(GetObjectID().val));
}

//
void	AView_Text::NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
											 const ABool inRedraw, const ABool inConstrainToImageSize,
											 const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//fprintf(stderr,"%16X NVIDO_ScrollPostProcess() - Start\n",(int)(GetObjectID().val));
	
	//X方向スクロールのときは全体再描画させる
	//（言語パート表示が乱れる問題の対策。どうせX方向スクロールの場合は全行再描画になるので、パフォーマンスへの影響は無い）#1042
	if( inDeltaX != 0 && inRedraw == true )
	{
		NVI_Refresh();
	}
	
	//#1031
	//キャレットのローカル位置を固定
	//スクロールバーまたはホイールによるスクロールの場合、キャレット位置を前回設定したキャレットのローカル座標に固定する
	//このタイミングで実行すると、スクロール実行と、現在行移動が同時に描画されるのでちらつき発生しない。
	//（SPI_ViewScrolled()よりも後で実行してしまうと、その中のNVMC_ExecuteDoDrawImmediately()で画面フラッシュが発生している？のでちらつき発生する
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFixCaretLocalPoint) == true &&
		GetTextDocumentConst().SPI_IsDiffTargetDocument() == false ) //比較元ビューではキャレット固定しない
	{
		switch(inScrollTrigger)
		{
		  case kScrollTrigger_ScrollBar:
		  case kScrollTrigger_Wheel:
		  case kScrollTrigger_ScrollKey:
			{
				//前回設定したキャレットのローカル座標からキャレット設定（スクロール後なので、キャレット移動する。）
				//「前回設定したキャレットのローカル座標」は更新しない
				AImagePoint	ipt = {0};
				NVM_GetImagePointFromLocalPoint(mLastCaretLocalPoint,ipt);
				ATextPoint	textpt = {0};
				GetTextPointFromImagePoint(ipt,textpt);
				ALocalPoint	lpt = mLastCaretLocalPoint;
				SPI_SetSelect(textpt,textpt);
				mLastCaretLocalPoint = lpt;
				//DoDraw+画面フラッシュ
				NVMC_ExecuteDoDrawImmediately();
			}
			break;
		  default:
			{
				//処理なし
				break;
			}
		}
	}
	
//	RestoreTempHideCaret();
	
	//#1031 AView::ScrollCore()から移動
	//AWindowオブジェクトへViewがスクロールされたことを伝える
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ViewScrolled(NVI_GetControlID(),inDeltaX,inDeltaY,inRedraw,inConstrainToImageSize);
	}
	
	if( mDisableAdjustDiffDisplayScrollAtScrollPostProcess == false )//#611
	{
		//#379
		//DiffViewを同期スクロールする
		if( mTextWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_AdjustDiffDisplayScroll(/*#695 mTextDocumentID*/mTextDocumentRef.GetObjectID(),NVI_GetControlID(),kIndex_Invalid);
		}
	}
	//ポップアップウインドウの位置更新
	AdjustPopupWindowsPosition();
	
	//fprintf(stderr,"%16X NVIDO_ScrollPostProcess() - End\n",(int)(GetObjectID().val));
}

//#379
/**
Viewで表示している最初の段落を取得する
*/
AIndex	AView_Text::SPI_GetViewStartParagraphIndex() const
{
	//ImageFrame取得
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//段落取得
	return GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetLineIndexByImageY(imageFrameRect.top));
}

//#379
/**
Viewで表示している最後の段落を取得する
*/
AIndex	AView_Text::SPI_GetViewEndParagraphIndex() const
{
	//ImageFrame取得
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	//段落取得
	return GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetLineIndexByImageY(imageFrameRect.bottom));
}

//#379
/**
指定段落へスクロールする
*/
void	AView_Text::SPI_ScrollToParagraph( const AIndex inStartParagraphIndex, const ANumber inOffset )
{
	AIndex	lineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(inStartParagraphIndex);
	AImagePoint	originImagePt = {0,0};
	originImagePt.y = GetImageYByLineIndex(lineIndex) + inOffset;
	if( originImagePt.y < 0 )   originImagePt.y = 0;
	NVI_ScrollTo(originImagePt);
}

//#450
/**
現在のスクロール位置の開始段落を取得
*/
AIndex	AView_Text::SPI_GetOriginParagraph() const
{
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	AIndex	lineIndex = GetLineIndexByImageY(imageFrameRect.top);
	return GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(lineIndex);
}

//#450
/**
指定行へスクロール
*/
void	AView_Text::SPI_ScrollToLine( const AIndex inStartLineIndex )
{
	AImagePoint	originImagePt = {0,0};
	originImagePt.y = GetImageYByLineIndex(inStartLineIndex);
	if( originImagePt.y < 0 )   originImagePt.y = 0;
	NVI_ScrollTo(originImagePt);
}

//#450
/**
viewフレームの開始行を取得
*/
AIndex	AView_Text::SPI_GetOriginLineIndex() const
{
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	AIndex	lineIndex = GetLineIndexByImageY(imageFrameRect.top);
	return lineIndex;
}

//
void	AView_Text::AdjustScroll( const ATextPoint& inTextPoint, const AIndex inOffset )//B0434
{
	//現在のフレームを取得
	AImageRect	oldFrameImageRect;
	NVM_GetImageViewRect(oldFrameImageRect);
	//フレームが空なら何もしない win
	if( oldFrameImageRect.left == oldFrameImageRect.right ||
				oldFrameImageRect.top == oldFrameImageRect.bottom )   return;
	//
	AImagePoint	newTopLeft;
	newTopLeft.x = oldFrameImageRect.left;
	newTopLeft.y = oldFrameImageRect.top;
	
	//通常時の行の高さ取得
	ANumber	normalLineHeight = mLineHeight+mLineMargin;
	//一番下のviewかどうかを取得
	ABool	isBottomView = false;
	if( mTextWindowID != kObjectID_Invalid )
	{
		isBottomView = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetIsBottomTextView(NVI_GetControlID());
	}
	//下部余白行数を計算
	ANumber	bottomMarginLineCount = 0;
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kBottomScrollAt25Percent) == true &&
	   isBottomView == true )
	{
		ALocalRect	frameLocalRect = {0};
		NVM_GetLocalViewRect(frameLocalRect);
		bottomMarginLineCount = (frameLocalRect.bottom-frameLocalRect.top)/normalLineHeight/4;
	}
	//上部・下部編集マージンを取得（下部編集マージンには上で計算した下部余白行数を足す）
	ANumber	topEditMargin = inOffset;
	ANumber	bottomEditMargin = inOffset+1;
	bottomEditMargin += bottomMarginLineCount;
	
	
	//Y方向補正
	ATextPoint	pt = inTextPoint;
	//指定ポイントのimage Yの位置がframeよりも前の時は補正
	if( GetImageYByLineIndex(pt.y) < oldFrameImageRect.top )
	{
		//指定ポイントのimage Yが上部編集マージンの余白の後に表示されるように調整
		newTopLeft.y = GetImageYByLineIndex(pt.y);
		newTopLeft.y -= topEditMargin*normalLineHeight;//B0434 2);
	}
	else if( GetImageYByLineIndex(pt.y+1) + bottomMarginLineCount*normalLineHeight >= oldFrameImageRect.bottom )
	{
		//指定ポイントのimage Yが下部編集マージンの余白の前に表示されるように調整
		//#450 おりたたみ対応により、右記では途中におりたたみが有った場合に正しくないので下記に修正。 newTopLeft.y = GetImageYByLineIndex(pt.y-(endLineIndex-startLineIndex)+inOffset);//B0434 2);
		//pt.y+inOffset行目のy位置が最下行になるような左上originを計算
		newTopLeft.y = GetImageYByLineIndex(pt.y+1);
		newTopLeft.y += bottomEditMargin*normalLineHeight;
		newTopLeft.y -= (oldFrameImageRect.bottom-oldFrameImageRect.top);
		if( newTopLeft.y < 0 )   newTopLeft.y = 0;
		//frameの開始を行の途中にしないように補正する。
		AIndex	newTopLineIndex = GetLineIndexByImageY(newTopLeft.y);
		newTopLeft.y = GetImageYByLineIndex(newTopLineIndex);
	}
	
	//X方向補正
	AImagePoint	imagePoint;
	GetImagePointFromTextPoint(pt,imagePoint);
	if( imagePoint.x > oldFrameImageRect.right-5 )//#652
	{
		ANumber	delta = (imagePoint.x-oldFrameImageRect.right)/100+1;
		delta *= 100;
		newTopLeft.x = oldFrameImageRect.left + delta;
	}
	else if( imagePoint.x < oldFrameImageRect.left+5 )//#652
	{
		ANumber	delta = (oldFrameImageRect.left-imagePoint.x)/100+1;
		delta *= 100;
		newTopLeft.x = oldFrameImageRect.left -= delta;
	}
	if( newTopLeft.x < 0 )   newTopLeft.x = 0;
	//#531 サブペインで行折り返しドキュメントを左右スクロールできるようにするため if( GetTextDocument().SPI_GetWrapMode() != kWrapMode_NoWrap )   newTopLeft.x = 0;
	
	//スクロール
	NVI_ScrollTo(newTopLeft);
	
	//#1031
	//キャレットのローカル位置を記憶
	SetLastCaretLocalPoint();
}

void	AView_Text::SPI_AdjustScroll_Center()
{
	AdjustScroll_Center(GetCaretTextPoint());
}

//#454
/**
*/
void	AView_Text::SPI_AdjustScroll_Top()
{
	AdjustScroll_Top(GetCaretTextPoint());
}

/**
タイプ指定AdjustScroll
*/
void	AView_Text::SPI_AdjustScroll( const AAdjustScrollType inType )
{
	switch(inType)
	{
	  case kAdjustScrollType_Top:
		{
			SPI_AdjustScroll_Top();
			break;
		}
	  case kAdjustScrollType_Center:
		{
			SPI_AdjustScroll_Center();
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
}

//#512
/**
指定範囲をAdjustScrollする
*/
void	AView_Text::SPI_AdjustScroll_Center( const AIndex inStartLineIndex, const AIndex inEndLineIndex, const ANumber inX )
{
	//現在のフレームを取得
	AImageRect	oldFrameImageRect = {0,0,0,0};
	NVM_GetImageViewRect(oldFrameImageRect);
	//フレームが空なら何もしない win
	if( oldFrameImageRect.left == oldFrameImageRect.right ||
				oldFrameImageRect.top == oldFrameImageRect.bottom )   return;
	//
	AImagePoint	newTopLeft = {0,0};
	newTopLeft.x = inX;
	newTopLeft.y = oldFrameImageRect.top;
	
	//フレームの開始行、終了行を取得
	AIndex	frameStartLineIndex = GetLineIndexByImageY(oldFrameImageRect.top);
	AIndex	frameEndLineIndex = GetLineIndexByImageY(oldFrameImageRect.bottom);
	
	//1. 全体表示できるなら、中間地点でAdjust_Centerする
	if( frameEndLineIndex - frameStartLineIndex > inEndLineIndex - inStartLineIndex )
	{
		//Y方向補正
		if( inStartLineIndex <= frameStartLineIndex || inEndLineIndex >= frameEndLineIndex-1 )
		{
			AIndex	newStartLineIndex = (inStartLineIndex+inEndLineIndex)/2 - (frameEndLineIndex-frameStartLineIndex)/2;
			if( newStartLineIndex < 0 )   newStartLineIndex = 0;
			newTopLeft.y = GetImageYByLineIndex(newStartLineIndex);
		}
	
	}
	//2. 全体表示できないなら、最初の行でAdjustScroll_Topする
	else
	{
		//Y方向補正
		newTopLeft.y = GetImageYByLineIndex(inStartLineIndex);
	}
	
	//スクロール
	NVI_ScrollTo(newTopLeft);
	
	//#1031
	//キャレットのローカル位置を記憶
	SetLastCaretLocalPoint();
}

//キャレット位置が現在のフレーム外の場合、キャレット位置がフレーム中央付近に来るようにスクロール補正する。
void	AView_Text::AdjustScroll_Center( const ATextPoint& inTextPoint, const ABool inRedraw )
{
	//現在のフレームを取得
	AImageRect	oldFrameImageRect;
	NVM_GetImageViewRect(oldFrameImageRect);
	//フレームが空なら何もしない win
	if( oldFrameImageRect.left == oldFrameImageRect.right ||
				oldFrameImageRect.top == oldFrameImageRect.bottom )   return;
	//
	AImagePoint	newTopLeft;
	newTopLeft.x = oldFrameImageRect.left;
	newTopLeft.y = oldFrameImageRect.top;
	
	//フレームの開始行、終了行を取得
	AIndex	startLineIndex = GetLineIndexByImageY(oldFrameImageRect.top);
	AIndex	endLineIndex = GetLineIndexByImageY(oldFrameImageRect.bottom);
	
	//Y方向補正
	if( inTextPoint.y <= startLineIndex || inTextPoint.y >= endLineIndex-1 )
	{
		/*#450 おりたたみ対応により、右記では途中におりたたみが有った場合に正しくないので下記に修正。AIndex	newStartLineIndex = GetCaretTextPoint().y - (endLineIndex-startLineIndex)/2;
		if( newStartLineIndex < 0 )   newStartLineIndex = 0;
		newTopLeft.y = GetImageYByLineIndex(newStartLineIndex);//#450 newStartLineIndex * GetLineHeightWithMargin();
		*/
		//inTextPoint.y行目のy位置がフレーム中央になるような左上originを計算
		newTopLeft.y = GetImageYByLineIndex(inTextPoint.y) - (oldFrameImageRect.bottom-oldFrameImageRect.top)/2;
		if( newTopLeft.y < 0 )   newTopLeft.y = 0;
		//frameの開始を行の途中にしないように補正する。
		AIndex	newTopLineIndex = GetLineIndexByImageY(newTopLeft.y);
		newTopLeft.y = GetImageYByLineIndex(newTopLineIndex);
	}
	
	//X方向補正
	ATextPoint	selSpt, selEpt;
	SPI_GetSelect(selSpt,selEpt);
	if( selSpt.y == selEpt.y )
	{
		AImagePoint	imagePoint;
		GetImagePointFromTextPoint(selSpt,imagePoint);
		if( (imagePoint.x < oldFrameImageRect.left) || (oldFrameImageRect.right-10 < imagePoint.x) )
		{
			newTopLeft.x = imagePoint.x - (oldFrameImageRect.right-oldFrameImageRect.left)/2;
		}
	}
	if( newTopLeft.x < 0 )   newTopLeft.x = 0;
	
	//スクロール
	NVI_ScrollTo(newTopLeft,inRedraw);
	
	//#1031
	//キャレットのローカル位置を記憶
	SetLastCaretLocalPoint();
}

//キャレット位置が現在のフレーム外の場合、キャレット位置がフレーム最上部に来るようにスクロール補正する。
void	AView_Text::AdjustScroll_Top( const ATextPoint& inTextPoint )
{
	//現在のフレームを取得
	AImageRect	oldFrameImageRect;
	NVM_GetImageViewRect(oldFrameImageRect);
	//フレームが空なら何もしない win
	if( oldFrameImageRect.left == oldFrameImageRect.right ||
				oldFrameImageRect.top == oldFrameImageRect.bottom )   return;
	//
	AImagePoint	newTopLeft;
	newTopLeft.x = oldFrameImageRect.left;
	newTopLeft.y = oldFrameImageRect.top;
	
	//フレームの開始行、終了行を取得
	AIndex	startLineIndex = GetLineIndexByImageY(oldFrameImageRect.top);
	AIndex	endLineIndex = GetLineIndexByImageY(oldFrameImageRect.bottom);
	
	//Y方向補正
	if( inTextPoint.y <= startLineIndex || inTextPoint.y >= endLineIndex )
	{
		newTopLeft.y = GetImageYByLineIndex(inTextPoint.y);//#450 inTextPoint.y * GetLineHeightWithMargin();
	}
	
	//Xは0
	newTopLeft.x = 0;
	
	//スクロール
	NVI_ScrollTo(newTopLeft);
	
	//#1031
	//キャレットのローカル位置を記憶
	SetLastCaretLocalPoint();
}

#pragma mark ===========================

#pragma mark ---フォーカス

void	AView_Text::EVTDO_DoViewFocusActivated()
{
	/*#688 これがあるとタブ切替時に先にキャレットが表示されてしまう。また、特に必要とは思われないので、削除する。
	if( IsCaretMode() == true )
	{
		ShowCaret();
	}
	*/
	/*高速化
	else
	{
		NVI_Refresh();
	}
	*/
	//#138
	//#858 SPI_UpdateFusen(false); 
	//#631
	//ツールメニュー更新
	GetApp().SPI_UpdateToolMenu(GetTextDocumentConst().SPI_GetModeIndex());
	
	//#725
	//見出しリストタイトル更新
	/*
	if( mTextWindowID != kObjectID_Invalid )
	{
		AText	text;
		//if( GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_IsSubTextView(NVI_GetControlID()) == false )
		{
			text.SetLocalizedText("SubWindowTitle_JumpList");
		}
		*
		else
		{
			text.SetLocalizedText("SubWindowTitle_JumpList_SubText");
		}
		*
		AText	title;
		NVM_GetWindow().NVI_GetTitle(NVM_GetWindow().NVI_GetCurrentTabIndex(),title);
		text.AddCstring("  (");
		text.AddText(title);
		text.AddCstring(")");
		GetApp().SPI_SetJumpListWindowTitle(mTextWindowID,text);
	}
	*/
	
	//#912
	//自動インデント文字設定
	//ドキュメントで推測されているインデント文字タイプをモード設定に設定する
	GetTextDocument().SPI_GuessIndentCharacterType();//#1003 推測も含めて実行する。（SPI_UpdateIndentCharacterTypeModePref()も中でコールされる）
	//#1003 GetTextDocument().SPI_UpdateIndentCharacterTypeModePref();
}

void	AView_Text::EVTDO_DoViewFocusDeactivated()
{
	if( IsCaretMode() == true )
	{
		HideCaret(false,false);
	}
	/*高速化
	else
	{
		NVI_Refresh();
	}
	*/
	//#138
	//#858 SPI_UpdateFusen(false);
	//#81
	//未保存データの保存トリガー
	GetTextDocument().SPI_AutoSave_Unsaved();
}

#pragma mark ===========================

#pragma mark ---検索・置換

/**
次を検索（インターフェイス）
*/
void	AView_Text::SPI_FindNext()
{
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	SPI_FindNext(param);
}
ABool	AView_Text::SPI_FindNext( const AFindParameter& inParam )
{
	//#1397
	//検索文字列が空のときは何もせずリターン
	if( inParam.findText.GetItemCount() == 0 )
	{
		return false;
	}
	
	//キー記録
	GetApp().SPI_RecordFindNext(inParam);
	
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	//
	return FindNext(inParam);
}

/**
次を検索（コア処理）
*/
ABool	AView_Text::FindNext( const AFindParameter& inParam )
{
	//行折り返し計算中は検索不可とする #1117
	ADocumentInitState	docState = SPI_GetTextDocument().SPI_GetDocumentInitState();
	if( docState != kDocumentInitState_SyntaxRecognizing && docState != kDocumentInitState_Completed )
	{
		AText	title, message;
		title.SetLocalizedText("Notification_FindResult_LineCalc_Title");
		message.SetLocalizedText("Notification_FindResult_LineCalc");
		SPI_ShowGeneralNotificationPopupWindow(title,message);
		return false;
	}
	
	AIndex	selectSpos, selectEpos;
	GetSelect(selectSpos,selectEpos);
	
	AIndex	start = selectEpos;
	AIndex	end = GetTextDocumentConst().SPI_GetTextLength();
	
	//返り値初期化
	ABool	result = false;
	//
	if( FindForward(inParam,start,end) == true )
	{
		result = true;
	}
	else
	{
		if( inParam.loop == true )
		{
			start = 0;
			end = selectEpos;
			if( FindForward(inParam,start,end) == true )
			{
				result = true;
			}
			else
			{
				ASoundWrapper::Beep();
				result = false;
			}
		}
		else
		{
			ASoundWrapper::Beep();
			result = false;
		}
	}
	//見つからない場合は、notification表示
	if( result == false )
	{
		//#846
		//モーダルセッション
		AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
		try
		{
			//前方向に検索
			AIndex	resultStart = 0, resultEnd = 0;
			ABool	modalSessionAborted = false;
			ABool	foundPrev = GetTextDocument().SPI_Find(inParam,true,0,selectSpos,resultStart,resultEnd,true,modalSessionAborted);
			ShowFindResultNotification(false,foundPrev,inParam.findText);
		}
		catch(...)
		{
			_ACError("",this);
		}
	}
	//
	return result;
}

/**
最初から検索（インターフェイス＋コア処理）
*/
ABool	AView_Text::SPI_FindFromFirst()
{
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	return SPI_FindFromFirst(param);
}
ABool	AView_Text::SPI_FindFromFirst( const AFindParameter& inParam )
{
	//#1397
	//検索文字列が空のときは何もせずリターン
	if( inParam.findText.GetItemCount() == 0 )
	{
		return false;
	}
	
	//キー記録
	GetApp().SPI_RecordFindFromFirst(inParam);
	
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	if( FindForward(inParam,0,GetTextDocumentConst().SPI_GetTextLength()) == true )
	{
		return true;
	}
	else
	{
		ASoundWrapper::Beep();
		return false;
	}
}

/**
前を検索（インターフェイス＋コア処理）
*/
void	AView_Text::SPI_FindPrev()
{
	AFindParameter	param;
	GetApp().SPI_GetFindParam(param);
	SPI_FindPrevious(param);
}
ABool	AView_Text::SPI_FindPrevious( const AFindParameter& inParam )
{
	//#1397
	//検索文字列が空のときは何もせずリターン
	if( inParam.findText.GetItemCount() == 0 )
	{
		return false;
	}
	
	//行折り返し計算中は検索不可とする #1117
	ADocumentInitState	docState = SPI_GetTextDocument().SPI_GetDocumentInitState();
	if( docState != kDocumentInitState_SyntaxRecognizing && docState != kDocumentInitState_Completed )
	{
		AText	title, message;
		title.SetLocalizedText("Notification_FindResult_LineCalc_Title");
		message.SetLocalizedText("Notification_FindResult_LineCalc");
		SPI_ShowGeneralNotificationPopupWindow(title,message);
		return false;
	}
	
	//キー記録
	GetApp().SPI_RecordFindPrevious(inParam);
	
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	AIndex	selectSpos, selectEpos;
	GetSelect(selectSpos,selectEpos);
	
	AIndex	start = 0;
	AIndex	end = selectSpos;
	
	//返り値初期化
	ABool	result = false;
	//
	if( FindBackward(inParam,start,end) == true )
	{
		result = true;
	}
	else
	{
		if( inParam.loop == true )
		{
			start = selectSpos;
			end = GetTextDocumentConst().SPI_GetTextLength();
			if( FindBackward(inParam,start,end) == true )
			{
				result = true;
			}
			else
			{
				ASoundWrapper::Beep();
				result = false;
			}
		}
		else
		{
			ASoundWrapper::Beep();
			result = false;
		}
	}
	//見つからない場合は、notification表示
	if( result == false )
	{
		//#846
		//モーダルセッション
		AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
		try
		{
			//次方向に検索
			AIndex	resultStart = 0, resultEnd = 0;
			ABool	modalSessionAborted = false;
			ABool	foundNext = GetTextDocument().SPI_Find(inParam,false,selectEpos,GetTextDocumentConst().SPI_GetTextLength(),
														   resultStart,resultEnd,true,modalSessionAborted);
			ShowFindResultNotification(foundNext,false,inParam.findText);
		}
		catch(...)
		{
			_ACError("",this);
		}
	}
	//
	return result;
}

const AItemCount	kPreHitTextLength = 100;
const AItemCount	kPostHitTextLength = 100;

/**
検索リスト（インターフェイス＋コア処理）
*/
void	AView_Text::SPI_FindList( const AFindParameter& inParam, const ABool inExtractMatchedText )
{
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	//結果表示ウインドウ表示
	AText	wintitle;
	wintitle.SetLocalizedText("IndexWindow_Title_FindResult");
	AText	grouptitle1, grouptitle2;
	grouptitle1.SetLocalizedText("FindResult_FindResult1");
	AText	datetime;
	ADateTimeWrapper::GetDateTimeText(datetime);
	grouptitle1.ReplaceParamText('0',datetime);
	grouptitle2.SetLocalizedText("FindResult_FindResult2");
	grouptitle2.ReplaceParamText('0',inParam.findText);
	AText	regexp;
	if( inParam.regExp == true )
	{
		regexp.SetLocalizedText("FindResult_RegExp");
	}
	grouptitle2.ReplaceParamText('1',regexp);
	//#92 ADocumentID	docID = GetApp().SPNVI_SelectOrCreateIndexWindowDocument(wintitle);
	//#92 検索結果をテキストウインドウ内に表示する設定追加
	ADocumentID	docID = kObjectID_Invalid;
	//#725 if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDisplayFindResultInTextWindow) == false || mTextWindowID == kObjectID_Invalid )
	{
		//別ウインドウに表示
		docID = GetApp().SPI_GetOrCreateFindResultWindowDocument();
	}
	/*#725
	else
	{
		//テキストウインドウ内に表示
		docID = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetOrCreateFindResultDocument();
	}
	*/
	AFileAcc	basefolder;//#465
	GetTextDocumentConst().NVI_GetParentFolder(basefolder);//#465
	GetApp().SPI_GetIndexWindowDocumentByID(docID).SPI_InsertGroup(0,grouptitle1,grouptitle2,basefolder);//#465
	
	//
	AText	returntext;
	returntext.SetLocalizedText("ReturnCodeText");
	AFileAcc	file;
	GetTextDocumentConst().NVI_GetFile(file);
	AText	filepath;
	if( GetTextDocumentConst().NVI_IsFileSpecified() == true )//#221
	{
		file.GetPath(filepath);
	}
	//#221 新規ドキュメントの場合は、ファイルパスにタブコード+DocumentIDを格納することにする
	else
	{
		filepath.Add(kUChar_Tab);
		filepath.AddNumber(GetApp().NVI_GetDocumentUniqID(GetTextDocumentConst().GetObjectID()).val);//#693
	}
	
	//#937
	AText	extractedText;
	
	//#846
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_FindList,true,false,true);
	ABool	aborted = false;
	AItemCount	hitcount = 0;//R0238
	try
	{
		//
		AItemCount	len = GetTextDocumentConst().SPI_GetTextLength();
		for( ATextIndex textpos = 0; textpos < len;  )
		{
			//モーダルセッション継続判定
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_FindList,hitcount,true,textpos,len) == false )
			{
				//キャンセル時
				aborted = true;
				break;
			}
			AIndex	resultStart, resultEnd;
			ABool	modalSessionAborted = false;
			if( GetTextDocument().SPI_Find(inParam,false,textpos,len,resultStart,resultEnd,true,modalSessionAborted) == true )
			{
				//一致テキスト抽出
				if( inExtractMatchedText == true )
				{
					AText	t;
					if( inParam.regExp == true && inParam.replaceText.GetItemCount() > 0 )
					{
						GetTextDocumentConst().SPI_ChangeReplaceText(inParam.replaceText,t);
					}
					else
					{
						GetTextDocumentConst().SPI_GetText(resultStart,resultEnd,t);
					}
					extractedText.AddText(t);
				}
				//
				AText	hit, prehit, posthit, positiontext;
				GetTextDocumentConst().SPI_GetText(resultStart,resultEnd,hit);
				ATextIndex	pos = resultStart - kPreHitTextLength;
				if( pos < 0 )   pos = 0;
				pos = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(pos);
				GetTextDocumentConst().SPI_GetText(pos,resultStart,prehit);//B0350
				pos = resultEnd + kPostHitTextLength;
				if( pos >= len )   pos = len;
				pos = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(pos);
				GetTextDocumentConst().SPI_GetText(resultEnd,pos,posthit);
				hit.ReplaceChar(kUChar_CR,returntext);
				prehit.ReplaceChar(kUChar_CR,returntext);
				posthit.ReplaceChar(kUChar_CR,returntext);
				//段落Index等取得
				ATextPoint	textpoint;
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(resultStart,textpoint);
				AIndex	paragraphIndex = GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(textpoint.y);
				//場所情報取得
				GetTextDocumentConst().SPI_GetPositionTextByLineIndex(textpoint.y,positiontext);
				//#465
				AText	paratext;
				GetTextDocumentConst().SPI_GetParagraphTextByLineIndex(textpoint.y,paratext);
				//
				GetApp().SPI_GetIndexWindowDocumentByID(docID).
						SPI_AddItem_TextPosition(0,filepath,hit,prehit,posthit,positiontext,
												 paratext,//#465
												 resultStart,resultEnd-resultStart,paragraphIndex,GetEmptyText());
				if( textpos < resultEnd )//B0117
				{
					textpos = resultEnd;
				}
				else
				{
					textpos = GetTextDocumentConst().SPI_GetNextCharTextIndex(textpos);
				}
				hitcount++;//R0238
			}
			else
			{
				//キャンセル、または、検索不一致時
				//キャンセル時は、modalSessionAbortedにtrueが入っている
				aborted = modalSessionAborted;
				break;
			}
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	//R0238
	AText	text;
	if( aborted == false )
	{
		text.SetLocalizedText("FindList_ResultCount");
	}
	else
	{
		text.SetLocalizedText("FindList_Aborted");
	}
	AText	numtext;
	numtext.AddNumber(hitcount);
	text.ReplaceParamText('0',numtext);
	AText	filecount("1");
	text.ReplaceParamText('1',filecount);
	AText	f,prehit,posthit,positiontext;
	GetApp().SPI_GetIndexWindowDocumentByID(docID).
			SPI_AddItem_TextPosition(0,f,text,prehit,posthit,positiontext,
			GetEmptyText(),//#465
									 0,0,-1,GetEmptyText());
	//抽出テキストを新規ドキュメントに表示
	if( inExtractMatchedText == true )
	{
		if( extractedText.GetItemCount() > 0 )
		{
			ADocumentID docID = GetApp().SPNVI_CreateNewTextDocument(kStandardModeIndex);
			if( docID != kObjectID_Invalid )
			{
				GetApp().SPI_GetTextDocumentByID(docID).SPI_InsertText(0,extractedText);
			}
		}
	}
}

/**
次を置換（インターフェイス）
*/
ABool	AView_Text::SPI_ReplaceNext( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return false;//B0000
	
	//キー記録 #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_Next);
	
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_SetFindHighlight(inParam);
	
	//コア処理
	return ReplaceNext(inParam);
}

/**
次を置換（コア処理）
*/
ABool	AView_Text::ReplaceNext( const AFindParameter& inParam )
{
	if( FindNext(inParam) == true )
	{
		AText	text;
		if( inParam.regExp == true )
		{
			GetTextDocumentConst().SPI_ChangeReplaceText(inParam.replaceText,text);
		}
		else
		{
			text.SetText(inParam.replaceText);
		}
		TextInput(undoTag_Replace1,text);
		return true;
	}
	else
	{
		return false;
	}
}

/**
キャレット以降を置換（インターフェイス）
*/
AItemCount	AView_Text::SPI_ReplaceAfterCaret( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return 0;//B0000
	
	//キー記録 #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_AfterCaret);
	
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_ClearFindHighlight();
	
	//
	ATextIndex	spos, epos;
	GetSelect(spos,epos);
	AItemCount	replacedCount;
	ABool	notaborted = Replace(inParam,epos,GetTextDocumentConst().SPI_GetTextLength(),replacedCount);
	//置換結果表示 #725
	ShowReplaceResultNotification(notaborted,replacedCount,inParam.findText,inParam.replaceText);
	
	return replacedCount;
}

/**
選択範囲内を置換（インターフェイス）
*/
AItemCount	AView_Text::SPI_ReplaceInSelection( const AFindParameter& inParam, const ABool inDontShowResultNotification )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return 0;//B0000
	
	//キー記録 #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_InSelection);
	
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_ClearFindHighlight();
	
	//
	if( mKukeiSelected == false )//#245
	{
		ATextIndex	spos, epos;
		GetSelect(spos,epos);
		AItemCount	replacedCount;
		ABool	notaborted = Replace(inParam,spos,epos,replacedCount);
		//置換結果表示 #725
		if( inDontShowResultNotification == false )
		{
			ShowReplaceResultNotification(notaborted,replacedCount,inParam.findText,inParam.replaceText);
		}
		if( notaborted == false )
		{
			return -1;
		}
		else
		{
			return replacedCount;
		}
	}
	else//#245 矩形選択中の場合（下記処理は矩形選択中でなくても多分OKだが、影響度を下げるために従来処理を使うことにした）
	{
		AArray<ATextIndex>	sposArray, eposArray;
		SPI_GetSelect(sposArray,eposArray);
		ABool	notaborted = true;
		AItemCount	totalRepleacedCount = 0;
		AItemCount	totalOffset = 0;
		for( AIndex i = 0; i < sposArray.GetItemCount(); i++ )
		{
			AItemCount	rc = 0;
			AItemCount	of = 0;
			notaborted = Replace(inParam,sposArray.Get(i)+totalOffset,eposArray.Get(i)+totalOffset,rc,of);
			totalRepleacedCount += rc;
			totalOffset += of;
			//キャンセル時はbreak
			if( notaborted == false )
			{
				break;
			}
		}
		//置換結果表示 #725
		if( inDontShowResultNotification == false )
		{
			ShowReplaceResultNotification(notaborted,totalRepleacedCount,inParam.findText,inParam.replaceText);
		}
		if( notaborted == false )
		{
			return -1;
		}
		else
		{
			return totalRepleacedCount;
		}
	}
}

/**
全てを置換（インターフェイス）
*/
AItemCount	AView_Text::SPI_ReplaceAll( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return 0;//B0000
	
	//キー記録 #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_All);
	
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_ClearFindHighlight();
	
	//
	AItemCount	replacedCount;
	ABool	notaborted = Replace(inParam,0,GetTextDocumentConst().SPI_GetTextLength(),replacedCount);
	//置換結果表示 #725
	ShowReplaceResultNotification(notaborted,replacedCount,inParam.findText,inParam.replaceText);
	return replacedCount;
}

/**
選択テキストのみ置換
*/
ABool	AView_Text::SPI_ReplaceSelectedTextOnly( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return false;//B0000
	
	//キー記録 #887
	GetApp().SPI_RecordReplace(inParam,kReplaceRangeType_ReplaceSelectedTextOnly);
	
	//最近使った検索・置換文字列に追加
	GetApp().SPI_AddRecentlyUsedFindText(inParam);
	GetApp().SPI_AddRecentlyUsedReplaceText(inParam);
	
	//検索ハイライト設定
	GetTextDocument().SPI_ClearFindHighlight();
	
	//
	if( SPI_CompareFindParamAndSelectedText(inParam) == true )
	{
		ATextPoint	spt, ept;
		SPI_GetSelect(spt,ept);
		SetCaretTextPoint(spt);
		return ReplaceNext(inParam);
	}
	return false;
}

/**
置換後次を検索
*/
ABool	AView_Text::SPI_ReplaceAndFind( const AFindParameter& inParam )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return false;//B0000
	
	//「選択テキストのみ置換」＋「次を検索」を実行する。
	//キー記録はそれぞれの関数内で実行。
	
	//
	if( SPI_ReplaceSelectedTextOnly(inParam) == true )
	{
		return SPI_FindNext(inParam);
	}
	return false;
}

//#935
/**
一括置換
*/
void	AView_Text::SPI_BatchReplace( const AFindParameter& inParam, const AText& inBatchText )
{
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_BatchReplace,true,false,true);
	//パラメータ取得
	AFindParameter	param;
	param.Set(inParam);
	//一括置換テキストの項目（2段落が1項目）ごとのループ
	AItemCount	len = inBatchText.GetItemCount();
	for( AIndex pos = 0; pos < len; )
	{
		//モーダルセッション継続判定
		if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_BatchReplace,0,true,pos,len) == false )
		{
			break;
		}
		
		//検索テキスト、置換テキスト取得
		inBatchText.GetLine(pos,param.findText);
		inBatchText.GetLine(pos,param.replaceText);
		//置換実行
		if( SPI_ReplaceInSelection(param,true) < 0 )
		{
			break;
		}
	}
}

//#725
/**
置換結果notification表示
*/
void	AView_Text::ShowReplaceResultNotification( const ABool inNotAborted, const ANumber inReplacedCount,
												  const AText& inFindText, const AText& inReplaceText )
{
	//検索結果表示オプションがOFFなら何もせずリターン #1322
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowFindResultPopup) == false )
	{
		return;
	}
	
	//
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_ReplaceResult(inNotAborted,inReplacedCount,inFindText,inReplaceText);
		SPI_ShowNotificationPopupWindow(true);
	}
}

//#725
/**
検索結果notification表示
*/
void	AView_Text::ShowFindResultNotification( const ABool inFoundInNext, const ABool inFoundInPrev,
											   const AText& inFindText )
{
	//検索結果表示オプションがOFFなら何もせずリターン #1322
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kShowFindResultPopup) == false )
	{
		return;
	}
	
	//
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_FindResult(inFoundInNext,inFoundInPrev,inFindText);
		SPI_ShowNotificationPopupWindow(true);
	}
}

//検索テキストと、選択中の内容を比較する（現在選択中の内容が検索一致したらtrueを返す）
ABool	AView_Text::SPI_CompareFindParamAndSelectedText( const AFindParameter& inParam )
{
	ATextIndex	selStart, selEnd, resultStart, resultEnd;
	GetSelect(selStart,selEnd);
	if( selStart == selEnd )   return false;
	ABool	modalSessionAborted = false;
	if( GetTextDocument().SPI_Find(inParam,false,selStart,selEnd,resultStart,resultEnd,false,modalSessionAborted) == true )
	{
		return ((selStart==resultStart)&&(selEnd==resultEnd));
	}
	return false;
}

//inStartからinEndの範囲内で検索し、最初に見つかったものを選択する
ABool	AView_Text::FindForward( const AFindParameter& inParam, const AIndex inStart, const AIndex inEnd )
{
	AIndex	resultStart, resultEnd;
	//#846
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
	ABool	result = false;
	try
	{
		//
		ABool	modalSessionAborted = false;
		if( GetTextDocument().SPI_Find(inParam,false,inStart,inEnd,resultStart,resultEnd,true,modalSessionAborted) == true )
		{
			SetSelect(resultStart,resultEnd,true);//#262
			AdjustScroll_Center(GetCaretTextPoint());
			result = true;
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	return result;
}

//inStartからinEndの範囲内で検索し、最後に見つかったものを選択する
ABool	AView_Text::FindBackward( const AFindParameter& inParam, const AIndex inStart, const AIndex inEnd )
{
	AIndex	resultStart, resultEnd;
	//#846
	//モーダルセッション
	AStEditProgressModalSession	modalSession(kEditProgressType_Find,true,false,true);
	ABool	result = false;
	try
	{
		//
		ABool	modalSessionAborted = false;
		if( GetTextDocument().SPI_Find(inParam,true,inStart,inEnd,resultStart,resultEnd,true,modalSessionAborted) == true )
		{
			SetSelect(resultStart,resultEnd,true);//#262
			AdjustScroll_Center(GetCaretTextPoint());
			result = true;
		}
	}
	catch(...)
	{
		_ACError("",this);
	}
	return result;
	/*#789
	AIndex	resultStart, resultEnd;
	if( GetTextDocument().SPI_Find(inParam,inStart,inEnd,resultStart,resultEnd) == false )
	{
		return false;
	}
	else
	{
		AIndex	finalResultStart = resultStart;
		AIndex	finalResultEnd = resultEnd;
		AIndex	start = resultEnd;
		AIndex	end = inEnd;
		while( GetTextDocument().SPI_Find(inParam,start,end,resultStart,resultEnd) == true )
		{
			finalResultStart = resultStart;
			finalResultEnd = resultEnd;
			if( start == resultEnd )//B0117 無限ループ防止
			{
				resultEnd = GetTextDocumentConst().SPI_GetNextCharTextIndex(resultEnd);
				if( resultEnd >= GetTextDocumentConst().SPI_GetTextLength() )   break;
			}
			start = resultEnd;
		}
		SetSelect(finalResultStart,finalResultEnd,true);//#262
		AdjustScroll_Center(GetCaretTextPoint());
		return true;
	}
	*/
}

//指定範囲内置換
ABool	AView_Text::Replace( const AFindParameter& inParam, const ATextIndex inStart, const ATextIndex inEnd, AItemCount& outReplacedCount,
		AItemCount& outOffset )//#245
{
	//#65
	//選択取得
	ATextIndex	selectSpos = 0, selectEpos = 0;
	GetSelect(selectSpos,selectEpos);
	//置換実行
	ABool	result = GetTextDocument().SPI_ReplaceText(inParam,inStart,inEnd,outReplacedCount,outOffset,selectSpos,selectEpos);
	
	/*#65
	outOffset = 0;//#245
	outReplacedCount = 0;
	
	ATextIndex	selectSpos, selectEpos;
	GetSelect(selectSpos,selectEpos);
	
	ATextIndex	startPos = inStart;
	ATextIndex	endPos = inEnd;
	
	//検討必要 選択解除、HideCaret()必要？
	
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Replace);
	
	//１つ目の検索
	ATextIndex	replaceStart1, replaceEnd1;
	if( GetTextDocument().SPI_Find(inParam,false,startPos,endPos,replaceStart1,replaceEnd1) == false )
	{
		return;
	}
	
	//置換文字列作成
	AText	replaceText1;
	if( inParam.regExp == true )
	{
		GetTextDocumentConst().SPI_ChangeReplaceText(inParam.replaceText,replaceText1);
	}
	else
	{
		replaceText1.SetText(inParam.replaceText);
	}
	//次の検索開始位置
	if( replaceStart1 == replaceEnd1 )
	{
		if( replaceEnd1 >= endPos )
		{
			startPos = endPos+1;//下までいったら終わらせる
		}
		else
		{
			startPos = GetTextDocumentConst().SPI_GetNextCharTextIndex(replaceEnd1);
		}
	}
	else
	{
		startPos = replaceEnd1;
	}
	
	//２つ目以降の検索
	AText	replaceText2;
	while(true) 
	{
		//次の検索
		ABool	found = false;
		ATextIndex	replaceStart2, replaceEnd2;
		if( startPos <= endPos )
		{
			found = GetTextDocument().SPI_Find(inParam,false,startPos,endPos,replaceStart2,replaceEnd2);
			//次の検索開始位置
			if( replaceStart2 == replaceEnd2 )
			{
				if( replaceEnd2 >= endPos )
				{
					startPos = endPos+1;//下までいったら終わらせる
				}
				else
				{
					startPos = GetTextDocumentConst().SPI_GetNextCharTextIndex(replaceEnd2);
				}
			}
			else
			{
				startPos = replaceEnd2;
			}
		}
		else
		{
			replaceStart2 = replaceEnd2 = startPos;
		}
		
		//B0182 まずreplaceText2に次の置換文字列を作成して保存する
		//次の置換文字列の作成
		replaceText2.DeleteAll();
		if( found == true )
		{
			if( inParam.regExp == true )
			{
				GetTextDocumentConst().SPI_ChangeReplaceText(inParam.replaceText,replaceText2);
			}
			else
			{
				replaceText2.SetText(inParam.replaceText);
			}
		}
		
		//１つ前に検索した部分の置換
		DeleteTextFromDocument(replaceStart1,replaceEnd1,true);//dont redraw
		if( replaceText1.GetItemCount() > 0 )
		{
			InsertTextToDocument(replaceStart1,replaceText1,true);//#846
		}
		outReplacedCount++;
		//置換による検索範囲の移動
		AItemCount	offset = replaceText1.GetItemCount() - (replaceEnd1-replaceStart1);
		startPos += offset;
		endPos += offset;
		outOffset += offset;//#245
		
		if( replaceStart1 < selectSpos ) 	selectSpos += offset;
		if( replaceEnd1 <= selectEpos )  	selectEpos += offset;
		
		//replaceStart1/replaceEnd1の更新
		replaceStart1 	= replaceStart2 + offset;
		replaceEnd1 	= replaceEnd2 + offset;
		
		if( found == false )   break;
		
		//次の置換文字列をreplaceText1にコピー
		replaceText1.SetText(replaceText2);
	}
	*/
	//#212 GetTextDocument().SPI_RefreshWindow();
	GetTextDocument().SPI_RefreshTextViews();
	//元々の選択範囲を選択し直す
	SetSelect(selectSpos,selectEpos);
	
	return result;
}

//#724
/**
「次を検索」ツールバー項目クリック時処理
*/
void	AView_Text::SPI_FindSelectedNext()
{
	//選択テキスト取得（キャレット時は、単語を選択）
	AText	text;
	GetSelectedText(text);
	if( mCaretMode == true )
	{
		ATextIndex	wordStart = 0, wordEnd = 0;
		GetTextDocumentConst().SPI_FindWord(GetTextDocument().SPI_GetTextIndexFromTextPoint(mCaretTextPoint),wordStart,wordEnd);
		GetTextDocumentConst().SPI_GetText(wordStart,wordEnd,text);
	}
	//Findパラメータ設定
	AFindParameter	param;
	param.findText.SetText(text);
	param.regExp = false;
	param.loop = false;
	//検索実行
	SPI_FindNext(param);
}

//#724
/**
「前を検索」ツールバー項目クリック時処理
*/
void	AView_Text::SPI_FindSelectedPrevious()
{
	//選択テキスト取得（キャレット時は、単語を選択）
	AText	text;
	GetSelectedText(text);
	if( mCaretMode == true )
	{
		ATextIndex	wordStart = 0, wordEnd = 0;
		GetTextDocumentConst().SPI_FindWord(GetTextDocument().SPI_GetTextIndexFromTextPoint(mCaretTextPoint),wordStart,wordEnd);
		GetTextDocumentConst().SPI_GetText(wordStart,wordEnd,text);
	}
	//Findパラメータ設定
	AFindParameter	param;
	param.findText.SetText(text);
	param.regExp = false;
	param.loop = false;
	//検索実行
	SPI_FindPrevious(param);
}

#pragma mark ===========================

#pragma mark ---補完
//#717

/**
リアルタイム補完検索実行
@note 条件を満たす場合のみ検索実行する。
*/
void	AView_Text::FindCandidateRealtime()
{
	//候補入力中状態のときは、リアルタイム候補検索はしない（新たな候補検索はしない）
	if( mAbbrevInputMode == true )
	{
		return;
	}
	
	//現在のキャレット位置取得
	ATextIndex	textIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	if( textIndex == 0 )   return;
	//現在単語取得
	ATextIndex	wordSpos = 0, wordEpos = 0;
	ATextIndex	prevCharPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textIndex);
	ABool	isAlphabet = GetTextDocumentConst().SPI_FindWordForKeyword(prevCharPos,wordSpos,wordEpos);
	AText	word;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
	
	//==================補完再変換==================
	//最後に補完入力で確定した単語と、現在単語が同じ時は、最後の補完入力状態を継続する（今回、新たな候補検索はしない）
	if( mLastAbbrevInput_Word.GetItemCount() > 0 && mLastAbbrevInput_Word.Compare(word) == true )
	{
		mAbbrevInputMode = true;
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(true,mAbbrevInputMode);
		return;
	}
	
	//==================補完検索要求するかどうかの判定==================
	//候補検索するかどうかのフラグ
	ABool	requestCandidateFinder = false;
	//単語が2文字以上なら検索実行
	if( wordEpos - wordSpos >= 2 && isAlphabet == true )
	{
		requestCandidateFinder = true;
	}
	//
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
	//クラス文字の後なら検索実行
	if( requestCandidateFinder == false )
	{
		//クラス文字(::等)の後かどうかを判断
		AItemCount	classlen = 0;
		if( syntaxDefinition.IsAfterClassString(GetTextDocumentConst().SPI_GetTextConst(),textIndex,classlen) == true )
		{
			requestCandidateFinder = true;
		}
		if( syntaxDefinition.IsAfterInstanceString(GetTextDocumentConst().SPI_GetTextConst(),textIndex,classlen) == true )
		{
			requestCandidateFinder = true;
		}
	}
	//状態が「クラス後」であれば、検索実行
	if( requestCandidateFinder == false )
	{
		ATextPoint	textpt = {0};
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textIndex,textpt);
		AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(textpt);
		if( syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsAfterClassState() == true )
		{
			requestCandidateFinder = true;
		}
	}
	//==================候補検索スレッド実行==================
	if( requestCandidateFinder == true )
	{
		//fprintf(stderr,"RequestCandidateFinder(1.TextInput) ");
		RequestCandidateFinder();
	}
	//==================候補検索スレッド実行しない==================
	else if( isAlphabet == false )
	{
		//アルファベット以外入力時はポップアップ消去
		if( mTextWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,false);
		}
	}
}

/**
補完候補用現在単語取得
*/
ABool	AView_Text::GetCurrentWordForAbbrev( AIndex& outWordSpos, AIndex& outWordEpos ) const
{
	//結果初期化
	outWordSpos = 0;
	outWordEpos = 0;
	//現在のキャレット位置取得
	ATextIndex	textIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	//ドキュメント最初なら何もしない
	if( textIndex == 0 )   return false;
	//単語取得
	ATextIndex	prevCharPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textIndex);
	ABool	isWord = GetTextDocumentConst().SPI_FindWordForKeyword(prevCharPos,outWordSpos,outWordEpos);
	//単語でない場合は、結果はキャレット位置にする。
	if( isWord == false )
	{
		outWordSpos = outWordEpos = textIndex;
	}
	return true;
}

/**
テキストから指定位置の親キーワード取得
*/
ABool	AView_Text::GetParentKeyword( const AIndex inTextIndex, AText& outParentKeyword ) const
{
	//結果初期化
	outParentKeyword.DeleteAll();
	//
	ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
	//text point取得
	ATextPoint	textpt = {0};
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inTextIndex,textpt);
	//状態取得
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(textpt);
	ABool	isAfterClassState = syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsAfterClassState();
	/*
	if( isAfterClassState == true )
	{
		syntaxDefinition.GetSyntaxDefinitionState(stateIndex).GetAfterClassState_ParentKeyword(outParentKeyword);
		if( outParentKeyword.GetItemCount() > 0 )
		{
			//キーワード情報スレッドに検索要求
			GetApp().SPI_GetIdInfoFinderThread().SetRequestData(GetTextDocumentConst().GetObjectID(),
																kObjectID_Invalid,mTextWindowID,GetObjectID(),
																outParentKeyword,GetEmptyText(),0,GetEmptyText(),
																stateIndex);
			//スレッドunpause
			GetApp().SPI_GetIdInfoFinderThread().NVI_UnpauseIfPaused();
			return;
		}
	}
	*/
	AItemCount	classlen = 0;
	//==================インスタンス文字の後なら直前のキーワードのタイプを親キーワードに設定==================
	ABool	afterInstanceString = false;
	if( isAfterClassState == false )
	{
		//インスタンス文字(.等)の後かどうかを判断
		afterInstanceString = syntaxDefinition.IsAfterInstanceString(GetTextDocumentConst().SPI_GetTextConst(),inTextIndex,classlen);
	}
	else
	{
		//状態が「クラス後」なら常にインスタンス文字の後として扱う
		afterInstanceString = true;
		classlen = 1;
	}
	if( afterInstanceString == true )
	{
		//インスタンス文字の後なら、インスタンス変数のtypeをparent keywordとして取得
		ATextIndex	p = inTextIndex-classlen-1;
		if( p >= 0 )
		{
			//GetApp().などの場合、()の前の識別子を対象にする。
			AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(p);
			if( ch == ')' || ch == ']' || ch == '}' )
			{
				ATextPoint	sp, ep;
				GetTextDocumentConst().SPI_GetTextPointFromTextIndex(p,sp);
				ep = sp;
				GetTextDocumentConst().SPI_GetBraceSelection(sp,ep,true);
				ATextIndex	pp = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(sp) -2;
				if( pp >= 0 )   p = pp;
			}
			AText	instancetext;
			AIndex	s,e;
			GetTextDocumentConst().SPI_FindWordForKeyword(p,s,e);
			GetTextDocumentConst().SPI_GetText(s,e,instancetext);
			if( GetTextDocumentConst().SPI_GetTypeOfKeyword(instancetext,outParentKeyword,GetObjectID()) == true )
			{
				return true;
			}
		}
	}
	//==================クラス文字の後なら直前のキーワードを親キーワードに設定==================
	ABool	afterClassString = false;
	if( isAfterClassState == false )
	{
		//クラス文字(::等)の後かどうかを判断
		afterClassString = syntaxDefinition.IsAfterClassString(GetTextDocumentConst().SPI_GetTextConst(),inTextIndex,classlen);
	}
	else
	{
		//状態が「クラス後」なら常にクラス文字の後として扱う
		afterClassString = true;
		classlen = 1;
	}
	if( afterClassString == true )
	{
		//クラス文字の後なら、クラスをparent keywordとして取得
		ATextIndex	p = inTextIndex-classlen-1;
		if( p >= 0 )
		{
			AIndex	s,e;
			GetTextDocumentConst().SPI_FindWordForKeyword(p,s,e);//たとえば、AText::のあとにキャレットがある場合、tの前からword検索する
			GetTextDocumentConst().SPI_GetText(s,e,outParentKeyword);
			return true;
		}
	}
	//==================ローカル開始識別子の親キーワード（＝典型的には関数のクラス）を取得==================
	GetTextDocumentConst().SPI_GetCurrentLocalStartIdentifierParentKeyword(textpt.y,outParentKeyword);
	return false;
}

/**
補完候補検索スレッドに検索要求
*/
ABool	AView_Text::RequestCandidateFinder()
{
	//現在単語取得
	ATextIndex	wordSpos = 0, wordEpos = 0;
	if( GetCurrentWordForAbbrev(wordSpos,wordEpos) == false )
	{
		//現在単語を取得できない時は、候補ポップアップウインドウを非表示にして終了
		if( mTextWindowID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,false);
		}
		return false;
	}
	AText	word;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
	
	//parent keyword取得
	AText	parentKeyword;
	ABool	afterParentKeyword = GetParentKeyword(wordSpos,parentKeyword);
	//★ここで継承クラスをparentKeywordArrayに入れたいが。。。
	ATextArray	parentKeywordArray;
	if( parentKeyword.GetItemCount() > 0 )
	{
		parentKeywordArray.Add(parentKeyword);
	}
	
	//検索要求
	if( afterParentKeyword == true || word.GetItemCount() > 0 )
	{
		//親キーワードの後、または、キャレット位置に単語がある場合のみ、候補要求する
		RequestCandidateFinder(word,parentKeywordArray,wordEpos);
		return true;
	}
	else
	{
		return false;
	}
}
void	AView_Text::RequestCandidateFinder( const AText& inWord, const ATextArray& inParentWord, const ATextIndex inTextIndex )
{
	//suppress中なら何もしない
	if( mSuppressRequestCandidateFinder == true )
	{
		return;
	}
	
	//候補検索したテキストを記憶
	mAbbrevCurrentRequestWord.SetText(inWord);
	
	//状態設定（状態：要求中）
	mAbbrevCandidateState_Requesting = true;
	mAbbrevCandidateState_Displayed = false;
	
	//検索要求データ設定
	ATextPoint	textpt = {0};
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inTextIndex,textpt);
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(textpt);
	GetApp().SPI_GetCandidateFinderThread().SetRequestData(GetTextDocumentConst().GetObjectID(),GetObjectID(),
				mAbbrevCurrentRequestWord,inParentWord,stateIndex);
	//スレッドunpause
	GetApp().SPI_GetCandidateFinderThread().NVI_UnpauseIfPaused();
	
	//新たな候補検索を行ったので、最後の補完入力確定単語を消去する
	mLastAbbrevInput_Word.DeleteAll();
	mLastAbbrevInput_CompletionText.DeleteAll();
}

/**
キャレット位置にヒントテキストが存在していたらヒントテキストを親キーワードとして候補検索要求する
*/
void	AView_Text::RequestCandidateFinderIfHintExist()
{
	//状態：要求中、表示中ではない場合のみ候補検索要求する
	if( mAbbrevCandidateState_Requesting == false && mAbbrevCandidateState_Displayed == false )
	{
		//キャレット位置のヒントテキスト取得
		ATextIndex	textIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
		AText	hintText;
		GetTextDocumentConst().SPI_GetHintText(textIndex,hintText);
		//ヒントテキストの半角スペース前までを親キーワードとして使用する
		AIndex	pos = 0;
		if( hintText.FindCstring(0," ",pos) == true )
		{
			hintText.DeleteAfter(pos);
		}
		//ヒントテキスト存在すれば、候補検索実行
		if( hintText.GetItemCount() > 0 )
		{
			//親キーワードに設定
			ATextArray	parentWordArray;
			if( hintText.GetItemCount() > 0 )
			{
				parentWordArray.Add(hintText);
			}
			//候補検索要求
			//fprintf(stderr,"RequestCandidateFinder(4.HintText) ");
			RequestCandidateFinder(GetEmptyText(),parentWordArray,textIndex);
		}
	}
}

/**
IdInfo表示時に、IdInfoの識別子の親キーワードに対応するキーワード候補を表示する（re-completion）
*/
void	AView_Text::SPI_RequestCandidateFromParentKeyword( const ATextArray& inParentWord )
{
	//状態：要求中、表示中ではない場合のみ候補検索要求する
	if( mAbbrevCandidateState_Requesting == false && mAbbrevCandidateState_Displayed == false )
	{
		//SDFでre-completion可能状態として定義されているかを取得
		ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
		AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(mCaretTextPoint);
		if( syntaxDefinition.GetSyntaxDefinitionState(stateIndex).IsEnableRecompletion() == true )
		{
			//空でないparent wordのみ、parentWordArrayに格納
			ATextArray	parentWordArray;
			for( AIndex i = 0; i < inParentWord.GetItemCount(); i++ )
			{
				AText	parentWord;
				inParentWord.Get(i,parentWord);
				if( parentWord.GetItemCount() > 0 )
				{
					parentWordArray.Add(parentWord);
				}
			}
			//parentWordArrayがあれば候補検索要求する
			if( parentWordArray.GetItemCount() > 0 )
			{
				ATextIndex	textIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
				//fprintf(stderr,"RequestCandidateFinder(3.IdInfoPopup(Parent Keyword match)) ");
				RequestCandidateFinder(GetEmptyText(),parentWordArray,textIndex);
			}
		}
	}
}

/**
補完候補スレッド結果処理
*/
void	AView_Text::SPI_DoCandidateFinderPaused()
{
	//要求時word取得
	AText	word;
	ATextArray	parentword;
	GetApp().SPI_GetCandidateFinderThread().GetWordForResult(word,parentword);
	//現在の候補index初期化
	mAbbrevCurrentCandidateIndex = kIndex_Invalid;
	//最後の変換候補テキスト初期化
	mLastAbbrevInput_Word.DeleteAll();
	mLastAbbrevInput_CompletionText.DeleteAll();
	//検索結果取得
	mAbbrevCandidateArray_Keyword.DeleteAll();
	mAbbrevCandidateArray_InfoText.DeleteAll();
	mAbbrevCandidateArray_CompletionText.DeleteAll();
	mAbbrevCandidateArray_CategoryIndex.DeleteAll();
	mAbbrevCandidateArray_ScopeType.DeleteAll();
	mAbbrevCandidateArray_FilePath.DeleteAll();
	mAbbrevCandidateArray_Priority.DeleteAll();
	mAbbrevCandidateArray_MatchedCount.DeleteAll();
	mAbbrevCandidateArray_ParentKeyword.DeleteAll();
	GetApp().SPI_GetCandidateFinderThread().
			GetResult(
					  mAbbrevCandidateArray_Keyword,mAbbrevCandidateArray_InfoText,mAbbrevCandidateArray_CategoryIndex,
					  mAbbrevCandidateArray_ScopeType,mAbbrevCandidateArray_FilePath,mAbbrevCandidateArray_Priority,
					  mAbbrevCandidateArray_ParentKeyword,mAbbrevCandidateArray_CompletionText,
					  mAbbrevCandidateArray_MatchedCount);
	//検索結果有無による処理分岐
	if( mAbbrevCandidateArray_Keyword.GetItemCount() >0 && mAbbrevCandidateState_Requesting == true )
	{
		//==================候補有り==================
		
		//候補結果を補完候補リストに表示
		GetApp().SPI_UpdateCandidateListWindows(GetTextDocumentConst().SPI_GetModeIndex(),
					NVM_GetWindow().GetObjectID(),NVI_GetControlID(),
					mAbbrevCandidateArray_Keyword,mAbbrevCandidateArray_InfoText,mAbbrevCandidateArray_CategoryIndex,
					mAbbrevCandidateArray_ScopeType,mAbbrevCandidateArray_FilePath,mAbbrevCandidateArray_Priority,
					mAbbrevCandidateArray_ParentKeyword,mAbbrevCandidateArray_MatchedCount);
		
		if( mAbbrevInputMode == true )
		{
			//==================候補入力中状態の場合==================
			//最初／最後の候補をテキストに入力
			
			//現在単語範囲取得
			ATextIndex	wordSpos = 0, wordEpos = 0;
			if( GetCurrentWordForAbbrev(wordSpos,wordEpos) == true )
			{
				AText	word;
				GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
				
				//補完入力キーを押した回数に従って、現在の項目indexを設定
				if( mAbbrevRequestInputKeyCount >= 0 )
				{
					//プラス・0の場合
					mAbbrevCurrentCandidateIndex = mAbbrevRequestInputKeyCount;
					if( mAbbrevCurrentCandidateIndex >= mAbbrevCandidateArray_Keyword.GetItemCount() )
					{
						mAbbrevCurrentCandidateIndex = mAbbrevCandidateArray_Keyword.GetItemCount()-1;
					}
				}
				else
				{
					//マイナスの場合
					mAbbrevCurrentCandidateIndex = mAbbrevCandidateArray_Keyword.GetItemCount() + mAbbrevRequestInputKeyCount;
					if( mAbbrevCurrentCandidateIndex < 0 )
					{
						mAbbrevCurrentCandidateIndex = 0;
					}
				}
				
				//候補項目のkeyword取得
				AText	keyword;
				mAbbrevCandidateArray_Keyword.Get(mAbbrevCurrentCandidateIndex,keyword);
				
				if( keyword.Compare(word) == false )
				{
					//Undoアクションタグ記録
					GetTextDocument().SPI_RecordUndoActionTag(undoTag_AbbrevInput);
					
					//現在単語選択
					SetSelect(wordSpos,wordEpos);
					//選択範囲削除、テキスト挿入
					DeleteAndInsertTextToDocument(keyword);
				}
				
				//最後に補完した単語を記憶
				mLastAbbrevInput_Word.SetText(keyword);
				mAbbrevCandidateArray_CompletionText.Get(mAbbrevCurrentCandidateIndex,mLastAbbrevInput_CompletionText);
				
				//#750
				//現在単語ハイライト
				//（補完選択間違い防止になる。）
				UpdateCurrentWordHighlight(keyword);
			}
		}
		else
		{
			mAbbrevCurrentCandidateIndex = kIndex_Invalid;
		}
		
		//==================
		if( mTextWindowID != kObjectID_Invalid )
		{
			//候補ポップアップウインドウ表示
			//候補が1つのみ、かつ、要求時のワードと同じ単語のみならポップアップ非表示
			ABool	shouldPopup = true;
			if( mAbbrevCandidateArray_Keyword.GetItemCount() == 1 )
			{
				if( word.Compare(mAbbrevCandidateArray_Keyword.GetTextConst(0)) == true )
				{
					shouldPopup = false;
					//------------------キーワード情報を要求------------------
					RequestIdInfo(word,parentword,0,mCaretTextPoint);
				}
			}
			//候補ポップアップウインドウ表示・非表示
			if( shouldPopup == true )
			{
				//候補ポップアップウインドウ表示
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(true,mAbbrevInputMode);
			}
			else
			{
				//候補ポップアップウインドウ非表示
				GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,mAbbrevInputMode);
			}
		}
		
		//候補ウインドウの項目を選択
		GetApp().SPI_SetCandidateListWindowsSelectedIndex(NVM_GetWindow().GetObjectID(),mAbbrevCurrentCandidateIndex);
		//状態：表示中
		mAbbrevCandidateState_Requesting = false;
		mAbbrevCandidateState_Displayed = true;
	}
	else
	{
		//==================候補なし==================
		
		//候補入力状態解除
		mAbbrevInputMode = false;
		
		if( mTextWindowID != kObjectID_Invalid )
		{
			//候補ウインドウの項目クリックによる入力を無効にする
			GetApp().SPI_InvalidateCandidateListWindowClick(mTextWindowID);
			//候補ポップアップウインドウ非表示
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,mAbbrevInputMode);
		}
		//状態：アイドル
		mAbbrevCandidateState_Requesting = false;
		mAbbrevCandidateState_Displayed = false;
	}
}

/**
補完キー入力時処理
*/
ABool	AView_Text::InputAbbrev( const ABool inNext )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return false;
	
	//現在の位置の単語取得
	ATextIndex	wordSpos = 0, wordEpos = 0;
	if( GetCurrentWordForAbbrev(wordSpos,wordEpos) == false )   return false;
	AText	word;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
	
	//==================補完再変換==================
	//最後に補完入力で確定した単語と、現在単語が同じ時は、最後の補完入力状態を継続する
	if( mLastAbbrevInput_Word.GetItemCount() > 0 && mLastAbbrevInput_Word.Compare(word) == true )
	{
		mAbbrevInputMode = true;
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(true,mAbbrevInputMode);
	}
	
	if( mAbbrevInputMode == false )
	{
		//==================候補入力中フラグOFFの場合==================
		//候補検索を実施する。
		//候補入力中フラグをONにして候補検索スレッドに要求を送る。
		//候補検索スレッドから応答がかえってきたときに、候補入力中フラグがONなので、最初の候補がドキュメントに入力される
		
		//次キーか前キーかを記憶。
		if( inNext == true )
		{
			mAbbrevRequestInputKeyCount = 0;
		}
		else
		{
			mAbbrevRequestInputKeyCount = -1;
		}
		//テキスト挿入位置にヒントテキストがあれば削除する
		ATextIndex	caretIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
		RemoveHintText(caretIndex);
		//候補入力状態フラグONにする
		mAbbrevInputMode = true;
		//候補検索実行
		//fprintf(stderr,"RequestCandidateFinder(2.Abbrev key input) ");
		return RequestCandidateFinder();
	}
	else
	{
		//==================候補入力中フラグONの場合==================
		
		if( mAbbrevCandidateState_Requesting == true )
		{
			//==================候補検索要求中の場合==================
			//候補検索スレッド要求中に先に補完キーを入力された場合、まだ候補データが出来ていないので、その場合は何もしない。
			//キー数記録
			if( inNext == true )
			{
				mAbbrevRequestInputKeyCount++;
			}
			else
			{
				mAbbrevRequestInputKeyCount--;
			}
		}
		else
		{
			//==================候補検索完了後の場合==================
			//次キー
			if( inNext == true )
			{
				//候補index次へ
				mAbbrevCurrentCandidateIndex++;
				//最後までいったら最初に戻る
				if( mAbbrevCurrentCandidateIndex >= mAbbrevCandidateArray_Keyword.GetItemCount() )
				{
					mAbbrevCurrentCandidateIndex = 0;
				}
			}
			//前キー
			else
			{
				//候補index前へ
				mAbbrevCurrentCandidateIndex--;
				//最初までいったら最後に移動
				if( mAbbrevCurrentCandidateIndex < 0 )
				{
					mAbbrevCurrentCandidateIndex = mAbbrevCandidateArray_Keyword.GetItemCount()-1;
				}
			}
			
			//キーワード取得
			AText	keyword;
			mAbbrevCandidateArray_Keyword.Get(mAbbrevCurrentCandidateIndex,keyword);
			
			if( keyword.Compare(word) == false )
			{
				//Undoアクションタグ記録
				if( GetTextDocument().SPI_GetCurrentUndoActionTag() != undoTag_AbbrevInput )
				{
					GetTextDocument().SPI_RecordUndoActionTag(undoTag_AbbrevInput);
				}
				
				//現在単語選択
				ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
				ATextIndex	spos = epos - mLastAbbrevInput_Word.GetItemCount();
				if( spos < 0 )   spos = 0;
				if( epos < 0 )   epos = 0;
				SetSelect(spos,epos);
				//選択範囲削除、テキスト挿入
				DeleteAndInsertTextToDocument(keyword);
			}
			
			//最後に補完した単語を記憶
			mLastAbbrevInput_Word.SetText(keyword);
			mAbbrevCandidateArray_CompletionText.Get(mAbbrevCurrentCandidateIndex,mLastAbbrevInput_CompletionText);
			
			//#823
			//現在単語ハイライト
			//（補完選択間違い防止になる。）
			UpdateCurrentWordHighlight(keyword);
			
			//候補ウインドウの選択index更新
			GetApp().SPI_SetCandidateListWindowsSelectedIndex(NVM_GetWindow().GetObjectID(),mAbbrevCurrentCandidateIndex);
		}
		return true;
	}
}

/**
省略入力の候補のIndexをTextViewの外側（例えば候補ウインドウ）から決める（テキストがその候補文字に入れ替わる）
*/
void	AView_Text::SPI_SetCandidateIndex( const AIndex inIndex )
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	
	//候補index設定
	mAbbrevCurrentCandidateIndex = inIndex;
	//キーワード取得
	AText	keyword;
	mAbbrevCandidateArray_Keyword.Get(mAbbrevCurrentCandidateIndex,keyword);
	
	//Undoアクションタグ記録
	if( GetTextDocument().SPI_GetCurrentUndoActionTag() != undoTag_AbbrevInput )
	{
		GetTextDocument().SPI_RecordUndoActionTag(undoTag_AbbrevInput);
	}
	
	//現在の位置の単語取得
	ATextIndex	wordSpos = 0, wordEpos = 0;
	if( GetCurrentWordForAbbrev(wordSpos,wordEpos) == false )   return;
	//現在単語選択
	SetSelect(wordSpos,wordEpos);
	//選択範囲削除、テキスト挿入
	DeleteAndInsertTextToDocument(keyword);
	
	//最後に補完した単語を記憶
	mLastAbbrevInput_Word.SetText(keyword);
	mAbbrevCandidateArray_CompletionText.Get(mAbbrevCurrentCandidateIndex,mLastAbbrevInput_CompletionText);
}

/**
候補入力状態解除（補完確定）
*/
void	AView_Text::ClearAbbrevInputMode( const ABool inHidePopupAlways, const ABool inDecideCompletion )
{
	mSuppressRequestCandidateFinder = true;
	
	//候補入力中であれば、これで補完確定ということなので、補完履歴に追加
	if( mAbbrevInputMode == true )
	{
		/*
		ATextIndex	wordSpos = 0, wordEpos = 0;
		GetCurrentWordForAbbrev(wordSpos,wordEpos);
		AText	word;
		GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
		*/
		GetApp().SPI_AddToRecentCompletionWord(mLastAbbrevInput_Word);//word);
		//キーワード情報表示
		IdInfoAuto();
		
		//補完確定時テキスト挿入
		if( inDecideCompletion == true && mLastAbbrevInput_CompletionText.GetItemCount() > 0 )
		{
			//現在単語選択
			ATextIndex	epos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mCaretTextPoint);
			ATextIndex	spos = epos - mLastAbbrevInput_Word.GetItemCount();
			if( spos < 0 )   spos = 0;
			if( epos < 0 )   epos = 0;
			SetSelect(spos,epos);
			//選択範囲削除、テキスト挿入
			DeleteTextFromDocument();
			//確定時テキスト挿入
			AText	insText;
			insText.SetText(mLastAbbrevInput_CompletionText);
			SPI_DoTool_Text(insText,true);
		}
	}
	
	//
	if( mAbbrevInputMode == true || inHidePopupAlways == true )
	{
		if( mTextWindowID != kObjectID_Invalid )
		{
			//候補ウインドウの項目クリックを無効化する
			GetApp().SPI_InvalidateCandidateListWindowClick(mTextWindowID);
			//候補ポップアップウインドウを非表示
			GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_ShowCandidatePopupWindow(false,mAbbrevInputMode);
		}
	}
	
	//候補入力状態解除
	mAbbrevInputMode = false;
	//状態：アイドル
	mAbbrevCandidateState_Requesting = false;
	mAbbrevCandidateState_Displayed = false;
	
	//
	mSuppressRequestCandidateFinder = false;
	//補完確定時は、ヒントテキスト存在していたら、ヒントテキストを親キーワードとする候補検索実行
	if( inDecideCompletion == true )
	{
		RequestCandidateFinderIfHintExist();
	}
}

#pragma mark ===========================

#pragma mark ---キーワード情報
//#853

/**
キーワード情報表示
*/
void	AView_Text::DisplayIdInfo( const ABool inDisplayAlways, const AIndex inTextIndex, const AIndex inArgIndex )
{
	//単語位置取得
	AIndex	wordSpos = 0, wordEpos = 0;
	if( inTextIndex == kIndex_Invalid )
	{
		//現在選択取得
		GetSelect(wordSpos,wordEpos);
		//選択範囲無しなら現在単語を取得
		if( wordSpos == wordEpos )
		{
			if( GetTextDocumentConst().SPI_FindWordForKeyword(wordSpos,wordSpos,wordEpos) == false )
			{
				return;
			}
		}
	}
	else
	{
		//指定text indexの周りの単語を取得
		if( GetTextDocumentConst().SPI_FindWordForKeyword(inTextIndex,wordSpos,wordEpos) == false )
		{
			return;
		}
	}
	//単語取得
	AText	word;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,word);
	//parent keyword取得
	AText	parentKeyword;
	GetParentKeyword(wordSpos,parentKeyword);
	//★ここで継承クラスをparentKeywordArrayに入れたいが。。。
	ATextArray	parentKeywordArray;
	if( parentKeyword.GetItemCount() > 0 )
	{
		parentKeywordArray.Add(parentKeyword);
	}
	//id info検索要求
	ATextPoint	textPoint = {0};
	GetTextDocument().SPI_GetTextPointFromTextIndex(wordSpos,textPoint);
	RequestIdInfo(word,parentKeywordArray,inArgIndex,textPoint);
}

/**
id info検索要求
*/
void	AView_Text::RequestIdInfo( const AText& inWord, const ATextArray& inParentWord, const AIndex inArgIndex,
								  const ATextPoint& inTextPoint )
{
	//現在の場所のfuncIdを取得
	AText	currentFuncFuncname, currentFuncClassname;
	ATextPoint	currentFuncMenuidentifierSpt = {0}, currentFuncMenuidentifierEpt = {0};
	AIndex	categoryIndex = kIndex_Invalid;
	GetTextDocument().SPI_GetLocalStartIdentifierDataByLineIndex(inTextPoint.y,currentFuncFuncname,currentFuncClassname,
																 categoryIndex,currentFuncMenuidentifierSpt,currentFuncMenuidentifierEpt);
	AText	filepath;
	GetTextDocument().NVI_GetFilePath(filepath);
	AText	callerFuncIdText;
	AView_CallGraf::SPI_GetFuncIdText(filepath,currentFuncClassname,currentFuncFuncname,callerFuncIdText);
	//キーワード情報スレッドに検索要求
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(inTextPoint);
	GetApp().SPI_GetIdInfoFinderThread().SetRequestData(GetTextDocumentConst().GetObjectID(),
														kObjectID_Invalid,mTextWindowID,GetObjectID(),
														inWord,inParentWord,inArgIndex,callerFuncIdText,
														stateIndex);
	//スレッドunpause
	GetApp().SPI_GetIdInfoFinderThread().NVI_UnpauseIfPaused();
}

/**
キー入力"("等による自動IdInfo表示
*/
void	AView_Text::IdInfoAuto()
{
	//キャレット位置取得
	AIndex	spos, epos;
	GetSelect(spos,epos);
	//キャレット位置より前でアルファベットが存在する位置を取得する
	for( spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos); spos > 0; spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos) )
	{
		if( GetTextDocumentConst().SPI_IsAsciiAlphabet(spos) == true )
		{
			break;
		}
		//#955
		//マルチバイトの場合はリターン
		AUChar	ch = GetTextDocumentConst().SPI_GetTextChar(spos);
		if( IsUTF8Multibyte(ch) == true )
		{
			return;
		}
	}
	//キーワード情報表示（引数index=0）
	DisplayIdInfo(false,spos,0);
}

/**
キー入力","等による自動IdInfo表示
*/
void	AView_Text::IdInfoAutoArg()
{
	//キャレット位置取得
	AIndex	spos = 0, epos = 0;
	GetSelect(spos,epos);
	//引数index取得
	AIndex	funcPos = kIndex_Invalid;
	AIndex	argIndex = kIndex_Invalid;
	AIndex	argSpos = 0, argEpos = 0;
	GetIdInfoArgIndex(spos,funcPos,argIndex,argSpos,argEpos);
	//キーワード情報表示（引数指定）
	DisplayIdInfo(false,funcPos,argIndex);
}

/**
*/
void	AView_Text::IdInfoHighlightArg( const AIndex inTextIndex )
{
	AIndex	funcPos = 0;
	AIndex	argIndex = 0;
	AIndex	argSpos = kIndex_Invalid, argEpos = kIndex_Invalid;
	GetIdInfoArgIndex(inTextIndex,funcPos,argIndex,argSpos,argEpos);
	//
	AIndex	wordSpos = 0, wordEpos = 0;
	GetTextDocumentConst().SPI_FindWordForKeyword(funcPos,wordSpos,wordEpos);
	//
	AText	funcText;
	GetTextDocumentConst().SPI_GetText(wordSpos,wordEpos,funcText);
	//
	if( GetApp().SPI_HighlightIdInfoArgIndex(NVM_GetWindow().GetObjectID(),funcText,argIndex) == true &&
				argSpos != kIndex_Invalid && argEpos != kIndex_Invalid )
	{
		if( mIdInfoCurrentArgHighlighted == true )
		{
			mIdInfoCurrentArgHighlighted = false;
			SPI_RefreshLines(mIdInfoCurrentArgStartTextPoint.y,mIdInfoCurrentArgStartTextPoint.y+1);
		}
		mIdInfoCurrentArgHighlighted = true;
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(argSpos,mIdInfoCurrentArgStartTextPoint);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(argEpos,mIdInfoCurrentArgEndTextPoint);
		SPI_RefreshLines(mIdInfoCurrentArgStartTextPoint.y,mIdInfoCurrentArgStartTextPoint.y+1);
	}
	else
	{
		if( mIdInfoCurrentArgHighlighted == true )
		{
			mIdInfoCurrentArgHighlighted = false;
			SPI_RefreshLines(mIdInfoCurrentArgStartTextPoint.y,mIdInfoCurrentArgStartTextPoint.y+1);
		}
	}
}

/**
キーワード情報用 引数index, 引数位置(text index)取得
*/
void	AView_Text::GetIdInfoArgIndex( const AIndex inTextIndex, AIndex& outFuncPos, AIndex& outArgIndex,
		AIndex& outArgStartTextIndex, AIndex& outArgEndTextIndex ) const
{
	const ADocument_Text& document = GetTextDocumentConst();
	//結果初期化
	outFuncPos = inTextIndex;
	outArgIndex = 0;
	outArgStartTextIndex = kIndex_Invalid;
	outArgEndTextIndex = kIndex_Invalid;
	//syntax def取得
	const ASyntaxDefinition&	syntaxDefinition = GetApp().SPI_GetModePrefDB(GetTextDocumentConst().SPI_GetModeIndex()).GetSyntaxDefinition();
	//引数開始文字が存在しないならすぐに終了（これがないと最初までbreakしない）
	if( syntaxDefinition.GetIdInfoArgStartCharCount() == 0 )   return;
	//検索開始index取得
	AIndex	searchStartIndex = inTextIndex-1000;
	if( searchStartIndex < 0 )
	{
		searchStartIndex = 0;
	}
	//検索終了index取得
	AIndex	searchEndIndex = inTextIndex+1000;
	if( searchEndIndex >= GetTextDocumentConst().SPI_GetTextLength() )
	{
		searchEndIndex = GetTextDocumentConst().SPI_GetTextLength();
	}
	//キャレット位置より前で'('等を検索する
	AIndex	level = 0;
	for( outFuncPos = document.SPI_GetPrevCharTextIndex(inTextIndex); 
				outFuncPos > searchStartIndex; 
				outFuncPos = document.SPI_GetPrevCharTextIndex(outFuncPos) )
	{
		AUChar	ch = document.SPI_GetTextChar(outFuncPos);
		//levelが0のときのみ
		if( level == 0 )
		{
			//IsIdInfoStartChar（C言語の場合は"("）ならもう１つ前に戻して終了
			if( syntaxDefinition.IsIdInfoStartChar(ch) == true )
			{
				//引数開始位置未設定の場合は、引数開始位置として設定
				if( outArgStartTextIndex == kIndex_Invalid )
				{
					outArgStartTextIndex = document.SPI_GetNextCharTextIndex(outFuncPos);
				}
				//関数位置設定、検索終了
				outFuncPos = document.SPI_GetPrevCharTextIndex(outFuncPos);
				break;
			}
			//IsIdInfoDelimiterChar（C言語の場合は","）ならargindexを+1して継続
			if( syntaxDefinition.IsIdInfoDelimiterChar(ch) == true )
			{
				//引数開始位置未設定の場合は、引数開始位置として設定
				if( outArgStartTextIndex == kIndex_Invalid )
				{
					outArgStartTextIndex = document.SPI_GetNextCharTextIndex(outFuncPos);
				}
				//引数indexインクリメント
				outArgIndex++;
			}
		}
		//()内を無視するためにlevelを上下
		if( syntaxDefinition.IsIdInfoStartChar(ch) == true )   level--;
		if( syntaxDefinition.IsIdInfoEndChar(ch) == true )   level++;
	}
	//キャレット位置より後で','等を検索する
	level = 0;
	for( AIndex pos = inTextIndex; 
				pos < searchEndIndex; 
				pos = document.SPI_GetNextCharTextIndex(pos) )
	{
		AUChar	ch = document.SPI_GetTextChar(pos);
		//levelが0のときのみ
		if( level == 0 )
		{
			//')', ','なら、引数終了位置設定
			if( syntaxDefinition.IsIdInfoEndChar(ch) == true || syntaxDefinition.IsIdInfoDelimiterChar(ch) == true )
			{
				outArgEndTextIndex = pos;
				break;
			}
		}
		//()内を無視するためにlevelを上下
		if( syntaxDefinition.IsIdInfoStartChar(ch) == true )   level++;
		if( syntaxDefinition.IsIdInfoEndChar(ch) == true )   level--;
	}
}

/**
ポップアップウインドウ位置調整
*/
void	AView_Text::AdjustPopupWindowsPosition()
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_AdjustPopupWindowsPosition();
	}
}

#pragma mark ===========================

#pragma mark ---テキストカウント
//#142

/**
テキストカウントウインドウ表示更新（選択範囲カウント、現在文字）
*/
void	AView_Text::UpdateTextCountWindows()
{
	//doc infoウインドウが無ければ何もしない
	if( GetApp().SPI_DocInfoWindowExist(NVM_GetWindow().GetObjectID()) == false )
	{
		return;
	}
	
	//選択範囲の文字数等カウント
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	AItemCount	selectedCharCount = 0, selectedWordCount = 0, paragraphCount = 0;
	GetTextDocumentConst().SPI_GetWordCount(spt,ept,selectedCharCount,selectedWordCount,paragraphCount);
	//#1123
	//矩形選択中は段落数として、矩形行数を表示する
	if( mKukeiSelected == true )
	{
		paragraphCount = mKukeiSelected_Start.GetItemCount();
	}
	AIndex	startIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	//単語6文字なら選択テキスト色として取得
	AText	selectedText;
	AIndex	s = 0, e = 0;
	GetTextDocumentConst().SPI_FindWordForKeyword(startIndex,s,e);
	if( e-s < 256 )
	{
		GetTextDocumentConst().SPI_GetText(s,e,selectedText);
	}
	//テキストカウントウインドウ更新
	GetApp().SPI_UpdateDocInfoWindows_SelectedCount(NVM_GetWindow().GetObjectID(),
				selectedCharCount, selectedWordCount, paragraphCount,selectedText);
	//sdf debug
	AText	debugtext;
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )
	{
		AText	stateText,pushedStateText;
		AIndex	stateIndex = 0, pushedStateIndex = 0;
		GetTextDocumentConst().SPI_GetCurrentStateIndexAndName(GetCaretTextPoint(),
					stateIndex,pushedStateIndex,stateText,pushedStateText);
		AText	t;
		t.SetFormattedCstring("State:%d(",stateIndex);
		t.AddText(stateText);
		t.AddFormattedCstring(")   PushedState:%d(",pushedStateIndex);
		t.AddText(pushedStateText);
		AItemCount	indentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(GetCaretTextPoint().y);
		t.AddFormattedCstring(")   Indent:%d \r",indentCount);
		t.AddFormattedCstring("Directive Level:%d ",GetTextDocument().ForDebug_GetTextInfo().GetDirectiveLevel(GetCaretTextPoint().y));
		debugtext.SetText(t);
	}
	//現在文字取得
	AUCS4Char	ch = 0, decomp = 0;
	GetTextDocumentConst().SPI_GetText1UCS4Char(startIndex,ch,decomp);
	//#906
	//見出しパス表示
	//前回取得時の見出しindexを取得し、違う場合のみ見出しパス更新
	AIndex	jumpMenuItemIndex = GetTextDocumentConst().SPI_GetJumpMenuItemIndexByLineIndex(spt.y);
	if( jumpMenuItemIndex != mCurrentHeaderPathJumpMenuItemIndex )
	{
		//見出しパス取得
		mCurrentHeaderPathJumpMenuItemIndex = jumpMenuItemIndex;
		GetTextDocumentConst().SPI_GetHeaderPathArray(spt.y,mCurrentHeaderPathArray_HeaderTextArray,mCurrentHeaderPathArray_HeaderParagraphIndexArray);
	}
	//現在文字をdoc infoウインドウに設定
	GetApp().SPI_UpdateDocInfoWindows_CurrentChar(NVM_GetWindow().GetObjectID(),ch,decomp,
				mCurrentHeaderPathArray_HeaderTextArray,mCurrentHeaderPathArray_HeaderParagraphIndexArray,debugtext);
}


#pragma mark ===========================

#pragma mark ---現在単語マーカー
//#823

/**
現在単語ハイライト設定
*/
void	AView_Text::SetCurrentWordHighlight()
{
	//現在単語ハイライトしない設定なら、現在単語ハイライトクリアのみして、終了
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayCurrentWordHighlight) == false )
	{
		GetTextDocument().SPI_ClearCurrentWordForHighlight(true);
		return;
	}
	
	//選択範囲取得
	AIndex	selSpos = 0, selEpos = 0;
	GetSelect(selSpos,selEpos);
	//単語
	AText	word;
	//
	if( selSpos != selEpos )
	{
		//選択文字列が存在する場合
		
		//選択範囲開始位置周辺の単語範囲取得
		AIndex	spos1 = 0, epos1 = 0;
		GetTextDocumentConst().SPI_FindWordForKeyword(selSpos,spos1,epos1);
		//選択範囲終了位置周辺の単語範囲取得
		AIndex	spos2 = 0, epos2 = 0;
		GetTextDocumentConst().SPI_FindWordForKeyword(GetTextDocumentConst().SPI_GetPrevCharTextIndex(selEpos),spos2,epos2);
		
		//上記の2単語の開始、終了までのテキストを取得
		GetTextDocumentConst().SPI_GetText(spos1,epos2,word);
	}
	else
	{
		//選択文字列が存在しない場合
		
		//現在単語取得
		ATextIndex	spos = 0, epos = 0;
		if( GetTextDocumentConst().SPI_FindWordForKeyword(selSpos,spos,epos) == true )
		{
			GetTextDocumentConst().SPI_GetText(spos,epos,word);
		}
	}
	//単語をハイライトとして設定
	UpdateCurrentWordHighlight(word);
	
}

/**
現在単語ハイライト更新
*/
void	AView_Text::UpdateCurrentWordHighlight( const AText& inWord )
{
	//ドキュメントに現在単語設定
	GetTextDocument().SPI_SetCurrentWordForHighlight(inWord,true,true);
	//メイン／サブ、もう片方の現在単語設定
	if( mTextWindowID != kObjectID_Invalid )
	{
		ADocumentID	mainDocID = GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetCurrentDocumentID();
		ADocumentID	subDocID = GetApp().SPI_GetTextWindowByID(mTextWindowID).SPI_GetCurrentSubTextPaneDocumentID();
		if( mainDocID == GetTextDocument().GetObjectID() && subDocID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(subDocID).SPI_SetCurrentWordForHighlight(inWord,true,true);
		}
		else if( mainDocID != kObjectID_Invalid )
		{
			GetApp().SPI_GetTextDocumentByID(mainDocID).SPI_SetCurrentWordForHighlight(inWord,true,true);
		}
	}
}

#pragma mark ===========================

#pragma mark ---ワーズリスト
//#723

/**
ワードリストウインドウへ単語検索を要求
*/
void	AView_Text::RequestWordsList()
{
	//選択範囲取得
	AIndex	selSpos = 0, selEpos = 0;
	GetSelect(selSpos,selEpos);
	//単語
	AText	word;
	//現在単語取得
	ATextIndex	spos = 0, epos = 0;
	if( GetTextDocumentConst().SPI_FindWordForKeyword(selSpos,spos,epos) == true )
	{
		//現在単語がアルファベットの場合、ワーズリストを要求する
		
		//単語取得
		GetTextDocumentConst().SPI_GetText(spos,epos,word);
		
		//ワーズリストウインドウに単語検索を要求
		AFileAcc	folder;
		GetTextDocumentConst().SPI_GetProjectFolder(folder);
		if( folder.IsSpecified() == false )
		{
			//プロジェクトフォルダ未設定なら、親フォルダを取得
			//ワードリスト検索スレッドからSPI_GetImportFileDataIDArrayForWordsList()がコールされたときに
			//プロジェクトフォルダでなければ、そのフォルダを非再帰的に検索する。
			GetTextDocumentConst().SPI_GetParentFolder(folder);
		}
		GetApp().SPI_RequestWordsList(mTextWindowID,folder,GetTextDocumentConst().SPI_GetModeIndex(),word);//ワーズリストウインドウがなければ何もしない
	}
}

#pragma mark ===========================

#pragma mark ---diff

/**
diff相手側ドキュメントの選択位置に合わせて、自分の選択位置を設定
*/
void	AView_Text::SPI_SetSelectByDiffTargetDocumentSelection( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint )
{
	ATextPoint	spt = {0}, ept = {0};
	if( GetTextDocumentConst().SPI_GetCorrespondingTextPointByDiffTargetTextPoint(inStartPoint,spt) == true &&
				GetTextDocumentConst().SPI_GetCorrespondingTextPointByDiffTargetTextPoint(inEndPoint,ept) == true )
	{
		SPI_SetSelect(spt,ept);
		ShowCaret();
	}
}

#pragma mark ===========================

#pragma mark ---KeyRecord

/**
キー記録再生
*/
void	AView_Text::SPI_PlayKeyRecord()
{
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Tool);
	//ツール実行
	AText	text;
	GetApp().SPI_GetRecordedText(text);
	InsertToolText(text,1);
}

#pragma mark ===========================

//
void	AView_Text::SPI_MoveToLine( const AIndex inLineIndex, const ABool inParagraphMode )
{
	AIndex	lineIndex = inLineIndex;
	if( inParagraphMode == true )
	{
		lineIndex = GetTextDocumentConst().SPI_GetLineIndexByParagraphIndex(inLineIndex);
	}
	ATextPoint	spt, ept;
	if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount()-1 )//#224 最終行の処理忘れ修正
	{
		spt.x = 0;
		spt.y = GetTextDocumentConst().SPI_GetLineCount()-1;
		ept.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(GetTextDocumentConst().SPI_GetLineCount()-1);
		ept.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	else
	{
		spt.x = 0;
		spt.y = lineIndex;
		ept.x = 0;
		ept.y = lineIndex+1;
	}
	SPI_SetSelect(spt,ept);
	SPI_AdjustScroll_Center();
}

void	AView_Text::SPI_DoJumpListSelected( const AIndex inIndex )
{
	AUniqID	identifierObjectID = GetTextDocumentConst().SPI_GetJumpMenuIdentifierUniqID(inIndex);
	/*#146
	ATextPoint	spt,ept;
	GetTextDocument().SPI_GetGlobalIdentifierRange(identifierObjectID,spt,ept);
	AdjustScroll_Top(spt);
	SetSelect(spt,ept);
	*/
	Jump(identifierObjectID);
	NVM_GetWindow().NVI_SelectWindow();//win 080714
}

//ドキュメントのSave前処理(ADocument_TextのSPI_Save()からコールされる)
void	AView_Text::SPI_SavePreProcess()
{
	//Caret, Select情報をDBへ保存
	GetTextDocument().GetDocPrefDB().SetData_Bool(ADocPrefDB::kCaretMode,			mCaretMode);
	/*B0425 yを段落番号にするように変更
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kCaretPointX,		GetCaretTextPoint().x);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kCaretPointY,		GetCaretTextPoint().y);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointX,		GetSelectTextPoint().x);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointY,		GetSelectTextPoint().y);
	*/
	ATextIndex	carettextindex	= GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
	AIndex	caretparaindex		= GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetCaretTextPoint().y);
	ATextIndex	caretparastart	= GetTextDocumentConst().SPI_GetParagraphStartTextIndex(caretparaindex);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kCaretPointX,		carettextindex-caretparastart);
	GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kCaretPointY,		caretparaindex);
	if( mCaretMode == true )
	{
		GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointX,		carettextindex-caretparastart);
		GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointY,		caretparaindex);
	}
	else
	{
		ATextIndex	selecttextindex	= GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetSelectTextPoint());
		AIndex	selectparaindex		= GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(GetSelectTextPoint().y);
		ATextIndex	selectparastart	= GetTextDocumentConst().SPI_GetParagraphStartTextIndex(selectparaindex);
		GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointX,		selecttextindex-/*#260selectparaindex*/selectparastart);
		GetTextDocument().GetDocPrefDB().SetData_Number(ADocPrefDB::kSelectPointY,		selectparaindex);
	}
}

void	AView_Text::SPI_SelectText( const AIndex inStartIndex, const AItemCount inLength, const ABool inByFind )
{
	ATextIndex	spos, epos;
	spos = inStartIndex;
	epos = spos + inLength;
	SetSelect(spos,epos,inByFind);
	AdjustScroll_Center(GetCaretTextPoint());
}

#if IMPLEMENTATION_FOR_MACOSX
//B0370
void	AView_Text::SPI_ServicePaste( const AScrapRef inScrapRef )
{
	AText	text;
	AScrapWrapper::GetTextScrap(inScrapRef,text);//#688 ,GetTextDocumentConst().SPI_GetPreferLegacyTextEncoding(),true);//win
	TextInput(undoTag_Paste,text);
}
#endif

#pragma mark ===========================

#pragma mark ---<内部モジュール>

#pragma mark ===========================

#pragma mark ---テキスト描画用データ取得

/*#450
//行の高さ取得
ANumber	AView_Text::GetLineHeightWithMargin() const
{
	return mLineHeight + mLineMargin;
}
ANumber	AView_Text::GetLineHeightWithoutMargin() const
{
	return mLineHeight;
}
*/
//#450
/**
*/
ANumber	AView_Text::GetLineHeightWithMargin( const AIndex inLineIndex ) const
{
	if( inLineIndex >= mLineImageInfo_Height.GetItemCount() )
	{
		return mLineHeight + mLineMargin;
	}
	else
	{
		return mLineImageInfo_Height.Get(inLineIndex);
	}
}
ANumber	AView_Text::GetLineHeightWithoutMargin( const AIndex inLineIndex ) const
{
	if( inLineIndex >= mLineImageInfo_Height.GetItemCount() )
	{
		return mLineHeight;
	}
	else
	{
		AFloatNumber	h = mLineImageInfo_Height.Get(inLineIndex);
		h *= mLineHeight;
		h /= (mLineHeight+mLineMargin);
		return h;
	}
}
ANumber	AView_Text::GetLineAscent( const AIndex inLineIndex ) const
{
	if( inLineIndex >= mLineImageInfo_Height.GetItemCount() )
	{
		return mLineAscent;
	}
	else
	{
		AFloatNumber	h = mLineImageInfo_Height.Get(inLineIndex);
		h *= mLineAscent;
		h /= (mLineHeight+mLineMargin);
		return h;
	} 
} 

//
/**
ImageYからテキスト行番号取得（各行ごとのImageY配列から2分法で検索）
*/
AIndex	AView_Text::GetLineIndexByImageY( const ANumber inImageY ) const
{
	if( mLineImageInfo_ImageY.GetItemCount() == 0 )   return 0;
	
	//#450 行折り返し対応
	//
	if( inImageY > mLineImageInfo_ImageY.Get(mLineImageInfo_ImageY.GetItemCount()-1) || inImageY < 0 )
	{
		//_ACError("image y invalid",this);
		return GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	//2分法
	AIndex	startLineIndex = 0;
	AIndex	endLineIndex = mLineImageInfo_ImageY.GetItemCount();
	AIndex	lineIndex = (startLineIndex+endLineIndex)/2;
	AItemCount	lineCount = mLineImageInfo_ImageY.GetItemCount();
	const ADocument_Text&	document = GetTextDocumentConst();
	for( AIndex i = 0; i < lineCount*2; i++ )//無限ループ防止（多くとも行数回のループで終わるはず）
	{
		ANumber	imageY = mLineImageInfo_ImageY.Get(lineIndex);
		ANumber	height = mLineImageInfo_Height.Get(lineIndex);
		if( inImageY >= imageY && inImageY <= imageY+height ) 
		{
			if( lineIndex < 0 )
			{
				lineIndex = 0;
			}
			if( lineIndex >= document.SPI_GetLineCount() )
			{
				lineIndex = document.SPI_GetLineCount()-1;
			}
			//ちょうど次の行との境界上の場合は次の行を返す（2分法の都合上(?)、上のinImageY <= imageY+heightの不等号を<に変えると一致行を見つけられない）
			if( inImageY == imageY+height )
			{
				lineIndex++;
			}
			//最終行以降になったら補正
			if( lineIndex >= document.SPI_GetLineCount() )
			{
				lineIndex = document.SPI_GetLineCount()-1;
			}
			//
			return lineIndex;
		}
		if( inImageY < imageY )
		{
			endLineIndex = lineIndex;
			lineIndex = (startLineIndex+endLineIndex)/2;
		}
		else if( inImageY > imageY+height )
		{
			startLineIndex = lineIndex+1;
			lineIndex = (startLineIndex+endLineIndex)/2;
		}
		if( startLineIndex+1 >= endLineIndex )
		{
			endLineIndex = startLineIndex+1;
			lineIndex = startLineIndex;
		}
	}
	_ACError("cannot find image y",this);
	return kIndex_Invalid;
	/*
	AIndex	lineIndex = inImageY/GetLineHeightWithMargin();
	if( lineIndex < 0 )
	{
		lineIndex = 0;
	}
	if( lineIndex >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		lineIndex = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	return lineIndex;
	*/
}

//
ANumber	AView_Text::GetImageYByLineIndex( const AIndex inLineIndex ) const
{
	if( mLineImageInfo_ImageY.GetItemCount() == 0 )   return 0;
	//#450 行折り返し対応
	if( inLineIndex < 0 )
	{
		return 0;
	}
	if( inLineIndex >= mLineImageInfo_ImageY.GetItemCount() )
	{
		return mLineImageInfo_ImageY.Get(mLineImageInfo_ImageY.GetItemCount()-1);
	}
	return mLineImageInfo_ImageY.Get(inLineIndex);
	/*#450
	return inLineIndex * GetLineHeightWithMargin();
	*/
}

//
void	AView_Text::GetLineImageRect( const AIndex inLineIndex, AImageRect& outLineImageRect ) const
{
	outLineImageRect.left	= 0;
	outLineImageRect.right	= NVM_GetImageWidth();
	outLineImageRect.top	= GetImageYByLineIndex(inLineIndex);//#450 inLineIndex*GetLineHeightWithMargin();
	outLineImageRect.bottom	= GetImageYByLineIndex(inLineIndex+1);//#450 (inLineIndex+1)*GetLineHeightWithMargin();
}

//
void	AView_Text::GetLineImageRect( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AImageRect& outLineImageRect ) const
{
	outLineImageRect.left	= 0;
	outLineImageRect.right	= NVM_GetImageWidth();
	outLineImageRect.top	= GetImageYByLineIndex(inStartLineIndex);//#450 inStartLineIndex*GetLineHeightWithMargin();
	outLineImageRect.bottom	= GetImageYByLineIndex(inEndLineIndex);//#450 inEndLineIndex*GetLineHeightWithMargin();
}

//
//★非const
void	AView_Text::GetImagePointFromTextPoint( const ATextPoint& inTextPoint, AImagePoint& outImagePoint )
{
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	ANumber	y = inTextPoint.y;
	if( y >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		y = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	//#470 GetTextDocument().SPI_GetTextDrawDataWithoutStyle(y,textDrawData);
	AAnalyzeDrawData	analyzeDrawData;
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextDrawDataWithStyle(y,textDrawData,false,caretPoint,selectPoint,analyzeDrawData,GetObjectID());
	
	//#450
	SetLineDefaultTextProperty(inTextPoint.y);
	
	//
	outImagePoint.x = NVMC_GetImageXByTextPosition(textDrawData,inTextPoint.x);
	outImagePoint.y = GetImageYByLineIndex(inTextPoint.y);//#450
	/*#450
	outImagePoint.y = y * GetLineHeightWithMargin();
	*/
} 

//
//★非const
void	AView_Text::GetTextPointFromImagePoint( const AImagePoint& inImagePoint, ATextPoint& outTextPoint )
{
	if( inImagePoint.y > NVM_GetImageHeight()/*#450 GetLineHeightWithMargin() * GetTextDocumentConst().SPI_GetLineCount()*/ )
	{
		outTextPoint.y = GetTextDocumentConst().SPI_GetLineCount()-1;
		outTextPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(outTextPoint.y);
		return;
	}
	//テキストY座標計算
	outTextPoint.y = GetLineIndexByImageY(inImagePoint.y);
	
	//#450
	SetLineDefaultTextProperty(outTextPoint.y);
	
	//対象行描画データ取得
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	//#470 GetTextDocument().SPI_GetTextDrawDataWithoutStyle(outTextPoint.y,textDrawData);
	AAnalyzeDrawData	analyzeDrawData;
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextDrawDataWithStyle(outTextPoint.y,textDrawData,false,caretPoint,selectPoint,analyzeDrawData,GetObjectID());
	//
	outTextPoint.x = NVMC_GetTextPositionByImageX(textDrawData,inImagePoint.x);
}

//#450
/**
行のデフォルトtextpropertyを設定
*/
void	AView_Text::SetLineDefaultTextProperty( const AIndex inLineIndex )
{
	/* 行ごとのmultiply機能見送りのため、一旦コメントアウト（高速化のため）GetImagePointFromTextPoint(), GetTextPointFromImagePoint()からコールされている
	AText	fontname;
	//#868 GetDocPrefDB().GetData_Text(ADocPrefDB::kTextFontName,fontname);
	GetTextDocumentConst().SPI_GetFontName(fontname);//#868
	AColor	lettercolor;
	GetModePrefDB().GetData_Color(AModePrefDB::kLetterColor,lettercolor);
	ABool	isAntiAlias = GetTextDocument().SPI_IsAntiAlias();
	AFloatNumber	fontsize = GetTextDocumentConst().SPI_GetFontSize()
					* GetTextDocumentConst().SPI_GetLineFontSizeMultiply(inLineIndex);
	NVMC_SetDefaultTextProperty(fontname,fontsize,lettercolor,kTextStyle_Normal,isAntiAlias);
	*/
}

//#379
/**
行番号からLocalYを取得
*/
AIndex	AView_Text::SPI_GetLocalYByLineIndex( const AIndex inLineIndex ) const
{
	AImagePoint	ipt = {0,0};
	ipt.y = GetImageYByLineIndex(inLineIndex);
	ALocalPoint	lpt = {0,0};
	NVM_GetLocalPointFromImagePoint(ipt,lpt);
	return lpt.y;
}

#pragma mark ===========================

#pragma mark ---キャレット制御

//キャレットモードかどうかを取得
ABool	AView_Text::IsCaretMode() const
{
	return mCaretMode;
}

//
void	AView_Text::SetCaretTextPoint( const ATextPoint& inTextPoint )
{
	//#496 行描画更新のために現在行の位置を記憶しておく
	ATextPoint	svCaretPoint = mCaretTextPoint;
	//#262
	mSelectionByFind = false;
	//
	if( IsCaretMode() == true )
	{
		//win
		if( inTextPoint.x == mCaretTextPoint.x && inTextPoint.y == mCaretTextPoint.y )
		{
			return;
		}
		//
		HideCaret(inTextPoint.y != mCaretTextPoint.y,true);
	}
	else
	{
		mCaretMode = true;
		AImageRect	imageRect;
		if( mKukeiSelected == true )
		{
			ATextPoint	spt, ept;
			GetKukeiContainRange(spt,ept);
			GetLineImageRect(spt.y,ept.y+1,imageRect);
			mKukeiSelected_Start.DeleteAll();
			mKukeiSelected_End.DeleteAll();
			mKukeiSelected = false;
		}
		else
		{
			ATextPoint	pt1, pt2;
			ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
			GetLineImageRect(pt1.y,pt2.y+1,imageRect);
		}
		ALocalRect	localRect;
		NVM_GetLocalRectFromImageRect(imageRect,localRect);
		//view右端までを描画
		ALocalRect	localFrameRect = {0};
		NVM_GetLocalViewRect(localFrameRect);
		if( localRect.right < localFrameRect.right )
		{
			localRect.right = localFrameRect.right;
		}
		//
		NVMC_RefreshRect(localRect);
	}
	//#231 マルチバイト文字の途中にならないように補正
	ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(inTextPoint);
	ATextIndex	adjustedindex = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(textindex);
	if( textindex == adjustedindex )
	{
		//補正無し
		mCaretTextPoint = inTextPoint;
	}
	else
	{
		//補正あり
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(adjustedindex,mCaretTextPoint);
	}
	//#571 エラー位置補正
	if( mCaretTextPoint.y < 0 )
	{
		mCaretTextPoint.y = 0;
	}
	if( mCaretTextPoint.y >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		mCaretTextPoint.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	if( mCaretTextPoint.x < 0 )
	{
		mCaretTextPoint.x = 0;
	}
	if( mCaretTextPoint.x > GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(mCaretTextPoint.y) )
	{
		mCaretTextPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(mCaretTextPoint.y);
	}
	
	GetImagePointFromTextPoint(GetCaretTextPoint(),mCaretImagePoint);
	if( NVI_IsFocusActive() == true )
	{
		//win Caretちらつき対策策（途中で描画せずに、ここでまとめて表示） ShowCaret();
		//Caret表示予約する
		ShowCaretReserve();//win
	}
	
	//#867
	//ローカル範囲更新トリガー
	TriggerKeyIdleWork(kKeyIdleWorkType_UpdateLocalRange,kKeyIdleWorkTimerValue);
	
	//
	SetOldArrowImageX(mCaretImagePoint.x);
	
	/*#844
	//#496 現在行下線描画ありの場合、行描画更新
	if( GetModePrefDB().GetData_Bool(AModePrefDB::kShowUnderlineAtCurrentLine) == true )
	*/
	if( svCaretPoint.y != mCaretTextPoint.y )
	{
		SPI_RefreshLine(svCaretPoint.y);
		SPI_RefreshLine(mCaretTextPoint.y);
	}
	/*
	*/
	//#688
	//TextInputClientのselected rangeを消去（InlineInput処理中かどうかの判断は関数内で実施）
	NVMC_ResetSelectedRangeForTextInput();
	
	//#450
	//設定したキャレット位置が折りたたみ内ならその行を折りたたみ解除する
	if( SPI_IsLineCollapsed(mCaretTextPoint.y) == true )
	{
		SPI_RevealCollapsedLine(mCaretTextPoint.y);
	}
	
	//#142
	//テキストカウント（選択範囲）更新
	UpdateTextCountWindows();
	
	//選択変更時JavaScriptプラグイン実行 #994
	//#994 GetTextDocumentConst().SPI_ExecuteEventListener("onSelectionChanged",GetEmptyText());
	
	//#750
	//現在単語ハイライト消去
	//GetTextDocument().SPI_ClearCurrentWordForHighlight(true);
	
	//ポップアップウインドウ位置調整
	AdjustPopupWindowsPosition();
	
	//キャレット位置のカラースロットindexに変換があった場合は、文法認識をトリガーする
	AIndex	stateIndex = GetTextDocumentConst().SPI_GetCurrentStateIndex(mCaretTextPoint);
	AIndex	colorSlotIndex = GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetColorSlotIndexByStateIndex(stateIndex);
	if( colorSlotIndex != mCurrentCaretStateColorSlotIndex )
	{
		//#905
		//文法認識とローカル範囲設定
		TriggerSyntaxRecognize();
		//キャレット位置カラースロット記憶
		mCurrentCaretStateColorSlotIndex = colorSlotIndex;
	}
	
	//ジャンプリスト更新のため、選択更新をウインドウへ通知
	NVM_GetWindow().OWICB_SelectionChanged(NVI_GetControlID());
	
	//キャレット位置にヒントテキストがあれば、それを親キーワードとして候補検索
	RequestCandidateFinderIfHintExist();
	
	//#1031
	//キャレットのローカル位置を記憶
	SetLastCaretLocalPoint();
	
	//括弧ハイライト #1406
	HighlightBrace();
}

//#1031
/**
キャレットのローカル位置を記憶
*/
void	AView_Text::SetLastCaretLocalPoint()
{
	AImagePoint	ipt = {0};
	GetImagePointFromTextPoint(mCaretTextPoint,ipt);
	NVM_GetLocalPointFromImagePoint(ipt,mLastCaretLocalPoint);
}

//#450
/**
*/
void	AView_Text::UpdateCaretImagePoint()
{
	if( IsCaretMode() == true )
	{
		HideCaret(true,true);
		
		GetImagePointFromTextPoint(GetCaretTextPoint(),mCaretImagePoint);
		SetOldArrowImageX(mCaretImagePoint.x);
		//SPI_EditPostProcess()からコールされたときに、上でHideCaret()してしまうので、
		//そのあとのSetCaretPoint()でshowcaretされず、deleteキー等の直後にキャレット表示されない問題の対策。#690
		ShowCaretReserve();
	}
}

//
void	AView_Text::SetOldArrowImageX( const ANumber inImageX )
{
	mOldArrowImageX = inImageX;
}

ANumber	AView_Text::GetOldArrowImageX() const
{
	return mOldArrowImageX;
}

//現在のキャレット描画データ取得
//返り値：キャレットが現在のViewRectに含まれるかどうか
void	AView_Text::GetCaretDrawData( ALocalPoint& outStartPoint, ALocalPoint& outEndPoint ) const
{
	AImagePoint	imageSpt = mCaretImagePoint;
	AImagePoint	imageEpt = imageSpt;
	imageEpt.y += GetLineHeightWithMargin(mCaretTextPoint.y)-1;//#450 GetLineHeightWithoutMargin()-1;
	NVM_GetLocalPointFromImagePoint(imageSpt,outStartPoint);
	NVM_GetLocalPointFromImagePoint(imageEpt,outEndPoint);
	//return (NVM_IsImagePointInViewRect(imageSpt) == true || NVM_IsImagePointInViewRect(imageEpt) == true);
}

//R0184
/**
キャレット描画
*/
void	AView_Text::DrawXorCaret( const ABool inFlush )//#1034
{
	ALocalPoint	spt,ept;
	GetCaretDrawData(spt,ept);
	//キャレット色（＝文字色）取得
	AColor	modeLetterColor = kColor_Black;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetModeData_Color(AModePrefDB::kLetterColor,modeLetterColor);
	//キャレット太さ
	if( GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kBigCaret) == false )
	{
		NVMC_DrawXorCaretLine(spt,ept,true,inFlush,false,1);//#688 #1034
	}
	else
	{
		spt.y -= 1;
		ept.y += 1;
		if( spt.x > 0 )
		{
			//#186 spt.x -= 1;
			//#186 ept.x -= 1;
		}
		//
		NVMC_DrawXorCaretLine(spt,ept,true,inFlush,false,2);//#688 #1034
	}
	//#448
	/*
	if( mTextWindowID != kObjectID_Invalid )
	{
		if( GetApp().SPI_GetTextWindowByID(mTextWindowID).NVI_GetInlineInputMode() == true )
		{
			spt = ept;
			spt.x += 2;
			NVMC_DrawXorCaretLine(spt,ept,true,false);
		}
	}
	*/
}

//キャレット表示
void	AView_Text::ShowCaret()
{
	//状態initial系のときは何もせずリターン
	if( GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	if( IsCaretMode() == true )
	{
		//===============通常キャレット===============
		if( NVI_IsFocusActive() == true )
		{
			if( mCaretBlinkStateVisible == false )
			{
				DrawXorCaret(mRefreshing == false);//#1034 テキスト描画予定がない場合のみflushする
			}
			mCaretTickCount = 0;
			mCaretBlinkStateVisible = true;
		}
		
		//===============クロスキャレットH===============
		{
			if( mCrossCaretMode == true && mCrossCaretHVisible == false )
			{
				XorCrossCaretH();
				mCrossCaretHVisible = true;
			}
		}
		//===============クロスキャレットV===============
		{
			if( mCrossCaretMode == true && mCrossCaretVVisible == false )
			{
				XorCrossCaretV();
				mCrossCaretVVisible = true;
			}
		}
	}
}

//キャレット非表示
void	AView_Text::HideCaret( const ABool inHideCrossCaretH, const ABool inHideCrossCaretV )
{
	//fprintf(stderr,"HideCaret()");
	if( IsCaretMode() == true )
	{
		//===============通常キャレット===============
		{
			if( mCaretBlinkStateVisible == true )
			{
				DrawXorCaret(false);//#1034 常にflushしない
			}
			mCaretTickCount = 0;
			mCaretBlinkStateVisible = false;
		}
		//===============クロスキャレットH===============
		if( mCrossCaretMode == true && mCrossCaretHVisible == true && inHideCrossCaretH == true )
		{
			XorCrossCaretH();
			mCrossCaretHVisible = false;
		}
		//===============クロスキャレットV===============
		if( mCrossCaretMode == true && mCrossCaretVVisible == true && inHideCrossCaretV == true )
		{
			XorCrossCaretV();
			mCrossCaretVVisible = false;
		}
	}
}

/**
Caret表示予約
WindowsでのCaretちらつき対策。（Windowsの場合、ShowCaret()をコールするとその時点で描画されるので、処理途中の箇所でCaret描画される。
実際にはここで描画せず、ShowCaretIfReserved()コール時、または、次回即タイムアウト時に描画する
*/
void	AView_Text::ShowCaretReserve()
{
	//fprintf(stderr,"ShowCaretReserve()");
	if( IsCaretMode() == true )
	{
		//ここでHideすると文字入力のたびにキャレットがちらついてしまう。
		//ここでHideせず、ShowCaretIfReserved()で、前回caretのhide(DrawXorCaretLine()内で実行される)＋今回showにすることで、
		//ちらつきしないようにする。
		if( mCaretBlinkStateVisible == true )
		{
			DrawXorCaret(false);//#1034 常にflushしない
		}
		//#1034
		//テキスト描画予定がない場合：次回ShowCaretIfReserved()実行時にキャレット描画を行う
		if( mRefreshing == false )
		{
			mCaretTickCount = 99999;
		}
		//テキスト描画予定がある場合：tick timer2回発生後にキャレット描画を行う
		else
		{
			mCaretTickCount = ATimerWrapper::GetCaretTime()-2;
		}
		mCaretBlinkStateVisible = false;
		/*
		if( mCrossCaretMode == true && mCrossCaretVisible == true )
		{
			XorCrossCaret(false);
			mCrossCaretVisible = false;
		}
		*/
	}
}

/**
Caret表示（Caret表示予約時）
*/
void	AView_Text::ShowCaretIfReserved()
{
	if( mCaretTickCount >= 99999 )
	{
		ShowCaret();
	}
}

/**
キャレット一時非表示
@note DoDraw()前、および、スクロール実行前にNVIDO_ScrollPreProcess()からコールされる
*/
void	AView_Text::TempHideCaret()
{
	//#688
	//フォーカス無いときは何もしない
	/*cross caret hをフォーカスがないviewでも表示したいのでコメントアウト
	if( NVI_IsFocusActive() == false )
	{
		return;
	}
	*/
	
	mTempCaretHideCount++;
	if( mTempCaretHideCount != 1 )   return;
	if( IsCaretMode() == true && NVI_IsFocusActive() == true )
	{
		if( mCaretBlinkStateVisible == true )
		{
			DrawXorCaret(false);//#1034 常にflushしない
			//fprintf(stderr,"TEMPHIDE ");
		}
		
		if( mCrossCaretMode == true && mCrossCaretHVisible == true )
		{
			XorCrossCaretH();
		}
		if( mCrossCaretMode == true && mCrossCaretVVisible == true )
		{
			XorCrossCaretV();
		}
		
	}
}

/**
キャレット一時非表示解除
@note DoDraw()後、および、スクロール実行後にNVIDO_ScrollPostProcess()からコールされる
*/
void	AView_Text::RestoreTempHideCaret()
{
	//#688
	//フォーカス無いときは何もしない
	/*cross caret hをフォーカスがないviewでも表示したいのでコメントアウト
	if( NVI_IsFocusActive() == false )
	{
		return;
	}
	*/
	
	mTempCaretHideCount--;
	if( mTempCaretHideCount != 0 )   return;
	if( IsCaretMode() == true )
	{
		if( mCaretBlinkStateVisible == true && NVI_IsFocusActive() == true )
		{
			DrawXorCaret(false);//#1034 常にflushしない
			//fprintf(stderr,"RESTORETEMPHIDE ");
		}
		
		//
		if( mCrossCaretMode == true && mCrossCaretHVisible == true )
		{
			XorCrossCaretH();
		}
		if( mCrossCaretMode == true && mCrossCaretVVisible == true )
		{
			XorCrossCaretV();
		}
	}
}

//キャレットブリンク（キャレットタイマー毎に呼ばれる）
void	AView_Text::BlinkCaret()
{
	//状態initial系のときは何もせずリターン
	if( GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreeningInThread ||
	   GetTextDocumentConst().SPI_GetDocumentInitState() == kDocumentInitState_Initial_FileScreened )
	{
		return;
	}
	
	//fprintf(stderr,"BlinkCaret()");
	if( IsCaretMode() == true && NVI_IsFocusActive() == true )
	{
		/*R0184 ALocalPoint	spt,ept;
		GetCaretDrawData(spt,ept);
		NVMC_DrawXorCaretLine(spt,ept,true);*/
		DrawXorCaret(true);//#1034 常にflushする
		mCaretBlinkStateVisible = !mCaretBlinkStateVisible;
		//fprintf(stderr,"BLINK ");
	}
	mCaretTickCount = 0;
}

/**
*/
void	AView_Text::XorCrossCaretH()
{
	return;
	/*
	//
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayCaretWithHorizontalLine) == true ||
				GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayCrossCaret) == true )
	{
		//
		ALocalPoint	cspt,cept;
		GetCaretDrawData(cspt,cept);
		ALocalPoint	spt = cspt;
		ALocalPoint	ept = cept;
		ALocalRect	localFrame;
		NVM_GetLocalViewRect(localFrame);
		spt = cspt;
		ept = cept;
		spt.x = localFrame.left;
		ept.x = localFrame.right;
		spt.y = ept.y;
		spt.y--;
		ept.y--;
		//
		AImagePoint	ipt = {0};
		NVI_GetOriginOfLocalFrame(ipt);
		if( (ipt.x%2) == 1 )
		{
			spt.x--;
		}
		//
		NVMC_DrawXorCaretLine(spt,ept,true,true,1);
		//
		GetLineNumberView().SPI_RefreshLine(mCaretTextPoint.y);
	}
	*/
}

/**
垂直線キャレット
*/
void	AView_Text::XorCrossCaretV()
{
	if( GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDrawVerticalLineAsCaret) == true )
	{
		//キャレット位置データ取得
		ALocalPoint	cspt,cept;
		GetCaretDrawData(cspt,cept);
		ALocalPoint	spt = cspt;
		ALocalPoint	ept = cept;
		ALocalRect	localFrame;
		NVM_GetLocalViewRect(localFrame);
		//垂直線
		spt.y = localFrame.top;
		ept.y = localFrame.bottom;
		//
		AImagePoint	ipt = {0};
		NVI_GetOriginOfLocalFrame(ipt);
		if( (ipt.y%2) == 1 )
		{
			spt.y--;
		}
		//キャレット描画
		if( AEnvWrapper::GetOSVersion() <= kOSVersion_MacOSX_10_13 )//#1409 macOS 10.13以前はalpha指定しないようにする
		{
			//macOS 10.13以前の場合：alpha指定なし、点線 #1409
			NVMC_DrawXorCaretLine(spt,ept,true,false,true,1);//#688 #1034 常にflushしない
		}
		else
		{
			//macOS 10.14以降の場合：alpha指定あり
			NVMC_DrawXorCaretLine(spt,ept,true,false,false,1,0.3);//#688 #1034 常にflushしない #1398
		}
	}
}

//
void	AView_Text::SPI_ShowHideCrossCaret()
{
	//if( NVI_IsFocusActive() == true )
	{
		if( IsCaretMode() == true )
		{
			HideCaret(true,true);
		}
	}
	mCrossCaretMode = (mCrossCaretMode==false);
	//if( NVI_IsFocusActive() == true )
	{
		if( IsCaretMode() == true )
		{
			ShowCaret();
		}
	}
}

//
ABool	AView_Text::SPI_GetSelect( ATextPoint& outStartPoint, ATextPoint& outEndPoint ) const
{
	if( IsCaretMode() == false )
	{
		ATextPoint	caretPoint, selectPoint;
		GetCaretTextPoint(caretPoint);
		GetSelectTextPoint(selectPoint);
		ADocument_Text::OrderTextPoint(caretPoint,selectPoint,outStartPoint,outEndPoint);
	}
	else
	{
		GetCaretTextPoint(outStartPoint);
		GetCaretTextPoint(outEndPoint);
	}
	return IsCaretMode();
}

void	AView_Text::SPI_GetSelect( AArray<ATextIndex>& outStart, AArray<ATextIndex>& outEnd ) const
{
	outStart.DeleteAll();
	outEnd.DeleteAll();
	if( IsCaretMode() == false )
	{
		if( mKukeiSelected == false )
		{
			ATextIndex	start, end;
			GetSelect(start,end);
			outStart.Add(start);
			outEnd.Add(end);
		}
		else
		{
			for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
			{
				ATextIndex	start = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mKukeiSelected_Start.Get(i));
				ATextIndex	end = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(mKukeiSelected_End.Get(i));
				outStart.Add(start);
				outEnd.Add(end);
			}
		}
	}
	else
	{
		ATextIndex	start, end;
		GetSelect(start,end);
		outStart.Add(start);
		outEnd.Add(end);
	}
}

//B0000
//選択範囲取得
void	AView_Text::SPI_GetSelect( AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	GetSelect(outStartIndex,outEndIndex);
}

//
void	AView_Text::GetSelect( AIndex& outStartIndex, AIndex& outEndIndex ) const
{
	ATextPoint	spt,ept;
	SPI_GetSelect(spt,ept);
	outStartIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	outEndIndex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept);
}

//
void	AView_Text::SPI_GetImageSelect( AImagePoint& outStartPoint, AImagePoint& outEndPoint )
{
	/*R0108 ATextPoint	spt,ept;
	SPI_GetSelect(spt,ept);
	GetImagePointFromTextPoint(spt,outStartPoint);
	GetImagePointFromTextPoint(ept,outEndPoint);*/
	if( mCaretMode == true )//B0391
	{
		outStartPoint =  outEndPoint = mCaretImagePoint;
	}
	else
	{
		OrderImagePoint(mCaretImagePoint,mSelectImagePoint,outStartPoint,outEndPoint);
	}
}

//
void	AView_Text::SPI_DoLineInfoChangeStart()
{
	GetSelect(mSavedStartTextIndex,mSavedEndTextIndex);
}

//
void	AView_Text::SPI_DoLineInfoChangeEnd()
{
	SetSelect(mSavedStartTextIndex,mSavedEndTextIndex);
}

//#413
/**
キャレット位置補正
*/
void	AView_Text::SPI_CorrectCaretIfInvalid()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( GetTextDocumentConst().SPI_IsTextPointValid(spt) == false ||
				GetTextDocumentConst().SPI_IsTextPointValid(ept) == false )
	{
		spt.x = spt.y = 0;
		SPI_SetSelect(spt,spt);
	}
}

/**
選択位置のglobal point取得
*/
void	AView_Text::SPI_GetSelectGlobalRect( AGlobalRect& outRect ) 
{
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	AImagePoint	ispt = {0}, iept = {0};
	GetImagePointFromTextPoint(spt,ispt);
	GetImagePointFromTextPoint(ept,iept);
	ALocalPoint	lspt = {0}, lept = {0};
	NVM_GetLocalPointFromImagePoint(ispt,lspt);
	NVM_GetLocalPointFromImagePoint(iept,lept);
	AGlobalPoint	gspt = {0}, gept = {0};
	NVM_GetWindowConst().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lspt,gspt);
	NVM_GetWindowConst().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lept,gept);
	outRect.left		= gspt.x;
	outRect.top			= gspt.y;
	outRect.right		= gept.x;
	outRect.bottom		= gept.y + GetLineHeightWithMargin(ept.y);
}

/**
現在単語のglobal point取得
*/
void	AView_Text::SPI_GetCurrentWordGlobalRect( AGlobalRect& outRect ) 
{
	ATextIndex	wordSpos = 0, wordEpos = 0;
	GetCurrentWordForAbbrev(wordSpos,wordEpos);
	ATextPoint	spt = {0}, ept = {0};
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(wordSpos,spt);
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(wordEpos,ept);
	AImagePoint	ispt = {0}, iept = {0};
	GetImagePointFromTextPoint(spt,ispt);
	GetImagePointFromTextPoint(ept,iept);
	ALocalPoint	lspt = {0}, lept = {0};
	NVM_GetLocalPointFromImagePoint(ispt,lspt);
	NVM_GetLocalPointFromImagePoint(iept,lept);
	AGlobalPoint	gspt = {0}, gept = {0};
	NVM_GetWindowConst().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lspt,gspt);
	NVM_GetWindowConst().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),lept,gept);
	outRect.left		= gspt.x;
	outRect.top			= gspt.y;
	outRect.right		= gept.x;
	outRect.bottom		= gept.y + GetLineHeightWithMargin(ept.y);
}

#pragma mark ===========================

#pragma mark ---選択制御

void	AView_Text::SetSelectTextPoint( const ATextPoint& inTextPoint, const ABool inByFind )
{
	//#262
	mSelectionByFind = inByFind;
	
	//inTextPointがキャレット位置と同じなら、キャレットとして設定
	if( inTextPoint.x == GetCaretTextPoint().x && inTextPoint.y == GetCaretTextPoint().y )
	{
		SetCaretTextPoint(inTextPoint);
		return;
	}
	
	if( mKukeiSelected == true )
	{
		mKukeiSelected_Start.DeleteAll();
		mKukeiSelected_End.DeleteAll();
		mKukeiSelected = false;
	}
	AImageRect	refreshImageRect;
	if( IsCaretMode() == true )
	{
		HideCaret(true,true);
		mCaretMode = false;
		//#231 マルチバイト文字の途中にならないように補正
		ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(inTextPoint);
		ATextIndex	adjustedindex = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(textindex);
		if( textindex == adjustedindex )
		{
			//補正無し
			mSelectTextPoint = inTextPoint;
		}
		else
		{
			//補正あり
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(adjustedindex,mSelectTextPoint);
		}
		
		ATextPoint	pt1, pt2;
		ADocument_Text::OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
		GetLineImageRect(pt1.y,pt2.y+1,refreshImageRect);
	}
	else
	{
		ATextPoint	pt1, pt2;
		ADocument_Text::OrderTextPoint(GetSelectTextPoint(),inTextPoint,pt1,pt2);
		//#231 マルチバイト文字の途中にならないように補正
		ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(inTextPoint);
		ATextIndex	adjustedindex = GetTextDocumentConst().SPI_GetCurrentCharTextIndex(textindex);
		if( textindex == adjustedindex )
		{
			//補正無し
			mSelectTextPoint = inTextPoint;
		}
		else
		{
			//補正あり
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(adjustedindex,mSelectTextPoint);
		}
		
		GetLineImageRect(pt1.y,pt2.y+1,refreshImageRect);
	}
	//#571 エラー位置補正
	if( mSelectTextPoint.y < 0 )
	{
		mSelectTextPoint.y = 0;
	}
	if( mSelectTextPoint.y >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		mSelectTextPoint.y = GetTextDocumentConst().SPI_GetLineCount()-1;
	}
	if( mSelectTextPoint.x < 0 )
	{
		mSelectTextPoint.x = 0;
	}
	if( mSelectTextPoint.x > GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(mSelectTextPoint.y) )
	{
		mSelectTextPoint.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(mSelectTextPoint.y);
	}
	//R0108 高速化のため
	GetImagePointFromTextPoint(GetSelectTextPoint(),mSelectImagePoint);
	
	ALocalRect	localRect;
	NVM_GetLocalRectFromImageRect(refreshImageRect,localRect);
	//view右端までを描画
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	if( localRect.right < localFrameRect.right )
	{
		localRect.right = localFrameRect.right;
	}
	//
	NVMC_RefreshRect(localRect);
	/*#844
	//#496 現在行下線描画ありの場合、行描画更新
	if( GetModePrefDB().GetData_Bool(AModePrefDB::kShowUnderlineAtCurrentLine) == true && mLineNumberViewID != kObjectID_Invalid )
	*/
	if( mLineNumberViewID != kObjectID_Invalid )
	{
		GetLineNumberView().SPI_RefreshLine(mCaretTextPoint.y);
	}
	/*
	*/
	//#688
	//TextInputClientのselected rangeを消去（InlineInput処理中かどうかの判断は関数内で実施）
	NVMC_ResetSelectedRangeForTextInput();
	//#142
	//選択範囲テキストカウント更新
	UpdateTextCountWindows();
	
	//選択変更時JavaScriptプラグイン実行 #994
	//#994 GetTextDocumentConst().SPI_ExecuteEventListener("onSelectionChanged",GetEmptyText());
	
	//#750
	//現在単語ハイライト消去
	//GetTextDocument().SPI_ClearCurrentWordForHighlight(true);
	
	//ジャンプリスト更新のため、選択更新をウインドウへ通知
	NVM_GetWindow().OWICB_SelectionChanged(NVI_GetControlID());
	
	//括弧ハイライト消去 #1406
	SetHighlightBraceVisible(false);
}

//
void	AView_Text::SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inByFind )//#262
{
	//行折り返し計算中は選択予約してリターン#699
	if( GetTextDocumentConst().SPI_IsWrapCalculating() == true )
	{
		mReservedSelection_StartTextIndex = kIndex_Invalid;
		mReservedSelection_EndTextIndex = kIndex_Invalid;
		mReservedSelection_StartTextPoint = inStartPoint;
		mReservedSelection_EndTextPoint = inEndPoint;
		mReservedSelection_IsSelectWithParagraph = false;
		return;
	}
	//
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.x == inStartPoint.x && spt.y == inStartPoint.y && ept.x == inEndPoint.x && ept.y == inEndPoint.y )
	{
		mSelectionByFind = inByFind;
		return;
	}
	if( (GetCaretTextPoint().x == inStartPoint.x && GetCaretTextPoint().y == inStartPoint.y) == false ||
		(inStartPoint.x == inEndPoint.x && inStartPoint.y == inEndPoint.y) )//B0434
	{
		SetCaretTextPoint(inStartPoint);
	}
	if( inStartPoint.x == inEndPoint.x && inStartPoint.y == inEndPoint.y )   return;
	SetSelectTextPoint(inEndPoint,inByFind);//#262
}
void	AView_Text::SetSelect( const AIndex inStart, const AIndex inEnd, const ABool inByFind )//#262
{
	//行折り返し計算中は選択予約してリターン#699
	if( GetTextDocumentConst().SPI_IsWrapCalculating() == true )
	{
		mReservedSelection_StartTextIndex = inStart;
		mReservedSelection_EndTextIndex = inEnd;
		mReservedSelection_StartTextPoint.x = mReservedSelection_StartTextPoint.y = kIndex_Invalid;
		mReservedSelection_EndTextPoint.x = mReservedSelection_EndTextPoint.y = kIndex_Invalid;
		mReservedSelection_IsSelectWithParagraph = false;
		return;
	}
	//
	ATextPoint	spt, ept;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inStart,spt);
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inEnd,ept);
	SetSelect(spt,ept,inByFind);//#262
}
void	AView_Text::SPI_SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inByFind )
{
	SetSelect(inStartPoint,inEndPoint,inByFind);
}
void	AView_Text::SPI_SetSelect( const AIndex inStart, const AIndex inEnd, const ABool inByFind )
{
	SetSelect(inStart,inEnd,inByFind);
}

//#699
/**
選択を初期化する
@note 行折り返し計算中でも実行する（選択予約しない）
*/
void	AView_Text::SPI_InitSelect()
{
	SetCaretTextPoint(kTextPoint_00);
}

//#699
/**
段落indexで選択
*/
void	AView_Text::SPI_SetSelectWithParagraphIndex( 
		const AIndex inStartParagraphIndex, const AIndex inStartOffset, 
		const AIndex inEndParagraphIndex, const AIndex inEndOffset )
{
	//行折り返し計算中は選択予約してリターン#699
	if( GetTextDocumentConst().SPI_IsWrapCalculating() == true )
	{
		mReservedSelection_StartTextIndex = kIndex_Invalid;
		mReservedSelection_EndTextIndex = kIndex_Invalid;
		mReservedSelection_StartTextPoint.x = inStartOffset;
		mReservedSelection_StartTextPoint.y = inStartParagraphIndex;
		mReservedSelection_EndTextPoint.x = inEndOffset;
		mReservedSelection_EndTextPoint.y = inEndParagraphIndex;
		mReservedSelection_IsSelectWithParagraph = true;
		return;
	}
	
	//選択実行
	AIndex	spos = GetTextDocumentConst().
			SPI_GetParagraphStartTextIndex(inStartParagraphIndex) + inStartOffset;
	AIndex	epos = GetTextDocumentConst().
			SPI_GetParagraphStartTextIndex(inEndParagraphIndex) + inEndOffset;
	SetSelect(spos,epos);
}

//指定TextPointが、選択範囲内にあるかどうか
//inIncludeBoundary: 境界を含めるかどうか
ABool	AView_Text::IsInSelect( const AImagePoint& inImagePoint, const ABool inIncludeBoundary )
{
	if( IsCaretMode() == true )   return false;
	ATextPoint	textpoint;
	GetTextPointFromImagePoint(inImagePoint,textpoint);
	/* ATextPoint	lineend = textpoint;
	lineend.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(textpoint.y);
	AImagePoint	lineendImagePoint;
	GetImagePointFromTextPoint(lineend,lineendImagePoint);
	if( inImagePoint.x < 0 || inImagePoint.x > lineendImagePoint.x+1 )   return false;*/
	if( mKukeiSelected == false )
	{
		ATextPoint	spt, ept;
		SPI_GetSelect(spt,ept);
		AImagePoint	imagespt, imageept;
		GetImagePointFromTextPoint(spt,imagespt);
		GetImagePointFromTextPoint(ept,imageept);
		//spt.y == ept.yの場合
		if( spt.y == ept.y )
		{
			if( textpoint.y == spt.y )
			{
				/* if( inIncludeBoundary == true )
				{
					return (textpoint.x >= spt.x && textpoint.x <= ept.x);
				}
				else
				{
					return (textpoint.x > spt.x && textpoint.x < ept.x);
				}*/
				return ( inImagePoint.x >= imagespt.x && inImagePoint.x <= imageept.x );
			}
			else
			{
				return false;
			}
		}
		//以降、spt.y < ept.yの場合
		else if( textpoint.y == spt.y )
		{
			/* if( inIncludeBoundary == true )
			{
				return (textpoint.x >= spt.x);
			}
			else
			{
				return (textpoint.x > spt.x);
			}*/
			if( inIncludeBoundary == true )
			{
				return ( inImagePoint.x >= imagespt.x );
			}
			else
			{
				return ( inImagePoint.x > imagespt.x );
			}
		}
		else if( textpoint.y == ept.y )
		{
			/* if( inIncludeBoundary == true )
			{
				return (textpoint.x <= ept.x);
			}
			else
			{
				return (textpoint.x < ept.x);
			}*/
			return ( inImagePoint.x < imageept.x );
		}
		else if( textpoint.y > spt.y && textpoint.y < ept.y )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
		{
			if( mKukeiSelected_Start.Get(i).y == textpoint.y )
			{
				if( inIncludeBoundary == true )
				{
					return ( textpoint.x >= mKukeiSelected_Start.Get(i).x && textpoint.x <= mKukeiSelected_End.Get(i).x);
				}
				else
				{
					return ( textpoint.x > mKukeiSelected_Start.Get(i).x && textpoint.x < mKukeiSelected_End.Get(i).x );
				}
			}
		}
		return false;
	}
}

//
void	AView_Text::SetKukeiSelect( const AImagePoint& inImagePoint1, const AImagePoint& inImagePoint2 )
{
	//旧矩形選択の行範囲を取得し、旧矩形選択データを削除
	AIndex	oldStartLineIndex = GetTextDocumentConst().SPI_GetLineCount();
	AIndex	oldEndLineIndex = 0;
	if( mKukeiSelected == true )
	{
		ATextPoint	spt, ept;
		GetKukeiContainRange(spt,ept);
		oldStartLineIndex = spt.y;
		oldEndLineIndex = ept.y;
	}
	mKukeiSelected_Start.DeleteAll();
	mKukeiSelected_End.DeleteAll();
	//
	ATextPoint	pt1, pt2;
	GetTextPointFromImagePoint(inImagePoint1,pt1);
	GetTextPointFromImagePoint(inImagePoint2,pt2);
	//
	AIndex	startLineIndex = pt1.y;
	AIndex	endLineIndex = pt2.y;
	if( startLineIndex == endLineIndex )
	{
		//同じ行なら通常の選択で十分なのでそちらで。（キャレットモードへの変更もそちらで）
		SPI_SetSelect(pt1,pt2);
		//描画更新
		SPI_RefreshLines(oldStartLineIndex,oldEndLineIndex+1);
		return;
	}
	if( endLineIndex < startLineIndex )
	{
		AIndex	tmp = endLineIndex;
		endLineIndex = startLineIndex;
		startLineIndex = tmp;
	}
	//
	if( IsCaretMode() == true )
	{
		HideCaret(true,true);
		mCaretMode = false;
	}
	//
	ANumber	startImageX = inImagePoint1.x;
	ANumber	endImageX = inImagePoint2.x;
	if( endImageX < startImageX )
	{
		ANumber	tmp = endImageX;
		endImageX = startImageX;
		startImageX = tmp;
	}
	//
	for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
	{
		AImagePoint	ispt;
		ispt.x = startImageX;
		ispt.y = GetImageYByLineIndex(lineIndex);
		AImagePoint	iept;
		iept.x = endImageX;
		iept.y = ispt.y;
		ATextPoint	start, end;
		GetTextPointFromImagePoint(ispt,start);
		GetTextPointFromImagePoint(iept,end);
		mKukeiSelected_Start.Add(start);
		mKukeiSelected_End.Add(end);
	}
	mKukeiSelected = true;
	mCaretMode = false;
	mSelectTextPoint = GetCaretTextPoint();//念のため
	//
	AIndex	drawStartLineIndex = startLineIndex;
	if( drawStartLineIndex > oldStartLineIndex )
	{
		drawStartLineIndex = oldStartLineIndex;
	}
	AIndex	drawEndLineIndex = endLineIndex;
	if( drawEndLineIndex < oldEndLineIndex )
	{
		drawEndLineIndex = oldEndLineIndex;
	}
	AImageRect	refreshImageRect;
	GetLineImageRect(drawStartLineIndex,drawEndLineIndex+1,refreshImageRect);
	ALocalRect	localRect;
	NVM_GetLocalRectFromImageRect(refreshImageRect,localRect);
	//view右端までを描画更新
	ALocalRect	localFrameRect = {0};
	NVM_GetLocalViewRect(localFrameRect);
	if( localRect.right < localFrameRect.right )
	{
		localRect.right = localFrameRect.right;
	}
	//#1123
	//テキスト情報更新
	UpdateTextCountWindows();
	//
	NVMC_RefreshRect(localRect);
}

//#1035
/**
矩形選択設定
*/
void	AView_Text::SetKukeiSelect( const AArray<ATextIndex>& inStartArray, const AArray<ATextIndex>& inEndArray )
{
	//矩形選択全削除
	mKukeiSelected_Start.DeleteAll();
	mKukeiSelected_End.DeleteAll();
	//設定
	for( AIndex i = 0; i < inStartArray.GetItemCount(); i++ )
	{
		ATextPoint	spt = {0};
		ATextPoint	ept = {0};
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inStartArray.Get(i),spt);
		GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inEndArray.Get(i),ept);
		mKukeiSelected_Start.Add(spt);
		mKukeiSelected_End.Add(ept);
	}
	//矩形選択モード設定
	mKukeiSelected = true;
	mCaretMode = false;
	mSelectTextPoint = GetCaretTextPoint();//念のため
	//#1123
	//テキスト情報更新
	UpdateTextCountWindows();
	//描画更新
	SPI_RefreshTextView();
}

void	AView_Text::GetKukeiContainRange( ATextPoint& outStart, ATextPoint& outEnd ) const
{
	outStart.x = 0;
	outStart.y = GetTextDocumentConst().SPI_GetLineCount();
	outEnd.x = 0;
	outEnd.y = 0;
	for( AIndex i = 0; i < mKukeiSelected_Start.GetItemCount(); i++ )
	{
		if( mKukeiSelected_Start.Get(i).y < outStart.y )
		{
			outStart = mKukeiSelected_Start.Get(i);
		}
		if( mKukeiSelected_End.Get(i).y > outEnd.y )
		{
			outEnd = mKukeiSelected_End.Get(i);
		}
	}
}

void	AView_Text::SPI_EditPreProcess( const ATextIndex inTextIndex, const AItemCount inInsertedCount )
{
	GetSelect(mSavedStartTextIndexForEditPrePostProcess,mSavedEndTextIndexForEditPrePostProcess);
	if( inInsertedCount > 0 )
	{
		if( mSavedStartTextIndexForEditPrePostProcess > inTextIndex )
		{
			mSavedStartTextIndexForEditPrePostProcess += inInsertedCount;
			mSavedEndTextIndexForEditPrePostProcess += inInsertedCount;
		}
		else if( mSavedEndTextIndexForEditPrePostProcess > inTextIndex )
		{
			mSavedEndTextIndexForEditPrePostProcess += inInsertedCount;
		}
	}
	if( inInsertedCount < 0 )
	{
		if( mSavedStartTextIndexForEditPrePostProcess > inTextIndex-inInsertedCount )
		{
			mSavedStartTextIndexForEditPrePostProcess += inInsertedCount;
			mSavedEndTextIndexForEditPrePostProcess += inInsertedCount;
		}
		else if( mSavedStartTextIndexForEditPrePostProcess > inTextIndex )
		{
			mSavedStartTextIndexForEditPrePostProcess = inTextIndex;
			mSavedEndTextIndexForEditPrePostProcess = inTextIndex;
		}
		else if( mSavedEndTextIndexForEditPrePostProcess > inTextIndex )
		{
			mSavedStartTextIndexForEditPrePostProcess = inTextIndex;
			mSavedEndTextIndexForEditPrePostProcess = inTextIndex;
		}
	}
}

void	AView_Text::SPI_EditPostProcess( const ATextIndex inTextIndex, const AItemCount inInsertedCount,
		const ABool inUpdateCaretImagePoint )//#846
{
	SetSelect(mSavedStartTextIndexForEditPrePostProcess,mSavedEndTextIndexForEditPrePostProcess);//★ #695
	/*無くてもキャレット描画位置が更新されないだけなので、速度優先にしたほうが良い
	if( inUpdateCaretImagePoint == true )//#846
	{
		UpdateCaretImagePoint();//キャレット描画位置補正 #0 フォントサイズ変更時等、ここでキャレット描画位置を補正する
	}
	*/
}

//#913
/**
段落全体を選択
*/
void	AView_Text::SelectWholeParagraph()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.x != ept.x || spt.y != ept.y )//B0327 条件追加（空行でキャレット状態の場合はept.y--しないようにする）
	{
		if( ept.x == 0 && ept.y > 0 )   ept.y--;
	}
	ATextIndex	spos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(spt.y));
	ATextIndex	epos = GetTextDocumentConst().SPI_GetParagraphStartTextIndex(GetTextDocumentConst().SPI_GetParagraphIndexByLineIndex(ept.y)+1);
	SetSelect(spos,epos);
}

//#913
/**
行全体を選択
*/
void	AView_Text::SelectWholeLine()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( ept.x == 0 && ept.y > 0 && ept.y > spt.y )   ept.y--;
	spt.x = 0;
	ept.x = 0;
	ept.y++;
	//B0361
	if( ept.y >= GetTextDocumentConst().SPI_GetLineCount() )
	{
		ept.y = GetTextDocumentConst().SPI_GetLineCount()-1;
		ept.x = GetTextDocumentConst().SPI_GetLineLengthWithLineEnd(ept.y);
	}
	
	SetSelect(spt,ept);
}

//#913
/**
マークを設定
*/
void	AView_Text::SetMark()
{
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	AIndex	oldMarkStartLineIndex = mMarkStartTextPoint.y;
	AIndex	oldMarkEndLineIndex = mMarkEndTextPoint.y;
	mMarkStartTextPoint = spt;
	mMarkEndTextPoint = ept;
	SPI_RefreshLine(oldMarkStartLineIndex);
	SPI_RefreshLine(oldMarkEndLineIndex);
	SPI_RefreshLine(mMarkStartTextPoint.y);
	SPI_RefreshLine(mMarkEndTextPoint.y);
}

//#913
/**
マークと選択範囲を交換
*/
void	AView_Text::SwapMark()
{
	ATextPoint	spt = {0}, ept = {0};
	SPI_GetSelect(spt,ept);
	if( GetTextDocumentConst().SPI_IsTextPointValid(mMarkStartTextPoint) == true &&
	   GetTextDocumentConst().SPI_IsTextPointValid(mMarkEndTextPoint) == true )
	{
		SPI_SetSelect(mMarkStartTextPoint,mMarkEndTextPoint);
	}
	AIndex	oldMarkStartLineIndex = mMarkStartTextPoint.y;
	AIndex	oldMarkEndLineIndex = mMarkEndTextPoint.y;
	mMarkStartTextPoint = spt;
	mMarkEndTextPoint = ept;
	SPI_RefreshLine(oldMarkStartLineIndex);
	SPI_RefreshLine(oldMarkEndLineIndex);
	SPI_RefreshLine(mMarkStartTextPoint.y);
	SPI_RefreshLine(mMarkEndTextPoint.y);
}

//#913
/**
マークまでを選択
*/
void	AView_Text::SelectToMark()
{
	if( GetTextDocumentConst().SPI_IsTextPointValid(mMarkStartTextPoint) == true &&
	   GetTextDocumentConst().SPI_IsTextPointValid(mMarkEndTextPoint) == true )
	{
		ATextPoint	spt = {0}, ept = {0};
		SPI_GetSelect(spt,ept);
		ATextPoint	dummy = {0};
		ADocument_Text::OrderTextPoint(mMarkStartTextPoint,spt,spt,dummy);
		ADocument_Text::OrderTextPoint(mMarkEndTextPoint,ept,dummy,ept);
		SetSelect(spt,ept);
	}
}

//#858
#if 0
#pragma mark ===========================

#pragma mark ---付箋紙

//#138
/**
付箋紙表示更新

DrawPostProcess等や、付箋紙データ更新時にドキュメントのメソッドからコールされる
Viewには、現在表示中の部分のみ、データが存在する。スクロールアウトされると付箋紙ウインドウは削除される。ビューがHideされても、付箋紙ウインドウは削除される。
*/
void	AView_Text::SPI_UpdateFusen( const ABool inUpdateAll )
{
	//付箋紙データ更新時等は、最初に付箋紙を全て削除する。
	if( inUpdateAll == true )
	{
		//付箋紙を全て削除する。
		while( mFusenArray_WindowID.GetItemCount() > 0 )
		{
			SPI_DeleteFusenWindow(mFusenArray_WindowID.Get(0));
		}
	}
	//TextViewのbounds取得
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//表示済みフラグ作成
	ABoolArray	shownFlagArray;
	for( AIndex i = 0; i < mFusenArray_CommentIndex.GetItemCount(); i++ )
	{
		shownFlagArray.Add(false);
	}
	//TextViewの表示範囲行を取得
	AIndex	startLineIndex = GetLineIndexByImageY(imageFrameRect.top);
	AIndex	endLineIndex = GetLineIndexByImageY(imageFrameRect.bottom);//#450 imageFrameRect.bottom/GetLineHeightWithMargin();
	for( AIndex lineIndex = startLineIndex; lineIndex <= endLineIndex; lineIndex++ )
	{
		//行に対応するコメントIndex取得
		AIndex	commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(lineIndex);
		if( commentIndex != kIndex_Invalid )
		{
			//TextView内部配列のIndexを取得
			AIndex	index = mFusenArray_CommentIndex.Find(commentIndex);
			if( index == kIndex_Invalid )
			{
				//AView_Text内の配列にデータ無しの場合
				
				//AWindow_Fusen生成
				AFusenWindowFactory	windowfactory(GetObjectID(),
									GetTextDocumentConst().SPI_GetProjectObjectID(),
									GetTextDocumentConst().SPI_GetProjectRelativePath(),
									GetTextDocumentConst().SPI_GetExternalCommentModuleName(commentIndex),
									GetTextDocumentConst().SPI_GetExternalCommentOffset(commentIndex));
				AObjectID	winID = GetApp().NVI_CreateWindow(windowfactory);
				GetApp().NVI_GetWindowByID(winID).NVI_Create(kObjectID_Invalid);
				GetApp().NVI_GetWindowByID(winID).NVI_SetAsStickyWindow(NVM_GetWindow().GetObjectID());
				//内部配列にデータ追加
				mFusenArray_CommentIndex.Add(commentIndex);
				mFusenArray_WindowID.Add(winID);
				AWindowPoint	windowPoint = {0,0};
				mFusenArray_WindowOffset.Add(windowPoint);
				//付箋紙の位置設定
				SPI_RepositionFusenWindow(winID);
				//表示
				if( NVI_IsVisible() == true && NVI_IsFocusActive() == true )
				{
					GetApp().NVI_GetWindowByID(winID).NVI_Show(false);
				}
				else
				{
					GetApp().NVI_GetWindowByID(winID).NVI_Hide();
				}
				//表示済みフラグOn
				shownFlagArray.Add(true);
			}
			else
			{
				//AView_Text内の配列にデータ有りの場合
				
				//Stickyウインドウの位置補正
				SPI_RepositionFusenWindow(mFusenArray_WindowID.Get(index));
				//表示
				if( NVI_IsVisible() == true && NVI_IsFocusActive() == true )
				{
					GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(index)).NVI_Show(false);
				}
				else
				{
					GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(index)).NVI_Hide();
				}
				//表示済みフラグOn
				shownFlagArray.Set(index,true);
			}
		}
	}
	//表示しなかったStickyウインドウは削除する
	for( AIndex i = 0; i < shownFlagArray.GetItemCount(); )
	{
		if( shownFlagArray.Get(i) == false )
		{
			shownFlagArray.Delete1(i);
			SPI_DeleteFusenWindow(mFusenArray_WindowID.Get(i));
		}
		else
		{
			i++;
		}
	}
}

//#138
/**
付箋紙のShow/Hide更新
*/
void	AView_Text::SPI_UpdateFusenVisible()
{
	for( AIndex i = 0; i < mFusenArray_CommentIndex.GetItemCount(); i++ )
	{
		if( NVI_IsVisible() == true )
		{
			GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(i)).NVI_Show();
		}
		else
		{
			GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(i)).NVI_Hide();
		}
	}
}

/**
付箋紙削除
*/
void	AView_Text::SPI_DeleteFusenWindow( const AWindowID inFusenWindowID )
{
	//AView_Text内内部配列削除
	AIndex index = mFusenArray_WindowID.Find(inFusenWindowID);
	if( index == kIndex_Invalid )   { _ACError("Fusen not found",this); return; }
	//内部配列から削除
	mFusenArray_CommentIndex.Delete1(index);
	mFusenArray_WindowID.Delete1(index);
	mFusenArray_WindowOffset.Delete1(index);
	//付箋ウインドウ削除
	GetApp().NVI_DeleteWindow(inFusenWindowID);
}

/**
付箋紙位置を現在のTextViewのスクロール位置に合わせる

@param inFusenWindowID 付箋紙のWindowID
*/
void	AView_Text::SPI_RepositionFusenWindow( const AWindowID inFusenWindowID )
{
	AIndex index = mFusenArray_WindowID.Find(inFusenWindowID);
	if( index == kIndex_Invalid )   { _ACError("Fusen not found",this); return; }
	
	//TextViewのbounds取得
	AImageRect	imageFrameRect;
	NVM_GetImageViewRect(imageFrameRect);
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	//付箋紙位置計算
	ALocalPoint	localPoint;
	AWindowPoint	windowPoint;
	localPoint.x = localFrameRect.right - GetApp().NVI_GetWindowByID(mFusenArray_WindowID.Get(index)).NVI_GetWindowWidth() + 24;
	localPoint.y = GetImageYByLineIndex(GetTextDocumentConst().SPI_GetExternalCommentLineIndex(mFusenArray_CommentIndex.Get(index))) - imageFrameRect.top;
	NVM_GetWindow().NVI_GetWindowPointFromControlLocalPoint(NVI_GetControlID(),localPoint,windowPoint);
	//内部配列に保存しておいた位置から変更があれば、位置移動
	AWindowPoint	oldwpt = mFusenArray_WindowOffset.Get(index);
	if( oldwpt.x != windowPoint.x || oldwpt.y != windowPoint.y )
	{
		GetApp().NVI_GetWindowByID(NVM_GetWindow().GetObjectID()).NVI_SetChildStickyWindowOffset(inFusenWindowID,windowPoint);
		mFusenArray_WindowOffset.Set(index,windowPoint);
	}
}

/**
付箋紙位置から、モジュール名・オフセットを取得

付箋紙編集ウインドウの位置を動かしたときに、付箋紙編集ウインドウから、モジュール名・オフセットを知るために使用される。
@param inFusenEditWindowPos 付箋紙編集ウインドウの位置
@param outModuleName モジュール名
@param outOffset モジュール名からのオフセット
*/
void	AView_Text::SPI_GetFusenModuleAndOffset( const APoint inFusenEditWindowPos, AText& outModuleName, ANumber& outOffset )
{
	//テキストウインドウ位置取得
	APoint	docWinPos;
	NVM_GetWindow().NVI_GetWindowPosition(docWinPos);
	//相対位置取得
	AWindowPoint	wpt;
	wpt.x = inFusenEditWindowPos.x - docWinPos.x;
	wpt.y = inFusenEditWindowPos.y - docWinPos.y;
	//TextViewのLocalPointへ変換→ImagePointへ変換→TextPointへ変換
	ALocalPoint	lpt;
	NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),wpt,lpt);
	AImagePoint	ipt;
	NVM_GetImagePointFromLocalPoint(lpt,ipt);
	ATextPoint	tpt;
	GetTextPointFromImagePoint(ipt,tpt);
	//行番号からモジュール名・オフセット取得
	AText	newModuleName;
	//ANumber	newOffset = 0;
	GetTextDocumentConst().SPI_GetModuleNameAndOffsetByLineIndex(tpt.y,outModuleName,outOffset);
}

/**
新規付箋紙／または既存付箋紙編集

@param inLineIndex 行番号
*/
void	AView_Text::EditOrNewFusen( const AIndex inLineIndex )
{
	//該当行のコメントIndex取得
	AIndex	commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(inLineIndex);
	if( commentIndex == kIndex_Invalid )
	{
		//コメントが存在していなければ新規付箋紙データ作成
		
		//行番号からモジュール名・オフセット取得
		AText	moduleName;
		ANumber	offset = 0;
		GetTextDocumentConst().SPI_GetModuleNameAndOffsetByLineIndex(inLineIndex,moduleName,offset);
		//新規付箋データ追加
		AText	comment;
		GetApp().SPI_SetExternalCommentToProjectDB(GetTextDocumentConst().SPI_GetProjectObjectID(),
				GetTextDocumentConst().SPI_GetProjectRelativePath(),
				moduleName,offset,comment);
		//該当行のコメントIndex取得
		commentIndex = GetTextDocument().SPI_FindExternalCommentIndexByLineIndex(inLineIndex);
		if( commentIndex == kIndex_Invalid )
		{
			_ACError("",this);
			return;
		}
	}
	//付箋紙表示ウインドウのwinIDを取得し、編集を指示
	AWindowID	fusenWindowID = mFusenArray_WindowID.Get(mFusenArray_CommentIndex.Find(commentIndex));
	GetApp().SPI_GetFusenWindow(fusenWindowID).SPI_EditFusen();
}
#endif

/**
見出しリストホバー更新時処理
*/
void	AView_Text::SPI_DoListViewHoverUpdated( const AIndex inHoverStartDBIndex, const AIndex inHoverEndDBIndex )
{
	/*実装途中
	AIndex	hoverStartLineIndex = kIndex_Invalid, hoverEndLineIndex = kIndex_Invalid;
	if( inHoverStartDBIndex != kIndex_Invalid )
	{
		AUniqID	identifier = GetTextDocumentConst().SPI_GetJumpMenuIdentifierUniqID(inHoverStartDBIndex);
		ATextPoint	spt = {0}, ept = {0};
		GetTextDocumentConst().SPI_GetGlobalIdentifierRange(identifier,spt,ept);
		hoverStartLineIndex = spt.y;
	}
	if( inHoverEndDBIndex != kIndex_Invalid )
	{
		AUniqID	identifier = GetTextDocumentConst().SPI_GetJumpMenuIdentifierUniqID(inHoverEndDBIndex);
		ATextPoint	spt = {0}, ept = {0};
		GetTextDocumentConst().SPI_GetGlobalIdentifierRange(identifier,spt,ept);
		hoverEndLineIndex = ept.y;
	}
	GetLineNumberView().SPI_DoListViewHoverUpdated(hoverStartLineIndex,hoverEndLineIndex);
	*/
}

#pragma mark ===========================

#pragma mark ---行image情報（折りたたみ・行ごと高さ対応）
//#450

/**
行ごと高さ・image位置情報初期化
*/
void	AView_Text::SPI_InitLineImageInfo()
{
	//mLineHeight等の更新
	SPI_UpdateTextDrawProperty();
	//
	mLineImageInfo_Height.DeleteAll();
	mLineImageInfo_ImageY.DeleteAll();
	mLineImageInfo_Collapsed.DeleteAll();
	
	//#695 行数拡大時の再割り当て増加量の設定
	AItemCount	lineReallocateStep = GetTextDocumentConst().SPI_GetLineReallocateStep();
	mLineImageInfo_Height.SetReallocateStep(lineReallocateStep);
	mLineImageInfo_ImageY.SetReallocateStep(lineReallocateStep);
	mLineImageInfo_Collapsed.SetReallocateStep(lineReallocateStep);
	
	//#695
	ANumber	lineHeightPlusMargin = GetLineHeightPlusMargin();
	
	//
	ANumber	imageY = 0;
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	{
		//LineImageInfoデータReserve#695
		mLineImageInfo_Height.Reserve(0,lineCount);
		mLineImageInfo_ImageY.Reserve(0,lineCount);
		mLineImageInfo_Collapsed.Reserve(0,lineCount);
		//ポインタ取得
		AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
		ANumber*	p_height = arrayptr_height.GetPtr();
		AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
		ANumber*	p_imageY = arrayptr_imageY.GetPtr();
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			ANumber	lineHeight = lineHeightPlusMargin;//#695 CalcLineHeight(lineIndex);
			/*#493一旦機能drop
			if( foldingLevelRef.Get(lineIndex) == kFoldingLevel_Header )
			{
			lineHeight *= multiplyRef.Get(lineIndex);
			lineHeight /= 100;
			}
			*/
			/*#695
			mLineImageInfo_Height.Add(lineHeight);
			mLineImageInfo_ImageY.Add(imageY);
			mLineImageInfo_Collapsed.Add(false);
			*/
			p_height[lineIndex] = lineHeight;
			p_imageY[lineIndex] = imageY;
			p_collapsed[lineIndex] = false;
			//
			imageY += lineHeight;
		}
	}
	//
	mLineImageInfo_Height.Add(0);
	mLineImageInfo_ImageY.Add(imageY);
	mLineImageInfo_Collapsed.Add(false);
	//
	SPI_UpdateImageSize();
}

/*#493一旦機能drop 
**
各行の高さを計算
*
ANumber	AView_Text::CalcLineHeight( const AIndex inLineIndex ) const
{
	ANumber	lineHeight = (GetLineHeightPlusMargin())
			* GetTextDocumentConst().SPI_GetLineFontSizeMultiply(inLineIndex);
	return lineHeight;
}
*/

/**
行ごと高さ・image位置情報更新
@param inStartLineIndex 編集開始行index
@param inInsertedLineCount 追加行数
@param inUpdateStartLineIndex 行情報更新開始index
*/
ANumber	AView_Text::SPI_UpdateLineImageInfo( const AIndex inStartLineIndex, const AItemCount inInsertedLineCount,
		const AIndex inUpdateStartLineIndex )
{
	//行挿入・削除時、挿入・削除開始行のcollapsedフラグがONなら、あらかじめexpandする
	//（挿入時：開始行以降に行挿入するので、collapse解除しておかないと、collapseフラグONの後に行追加してしまう。
	// 削除時：削除開始行のmLineImageInfo_xxxは削除せず、それ以降の削除行のmLineImageInfo_xxxが削除されるので、
	// 削除開始行のcollapse解除しておかないと、collapse表示が残ってしまう。（collpaseされていた行は削除される。）
	if( inInsertedLineCount != 0 )
	{
		if( mLineImageInfo_Collapsed.Get(inStartLineIndex) == true )
		{
			SPI_ExpandCollapse(inStartLineIndex,kExpandCollapseType_ForceExpand,false);
		}
	}
	
	//#695
	ANumber	lineHeightPlusMargin = GetLineHeightPlusMargin();
	
	//#695 行数拡大時の再割り当て増加量の設定
	AItemCount	lineReallocateStep = GetTextDocumentConst().SPI_GetLineReallocateStep();
	mLineImageInfo_Height.SetReallocateStep(lineReallocateStep);
	mLineImageInfo_ImageY.SetReallocateStep(lineReallocateStep);
	mLineImageInfo_Collapsed.SetReallocateStep(lineReallocateStep);
	
	//inUpdateStartLineIndex～inStartLineIndexの更新
	ANumber	imageY = mLineImageInfo_ImageY.Get(inUpdateStartLineIndex);
	AIndex	lineIndex = inUpdateStartLineIndex;
	{
		//ポインタ取得
		AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
		ANumber*	p_height = arrayptr_height.GetPtr();
		AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
		ANumber*	p_imageY = arrayptr_imageY.GetPtr();
		for( ; lineIndex <= inStartLineIndex; lineIndex++ )
		{
			ANumber	lineHeight = lineHeightPlusMargin;//#695 CalcLineHeight(lineIndex);
			/*#493一旦機能drop
			if( foldingLevelRef.Get(lineIndex) == kFoldingLevel_Header )
			{
			lineHeight *= multiplyRef.Get(lineIndex);
			lineHeight /= 100;
			}
			*/
			//
			//元々高さ0（＝collapse中）なら高さ0のままにする
			//if( mLineImageInfo_Height.Get(lineIndex) == 0 )
			if( p_height[lineIndex] == 0 )
			{
				lineHeight = 0;
			}
			//
			//mLineImageInfo_Height.Set(lineIndex,lineHeight);
			//mLineImageInfo_ImageY.Set(lineIndex,imageY);
			p_height[lineIndex] = lineHeight;
			p_imageY[lineIndex] = imageY;
			imageY += lineHeight;
		}
	}
	//行削除
	if( inInsertedLineCount < 0 )
	{
		mLineImageInfo_Height.Delete(lineIndex,-inInsertedLineCount);
		mLineImageInfo_ImageY.Delete(lineIndex,-inInsertedLineCount);
		mLineImageInfo_Collapsed.Delete(lineIndex,-inInsertedLineCount);
	}
	//行追加
	if( inInsertedLineCount > 0 )
	{
		//LineImageInfoデータReserve#695
		mLineImageInfo_Height.Reserve(lineIndex,inInsertedLineCount);
		mLineImageInfo_ImageY.Reserve(lineIndex,inInsertedLineCount);
		mLineImageInfo_Collapsed.Reserve(lineIndex,inInsertedLineCount);
		//ポインタ取得
		AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
		ANumber*	p_height = arrayptr_height.GetPtr();
		AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
		ANumber*	p_imageY = arrayptr_imageY.GetPtr();
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		for( AIndex i = 0; i < inInsertedLineCount; i++ )
		{
			ANumber	lineHeight = lineHeightPlusMargin;//#695 CalcLineHeight(lineIndex);
			/*#493一旦機能drop
			if( foldingLevelRef.Get(lineIndex) == kFoldingLevel_Header )
			{
				lineHeight *= multiplyRef.Get(lineIndex);
				lineHeight /= 100;
			}
			 */
			/*#695
			mLineImageInfo_Height.Insert1(lineIndex,lineHeight);
			mLineImageInfo_ImageY.Insert1(lineIndex,imageY);
			mLineImageInfo_Collapsed.Insert1(lineIndex,false);
			*/
			p_height[lineIndex] = lineHeight;
			p_imageY[lineIndex] = imageY;
			p_collapsed[lineIndex] = false;
			//
			imageY += lineHeight;
			lineIndex++;
		}
	}
	//
	//削除・追加行よりあとのimageYをずらしていく（ずれがあるばあいのみ）
	ANumber	delta = imageY - mLineImageInfo_ImageY.Get(lineIndex);
	if( delta != 0 )
	{
		AStArrayPtr<ANumber>	arrayptr(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());//#695
		ANumber*	p = arrayptr.GetPtr();
		AItemCount	itemCount = mLineImageInfo_ImageY.GetItemCount();//#695
		for( ; lineIndex < itemCount; lineIndex++ )
		{
			//#695 mLineImageInfo_ImageY.Set(lineIndex,mLineImageInfo_ImageY.Get(lineIndex)+delta);
			p[lineIndex] += delta;
		}
	}
	//行削除の場合、削除した行のなかに、collapsed開始行が含まれている可能性があり、削除済み行の次の行以降がcollapsedされている可能性があるので、これを解除する
	if( inInsertedLineCount < 0 )
	{
		if( mLineImageInfo_Height.Get(inStartLineIndex+1) == 0 )
		{
			SPI_ExpandCollapse(inStartLineIndex,kExpandCollapseType_ForceExpand,false);
		}
	}
	//imagesizeの更新
	SPI_UpdateImageSize();
	//削除・追加行よりあとのimageYにずれがあったらtrueを返す
	return delta;
}

/**
折りたたまれた行かどうかを取得
*/
ABool	AView_Text::SPI_IsCollapsedLine( const AIndex inLineIndex ) const
{
	return mLineImageInfo_Collapsed.Get(inLineIndex);
}

/**
折りたたみexpand/collapse
@note inLineIndexは折りたたみ開始行、または、終了行
*/
void	AView_Text::SPI_ExpandCollapse( const AIndex inLineIndex, const AExpandCollapseType inExpandCollapseType,
		const ABool inRedraw )
{
	//クリック行の元々のimage yを記憶
	ANumber	originalImageY = GetImageYByLineIndex(inLineIndex);
	
	//折りたたみ開始行／終了行
	AIndex	foldingStartLineIndex = inLineIndex;
	AIndex	foldingEndLineIndex = inLineIndex;
	//クリック行の折りたたみレベル取得
	AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(inLineIndex);
	ABool	isFoldingStart = SPI_IsFoldingStart(foldingLevel);
	ABool	isFoldingEnd = SPI_IsFoldingEnd(foldingLevel);
	//指定行が折りたたみ開始でも終了でもなく、かつ、強制折りたたみ解除の場合、指定行より後で最初にcollapsed状態ではなくなる行を終了行にする
	if( (isFoldingStart == false && isFoldingEnd == false ) || inExpandCollapseType == kExpandCollapseType_ForceExpand )
	{
		if( inLineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
		{
			if( mLineImageInfo_Height.Get(inLineIndex+1) == 0 )
			{
				//最初にcollapsed状態ではなくなる行を終了行にする
				foldingEndLineIndex = inLineIndex+1;
				for( ; foldingEndLineIndex < GetTextDocumentConst().SPI_GetLineCount(); foldingEndLineIndex++ )
				{
					if( mLineImageInfo_Height.Get(foldingEndLineIndex) > 0 )
					{
						break;
					}
				}
			}
		}
	}
	//折りたたみ開始行クリックの場合、折りたたみ終了行を取得
	else if( isFoldingStart == true )
	{
		foldingEndLineIndex = SPI_FindFoldingEndLineIndex(inLineIndex);
	}
	//折りたたみ終了行クリックの場合、折りたたみ開始行を取得
	else if( isFoldingEnd == true )
	{
		foldingStartLineIndex = SPI_FindFoldingStartLineIndex(inLineIndex);
	}
	
	//開始行／終了行検索に失敗した場合は何もせずリターン
	if( foldingStartLineIndex == foldingEndLineIndex )
	{
		//指定行が折りたたみ状態となっている場合は、異常状態なので、折りたたみ状態を解除する。
		if( mLineImageInfo_Collapsed.Get(inLineIndex) == true )
		{
			mLineImageInfo_Collapsed.Set(inLineIndex,false);
			//textview, linenumber view, diff viewを表示更新
			SPI_RefreshTextView();
		}
		//
		return;
	}
	
	//行数取得
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	//折りたたみ開始位置の次の行からlineIndex開始
	AIndex lineIndex = foldingStartLineIndex+1;
	if( lineIndex >= lineCount )
	{
		return;
	}
	
	//imageY
	ANumber	imageY = mLineImageInfo_ImageY.Get(lineIndex);
	
	//collapseするかexpandするかを決定
	ABool	collapse = false;
	switch(inExpandCollapseType)
	{
	  case kExpandCollapseType_Switch:
		{
			collapse = !(mLineImageInfo_Collapsed.Get(foldingStartLineIndex));
			break;
		}
	  case kExpandCollapseType_Collapse:
		{
			collapse = true;
			break;
		}
	  case kExpandCollapseType_Expand:
	  case kExpandCollapseType_ForceExpand:
		{
			collapse = false;
			break;
		}
	  default:
		{
			//処理なし
			break;
		}
	}
	
	//#695
	ANumber	lineHeightPlusMargin = GetLineHeightPlusMargin();
	
	{
		//ポインタ取得
		AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
		ANumber*	p_height = arrayptr_height.GetPtr();
		AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
		ANumber*	p_imageY = arrayptr_imageY.GetPtr();
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		if( collapse == false )
		{
			//Expandする場合
			
			//データ設定
			//mLineImageInfo_Collapsed.Set(foldingStartLineIndex,false);
			p_collapsed[foldingStartLineIndex] = false;
			//折りたたみ終了行までの行高さを復元する
			for( ; lineIndex < foldingEndLineIndex; lineIndex++ )
			{
				//高さ・ImageY設定
				ANumber	lineHeight = lineHeightPlusMargin;//#493一旦機能drop CalcLineHeight(lineIndex);
				//mLineImageInfo_Height.Set(lineIndex,lineHeight);
				//mLineImageInfo_ImageY.Set(lineIndex,imageY);
				p_height[lineIndex] = lineHeight;
				p_imageY[lineIndex] = imageY;
				imageY += lineHeight;
				
				//Expandする内部にCollapsedがあったらその部分は高さ0のままにする
				//if( mLineImageInfo_Collapsed.Get(lineIndex) == true )
				if( p_collapsed[lineIndex] == true )
				{
					//対応する折りたたみ終了行を取得
					AIndex	callapseEndLineIndex = SPI_FindFoldingEndLineIndex(lineIndex);
					
					//Collapseマークの次の行から、Collapseマークの行のインデントと同じインデント量の行まで、高さ0設定する
					if( lineIndex+1 < lineCount && callapseEndLineIndex > lineIndex )
					{
						lineIndex++;
						for( ; lineIndex < callapseEndLineIndex; lineIndex++ )
						{
							//高さ・ImageY設定
							//mLineImageInfo_Height.Set(lineIndex,0);
							//mLineImageInfo_ImageY.Set(lineIndex,imageY);
							p_height[lineIndex] = 0;
							p_imageY[lineIndex] = imageY;
						}
						lineIndex--;
						/*
						//Collapseマークの行のインデントと同じインデント量になった行の高さ設定
						if( lineIndex < lineCount )
						{
							ANumber	lineHeight = lineHeightPlusMargin;//#493一旦機能drop CalcLineHeight(lineIndex);
							//mLineImageInfo_Height.Set(lineIndex,lineHeight);
							//mLineImageInfo_ImageY.Set(lineIndex,imageY);
							p_height[lineIndex] = lineHeight;
							p_imageY[lineIndex] = imageY;
							imageY += lineHeight;
						}
						else
						{
							break;
						}*/
					}
					else
					{
						//折りたたみ終了行を正しく見つけられない場合は、
						//折りたたみ状態となっていることが異常なので、折りたたみ状態を解除する。
						p_collapsed[lineIndex] = false;
					}
				}
			}
		}
		else
		{
			//Collapseする場合
			
			//データ設定
			//mLineImageInfo_Collapsed.Set(foldingStartLineIndex,true);
			p_collapsed[foldingStartLineIndex] = true;
			//折りたたみ終了行までの行高さを0にする
			for( ; lineIndex < foldingEndLineIndex; lineIndex++ )
			{
				//高さ・ImageY設定
				//mLineImageInfo_Height.Set(lineIndex,0);
				//mLineImageInfo_ImageY.Set(lineIndex,imageY);
				p_height[lineIndex] = 0;
				p_imageY[lineIndex] = imageY;
			}
		}
		
		//以降の行のimageYをずらす
		ANumber	delta = imageY - mLineImageInfo_ImageY.Get(lineIndex);
		if( delta != 0 )
		{
			//ImageY設定
			for( ; lineIndex < mLineImageInfo_ImageY.GetItemCount(); lineIndex++ )
			{
				//mLineImageInfo_ImageY.Set(lineIndex,mLineImageInfo_ImageY.Get(lineIndex)+delta);
				p_imageY[lineIndex] += delta;
			}
		}
	}
	//ImageSize更新
	SPI_UpdateImageSize();
	
	if( inRedraw == true )
	{
		//キャレット・選択位置補正
		if( mLineImageInfo_Collapsed.Get(foldingStartLineIndex) == true )
		{
			//Collapseの場合
			ATextPoint	spt,ept;
			SPI_GetSelect(spt,ept);
			if( (spt.y > foldingStartLineIndex && spt.y < foldingEndLineIndex) ||
						(ept.y > foldingStartLineIndex && ept.y < foldingEndLineIndex) )
			{
				spt.x = 0;
				spt.y = foldingStartLineIndex;
				SetCaretTextPoint(spt);
				//AdjustScroll(spt);
			}
			else
			{
				if( IsCaretMode() == true )
				{
					//キャレットdraw位置更新のため
					UpdateCaretImagePoint();
				}
			}
		}
		else
		{
			//Expandの場合
			if( IsCaretMode() == true )
			{
				//キャレットdraw位置更新のため
				UpdateCaretImagePoint();
			}
		}
		
		//クリック行のスクロール位置を固定する
		NVI_Scroll(0,GetImageYByLineIndex(inLineIndex)-originalImageY,false);
		//image yに差分があれば、redraw
		//if( delta != 0 )
		{
			//textview, linenumber view, diff viewを表示更新
			SPI_RefreshTextView();
		}
	}
}

/**
折りたたみ開始かどうかを判断
*/
ABool	AView_Text::SPI_IsFoldingStart( const AFoldingLevel inFoldingLevel ) const
{
	ABool	result = false;
	if( (inFoldingLevel&kFoldingLevel_Header) != 0 || (inFoldingLevel&kFoldingLevel_BlockStart) != 0 ||
				(inFoldingLevel&kFoldingLevel_CommentStart) != 0 || (inFoldingLevel&kFoldingLevel_IfStart) != 0 )
	{
		result = true;
	}
	return result;
}

/**
折りたたみ終了かどうかを判断
*/
ABool	AView_Text::SPI_IsFoldingEnd( const AFoldingLevel inFoldingLevel ) const
{
	ABool	result = false;
	if( (inFoldingLevel&kFoldingLevel_BlockEnd) != 0 || (inFoldingLevel&kFoldingLevel_CommentEnd) != 0 ||
				(inFoldingLevel&kFoldingLevel_IfEnd) != 0 )
	{
		result = true;
	}
	return result;
}

/**
指定行（折りたたみ開始）に対する折りたたみ終了行を検索
*/
AIndex	AView_Text::SPI_FindFoldingEndLineIndex( const AIndex inStartLineIndex ) const
{
	//指定行の折りたたみレベル取得
	AFoldingLevel	startFoldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(inStartLineIndex);
	//折りたたみ開始でなければ何もせずにリターン
	if( SPI_IsFoldingStart(startFoldingLevel) == false )   return inStartLineIndex;
	
	//折りたたみ開始行のインデントレベルを取得
	AItemCount	indentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(inStartLineIndex);
	//折りたたみ開始行のアウトラインレベルを取得
	AIndex	startOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inStartLineIndex);
	//折りたたみ開始行のdirectiveレベルを取得
	AIndex	startDirectiveLevel = GetTextDocumentConst().SPI_GetDirectiveLevel(inStartLineIndex);
	
	//行数取得
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	//折りたたみ開始行の次の行から始めて、プラス方向に折りたたみ終了行を検索
	AIndex lineIndex = inStartLineIndex+1;
	for( ; lineIndex < lineCount; lineIndex++ )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//==================見出し==================
		if( (startFoldingLevel&kFoldingLevel_Header) != 0 )
		{
			//開始行の見出しアウトラインレベルと同じレベルまたは上位の見出しなら終了とする
			if( (foldingLevel&kFoldingLevel_Header) != 0 )
			{
				if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= startOutlineLevel)
				{
					return lineIndex;
				}
			}
			//次の行のインデントレベルが開始行のインデントレベルよりも小さくなったら終了とする
			if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
			{
				if( indentCount > GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex+1) )
				{
					return lineIndex;
				}
			}
			//
			if( lineIndex == lineCount-1 )
			{
				return lineIndex;
			}
		}
		//==================ブロック==================
		if( (startFoldingLevel&kFoldingLevel_BlockStart) != 0 )
		{
			//おりたたみ開始行と同じインデントレベル、かつ、blockendなら終了とする
			if( (foldingLevel&kFoldingLevel_BlockEnd) != 0 )
			{
				if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					if( indentCount == GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex+1) )
					{
						return lineIndex;
					}
				}
			}
		}
		//==================#if==================
		if( (startFoldingLevel&kFoldingLevel_IfStart) != 0 )
		{
			if( (foldingLevel&kFoldingLevel_IfEnd) != 0 )
			{
				//次の行のdirectiveレベルが開始行のdirectiveレベルと同じなら終了とする
				if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					if( startDirectiveLevel == GetTextDocumentConst().SPI_GetDirectiveLevel(lineIndex+1) )
					{
						return lineIndex;
					}
				}
			}
		}
		//==================コメント==================
		if( (startFoldingLevel&kFoldingLevel_CommentStart) != 0 )
		{
			if( (foldingLevel&kFoldingLevel_CommentEnd) != 0 )
			{
				return lineIndex;
			}
		}
	}
	return inStartLineIndex;
}

/**
指定行（折りたたみ終了）に対する折りたたみ開始行を検索
*/
AIndex	AView_Text::SPI_FindFoldingStartLineIndex( const AIndex inEndLineIndex ) const
{
	//指定行の折りたたみレベル取得
	AFoldingLevel	endFoldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(inEndLineIndex);
	//折りたたみ終了でなければ何もせずにリターン
	if( SPI_IsFoldingEnd(endFoldingLevel) == false )   return inEndLineIndex;
	
	//折りたたみ終了行のインデントレベルを取得
	AItemCount	indentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(inEndLineIndex);
	//折りたたみ終了行のアウトラインレベルを取得
	AIndex	endOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inEndLineIndex);
	//折りたたみ終了行のdirectiveレベルを取得
	AIndex	endDirectiveLevel = GetTextDocumentConst().SPI_GetDirectiveLevel(inEndLineIndex);
	
	//折りたたみ終了行の前の行から始めて、マイナス方向に折りたたみ開始行を検索
	AIndex lineIndex = inEndLineIndex-1;
	for( ; lineIndex > 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//==================見出し==================
		if( (endFoldingLevel&kFoldingLevel_Header) != 0 )
		{
			//開始行の見出しアウトラインレベルと同じレベルまたは上位の見出しなら終了とする
			if( (foldingLevel&kFoldingLevel_Header) != 0 )
			{
				if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= endOutlineLevel )
				{
					return lineIndex;
				}
			}
		}
		//==================ブロック==================
		if( (endFoldingLevel&kFoldingLevel_BlockEnd) != 0 )
		{
			//折りたたみ終了行と同じインデントレベル、かつ、block startなら終了とする
			if( (foldingLevel&kFoldingLevel_BlockStart) != 0 )
			{
				if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					if( indentCount == GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex+1) )
					{
						return lineIndex;
					}
				}
			}
		}
		//==================#endif==================
		if( (endFoldingLevel&kFoldingLevel_IfEnd) != 0 )
		{
			if( (foldingLevel&kFoldingLevel_IfStart) != 0 )
			{
				//次の行のdirectiveレベルが終了行のdirectiveレベルと同じなら終了とする
				if( lineIndex+1 < GetTextDocumentConst().SPI_GetLineCount() )
				{
					if( endDirectiveLevel == GetTextDocumentConst().SPI_GetDirectiveLevel(lineIndex+1) )
					{
						return lineIndex;
					}
				}
			}
		}
		//==================コメント==================
		if( (endFoldingLevel&kFoldingLevel_CommentEnd) != 0 )
		{
			if( (foldingLevel&kFoldingLevel_CommentStart) != 0 )
			{
				return lineIndex;
			}
		}
	}
	return inEndLineIndex;
}

/**
折りたたみ状態取得
*/
ABool	AView_Text::SPI_GetFoldingCollapsed( const AIndex inLineIndex ) const
{
	return mLineImageInfo_Collapsed.Get(inLineIndex);
}

/**
*/
void	AView_Text::CollapseAllHeaders()
{
	{
		//ポインタ取得
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			//ヘッダ行なら、collapse
			if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_Header) != 0 )
			{
				p_collapsed[lineIndex] = true;
			}
		}
	}
	//全ての行ImageY,高さを、collapsedパラメータに従って更新
	UpdateLineImageInfoAllByCollapsedParameter();
}

/**
SCM変更を含まない関数をcollapse
*/
void	AView_Text::CollapseAllHeadersWithMNoChange()
{
	{
		//ポインタ取得
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			//ヘッダ行、かつ、SCM変更を含んでいなければ、collapse
			if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_Header) != 0 )
			{
				AIndex	endLineIndex = SPI_FindFoldingEndLineIndex(lineIndex);
				if( GetTextDocumentConst().SPI_DiffExistInRange(lineIndex,endLineIndex) == false )
				{
					p_collapsed[lineIndex] = true;
				}
			}
		}
	}
	//全ての行ImageY,高さを、collapsedパラメータに従って更新
	UpdateLineImageInfoAllByCollapsedParameter();
}

/**
全てexpand
*/
void	AView_Text::ExpandAllFoldings()
{
	ANumber	lineHeight = GetLineHeightPlusMargin();
	ANumber	imageY = 0;
	//ポインタ取得
	AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
	ANumber*	p_height = arrayptr_height.GetPtr();
	AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
	ANumber*	p_imageY = arrayptr_imageY.GetPtr();
	AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
	ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
	//
	AItemCount	itemCount = mLineImageInfo_ImageY.GetItemCount();
	for( AIndex lineIndex = 0; lineIndex < itemCount; lineIndex++ )
	{
		p_height[lineIndex] = lineHeight;
		p_imageY[lineIndex] = imageY;
		p_collapsed[lineIndex] = false;
		//
		imageY += lineHeight;
	}
	//ImageSize更新
	SPI_UpdateImageSize();
	//textview, linenumber view, diff viewを表示更新
	SPI_RefreshTextView();
}

/**
指定行を内包するfolding開始位置を取得
*/
AIndex	AView_Text::FindCurrentFoldingStartLineIndex( const AIndex inCurrentLineIndex ) const
{
	//指定行がfolding開始位置なら指定行を返す
	AFoldingLevel	currentLineFoldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(inCurrentLineIndex);
	if( (currentLineFoldingLevel&kFoldingLevel_Header) != 0 ||  (currentLineFoldingLevel&kFoldingLevel_BlockStart) != 0 )
	{
		return inCurrentLineIndex;
	}
	
	//現在行のインデントレベルを取得
	AItemCount	currentLineIndentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(inCurrentLineIndex);
	//現在行のアウトラインレベルを取得
	AIndex	currentLineOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inCurrentLineIndex);
	
	//前の行以前で、最初にレベルが同じまたは小さくなるfolding開始位置を取得（ただしブロックの場合は、同じインデントレベルは除外）
	for( AIndex lineIndex = inCurrentLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//見出し
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= currentLineOutlineLevel )
			{
				return lineIndex;
			}
		}
		//ブロック
		else if( (foldingLevel&kFoldingLevel_BlockStart) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex) < currentLineIndentCount )
			{
				return lineIndex;
			}
		}
	}
	return kIndex_Invalid;
}

/**
指定行を含むfoldingをexpand/collapse
@note inLineIndexは折りたたみ開始行・終了行以外でも良い
*/
void	AView_Text::SPI_ExpandCollapseAtCurrentLine( const AIndex inLineIndex, const ABool inCollapse )
{
	//指定行を内包するfolding開始位置を取得
	AIndex	foldingStartLineIndex = FindCurrentFoldingStartLineIndex(inLineIndex);
	if( foldingStartLineIndex == kIndex_Invalid )
	{
		//folding開始行なし
		return;
	}
	//
	if( inCollapse == true )
	{
		SPI_ExpandCollapse(foldingStartLineIndex,kExpandCollapseType_Collapse,true);
	}
	else
	{
		SPI_ExpandCollapse(foldingStartLineIndex,kExpandCollapseType_Expand,true);
	}
}

/**
指定行のfolding levelと同じfoldingを全てexpand/collapse
@note inLineIndexは折りたたみ開始行・終了行以外でも良い
*/
void	AView_Text::SPI_ExpandCollapseAllAtCurrentLevel( const AIndex inLineIndex, const ABool inCollapse, const ABool inByClickFoldingMark )
{
	//指定行を内包するfolding開始位置を取得
	AIndex	foldingStartLineIndex = FindCurrentFoldingStartLineIndex(inLineIndex);
	if( foldingStartLineIndex == kIndex_Invalid )
	{
		//folding開始行なし
		return;
	}
	if( inByClickFoldingMark == true )
	{
		//行番号エリアの折りたたみマーククリックによる場合は、コメントも対象とする
		if( (GetTextDocumentConst().SPI_GetFoldingLevel(inLineIndex)&kFoldingLevel_CommentStart) != 0 )
		{
			foldingStartLineIndex = inLineIndex;
		}
		//
		if( foldingStartLineIndex != inLineIndex )
		{
			//行番号エリアの折りたたみマーククリックによる場合は、クリック行が開始行にならない限り、通常のexpand/collapseを行う。
			SPI_ExpandCollapse(inLineIndex,kExpandCollapseType_Switch,true);
			return;
		}
	}
	//
	AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(foldingStartLineIndex);
	if( (foldingLevel&kFoldingLevel_Header) != 0 )
	{
		//==================同じレベルの見出し折りたたみ==================
		
		//
		AItemCount	outlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(foldingStartLineIndex);
		//ポインタ取得
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_Header) != 0 )
			{
				if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) == outlineLevel )
				{
					p_collapsed[lineIndex] = inCollapse;
				}
			}
		}
		//全ての行ImageY,高さを、collapsedパラメータに従って更新
		UpdateLineImageInfoAllByCollapsedParameter();
	}
	else if( (foldingLevel&kFoldingLevel_BlockStart) != 0 )
	{
		//==================同じインデントレベルのブロック折りたたみ==================
		
		//
		AItemCount	indentCount = GetTextDocumentConst().SPI_GetTextInfoIndentCount(foldingStartLineIndex);
		//ポインタ取得
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		/*ローカル範囲内にしようと思ったが、全範囲に変更（そちらのほうが使い勝手が良い。特にHTMLの場合）
		AIndex	localStartLineIndex = 0, localEndLineIndex = 0;
		GetTextDocumentConst().SPI_GetLocalRangeForLineIndex(foldingStartLineIndex,localStartLineIndex,localEndLineIndex);
		//
		if( localStartLineIndex != kIndex_Invalid && localEndLineIndex != kIndex_Invalid )
		*/
		{
			//for( AIndex lineIndex = localStartLineIndex; lineIndex < localEndLineIndex; lineIndex++ )
			AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
			for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
			{
				if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_BlockStart) != 0 )
				{
					if( GetTextDocumentConst().SPI_GetTextInfoIndentCount(lineIndex) == indentCount )
					{
						p_collapsed[lineIndex] = inCollapse;
					}
				}
			}
			//全ての行ImageY,高さを、collapsedパラメータに従って更新
			UpdateLineImageInfoAllByCollapsedParameter();
		}
	}
	else if( (foldingLevel&kFoldingLevel_CommentStart) != 0 )
	{
		//==================ローカル範囲内のコメントを全て折りたたみ==================
		
		//ポインタ取得
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		//
		AIndex	localStartLineIndex = 0, localEndLineIndex = 0;
		GetTextDocumentConst().SPI_GetLocalRangeForLineIndex(foldingStartLineIndex,localStartLineIndex,localEndLineIndex);
		//
		if( localStartLineIndex != kIndex_Invalid && localEndLineIndex != kIndex_Invalid )
		{
			//
			for( AIndex lineIndex = localStartLineIndex; lineIndex < localEndLineIndex; lineIndex++ )
			{
				if( (GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)&kFoldingLevel_CommentStart) != 0 )
				{
					p_collapsed[lineIndex] = inCollapse;
				}
			}
			//全ての行ImageY,高さを、collapsedパラメータに従って更新
			UpdateLineImageInfoAllByCollapsedParameter();
		}
	}
}

/**
collapseされた行のリストを取得
*/
void	AView_Text::SPI_GetFoldingCollapsedLines( ANumberArray& outCollapsedLines )
{
	//ポインタ取得
	AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
	ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
	//
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		if( p_collapsed[lineIndex] == true )
		{
			outCollapsedLines.Add(lineIndex);
		}
	}
}

/**
全ての行ImageY,高さを、collapsedパラメータに従って更新
*/
void	AView_Text::UpdateLineImageInfoAllByCollapsedParameter()
{
	//折りたたみ適用前の開始行indexを取得（折りたたみ後、開始行が変化しないようにスクロール実行するため）
	AImageRect	imageFrameRect = {0};
	NVM_GetImageViewRect(imageFrameRect);
	AIndex	oldStartLineIndex = GetLineIndexByImageY(imageFrameRect.top);
	
	//
	ANumber	lineHeight = GetLineHeightPlusMargin();
	ANumber	imageY = 0;
	//ポインタ取得
	AStArrayPtr<ANumber>	arrayptr_height(mLineImageInfo_Height,0,mLineImageInfo_Height.GetItemCount());
	ANumber*	p_height = arrayptr_height.GetPtr();
	AStArrayPtr<ANumber>	arrayptr_imageY(mLineImageInfo_ImageY,0,mLineImageInfo_ImageY.GetItemCount());
	ANumber*	p_imageY = arrayptr_imageY.GetPtr();
	AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
	ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
	//
	AItemCount	itemCount = mLineImageInfo_ImageY.GetItemCount();
	for( AIndex lineIndex = 0; lineIndex < itemCount; lineIndex++ )
	{
		//高さ・ImageY設定
		p_height[lineIndex] = lineHeight;
		p_imageY[lineIndex] = imageY;
		imageY += lineHeight;
		
		//
		if( p_collapsed[lineIndex] == true )
		{
			//対応する折りたたみ終了行を取得
			AIndex	callapseEndLineIndex = SPI_FindFoldingEndLineIndex(lineIndex);
			
			//Collapseマークの次の行から、Collapseマークの行のインデントと同じインデント量の行まで、高さ0設定する
			if( lineIndex+1 < itemCount && callapseEndLineIndex > lineIndex )
			{
				lineIndex++;
				for( ; lineIndex < callapseEndLineIndex; lineIndex++ )
				{
					//高さ・ImageY設定
					p_height[lineIndex] = 0;
					p_imageY[lineIndex] = imageY;
				}
				lineIndex--;
				/*
				//Collapseマークの行のインデントと同じインデント量になった行の高さ設定
				if( lineIndex < itemCount )
				{
					p_height[lineIndex] = lineHeight;
					p_imageY[lineIndex] = imageY;
					imageY += lineHeight;
				}
				else
				{
					break;
				}
				*/
			}
			else
			{
				//折りたたみ終了行を正しく見つけられない場合は、
				//折りたたみ状態となっていることが異常なので、折りたたみ状態を解除する。
				p_collapsed[lineIndex] = false;
			}
		}
	}
	//ImageSize更新
	SPI_UpdateImageSize();
	//折りたたみ前の開始行位置へスクロール
	AImagePoint	origin = {0};
	origin.x = imageFrameRect.left;
	origin.y = GetImageYByLineIndex(oldStartLineIndex);
	NVI_ScrollTo(origin,false);
	//キャレットimage point更新
	UpdateCaretImagePoint();
	//textview, linenumber view, diff viewを表示更新
	SPI_RefreshTextView();
}

/**
行のリストに従って行をcollapse
*/
void	AView_Text::SPI_CollapseLines( const ANumberArray& inCollapsedLines, const ABool inApplyDefaultFoldingSetting,
									 const ABool inExpandOtherLines )
{
	{
		//ポインタ取得
		AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
		ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
		
		//==================inExpandOtherLinesがtrueならまず全ての行をexpand==================
		if( inExpandOtherLines == true )
		{
			AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
			for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
			{
				p_collapsed[lineIndex] = false;
			}
		}
		
		//==================リストに指定された行をcollapse==================
		//
		AItemCount	itemCount = inCollapsedLines.GetItemCount();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			AIndex	lineIndex = inCollapsedLines.Get(i);
			if( lineIndex >= 0 && lineIndex < GetTextDocumentConst().SPI_GetLineCount() )
			{
				p_collapsed[lineIndex] = true;
			}
		}
	}
	
	//==================デフォルト折りたたみを適用==================
	//
	if( inApplyDefaultFoldingSetting == true && 
				GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Bool(AModePrefDB::kUseDefaultFoldingSetting) == true )
	{
		SetFoldingToDefaultCore(false);
	}
	
	//全ての行ImageY,高さを、collapsedパラメータに従って更新
	UpdateLineImageInfoAllByCollapsedParameter();
	
	//==================もしキャレット位置が折りたたまれてしまったら、キャレット位置のみは折りたたみ解除する==================
	if( SPI_IsLineCollapsed(mCaretTextPoint.y) == true )
	{
		SPI_RevealCollapsedLine(mCaretTextPoint.y);
	}
}

/**
折りたたみをデフォルトにする
*/
void	AView_Text::SPI_SetFoldingToDefault()
{
	//デフォルト折りたたみ適用
	SetFoldingToDefaultCore(true);
	
	//全ての行ImageY,高さを、collapsedパラメータに従って更新
	UpdateLineImageInfoAllByCollapsedParameter();
}

/**
モード設定によるデフォルト折りたたみ適用
*/
void	AView_Text::SetFoldingToDefaultCore( const ABool inExpandForNotMatched )
{
	//ポインタ取得
	AStArrayPtr<ABool>	arrayptr_collapsed(mLineImageInfo_Collapsed,0,mLineImageInfo_Collapsed.GetItemCount());
	ABool*	p_collapsed = arrayptr_collapsed.GetPtr();
	
	//デフォルトで折りたたむ行の開始テキスト設定を取得
	AText	settingText;
	GetApp().SPI_GetModePrefDB(GetTextDocument().SPI_GetModeIndex()).GetData_Text(AModePrefDB::kDefaultFoldingLineText,settingText);
	ATextArray	settingTextArray;
	settingTextArray.ExplodeFromText(settingText,kUChar_LineEnd);
	AItemCount	settingTextArrayItemCount = settingTextArray.GetItemCount();
	if( settingTextArrayItemCount > 0 )
	{
		//スペース・タブを取り除く
		for( AIndex i = 0; i < settingTextArrayItemCount; i++ )
		{
			AText	t;
			settingTextArray.Get(i,t);
			t.RemoveSpaceTab();
			settingTextArray.Set(i,t);
		}
		//設定データに一致する行を自動collapseする。
		AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
		for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
		{
			if( inExpandForNotMatched == true )
			{
				//一致しない行は折りたたみ解除
				p_collapsed[lineIndex] = false;
			}
			//
			if( SPI_IsFoldingStart(GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex)) == true )
			{
				AText	linetext;
				GetTextDocumentConst().SPI_GetLineText(lineIndex,linetext);
				linetext.RemoveSpaceTab();
				//設定データ項目毎のループ
				for( AIndex i = 0; i < settingTextArrayItemCount; i++ )
				{
					//設定テキストの長さで、設定テキストと行テキストを比較する（行テキストの長さの方が短ければ、比較しない。out of rangeにならないようにするため、かつ、比較は絶対に一致しないので。）
					AItemCount	l = settingTextArray.GetTextLen(i);
					if( l > 0 && l <= linetext.GetItemCount() )
					{
						if( settingTextArray.Compare(i,linetext,0,l) == true )
						{
							//一致したら折りたたむ
							p_collapsed[lineIndex] = true;
							break;
						}
					}
				}
			}
		}
	}
}

/**
指定行が折りたたまれている場合に、指定行が表示されるように折りたたみ解除する（検索結果箇所を表示する場合など）
*/
void	AView_Text::SPI_RevealCollapsedLine( const AIndex inLineIndex )
{
	//collapseされた行でなければ何もしない
	if( mLineImageInfo_Height.Get(inLineIndex) > 0 )
	{
		return;
	}
	//行をさかのぼって、collpaseフラグONの行を検索する。
	//★ロジック改善検討 revealしたい行の前に、独立したブロックがあってそれがcollapseされており、かつ、さらに全体がcollapseの場合
	for( AIndex lineIndex = inLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		if( mLineImageInfo_Collapsed.Get(lineIndex) == true )
		{
			if( mLineImageInfo_Height.Get(lineIndex) == 0 )
			{
				//collapseフラグON、かつ、その行もcollapsedなら、その行のcollpaseフラグをOFFにして、さらにさかのぼる
				mLineImageInfo_Collapsed.Set(lineIndex,false);
			}
			else
			{
				//collapseフラグON、かつ、その行はcollapsedでなければ、その行からexpandを実行する。
				SPI_ExpandCollapse(lineIndex,kExpandCollapseType_Expand,true);
				break;
			}
		}
	}
}

/**
行が折りたたまれているかどうかを取得
*/
ABool	AView_Text::SPI_IsLineCollapsed( const AIndex inLineIndex ) const
{
	return (mLineImageInfo_Height.Get(inLineIndex)==0);
}

/**
親見出しの行を取得
*/
AIndex	AView_Text::FindParentHeader( const AIndex inLineIndex ) const
{
	//現在行のアウトラインレベルを取得
	AIndex	currentLineOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inLineIndex);
	
	for( AIndex lineIndex = inLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//見出し
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) < currentLineOutlineLevel )
			{
				return lineIndex;
			}
		}
	}
	return kIndex_Invalid;
}

/**
次の見出し行を取得
*/
AIndex	AView_Text::FindNextHeader( const AIndex inLineIndex ) const
{
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	for( AIndex lineIndex = inLineIndex+1; lineIndex < lineCount; lineIndex++ )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//見出し
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			return lineIndex;
		}
	}
	return kIndex_Invalid;
}

/**
前の見出し行を取得
*/
AIndex	AView_Text::FindPrevHeader( const AIndex inLineIndex ) const
{
	for( AIndex lineIndex = inLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//見出し
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			return lineIndex;
		}
	}
	return kIndex_Invalid;
}

/**
同じレベルの次の見出しを取得
*/
AIndex	AView_Text::FindSameOrHigherLevelNextHeader( const AIndex inLineIndex ) const
{
	//現在行のアウトラインレベルを取得
	AIndex	currentLineOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inLineIndex);
	
	AItemCount	lineCount = GetTextDocumentConst().SPI_GetLineCount();
	for( AIndex lineIndex = inLineIndex+1; lineIndex < lineCount; lineIndex++ )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//見出し
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= currentLineOutlineLevel )
			{
				return lineIndex;
			}
		}
	}
	return kIndex_Invalid;
}

/**
同じレベルの前の見出しを取得
*/
AIndex	AView_Text::FindSameOrHigherLevelPrevHeader( const AIndex inLineIndex ) const
{
	//現在行のアウトラインレベルを取得
	AIndex	currentLineOutlineLevel = GetTextDocumentConst().SPI_GetOutlineLevel(inLineIndex);
	
	for( AIndex lineIndex = inLineIndex-1; lineIndex >= 0; lineIndex-- )
	{
		AFoldingLevel	foldingLevel = GetTextDocumentConst().SPI_GetFoldingLevel(lineIndex);
		//見出し
		if( (foldingLevel&kFoldingLevel_Header) != 0 )
		{
			if( GetTextDocumentConst().SPI_GetOutlineLevel(lineIndex) <= currentLineOutlineLevel )
			{
				return lineIndex;
			}
		}
	}
	return kIndex_Invalid;
}

#pragma mark ===========================

#pragma mark ---図形モード
//#478

/**
*/
void	AView_Text::FigMode_DrawLine( const AImagePoint& inStartImagePoint, const AImagePoint& inEndImagePoint )
{
	AText	space(" ");
	ANumber	spacewidth = static_cast<ANumber>(NVMC_GetDrawTextWidth(space));
	FigMode_DrawLine(inStartImagePoint.x/spacewidth,inEndImagePoint.x/spacewidth,GetLineIndexByImageY(inStartImagePoint.y));
}

/**
*/
void	AView_Text::FigMode_DrawLine( const ANumber inStartTextX, const ANumber inEndTextX, const ANumber inTextY )
{
	CTextDrawData	textDrawData(GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kDisplayEachCanonicalDecompChar));
	AAnalyzeDrawData	analyzeDrawData;
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextDrawDataWithStyle(inTextY,textDrawData,false,caretPoint,selectPoint,analyzeDrawData,GetObjectID());
	
	//
	AIndex	lineLength = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(inTextY);
	//
	ANumber	spaceInsert = 0;
	ANumber	startOffset = inStartTextX;
	ANumber	deleteEnd = inEndTextX;
	if( inEndTextX <= textDrawData.UTF16DrawText.GetItemCount() )
	{
		//
		startOffset = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inStartTextX);
		deleteEnd = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inEndTextX);
	}
	else if( inStartTextX <= textDrawData.UTF16DrawText.GetItemCount() )
	{
		//
		startOffset = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(inStartTextX);
		deleteEnd = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.UTF16DrawText.GetItemCount()-1);
	}
	else
	{
		//
		spaceInsert = inStartTextX-lineLength;
		//
		startOffset = textDrawData.UTF16DrawTextArray_OriginalTextIndex.Get(textDrawData.UTF16DrawText.GetItemCount()-1) + spaceInsert;
		deleteEnd = startOffset;
	}
	
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(undoTag_Typing);
	//削除
	ATextPoint	deletespt;
	deletespt.x = startOffset;
	deletespt.y = inTextY;
	ATextPoint	deleteept;
	deleteept.x = deleteEnd;
	deleteept.y = inTextY;
	if( startOffset < deleteEnd )
	{
		GetTextDocument().SPI_DeleteText(deletespt,deleteept);
	}
	//
	ATextPoint	pt = deletespt;
	//スペース挿入
	AText	spacetext;
	for( AIndex i = 0; i < spaceInsert; i++ )
	{
		spacetext.Add(kUChar_Space);
	}
	GetTextDocument().SPI_InsertText(pt,spacetext);
	pt.x += spacetext.GetItemCount();
	//線挿入
	AText	linetext;
	for( AIndex i = inStartTextX; i < inEndTextX; i++ )
	{
		linetext.Add('-');
	}
	GetTextDocument().SPI_InsertText(pt,linetext);
}

#pragma mark ===========================

#pragma mark ---Notificationウインドウ
//#725

/**
Notificationポップアップウインドウを表示／非表示
*/
void	AView_Text::SPI_ShowNotificationPopupWindow( const ABool inShow )
{
	if( inShow == true )
	{
		//notification ポップアップウインドウのbounds更新
		UpdateNotificationPopupWindowBounds(true);
		
		//表示
		SPI_GetPopupNotificationWindow().NVI_Show(false);
	}
	else
	{
		//非表示
		SPI_GetPopupNotificationWindow().NVI_Hide();
	}
}

/**
Notificationウインドウ取得
*/
AWindow_Notification&	AView_Text::SPI_GetPopupNotificationWindow()
{
	//未生成なら生成
	if( mPopupNotificationWindowID == kObjectID_Invalid )
	{
		AWindowDefaultFactory<AWindow_Notification>	factory;
		mPopupNotificationWindowID = GetApp().NVI_CreateWindow(factory);
		GetApp().NVI_GetWindowByID(mPopupNotificationWindowID).NVI_ChangeToOverlay(NVM_GetWindow().GetObjectID(),true);
		SPI_GetPopupNotificationWindow().NVI_Create(kObjectID_Invalid);
	}
	//Notificationウインドウ取得
	MACRO_RETURN_WINDOW_DYNAMIC_CAST(AWindow_Notification,kWindowType_Notification,mPopupNotificationWindowID);
}

/**
notificationウインドウのbounds更新
*/
void	AView_Text::UpdateNotificationPopupWindowBounds( const ABool inAlwaysUpdate )
{
	//text view全体のglobal rect取得
	AGlobalRect	viewGlobalRect = {0};
	NVI_GetGlobalViewRect(viewGlobalRect);
	
	if( inAlwaysUpdate == false )
	{
		//notification表示中でなければ、何もしない
		if( mPopupNotificationWindowID == kObjectID_Invalid )
		{
			return;
		}
		if( SPI_GetPopupNotificationWindow().NVI_IsWindowVisible() == false )
		{
			return;
		}
	}
	
	//==================Notificationウインドウ配置==================
	
	const ANumber	kNotificationWindowRightMargin = 2;
	const ANumber	kNotificationWindowTopMargin = 3;
	
	ARect	windowRect = {0};
	if( NVI_GetVerticalMode() == false )
	{
		windowRect.right	= viewGlobalRect.right - kNotificationWindowRightMargin;
		windowRect.left		= windowRect.right - 320;
		windowRect.top		= viewGlobalRect.top + kNotificationWindowTopMargin;
		windowRect.bottom	= windowRect.top +
							SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetWindowHeight();
	}
	else
	{
		//縦書きモード時は行番号エリアの右上に表示する #1387
		GetLineNumberView().NVI_GetGlobalViewRect(viewGlobalRect);
		
		windowRect.right	= viewGlobalRect.right - kNotificationWindowRightMargin;
		windowRect.left		= windowRect.right - 320;
		windowRect.top		= viewGlobalRect.top + kNotificationWindowTopMargin;
		windowRect.bottom	= windowRect.top +
							SPI_GetPopupNotificationWindow().SPI_GetNotificationView().SPI_GetWindowHeight();
	}
	
	SPI_GetPopupNotificationWindow().NVI_SetWindowBounds(windowRect);
}

//#1117
/**
汎用ポップアップ通知表示
*/
void	AView_Text::SPI_ShowGeneralNotificationPopupWindow( const AText& inTitle, const AText& inMessage )
{
	if( mTextWindowID != kObjectID_Invalid )
	{
		SPI_GetPopupNotificationWindow().SPI_GetNotificationView().
				SPI_SetNotification_General(inTitle,inMessage,GetEmptyText());
		SPI_ShowNotificationPopupWindow(true);
	}
}
