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

AObjectArrayItem

*/

#include "stdafx.h"

#include "ABase.h"

//#693 ObjectIDにはポインタを格納するように変更
#if 0
const /*#216 AObjectID*/ANumber	kObjectID_Min = 1;//#216
const /*#216 AObjectID*/ANumber	kObjectID_Max = 0x7FFFFFFF;//MacもWinも、仮想メモリ空間32bitなので、これだけあれば十分のはず（こちらがボトルネックになることはない） #216

/*#216 AObjectID*/ANumber	gNextObjectIDCandidate = kObjectID_Min;
AArray<AObjectArrayItem*>	gObjectIDHash;
AItemCount	gObjectIDHash_DeletedCount = 0;//#303 "Deleted"の個数をRehash判断に使用する
AItemCount	gObjectIDItemCount;
AThreadMutex	gObjectIDMutex;

AObjectArrayItem*	kObjectArrayItemHash_Deleted = (AObjectArrayItem*)0xFFFFFFFF;
AObjectArrayItem*	kObjectArrayItemHash_NoData = (AObjectArrayItem*)NULL;
#endif


#pragma mark ===========================
#pragma mark [クラス]AObjectArrayItem
#pragma mark ===========================

#pragma mark ---コンストラクタ／デストラクタ

//コンストラクタ
AObjectArrayItem::AObjectArrayItem( AObjectArrayItem* inParent ) : mObjectID(kObjectID_Invalid), mParentObjectArrayItem(NULL)
,mRetainCount(0)//#717
{
	/*#693 ObjectIDにはポインタを格納するように変更
	if( inParent != NULL )
	{
		mObjectID = RegisterObject(this);
		mParentObjectArrayItem = inParent;
	}*/
	mObjectID.val = this;//#693
	mParentObjectArrayItem = inParent;//#693
}

//デストラクタ
AObjectArrayItem::~AObjectArrayItem()
{
	/*#693 ObjectIDにはポインタを格納するように変更
	if( mObjectID != kObjectID_Invalid )
	{
		UnregisterObject(mObjectID);
	}
	*/
}

//#693 ObjectIDにはポインタを格納するように変更
#if 0
#pragma mark ===========================

#pragma mark ---ObjectID管理(static)

/*

AObjectArray経由で生成されたAObjectArrayItemは、アプリ内で一意なObjectIDを持つ。
また、ObjectIDと、そのオブジェクトへのポインタは、gObjectIDHashにて管理され、
ObjectIDからポインタを検索できる。
一意なObjectIDを見つけるときは、このgObjectIDHashを用いる。

*/

//AObjectArrayItem登録
AObjectID	AObjectArrayItem::RegisterObject( AObjectArrayItem* inObject )
{
	AStMutexLocker	locker(gObjectIDMutex);
	//#216 AObjectID	objectID = gNextObjectIDCandidate;
	AObjectID	objectID;//#216
	objectID.val = gNextObjectIDCandidate;//#216
	while( FindHashIndexByObjectID(objectID) != kIndex_Invalid )
	{
		if( objectID.val < kObjectID_Max )//#216
		{
			objectID.val++;//#216
		}
		else
		{
			objectID.val = kObjectID_Min;//#216
		}
		//ObjectIDに空きなしの場合
		if( objectID.val == gNextObjectIDCandidate )//#216
		{
			_ACThrow("ObjectID Full",NULL);
		}
	}
	RegisterToHash(objectID,inObject);
	gNextObjectIDCandidate = objectID.val+1;//#216
	//#303 次回候補値がMax以上になったらMinを設定する
	if( gNextObjectIDCandidate >= kObjectID_Max )
	{
		gNextObjectIDCandidate = kObjectID_Min;
	}
	return objectID;
}

//AObjectArrayItem登録解除
void	AObjectArrayItem::UnregisterObject( const AObjectID inObjectID )
{
	AStMutexLocker	locker(gObjectIDMutex);
	DeleteFromHash(inObjectID);
}

//次のHash位置取得
inline AIndex	AObjectArrayItem::GetNextHashPos( const AIndex inHashPosition )
{
	if( inHashPosition == 0 )
	{
		return gObjectIDHash.GetItemCount()-1;
	}
	else
	{
		return inHashPosition -1;
	}
}

//ハッシュから検索
AIndex	AObjectArrayItem::FindHashIndexByObjectID( const AObjectID inObjectID )
{
	if( gObjectIDHash.GetItemCount() == 0 )   return kIndex_Invalid;
	//ハッシュ検索
	unsigned long	hash = GetHashValue(inObjectID);
	AIndex	hashstartpos = hash%(gObjectIDHash.GetItemCount());
	
	//統計情報記録 #271
	gStatics_AObjectArrayItem_FindCount++;
	gStatics_AObjectArrayItem_CompareCount++;
	//ヒットしない頻度のほうが高いので
	AObjectArrayItem*	firstHitData = gObjectIDHash.Get(hashstartpos);
	if( firstHitData == kObjectArrayItemHash_NoData )
	{
		gStatics_AObjectArrayItem_FirstDataNull++;//#271 統計情報記録
		return kIndex_Invalid;
	}
	//一度でヒットする場合はここでリターン#271
	else if( firstHitData != kObjectArrayItemHash_Deleted )
	{
		if( firstHitData->GetObjectID() == inObjectID )
		{
			gStatics_AObjectArrayItem_FirstDataHit++;//#271 統計情報記録
			return hashstartpos;
		}
	}
	//高速化のため、ポインタ使用→検索回数は平均数回程度なので無理してポインタ使用しないほうが速いはず
	//#271 AStArrayPtr<AObjectArrayItem*>	arrayptr(gObjectIDHash,0,gObjectIDHash.GetItemCount());
	//#271 AObjectArrayItem**	p = arrayptr.GetPtr();
	AIndex	hashpos = hashstartpos;
	hashpos = GetNextHashPos(hashpos);//#271 最初の位置は上記で検索済みなので次の位置から始める
	while( true )
	{
		gStatics_AObjectArrayItem_CompareCount++;//統計情報#271
		AObjectArrayItem*	ptr = gObjectIDHash.Get(hashpos);//#271 p[hashpos];
		if( ptr == kObjectArrayItemHash_NoData )   break;
		if( ptr != kObjectArrayItemHash_Deleted )
		{
			if( ptr->GetObjectID() == inObjectID )
			{
				return hashpos;
			}
		}
		hashpos = GetNextHashPos(hashpos);
		if( hashpos == hashstartpos )   break;
	}
	return kIndex_Invalid;
	
}

//ハッシュへ登録
void	AObjectArrayItem::RegisterToHash( const AObjectID inObjectID, AObjectArrayItem* inObject )
{
	//ハッシュサイズが対象データ配列の現在のサイズの3倍よりも小さければ、ハッシュテーブルを再構成する。
	if( (gObjectIDHash.GetItemCount() <= (gObjectIDItemCount+gObjectIDHash_DeletedCount)*3) || 
				(gObjectIDHash_DeletedCount>100000) )//#303 "Deleted"の個数をRehash判断に使用する
	{
		MakeHash();
	}
	//ハッシュに登録
	WriteToHash(inObjectID,inObject);
	gObjectIDItemCount++;
}

//ハッシュから削除
void	AObjectArrayItem::DeleteFromHash( const AObjectID inObjectID )
{
	/*win 080712 削除マークだらけになってしまう問題対策。下のソースに変更。元々「１」の処理はやってるので、２でどれだけ効果があるか様子見。
	AIndex index = FindHashIndexByObjectID(inObjectID);
	if( index == kIndex_Invalid )   _ACError("",NULL);
	if( gObjectIDHash.Get(GetNextHashPos(index)) == kObjectArrayItemHash_NoData )
	{
		gObjectIDHash.Set(index,kObjectArrayItemHash_NoData);
	}
	else
	{
		gObjectIDHash.Set(index,kObjectArrayItemHash_Deleted);
	}*/
	//ハッシュ検索
	unsigned long	hash = GetHashValue(inObjectID);
	AIndex	hashstartpos = hash%(gObjectIDHash.GetItemCount());
	{//#598 arrayptrの有効範囲を最小にする
		//高速化のため、ポインタ使用
		AStArrayPtr<AObjectArrayItem*>	arrayptr(gObjectIDHash,0,gObjectIDHash.GetItemCount());
		AObjectArrayItem**	p = arrayptr.GetPtr();
		AIndex	hashpos = hashstartpos;
		while( true )
		{
			AObjectArrayItem*	ptr = p[hashpos];
			if( ptr == kObjectArrayItemHash_NoData )   {_ACError("cannot delete(not found)",NULL);break;}
			if( ptr != kObjectArrayItemHash_Deleted )
			{
				if( ptr->GetObjectID() == inObjectID )
				{
					if( p[GetNextHashPos(hashpos)] == kObjectArrayItemHash_NoData )
					{
						//１ 次のハッシュ位置（物理的には前）のデータがNoDataなら、削除する位置もNoDataにしてよい
						p[hashpos] = kObjectArrayItemHash_NoData;
						//２ さらに、前のハッシュ位置（物理的には次）のデータがDeletedだった場合は、Deleted以外が見つかるまでNoDataにする
						//[hashpos-1]:NoData
						//[hashpos  ]:NoData←今回書き込み
						//[hashpos+1]:Deleted←このDeletedはNoDataにする（hashposの位置のデータへのリンクをつなげるために存在していたDeletedなので。）
						//[hashpos+2]:Deleted←さらに次もDeletedならここもNoData
						AIndex	hp = hashpos+1;
						if( hp >= gObjectIDHash.GetItemCount() )   hp = 0;
						while( p[hp] == kObjectArrayItemHash_Deleted )
						{
							p[hp] = kObjectArrayItemHash_NoData;
							hp++;
							if( hp >= gObjectIDHash.GetItemCount() )   hp = 0;
							//"Deleted"を"NoData"にしたので"Deleted"個数をデクリメント   #303 "Deleted"の個数をRehash判断に使用する
							gObjectIDHash_DeletedCount--;
						}
					}
					else
					{
						//次のハッシュ位置のデータがNoData以外なら、Deletedにしておく必要がある
						p[hashpos] = kObjectArrayItemHash_Deleted;
						//"Deleted"にしたので"Deleted"個数をインクリメント   #303 "Deleted"の個数をRehash判断に使用する
						gObjectIDHash_DeletedCount++;
					}
					break;
				}
			}
			hashpos = GetNextHashPos(hashpos);
			if( hashpos == hashstartpos )   {_ACError("cannot delete(not found)",NULL);break;}
		}
	}
	
	gObjectIDItemCount--;
}

//ハッシュ再構成、または、新規作成
void	AObjectArrayItem::MakeHash()
{
	//対象Arrayのサイズが大きすぎたらreturn
	if( gObjectIDItemCount >= kItemCount_HashMaxSize/2 )
	{
		_ACError("cannot make hash because array is too big",NULL);
		return;
	}
	//ハッシュから一時的にデータを移す
	AArray<AObjectArrayItem*>	tmpArray;
	for( AIndex i = 0; i < gObjectIDHash.GetItemCount(); i++ )
	{
		AObjectArrayItem*	ptr = gObjectIDHash.Get(i);
		if( ptr == kObjectArrayItemHash_NoData || ptr == kObjectArrayItemHash_Deleted )   continue;
		tmpArray.Add(ptr);
	}
	if( tmpArray.GetItemCount() != gObjectIDItemCount )   _ACError("",NULL);
	//ハッシュテーブル全削除
	gObjectIDHash.DeleteAll();
	AItemCount	hashsize = gObjectIDItemCount*4 + 1000000;//#303 基本サイズを65536→1000000へ変更 +0x10000;
	//ハッシュサイズが最大ハッシュサイズよりも大きければ、補正する。
	if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
	//ハッシュテーブル確保
	gObjectIDHash.Reserve(0,hashsize);
	//ハッシュテーブル初期化
	for( AIndex i = 0; i < hashsize; i++ )
	{
		gObjectIDHash.Set(i,kObjectArrayItemHash_NoData);
	}
	//対象Arrayの現在のデータを全て登録
	for( AIndex i = 0; i < tmpArray.GetItemCount(); i++ )
	{
		AObjectArrayItem*	ptr = tmpArray.Get(i);
		WriteToHash(ptr->GetObjectID(),ptr);
	}
	//"Deleted"個数初期化   #303 "Deleted"の個数をRehash判断に使用する
	gObjectIDHash_DeletedCount = 0;
	//#298 統計情報記録
	gStatics_AObjectArrayItem_MakeHashCount++;
}

//ハッシュへ書き込み
void	AObjectArrayItem::WriteToHash( const AObjectID inObjectID, AObjectArrayItem* inObject )
{
	//ハッシュ値計算
	unsigned long	hash = GetHashValue(inObjectID);
	AIndex	hashstartpos = hash%(gObjectIDHash.GetItemCount());
	
	//登録箇所検索
	AIndex	hashpos = hashstartpos;
	while( true )
	{
		AObjectArrayItem*	ptr = gObjectIDHash.Get(hashpos);
		if( ptr == kObjectArrayItemHash_NoData )   break;
		if( ptr == kObjectArrayItemHash_Deleted )
		{
			//"Deleted"にデータ上書きするので"Deleted"個数をデクリメント   #303 "Deleted"の個数をRehash判断に使用する
			gObjectIDHash_DeletedCount--;
			break;
		}
		hashpos = GetNextHashPos(hashpos);
		if( hashpos == hashstartpos )
		{
			_ACError("no area in hash",NULL);
			return;
		}
	}
	
	//登録
	gObjectIDHash.Set(hashpos,inObject);
}

//ハッシュ関数
inline unsigned long	AObjectArrayItem::GetHashValue( const AObjectID inObjectID )
{
	return static_cast<unsigned long>(inObjectID.val*3);//#216
}
#endif
//#271
/**
ObjectArrayItem統計情報取得
*/
void	AObjectArrayItem::GetStatics( AItemCount& outHashSize, AItemCount& outItemCount, AItemCount& outNoDataCount, AItemCount& outDeletedCount,
		ANumber& outNextCandidate )
{
	/*#693
	//
	outHashSize = gObjectIDHash.GetItemCount();
	outItemCount = gObjectIDItemCount;
	outNoDataCount = 0;
	outDeletedCount = 0;
	outNextCandidate = gNextObjectIDCandidate;
	//
	AStMutexLocker	locker(gObjectIDMutex);
	for( AIndex i = 0; i < gObjectIDHash.GetItemCount(); i++ )
	{
		if( gObjectIDHash.Get(i) == kObjectArrayItemHash_NoData )   outNoDataCount++;
		if( gObjectIDHash.Get(i) == kObjectArrayItemHash_Deleted )   outDeletedCount++;
	}
	//
	if( outDeletedCount != gObjectIDHash_DeletedCount )   _ACError("AObjectArrayItem Hash Deleted coutn is not correct",NULL);
	*/
}

//#271
AByteCount	AObjectArrayItem::GetAllocatedSize() const
{
	return AMemoryWrapper::AllocatedSize(this);
}

//
void	AObjectArrayItem::DumpObjectID( const AFileAcc& inDumpFile )
{
	/*テスト
	ハッシュ内容書き出し
	AText	text;
	for( AIndex i = 0; i < gObjectIDHash.GetItemCount(); i++ )
	{
		AObjectID	id = kObjectID_Invalid;
		if( gObjectIDHash.Get(i) != kObjectArrayItemHash_NoData &&
		gObjectIDHash.Get(i) != kObjectArrayItemHash_Deleted )
		{
			id = gObjectIDHash.Get(i)->GetObjectID();
		}
		text.AddFormattedCstring("%06d %08X %06d\n",i,gObjectIDHash.Get(i),id.val);
	}
	inDumpFile.WriteFile(text);
	*/
	/*#693
	ANumber	totalAllocatedSize = 0;
	ANumber	identifierListAllocatedSize = 0;//#348
	//
	AText	text;
	AObjectID	objectID;//#216
	objectID.val = kObjectID_Min;//#216
	AItemCount	itemCount = 0;
	while( itemCount < gObjectIDItemCount )
	{
		AObjectArrayItem*	ptr = NULL;
		while(true)
		{
			AIndex	index = FindHashIndexByObjectID(objectID);
			if( index == kIndex_Invalid )
			{
				objectID.val++;//#216
				continue;
			}
			else
			{
				ptr = gObjectIDHash.Get(index);
				break;
			}
		}
		text.AddFormattedCstring("ObjectID:%09d AllocatedSize:%011d ObjectPtr:%08X Class::%s\r",objectID,ptr->GetAllocatedSize(),ptr,ptr->GetClassName());
		totalAllocatedSize += ptr->GetAllocatedSize();
		//#348
		AText	className;
		className.SetCstring(ptr->GetClassName());
		if( className.Compare("AIdentifierList") == true )
		{
			identifierListAllocatedSize += ptr->GetAllocatedSize();
		}
		objectID.val++;//#216
		itemCount++;
	}
	text.Add(kUChar_LineEnd);
	text.AddFormattedCstring("TotalAllocatedSize:%d \n",totalAllocatedSize);
	text.AddFormattedCstring("Size of all AIdentifierList:%d \n",identifierListAllocatedSize);//#348
	inDumpFile.WriteFile(text);
	*/
	/*
	AText	text;
	AItemCount	itemCount = gObjectIDArrayForDebug.GetItemCount();
	for( AIndex i = 0; i < itemCount; i++ )
	{
		AObjectArrayItem*	ptr = gObjectIDArrayForDebug.Get(i);
		AText	className;
		className.SetCstring(ptr->GetClassName());
		AText	dataSammary;
		ptr->GetDataSummary(dataSammary);
		text.AddFormattedCstring("ObjectID:%08x Class: ",ptr);
		text.AddText(className);
		text.AddCstring("\n");
		text.AddText(dataSammary);
		text.AddCstring("\n");
	}
	inDumpFile.WriteFile(text);
	 */
}

