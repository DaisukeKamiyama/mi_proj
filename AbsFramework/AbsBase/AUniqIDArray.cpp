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

AUniqIDArray
#693

*/

#include "AUniqIDArray.h"

//UniqIDのUnuseマークの定義
const AUniqID	kUniqID_UnusedMark = {-2};

#pragma mark ===========================

#pragma mark ---コンストラクタ・デストラクタ

/**
コンストラクタ
*/
AUniqIDArray::AUniqIDArray( const AItemCount inInitialAllocateCount, const AItemCount inReallocateStepCount ) : 
		mUniqIDArray(NULL,inInitialAllocateCount,inReallocateStepCount),
		mUniqIDToIndexArray(NULL,inInitialAllocateCount,inReallocateStepCount),
		mUnusedItemIndexArray(NULL,kUnusedItemIndexArray_InitialAllocateCount,kUnusedItemIndexArray_ReallocateStepCount),
		mUnusignedUniqIDValArray(NULL,kUnusignedUniqIDValArray_InitialAllocateCount,kUnusignedUniqIDValArray_ReallocateStepCount)
{
}

/**
デストラクタ
*/
AUniqIDArray::~AUniqIDArray()
{
}

#pragma mark ===========================

#pragma mark ---UniqID生成・削除（+Index番号との結びつけ）

/**
UniqIDを主配列のインデックスinIndexからinCount個割り当ててSetする。
この関数コール時、主配列mUniqIDArrayの要素は確保済みである。
*/
void	AUniqIDArray::AssignUniqIDs( const AIndex inIndex, const AItemCount inCount )
{
	//currentIndexに現在の主配列のindex位置
	AIndex	currentIndex = inIndex;
	
	//まず未割り当てUniqIDを使用する
	//mUnusignedUniqIDValArrayに未使用化したUniqIDが入っているので後ろからpopしていく
	{
		AItemCount	itemCount = mUnusignedUniqIDValArray.GetItemCount();
		for( AIndex i = itemCount-1; i >= 0; i-- )
		{
			//UniqIDをpop
			AUniqID	uniqID = {mUnusignedUniqIDValArray.Get(i)};
			mUnusignedUniqIDValArray.Delete1(i);
			//補助配列mUniqIDToIndexArray内の対応するindexを計算し、そこに現在の主配列indexを設定
			AIndex	uniqIDToIndexArayIndex = uniqID.val - kUniqIDValMin;
			mUniqIDToIndexArray.Set(uniqIDToIndexArayIndex,currentIndex);
			//主配列の現在のindexにUniqIDを設定
			mUniqIDArray.Set(currentIndex,uniqID);
			//主配列の現在のindexをインクリメントし、inCount分割り当て完了ならbreak
			currentIndex++;
			if( currentIndex >= inIndex + inCount )
			{
				break;
			}
		}
	}
	
	//補助配列mUniqIDToIndexArrayに、必要個数分の領域を確保。uniqIDToIndexArayIndexに現在の補助配列mUniqIDToIndexArray側indexを設定。
	AIndex	uniqIDToIndexArayIndex = mUniqIDToIndexArray.GetItemCount();
	mUniqIDToIndexArray.Reserve(uniqIDToIndexArayIndex,(inIndex + inCount) - currentIndex);
	//補助配列mUniqIDToIndexArrayのindexからUniqIDを計算して、それぞれ設定していく。
	for( ; currentIndex < inIndex + inCount; currentIndex++ )
	{
		//補助配列mUniqIDToIndexArrayに現在の主配列indexを設定
		mUniqIDToIndexArray.Set(uniqIDToIndexArayIndex,currentIndex);
		//主配列にUniqIDを設定
		AUniqID	uniqID = {kUniqIDValMin + uniqIDToIndexArayIndex};
		mUniqIDArray.Set(currentIndex,uniqID);
		//補助配列のindexをインクリメント
		uniqIDToIndexArayIndex++;
	}
}

/**
UniqID生成・Indexとの結びつけ（挿入）
*/
void	AUniqIDArray::InsertItems( const AIndex inIndex, const AItemCount inInsertCount )
{
	//補助配列mUniqIDToIndexArrayはUniqIDの値ごとに、対応する主配列mUniqIDArrayのindexを格納している
	//補助配列mUniqIDToIndexArrayのindexはUniqID順に対応しており、
	//0番目の要素にはUniqID:{kUniqIDValMin}割り当て先の主配列mUniqIDArray配列のindexが入っている
	
	//最後への追加でなければ、補助配列mUniqIDToIndexArrayのindexをずらす
	if( inIndex < mUniqIDArray.GetItemCount() )
	{
		//Unuse機構使用中は途中挿入は未サポート
		if( mUnusedItemIndexArray.GetItemCount() >0 )
		{
			_ACError("",NULL);
		}
		
		//補助配列mUniqIDToIndexArrayのindexを追加個数分ずらす
		AItemCount	itemCount = mUniqIDToIndexArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr(mUniqIDToIndexArray,0,itemCount);
		AIndex*	p = arrayptr.GetPtr();
		for( AIndex i = 0; i < itemCount; i++ )
		{
			if( p[i] >= inIndex )
			{
				p[i] += inInsertCount;
			}
		}
	}
	
	//主配列に領域確保
	mUniqIDArray.Reserve(inIndex,inInsertCount);
	
	//UniqIDを割り当てて設定
	AssignUniqIDs(inIndex,inInsertCount);
}

/**
UniqID生成・Indexとの結びつけ（1つ挿入）
*/
void	AUniqIDArray::Insert1Item( const AIndex inIndex )
{
	InsertItems(inIndex,1);
}

/**
UniqID生成・Indexとの結びつけ（1つ追加）
*/
AIndex	AUniqIDArray::AddItem()
{
	AIndex	index = mUniqIDArray.GetItemCount();
	InsertItems(index,1);
	return index;
}

/**
UniqID削除・Indexとの結びつけ解除
*/
void	AUniqIDArray::DeleteItems( const AIndex inIndex, const AItemCount inDeleteCount )
{
	//Unuse機構使用中は削除は未サポート
	if( mUnusedItemIndexArray.GetItemCount() >0 )
	{
		_ACError("",NULL);
	}
	
	//補助配列内の削除項目にkIndex_Invalidを設定し、削除した以降の項目のindexをずらす
	{
		//主配列側ポインタ
		AItemCount	uniqIDArrayItemCount = mUniqIDArray.GetItemCount();
		AStArrayPtr<AUniqID>	arrayptr_uniqIDArray(mUniqIDArray,0,uniqIDArrayItemCount);
		AUniqID*	p_uniqIDArray = arrayptr_uniqIDArray.GetPtr();
		//補助配列側ポインタ
		AItemCount	toIndexArrayItemCount = mUniqIDToIndexArray.GetItemCount();
		AStArrayPtr<AIndex>	arrayptr_toIndexArray(mUniqIDToIndexArray,0,toIndexArrayItemCount);
		AIndex*	p_toIndexArray = arrayptr_toIndexArray.GetPtr();
		//主配列の削除項目からUniqIDを取得し、対応する補助配列項目の値をkIndex_Invalidに設定する
		for( AIndex uniqIDArrayIndex = inIndex; uniqIDArrayIndex < inIndex + inDeleteCount; uniqIDArrayIndex++ )
		{
			//UniqID取得
			AUniqID	uniqID = p_uniqIDArray[uniqIDArrayIndex];
			//補助配列の値を更新
			p_toIndexArray[uniqID.val-kUniqIDValMin] = kIndex_Invalid;
			//未割り当てUniqID配列にpush
			mUnusignedUniqIDValArray.Add(uniqID.val);
		}
		//主配列の削除項目より後のUniqIDを取得し、対応する補助配列項目の値をずらす
		for( AIndex uniqIDArrayIndex = inIndex + inDeleteCount; uniqIDArrayIndex < uniqIDArrayItemCount; uniqIDArrayIndex++ )
		{
			//UniqID取得
			AUniqID	uniqID = p_uniqIDArray[uniqIDArrayIndex];
			//補助配列の値を更新
			p_toIndexArray[uniqID.val-kUniqIDValMin] -= inDeleteCount;
		}
	}
	//主配列の項目を削除
	mUniqIDArray.Delete(inIndex,inDeleteCount);
}

/**
UniqID削除・Indexとの結びつけ解除
*/
void	AUniqIDArray::Delete1Item( const AIndex inIndex )
{
	DeleteItems(inIndex,1);
}

/**
全削除
*/
void	AUniqIDArray::DeleteAll()
{
	mUniqIDArray.DeleteAll();
	mUniqIDToIndexArray.DeleteAll();
	mUnusedItemIndexArray.DeleteAll();
	mUnusignedUniqIDValArray.DeleteAll();
}

/**
IndexからUniqID取得
*/
AUniqID	AUniqIDArray::Get( const AIndex inIndex ) const
{
	return mUniqIDArray.Get(inIndex);
}

/**
UniqIDからIndex取得
*/
AIndex	AUniqIDArray::Find( const AUniqID inUniqID ) const
{
	return mUniqIDToIndexArray.Get(inUniqID.val-kUniqIDValMin);
}

#pragma mark ===========================

#pragma mark ---UnusedItem管理（削除マーク項目）

/*
項目に対してUnuseマークをつけることで、項目を削除せず、未使用領域に設定する。
AUniqIDArrayと同じ数の列（配列）を並べてテーブルを構成することで、
行追加・削除時にmemmoveをしないテーブルを実現するための機構。
*/

/**
Unuseとなった項目を検索し、再度UniqIDを割り振って、再利用する。Unuse項目がなければkIndex_Invalidを返す。
*/
AIndex	AUniqIDArray::ReuseUnusedItem()
{
	AItemCount	unuseItemCount = mUnusedItemIndexArray.GetItemCount();
	if( unuseItemCount > 0 )
	{
		//Unuse項目配列からpop（＝主配列のindex）
		AIndex	index = mUnusedItemIndexArray.Get(unuseItemCount-1);
		mUnusedItemIndexArray.Delete1(unuseItemCount-1);
		//そのindexにUniqID割り当て
		AssignUniqIDs(index,1);
		return index;
	}
	else
	{
		return kIndex_Invalid;
	}
}

/**
項目をUnuseに設定する
*/
void	AUniqIDArray::Unuse1Item( const AIndex inIndex )
{
	//Unuseにする項目のUniqIDを取得、補助配列の値をkIndex_Invalidに設定
	AUniqID	uniqID = mUniqIDArray.Get(inIndex);
	mUniqIDToIndexArray.Set(uniqID.val-kUniqIDValMin,kIndex_Invalid);
	//Unuse項目配列へpush
	mUnusignedUniqIDValArray.Add(uniqID.val);
	//主配列の項目にはUnuseマークを設定
	mUniqIDArray.Set(inIndex,kUniqID_UnusedMark);
	//Unuse項目配列へ主配列のindexをpush
	mUnusedItemIndexArray.Add(inIndex);
}

