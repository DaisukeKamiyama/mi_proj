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

AWindow_EditProgress
#846

*/

#include "stdafx.h"

#include "AWindow_EditProgress.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AWindow_EditProgress
#pragma mark ===========================
//編集プログレスウインドウ
const AControlID		kMessageText		= 101;
const AControlID		kProgressBar		= 102;
const AControlID		kAbortButton		= 103;
const AControlID		kTitleText			= 104;
const AControlID		kProgressText		= 105;

#pragma mark --- コンストラクタ／デストラクタ

/**
コンストラクタ
*/
AWindow_EditProgress::AWindow_EditProgress():AWindow(), mErrorMode(false)
{
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

/**
クリック時処理
*/
ABool	AWindow_EditProgress::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kAbortButton:
		{
			NVI_StopModalSession();
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
void	AWindow_EditProgress::NVIDO_Create( const ADocumentID inDocumentID )
{
	//ウインドウ生成
	NVM_CreateWindow("main/EditProgress");
	
}

#pragma mark ===========================

#pragma mark ---プログレス表示

/**
プログレス設定
*/
void	AWindow_EditProgress::SPI_SetProgress( const ANumber inPercent )
{
	if( NVI_IsWindowCreated() == false )   return;
#if IMPLEMENTATION_FOR_MACOSX
	NVI_SetProgressIndicatorProgress(0,inPercent);
#else
	NVI_SetControlNumber(kProgressBar,inPercent);
#endif
}

/**
各テキスト設定
*/
void	AWindow_EditProgress::SPI_SetText( const AText& inTitle, const AText& inMessage )
{
	if( NVI_IsWindowCreated() == false )   return;
	NVI_SetControlText(kTitleText,inTitle);
	NVI_SetControlText(kMessageText,inMessage);
	NVI_SetControlText(kProgressText,GetEmptyText());
}

/**
プログレステキスト設定
*/
void	AWindow_EditProgress::SPI_SetProgressText( const AText& inText )
{
	NVI_SetControlText(kProgressText,inText);
}

/**
キャンセルボタンenable/disable設定
*/
void	AWindow_EditProgress::SPI_SetEnableCancelButton( const ABool inEnable )
{
	NVI_SetControlEnable(kAbortButton,inEnable);
	NVI_RefreshWindow();
}

