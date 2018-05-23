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

#include "stdafx.h"

#include "ABase.h"

#pragma mark ===========================
#pragma mark [クラス]AddToObjectArrayItemTrash
#pragma mark ===========================
//ObjectArrayItemゴミ箱
#pragma mark ---コンストラクタ／デストラクタ

//コンストラクタ
AObjectArrayItemTrash::AObjectArrayItemTrash()
{
	mTrash.SetThrowIfOutOfBounds();//B0000
}
//デストラクタ
AObjectArrayItemTrash::~AObjectArrayItemTrash()
{
}

#pragma mark ===========================

#pragma mark ---削除制御

void	AObjectArrayItemTrash::AddToObjectArrayItemTrash( AObjectArrayItem* inObjectPtr )
{
	mTrash.Add(inObjectPtr);
}

void	AObjectArrayItemTrash::DoObjectArrayItemTrashDelete()
{
	//#717 while(mTrash.GetItemCount()>0)
	for( AIndex index = mTrash.GetItemCount()-1; index >= 0; index-- )//#717
	{
		AObjectArrayItem*	ptr = mTrash.Get(index);//#717 mTrash.GetItemCount()-1);
		if( ptr->GetRetainCount() <= 0 )//#717 retainされているときは今回は削除しない。
		{
			//オブジェクトを完全に削除する。
			//なお、Delete1()内で別のオブジェクトを削除することにより、mTrashに新たに追加されることがある。
			//この場合、今回は削除せずに、次のイベントトランザクション終了時に削除することにする。
			//今回は、元々存在していたmTrashを、この関数コール時の最後の位置から前へ順に削除する。
			mTrash.Delete1(index);//#717 mTrash.GetItemCount()-1);
			delete ptr;
		}
	}
	//mTrashにオブジェクトが残っている場合はfprintf表示 #717
	//（mTrashに残ったままにならないことの確認のため）
	/*
	if( true )
	{
		if( mTrash.GetItemCount() > 0 )
		{
			fprintf(stderr,"AObjectArrayItemTrash::DoObjectArrayItemTrashDelete() trash remained:%d ",mTrash.GetItemCount());
		}
	}
	*/
}


