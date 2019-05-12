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

AbsFrame.h

*/

#pragma once

typedef ANumber	ADocumentType;
typedef ANumber	AWindowType;
typedef ANumber	AViewType;

const ADocumentType	kDocumentType_Invalid = -1;
const AWindowType	kWindowType_Invalid = -1;
const AViewType		kViewType_Invalid = -1;

typedef AObjectID	ADocumentID;
typedef AObjectID	AWindowID;
typedef AObjectID	AViewID;//#199

enum AModificationType
{
	kModificationType_ValueChanged = 0,
	kModificationType_RowAdded,
	kModificationType_RowDeleted,
	kModificationType_TransactionCompleted_DeleteRows,
	kModificationType_TransactionCompleted_AddNew,
	kModificationType_TransactionCompleted_MoveRow
};

//const AIconID	kIconID_Abs_RowMoveUp			= 1;
//const AIconID	kIconID_Abs_RowMoveDown			= 2;
//const AIconID	kIconID_Abs_RowMoveTop			= 3;
//const AIconID	kIconID_Abs_RowMoveBottom		= 4;
//#1012 const AIconID	kIconID_CheckBoxOn				= 5;//win 080618
//#1012 const AIconID	kIconID_CheckBoxOff				= 6;//win 080618
//const AIconID	kIconID_Abs_MenuTriangle		= 7;//#232
//const AIconID	kIconID_Abs_Document			= 8;//win
//const AIconID	kIconID_Abs_Folder				= 9;//win

//#1012
const AImageID	kImageID_iconChecked = 10;
const AImageID	kImageID_iconUnchecked = 11;

//win 080618
const ADocumentType	kDocumentType_EditBox 		= 9000;

/**
AbsFrame内WindowType
*/
const AWindowType	kWindowType_VSeparatorOverlay	= 1;
const AWindowType	kWindowType_HSeparatorOverlay	= 2;
const AWindowType	kWindowType_OK					= 3;//#291
const AWindowType	kWindowType_OKCancel			= 4;//#291
const AWindowType	kWindowType_OKCancelDiscard		= 5;//#291
const AWindowType	kWindowType_ModalAlert			= 6;//#291
const AWindowType	kWindowType_ButtonOverlay		= 7;//win
const AWindowType	kWindowType_ToolTip				= 8;//#688
const AWindowType	kWindowType_SaveCancelDiscard	= 9;//#1034

//#199
/**
AbsFrame内ViewType
*/
const AViewType		kViewType_List				= 5001;
const AViewType		kViewType_EditBox			= 5002;
const AViewType		kViewType_Frame				= 5003;
const AViewType		kViewType_ListFrame			= 5004;
const AViewType		kViewType_Button			= 5005;
const AViewType		kViewType_VSeparator		= 5006;
const AViewType		kViewType_HSeparator		= 5007;
const AViewType		kViewType_Plain				= 5008;//#634
const AViewType		kViewType_SizeBox			= 5009;//#725
const AViewType		kViewType_TextPanel			= 5010;//#725

/**
InternalEvent
*/
const AInternalEventType	kInternalEvent_Curl				= 50;//#427
const AInternalEventType	kInternalEvent_FileChangedNotification	= 51;//#1422

//#232
typedef ADataID		ATextArrayMenuID;

//#232
typedef AIndex		AContextMenuID;
const AContextMenuID	kContextMenuID_Invalid 				= kIndex_Invalid;
const AContextMenuID	kContextMenuID_ButtonView			= 1;
const AContextMenuID	kContextMenuID_EditBox				= 2;//#135

//#232
#if IMPLEMENTATION_FOR_MACOSX
const AScrapType	kScrapType_ButtonView = 129;//#688 'BtVw';
const AScrapType	kScrapType_ListViewItem = 130;//#688 'LVIt';
#endif
#if IMPLEMENTATION_FOR_WINDOWS
const AScrapType	kScrapType_ButtonView = CF_PRIVATEFIRST+1;
const AScrapType	kScrapType_ListViewItem = CF_PRIVATEFIRST+2;
//+0x3FまでをAbsFrameに割り当て。+0x40～をアプリに割り当て
#endif

//#427
typedef AIndex ACurlTransactionType;
const ACurlTransactionType	kCurlTransactionType_None = 0;

//#92
/**
表示位置
*/
enum ADisplayPosition
{
	kDisplayPosition_None = 0,
	kDisplayPosition_MiddleBottom
};

//#212
/**
セパレータ線表示位置
*/
enum ASeparatorLinePosition
{
	kSeparatorLinePosition_None = -1,
	kSeparatorLinePosition_Middle = 0,
	kSeparatorLinePosition_Left,
	kSeparatorLinePosition_Right,
	kSeparatorLinePosition_Top,
	kSeparatorLinePosition_Bottom
};

//#379
/**
非同期プロセス実行コマンドタイプ
*/
typedef ANumber	AGeneralAsyncCommandType;

//#473
/**
TextEncoding Fallbackタイプ
*/
enum ATextEncodingFallbackType
{
	kTextEncodingFallbackType_None = 0,
	kTextEncodingFallbackType_Normal,
	kTextEncodingFallbackType_Lossless
};

//#643
enum AHelpTagSide
{
	kHelpTagSide_Default = 0,
	kHelpTagSide_Right,
	kHelpTagSide_Top
};

/**
保存／保存しない／キャンセル確認ダイアログの結果
*/
enum AAskSaveChangesResult
{
	kAskSaveChangesResult_None = 0,
	kAskSaveChangesResult_Save,
	kAskSaveChangesResult_DontSave,
	kAskSaveChangesResult_Cancel
};

//#199
/**
Window派生クラスdynamic_cast取得
*/
#define MACRO_RETURN_WINDOW_DYNAMIC_CAST(inWindowClass,inWindowType,inObjectID)	{ AWindow& window = AApplication::GetApplication().NVI_GetWindowByID(inObjectID); if( window.NVI_GetWindowType() != (inWindowType) )   _ACThrow("Window dynamic_cast type error",NULL); return dynamic_cast<inWindowClass&>(window);}
#define MACRO_RETURN_CONSTWINDOW_DYNAMIC_CAST(inWindowClass,inWindowType,inObjectID)	{ const AWindow& window = AApplication::GetApplication().NVI_GetWindowConstByID(inObjectID); if( window.NVI_GetWindowType() != (inWindowType) )   _ACThrow("Window dynamic_cast type error",NULL); return dynamic_cast<const inWindowClass&>(window);}

/**
Document派生クラスdynamic_cast取得
*/
#define MACRO_RETURN_DOCUMENT_DYNAMIC_CAST(inDocumentClass,inDocumentType,inObjectID) { ADocument& document = AApplication::GetApplication().NVI_GetDocumentByID(inObjectID); if( document.NVI_GetDocumentType() != (inDocumentType) )   _ACThrow("Document dynamic_cast type error",NULL); return dynamic_cast<inDocumentClass&>(document); }
#define MACRO_RETURN_CONSTDOCUMENT_DYNAMIC_CAST(inDocumentClass,inDocumentType,inObjectID) { const ADocument& document = AApplication::GetApplication().NVI_GetDocumentConstByID(inObjectID); if( document.NVI_GetDocumentType() != (inDocumentType) )   _ACThrow("Document dynamic_cast type error",NULL); return dynamic_cast<const inDocumentClass&>(document); }

/**
View派生クラスdynamic_cast取得（ViewIDから取得・全てのクラスから使用可能）
*/
#define MACRO_RETURN_VIEW_DYNAMIC_CAST_VIEWID(inViewClass,inViewType,inObjectID) { AView& view = AApplication::GetApplication().NVI_GetViewByID(inObjectID); if( view.NVI_GetViewType() != (inViewType) )   _ACThrow("View dynamic_cast type error",NULL); return dynamic_cast<inViewClass&>(view); }
#define MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_VIEWID(inViewClass,inViewType,inObjectID) { const AView& view = AApplication::GetApplication().NVI_GetViewConstByID(inObjectID); if( view.NVI_GetViewType() != (inViewType) )   _ACThrow("View dynamic_cast type error",NULL); return dynamic_cast<const inViewClass&>(view); }

/**
View派生クラスdynamic_cast取得（ControlIDから取得・AWindow派生クラスからのみ使用可能）
*/
#define MACRO_RETURN_VIEW_DYNAMIC_CAST_CONTROLID(inViewClass,inViewType,inControlID) { AView& view = NVI_GetViewByControlID(inControlID); if( view.NVI_GetViewType() != (inViewType) )   _ACThrow("View dynamic_cast type error",NULL); return dynamic_cast<inViewClass&>(view); }
#define MACRO_RETURN_CONSTVIEW_DYNAMIC_CAST_CONTROLID(inViewClass,inViewType,inControlID) { const AView& view = NVI_GetViewConstByControlID(inControlID); if( view.NVI_GetViewType() != (inViewType) )   _ACThrow("View dynamic_cast type error",NULL); return dynamic_cast<const inViewClass&>(view); }


