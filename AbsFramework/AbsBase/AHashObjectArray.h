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

AHashObjectArray
(C) 2008 Daisuke Kamiyama, All Rights Reserved.

*/

#pragma once

#include "ABaseTypes.h"
#include "AObjectArray.h"
#include "AObjectArrayIDIndexed.h"
#include "AObjectArrayItem.h"

//#693 ハッシュ用定義
#ifdef __LP64__
#define	kObjectArrayItemHash_Deleted ((AObjectArrayItem*)0xFFFFFFFFFFFFFFFF)
#else
#define	kObjectArrayItemHash_Deleted ((AObjectArrayItem*)0xFFFFFFFF)
#endif
#define	kObjectArrayItemHash_NoData ((AObjectArrayItem*)NULL)

#pragma mark ===========================
#pragma mark [クラス]AHashObjectArray
/**
オブジェクトの可変長配列（ハッシュ検索）クラス
AObjectArrayオブジェクトをコンポジションで持ち、そのデータをハッシュ検索できる。

Tは配列にするオブジェクト、Uは検索時にキーとして使用するオブジェクトもしくは基本型で、そのオブジェクト自体、もしくは、オブジェクトが所有するメンバーのいずれか
*/
template <class T, class U> class AHashObjectArray : public AObjectArrayItem
{
  public:
	//コンストラクタ
	AHashObjectArray( AObjectArrayItem* inParent = NULL, const ANumber inHashSizeMultiple = 3 ) : AObjectArrayItem(inParent),
			mHashSizeMultiple(inHashSizeMultiple)//B0000 高速化
	,mHash_DeletedCount(0)//#303
	,mMutexEnabled(true)//#598
	{
		//#598 こちらのクラスで排他制御するので下位のmObjectArrayでは制御不要
		mObjectArray.EnableThreadSafe(false);
		
		//
		MakeHash();
	}
	//デストラクタ
	virtual ~AHashObjectArray()
	{
	}
	
	//新規オブジェクト新規挿入
	void	InsertNew1Object( const AIndex inIndex, AAbstractFactoryForObjectArray<T>& inFactory )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		//オブジェクト追加
		mObjectArray.InsertNew1Object(inIndex,inFactory);
		
		//ハッシュ更新
		UpdateHash_DataAdded(inIndex,mObjectArray.GetObjectConst(inIndex).GetObjectID());
	}
	
	//新規オブジェクト新規挿入（デフォルトAbstractFactory使用）
	void	InsertNew1Object( const AIndex inIndex )
	{
		ADefaultAbstractFactoryForObjectArray<T>	defaultFactory(this);
		InsertNew1Object(inIndex,defaultFactory);
	}
	
	//新規オブジェクト新規追加
	AIndex	AddNewObject( AAbstractFactoryForObjectArray<T>& inFactory )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		//オブジェクト追加
		AIndex	index = mObjectArray.AddNewObject(inFactory);
		
		//ハッシュ更新
		UpdateHash_DataAdded(index,mObjectArray.GetObjectConst(index).GetObjectID());
		
		return index;
	}
	
	//新規オブジェクト新規追加（デフォルトAbstractFactory使用）
	AIndex AddNewObject()
	{
		ADefaultAbstractFactoryForObjectArray<T>	defaultFactory;
		return AddNewObject(defaultFactory);
	}
	
	//オブジェクト取得
  public:
	T& GetObject( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetObject(inIndex);
	}
	//オブジェクト取得(const)
	const T& GetObjectConst( const AIndex inIndex ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetObjectConst(inIndex);
	}
  public:
	//ObjectIDからオブジェクト取得
	T& GetObjectByID( const AObjectID inObjectID )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetObjectByID(inObjectID);
	}
	//ObjectIDからオブジェクト取得(const)
	T& GetObjectConstByID( const AObjectID inObjectID ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetObjectConstByID(inObjectID);
	}
	
	//オブジェクト更新に対してハッシュ更新
	void	ObjectUpdateStart( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		AObjectID	objectID = mObjectArray.GetObjectConst(inIndex).GetObjectID();
		UpdateHash_DataDeleted(inIndex,objectID);
	}
	void	ObjectUpdateEnd( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		AObjectID	objectID = mObjectArray.GetObjectConst(inIndex).GetObjectID();
		UpdateHash_DataAdded(inIndex,objectID);
	}
	
	//削除
	void	Delete1Object( const AIndex inIndex )
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		//削除しようとするオブジェクトのObjectID取得
		AObjectID	objectID = mObjectArray.GetObjectConst(inIndex).GetObjectID();
	
		//ハッシュ更新
		UpdateHash_DataDeleted(inIndex,objectID);
		
		//オブジェクト削除
		mObjectArray.Delete1Object(inIndex);
	}
	
	//全削除
	void	DeleteAll( const ABool inRetainSameHashSize = false )//win 080714 高速化（現在のハッシュサイズを維持するフラグ追加）
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		/* B0000 高速化 １つずつ消すとHash更新の時間がかかるので、一気に消して、MakeHash()したほうが速い
		while( GetItemCount() > 0 )
		{
			Delete1Object(GetItemCount()-1);
		}*/
		mObjectArray.DeleteAll();
		MakeHash(inRetainSameHashSize);//win 080714
	}
	
	//ObjectID管理
	ANumber	GetID( const AIndex inIndex ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetID(inIndex);
	}
	/**
	ObjectIDからIndex取得（低速なので極力使用しないこと）
	*/
	AIndex	GetIndexByID( const AObjectID inID ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetIndexByID(inID);
	}
	
	//検索（inTargetIndex, inTargetItemCountは、Targetの一部分と一致する配列要素を検索するために使用する。）
	//★（ID->Indexが遅い！）
	AIndex	Find( const U& inTargetData, const AIndex inTargetIndex = 0, const AItemCount inTargetItemCount = kIndex_Invalid ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		unsigned long	hash = GetHashValue(inTargetData,inTargetIndex,inTargetItemCount);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		AIndex	hashpos = hashstartpos;
		while( true )
		{
			AObjectID	dataObjectID = mHash.Get(hashpos);
			if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;//#216
			if( dataObjectID.val != kObjectArrayItemHash_Deleted )//#216
			{
				AIndex	dataIndex = mObjectArray.GetIndexByID(dataObjectID);//#598
				if( mObjectArray.GetObjectConst(dataIndex).Compare(inTargetData,inTargetIndex,inTargetItemCount) == true )
				{
					return dataIndex;
				}
			}
			hashpos = GetNextHashPos(hashpos);
			if( hashpos == hashstartpos )   break;
		}
		return kIndex_Invalid;
	}
	//検索（ObjectIDを取得）
	AObjectID	FindObjectID( const U& inTargetData, const AIndex inTargetIndex = 0, const AItemCount inTargetItemCount = kIndex_Invalid ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		unsigned long	hash = GetHashValue(inTargetData,inTargetIndex,inTargetItemCount);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		{//#598 arrayptrの有効範囲を最小にする
			//高速化のため、ポインタ使用
			AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			AObjectID*	p = arrayptr.GetPtr();
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AObjectID	dataObjectID = p[hashpos];
				if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;//#216
				if( dataObjectID.val != kObjectArrayItemHash_Deleted )
				{
					if( mObjectArray.GetObjectConstByID(dataObjectID).Compare(inTargetData,inTargetIndex,inTargetItemCount) == true )
					{
						return dataObjectID;
					}
				}
				hashpos = GetNextHashPos(hashpos);
				if( hashpos == hashstartpos )   break;
			}
		}
		return kObjectID_Invalid;
	}
	//検索（ObjectIDを取得）
	void	FindObjectID( const U& inTargetData, const AIndex inTargetIndex/* = 0*/, const AItemCount inTargetItemCount/* = kIndex_Invalid*/,
			AArray<AObjectID> &outObjectIDArray ) const
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		outObjectIDArray.DeleteAll();
		
		unsigned long	hash = GetHashValue(inTargetData,inTargetIndex,inTargetItemCount);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		{//#598 arrayptrの有効範囲を最小にする
			//高速化のため、ポインタ使用
			AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			AObjectID*	p = arrayptr.GetPtr();
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AObjectID	dataObjectID = p[hashpos];
				if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;//#216
				if( dataObjectID.val != kObjectArrayItemHash_Deleted )//#216
				{
					if( mObjectArray.GetObjectConstByID(dataObjectID).Compare(inTargetData,inTargetIndex,inTargetItemCount) == true )
					{
						outObjectIDArray.Add(dataObjectID);
					}
				}
				hashpos = GetNextHashPos(hashpos);
				if( hashpos == hashstartpos )   break;
			}
		}
	}
	
	//ItemCount
	AItemCount	GetItemCount() const 
	{
		AStMutexLocker	mutexLocker(mMutex,mMutexEnabled);//#598
		
		return mObjectArray.GetItemCount();
	}
	
	
  private:
	//Hash値取得
	//各T毎に定義すること。
	unsigned long	GetHashValue( const U& inData , const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const;
	
	//ハッシュテーブルを初期化し、対象Arrayの全データを登録する。
	void	MakeHash( const ABool inRetainSameHashSize = false )//win 080714 高速化（現在のハッシュサイズを維持するフラグ追加）
	{
		//対象Arrayのサイズが大きすぎたらreturn
		if( mObjectArray.GetItemCount() >= kItemCount_HashMaxSize/2 )
		{
			_ACError("cannot make hash because array is too big",this);
			return;
		}
		AItemCount	hashsize = mHash.GetItemCount();//win 080714 高速化
		if( inRetainSameHashSize == false )//win 080714 高速化
		{
			//ハッシュテーブル全削除
			mHash.DeleteAll();
			hashsize = mObjectArray.GetItemCount()* /*B0000 高速化 3*/mHashSizeMultiple+128;
			//ハッシュサイズが最大ハッシュサイズよりも大きければ、補正する。
			if( hashsize > kItemCount_HashMaxSize )   hashsize = kItemCount_HashMaxSize;
			//ハッシュテーブル確保
			mHash.Reserve(0,hashsize);
		}
		//ハッシュテーブル初期化
		{//#598 arrayptrの有効範囲を最小にする
			//高速化のため、ポインタ使用
			AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			AObjectID*	p = arrayptr.GetPtr();
			for( AIndex i = 0; i < hashsize; i++ )
			{
				p[i].val = kObjectArrayItemHash_NoData;//#216
			}
		}
		//対象Arrayの現在のデータを全て登録
		AItemCount	datacount = mObjectArray.GetItemCount();
		for( AIndex i = 0; i < datacount; i++ )
		{
			RegisterToHash(i,mObjectArray.GetObjectConst(i).GetObjectID());
		}
		//fprintf(stderr,"AHashObjectArray::MakeHash() ");
		//"Deleted"個数初期化   #303 "Deleted"の個数をRehash判断に使用する
		mHash_DeletedCount = 0;
		//統計情報#271
		gStatics_AHashObjectArray_MakeHashCount++;
	}
	
	//ハッシュ更新（追加）
	void	UpdateHash_DataAdded( const AIndex inIndex, const AObjectID& inObjectID )
	{
		//ハッシュサイズが対象データ配列の現在のサイズの2倍よりも小さければ、ハッシュテーブルを再構成する。
		if( mHash.GetItemCount() < (mObjectArray.GetItemCount()+mHash_DeletedCount)* /*B0000 高速化 2*/(mHashSizeMultiple*2/3) )//#303 "Deleted"の個数をRehash判断に使用する
		{
			MakeHash();
		}
		//そうでない場合は対象アイテムだけ追加登録する。
		else
		{
			//ハッシュに登録
			RegisterToHash(inIndex,inObjectID);
		}
	}
	
	//ハッシュ更新（削除）
	void UpdateHash_DataDeleted( const AIndex inIndex, const AObjectID inObjectID )
	{
		//ハッシュ値計算
		unsigned long	hash = GetHashValue(mObjectArray.GetObjectConst(inIndex).GetPrimaryKeyData(),0,kIndex_Invalid);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		{//#598 arrayptrの有効範囲を最小にする
			//高速化のため、ポインタ使用
			AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			AObjectID*	p = arrayptr.GetPtr();
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AObjectID	dataObjectID = p[hashpos];
				if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;//#216
				if( dataObjectID.val != kObjectArrayItemHash_Deleted )//#216
				{
					if( dataObjectID.val == inObjectID.val )//#216
					{
						//削除実行
						if( p[GetNextHashPos(hashpos)].val == kObjectArrayItemHash_NoData )
						{
							p[hashpos].val = kObjectArrayItemHash_NoData;//#216
							//win 080713 NoDataを書き込んだとき、前のハッシュ位置（物理的には次）がDeletedならNoDataに書き換え
							//[hashpos-1]:NoData
							//[hashpos  ]:NoData←今回書き込み
							//[hashpos+1]:Deleted←このDeletedはNoDataにする（hashposの位置のデータへのリンクをつなげるために存在していたDeletedなので。）
							//[hashpos+2]:Deleted←さらに次もDeletedならここもNoData
							AIndex	hp = hashpos+1;
							if( hp >= mHash.GetItemCount() )   hp = 0;
							while( p[hp].val == kObjectArrayItemHash_Deleted )//#216
							{
								//DeletedならNoDataに書き換えて継続
								p[hp].val = kObjectArrayItemHash_NoData;//#216
								hp++;
								if( hp >= mHash.GetItemCount() )   hp = 0;
								//"Deleted"を"NoData"にしたので"Deleted"個数をデクリメント   #303 "Deleted"の個数をRehash判断に使用する
								mHash_DeletedCount--;
							}
						}
						else
						{
							p[hashpos].val = kObjectArrayItemHash_Deleted;//#216
							//"Deleted"にしたので"Deleted"個数をインクリメント   #303 "Deleted"の個数をRehash判断に使用する
							mHash_DeletedCount++;
						}
						return;
					}
				}
				hashpos = GetNextHashPos(hashpos);
				if( hashpos == hashstartpos )   break;
			}
		}
	}
	
	//ハッシュテーブルに、対象Arrayの指定アイテムを登録する。
	//データIndex番号の移動なし
	void RegisterToHash( const AIndex inIndex, const AObjectID& inObjectID )
	{
		//ハッシュ値計算
		unsigned long	hash = GetHashValue(mObjectArray.GetObjectConst(inIndex).GetPrimaryKeyData(),0,kIndex_Invalid);
		AIndex	hashstartpos = hash%(mHash.GetItemCount());
		
		{//#598 arrayptrの有効範囲を最小にする
			//高速化のため、ポインタ使用
			//#695 AStArrayPtr<AObjectID>	arrayptr(mHash,0,mHash.GetItemCount());
			//#695 AObjectID*	p = arrayptr.GetPtr();
			//高速化のため、直にAArray::mDataPtrを取得する。
			//AStArrayPtr<>は、あくまでコーディングミス防止なので、変更等が頻繁には発生しないAbsBase内コードでは、
			//必ずしも使う必要が無い。
			//AHashArrayへの10M項目追加で、9s→2sの効果。
			//rehash時のMakeHashにも使われているので、ここの高速化は重要。
			AObjectID*	p = mHash.GetDataPtrWithoutCheck();
			//登録箇所検索
			AIndex	hashpos = hashstartpos;
			while( true )
			{
				AObjectID	dataObjectID = p[hashpos];
				if( dataObjectID.val == kObjectArrayItemHash_NoData )   break;
				if( dataObjectID.val == kObjectArrayItemHash_Deleted )
				{
					//"Deleted"にデータ上書きするので"Deleted"個数をデクリメント   #303 "Deleted"の個数をRehash判断に使用する
					mHash_DeletedCount--;
					break;
				}
				hashpos = GetNextHashPos(hashpos);
				if( hashpos == hashstartpos )
				{
					_ACError("no area in hash",this);
					return;
				}
			}
			
			//登録
			p[hashpos] = inObjectID;
		}
	}
	
	//次のHash位置取得
	AIndex	GetNextHashPos( const AIndex& inHashPosition ) const
	{
		if( inHashPosition == 0 )
		{
			return mHash.GetItemCount()-1;
		}
		else
		{
			return inHashPosition -1;
		}
	}
	
	//ハッシュ
	//配列データのObjectIDを格納する。
	AArray<AObjectID>	mHash;
	AItemCount	mHash_DeletedCount;//#303
	
	//配列データ
	AObjectArrayIDIndexed<T>	mObjectArray;//#693
	
	//ハッシュサイズ倍数 B0000 高速化 HTMLのname属性をグローバル識別子に入れるようにしたら、php_manual_ja.htmlで描画速度が極端に遅くなったため
	//AIdentifierListのメンバmIdentifierArrayの倍数を10に設定する。それ以外は従来通り3とする。080822
	ANumber	mHashSizeMultiple;
	
	//#598
	/**
	スレッドセーフEnable/Disable設定
	inEnable=trueの場合、AHashObjectArrayの内部データ（ただしArray要素のObjectは含まない）についてはスレッドセーフとなる。
	（外部に渡したデータについてはスレッドセーフではない（内部データとの整合性保証されない）ので注意。
	たとえばGetItemCount()で項目数を取得した場合、その値はコール側でmutexをかけない限り、
	保証されない。（別スレッドでInsert/Deleteされる可能性がある。）
	また、Array要素のObjectもスレッドセーフではないので、必要なら別途mutex必要。
	デフォルトはEnable。
	*/
  public:
	void	EnableThreadSafe( const ABool inEnable ) { mMutexEnabled = inEnable; }
  private:
	mutable AThreadMutex	mMutex;
	ABool	mMutexEnabled;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AHashObjectArray<>"; }
	
};


