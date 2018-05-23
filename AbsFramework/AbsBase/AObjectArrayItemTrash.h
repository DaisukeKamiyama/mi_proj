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

AObjectArrayItemTrash

*/

#pragma once

#include "ABaseTypes.h"
#include "AObjectArrayItem.h"
#include "AArray.h"

#pragma mark ===========================
#pragma mark [クラス]AObjectArrayItemTrash
/**
ObjectArrayItemゴミ箱（ガーベージ）

ObjectArrayからオブジェクトを削除すると、ここにつながれて、
イベントトランザクション最後で中身を実際に削除する。
ポインタ不正参照を防ぐ仕組み。
*/
class AObjectArrayItemTrash
{
	//コンストラクタ／デストラクタ
  public:
	AObjectArrayItemTrash();
	~AObjectArrayItemTrash();
	
	//
  public:
	void	AddToObjectArrayItemTrash( AObjectArrayItem* inObjectPtr );
	void	DoObjectArrayItemTrashDelete();
	
	//
  private:
	AArray<AObjectArrayItem*>	mTrash;
};


