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

AWindow_MultiFileFindProgress

*/

#include "stdafx.h"

#include "AWindow_MultiFileFindProgress.h"
#include "AApp.h"

const AControlID		kProgressText		= 101;
const AControlID		kProgressBar		= 102;
const AControlID		kAbortButton		= 103;


#pragma mark ===========================
#pragma mark [クラス]AWindow_MultiFileFindProgress
#pragma mark ===========================
//マルチファイル検索プログレス

#pragma mark --- コンストラクタ／デストラクタ
//コンストラクタ
AWindow_MultiFileFindProgress::AWindow_MultiFileFindProgress():AWindow(/*#175NULL*/)
{
	NVM_SetWindowPositionDataID(AAppPrefDB::kMultiFileFindProgressPosition);
}

#pragma mark ===========================

#pragma mark <イベント処理>

#pragma mark ===========================

//中止ボタン
ABool	AWindow_MultiFileFindProgress::EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys )
{
	ABool	result = false;
	switch(inID)
	{
	  case kAbortButton:
		{
			GetApp().SPI_AbortMultiFileFind();
			AText	text;
			text.SetLocalizedText("Progress_Aborting");
			NVI_SetControlText(kProgressText,text);
			break;
		}
	  default:
		{
			_ACError("",this);
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
void	AWindow_MultiFileFindProgress::NVIDO_Create( const ADocumentID inDocumentID )
{
	NVM_CreateWindow("main/MultiFileFindProgress");
}

#pragma mark ===========================

#pragma mark ---プログレス表示制御

void	AWindow_MultiFileFindProgress::SPI_SetProgressText( const AText& inText )
{
	if( NVI_IsWindowVisible() == false )   return;
	NVI_SetControlText(kProgressText,inText);
}

void	AWindow_MultiFileFindProgress::SPI_SetProgress( const short inPercent )
{
	if( NVI_IsWindowVisible() == false )   return;
#if IMPLEMENTATION_FOR_MACOSX
	NVI_SetProgressIndicatorProgress(0,inPercent);
#else
	NVI_SetControlNumber(kProgressBar,inPercent);
	#endif
}

