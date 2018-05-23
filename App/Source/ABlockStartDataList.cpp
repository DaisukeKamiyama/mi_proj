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

ABlockStartDataList
#695

*/

#include "stdafx.h"

#include "ABlockStartDataList.h"

/**
BlockStartData追加
*/
AUniqID	ABlockStartDataList::Add( const AUniqID inPreviousID, const AIndex inVariableIndex, const AItemCount inIndentCount )
{
	//配列へ追加
	AIndex	index = mUniqIDArray.AddItem();
	AUniqID	addedUniqID = mUniqIDArray.Get(index);
	mVariableIndexArray.Add(inVariableIndex);
	mIndentCountArray.Add(inIndentCount);
	mNextUniqIDArray.Add(kUniqID_Invalid);
	
	//リスト接続
	//inPreviousIDがkUniqID_Invalidでない場合（＝リスト先頭以外）、前のNextに自オブジェクトを設定する。
	if( inPreviousID != kUniqID_Invalid )
	{
		//前のIdentifierのNextUniqIDに、追加したIdentifierのUniqIDを設定する。
		AIndex	prevIndex = mUniqIDArray.Find(inPreviousID);
		if( mNextUniqIDArray.Get(prevIndex) == kUniqID_Invalid )
		{
			mNextUniqIDArray.Set(prevIndex,addedUniqID);
		}
		else
		{
			//prevにUniqID設定済み（最後尾追加でない）場合はthrow
			_ACThrow("not last object",this);
		}
	}
	return addedUniqID;
}

/**
BlockStartData削除
リストに連なる識別子を全て削除する。
*/
void	ABlockStartDataList::DeleteIdentifiers( const AUniqID inFirstUniqID )
{
	//まず次の識別子以降を削除（再帰）
	AIndex	index0 = mUniqIDArray.Find(inFirstUniqID);
	AUniqID	nextUniqID = mNextUniqIDArray.Get(index0);
	if( nextUniqID == inFirstUniqID )   {_ACError("uniq id error (loop)",NULL);return;}
	if( nextUniqID != kUniqID_Invalid )
	{
		DeleteIdentifiers(nextUniqID);
	}
	//自身を削除
	//indexはここで再度検索する必要がある。（再帰コールで次以降が削除済みなので、最初とはIndexが変わっている。）
	AIndex	index = mUniqIDArray.Find(inFirstUniqID);
	mUniqIDArray.Delete1Item(index);
	mVariableIndexArray.Delete1(index);
	mIndentCountArray.Delete1(index);
	mNextUniqIDArray.Delete1(index);
}

/**
全削除
*/
void	ABlockStartDataList::DeleteAll()
{
	mUniqIDArray.DeleteAll();
	mVariableIndexArray.DeleteAll();
	mIndentCountArray.DeleteAll();
	mNextUniqIDArray.DeleteAll();
}

/**
VariableIndex取得
*/
AIndex	ABlockStartDataList::GetVariableIndex( const AUniqID inUniqID ) const
{
	AIndex	index = mUniqIDArray.Find(inUniqID);
	return mVariableIndexArray.Get(index);
}

/**
IndentCount取得
*/
AItemCount	ABlockStartDataList::GetIndentCount( const AUniqID inUniqID ) const
{
	AIndex	index = mUniqIDArray.Find(inUniqID);
	return mIndentCountArray.Get(index);
}

/**
NextUniqID取得
*/
AUniqID	ABlockStartDataList::GetNextUniqID( const AUniqID inUniqID ) const
{
	AIndex	index = mUniqIDArray.Find(inUniqID);
	return mNextUniqIDArray.Get(index);
}

