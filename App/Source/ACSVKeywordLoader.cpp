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

ACSVKeywordLoader

*/

#include "stdafx.h"

#include "ACSVKeywordLoader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]ACSVKeywordLoader
#pragma mark ===========================
//マルチファイル検索実行

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
ACSVKeywordLoader::ACSVKeywordLoader( AObjectArrayItem* inParent ) : AThread(inParent), mModeIndex(kIndex_Invalid), mCategoryIndex(kIndex_Invalid)
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
*/
void	ACSVKeywordLoader::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ACSVKeywordLoader::NVIDO_ThreadMain started.",this);
	try
	{
		GetApp().SPI_GetModePrefDB(mModeIndex).AddCategoryKeywordListFromCSV(mCSVText,mCategoryIndex,mAborted);
	}
	catch(...)
	{
		_ACError("ACSVKeywordLoader::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//完了通知
	AObjectIDArray	objectIDArray;
	ABase::PostToMainInternalEventQueue(kInternalEvent_CSVKeywordLoaderCompleted,
										GetObjectID(),0,GetEmptyText(),objectIDArray);
	
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ACSVKeywordLoader::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---スレッド起動

/**
スレッド起動
*/
void	ACSVKeywordLoader::SPNVI_Run( const AIndex inModeIndex, const AText& inCSVText, const AIndex inCategoryIndex )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//要求データをコピー
	mModeIndex = inModeIndex;
	mCSVText.SetText(inCSVText);
	mCategoryIndex = inCategoryIndex;
	
	//スレッド起動
	NVI_Run();
}

