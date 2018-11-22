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

AWindow_ChangeToolButton

*/

#include "stdafx.h"

#include "AWindow_ChangeToolButton.h"
#include "AApp.h"

//コントロールID
const AControlID	kToolName				= 5200;
const AControlID	kOK						= 5001;
const AControlID	kCancel					= 5002;

const ANumber	kButtonWidth = 24;//win
const ANumber	kButtonHeight = 24;//win

#pragma mark ===========================
#pragma mark [クラス]AWindow_ChangeToolButton
#pragma mark ===========================
//環境設定ウインドウ

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_ChangeToolButton::AWindow_ChangeToolButton( const AWindowID inParentWindowID ) 
		: AWindow(),mParentWindowID(inParentWindowID),mModeIndex(kStandardModeIndex)
{
}
//デストラクタ
AWindow_ChangeToolButton::~AWindow_ChangeToolButton()
{
}

#pragma mark ===========================

#pragma mark <関連オブジェクト取得>

#pragma mark ===========================

/**
親Window取得
*/
AWindow&	AWindow_ChangeToolButton::GetParentWindow()
{
	return AApplication::GetApplication().NVI_GetWindowByID(mParentWindowID);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
*/
ABool	AWindow_ChangeToolButton::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kOK:
		{
			//ツールフォルダ取得
			AFileAcc	toolbarFolder;
			GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(toolbarFolder);
			//UIからファイル名取得
			AText	filename;
			NVI_GetControlText(kToolName,filename);
			//ツールバー項目名称変更
			AFileAcc	newfile;
			GetApp().SPI_GetModePrefDB(mModeIndex).SetToolbarName(mFile,filename,newfile);
			//閉じる
			NVI_Close();
			break;
		}
	  case kCancel:
		{
			NVI_Close();
			break;
		}
	}
	return result;
}

/**
*/
void	AWindow_ChangeToolButton::EVTDO_TextInputted( const AControlID inID )
{
	switch(inID)
	{
	  case kToolName:
		{
			NVM_UpdateControlStatus();
			break;
		}
	}
}

/**
ウインドウ閉じるボタン
*/
void	AWindow_ChangeToolButton::EVTDO_DoCloseButton()
{
	NVI_Close();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void AWindow_ChangeToolButton::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVI_SetFixTitle(true);
	NVM_CreateWindow("main/ChangeToolButton");
	
	NVI_RegisterToFocusGroup(kToolName);
	NVI_SetWindowStyleToSheet(GetParentWindow().GetObjectID());
	NVI_SetDefaultOKButton(kOK);
	NVI_SetDefaultCancelButton(kCancel);
}

/**
*/
void	AWindow_ChangeToolButton::SPI_SetMode( const AModeIndex inModeIndex, const AFileAcc& inFile )
{
	//
	mModeIndex = inModeIndex;
	mFile = inFile;
	//ファイル名取得
	AText	title;
	mFile.GetFilename(title);
	//初期ツール名をUIに設定
	NVI_SetControlText(kToolName,title);
	//表示更新
	NVM_UpdateControlStatus();
}

#pragma mark ===========================

#pragma mark <汎化メソッド特化部分>

#pragma mark ===========================

//コントロール状態（Enable/Disable等）を更新
//NVI_Update(), EVT_Clicked(), EVT_ValueChanged(), EVT_WindowActivated()からコールされる
//クリックやアクティベートでコールされるので、あまり重い処理をNVMDO_UpdateControlStatus()には入れないこと。
void	AWindow_ChangeToolButton::NVMDO_UpdateControlStatus()
{
	AText	filename;
	NVI_GetControlText(kToolName,filename);
	if( filename.GetItemCount() == 0 )
	{
		NVI_SetControlEnable(kOK,false);
	}
	else
	{
		AFileAcc	toolbarFolder;
		GetApp().SPI_GetModePrefDB(mModeIndex).GetToolbarFolder(toolbarFolder);
		AFileAcc	file;
		file.SpecifyChild(toolbarFolder,filename);
		NVI_SetControlEnable(kOK,(file.Exist()==false));
	}
}

