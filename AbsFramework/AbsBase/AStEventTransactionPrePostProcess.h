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

#pragma once

#include "ABaseTypes.h"
#include "AStackOnlyObject.h"

class AObjectArrayItem;

#pragma mark ===========================
#pragma mark [クラス]AStEventTransactionPrePostProcess
/**イベントトランザクション前処理／後処理用クラス
OSから受信したマウスクリック等のイベントトランザクションの間、このオブジェクトを生成する。
具体的には、イベント処理モジュールの最初に、ローカル変数として定義する。
これにより、AObjectArray<>で（仮）削除されたオブジェクトのdelete等、イベント処理の前処理、後処理を行う。
*/
class AStEventTransactionPrePostProcess : public AStackOnlyObject
{
	//コンストラクタ／デストラクタ
  public:
	AStEventTransactionPrePostProcess( const ABool inPeriodical );
	~AStEventTransactionPrePostProcess();
	
	//1イベント処理終了時に、deleteするAObjectArrayItemを登録する。
  public:
	void	DeleteWhenEventEnd( AObjectArrayItem* inObjectPtr );
	
  private:
	//イベント処理のコールの深さ
	static ANumber sEventDepth;
	//
	static ABool	sPeriodical;
};


