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

AWindow_NewFTPFile

*/

#include "stdafx.h"

#include "AWindow_NewFTPFile.h"
#include "AApp.h"
//#include "AUtil.h"
#include "AView_Text.h"

const AControlID	kOK = 1;
const AControlID	kCancel = 2;
const AControlID	kName = 200;
const AControlID	kPath = 201;

#pragma mark ===========================
#pragma mark [クラス]AWindow_NewFTPFile
#pragma mark ===========================
//

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AWindow_NewFTPFile::AWindow_NewFTPFile():AWindow(/*#175NULL*/)
{
	//#725 タイマー対象にする
	NVI_AddToTimerActionWindowArray();
}
//デストラクタ
AWindow_NewFTPFile::~AWindow_NewFTPFile()
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//コントロールのクリック時のコールバック
ABool	AWindow_NewFTPFile::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kOK:
		{
			AText	ftpurl;
			ftpurl.SetText(mFTPFolderURL);
			AText	text;
			NVI_GetControlText(kName,text);
			ftpurl.AddText(text);
			GetApp().SPI_RequestOpenFileToRemoteConnection(ftpurl,true);//#361
			//
			NVI_StopModalSession();
			//NVI_Close();
			break;
		}
	  case kCancel:
		{
			NVI_StopModalSession();
			//NVI_Close();
			break;
		}
	}
	return result;
}

void	AWindow_NewFTPFile::EVTDO_TextInputted( const AControlID inID )
{
	NVM_UpdateControlStatus();
}

/**
ウインドウ閉じるボタン
*/
void	AWindow_NewFTPFile::EVTDO_DoCloseButton()
{
	NVI_StopModalSession();
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_NewFTPFile::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/NewFTPFile");
	NVI_CreateEditBoxView(kName);
	NVI_SetDefaultOKButton(kOK);
	
	NVI_RegisterToFocusGroup(kCancel,true);//#353
	NVI_RegisterToFocusGroup(kOK,true);//#353
}

void	AWindow_NewFTPFile::SPI_SetFolder( const AText& inText )
{
	mFTPFolderURL.SetText(inText);
	mFTPFolderURL.AddPathDelimiter(kUChar_Slash);//#361
	NVI_SetControlText(kPath,inText);
}

void	AWindow_NewFTPFile::NVMDO_UpdateControlStatus()
{
	AText	text;
	NVI_GetControlText(kName,text);
	NVI_SetControlEnable(kOK,(text.GetItemCount() > 0));
}

void	AWindow_NewFTPFile::NVIDO_Hide()
{
}

void	AWindow_NewFTPFile::NVIDO_Show()
{
}




