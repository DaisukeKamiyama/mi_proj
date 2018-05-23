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

AWindow_FTPProgress

*/

#include "stdafx.h"

#include "AWindow_FTPProgress.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_FTPProgress
#pragma mark ===========================
//FTPプログレスウインドウ
const AControlID		kProgressText		= 101;
const AControlID		kProgressBar		= 102;
const AControlID		kAbortButton		= 103;
const AControlID		kTitleText			= 104;

#pragma mark --- コンストラクタ／デストラクタ

//コンストラクタ
AWindow_FTPProgress::AWindow_FTPProgress():AWindow(/*#175NULL*/), mErrorMode(false)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kFTPProgressPosition);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//
ABool	AWindow_FTPProgress::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kAbortButton:
		{
			if( mErrorMode == true )
			{
				NVI_Hide();
			}
			else
			{
				GetApp().SPI_GetFTPConnection().NVI_AbortThread();
			}
			break;
		}
	}
	return result;
}


#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_FTPProgress::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ生成
	NVM_CreateWindow("main/FTPProgress");
	
	//中止ボタンの設定
	AText	text;
	text.SetLocalizedText("FTPProgress_Cancel");
	NVI_SetControlText(kAbortButton,text);
	NVI_SetControlEnable(kAbortButton,mEnableCancel);
	
	//エラー表示モード
	mErrorMode = false;
}

//ウインドウ表示
void	AWindow_FTPProgress::SPNVI_Show( const ABool inEnableCancel )
{
	mEnableCancel = inEnableCancel;
	
	//中止ボタンの設定
	AText	text;
	text.SetLocalizedText("FTPProgress_Cancel");
	NVI_SetControlText(kAbortButton,text);
	NVI_SetControlEnable(kAbortButton,mEnableCancel);
	
	NVI_Show();
}

#pragma mark ===========================

#pragma mark ---プログレス表示

void	AWindow_FTPProgress::SPI_SetTitle( const AText& inText )
{
	//B0000 if( NVI_IsWindowVisible() == false )   return;
	if( NVI_IsWindowCreated() == false )   return;
	NVI_SetControlText(kTitleText,inText);
}

void	AWindow_FTPProgress::SPI_SetProgress( const ANumber inPercent )
{
	//B0000 if( NVI_IsWindowVisible() == false )   return;
	if( NVI_IsWindowCreated() == false )   return;
#if IMPLEMENTATION_FOR_MACOSX
	NVI_SetProgressIndicatorProgress(0,inPercent);
#else
	NVI_SetControlNumber(kProgressBar,inPercent);//B0000 kProgressText→kProgressBar
#endif
}

void	AWindow_FTPProgress::SPI_SetProgressText( const AText& inText )
{
	//B0000 if( NVI_IsWindowVisible() == false )   return;
	if( NVI_IsWindowCreated() == false )   return;
	NVI_SetControlText(kProgressText,inText);
}

void	AWindow_FTPProgress::SPI_SetError( const AText& inText )
{
	//B0000 if( NVI_IsWindowVisible() == false )   return;
	if( NVI_IsWindowCreated() == false )   return;
	AText	text;
	text.SetLocalizedText("FTPProgress_Error");
	NVI_SetControlText(kTitleText,text);
	text.SetLocalizedText("FTPProgress_OK");
	NVI_SetControlText(kAbortButton,text);
	NVI_SetControlEnable(kAbortButton,true);
	NVI_SetControlText(kProgressText,inText);
	mErrorMode = true;
}


#pragma mark ===========================
#pragma mark [クラス]AWindow_FTPLog
#pragma mark ===========================
//FTPログウインドウ

#pragma mark --- コンストラクタ／デストラクタ

//コンストラクタ
AWindow_FTPLog::AWindow_FTPLog():AWindow(/*#175NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kFTPLogWindowPosition);
}

#pragma mark ===========================

#pragma mark <インターフェイス>

#pragma mark ===========================

#pragma mark ---ウインドウ制御

//ウインドウ生成
void	AWindow_FTPLog::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/FTPLog");
	//#688 NVI_RegisterTextEditPane(1,true,true,false,false);
	NVI_CreateEditBoxView(1,true,true,false);//#688
	AText	fontname;//win
	AFontWrapper::GetAppDefaultFontName(fontname);
	NVI_SetControlTextFont(1,/*win AFontWrapper::GetLogFont()*/fontname,9);
}

#pragma mark ===========================

#pragma mark ---ログ追加

//ログ追加
void	AWindow_FTPLog::SPI_AddLog( const AText& inText )
{
	//B0000 if( NVI_IsWindowVisible() == false )   return;
	if( NVI_IsWindowCreated() == false )   return;
	NVI_AddControlText(1,inText);
}



