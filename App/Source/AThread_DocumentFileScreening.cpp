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

AThread_DocumentFileScreening

*/

#include "stdafx.h"

#include "AThread_DocumentFileScreening.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]AThread_DocumentFileScreening
#pragma mark ===========================
//ファイルのスクリーニング実行
//Mac OS Xでは、ネットワークファイル上のファイルについて、リモート側がネットワーク切断された場合、
//ファイルアクセス時にNGと判定するまで、非常に時間がかかる（90秒程度）。
//その間、関数はブロックするので、メインスレッドで実行するとアプリが停止する。

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AThread_DocumentFileScreening::AThread_DocumentFileScreening( AObjectArrayItem* inParent ) : AThread(inParent), mDocumentID(kObjectID_Invalid)
{
}

#pragma mark ===========================

#pragma mark ---スレッドメインルーチン

/**
スレッドメインルーチン
*/
void	AThread_DocumentFileScreening::NVIDO_ThreadMain()
{
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_DocumentFileScreening::NVIDO_ThreadMain started.",this);
	try
	{
		//documentオブジェクトを取得し、retainする（スレッドからもコールされるため、retain中にdocumentオブジェクト解放されないようにする）
		ADocument*	doc = GetApp().NVI_GetDocumentAndRetain(mDocumentID);
		if( doc != NULL )
		{
			//documentオブジェクトのretainを必ず解放するためのスタッククラス
			AStDecrementRetainCounter	releaser(doc);
			
			//document取得
			ADocument_Text&	textdoc = reinterpret_cast<ADocument_Text&>(*doc);
			
			//スクリーニング実行
			textdoc.SPI_ScreenDocumentFile();
		}
	}
	catch(...)
	{
		_ACError("AThread_DocumentFileScreening::NVIDO_ThreadMain() caught exception.",NULL);//#199
	}
	//完了通知
	AObjectIDArray	objectIDArray;
	objectIDArray.Add(mDocumentID);
	ABase::PostToMainInternalEventQueue(kInternalEvent_DocumentFileScreenCompleted,
										GetObjectID(),0,GetEmptyText(),objectIDArray);
	//メインスレッドのsleepを解除するためのトリガー
	GetApp().NVI_WakeTrigger();
	if( AApplication::NVI_GetEnableDebugTraceMode() == true )   _AInfo("AThread_DocumentFileScreening::NVIDO_ThreadMain ended.",this);
}

#pragma mark ===========================

#pragma mark ---スレッド起動

/**
スレッド起動
*/
void	AThread_DocumentFileScreening::SPNVI_Run( const ADocumentID inDocumentID )
{
	if( NVI_IsThreadWorking() == true )   {_ACError("",this);return;}
	
	//要求データをコピー
	mDocumentID = inDocumentID;
	
	//スレッド起動
	NVI_Run();
}

