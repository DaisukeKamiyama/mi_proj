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

AWindow_OK

*/

#include "stdafx.h"

#include "AWindow_OK.h"

const AControlID	kOKButton		= 10001;
const AControlID	kMessage1		= 10003;
const AControlID	kMessage2		= 10004;

#pragma mark ===========================
#pragma mark [クラス]AWindow_OK
#pragma mark ===========================
//OKCancel子ウインドウ

/**
コンストラクタ
*/
AWindow_OK::AWindow_OK() :
		AWindow(), mParentWindowID(kObjectID_Invalid), mLongDialog(false)
{
}

/**
デストラクタ
*/
AWindow_OK::~AWindow_OK()
{
}

/**
ウインドウ生成
*/
void	AWindow_OK::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);
	if( mLongDialog == false )
	{
		NVM_CreateWindow("AbsFrameworkCommonResource/SheetOK");
	}
	else
	{
		NVM_CreateWindow("AbsFrameworkCommonResource/SheetOKLong");
	}
	NVI_SetDefaultOKButton(kOKButton);
	NVI_SetDefaultCancelButton(kOKButton);
	NVI_RegisterToFocusGroup(kOKButton,true);//#353
}

/**
ウインドウが閉じられた
*/
void	AWindow_OK::NVIDO_Close()
{
	AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID).NVI_DoChildWindowClosed_OK();
}

/**
設定＋ウインドウ表示
*/
void	AWindow_OK::SPNVI_CreateAndShow( const AWindowID inParentWindowID, 
		const AText& inMessage1, const AText& inMessage2, const ABool inLongDialog )
{
	mParentWindowID = inParentWindowID;
	mLongDialog = inLongDialog;
	//
	NVI_Create(kObjectID_Invalid);
	//
	NVI_SetWindowStyleToSheet(mParentWindowID);
	NVI_SetControlText(kMessage1,inMessage1);
	NVI_SetControlText(kMessage2,inMessage2);
	NVI_Show();
}

/**
コントロールのクリック時のコールバック
*/
ABool	AWindow_OK::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kOKButton:
		{
			NVI_Close();
			result = true;
			break;
		}
	}
	return result;
}

/**
メニュー選択時処理
*/
ABool	AWindow_OK::EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
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
void	AWindow_OK::EVTDO_UpdateMenu()
{
	AApplication::GetApplication().NVI_GetMenuManager().SetEnableMenuItem(kMenuItemID_Close,true);
}

//win
/**
ウインドウ閉じるボタン
*/
void	AWindow_OK::EVTDO_DoCloseButton()
{
	NVI_Close();
}

//#427
/**
OKボタンのEnable/Disable制御
*/
void	AWindow_OK::SPI_SetEnableOKButton( const ABool inEnable )
{
	NVI_SetControlEnable(kOKButton,inEnable);
}

//#427
/**
テキスト設定
*/
void	AWindow_OK::SPI_SetText( const AText& inMessage1, const AText& inMessage2 )
{
	NVI_SetControlText(kMessage1,inMessage1);
	NVI_SetControlText(kMessage2,inMessage2);
}

