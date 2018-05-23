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

AView_EditBox

*/

#include "stdafx.h"

#include "../Frame.h"

AText			AView_EditBox::sDefaultFontName;//AApplication::AApplication()にてSPI_SetDefaultFont()経由で設定される
AFloatNumber	AView_EditBox::sDefaultFontSize = 12.0;

#pragma mark ===========================
#pragma mark [クラス]AView_EditBox
#pragma mark ===========================
//インデックスウインドウのメインView

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AView_EditBox::AView_EditBox( const AWindowID inWindowID, const AControlID inID ) //#182
: AView(inWindowID,inID), mCaretMode(true), mCaretBlinkStateVisible(false), mFontSize(12.0), mColor(kColor_Black), 
		mOldArrowImageX(0), mListViewID(kControlID_Invalid), mInlineInputNotFixedTextLen(0),
		mMouseTrackingMode(kMouseTrackingMode_None)//#688 , mMouseTrackByLoop(true)
		,mBackgroundColor(kColor_White),mFrameColor(kColor_Gray),mColorDeactive(kColor_Gray70Percent),mFrameColorDeactive(kColor_Gray50Percent)
		/*#135 ,mFrameViewID(kObjectID_Invalid)*/, mEnableEdit(true)
		,mAdjustScrollXOffsetRight(30), mAdjustScrollXOffsetLeft(50)//#261
		,mDragging(false), mDragDrawn(false), mDragWordDragDrop(false)//#135
		,mContextMenuID(kContextMenuID_EditBox)//#135
		,mEnableBackgroundColorForEmptyState(false), mBackgroundActiveColorForEmptyState(kColor_White), mBackgroundDeactiveColorForEmptyState(kColor_White)//#798
,mTransparency(1.0), mEditBoxType(kEditBoxType_Normal)
, mHoveringSearchCancelButton(false), mPressingSearchCancelButton(false), mTrackingPressSearchCancelButton(false)
,mDrawLineEnd(true), mCenterizeWhen1Line(false)//#894
,mReadOnly(false)//#688
,mSetSelect00AfterSetText(false)
,mTempCaretHideCount(0)
{
	NVMC_SetOffscreenMode(true);//win
	//#92 初期化はNVIDO_Init()へ移動
}

/**
初期化（View作成直後に必ずコールされる）
*/
void	AView_EditBox::NVIDO_Init()
{
	//
	AEditBoxDocumentFactory	factory(this);
	mDocumentID = AApplication::GetApplication().NVI_CreateDocument(factory);
	GetTextDocument().NVI_RegisterView(GetObjectID());//#379 基本ルールとしてDocumentにはViewを登録。登録解除でDocument削除。
	
	//改行コードテキスト取得
	mLineEndDisplayText.SetLocalizedText("ReturnCodeText");
	mLineEndDisplayText_UTF16.SetText(mLineEndDisplayText);
	mLineEndDisplayText_UTF16.ConvertFromUTF8ToUTF16();
	
	//
	//win AFontWrapper::GetDefaultFontName(mFontName);
	ATextPoint	pt = {0,0};
	SetCaretTextPoint(pt);
	NVI_SetDefaultCursor(kCursorType_IBeam);
	mInlineInputHiliteColorArray.Add(kColor_Black);
	mInlineInputHiliteColorArray.Add(kColor_LightGray);
	mInlineInputHiliteColorArray.Add(kColor_Gray);
	mInlineInputHiliteColorArray.Add(kColor_Black);
	mInlineInputHiliteColorArray.Add(kColor_Black);
	mInlineInputHiliteColorArray.Add(kColor_Black);
	
	//#688
	//フォーカス可
	NVMC_SetAcceptFocus(true);
	
	/*#688
#if IMPLEMENTATION_FOR_WINDOWS
	mMouseTrackByLoop = false;
#endif
	*/
	//#135 CreateFrameView();
	//#137 デフォルトTextDrawProperty設定
	/*win
	AText	defaultfontname;
	AFontWrapper::GetDefaultFontName(defaultfontname);
	*/
	//win SPI_SetTextDrawProperty(windefaultfontname,14.0,kColor_Black,kColor_Gray70Percent);//#182 kColor_Gray20Percent,kColor_Gray20Percent);
	SPI_SetTextDrawProperty(sDefaultFontName,sDefaultFontSize,kColor_Black,kColor_Gray70Percent);//win
	//#135
	NVI_SetAutomaticSelectBySwitchFocus(true);
	//#137 デフォルトではReturn/TabはEditBoxでは処理しない
	NVI_SetCatchReturn(false);
	NVI_SetCatchTab(false);
	//#135
	SPI_UpdateImageSize();
	//Drag&Drop対応 #135 
	AArray<AScrapType>	scrapTypeArray;
	scrapTypeArray.Add(kScrapType_UnicodeText);
	NVMC_EnableDrop(scrapTypeArray);
	//#725
	NVMC_EnableMouseLeaveEvent();
}

//デストラクタ
AView_EditBox::~AView_EditBox()
{
	/*#92 NVIDO_DoDeleted()へ移動
	//#138
	DeleteFrameView();
	//
	if( mListViewID != kControlID_Invalid )
	{
		AText	text;
		GetTextDocumentConst().SPI_GetText(text);
		NVM_GetWindow().NVI_GetListView(mListViewID).SPI_DoEditBoxDeleted(text);
	}
	AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
	*/
}

//#92
/**
削除時処理（削除時に必ずコールされる）
デストラクタはGarbageCollection時にコールされるので、基本的にはこちらで削除処理を行うこと
*/
void	AView_EditBox::NVIDO_DoDeleted()
{
	//#138
	//#135 DeleteFrameView();
	//
	if( mListViewID != kControlID_Invalid )
	{
		AText	text;
		GetTextDocumentConst().SPI_GetText(text);
		NVM_GetWindow().NVI_GetListView(mListViewID).SPI_DoEditBoxDeleted(text);
	}
	//#379 AApplication::GetApplication().NVI_DeleteDocument(mDocumentID);
	GetTextDocument().NVI_UnregisterView(GetObjectID());//#379 基本ルールとしてDocumentにはViewを登録。登録解除でDocument削除。
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

//
ADocument_EditBox&	AView_EditBox::GetTextDocument()
{
	MACRO_RETURN_DOCUMENT_DYNAMIC_CAST(ADocument_EditBox,kDocumentType_EditBox,mDocumentID);
	/*#199
	if( AApplication::GetApplication().NVI_GetDocumentTypeByID(mDocumentID) != kDocumentType_EditBox )
	{
		_ACThrow("document type different",this);
	}
	return dynamic_cast<ADocument_EditBox&>(AApplication::GetApplication().NVI_GetDocumentByID(mDocumentID));
	*/
}
//
const ADocument_EditBox&	AView_EditBox::GetTextDocumentConst() const
{
	MACRO_RETURN_CONSTDOCUMENT_DYNAMIC_CAST(ADocument_EditBox,kDocumentType_EditBox,mDocumentID);
	/*#199
	if( AApplication::GetApplication().NVI_GetDocumentTypeByID(mDocumentID) != kDocumentType_EditBox )
	{
		_ACThrow("document type different",this);
	}
	return dynamic_cast<const ADocument_EditBox&>(AApplication::GetApplication().NVI_GetDocumentConstByID(mDocumentID));
	*/
}

/*#135
//#199
AView_Frame&	AView_EditBox::SPI_GetFrameView()
{
	MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(AView_Frame,kViewType_Frame,mFrameViewID);
}
*/

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//メニュー選択時処理
ABool	AView_EditBox::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	ABool	result = true;
	ABool	touch = false;//#135
	switch(inMenuItemID)
	{
		//Undo
	  case kMenuItemID_Undo:
		{
			UndoRedo(true);
			touch = true;//#135
			break;
		}
		//Redo
	  case kMenuItemID_Redo:
		{
			UndoRedo(false);
			touch = true;//#135
			break;
		}
		//コピー
	  case kMenuItemID_Copy:
	  //case kMenuItemID_CM_Copy:
		{
			Copy(false);
			break;
		}
		//カット
	  case kMenuItemID_Cut:
	  //case kMenuItemID_CM_Cut:
		{
			Cut(false);
			touch = true;//#135
			break;
		}
		//ペースト
	  case kMenuItemID_Paste:
	  //case kMenuItemID_CM_Paste:
		{
			Paste();
			touch = true;//#135
			break;
		}
		//削除
	  case kMenuItemID_Clear:
	  //case kMenuItemID_CM_Clear:
		{
			DeleteSelect(kEditBoxUndoTag_DELKey);
			touch = true;//#135
			break;
		}
		//全てを選択
	  case kMenuItemID_SelectAll:
	  //case kMenuItemID_CM_SelectAll:
		{
			NVI_SetSelectAll();
			break;
		}
		//バックスラッシュ入力 #137
	  case kMenuItemID_InputBackslashYen:
		{
			if( NVI_GetInputBackslashByYenKeyMode() == true )
			{
				AText	yen;
				yen.SetLocalizedText("Text_Yen");
				TextInput(kEditBoxUndoTag_Typing,yen);
			}
			else
			{
				AText	backslash;
				backslash.Add(kUChar_Backslash);
				TextInput(kEditBoxUndoTag_Typing,backslash);
			}
			touch = true;//#135
			break;
		}
	  default:
		{
			result = false;
			break;
		}
	}
	//#135
	if( touch == true )
	{
		NVM_GetWindow().EVT_TextInputted(NVI_GetControlID());
	}
	//背景色更新
	UpdateBackgroundColor();
	//イメージサイズ更新
	SPI_UpdateImageSize();
	return result;
}

//
void	AView_EditBox::EVTDO_UpdateMenu()
{
	ABool	b;
	AText	text;
	
	//Undo
	b = GetTextDocument().SPI_GetUndoText(text);
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Undo,b);
	AApplication::GetApplication().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_Undo,text);
	//Redo
	b = GetTextDocument().SPI_GetRedoText(text);
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Redo,b);
	AApplication::GetApplication().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_Redo,text);
	//コピー
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Copy,(IsCaretMode()==false));
	//カット
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Cut,((IsCaretMode()==false)&&(GetTextDocument().NVI_IsReadOnly()==false)));
	//ペースト
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Paste,
			((GetTextDocument().NVI_IsReadOnly()==false)&&(AScrapWrapper::ExistClipboardTextScrapData()==true)));
	//削除
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Clear,((IsCaretMode()==false)&&(GetTextDocument().NVI_IsReadOnly()==false)));
	//全てを選択
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_SelectAll,true);
	//バックスラッシュ入力
	{
		AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_InputBackslashYen,true);
		AText	text;
		if( NVI_GetInputBackslashByYenKeyMode() == true )
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
		AApplication::GetApplication().NVI_GetMenuManager().SetMenuItemText(kMenuItemID_InputBackslashYen,text);
	}
}


//描画要求時のコールバック(AView用)
void	AView_EditBox::EVTDO_DoDraw()
{
	ADocument_EditBox&	document = GetTextDocument();
	
	//背景描画
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	if( NVI_GetFrameViewControlID() == kControlID_Invalid )
	{
		NVMC_PaintRect(localFrameRect,mBackgroundColor,mTransparency);
	}
	
	//文字色取得
	AColor	color = mColor;
	//
	if( NVMC_IsControlEnabled() == false )
	{
		color = kColor_Gray;
	}
	
	/*
	if( NVM_GetWindow().NVI_IsWindowActive() == false )
	{
		color = mColorDeactive;
	}
	if( NVI_GetEnabled() == false )//#135
	{
		color = mColorDeactive;
	}
	*/
	
	//#798
	//テキストが空の場合の背景描画
	if( GetTextDocumentConst().SPI_GetTextLength() == 0 && mEnableBackgroundColorForEmptyState == true )
	{
		AColor	backgroundColor = mBackgroundActiveColorForEmptyState;
		if( NVM_GetWindow().NVI_IsWindowActive() == false )
		{
			backgroundColor = mBackgroundDeactiveColorForEmptyState;
		}
		NVMC_PaintRect(localFrameRect,backgroundColor,mTransparency);
	}
	//テキストが空の場合のテキスト描画
	if( GetTextDocumentConst().SPI_GetTextLength() == 0 && mTextForEmptyState.GetItemCount() > 0 )
	{
		//テキストが空の場合のテキスト描画
		NVMC_DrawText(localFrameRect,mTextForEmptyState,kColor_Gray60Percent,kTextStyle_Normal);
		//ショートカット文字表示
		ALocalRect	r = localFrameRect;
		r.left = localFrameRect.right - NVMC_GetDrawTextWidth(mTextForEmptyState_Shortcut) - 3;
		NVMC_DrawText(r,mTextForEmptyState_Shortcut,kColor_Gray60Percent,kTextStyle_Normal);
	}
	
	//検索キャンセルボタン表示
	if( IsSearchCancelButtonDisplayed() == true )
	{
		ALocalRect	rect = {0};
		GetSearchCancelButtonRect(rect);
		ALocalPoint	pt = {rect.left,rect.top};
		AImageID	buttonImageID = kImageID_iconSwCancel;
		if( mPressingSearchCancelButton == true )
		{
			buttonImageID = kImageID_iconSwCancel_p;
		}
		else if( mHoveringSearchCancelButton == true )
		{
			buttonImageID = kImageID_iconSwCancel_h;
		}
		NVMC_DrawImage(buttonImageID,pt);
	}
	
	//1行ごとに描画
	AItemCount	lineCount = GetTextDocument().SPI_GetLineCount();
	for( AIndex lineIndex = 0; lineIndex < lineCount; lineIndex++ )
	{
		AImageRect	lineImageRect;
		GetLineImageRect(lineIndex,lineImageRect);
		ALocalRect	lineLocalRect;
		NVM_GetLocalRectFromImageRect(lineImageRect,lineLocalRect);
		if( NVMC_IsRectInDrawUpdateRegion(lineLocalRect) == false )   continue;
		
		//
		//#455 NVMC_SetDefaultTextProperty(mFontName,mFontSize,color,kTextStyle_Normal,true);
		//#455 
		ATextStyle	style = kTextStyle_Normal;
		if( mLineStyleArray_Style.GetItemCount() > lineIndex )
		{
			style = mLineStyleArray_Style.Get(lineIndex);
			color = mLineStyleArray_Color.Get(lineIndex);
		}
		NVMC_SetDefaultTextProperty(mFontName,mFontSize,color,style,true);
		//
		CTextDrawData	textDrawData(false);
		GetTextDocument().SPI_GetTextDrawData(lineIndex,textDrawData);
		
		//選択色取得
		AColor	selectionColor;
		if( NVI_IsFocusActive() == true )
		{
			AColorWrapper::GetHighlightColor(selectionColor);
		}
		else
		{
			selectionColor = kColor_Gray90Percent;
		}
		textDrawData.selectionColor = selectionColor;
		//選択表示
		textDrawData.drawSelection = false;
		if( mCaretMode == false )
		{
			//ほかのViewでのテキスト編集を反映させる場合等、どうしても選択範囲がイリーガルになることがありうるので、
			//イリーガルの場合は選択範囲表示はさせないようにする。
			//if( IsTextPointValid(inSelectTextPoint1) == true && IsTextPointValid(inSelectTextPoint2) == true )
			{
				ATextPoint	spt, ept;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),spt,ept);
				if( spt.y == lineIndex && ept.y == lineIndex )
				{
					textDrawData.drawSelection = true;
					textDrawData.selectionStart = textDrawData.OriginalTextArray_UnicodeOffset.Get(spt.x);
					textDrawData.selectionEnd = textDrawData.OriginalTextArray_UnicodeOffset.Get(ept.x);
				}
				else if( spt.y < lineIndex && ept.y == lineIndex )
				{
					textDrawData.drawSelection = true;
					textDrawData.selectionStart = kIndex_Invalid;
					textDrawData.selectionEnd = textDrawData.OriginalTextArray_UnicodeOffset.Get(ept.x);
				}
				else if( spt.y < lineIndex && ept.y > lineIndex )
				{
					textDrawData.drawSelection = true;
					textDrawData.selectionStart = textDrawData.OriginalTextArray_UnicodeOffset.Get(0);
					textDrawData.selectionEnd = kIndex_Invalid;
				}
				else if( spt.y == lineIndex && ept.y > lineIndex )
				{
					textDrawData.drawSelection = true;
					textDrawData.selectionStart = textDrawData.OriginalTextArray_UnicodeOffset.Get(spt.x);
					textDrawData.selectionEnd = kIndex_Invalid;
				}
			}
		}
		
		//改行コードテキスト追加
		if( mDrawLineEnd == true && document.SPI_ExistLineEnd(lineIndex) == true )
		{
			textDrawData.AddText(mLineEndDisplayText,mLineEndDisplayText_UTF16,kColor_Gray);
		}
		
		//テキスト描画
		AImageRect	imageDrawRect;
		GetLineImageRect(lineIndex,imageDrawRect);
		ALocalRect	localDrawRect = localFrameRect;
		NVM_GetLocalRectFromImageRect(imageDrawRect,localDrawRect);
		/*
		ANumber	yoffset = 0;
		if( lineCount == 1 )
		{
			yoffset = ((localFrameRect.bottom - localFrameRect.top ) - (localDrawRect.bottom - localDrawRect.top))/2;
			localDrawRect.top += yoffset;
			localDrawRect.bottom += yoffset;
		}
		*/
		//
		NVMC_DrawText(localDrawRect,textDrawData);
		
		//InlineInput ハイライト下線表示
		for( AIndex index = 0; index < mHiliteLineStyleIndex.GetItemCount(); index++ )
		{
			AIndex	hiliteLineStyleIndex = mHiliteLineStyleIndex.Get(index);
			if( hiliteLineStyleIndex == kIndex_Invalid )   continue;
			AColor	color = mInlineInputHiliteColorArray.Get(hiliteLineStyleIndex);
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
				lineLocalStart.y += GetLineHeightWithoutMargin()-1;//win 080618
				lineLocalEnd.y += GetLineHeightWithoutMargin()-1;//win 080618
				lineLocalEnd.x -= 2;//#185
				if( lineLocalEnd.x < lineLocalStart.x )   lineLocalEnd.x = lineLocalStart.x;//#185
				NVMC_DrawLine(lineLocalStart,lineLocalEnd,color);
			}
		}
	}
}

/**
マウスボタン押下時のOSイベントコールバック

@param inLocalPoint マウスボタン押下の場所（コントロールローカル座標）
@param inModifierKeys ModifierKeysの状態
@param inClickCout クリック回数
*/
ABool	AView_EditBox::EVTDO_DoMouseDown( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	//フレームをクリック時は、(-1,-1)がセットされてここに来るので、全体選択してリターン。
	if( inLocalPoint.x == -1 && inLocalPoint.y == -1 )
	{
		NVI_SetSelectAll();
		return true;
	}
	
	//編集不可なら何もしないで終了
	//#455 if( mEnableEdit == false )   return false;
	
	//自ViewがActiveでない場合は、Windowへフォーカスの移動を依頼する
	if( NVI_IsFocusActive() == false && NVMC_IsControlEnabled() == true )
	{
		ABool	svSelectAllAutomatic = NVI_GetAutomaticSelectBySwitchFocus();
		NVI_SetAutomaticSelectBySwitchFocus(false);
		//フォーカス設定
		NVM_GetWindow().NVI_SwitchFocusTo(NVI_GetControlID());
		//
		NVI_SetAutomaticSelectBySwitchFocus(svSelectAllAutomatic);
	}
	
	if( inClickCount == 1 )
	{
		DoMouseDownSingleClick(inLocalPoint,inModifierKeys);
	}
	else if( inClickCount == 2 )
	{
		DoMouseDownDoubleClick(inLocalPoint,inModifierKeys);
	}
	else if( inClickCount == 3 )
	{
		DoMouseDownTripleClick(inLocalPoint,inModifierKeys);
	}
	return false;
}

/**
マウスがView外へ移動した時のOSイベントコールバック

@param inLocalPoint マウスカーソルの場所（コントロールローカル座標）
*/
void	AView_EditBox::EVTDO_DoMouseLeft( const ALocalPoint& inLocalPoint )
{
	if( mHoveringSearchCancelButton == true )
	{
		mHoveringSearchCancelButton = false;
		NVI_Refresh();
	}
}

//マウスクリック（シングルクリック）
void	AView_EditBox::DoMouseDownSingleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	
	//#725
	//検索キャンセルボタンmouse down時処理（mouse up時にクリック時処理実行）
	if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
	{
		mMouseTrackingMode = kMouseTrackingMode_SingleClick;
		mTrackingPressSearchCancelButton = true;
		mPressingSearchCancelButton = true;
		NVI_Refresh();
		NVM_SetMouseTrackingMode(true);
		return;
	}
	
	//選択範囲内クリック（Dragまたはキャレット設定）
	if( IsInSelect(clickImagePoint,true) == true )
	{
		//Windowsの場合は、ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
		//#688 if( mMouseTrackByLoop == false )
		//#688 {
		//TrackingMode設定
		mMouseTrackingMode = kMouseTrackingMode_SingleClickInSelect;
		mMouseTrackStartImagePoint = clickImagePoint;
		mMouseTrackResultIsDrag = false;
		NVM_SetMouseTrackingMode(true);
		return;
		//#688 }
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
		//#688 SetCaretTextPoint()内でShowCaretReserve()を実行しているので不要 ShowCaret();
	}
	
	//マウスドラッグによる選択
	
	//Windowsの場合は、ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_SingleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
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
	//ATickCount	firsttickcount = ATimerWrapper::GetTickCount();
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
	*/
}

//Mouse Track
void	AView_EditBox::DoMouseTrack_SingleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//表示フレームをImage座標で取得
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//表示フレーム外にマウス移動したときはスクロールする(Y方向)
		if( mouseImagePoint.y < frameImageRect.top )
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + DragScrollV(mouseImagePoint.y-frameImageRect.top,inModifierKeys);
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > frameImageRect.bottom )
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + DragScrollV(mouseImagePoint.y-frameImageRect.bottom,inModifierKeys);
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
			//#400 現在行の端までしか左右スクロール出来ないようにする。
			//現在行の表示幅を取得
			ATextPoint	pt;
			GetTextPointFromImagePoint(inMouseImagePoint,pt);
			pt.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(pt.y);
			AImagePoint	ipt;
			GetImagePointFromTextPoint(pt,ipt);
			ANumber	imageWidth = ipt.x;
			//
			if( frameImageRect.right < /*#400 NVM_GetImageWidth()*/imageWidth )
			{
				NVI_Scroll(NVI_GetHScrollBarUnit(),0);
				mouseImagePoint.x = frameImageRect.right + NVI_GetHScrollBarUnit() -1;
			}
			else
			{
				mouseImagePoint.x = frameImageRect.right;
			}
		}
		
		ATextPoint	selectTextPoint;
		GetTextPointFromImagePoint(mouseImagePoint,selectTextPoint);
		SetSelectTextPoint(selectTextPoint);
	}
}

//
void	AView_EditBox::DoMouseTrackEnd_SingleClickInSelect( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	ALocalPoint	localPoint;
	NVM_GetLocalPointFromImagePoint(inMouseImagePoint,localPoint);
	ATextPoint	textPoint;
	GetTextPointFromImagePoint(inMouseImagePoint,textPoint);
	//#135 Drag&Drop対応
	if( mMouseTrackResultIsDrag == true )
	{
		//マウスボタン押下中にマウスを動かした場合
		//Drag領域設定
		AArray<ALocalRect>	dragRect;
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
			imageRect.bottom	= imageEndPoint.y + GetLineHeightWithMargin();
			if( y != ept.y )
			{
				imageRect.right = viewImageRect.right;
			}
			ALocalRect	localRect;
			NVM_GetLocalRectFromImageRect(imageRect,localRect);
			dragRect.Add(localRect);
			sx = 0;
		}
		//Scrap設定
		AArray<AScrapType>	scrapType;
		ATextArray	data;
		//選択中のテキスト取得
		AText	text;
		GetSelectedText(text);
		//#688
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
		legacyText.ConvertFromUTF8(ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName));//win
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
		//#688 SetCaretTextPoint()内でShowCaretReserve()を実行しているので不要 ShowCaret();
	}
}

//マウスクリック（ダブルクリック）
void	AView_EditBox::DoMouseDownDoubleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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
	//
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	
	//マウスドラッグによる選択
	
	//Windowsの場合は、ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_DoubleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
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
void	AView_EditBox::DoMouseTrack_DoubleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//表示フレームをImage座標で取得
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//表示フレーム外にマウス移動したときはスクロールする(Y方向)
		if( mouseImagePoint.y < frameImageRect.top )
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + DragScrollV(mouseImagePoint.y-frameImageRect.top,inModifierKeys);
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > frameImageRect.bottom )
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + DragScrollV(mouseImagePoint.y-frameImageRect.bottom,inModifierKeys);
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
			//#400 現在行の端までしか左右スクロール出来ないようにする。
			//現在行の表示幅を取得
			ATextPoint	pt;
			GetTextPointFromImagePoint(inMouseImagePoint,pt);
			pt.x = GetTextDocumentConst().SPI_GetLineLengthWithoutLineEnd(pt.y);
			AImagePoint	ipt;
			GetImagePointFromTextPoint(pt,ipt);
			ANumber	imageWidth = ipt.x;
			//
			if( frameImageRect.right < /*#400 NVM_GetImageWidth()*/imageWidth )
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
void	AView_EditBox::DoMouseDownTripleClick( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
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
	SetSelect(mFirstSelectStart,mFirstSelectEnd);
	mClickTextLineIndex = clickTextPoint.y;
	
	//マウスドラッグによる選択
	
	//Windowsの場合は、ここで一旦イベントコールバックを抜ける。MouseTrackは別のイベントで行う。
	//#688 if( mMouseTrackByLoop == false )
	//#688 {
	//TrackingMode設定
	mMouseTrackingMode = kMouseTrackingMode_TripleClick;
	mMouseTrackStartImagePoint = clickImagePoint;
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
void	AView_EditBox::DoMouseTrack_TripleClick( const AImagePoint& inMouseImagePoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	mouseImagePoint = inMouseImagePoint;
	{
		//表示フレームをImage座標で取得
		AImageRect	frameImageRect;
		NVM_GetImageViewRect(frameImageRect);
		//表示フレーム外にマウス移動したときはスクロールする(Y方向)
		if( mouseImagePoint.y < frameImageRect.top )
		{
			if( frameImageRect.top > 0 )
			{
				mouseImagePoint.y = frameImageRect.top + DragScrollV(mouseImagePoint.y-frameImageRect.top,inModifierKeys);
			}
			else
			{
				mouseImagePoint.y = frameImageRect.top + 1;
			}
		}
		else if( mouseImagePoint.y > frameImageRect.bottom )
		{
			if( frameImageRect.bottom < NVM_GetImageHeight() )
			{
				mouseImagePoint.y = frameImageRect.bottom + DragScrollV(mouseImagePoint.y-frameImageRect.bottom,inModifierKeys);
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
}

//#688 #if IMPLEMENTATION_FOR_WINDOWS
//Mouse Tracking
void	AView_EditBox::EVTDO_DoMouseTracking( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//検索キャンセルボタンtracking処理
	if( mTrackingPressSearchCancelButton == true )
	{
		if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
		{
			mPressingSearchCancelButton = true;
		}
		else
		{
			mPressingSearchCancelButton = false;
		}
		NVI_Refresh();
		return;
	}
	
	//
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	switch(mMouseTrackingMode)
	{
	  case kMouseTrackingMode_SingleClick:
		{
			//Mouse Track
			DoMouseTrack_SingleClick(mouseImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_SingleClickInSelect:
		{
			if( mouseImagePoint.x != mMouseTrackStartImagePoint.x || mouseImagePoint.y != mMouseTrackStartImagePoint.y )
			{
				mMouseTrackResultIsDrag = true;
				//
				DoMouseTrackEnd_SingleClickInSelect(mouseImagePoint,inModifierKeys);
				//TrackingMode解除
				mMouseTrackingMode = kMouseTrackingMode_None;
				NVM_SetMouseTrackingMode(false);
			}
			break;
		}
	  case kMouseTrackingMode_DoubleClick:
		{
			//Mouse Track
			DoMouseTrack_DoubleClick(mouseImagePoint,inModifierKeys);
			break;
		}
	  case kMouseTrackingMode_TripleClick:
		{
			//Mouse Track
			DoMouseTrack_TripleClick(mouseImagePoint,inModifierKeys);
			break;
		}
	  default:
		{
			_ACError("",this);
			break;
		}
	}
}

//Mouse Tracking終了（マウスボタンUp）
void	AView_EditBox::EVTDO_DoMouseTrackEnd( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//検索キャンセルボタンクリック時処理
	if( mTrackingPressSearchCancelButton == true )
	{
		if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
		{
			NVI_SetText(GetEmptyText());
			NVM_GetWindow().EVT_TextInputted(NVI_GetControlID());
		}
		mPressingSearchCancelButton = false;
		mTrackingPressSearchCancelButton = false;
		NVI_Refresh();
		//
		UpdateBackgroundColor();
		return;
	}
	
	//
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	//
	if( mMouseTrackingMode == kMouseTrackingMode_SingleClickInSelect )
	{
		DoMouseTrackEnd_SingleClickInSelect(mouseImagePoint,inModifierKeys);
	}
	//TrackingMode解除
	mMouseTrackingMode = kMouseTrackingMode_None;
	NVM_SetMouseTrackingMode(false);
}
//#688 #endif

//B0260
ANumber	AView_EditBox::DragScrollV( const ANumber inMouseOffset, const AModifierKeys inModifierKeys )
{
	ANumber	delta = GetLineHeightWithMargin();
	if( true )
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

//
void	AView_EditBox::EVTDO_DoMouseWheel( const AFloatNumber inDeltaX, const AFloatNumber inDeltaY, const AModifierKeys inModifierKeys,
											const ALocalPoint inLocalPoint )//win 080706
{
	//スクロール率
	ANumber	scrollPercent;
	if( AKeyWrapper::IsCommandKeyPressed(inModifierKeys) == true  || AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
	{
		scrollPercent = 300;
	}
	else
	{
		scrollPercent = 100;
	}
	//スクロール実行
	NVI_Scroll(0,inDeltaY*NVI_GetVScrollBarUnit()*scrollPercent/100);
}

//
ABool	AView_EditBox::EVTDO_DoMouseMoved( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//検索キャンセルボタンホバー処理
	ABool	oldHoveringSearchCancelButton = mHoveringSearchCancelButton;
	if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
	{
		mHoveringSearchCancelButton = true;
	}
	else
	{
		mHoveringSearchCancelButton = false;
	}
	if( oldHoveringSearchCancelButton != mHoveringSearchCancelButton )
	{
		NVI_Refresh();
	}
	return true;
}

//#135
/**
マウスボタン右クリック時のコールバック
*/
ABool	AView_EditBox::EVTDO_DoContextMenu( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys, const ANumber inClickCount )
{
	if( mContextMenuID == kIndex_Invalid )   return false;
	//
	AGlobalPoint	globalPoint;
	NVM_GetWindow().NVI_GetGlobalPointFromControlLocalPoint(NVI_GetControlID(),inLocalPoint,globalPoint);
	globalPoint.y += GetLineHeightWithoutMargin();
	AImagePoint	mouseImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,mouseImagePoint);
	//
	const AIndex	kCut = 0;
	const AIndex	kCopy = 1;
	const AIndex	kPaste = 2;
	//
	ABool	isInSelect = IsInSelect(mouseImagePoint,true);
	AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuEnableMenuItem(mContextMenuID,kCut,
				mEnableEdit&&NVM_GetWindow().NVI_IsControlEnable(NVI_GetControlID())&&isInSelect);
	AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuEnableMenuItem(mContextMenuID,kCopy,isInSelect);
	AApplication::GetApplication().NVI_GetMenuManager().SetContextMenuEnableMenuItem(mContextMenuID,kPaste,
				mEnableEdit&&NVM_GetWindow().NVI_IsControlEnable(NVI_GetControlID())&&(AScrapWrapper::ExistClipboardTextScrapData()==true));
	//コンテキストメニュー表示
	//#688 AApplication::GetApplication().NVI_GetMenuManager().ShowContextMenu(mContextMenuID,globalPoint,NVM_GetWindow().NVI_GetWindowRef());
	NVMC_ShowContextMenu(mContextMenuID,globalPoint);//#688
	return true;
}

//カーソル
ACursorType	AView_EditBox::EVTDO_GetCursorType( const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	//検索キャンセルボタン時は矢印カーソル
	if( MouseIsInSearchCancelButtonRect(inLocalPoint) == true )
	{
		return kCursorType_Arrow;
	}
	
	//選択範囲内なら矢印カーソル
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
						AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true )
			{
				return kCursorType_ArrowCopy;
			}
			else
			{
				return kCursorType_Arrow;
			}
		}
	}
	
	//上記以外はIビーム
	return kCursorType_IBeam;
}

//
void	AView_EditBox::EVTDO_DoTickTimer()
{
	//
	mCaretTickCount++;
	if( mCaretTickCount >= ATimerWrapper::GetCaretTime() )//#688 >から>=へ変更
	{
		if( NVI_IsFocusActive() == true )
		{
			BlinkCaret();
		}
		else
		{
			HideCaret();
		}
	}
}

//
ABool	AView_EditBox::EVTDO_DoTextInput( const AText& inText, //#688 const AOSKeyEvent& inOSKeyEvent, 
		const AKeyBindKey inKeyBindKey, const AModifierKeys inModifierKeys, const AKeyBindAction inKeyBindAction,
		ABool& outUpdateMenu )
{
	//メニュー更新フラグOFFに設定 #688
	outUpdateMenu = false;
	
	//編集不可なら何もしないで終了
	if( mEnableEdit == false )   return false;
	
	//コントロールdisableならテキスト入力不可
	if( NVMC_IsControlEnabled() == false )
	{
		return false;
	}
	
	//#688
	//tick timerよりも前に文字入力された場合に、ここでreserveされたcaret描画を実行する。
	//（ここで描画しないとオートリピートしたとき等、キャレット描画が遅れて見える）
	ShowCaretIfReserved();
	
	//メニュー更新フラグONに設定 #688
	outUpdateMenu = true;
	
	ABool	handled = false;
	ABool	touch = false;//#135
	if( inText.GetItemCount() >0 )//#688 == 1 )
	{
		AModifierKeys	modifiers = inModifierKeys;//#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent);
		ABool	shift = AKeyWrapper::IsShiftKeyPressed(modifiers);
		ABool	option = AKeyWrapper::IsOptionKeyPressed(modifiers);
		ABool	cmd = AKeyWrapper::IsCommandKeyPressed(modifiers);
		ABool	ctrl = AKeyWrapper::IsControlKeyPressed(modifiers);//#688
		AUChar	ch = inText.Get(0);
		if( mListViewID != kControlID_Invalid )
		{
			if( NVM_GetWindow().NVI_GetListView(mListViewID).SPI_EditBoxInput(ch) == true )
			{
				return true;
			}
		}
		switch(/*#688 ch*/inKeyBindKey)
		{
			//arrow
		  case kKeyBindKey_Up://#688 0x1C:
		  case kKeyBindKey_Down://#688 0x1D:
		  case kKeyBindKey_Right://#688 0x1E:
		  case kKeyBindKey_Left://#688 0x1F:
			{
				//#798
				//上下キーの場合はウインドウへ通知（実行した場合はbreak）
				if( inKeyBindKey == kKeyBindKey_Up )
				{
					if( NVM_GetWindow().OWICB_ViewArrowUpKeyPressed(NVI_GetControlID(),modifiers) == true )
					{
						handled = true;
						break;
					}
				}
				if( inKeyBindKey == kKeyBindKey_Down )
				{
					if( NVM_GetWindow().OWICB_ViewArrowDownKeyPressed(NVI_GetControlID(),modifiers) == true )
					{
						handled = true;
						break;
					}
				}
				//#688
				//charに変換
				AUChar	ch = 0;
				switch(inKeyBindKey)
				{
				  case kKeyBindKey_Left:
					{
						ch = 0x1C;
						break;
					}
				  case kKeyBindKey_Right:
					{
						ch = 0x1D;
						break;
					}
				  case kKeyBindKey_Up:
					{
						ch = 0x1E;
						break;
					}
				  case kKeyBindKey_Down:
					{
						ch = 0x1F;
						break;
					}
				  default:
					{
						//処理なし
						break;
					}
				}
				//
				if( option == true )
				{
					ArrowKeyWord(ch,shift,true);
				}
				else if( cmd == true )
				{
					ArrowKeyEdge(ch,shift);
				}
				else if( shift == true )
				{
					ArrowKeySelect(ch);
				}
				else
				{
					ArrowKey(ch);
				}
				handled = true;
				break;
			}
		  case kKeyBindKey_BS://#688 0x08:
			{
				DeletePreviousChar();
				touch = true;//#135
				handled = true;
				break;
			}
		  case kKeyBindKey_DEL://#688 0x7F:
			{
				DeleteNextChar();
				touch = true;//#135
				handled = true;
				break;
			}
		  case kKeyBindKey_Period://#688 '.'://#135
			{
				if( cmd == true )
				{
					NVM_GetWindow().OWICB_ViewEscapeKeyPressed(NVI_GetControlID(),modifiers);
					handled = true;
				}
				break;
			}
		  case kKeyBindKey_Return://#688 '\r'://ReturnKey
		  case kKeyBindKey_Enter://#688 3://EnterKey
			{
				if( NVI_GetCatchReturn() == true || option == true || ctrl == true || cmd == true )//#688 option+returnは常に改行入力になるよう変更
				{
					//------------------改行コード入力の場合------------------
					AText	text;
					text.Add(kUChar_LineEnd);
					TextInput(kEditBoxUndoTag_Typing,text);
					touch = true;//#135
				}
				else
				{
					//------------------改行コード入力しない場合------------------
					//（検索ダイアログでの検索実行などを行う。改行コード入力した場合は、実行されない）
					NVM_GetWindow().OWICB_ViewReturnKeyPressed(NVI_GetControlID(),modifiers);//#135
				}
				handled = true;
				break;
			}
		  case kKeyBindKey_Tab://#688 '\t':
			{
				if( NVI_GetCatchTab() == true || option == true || ctrl == true || cmd == true )//#688 option+tabは常にタブ入力になるよう変更
				{
					//------------------タブコード入力の場合------------------
					AText	text;
					text.Add('\t');
					TextInput(kEditBoxUndoTag_Typing,text);
					touch = true;//#135
				}
				else
				{
					//------------------タブコード入力しない場合------------------
					//（検索ダイアログでの検索実行などを行う。タブコード入力した場合は、実行されない）
					NVM_GetWindow().OWICB_ViewTabKeyPressed(NVI_GetControlID(),modifiers);//#135
				}
				handled = true;
				break;
			}
		  case kKeyBindKey_Escape://#688 0x1B://#137
			{
				NVM_GetWindow().OWICB_ViewEscapeKeyPressed(NVI_GetControlID(),modifiers);//#135
				handled = true;
				break;
			}
		  default:
			{
				//処理なし
				break;
			}
		}
		if( ch <= 0x1F )   handled = true;
	}
	//#137
	AText	yen;
	yen.SetLocalizedText("Text_Yen");
	if( yen.Compare(inText) == true )
	{
		AText	backslash;
		backslash.Add(kUChar_Backslash);
		if( NVI_GetInputBackslashByYenKeyMode() == true )
		{
			TextInput(kEditBoxUndoTag_Typing,backslash);
			touch = true;//#135
			handled = true;
		}
	}
	//#137
	//該当メニューコマンドが使用不可能な場合のコマンドショートカットキーの場合、文字入力として送られてくる
	//ので、それは文字入力として扱わないようにする
	if( AKeyWrapper::IsCommandKeyPressed(/*#688 AKeyWrapper::GetEventKeyModifiers(inOSKeyEvent)*/inModifierKeys) == true )
	{
		handled = true;
	}
	
	//#135 キーフィルター
	switch(NVI_GetDataType())
	{
	  case kDataType_Number:
		{
			for( AIndex i = 0; i < inText.GetItemCount(); i++ )
			{
				AUChar ch = inText.Get(i);
				if( (ch >= '0' && ch <= '9') || ch == '-' )   continue;
				else 
				{
					handled = true;
					break;
				}
			}
			break;
		}
	  case kDataType_FloatNumber:
		{
			for( AIndex i = 0; i < inText.GetItemCount(); i++ )
			{
				AUChar ch = inText.Get(i);
				if( (ch >= '0' && ch <= '9') || ch == '-' || ch == '.' )   continue;
				else 
				{
					handled = true;
					break;
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
	//
	if( handled == false )
	{
		if( inText.GetItemCount() > 0 )//win
		{
			TextInput(kEditBoxUndoTag_Typing,inText);
			touch = true;//#135
			handled = true;
		}
		else//win
		{
			handled = false;
		}
	}
	//#0 SPI_UpdateImageSize();
	//win ACursorWrapper::ObscureCursor();
	//#135
	if( touch == true )
	{
		SPI_UpdateImageSize();
		NVM_GetWindow().EVT_TextInputted(NVI_GetControlID());
	}
	return handled;//win
}

//矢印キー（ノーマル）
void	AView_EditBox::ArrowKey( const AUChar inChar )
{
	//ArrowKeySelectと同じアルゴリズムじゃダメ？検討必要
	ATextPoint	caretTextPoint;
	AImagePoint	imagePoint;
	switch(inChar)
	{
		//左
	  case 0x1C:
		{
			if( IsCaretMode() == true )
			{
				GetCaretTextPoint(caretTextPoint);
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
					SetCaretTextPoint(caretTextPoint);
				}
			}
			else
			{
				ATextPoint	pt1, pt2;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt1);
			}
			break;
		}
		//右
	  case 0x1D:
		{
			if( IsCaretMode() == true )
			{
				GetCaretTextPoint(caretTextPoint);
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
					SetCaretTextPoint(caretTextPoint);
				}
			}
			else
			{
				ATextPoint	pt1, pt2;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt2);
			}
			break;
		}
		//上
	  case 0x1E:
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
					imagePoint.y = caretTextPoint.y * GetLineHeightWithMargin() +2;
					imagePoint.x = GetOldArrowImageX();
					GetTextPointFromImagePoint(imagePoint,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);
				SetOldArrowImageX(svOldArrowImageX);
			}
			else
			{
				ATextPoint	pt1, pt2;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
				SetCaretTextPoint(pt1);
			}
			break;
		}
		//下
	  case 0x1F:
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
					imagePoint.y = caretTextPoint.y * GetLineHeightWithMargin() +2;
					imagePoint.x = GetOldArrowImageX();
					GetTextPointFromImagePoint(imagePoint,caretTextPoint);
				}
				SetCaretTextPoint(caretTextPoint);
				SetOldArrowImageX(svOldArrowImageX);
			}
			else
			{
				ATextPoint	pt1, pt2;
				OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
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
void	AView_EditBox::ArrowKeySelect( const AUChar inChar )
{
	ATextPoint	selectPoint = GetCaretTextPoint();
	if( IsCaretMode() == false )
	{
		selectPoint = GetSelectTextPoint();
	}
	ATextIndex	selectPos = GetTextDocument().SPI_GetTextIndexFromTextPoint(selectPoint);
	switch(inChar)
	{
		//左
	  case 0x1C:
		{
			selectPos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(selectPos);
			GetTextDocument().SPI_GetTextPointFromTextIndex(selectPos,selectPoint,true);//B0000
			break;
		}
		//右
	  case 0x1D:
		{
			selectPos = GetTextDocumentConst().SPI_GetNextCharTextIndex(selectPos);
			GetTextDocument().SPI_GetTextPointFromTextIndex(selectPos,selectPoint,true);//B0000
			break;
		}
		//上
	  case 0x1E:
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
				imagePoint.y -= GetLineHeightWithMargin();
				GetTextPointFromImagePoint(imagePoint,selectPoint);
			}
			break;
		}
		//下
	  case 0x1F:
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
				imagePoint.y += GetLineHeightWithMargin();
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
	SetSelect(GetCaretTextPoint(),selectPoint);
	
	//スクロール位置補正
	AdjustScroll(GetSelectTextPoint());
}

//矢印キー（単語移動）
void	AView_EditBox::ArrowKeyWord( const AUChar inChar, const ABool inSelect, const ABool inLineStop )
{
	ATextPoint	textpoint = GetCaretTextPoint();
	if( IsCaretMode() == false )
	{
		if( inSelect == false )
		{
			ArrowKey(inChar);
			return;
		}
		textpoint = GetSelectTextPoint();
	}
	ATextIndex	textpos = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(textpoint);
	switch(inChar)
	{
		//左
	  case 0x1C:
		{
			if( textpos == 0 )   break;
			textpos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(textpos);
			ATextIndex	spos,epos;
			GetTextDocumentConst().SPI_FindWord(textpos,spos,epos);
			while( spos > 0 )
			{
				if( inLineStop == true && GetTextDocumentConst().SPI_GetTextChar(spos-1) == kUChar_LineEnd )   break;
				if( GetTextDocumentConst().SPI_IsAsciiAlphabet(spos) == true || GetTextDocumentConst().SPI_IsUTF8Multibyte(spos) == true )   break;
				spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
				GetTextDocumentConst().SPI_FindWord(spos,spos,epos);
			}
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(spos,textpoint,true);//B0000
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
	  case 0x1D:
		{
			ATextIndex	spos,epos;
			GetTextDocumentConst().SPI_FindWord(textpos,spos,epos);
			while( epos < GetTextDocumentConst().SPI_GetTextLength() )
			{
				if( inLineStop == true && GetTextDocumentConst().SPI_GetTextChar(epos) == kUChar_LineEnd )   break;
				if( GetTextDocumentConst().SPI_IsAsciiAlphabet(epos-1) == true || GetTextDocumentConst().SPI_IsUTF8Multibyte(epos-1) == true )   break;//B0000
				GetTextDocumentConst().SPI_FindWord(epos,spos,epos);
			}
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(epos,textpoint,true);//B0000
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
	  case 0x1E:
	  case 0x1F:
		{
			if( inSelect == true )
			{
				ArrowKeySelect(inChar);
			}
			else
			{
				ArrowKey(inChar);
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
void	AView_EditBox::ArrowKeyEdge( const AUChar inChar, const ABool inSelect )
{
	ATextPoint	start, end;
	SPI_GetSelect(start,end);
	ATextPoint	textpoint;
	switch(inChar)
	{
		//左
	  case 0x1C:
		{
			textpoint = start;
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
	  case 0x1D:
		{
			textpoint = end;
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
	  case 0x1E:
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
	  case 0x1F:
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

//
void	AView_EditBox::SetOldArrowImageX( const ANumber inImageX )
{
	mOldArrowImageX = inImageX;
}

ANumber	AView_EditBox::GetOldArrowImageX() const
{
	return mOldArrowImageX;
}

//
void	AView_EditBox::DeleteNextChar()
{
	if( IsCaretMode() == false )
	{
		DeleteSelect(kEditBoxUndoTag_DELKey);
	}
	else
	{
		AIndex	start = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
		AIndex	end = GetTextDocumentConst().SPI_GetNextCharTextIndex(start);
		if( end > GetTextDocumentConst().SPI_GetTextLength() )   return;
		//Undoアクションタグ記録
		GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_DELKey);
		//
		DeleteTextFromDocument(start,end);
	}
}

//
void	AView_EditBox::DeletePreviousChar()
{
	if( IsCaretMode() == false )
	{
		DeleteSelect(kEditBoxUndoTag_BSKey);
	}
	else
	{
		AIndex	end = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(GetCaretTextPoint());
		AIndex	start = GetTextDocumentConst().SPI_GetPrevCharTextIndex(end);
		if( start < 0 )   return;
		//Undoアクションタグ記録
		GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_BSKey);
		//
		DeleteTextFromDocument(start,end);
	}
}

//
void	AView_EditBox::DeleteSelect( const AEditBoxUndoTag inUndoActionTag )
{
	if( IsCaretMode() == true )   return;
	//Undoアクションタグ記録
	GetTextDocument().SPI_RecordUndoActionTag(inUndoActionTag);
	//選択範囲削除実行
	DeleteTextFromDocument();
}

void	AView_EditBox::UndoRedo( const ABool inUndo )
{
	//
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());
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
	ATextPoint	caretPoint, selectPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(selectTextIndex,caretPoint);
	SetCaretTextPoint(caretPoint);
	if( selectTextLength > 0 )
	{
		GetTextDocument().SPI_GetTextPointFromTextIndex(selectTextIndex+selectTextLength,selectPoint);
		SetSelectTextPoint(selectPoint);
	}
	AdjustScroll(GetCaretTextPoint());
	//
	Refresh();
}

//
void	AView_EditBox::Cut( const ABool inAdditional )//R0213
{
	AText	text;
	GetSelectedText(text);
	if( inAdditional == true )//R0213
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);
		text.InsertText(0,scrapText);
	}
	AScrapWrapper::SetClipboardTextScrap(text,true,true);//#1044 #688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);//win
	DeleteSelect(kEditBoxUndoTag_Cut);
}

//
void	AView_EditBox::Copy( const ABool inAdditional )//R0213
{
	AText	text;
	GetSelectedText(text);
	if( text.GetItemCount() == 0 )   return;
	if( inAdditional == true )//R0213
	{
		AText	scrapText;
		AScrapWrapper::GetClipboardTextScrap(scrapText);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);//win
		text.InsertText(0,scrapText);
	}
	AScrapWrapper::SetClipboardTextScrap(text,true,true);//#1044 #688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);//win 
}

void	AView_EditBox::Paste()
{
	if( GetTextDocument().NVI_IsReadOnly() == true )   return;
	AText	text;
	AScrapWrapper::GetClipboardTextScrap(text);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName),true);//win
	TextInput(kEditBoxUndoTag_Paste,text);
}

//
void	AView_EditBox::TextInput( const AEditBoxUndoTag inUndoActionTag, const AText& inText )
{
	//Undoアクションタグ記録
	if( inUndoActionTag != kEditBoxUndoTag_NOP )//B0381
	{
		GetTextDocument().SPI_RecordUndoActionTag(inUndoActionTag);
	}
	
	//選択範囲削除、テキスト挿入
	DeleteAndInsertTextToDocument(inText);
	
}

//ドキュメントへテキスト挿入
void	AView_EditBox::DeleteAndInsertTextToDocument( const AText& inText )
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
void	AView_EditBox::InsertTextToDocument( const ATextIndex inInsertIndex, const AText& inText )
{
	//
	GetTextDocument().SPI_InsertText(inInsertIndex,inText);
	//
	SPI_UpdateImageSize();
	//キャレット設定
	ATextPoint	pt;
	GetTextDocument().SPI_GetTextPointFromTextIndex(inInsertIndex+inText.GetItemCount(),pt);
	SetCaretTextPoint(pt);
	//
	AdjustScroll(GetCaretTextPoint());
	//#725
	//背景色更新（空の場合に背景色が変わる場合に対応）
	UpdateBackgroundColor();
	//
	Refresh();
}
void	AView_EditBox::InsertTextToDocument( const ATextPoint& inInsertPoint, const AText& inText )
{
	InsertTextToDocument(GetTextDocument().SPI_GetTextIndexFromTextPoint(inInsertPoint),inText);
}

//ドキュメントからテキスト削除
void	AView_EditBox::DeleteTextFromDocument( const ATextIndex inStart, const ATextIndex inEnd, const ABool inDontRedraw )
{
	//
	GetTextDocument().SPI_DeleteText(inStart,inEnd);
	//
	SPI_UpdateImageSize();
	//キャレット設定
	ATextPoint	pt;
	GetTextDocument().SPI_GetTextPointFromTextIndex(inStart,pt);
	SetCaretTextPoint(pt);
	//
	AdjustScroll(GetCaretTextPoint());
	//#725
	//背景色更新（空の場合に背景色が変わる場合に対応）
	UpdateBackgroundColor();
	//
	Refresh();
}
void	AView_EditBox::DeleteTextFromDocument( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint, const ABool inDontRedraw )
{
	DeleteTextFromDocument(GetTextDocument().SPI_GetTextIndexFromTextPoint(inStartPoint),GetTextDocument().SPI_GetTextIndexFromTextPoint(inEndPoint),
			inDontRedraw);
}
//ドキュメントからテキスト削除（選択範囲）
void	AView_EditBox::DeleteTextFromDocument()
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	DeleteTextFromDocument(spt,ept);
}

//
ABool	AView_EditBox::EVTDO_DoInlineInput( const AText& inText, const AItemCount inFixLen,
		const AArray<AIndex>& inHiliteLineStyleIndex, const AArray<AIndex>& inHiliteStartPos, const AArray<AIndex>& inHiliteEndPos,
		const AItemCount inReplaceLengthInUTF16,//#688
		ABool& outUpdateMenu )
{
	//編集不可なら何もしないで終了
	if( mEnableEdit == false )   return false;
	
	//コントロールdisableならテキスト入力不可
	if( NVMC_IsControlEnabled() == false )
	{
		return false;
	}
	
	//メニュー状態更新
	outUpdateMenu = true;//#135
	
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
		GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_Typing);
	}
	
	AText	insertText;
	insertText.SetText(inText);
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
			GetTextDocument().SPI_DeleteText(spos,epos);
		}
		//挿入位置は前回未確定部分のうち削除しなかった部分の最後から
		insertionTextIndex = mInlineInputNotFixedTextPos + samecount;
		inlineInputFirstPos = mInlineInputNotFixedTextPos;
		//
		mInlineInputNotFixedTextPos = mInlineInputNotFixedTextPos+inFixLen;
		mInlineInputNotFixedTextLen = inText.GetItemCount() - inFixLen;
		//
		insertText.Delete(0,samecount);
	}
	else
	{
		//InlineInput最初からの場合
		ATextPoint	insertionPoint;
		if( IsCaretMode() == true )
		{
			//#688 英数2度押し対応。現在の選択位置から指定文字数分前までを削除する。
			if( inReplaceLengthInUTF16 > 0 )
			{
				AIndex	spos = 0, epos = 0;
				SPI_GetSelect(spos,epos);
				for( AIndex i = 0; i < inReplaceLengthInUTF16; i++ )
				{
					spos = GetTextDocumentConst().SPI_GetPrevCharTextIndex(spos);
				}
				DeleteTextFromDocument(spos,epos);
			}
			//
			GetCaretTextPoint(insertionPoint);
		}
		else
		{
			ATextPoint	ept;
			SPI_GetSelect(insertionPoint,ept);
			DeleteTextFromDocument(insertionPoint,ept);
			GetCaretTextPoint(insertionPoint);//B0349 削除の結果、キャレットのTextPointは変わることがある。（行折り返しありの場合）
		}
		insertionTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(insertionPoint);
		inlineInputFirstPos = GetTextDocument().SPI_GetTextIndexFromTextPoint(insertionPoint);
		//
		mInlineInputNotFixedTextPos = insertionTextIndex+inFixLen;
		mInlineInputNotFixedTextLen = inText.GetItemCount() - inFixLen;
	}
	
	if( mInlineInputNotFixedTextLen > 0 )//B0351 AquaSKK対策　全確定ならハイライト下線情報は全削除状態にする
	{
		//InlineInputハイライト下線の情報を保存
		mHiliteLineStyleIndex.SetFromObject(inHiliteLineStyleIndex);
		mHiliteStartPos.SetFromObject(inHiliteStartPos);
		mHiliteEndPos.SetFromObject(inHiliteEndPos);
		//mHiliteStartPos, mHiliteEndPosはテキスト全体のインデックスにしておく
		for( AIndex i = 0; i < mHiliteLineStyleIndex.GetItemCount(); i++ )
		{
			mHiliteStartPos.Set(i,mHiliteStartPos.Get(i)+inlineInputFirstPos);
			mHiliteEndPos.Set(i,mHiliteEndPos.Get(i)+inlineInputFirstPos);
		}
	}
	
	//B0332
	if( insertText.GetItemCount() >= 2 )
	{
		AUChar	ch1 = insertText.Get(0);
		AUChar	ch2 = insertText.Get(1);
		if( ch1 == 0xC2 && ch2 == 0xA5 )
		{
			if( NVI_GetInputBackslashByYenKeyMode() == true )
			{
				insertText.Delete(0,2);
				insertText.Insert1(0,kUChar_Backslash);
				//B0380 Fix部分が0の場合は、バイト数を減らした部分はNotFixedの部分のはずなので、mInlineInputNotFixedTextLenを1byte分減らす
				if( inFixLen == 0 )
				{
					mInlineInputNotFixedTextLen--;
				}
			}
		}
	}
	
	//テキスト挿入
	//InsertTextToDocument(insertionTextIndex,inText);
	GetTextDocument().SPI_InsertText(insertionTextIndex,insertText);
	
	//キャレット設定
	ATextPoint	caretPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertionTextIndex+insertText.GetItemCount(),caretPoint);
	
	//Hilite情報にキャレット情報が含まれている場合はそれに従う
	if( mInlineInputNotFixedTextLen > 0 )//B0351 AquaSKK対策　全確定ならキャレットは入力文字列最後のままにしておく
	{
		for( AIndex i = 0; i < inHiliteLineStyleIndex.GetItemCount(); i++ )
		{
			if( inHiliteLineStyleIndex.Get(i) == kIndex_Invalid )
			{
				GetTextDocument().SPI_GetTextPointFromTextIndex(inlineInputFirstPos+inHiliteStartPos.Get(i),caretPoint);
			}
		}
	}
	
	//
	SetCaretTextPoint(caretPoint);
	AdjustScroll(GetCaretTextPoint());
	
	//
	ATextPoint	insertionTextPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(insertionTextIndex,insertionTextPoint);
	ATextPoint	oldNotFixedStartTextPoint;
	GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos-inFixLen,oldNotFixedStartTextPoint);
	if( oldNotFixedStartTextPoint.y < insertionTextPoint.y )
	{
		//
		AImageRect	imageLineRect;
		GetLineImageRect(oldNotFixedStartTextPoint.y,insertionTextPoint.y,imageLineRect);
		ALocalRect	localLineRect;
		NVM_GetLocalRectFromImageRect(imageLineRect,localLineRect);
		NVMC_RefreshRect(localLineRect);
	}
	//#725
	//背景色更新（空の場合に背景色が変わる場合に対応）
	UpdateBackgroundColor();
	
	//
	Refresh();
	
	//#135
	NVM_GetWindow().EVT_TextInputted(NVI_GetControlID());
	
	//win Caretちらつき対策（途中で描画せずに、ここでまとめて表示）
	//Caret表示予約があれば表示する
	//キャレット残り対策 ShowCaretIfReserved();
	
	return true;
}

ABool	AView_EditBox::EVTDO_DoInlineInputOffsetToPos( const AIndex inOffset, ALocalPoint& outPos )
{
	ATextPoint	textPoint;
	if( mInlineInputNotFixedTextLen > 0 )
	{
		GetTextDocument().SPI_GetTextPointFromTextIndex(mInlineInputNotFixedTextPos+inOffset,textPoint);
	}
	else
	{
		GetCaretTextPoint(textPoint);
	}
	AImagePoint	imagePoint;
	GetImagePointFromTextPoint(textPoint,imagePoint);
	imagePoint.y += GetLineHeightWithoutMargin();
	NVM_GetLocalPointFromImagePoint(imagePoint,outPos);
	return true;
}

ABool	AView_EditBox::EVTDO_DoInlineInputPosToOffset( const ALocalPoint& inPos, AIndex& outOffset )
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

ABool	AView_EditBox::EVTDO_DoInlineInputGetSelectedText( AText& outText )
{
	GetSelectedText(outText);//#373
	return true;
}

//
void	AView_EditBox::EVTDO_DrawPreProcess()
{
	TempHideCaret();
}

//
void	AView_EditBox::EVTDO_DrawPostProcess()
{
	RestoreTempHideCaret();
}

//Drag&Drop処理（Drag中）
void	AView_EditBox::EVTDO_DoDragTracking( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys,
		ABool& outIsCopyOperation )
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
	//表示フレームをImage座標で取得
	AImageRect	frameImageRect;
	NVM_GetImageViewRect(frameImageRect);
	//
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	//scroll
	/*#688 
	if( mMouseTrackByLoop == true )
	{
		if( clickImagePoint.y < frameImageRect.top+kDragDropScrollRange && clickImagePoint.y > frameImageRect.top && frameImageRect.top > 0 )
		{
			AImagePoint	mouseImagePoint = clickImagePoint;
			AWindowPoint	mouseWindowPoint;
			AModifierKeys	modifiers;
			while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
			{
				//Image座標に変換
				ALocalPoint	mouseLocalPoint;
				NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
				NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
				//
				NVM_GetImageViewRect(frameImageRect);
				//
				if( frameImageRect.top <= 0 )   break;
				//
				if( mouseImagePoint.y < frameImageRect.top+kDragDropScrollRange && mouseImagePoint.y > frameImageRect.top )
				{
					mouseImagePoint.y = frameImageRect.top+kDragDropScrollRange + 
							DragScrollV(mouseImagePoint.y-(frameImageRect.top+kDragDropScrollRange),inModifierKeys);
				}
				else
				{
					break;
				}
			}
			clickImagePoint = mouseImagePoint;
		}
		else if( clickImagePoint.y > frameImageRect.bottom-kDragDropScrollRange && clickImagePoint.y < frameImageRect.bottom &&
					frameImageRect.bottom < NVM_GetImageHeight() )
		{
			AImagePoint	mouseImagePoint = clickImagePoint;
			AWindowPoint	mouseWindowPoint;
			AModifierKeys	modifiers;
			while( AMouseWrapper::TrackMouseDown(mouseWindowPoint,modifiers) == true )
			{
				//Image座標に変換
				ALocalPoint	mouseLocalPoint;
				NVM_GetWindow().NVI_GetControlLocalPointFromWindowPoint(NVI_GetControlID(),mouseWindowPoint,mouseLocalPoint);
				NVM_GetImagePointFromLocalPoint(mouseLocalPoint,mouseImagePoint);
				//
				NVM_GetImageViewRect(frameImageRect);
				//
				if( frameImageRect.bottom >= NVM_GetImageHeight() )   break;
				//
				if( mouseImagePoint.y > frameImageRect.bottom-kDragDropScrollRange && mouseImagePoint.y < frameImageRect.bottom )
				{
					mouseImagePoint.y = frameImageRect.bottom-kDragDropScrollRange + 
							DragScrollV(mouseImagePoint.y-(frameImageRect.bottom-kDragDropScrollRange),inModifierKeys);
				}
				else
				{
					break;
				}
			}
			clickImagePoint = mouseImagePoint;
		}
	}
	else
	*/
	//688{
	//イベントドリブンでドラッグスクロールさせる
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
	//#688 }
	//
	ATextPoint	clickTextPoint;
	GetTextPointFromImagePoint(clickImagePoint,clickTextPoint);
	if( IsInSelect(clickImagePoint,false) == false )
	{
		//単語置き換えDrag&Drop
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true )
		{
			ATextIndex	clickTextIndex = GetTextDocument().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			ATextIndex	wordStart, wordEnd;
			GetTextDocument().SPI_FindWord(clickTextIndex,wordStart,wordEnd);
			if( wordStart != wordEnd )
			{
				GetTextDocument().SPI_GetTextPointFromTextIndex(wordStart,mDragCaretTextPoint,true);
				GetTextDocument().SPI_GetTextPointFromTextIndex(wordEnd,mDragSelectTextPoint,false);
				//Draw
				XorWordDragDrop();
				mDragWordDragDrop = true;
			}
			else
			{
				mDragCaretTextPoint = clickTextPoint;
				//Draw
				XorDragCaret();
				mDragDrawn = true;
			}
		}
		//通常Drag&Drop
		else
		{
			mDragCaretTextPoint = clickTextPoint;
			//Draw
			XorDragCaret();
			mDragDrawn = true;
		}
	}
	if( AKeyWrapper::IsOptionKeyPressed(inModifierKeys) == true || AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true ||
				AKeyWrapper::IsControlKeyPressed(inModifierKeys) == true ||//win 080709
				mDragging == false )//B0347
	{
		//#688 ACursorWrapper::SetCursor(kCursorType_ArrowCopy);
		outIsCopyOperation = true;//#688
	}
	else
	{
		//#688 ACursorWrapper::SetCursor(kCursorType_Arrow);
		outIsCopyOperation = false;//#688
	}
}

//Drag&Drop処理（DragがViewに入った）
void	AView_EditBox::EVTDO_DoDragEnter( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
	AImagePoint	clickImagePoint;
	NVM_GetImagePointFromLocalPoint(inLocalPoint,clickImagePoint);
	GetTextPointFromImagePoint(clickImagePoint,mDragCaretTextPoint);
	if( IsInSelect(clickImagePoint,false) == false )
	{
		//Draw
		XorDragCaret();
		mDragDrawn = true;
	}
}

//Drag&Drop処理（DragがViewから出た）
void	AView_EditBox::EVTDO_DoDragLeave( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
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
void	AView_EditBox::EVTDO_DoDragReceive( const ADragRef inDragRef, const ALocalPoint& inLocalPoint, const AModifierKeys inModifierKeys )
{
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
	
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());//B0079
	
	//URL Drop
	AText	url, title;
	if( AScrapWrapper::GetURLDragData(inDragRef,url,title) == true )
	{
		//DragCaret位置をキャレット位置として設定
		SetCaretTextPoint(clickTextPoint);
		//
		AText	text;
		AIndex	caretIndex = kIndex_Invalid;
		text.SetText(url);
		text.Add(kUChar_LineEnd);
		text.AddText(title);
		TextInput(kEditBoxUndoTag_DragDrop,text);
		if( caretIndex != kIndex_Invalid )
		{
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			textindex += caretIndex;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,clickTextPoint);
			SetCaretTextPoint(clickTextPoint);
		}
		return;
	}
	
	AText	text;
	AScrapWrapper::GetTextDragData(inDragRef,text);//#688 ,ATextEncodingWrapper::GetLegacyTextEncodingFromFont(mFontName));//win
	if( text.GetItemCount() > 0 )//B0348
	{
		//
		if( AKeyWrapper::IsShiftKeyPressed(inModifierKeys) == true )//B0347 ifブロック順序入れ替え（shift優先）
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
			TextInput(kEditBoxUndoTag_DragDrop,text);
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
			//DragCaret位置をキャレット位置として設定
			SetCaretTextPoint(clickTextPoint);
			//#231
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			//
			TextInput(kEditBoxUndoTag_DragDrop,text);
			//
			ATextPoint	ept = GetCaretTextPoint();
			//#231 すでにclickTextPointは正しくない可能性がある（行折り返しありの場合、文字挿入後は、挿入文字列開始位置が前行に移動する）
			ATextPoint	spt;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(textindex,spt);
			SetSelect(/*#231 clickTextPoint*/spt,ept);
		}
		else
		{
			//移動先TextIndexを保存
			ATextIndex	textindex = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(clickTextPoint);
			//
			ATextIndex	spos, epos;
			SPI_GetSelect(spos,epos);
			
			//削除後のTextIndexを計算（移動先が、現在の選択範囲以降の場合、削除するとその分、前に移動しなくてはいけない）
			ATextIndex	newtextindex = textindex;
			if( textindex >= epos )
			{
				newtextindex = textindex - (epos-spos);
			}
			//Undoアクションタグ記録
			GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_DragDrop);
			//削除
			DeleteTextFromDocument();
			//DragCaret位置をキャレット位置として設定
			ATextPoint	textpoint;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(newtextindex,textpoint);
			SetCaretTextPoint(textpoint);
			//
			//テキスト挿入
			InsertTextToDocument(textpoint,text);
			//
			ATextPoint	ept = GetCaretTextPoint();
			//#231 すでにtextpointは正しくない可能性がある（行折り返しありの場合、文字挿入後は、挿入文字列開始位置が前行に移動する）
			ATextPoint	spt;
			GetTextDocumentConst().SPI_GetTextPointFromTextIndex(newtextindex,spt);
			SetSelect(/*#231 textpoint*/spt,ept);
		}
	}
}

/**
ドラッグ中のドラッグ先キャレット描画
*/
void	AView_EditBox::XorDragCaret()
{
	AImagePoint	caretImageStart, caretImageEnd;
	GetImagePointFromTextPoint(mDragCaretTextPoint,caretImageStart);
	caretImageEnd = caretImageStart;
	caretImageEnd.y += GetLineHeightWithMargin();
	ALocalPoint	caretLocalStart, caretLocalEnd;
	NVM_GetLocalPointFromImagePoint(caretImageStart,caretLocalStart);
	NVM_GetLocalPointFromImagePoint(caretImageEnd,caretLocalEnd);
	NVMC_DrawXorCaretLine(caretLocalStart,caretLocalEnd,true,true);//#1034 常にflushする
}

/**
ドラッグ中のワードドラッグ先キャレット描画
*/
void	AView_EditBox::XorWordDragDrop()
{
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

void	AView_EditBox::Refresh()
{
	ALocalRect	localFrameRect;
	NVM_GetLocalViewRect(localFrameRect);
	NVMC_RefreshRect(localFrameRect);
}

//#725
/**
背景色更新（テキスト空の場合に背景色がかわる処理対応）
*/
void	AView_EditBox::UpdateBackgroundColor()
{
	//（自身の背景色はDoDraw()で対応）
	
	//フレームが存在していたら、フレームの背景色設定
	if( NVI_ExistFrameView() == true )
	{
		if( GetTextDocumentConst().SPI_GetTextLength() > 0 )
		{
			//テキスト存在する場合
			NVI_GetFrameView().SPI_SetColor(mBackgroundColor,mFrameColor,mFrameColorDeactive,mTransparency);
		}
		else
		{
			//テキストが空の場合
			NVI_GetFrameView().SPI_SetColor(mBackgroundActiveColorForEmptyState,mFrameColor,mFrameColorDeactive,mTransparency);
		}
		//描画更新
		NVI_GetFrameView().NVI_Refresh();
	}
}

/**
サイズ変更時処理
*/
void	AView_EditBox::NVIDO_SetSize( const ANumber inWidth, const ANumber inHeight )
{
	AdjustScroll();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---テキスト設定・取得

//
/**
テキスト設定
*/
void	AView_EditBox::NVIDO_SetText( const AText& inText )
{
	HideCaret();
	//まずインラインインプットは確定させる（アプリ内のテキストを変更するので、その前にTSM側と同期させる必要有る）
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());
	//
	GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_Typing);
	//
	AText	text;
	text.SetText(inText);
	text.ConvertLineEndToCR();
	GetTextDocument().SPI_DeleteText(0,GetTextDocument().SPI_GetTextLength());
	GetTextDocument().SPI_InsertText(0,text);
	if( mSetSelect00AfterSetText == true )
	{
		SetSelect(0,0);
	}
	else
	{
		SetSelect(text.GetItemCount(),text.GetItemCount());//#242
	}
	SPI_UpdateImageSize();
	AdjustScroll();//#427
	UpdateBackgroundColor();//#798
	Refresh();
}

//#182
/**
テキスト追加
*/
void	AView_EditBox::NVIDO_AddText( const AText& inText )
{
	HideCaret();
	//まずインラインインプットは確定させる（アプリ内のテキストを変更するので、その前にTSM側と同期させる必要有る）
	NVM_GetWindow().NVI_FixInlineInput(NVI_GetControlID());
	//
	GetTextDocument().SPI_RecordUndoActionTag(kEditBoxUndoTag_Typing);
	//
	AText	text;
	text.SetText(inText);
	text.ConvertLineEndToCR();
	GetTextDocument().SPI_InsertText(GetTextDocument().SPI_GetTextLength(),text);
	SPI_UpdateImageSize();
	AdjustScroll();//#427
	Refresh();
}

//#801
/**
現在の選択範囲削除してテキスト追加
*/
void	AView_EditBox::SPI_DeleteAndInsertText( const AText& inText )
{
	DeleteAndInsertTextToDocument(inText);
}

//
/**
テキスト取得
*/
void	AView_EditBox::NVIDO_GetText( AText& outText ) const
{
	GetTextDocumentConst().SPI_GetText(outText);
}

//#182
/**
数値設定
*/
void	AView_EditBox::NVIDO_SetNumber( const ANumber inNumber )
{
	AText	text;
	text.SetNumber(inNumber);
	NVI_SetText(text);
}

//#182
/**
数値取得
*/
ANumber	AView_EditBox::NVIDO_GetNumber() const
{
	AText	text;
	NVI_GetText(text);
	return text.GetNumber();
}

//#182
/**
Float数値設定
*/
void	AView_EditBox::NVIDO_SetFloatNumber( const AFloatNumber inNumber )
{
	AText	text;
	text.SetFormattedCstring("%g",inNumber);
	NVI_SetText(text);
}

//#182
/**
Float数値取得
*/
AFloatNumber	AView_EditBox::NVIDO_GetFloatNumber() const
{
	AText	text;
	NVI_GetText(text);
	return text.GetFloatNumber();
}

#pragma mark ===========================

#pragma mark ---テキストフォント設定・取得
//#182

/**
テキストフォント設定
*/
void	AView_EditBox::NVIDO_SetTextFont( const AText& inFontName, const AFloatNumber inFontSize )
{
	SPI_SetTextDrawProperty(inFontName,inFontSize,mColor,mColorDeactive);
}

//#349
/**
フォントサイズ設定
*/
void	AView_EditBox::SPI_SetTextFontSize( const AFloatNumber inFontSize )
{
	AText	fontname(mFontName);
	SPI_SetTextDrawProperty(fontname,inFontSize,mColor,mColorDeactive);
}

#pragma mark ===========================

#pragma mark ---属性設定

//フォント等のText描画プロパティが変更された場合の処理
void	AView_EditBox::SPI_SetTextDrawProperty( const AText& inFontName, const AFloatNumber inFontSize, const AColor inColor, const AColor inColorDeactive )
{
	mFontName.SetText(inFontName);
	mFontSize = inFontSize;
	/*
#if IMPLEMENTATION_FOR_WINDOWS
	mFontSize *= 0.8;
	if( mFontSize < 7.8 )   mFontSize = 7.8;
#endif
	*/
	mColor = inColor;
	mColorDeactive = inColorDeactive;
	NVMC_SetDefaultTextProperty(inFontName,inFontSize,inColor,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mLineHeight,mLineAscent);
	//#894
	//行数が1行のみの場合、上下方向センター表示するようにImageYマージン／スクロール位置を調整
	UpdateImageYMinMarginFor1LineCentering();
	//
	NVI_Refresh();
	SPI_UpdateScrollBarUnit();
}

//#677
/**
テキスト色設定
*/
void	AView_EditBox::SPI_SetTextColor( const AColor inColor, const AColor inColorDeactive )
{
	mColor = inColor;
	mColorDeactive = inColorDeactive;
	NVMC_SetDefaultTextProperty(mFontName,mFontSize,inColor,kTextStyle_Normal,true);
	NVMC_GetMetricsForDefaultTextProperty(mLineHeight,mLineAscent);
	//#894
	//行数が1行のみの場合、上下方向センター表示するようにImageYマージン／スクロール位置を調整
	UpdateImageYMinMarginFor1LineCentering();
	//
	NVI_Refresh();
}

//#894
/**
行数が1行のみの場合、上下方向センター表示するようにImageYマージン／スクロール位置を調整
*/
void	AView_EditBox::UpdateImageYMinMarginFor1LineCentering()
{
	if( GetTextDocumentConst().SPI_GetLineCount() == 1 && mCenterizeWhen1Line == true )
	{
		//------------------1行のみの場合------------------
		//イメージサイズ更新
		SPI_UpdateImageSize();
		
		//マージン計算
		ALocalRect	localFrameRect = {0};
		NVM_GetLocalViewRect(localFrameRect);
		ANumber	margin = ((localFrameRect.bottom-localFrameRect.top)-GetLineHeightWithMargin())/2;
		if( margin > 0 )
		{
			NVI_SetImageYMargin(margin,0);
		}
		
		//センターになるようにスクロール調整
		AImagePoint	origin = {0,-mImageYMinMargin};
		NVI_ScrollTo(origin);
	}
	else
	{
		//------------------複数行の場合------------------
		//ImageYマージンなし
		NVI_SetImageYMargin(0,0);
	}
}

//#135
/**
Scroll・PageUp/Down単位設定
*/
void	AView_EditBox::SPI_UpdateScrollBarUnit()
{
	ALocalRect	rect;
	NVM_GetLocalViewRect(rect);
	NVI_SetScrollBarUnit(kHScrollBarUnit,GetLineHeightWithMargin(),rect.right-rect.left-kHScrollBarUnit*5,rect.bottom-rect.top-GetLineHeightWithMargin()*5);
}

//背景色等設定
void	AView_EditBox::SPI_SetBackgroundColor( const AColor inBackColor, const AColor inFrameColor, const AColor inFrameColorDeactive )
{
	//
	mBackgroundColor = inBackColor;
	mFrameColor = inFrameColor;
	mFrameColorDeactive = inFrameColorDeactive;
	//
	//#135 SPI_GetFrameView().SPI_SetColor(inBackColor,inFrameColor,inFrameColorDeactive);
	/*#199 dynamic_cast<AView_Frame&>(NVM_GetWindow().NVI_GetViewByControlID(mFrameViewControlID)).SPI_SetColor(inBackColor,inFrameColor,inFrameColorDeactive);
	*/
	NVI_SetFrameViewColor(inBackColor,inFrameColor,inFrameColorDeactive);
}

/**
編集可／不可設定
*/
void	AView_EditBox::SPI_SetEnableEdiit( const ABool inEnable )
{
	mEnableEdit = inEnable;
	if( mEnableEdit == false )
	{
		HideCaret();
	}
}

//#455
/**
行スタイル設定
*/
void	AView_EditBox::SPI_SetLineStyle( const AArray<ATextStyle>& inStyleArray, const AArray<AColor>& inColorArray )
{
	mLineStyleArray_Style.DeleteAll();
	mLineStyleArray_Color.DeleteAll();
	for( AIndex i = 0; i < inStyleArray.GetItemCount(); i++ )
	{
		mLineStyleArray_Style.Add(inStyleArray.Get(i));
		mLineStyleArray_Color.Add(inColorArray.Get(i));
	}
}

#pragma mark ===========================

#pragma mark ---キャレット制御

//#390
/**
コマンド＋矢印キーsimulate
*/
void	AView_EditBox::SPI_ArrowKeyEdge( const AUChar inChar, const ABool inSelect )
{
	ArrowKeyEdge(inChar,inSelect);
}

//キャレットモードかどうかを取得
ABool	AView_EditBox::IsCaretMode() const
{
	return mCaretMode;
}

//
void	AView_EditBox::SetCaretTextPoint( const ATextPoint& inTextPoint )
{
	if( IsCaretMode() == true )
	{
		HideCaret();
	}
	else
	{
		mCaretMode = true;
		Refresh();
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
	
	GetImagePointFromTextPoint(GetCaretTextPoint(),mCaretImagePoint);
	
	//
	SetOldArrowImageX(mCaretImagePoint.x);
	
	if( NVI_IsFocusActive() == true )
	{
		//#688 ShowCaret();
		//Caret表示予約する
		ShowCaretReserve();//win
	}
	
	//#688
	//TextInputClientのselected rangeを消去（InlineInput処理中かどうかの判断は関数内で実施）
	NVMC_ResetSelectedRangeForTextInput();
}

//現在のキャレット描画データ取得
//返り値：キャレットが現在のViewRectに含まれるかどうか
void	AView_EditBox::GetCaretDrawData( ALocalPoint& outStartPoint, ALocalPoint& outEndPoint ) const
{
	AImagePoint	imageSpt = mCaretImagePoint;
	AImagePoint	imageEpt = imageSpt;
	imageEpt.y += mLineHeight-1;
	NVM_GetLocalPointFromImagePoint(imageSpt,outStartPoint);
	NVM_GetLocalPointFromImagePoint(imageEpt,outEndPoint);
}

//R0184
/**
キャレット描画
*/
void	AView_EditBox::DrawXorCaret( const ABool inFlush )//#1034
{
	if( mReadOnly == true )
	{
		return;
	}
	
	
	//fprintf(stderr,"DrawXorCaret()");
	ALocalPoint	spt,ept;
	GetCaretDrawData(spt,ept);
	//キャレット太さ
	spt.y -= 1;
	ept.y += 1;
	//
	NVMC_DrawXorCaretLine(spt,ept,true,inFlush,false,2);//#688
}

//キャレット表示
void	AView_EditBox::ShowCaret()
{
	if( mReadOnly == true )
	{
		return;
	}
	
	//fprintf(stderr,"ShowCaret()");
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
	}
}

//キャレット非表示
void	AView_EditBox::HideCaret()
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
	}
}

/**
Caret表示予約
WindowsでのCaretちらつき対策。（Windowsの場合、ShowCaret()をコールするとその時点で描画されるので、処理途中の箇所でCaret描画される。
実際にはここで描画せず、ShowCaretIfReserved()コール時、または、次回即タイムアウト時に描画する
*/
void	AView_EditBox::ShowCaretReserve()
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
	}
}

/**
Caret表示（Caret表示予約時）
*/
void	AView_EditBox::ShowCaretIfReserved()
{
	if( mCaretTickCount >= 99999 )
	{
		ShowCaret();
	}
}

//キャレット一時非表示
void	AView_EditBox::TempHideCaret()
{
	mTempCaretHideCount++;
	if( mTempCaretHideCount != 1 )   return;
	if( IsCaretMode() == true && NVI_IsFocusActive() == true )
	{
		if( mCaretBlinkStateVisible == true )
		{
			DrawXorCaret(false);//#1034 常にflushしない
		}
	}
}

//キャレット一時非表示解除
void	AView_EditBox::RestoreTempHideCaret()
{
	mTempCaretHideCount--;
	if( mTempCaretHideCount != 0 )   return;
	if( IsCaretMode() == true )
	{
		if( mCaretBlinkStateVisible == true && NVI_IsFocusActive() == true )
		{
			DrawXorCaret(false);//#1034 常にflushしない
		}
	}
}

//キャレットブリンク（キャレットタイマー毎に呼ばれる）
void	AView_EditBox::BlinkCaret()
{
	if( IsCaretMode() == true && NVI_IsFocusActive() == true )
	{
		DrawXorCaret(true);//#1034 常にflushする
		mCaretBlinkStateVisible = !mCaretBlinkStateVisible;
	}
	mCaretTickCount = 0;
}

//
void	AView_EditBox::XorCrossCaret()
{
	ALocalPoint	cspt,cept;
	GetCaretDrawData(cspt,cept);
	ALocalPoint	spt = cspt;
	ALocalPoint	ept = cept;
	ALocalRect	localFrame;
	NVM_GetLocalViewRect(localFrame);
	spt.y = localFrame.top;
	ept.y = localFrame.bottom;
	NVMC_DrawXorCaretLine(spt,ept,true,false,true);//#1034 #1034 常にflushしない
	spt = cspt;
	ept = cept;
	spt.x = localFrame.left;
	ept.x = localFrame.right;
	spt.y = ept.y;
	NVMC_DrawXorCaretLine(spt,ept,true,false,true);//#1034 #1034 常にflushしない
}

#pragma mark ===========================

#pragma mark ---選択制御

void	AView_EditBox::SetSelectTextPoint( const ATextPoint& inTextPoint )
{
	//inTextPointがキャレット位置と同じなら、キャレットとして設定
	if( inTextPoint.x == GetCaretTextPoint().x && inTextPoint.y == GetCaretTextPoint().y )
	{
		SetCaretTextPoint(inTextPoint);
		return;
	}
	
	if( IsCaretMode() == true )
	{
		HideCaret();
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
		OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),pt1,pt2);
	}
	else
	{
		ATextPoint	pt1, pt2;
		OrderTextPoint(GetSelectTextPoint(),inTextPoint,pt1,pt2);
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
		
	}
	//R0108 高速化のため
	GetImagePointFromTextPoint(GetSelectTextPoint(),mSelectImagePoint);
	
	//#688
	//TextInputClientのselected rangeを消去（InlineInput処理中かどうかの判断は関数内で実施）
	NVMC_ResetSelectedRangeForTextInput();
	
	Refresh();
}

//
ABool	AView_EditBox::SPI_GetSelect( ATextPoint& outStartPoint, ATextPoint& outEndPoint ) const
{
	if( IsCaretMode() == false )
	{
		OrderTextPoint(GetCaretTextPoint(),GetSelectTextPoint(),outStartPoint,outEndPoint);
	}
	else
	{
		outStartPoint = outEndPoint = GetCaretTextPoint();
	}
	return IsCaretMode();
}

//
ABool	AView_EditBox::SPI_GetSelect( ATextIndex& outStart, ATextIndex& outEnd ) const
{
	ATextPoint	spt, ept;
	ABool	result = SPI_GetSelect(spt,ept);
	outStart = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(spt);
	outEnd = GetTextDocumentConst().SPI_GetTextIndexFromTextPoint(ept);
	return result;
}

//
void	AView_EditBox::SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint )
{
	ATextPoint	spt, ept;
	SPI_GetSelect(spt,ept);
	if( spt.x == inStartPoint.x && spt.y == inStartPoint.y && ept.x == inEndPoint.x && ept.y == inEndPoint.y )
	{
		return;
	}
	SetCaretTextPoint(inStartPoint);
	if( inStartPoint.x == inEndPoint.x && inStartPoint.y == inEndPoint.y )   return;
	SetSelectTextPoint(inEndPoint);
}
void	AView_EditBox::SetSelect( const AIndex inStart, const AIndex inEnd )
{
	ATextPoint	spt, ept;
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inStart,spt);
	GetTextDocumentConst().SPI_GetTextPointFromTextIndex(inEnd,ept);
	SetSelect(spt,ept);
}
void	AView_EditBox::SPI_SetSelect( const ATextPoint& inStartPoint, const ATextPoint& inEndPoint )
{
	SetSelect(inStartPoint,inEndPoint);
}
void	AView_EditBox::SPI_SetSelect( const AIndex inStart, const AIndex inEnd )
{
	SetSelect(inStart,inEnd);
}

//#135
/**
全選択
*/
void	AView_EditBox::NVIDO_SetSelectAll()
{
	SetSelect(0,GetTextDocumentConst().SPI_GetTextLength());
	//スクロール位置 #135
	AImagePoint	pt = {0,0};
	NVI_ScrollTo(pt);
}

//
void	AView_EditBox::GetSelectedText( AText& outText ) const
{
	outText.DeleteAll();
	if( IsCaretMode() == false )
	{
		ATextPoint	caretPoint, selectPoint;
		GetCaretTextPoint(caretPoint);
		GetSelectTextPoint(selectPoint);
		GetTextDocumentConst().SPI_GetText(caretPoint,selectPoint,outText);
	}
}

//指定TextPointが、選択範囲内にあるかどうか
//inIncludeBoundary: 境界を含めるかどうか
ABool	AView_EditBox::IsInSelect( const AImagePoint& inImagePoint, const ABool inIncludeBoundary )
{
	if( IsCaretMode() == true )   return false;
	ATextPoint	textpoint;
	GetTextPointFromImagePoint(inImagePoint,textpoint);
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

#pragma mark ===========================

#pragma mark ---座標変換

//
void	AView_EditBox::GetImagePointFromTextPoint( const ATextPoint& inTextPoint, AImagePoint& outImagePoint ) 
{
	CTextDrawData	textDrawData(false);
	GetTextDocument().SPI_GetTextDrawData(inTextPoint.y,textDrawData);
	
	outImagePoint.x = NVMC_GetImageXByTextPosition(textDrawData,inTextPoint.x);
	outImagePoint.y = GetImageYByLineIndex(inTextPoint.y);
}

//
void	AView_EditBox::GetTextPointFromImagePoint( const AImagePoint& inImagePoint, ATextPoint& outTextPoint ) 
{
	//テキストY座標計算
	outTextPoint.y = GetLineIndexByImageY(inImagePoint.y);
	//対象行描画データ取得
	CTextDrawData	textDrawData(false);
	GetTextDocument().SPI_GetTextDrawData(outTextPoint.y,textDrawData);
	//
	outTextPoint.x = NVMC_GetTextPositionByImageX(textDrawData,inImagePoint.x);
}

//
AIndex	AView_EditBox::GetLineIndexByImageY( const ANumber inImageY ) const
{
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
}

//
ANumber	AView_EditBox::GetImageYByLineIndex( const AIndex inLineIndex ) const
{
	return inLineIndex * GetLineHeightWithMargin();
}

//
void	AView_EditBox::GetLineImageRect( const AIndex inLineIndex, AImageRect& outLineImageRect ) const
{
	outLineImageRect.left	= 0;
	outLineImageRect.right	= NVM_GetImageWidth();
	outLineImageRect.top	= inLineIndex*GetLineHeightWithMargin();
	outLineImageRect.bottom	= (inLineIndex+1)*GetLineHeightWithMargin();
}

//
void	AView_EditBox::GetLineImageRect( const AIndex inStartLineIndex, const AIndex inEndLineIndex, AImageRect& outLineImageRect ) const
{
	outLineImageRect.left	= 0;
	outLineImageRect.right	= NVM_GetImageWidth();
	outLineImageRect.top	= inStartLineIndex*GetLineHeightWithMargin();
	outLineImageRect.bottom	= inEndLineIndex*GetLineHeightWithMargin();
}

//行の高さ取得
ANumber	AView_EditBox::GetLineHeightWithMargin() const
{
	return mLineHeight;
}
ANumber	AView_EditBox::GetLineHeightWithoutMargin() const
{
	return mLineHeight;
}

#pragma mark ===========================

#pragma mark ---スクロール

//
void	AView_EditBox::NVIDO_ScrollPreProcess( const ANumber inDeltaX, const ANumber inDeltaY, 
											   const ABool inRedraw, const ABool inConstrainToImageSize,
											   const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//TempHideCaret();
}

//
void	AView_EditBox::NVIDO_ScrollPostProcess( const ANumber inDeltaX, const ANumber inDeltaY,
												const ABool inRedraw, const ABool inConstrainToImageSize,
												const AScrollTrigger inScrollTrigger )//#138 #1031
{
	//RestoreTempHideCaret();
}

//
void	AView_EditBox::AdjustScroll( const ATextPoint& inTextPoint, const AIndex inOffset )
{
	//現在のフレームを取得
	AImageRect	oldFrameImageRect;
	NVM_GetImageViewRect(oldFrameImageRect);
	AImagePoint	newTopLeft;
	newTopLeft.x = oldFrameImageRect.left;
	newTopLeft.y = oldFrameImageRect.top;
	
	//イメージサイズが空のときは、常に(0,0)にスクロール（edit box生成直後等に、異常位置にスクロールされるのを防ぐため）
	if( oldFrameImageRect.bottom - oldFrameImageRect.top <= 0 || oldFrameImageRect.right - oldFrameImageRect.left <= 0 )
	{
		AImagePoint	pt = {0,0};
		NVI_ScrollTo(pt);
		return;
	}
	
	//フレームの開始行、終了行を取得
	AIndex	startLineIndex = GetLineIndexByImageY(oldFrameImageRect.top+GetLineHeightWithMargin()-1);
	AIndex	endLineIndex = GetLineIndexByImageY(oldFrameImageRect.bottom);
	
	//Y方向補正
	ATextPoint	pt = inTextPoint;
	if( pt.y < startLineIndex )
	{
		AIndex	lineIndex = pt.y-inOffset;//#137
		if( lineIndex < 0 )   lineIndex = 0;//#137
		newTopLeft.y = GetImageYByLineIndex(lineIndex);
	}
	else if( pt.y >= endLineIndex )
	{
		AIndex	lineIndex = pt.y-(endLineIndex-startLineIndex)+inOffset;//#137
		if( lineIndex >= GetTextDocument().SPI_GetLineCount() )   lineIndex = GetTextDocument().SPI_GetLineCount()-1;//#137
		newTopLeft.y = GetImageYByLineIndex(lineIndex);
	}
	
	//X方向補正
	AImagePoint	imagePoint;
	GetImagePointFromTextPoint(pt,imagePoint);
	if( imagePoint.x > oldFrameImageRect.right )
	{
		ANumber	delta = (imagePoint.x-oldFrameImageRect.right)/mAdjustScrollXOffsetRight+1;//#261
		delta *= mAdjustScrollXOffsetRight;//#261
		newTopLeft.x = oldFrameImageRect.left + delta;
	}
	else if( imagePoint.x < oldFrameImageRect.left )
	{
		ANumber	delta = (oldFrameImageRect.left-imagePoint.x)/mAdjustScrollXOffsetLeft+1;//#261
		delta *= mAdjustScrollXOffsetLeft;//#261
		newTopLeft.x = oldFrameImageRect.left -= delta;
	}
	if( newTopLeft.x < 0 )   newTopLeft.x = 0;
	
	//スクロール
	NVI_ScrollTo(newTopLeft);
}

//#427
/**
スクロール位置を現在の選択範囲が見えるように調整
*/
void	AView_EditBox::AdjustScroll()
{
	ATextPoint	spt = {0,0}, ept = {0,0};
	SPI_GetSelect(spt,ept);
	AdjustScroll(spt);
}

//
void	AView_EditBox::SPI_UpdateImageSize()
{
	ANumber	width = 10000;
	ANumber	height = GetTextDocument().SPI_GetLineCount() * GetLineHeightWithMargin();
	NVM_SetImageSize(width,height);
}

#pragma mark ===========================

#pragma mark ---フォーカス
//#137

void	AView_EditBox::EVTDO_DoViewFocusActivated()
{
	if( IsCaretMode() == true )
	{
		ShowCaret();
	}
	else
	{
		NVI_Refresh();
	}
}

void	AView_EditBox::EVTDO_DoViewFocusDeactivated()
{
	if( IsCaretMode() == true )
	{
		HideCaret();
	}
	else
	{
		NVI_Refresh();
	}
	//フォーカス解除時は選択解除 #135
	SetSelect(GetCaretTextPoint(),GetCaretTextPoint());
	//#391
	//ここでValueChangedを実行するとキーバインドでControlキーをOff/Onしたときに文字列が削除される問題が発生した→様子見
	NVM_GetWindow().EVT_ValueChanged(NVI_GetControlID());
	//編集モード終了
	if( mListViewID != kControlID_Invalid )
	{
		NVM_GetWindow().NVI_GetListView(mListViewID).SPI_EndEditMode();
	}
}

//#135
/**
Undo情報をクリア
*/
void	AView_EditBox::NVIDO_ClearUndoInfo()
{
	GetTextDocument().SPI_ClearUndoInfo();
}

#pragma mark ===========================

#pragma mark ---サーチキャンセルボタン

/**
検索キャンセルボタンrect取得
*/
void	AView_EditBox::GetSearchCancelButtonRect( ALocalRect& outRect ) const
{
	const ANumber	kCancelButtonOffset = 16;
	NVM_GetLocalViewRect(outRect);
	outRect.left = outRect.right - kCancelButtonOffset;
}

/**
指定ポイントが検索キャンセルボタンrect内かどうかを取得
*/
ABool	AView_EditBox::MouseIsInSearchCancelButtonRect( const ALocalPoint inMousePoint ) const
{
	//検索キャンセルボタン非表示なら常にfalse
	if( IsSearchCancelButtonDisplayed() == false )
	{
		return false;
	}
	//検索キャンセルボタンrect内かどうかを判定
	ALocalRect	rect = {0};
	GetSearchCancelButtonRect(rect);
	if( inMousePoint.x >= rect.left && inMousePoint.x <= rect.right &&
				inMousePoint.y >= rect.top && inMousePoint.y <= rect.bottom )
	{
		return true;
	}
	return false;
}

/**
検索キャンセルボタン表示中かどうかを判定
*/
ABool	AView_EditBox::IsSearchCancelButtonDisplayed() const
{
	if( mEditBoxType == kEditBoxType_FilterBox && GetTextDocumentConst().SPI_GetTextLength() > 0 )
	{
		return true;
	}
	return false;
}

#pragma mark ===========================

#pragma mark ---情報取得

//#138
/**
行の最大幅を取得する。

@return 現在のテキストの各行の表示幅のうち、最大のもの
*/
ANumber	AView_EditBox::SPI_GetMaxDisplayWidth() const
{
	ANumber	width = 0;
	for( AIndex lineIndex = 0; lineIndex < GetTextDocumentConst().SPI_GetLineCount(); lineIndex++ )
	{
		CTextDrawData	textDrawData(false);
		GetTextDocumentConst().SPI_GetTextDrawData(lineIndex,textDrawData);
		ANumber	w = static_cast<ANumber>(NVMC_GetDrawTextWidth(textDrawData));
		if( w > width )   width = w;
	}
	return width;
}

#pragma mark ===========================

#pragma mark ---ListView連携

void	AView_EditBox::SPI_SetListViewID( const AControlID inID )
{
	mListViewID = inID;
}

