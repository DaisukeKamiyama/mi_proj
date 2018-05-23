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

AStEventTransactionPrePostProcess

*/

#include "stdafx.h"

#include "AStEventTransactionPrePostProcess.h"
#include "ABaseFunction.h"

extern void	EventTransactionPostProcess( const ABool inPeriodical );

#pragma mark ===========================
#pragma mark [クラス]AStEventTransactionPrePostProcess
#pragma mark ===========================
//イベントトランザクション前処理／後処理用クラス
//OSから受信したマウスクリック等のイベントトランザクションの間、このオブジェクトを生成する。

//イベント処理中、OS APIコールにより、コールバックでイベント処理モジュールが呼ばれることがあるので、
//イベント処理のコールの深さをsEventDepthで管理する。
ANumber AStEventTransactionPrePostProcess::sEventDepth = 0;
//
ABool	AStEventTransactionPrePostProcess::sPeriodical = false;

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
AStEventTransactionPrePostProcess::AStEventTransactionPrePostProcess( const ABool inPeriodical )
{
	if( inPeriodical == true )
	{
		sPeriodical = inPeriodical;
	}
	
	//イベント処理のコールの深さを+1
	AStEventTransactionPrePostProcess::sEventDepth++;
}

//デストラクタ
AStEventTransactionPrePostProcess::~AStEventTransactionPrePostProcess()
{
	try
	{
		//イベント処理のコールの深さが1のとき、つまり、最終的に待機状態に戻るときかどうかを判定する。
		if( AStEventTransactionPrePostProcess::sEventDepth == 1 )
		{
			//イベント処理中にGarbageに追加されたObjectを、実際にdeleteする。
			ABaseFunction::DoObjectArrayItemTrashDelete();
			
			EventTransactionPostProcess(sPeriodical);
			
			sPeriodical = false;
		}
		
		//イベント処理のコールの深さを-1
		AStEventTransactionPrePostProcess::sEventDepth--;
	}
	catch(...)
	{
	}
	
}

