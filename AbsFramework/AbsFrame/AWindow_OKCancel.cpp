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

AWindow_OKCancel

*/

#include "stdafx.h"

#include "AWindow_OKCancel.h"

const AControlID	kOKButton		= 10001;
const AControlID	kCancelButton	= 10002;
const AControlID	kMessage1		= 10003;
const AControlID	kMessage2		= 10004;

#pragma mark ===========================
#pragma mark [クラス]AWindow_OKCancel
#pragma mark ===========================
//OKCancel子ウインドウ

/**
コンストラクタ
*/
AWindow_OKCancel::AWindow_OKCancel() :
		AWindow(), mParentWindowID(kObjectID_Invalid), mOKButtonVirtualControlID(kControlID_Invalid)
{
}

/**
デストラクタ
*/
AWindow_OKCancel::~AWindow_OKCancel()
{
}

/**
ウインドウ生成
*/
void	AWindow_OKCancel::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);
	NVM_CreateWindow("AbsFrameworkCommonResource/SheetOKCancel");
	NVI_SetDefaultOKButton(kCancelButton);
	NVI_SetDefaultCancelButton(kCancelButton);
	NVI_RegisterToFocusGroup(kCancelButton,true);//#353
	NVI_RegisterToFocusGroup(kOKButton,true);//#353
}

/**
ウインドウが閉じられた
*/
void	AWindow_OKCancel::NVIDO_Close()
{
	AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID).NVI_DoChildWindowClosed_OKCancel();
}

/**
メニュー選択時処理
*/
ABool	AWindow_OKCancel::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
{
	if( inMenuItemID == kMenuItemID_Close )
	{
		NVI_Close();
		return true;
	}
	return false;
}

/**
メニュー更新処理
*/
void	AWindow_OKCancel::EVTDO_UpdateMenu()
{
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,true);
}

/**
設定＋ウインドウ表示
*/
void	AWindow_OKCancel::SPNVI_Show( const AWindowID inParentWindowID, 
		const AText& inMessage1, const AText& inMessage2, const AText& inOKButtonMessage, 
		const AControlID inOKButtonVirtualControlID )
{
	mParentWindowID = inParentWindowID;
	mOKButtonVirtualControlID = inOKButtonVirtualControlID;
	//
	NVI_SetWindowStyleToSheet(mParentWindowID);
	NVI_SetControlText(kMessage1,inMessage1);
	NVI_SetControlText(kMessage2,inMessage2);
	if( inOKButtonMessage.GetItemCount() > 0 )
	{
		NVI_SetControlText(kOKButton,inOKButtonMessage);
	}
	NVI_Show();
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_OKCancel::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kOKButton:
		{
			NVI_Close();//#427 closeタイミングをEVT_Clicked前に変更
			AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID).EVT_Clicked(mOKButtonVirtualControlID,inModifierKeys);
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
void	AWindow_OKCancel::EVTDO_DoCloseButton()
{
	NVI_Close();
}

