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

AWindow_SaveCancelDiscard

*/

#include "stdafx.h"

#include "AWindow_SaveCancelDiscard.h"

const AControlID	kSaveButton		= 10001;
const AControlID	kCancelButton	= 10002;
const AControlID	kMessage1		= 10003;
const AControlID	kMessage2		= 10004;
const AControlID	kDiscardButton	= 10005;

#pragma mark ===========================
#pragma mark [クラス]AWindow_SaveCancelDiscard
#pragma mark ===========================
//SaveCancelDiscard子ウインドウ

/**
コンストラクタ
*/
AWindow_SaveCancelDiscard::AWindow_SaveCancelDiscard() :
		AWindow(), mParentWindowID(kObjectID_Invalid), 
		mAskSaveChangesDocumentID(kObjectID_Invalid),
		mModal(false)
{
}

/**
デストラクタ
*/
AWindow_SaveCancelDiscard::~AWindow_SaveCancelDiscard()
{
}

/**
ウインドウ生成
*/
void	AWindow_SaveCancelDiscard::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);
	NVM_CreateWindow("AbsFrameworkCommonResource/SheetSaveCancelDiscard");
	NVI_SetDefaultCancelButton(kCancelButton);
	NVI_RegisterToFocusGroup(kDiscardButton,true);//#353
	NVI_RegisterToFocusGroup(kCancelButton,true);//#353
	NVI_RegisterToFocusGroup(kSaveButton,true);//#353
}

/**
ウインドウが閉じられた
*/
void	AWindow_SaveCancelDiscard::NVIDO_Close()
{
	AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID).NVI_DoChildWindowClosed_SaveCancelDiscard();
}

/**
メニュー選択時処理
*/
ABool	AWindow_SaveCancelDiscard::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	return false;
}

/**
メニュー更新処理
*/
void	AWindow_SaveCancelDiscard::EVTDO_UpdateMenu()
{
}

/**
設定＋ウインドウ表示
*/
void	AWindow_SaveCancelDiscard::SPNVI_Show( const ADocumentID inAskSaveChangesDocumentID, const AWindowID inParentWindowID, 
											   const AText& inMessage1, const AText& inMessage2, const ABool inModal )
{
	//対象ドキュメント記憶
	mAskSaveChangesDocumentID = inAskSaveChangesDocumentID;
	//親ドキュメント記憶
	mParentWindowID = inParentWindowID;
	//モーダルかどうか記憶
	mModal = inModal;
	//モーダル以外なら、シートとして設定
	if( mModal == false )
	{
		NVI_SetWindowStyleToSheet(mParentWindowID);
	}
	//メッセージ設定
	NVI_SetControlText(kMessage1,inMessage1);
	NVI_SetControlText(kMessage2,inMessage2);
	
	//モーダルならここで、ボタンが押されるまでブロック
	if( mModal == true )
	{
		//モーダル開始（モーダル終了までブロックする）（NVI_RunModalWindow()内でウインドウ表示される）
		NVI_RunModalWindow();
	}
	//モーダルでなければ、ウインドウ表示だけして戻る
	else
	{
		//ウインドウ表示
		NVI_Show();
	}
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_SaveCancelDiscard::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	AAskSaveChangesResult	askSaveChanges = kAskSaveChangesResult_Cancel;
	switch(inID)
	{
	  case kSaveButton:
		{
			askSaveChanges = kAskSaveChangesResult_Save;
			NVI_Close();
			result = true;
			break;
		}
	  case kDiscardButton:
		{
			askSaveChanges = kAskSaveChangesResult_DontSave;
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
	//親ウインドウのコールバック実行
	AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID).EVT_AskSaveChangesReply(mAskSaveChangesDocumentID,askSaveChanges);
	AApplication::GetApplication().NVI_UpdateMenu();
	
	//モーダルならモーダル終了
	if( mModal == true )
	{
		NVI_StopModalSession();
	}
	return result;
}

/**
ウインドウ閉じるボタン
*/
void	AWindow_SaveCancelDiscard::EVTDO_DoCloseButton()
{
}

