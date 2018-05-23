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

ASameFolderLoader

*/

#include "stdafx.h"

#include "ASameFolderLoader.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]ASameFolderLoader
#pragma mark ===========================
//マルチファイル検索実行

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
ASameFolderLoader::ASameFolderLoader( AObjectArrayItem* inParent ) : AThread(inParent), mSameFolderIndex(kIndex_Invalid)
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
*/
void	ASameFolderLoader::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ASameFolderLoader::NVIDO_ThreadMain started.",this);
	try
	{
		//2秒間ウエイト
		GetApp().NVI_SleepWithTimer(2);
		
		//フォルダが存在していたら同じフォルダのデータを構成
		if( mFolder.Exist() == true )
		{
			GetApp().SPI_MakeFilenameListSameFolder(mSameFolderIndex,mFolder);
		}
	}
	catch(...)
	{
		_ACError("ASameFolderLoader::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//フォルダパス取得
	AText	folderpath;
	mFolder.GetPath(folderpath);
	//完了通知
	AObjectIDArray	objectIDArray;
	ABase::PostToMainInternalEventQueue(kInternalEvent_SameFolderLoaderCompleted,
										GetObjectID(),0,folderpath,objectIDArray);
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("ASameFolderLoader::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---スレッド起動

/**
スレッド起動
*/
void	ASameFolderLoader::SPNVI_Run( const AIndex inSameFolderIndex, const AFileAcc& inFolder )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//要求データをコピー
	mSameFolderIndex = inSameFolderIndex;
	mFolder = inFolder;
	
	//スレッド起動
	NVI_Run();
}

